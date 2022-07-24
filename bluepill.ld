MEMORY
{
	rom (rx) : ORIGIN = 0x08000000, LENGTH = 64K
	ram (rwx) : ORIGIN = 0x20000000, LENGTH = 20K
}

INCLUDE /usr/local/libopencm3-1.0.0/lib/cortex-m-generic.ld
