#ifndef STOPWATCH_H
#define STOPWATCH_H
#include <stdint.h>

/* !! ASSUMES 8MHz !! */
class Stopwatch
{
    enum class ClkFreq { _8000000Hz, _3686400Hz};
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
    public:
        Stopwatch();
        uint32_t millis(void);
        void enable(void);
        void disable(void);
    private:

};
#endif // STOPWATCH_H
