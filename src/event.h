/* event.h
 * 
 *  This file is part of cc8.
 *  
 *  Copyright © 2014 molko <molkoback@gmail.com>
 *  This work is free. You can redistribute it and/or modify it under the
 *  terms of the Do What The Fuck You Want To Public License, Version 2,
 *  as published by Sam Hocevar. See the COPYING file for more details.
 *
 */

#ifndef KEYS_H
#define KEYS_H

#include <stdint.h>

/* Returns 1 if SDL_QUIT has occured. */
int event_checkQuit(void);

/* Returns currently pressed keys as integer. */
uint16_t event_getKeys(void);

#endif
