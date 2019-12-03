#ifndef LED_PWM_H
#define LED_PWM_H

enum PWM_CHANNEL {
    CHAN_PB2 = 1,
    CHAN_PD3 = (1<<1),
//    CHAN_PD5 = (1<<2),
    CHAN_PD6 = (1<<3)
};

void set_LED_PWM(enum PWM_CHANNEL channel, char led_frequency)
{
    if(channel & CHAN_PB2) OCR1B = led_frequency;
    if(channel & CHAN_PD3) OCR2B = led_frequency;
//    if(channel & CHAN_PD5) OCR0B = led_frequency;
    if(channel & CHAN_PD6) OCR0A = led_frequency;
}

void LED_PWM_on()
{
    //enable ports as pwm
    TCCR0A |= (1<<COM0A1); //PD6
//    TCCR0A |= (1<<COM0B1); //PD5
    TCCR1A |= (1<<COM1B1); //PB2
    TCCR2A |= (1<<COM2B1); //PD3
    //set mode of operation
    TCCR0A |= (1<<WGM00);
    TCCR1A |= (1<<WGM10);
    TCCR2A |= (1<<WGM20);
    //set prescaler
    TCCR0B |= (1<<CS00);
    TCCR1B |= (1<<CS10);
    TCCR2B |= (1<<CS20);

    DDRB |= (1 << PB2);
    PORTB &= ~(1 << PB2);
    DDRD |= (1 << PD3);
//    DDRD |= (1 << PD5);
    DDRD |= (1 << PD6);
    PORTD &= ~(1 << PD3);
//    PORTD &= ~(1 << PD5);
    PORTD &= ~(1 << PD6);

    //initialize channels
    set_LED_PWM(CHAN_PB2,0);
    set_LED_PWM(CHAN_PD3,0);
//    set_LED_PWM(CHAN_PD5,0);
    set_LED_PWM(CHAN_PD6,0);
}

void LED_PWM_off()
{
    TCCR0A &= ~(1<<COM0A1); //PD6
//    TCCR0A &= ~(1<<COM0B1); //PD5
    TCCR1A &= ~(1<<COM1B1); //PB2
    TCCR2A &= ~(1<<COM2B1); //PD3
}
#endif
