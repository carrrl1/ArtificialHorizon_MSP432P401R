#include "LCD.hpp"

//*****************************************************************************
//
//! Draws a pixel on the screen.
//!
//! \param pDisplay is a pointer to the driver-specific data for this
//! display driver.
//! \param i_i16lX is the X coordinate of the pixel.
//! \param lY is the Y coordinate of the pixel.
//! \param ulValue is the color of the pixel.
//!
//! This function sets the given pixel to a particular color.  The coordinates
//! of the pixel are assumed to be within the extents of the display.
//!
//! \return None.
//
//*****************************************************************************
static void PixelDraw(const Graphics_Display *i_pDisplay, 
                          int16_t i_u16lX, 
                          int16_t i_u16lY, 
                          uint16_t i_u16ulValue)
{

    LCD::SetDrawFrame(i_u16lX,i_u16lY,i_u16lX,i_u16lY);

    //
    // Write the pixel value.
    //
    LCD::HAL_LCD_writeCommand(CM_RAMWR);
    LCD::HAL_LCD_writeData(i_u16ulValue>>8);
    LCD::HAL_LCD_writeData(i_u16ulValue);
}

//*****************************************************************************
//
//! Draws a horizontal sequence of pixels on the screen.
//!
//! \param pDisplay is a pointer to the driver-specific data for this
//! display driver.
//! \param i_i16lX is the X coordinate of the first pixel.
//! \param i_i16lY is the Y coordinate of the first pixel.
//! \param i_i16lX0 is sub-pixel offset within the pixel data, which is valid for 1
//! or 4 bit per pixel formats.
//! \param i_i16lCount is the number of pixels to draw.
//! \param i_i16lBPP is the number of bits per pixel; must be 1, 4, or 8.
//! \param i_pData is a pointer to the pixel data.  For 1 and 4 bit per pixel
//! formats, the most significant bit(s) represent the left-most pixel.
//! \param i_pPalette is a pointer to the palette used to draw the pixels.
//!
//! This function draws a horizontal sequence of pixels on the screen, using
//! the supplied palette.  For 1 bit per pixel format, the palette contains
//! pre-translated colors; for 4 and 8 bit per pixel formats, the palette
//! contains 24-bit RGB values that must be translated before being written to
//! the display.
//!
//! \return None.
//
//*****************************************************************************
static void PixelDrawMultiple(const Graphics_Display *i_pDisplay,
                                                  int16_t i_i16lX,
                                                  int16_t i_i16lY,
                                                  int16_t i_i16lX0,
                                                  int16_t i_i16lCount,
                                                  int16_t i_i16lBPP,
                                                  const uint8_t *i_pData,
                                                  const uint32_t *i_pPalette)
{
    uint16_t Data;

    //
    // Set the cursor increment to left to right, followed by top to bottom.
    //
    LCD::SetDrawFrame(i_i16lX,i_i16lY,i_i16lX+i_i16lCount,127);
    LCD::HAL_LCD_writeCommand(CM_RAMWR);

    //
    // Determine how to interpret the pixel data based on the number of bits
    // per pixel.
    //
    switch(i_i16lBPP)
    {
        // The pixel data is in 1 bit per pixel format
        case 1:
        {
            // Loop while there are more pixels to draw
            while(i_i16lCount > 0)
            {
                // Get the next byte of image data
                Data = *i_pData++;

                // Loop through the pixels in this byte of image data
                for(; (i_i16lX0 < 8) && i_i16lCount; i_i16lX0++, i_i16lCount--)
                {
                    // Draw this pixel in the appropriate color
                    LCD::HAL_LCD_writeData((((uint32_t *)i_pPalette)[(Data >>
                                                             (7 - i_i16lX0)) & 1])>>8);
                    LCD::HAL_LCD_writeData(((uint32_t *)i_pPalette)[(Data >>
                                                             (7 - i_i16lX0)) & 1]);
                }

                // Start at the beginning of the next byte of image data
                i_i16lX0 = 0;
            }
            // The image data has been drawn

            break;
        }

        // The pixel data is in 4 bit per pixel format
        case 4:
        {
            // Loop while there are more pixels to draw.  "Duff's device" is
            // used to jump into the middle of the loop if the first nibble of
            // the pixel data should not be used.  Duff's device makes use of
            // the fact that a case statement is legal anywhere within a
            // sub-block of a switch statement.  See
            // http://en.wikipedia.org/wiki/Duff's_device for detailed
            // information about Duff's device.
            switch(i_i16lX0 & 1)
            {
                case 0:

                    while(i_i16lCount)
                    {
                        // Get the upper nibble of the next byte of pixel data
                        // and extract the corresponding entry from the palette
                        Data = (*i_pData >> 4);
                        Data = (*(uint16_t *)(i_pPalette + Data));
                        // Write to LCD screen
                        LCD::HAL_LCD_writeData(Data>>8);
                        LCD::HAL_LCD_writeData(Data);

                        // Decrement the count of pixels to draw
                        i_i16lCount--;

                        // See if there is another pixel to draw
                        if(i_i16lCount)
                        {
                case 1:
                            // Get the lower nibble of the next byte of pixel
                            // data and extract the corresponding entry from
                            // the palette
                            Data = (*i_pData++ & 15);
                            Data = (*(uint16_t *)(i_pPalette + Data));
                            // Write to LCD screen
                            LCD::HAL_LCD_writeData(Data>>8);
                            LCD::HAL_LCD_writeData(Data);

                            // Decrement the count of pixels to draw
                            i_i16lCount--;
                        }
                    }
            }
            // The image data has been drawn.

            break;
        }

        // The pixel data is in 8 bit per pixel format
        case 8:
        {
            // Loop while there are more pixels to draw
            while(i_i16lCount--)
            {
                // Get the next byte of pixel data and extract the
                // corresponding entry from the palette
                Data = *i_pData++;
                Data = (*(uint16_t *)(i_pPalette + Data));
                // Write to LCD screen
                LCD::HAL_LCD_writeData(Data>>8);
                LCD::HAL_LCD_writeData(Data);
            }
            // The image data has been drawn
            break;
        }

        //
        // We are being passed data in the display's native format.  Merely
        // write it directly to the display.  This is a special case which is
        // not used by the graphics library but which is helpful to
        // applications which may want to handle, for example, JPEG images.
        //
        case 16:
        {
            uint16_t usData;

            // Loop while there are more pixels to draw.

            while(i_i16lCount--)
            {
                // Get the next byte of pixel data and extract the
                // corresponding entry from the palette
                usData = *((uint16_t *)i_pData);
                i_pData += 2;

                // Translate this palette entry and write it to the screen
                LCD::HAL_LCD_writeData(usData>>8);
                LCD::HAL_LCD_writeData(usData);
            }
        }
    }
}

