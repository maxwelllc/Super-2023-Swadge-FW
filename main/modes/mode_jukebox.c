/*
 * mode_jukebox.c
 *
 *  Created on: 27 Oct 2022
 *      Author: VanillyNeko#4169 & Bryce
 */

/*==============================================================================
 * Includes
 *============================================================================*/

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <math.h>

#include "display.h"
#include "led_util.h"
#include "mode_dance.h"
#include "mode_main_menu.h"
#include "musical_buzzer.h"
#include "settingsManager.h"
#include "swadgeMode.h"
#include "swadge_util.h"
#include "touch_sensor.h"
#include "nvs_manager.h"

#include "fighter_music.h"
#include "fighter_mp_result.h"
#include "mode_fighter.h"
#include "mode_tiltrads.h"
#include "platformer_sounds.h"
#include "mode_jumper.h"
#include "mode_tunernome.h"
#include "mode_credits.h"
#include "mode_test.h"
#include "mode_picross.h"
#include "picross_menu.h"
#include "picross_music.h"
#include "paint_song.h"

#include "mode_jukebox.h"
#include "meleeMenu.h"

/*==============================================================================
 * Defines
 *============================================================================*/

#define CORNER_OFFSET 14
#define LINE_BREAK_Y 8

#define MAX_LED_BRIGHTNESS 7

#define lengthof(x) (sizeof(x) / sizeof(x[0]))

/*==============================================================================
 * Enums
 *============================================================================*/

// The state data
typedef enum
{
    JUKEBOX_MENU,
    JUKEBOX_PLAYER
} jukeboxScreen_t;

/*==============================================================================
 * Prototypes
 *============================================================================*/

void  jukeboxEnterMode(display_t* disp);
void  jukeboxExitMode(void);
void  jukeboxButtonCallback(buttonEvt_t* evt);
void  jukeboxTouchCallback(touch_event_t* evt);
void  jukeboxMainLoop(int64_t elapsedUs);
void  jukeboxMainMenuCb(const char* opt);
void  jukeboxBackgroundDrawCb(display_t* disp, int16_t x, int16_t y,
                             int16_t w, int16_t h, int16_t up, int16_t upNum );

void setJukeboxMainMenu(bool resetPos);
static bool beatenRickLevel(void);

/*==============================================================================
 * Structs
 *============================================================================*/

typedef struct
{
    display_t* disp;

    font_t ibm_vga8;
    font_t radiostars;
    font_t mm;

    wsg_t arrow;
    wsg_t background;

    // Touch
    bool touchHeld;
    int32_t touchPosition;
    int32_t touchIntensity;

    // Light Dances
    portableDance_t* portableDances;

    // Jukebox Stuff
    uint8_t categoryIdx;
    uint8_t songIdx;
    bool inMusicSubmode;

    meleeMenu_t* menu;
    uint16_t mainMenuPos;
    jukeboxScreen_t screen;
} jukebox_t;

jukebox_t* jukebox;

typedef struct
{
    char* name;
    const song_t* song;
} jukeboxSong;

typedef struct
{
    char* categoryName;
    uint8_t numSongs;
    const jukeboxSong* songs;
} jukeboxCategory;

/*==============================================================================
 * Variables
 *============================================================================*/

swadgeMode modeJukebox =
{
    .modeName = "Jukebox",
    .fnEnterMode = jukeboxEnterMode,
    .fnExitMode = jukeboxExitMode,
    .fnButtonCallback = jukeboxButtonCallback,
    .fnTouchCallback = jukeboxTouchCallback,
    .fnMainLoop = jukeboxMainLoop,
    .wifiMode = NO_WIFI,
    .fnEspNowRecvCb = NULL,
    .fnEspNowSendCb = NULL,
    .fnAccelerometerCallback = NULL,
    .fnAudioCallback = NULL,
    .fnBackgroundDrawCallback = jukeboxBackgroundDrawCb,
    .overrideUsb = false
};

/*==============================================================================
 * Const Variables
 *============================================================================*/

// Text
static const char str_jukebox[]  = "Jukebox";
static const char str_bgm_muted[] =  "Swadge music is muted!";
static const char str_sfx_muted[] =  "Swadge SFX are muted!";
static const char str_bgm[] = "Music";
static const char str_sfx[] = "SFX";
static const char str_exit[] = "Exit";
static const char str_leds[] = "Sel: LEDs:";
static const char str_back[] = "Start: Back";
static const char str_brightness[] = "Y~X: LED Brightness:";
static const char str_stop[] = ": Stop";
static const char str_play[] = ": Play";

// Songs
static const song_t hotrod =
{
    .notes =
    {
        {.note = A_2, .timeMs = 187},
        {.note = C_3, .timeMs = 187},
        {.note = SILENCE, .timeMs = 187},
        {.note = A_2, .timeMs = 187},
        {.note = D_3, .timeMs = 187},
        {.note = E_3, .timeMs = 187},
        {.note = G_2, .timeMs = 187},
        {.note = C_3, .timeMs = 187},
        {.note = SILENCE, .timeMs = 187},
        {.note = G_2, .timeMs = 187},
        {.note = D_SHARP_3, .timeMs = 187},
        {.note = D_3, .timeMs = 187},
        {.note = F_SHARP_2, .timeMs = 187},
        {.note = C_3, .timeMs = 187},
        {.note = SILENCE, .timeMs = 187},
        {.note = F_SHARP_2, .timeMs = 187},
        {.note = D_3, .timeMs = 187},
        {.note = B_2, .timeMs = 187},
        {.note = F_2, .timeMs = 187},
        {.note = C_3, .timeMs = 187},
        {.note = SILENCE, .timeMs = 187},
        {.note = F_2, .timeMs = 187},
        {.note = F_3, .timeMs = 187},
        {.note = E_3, .timeMs = 187},
        {.note = A_2, .timeMs = 187},
        {.note = C_3, .timeMs = 187},
        {.note = SILENCE, .timeMs = 187},
        {.note = A_2, .timeMs = 187},
        {.note = D_3, .timeMs = 187},
        {.note = E_3, .timeMs = 187},
        {.note = G_2, .timeMs = 187},
        {.note = C_3, .timeMs = 187},
        {.note = SILENCE, .timeMs = 187},
        {.note = G_2, .timeMs = 187},
        {.note = D_SHARP_3, .timeMs = 187},
        {.note = D_3, .timeMs = 187},
        {.note = F_SHARP_2, .timeMs = 187},
        {.note = C_3, .timeMs = 187},
        {.note = SILENCE, .timeMs = 187},
        {.note = F_SHARP_2, .timeMs = 187},
        {.note = D_3, .timeMs = 187},
        {.note = B_2, .timeMs = 187},
        {.note = F_2, .timeMs = 187},
        {.note = C_3, .timeMs = 187},
        {.note = SILENCE, .timeMs = 187},
        {.note = F_2, .timeMs = 187},
        {.note = F_3, .timeMs = 187},
        {.note = E_3, .timeMs = 187},
        {.note = A_3, .timeMs = 46},
        {.note = A_4, .timeMs = 140},
        {.note = C_3, .timeMs = 187},
        {.note = SILENCE, .timeMs = 187},
        {.note = A_2, .timeMs = 187},
        {.note = D_3, .timeMs = 187},
        {.note = E_3, .timeMs = 187},
        {.note = G_2, .timeMs = 187},
        {.note = C_3, .timeMs = 187},
        {.note = SILENCE, .timeMs = 187},
        {.note = G_2, .timeMs = 187},
        {.note = D_SHARP_3, .timeMs = 187},
        {.note = D_3, .timeMs = 187},
        {.note = F_SHARP_2, .timeMs = 187},
        {.note = C_3, .timeMs = 187},
        {.note = SILENCE, .timeMs = 187},
        {.note = F_SHARP_2, .timeMs = 187},
        {.note = D_3, .timeMs = 187},
        {.note = B_2, .timeMs = 187},
        {.note = F_2, .timeMs = 187},
        {.note = C_3, .timeMs = 187},
        {.note = SILENCE, .timeMs = 187},
        {.note = F_2, .timeMs = 187},
        {.note = F_3, .timeMs = 187},
        {.note = E_3, .timeMs = 187},
        {.note = A_3, .timeMs = 46},
        {.note = A_4, .timeMs = 140},
        {.note = C_3, .timeMs = 187},
        {.note = SILENCE, .timeMs = 187},
        {.note = A_2, .timeMs = 187},
        {.note = D_3, .timeMs = 187},
        {.note = E_3, .timeMs = 187},
        {.note = G_2, .timeMs = 187},
        {.note = C_3, .timeMs = 187},
        {.note = SILENCE, .timeMs = 187},
        {.note = G_2, .timeMs = 187},
        {.note = D_SHARP_3, .timeMs = 187},
        {.note = D_3, .timeMs = 187},
        {.note = F_SHARP_2, .timeMs = 187},
        {.note = C_3, .timeMs = 187},
        {.note = SILENCE, .timeMs = 187},
        {.note = F_SHARP_4, .timeMs = 187},
        {.note = G_4, .timeMs = 187},
        {.note = G_SHARP_4, .timeMs = 187},
        {.note = A_2, .timeMs = 23},
        {.note = G_SHARP_5, .timeMs = 23},
        {.note = A_5, .timeMs = 23},
        {.note = SILENCE, .timeMs = 117},
        {.note = A_2, .timeMs = 23},
        {.note = G_SHARP_5, .timeMs = 23},
        {.note = A_5, .timeMs = 23},
        {.note = SILENCE, .timeMs = 117},
        {.note = A_2, .timeMs = 23},
        {.note = G_SHARP_5, .timeMs = 23},
        {.note = A_5, .timeMs = 23},
        {.note = G_SHARP_5, .timeMs = 23},
        {.note = SILENCE, .timeMs = 93},
        {.note = A_2, .timeMs = 23},
        {.note = G_SHARP_5, .timeMs = 23},
        {.note = A_5, .timeMs = 23},
        {.note = G_SHARP_5, .timeMs = 23},
        {.note = SILENCE, .timeMs = 93},
        {.note = A_5, .timeMs = 23},
        {.note = G_SHARP_5, .timeMs = 23},
        {.note = G_5, .timeMs = 23},
        {.note = F_5, .timeMs = 23},
        {.note = D_5, .timeMs = 23},
        {.note = C_5, .timeMs = 23},
        {.note = A_4, .timeMs = 23},
        {.note = F_4, .timeMs = 23},
        {.note = D_4, .timeMs = 23},
        {.note = C_4, .timeMs = 23},
        {.note = A_3, .timeMs = 23},
        {.note = F_3, .timeMs = 23},
        {.note = C_SHARP_3, .timeMs = 23},
        {.note = B_2, .timeMs = 23},
        {.note = A_2, .timeMs = 46},
        {.note = A_2, .timeMs = 140},
        {.note = SILENCE, .timeMs = 46},
        {.note = A_3, .timeMs = 140},
        {.note = SILENCE, .timeMs = 234},
        {.note = A_3, .timeMs = 187},
        {.note = G_SHARP_3, .timeMs = 187},
        {.note = G_3, .timeMs = 140},
        {.note = SILENCE, .timeMs = 46},
        {.note = G_2, .timeMs = 140},
        {.note = SILENCE, .timeMs = 46},
        {.note = G_3, .timeMs = 140},
        {.note = SILENCE, .timeMs = 234},
        {.note = G_3, .timeMs = 140},
        {.note = SILENCE, .timeMs = 46},
        {.note = G_3, .timeMs = 187},
        {.note = D_SHARP_3, .timeMs = 140},
        {.note = SILENCE, .timeMs = 46},
        {.note = F_SHARP_2, .timeMs = 140},
        {.note = SILENCE, .timeMs = 46},
        {.note = F_SHARP_3, .timeMs = 140},
        {.note = SILENCE, .timeMs = 234},
        {.note = F_SHARP_3, .timeMs = 140},
        {.note = SILENCE, .timeMs = 46},
        {.note = F_SHARP_3, .timeMs = 187},
        {.note = D_3, .timeMs = 140},
        {.note = SILENCE, .timeMs = 46},
        {.note = F_2, .timeMs = 140},
        {.note = SILENCE, .timeMs = 46},
        {.note = F_3, .timeMs = 140},
        {.note = SILENCE, .timeMs = 234},
        {.note = F_3, .timeMs = 140},
        {.note = SILENCE, .timeMs = 46},
        {.note = F_3, .timeMs = 187},
        {.note = E_3, .timeMs = 140},
        {.note = SILENCE, .timeMs = 46},
        {.note = A_2, .timeMs = 140},
        {.note = SILENCE, .timeMs = 46},
        {.note = A_3, .timeMs = 140},
        {.note = SILENCE, .timeMs = 234},
        {.note = A_3, .timeMs = 187},
        {.note = G_SHARP_3, .timeMs = 187},
        {.note = G_3, .timeMs = 140},
        {.note = SILENCE, .timeMs = 46},
        {.note = G_2, .timeMs = 140},
        {.note = SILENCE, .timeMs = 46},
        {.note = G_3, .timeMs = 140},
        {.note = SILENCE, .timeMs = 234},
        {.note = G_3, .timeMs = 140},
        {.note = SILENCE, .timeMs = 46},
        {.note = G_3, .timeMs = 187},
        {.note = D_SHARP_3, .timeMs = 140},
        {.note = SILENCE, .timeMs = 46},
        {.note = F_SHARP_2, .timeMs = 140},
        {.note = SILENCE, .timeMs = 46},
        {.note = F_SHARP_3, .timeMs = 140},
        {.note = SILENCE, .timeMs = 234},
        {.note = F_SHARP_3, .timeMs = 140},
        {.note = SILENCE, .timeMs = 46},
        {.note = F_SHARP_3, .timeMs = 187},
        {.note = D_3, .timeMs = 140},
        {.note = SILENCE, .timeMs = 46},
        {.note = F_2, .timeMs = 140},
        {.note = SILENCE, .timeMs = 46},
        {.note = F_3, .timeMs = 140},
        {.note = SILENCE, .timeMs = 234},
        {.note = F_3, .timeMs = 140},
        {.note = SILENCE, .timeMs = 46},
        {.note = F_3, .timeMs = 187},
        {.note = E_3, .timeMs = 140},
        {.note = SILENCE, .timeMs = 46},
        {.note = A_3, .timeMs = 187},
        {.note = C_4, .timeMs = 187},
        {.note = SILENCE, .timeMs = 187},
        {.note = A_3, .timeMs = 187},
        {.note = D_4, .timeMs = 187},
        {.note = E_4, .timeMs = 187},
        {.note = G_3, .timeMs = 187},
        {.note = C_4, .timeMs = 187},
        {.note = SILENCE, .timeMs = 187},
        {.note = G_3, .timeMs = 187},
        {.note = D_SHARP_4, .timeMs = 187},
        {.note = D_4, .timeMs = 187},
        {.note = F_SHARP_3, .timeMs = 187},
        {.note = C_4, .timeMs = 187},
        {.note = SILENCE, .timeMs = 187},
        {.note = F_SHARP_3, .timeMs = 187},
        {.note = D_4, .timeMs = 187},
        {.note = B_3, .timeMs = 187},
        {.note = F_3, .timeMs = 187},
        {.note = C_4, .timeMs = 187},
        {.note = SILENCE, .timeMs = 187},
        {.note = F_3, .timeMs = 187},
        {.note = F_4, .timeMs = 187},
        {.note = E_4, .timeMs = 187},
        {.note = A_3, .timeMs = 187},
        {.note = C_4, .timeMs = 187},
        {.note = SILENCE, .timeMs = 187},
        {.note = A_3, .timeMs = 187},
        {.note = D_4, .timeMs = 187},
        {.note = E_4, .timeMs = 187},
        {.note = G_3, .timeMs = 187},
        {.note = C_4, .timeMs = 187},
        {.note = SILENCE, .timeMs = 187},
        {.note = G_3, .timeMs = 187},
        {.note = D_SHARP_4, .timeMs = 187},
        {.note = D_4, .timeMs = 187},
        {.note = F_SHARP_3, .timeMs = 187},
        {.note = C_4, .timeMs = 187},
        {.note = SILENCE, .timeMs = 187},
        {.note = F_SHARP_3, .timeMs = 187},
        {.note = D_4, .timeMs = 187},
        {.note = B_3, .timeMs = 187},
        {.note = F_3, .timeMs = 187},
        {.note = C_4, .timeMs = 187},
        {.note = SILENCE, .timeMs = 187},
        {.note = F_3, .timeMs = 187},
        {.note = F_4, .timeMs = 187},
        {.note = E_4, .timeMs = 187},
        {.note = A_4, .timeMs = 187},
        {.note = A_3, .timeMs = 187},
        {.note = SILENCE, .timeMs = 187},
        {.note = A_2, .timeMs = 187},
        {.note = SILENCE, .timeMs = 187},
        {.note = A_2, .timeMs = 187},
        {.note = G_4, .timeMs = 187},
        {.note = G_3, .timeMs = 187},
        {.note = SILENCE, .timeMs = 187},
        {.note = G_2, .timeMs = 187},
        {.note = SILENCE, .timeMs = 187},
        {.note = G_2, .timeMs = 187},
        {.note = F_SHARP_4, .timeMs = 187},
        {.note = F_SHARP_3, .timeMs = 187},
        {.note = SILENCE, .timeMs = 187},
        {.note = F_SHARP_2, .timeMs = 187},
        {.note = SILENCE, .timeMs = 187},
        {.note = F_SHARP_2, .timeMs = 187},
        {.note = F_4, .timeMs = 187},
        {.note = F_3, .timeMs = 187},
        {.note = SILENCE, .timeMs = 187},
        {.note = F_2, .timeMs = 187},
        {.note = F_3, .timeMs = 187},
        {.note = E_3, .timeMs = 187},
        {.note = A_4, .timeMs = 187},
        {.note = A_3, .timeMs = 187},
        {.note = SILENCE, .timeMs = 187},
        {.note = A_2, .timeMs = 187},
        {.note = SILENCE, .timeMs = 187},
        {.note = A_2, .timeMs = 187},
        {.note = G_4, .timeMs = 187},
        {.note = G_3, .timeMs = 187},
        {.note = SILENCE, .timeMs = 187},
        {.note = G_2, .timeMs = 187},
        {.note = SILENCE, .timeMs = 187},
        {.note = G_2, .timeMs = 187},
        {.note = F_SHARP_4, .timeMs = 187},
        {.note = F_SHARP_3, .timeMs = 187},
        {.note = SILENCE, .timeMs = 187},
        {.note = F_SHARP_2, .timeMs = 187},
        {.note = SILENCE, .timeMs = 187},
        {.note = F_SHARP_2, .timeMs = 187},
        {.note = F_4, .timeMs = 187},
        {.note = F_3, .timeMs = 187},
        {.note = SILENCE, .timeMs = 187},
        {.note = F_2, .timeMs = 187},
        {.note = F_3, .timeMs = 187},
        {.note = E_3, .timeMs = 187},
        {.note = A_2, .timeMs = 187},
        {.note = A_3, .timeMs = 187},
        {.note = SILENCE, .timeMs = 187},
        {.note = A_2, .timeMs = 218},
        {.note = SILENCE, .timeMs = 156},
        {.note = A_2, .timeMs = 187},
        {.note = B_2, .timeMs = 187},
        {.note = B_3, .timeMs = 187},
        {.note = SILENCE, .timeMs = 187},
        {.note = B_2, .timeMs = 218},
        {.note = SILENCE, .timeMs = 156},
        {.note = B_2, .timeMs = 187},
        {.note = G_2, .timeMs = 187},
        {.note = G_3, .timeMs = 187},
        {.note = SILENCE, .timeMs = 187},
        {.note = G_2, .timeMs = 218},
        {.note = SILENCE, .timeMs = 156},
        {.note = G_2, .timeMs = 187},
        {.note = F_2, .timeMs = 187},
        {.note = F_3, .timeMs = 187},
        {.note = SILENCE, .timeMs = 187},
        {.note = F_2, .timeMs = 187},
        {.note = F_3, .timeMs = 187},
        {.note = E_3, .timeMs = 187},
        {.note = A_2, .timeMs = 187},
        {.note = A_3, .timeMs = 187},
        {.note = SILENCE, .timeMs = 187},
        {.note = A_2, .timeMs = 218},
        {.note = SILENCE, .timeMs = 156},
        {.note = A_2, .timeMs = 187},
        {.note = B_2, .timeMs = 187},
        {.note = B_3, .timeMs = 187},
        {.note = SILENCE, .timeMs = 187},
        {.note = B_2, .timeMs = 218},
        {.note = SILENCE, .timeMs = 156},
        {.note = B_2, .timeMs = 187},
        {.note = G_2, .timeMs = 187},
        {.note = G_3, .timeMs = 187},
        {.note = SILENCE, .timeMs = 187},
        {.note = G_2, .timeMs = 218},
        {.note = SILENCE, .timeMs = 156},
        {.note = G_2, .timeMs = 187},
        {.note = F_2, .timeMs = 187},
        {.note = F_3, .timeMs = 187},
        {.note = SILENCE, .timeMs = 187},
        {.note = F_2, .timeMs = 187},
        {.note = F_3, .timeMs = 187},
        {.note = E_3, .timeMs = 187},
        {.note = A_3, .timeMs = 187},
        {.note = SILENCE, .timeMs = 187},
        {.note = D_SHARP_4, .timeMs = 187},
        {.note = D_4, .timeMs = 187},
        {.note = SILENCE, .timeMs = 187},
        {.note = C_4, .timeMs = 187},
        {.note = SILENCE, .timeMs = 187},
        {.note = A_3, .timeMs = 281},
        {.note = SILENCE, .timeMs = 93},
        {.note = E_3, .timeMs = 187},
        {.note = G_3, .timeMs = 187},
        {.note = A_3, .timeMs = 281},
        {.note = SILENCE, .timeMs = 93},
        {.note = A_3, .timeMs = 187},
        {.note = B_3, .timeMs = 187},
        {.note = C_4, .timeMs = 187},
        {.note = D_4, .timeMs = 140},
        {.note = SILENCE, .timeMs = 46},
        {.note = D_4, .timeMs = 140},
        {.note = SILENCE, .timeMs = 46},
        {.note = D_4, .timeMs = 140},
        {.note = SILENCE, .timeMs = 46},
        {.note = D_4, .timeMs = 140},
        {.note = SILENCE, .timeMs = 46},
        {.note = D_4, .timeMs = 187},
        {.note = C_4, .timeMs = 187},
        {.note = A_3, .timeMs = 187},
        {.note = B_3, .timeMs = 890},
        {.note = SILENCE, .timeMs = 234},
        {.note = B_3, .timeMs = 187},
        {.note = C_4, .timeMs = 187},
        {.note = D_4, .timeMs = 187},
        {.note = E_4, .timeMs = 140},
        {.note = SILENCE, .timeMs = 46},
        {.note = E_4, .timeMs = 140},
        {.note = SILENCE, .timeMs = 46},
        {.note = E_4, .timeMs = 140},
        {.note = SILENCE, .timeMs = 46},
        {.note = E_4, .timeMs = 140},
        {.note = SILENCE, .timeMs = 46},
        {.note = D_SHARP_4, .timeMs = 187},
        {.note = D_4, .timeMs = 187},
        {.note = C_4, .timeMs = 187},
        {.note = D_4, .timeMs = 187},
        {.note = SILENCE, .timeMs = 187},
        {.note = C_4, .timeMs = 234},
        {.note = SILENCE, .timeMs = 140},
        {.note = A_3, .timeMs = 234},
        {.note = SILENCE, .timeMs = 140},
        {.note = E_3, .timeMs = 187},
        {.note = G_3, .timeMs = 187},
        {.note = A_3, .timeMs = 140},
        {.note = SILENCE, .timeMs = 46},
        {.note = C_4, .timeMs = 140},
        {.note = SILENCE, .timeMs = 46},
        {.note = C_4, .timeMs = 140},
        {.note = SILENCE, .timeMs = 46},
        {.note = C_4, .timeMs = 140},
        {.note = SILENCE, .timeMs = 46},
        {.note = C_4, .timeMs = 140},
        {.note = SILENCE, .timeMs = 46},
        {.note = B_3, .timeMs = 187},
        {.note = A_3, .timeMs = 187},
        {.note = G_3, .timeMs = 187},
        {.note = A_3, .timeMs = 890},
        {.note = SILENCE, .timeMs = 234},
        {.note = A_3, .timeMs = 140},
        {.note = SILENCE, .timeMs = 46},
        {.note = C_4, .timeMs = 187},
        {.note = D_4, .timeMs = 187},
        {.note = D_SHARP_4, .timeMs = 140},
        {.note = SILENCE, .timeMs = 46},
        {.note = D_SHARP_4, .timeMs = 140},
        {.note = SILENCE, .timeMs = 46},
        {.note = D_SHARP_4, .timeMs = 140},
        {.note = SILENCE, .timeMs = 46},
        {.note = D_SHARP_4, .timeMs = 140},
        {.note = SILENCE, .timeMs = 46},
        {.note = D_4, .timeMs = 140},
        {.note = SILENCE, .timeMs = 46},
        {.note = C_4, .timeMs = 140},
        {.note = SILENCE, .timeMs = 46},
        {.note = D_4, .timeMs = 140},
        {.note = SILENCE, .timeMs = 46},
        {.note = D_SHARP_4, .timeMs = 187},
        {.note = A_2, .timeMs = 187},
        {.note = D_4, .timeMs = 23},
        {.note = D_SHARP_4, .timeMs = 164},
        {.note = D_4, .timeMs = 23},
        {.note = D_SHARP_4, .timeMs = 164},
        {.note = A_2, .timeMs = 187},
        {.note = D_3, .timeMs = 187},
        {.note = E_3, .timeMs = 187},
        {.note = G_2, .timeMs = 187},
        {.note = F_SHARP_3, .timeMs = 23},
        {.note = G_3, .timeMs = 164},
        {.note = F_SHARP_3, .timeMs = 23},
        {.note = G_3, .timeMs = 164},
        {.note = G_2, .timeMs = 187},
        {.note = D_SHARP_3, .timeMs = 187},
        {.note = D_3, .timeMs = 187},
        {.note = F_SHARP_2, .timeMs = 187},
        {.note = F_3, .timeMs = 23},
        {.note = F_SHARP_3, .timeMs = 164},
        {.note = F_3, .timeMs = 23},
        {.note = F_SHARP_3, .timeMs = 164},
        {.note = F_SHARP_2, .timeMs = 187},
        {.note = D_3, .timeMs = 187},
        {.note = B_2, .timeMs = 187},
        {.note = F_2, .timeMs = 187},
        {.note = E_3, .timeMs = 23},
        {.note = F_3, .timeMs = 164},
        {.note = E_3, .timeMs = 23},
        {.note = F_3, .timeMs = 164},
        {.note = F_2, .timeMs = 187},
        {.note = F_3, .timeMs = 187},
        {.note = E_3, .timeMs = 187},
        {.note = A_2, .timeMs = 187},
        {.note = D_4, .timeMs = 23},
        {.note = D_SHARP_4, .timeMs = 164},
        {.note = D_4, .timeMs = 23},
        {.note = D_SHARP_4, .timeMs = 164},
        {.note = A_2, .timeMs = 187},
        {.note = D_3, .timeMs = 187},
        {.note = E_3, .timeMs = 187},
        {.note = G_2, .timeMs = 187},
        {.note = F_SHARP_3, .timeMs = 23},
        {.note = G_3, .timeMs = 164},
        {.note = F_SHARP_3, .timeMs = 23},
        {.note = G_3, .timeMs = 164},
        {.note = G_2, .timeMs = 187},
        {.note = D_SHARP_3, .timeMs = 187},
        {.note = D_3, .timeMs = 187},
        {.note = F_SHARP_2, .timeMs = 187},
        {.note = F_3, .timeMs = 23},
        {.note = F_SHARP_3, .timeMs = 164},
        {.note = F_3, .timeMs = 23},
        {.note = F_SHARP_3, .timeMs = 164},
        {.note = F_SHARP_2, .timeMs = 187},
        {.note = D_3, .timeMs = 187},
        {.note = B_2, .timeMs = 187},
        {.note = F_2, .timeMs = 187},
        {.note = E_3, .timeMs = 23},
        {.note = F_3, .timeMs = 164},
        {.note = E_3, .timeMs = 23},
        {.note = F_3, .timeMs = 164},
        {.note = F_2, .timeMs = 187},
        {.note = F_3, .timeMs = 187},
        {.note = E_3, .timeMs = 183},
        {.note = SILENCE, .timeMs = 187},

    },
    .numNotes = 489,
    .shouldLoop = true
};

