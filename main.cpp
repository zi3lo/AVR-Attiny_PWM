/*
 */

#include <avr/io.h>
#include <util/delay.h>

void adc_init();
uint8_t adc_read();
void pwm_init();
void pwm_set(uint8_t);
void delay_us(uint16_t);
uint8_t to_linear(uint8_t);

int main(void)
{
    DDRB |= (1 << PB0); // wyjście (nasz PWM)
    PORTB |= (1 << PB0);
    adc_init();
    pwm_init();
    uint8_t value, dir = 1, i = 255;
    while(1)
    {

        value = adc_read();

        if (value > 250)
        {
            pwm_set(1);
        }
        else
        {
            if (dir == 1)
            {
                pwm_set(to_linear(i--));
    //            pwm_set(i++);
                if (i == 0) dir = 0;
            }
            if (dir == 0)
            {

                pwm_set(to_linear(++i));
    //            pwm_set(--i);
                if (i == 255) dir = 1;
            }
            delay_us(12*value);
        }


    }
    return 0;
}

void adc_init()
{
    TCCR0B |= (1 << CS01);                                  // prescaler na 8 (8Mhz / 8 / 256 =~ 4kHz) enough ;)
    ADMUX |= (1 << ADLAR) | (1 << MUX1) | (1 << MUX0);      // leweostronnie, źródnło odniesienia VCC (domyślnie), wejście ADC3 (PB3)
    ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADEN);    // prescaler 64 (8MHz / 64 = 125kHz) i włączyć
}

uint8_t adc_read()
{
    ADCSRA |= (1 << ADSC);                                  // konwersja
    while (ADCSRA & (1 << ADSC));                           // po skończonej ADSC zmieni się na 1
    return ADCH;                                            // w ADCH 8 starszych bitów konwersji (bo leweostronnie)
}

void pwm_init()
{
    TCCR0A |= (1 << WGM01) | (1 << WGM00);  // kasuj OC0A (PB0) przy osągnięgciu, fast PWM do 0xff
    TCCR0A |= (1 << COM0A1) | (1 << COM0A0);
}

void pwm_set(uint8_t val)
{
    OCR0A = val;                                        // wartość dla pwm
}

void delay_us(uint16_t t)
{
    while(t--)
        _delay_us(1);
}
uint8_t to_linear(uint8_t val)
{
    uint8_t tmp = 255 - ((val * val) / 255);
   // if (tmp == 0)
//    if ((tmp == 0) && (val > 6))
//        return 1;
//    else
        return tmp;
}

