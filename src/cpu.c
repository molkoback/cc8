/* cpu.c
 * 
 *  This file is part of cc8.
 *  
 *  Copyright © 2014 molko <molkoback@gmail.com>
 *  This work is free. You can redistribute it and/or modify it under the
 *  terms of the Do What The Fuck You Want To Public License, Version 2,
 *  as published by Sam Hocevar. See the COPYING file for more details.
 *
 */

#include "cpu.h"

#include "event.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* If we want to see debug information. */
#define DEBUG 0

/* Handle single operation code. */
static int handle_op(CPU *cpu, uint16_t op);

/* Reset machine. */
void cpu_reset(CPU *cpu)
{
	size_t i;
	srand(time(0));
	
	/* The most programs begin at memory location 512 becaus CHIP-8
	 * interpreter itself occupies the first 512 bytes on the original
	 * systems. */
	cpu->pc = 512;
	memset(cpu->mem, 0, 4096 * sizeof (uint8_t));
	
	memset(cpu->v, 0, 16 * sizeof (uint8_t));
	cpu->i = 0;
	
	cpu->dt = cpu->st = 0;
	
	memset(cpu->stack, 0, 16 * sizeof (uint16_t));
	cpu->sp = 0;
	
	for(i = 0; i < 32; i++){
		memset(cpu->display[i], 0, 64 * sizeof (uint8_t));
	}
	
	cpu->quit = 0;
}

/* One emulator cycle. */
void cpu_cycle(CPU *cpu)
{
	uint16_t op;
	#if DEBUG == 1
	int i;
	#endif
	
	/* Check if SQL_QUIT event has occured. */
	if((cpu->quit = event_checkQuit()) != 0){
		return;
	}
	
	/* One operation code is two bytes long. */
	op = (cpu->mem[cpu->pc] << 8) | cpu->mem[cpu->pc + 1];
	
	#if DEBUG == 1
	
	/* Debug shit */
	fprintf(stdout, "%04X: %04X", cpu->pc, op);
	
	if(handle_op(cpu, op) < 0){
		fprintf(stderr, " <- UNKNOWN OPCODE");
	}
	
	for(i = 0; i < 15; i++){
		if(cpu->v[i] != 0){
			fprintf(stdout, "  V[%X]: %02X", i, cpu->v[i]);
		}
		if(cpu->stack[i] != 0){
			fprintf(stdout, "  S[%X]: %04X", i, cpu->stack[i]);
		}
	}
	if(cpu->i != 0){
		fprintf(stdout, "  I: %04X", cpu->i);
	}
	fprintf(stdout, "\n");
	#else
	handle_op(cpu, op);
	#endif
	
	/* Increment by 2 because one operation is two bytes long. */
	cpu->pc += 2;
	if(cpu->pc > 4095){
		fprintf(stderr, "PC reached the maximum value\n");
		cpu->quit = 1;
	}
	
	/* Handle timers. */
	if(cpu->dt > 0){
		(cpu->dt)--;
	}
	if(cpu->st > 0){
		(cpu->st)--;
	}
}

