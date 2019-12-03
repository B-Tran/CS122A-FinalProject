/*	Author: Brandon Tran btran054@ucr.edu
 *  Partner(s) Name: Angel Renteria arent007@ucr.edu
 *	Lab Section: 023
 *	Assignment: Lab 6  Exercise 3
 *	Exercise Description: [optional - include for your own benefit]
 *  This lab exercise focuses on implementing SPI communication between two
 *  atmega 1284 microcontrollers. In this particular lab exercise, we have a master
 *  that transmits two types of data placed into one 8-bit value. The 8-bit value contains
 *  both a pattern value and speed value. This pattern value is sent to the servant and display on a bank of 8 LEDs.
 *  Depending on the pattern value sent, the pattern displayed will change accordingly. The speed value changes the frequency at which
 *  the pattern is displayed at. The biggest difference is that this challenge problem implements multiple servant that can be selected to
 *  change the pattern and speed, without changing the other servants. The changes made in this lab allow for a particular servant to be selected
 *  and changed. This source file contains the master code for the exercise.
 * 
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */

#include "../header/scheduler.h"
#include "../header/timer.h"
#include "../header/io.h"
#include "../header/ledPWM.h"


#include <avr/io.h>
//#include <util/delay.h>
#define F_CPU = 8000000UL

/*---------------------------------------------------------------
  Declarations of enums for the state machines below
  ---------------------------------------------------------------*/
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
int dim_Tick(int state)   {
    uint8_t dimVal;
    switch(state)   {
        case dimInit:
            break;
        case dimSet:
            state = dimSet;
            break;
        default:
            break;
    }
    switch(state)   { 
        case dimInit:
            state = dimSet;
            break;
        case dimSet:
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
   DDRC = 0x00; PORTC = 0xFF;
   LED_PWM_on();
   TimerOn();
   TimerSet(50);
   int curState = dimInit;
   while(1)
   {
       curState = dim_Tick(curState);
       while(!TimerFlag);
       TimerFlag = 0;
   }
}

