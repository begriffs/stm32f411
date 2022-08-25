#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#ifndef NDEBUG
	// turn assertion failures into uninterrupted loops that we can pause
	// and look at in the debugger
	#define configASSERT( x ) if( ( x ) == 0 ) { taskDISABLE_INTERRUPTS(); for(;;); }

	// sounds nice
	#define configCHECK_FOR_STACK_OVERFLOW  1
#endif

// we use this to delay blinks
#define INCLUDE_vTaskDelay              1

// IDK, but copied from most similar demo, CORTEX_M4F_STM32F407ZG-SK.
// Many other demos use 128, so should be safe.
#define configMINIMAL_STACK_SIZE        130

// the device has 128k of SRAM, and I don't know how much the kernel
// uses, so let's just say 64k for now
#define configTOTAL_HEAP_SIZE           (1u<<6)

#define configUSE_PREEMPTION            1
#define configUSE_IDLE_HOOK             0
#define configUSE_TICK_HOOK             0

// up to us to clock the chip to match using libopencm3
#define configCPU_CLOCK_HZ              1e8

// Up to us, not mandated by the device. Sets the smallest granularity
// of time that pdMS_TO_TICKS can specify. Need at least 1khz to get
// single-ms control
#define configTICK_RATE_HZ              1e3

// we're not on a restrictive 8- or 16-bit chip
#define configUSE_16_BIT_TICKS          0

// Tasks can be assigned a priority from zero, which is the lowest priority, to
// (configMAX_PRIORITIES – 1). Better use 5 because of the Cortex M priorities
// mentioned at the end of this file.
#define configMAX_PRIORITIES            5

// For user-defined descriptive task names. Pulled 16 from an example, and
// seems fine.
#define configMAX_TASK_NAME_LEN         16

/******* Cortex-M specific magic ********/

#ifdef __NVIC_PRIO_BITS
    /* __BVIC_PRIO_BITS will be specified when CMSIS is being used. */
    #define configPRIO_BITS             __NVIC_PRIO_BITS
#else
    #define configPRIO_BITS             4        /* 15 priority levels */
#endif

/* The lowest interrupt priority that can be used in a call to a "set priority"
 * function. */
#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY         0xf

/* The highest interrupt priority that can be used by any interrupt service
 * routine that makes calls to interrupt safe FreeRTOS API functions.  DO NOT CALL
 * INTERRUPT SAFE FREERTOS API FUNCTIONS FROM ANY INTERRUPT THAT HAS A HIGHER
 * PRIORITY THAN THIS! (higher priorities are lower numeric values. */
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY    5

/* Interrupt priorities used by the kernel port layer itself.  These are generic
 * to all Cortex-M ports, and do not rely on any particular library functions. */
#define configKERNEL_INTERRUPT_PRIORITY         ( configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )
/* !!!! configMAX_SYSCALL_INTERRUPT_PRIORITY must not be set to zero !!!!
 * See http://www.FreeRTOS.org/RTOS-Cortex-M3-M4.html. */
#define configMAX_SYSCALL_INTERRUPT_PRIORITY    ( configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )

/* Definitions that map the FreeRTOS port interrupt handlers to their CMSIS
 * standard names. */
#define vPortSVCHandler SVC_Handler
#define xPortPendSVHandler PendSV_Handler
#define xPortSysTickHandler SysTick_Handler


#endif
