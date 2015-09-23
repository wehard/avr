#include <avr/io.h>
#define F_CPU 1000000UL  // 1 MHz
#include <util/delay.h>

// Function prototypes
int set_PORTB_bit(int position, int value);

// Main loop
int main()
{
	// Set pins to output
	DDRB |= (1 << PB0) | (1 << PB1);

        while(1)
	{
		set_PORTB_bit(0,1);
		set_PORTB_bit(1,0);
		_delay_ms(500); // Wait 250ms
		set_PORTB_bit(0,0);
		set_PORTB_bit(1,1);
		_delay_ms(500);	// Wail 250ms
    }

    return 1;
}

int set_PORTB_bit(int position, int value)
{
        // Sets or clears the bit in position 'position' 
        // either high or low (1 or 0) to match 'value'.
        // Leaves all other bits in PORTB unchanged.
        
        if (value == 0)
        {
                PORTB &= ~(1 << position);      // Set bit # 'position' low
        }
        else
        {
                PORTB |= (1 << position);       // Set bit # 'position' high
        }
        return 1;
}
