/* WeAct "blackpill" STM32F411CEU6 */

MEMORY
{
	rom (rx)  : ORIGIN = 0x08000000, LENGTH = 512K
	ram (rwx) : ORIGIN = 0x20000000, LENGTH = 128K
}

/* provided by libopencm3 */
INCLUDE cortex-m-generic.ld

/* heap_useNewlib_NXP.c expects these symbols:
 * __HeapBase
 * __HeapLimit
 * HEAP_SIZE
 *
 * We start __HeapBase immediately after other objects that
 * cortex-m-generic.ld placed at the start of ram
 */

. = ALIGN(8);
__HeapBase = .;

/* Extend the heap all the way to the end of ram, minus
 * space for our stack.
 *
 * We'll enable FreeRTOS's stack-overflow detection in case
 * this stack space proves too small
 */

PROVIDE(__HeapLimit = ORIGIN(ram) + LENGTH(ram) - 4K);
PROVIDE(HEAP_SIZE   = __HeapLimit - __HeapBase);
