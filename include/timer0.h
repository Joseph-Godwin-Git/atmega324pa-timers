#ifndef TIMER0_H
#define TIMER0_H
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include "TimerInterface.h"

class TIMER0 : public TimerInferface 
{
public:
    enum class Prescaler : uint16_t
    {
        None = 1,
        Clk_8 = 8,
        Clk_64 = 64,
        Clk_256 = 256,
        Clk_1024 = 1024
    };

    TIMER0(); // Default constructor

    void init() override;
    void init(Prescaler prescale);
    void enablePwm(Channel ch);
    void setPwmInvert(Channel ch, bool invert) override;
    void setPwmCompareValue(Channel ch, uint8_t value) override;

    void enableCounter(void);
    void disableCounter(void);
    uint32_t millis(void);
    void resetCounter(void);
private:
    Prescaler prescaler;
    void applyPrescale(Prescaler prescale);
};
#endif // TIMER0_H