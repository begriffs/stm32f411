.POSIX:

ABI = arm-none-eabi
CC = $(ABI)-gcc
AR = $(ABI)-ar
OBJCOPY = $(ABI)-objcopy

OPENCM3 = /usr/local/libopencm3-1.0.0
RTOS = /usr/local/freertos-202012.04
DERP = /home/j/dev/libderp

INC_RTOS = -I$(RTOS)/include -I$(RTOS)/portable/GCC/ARM_CM4F
INC_OPENCM3 = -DSTM32F4 -I$(OPENCM3)/include
INC_DERP = -I$(DERP)/include

# copied x-compiler flags from libopencm3's stm32/f4 makefile
STMF411 = -mcpu=cortex-m4 -mthumb \
          -mfloat-abi=hard -mfpu=fpv4-sp-d16

CFLAGS = -Wall -Wextra -Wshadow \
         -g -Os $(STMF411) -I. $(INC_RTOS) $(INC_OPENCM3) \
		 $(INC_DERP)

LDFLAGS = -nostartfiles -nostdlib \
          -L. -L$(OPENCM3)/lib -L/usr/local/$(ABI)/lib/fpu \
		  -L$(DERP)/build/release \
          -Tblackpill.ld

LDLIBS = -lopencm3_stm32f4 -lderp -lfreertos -lg -lgcc

## Programs ##########################################

blink.axf blink.bin : blink.c libfreertos.a blackpill.ld
	$(CC) $(CFLAGS) blink.c $(LDFLAGS) -o blink.axf $(LDLIBS)
	$(OBJCOPY) -O binary blink.axf blink.bin

## FreeRTOS ##########################################

RTOS_SRCS = croutine.c event_groups.c list.c \
            queue.c stream_buffer.c tasks.c \
            timers.c port.c heap_useNewlib_ST.c
RTOS_OBJS = $(RTOS_SRCS:.c=.o)

VPATH = $(RTOS):$(RTOS)/portable/GCC/ARM_CM4F:$(RTOS)/portable/MemMang

$(RTOS_OBJS) : FreeRTOSConfig.h
libfreertos.a : $(RTOS_OBJS)
	$(AR) r $@ $?

######################################################

.PHONY:
clean :
	rm -f *.axf *.bin *.[ao]
