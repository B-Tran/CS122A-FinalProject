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
#include "../header/PinChangeInterrupt.h"
#include <avr/io.h>
/*---------------------------------------------------------------
  Declarations of enums for the state machines below
  ---------------------------------------------------------------*/
enum motionSensor{motionInit, motionWait, motionDetect, motionLights};
enum uartRec {ur_Init, ur_Wait, ur_Receive};
enum uartSend {us_Init, us_Wait, us_Send};
/*---------------------------------------------------------------
  Declarations of global variables and #defines below
  ---------------------------------------------------------------*/
unsigned char lightCount = 0x00;
unsigned char data = 0x00;
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
            if(motionDetected)   {
                state = motionDetect;
            }
            else   {
                state = motionWait;
            }
            break;
        case motionDetect:
            state = motionLights;
            break;
        case motionLights:
            if(lightCount < 100)   {
                ++lightCount;
                state = motionLights;
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
            lightCount = 0;
            break;
        case motionDetect:
            PORTB = 0xFF;
            break;
        case motionLights:
            PORTB = 0xFF;
            break;
        default:
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
            break;
        default:
            break;
    }
    return state;
}

int us_Tick(int state)   {
    switch(state)   {
        case us_Init:
            break;
        case us_Wait:
            if(USART_IsSendReady(0) && motionDetected)   {
                state = us_Send;
            }
            else   {
                state = us_Wait;
            }
            break;
        case us_Send:
            if(motionDetected)   {
                state = us_Send;
            }
            else   {
                state = us_Wait;
            }
            break;
        default:
            state = us_Init;
            break;
    }
    switch(state)   {
        case us_Init:
            state = us_Wait;
            break;
        case us_Wait:
            USART_Send(0,0);
            break;
        case us_Send:
            USART_Send(motionDetected, 0);
            break;
        default:
            break;
    
    }
    return state;
}

int main(void)
{
    /* Replace with your application code */
	//DDRB = 0xBF; PORTB = 0x40;
    DDRA = 0x00; PORTA = 0xFF; // button inputs
    DDRB = 0xFF; PORTB = 0x00; // LCD backlight
	//Initialization of SPI and Timer functionality
    PinChangeInit();
    initUSART(0);
    static task Task1,Task2, Task3;
    unsigned char i;
    unsigned long long taskPeriod = 50;
	unsigned char taskSize = 3;

    static task* tasks[] = {&Task1, &Task2, &Task3};
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
    Task3.state = us_Init;
    Task3.period = 500;
    Task3.elapsedTime = Task3.period;
    Task3.TickFct = us_Tick;
    Task3.active = 1;
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

