#ifndef PWM_H
#define PWM_H
#include <stdint.h>

class PWM
{
public:
    enum class Timer : uint8_t {Timer0 = 0, Timer1 = 1, Timer2 = 2};
    enum class Channel : uint8_t {A, B};
    enum class Prescaler : uint16_t 
    {
        None = 0,
        Clk_8 = 1,
        Clk_32 = 2,
        Clk_64 = 3,
        Clk_128 = 4,
        Clk_256 = 5,
        Clk_1024 = 6
    };

    PWM(Timer timer, Channel ch);
    void setPrescaler(Prescaler prescale);
    void enable();
    void setInvert(bool invert);
    void setDutyCyclePercent(float percent);
    void setDutyCycleRaw(uint8_t raw);
private:
    typedef struct
    {
        volatile uint8_t *TCCRA;
        volatile uint8_t *TCCRB;
        volatile uint8_t *OCRA;
        volatile uint8_t *OCRB;
        volatile uint16_t *OCRA_16;
        volatile uint16_t *OCRB_16;
        volatile uint8_t *DDR;
        volatile uint8_t PIN;
    } REG;

    REG registers;
    Timer timer;
    Channel channel;
    Prescaler prescale;
};
#endif // PWM_H