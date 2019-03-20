/*
 * Copyright 2019 Fabio Baltieri (fabio.baltieri@gmail.com)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 */

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "usbdrv.h"

#include "board.h"
#include "requests.h"

static void reset_cpu(void)
{
	wdt_disable();
	wdt_enable(WDTO_15MS);
	for (;;);
}

static void set_leds(uint16_t mask)
{
	if (mask & 0x01)
		DUCK_DDR |= _BV(DUCK_A);
	else
		DUCK_DDR &= ~_BV(DUCK_A);

	if (mask & 0x02)
		DUCK_DDR |= _BV(DUCK_B);
	else
		DUCK_DDR &= ~_BV(DUCK_B);

	if (mask & 0x04)
		DUCK_DDR |= _BV(DUCK_C);
	else
		DUCK_DDR &= ~_BV(DUCK_C);
}


usbMsgLen_t usbFunctionSetup(uint8_t data[8])
{
	struct usbRequest *rq = (void *)data;

	switch (rq->bRequest) {
	case CUSTOM_RQ_SET_DUCK:
		set_leds(rq->wValue.word);
		break;
	case CUSTOM_RQ_RESET:
		reset_cpu();
		break;
	default:
		/* do nothing */
		;
	}

	return 0;
}

static void hello(void)
{
	uint8_t i;

	for (i = 0; i < 8; i++) {
		led_a_toggle();
		_delay_ms(50);
	}
}

int main(void)
{
	uint8_t i;

	led_init();

	wdt_enable(WDTO_1S);

	hello();
	led_a_on();

	usbInit();
	usbDeviceDisconnect();

	i = 0;
	while (--i) {
		wdt_reset();
		_delay_ms(1);
	}

	usbDeviceConnect();

	sei();
	for (;;) {
		wdt_reset();
		usbPoll();
	}
}
