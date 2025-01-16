#include <stdio.h>
#include <stdarg.h>
#include <tonc.h>
#include "mgba.h"

#include "main.h"
#include "movement.h"

#include "metatiles.h"
#include "testmap_tilemap.h"

#include "testtileset.h"
#include "coqmansheet.h"
#include "coqmansheet_metasprite.h"

OBJ_ATTR obj_buffer[128];
OBJ_AFFINE *obj_aff_buffer= (OBJ_AFFINE*)obj_buffer;

// Scroll around some
int bgx= 0, bgy= 0;

//player values
unsigned int playx = PIX_TO_SUBPIX(48), playy = PIX_TO_SUBPIX(32);
unsigned int playxs = 0, playys = 0; //player x screen, y screen
int playxv = 0, playyv = 0; //player x velocity, y velocity
int playxdirection = 1;
int playydirection = 0;
int oldplayxdirection = 1;
int oldplayydirection = 0;
unsigned int playonground = 0;
enum grab_state playgrabbing = NONE;
enum player_state play_state = STAND;

unsigned int playtongx[4], playtongy[4];
unsigned int playtongxs[4], playtongys[4];
int playtongv = 0;
s16 playtongangle = 0;
s16 playtonganglegoal = 0;
s16 playtonganglev = 0;
int playtonglength = 0;

void init_tiles() {
	// Load palette
	memcpy32(&pal_bg_mem[0], testtileset_pal, (sizeof(testtileset_pal) >> 2));
	// Load tiles into CBB 0
	memcpy32(&tile_mem[0][0], testtileset_char, (sizeof(testtileset_char) >> 2));
}

void init_play() {
    //load player palette
    memcpy32(&pal_obj_mem[0], coqmansheet_pal, (sizeof(coqmansheet_pal) >> 2));
    //Load player tiles
    memcpy32(&tile_mem_obj[0][0], coqmansheet_char, (sizeof(coqmansheet_char) >> 2));
    
    //player tongue
    obj_set_attr(&obj_buffer[0], ATTR0_SQUARE | ATTR0_AFF, ATTR1_SIZE_8 | ATTR1_AFF_ID(0), ATTR2_PALBANK(0) | 630);
    obj_set_pos(&obj_buffer[0], 60, 4);
    obj_aff_identity(&obj_aff_buffer[0]);
    
    obj_set_attr(&obj_buffer[1], ATTR0_SQUARE | ATTR0_AFF, ATTR1_SIZE_16 | ATTR1_AFF_ID(0), ATTR2_PALBANK(0) | 628);
    obj_set_pos(&obj_buffer[1], 44, 0);
    obj_aff_identity(&obj_aff_buffer[1]);
    
    obj_set_attr(&obj_buffer[2], ATTR0_SQUARE | ATTR0_AFF, ATTR1_SIZE_16 | ATTR1_AFF_ID(0), ATTR2_PALBANK(0) | 628);
    obj_set_pos(&obj_buffer[2], 28, 0);
    obj_aff_identity(&obj_aff_buffer[2]);
    
    obj_set_attr(&obj_buffer[3], ATTR0_SQUARE | ATTR0_AFF, ATTR1_SIZE_16 | ATTR1_AFF_ID(0), ATTR2_PALBANK(0) | 628);
    obj_set_pos(&obj_buffer[3], 12, 0);
    obj_aff_identity(&obj_aff_buffer[3]);
    
    obj_hide_multi(&obj_buffer[0], 4);
}

void metasprite_build(unsigned int oamindex, unsigned int x, unsigned int y, const u16 * frame, unsigned int hflip, unsigned int vflip) {
    //always know how many sprites each metasprite frame takes before calling
    for (int i = 0, j = 0; i < (sizeof(frame)<<1); i+=4, j++){
        obj_set_attr(&obj_buffer[oamindex + j], ((frame[i+3] & 0b0011000000000000) << 2), ((frame[i+3] & ATTR1_SIZE_MASK)) | (hflip << 12) | (vflip << 13), frame[i+2]);
        #ifdef DEBUG
        mlog("sprite number: %d", oamindex+j);
        mlog("sprite shape: %x", ((frame[i+3] & 0b0011000000000000) << 2)); //attr0 shape mask is shifted right 2 bits and then shifted left again
        mlog("sprite size: %x", ((frame[i+3] & ATTR1_SIZE_MASK)));
        #endif
        obj_set_pos(&obj_buffer[oamindex + j], x + (hflip ? (~frame[i+1]) + 32 : frame[i+1]), y + (vflip ? (~frame[i]) + 8 : frame[i]));
    }
}

