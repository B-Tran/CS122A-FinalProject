#ifndef LED_PWM_H
#define LED_PWM_H
void set_LED_PWM(char led_frequency)
{
    OCR1A = led_frequency;
}

void LED_PWM_on()
{
    // COM3A0: Toggle PB1 on compare match between counter and OCR1A
    TCCR1A |= (1 << COM1A1) | (1 << WGM10);
    // WGM32: When counter (TCNT3) matches OCR2A, reset counter
    // CS01 & CS00: Set a prescalar of 64
    TCCR1B |= (1 << WGM12) | (1 << CS11);
    set_LED_PWM(0);
}

void LED_PWM_off()
{
    TCCR0A &= ~(1 << COM1A1) | ~(1 << WGM10);
    TCCR0B &= ~(1 << WGM12) & ~(1 << CS11);
}
#endif
