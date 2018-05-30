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
    
    m_sSky.xMin = 0;
    m_sSky.xMax = DISPLAY_SIZE;
    m_sSky.yMin = 0;
    m_sSky.yMax = DISPLAY_MID_LOW;

    m_sEarth.xMin = 0;
    m_sEarth.xMax = DISPLAY_SIZE;
    m_sEarth.yMin = DISPLAY_MID_HIGH;
    m_sEarth.yMax = DISPLAY_SIZE;
}

uint8_t LCD::run()
{


    //Receive message
    st_Message * l_st_ReceiveMessage;
    l_st_ReceiveMessage=this->m_pMailbox->GetMessage(this->m_u8TaskID);

    int32_t l_i32Data=l_st_ReceiveMessage->u32Content;

    //double l_iResult=128*43/(l_dData-30)
    //int8_t l_u8Elevation = (int8_t)(63.5 + 0.7*l_i32Data);
    int8_t l_u8Elevation = (int8_t)((30 + l_i32Data)*128/(-46+30));
    //uint32_t l_u32Elevation = (uint32_t)(63.5 + 6.4*l_dData);

    if (l_u8Elevation > 127)
    {
        l_u8Elevation = 127;
    }
    if (l_u8Elevation < 0)
    {
        l_u8Elevation = 0;
    }

    m_sSky.yMax = l_u8Elevation;
    m_sEarth.yMin = l_u8Elevation;


    Graphics_setForegroundColor(&m_sContext, SKY_COLOR);
    Graphics_fillRectangle(&m_sContext, &m_sSky);
    Graphics_setForegroundColor(&m_sContext, EARTH_COLOR);
    Graphics_fillRectangle(&m_sContext, &m_sEarth);

    char string[20];
    sprintf(string, "A: %4d", l_i32Data);
    Graphics_drawStringCentered(&m_sContext,
                                    (int8_t *)string,
                                    8,
                                    64,
                                    50,
                                    OPAQUE_TEXT);

    Graphics_Rectangle l_sLine;
    Graphics_setForegroundColor(&m_sContext, LINE_COLOR);

    l_sLine.xMin = 0;
    l_sLine.xMax = DISPLAY_SIZE;
    l_sLine.yMin = l_u8Elevation-1;
    l_sLine.yMax = l_u8Elevation+1;

    Graphics_fillRectangle(&m_sContext, &l_sLine);



    //Graphics_clearDisplay(&m_sContext);
    //Graphics_setForegroundColor(&m_sContext, GRAPHICS_COLOR_RED);
    /*Graphics_drawStringCentered(&m_sContext,
                                    (int8_t *)"Accelerometer:",
                                    AUTO_STRING_LENGTH,
                                    64,
                                    30,
                                    OPAQUE_TEXT);
    //Graphics_setForegroundColor(&m_sContext, ClrBlue);*/

    



    return(NO_ERR);
}

uint8_t LCD::setup()
{
    //LCD setup
    MAP_Interrupt_disableMaster();

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


    //Set the orientation
    SetOrientation(LCD_ORIENTATION_UP);

    /* Initializes graphics context */
    //Graphics_initContext(&m_sContext, &m_sDisplay, &m_sDisplay_Functions);
    Graphics_initContext(&m_sContext, &m_sDisplay, &g_sDisplay_Functions);
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
    MAP_Interrupt_enableMaster();

    return(NO_ERR);
}
