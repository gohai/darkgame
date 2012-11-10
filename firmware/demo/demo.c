#include <avr/io.h>
#include <util/delay.h>

int main (void)
{
	DDRB |= 0xF0;
	DDRC |= 0xF0;

	while (1) {
		PORTB |= 0xF0;
		PORTC |= 0XF0;
		_delay_ms(10000);

		PORTB &= 0x0F;
		PORTC &= 0x0F;
		_delay_ms(10000);
	}

	return 0;
}
