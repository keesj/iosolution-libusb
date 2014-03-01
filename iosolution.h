#ifndef __IOSOLUTION_H__
#define __IOSOLUTION_H__
#include <libusb.h>
#include <stdio.h>
#define SEND_PACKET_LEN 8
#define USB_TIMEOUT 1000

#define IOS_PORT_0 0
#define IOS_PORT_1 1

#define IO_PIN_0 (1 << 0)
#define IO_PIN_1 (1 << 1)
#define IO_PIN_2 (1 << 2)
#define IO_PIN_3 (1 << 3)
#define IO_PIN_4 (1 << 4)
#define IO_PIN_5 (1 << 5)
#define IO_PIN_6 (1 << 6)
#define IO_PIN_7 (1 << 7)

#define IO_PIN_ALL 0xFF

struct ios_handle
{
	unsigned char data[8];
	/* pointer to the usb handle */
	libusb_device_handle *device_handle;
};

/*
  ios_handle.data[0] = 0xff;  power value of p0.3 and p0.2 
  ios_handle.data[1] = 0xff;  power value of p0.1 and p0.0 
  ios_handle.data[2] = 0xff;  power value of p1.3 and p1.2 
  ios_handle.data[3] = 0xff;  power value of p1.1 and p1.0 
  ios_handle.data[4] = 0x00;  value of the pull-ups
  ios_handle.data[5] = 0x00;  value of the pull-ups
  ios_handle.data[6] = 0x00;  value of port 1
  ios_handle.data[7] = 0x00;  value of port 0
*/

/*
 * Set the data register of a given
 * port to the specified value.
 * 
 * Each port has 8 output but on the
 * iosolution chip only 12 outputs are guided to the outside. 
 * Therefore for the second port only the  4 lsb outputs are
 * are guided to the outside
 * 
 * return 0 on success
 */
int ios_set_data(struct ios_handle *handle, int port, char value);

/*
 * Gets the data registers for the given port
 * NOTE: I started using this method but stopped
 * after a short time. Here is the reason.
 * You can only get data back if the state changed
 * between the last call to ios_get_data. making  it pretty
 * useless
 */
int ios_get_data(struct ios_handle *handle, int port, char value);

/*
 * Set the pullup register of a given port.
 * to the specified value.
 *
 * Setting a value to 0b enables the pullup
 * the 4 msb outputs of the second port are not guided to the outside and 
 * it is advisable to put their state to have an enabled pullup
 */
int ios_set_pull_up(struct ios_handle *handle, int port, char value);

/*
 * Output textual information about the current output state
 * the second argument is the stream to use to output the data.
 *
 * Use stderr or stdout to output to the console
 */
void ios_dump_state(struct ios_handle *handle, FILE * stream);

/*
 * Send the data given in the handle to the device
 * this method gets called by other methods
 */
int ios_send(struct ios_handle *handle);
#endif /* __IOSOLUTION_H__ */
