#ifndef PWM_H
#define PWM_H
// 0.954 hz is the lowest frequency possible with this function,
// based on setting in PWM_on()
// Passing in 0 as the frequency will stop the speaker from generating
// sound
void set_PWM(double frequency)
{
    // Keeps track of the currently set frequency
    // Will only update thr registers when the frequency changes,
    // otherwise allows music to be played interupted
    static double current_frequency;

    if(frequency != current_frequency)
    {
        //stops timer/counter
        if(!frequency)
        {
            TCCR1B &= 0x08;
        }
        //resumes/continues timer/counter
        else
        {
            TCCR1B |= 0x03;
        }

        //prevents OCR3A from overflowing, using prescalar 64
        // 0.954 is the smallest frequenct that will not result in
        // overflow
        if(frequency < 0.954)
        {
            OCR1A = 0xFFFF;
        }
        // prevents OCR3A from underflowing, using prescalar 64
        // 31250 is the largest frequency that will not result in underflow
        else if(frequency > 31250)
        {
            OCR1A = 0x0000;
        }
        //set the OCR3A based on desired frequency
        else
        {
            OCR1A = (short) (8000000 / (128 * frequency)) - 1;
        }

        //resets counter
        TCNT1 = 0;
        //updates the current frequency
        current_frequency = frequency;
    }
}

void PWM_on()
{
    // COM3A0: Toggle PB1 on compare match between counter and OCR1A
    TCCR1A |= (1 << COM1A1) | (1 << WGM10);
    // WGM32: When counter (TCNT3) matches OCR2A, reset counter
    // CS01 & CS00: Set a prescalar of 64
    TCCR1B |= (1 << WGM12) | (1 << CS11) | (1 << CS10);
    set_PWM(0);
}

void PWM_off()
{
    TCCR0A = 0x00;
    TCCR0B = 0x00;
}
#endif
