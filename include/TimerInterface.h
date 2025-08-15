#ifndef TIMERINTERFACE_H
#define TIMERINTERFACE_H

class TimerInferface
{
public:
    enum class Channel : uint8_t { A, B };

    /* Initialize Timer0 */
    virtual void init(void) = 0;

    /* PWM Functions */
    virtual void enablePwm(Channel ch) = 0;
    virtual void setPwmInvert(Channel ch, bool invert) = 0;
    virtual void setPwmCompareValue(Channel ch, uint8_t value) = 0;

    /* Stopwatch Functions */
    virtual void enableCounter(void) = 0;
    virtual void disableCounter(void) = 0;
    virtual uint32_t millis(void) = 0;
    virtual void resetCounter(void) = 0;
};
#endif // TIMERINTERFACE_H