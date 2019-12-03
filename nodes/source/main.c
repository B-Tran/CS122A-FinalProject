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
uint8_t lightCount = 0x00;
uint8_t data = 0x00;
uint8_t sendFlag = 0x00;
static task Task1,Task2, Task3;
uint8_t lightPort = 0x00;
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
            lightCount = 0;
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
            switch(data)   {
                case 0x30:
                    Task3.active = 0;
                    break;
                case 0x31:
                    Task3.active = 1;
                    break;
                case 0x32:
                    Task1.active = 0;
                    lightPort = 0xFF;
                    break;
                case 0x33:
                    Task1.active = 0;
                    lightPort = 0x00;
                    break;
                case 0x34:
                    Task1.active = 1;
                    break;
                default:
                    PORTB = 0xAB;
                    break;
            }
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
                    sendFlag = 0x00;
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
                break;
            case us_Send:
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
    /* Replace with your application code */
	//DDRB = 0xBF; PORTB = 0x40;
    DDRA = 0x00; PORTA = 0xFF; // button inputs
    DDRB = 0xFF; PORTB = 0x00; // LCD backlight
	//Initialization of SPI and Timer functionality
    PinChangeInit();
    initUSART(0);
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
        if(Task1.active == 0)   {
            PORTB = lightPort;
        }
    }
}

