.POSIX:

ABI = arm-none-eabi
CC = $(ABI)-gcc
AR = $(ABI)-ar
OBJCOPY = $(ABI)-objcopy

OPENCM3 = /usr/local/libopencm3-1.0.0
RTOS = /usr/local/freertos-202012.04
RTOS_PORT = $(RTOS)/portable/GCC/ARM_CM4F

INC_RTOS = -I./rtos -I$(RTOS)/include -I$(RTOS_PORT)
INC_OPENCM3 = -I$(OPENCM3)/include

# copy floating point flags from stm32/f4 in libopencm3 makefile
FP_FLAGS = -mfloat-abi=hard -mfpu=fpv4-sp-d16

STMF411 = -mcpu=cortex-m4 -mthumb $(FP_FLAGS) -DSTM32F4

CFLAGS = -std=c99 -pedantic -Wall -Wextra -Wshadow \
         -g -Os $(STMF411) $(INC_RTOS) $(INC_OPENCM3)

LDFLAGS = -Lrtos -L$(OPENCM3)/lib -L/usr/local/arm-none-eabi/lib/fpu \
		  -nostartfiles -nostdlib \
		  -Tblackpill.ld
LDLIBS = -lopencm3_stm32f4 -lfreertos -lg

blink.axf blink.bin : blink.c rtos/libfreertos.a blackpill.ld
	$(CC) $(CFLAGS) blink.c $(LDFLAGS) -o blink.axf $(LDLIBS)
	$(OBJCOPY) -O binary blink.axf blink.bin

miniblink.axf miniblink.bin : miniblink.c blackpill.ld
	$(CC) $(CFLAGS) miniblink.c $(LDFLAGS) -o miniblink.axf $(LDLIBS)
	$(OBJCOPY) -O binary miniblink.axf miniblink.bin

clean :
	rm -f *.axf *.bin
	rm -f rtos/*.[ao]

#### Build FreeRTOS customized for our app config and target MCU

## TODO: is there a non-GNU way to simplify the repetition?

rtos/libfreertos.a : rtos/tasks.o rtos/port.o rtos/list.o rtos/heap.o
	$(AR) r $@ $?
rtos/tasks.o : $(RTOS)/tasks.c rtos/FreeRTOSConfig.h
	$(CC) $(CFLAGS) -o $@ -c $(RTOS)/tasks.c
rtos/list.o : $(RTOS)/list.c rtos/FreeRTOSConfig.h
	$(CC) $(CFLAGS) -o $@ -c $(RTOS)/list.c
rtos/port.o : $(RTOS_PORT)/port.c rtos/FreeRTOSConfig.h
	$(CC) $(CFLAGS) -o $@ -c $(RTOS_PORT)/port.c
rtos/heap.o : $(RTOS)/portable/MemMang/heap_4.c rtos/FreeRTOSConfig.h
	$(CC) $(CFLAGS) -o $@ -c $(RTOS)/portable/MemMang/heap_4.c