//*****************************************************************************
//
//! Draws a horizontal line.
//!
//! \param i_pDisplay is a pointer to the driver-specific data for this
//! display driver.
//! \param i_i16lX1 is the X coordinate of the start of the line.
//! \param i_i16lX2 is the X coordinate of the end of the line.
//! \param i_i16lY is the Y coordinate of the line.
//! \param i_u16Value is the color of the line.
//!
//! This function draws a horizontal line on the display.  The coordinates of
//! the line are assumed to be within the extents of the display.
//!
//! \return None.
//
//*****************************************************************************
static void LineDrawH(const Graphics_Display *i_pDisplay,
                                          int16_t i_i16lX1,
                                          int16_t i_i16lX2,
                                          int16_t i_i16lY,
                                          uint16_t i_u16Value)
{
    LCD::SetDrawFrame(i_i16lX1, i_i16lY, i_i16lX2, i_i16lY);

    // Write the pixel value.

    LCD::HAL_LCD_writeCommand(CM_RAMWR);
    for (int i = i_i16lX1; i <= i_i16lX2; i++)
    {
        LCD::HAL_LCD_writeData(i_u16Value>>8);
        LCD::HAL_LCD_writeData(i_u16Value);
    }
}

//*****************************************************************************
//
//! Draws a vertical line.
//!
//! \param pDisplay is a pointer to the driver-specific data for this
//! display driver.
//! \param i_i16lX is the X coordinate of the line.
//! \param i_i16lY1 is the Y coordinate of the start of the line.
//! \param i_i16lY2 is the Y coordinate of the end of the line.
//! \param i_u16Value is the color of the line.
//!
//! This function draws a vertical line on the display.  The coordinates of the
//! line are assumed to be within the extents of the display.
//!
//! \return None.
//
//*****************************************************************************
static void LineDrawV(const Graphics_Display *pDisplay,
                                          int16_t i_i16lX,
                                          int16_t i_i16lY1,
                                          int16_t i_i16lY2,
                                          uint16_t i_u16Value)
{
    LCD::SetDrawFrame(i_i16lX, i_i16lY1, i_i16lX, i_i16lY2);

    // Write the pixel value.

    LCD::HAL_LCD_writeCommand(CM_RAMWR);
    for (int i = i_i16lY1; i <= i_i16lY2; i++)
    {
        LCD::HAL_LCD_writeData(i_u16Value>>8);
        LCD::HAL_LCD_writeData(i_u16Value);
    }
}

