#ifndef LED_PWM_H
#define LED_PWM_H
//Summary: this is a header for led pwms for the atmega328p

//note: commented out code is for led pwm on PD5

//channel selection
enum PWM_CHANNEL {
    CHAN_PB2 = 1,
    CHAN_PB3 = (1<<1),
    CHAN_PD3 = (1<<2),
//    CHAN_PD5 = (1<<2),
    CHAN_PD6 = (1<<3)
};

//this sets the pwm frequency for the selected channels
void set_LED_PWM(enum PWM_CHANNEL channel, char led_frequency)
{
    if(channel & CHAN_PB2) OCR1B = led_frequency;
    if(channel & CHAN_PB3) OCR2A = led_frequency;
    if(channel & CHAN_PD3) OCR2B = led_frequency;
//    if(channel & CHAN_PD5) OCR0B = led_frequency;
    if(channel & CHAN_PD6) OCR0A = led_frequency;
}

//enables pwm for the atmega
void LED_PWM_on()
{
    //enable ports as pwm
    TCCR0A |= (1<<COM0A1); //PD6
//    TCCR0A |= (1<<COM0B1); //PD5
    TCCR1A |= (1<<COM1B1); //PB2
    TCCR2A |= (1<<COM2A1); //PD3
    TCCR2A |= (1<<COM2B1); //PD3

    //set mode of operation to PWM phase correct
    TCCR0A |= (1<<WGM00);
    TCCR1A |= (1<<WGM10);
    TCCR2A |= (1<<WGM20);

    //set prescaler to no prescaling
    TCCR0B |= (1<<CS00);
    TCCR1B |= (1<<CS10);
    TCCR2B |= (1<<CS20);

    //initializing the PWM pins
    DDRB |= (1 << PB2);
    DDRB |= (1 << PB3);
    PORTB &= ~(1 << PB2);
    PORTB &= ~(1 << PB3);
    DDRD |= (1 << PD3);
//    DDRD |= (1 << PD5);
    DDRD |= (1 << PD6);
    PORTD &= ~(1 << PD3);
//    PORTD &= ~(1 << PD5);
    PORTD &= ~(1 << PD6);

    //initialize channels
    set_LED_PWM(CHAN_PB2,0);
    set_LED_PWM(CHAN_PB3,0);
    set_LED_PWM(CHAN_PD3,0);
//    set_LED_PWM(CHAN_PD5,0);
    set_LED_PWM(CHAN_PD6,0);
}

//disables pwms and returns the pins to normal operation
void LED_PWM_off()
{
    TCCR0A &= ~(1<<COM0A1); //PD6
//    TCCR0A &= ~(1<<COM0B1); //PD5
    TCCR1A &= ~(1<<COM1B1); //PB2
    TCCR2A &= ~(1<<COM2A1); //PB3
    TCCR2A &= ~(1<<COM2B1); //PD3
}
#endif
