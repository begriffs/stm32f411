#include <FreeRTOS.h>
#include <task.h>

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

void vApplicationStackOverflowHook(
	TaskHandle_t task, char *name)
{
	(void)task;
	(void)name;
	while (1)
		;
}

static void blink(void *args)
{
	(void)args;
	while (1)
	{
		gpio_toggle(GPIOC, GPIO13);
		vTaskDelay(pdMS_TO_TICKS(100));
	}
}

int main(void)
{
	rcc_periph_clock_enable(RCC_GPIOC);
	gpio_mode_setup(
		GPIOC,
		GPIO_MODE_OUTPUT,
		GPIO_PUPD_NONE,
		GPIO13);

	xTaskCreate(
		blink, "LED", 100, NULL, configMAX_PRIORITIES-1, NULL);
	vTaskStartScheduler();

	while (1)
		;
}