//*****************************************************************************
//
//! Fills a rectangle.
//!
//! \param i_pDisplay is a pointer to the driver-specific data for this
//! display driver.
//! \param pRect is a pointer to the structure describing the rectangle.
//! \param ulValue is the color of the rectangle.
//!
//! This function fills a rectangle on the display.  The coordinates of the
//! rectangle are assumed to be within the extents of the display, and the
//! rectangle specification is fully inclusive (in other words, both sXMin and
//! sXMax are drawn, along with sYMin and sYMax).
//!
//! \return None.
//
//*****************************************************************************
static void RectFill(const Graphics_Display *i_pDisplay,
                                         const Graphics_Rectangle *i_pRect,
                                         uint16_t i_u16Value)
{
    int16_t l_u16x0 = i_pRect->sXMin;
    int16_t l_u16x1 = i_pRect->sXMax;
    int16_t l_u16y0 = i_pRect->sYMin;
    int16_t l_u16y1 = i_pRect->sYMax;

    LCD::SetDrawFrame(l_u16x0, l_u16y0, l_u16x1, l_u16y1);

    // Write the pixel value.

    int16_t l_i16pixels = (l_u16x1 - l_u16x0 + 1) * (l_u16y1 - l_u16y0 + 1);
    LCD::HAL_LCD_writeCommand(CM_RAMWR);
    for (int i = 0; i <= l_i16pixels; i++)
    {
        LCD::HAL_LCD_writeData(i_u16Value>>8);
        LCD::HAL_LCD_writeData(i_u16Value);
    }
}

//*****************************************************************************
//
//! Translates a 24-bit RGB color to a display driver-specific color.
//!
//! \param i_pDisplay is a pointer to the driver-specific data for this
//! display driver.
//! \param i_u32Value is the 24-bit RGB color.  The least-significant byte is the
//! blue channel, the next byte is the green channel, and the third byte is the
//! red channel.
//!
//! This function translates a 24-bit RGB color into a value that can be
//! written into the display's frame buffer in order to reproduce that color,
//! or the closest possible approximation of that color.
//!
//! \return Returns the display-driver specific color.
//
//*****************************************************************************
static uint32_t ColorTranslate(const Graphics_Display *i_pDisplay,
                                                   uint32_t i_u32Value)
{
    //
    // Translate from a 24-bit RGB color to a 5-6-5 RGB color.
    //
    return(((((i_u32Value) & 0x00f80000) >> 8) |
            (((i_u32Value) & 0x0000fc00) >> 5) |
            (((i_u32Value) & 0x000000f8) >> 3)));
}

