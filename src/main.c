/* main.c
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
#include "video.h"

#include <SDL2/SDL.h>

#include <stdio.h>
#include <stdint.h>

/* Reads a binary file and saves it to mem. */
static int rom_read(const char *path, uint8_t *mem)
{
	int c;
	size_t i;
	FILE *fh;
	
	if((fh = fopen(path, "rb")) == NULL){
		fprintf(stderr, "couldn't open file '%s'\n", path);
		return -1;
	}
	
	i = 512;
	while((c = getc(fh)) != EOF){
		if(i > 4095){
			fprintf(stderr, "file '%s' is too large\n", path);
			fclose(fh);
			return -1;
		}
		mem[i] = c;
		i++;
	}
	fclose(fh);
	return 0;
}

int main(int argc, char *argv[])
{
	CPU cpu;
	if(argc < 2){
		fprintf(stderr, "no rom file given\n");
		return 1;
	}
	
	cpu_reset(&cpu);
	if(rom_read(argv[1], cpu.mem) < 0){
		return 1;
	}
	
	if(video_init() < 0){
		return 1;
	}
	
	while(cpu.quit == 0){
		cpu_cycle(&cpu);
		video_draw(cpu.display);
		if(cpu.st == 1){
			printf("BEEP\n");
		}
	}
	 
	video_quit();
	return 0;
}
