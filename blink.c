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
		vTaskDelay(pdMS_TO_TICKS(500));
	}
}

int main(void)
{
	rcc_clock_setup_pll(&rcc_hse_configs[RCC_CLOCK_HSE8_72MHZ]);
	rcc_periph_clock_enable(RCC_GPIOC);
	gpio_set_mode(
		GPIOC,
		GPIO_MODE_OUTPUT_2_MHZ,
		GPIO_CNF_OUTPUT_PUSHPULL,
		GPIO13);

	xTaskCreate(
		blink, "LED", 100, NULL, configMAX_PRIORITIES-1, NULL);
	vTaskStartScheduler();

	while (1)
		;
	return 0;
}
