/* WeAct "blackpill" STM32F411CEU6 */

MEMORY
{
	rom (rx)  : ORIGIN = 0x08000000, LENGTH = 512K
	ram (rwx) : ORIGIN = 0x20000000, LENGTH = 128K
}

/* provided by libopencm3 */
INCLUDE cortex-m-generic.ld