static const song_t fauxrio_kart =
{
    .notes =
    {
        {.note = F_4, .timeMs = 111},
        {.note = SILENCE, .timeMs = 111},
        {.note = C_3, .timeMs = 55},
        {.note = SILENCE, .timeMs = 167},
        {.note = E_4, .timeMs = 111},
        {.note = SILENCE, .timeMs = 111},
        {.note = C_3, .timeMs = 55},
        {.note = SILENCE, .timeMs = 167},
        {.note = A_SHARP_3, .timeMs = 55},
        {.note = SILENCE, .timeMs = 167},
        {.note = C_4, .timeMs = 55},
        {.note = SILENCE, .timeMs = 55},
        {.note = G_3, .timeMs = 111},
        {.note = A_SHARP_2, .timeMs = 55},
        {.note = SILENCE, .timeMs = 55},
        {.note = C_3, .timeMs = 55},
        {.note = SILENCE, .timeMs = 55},
        {.note = F_4, .timeMs = 223},
        {.note = C_3, .timeMs = 55},
        {.note = SILENCE, .timeMs = 167},
        {.note = E_4, .timeMs = 55},
        {.note = SILENCE, .timeMs = 55},
        {.note = C_3, .timeMs = 55},
        {.note = SILENCE, .timeMs = 55},
        {.note = A_SHARP_3, .timeMs = 55},
        {.note = SILENCE, .timeMs = 167},
        {.note = C_4, .timeMs = 55},
        {.note = SILENCE, .timeMs = 55},
        {.note = G_3, .timeMs = 223},
        {.note = G_2, .timeMs = 55},
        {.note = SILENCE, .timeMs = 55},
        {.note = F_2, .timeMs = 111},
        {.note = SILENCE, .timeMs = 111},
        {.note = G_2, .timeMs = 55},
        {.note = SILENCE, .timeMs = 167},
        {.note = A_SHARP_2, .timeMs = 55},
        {.note = SILENCE, .timeMs = 167},
        {.note = F_4, .timeMs = 111},
        {.note = SILENCE, .timeMs = 111},
        {.note = D_SHARP_3, .timeMs = 55},
        {.note = SILENCE, .timeMs = 167},
        {.note = D_SHARP_4, .timeMs = 111},
        {.note = SILENCE, .timeMs = 111},
        {.note = D_SHARP_3, .timeMs = 55},
        {.note = SILENCE, .timeMs = 167},
        {.note = A_SHARP_3, .timeMs = 55},
        {.note = SILENCE, .timeMs = 167},
        {.note = C_4, .timeMs = 55},
        {.note = SILENCE, .timeMs = 55},
        {.note = G_3, .timeMs = 111},
        {.note = C_SHARP_3, .timeMs = 55},
        {.note = SILENCE, .timeMs = 55},
        {.note = D_SHARP_3, .timeMs = 55},
        {.note = SILENCE, .timeMs = 55},
        {.note = F_4, .timeMs = 223},
        {.note = D_SHARP_3, .timeMs = 55},
        {.note = SILENCE, .timeMs = 167},
        {.note = C_4, .timeMs = 111},
        {.note = D_SHARP_3, .timeMs = 55},
        {.note = SILENCE, .timeMs = 55},
        {.note = G_4, .timeMs = 446},
        {.note = G_1, .timeMs = 55},
        {.note = B_1, .timeMs = 55},
        {.note = D_2, .timeMs = 55},
        {.note = F_2, .timeMs = 55},
        {.note = G_2, .timeMs = 55},
        {.note = B_2, .timeMs = 55},
        {.note = D_3, .timeMs = 55},
        {.note = G_3, .timeMs = 55},
        {.note = D_3, .timeMs = 55},
        {.note = B_2, .timeMs = 55},
        {.note = G_2, .timeMs = 55},
        {.note = F_2, .timeMs = 55},
        {.note = D_2, .timeMs = 55},
        {.note = B_1, .timeMs = 55},
        {.note = G_1, .timeMs = 55},
        {.note = SILENCE, .timeMs = 55},
        {.note = F_4, .timeMs = 111},
        {.note = SILENCE, .timeMs = 111},
        {.note = C_3, .timeMs = 55},
        {.note = SILENCE, .timeMs = 167},
        {.note = E_4, .timeMs = 111},
        {.note = SILENCE, .timeMs = 111},
        {.note = C_3, .timeMs = 55},
        {.note = SILENCE, .timeMs = 167},
        {.note = A_SHARP_3, .timeMs = 55},
        {.note = SILENCE, .timeMs = 167},
        {.note = C_4, .timeMs = 55},
        {.note = SILENCE, .timeMs = 55},
        {.note = G_3, .timeMs = 111},
        {.note = A_SHARP_2, .timeMs = 55},
        {.note = SILENCE, .timeMs = 55},
        {.note = C_3, .timeMs = 55},
        {.note = SILENCE, .timeMs = 55},
        {.note = F_4, .timeMs = 223},
        {.note = C_3, .timeMs = 55},
        {.note = SILENCE, .timeMs = 167},
        {.note = E_4, .timeMs = 55},
        {.note = SILENCE, .timeMs = 55},
        {.note = C_3, .timeMs = 55},
        {.note = SILENCE, .timeMs = 55},
        {.note = A_SHARP_3, .timeMs = 111},
        {.note = SILENCE, .timeMs = 111},
        {.note = C_4, .timeMs = 60},
        {.note = SILENCE, .timeMs = 51},
        {.note = G_3, .timeMs = 223},
        {.note = C_3, .timeMs = 55},
        {.note = SILENCE, .timeMs = 55},
        {.note = A_SHARP_2, .timeMs = 55},
        {.note = SILENCE, .timeMs = 167},
        {.note = C_3, .timeMs = 55},
        {.note = SILENCE, .timeMs = 167},
        {.note = G_3, .timeMs = 111},
        {.note = SILENCE, .timeMs = 111},
        {.note = G_SHARP_3, .timeMs = 111},
        {.note = C_3, .timeMs = 55},
        {.note = SILENCE, .timeMs = 167},
        {.note = G_SHARP_2, .timeMs = 55},
        {.note = SILENCE, .timeMs = 167},
        {.note = C_3, .timeMs = 55},
        {.note = SILENCE, .timeMs = 55},
        {.note = G_SHARP_3, .timeMs = 111},
        {.note = SILENCE, .timeMs = 111},
        {.note = A_SHARP_3, .timeMs = 111},
        {.note = D_3, .timeMs = 55},
        {.note = SILENCE, .timeMs = 167},
        {.note = A_SHARP_2, .timeMs = 55},
        {.note = SILENCE, .timeMs = 167},
        {.note = D_3, .timeMs = 55},
        {.note = SILENCE, .timeMs = 55},
        {.note = A_SHARP_3, .timeMs = 111},
        {.note = SILENCE, .timeMs = 111},
        {.note = G_SHARP_3, .timeMs = 111},
        {.note = G_SHARP_2, .timeMs = 55},
        {.note = SILENCE, .timeMs = 55},
        {.note = F_2, .timeMs = 55},
        {.note = SILENCE, .timeMs = 55},
        {.note = G_SHARP_2, .timeMs = 55},
        {.note = SILENCE, .timeMs = 55},
        {.note = C_3, .timeMs = 55},
        {.note = SILENCE, .timeMs = 55},
        {.note = D_SHARP_3, .timeMs = 55},
        {.note = SILENCE, .timeMs = 55},
        {.note = G_SHARP_3, .timeMs = 167},
        {.note = SILENCE, .timeMs = 55},
        {.note = A_SHARP_3, .timeMs = 111},
        {.note = B_2, .timeMs = 55},
        {.note = SILENCE, .timeMs = 55},
        {.note = D_3, .timeMs = 55},
        {.note = SILENCE, .timeMs = 55},
        {.note = F_3, .timeMs = 55},
        {.note = SILENCE, .timeMs = 55},
        {.note = B_3, .timeMs = 111},
        {.note = SILENCE, .timeMs = 334},
        {.note = F_5, .timeMs = 111},
        {.note = SILENCE, .timeMs = 111},
        {.note = C_3, .timeMs = 55},
        {.note = SILENCE, .timeMs = 55},
        {.note = C_3, .timeMs = 55},
        {.note = SILENCE, .timeMs = 55},
        {.note = E_5, .timeMs = 111},
        {.note = SILENCE, .timeMs = 111},
        {.note = C_3, .timeMs = 55},
        {.note = SILENCE, .timeMs = 55},
        {.note = C_3, .timeMs = 55},
        {.note = SILENCE, .timeMs = 55},
        {.note = A_SHARP_4, .timeMs = 55},
        {.note = SILENCE, .timeMs = 167},
        {.note = C_5, .timeMs = 55},
        {.note = SILENCE, .timeMs = 55},
        {.note = G_4, .timeMs = 111},
        {.note = A_SHARP_2, .timeMs = 55},
        {.note = SILENCE, .timeMs = 55},
        {.note = C_3, .timeMs = 55},
        {.note = SILENCE, .timeMs = 55},
        {.note = E_5, .timeMs = 55},
        {.note = F_5, .timeMs = 167},
        {.note = C_3, .timeMs = 55},
        {.note = SILENCE, .timeMs = 55},
        {.note = C_3, .timeMs = 55},
        {.note = SILENCE, .timeMs = 55},
        {.note = E_5, .timeMs = 55},
        {.note = SILENCE, .timeMs = 55},
        {.note = C_3, .timeMs = 55},
        {.note = SILENCE, .timeMs = 55},
        {.note = A_SHARP_4, .timeMs = 55},
        {.note = SILENCE, .timeMs = 55},
        {.note = C_3, .timeMs = 55},
        {.note = SILENCE, .timeMs = 55},
        {.note = C_5, .timeMs = 55},
        {.note = SILENCE, .timeMs = 55},
        {.note = G_4, .timeMs = 223},
        {.note = C_3, .timeMs = 55},
        {.note = SILENCE, .timeMs = 55},
        {.note = A_SHARP_2, .timeMs = 111},
        {.note = SILENCE, .timeMs = 55},
        {.note = B_2, .timeMs = 55},
        {.note = C_3, .timeMs = 111},
        {.note = SILENCE, .timeMs = 111},
        {.note = D_SHARP_3, .timeMs = 55},
        {.note = SILENCE, .timeMs = 55},
        {.note = E_3, .timeMs = 55},
        {.note = SILENCE, .timeMs = 55},
        {.note = F_5, .timeMs = 111},
        {.note = SILENCE, .timeMs = 111},
        {.note = C_SHARP_3, .timeMs = 55},
        {.note = SILENCE, .timeMs = 55},
        {.note = D_SHARP_3, .timeMs = 55},
        {.note = SILENCE, .timeMs = 55},
        {.note = D_SHARP_5, .timeMs = 111},
        {.note = D_SHARP_3, .timeMs = 55},
        {.note = SILENCE, .timeMs = 55},
        {.note = D_SHARP_3, .timeMs = 55},
        {.note = SILENCE, .timeMs = 167},
        {.note = A_SHARP_4, .timeMs = 55},
        {.note = SILENCE, .timeMs = 55},
        {.note = D_SHARP_3, .timeMs = 55},
        {.note = SILENCE, .timeMs = 55},
        {.note = C_5, .timeMs = 55},
        {.note = SILENCE, .timeMs = 55},
        {.note = G_4, .timeMs = 111},
        {.note = C_SHARP_3, .timeMs = 55},
        {.note = SILENCE, .timeMs = 55},
        {.note = D_SHARP_3, .timeMs = 55},
        {.note = SILENCE, .timeMs = 55},
        {.note = F_5, .timeMs = 223},
        {.note = D_SHARP_3, .timeMs = 55},
        {.note = SILENCE, .timeMs = 55},
        {.note = F_3, .timeMs = 55},
        {.note = SILENCE, .timeMs = 55},
        {.note = C_5, .timeMs = 111},
        {.note = G_3, .timeMs = 55},
        {.note = F_SHARP_5, .timeMs = 55},
        {.note = G_5, .timeMs = 446},
        {.note = G_3, .timeMs = 55},
        {.note = D_3, .timeMs = 55},
        {.note = G_2, .timeMs = 55},
        {.note = SILENCE, .timeMs = 55},
        {.note = B_2, .timeMs = 55},
        {.note = D_3, .timeMs = 55},
        {.note = G_2, .timeMs = 55},
        {.note = SILENCE, .timeMs = 55},
        {.note = D_3, .timeMs = 55},
        {.note = SILENCE, .timeMs = 55},
        {.note = G_2, .timeMs = 55},
        {.note = D_3, .timeMs = 55},
        {.note = B_2, .timeMs = 55},
        {.note = SILENCE, .timeMs = 55},
        {.note = G_2, .timeMs = 55},
        {.note = D_3, .timeMs = 55},
        {.note = C_3, .timeMs = 111},
        {.note = SILENCE, .timeMs = 111},
        {.note = A_5, .timeMs = 111},
        {.note = C_3, .timeMs = 55},
        {.note = SILENCE, .timeMs = 55},
        {.note = G_5, .timeMs = 55},
        {.note = SILENCE, .timeMs = 167},
        {.note = A_SHARP_2, .timeMs = 55},
        {.note = SILENCE, .timeMs = 55},
        {.note = C_3, .timeMs = 55},
        {.note = SILENCE, .timeMs = 55},
        {.note = E_5, .timeMs = 55},
        {.note = SILENCE, .timeMs = 167},
        {.note = F_5, .timeMs = 55},
        {.note = SILENCE, .timeMs = 55},
        {.note = C_5, .timeMs = 111},
        {.note = A_SHARP_2, .timeMs = 55},
        {.note = SILENCE, .timeMs = 55},
        {.note = C_3, .timeMs = 55},
        {.note = G_5, .timeMs = 55},
        {.note = G_SHARP_5, .timeMs = 223},
        {.note = F_3, .timeMs = 55},
        {.note = SILENCE, .timeMs = 55},
        {.note = F_3, .timeMs = 55},
        {.note = SILENCE, .timeMs = 55},
        {.note = G_5, .timeMs = 55},
        {.note = SILENCE, .timeMs = 55},
        {.note = F_3, .timeMs = 55},
        {.note = SILENCE, .timeMs = 55},
        {.note = D_SHARP_5, .timeMs = 55},
        {.note = SILENCE, .timeMs = 55},
        {.note = G_3, .timeMs = 55},
        {.note = SILENCE, .timeMs = 55},
        {.note = F_5, .timeMs = 55},
        {.note = SILENCE, .timeMs = 55},
        {.note = A_SHARP_4, .timeMs = 223},
        {.note = G_3, .timeMs = 55},
        {.note = SILENCE, .timeMs = 167},
        {.note = F_3, .timeMs = 55},
        {.note = SILENCE, .timeMs = 55},
        {.note = C_5, .timeMs = 55},
        {.note = SILENCE, .timeMs = 55},
        {.note = G_3, .timeMs = 55},
        {.note = SILENCE, .timeMs = 55},
        {.note = D_SHARP_5, .timeMs = 446},
        {.note = G_SHARP_2, .timeMs = 55},
        {.note = SILENCE, .timeMs = 55},
        {.note = G_SHARP_2, .timeMs = 55},
        {.note = SILENCE, .timeMs = 167},
        {.note = G_SHARP_2, .timeMs = 55},
        {.note = SILENCE, .timeMs = 167},
        {.note = G_SHARP_2, .timeMs = 55},
        {.note = SILENCE, .timeMs = 55},
        {.note = A_SHARP_2, .timeMs = 55},
        {.note = SILENCE, .timeMs = 55},
        {.note = A_SHARP_2, .timeMs = 55},
        {.note = SILENCE, .timeMs = 167},
        {.note = A_SHARP_2, .timeMs = 55},
        {.note = SILENCE, .timeMs = 167},
        {.note = A_SHARP_2, .timeMs = 55},
        {.note = SILENCE, .timeMs = 55},
        {.note = G_5, .timeMs = 111},
        {.note = F_5, .timeMs = 111},
        {.note = E_5, .timeMs = 111},
        {.note = F_3, .timeMs = 55},
        {.note = SILENCE, .timeMs = 55},
        {.note = F_3, .timeMs = 55},
        {.note = SILENCE, .timeMs = 55},
        {.note = G_5, .timeMs = 111},
        {.note = F_3, .timeMs = 55},
        {.note = SILENCE, .timeMs = 55},
        {.note = F_3, .timeMs = 55},
        {.note = SILENCE, .timeMs = 55},
        {.note = F_5, .timeMs = 111},
        {.note = G_3, .timeMs = 55},
        {.note = SILENCE, .timeMs = 55},
        {.note = E_5, .timeMs = 111},
        {.note = G_3, .timeMs = 55},
        {.note = SILENCE, .timeMs = 167},
        {.note = G_3, .timeMs = 55},
        {.note = SILENCE, .timeMs = 55},
        {.note = C_5, .timeMs = 111},
        {.note = G_3, .timeMs = 55},
        {.note = SILENCE, .timeMs = 167},
        {.note = F_SHARP_3, .timeMs = 55},
        {.note = SILENCE, .timeMs = 55},
        {.note = F_3, .timeMs = 111},
        {.note = SILENCE, .timeMs = 111},
        {.note = F_3, .timeMs = 55},
        {.note = SILENCE, .timeMs = 55},
        {.note = C_4, .timeMs = 55},
        {.note = SILENCE, .timeMs = 55},
        {.note = D_SHARP_5, .timeMs = 111},
        {.note = F_3, .timeMs = 55},
        {.note = SILENCE, .timeMs = 167},
        {.note = F_3, .timeMs = 55},
        {.note = SILENCE, .timeMs = 55},
        {.note = D_5, .timeMs = 111},
        {.note = F_3, .timeMs = 55},
        {.note = SILENCE, .timeMs = 167},
        {.note = F_3, .timeMs = 55},
        {.note = SILENCE, .timeMs = 55},
        {.note = C_5, .timeMs = 111},
        {.note = F_3, .timeMs = 55},
        {.note = SILENCE, .timeMs = 167},
        {.note = A_SHARP_4, .timeMs = 111},
        {.note = F_3, .timeMs = 55},
        {.note = SILENCE, .timeMs = 55},
        {.note = F_3, .timeMs = 55},
        {.note = SILENCE, .timeMs = 55},
        {.note = C_5, .timeMs = 111},
        {.note = F_3, .timeMs = 55},
        {.note = SILENCE, .timeMs = 55},
        {.note = A_SHARP_4, .timeMs = 111},
        {.note = F_3, .timeMs = 55},
        {.note = SILENCE, .timeMs = 55},
        {.note = C_5, .timeMs = 111},
        {.note = A_4, .timeMs = 334},
        {.note = F_3, .timeMs = 111},
        {.note = G_SHARP_3, .timeMs = 111},
        {.note = A_SHARP_3, .timeMs = 111},
        {.note = C_4, .timeMs = 111},
        {.note = A_4, .timeMs = 111},
        {.note = A_SHARP_4, .timeMs = 111},
        {.note = C_5, .timeMs = 111},
        {.note = A_4, .timeMs = 111},
        {.note = F_4, .timeMs = 111},
        {.note = D_SHARP_5, .timeMs = 111},
        {.note = C_5, .timeMs = 111},
        {.note = A_4, .timeMs = 111},
        {.note = G_5, .timeMs = 223},
        {.note = F_3, .timeMs = 111},
        {.note = F_5, .timeMs = 111},
        {.note = D_SHARP_5, .timeMs = 111},
        {.note = D_5, .timeMs = 111},
        {.note = D_SHARP_5, .timeMs = 111},
        {.note = D_5, .timeMs = 111},
        {.note = C_5, .timeMs = 111},
        {.note = A_SHARP_4, .timeMs = 111},
        {.note = C_5, .timeMs = 111},
        {.note = F_3, .timeMs = 55},
        {.note = SILENCE, .timeMs = 55},
        {.note = C_5, .timeMs = 111},
        {.note = A_SHARP_4, .timeMs = 111},
        {.note = SILENCE, .timeMs = 111},
        {.note = C_5, .timeMs = 111},
        {.note = F_3, .timeMs = 111},
        {.note = A_4, .timeMs = 55},
        {.note = A_SHARP_3, .timeMs = 55},
        {.note = C_4, .timeMs = 111},
        {.note = A_SHARP_3, .timeMs = 111},
        {.note = G_SHARP_3, .timeMs = 111},
        {.note = A_SHARP_3, .timeMs = 223},
        {.note = G_SHARP_3, .timeMs = 111},
        {.note = G_SHARP_4, .timeMs = 111},
        {.note = A_4, .timeMs = 111},
        {.note = A_SHARP_4, .timeMs = 223},
        {.note = A_SHARP_2, .timeMs = 223},
        {.note = G_SHARP_5, .timeMs = 111},
        {.note = SILENCE, .timeMs = 111},
        {.note = A_SHARP_2, .timeMs = 223},
        {.note = G_5, .timeMs = 111},
        {.note = SILENCE, .timeMs = 111},
        {.note = G_SHARP_2, .timeMs = 111},
        {.note = A_SHARP_2, .timeMs = 111},
        {.note = F_5, .timeMs = 111},
        {.note = A_SHARP_2, .timeMs = 111},
        {.note = SILENCE, .timeMs = 111},
        {.note = D_SHARP_5, .timeMs = 223},
        {.note = A_SHARP_2, .timeMs = 111},
        {.note = F_5, .timeMs = 111},
        {.note = A_SHARP_2, .timeMs = 111},
        {.note = D_SHARP_5, .timeMs = 111},
        {.note = A_SHARP_2, .timeMs = 111},
        {.note = F_5, .timeMs = 55},
        {.note = SILENCE, .timeMs = 55},
        {.note = D_5, .timeMs = 223},
        {.note = A_SHARP_2, .timeMs = 111},
        {.note = C_SHARP_3, .timeMs = 111},
        {.note = D_SHARP_3, .timeMs = 111},
        {.note = C_SHARP_3, .timeMs = 111},
        {.note = A_SHARP_2, .timeMs = 111},
        {.note = D_5, .timeMs = 111},
        {.note = C_SHARP_5, .timeMs = 111},
        {.note = C_5, .timeMs = 111},
        {.note = G_SHARP_2, .timeMs = 111},
        {.note = G_SHARP_4, .timeMs = 111},
        {.note = D_SHARP_5, .timeMs = 111},
        {.note = G_SHARP_2, .timeMs = 111},
        {.note = C_5, .timeMs = 111},
        {.note = G_SHARP_5, .timeMs = 334},
        {.note = C_3, .timeMs = 111},
        {.note = D_SHARP_3, .timeMs = 111},
        {.note = G_SHARP_3, .timeMs = 111},
        {.note = D_SHARP_3, .timeMs = 111},
        {.note = C_3, .timeMs = 111},
        {.note = A_4, .timeMs = 111},
        {.note = A_SHARP_4, .timeMs = 111},
        {.note = B_4, .timeMs = 92},
        {.note = G_2, .timeMs = 111},
        {.note = SILENCE, .timeMs = 18},
        {.note = G_4, .timeMs = 111},
        {.note = D_5, .timeMs = 111},
        {.note = SILENCE, .timeMs = 111},
        {.note = G_4, .timeMs = 111},
        {.note = G_5, .timeMs = 334},
        {.note = G_2, .timeMs = 111},
        {.note = B_2, .timeMs = 111},
        {.note = D_3, .timeMs = 111},
        {.note = B_2, .timeMs = 111},
        {.note = G_2, .timeMs = 111},
        {.note = F_2, .timeMs = 111},
        {.note = D_2, .timeMs = 111},
        {.note = SILENCE, .timeMs = 111},

    },
    .numNotes = 464,
    .shouldLoop = true
};

