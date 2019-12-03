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
#include "../header/spi_1284.h"
#include "../header/io.h"
#include "../header/usart_ATmega1284.h"
#include <avr/io.h>
/*---------------------------------------------------------------
  Declarations of enums for the state machines below
  ---------------------------------------------------------------*/
enum motionSensor{motionInit, motionWait, motionDetect};
enum uartRec {ur_Init, ur_Wait, ur_Receive};
/*---------------------------------------------------------------
  Declarations of global variables and #defines below
  ---------------------------------------------------------------*/
unsigned char data = 0x00;
unsigned char baseSignal = 0x00;
/*---------------------------------------------------------------
  Declaration of functions below
  ---------------------------------------------------------------*/

/*---------------------------------------------------------------
  Declaration of state machine functions below
  ---------------------------------------------------------------*/
int sensor_Tick(int state)   {
    /*state transitions*/
    switch(state)   {
        case motionInit:
            break;
        case motionWait:
            if(baseSignal)   {
                state = motionDetect;
            }
            else   {
                state = motionWait;
            }
            break;
        case motionDetect:
            if(baseSignal)   {
                state = motionDetect;
            }
            else   {
                state = motionWait;
            }
            break;
        default:
            state = motionInit;
            break;
    }
    /*state actions*/
    switch(state)   { 
        case motionInit:
            state = motionWait;
            break;
        case motionWait:
            PORTB = 0x00;
            break;
        case motionDetect:
            PORTB = 0x01;
            break;
        default:
            PORTB = 0xC0;
            break;
    }
    return state;
}

int ur_Tick(int state)   {
    switch(state)   {
        case ur_Init:
            break;
        case ur_Wait:
            if(USART_HasReceived(0))   {
                state = ur_Receive;
            }
            else   {
                state = ur_Wait;
            }
            break;
        case ur_Receive:
            state = ur_Wait;
            break;
        default:
            state = ur_Init;
            break;
    }
    switch(state)   {
        case ur_Init:
            state = ur_Wait;
            break;
        case ur_Wait:
            break;
        case ur_Receive:
            data = USART_Receive(0);
            if(data == 1)   {
                baseSignal = 1;
            }
            else   {
                baseSignal = 0;
            }
            break;
        default:
            data = 0x0F;
            break;
    }
    return state;
}


int main(void)
{
    /* Replace with your application code */
	//DDRB = 0xBF; PORTB = 0x40;
    DDRB = 0xFF; PORTB = 0x00; // LCD backlight
	//Initialization of SPI and Timer functionality
    initUSART(0);
    static task Task1,Task2;
    unsigned char i;
    unsigned long long taskPeriod = 50;
	unsigned char taskSize = 2;

    static task* tasks[] = {&Task1, &Task2};
    Task1.state = motionInit;
    Task1.period = 50;
    Task1.elapsedTime = Task1.period;
    Task1.TickFct = sensor_Tick;
    Task1.active = 1;
    Task2.state = ur_Init;
    Task2.period = 50;
    Task2.elapsedTime = Task2.period;
    Task2.TickFct = ur_Tick;
    Task2.active = 1;
    TimerOn();
	TimerSet(taskPeriod);
    while (1) 
    {
		for(i = 0; i < taskSize; i++)   {
            if(tasks[i]->active)
            {
                if(tasks[i]->elapsedTime >= tasks[i]->period)   {
                    tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
                    tasks[i]->elapsedTime = 0;
                }
                tasks[i]->elapsedTime += taskPeriod;
            }
		}
		while(!TimerFlag);
		TimerFlag = 0;
    }
}

