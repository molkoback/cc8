/* event.c
 * 
 *  This file is part of cc8.
 *  
 *  Copyright © 2014 molko <molkoback@gmail.com>
 *  This work is free. You can redistribute it and/or modify it under the
 *  terms of the Do What The Fuck You Want To Public License, Version 2,
 *  as published by Sam Hocevar. See the COPYING file for more details.
 *
 */ 

#include "event.h"

#include <SDL2/SDL.h>

#include <stdint.h>

/* Returns 1 if SDL_QUIT has occured. */
int event_checkQuit(void)
{
	SDL_Event event;
	while(SDL_PollEvent(&event)){
		if(event.type == SDL_QUIT){
			return 1;
		}
	}
	return 0;
}

/*
 * CHIP-8 original keypad layout.
 * 
 *  Keypad                   Keyboard
 * +-+-+-+-+                +-+-+-+-+
 * |1|2|3|C|                |1|2|3|4|
 * +-+-+-+-+                +-+-+-+-+
 * |4|5|6|D|                |Q|W|E|R|
 * +-+-+-+-+       =>       +-+-+-+-+
 * |7|8|9|E|                |A|S|D|F|
 * +-+-+-+-+                +-+-+-+-+
 * |A|0|B|F|                |Z|X|C|V|
 * +-+-+-+-+                +-+-+-+-+
 *
 */

/* Returns currently pressed keys as integer. */
uint16_t event_getKeys(void)
{
	uint16_t keys;
	const uint8_t *state;
	state = SDL_GetKeyboardState(NULL);
	
	keys = 0;
	if(state[SDL_SCANCODE_1]){
		keys |= 2;
	}
	if(state[SDL_SCANCODE_2]){
		keys |= 4;
	}
	if(state[SDL_SCANCODE_3]){
		keys |= 8;
	}
	if(state[SDL_SCANCODE_4]){
		keys |= 4096;
	}
	if(state[SDL_SCANCODE_Q]){
		keys |= 16;
	}
	if(state[SDL_SCANCODE_W]){
		keys |= 32;
	}
	if(state[SDL_SCANCODE_E]){
		keys |= 64;
	}
	if(state[SDL_SCANCODE_R]){
		keys |= 8192;
	}
	if(state[SDL_SCANCODE_A]){
		keys |= 128;
	}
	if(state[SDL_SCANCODE_S]){
		keys |= 256;
	}
	if(state[SDL_SCANCODE_D]){
		keys |= 512;
	}
	if(state[SDL_SCANCODE_F]){
		keys |= 16384;
	}
	if(state[SDL_SCANCODE_Z]){
		keys |= 1024;
	}
	if(state[SDL_SCANCODE_X]){
		keys |= 1;
	}
	if(state[SDL_SCANCODE_C]){
		keys |= 2048;
	}
	if(state[SDL_SCANCODE_V]){
		keys |= 32768;
	}
	return keys;
}
