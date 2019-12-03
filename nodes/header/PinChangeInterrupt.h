#ifndef PIN_CHANGE_INTERRUPT_H
#define PIN_CHANGE_INTERRUPT_H
#include <avr/io.h>
unsigned char motionDetected = 0x00;
void PinChangeInit(void)
{
    //enable global interrupts
    SREG |= 0x80;
    //set pins[15:8] to be a pin change interrupt
    PCICR |= (1 << PCIE1);
    //set pin[8] to enable the pin change interrupt
    PCMSK1 |= (1 << PCINT8);
    //enable interupts
    sei();
}

ISR(PCINT1_vect)
{
    if(~PINB & 0x01)   {
        motionDetected = 0x00;
    }
    else   {
        motionDetected = 0x01;
    }
}
#endif //PIN_CHANGE_INTERRUPT_H
