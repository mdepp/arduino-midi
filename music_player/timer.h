/*
 * 
 */

#ifndef _TIMER_H
#define _TIMER_H

namespace Timer3
{
    namespace Internal
    {
        typedef void (*Callback) (void);
        Callback callback;

        
        // Called with frequency as set by initTimer. Assumes callback is set.
        ISR(TIMER3_COMPA_vect)
        {
            callback(); // Call the callback
        }
        
        struct ScalingData
        {
            uint16_t scaling;
            uint32_t compare_match;
            uint32_t real_frequency;
        };
        ScalingData calculateScaling(uint32_t clock_speed, uint32_t ideal_frequency)
        {
            uint8_t num_options = 5;
            uint16_t options[] = {1024, 256, 64, 8, 1};

            const uint32_t max_compare = 65535; // The maximum value the (16-bit) compare match register can be set to

            uint32_t smallest_difference;
            int8_t best_index = -1;

            for (int8_t i=0; i<num_options; ++i)
            {
                uint32_t compare_match = min(max_compare, clock_speed/options[i]/ideal_frequency-1);
                uint32_t real_frequency = clock_speed/options[i]/(compare_match+1);
                uint32_t difference = abs(ideal_frequency-real_frequency);

                if (best_index==-1 || difference < smallest_difference)
                {
                    best_index = i;
                    smallest_difference = difference;
                }
            }

            ScalingData re;
            re.scaling = options[best_index];
            re.compare_match = min(max_compare, clock_speed/re.scaling/ideal_frequency-1);
            re.real_frequency = clock_speed/re.scaling/(re.compare_match+1);
            return re;
        }
        uint8_t getScalingFlags(uint16_t scaling)
        {
            switch (scaling)
            {
                case 1: return (1<<CS30);
                case 8: return (1<<CS31);
                case 64: return (1<<CS31) | (1<<CS30);
                case 246: return (1<<CS32);
                case 1024: return (1<<CS32) | (1<<CS30);
                default: return (1<<CS30); // clk/1 for invalid options
            }
        }
    }

    /*
     * Initialize timer. Attempts to match clock settings with specified frequency,
     * and will return the actual frequency that the callback is called.
     */
    uint32_t initTimer(uint32_t clock_speed, uint32_t ideal_frequency, Internal::Callback cb)
    {
        Internal::callback = cb;
        // Initialize timer3

        cli(); // Disable global interrupts
        TCCR3A = 0;
        TCCR3B = 0;
        // Calculate scaling
        Internal::ScalingData sd = Internal::calculateScaling(clock_speed, ideal_frequency);

        TCCR3B |= (1 << WGM32); // Turn on CTC mode
        TCCR3B |= Internal::getScalingFlags(sd.scaling); // Turn on appropriate scaling
        TIMSK3 |= (1 << OCIE3A); // Enable compare interrupt
        
        OCR3A = sd.compare_match; // Set compare match register to desired timer count

        sei(); // Enable global interrupts

        return sd.real_frequency;
    }

    // Stop timer1
    void quitTimer()
    {
        // Clear CS30, 1, 2
        TCCR3B &= ~(1 << CS30);
        TCCR3B &= ~(1 << CS31);
        TCCR3B &= ~(1 << CS32);
    }
};


#endif // _TIMER_H