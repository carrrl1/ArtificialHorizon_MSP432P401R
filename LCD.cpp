#include "LCD.hpp"

LCD::LCD()
{
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

    //Graphics_clearDisplay(&m_sContext);
    //Graphics_setForegroundColor(&m_sContext, GRAPHICS_COLOR_RED);
    /*Graphics_drawStringCentered(&m_sContext,
                                    (int8_t *)"Accelerometer:",
                                    AUTO_STRING_LENGTH,
                                    64,
                                    30,
                                    OPAQUE_TEXT);
    //Graphics_setForegroundColor(&m_sContext, ClrBlue);*/

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

    //Set the orientation
    SetOrientation(LCD_ORIENTATION_UP);

    /* Initializes graphics context */
    //Graphics_initContext(&m_sContext, &m_sDisplay, &m_sDisplay_Functions);
    Graphics_initContext(&m_sContext, &m_sDisplay, &l_sDisplay_Functions);
    Graphics_setForegroundColor(&m_sContext, GRAPHICS_COLOR_RED);
    Graphics_setBackgroundColor(&m_sContext, ClrBlue);
    GrContextFontSet(&m_sContext, &g_sFontFixed6x8);

    Graphics_drawStringCentered(&m_sContext,
                                    (int8_t *)"ArtificialHorizon:",
                                    AUTO_STRING_LENGTH,
                                    64,
                                    30,
                                    OPAQUE_TEXT);

    //drawTitle();

    return(NO_ERR);
}