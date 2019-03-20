/* LEDs */

#define LED_PORT	PORTC
#define LED_DDR		DDRC
#define LED_A		PC3

#define led_a_on()	LED_PORT |=  _BV(LED_A)
#define led_a_off()	LED_PORT &= ~_BV(LED_A)
#define led_a_toggle()	LED_PORT ^=  _BV(LED_A)

#define led_init() {LED_DDR |= _BV(LED_A);}

/* Ducks */
#define DUCK_DDR	DDRB
#define DUCK_A		PB2
#define DUCK_B		PB4
#define DUCK_C		PB5
