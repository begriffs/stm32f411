.POSIX:

ABI = arm-none-eabi
CC = $(ABI)-gcc
AR = $(ABI)-ar
OBJCOPY = $(ABI)-objcopy

OPENCM3 = /usr/local/libopencm3-1.0.0
RTOS = /usr/local/freertos-202012.04
RTOS_POSIX = /usr/local/freertos-posix/FreeRTOS-Plus-POSIX/

INC_RTOS = -I$(RTOS)/include -I$(RTOS)/portable/GCC/ARM_CM4F
INC_RTOS_POSIX = -I$(RTOS_POSIX)/include -I$(RTOS_POSIX)/include/portable -I/usr/local/freertos-posix/include -I/usr/local/freertos-posix/include/private
INC_OPENCM3 = -DSTM32F4 -I$(OPENCM3)/include

# copied x-compiler flags from libopencm3's stm32/f4 makefile
STMF411 = -mcpu=cortex-m4 -mthumb \
          -mfloat-abi=hard -mfpu=fpv4-sp-d16

CFLAGS = -std=c99 -pedantic -Wall -Wextra -Wshadow -fno-builtin-printf \
         -g -Os $(STMF411) -I. $(INC_RTOS) \
		 $(INC_RTOS_POSIX) $(INC_OPENCM3)

LDFLAGS = -nostartfiles -nostdlib \
          -L. -L$(OPENCM3)/lib -L/usr/local/$(ABI)/lib/fpu \
		  -Wl,--print-memory-usage \
          -Tblackpill.ld

LDLIBS = -lopencm3_stm32f4 -lfreertos_posix -lfreertos -lg

## Programs ##########################################

blink.axf blink.bin : blink.c libfreertos.a blackpill.ld
	$(CC) $(CFLAGS) blink.c $(LDFLAGS) -o blink.axf $(LDLIBS)
	$(OBJCOPY) -O binary blink.axf blink.bin

uart.axf uart.bin : uart.c libfreertos.a blackpill.ld
	$(CC) $(CFLAGS) uart.c $(LDFLAGS) -o uart.axf $(LDLIBS)
	$(OBJCOPY) -O binary uart.axf uart.bin

## FreeRTOS ##########################################

RTOS_SRCS = croutine.c event_groups.c list.c \
            queue.c stream_buffer.c tasks.c \
            timers.c port.c systask_static_mem.c
RTOS_OBJS = $(RTOS_SRCS:.c=.o)

VPATH = $(RTOS):$(RTOS)/portable/GCC/ARM_CM4F

$(RTOS_OBJS) : FreeRTOSConfig.h
libfreertos.a : $(RTOS_OBJS)
	$(AR) r $@ $?

## FreeRTOS POSIX ####################################

RTOS_POSIX_SRCS = FreeRTOS_POSIX_clock.c \
				  FreeRTOS_POSIX_mqueue.c \
				  FreeRTOS_POSIX_pthread.c \
				  FreeRTOS_POSIX_pthread_barrier.c \
				  FreeRTOS_POSIX_pthread_cond.c \
				  FreeRTOS_POSIX_pthread_mutex.c \
				  FreeRTOS_POSIX_sched.c \
				  FreeRTOS_POSIX_semaphore.c \
				  FreeRTOS_POSIX_timer.c \
				  FreeRTOS_POSIX_unistd.c \
				  FreeRTOS_POSIX_utils.c

RTOS_POSIX_OBJS = $(RTOS_POSIX_SRCS:.c=.o)

VPATH = $(RTOS_POSIX)/source

$(RTOS_OBJS) : FreeRTOSConfig.h
libfreertos_posix.a : $(RTOS_POSIX_OBJS)
	$(AR) r $@ $?

######################################################

.PHONY:
clean :
	rm -f *.axf *.bin *.[ao]
