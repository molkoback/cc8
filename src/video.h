/* video.h
 * 
 *  This file is part of cc8.
 *  
 *  Copyright © 2014 molko <molkoback@gmail.com>
 *  This work is free. You can redistribute it and/or modify it under the
 *  terms of the Do What The Fuck You Want To Public License, Version 2,
 *  as published by Sam Hocevar. See the COPYING file for more details.
 *
 */

#ifndef VIDEO_H
#define VIDEO_H

#include <stdint.h>

/* Initializes SDL_Window and SLD_Renderer and stuff. Returns -1 on
 * failure. */
int video_init(void);

/* Clean up SDL variables. */
void video_quit(void);

/* Draw the display. */
int video_draw(uint8_t display[32][64]);

#endif
