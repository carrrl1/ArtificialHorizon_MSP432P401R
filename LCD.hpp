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
#include "LCD_DRIVER.hpp"
//#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
//#include <ti/grlib/grlib.h>

//LCD class
class LCD : public Task
{
    public:
        LCD();
        virtual uint8_t run(void);
        virtual uint8_t setup(void);

    protected:
    private:
    	
    	/* Graphic library context */
		Graphics_Context m_sContext;
		Graphics_Display m_sDisplay;
		//Graphics_Display_Functions m_sDisplay_Functions;
		//Graphics_Display m_sFontFixed;
};

#endif /* LCD_HPP_ */
