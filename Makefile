.POSIX:

ABI = arm-none-eabi
CC = $(ABI)-gcc
AR = $(ABI)-ar
OBJCOPY = $(ABI)-objcopy

OPENCM3 = /usr/local/libopencm3-1.0.0
RTOS = /usr/local/freertos-202012.04

INC_RTOS = -I./rtos -I$(RTOS)/include -I$(RTOS)/portable/GCC/ARM_CM3
INC_OPENCM3 = -DSTM32F1 -I$(OPENCM3)/include

STMF103 = -mcpu=cortex-m3 -mthumb -mno-thumb-interwork -mfpu=vfp \
          -msoft-float -mfix-cortex-m3-ldrd

CFLAGS = -std=c99 -pedantic -Wall -Wextra -Wshadow \
         -g -Os $(STMF103) $(INC_RTOS) $(INC_OPENCM3)

LDFLAGS = -Lrtos -L$(OPENCM3)/lib -Tbluepill.ld -static -nostartfiles
LDLIBS = -lopencm3_stm32f1 -lfreertos

blink.axf blink.bin : blink.c rtos/libfreertos.a
	$(CC) $(CFLAGS) blink.c $(LDFLAGS) -o blink.axf $(LDLIBS)
	$(OBJCOPY) -O binary blink.axf blink.bin

#### Build FreeRTOS customized for our app config and target MCU

## TODO: is there a non-GNU way to simplify the repetition?

rtos/libfreertos.a : rtos/tasks.o rtos/port.o rtos/list.o rtos/heap.o
	$(AR) r $@ $?
rtos/tasks.o : $(RTOS)/tasks.c rtos/FreeRTOSConfig.h
	$(CC) $(CFLAGS) -o $@ -c $(RTOS)/tasks.c
rtos/list.o : $(RTOS)/list.c rtos/FreeRTOSConfig.h
	$(CC) $(CFLAGS) -o $@ -c $(RTOS)/list.c
rtos/port.o : $(RTOS)/portable/GCC/ARM_CM3/port.c rtos/FreeRTOSConfig.h
	$(CC) $(CFLAGS) -o $@ -c $(RTOS)/portable/GCC/ARM_CM3/port.c
rtos/heap.o : $(RTOS)/portable/MemMang/heap_4.c rtos/FreeRTOSConfig.h
	$(CC) $(CFLAGS) -o $@ -c $(RTOS)/portable/MemMang/heap_4.c
