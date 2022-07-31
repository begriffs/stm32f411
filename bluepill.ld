MEMORY
{
	rom (rx)  : ORIGIN = 0x08000000, LENGTH = 64K
	ram (rwx) : ORIGIN = 0x20000000, LENGTH = 10K
}

/* provided by libopencm3 */
INCLUDE cortex-m-generic.ld