//*****************************************************************************
//
//! Flushes any cached drawing operations.
//!
//! \param i_pDisplay is a pointer to the driver-specific data for this
//! display driver.
//!
//! This functions flushes any cached drawing operations to the display.  This
//! is useful when a local frame buffer is used for drawing operations, and the
//! flush would copy the local frame buffer to the display.  For the SSD2119
//! driver, the flush is a no operation.
//!
//! \return None.
//
//*****************************************************************************
static void Flush(const Graphics_Display *i_pDisplay)
{
    //
    // There is nothing to be done.
    //
}

//*****************************************************************************
//
//! Send command to clear screen.
//!
//! \param i_pDisplay is a pointer to the driver-specific data for this
//! display driver.
//!
//! This function does a clear screen and the Display Buffer contents
//! are initialized to the current background color.
//!
//! \return None.
//
//*****************************************************************************
static void ClearScreen (const Graphics_Display *i_pDisplay,
                                 uint16_t i_u16Value)
{
    Graphics_Rectangle l_sRect = { 0, 0, LCD_VERTICAL_MAX-1, LCD_VERTICAL_MAX-1};
    RectFill(i_pDisplay, &l_sRect, i_u16Value);
}

LCD::LCD()
{
    m_u8Orientation=LCD_ORIENTATION_UP;

    //! The display structure that describes the driver for the Kitronix
    //! K350QVG-V1-F TFT panel with an SSD2119 controller.
    m_sDisplay =
    {
        sizeof(Graphics_Display),
        0,
        LCD_VERTICAL_MAX,
        LCD_HORIZONTAL_MAX,
    };
    
    /*
    m_sDisplay_Functions=
    {
        PixelDraw,
        PixelDrawMultiple,
        LineDrawH,
        LineDrawV,
        RectFill,
        ColorTranslate,
        Flush,
        ClearScreen
    };*/
}

uint8_t LCD::run()
{
    //Send message
    //st_Message l_st_SendMessage;

    //l_st_SendMessage.u8Sender = this->m_u8TaskID;
    //l_st_SendMessage.u8Receiver = this->m_u8TaskID;
    //l_st_SendMessage.u32Content ^= m_u16BITN;

    //this->m_pMailbox->SendMessage(&l_st_SendMessage);

    //Receive message
    //st_Message * l_st_ReceiveMessage;
    //l_st_ReceiveMessage=this->m_pMailbox->GetMessage(this->m_u8TaskID);

    //uint16_t l_u16Data=l_st_ReceiveMessage->u32Content;



    return(NO_ERR);
}

