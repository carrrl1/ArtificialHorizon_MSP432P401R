/*
 * LCD.hpp
 *
 *  Created on: May 22, 2018
 *      Author: cesolano
 */

#ifndef LCD_HPP_
#define LCD_HPP_
#define __NOP __nop

#include "msp.h"
#include "Task.hpp"
#include <driverlib.h>
#include <grlib/grlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// System clock speed (in Hz)
#define LCD_SYSTEM_CLOCK_SPEED                 48000000
// SPI clock speed (in Hz)
#define LCD_SPI_CLOCK_SPEED                    16000000

// Ports from MSP432 connected to LCD
#define LCD_SCK_PORT          GPIO_PORT_P1
#define LCD_SCK_PIN_FUNCTION  GPIO_PRIMARY_MODULE_FUNCTION
#define LCD_MOSI_PORT         GPIO_PORT_P1
#define LCD_MOSI_PIN_FUNCTION GPIO_PRIMARY_MODULE_FUNCTION
#define LCD_RST_PORT          GPIO_PORT_P5
#define LCD_CS_PORT           GPIO_PORT_P5
#define LCD_DC_PORT           GPIO_PORT_P3

// Definition of USCI base address to be used for SPI communication
#define LCD_EUSCI_BASE        EUSCI_B0_BASE

// Pins from MSP432 connected to LCD
#define LCD_SCK_PIN           GPIO_PIN5
#define LCD_MOSI_PIN          GPIO_PIN6
#define LCD_RST_PIN           GPIO_PIN7
#define LCD_CS_PIN            GPIO_PIN0
#define LCD_DC_PIN            GPIO_PIN7

// LCD Screen Dimensions
#define LCD_VERTICAL_MAX                   128
#define LCD_HORIZONTAL_MAX                 128

#define LCD_ORIENTATION_UP    0
#define LCD_ORIENTATION_LEFT  1
#define LCD_ORIENTATION_DOWN  2
#define LCD_ORIENTATION_RIGHT 3

// ST7735 LCD controller Command Set
#define CM_NOP             0x00
#define CM_SWRESET         0x01
#define CM_RDDID           0x04
#define CM_RDDST           0x09
#define CM_SLPIN           0x10
#define CM_SLPOUT          0x11
#define CM_PTLON           0x12
#define CM_NORON           0x13
#define CM_INVOFF          0x20
#define CM_INVON           0x21
#define CM_GAMSET          0x26
#define CM_DISPOFF         0x28
#define CM_DISPON          0x29
#define CM_CASET           0x2A
#define CM_RASET           0x2B
#define CM_RAMWR           0x2C
#define CM_RGBSET          0x2d
#define CM_RAMRD           0x2E
#define CM_PTLAR           0x30
#define CM_MADCTL          0x36
#define CM_COLMOD          0x3A
#define CM_SETPWCTR        0xB1
#define CM_SETDISPL        0xB2
#define CM_FRMCTR3         0xB3
#define CM_SETCYC          0xB4
#define CM_SETBGP          0xb5
#define CM_SETVCOM         0xB6
#define CM_SETSTBA         0xC0
#define CM_SETID           0xC3
#define CM_GETHID          0xd0
#define CM_SETGAMMA        0xE0
#define CM_MADCTL_MY       0x80
#define CM_MADCTL_MX       0x40
#define CM_MADCTL_MV       0x20
#define CM_MADCTL_ML       0x10
#define CM_MADCTL_BGR      0x08
#define CM_MADCTL_MH       0x04

// Custom __delay_cycles() for non CCS Compiler
#if !defined( __TI_ARM__ )
#undef __delay_cycles
#define __delay_cycles(x)     SysCtlDelay(x)
void SysCtlDelay(uint32_t);
#endif

#define HAL_LCD_delay(x)      __delay_cycles(x * 48)

//LCD class
class LCD : public Task
{
    public:
        LCD();
        virtual uint8_t run(void);
        virtual uint8_t setup(void);
        static void HAL_LCD_writeCommand(uint8_t i_u8Command);
		static void HAL_LCD_writeData(uint8_t i_u8Data);
		static void SetDrawFrame(uint16_t i_u16x0, uint16_t i_u16y0, uint16_t i_u16x1, uint16_t i_u16y1);
		static void SetOrientation(uint8_t i_u8Orientation);
		/*
		void PixelDraw(const Graphics_Display *i_pDisplay, 
								int16_t i_u16lX, 
								int16_t i_u16lY, uint16_t i_u16ulValue);
		void PixelDrawMultiple(const Graphics_Display *i_pDisplay,
                                          int16_t i_i16lX,
                                          int16_t i_i16lY,
                                          int16_t i_i16lX0,
                                          int16_t i_i16lCount,
                                          int16_t i_i16lBPP,
                                          const uint8_t *i_pData,
                                          const uint32_t *i_pPalette);
		void LineDrawH(const Graphics_Display *i_pDisplay,
                                          int16_t i_i16lX1,
                                          int16_t i_i16lX2,
                                          int16_t i_i16lY,
                                          uint16_t i_u16Value);
		void LineDrawV(const Graphics_Display *pDisplay,
                                          int16_t i_i16lX,
                                          int16_t i_i16lY1,
                                          int16_t i_i16lY2,
                                          uint16_t i_u16Value);
		void RectFill(const Graphics_Display *i_pDisplay,
                                          const Graphics_Rectangle *i_pRect,
                                          uint16_t i_u16Value);
		uint32_t ColorTranslate(const Graphics_Display *i_pDisplay,
                                          uint32_t i_u32Value);
		void Flush(const Graphics_Display *i_pDisplay);
		void ClearScreen (const Graphics_Display *i_pDisplay,
                                		  uint16_t i_u16Value);*/

    protected:
    private:
    	static uint8_t m_u8Orientation;
    	/* Graphic library context */
		Graphics_Context m_sContext;
		Graphics_Display m_sDisplay;
		//Graphics_Display_Functions m_sDisplay_Functions;
		//Graphics_Display m_sFontFixed;
};

#endif /* LCD_HPP_ */