/* Handle a single operation code. */
static int handle_op(CPU *cpu, uint16_t op)
{
	uint16_t i, j, k;
	uint16_t x, y, n, nn, nnn;
	
	/* -x-- */
	x = (op & 0x0F00) >> 8;
	
	/* --y- */
	y = (op & 0x00F0) >> 4;
	
	/* ---n */
	n = op & 0x000F;
	
	/* --nn */
	nn = op & 0x00FF;
	
	/* -nnn */
	nnn = op & 0x0FFF;
	
	switch(op & 0xF000){
		case 0x0000:
			switch(op){
				
				/* 00E0 */
				/* Clear the screen. */
				case 0x00E0:
					for(i = 0; i < 32; i++){
						for(j = 0; j < 64; j++){
							cpu->display[i][j] = 0;
						}
					}
					break;
				
				/* 00EE */
				/* Return from subroutine. */
				case 0x00EE:
					cpu->pc = cpu->stack[--(cpu->sp)];
					break;
				
				default:
					return -1;
			}
			break;
		
		/* 1nnn */
		/* Jump to address nnn. */
		case 0x1000:
			cpu->pc = nnn;
			
			/* The value of pc will increment after we return from this
			 * function so we decrease it. We need to do this in
			 * operations 1nnn, 2nnn and Bnnn. */
			cpu->pc -= 2;
			break;
		
		/* 2nnn */
		/* Call subroutine at nnn */
		case 0x2000:
			cpu->stack[(cpu->sp)++] = cpu->pc;
			cpu->pc = nnn;
			cpu->pc -= 2;
			break;
		
		/* 3xnn */
		/* Skip the next instruction if vx equals nn. */
		case 0x3000:
			if(cpu->v[x] == nn){
				cpu->pc += 2;
			}
			break;
		
		/* 4xnn */
		/* Skip the next instruction if vx doesn't equal nn. */
		case 0x4000:
			if(cpu->v[x] != nn){
				cpu->pc += 2;
			}
			break;
		
		case 0x5000:
			switch(op & 0x000F){
				/* 5xy0 */
				/* Skip the next instruction if vx equals vy. */
				case 0x0000:
					if(cpu->v[x] == cpu->v[y]){
						cpu->pc += 2;
					}
					break;
				
				default:
					return -1;
			}
			break;
		
		/* 6xnn */
		/* Set vx equal to nn. */
		case 0x6000:
			cpu->v[x] = nn;
			break;
		
		/* 7xnn */
		/* Set vx equal to vx plus nn. */
		case 0x7000:
			cpu->v[x] += nn;
			break;
		
		case 0x8000:
			switch(op & 0x000F){
				
				/* 8xy0 */
				/* Set vx equal to vy. */
				case 0x0000:
					cpu->v[x] = cpu->v[y];
					break;
				
				/* 8xy1 */
				/* Set vx equal to vx or vy. */
				case 0x0001:
					cpu->v[x] |= cpu->v[y];
					break;
				
				/* 8xy2 */
				/* Set vx equal to vx and vy. */
				case 0x0002:
					cpu->v[x] &= cpu->v[y];
					break;
				
				/* 8xy3 */
				/* Set vx equal to vx xor vy. */
				case 0x0003:
					cpu->v[x] ^= cpu->v[y];
					break;
				
				/* 8xy4 */
				/* Set vx equal to vx plus vy. Set vf to 1 if there's
				 * carry and to 0 if there isn't. */
				case 0x0004:
					cpu->v[0xF] = ((cpu->v[x] + cpu->v[y]) > 255);
					cpu->v[x] += cpu->v[y];
					break;
				
				/* 8xy5 */
				/* Set vx equal to vx minus vy. Set vf to 0 if there's
				 * borrow and to 1 if there isn't. */
				case 0x0005:
					cpu->v[0xF] = ((cpu->v[x] - cpu->v[y]) >= 0);
					cpu->v[x] -= cpu->v[y];
					break;
				
				/* 8xy6 */
				/* Set vf equal to the least significant bit of vx. Shift
				 * vx right by one. */
				case 0x0006:
					cpu->v[0xF] = cpu->v[x] & 1;
					cpu->v[x] >>= 1;
					break;
				
				/* 8xy7 */
				/* Set vx equal to vy minus vx. Set vf to 0 if there's
				 * borrow and to 1 if there isn't. */
				case 0x0007:
					cpu->v[0xF] = ((cpu->v[y] - cpu->v[x]) >= 0);
					cpu->v[x] = cpu->v[y] - cpu->v[x];
					break;
				
				/* 8xyE */
				/* Set vf equal to the most significant bit of vx. Shift
				 * vx left by one. */
				case 0x000E:
					cpu->v[0xF] = cpu->v[x] & 128;
					cpu->v[x] <<= 1;
					break;
				
				default:
					return -1;
				
			}
			break;
		
		/* 9xy0 */
		/* Skip the next instruction if vx doesn't equal vy. */
		case 0x9000:
			if(cpu->v[x] != cpu->v[y]){
				cpu->pc += 2;
			}
			break;
		
		/* Annn */
		/* Set i equal to nnn. */
		case 0xA000:
			cpu->i = nnn;
			break;
		
		/* Bnnn */
		/* Jump to address nnn plus v0. */
		case 0xB000:
			cpu->pc = nnn + cpu->v[0];
			cpu->pc -= 2;
			break;
		
		/* cxnn */
		/* Set vx to random number and nn. */
		case 0xC000:
			cpu->v[x] = (rand() % 256) & nn;
			break;
		
		/* Dxyn */
		/* Display n-byte sprite starting at i at (vx, vy). Set vf equal
		 * to 1 if there's a collision and 0 if there isn't. */
		case 0xD000:
			cpu->v[0xF] = 0;
			
			for(i = 0; i < n; i++){
				k = cpu->mem[cpu->i + i];
				for(j = 0; j < 8; j++){
					if((k & 128) > 0){
						if(cpu->display[cpu->v[y] + i][cpu->v[x] + j] == 1){
							cpu->v[0xF] = 1;
						}
						cpu->display[cpu->v[y] + i][cpu->v[x] + j] ^= 1;
					}
					k <<= 1;
				}
			}
			break;
		
		case 0xE000:
			switch(op & 0x00FF){
				
				/* Ex9E */
				/* Skip the next instruction if the key stored in vx is
				 * pressed. */
				case 0x009E:
					if(event_getKeys() & (1 << cpu->v[x])){
						cpu->pc += 2;
					}
					break;
				
				/* ExA1 */
				/* Skip the next instruction if the key stored in vx isn't
				 * pressed. */
				case 0x00A1:
					if(!(event_getKeys() & (1 << cpu->v[x]))){
						cpu->pc += 2;
					}
					break;
				
				default:
					return -1;
			}
			break;
		
		case 0xF000:
			switch(op & 0x00FF){
				
				/* Fx07 */
				/* Set vx equal to dt. */
				case 0x0007:
					cpu->v[x] = cpu->dt;
					break;
				
				/* Fx0A */
				/* Wait for keypress and store to vx. */
				case 0x000A:
					while((j = event_getKeys()) == 0){
						
						/* Update SDL events so we get the keypresses and
						 * check if SDL_QUIT has occured. */
						if((cpu->quit = event_checkQuit()) != 0){
							break;
						}
					}
					for(i = 0; i < 16; i++){
						if(j & (1 << i)){
							cpu->v[x] = i;
						}
					}
					break;
				
				/* Fx15 */
				/* Set dt equal to vx. */
				case 0x0015:
					cpu->dt = cpu->v[x];
					break;
				
				/* Fx18 */
				/* Set st equal to vx. */
				case 0x0018:
					cpu->st = cpu->v[x];
					break;
				
				/* Fx1E */
				/* Set i equal to i plus vx. */
				case 0x001E:
					cpu->i += cpu->v[x];
					break;
				
				/* Fx29 */
				/* Set I equal to location of sprite for digit vx. */
				case 0x0029:
					cpu->i = cpu->v[x] * 5;
					break;
				
				/* Fx33 */
				/* Store BCD representation of vx in memory starting at
				 * i. */
				case 0x0033:
					j = cpu->v[x];
					for(i = 3; i > 0; i--){
						cpu->mem[cpu->i + i - 1] = j % 10;
						j /= 10;
					}
					break;
				
				/* Fx55 */
				/* Store v0 to vx in memory starting at address i. */
				case 0x0055:
					for(i = 0; i <= x; i++){
						cpu->mem[cpu->i + i] = cpu->v[i];
					}
					break;
				
				/* Fx65 */
				/* Load v0 to vx from memory starting at address i. */
				case 0x0065:
					for(i = 0; i <= x; i++){
						cpu->v[i] = cpu->mem[cpu->i + i];
					}
					break;
				
				default:
					return -1;
			}
			break;
		default:
			return -1;
	}
	return 0;
}
