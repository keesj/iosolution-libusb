#include <libusb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "iosolution.h"

#define USB_VENDOR_ID 0x07a0
#define USB_PRODUCT_ID 0x1001

static libusb_context *ctx;

/* This method looks if the kernel already has a driver attached
 * to the device. if so I will take over the device.
 */
static int
takeover_device(libusb_device_handle * udev, int interface)
{
	char driver_name[255];

	memset(driver_name, 0, 255);
	int ret = -1;

	if (libusb_kernel_driver_active(udev, interface)) {
		printf("kernel is owning the device trying to take over\n");
		ret = libusb_detach_kernel_driver(udev, interface);
		if (ret != 0) {
			fprintf(stderr, "Failed to disconnect OS driver: %s\n",
			    libusb_error_name(ret));
			return 1;
		}
	}

	/* claim interface */
	ret = libusb_claim_interface(udev, interface);
	if (ret != 0) {
		fprintf(stderr, "Claim interface error: %s\n",
		    libusb_error_name(ret));
		return -1;
	} else {
		libusb_set_configuration(udev, 1);
//              printf("%s:%s,%d fixme..\n", __FILE__, __FUNCTION__, __LINE__);
		// libusb_set_interface_alt_settings(udev, interface);
	}
	return 0;
}

/* Iterates over the usb devices on the usb busses and returns a handle to the
 * first device found that matches the predefined vendor and product id
 */
libusb_device_handle *
open_device(void)
{
	/* TODO: replace this method with     
	handle = libusb_open_device_with_vid_pid(ctx, USB_VENDOR_ID, USB_PRODUCT_ID);
	*/

	libusb_device *dev;
	libusb_device_handle *handle;	/* todo malloc handle? */
	struct libusb_device_descriptor desc;

	libusb_device **list;
	int ret;

	handle = NULL;
	int i = 0;

	libusb_get_device_list(ctx, &list);
	while (list[i++] != NULL) {
		dev = list[i];
		ret = libusb_get_device_descriptor(dev, &desc);
		if (ret != 0) {
			fprintf(stderr,
			    "Failed to open the device descriptor: %s\n",
			    libusb_error_name(ret));
			continue;
		}

		if ((desc.idVendor == USB_VENDOR_ID) &&
		    (desc.idProduct == USB_PRODUCT_ID)) {
			ret = libusb_open(dev, &handle);
			if (ret != 0) {
				printf("Failed to open the device : %s\n",
				    libusb_error_name(ret));
				return NULL;
			}
			if (takeover_device(handle, 0)) {
				fprintf(stderr,
				    "Failed to take over the device\n");
				return NULL;
			}
			break;
		}
	}
	libusb_free_device_list(list, 1);

	return handle;
}

int
main(int argc, char **argv)
{
	struct ios_handle ios;
	libusb_init(&ctx);
	ios.device_handle = open_device();
	if (!ios.device_handle) {
		fprintf(stderr, "Failed to find the device\n");
		return -1;
	}
	ios.data[0] = 0x00;	/* power value of p0.3 and p0.2 */
	ios.data[1] = 0x00;	/* power value of p0.1 and p0.0 */
	ios.data[2] = 0x00;	/* power value of p1.3 and p1.2 */
	ios.data[3] = 0x00;	/* power value of p1.1 and p1.0 */
	ios.data[4] = 0x00;	/* pullup */
	ios.data[5] = 0x00;	/* pullup */
	ios.data[6] = 0x00;	/* port 1 state */
	ios.data[7] = 0x00;	/* port 0 state */

	if (argc < 2) {
		fprintf(stderr, "Usage: %s <int value>\n", argv[0]);
		return -1;

	}
	ios_set_data(&ios, IOS_PORT_0, atoi(argv[1]));

	libusb_close(ios.device_handle);
	return 0;
}