static const song_t the_lake =
{
    .notes =
    {
        {.note = A_6, .timeMs = 193},
        {.note = D_6, .timeMs = 193},
        {.note = A_SHARP_5, .timeMs = 193},
        {.note = G_5, .timeMs = 193},
        {.note = G_6, .timeMs = 193},
        {.note = D_6, .timeMs = 193},
        {.note = A_SHARP_5, .timeMs = 193},
        {.note = G_5, .timeMs = 193},
        {.note = F_SHARP_6, .timeMs = 193},
        {.note = D_6, .timeMs = 193},
        {.note = A_SHARP_5, .timeMs = 193},
        {.note = G_5, .timeMs = 193},
        {.note = G_6, .timeMs = 193},
        {.note = D_6, .timeMs = 193},
        {.note = A_SHARP_5, .timeMs = 193},
        {.note = G_5, .timeMs = 193},
        {.note = G_6, .timeMs = 193},
        {.note = C_6, .timeMs = 193},
        {.note = A_5, .timeMs = 193},
        {.note = F_5, .timeMs = 193},
        {.note = F_6, .timeMs = 193},
        {.note = C_6, .timeMs = 193},
        {.note = A_5, .timeMs = 193},
        {.note = F_5, .timeMs = 193},
        {.note = E_6, .timeMs = 193},
        {.note = C_6, .timeMs = 193},
        {.note = A_5, .timeMs = 193},
        {.note = F_5, .timeMs = 193},
        {.note = F_6, .timeMs = 193},
        {.note = C_6, .timeMs = 193},
        {.note = A_5, .timeMs = 193},
        {.note = F_5, .timeMs = 193},
        {.note = F_6, .timeMs = 193},
        {.note = A_SHARP_5, .timeMs = 193},
        {.note = G_5, .timeMs = 193},
        {.note = E_5, .timeMs = 193},
        {.note = E_6, .timeMs = 193},
        {.note = A_SHARP_5, .timeMs = 193},
        {.note = G_5, .timeMs = 193},
        {.note = E_5, .timeMs = 193},
        {.note = D_SHARP_6, .timeMs = 193},
        {.note = A_SHARP_5, .timeMs = 193},
        {.note = G_5, .timeMs = 193},
        {.note = E_5, .timeMs = 193},
        {.note = E_6, .timeMs = 193},
        {.note = A_SHARP_5, .timeMs = 193},
        {.note = G_5, .timeMs = 193},
        {.note = E_5, .timeMs = 193},
        {.note = E_6, .timeMs = 193},
        {.note = A_5, .timeMs = 193},
        {.note = F_5, .timeMs = 193},
        {.note = D_5, .timeMs = 193},
        {.note = D_6, .timeMs = 193},
        {.note = A_5, .timeMs = 193},
        {.note = F_5, .timeMs = 193},
        {.note = D_5, .timeMs = 193},
        {.note = C_SHARP_6, .timeMs = 193},
        {.note = A_5, .timeMs = 193},
        {.note = F_5, .timeMs = 193},
        {.note = D_5, .timeMs = 193},
        {.note = D_6, .timeMs = 193},
        {.note = A_5, .timeMs = 193},
        {.note = F_5, .timeMs = 193},
        {.note = D_5, .timeMs = 193},
        {.note = A_6, .timeMs = 193},
        {.note = D_6, .timeMs = 193},
        {.note = A_SHARP_5, .timeMs = 193},
        {.note = G_5, .timeMs = 193},
        {.note = G_6, .timeMs = 193},
        {.note = D_6, .timeMs = 193},
        {.note = A_SHARP_5, .timeMs = 193},
        {.note = G_5, .timeMs = 193},
        {.note = F_SHARP_6, .timeMs = 193},
        {.note = D_6, .timeMs = 193},
        {.note = A_SHARP_5, .timeMs = 193},
        {.note = G_5, .timeMs = 193},
        {.note = G_6, .timeMs = 193},
        {.note = D_6, .timeMs = 193},
        {.note = A_SHARP_5, .timeMs = 193},
        {.note = G_5, .timeMs = 193},
        {.note = A_SHARP_6, .timeMs = 193},
        {.note = D_SHARP_6, .timeMs = 193},
        {.note = C_6, .timeMs = 193},
        {.note = A_5, .timeMs = 193},
        {.note = A_6, .timeMs = 193},
        {.note = D_SHARP_6, .timeMs = 193},
        {.note = C_6, .timeMs = 193},
        {.note = A_5, .timeMs = 193},
        {.note = G_SHARP_6, .timeMs = 193},
        {.note = D_SHARP_6, .timeMs = 193},
        {.note = C_6, .timeMs = 193},
        {.note = A_5, .timeMs = 193},
        {.note = A_6, .timeMs = 193},
        {.note = D_SHARP_6, .timeMs = 193},
        {.note = C_6, .timeMs = 193},
        {.note = A_5, .timeMs = 193},
        {.note = C_7, .timeMs = 193},
        {.note = D_6, .timeMs = 193},
        {.note = A_SHARP_5, .timeMs = 193},
        {.note = G_5, .timeMs = 193},
        {.note = A_SHARP_6, .timeMs = 193},
        {.note = D_6, .timeMs = 193},
        {.note = A_SHARP_5, .timeMs = 193},
        {.note = G_5, .timeMs = 193},
        {.note = A_6, .timeMs = 193},
        {.note = D_6, .timeMs = 193},
        {.note = A_SHARP_5, .timeMs = 193},
        {.note = G_5, .timeMs = 193},
        {.note = A_SHARP_6, .timeMs = 193},
        {.note = D_6, .timeMs = 193},
        {.note = A_SHARP_5, .timeMs = 193},
        {.note = G_5, .timeMs = 193},
        {.note = A_6, .timeMs = 193},
        {.note = A_SHARP_5, .timeMs = 193},
        {.note = G_5, .timeMs = 193},
        {.note = E_5, .timeMs = 193},
        {.note = G_6, .timeMs = 193},
        {.note = A_SHARP_5, .timeMs = 193},
        {.note = G_5, .timeMs = 193},
        {.note = E_5, .timeMs = 193},
        {.note = F_6, .timeMs = 193},
        {.note = A_SHARP_5, .timeMs = 193},
        {.note = G_5, .timeMs = 193},
        {.note = E_5, .timeMs = 193},
        {.note = E_6, .timeMs = 193},
        {.note = A_SHARP_5, .timeMs = 193},
        {.note = G_5, .timeMs = 193},
        {.note = E_5, .timeMs = 193},
		{.note = A_SHARP_2, .timeMs = 125},
        {.note = A_6, .timeMs = 125},
        {.note = A_SHARP_3, .timeMs = 125},
        {.note = D_6, .timeMs = 125},
        {.note = D_4, .timeMs = 125},
        {.note = A_SHARP_5, .timeMs = 125},
        {.note = A_4, .timeMs = 125},
        {.note = G_5, .timeMs = 125},
        {.note = SILENCE, .timeMs = 125},
        {.note = G_6, .timeMs = 125},
        {.note = SILENCE, .timeMs = 125},
        {.note = D_6, .timeMs = 125},
        {.note = SILENCE, .timeMs = 125},
        {.note = A_SHARP_5, .timeMs = 125},
        {.note = SILENCE, .timeMs = 125},
        {.note = G_5, .timeMs = 125},
        {.note = SILENCE, .timeMs = 125},
        {.note = F_SHARP_6, .timeMs = 125},
        {.note = D_4, .timeMs = 125},
        {.note = D_6, .timeMs = 125},
        {.note = A_4, .timeMs = 125},
        {.note = A_SHARP_5, .timeMs = 125},
        {.note = G_4, .timeMs = 125},
        {.note = G_5, .timeMs = 125},
        {.note = C_5, .timeMs = 125},
        {.note = G_6, .timeMs = 125},
        {.note = A_4, .timeMs = 125},
        {.note = D_6, .timeMs = 125},
        {.note = G_4, .timeMs = 125},
        {.note = A_SHARP_5, .timeMs = 125},
        {.note = D_4, .timeMs = 125},
        {.note = G_5, .timeMs = 125},
        {.note = A_2, .timeMs = 125},
        {.note = G_6, .timeMs = 125},
        {.note = A_3, .timeMs = 125},
        {.note = C_6, .timeMs = 125},
        {.note = C_4, .timeMs = 125},
        {.note = A_5, .timeMs = 125},
        {.note = G_4, .timeMs = 125},
        {.note = F_5, .timeMs = 125},
        {.note = SILENCE, .timeMs = 125},
        {.note = F_6, .timeMs = 125},
        {.note = SILENCE, .timeMs = 125},
        {.note = C_6, .timeMs = 125},
        {.note = SILENCE, .timeMs = 125},
        {.note = A_5, .timeMs = 125},
        {.note = SILENCE, .timeMs = 125},
        {.note = F_5, .timeMs = 125},
        {.note = A_2, .timeMs = 125},
        {.note = E_6, .timeMs = 125},
        {.note = A_3, .timeMs = 125},
        {.note = C_6, .timeMs = 125},
        {.note = G_4, .timeMs = 125},
        {.note = A_5, .timeMs = 125},
        {.note = F_4, .timeMs = 125},
        {.note = F_5, .timeMs = 125},
        {.note = C_5, .timeMs = 125},
        {.note = F_6, .timeMs = 125},
        {.note = G_4, .timeMs = 125},
        {.note = C_6, .timeMs = 125},
        {.note = F_4, .timeMs = 125},
        {.note = A_5, .timeMs = 125},
        {.note = C_4, .timeMs = 125},
        {.note = F_5, .timeMs = 125},
        {.note = G_2, .timeMs = 125},
        {.note = F_6, .timeMs = 125},
        {.note = G_3, .timeMs = 125},
        {.note = A_SHARP_5, .timeMs = 125},
        {.note = A_SHARP_3, .timeMs = 125},
        {.note = G_5, .timeMs = 125},
        {.note = F_4, .timeMs = 125},
        {.note = E_5, .timeMs = 125},
        {.note = SILENCE, .timeMs = 125},
        {.note = E_6, .timeMs = 125},
        {.note = SILENCE, .timeMs = 125},
        {.note = A_SHARP_5, .timeMs = 125},
        {.note = SILENCE, .timeMs = 125},
        {.note = G_5, .timeMs = 125},
        {.note = SILENCE, .timeMs = 125},
        {.note = E_5, .timeMs = 125},
        {.note = G_2, .timeMs = 125},
        {.note = D_SHARP_6, .timeMs = 125},
        {.note = G_3, .timeMs = 125},
        {.note = A_SHARP_5, .timeMs = 125},
        {.note = F_4, .timeMs = 125},
        {.note = G_5, .timeMs = 125},
        {.note = E_4, .timeMs = 125},
        {.note = E_5, .timeMs = 125},
        {.note = G_4, .timeMs = 125},
        {.note = E_6, .timeMs = 125},
        {.note = E_4, .timeMs = 125},
        {.note = A_SHARP_5, .timeMs = 125},
        {.note = C_4, .timeMs = 125},
        {.note = G_5, .timeMs = 125},
        {.note = A_SHARP_3, .timeMs = 125},
        {.note = E_5, .timeMs = 125},
        {.note = F_2, .timeMs = 125},
        {.note = E_6, .timeMs = 125},
        {.note = F_3, .timeMs = 125},
        {.note = A_5, .timeMs = 125},
        {.note = A_3, .timeMs = 125},
        {.note = F_5, .timeMs = 125},
        {.note = E_4, .timeMs = 125},
        {.note = D_5, .timeMs = 125},
        {.note = SILENCE, .timeMs = 125},
        {.note = D_6, .timeMs = 125},
        {.note = SILENCE, .timeMs = 125},
        {.note = A_5, .timeMs = 125},
        {.note = SILENCE, .timeMs = 125},
        {.note = F_5, .timeMs = 125},
        {.note = SILENCE, .timeMs = 125},
        {.note = D_5, .timeMs = 125},
        {.note = F_2, .timeMs = 125},
        {.note = C_SHARP_6, .timeMs = 125},
        {.note = A_3, .timeMs = 125},
        {.note = A_5, .timeMs = 125},
        {.note = E_4, .timeMs = 125},
        {.note = F_5, .timeMs = 125},
        {.note = D_4, .timeMs = 125},
        {.note = D_5, .timeMs = 125},
        {.note = G_4, .timeMs = 125},
        {.note = D_6, .timeMs = 125},
        {.note = F_4, .timeMs = 125},
        {.note = A_5, .timeMs = 125},
        {.note = C_5, .timeMs = 125},
        {.note = F_5, .timeMs = 125},
        {.note = A_SHARP_4, .timeMs = 125},
        {.note = D_5, .timeMs = 125},
        {.note = A_SHARP_2, .timeMs = 125},
        {.note = A_6, .timeMs = 125},
        {.note = A_SHARP_3, .timeMs = 125},
        {.note = D_6, .timeMs = 125},
        {.note = D_4, .timeMs = 125},
        {.note = A_SHARP_5, .timeMs = 125},
        {.note = A_4, .timeMs = 125},
        {.note = G_5, .timeMs = 125},
        {.note = SILENCE, .timeMs = 125},
        {.note = G_6, .timeMs = 125},
        {.note = SILENCE, .timeMs = 125},
        {.note = D_6, .timeMs = 125},
        {.note = SILENCE, .timeMs = 125},
        {.note = A_SHARP_5, .timeMs = 125},
        {.note = SILENCE, .timeMs = 125},
        {.note = G_5, .timeMs = 125},
        {.note = A_SHARP_2, .timeMs = 125},
        {.note = F_SHARP_6, .timeMs = 125},
        {.note = A_SHARP_3, .timeMs = 125},
        {.note = D_6, .timeMs = 125},
        {.note = A_4, .timeMs = 125},
        {.note = A_SHARP_5, .timeMs = 125},
        {.note = G_4, .timeMs = 125},
        {.note = G_5, .timeMs = 125},
        {.note = D_5, .timeMs = 125},
        {.note = G_6, .timeMs = 125},
        {.note = A_4, .timeMs = 125},
        {.note = D_6, .timeMs = 125},
        {.note = G_4, .timeMs = 125},
        {.note = A_SHARP_5, .timeMs = 125},
        {.note = D_4, .timeMs = 125},
        {.note = G_5, .timeMs = 125},
        {.note = A_2, .timeMs = 125},
        {.note = A_SHARP_6, .timeMs = 125},
        {.note = A_3, .timeMs = 125},
        {.note = D_SHARP_6, .timeMs = 125},
        {.note = C_4, .timeMs = 125},
        {.note = C_6, .timeMs = 125},
        {.note = C_5, .timeMs = 125},
        {.note = A_5, .timeMs = 125},
        {.note = SILENCE, .timeMs = 125},
        {.note = A_6, .timeMs = 125},
        {.note = SILENCE, .timeMs = 125},
        {.note = D_SHARP_6, .timeMs = 125},
        {.note = SILENCE, .timeMs = 125},
        {.note = C_6, .timeMs = 125},
        {.note = SILENCE, .timeMs = 125},
        {.note = A_5, .timeMs = 125},
        {.note = D_3, .timeMs = 125},
        {.note = G_SHARP_6, .timeMs = 125},
        {.note = D_4, .timeMs = 125},
        {.note = D_SHARP_6, .timeMs = 125},
        {.note = C_5, .timeMs = 125},
        {.note = C_6, .timeMs = 125},
        {.note = A_4, .timeMs = 125},
        {.note = A_5, .timeMs = 125},
        {.note = D_SHARP_5, .timeMs = 125},
        {.note = A_6, .timeMs = 125},
        {.note = D_5, .timeMs = 125},
        {.note = D_SHARP_6, .timeMs = 125},
        {.note = C_5, .timeMs = 125},
        {.note = C_6, .timeMs = 125},
        {.note = A_4, .timeMs = 125},
        {.note = A_5, .timeMs = 125},
        {.note = G_2, .timeMs = 125},
        {.note = C_7, .timeMs = 125},
        {.note = A_SHARP_3, .timeMs = 125},
        {.note = D_6, .timeMs = 125},
        {.note = D_4, .timeMs = 125},
        {.note = A_SHARP_5, .timeMs = 125},
        {.note = A_SHARP_4, .timeMs = 125},
        {.note = G_5, .timeMs = 125},
        {.note = SILENCE, .timeMs = 125},
        {.note = A_SHARP_6, .timeMs = 125},
        {.note = SILENCE, .timeMs = 125},
        {.note = D_6, .timeMs = 125},
        {.note = SILENCE, .timeMs = 125},
        {.note = A_SHARP_5, .timeMs = 125},
        {.note = SILENCE, .timeMs = 125},
        {.note = G_5, .timeMs = 125},
        {.note = G_2, .timeMs = 125},
        {.note = A_6, .timeMs = 125},
        {.note = A_SHARP_3, .timeMs = 125},
        {.note = D_6, .timeMs = 125},
        {.note = A_SHARP_4, .timeMs = 125},
        {.note = A_SHARP_5, .timeMs = 125},
        {.note = A_4, .timeMs = 125},
        {.note = G_5, .timeMs = 125},
        {.note = C_5, .timeMs = 125},
        {.note = A_SHARP_6, .timeMs = 125},
        {.note = A_SHARP_4, .timeMs = 125},
        {.note = D_6, .timeMs = 125},
        {.note = G_4, .timeMs = 125},
        {.note = A_SHARP_5, .timeMs = 125},
        {.note = D_4, .timeMs = 125},
        {.note = G_5, .timeMs = 125},
        {.note = C_3, .timeMs = 125},
        {.note = A_6, .timeMs = 125},
        {.note = C_4, .timeMs = 125},
        {.note = A_SHARP_5, .timeMs = 125},
        {.note = G_4, .timeMs = 125},
        {.note = G_5, .timeMs = 125},
        {.note = A_SHARP_4, .timeMs = 125},
        {.note = E_5, .timeMs = 125},
        {.note = SILENCE, .timeMs = 125},
        {.note = G_6, .timeMs = 125},
        {.note = SILENCE, .timeMs = 125},
        {.note = A_SHARP_5, .timeMs = 125},
        {.note = SILENCE, .timeMs = 125},
        {.note = G_5, .timeMs = 125},
        {.note = SILENCE, .timeMs = 125},
        {.note = E_5, .timeMs = 125},
        {.note = C_3, .timeMs = 125},
        {.note = F_6, .timeMs = 125},
        {.note = C_4, .timeMs = 125},
        {.note = A_SHARP_5, .timeMs = 125},
        {.note = A_SHARP_4, .timeMs = 125},
        {.note = G_5, .timeMs = 125},
        {.note = G_4, .timeMs = 125},
        {.note = E_5, .timeMs = 62},
        {.note = SILENCE, .timeMs = 62},
        {.note = E_5, .timeMs = 125},
        {.note = E_6, .timeMs = 125},
        {.note = D_5, .timeMs = 125},
        {.note = A_SHARP_5, .timeMs = 125},
        {.note = G_5, .timeMs = 62},
        {.note = SILENCE, .timeMs = 62},
        {.note = G_5, .timeMs = 125},
        {.note = E_5, .timeMs = 62},
        {.note = SILENCE, .timeMs = 62},
        {.note = E_5, .timeMs = 125},
        {.note = SILENCE, .timeMs = 193},

    },
    .numNotes = 388,
    .shouldLoop = true
};

