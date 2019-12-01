#ifndef PIN_CHANGE_INTERRUPT_H
#define PIN_CHANGE_INTERRUPT_H
#include <avr/io.h>

void PinChangeInit(void)
{
    //enable global interrupts
    SREG |= 0x80;
    //set pins[7:0] to be a pin change interrupt
    PCICR |= (1 << PCIE0);
    //set pin[0] to enable the pin change interrupt
    PCMSK0 |= (1 << PCINT0);
    //enable interupts
    sei();
}

ISR(PCINT0_vect)
{
    if(~PINA & 0x01)   {
        PORTB = 0x00;
    }
    else   {
        PORTB = 0x01;
    }
}
#endif //PIN_CHANGE_INTERRUPT_H
