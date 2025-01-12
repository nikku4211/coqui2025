#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <tonc.h>
#pragma once

extern unsigned int is_colliding_aabb(unsigned int x1, unsigned int y1, unsigned int width1, unsigned int height1, \
                               unsigned int x2, unsigned int y2, unsigned int width2, unsigned int height2);

extern unsigned int is_colliding_bg(unsigned int x, unsigned int y);

extern void play_movement();