static const song_t herecomesthesun =
{
    .notes =
    {
        {.note = A_2, .timeMs = 116},
        {.note = SILENCE, .timeMs = 116},
        {.note = E_3, .timeMs = 116},
        {.note = SILENCE, .timeMs = 116},
        {.note = C_SHARP_5, .timeMs = 116},
        {.note = SILENCE, .timeMs = 116},
        {.note = A_4, .timeMs = 116},
        {.note = SILENCE, .timeMs = 116},
        {.note = B_4, .timeMs = 116},
        {.note = SILENCE, .timeMs = 116},
        {.note = C_SHARP_5, .timeMs = 116},
        {.note = SILENCE, .timeMs = 116},
        {.note = A_2, .timeMs = 116},
        {.note = SILENCE, .timeMs = 116},
        {.note = A_4, .timeMs = 116},
        {.note = SILENCE, .timeMs = 116},
        {.note = A_2, .timeMs = 116},
        {.note = SILENCE, .timeMs = 116},
        {.note = E_3, .timeMs = 116},
        {.note = SILENCE, .timeMs = 116},
        {.note = C_SHARP_5, .timeMs = 116},
        {.note = SILENCE, .timeMs = 116},
        {.note = B_4, .timeMs = 116},
        {.note = SILENCE, .timeMs = 116},
        {.note = B_2, .timeMs = 116},
        {.note = SILENCE, .timeMs = 116},
        {.note = A_4, .timeMs = 116},
        {.note = SILENCE, .timeMs = 116},
        {.note = C_SHARP_3, .timeMs = 116},
        {.note = E_4, .timeMs = 116},
        {.note = F_SHARP_4, .timeMs = 116},
        {.note = SILENCE, .timeMs = 116},
        {.note = D_3, .timeMs = 116},
        {.note = SILENCE, .timeMs = 116},
        {.note = A_4, .timeMs = 116},
        {.note = SILENCE, .timeMs = 116},
        {.note = A_3, .timeMs = 116},
        {.note = SILENCE, .timeMs = 116},
        {.note = B_4, .timeMs = 116},
        {.note = SILENCE, .timeMs = 116},
        {.note = G_SHARP_3, .timeMs = 116},
        {.note = SILENCE, .timeMs = 116},
        {.note = A_4, .timeMs = 116},
        {.note = SILENCE, .timeMs = 116},
        {.note = F_SHARP_3, .timeMs = 116},
        {.note = SILENCE, .timeMs = 116},
        {.note = F_SHARP_4, .timeMs = 116},
        {.note = E_3, .timeMs = 116},
        {.note = G_SHARP_4, .timeMs = 116},
        {.note = SILENCE, .timeMs = 116},
        {.note = F_SHARP_4, .timeMs = 116},
        {.note = SILENCE, .timeMs = 116},
        {.note = G_SHARP_4, .timeMs = 116},
        {.note = SILENCE, .timeMs = 116},
        {.note = A_4, .timeMs = 116},
        {.note = SILENCE, .timeMs = 116},
        {.note = E_3, .timeMs = 116},
        {.note = SILENCE, .timeMs = 116},
        {.note = B_4, .timeMs = 116},
        {.note = SILENCE, .timeMs = 348},
        {.note = B_4, .timeMs = 116},
        {.note = SILENCE, .timeMs = 116},
        {.note = A_2, .timeMs = 116},
        {.note = SILENCE, .timeMs = 116},
        {.note = E_3, .timeMs = 116},
        {.note = SILENCE, .timeMs = 116},
        {.note = C_SHARP_5, .timeMs = 116},
        {.note = SILENCE, .timeMs = 116},
        {.note = A_4, .timeMs = 116},
        {.note = SILENCE, .timeMs = 116},
        {.note = B_4, .timeMs = 116},
        {.note = SILENCE, .timeMs = 116},
        {.note = C_SHARP_5, .timeMs = 116},
        {.note = SILENCE, .timeMs = 116},
        {.note = A_2, .timeMs = 116},
        {.note = SILENCE, .timeMs = 116},
        {.note = A_4, .timeMs = 116},
        {.note = SILENCE, .timeMs = 116},
        {.note = A_2, .timeMs = 116},
        {.note = SILENCE, .timeMs = 116},
        {.note = E_3, .timeMs = 116},
        {.note = SILENCE, .timeMs = 116},
        {.note = C_SHARP_5, .timeMs = 116},
        {.note = SILENCE, .timeMs = 116},
        {.note = B_4, .timeMs = 116},
        {.note = SILENCE, .timeMs = 116},
        {.note = B_2, .timeMs = 116},
        {.note = SILENCE, .timeMs = 116},
        {.note = A_4, .timeMs = 116},
        {.note = SILENCE, .timeMs = 116},
        {.note = C_SHARP_3, .timeMs = 116},
        {.note = E_4, .timeMs = 116},
        {.note = F_SHARP_4, .timeMs = 116},
        {.note = SILENCE, .timeMs = 116},
        {.note = D_3, .timeMs = 116},
        {.note = SILENCE, .timeMs = 116},
        {.note = C_SHARP_5, .timeMs = 116},
        {.note = SILENCE, .timeMs = 116},
        {.note = A_3, .timeMs = 116},
        {.note = SILENCE, .timeMs = 116},
        {.note = B_4, .timeMs = 116},
        {.note = SILENCE, .timeMs = 116},
        {.note = G_SHARP_3, .timeMs = 116},
        {.note = SILENCE, .timeMs = 116},
        {.note = A_4, .timeMs = 116},
        {.note = SILENCE, .timeMs = 116},
        {.note = F_SHARP_3, .timeMs = 116},
        {.note = SILENCE, .timeMs = 116},
        {.note = G_SHARP_4, .timeMs = 116},
        {.note = SILENCE, .timeMs = 116},
        {.note = E_2, .timeMs = 116},
        {.note = SILENCE, .timeMs = 116},
        {.note = B_2, .timeMs = 116},
        {.note = SILENCE, .timeMs = 116},
        {.note = E_3, .timeMs = 58},
        {.note = F_SHARP_3, .timeMs = 58},
        {.note = SILENCE, .timeMs = 116},
        {.note = E_3, .timeMs = 116},
        {.note = SILENCE, .timeMs = 348},
        {.note = E_2, .timeMs = 116},
        {.note = SILENCE, .timeMs = 116},
        {.note = E_2, .timeMs = 116},
        {.note = SILENCE, .timeMs = 116},
        {.note = E_2, .timeMs = 116},
        {.note = SILENCE, .timeMs = 116},
        {.note = A_2, .timeMs = 116},
        {.note = SILENCE, .timeMs = 116},
        {.note = E_3, .timeMs = 116},
        {.note = SILENCE, .timeMs = 116},
        {.note = C_SHARP_5, .timeMs = 116},
        {.note = SILENCE, .timeMs = 116},
        {.note = B_4, .timeMs = 116},
        {.note = SILENCE, .timeMs = 116},
        {.note = A_2, .timeMs = 116},
        {.note = SILENCE, .timeMs = 116},
        {.note = C_SHARP_5, .timeMs = 116},
        {.note = SILENCE, .timeMs = 116},
        {.note = E_3, .timeMs = 116},
        {.note = SILENCE, .timeMs = 116},
        {.note = A_4, .timeMs = 116},
        {.note = SILENCE, .timeMs = 116},
        {.note = A_2, .timeMs = 116},
        {.note = SILENCE, .timeMs = 116},
        {.note = E_3, .timeMs = 116},
        {.note = SILENCE, .timeMs = 116},
        {.note = C_SHARP_5, .timeMs = 116},
        {.note = SILENCE, .timeMs = 116},
        {.note = A_4, .timeMs = 116},
        {.note = SILENCE, .timeMs = 116},
        {.note = B_4, .timeMs = 116},
        {.note = SILENCE, .timeMs = 116},
        {.note = C_SHARP_5, .timeMs = 116},
        {.note = SILENCE, .timeMs = 116},
        {.note = B_2, .timeMs = 116},
        {.note = SILENCE, .timeMs = 116},
        {.note = C_SHARP_3, .timeMs = 116},
        {.note = SILENCE, .timeMs = 116},
        {.note = D_3, .timeMs = 116},
        {.note = SILENCE, .timeMs = 116},
        {.note = A_3, .timeMs = 116},
        {.note = SILENCE, .timeMs = 116},
        {.note = C_SHARP_5, .timeMs = 116},
        {.note = SILENCE, .timeMs = 116},
        {.note = B_4, .timeMs = 116},
        {.note = SILENCE, .timeMs = 116},
        {.note = D_3, .timeMs = 116},
        {.note = SILENCE, .timeMs = 116},
        {.note = C_SHARP_5, .timeMs = 116},
        {.note = SILENCE, .timeMs = 116},
        {.note = F_SHARP_3, .timeMs = 116},
        {.note = SILENCE, .timeMs = 116},
        {.note = A_4, .timeMs = 116},
        {.note = E_3, .timeMs = 116},
        {.note = D_SHARP_3, .timeMs = 116},
        {.note = SILENCE, .timeMs = 116},
        {.note = B_3, .timeMs = 116},
        {.note = SILENCE, .timeMs = 116},
        {.note = F_SHARP_3, .timeMs = 116},
        {.note = SILENCE, .timeMs = 116},
        {.note = A_4, .timeMs = 116},
        {.note = SILENCE, .timeMs = 116},
        {.note = B_4, .timeMs = 116},
        {.note = SILENCE, .timeMs = 116},
        {.note = B_2, .timeMs = 116},
        {.note = SILENCE, .timeMs = 116},
        {.note = A_4, .timeMs = 116},
        {.note = SILENCE, .timeMs = 116},
        {.note = F_SHARP_3, .timeMs = 116},
        {.note = SILENCE, .timeMs = 116},
        {.note = E_3, .timeMs = 116},
        {.note = SILENCE, .timeMs = 116},
        {.note = C_SHARP_5, .timeMs = 116},
        {.note = SILENCE, .timeMs = 116},
        {.note = A_3, .timeMs = 116},
        {.note = SILENCE, .timeMs = 116},
        {.note = B_4, .timeMs = 116},
        {.note = SILENCE, .timeMs = 116},
        {.note = A_2, .timeMs = 116},
        {.note = SILENCE, .timeMs = 116},
        {.note = A_4, .timeMs = 116},
        {.note = SILENCE, .timeMs = 116},
        {.note = C_SHARP_3, .timeMs = 116},
        {.note = SILENCE, .timeMs = 116},
        {.note = E_4, .timeMs = 116},
        {.note = SILENCE, .timeMs = 116},
        {.note = D_3, .timeMs = 58},
        {.note = F_SHARP_4, .timeMs = 58},
        {.note = SILENCE, .timeMs = 116},
        {.note = A_4, .timeMs = 116},
        {.note = SILENCE, .timeMs = 116},
        {.note = B_4, .timeMs = 116},
        {.note = SILENCE, .timeMs = 116},
        {.note = C_SHARP_3, .timeMs = 58},
        {.note = E_4, .timeMs = 58},
        {.note = SILENCE, .timeMs = 116},
        {.note = A_4, .timeMs = 116},
        {.note = SILENCE, .timeMs = 116},
        {.note = B_4, .timeMs = 116},
        {.note = SILENCE, .timeMs = 116},
        {.note = A_2, .timeMs = 58},
        {.note = D_4, .timeMs = 58},
        {.note = SILENCE, .timeMs = 116},
        {.note = A_4, .timeMs = 116},
        {.note = SILENCE, .timeMs = 58},
        {.note = D_3, .timeMs = 58},
        {.note = B_4, .timeMs = 116},
        {.note = SILENCE, .timeMs = 116},
        {.note = E_3, .timeMs = 58},
        {.note = E_4, .timeMs = 58},
        {.note = SILENCE, .timeMs = 116},
        {.note = A_4, .timeMs = 116},
        {.note = SILENCE, .timeMs = 116},
        {.note = B_4, .timeMs = 116},
        {.note = SILENCE, .timeMs = 116},
        {.note = E_3, .timeMs = 58},
        {.note = A_4, .timeMs = 58},
        {.note = SILENCE, .timeMs = 116},
        {.note = G_SHARP_4, .timeMs = 116},
        {.note = SILENCE, .timeMs = 116},
        {.note = E_2, .timeMs = 58},
        {.note = F_SHARP_4, .timeMs = 58},
        {.note = SILENCE, .timeMs = 116},
        {.note = E_4, .timeMs = 116},
        {.note = SILENCE, .timeMs = 116},

    },
    .numNotes = 244,
    .shouldLoop = true
};

