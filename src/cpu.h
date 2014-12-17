/* cpu.h
 * 
 *  This file is part of cc8.
 *  
 *  Copyright © 2014 molko <molkoback@gmail.com>
 *  This work is free. You can redistribute it and/or modify it under the
 *  terms of the Do What The Fuck You Want To Public License, Version 2,
 *  as published by Sam Hocevar. See the COPYING file for more details.
 *
 */

#ifndef CPU_H
#define CPU_H

#include <stdint.h>

/* See http://en.wikipedia.org/wiki/CHIP-8 */
typedef struct{
	/* Program counter. */
	uint16_t pc;
	
	/* Memory. The most programs begin at memory location 512 because
	 * the CHIP-8 interpreter itself occupies the first 512 bytes on
	 * the original systems. */
	uint8_t mem[4096];
	
	/* "v" registers. */
	uint8_t v[16];
	
	/* "i" register. */
	uint16_t i;
	
	/* Delay and sound timers. Both cound down from 60 until they reach
	 * 0. */
	uint8_t dt, st;
	
	/* Stack. Stores subroutine return pointers. */
	uint16_t stack[16];
	
	/* Stack pointer. So we can keep up with stack. */
	uint8_t sp;
	
	/* 64x32 pixel monochrome display. */
	uint8_t display[32][64];
	
	/* Set to 1 when we want to quit. */
	int quit;
}CPU;

/* Resets machine. */
void cpu_reset(CPU *cpu);

/* One emulator cycle. */
void cpu_cycle(CPU *cpu);

#endif
