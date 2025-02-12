#include <stdio.h>
#include <tonc.h>

#include "metatiles.h"

const u16 debug_metatiles[] = {
    0x0000, 0x0000, 0x0000, 0x0000,
    0x0002 | SE_HFLIP, 0x0001, 0x0022 | SE_HFLIP, 0x0021,
    0x0001, 0x0001, 0x0021, 0x0021,
    0x0001, 0x0002, 0x0021, 0x0022,
    0x0003 | SE_HFLIP, 0x0001, 0x0021, 0x0021,
    0x0001, 0x0003, 0x0021, 0x0021,
    0x0004, 0x0000, 0x0000, 0x0000,
    0x0021, 0x0021, 0x0021, 0x0005 | SE_VFLIP,
    0x0021, 0x0021, 0x0005 | SE_VFLIP | SE_HFLIP, 0x0021,
    
    0x0000, 0x0000, 0x0000, 0x0000,
    0x0022 | SE_HFLIP, 0x0021, 0x0022 | SE_HFLIP, 0x0021,
    0x0021, 0x0021, 0x0021, 0x0021,
    0x0021, 0x0022, 0x0021, 0x0022,
    0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000,
    0x0021, 0x0005, 0x0021, 0x0021,
    0x0005 | SE_HFLIP, 0x0021, 0x0021, 0x0021,
    
    0x0000, 0x0000, 0x0000, 0x0000,
    0x0022 | SE_HFLIP, 0x0021, 0x0002 | SE_HFLIP | SE_VFLIP, 0x0001 | SE_VFLIP,
    0x0021, 0x0021, 0x0001 | SE_VFLIP, 0x0001 | SE_VFLIP, 
    0x0021, 0x0022, 0x0001 | SE_VFLIP, 0x0002 | SE_VFLIP
};

const unsigned int debug_metatile_collisions[] = {
    EMPTY,
    SOLID_UP | SOLID_DOWN | SOLID_HORIZONTAL,
    SOLID_UP | SOLID_DOWN | SOLID_HORIZONTAL,
    SOLID_UP | SOLID_DOWN | SOLID_HORIZONTAL,
    SOLID_UP,
    SOLID_UP,
    GRIP,
    EMPTY,
    EMPTY,
    
    EMPTY,
    SOLID_UP | SOLID_DOWN | SOLID_HORIZONTAL,
    SOLID_UP | SOLID_DOWN | SOLID_HORIZONTAL,
    SOLID_UP | SOLID_DOWN | SOLID_HORIZONTAL,
    EMPTY,
    EMPTY,
    EMPTY,
    SOLID_UP,
    SOLID_UP,
    
    EMPTY,
    SOLID_UP | SOLID_DOWN | SOLID_HORIZONTAL,
    SOLID_UP | SOLID_DOWN | SOLID_HORIZONTAL,
    SOLID_UP | SOLID_DOWN | SOLID_HORIZONTAL
};