static const song_t bananaphone =
{
    .notes =
    {
        {.note = A_4, .timeMs = 157},
        {.note = SILENCE, .timeMs = 157},
        {.note = A_4, .timeMs = 19},
        {.note = C_SHARP_5, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = A_4, .timeMs = 19},
        {.note = C_SHARP_5, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = SILENCE, .timeMs = 39},
        {.note = B_4, .timeMs = 19},
        {.note = D_5, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = B_4, .timeMs = 19},
        {.note = D_5, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = SILENCE, .timeMs = 39},
        {.note = C_SHARP_5, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = A_5, .timeMs = 19},
        {.note = C_SHARP_5, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = A_5, .timeMs = 19},
        {.note = SILENCE, .timeMs = 39},
        {.note = B_4, .timeMs = 19},
        {.note = D_5, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = B_4, .timeMs = 19},
        {.note = D_5, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = SILENCE, .timeMs = 39},
        {.note = A_4, .timeMs = 19},
        {.note = C_SHARP_5, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = A_4, .timeMs = 19},
        {.note = C_SHARP_5, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = SILENCE, .timeMs = 197},
        {.note = E_4, .timeMs = 19},
        {.note = G_SHARP_4, .timeMs = 19},
        {.note = B_4, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = E_4, .timeMs = 19},
        {.note = G_SHARP_4, .timeMs = 19},
        {.note = B_4, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = E_4, .timeMs = 19},
        {.note = G_SHARP_4, .timeMs = 19},
        {.note = B_4, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = E_4, .timeMs = 19},
        {.note = G_SHARP_4, .timeMs = 19},
        {.note = B_4, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = SILENCE, .timeMs = 157},
        {.note = E_4, .timeMs = 19},
        {.note = G_SHARP_4, .timeMs = 19},
        {.note = B_4, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = E_4, .timeMs = 19},
        {.note = G_SHARP_4, .timeMs = 19},
        {.note = B_4, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = E_4, .timeMs = 19},
        {.note = G_SHARP_4, .timeMs = 19},
        {.note = B_4, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = E_4, .timeMs = 19},
        {.note = G_SHARP_4, .timeMs = 19},
        {.note = B_4, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = E_4, .timeMs = 19},
        {.note = G_SHARP_4, .timeMs = 19},
        {.note = B_4, .timeMs = 19},
        {.note = D_5, .timeMs = 19},
        {.note = E_4, .timeMs = 19},
        {.note = G_SHARP_4, .timeMs = 19},
        {.note = B_4, .timeMs = 19},
        {.note = D_5, .timeMs = 19},
        {.note = E_4, .timeMs = 19},
        {.note = G_SHARP_4, .timeMs = 19},
        {.note = B_4, .timeMs = 19},
        {.note = D_5, .timeMs = 19},
        {.note = E_4, .timeMs = 19},
        {.note = G_SHARP_4, .timeMs = 19},
        {.note = B_4, .timeMs = 19},
        {.note = D_5, .timeMs = 19},
        {.note = SILENCE, .timeMs = 157},
        {.note = A_4, .timeMs = 157},
        {.note = SILENCE, .timeMs = 157},
        {.note = A_4, .timeMs = 19},
        {.note = C_SHARP_5, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = A_4, .timeMs = 19},
        {.note = C_SHARP_5, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = SILENCE, .timeMs = 39},
        {.note = B_4, .timeMs = 19},
        {.note = D_5, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = B_4, .timeMs = 19},
        {.note = D_5, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = SILENCE, .timeMs = 39},
        {.note = C_SHARP_5, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = A_5, .timeMs = 19},
        {.note = C_SHARP_5, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = A_5, .timeMs = 19},
        {.note = SILENCE, .timeMs = 39},
        {.note = B_4, .timeMs = 19},
        {.note = D_5, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = B_4, .timeMs = 19},
        {.note = D_5, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = SILENCE, .timeMs = 39},
        {.note = A_4, .timeMs = 19},
        {.note = C_SHARP_5, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = A_4, .timeMs = 19},
        {.note = C_SHARP_5, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = SILENCE, .timeMs = 197},
        {.note = E_4, .timeMs = 19},
        {.note = G_SHARP_4, .timeMs = 19},
        {.note = B_4, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = E_4, .timeMs = 19},
        {.note = G_SHARP_4, .timeMs = 19},
        {.note = B_4, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = E_4, .timeMs = 19},
        {.note = G_SHARP_4, .timeMs = 19},
        {.note = B_4, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = E_4, .timeMs = 19},
        {.note = G_SHARP_4, .timeMs = 19},
        {.note = B_4, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = SILENCE, .timeMs = 157},
        {.note = E_4, .timeMs = 19},
        {.note = G_SHARP_4, .timeMs = 19},
        {.note = B_4, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = E_4, .timeMs = 19},
        {.note = G_SHARP_4, .timeMs = 19},
        {.note = B_4, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = E_4, .timeMs = 19},
        {.note = G_SHARP_4, .timeMs = 19},
        {.note = B_4, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = E_4, .timeMs = 19},
        {.note = G_SHARP_4, .timeMs = 19},
        {.note = B_4, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = E_4, .timeMs = 19},
        {.note = G_SHARP_4, .timeMs = 19},
        {.note = B_4, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = E_4, .timeMs = 19},
        {.note = G_SHARP_4, .timeMs = 19},
        {.note = B_4, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = E_4, .timeMs = 19},
        {.note = G_SHARP_4, .timeMs = 19},
        {.note = B_4, .timeMs = 19},
        {.note = E_5, .timeMs = 39},
        {.note = G_SHARP_5, .timeMs = 39},
        {.note = B_5, .timeMs = 39},
        {.note = D_6, .timeMs = 39},
        {.note = E_6, .timeMs = 39},
        {.note = G_SHARP_6, .timeMs = 39},
        {.note = B_6, .timeMs = 19},
        // loop start
        {.note = E_5, .timeMs = 19},
        {.note = A_5, .timeMs = 19},
        {.note = C_SHARP_6, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = A_5, .timeMs = 19},
        {.note = C_SHARP_6, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = A_5, .timeMs = 19},
        {.note = SILENCE, .timeMs = 157},
        {.note = E_5, .timeMs = 19},
        {.note = A_5, .timeMs = 19},
        {.note = C_SHARP_6, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = A_5, .timeMs = 19},
        {.note = C_SHARP_6, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = A_5, .timeMs = 19},
        {.note = SILENCE, .timeMs = 157},
        {.note = E_5, .timeMs = 19},
        {.note = A_5, .timeMs = 19},
        {.note = E_6, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = A_5, .timeMs = 19},
        {.note = E_6, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = A_5, .timeMs = 19},
        {.note = SILENCE, .timeMs = 157},
        {.note = E_5, .timeMs = 19},
        {.note = A_5, .timeMs = 19},
        {.note = E_6, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = A_5, .timeMs = 19},
        {.note = E_6, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = A_5, .timeMs = 19},
        {.note = SILENCE, .timeMs = 157},
        {.note = F_SHARP_5, .timeMs = 19},
        {.note = A_5, .timeMs = 19},
        {.note = D_SHARP_6, .timeMs = 19},
        {.note = F_SHARP_5, .timeMs = 19},
        {.note = A_5, .timeMs = 19},
        {.note = D_SHARP_6, .timeMs = 19},
        {.note = F_SHARP_5, .timeMs = 19},
        {.note = A_5, .timeMs = 19},
        {.note = SILENCE, .timeMs = 157},
        {.note = F_SHARP_5, .timeMs = 19},
        {.note = A_5, .timeMs = 19},
        {.note = D_SHARP_6, .timeMs = 19},
        {.note = F_SHARP_5, .timeMs = 19},
        {.note = A_5, .timeMs = 19},
        {.note = D_SHARP_6, .timeMs = 19},
        {.note = F_SHARP_5, .timeMs = 19},
        {.note = A_5, .timeMs = 19},
        {.note = SILENCE, .timeMs = 157},
        {.note = F_5, .timeMs = 19},
        {.note = B_5, .timeMs = 19},
        {.note = D_6, .timeMs = 19},
        {.note = F_5, .timeMs = 19},
        {.note = B_5, .timeMs = 19},
        {.note = D_6, .timeMs = 19},
        {.note = F_5, .timeMs = 19},
        {.note = B_5, .timeMs = 19},
        {.note = SILENCE, .timeMs = 157},
        {.note = E_5, .timeMs = 39},
        {.note = F_SHARP_5, .timeMs = 39},
        {.note = G_5, .timeMs = 39},
        {.note = G_SHARP_5, .timeMs = 39},
        {.note = A_5, .timeMs = 39},
        {.note = A_SHARP_5, .timeMs = 39},
        {.note = B_5, .timeMs = 39},
        {.note = C_6, .timeMs = 39},
        {.note = E_5, .timeMs = 19},
        {.note = A_5, .timeMs = 19},
        {.note = C_SHARP_6, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = A_5, .timeMs = 19},
        {.note = C_SHARP_6, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = A_5, .timeMs = 19},
        {.note = E_5, .timeMs = 78},
        {.note = C_SHARP_5, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = A_5, .timeMs = 19},
        {.note = C_SHARP_5, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = A_5, .timeMs = 19},
        {.note = C_SHARP_5, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = A_4, .timeMs = 236},
        {.note = A_4, .timeMs = 19},
        {.note = C_SHARP_5, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = A_4, .timeMs = 19},
        {.note = C_SHARP_5, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = A_4, .timeMs = 19},
        {.note = C_SHARP_5, .timeMs = 19},
        {.note = E_4, .timeMs = 315},
        {.note = SILENCE, .timeMs = 157},
        {.note = A_4, .timeMs = 19},
        {.note = C_SHARP_5, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = A_4, .timeMs = 19},
        {.note = C_SHARP_5, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = A_4, .timeMs = 19},
        {.note = C_SHARP_5, .timeMs = 19},
        {.note = SILENCE, .timeMs = 157},
        {.note = A_4, .timeMs = 19},
        {.note = C_SHARP_5, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = A_4, .timeMs = 19},
        {.note = C_SHARP_5, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = A_4, .timeMs = 19},
        {.note = C_SHARP_5, .timeMs = 19},
        {.note = B_4, .timeMs = 19},
        {.note = D_SHARP_5, .timeMs = 19},
        {.note = F_SHARP_5, .timeMs = 19},
        {.note = B_4, .timeMs = 19},
        {.note = D_SHARP_5, .timeMs = 19},
        {.note = F_SHARP_5, .timeMs = 19},
        {.note = B_4, .timeMs = 19},
        {.note = D_SHARP_5, .timeMs = 19},
        {.note = A_4, .timeMs = 19},
        {.note = C_SHARP_5, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = A_4, .timeMs = 19},
        {.note = C_SHARP_5, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = A_4, .timeMs = 19},
        {.note = C_SHARP_5, .timeMs = 19},
        {.note = SILENCE, .timeMs = 157},
        {.note = E_4, .timeMs = 315},
        {.note = E_5, .timeMs = 19},
        {.note = A_5, .timeMs = 19},
        {.note = C_SHARP_6, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = A_5, .timeMs = 19},
        {.note = C_SHARP_6, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = A_5, .timeMs = 19},
        {.note = SILENCE, .timeMs = 157},
        {.note = E_5, .timeMs = 19},
        {.note = A_5, .timeMs = 19},
        {.note = C_SHARP_6, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = A_5, .timeMs = 19},
        {.note = C_SHARP_6, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = A_5, .timeMs = 19},
        {.note = SILENCE, .timeMs = 157},
        {.note = E_5, .timeMs = 19},
        {.note = A_5, .timeMs = 19},
        {.note = E_6, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = A_5, .timeMs = 19},
        {.note = E_6, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = A_5, .timeMs = 19},
        {.note = SILENCE, .timeMs = 157},
        {.note = E_5, .timeMs = 19},
        {.note = A_5, .timeMs = 19},
        {.note = E_6, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = A_5, .timeMs = 19},
        {.note = E_6, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = A_5, .timeMs = 19},
        {.note = SILENCE, .timeMs = 157},
        {.note = D_5, .timeMs = 19},
        {.note = F_SHARP_5, .timeMs = 19},
        {.note = D_6, .timeMs = 19},
        {.note = D_5, .timeMs = 19},
        {.note = F_SHARP_5, .timeMs = 19},
        {.note = D_6, .timeMs = 19},
        {.note = D_5, .timeMs = 19},
        {.note = F_SHARP_5, .timeMs = 19},
        {.note = SILENCE, .timeMs = 157},
        {.note = D_5, .timeMs = 19},
        {.note = F_SHARP_5, .timeMs = 19},
        {.note = D_6, .timeMs = 19},
        {.note = D_5, .timeMs = 19},
        {.note = F_SHARP_5, .timeMs = 19},
        {.note = D_6, .timeMs = 19},
        {.note = D_5, .timeMs = 19},
        {.note = F_SHARP_5, .timeMs = 19},
        {.note = SILENCE, .timeMs = 157},
        {.note = D_SHARP_5, .timeMs = 19},
        {.note = G_SHARP_5, .timeMs = 19},
        {.note = D_SHARP_6, .timeMs = 19},
        {.note = D_SHARP_5, .timeMs = 19},
        {.note = G_SHARP_5, .timeMs = 19},
        {.note = D_SHARP_6, .timeMs = 19},
        {.note = D_SHARP_5, .timeMs = 19},
        {.note = G_SHARP_5, .timeMs = 19},
        {.note = SILENCE, .timeMs = 473},
        {.note = E_5, .timeMs = 19},
        {.note = G_5, .timeMs = 19},
        {.note = E_6, .timeMs = 19},
        {.note = G_5, .timeMs = 19},
        {.note = E_6, .timeMs = 19},
        {.note = G_5, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = G_5, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = E_6, .timeMs = 19},
        {.note = SILENCE, .timeMs = 39},
        {.note = D_5, .timeMs = 19},
        {.note = F_SHARP_5, .timeMs = 19},
        {.note = D_6, .timeMs = 19},
        {.note = F_SHARP_5, .timeMs = 19},
        {.note = D_6, .timeMs = 19},
        {.note = F_SHARP_5, .timeMs = 19},
        {.note = D_5, .timeMs = 19},
        {.note = F_SHARP_5, .timeMs = 19},
        {.note = D_5, .timeMs = 19},
        {.note = D_6, .timeMs = 19},
        {.note = SILENCE, .timeMs = 197},
        {.note = E_5, .timeMs = 19},
        {.note = A_5, .timeMs = 19},
        {.note = C_SHARP_6, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = A_5, .timeMs = 19},
        {.note = C_SHARP_6, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = A_5, .timeMs = 19},
        {.note = C_SHARP_6, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = A_5, .timeMs = 19},
        {.note = C_SHARP_6, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = A_5, .timeMs = 19},
        {.note = C_SHARP_6, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = A_5, .timeMs = 19},
        {.note = C_SHARP_6, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = A_5, .timeMs = 19},
        {.note = C_SHARP_6, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = A_5, .timeMs = 19},
        {.note = D_5, .timeMs = 19},
        {.note = A_5, .timeMs = 19},
        {.note = B_5, .timeMs = 19},
        {.note = D_5, .timeMs = 19},
        {.note = A_5, .timeMs = 19},
        {.note = B_5, .timeMs = 19},
        {.note = D_5, .timeMs = 19},
        {.note = A_5, .timeMs = 19},
        {.note = B_5, .timeMs = 19},
        {.note = D_5, .timeMs = 19},
        {.note = A_5, .timeMs = 19},
        {.note = B_5, .timeMs = 19},
        {.note = D_5, .timeMs = 19},
        {.note = A_5, .timeMs = 19},
        {.note = B_5, .timeMs = 19},
        {.note = D_5, .timeMs = 19},
        {.note = A_5, .timeMs = 19},
        {.note = B_5, .timeMs = 19},
        {.note = C_SHARP_5, .timeMs = 19},
        {.note = F_SHARP_5, .timeMs = 19},
        {.note = A_SHARP_5, .timeMs = 19},
        {.note = C_SHARP_5, .timeMs = 19},
        {.note = F_SHARP_5, .timeMs = 19},
        {.note = A_SHARP_5, .timeMs = 19},
        {.note = C_SHARP_5, .timeMs = 19},
        {.note = F_SHARP_5, .timeMs = 19},
        {.note = A_SHARP_5, .timeMs = 19},
        {.note = C_SHARP_5, .timeMs = 19},
        {.note = F_SHARP_5, .timeMs = 19},
        {.note = A_SHARP_5, .timeMs = 19},
        {.note = C_SHARP_5, .timeMs = 19},
        {.note = F_SHARP_5, .timeMs = 19},
        {.note = A_SHARP_5, .timeMs = 19},
        {.note = C_SHARP_5, .timeMs = 19},
        {.note = F_SHARP_5, .timeMs = 19},
        {.note = A_SHARP_5, .timeMs = 19},
        {.note = C_SHARP_5, .timeMs = 19},
        {.note = F_SHARP_5, .timeMs = 19},
        {.note = A_SHARP_5, .timeMs = 19},
        {.note = C_SHARP_5, .timeMs = 19},
        {.note = F_SHARP_5, .timeMs = 19},
        {.note = SILENCE, .timeMs = 315},
        {.note = F_SHARP_5, .timeMs = 78},
        {.note = G_5, .timeMs = 78},
        {.note = G_SHARP_5, .timeMs = 78},
        {.note = A_5, .timeMs = 78},
        {.note = A_SHARP_5, .timeMs = 78},
        {.note = B_5, .timeMs = 394},
        {.note = C_SHARP_6, .timeMs = 19},
        {.note = A_5, .timeMs = 19},
        {.note = F_SHARP_5, .timeMs = 19},
        {.note = C_SHARP_6, .timeMs = 19},
        {.note = A_5, .timeMs = 19},
        {.note = F_SHARP_5, .timeMs = 19},
        {.note = C_SHARP_6, .timeMs = 19},
        {.note = A_5, .timeMs = 19},
        {.note = F_SHARP_5, .timeMs = 19},
        {.note = C_SHARP_6, .timeMs = 19},
        {.note = A_5, .timeMs = 19},
        {.note = F_SHARP_5, .timeMs = 19},
        {.note = SILENCE, .timeMs = 78},
        {.note = B_5, .timeMs = 19},
        {.note = A_5, .timeMs = 19},
        {.note = F_SHARP_5, .timeMs = 19},
        {.note = B_5, .timeMs = 19},
        {.note = A_5, .timeMs = 19},
        {.note = F_SHARP_5, .timeMs = 19},
        {.note = B_5, .timeMs = 19},
        {.note = A_5, .timeMs = 19},
        {.note = F_SHARP_5, .timeMs = 19},
        {.note = B_5, .timeMs = 19},
        {.note = A_5, .timeMs = 19},
        {.note = F_SHARP_5, .timeMs = 19},
        {.note = SILENCE, .timeMs = 78},
        {.note = A_SHARP_5, .timeMs = 19},
        {.note = F_SHARP_5, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = A_SHARP_5, .timeMs = 19},
        {.note = F_SHARP_5, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = A_SHARP_5, .timeMs = 19},
        {.note = F_SHARP_5, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = A_SHARP_5, .timeMs = 19},
        {.note = F_SHARP_5, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = SILENCE, .timeMs = 78},
        {.note = C_SHARP_6, .timeMs = 19},
        {.note = A_5, .timeMs = 19},
        {.note = F_5, .timeMs = 19},
        {.note = C_SHARP_6, .timeMs = 19},
        {.note = A_5, .timeMs = 19},
        {.note = F_5, .timeMs = 19},
        {.note = C_SHARP_6, .timeMs = 19},
        {.note = A_5, .timeMs = 19},
        {.note = F_5, .timeMs = 19},
        {.note = C_SHARP_6, .timeMs = 19},
        {.note = A_5, .timeMs = 19},
        {.note = F_5, .timeMs = 98},
        {.note = B_5, .timeMs = 59},
        {.note = C_SHARP_6, .timeMs = 256},
        {.note = B_5, .timeMs = 19},
        {.note = A_5, .timeMs = 19},
        {.note = F_5, .timeMs = 19},
        {.note = B_5, .timeMs = 19},
        {.note = A_5, .timeMs = 19},
        {.note = F_5, .timeMs = 19},
        {.note = B_5, .timeMs = 19},
        {.note = A_5, .timeMs = 19},
        {.note = F_5, .timeMs = 19},
        {.note = B_5, .timeMs = 19},
        {.note = A_5, .timeMs = 19},
        {.note = F_5, .timeMs = 19},
        {.note = B_5, .timeMs = 19},
        {.note = A_5, .timeMs = 19},
        {.note = F_5, .timeMs = 19},
        {.note = B_5, .timeMs = 19},
        {.note = A_5, .timeMs = 19},
        {.note = F_5, .timeMs = 19},
        {.note = B_5, .timeMs = 19},
        {.note = A_5, .timeMs = 19},
        {.note = F_5, .timeMs = 19},
        {.note = B_5, .timeMs = 19},
        {.note = A_5, .timeMs = 19},
        {.note = F_5, .timeMs = 19},
        {.note = B_5, .timeMs = 19},
        {.note = A_5, .timeMs = 19},
        {.note = F_5, .timeMs = 19},
        {.note = B_5, .timeMs = 19},
        {.note = A_5, .timeMs = 19},
        {.note = F_5, .timeMs = 19},
        {.note = B_5, .timeMs = 19},
        {.note = A_5, .timeMs = 19},
        {.note = SILENCE, .timeMs = 315},
        {.note = B_5, .timeMs = 19},
        {.note = A_5, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = B_5, .timeMs = 19},
        {.note = A_5, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = B_5, .timeMs = 19},
        {.note = A_5, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = B_5, .timeMs = 19},
        {.note = A_5, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = SILENCE, .timeMs = 78},
        {.note = A_5, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = C_SHARP_5, .timeMs = 19},
        {.note = A_5, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = C_SHARP_5, .timeMs = 19},
        {.note = A_5, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = C_SHARP_5, .timeMs = 19},
        {.note = A_5, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = C_SHARP_5, .timeMs = 19},
        {.note = SILENCE, .timeMs = 78},
        {.note = G_SHARP_5, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = C_SHARP_5, .timeMs = 19},
        {.note = G_SHARP_5, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = C_SHARP_5, .timeMs = 19},
        {.note = G_SHARP_5, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = C_SHARP_5, .timeMs = 19},
        {.note = G_SHARP_5, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = C_SHARP_5, .timeMs = 19},
        {.note = SILENCE, .timeMs = 78},
        {.note = B_5, .timeMs = 19},
        {.note = F_SHARP_5, .timeMs = 19},
        {.note = C_SHARP_5, .timeMs = 19},
        {.note = B_5, .timeMs = 19},
        {.note = F_SHARP_5, .timeMs = 19},
        {.note = C_SHARP_5, .timeMs = 19},
        {.note = B_5, .timeMs = 19},
        {.note = F_SHARP_5, .timeMs = 19},
        {.note = C_SHARP_5, .timeMs = 19},
        {.note = B_5, .timeMs = 19},
        {.note = F_SHARP_5, .timeMs = 19},
        {.note = C_SHARP_5, .timeMs = 98},
        {.note = A_5, .timeMs = 59},
        {.note = B_5, .timeMs = 256},
        {.note = A_SHARP_5, .timeMs = 19},
        {.note = F_SHARP_5, .timeMs = 19},
        {.note = C_SHARP_5, .timeMs = 19},
        {.note = A_SHARP_5, .timeMs = 19},
        {.note = F_SHARP_5, .timeMs = 19},
        {.note = C_SHARP_5, .timeMs = 19},
        {.note = A_SHARP_5, .timeMs = 19},
        {.note = F_SHARP_5, .timeMs = 19},
        {.note = C_SHARP_5, .timeMs = 19},
        {.note = A_SHARP_5, .timeMs = 19},
        {.note = F_SHARP_5, .timeMs = 19},
        {.note = C_SHARP_5, .timeMs = 19},
        {.note = A_SHARP_5, .timeMs = 19},
        {.note = F_SHARP_5, .timeMs = 19},
        {.note = C_SHARP_5, .timeMs = 19},
        {.note = A_SHARP_5, .timeMs = 19},
        {.note = F_SHARP_5, .timeMs = 19},
        {.note = C_SHARP_5, .timeMs = 19},
        {.note = A_SHARP_5, .timeMs = 19},
        {.note = F_SHARP_5, .timeMs = 19},
        {.note = C_SHARP_5, .timeMs = 19},
        {.note = A_SHARP_5, .timeMs = 19},
        {.note = F_SHARP_5, .timeMs = 19},
        {.note = C_SHARP_5, .timeMs = 19},
        {.note = A_SHARP_5, .timeMs = 19},
        {.note = F_SHARP_5, .timeMs = 19},
        {.note = C_SHARP_5, .timeMs = 19},
        {.note = A_SHARP_5, .timeMs = 19},
        {.note = F_SHARP_5, .timeMs = 19},
        {.note = C_SHARP_5, .timeMs = 19},
        {.note = A_SHARP_5, .timeMs = 19},
        {.note = F_SHARP_5, .timeMs = 19},
        {.note = SILENCE, .timeMs = 315},
        {.note = A_5, .timeMs = 236},
        {.note = SILENCE, .timeMs = 78},
        {.note = F_5, .timeMs = 236},
        {.note = SILENCE, .timeMs = 78},
        {.note = D_5, .timeMs = 236},
        {.note = SILENCE, .timeMs = 78},
        {.note = A_5, .timeMs = 236},
        {.note = SILENCE, .timeMs = 78},
        {.note = F_5, .timeMs = 236},
        {.note = C_SHARP_6, .timeMs = 315},
        {.note = F_5, .timeMs = 157},
        {.note = A_5, .timeMs = 236},
        {.note = SILENCE, .timeMs = 78},
        {.note = B_5, .timeMs = 19},
        {.note = G_5, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = B_5, .timeMs = 19},
        {.note = G_5, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = B_5, .timeMs = 19},
        {.note = G_5, .timeMs = 19},
        {.note = SILENCE, .timeMs = 157},
        {.note = E_4, .timeMs = 315},
        {.note = B_5, .timeMs = 19},
        {.note = G_5, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = B_5, .timeMs = 19},
        {.note = G_5, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = B_5, .timeMs = 19},
        {.note = G_5, .timeMs = 19},
        {.note = SILENCE, .timeMs = 157},
        {.note = E_4, .timeMs = 315},
        {.note = B_5, .timeMs = 19},
        {.note = A_5, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = B_5, .timeMs = 19},
        {.note = A_5, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = B_5, .timeMs = 19},
        {.note = A_5, .timeMs = 19},
        {.note = SILENCE, .timeMs = 78},
        {.note = B_5, .timeMs = 19},
        {.note = A_5, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = B_5, .timeMs = 19},
        {.note = A_5, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = B_5, .timeMs = 19},
        {.note = A_5, .timeMs = 19},
        {.note = SILENCE, .timeMs = 157},
        {.note = B_5, .timeMs = 19},
        {.note = G_SHARP_5, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = B_5, .timeMs = 19},
        {.note = G_SHARP_5, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = B_5, .timeMs = 19},
        {.note = G_SHARP_5, .timeMs = 19},
        {.note = SILENCE, .timeMs = 157},
        {.note = C_6, .timeMs = 19},
        {.note = G_SHARP_5, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = C_6, .timeMs = 19},
        {.note = G_SHARP_5, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = C_6, .timeMs = 19},
        {.note = G_SHARP_5, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = C_6, .timeMs = 19},
        {.note = G_SHARP_5, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = C_6, .timeMs = 19},
        {.note = G_SHARP_5, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = C_6, .timeMs = 19},
        {.note = G_SHARP_5, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = C_6, .timeMs = 19},
        {.note = G_SHARP_5, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = C_6, .timeMs = 19},
        {.note = G_SHARP_5, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = C_6, .timeMs = 19},
        {.note = G_SHARP_5, .timeMs = 19},
        {.note = E_5, .timeMs = 19},
        {.note = C_6, .timeMs = 39},
        {.note = SILENCE, .timeMs = 19},

    },
    .numNotes = 726,
    .loopStartNote = 175,
    .shouldLoop = true
};

