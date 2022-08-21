#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

int main(void)
{
	rcc_periph_clock_enable(RCC_GPIOC);
	gpio_mode_setup(
		GPIOC,
		GPIO_MODE_OUTPUT,
		GPIO_PUPD_NONE,
		GPIO13);

	for (;;)
	{
		int i;

		gpio_clear(GPIOC,GPIO13);	/* LED on */
		for (i = 0; i < 1500000; i++)	/* Wait a bit. */
			__asm__("nop");

		gpio_set(GPIOC,GPIO13);		/* LED off */
		for (i = 0; i < 500000; i++)	/* Wait a bit. */
			__asm__("nop");
	}

	return 0;
}
