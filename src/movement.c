#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <tonc.h>
#include "mgba.h"

#include "main.h"

#include "metatiles.h"
#include "testmap_tilemap.h"

unsigned int is_colliding_aabb(unsigned int x1, unsigned int y1, unsigned int width1, unsigned int height1, \
                               unsigned int x2, unsigned int y2, unsigned int width2, unsigned int height2){
    if (x1 < x2 + width2 && \
        x1 + width1 > x2 && \
        y1 < y2 + height2 && \
        y1 + height1 > y2)
        return 1;
    return 0;
}

unsigned int is_colliding_bg(unsigned int x, unsigned int y){
    unsigned int metatile_loc = ((x >> 4) & 0x0f) + (y & 0xf0) + (x & 0xffff00) + ((y << 1) & 0xfffe00);
    unsigned int metatile_typ = (u8)testmap_tilemap[metatile_loc];
    return debug_metatile_collisions[metatile_typ];
}

void play_movement() {
    playx += playxv;
    
    if (playgrabbing != GRABBED){
        if (!playonground){
            playy += playyv;
            if (playyv < PLAY_TOP_GRAV_SPEED) {
                playyv += PLAY_GRAV_ACCEL;
            } else {
                playyv = PLAY_TOP_GRAV_SPEED;
            }
        } else {
            play_state = STAND;
        }
    }
    
    playxs = SUBPIX_TO_PIX(playx) - bgx;
    playys = SUBPIX_TO_PIX(playy) - bgy;
    
    //int playxd = 0, playyd = 0; //player x distance, y distance
    if (playxs > PLAY_RIGHT_SCROLL_AREA && bgx < SCROLL_LIMIT_RIGHT){
        bgx = SUBPIX_TO_PIX(playx) - PLAY_RIGHT_SCROLL_AREA;
        playxs = PLAY_RIGHT_SCROLL_AREA;
    } else if (playxs < PLAY_LEFT_SCROLL_AREA && bgx > 1){
        bgx = SUBPIX_TO_PIX(playx) - PLAY_LEFT_SCROLL_AREA;
        playxs = PLAY_LEFT_SCROLL_AREA;
    }
    
    if (playys > PLAY_DOWN_SCROLL_AREA && bgy < SCROLL_LIMIT_DOWN){
        bgy = SUBPIX_TO_PIX(playy) - PLAY_DOWN_SCROLL_AREA;
        playys = PLAY_DOWN_SCROLL_AREA;
    } else if (playys < PLAY_UP_SCROLL_AREA && bgy > 1){
        bgy = SUBPIX_TO_PIX(playy) - PLAY_UP_SCROLL_AREA;
        playys = PLAY_UP_SCROLL_AREA;
    }
    
    if (playxv < 0) {
        playxv += PLAY_X_DECEL;
    } else if (playxv > 0) {
        playxv -= PLAY_X_DECEL;
    }
    //free movement only
    /* if (playyv < 0) {
        playyv += PLAY_Y_DECEL;
    } else if (playyv > 0) {
        playyv -= PLAY_Y_DECEL;
    } */
    if (playgrabbing != GRABBED) {
        if (key_tri_horz() < 0) {
            play_state = RUN;
            playxdirection = -1;
            if (playxv > -PLAY_TOP_X_SPEED) {
                playxv -= PLAY_X_ACCEL;
            } else {
                playxv = -PLAY_TOP_X_SPEED;
            }
        } else if (key_tri_horz() > 0) {
            play_state = RUN;
            playxdirection = 1;
            if (playxv < PLAY_TOP_X_SPEED) {
                playxv += PLAY_X_ACCEL;
            } else {
                playxv = PLAY_TOP_X_SPEED;
            }
        }
    }
    
    // uncomment for free movement
    if (key_tri_vert() < 0) {
        playydirection = -1;
        /* if (playyv > -PLAY_TOP_Y_SPEED) {
            playyv -= PLAY_Y_ACCEL;
        } else {
            playyv = -PLAY_TOP_Y_SPEED;
        } */
    } else if (key_tri_vert() > 0) {
        playydirection = 1;
        /* if (playyv < PLAY_TOP_Y_SPEED) {
            playyv += PLAY_Y_ACCEL;
        } else {
            playyv = PLAY_TOP_Y_SPEED;
        } */
    } else {
        playydirection = 0;
    }
    
    if (!playonground && play_state != SWINGFALL){
        play_state = FALL;
    }
    
    unsigned int playx_left = (SUBPIX_TO_PIX(playx) + PLAY_LEFT);
    unsigned int playx_right = (SUBPIX_TO_PIX(playx) + PLAY_RIGHT);
    unsigned int playy_up = (SUBPIX_TO_PIX(playy) + PLAY_TOP);
    unsigned int playy_mid = (SUBPIX_TO_PIX(playy) + PLAY_MID);
    unsigned int playy_down = (SUBPIX_TO_PIX(playy) + PLAY_BOTTOM);

    unsigned int collision_up = 0;
    unsigned int collision_down = 0;
    unsigned int collision_left = 0;
    unsigned int collision_right = 0;
    unsigned int eject_up = 0;
    unsigned int eject_down = 0;
    unsigned int eject_left = 0;
    unsigned int eject_right = 0;
    
#ifdef DEBUG
    //mlog("%d\n", playonground);
    //mlog("%d\n", collision_down);
#endif
    
    // check the const data first
    if (((is_colliding_bg(playx_left, playy_up) & SOLID_HORIZONTAL) && \
        (is_colliding_bg(playx_left, playy_mid) & SOLID_HORIZONTAL)) || \
        (is_colliding_bg(playx_left, playy_mid) & SOLID_HORIZONTAL) || \
        ((is_colliding_bg(playx_left, playy_down) & SOLID_HORIZONTAL) && \
        (is_colliding_bg(playx_left, playy_mid) & SOLID_HORIZONTAL))
        ){
        collision_left = 1;
        eject_left = (playx_left+2) & 0x0f;
    } else {
        collision_left = 0;
    }
    if (((is_colliding_bg(playx_right, playy_up) & SOLID_HORIZONTAL) && \
        (is_colliding_bg(playx_right, playy_mid) & SOLID_HORIZONTAL)) || \
        ((is_colliding_bg(playx_right, playy_down) & SOLID_HORIZONTAL) && \
        (is_colliding_bg(playx_right, playy_mid) & SOLID_HORIZONTAL))
        ){
        collision_right = 1;
        eject_right = (playx_right+1) & 0x0f;
    } else {
        collision_right = 0;
    }
    
    //then use the recorded versions of the checks later instead of checking again
    if (playxv < 0) {
        if (collision_left){
            playx += PIX_TO_SUBPIX(eject_left);
            playxv = PIX_TO_SUBPIX(0);
        #ifdef DEBUG
            //mlog("left col \n");
        #endif
        }
        if (playx_left < 16) {
            playx += PIX_TO_SUBPIX(eject_left);
            playxv = PIX_TO_SUBPIX(0);
        }
    }
    if (playxv > 0) {
        if (collision_right)
        {
            playx -= PIX_TO_SUBPIX(eject_right);
            playxv = PIX_TO_SUBPIX(0);
        #ifdef DEBUG
            //mlog("right col \n");
        #endif
        }
        if (playx_right > 1024) {
            playx -= PIX_TO_SUBPIX(eject_right);
            playxv = PIX_TO_SUBPIX(0);
        }
    }
    
    //jump step's all i know
    if (playyv > PLAY_SAFE_GRAV_STEP){
    for (int jump_step = playyv; jump_step > PLAY_SAFE_GRAV_STEP; jump_step -= PLAY_SAFE_GRAV_STEP){
        playy += PLAY_SAFE_GRAV_STEP;
        
        playy_up = (SUBPIX_TO_PIX(playy) + PLAY_TOP);
        playy_down = (SUBPIX_TO_PIX(playy) + PLAY_BOTTOM);
        
        if ((is_colliding_bg(playx_left, playy_up) & SOLID_DOWN) || \
            (is_colliding_bg(playx_right, playy_up) & SOLID_DOWN)){
            collision_up = 1;
            eject_up = (playy_up+2) & 0x0f;
        } else {
            collision_up = 0;
        }
        if ((is_colliding_bg(playx_left, playy_down) & SOLID_UP) || \
            (is_colliding_bg(playx_right, playy_down) & SOLID_UP)) {
            collision_down = 1;
            eject_down = playy_down & 0x0f;
        } else {
            collision_down = 0;
        }
    
        
        if (playyv < 0) {
            if ((collision_up) && \
            playy_up >= 0
            ){
                playy += PIX_TO_SUBPIX(eject_up);
                playyv = PIX_TO_SUBPIX(0);
            #ifdef DEBUG
                //mlog("up fast col\n");
            #endif
            }
        } else if (playyv > 0) {          
            if ((collision_down) && \
            (!collision_up) && \
            playy_down < 1024
            ){
                playonground = 1;
                playy -= PIX_TO_SUBPIX(eject_down);
                playyv = PIX_TO_SUBPIX(0);
            #ifdef DEBUG
                //mlog("down fast col\n");
            #endif
            }
        } else {
            if ((!collision_down)){
                playonground = 0;
            }
        }
    }
    }
    
    playy_up = (SUBPIX_TO_PIX(playy) + PLAY_TOP);
    playy_down = (SUBPIX_TO_PIX(playy) + PLAY_BOTTOM);
    
    if ((is_colliding_bg(playx_left, playy_up) & SOLID_DOWN) || \
        (is_colliding_bg(playx_right, playy_up) & SOLID_DOWN)){
        collision_up = 1;
        eject_up = (playy_up+2) & 0x0f;
    } else {
        collision_up = 0;
    }
    if ((is_colliding_bg(playx_left, playy_down) & SOLID_UP) || \
        (is_colliding_bg(playx_right, playy_down) & SOLID_UP)) {
        collision_down = 1;
        eject_down = playy_down & 0x0f;
    } else {
        collision_down = 0;
    }
    
    
    if (playyv < 0) {
        if ((collision_up) && \
        playy_up >= 0
        ){
            playy += PIX_TO_SUBPIX(eject_up);
            playyv = PIX_TO_SUBPIX(0);
        #ifdef DEBUG
            //mlog("up slow col\n");
        #endif
        }
    } else if (playyv > 0) {
        if ((collision_down == 1) && \
        (!collision_up) && \
        playy_down < 1024
        ){
            playonground = 1;
            playy -= PIX_TO_SUBPIX(eject_down);
            playyv = PIX_TO_SUBPIX(0);
        #ifdef DEBUG
            //mlog("down slow col\n");
        #endif
        }
    } else {
        if ((!collision_down)){
            playonground = 0;
        }
    }
    
    if (key_hit(KEY_A) && playonground) {
        playyv = -PLAY_JUMP_SPEED;
        playonground = 0;
    }
    
    if (key_held(KEY_R)) {
        if (play_state != SWINGFALL){
            if (playgrabbing != GRABBED) {
                play_state = GRAB;
                playgrabbing = GRABBING;
                
                if (key_tri_vert() < 0){
                    if (key_tri_horz() != 0)
                        playtongangle = ArcTan2(playxdirection,playydirection);
                    else
                        playtongangle = ArcTan2(0,playydirection);
                } else {
                    playtongangle = ArcTan2(playxdirection,0);
                }
                
                playtongx[0] = (PLAY_TONG_LENGTH * SINLUTR_TO_SUBPIX(lu_cos(playtongangle))) + playx + PLAY_TONG_X_END_OFS;
                playtongy[0] = (PLAY_TONG_LENGTH * SINLUTR_TO_SUBPIX(lu_sin(playtongangle))) + playy + PLAY_TONG_Y_OFS;
                playtongx[1] = (38 * SINLUTR_TO_SUBPIX(lu_cos(playtongangle))) + playx + PLAY_TONG_X_OFS;
                playtongy[1] = (38 * SINLUTR_TO_SUBPIX(lu_sin(playtongangle))) + playy;
                playtongx[2] = (23 * SINLUTR_TO_SUBPIX(lu_cos(playtongangle))) + playx + PLAY_TONG_X_OFS;
                playtongy[2] = (23 * SINLUTR_TO_SUBPIX(lu_sin(playtongangle))) + playy;
                playtongx[3] = (8 * SINLUTR_TO_SUBPIX(lu_cos(playtongangle))) + playx + PLAY_TONG_X_OFS;
                playtongy[3] = (8 * SINLUTR_TO_SUBPIX(lu_sin(playtongangle))) + playy;
                
                unsigned int playtong_left = SUBPIX_TO_PIX(playtongx[0]) + PLAY_TONG_END_TOPLEFT;
                unsigned int playtong_up = SUBPIX_TO_PIX(playtongy[0]) + PLAY_TONG_END_TOPLEFT;
                unsigned int playtong_right = SUBPIX_TO_PIX(playtongx[0]) + PLAY_TONG_END_BOTTOMRIGHT;
                unsigned int playtong_down = SUBPIX_TO_PIX(playtongy[0]) + PLAY_TONG_END_BOTTOMRIGHT;
                
                if ((is_colliding_bg(playtong_left, playtong_up) & GRIP) || \
                    (is_colliding_bg(playtong_right, playtong_up) & GRIP) || \
                    (is_colliding_bg(playtong_left, playtong_down) & GRIP) || \
                    (is_colliding_bg(playtong_right, playtong_down) & GRIP)
                ){
                    play_state = SWING;
                    playgrabbing = GRABBED;
                    if (is_colliding_bg(playtong_left, playtong_up) & GRIP){
                        playtongx[0] = PIX_TO_SUBPIX(playtong_left & 0xfffff0);
                        playtongy[0] = PIX_TO_SUBPIX(playtong_up & 0xfffff0);
                    }else if (is_colliding_bg(playtong_right, playtong_up) & GRIP){
                        playtongx[0] = PIX_TO_SUBPIX(playtong_right & 0xfffff0);
                        playtongy[0] = PIX_TO_SUBPIX(playtong_up & 0xfffff0);
                    }else if (is_colliding_bg(playtong_left, playtong_down) & GRIP){
                        playtongx[0] = PIX_TO_SUBPIX(playtong_left & 0xfffff0);
                        playtongy[0] = PIX_TO_SUBPIX(playtong_down & 0xfffff0);
                    }else if (is_colliding_bg(playtong_right, playtong_down) & GRIP){
                        playtongx[0] = PIX_TO_SUBPIX(playtong_right & 0xfffff0);
                        playtongy[0] = PIX_TO_SUBPIX(playtong_down & 0xfffff0);
                    }
                    
                    playtongangle = ArcTan2(playtongx[0] - playx, playtongy[0] - playy);
                    playxv = 0;
                    playyv = 0;
                }
            } else {
                int playpendforce = SUBPIX_TO_PIX(SUBPIX_TO_PIX(PLAY_TOP_GRAV_SPEED) * SINLUTR_TO_SUBPIX(lu_cos(playtongangle)));
            
                playtonganglev += playpendforce * DIVLUTR_TO_PIX(lu_div(PLAY_TONG_LENGTH));
                playtongangle -= playtonganglev;
                
            #ifdef DEBUG
                //mlog("pendulum force: %x\n", playtonganglev);
            #endif
            
                if (playxv < PLAY_TOP_X_SPEED && playxv > -PLAY_TOP_X_SPEED)
                    playxv = playtonganglev;
                else if (playxv > PLAY_TOP_X_SPEED)
                    playxv = PLAY_TOP_X_SPEED;
                else
                    playxv = -PLAY_TOP_X_SPEED;
                
                if (playyv < PLAY_TOP_GRAV_SPEED && playyv > -PLAY_TOP_GRAV_SPEED)
                    playyv = -playtonganglev;
                else if (playyv > PLAY_TOP_GRAV_SPEED)
                    playyv = PLAY_TOP_GRAV_SPEED;
                else
                    playyv = -PLAY_TOP_GRAV_SPEED;
                
                playtongx[1] = (41 * SINLUTR_TO_SUBPIX(lu_cos(32768-playtongangle))) + playtongx[0] - PLAY_TONG_X_OFS;
                playtongy[1] = (41 * SINLUTR_TO_SUBPIX(lu_sin(-playtongangle))) + playtongy[0] - PLAY_TONG_Y_OFS;
                playtongx[2] = (26 * SINLUTR_TO_SUBPIX(lu_cos(32768-playtongangle))) + playtongx[0] - PLAY_TONG_X_OFS;
                playtongy[2] = (26 * SINLUTR_TO_SUBPIX(lu_sin(-playtongangle))) + playtongy[0] - PLAY_TONG_Y_OFS;
                playtongx[3] = (11 * SINLUTR_TO_SUBPIX(lu_cos(32768-playtongangle))) + playtongx[0] - PLAY_TONG_X_OFS;
                playtongy[3] = (11 * SINLUTR_TO_SUBPIX(lu_sin(-playtongangle))) + playtongy[0] - PLAY_TONG_Y_OFS;
                playx = (PLAY_TONG_LENGTH * SINLUTR_TO_SUBPIX(lu_cos(32768-playtongangle))) + playtongx[0] - PLAY_TONG_X_END_OFS;
                playy = (PLAY_TONG_LENGTH * SINLUTR_TO_SUBPIX(lu_sin(-playtongangle))) + playtongy[0] - PLAY_TONG_Y_OFS;
                if (key_hit(KEY_A)) {
                    play_state = SWINGFALL;
                    playgrabbing = NONE;
                    //set the player velocities to play nice with deceleration code
                    playyv = (playyv & 0xfffffff0) - PLAY_JUMP_SPEED;
                    playxv &= 0xfffffff0;
                }
            }
        }
    } else if (key_released(KEY_R)) {
        playgrabbing = NONE;
        //set the player velocities to play nice with deceleration code
        playyv &= 0xfffffff0;
        playxv &= 0xfffffff0;
    }
    
    #ifdef DEBUG
        //mlog("tongx: %x\t", playtongx);
        //mlog("tongy: %x\n", playtongy);
        //mlog("angle: %x\n", playtongangle);
        mlog("x velocity: %x\n", playxv);
        mlog("y velocity: %x\n", playyv);
    #endif
    playtongxs[0] = SUBPIX_TO_PIX(playtongx[0]) - bgx;
    playtongys[0] = SUBPIX_TO_PIX(playtongy[0]) - bgy;
    playtongxs[1] = SUBPIX_TO_PIX(playtongx[1]) - bgx;
    playtongys[1] = SUBPIX_TO_PIX(playtongy[1]) - bgy;
    playtongxs[2] = SUBPIX_TO_PIX(playtongx[2]) - bgx;
    playtongys[2] = SUBPIX_TO_PIX(playtongy[2]) - bgy;
    playtongxs[3] = SUBPIX_TO_PIX(playtongx[3]) - bgx;
    playtongys[3] = SUBPIX_TO_PIX(playtongy[3]) - bgy;
#ifdef DEBUG
    //mlog("%d", play_state);
#endif
}