// Arrays
static const jukeboxSong fighterMusic[] =
{
    {.name = "Battlefield", .song = &battlefield_music},
    {.name = "Final Destination", .song = &final_dest_music},
};

static const jukeboxSong platformerMusic[] =
{
    {.name = "Intro", .song = &bgmIntro},
    {.name = "Demagio", .song = &bgmDemagio},
    {.name = "Smooth", .song = &bgmSmooth},
    {.name = "Underground", .song = &bgmUnderground},
    {.name = "Castle", .song = &bgmCastle},
    {.name = "Name Entry", .song = &bgmNameEntry},
    {.name = "Game Over", .song = &bgmGameOver},
};

static const jukeboxSong jumperMusic[] =
{
    {.name = "Game", .song = &jumpGameLoop},
    {.name = "Death", .song = &jumpDeathTune},
    {.name = "Game Over", .song = &jumpGameOverTune},
    {.name = "Winner", .song = &jumpWinTune},
    {.name = "Perfect", .song = &jumpPerfectTune},
};

static const jukeboxSong picrossMusic[] =
{
    {.name = "Game", .song = &picross_music_bg},
    {.name = "Win", .song = &picross_music_win},
    {.name = "Rick", .song = &picross_music_rick} // This must be last
};

static const jukeboxSong tiltradsMusic[] =
{
    {.name = "Title", .song = &titleMusic},
    {.name = "Game (Unused)", .song = &gameMusic},
};

