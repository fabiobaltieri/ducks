/*
 * Copyright 2013 Fabio Baltieri (fabio.baltieri@gmail.com)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <libusb.h>

#include "../firmware/requests.h"

#define VENDOR_ID 0x1d50
#define PRODUCT_ID 0x6061

static void set_leds(libusb_device_handle *usbdev, int mask)
{
	int ret;
	uint16_t usb_index;
	uint16_t usb_value;

	usb_index = 0;
	usb_value = mask & 0xffff;

	printf("%x\n", usb_value);
	ret = libusb_control_transfer(usbdev,
			LIBUSB_REQUEST_TYPE_VENDOR |
			LIBUSB_RECIPIENT_DEVICE |
			LIBUSB_ENDPOINT_IN,
			CUSTOM_RQ_SET_DUCK,
			usb_value, usb_index,
			NULL, 0, 1000);

	if (ret < 0) {
		printf("libusb_control_transfer: %s\n", libusb_error_name(ret));
		exit(1);
	}
}

int main(int argc, char **argv)
{
	libusb_context *ctx;
	libusb_device_handle *usbdev;
	int mask = 0;

	if (argc != 2) {
		fprintf(stderr, "syntax: %s <mask>\n", argv[0]);
		return 1;
	}

	libusb_init(&ctx);

	usbdev = libusb_open_device_with_vid_pid(ctx, VENDOR_ID, PRODUCT_ID);
	if (!usbdev) {
		fprintf(stderr, "error: could not find USB device %04x:%04x\n",
				VENDOR_ID, PRODUCT_ID);
		exit(1);
	}

	mask = strtol(argv[1], NULL, 0);
	set_leds(usbdev, mask);

	libusb_close(usbdev);

	libusb_exit(ctx);

	return 0;
}
