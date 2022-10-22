.POSIX:

ABI = arm-none-eabi
CC = $(ABI)-gcc
AR = $(ABI)-ar
OBJCOPY = $(ABI)-objcopy

OPENCM3 = /usr/local/libopencm3-1.0.0
RTOS = /usr/local/freertos-202012.04

INC_RTOS = -I$(RTOS)/include -I$(RTOS)/portable/GCC/ARM_CM4F
INC_OPENCM3 = -DSTM32F4 -I$(OPENCM3)/include

# copied x-compiler flags from libopencm3's stm32/f4 makefile
STMF411 = -mcpu=cortex-m4 -mthumb \
          -mfloat-abi=hard -mfpu=fpv4-sp-d16

CFLAGS = -std=c99 -pedantic -Wall -Wextra -Wshadow -fno-builtin-printf \
         -g -Os $(STMF411) -I. $(INC_RTOS) $(INC_OPENCM3)

LDFLAGS = -nostartfiles -nostdlib \
          -L. -L$(OPENCM3)/lib -L/usr/local/$(ABI)/lib/fpu \
		  -Wl,--print-memory-usage \
          -Tblackpill.ld

LDLIBS = -lopencm3_stm32f4 -lfreertos -lg

## Programs ##########################################

all : blink.axf uart.axf

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

######################################################

.PHONY:
clean :
	rm -f *.axf *.bin *.[ao]