static const jukeboxSong paintMusic[] =
{
    {.name = "Dice", .song = &paintBgm},
};

static const jukeboxSong jukeboxMusic[] =
{
    {.name = "Hot Rod", .song = &hotrod},
    {.name = "Fauxrio Kart", .song = &fauxrio_kart},
    {.name = "The Lake", .song = &the_lake},
    {.name = "Ya like jazz?", .song = &herecomesthesun},
    {.name = "Banana", .song = &bananaphone},
};

static const jukeboxSong creditsMusic[] =
{
    {.name = "Credits", .song = &creditsSong},
};

static const jukeboxSong testMusic[] =
{
    {.name = "Hello World", .song = &BlackDog},
};

static const jukeboxCategory musicCategories[] =
{
    {.categoryName = "Swadge Bros", .numSongs = lengthof(fighterMusic), .songs = fighterMusic},
    {.categoryName = "Swadge Land", .numSongs = lengthof(platformerMusic), .songs = platformerMusic},
    {.categoryName = "Donut Jump", .numSongs = lengthof(jumperMusic), .songs = jumperMusic},
    {.categoryName = "\x7f-cross", .numSongs = lengthof(picrossMusic), .songs = picrossMusic},
    {.categoryName = "Tiltrads", .numSongs = lengthof(tiltradsMusic), .songs = tiltradsMusic},
    {.categoryName = "MFPaint", .numSongs = lengthof(paintMusic), .songs = paintMusic},
    {.categoryName = "Jukebox", .numSongs = lengthof(jukeboxMusic), .songs = jukeboxMusic},
    {.categoryName = "Credits", .numSongs = lengthof(creditsMusic), .songs = creditsMusic},
    {.categoryName = "Test Mode", .numSongs = lengthof(testMusic), .songs = testMusic},
};

static const jukeboxSong fighterSfx[] =
{
    {.name = "Fighter 1 Hit", .song = &f1hit},
    {.name = "Fighter 2 Hit", .song = &f2hit},
    {.name = "Victory", .song = &fVictoryJingle},
    {.name = "Loss", .song = &fLossJingle},
};

static const jukeboxSong tiltradsSfx[] =
{
    {.name = "Game Start", .song = &gameStartSting},
    {.name = "Line 1", .song = &lineOneSFX},
    {.name = "Line 2", .song = &lineTwoSFX},
    {.name = "Line 3", .song = &lineThreeSFX},
    {.name = "Line 4", .song = &lineFourSFX},
    {.name = "Line 5", .song = &lineFiveSFX},
    {.name = "Line 6", .song = &lineSixSFX},
    {.name = "Line 7", .song = &lineSevenSFX},
    {.name = "Line 8", .song = &lineEightSFX},
    {.name = "Line 9", .song = &lineNineSFX},
    {.name = "Line 10", .song = &lineTenSFX},
    {.name = "Line 11", .song = &lineElevenSFX},
    {.name = "Line 12", .song = &lineTwelveSFX},
    {.name = "Line 13", .song = &lineThirteenSFX},
    {.name = "Line 14", .song = &lineFourteenSFX},
    {.name = "Line 15", .song = &lineFifteenSFX},
    {.name = "Line 16", .song = &lineSixteenSFX},
    {.name = "1-Line Clear", .song = &singleLineClearSFX},
    {.name = "2-Line Clear", .song = &doubleLineClearSFX},
    {.name = "3-Line Clear", .song = &tripleLineClearSFX},
    {.name = "4-Line Clear", .song = &quadLineClearSFX},
    {.name = "Game Over", .song = &gameOverSting},
};

static const jukeboxSong platformerSfx[] =
{
    {.name = "Menu Select", .song = &sndMenuSelect},
    {.name = "Menu Confirm", .song = &sndMenuConfirm},
    {.name = "Menu Deny", .song = &sndMenuDeny},
    {.name = "Start (Unused)", .song = &sndGameStart},
    {.name = "Jump 1", .song = &sndJump1},
    {.name = "Jump 2", .song = &sndJump2},
    {.name = "Jump 3", .song = &sndJump3},
    {.name = "Hurt", .song = &sndHurt},
    {.name = "Hit", .song = &sndHit},
    {.name = "Squish", .song = &sndSquish},
    {.name = "Break", .song = &sndBreak},
    {.name = "Coin", .song = &sndCoin},
    {.name = "Power Up", .song = &sndPowerUp},
    {.name = "Warp", .song = &sndWarp},
    {.name = "Wave Ball", .song = &sndWaveBall},
    {.name = "Death", .song = &sndDie},
    {.name = "Pause", .song = &sndPause},
    {.name = "1up", .song = &snd1up},
    {.name = "Checkpoint", .song = &sndCheckpoint},
    {.name = "Level Clear D", .song = &sndLevelClearD},
    {.name = "Level Clear C", .song = &sndLevelClearC},
    {.name = "Level Clear B", .song = &sndLevelClearB},
    {.name = "Level Clear A", .song = &sndLevelClearA},
    {.name = "Level Clear S", .song = &sndLevelClearS},
    {.name = "Tally", .song = &sndTally},
    {.name = "OuttaTime", .song = &sndOuttaTime},
};

