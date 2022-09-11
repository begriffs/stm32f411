/* WeAct "blackpill" STM32F411CEU6 */

MEMORY
{
	rom (rx)  : ORIGIN = 0x08000000, LENGTH = 512K
	ram (rwx) : ORIGIN = 0x20000000, LENGTH = 128K
}

/* for heap_useNewlib_ST.c */
PROVIDE(__HeapBase  = ORIGIN(rom)              );
PROVIDE(__HeapLimit = ORIGIN(rom) + LENGTH(rom));
PROVIDE(__heap_size =               LENGTH(rom));
PROVIDE(HEAP_SIZE   =               LENGTH(rom));

/* provided by libopencm3 */
INCLUDE cortex-m-generic.ld
