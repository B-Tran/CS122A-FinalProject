/*	Author: Brandon Tran btran054@ucr.edu
 *  Partner(s) Name: Angel Renteria arent007@ucr.edu
 *	Lab Section: 023
 *	Assignment: Final Project
 *	Exercise Description: [optional - include for your own benefit]
 *  This code is for the outputs of the led brighness based on
 *  the light code that the subcontroller sends
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */

#include "../header/scheduler.h"
#include "../header/timer.h"
#include "../header/io.h"
#include "../header/ledPWM.h" //library for atmega328p pwms


#include <avr/io.h>
#define F_CPU = 8000000UL

/*---------------------------------------------------------------
  Declarations of enums for the state machines below
  ---------------------------------------------------------------*/
//states for setting the dimming of lights
enum dimSM {dimInit, dimSet};
/*---------------------------------------------------------------
  Declarations of global variables and #defines below
  ---------------------------------------------------------------*/
#define lightVal (PINC & 0x0F)
/*---------------------------------------------------------------
  Declaration of functions below
  ---------------------------------------------------------------*/

/*---------------------------------------------------------------
  Declaration of state machine functions below
  ---------------------------------------------------------------*/

/*---------------------------------------------------------------
 * This tick function sets the brightness of the leds using pwm
  ---------------------------------------------------------------*/
int dim_Tick(int state)   {
    uint8_t dimVal;     //variable to store the pwm dimness value

//transitions
    switch(state)   {
        case dimInit:
            break;
        case dimSet:
            state = dimSet;
            break;
        default:
            break;
    }
//actions
    switch(state)   { 
        case dimInit:
            state = dimSet;
            break;
        case dimSet:
//sets the dimness value based on the lightVal
            switch(lightVal)   {
                case 0x00:
                    dimVal = 0;
                    break;
                case 0x01:
                    dimVal = 17;
                    break;
                case 0x02:
                    dimVal = 34;
                    break;
                case 0x03:
                    dimVal = 51;
                    break;
                case 0x04:
                    dimVal = 68;
                    break;
                case 0x05:
                    dimVal = 85;
                    break;
                case 0x06:
                    dimVal = 102;
                    break;
                case 0x07:
                    dimVal = 119;
                    break;
                case 0x08:
                    dimVal = 136;
                    break;
                case 0x09:
                    dimVal = 153;
                    break;
                case 0x0A:
                    dimVal = 170;
                    break;
                case 0x0B:
                    dimVal = 187;
                    break;
                case 0x0C:
                    dimVal = 204;
                    break;
                case 0x0D:
                    dimVal = 221;
                    break;
                case 0x0E:
                    dimVal = 238;
                    break;
                case 0x0F:
                    dimVal = 255;
                    break;
                default:
                    dimVal = 0;
                    break;
            }
//set each of the pwm channels to the value assigned to dimVal
            set_LED_PWM(CHAN_PB2, dimVal);
            set_LED_PWM(CHAN_PB3, dimVal);
            set_LED_PWM(CHAN_PD3, dimVal);
            set_LED_PWM(CHAN_PD6, dimVal);
            break;
        default:
            break;
    }
    return state;
}

int main(void)
{
    //set port C as inputs for the lightval
   DDRC = 0x00; PORTC = 0xFF;
   //enable pwm for leds
   LED_PWM_on();
   //enable timer
   TimerOn();
   //set timer to 50 ms
   TimerSet(50);
   //intialize the task
   int curState = dimInit;

   while(1)
   {
       curState = dim_Tick(curState);
       while(!TimerFlag);
       TimerFlag = 0;
   }
}

