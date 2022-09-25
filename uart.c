#include <FreeRTOS.h>
#include <task.h>

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>

void stream_chars(void *args)
{
	(void)args;

	int j = 0, c = 0;

	while (1)
	{
		gpio_toggle(GPIOC, GPIO13);	/* LED on/off */
		usart_send_blocking(USART1, c + '0'); /* USART1: Send byte. */
		c = (c+1)%10;
		if ((j++ % 80) == 0)
		{
			/* Newline after line full. */
			usart_send_blocking(USART1, '\r');
			usart_send_blocking(USART1, '\n');
		}
		vTaskDelay(pdMS_TO_TICKS(500));
	}
}

void clock_setup(void)
{
	// main system clock, go faster than necessary
	rcc_clock_setup_pll(&rcc_hsi_configs[RCC_CLOCK_3V3_96MHZ]);

	// GPIO clock for USART1
	rcc_periph_clock_enable(RCC_GPIOA);
	// and LED
	rcc_periph_clock_enable(RCC_GPIOC);

	// then for USART1 directly for some reason
	rcc_periph_clock_enable(RCC_USART1);
}

void usart_setup(void)
{
	// serial params
	usart_set_baudrate(USART1, 38400);
	usart_set_databits(USART1, 8);
	usart_set_stopbits(USART1, USART_STOPBITS_1);
	usart_set_mode(USART1, USART_MODE_TX);
	usart_set_parity(USART1, USART_PARITY_NONE);
	usart_set_flow_control(USART1, USART_FLOWCONTROL_NONE);

	// ready to go
	usart_enable(USART1);
}

/*
 * USART1_CK   29  PA8
 * USART1_TX   30  PA9
 * USART1_RX   31  PA10
 * USART1_CTS  32  PA11
 * USART1_RTS  33  PA12
 */

void gpio_setup(void)
{
	// Setup GPIO pin for LED
	gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO13);

	// Setup GPIO pin for USART1 transmit
	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO9);

	// Setup USART1 TX pin as alternate function
	gpio_set_af(GPIOA, GPIO_AF7, GPIO9);
}

int main(void)
{
	clock_setup();
	gpio_setup();
	usart_setup();

	xTaskCreate(
		stream_chars, "UART", 100, NULL,
		configMAX_PRIORITIES-1, NULL);

	vTaskStartScheduler(); // FreeRTOS, take the wheel!
	configASSERT(0); // shouldn't get here
}
