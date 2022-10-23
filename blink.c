#include <FreeRTOS.h>
#include <task.h>

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

void app(void *args)
{
	(void)args;
	while (1)
	{
		gpio_toggle(GPIOC, GPIO13);
		vTaskDelay(pdMS_TO_TICKS(500));
	}
}

StaticTask_t appTaskBuf;
StackType_t  appTaskStack[configMINIMAL_STACK_SIZE];

int main(void)
{
	rcc_clock_setup_pll(&rcc_hsi_configs[RCC_CLOCK_3V3_96MHZ]);

	rcc_periph_clock_enable(RCC_GPIOC);
	gpio_mode_setup(
		GPIOC,
		GPIO_MODE_OUTPUT,
		GPIO_PUPD_NONE,
		GPIO13);

	xTaskCreateStatic(
		app, "app", configMINIMAL_STACK_SIZE, NULL,
		configMAX_PRIORITIES-1, appTaskStack, &appTaskBuf);

	vTaskStartScheduler(); // FreeRTOS, take the wheel!
	configASSERT(0); // shouldn't get here
}
