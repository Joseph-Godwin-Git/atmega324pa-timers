#include <avr/io.h>
#include "pwm.h"

#define COMB0 4
#define COMB1 5
#define COMA0 6
#define COMA1 7

PWM::PWM(Timer tr, Channel ch)
{
    timer = tr;
    channel = ch;
    switch(timer)
    {
        case Timer::Timer0:
        {
            registers.TCCRA = &TCCR0A;
            registers.TCCRB = &TCCR0B;
            registers.OCRA = &OCR0A;
            registers.OCRB = &OCR0B;
            registers.OCRA_16 = nullptr;
            registers.OCRB_16 = nullptr;
            registers.DDR = &DDRB;
            registers.PIN = (Channel::A == ch) ? PB3 : PB4;
            // Set to Fast PWM w/ Top 0xFF
            TCCR0A |= (1<<WGM01) | (1<<WGM00);
            TCCR0B &= ~(1<<WGM02);
            break;
        }
        case Timer::Timer1:
        {
            registers.TCCRA = &TCCR1A;
            registers.TCCRB = &TCCR1B;
            registers.OCRA = nullptr;
            registers.OCRB = nullptr;
            registers.OCRA_16 = &OCR1A;
            registers.OCRB_16 = &OCR1B;
            registers.DDR = &DDRD;
            registers.PIN = (Channel::A == ch) ? PD5 : PD4;
            // Set to Fast PWM w/ Top 0xFF
            TCCR1A &= ~(1<<WGM13);
            TCCR1A |= (1<<WGM12);
            TCCR1B &= ~(1<<WGM11);
            TCCR1B |= (1<<WGM10);
            break;
        }
        case Timer::Timer2:
        {
            registers.TCCRA = &TCCR2A;
            registers.TCCRB = &TCCR2B;
            registers.OCRA = &OCR2A;
            registers.OCRB = &OCR2B;
            registers.OCRA_16 = nullptr;
            registers.OCRB_16 = nullptr;
            registers.DDR = &DDRD;
            registers.PIN = (Channel::A == ch) ? PD7 : PD6;
            // Set to Fast PWM w/ Top 0xFF
            TCCR2A |= (1<<WGM21) | (1<<WGM20);
            TCCR2B &= ~(1<<WGM22);
            break;
        }
        default:
        {
            return;
        }
    }

    setPrescaler(Prescaler::None);
    setInvert(false);
    setDutyCyclePercent(50);
    enable();
}

void PWM::setPrescaler(Prescaler ps)
{
    /* Table access depends on Prescaler and Timer enums being default values (0,1,2,3) etc
     * Returns the appropriate CSnX bits to configure the prescaler in TCCRnB when accessed:
     * PRESCALARS[Timer][Prescaler] */
    static constexpr uint8_t PRESCALERS[3][7] = {
        {0b001, 0b010, 0b000, 0b011, 0b000, 0b100, 0b101},
        {0b001, 0b010, 0b000, 0b011, 0b000, 0b100, 0b101},
        {0b001, 0b010, 0b011, 0b100, 0b101, 0b110, 0b111}
    };

    prescale = ps;
    volatile uint8_t &tccrb = *(registers.TCCRB);
    if(((uint8_t)ps > 6) || ((uint8_t)timer > 2)) { 
        return;
    }

    uint8_t regVal = PRESCALERS[(uint8_t)timer][(uint8_t)prescale];
    tccrb &= 0b11111000; // Clear last 3 bits
    tccrb |= regVal;
}

void PWM::enable(void)
{
    *(registers.DDR) |= (1 << registers.PIN);
}

void PWM::setInvert(bool invert)
{
    volatile uint8_t &tccra = *(registers.TCCRA);
    uint8_t shiftVal = (invert) ? 1 : 0;

    switch(channel)
    {
        case Channel::A:
            tccra |= (1<<COMA1); // Set to 1 for both clear on match and set on match
            tccra = (tccra & ~(1 << COMA0)) | (shiftVal << COMA0);  
            break;
        case Channel::B:
            tccra |= (1<<COMB1);
            tccra = (tccra & ~(1 << COMB0)) | (shiftVal << COMB0); 
            break;
    }  
}

void PWM::setDutyCyclePercent(float percent)
{
    constexpr uint8_t MAX_RAW = 255;
    if(percent > 100.0f){percent = 100.0f;}
    else if(percent < 0.0f){percent = 0.0f;}

    float raw = MAX_RAW * (percent/100.0f);
    raw += 0.5f; // Forces rounding instead of truncating
    setDutyCycleRaw((uint8_t)raw);
}

void PWM::setDutyCycleRaw(uint8_t value)
{
    switch(channel)
    {
        case Channel::A:
            if(timer == Timer::Timer1){*(registers.OCRA_16) = (uint16_t)value;}
            else{*(registers.OCRA) = value;}
        break;
        case Channel::B:
            if(timer == Timer::Timer1){*(registers.OCRB_16) = (uint16_t)value;}
            else{*(registers.OCRB) = value;}
        break;
    }
}