/*	Author: Brandon Tran btran054@ucr.edu
 *  Partner(s) Name: Angel Renteria arent007@ucr.edu
 *	Lab Section: 023
 *	Assignment: Final Project
 *	Exercise Description: [optional - include for your own benefit]
 *  This is the code for the subcontroller, which communicates to the
 *  base station when motion is detected and controls the lights
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include "../header/scheduler.h"
#include "../header/timer.h"
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
uint8_t lightCount = 0x00;  //counter for light durration
uint8_t data = 0x00;        //gets data from UART/xBee
uint8_t dataFlag = 0x00;
static task Task1,Task2, Task3; //the tasks
uint8_t lightPort = 0xFF;   //output light codes
uint8_t sendFlag = 0x00;    //flag to check when send is complete
/*---------------------------------------------------------------
  Declaration of functions below
  ---------------------------------------------------------------*/
void setDimness();
/*---------------------------------------------------------------
  Declaration of state machine functions below
  ---------------------------------------------------------------*/
/*---------------------------------------------------------------
 * This state machine is for the detection of the motion sensor
 * --------------------------------------------------------------*/
int sensor_Tick(int state)   {
    /*state transitions*/
    switch(state)   {
        case motionInit:
            break;
        case motionWait:
            //waits until motion is detected
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
            //waits until the light duration has ended
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
            //turns off the lights and resets counter
            //while waiting
            PORTA = 0x00;
            lightCount = 0;
            break;
        case motionDetect:
            //turns on the lights
            //and intializes the light counter
            lightCount = 0;
            PORTA = lightPort;
            break;
        case motionLights:
            //keep displaying the lights
            PORTA = lightPort;
            break;
        default:
            break;
    }
    return state;
}

/*---------------------------------------------------------------
 * This state machine is for the retrieval of 
 * data from the xBee via UART
 * --------------------------------------------------------------*/
int ur_Tick(int state)   {
    /*state transitions*/
    switch(state)   {
        case ur_Init:
            break;
        case ur_Wait:
            //wait until the data has been recieved
            if(USART_HasReceived(0))   {
                state = ur_Receive;
            }
            else   {
                state = ur_Wait;
            }
            break;
        case ur_Receive:
            //return to wait once data has been retrieved
            state = ur_Wait;
            break;
        default:
            state = ur_Init;
            break;
    }
    /*state actions*/
    switch(state)   {
        case ur_Init:
            state = ur_Wait;
            break;
        case ur_Wait:
            break;
        case ur_Receive:
            //get the data from the xBee
            data = USART_Receive(0);
            //enable or disable tasks based on
            //the data recieved from the xBee
            if(dataFlag == 0x00)   {
                switch(data)   {
                    case 0x30:
                        Task3.active = 0;
                        break;
                    case 0x31:
                        Task3.active = 1;
                        break;
                    case 0x32:
                        Task1.active = 0;
                        dataFlag = 1;
                        break;
                    case 0x33:
                        Task1.active = 0;
                        lightPort = 0x00;
                        break;
                    case 0x34:
                        Task1.active = 1;
                        dataFlag = 1;
                        break;
                    default:
                        PORTA = 0x01;
                        break;
                }
            }
            else   {
                lightPort = data;
                dataFlag = 0x00;
            }
            break;
        default:
            break;
    }
    return state;
}

/*---------------------------------------------------------------
 * This state machine is for the tranmission of 
 * motion detection from the xBee via UART
 * --------------------------------------------------------------*/
int us_Tick(int state)   {
    /*state transitions*/
        switch(state)   {
            case us_Init:
                break;
            case us_Wait:
                //waits until motion is detected and is ready
                //to send
                if(USART_IsSendReady(0) && motionDetected)   {
                    state = us_Send;
                }
                else   {
                    state = us_Wait;
                }
                break;
            case us_Send:
                //send the data while motion is detected
                if(motionDetected)   {
                    state = us_Send;
                }
                else   {
                    state = us_Wait;
                    sendFlag = 0x00;
                }
                break;
            default:
                state = us_Init;
                break;
        }
    /*state actions*/
        switch(state)   {
            case us_Init:
                state = us_Wait;
                break;
            case us_Wait:
                break;
            case us_Send:
                //if the data is ready to send
                //send the data to the xBee and mark
                //the uart as busy
                if(sendFlag == 0)   {
                    USART_Send(motionDetected, 0);
                    sendFlag = 0x01;
                }
                break;
            default:
                break;
    
        }
    return state;
}

int main(void)
{
    //initializing PORTA as output
    //for the light codes
    DDRA = 0xFF; PORTA = 0x00;
    //initializing PORTB as input for
    //the motion sensors
    DDRB = 0x00; PORTB = 0xFF;
    
    //initialize the pin interrupt on PB1
    PinChangeInit();
    //initialize uart 0 for communication to xBee
    initUSART(0);
    //initialization of tasks
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
    Task3.active = 0;
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
        if(Task1.active == 0x00)   {
            PORTA = lightPort;
        }
    }
}

