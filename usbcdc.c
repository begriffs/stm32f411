#include <FreeRTOS.h>
#include <queue.h>
#include <task.h>
#include <stdbool.h>
#include <ctype.h>

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/usb/usbd.h>
#include <libopencm3/usb/cdc.h>

/**** USB DESCRIPTORS ***********************************/

const struct usb_device_descriptor dev = {
	.bLength = USB_DT_DEVICE_SIZE,
	.bDescriptorType = USB_DT_DEVICE,
	.bcdUSB = 0x0200,
	.bDeviceClass = USB_CLASS_CDC,
	.bDeviceSubClass = 0,
	.bDeviceProtocol = 0,
	.bMaxPacketSize0 = 64,
	.idVendor = 0x0483,
	.idProduct = 0x5740,
	.bcdDevice = 0x0200,
	.iManufacturer = 1,
	.iProduct = 2,
	.iSerialNumber = 3,
	.bNumConfigurations = 1,
};

const struct usb_endpoint_descriptor data_endp[] = {
	{
		.bLength = USB_DT_ENDPOINT_SIZE,
		.bDescriptorType = USB_DT_ENDPOINT,
		.bEndpointAddress = 0x01,
		.bmAttributes = USB_ENDPOINT_ATTR_BULK,
		.wMaxPacketSize = 64,
		.bInterval = 1,
	}, {
		.bLength = USB_DT_ENDPOINT_SIZE,
		.bDescriptorType = USB_DT_ENDPOINT,
		.bEndpointAddress = 0x82,
		.bmAttributes = USB_ENDPOINT_ATTR_BULK,
		.wMaxPacketSize = 64,
		.bInterval = 1,
	}
};

const struct {
	struct usb_cdc_header_descriptor header;
	struct usb_cdc_call_management_descriptor call_mgmt;
	struct usb_cdc_acm_descriptor acm;
	struct usb_cdc_union_descriptor cdc_union;
} __attribute__((packed)) cdcacm_functional_descriptors = {
	.header = {
		.bFunctionLength = sizeof(struct usb_cdc_header_descriptor),
		.bDescriptorType = CS_INTERFACE,
		.bDescriptorSubtype = USB_CDC_TYPE_HEADER,
		.bcdCDC = 0x0110,
	},
	.call_mgmt = {
		.bFunctionLength =
			sizeof(struct usb_cdc_call_management_descriptor),
		.bDescriptorType = CS_INTERFACE,
		.bDescriptorSubtype = USB_CDC_TYPE_CALL_MANAGEMENT,
		.bmCapabilities = 0,
		.bDataInterface = 1,
	},
	.acm = {
		.bFunctionLength = sizeof(struct usb_cdc_acm_descriptor),
		.bDescriptorType = CS_INTERFACE,
		.bDescriptorSubtype = USB_CDC_TYPE_ACM,
		.bmCapabilities = 0,
	},
	.cdc_union = {
		.bFunctionLength = sizeof(struct usb_cdc_union_descriptor),
		.bDescriptorType = CS_INTERFACE,
		.bDescriptorSubtype = USB_CDC_TYPE_UNION,
		.bControlInterface = 0,
		.bSubordinateInterface0 = 1,
	}
};

const struct usb_interface_descriptor data_iface[] = {
	{
		.bLength = USB_DT_INTERFACE_SIZE,
		.bDescriptorType = USB_DT_INTERFACE,
		.bInterfaceNumber = 1,
		.bAlternateSetting = 0,
		.bNumEndpoints = 2,
		.bInterfaceClass = USB_CLASS_DATA,
		.bInterfaceSubClass = 0,
		.bInterfaceProtocol = 0,
		.iInterface = 0,
		.endpoint = data_endp,
	}
};

const struct usb_interface ifaces[] = {
	{
		.num_altsetting = 1,
		.altsetting = data_iface,
	}
};

const struct usb_config_descriptor config = {
	.bLength = USB_DT_CONFIGURATION_SIZE,
	.bDescriptorType = USB_DT_CONFIGURATION,
	.wTotalLength = 0,
	.bNumInterfaces = 1,
	.bConfigurationValue = 1,
	.iConfiguration = 0,
	.bmAttributes = 0x80,
	.bMaxPower = 0x32,
	.interface = ifaces,
};

static const char * usb_strings[] = {
	"usbcdc.c driver",
	"usbcdc module",
	"usbcdcdemo",
};

uint8_t usbd_control_buffer[128];

/**** CALLBACKS *****************************************/

enum usbd_request_return_codes
cdcacm_control_request(
	usbd_device *usbd_dev,
	struct usb_setup_data *req,
	uint8_t **buf, uint16_t *len,
	void (**complete)(
		usbd_device *usbd_dev,
		struct usb_setup_data *req))
{
	(void)usbd_dev;
	(void)buf;
	(void)complete;

