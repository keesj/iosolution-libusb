#include <usb.h>
#include <libusb.h>
#include <stdio.h>
#include "iosolution.h"

#define SEND_PACKET_LEN 8
#define USB_TIMEOUT 1000

/* For API documentation see iosolution.h */
int
ios_send(struct ios_handle *handle)
{

	/* please fix, this is a little to much magic */
	/* replace 33 0x09 and the others by constants */
	/* 
	 * grabbed from a running system using iosolution
	 * c45861c0 1390935291 S Co:4:002:0 s 22 09 0000 0000 0008 8 = 00000000 00000001
	 * c45861c0 1390938913 C Co:4:002:0 0 8 >
	 * e81d98c0 1410668658 S Co:4:002:0 s 22 09 0000 0000 0008 8 = 00000000 0000ffff
	 * e81d98c0 1410671680 C Co:4:002:0 0 8 >
	 * */
	int writen_size = libusb_control_transfer(handle->device_handle,
	    USB_TYPE_CLASS | USB_RECIP_INTERFACE,
	    0x09, 0x00,
	    0,
	    handle->data,
	    SEND_PACKET_LEN, 1000);

	if (writen_size != SEND_PACKET_LEN) {
		fprintf(stderr,
		    "Failed to send all the byte of the packet (%i)\n",
		    writen_size);
		return -1;
	}

	return 0;
}

int
ios_set_data(struct ios_handle *handle, int port, char value)
{
	/* When we send the data byte 6 and 7 are the port data registers
	 * register of port 1 is written first and then only the register of
	 * port 0 */
	handle->data[7 - port] = value;
	return ios_send(handle);
}

int
ios_get_data(struct ios_handle *handle, int port, char value)
{
	unsigned char data[8];
	int ret;
	int len;

	ret = libusb_interrupt_transfer(handle->device_handle, 1, data, 8,&len, 5000);

	if (ret > 0) {
		fprintf(stderr, "Got a notification %8x %i\n", data[0], ret);
	} else {
		fprintf(stderr, "No notification %08x %i\n", data[0], ret);
	}
	return -1;
}

int
ios_set_pull_up(struct ios_handle *handle, int port, char value)
{
	/* When we send the data byte 6 and 7 are the port data registers
	 * register of port 1 is written first and then only the register of
	 * port 0 */
	handle->data[5 - port] = value;
	return ios_send(handle);
}

void
ios_dump_state(struct ios_handle *handle, FILE * stream)
{
	fprintf(stream, "iosolution output:\n");
}
