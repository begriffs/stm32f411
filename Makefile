.POSIX:

ABI = arm-none-eabi
CC = $(ABI)-gcc
AR = $(ABI)-ar

OPENCM3 = /usr/local/libopencm3-1.0.0
RTOS = /usr/local/freertos-202012.04

STMF103 = -mcpu=cortex-m3 -mthumb -mno-thumb-interwork -mfpu=vfp \
          -msoft-float -mfix-cortex-m3-ldrd

CFLAGS = -std=c99 -pedantic -Wall -Wextra -Wshadow \
         -g $(STMF103) \
         -I./rtos -I$(RTOS)/include -I$(RTOS)/portable/GCC/ARM_CM3 \
         -I$(OPENCM3)/include

LDFLAGS = -L$(OPENCM3)/lib

rtos.a : rtos/tasks.o
	$(AR) r $@ $?

rtos/tasks.o : $(RTOS)/tasks.c
	$(CC) $(CFLAGS) -o $@ -c $?
