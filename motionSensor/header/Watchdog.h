#ifndef WATCHDOG_H
#define WATCHDOG_H
#include <avr/wdt.h>

void WDT_Init()
{
    //clear global interrupt
    cli();

    //reset watchdog
    wdt_reset();
    //set up WDT interrupt
    WDTCSR |= (1<<WDIE);

    WDTCSR |= (1<<WDCE) | (1 << WDE);

    //Start watchdog timer with 4 second prescaller
    WDTCSR = (1<<WDE)|(1<<WDP3);

    // this works for 4 seconds and sets up the ISR and a reboot
    // enable global interrupts
    sei();
}

ISR(WDT_vect)
{
    /* insert any code you need here */
}
#endif //WATCHDOG_H