	switch (req->bRequest)
	{
	case USB_CDC_REQ_SET_LINE_CODING:
		return *len >= sizeof *req
			? USBD_REQ_HANDLED
			: USBD_REQ_NOTSUPP;
	default:
		return USBD_REQ_NOTSUPP;
	}
}

#define QUEUE_LEN 128
#define ITEM_SIZ  1
#define MIN(x,y) ((x)<(y))?(x):(y)

/* bundled queue data for static allocation */
struct usb_queue
{
	QueueHandle_t h;
	StaticQueue_t q;
	uint8_t       mem[QUEUE_LEN * ITEM_SIZ];
} usb_txq, usb_rxq;

volatile bool initialized = false;

void cdcacm_data_rx_cb(
	usbd_device *usbd_dev,
	uint8_t ep)
{
	(void)ep;

	unsigned i, len,
		 rx_avail = uxQueueSpacesAvailable(usb_rxq.h);
	char buf[64];

	if (rx_avail <= 0)
		return;
	len = usbd_ep_read_packet(usbd_dev, 0x01, buf, MIN(sizeof buf, rx_avail));

	// enqueue received data
	for (i = 0; i < len; i++)
		xQueueSend(usb_rxq.h, &buf[i], 0);
}

void cdcacm_set_config(usbd_device *usbd_dev, uint16_t unused)
{
	(void)unused;

	usbd_ep_setup(usbd_dev, 0x01, USB_ENDPOINT_ATTR_BULK, 64, cdcacm_data_rx_cb);
	usbd_ep_setup(usbd_dev, 0x82, USB_ENDPOINT_ATTR_BULK, 64, NULL);

	usbd_register_control_callback(
		usbd_dev,
		USB_REQ_TYPE_CLASS | USB_REQ_TYPE_INTERFACE,
		USB_REQ_TYPE_TYPE  | USB_REQ_TYPE_RECIPIENT,
		cdcacm_control_request);

	initialized = true;
}

int usb_getc(void)
{
	char ch;
	return xQueueReceive(usb_rxq.h, &ch, portMAX_DELAY) == pdPASS
		? ch : -1;
}

void usb_putc(char ch)
{
	static const char cr = '\r';
	while (!initialized)
		taskYIELD();
	if (ch == '\n') // cooked mode
		xQueueSend(usb_txq.h, &cr, portMAX_DELAY);
	xQueueSend(usb_txq.h, &ch, portMAX_DELAY);
}

/**** TASKS *********************************************/

StaticTask_t appTaskBuf;
StackType_t  appTaskStack[configMINIMAL_STACK_SIZE];

void app_task(void *arg)
{
	(void)arg;

	int c;
	while ((c = usb_getc()))
		if (c > 0)
			usb_putc(toupper(c));
}

StaticTask_t usbTaskBuf;
StackType_t  usbTaskStack[configMINIMAL_STACK_SIZE];

/* Poll device to keep it ticking, and transmit from queue.
 * The rx, by contrast, will happen in a callback */
void usb_task(void *arg)
{
	usbd_device *udev = arg;
	char txbuf[32];
	unsigned txlen = 0;

	while (1)
	{
		usbd_poll(udev);
		if (initialized)
		{
			while (txlen < sizeof txbuf &&
					pdPASS == xQueueReceive(usb_txq.h, &txbuf[txlen], 0))
				txlen++;
			if (txlen > 0)
			{
				if (usbd_ep_write_packet(udev, 0x82, txbuf, txlen))
					txlen = 0;
			}
			else
				taskYIELD();
		}
	}
}

void usb_start(void)
{
	usbd_device *udev = NULL;

	usb_txq.h = xQueueCreateStatic(QUEUE_LEN, ITEM_SIZ, usb_txq.mem, &usb_txq.q);
	usb_rxq.h = xQueueCreateStatic(QUEUE_LEN, ITEM_SIZ, usb_rxq.mem, &usb_rxq.q);

	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_OTGFS);

	udev = usbd_init(
		&otgfs_usb_driver,
		&dev, &config, usb_strings, 3,
		usbd_control_buffer,
		sizeof usbd_control_buffer);

	usbd_register_set_config_callback(udev, cdcacm_set_config);
	xTaskCreateStatic(
		usb_task, "USBTX", configMINIMAL_STACK_SIZE, NULL,
		configMAX_PRIORITIES-1, usbTaskStack, &usbTaskBuf);
}

/**** ENTRYPOINT ****************************************/

int main(void)
{
	rcc_clock_setup_pll(&rcc_hsi_configs[RCC_CLOCK_3V3_96MHZ]);
	usb_start();

	xTaskCreateStatic(
		app_task, "app", configMINIMAL_STACK_SIZE, NULL,
		configMAX_PRIORITIES-1, appTaskStack, &appTaskBuf);

	vTaskStartScheduler(); // FreeRTOS, take the wheel!
	configASSERT(0); // shouldn't get here
}
