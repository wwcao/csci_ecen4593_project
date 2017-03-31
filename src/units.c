#include "units.h"
#include <stdio.h>


void init_units() {
	register_file[$sp] = memory[0];
	register_file[$fp] = memory[1];
	PC = memory[5];
	printf("Initializing units\n------\n$sp[0x%08x]\t$fp[0x%08x]\nPC[0x%08x]\n------\n",
			register_file[$sp], register_file[$fp], PC);
}
