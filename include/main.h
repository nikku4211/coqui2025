#pragma once
#include <stdio.h>
#include <tonc.h>

#define DEBUG 1

#define PLAY_RIGHT_SCROLL_AREA 120
#define PLAY_LEFT_SCROLL_AREA 104
#define PLAY_UP_SCROLL_AREA 56
#define PLAY_DOWN_SCROLL_AREA 72

#define SCROLL_LIMIT_RIGHT 272
#define SCROLL_LIMIT_DOWN 336

#define PIX_TO_SUBPIX(n) ((n) << 8)
#define SUBPIX_TO_PIX(n) ((n) >> 8)
#define PIX_TO_SINLUT(n) ((n) << 8)
#define SINLUTR_TO_SUBPIX(n) ((n) >> 4)
#define DIVLUTR_TO_PIX(n) ((n) >> 8)
#define SINLUTA_TO_ATAN2(n) ((n) << 1)
#define ATAN2_TO_SINLUTA(n) ((n) >> 1)

#define PLAY_LEFT 7
#define PLAY_RIGHT 15
#define PLAY_TOP 1
#define PLAY_MID 17
#define PLAY_BOTTOM 31
#define PLAY_X_ACCEL 32
#define PLAY_X_DECEL 16
#define PLAY_TOP_X_SPEED PIX_TO_SUBPIX(2)
//gravity
#define PLAY_GRAV_ACCEL 32
#define PLAY_JUMP_SPEED PIX_TO_SUBPIX(3)
#define PLAY_TOP_GRAV_SPEED PIX_TO_SUBPIX(5)
#define PLAY_SAFE_GRAV_STEP PIX_TO_SUBPIX(4)
//free movement
#define PLAY_Y_ACCEL 32
#define PLAY_Y_DECEL 16
#define PLAY_TOP_Y_SPEED PIX_TO_SUBPIX(2)

//tongue
#define PLAY_TONG_ACCEL 32
#define PLAY_TONG_DECEL 16
#define PLAY_TONG_LENGTH 45 //minus end
#define PLAY_TONG_END_TOPLEFT 6
#define PLAY_TONG_END_BOTTOMRIGHT 10
#define PLAY_TONG_X_OFS PIX_TO_SUBPIX(4)
#define PLAY_TONG_Y_OFS PIX_TO_SUBPIX(4)
#define PLAY_TONG_X_END_OFS PIX_TO_SUBPIX(8)

//player states
enum player_state {
    STAND,
    RUN,
    FALL,
    GRAB,
    SWING,
    SWINGFALL
};

//player states
enum grab_state {
    NONE,
    GRABBING,
    GRABBED
};

// Scroll around some
extern int bgx, bgy;
extern unsigned int playx, playy;
extern unsigned int playxs, playys; //player x screen, y screen
extern int playxv, playyv; //player x velocity, y velocity
extern int playxdirection;
extern int playydirection;
extern unsigned int playonground;
extern enum grab_state playgrabbing;
extern enum player_state play_state;

extern unsigned int playtongx[4], playtongy[4];
extern unsigned int playtongxs[4], playtongys[4];
extern int playtongxv, playtongyv;
extern unsigned int playtongangle;
extern int playtonganglev;