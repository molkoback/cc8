/* video.c
 * 
 *  This file is part of cc8.
 *  
 *  Copyright © 2014 molko <molkoback@gmail.com>
 *  This work is free. You can redistribute it and/or modify it under the
 *  terms of the Do What The Fuck You Want To Public License, Version 2,
 *  as published by Sam Hocevar. See the COPYING file for more details.
 *
 */

#include <stdio.h>

#include <SDL2/SDL.h>

static const int DELAY = 1;

static SDL_Renderer *rend = NULL;
static SDL_Window *win = NULL;
static SDL_Rect rect;

/* Initializes SDL_Window and SLD_Renderer and stuff. Returns -1 on
 * failure. */
int video_init(void)
{
	if(SDL_Init(SDL_INIT_VIDEO) < 0){
		goto error;
	}
	win = SDL_CreateWindow(
		"cc8",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		640,
		320,
		SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE
	);
	if(win == NULL){
		goto error;
	}
	rend = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
	if(rend == NULL){
		goto error;
	}
	
	rect.w = rect.h = 10;
	return 0;
	error:
	fprintf(stderr, "SDL_Error: %s\n", SDL_GetError());
	return -1;
}

/* Clean up SDL on exit. */
void video_quit(void)
{
	SDL_DestroyRenderer(rend);
	SDL_DestroyWindow(win);
	SDL_Quit();
}

/* Draw the display. */
int video_draw(uint8_t  display[32][64])
{
	size_t i, j;
	for(i = 0; i < 32; i++){
		for(j = 0; j < 64; j++){
			rect.x = j * 10;
			rect.y = i * 10;
			
			if(display[i][j] == 0){
				SDL_SetRenderDrawColor(rend, 18, 18, 18, 255);
			}else{
				SDL_SetRenderDrawColor(rend, 84, 255, 84, 255);
			}
			SDL_RenderFillRect(rend, &rect);
		}
	}
	
	SDL_RenderPresent(rend);
	SDL_RenderClear(rend);
	SDL_Delay(DELAY);
	return 0;
}
