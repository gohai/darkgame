//
// Darkgame Firmware
//
// based on LUFA Library, Copyright (C) Dean Camera, 2012
//

#include "Darkgame.h"

// Circular buffer to hold data from the host
static RingBuffer_t USBIn_Buffer;

// Underlying data buffer for USBIn_Buffer, where the stored bytes are located
static uint8_t      USBIn_Buffer_Data[128];

// Circular buffer to hold data to be send to the host
static RingBuffer_t USBOut_Buffer;

// Underlying data buffer for USBOut_Buffer, where the stored bytes are located
static uint8_t      USBOut_Buffer_Data[128];

static uint8_t		port_mapping[] = { 4, 5, 6, 7, 3, 2, 1, 0 };

// LUFA CDC Class driver interface configuration and state information
USB_ClassInfo_CDC_Device_t VirtualSerial_CDC_Interface =
	{
		.Config =
			{
				.ControlInterfaceNumber         = 0,
				.DataINEndpoint                 =
					{
						.Address                = CDC_TX_EPADDR,
						.Size                   = CDC_TXRX_EPSIZE,
						.Banks                  = 1,
					},
				.DataOUTEndpoint                =
					{
						.Address                = CDC_RX_EPADDR,
						.Size                   = CDC_TXRX_EPSIZE,
						.Banks                  = 1,
					},
				.NotificationEndpoint           =
					{
						.Address                = CDC_NOTIFICATION_EPADDR,
						.Size                   = CDC_NOTIFICATION_EPSIZE,
						.Banks                  = 1,
					},
			},
	};

int main(void)
{
	SetupHardware();

	RingBuffer_InitBuffer(&USBIn_Buffer, USBIn_Buffer_Data, sizeof(USBIn_Buffer_Data));
	RingBuffer_InitBuffer(&USBOut_Buffer, USBOut_Buffer_Data, sizeof(USBOut_Buffer_Data));

	GlobalInterruptEnable();

	for (;;)
	{
		// Only try to read in bytes from the CDC interface if the transmit buffer is not full
		if (!(RingBuffer_IsFull(&USBIn_Buffer)))
		{
			int16_t ReceivedByte = CDC_Device_ReceiveByte(&VirtualSerial_CDC_Interface);

			// Read bytes from the USB OUT endpoint into the USART transmit buffer
			if (!(ReceivedByte < 0)) {
				// for each bit
				for (uint8_t bit=0; bit < 8; bit++) {
					// get the corresponding port
					if (port_mapping[bit] < 4) {
						// enable/disable the port depending on the bit value of the received byte
						// the +4 offset is here because we're only manipulating the higher 4 bits of the port
						PORTB = (PORTB & ~(1 << (port_mapping[bit]+4))) | (((ReceivedByte >> bit) & 1) << (port_mapping[bit]+4));
					} else {
						PORTC = (PORTC & ~(1 << port_mapping[bit])) | (((ReceivedByte >> bit) & 1) << port_mapping[bit]);
					}
				}
				RingBuffer_Insert(&USBIn_Buffer, ReceivedByte);
			}
		}

		// Check if the UART receive buffer flush timer has expired or the buffer is nearly full
		uint16_t BufferCount = RingBuffer_GetCount(&USBOut_Buffer);
		if (BufferCount)
		{
			Endpoint_SelectEndpoint(VirtualSerial_CDC_Interface.Config.DataINEndpoint.Address);

			// Check if a packet is already enqueued to the host - if so, we shouldn't try to send more data
			// until it completes as there is a chance nothing is listening and a lengthy timeout could occur
			if (Endpoint_IsINReady())
			{
				// Never send more than one bank size less one byte to the host at a time, so that we don't block
				// while a Zero Length Packet (ZLP) to terminate the transfer is sent if the host isn't listening
				uint8_t BytesToSend = MIN(BufferCount, (CDC_TXRX_EPSIZE - 1));

				// Read bytes from the USART receive buffer into the USB IN endpoint
				while (BytesToSend--)
				{
					// Try to send the next byte of data to the host, abort if there is an error without dequeuing
					if (CDC_Device_SendByte(&VirtualSerial_CDC_Interface,
											RingBuffer_Peek(&USBOut_Buffer)) != ENDPOINT_READYWAIT_NoError)
					{
						break;
					}

					// Dequeue the already sent byte from the buffer now we have confirmed that no transmission error occurred
					RingBuffer_Remove(&USBOut_Buffer);
				}
			}
		}

		// Load the next byte from the USART transmit buffer into the USART */
		if (!(RingBuffer_IsEmpty(&USBIn_Buffer))) {
			// TODO
			RingBuffer_Remove(&USBIn_Buffer);
		}

		CDC_Device_USBTask(&VirtualSerial_CDC_Interface);
		USB_USBTask();
	}
}

void SetupHardware(void)
{
	// Disable watchdog if enabled by bootloader/fuses
	MCUSR &= ~(1 << WDRF);
	wdt_disable();

	// Disable clock division
	clock_prescale_set(clock_div_1);

	// Hardware Initialization
	USB_Init();

	DDRB |= (1<<7)|(1<<6)|(1<<5)|(1<<4);
	DDRC |= (1<<7)|(1<<6)|(1<<5)|(1<<4);
}

// Event handler for the library USB Connection event
void EVENT_USB_Device_Connect(void)
{
	// TODO
}

// Event handler for the library USB Disconnection event
void EVENT_USB_Device_Disconnect(void)
{
	// TODO
}

// Event handler for the library USB Configuration Changed event
void EVENT_USB_Device_ConfigurationChanged(void)
{
	CDC_Device_ConfigureEndpoints(&VirtualSerial_CDC_Interface);
}

// Event handler for the library USB Control Request reception event
void EVENT_USB_Device_ControlRequest(void)
{
	CDC_Device_ProcessControlRequest(&VirtualSerial_CDC_Interface);
}
