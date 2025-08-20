#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include "stopwatch.h"

volatile uint32_t milliseconds = 0;

Stopwatch::Stopwatch()
{
    TCCR2B = 0;
    TCCR2A = 0;

    /**
     * At 8MHz, each tick takes 0.125 us (1 / 8000000)
     * Each 1ms interval takes 8000 ticks (1000us = 1ms) => (1000us / 0.125us) = 8000
     * We need to adjust the prescaler to get the 8000 value within the 8-bit range 0-255
     * 8000 / 8 = 1000 (Too high)
     * 8000 / 64 = 125 (perfect!)
     * Using a prescale of 64 means that when we hit 125 total ticks, exactly 1ms has passed
     * This is the value we set OCRA to and use to trigger our interrupt to increment our millisecond counter
     */

    /* Set CTC Mode: WGM2:0 = 010 */
    TCCR2B |= (0 << WGM22);
    TCCR2A |= (1 << WGM21) | (0 << WGM20);

    /* Prescale Clk / 64: CS2:0 = 100*/
    TCCR2B |= (1 << CS22) | (0 << CS21) | (0 << CS20);

    /* Set TOP: Total Calculated Ticks - 1 */
    OCR2A = 124;

    /* Enable the COMPA Interrupt */
    TIMSK2 |= (1<<OCIE2A);
}

uint32_t Stopwatch::millis(void)
{
    uint32_t millis;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
        millis = milliseconds;
    }
    return millis;
}

void Stopwatch::enable(void)
{
    /* Prescale Clk / 64: CS2:0 = 100*/
    TCCR2B |= CS22;
    TCCR2B &= ~(1<<CS21);
    TCCR2B &= ~(1<<CS20);
}

void Stopwatch::disable(void)
{
    TCCR2B &= ~(1<<CS22);
    TCCR2B &= ~(1<<CS21);
    TCCR2B &= ~(1<<CS20);
}

ISR(TIMER2_COMPA_vect)
{
    milliseconds++;
}