void play_update() {
    if (playxdirection < 0) {
        metasprite_build(4, playxs - 8, playys - 8, coqman__stand_0, 1, 0);
    } else {
        metasprite_build(4, playxs - 8, playys - 8, coqman__stand_0, 0, 0);
    }
    if (playgrabbing != NONE) {
        obj_unhide(&obj_buffer[0], ATTR0_AFF);
        obj_set_pos(&obj_buffer[0], playtongxs[0], playtongys[0]);
        if (playtonglength >= PIX_TO_SUBPIX(36)){
            obj_unhide(&obj_buffer[1], ATTR0_AFF);
            obj_set_pos(&obj_buffer[1], playtongxs[1], playtongys[1]);
        }else
            obj_hide(&obj_buffer[1]);
        if (playtonglength >= PIX_TO_SUBPIX(24)){
            obj_unhide(&obj_buffer[2], ATTR0_AFF);
            obj_set_pos(&obj_buffer[2], playtongxs[2], playtongys[2]);
        }else
            obj_hide(&obj_buffer[2]);
        if (playtonglength >= PIX_TO_SUBPIX(12)){
            obj_unhide(&obj_buffer[3], ATTR0_AFF);
            obj_set_pos(&obj_buffer[3], playtongxs[3], playtongys[3]);
        }else
            obj_hide(&obj_buffer[3]);
        obj_aff_rotate(&obj_aff_buffer[0], -playtongangle);
    } else {
        obj_hide_multi(&obj_buffer[0], 4);
    }
}

void init_map() {
    u32 *dst = (u32*)se_mem;
    u32 *src = (u32*)debug_metatiles;
    //each tilemap entry within a screenblock is 2 bytes (1 halfword)
    
    //each metatile is defined as 4 tilemap entries, which is 8 bytes, 4 halfwords, and 2 full words
    
    //we are reading one byte and then writing 4 bytes (1 full word)
    for (int j = 0; j < 2048; j += 512){ //all 4 screenblocks combined have 4096 tiles or 1024 metatiles
        for (int y = 0; y < 512; y += 32){ //each screenblock has 1024 tiles or 256 metatiles
            for (int x = 0; x < 16; x++){ //each screenblock is 32 tiles across or 16 metatiles across
                dst[(x) + (y) + (j) + (28*512)] = src[((u8)testmap_tilemap[(x) + (y>>1) + (j>>1)]<<1)];
                dst[(x) + (y+16) + (j) + (28*512)] = src[((u8)testmap_tilemap[(x) + (y>>1) + (j>>1)]<<1)+1];
            }
        }
    }
}

IWRAM_CODE void isr_master();
IWRAM_CODE void hbl_grad_direct();

// Function pointers to master isrs.
const fnptr master_isrs[2]=
{
	(fnptr)isr_master,
	(fnptr)hbl_grad_direct
};

int main() {
    REG_DISPCNT= DCNT_BLANK;
    
    init_tiles();
    init_map();
    
    oam_init(obj_buffer, 128);
    init_play();
    
    // enable vblank register
	irq_init(master_isrs[0]);
    irq_add(II_VBLANK, NULL);
    
    #ifdef DEBUG
    mlog("Game start.\n");
    #endif
    REG_BG0CNT= BG_CBB(0) | BG_SBB(28) | BG_4BPP | BG_REG_64x64;
    REG_DISPCNT= DCNT_MODE0 | DCNT_BG0 | DCNT_OBJ | DCNT_OBJ_2D;
    
    while(1){
        VBlankIntrWait();
        key_poll();
        
        oldplayxdirection = playxdirection;
        oldplayydirection = playydirection;
        
        play_movement();

		REG_BG0HOFS= bgx;
		REG_BG0VOFS= bgy;
        
        play_update();
        
        oam_copy(oam_mem, obj_buffer, 9);	// only need to update a few
        // we have 1 OBJ_AFFINEs, update these separately
		obj_aff_copy(obj_aff_mem, obj_aff_buffer, 1);
    }
}