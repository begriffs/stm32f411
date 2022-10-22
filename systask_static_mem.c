#ifndef SYSTASK_STATIC_MEM_H
#define SYSTASK_STATIC_MEM_H

#include <FreeRTOS.h>

#if configSUPPORT_STATIC_ALLOCATION == 1

/* from https://www.freertos.org/a00110.html#configSUPPORT_STATIC_ALLOCATION */

/* configSUPPORT_STATIC_ALLOCATION is set to 1, so the application must provide
 * an implementation of vApplicationGetIdleTaskMemory() to provide the memory
 * that is used by the Idle task. */
void vApplicationGetIdleTaskMemory(
	StaticTask_t **ppxIdleTaskTCBBuffer,
	StackType_t **ppxIdleTaskStackBuffer,
	uint32_t *pulIdleTaskStackSize)
{
	/* the storage */
	static StaticTask_t xIdleTaskTCB;
	static StackType_t uxIdleTaskStack[configMINIMAL_STACK_SIZE];

	*ppxIdleTaskTCBBuffer = &xIdleTaskTCB;
	*ppxIdleTaskStackBuffer = uxIdleTaskStack;
	*pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

#if configUSE_TIMERS == 1

/* configUSE_TIMERS is set to 1, so the application must provide an
 * implementation of vApplicationGetTimerTaskMemory() to provide the memory
 * that is used by the Timer service task. */
void vApplicationGetTimerTaskMemory(
	StaticTask_t **ppxTimerTaskTCBBuffer,
	StackType_t **ppxTimerTaskStackBuffer,
	uint32_t *pulTimerTaskStackSize)
{
	/* the storage */
	static StaticTask_t xTimerTaskTCB;
	static StackType_t uxTimerTaskStack[configTIMER_TASK_STACK_DEPTH];

	*ppxTimerTaskTCBBuffer = &xTimerTaskTCB;
	*ppxTimerTaskStackBuffer = uxTimerTaskStack;
	*pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}

#endif // configUSE_TIMERS
#endif // configSUPPORT_STATIC_ALLOCATION
#endif // SYSTASK_STATIC_MEM_H