uint8_t LCD::setup()
{
    //LCD setup

    // Set up SPI Ports
    // LCD_SCK
    GPIO_setAsPeripheralModuleFunctionOutputPin(LCD_SCK_PORT, LCD_SCK_PIN, GPIO_PRIMARY_MODULE_FUNCTION);
    // LCD_MOSI
    GPIO_setAsPeripheralModuleFunctionOutputPin(LCD_MOSI_PORT, LCD_MOSI_PIN, GPIO_PRIMARY_MODULE_FUNCTION);
    // LCD_RST
    GPIO_setAsOutputPin(LCD_RST_PORT, LCD_RST_PIN);
    // LCD_RS
    GPIO_setAsOutputPin(LCD_DC_PORT, LCD_DC_PIN);
    // LCD_CS
    GPIO_setAsOutputPin(LCD_CS_PORT, LCD_CS_PIN);

    // Set up SPI 
    eUSCI_SPI_MasterConfig config =
    {
        EUSCI_B_SPI_CLOCKSOURCE_SMCLK,
        LCD_SYSTEM_CLOCK_SPEED,
        LCD_SPI_CLOCK_SPEED,
        EUSCI_B_SPI_MSB_FIRST,
        EUSCI_B_SPI_PHASE_DATA_CAPTURED_ONFIRST_CHANGED_ON_NEXT,
        EUSCI_B_SPI_CLOCKPOLARITY_INACTIVITY_LOW,
        EUSCI_B_SPI_3PIN
    };
    SPI_initMaster(LCD_EUSCI_BASE, &config);
    SPI_enableModule(LCD_EUSCI_BASE);

    GPIO_setOutputLowOnPin(LCD_CS_PORT, LCD_CS_PIN);

    GPIO_setOutputHighOnPin(LCD_DC_PORT, LCD_DC_PIN);

    //Write first data

    GPIO_setOutputLowOnPin(LCD_RST_PORT, LCD_RST_PIN);
    HAL_LCD_delay(50);
    GPIO_setOutputHighOnPin(LCD_RST_PORT, LCD_RST_PIN);
    HAL_LCD_delay(120);

    HAL_LCD_writeCommand(CM_SLPOUT);
    HAL_LCD_delay(200);

    HAL_LCD_writeCommand(CM_GAMSET);
    HAL_LCD_writeData(0x04);

    HAL_LCD_writeCommand(CM_SETPWCTR);
    HAL_LCD_writeData(0x0A);
    HAL_LCD_writeData(0x14);

    HAL_LCD_writeCommand(CM_SETSTBA);
    HAL_LCD_writeData(0x0A);
    HAL_LCD_writeData(0x00);

    HAL_LCD_writeCommand(CM_COLMOD);
    HAL_LCD_writeData(0x05);
    HAL_LCD_delay(10);

    HAL_LCD_writeCommand(CM_MADCTL);
    HAL_LCD_writeData(CM_MADCTL_BGR);

    HAL_LCD_writeCommand(CM_NORON);

    //Lcd_ScreenWidth  = LCD_VERTICAL_MAX;
    //Lcd_ScreenHeigth = LCD_HORIZONTAL_MAX;
    //Lcd_PenSolid  = 0;
    //Lcd_FontSolid = 1;
    //Lcd_FlagRead  = 0;
    //Lcd_TouchTrim = 0;

    SetDrawFrame(0, 0, 127, 127);
    HAL_LCD_writeCommand(CM_RAMWR);
    int i;
    for (i = 0; i < 16384; i++)
    {
        HAL_LCD_writeData(0xFF);
        HAL_LCD_writeData(0xFF);
    }

    HAL_LCD_delay(10);
    HAL_LCD_writeCommand(CM_DISPON);

    //
    const Graphics_Display_Functions l_sDisplay_Functions =
    {
        PixelDraw,
        PixelDrawMultiple,
        LineDrawH,
        LineDrawV,
        RectFill,
        ColorTranslate,
        Flush,
        ClearScreen
    };

    /* Initializes graphics context */
    //Graphics_initContext(&m_sContext, &m_sDisplay, &m_sDisplay_Functions);
    Graphics_initContext(&m_sContext, &m_sDisplay, &l_sDisplay_Functions);
    Graphics_setForegroundColor(&m_sContext, GRAPHICS_COLOR_RED);
    Graphics_setBackgroundColor(&m_sContext, GRAPHICS_COLOR_WHITE);
    GrContextFontSet(&m_sContext, &g_sFontFixed6x8);
    //drawTitle();

    return(NO_ERR);
}

//*****************************************************************************
//
// Writes a command to the CFAF128128B-0145T.  This function implements the basic SPI
// interface to the LCD display.
//
//*****************************************************************************
void LCD::HAL_LCD_writeCommand(uint8_t i_u8Command)
{
    // Set to command mode
    GPIO_setOutputLowOnPin(LCD_DC_PORT, LCD_DC_PIN);

    // USCI_B0 Busy? //
    while (UCB0STATW & UCBUSY);

    // Transmit data
    UCB0TXBUF = i_u8Command;

    // USCI_B0 Busy? //
    while (UCB0STATW & UCBUSY);

    // Set back to data mode
    GPIO_setOutputHighOnPin(LCD_DC_PORT, LCD_DC_PIN);
}


