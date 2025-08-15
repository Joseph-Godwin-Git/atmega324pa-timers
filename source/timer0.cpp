#include "timer0.h"

static uint32_t overflowCounter;

// float pwmPeriodMicros;
// float timerTickDurationMicros;
// float pwmFrequencyHz;
uint32_t nanosPerTick;

TIMER0::TIMER0()
{

}

void TIMER0::init() 
{
    init(Prescaler::None);
}

void TIMER0::init(Prescaler prescale)
{
    prescaler = prescale;
    applyPrescale(prescaler);

    // Set Fast PWM Mode
    TCCR0A |= (1<<WGM01) | (1<<WGM00);
    TCCR0B &= ~((1<<WGM02));

    // pwmFrequencyHz = F_CPU / (256.0f * static_cast<float>(prescaler));
    // pwmPeriodMicros = 1000000.0 / pwmFrequencyHz;
    // timerTickDurationMicros = pwmPeriodMicros / 256.0;
    nanosPerTick = (1000000000UL * static_cast<uint32_t>(prescaler)) / F_CPU;
}

void TIMER0::applyPrescale(Prescaler prescale)
{
    constexpr uint8_t CS0 = CS00;
    constexpr uint8_t CS1 = CS01;
    constexpr uint8_t CS2 = CS02;

    TCCR0B &= ~((1<<CS2) | (1<<CS1) | (1<<CS0)); // Clear prescalar bits

    switch(prescaler)
    {
        case Prescaler::None:
            TCCR0B |= (1<<CS0);
            break;
        case Prescaler::Clk_8:
            TCCR0B |= (1<<CS1);
            break;
        case Prescaler::Clk_64:
            TCCR0B |= (1<<CS1) | (1<<CS0);
            break;
        case Prescaler::Clk_256:
            TCCR0B |= (1<<CS2);
            break;
        case Prescaler::Clk_1024:
            TCCR0B |= (1<<CS2) | (1<<CS0);
            break;
        default:
            break; // No prescale
    }
}

void TIMER0::enablePwm(Channel ch)
{
    switch (ch)
    {
        case Channel::A:
            DDRB |= (1 << PB3); // OC0A = PB3
            break;
        case Channel::B:
            DDRB |= (1 << PB4); // OC0B = PB4
            break;
    }
}

void TIMER0::setPwmInvert(Channel ch, bool invert)
{
    constexpr uint8_t COMA1 = COM0A1;
    constexpr uint8_t COMA0 = COM0A0;
    constexpr uint8_t COMB1 = COM0B1;
    constexpr uint8_t COMB0 = COM0B0;

    switch(ch)
    {
        case Channel::A:
            TCCR0A |= (1<<COMA1);
            TCCR0A = (TCCR0A & ~(1 << COMA0)) | (invert << COMA0);
            break;
        case Channel::B:
            TCCR0A |= (1<<COMB1);
            TCCR0A = (TCCR0A & ~(1 << COMB0)) | (invert << COMB0);
            break;
    }
}

void TIMER0::setPwmCompareValue(Channel ch, uint8_t value)
{
    switch(ch)
    {
        case Channel::A:
            OCR0A = value;
            break;
        case Channel::B:
            OCR0B = value;
    }
}

void TIMER0::enableCounter(void)
{
    TIMSK0 |= (1<<TOIE0);
}

void TIMER0::disableCounter(void)
{
    TIMSK0 &= ~(1<<TOIE0);
}

uint32_t TIMER0::millis(void)
{
    uint8_t tcnt;
    uint32_t overflows;

    cli();
    tcnt = TCNT0;
    overflows = overflowCounter;
    sei();

    // total_ticks = 256 * overflows + tcnt
    uint32_t totalTicks = (overflows << 8) + tcnt;

    uint64_t nanos = static_cast<uint64_t>(totalTicks) * nanosPerTick;
    return static_cast<uint32_t>(nanos / 1000000UL);
}

void TIMER0::resetCounter(void)
{
    cli();
    overflowCounter = 0;
    TCNT0 = 0x00;
    sei();
}

ISR(TIMER0_OVF_vect)
{
    overflowCounter++;
}