static const jukeboxSong jumperSfx[] =
{
    {.name = "Jump", .song = &jumpPlayerJump},
    {.name = "Collect", .song = &jumperPlayerCollect},
    {.name = "Broke Combo", .song = &jumpPlayerBrokeCombo},
    {.name = "EvilDonut Jump", .song = &jumpEvilDonutJump},
    {.name = "Blump Jump", .song = &jumpBlumpJump},
    {.name = "Countdown", .song = &jumpCountdown},
};

static const jukeboxSong tunernomesfx[] =
{
    {.name = "Primary", .song = &metronome_primary},
    {.name = "Secondary", .song = &metronome_secondary},
};

static const jukeboxCategory sfxCategories[] =
{
    {.categoryName = "Swadge Bros", .numSongs = lengthof(fighterSfx), .songs = fighterSfx},
    {.categoryName = "Tiltrads", .numSongs = lengthof(tiltradsSfx), .songs = tiltradsSfx},
    {.categoryName = "Swadge Land", .numSongs = lengthof(platformerSfx), .songs = platformerSfx},
    {.categoryName = "Donut Jump", .numSongs = lengthof(jumperSfx), .songs = jumperSfx},
    {.categoryName = "Tunernome", .numSongs = lengthof(tunernomesfx), .songs = tunernomesfx},
};

/*============================================================================
 * Functions
 *==========================================================================*/

/**
 * Initializer for jukebox
 */
void  jukeboxEnterMode(display_t* disp)
{
    // Allocate zero'd memory for the mode
    jukebox = calloc(1, sizeof(jukebox_t));

    jukebox->disp = disp;

    loadFont("ibm_vga8.font", &jukebox->ibm_vga8);
    loadFont("radiostars.font", &jukebox->radiostars);
    loadFont("mm.font", &jukebox->mm);

    loadWsg("arrow12.wsg", &jukebox->arrow);

    // Load a background image to SPI RAM
    loadWsgSpiRam("jukebox.wsg", &jukebox->background, true);

    jukebox->menu = initMeleeMenu(str_jukebox, &(jukebox->mm), jukeboxMainMenuCb);

    jukebox->portableDances = initPortableDance(NULL);

    // Disable Comet {R,G,B}, Rise {R,G,B}, Pulse {R,G,B}, and Fire {G,B}
    portableDanceDisableDance(jukebox->portableDances, "Comet R");
    portableDanceDisableDance(jukebox->portableDances, "Comet G");
    portableDanceDisableDance(jukebox->portableDances, "Comet B");
    portableDanceDisableDance(jukebox->portableDances, "Rise R");
    portableDanceDisableDance(jukebox->portableDances, "Rise G");
    portableDanceDisableDance(jukebox->portableDances, "Rise B");
    portableDanceDisableDance(jukebox->portableDances, "Pulse R");
    portableDanceDisableDance(jukebox->portableDances, "Pulse G");
    portableDanceDisableDance(jukebox->portableDances, "Pulse B");
    portableDanceDisableDance(jukebox->portableDances, "Fire G");
    portableDanceDisableDance(jukebox->portableDances, "Fire B");
    portableDanceDisableDance(jukebox->portableDances, "Flashlight");

    setJukeboxMainMenu(true);

    stopNote();
}

/**
 * Called when jukebox is exited
 */
void  jukeboxExitMode(void)
{
    stopNote();

    freeFont(&jukebox->ibm_vga8);
    freeFont(&jukebox->radiostars);
    freeFont(&jukebox->mm);

    freeWsg(&jukebox->arrow);
    freeWsg(&jukebox->background);

    freePortableDance(jukebox->portableDances);

    deinitMeleeMenu(jukebox->menu);

    free(jukebox);
}

/**
 * @brief Button callback function
 *
 * @param evt The button event that occurred
 */
void  jukeboxButtonCallback(buttonEvt_t* evt)
{
    switch(jukebox->screen)
    {
        case JUKEBOX_MENU:
        {
            if (evt->down)
            {
                meleeMenuButton(jukebox->menu, evt->button);
            }
            break;
        }
        case JUKEBOX_PLAYER:
        {
            if(evt->down)
            {
                switch(evt->button)
                {
                    case BTN_A:
                    {
                        if(jukebox->inMusicSubmode)
                        {
                            buzzer_play_bgm(musicCategories[jukebox->categoryIdx].songs[jukebox->songIdx].song);
                        }
                        else
                        {
                            buzzer_play_sfx(sfxCategories[jukebox->categoryIdx].songs[jukebox->songIdx].song);
                        }
                        break;
                    }
                    case BTN_B:
                    {
                        buzzer_stop();
                        break;
                    }
                    case SELECT:
                    {
                        portableDanceNext(jukebox->portableDances);
                        break;
                    }
                    case START:
                    {
                        setJukeboxMainMenu(false);
                        break;
                    }
                    case UP:
                    {
                        buzzer_stop();
                        uint8_t length;
                        if(jukebox->inMusicSubmode)
                        {
                            length = lengthof(musicCategories);
                        }
                        else
                        {
                            length = lengthof(sfxCategories);
                        }

                        if(jukebox->categoryIdx == 0)
                        {
                            jukebox->categoryIdx = length;
                        }
                        jukebox->categoryIdx = jukebox->categoryIdx - 1;

                        jukebox->songIdx = 0;
                        break;
                    }
                    case DOWN:
                    {
                        buzzer_stop();
                        uint8_t length;
                        if(jukebox->inMusicSubmode)
                        {
                            length = lengthof(musicCategories);
                        }
                        else
                        {
                            length = lengthof(sfxCategories);
                        }

                        jukebox->categoryIdx = (jukebox->categoryIdx + 1) % length;

                        jukebox->songIdx = 0;
                        break;
                    }
                    case LEFT:
                    {
                        buzzer_stop();
                        uint8_t length;
                        if(jukebox->inMusicSubmode)
                        {
                            length = musicCategories[jukebox->categoryIdx].numSongs;
                            // Hide the last picross song if not unlocked
                            if(picrossMusic == musicCategories[jukebox->categoryIdx].songs &&
                                !beatenRickLevel())
                            {
                                length--;
                            }
                        }
                        else
                        {
                            length = sfxCategories[jukebox->categoryIdx].numSongs;
                        }

                        if(jukebox->songIdx == 0)
                        {
                            jukebox->songIdx = length;
                        }
                        jukebox->songIdx = jukebox->songIdx - 1;
                        break;
                    }
                    case RIGHT:
                    {
                        buzzer_stop();
                        uint8_t length;
                        if(jukebox->inMusicSubmode)
                        {
                            length = musicCategories[jukebox->categoryIdx].numSongs;
                            // Hide the last picross song if not unlocked
                            if(picrossMusic == musicCategories[jukebox->categoryIdx].songs &&
                                !beatenRickLevel())
                            {
                                length--;
                            }
                        }
                        else
                        {
                            length = sfxCategories[jukebox->categoryIdx].numSongs;
                        }

                        jukebox->songIdx = (jukebox->songIdx + 1) % length;
                        break;
                    }
                    default:
                    {
                        break;
                    }
                }
            }
            break;
        }
    }
}

/**
 * This function is called when a button press is pressed. Buttons are
 * handled by interrupts and queued up for this callback, so there are no
 * strict timing restrictions for this function.
 *
 * @param evt The button event that occurred
 */
void  jukeboxTouchCallback(touch_event_t* evt)
{
    jukebox->touchHeld = evt->state != 0;

    if(jukebox->touchHeld)
    {
        jukebox->touchPosition = roundf((evt->position * MAX_LED_BRIGHTNESS) / 255.0f);

        setAndSaveLedBrightness(jukebox->touchPosition);
    }
}

/**
 * Update the display by drawing the current state of affairs
 */
void  jukeboxMainLoop(int64_t elapsedUs)
{
    switch(jukebox->screen)
    {
        case JUKEBOX_MENU:
        {
            jukebox->disp->clearPx();
            drawMeleeMenu(jukebox->disp, jukebox->menu);
            break;
        }
        case JUKEBOX_PLAYER:
        {
            portableDanceMainLoop(jukebox->portableDances, elapsedUs);

            //fillDisplayArea(jukebox->disp, 0, 0, jukebox->disp->w, jukebox->disp->h, c010);

            // Plot the button funcs
            // LEDs
            drawText(
                jukebox->disp,
                &jukebox->radiostars, c555,
                str_leds,
                CORNER_OFFSET,
                CORNER_OFFSET);
            // Light dance name
            drawText(jukebox->disp, &(jukebox->radiostars), c111,
                portableDanceGetName(jukebox->portableDances),
                jukebox->disp->w - CORNER_OFFSET - textWidth(&jukebox->radiostars, portableDanceGetName(jukebox->portableDances)),
                CORNER_OFFSET);

            // Back
            drawText(
                jukebox->disp,
                &jukebox->radiostars, c555,
                str_back,
                CORNER_OFFSET,
                CORNER_OFFSET + LINE_BREAK_Y + jukebox->radiostars.h);

            // LED Brightness
            drawText(
                jukebox->disp,
                &jukebox->radiostars, c555,
                str_brightness,
                CORNER_OFFSET,
                CORNER_OFFSET + (LINE_BREAK_Y + jukebox->radiostars.h) * 2);
            char text[32];
            snprintf(text, sizeof(text), "%d", getLedBrightness());
            drawText(
                jukebox->disp,
                &jukebox->radiostars, c111,
                text,
                jukebox->disp->w - textWidth(&jukebox->radiostars, text) - CORNER_OFFSET,
                CORNER_OFFSET + (LINE_BREAK_Y + jukebox->radiostars.h) * 2);

            // Stop
            int16_t afterText = drawText(
                                    jukebox->disp,
                                    &jukebox->radiostars, c511,
                                    "B",
                                    CORNER_OFFSET,
                                    jukebox->disp->h - jukebox->radiostars.h - CORNER_OFFSET);
            drawText(
                jukebox->disp,
                &jukebox->radiostars, c555,
                str_stop,
                afterText,
                jukebox->disp->h - jukebox->radiostars.h - CORNER_OFFSET);

            // Play
            afterText = drawText(
                            jukebox->disp,
                            &jukebox->radiostars, c151,
                            "A",
                            jukebox->disp->w - textWidth(&jukebox->radiostars, str_play) - textWidth(&jukebox->radiostars, "A") - CORNER_OFFSET,
                            jukebox->disp->h - jukebox->radiostars.h - CORNER_OFFSET);
            drawText(
                jukebox->disp,
                &jukebox->radiostars, c555,
                str_play,
                afterText,
                jukebox->disp->h - jukebox->radiostars.h - CORNER_OFFSET);



            char* categoryName;
            char* songName;
            char* songTypeName;
            uint8_t numSongs;
            bool drawNames = false;
            if(jukebox->inMusicSubmode)
            {
                // Warn the user that the swadge is muted, if that's the case
                if(getBgmIsMuted())
                {
                    drawText(
                        jukebox->disp,
                        &jukebox->radiostars, c551,
                        str_bgm_muted,
                        (jukebox->disp->w - textWidth(&jukebox->radiostars, str_bgm_muted)) / 2,
                        jukebox->disp->h / 2);
                }
                else
                {
                    categoryName = musicCategories[jukebox->categoryIdx].categoryName;
                    songName = musicCategories[jukebox->categoryIdx].songs[jukebox->songIdx].name;
                    songTypeName = "Music";
                    numSongs = musicCategories[jukebox->categoryIdx].numSongs;
                    // Hide the last picross song if not unlocked
                    if(picrossMusic == musicCategories[jukebox->categoryIdx].songs &&
                        !beatenRickLevel())
                    {
                        numSongs--;
                    }
                    drawNames = true;
                }
            }
            else
            {
                // Warn the user that the swadge is muted, if that's the case
                if(getSfxIsMuted())
                {
                    drawText(
                        jukebox->disp,
                        &jukebox->radiostars, c551,
                        str_sfx_muted,
                        (jukebox->disp->w - textWidth(&jukebox->radiostars, str_sfx_muted)) / 2,
                        jukebox->disp->h / 2);
                }
                else
                {
                    categoryName = sfxCategories[jukebox->categoryIdx].categoryName;
                    songName = sfxCategories[jukebox->categoryIdx].songs[jukebox->songIdx].name;
                    songTypeName = "SFX";
                    numSongs = sfxCategories[jukebox->categoryIdx].numSongs;
                    drawNames = true;
                }
            }

            if(drawNames)
            {
                // Draw the mode name
                snprintf(text, sizeof(text), "Mode: %s", categoryName);
                int16_t width = textWidth(&(jukebox->radiostars), text);
                int16_t yOff = (jukebox->disp->h - jukebox->radiostars.h) / 2 - jukebox->radiostars.h * 0;
                drawText(jukebox->disp, &(jukebox->radiostars), c313,
                        text,
                        (jukebox->disp->w - width) / 2,
                        yOff);
                // Draw some arrows
                drawWsg(jukebox->disp, &jukebox->arrow,
                        ((jukebox->disp->w - width) / 2) - 8 - jukebox->arrow.w, yOff,
                        false, false, 0);
                drawWsg(jukebox->disp, &jukebox->arrow,
                        ((jukebox->disp->w - width) / 2) + width + 8, yOff,
                        false, false, 180);

                // Draw the song name
                snprintf(text, sizeof(text), "%s: %s", songTypeName, songName);
                yOff = (jukebox->disp->h - jukebox->radiostars.h) / 2 + jukebox->radiostars.h * 2.5f;
                width = textWidth(&(jukebox->radiostars), text);
                drawText(jukebox->disp, &(jukebox->radiostars), c113,
                        text,
                        (jukebox->disp->w - width) / 2,
                        yOff);
                // Draw some arrows if this category has more than 1 song
                if(numSongs > 1)
                {
                    drawWsg(jukebox->disp, &jukebox->arrow,
                            ((jukebox->disp->w - width) / 2) - 8 - jukebox->arrow.w, yOff,
                            false, false, 270);
                    drawWsg(jukebox->disp, &jukebox->arrow,
                            ((jukebox->disp->w - width) / 2) + width + 8, yOff,
                            false, false, 90);
                }
            }
            break;

            // // Touch Controls
            // if(jukebox->touchHeld)
            // {
            //     getTouchCentroid(&jukebox->touchPosition, &jukebox->touchIntensity);
            //     jukebox->touchPosition = (jukebox->touchPosition * MAX_LED_BRIGHTNESS) / 1023;

            //     setAndSaveLedBrightness(jukebox->touchPosition);
            // }
        }
    }
}

void setJukeboxMainMenu(bool resetPos)
{
    buzzer_stop();

    resetMeleeMenu(jukebox->menu, str_jukebox, jukeboxMainMenuCb);
    addRowToMeleeMenu(jukebox->menu, str_bgm);
    addRowToMeleeMenu(jukebox->menu, str_sfx);
    addRowToMeleeMenu(jukebox->menu, str_exit);
    // Set the position
    if(resetPos)
    {
        jukebox->mainMenuPos = 0;
    }
    jukebox->menu->selectedRow = jukebox->mainMenuPos;
    jukebox->screen = JUKEBOX_MENU;
}

void jukeboxMainMenuCb(const char * opt)
{
    // Save the position
    jukebox->mainMenuPos = jukebox->menu->selectedRow;

    if (opt == str_exit)
    {
        switchToSwadgeMode(&modeMainMenu);
        return;
    }
    else if (opt == str_bgm)
    {
        jukebox->screen = JUKEBOX_PLAYER;
        jukebox->categoryIdx = 0;
        jukebox->songIdx = 0;
        jukebox->inMusicSubmode = true;
    }
    else if (opt == str_sfx)
    {
        jukebox->screen = JUKEBOX_PLAYER;
        jukebox->categoryIdx = 0;
        jukebox->songIdx = 0;
        jukebox->inMusicSubmode = false;
    }
}

/**
 * @brief Draw a portion of the background when requested
 *
 * @param disp The display to draw to
 * @param x The X offset to draw
 * @param y The Y offset to draw
 * @param w The width to draw
 * @param h The height to draw
 * @param up The current number of the update call
 * @param upNum The total number of update calls for this frame
 */
void jukeboxBackgroundDrawCb(display_t* disp, int16_t x, int16_t y,
                             int16_t w, int16_t h, int16_t up, int16_t upNum )
{
    switch (jukebox->screen)
    {
        case JUKEBOX_MENU:
        {
            // This draws menu background
            break;
        }
        case JUKEBOX_PLAYER:
        {
            // Figure out source and destination pointers
            paletteColor_t* dst = &disp->pxFb[(y * disp->w) + x];
            paletteColor_t* src = &jukebox->background.px[(y * disp->w) + x];
            // Copy the image to the framebuffer
            memcpy(dst, src, w * h);
            break;
        }
    }
}

/**
 * @return true if Rick was unlocked, false if it was not
 */
static bool beatenRickLevel(void)
{
    picrossVictoryData_t vict = {0};
    size_t victSize = sizeof(picrossVictoryData_t);
    if(readNvsBlob(picrossCompletedLevelData, &vict, &victSize) && victSize >= sizeof(picrossVictoryData_t))
    {
        return vict.victories[29] ? true : false;
    }
    return false;
}
