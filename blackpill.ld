/* WeAct "blackpill" STM32F411CEU6 */

MEMORY
{
	rom (rx)  : ORIGIN = 0x08000000, LENGTH = 512K
	ram (rwx) : ORIGIN = 0x20000000, LENGTH = 128K
}

/* for heap_useNewlib_ST.c */
PROVIDE(__HeapBase  = ORIGIN(ram)              );
PROVIDE(__HeapLimit = ORIGIN(ram) + LENGTH(ram));
PROVIDE(__heap_size =               LENGTH(ram));
PROVIDE(HEAP_SIZE   =               LENGTH(ram));

/* provided by libopencm3 */
INCLUDE cortex-m-generic.ld
