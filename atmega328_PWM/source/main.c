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
#include "../header/timer16.h"
#include "../header/io.h"
#include "../header/ledPWM.h"


#include <avr/io.h>
//#include <util/delay.h>
#define F_CPU = 16000000UL

/*---------------------------------------------------------------
  Declarations of enums for the state machines below
  ---------------------------------------------------------------*/
/*---------------------------------------------------------------
  Declarations of global variables and #defines below
  ---------------------------------------------------------------*/

/*---------------------------------------------------------------
  Declaration of functions below
  ---------------------------------------------------------------*/

/*---------------------------------------------------------------
  Declaration of state machine functions below
  ---------------------------------------------------------------*/
int main(void)
{
   uint8_t timer = 0;
   uint8_t ledFlag = 0;
   LED_PWM_on();
   TimerOn();
   TimerSet(5);
   while(1)
   {
        //OCR1A = timer;
        //OCR1B = timer;
        set_LED_PWM(CHAN_PB2, timer);
        set_LED_PWM(CHAN_PD3, timer);
//        set_LED_PWM(CHAN_PD5, timer);
        set_LED_PWM(CHAN_PD6, timer);
       if(timer < 255 && !ledFlag) // fade low to high
       {
           timer++;
           //set_LED_PWM(timer);
       }
       else
       {
           ledFlag = 1;
       }
        if(timer > 1 && ledFlag)
        {
            timer--;
        }
        else
        {
            ledFlag = 0;
        }
       while(!TimerFlag);
       TimerFlag = 0;
   }
}