//*****************************************************************************
//
// Writes a data to the CFAF128128B-0145T.  This function implements the basic SPI
// interface to the LCD display.
//
//*****************************************************************************
void LCD::HAL_LCD_writeData(uint8_t i_u8Data)
{
    // USCI_B0 Busy? //
    while (UCB0STATW & UCBUSY);

    // Transmit data
    UCB0TXBUF = i_u8Data;

    // USCI_B0 Busy? //
    while (UCB0STATW & UCBUSY);
}

void LCD::SetDrawFrame(uint16_t i_u16x0, uint16_t i_u16y0, uint16_t i_u16x1, uint16_t i_u16y1)
{
    switch (m_u8Orientation) {
        case 0:
            i_u16x0 += 2;
            i_u16y0 += 3;
            i_u16x1 += 2;
            i_u16y1 += 3;
            break;
        case 1:
            i_u16x0 += 3;
            i_u16y0 += 2;
            i_u16x1 += 3;
            i_u16y1 += 2;
            break;
        case 2:
            i_u16x0 += 2;
            i_u16y0 += 1;
            i_u16x1 += 2;
            i_u16y1 += 1;
            break;
        case 3:
            i_u16x0 += 1;
            i_u16y0 += 2;
            i_u16x1 += 1;
            i_u16y1 += 2;
            break;
        default:
            break;
    }

    HAL_LCD_writeCommand(CM_CASET);
    HAL_LCD_writeData((uint8_t)(i_u16x0 >> 8));
    HAL_LCD_writeData((uint8_t)(i_u16x0));
    HAL_LCD_writeData((uint8_t)(i_u16x1 >> 8));
    HAL_LCD_writeData((uint8_t)(i_u16x1));

    HAL_LCD_writeCommand(CM_RASET);
    HAL_LCD_writeData((uint8_t)(i_u16y0 >> 8));
    HAL_LCD_writeData((uint8_t)(i_u16y0));
    HAL_LCD_writeData((uint8_t)(i_u16y1 >> 8));
    HAL_LCD_writeData((uint8_t)(i_u16y1));
}

//*****************************************************************************
//
//! Sets the LCD Orientation.
//!
//! \param orientation is the desired orientation for the LCD. Valid values are:
//!           - \b LCD_ORIENTATION_UP,
//!           - \b LCD_ORIENTATION_LEFT,
//!           - \b LCD_ORIENTATION_DOWN,
//!           - \b LCD_ORIENTATION_RIGHT,
//!
//! This function sets the orientation of the LCD
//!
//! \return None.
//
//*****************************************************************************
void LCD::SetOrientation(uint8_t i_u8Orientation)
{
    m_u8Orientation = i_u8Orientation;
    HAL_LCD_writeCommand(CM_MADCTL);
    switch (m_u8Orientation) {
        case LCD_ORIENTATION_UP:
            HAL_LCD_writeData(CM_MADCTL_MX | CM_MADCTL_MY | CM_MADCTL_BGR);
            break;
        case LCD_ORIENTATION_LEFT:
            HAL_LCD_writeData(CM_MADCTL_MY | CM_MADCTL_MV | CM_MADCTL_BGR);
            break;
        case LCD_ORIENTATION_DOWN:
            HAL_LCD_writeData(CM_MADCTL_BGR);
            break;
        case LCD_ORIENTATION_RIGHT:
            HAL_LCD_writeData(CM_MADCTL_MX | CM_MADCTL_MV | CM_MADCTL_BGR);
            break;
    }
}
