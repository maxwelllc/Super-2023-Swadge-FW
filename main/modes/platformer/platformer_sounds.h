#ifndef _PLATFORMER_SOUNDS_H_
#define _PLATFORMER_SOUNDS_H_

//==============================================================================
// Includes
//==============================================================================
#include <stdint.h>
#include "musical_buzzer.h"

//==============================================================================
// Constants
//==============================================================================
static const song_t bgmDemagio =
{
    .notes =
    {
        {.note = A_2, .timeMs = 187},
        {.note = C_SHARP_5, .timeMs = 187},
        {.note = E_5, .timeMs = 187},
        {.note = C_SHARP_3, .timeMs = 187},
        {.note = A_4, .timeMs = 187},
        {.note = SILENCE, .timeMs = 187},
        {.note = E_4, .timeMs = 187},
        {.note = F_4, .timeMs = 187},
        {.note = F_SHARP_4, .timeMs = 187},
        {.note = D_3, .timeMs = 187},
        {.note = A_4, .timeMs = 187},
        {.note = SILENCE, .timeMs = 187},
        {.note = E_4, .timeMs = 187},
        {.note = C_SHARP_3, .timeMs = 187},
        {.note = D_SHARP_4, .timeMs = 187},
        {.note = C_3, .timeMs = 187},
        {.note = B_2, .timeMs = 187},
        {.note = D_4, .timeMs = 187},
        {.note = A_4, .timeMs = 187},
        {.note = SILENCE, .timeMs = 187},
        {.note = A_4, .timeMs = 187},
        {.note = B_2, .timeMs = 187},
        {.note = E_5, .timeMs = 187},
        {.note = SILENCE, .timeMs = 187},
        {.note = C_SHARP_5, .timeMs = 187},
        {.note = E_3, .timeMs = 187},
        {.note = E_2, .timeMs = 187},
        {.note = B_4, .timeMs = 187},
        {.note = SILENCE, .timeMs = 187},
        {.note = G_SHARP_4, .timeMs = 187},
        {.note = G_4, .timeMs = 187},
        {.note = F_SHARP_4, .timeMs = 187},
        {.note = A_2, .timeMs = 187},
        {.note = C_SHARP_5, .timeMs = 187},
        {.note = E_5, .timeMs = 187},
        {.note = C_SHARP_3, .timeMs = 187},
        {.note = A_4, .timeMs = 187},
        {.note = SILENCE, .timeMs = 187},
        {.note = E_4, .timeMs = 187},
        {.note = F_4, .timeMs = 187},
        {.note = F_SHARP_4, .timeMs = 187},
        {.note = D_3, .timeMs = 187},
        {.note = A_4, .timeMs = 187},
        {.note = SILENCE, .timeMs = 187},
        {.note = A_4, .timeMs = 187},
        {.note = D_SHARP_3, .timeMs = 187},
        {.note = B_4, .timeMs = 187},
        {.note = SILENCE, .timeMs = 187},
        {.note = E_3, .timeMs = 187},
        {.note = C_SHARP_5, .timeMs = 187},
        {.note = A_4, .timeMs = 187},
        {.note = E_4, .timeMs = 187},
        {.note = F_SHARP_4, .timeMs = 187},
        {.note = E_2, .timeMs = 187},
        {.note = E_5, .timeMs = 187},
        {.note = SILENCE, .timeMs = 187},
        {.note = A_4, .timeMs = 187},
        {.note = A_2, .timeMs = 187},
        {.note = SILENCE, .timeMs = 187},
        {.note = A_2, .timeMs = 93},
        {.note = SILENCE, .timeMs = 93},
        {.note = A_2, .timeMs = 187},
        {.note = B_2, .timeMs = 187},
        {.note = C_3, .timeMs = 187},
        {.note = C_SHARP_3, .timeMs = 187},
        {.note = D_3, .timeMs = 187},
        {.note = F_SHARP_4, .timeMs = 187},
        {.note = A_4, .timeMs = 187},
        {.note = D_3, .timeMs = 187},
        {.note = A_4, .timeMs = 187},
        {.note = D_3, .timeMs = 187},
        {.note = F_SHARP_4, .timeMs = 187},
        {.note = D_3, .timeMs = 187},
        {.note = E_4, .timeMs = 187},
        {.note = A_4, .timeMs = 187},
        {.note = C_SHARP_3, .timeMs = 187},
        {.note = A_4, .timeMs = 187},
        {.note = C_SHARP_3, .timeMs = 187},
        {.note = E_4, .timeMs = 187},
        {.note = C_3, .timeMs = 187},
        {.note = SILENCE, .timeMs = 187},
        {.note = D_4, .timeMs = 187},
        {.note = B_2, .timeMs = 187},
        {.note = F_SHARP_3, .timeMs = 187},
        {.note = A_4, .timeMs = 187},
        {.note = G_SHARP_4, .timeMs = 187},
        {.note = E_3, .timeMs = 187},
        {.note = E_2, .timeMs = 187},
        {.note = E_4, .timeMs = 187},
        {.note = C_SHARP_4, .timeMs = 187},
        {.note = A_2, .timeMs = 187},
        {.note = SILENCE, .timeMs = 187},
        {.note = B_2, .timeMs = 187},
        {.note = C_SHARP_3, .timeMs = 187},
        {.note = SILENCE, .timeMs = 187},
        {.note = E_3, .timeMs = 187},
        {.note = SILENCE, .timeMs = 187},
        {.note = D_3, .timeMs = 187},
        {.note = F_SHARP_4, .timeMs = 187},
        {.note = A_4, .timeMs = 187},
        {.note = D_3, .timeMs = 187},
        {.note = A_4, .timeMs = 187},
        {.note = D_3, .timeMs = 187},
        {.note = F_SHARP_4, .timeMs = 187},
        {.note = D_3, .timeMs = 187},
        {.note = E_4, .timeMs = 187},
        {.note = A_4, .timeMs = 187},
        {.note = C_SHARP_3, .timeMs = 187},
        {.note = A_4, .timeMs = 187},
        {.note = C_SHARP_3, .timeMs = 187},
        {.note = E_4, .timeMs = 187},
        {.note = C_3, .timeMs = 187},
        {.note = SILENCE, .timeMs = 187},
        {.note = B_2, .timeMs = 187},
        {.note = D_SHARP_4, .timeMs = 187},
        {.note = A_4, .timeMs = 187},
        {.note = B_2, .timeMs = 187},
        {.note = A_4, .timeMs = 187},
        {.note = B_2, .timeMs = 187},
        {.note = C_SHARP_5, .timeMs = 187},
        {.note = B_2, .timeMs = 187},
        {.note = B_4, .timeMs = 187},
        {.note = E_2, .timeMs = 187},
        {.note = SILENCE, .timeMs = 187},
        {.note = B_2, .timeMs = 187},
        {.note = E_3, .timeMs = 187},
        {.note = F_SHARP_3, .timeMs = 187},
        {.note = G_3, .timeMs = 187},
        {.note = G_SHARP_3, .timeMs = 187},
        {.note = A_3, .timeMs = 187},
        {.note = C_SHARP_5, .timeMs = 187},
        {.note = E_5, .timeMs = 187},
        {.note = C_SHARP_3, .timeMs = 187},
        {.note = A_4, .timeMs = 187},
        {.note = SILENCE, .timeMs = 187},
        {.note = E_4, .timeMs = 187},
        {.note = F_4, .timeMs = 187},
        {.note = F_SHARP_4, .timeMs = 187},
        {.note = D_3, .timeMs = 187},
        {.note = A_4, .timeMs = 187},
        {.note = SILENCE, .timeMs = 187},
        {.note = F_SHARP_4, .timeMs = 187},
        {.note = D_SHARP_3, .timeMs = 187},
        {.note = A_4, .timeMs = 187},
        {.note = D_SHARP_3, .timeMs = 187},
        {.note = E_3, .timeMs = 187},
        {.note = E_4, .timeMs = 187},
        {.note = A_4, .timeMs = 187},
        {.note = SILENCE, .timeMs = 187},
        {.note = E_5, .timeMs = 187},
        {.note = E_2, .timeMs = 187},
        {.note = C_SHARP_5, .timeMs = 187},
        {.note = B_4, .timeMs = 187},
        {.note = A_4, .timeMs = 187},
        {.note = E_3, .timeMs = 187},
        {.note = E_2, .timeMs = 187},
        {.note = E_3, .timeMs = 187},
        {.note = A_2, .timeMs = 187},
        {.note = D_2, .timeMs = 187},
        {.note = E_2, .timeMs = 187},
        {.note = F_2, .timeMs = 187},
        {.note = F_SHARP_2, .timeMs = 187},
        {.note = F_SHARP_4, .timeMs = 187},
        {.note = A_SHARP_3, .timeMs = 187},
        {.note = E_4, .timeMs = 187},
        {.note = F_SHARP_2, .timeMs = 187},
        {.note = C_SHARP_3, .timeMs = 187},
        {.note = E_4, .timeMs = 187},
        {.note = D_SHARP_4, .timeMs = 187},
        {.note = B_2, .timeMs = 187},
        {.note = D_SHARP_4, .timeMs = 187},
        {.note = F_SHARP_3, .timeMs = 187},
        {.note = A_3, .timeMs = 187},
        {.note = B_2, .timeMs = 187},
        {.note = G_SHARP_3, .timeMs = 187},
        {.note = F_SHARP_3, .timeMs = 187},
        {.note = A_SHARP_3, .timeMs = 187},
        {.note = F_SHARP_2, .timeMs = 187},
        {.note = F_SHARP_4, .timeMs = 187},
        {.note = G_SHARP_4, .timeMs = 187},
        {.note = E_4, .timeMs = 187},
        {.note = F_SHARP_2, .timeMs = 187},
        {.note = E_4, .timeMs = 187},
        {.note = F_SHARP_4, .timeMs = 187},
        {.note = D_SHARP_4, .timeMs = 187},
        {.note = B_2, .timeMs = 187},
        {.note = D_SHARP_4, .timeMs = 187},
        {.note = F_SHARP_3, .timeMs = 187},
        {.note = A_3, .timeMs = 187},
        {.note = B_2, .timeMs = 187},
        {.note = B_3, .timeMs = 187},
        {.note = D_SHARP_4, .timeMs = 187},
        {.note = F_SHARP_4, .timeMs = 187},
        {.note = E_2, .timeMs = 187},
        {.note = G_SHARP_4, .timeMs = 187},
        {.note = D_4, .timeMs = 187},
        {.note = E_2, .timeMs = 187},
        {.note = F_SHARP_4, .timeMs = 187},
        {.note = D_4, .timeMs = 187},
        {.note = E_2, .timeMs = 187},
        {.note = F_SHARP_4, .timeMs = 187},
        {.note = A_2, .timeMs = 187},
        {.note = E_4, .timeMs = 187},
        {.note = D_4, .timeMs = 187},
        {.note = C_SHARP_4, .timeMs = 187},
        {.note = A_2, .timeMs = 187},
        {.note = G_3, .timeMs = 187},
        {.note = C_SHARP_4, .timeMs = 187},
        {.note = E_4, .timeMs = 187},
        {.note = E_2, .timeMs = 187},
        {.note = G_SHARP_4, .timeMs = 187},
        {.note = D_4, .timeMs = 187},
        {.note = E_2, .timeMs = 187},
        {.note = F_SHARP_4, .timeMs = 187},
        {.note = D_4, .timeMs = 187},
        {.note = E_2, .timeMs = 187},
        {.note = F_SHARP_4, .timeMs = 187},
        {.note = A_2, .timeMs = 187},
        {.note = E_4, .timeMs = 187},
        {.note = C_SHARP_4, .timeMs = 187},
        {.note = G_SHARP_2, .timeMs = 187},
        {.note = D_SHARP_4, .timeMs = 187},
        {.note = C_4, .timeMs = 187},
        {.note = G_2, .timeMs = 187},
        {.note = D_4, .timeMs = 187},
        {.note = F_SHARP_2, .timeMs = 187},
        {.note = F_SHARP_4, .timeMs = 187},
        {.note = A_SHARP_3, .timeMs = 187},
        {.note = E_4, .timeMs = 187},
        {.note = F_SHARP_2, .timeMs = 187},
        {.note = C_SHARP_3, .timeMs = 187},
        {.note = E_4, .timeMs = 187},
        {.note = D_SHARP_4, .timeMs = 187},
        {.note = B_2, .timeMs = 187},
        {.note = D_SHARP_4, .timeMs = 187},
        {.note = F_SHARP_3, .timeMs = 187},
        {.note = A_3, .timeMs = 187},
        {.note = B_2, .timeMs = 187},
        {.note = G_SHARP_3, .timeMs = 187},
        {.note = F_SHARP_3, .timeMs = 187},
        {.note = A_SHARP_3, .timeMs = 187},
        {.note = F_SHARP_2, .timeMs = 187},
        {.note = F_SHARP_4, .timeMs = 187},
        {.note = G_SHARP_4, .timeMs = 187},
        {.note = E_4, .timeMs = 187},
        {.note = F_SHARP_2, .timeMs = 187},
        {.note = E_4, .timeMs = 187},
        {.note = F_SHARP_4, .timeMs = 187},
        {.note = B_4, .timeMs = 187},
        {.note = B_2, .timeMs = 187},
        {.note = B_4, .timeMs = 187},
        {.note = A_4, .timeMs = 187},
        {.note = B_2, .timeMs = 187},
        {.note = F_SHARP_4, .timeMs = 187},
        {.note = D_SHARP_4, .timeMs = 187},
        {.note = E_4, .timeMs = 187},
        {.note = F_SHARP_4, .timeMs = 187},
        {.note = E_2, .timeMs = 187},
        {.note = G_SHARP_4, .timeMs = 187},
        {.note = B_4, .timeMs = 187},
        {.note = E_2, .timeMs = 187},
        {.note = F_SHARP_4, .timeMs = 187},
        {.note = B_4, .timeMs = 187},
        {.note = E_2, .timeMs = 187},
        {.note = E_4, .timeMs = 187},
        {.note = A_2, .timeMs = 187},
        {.note = E_4, .timeMs = 187},
        {.note = D_4, .timeMs = 187},
        {.note = C_SHARP_4, .timeMs = 187},
        {.note = A_2, .timeMs = 187},
        {.note = G_3, .timeMs = 187},
        {.note = C_SHARP_4, .timeMs = 187},
        {.note = E_4, .timeMs = 187},
        {.note = E_2, .timeMs = 187},
        {.note = G_SHARP_4, .timeMs = 187},
        {.note = B_4, .timeMs = 187},
        {.note = E_2, .timeMs = 187},
        {.note = F_SHARP_4, .timeMs = 187},
        {.note = B_4, .timeMs = 187},
        {.note = E_2, .timeMs = 187},
        {.note = E_4, .timeMs = 187},
        {.note = A_2, .timeMs = 187},
        {.note = E_4, .timeMs = 187},
        {.note = C_SHARP_4, .timeMs = 187},
        {.note = E_4, .timeMs = 187},
        {.note = A_4, .timeMs = 187},
        {.note = A_SHARP_4, .timeMs = 187},
        {.note = B_4, .timeMs = 187},
        {.note = C_5, .timeMs = 179},

    },
    .numNotes = 289,
    .shouldLoop = true
};

static const song_t bgmIntro =
{
    .notes =
    {
        {C_SHARP_5, 130},
        {C_SHARP_3, 62},
        {SILENCE, 67},
        {A_3, 62},
        {SILENCE, 67},
        {E_4, 62},
        {SILENCE, 67},
        {A_4, 130},
        {C_SHARP_5, 130},
        {D_5, 130},
        {C_3, 62},
        {SILENCE, 67},
        {A_3, 62},
        {SILENCE, 67},
        {E_4, 67},
        {SILENCE, 62},
        {A_4, 130},
        {D_5, 130},
        {E_5, 130},
        {B_4, 130},
        {A_4, 130},
        {E_4, 130},
        {B_3, 83},
        {A_3, 78},
        {E_3, 78},
        {B_2, 83},
        {E_2, 317}
    },
    .numNotes = 27,
    .shouldLoop = false
};

static const song_t bgmSmooth =
{
    .notes =
    {
        {.note = F_3, .timeMs = 187},
        {.note = SILENCE, .timeMs = 187},
        {.note = F_4, .timeMs = 187},
        {.note = F_3, .timeMs = 187},
        {.note = SILENCE, .timeMs = 187},
        {.note = G_SHARP_4, .timeMs = 187},
        {.note = G_SHARP_3, .timeMs = 187},
        {.note = A_3, .timeMs = 187},
        {.note = A_SHARP_3, .timeMs = 187},
        {.note = A_SHARP_4, .timeMs = 187},
        {.note = SILENCE, .timeMs = 187},
        {.note = A_SHARP_4, .timeMs = 187},
        {.note = SILENCE, .timeMs = 187},
        {.note = A_SHARP_3, .timeMs = 187},
        {.note = G_SHARP_3, .timeMs = 187},
        {.note = A_SHARP_3, .timeMs = 187},
        {.note = F_3, .timeMs = 187},
        {.note = SILENCE, .timeMs = 187},
        {.note = F_4, .timeMs = 187},
        {.note = F_3, .timeMs = 187},
        {.note = SILENCE, .timeMs = 187},
        {.note = G_SHARP_4, .timeMs = 187},
        {.note = G_SHARP_3, .timeMs = 187},
        {.note = A_3, .timeMs = 187},
        {.note = A_SHARP_3, .timeMs = 187},
        {.note = A_SHARP_4, .timeMs = 187},
        {.note = SILENCE, .timeMs = 187},
        {.note = A_SHARP_4, .timeMs = 187},
        {.note = SILENCE, .timeMs = 187},
        {.note = A_SHARP_3, .timeMs = 187},
        {.note = G_SHARP_3, .timeMs = 93},
        {.note = A_SHARP_3, .timeMs = 93},
        {.note = C_4, .timeMs = 187},
        {.note = F_3, .timeMs = 187},
        {.note = SILENCE, .timeMs = 187},
        {.note = F_4, .timeMs = 187},
        {.note = F_3, .timeMs = 187},
        {.note = SILENCE, .timeMs = 187},
        {.note = G_SHARP_4, .timeMs = 187},
        {.note = G_SHARP_3, .timeMs = 187},
        {.note = A_3, .timeMs = 187},
        {.note = A_SHARP_3, .timeMs = 187},
        {.note = A_SHARP_4, .timeMs = 187},
        {.note = SILENCE, .timeMs = 187},
        {.note = A_SHARP_4, .timeMs = 187},
        {.note = SILENCE, .timeMs = 187},
        {.note = A_SHARP_3, .timeMs = 187},
        {.note = G_SHARP_3, .timeMs = 187},
        {.note = A_SHARP_3, .timeMs = 187},
        {.note = F_3, .timeMs = 187},
        {.note = SILENCE, .timeMs = 187},
        {.note = F_4, .timeMs = 187},
        {.note = F_3, .timeMs = 187},
        {.note = SILENCE, .timeMs = 187},
        {.note = G_SHARP_4, .timeMs = 187},
        {.note = G_SHARP_3, .timeMs = 187},
        {.note = A_3, .timeMs = 187},
        {.note = D_6, .timeMs = 46},
        {.note = D_SHARP_6, .timeMs = 140},
        {.note = A_SHARP_5, .timeMs = 187},
        {.note = G_SHARP_5, .timeMs = 187},
        {.note = A_SHARP_5, .timeMs = 187},
        {.note = G_5, .timeMs = 187},
        {.note = G_SHARP_5, .timeMs = 187},
        {.note = G_5, .timeMs = 187},
        {.note = D_SHARP_5, .timeMs = 187},
        {.note = C_SHARP_3, .timeMs = 187},
        {.note = F_5, .timeMs = 187},
        {.note = F_3, .timeMs = 187},
        {.note = C_SHARP_3, .timeMs = 187},
        {.note = A_SHARP_5, .timeMs = 187},
        {.note = F_3, .timeMs = 187},
        {.note = G_SHARP_5, .timeMs = 187},
        {.note = G_5, .timeMs = 187},
        {.note = D_SHARP_3, .timeMs = 187},
        {.note = C_6, .timeMs = 187},
        {.note = G_3, .timeMs = 187},
        {.note = D_SHARP_3, .timeMs = 187},
        {.note = A_SHARP_5, .timeMs = 187},
        {.note = G_SHARP_5, .timeMs = 187},
        {.note = G_3, .timeMs = 187},
        {.note = G_SHARP_5, .timeMs = 187},
        {.note = F_5, .timeMs = 187},
        {.note = F_3, .timeMs = 187},
        {.note = G_SHARP_3, .timeMs = 187},
        {.note = F_3, .timeMs = 187},
        {.note = SILENCE, .timeMs = 187},
        {.note = G_SHARP_3, .timeMs = 187},
        {.note = A_3, .timeMs = 187},
        {.note = A_SHARP_3, .timeMs = 187},
        {.note = F_3, .timeMs = 187},
        {.note = SILENCE, .timeMs = 187},
        {.note = G_SHARP_3, .timeMs = 187},
        {.note = F_3, .timeMs = 187},
        {.note = SILENCE, .timeMs = 187},
        {.note = B_3, .timeMs = 46},
        {.note = C_4, .timeMs = 140},
        {.note = A_SHARP_3, .timeMs = 187},
        {.note = G_SHARP_3, .timeMs = 187},
        {.note = C_SHARP_3, .timeMs = 187},
        {.note = F_5, .timeMs = 187},
        {.note = F_3, .timeMs = 187},
        {.note = C_SHARP_3, .timeMs = 187},
        {.note = A_SHARP_5, .timeMs = 187},
        {.note = F_3, .timeMs = 187},
        {.note = G_SHARP_5, .timeMs = 187},
        {.note = G_5, .timeMs = 187},
        {.note = D_SHARP_3, .timeMs = 187},
        {.note = C_6, .timeMs = 187},
        {.note = G_3, .timeMs = 187},
        {.note = D_SHARP_3, .timeMs = 187},
        {.note = A_SHARP_5, .timeMs = 187},
        {.note = A_SHARP_3, .timeMs = 187},
        {.note = C_6, .timeMs = 187},
        {.note = A_SHARP_3, .timeMs = 187},
        {.note = F_5, .timeMs = 187},
        {.note = F_3, .timeMs = 187},
        {.note = G_SHARP_3, .timeMs = 187},
        {.note = F_3, .timeMs = 187},
        {.note = SILENCE, .timeMs = 187},
        {.note = G_SHARP_3, .timeMs = 187},
        {.note = A_3, .timeMs = 187},
        {.note = A_SHARP_3, .timeMs = 187},
        {.note = F_3, .timeMs = 187},
        {.note = SILENCE, .timeMs = 187},
        {.note = G_SHARP_3, .timeMs = 187},
        {.note = F_3, .timeMs = 187},
        {.note = SILENCE, .timeMs = 187},
        {.note = B_3, .timeMs = 46},
        {.note = C_4, .timeMs = 140},
        {.note = A_SHARP_3, .timeMs = 187},
        {.note = F_4, .timeMs = 187},
        {.note = C_SHARP_3, .timeMs = 187},
        {.note = F_5, .timeMs = 187},
        {.note = F_3, .timeMs = 187},
        {.note = C_SHARP_3, .timeMs = 187},
        {.note = A_SHARP_5, .timeMs = 187},
        {.note = F_3, .timeMs = 187},
        {.note = G_SHARP_5, .timeMs = 187},
        {.note = G_5, .timeMs = 187},
        {.note = D_SHARP_3, .timeMs = 187},
        {.note = C_6, .timeMs = 187},
        {.note = G_3, .timeMs = 187},
        {.note = D_SHARP_3, .timeMs = 187},
        {.note = A_SHARP_5, .timeMs = 187},
        {.note = G_SHARP_5, .timeMs = 187},
        {.note = G_3, .timeMs = 187},
        {.note = A_SHARP_5, .timeMs = 187},
        {.note = F_5, .timeMs = 187},
        {.note = F_3, .timeMs = 187},
        {.note = G_SHARP_3, .timeMs = 187},
        {.note = F_3, .timeMs = 187},
        {.note = SILENCE, .timeMs = 187},
        {.note = G_SHARP_3, .timeMs = 187},
        {.note = A_3, .timeMs = 187},
        {.note = A_SHARP_3, .timeMs = 187},
        {.note = F_3, .timeMs = 187},
        {.note = SILENCE, .timeMs = 187},
        {.note = G_SHARP_3, .timeMs = 187},
        {.note = F_3, .timeMs = 187},
        {.note = SILENCE, .timeMs = 187},
        {.note = B_3, .timeMs = 46},
        {.note = C_4, .timeMs = 140},
        {.note = A_SHARP_3, .timeMs = 187},
        {.note = C_3, .timeMs = 187},
        {.note = C_SHARP_3, .timeMs = 187},
        {.note = F_5, .timeMs = 187},
        {.note = F_3, .timeMs = 187},
        {.note = C_SHARP_3, .timeMs = 187},
        {.note = A_SHARP_5, .timeMs = 187},
        {.note = F_3, .timeMs = 187},
        {.note = G_SHARP_5, .timeMs = 187},
        {.note = G_5, .timeMs = 187},
        {.note = D_SHARP_3, .timeMs = 187},
        {.note = C_6, .timeMs = 187},
        {.note = G_3, .timeMs = 187},
        {.note = D_SHARP_3, .timeMs = 187},
        {.note = A_SHARP_5, .timeMs = 187},
        {.note = A_SHARP_3, .timeMs = 187},
        {.note = C_6, .timeMs = 187},
        {.note = A_SHARP_3, .timeMs = 187},
        {.note = A_SHARP_5, .timeMs = 187},
        {.note = A_SHARP_2, .timeMs = 187},
        {.note = D_3, .timeMs = 187},
        {.note = A_SHARP_2, .timeMs = 187},
        {.note = SILENCE, .timeMs = 187},
        {.note = A_SHARP_2, .timeMs = 187},
        {.note = D_3, .timeMs = 187},
        {.note = F_3, .timeMs = 187},
        {.note = D_SHARP_6, .timeMs = 187},
        {.note = D_SHARP_4, .timeMs = 187},
        {.note = C_4, .timeMs = 187},
        {.note = G_SHARP_3, .timeMs = 187},
        {.note = A_SHARP_3, .timeMs = 187},
        {.note = B_3, .timeMs = 46},
        {.note = C_4, .timeMs = 140},
        {.note = B_3, .timeMs = 46},
        {.note = C_4, .timeMs = 93},
        {.note = SILENCE, .timeMs = 46},
        {.note = C_4, .timeMs = 46},
        {.note = SILENCE, .timeMs = 46},
        {.note = C_4, .timeMs = 46},
        {.note = SILENCE, .timeMs = 46},
        {.note = F_3, .timeMs = 140},
        {.note = SILENCE, .timeMs = 46},
        {.note = F_3, .timeMs = 39},
        {.note = SILENCE, .timeMs = 54},
        {.note = F_3, .timeMs = 39},
        {.note = SILENCE, .timeMs = 54},
        {.note = F_4, .timeMs = 187},
        {.note = F_3, .timeMs = 140},
        {.note = SILENCE, .timeMs = 46},
        {.note = F_3, .timeMs = 39},
        {.note = SILENCE, .timeMs = 54},
        {.note = G_SHARP_3, .timeMs = 39},
        {.note = SILENCE, .timeMs = 54},
        {.note = G_SHARP_4, .timeMs = 187},
        {.note = G_SHARP_3, .timeMs = 187},
        {.note = A_3, .timeMs = 187},
        {.note = A_SHARP_3, .timeMs = 187},
        {.note = A_SHARP_4, .timeMs = 187},
        {.note = A_SHARP_3, .timeMs = 39},
        {.note = SILENCE, .timeMs = 54},
        {.note = A_SHARP_4, .timeMs = 39},
        {.note = SILENCE, .timeMs = 54},
        {.note = A_SHARP_4, .timeMs = 117},
        {.note = SILENCE, .timeMs = 70},
        {.note = A_SHARP_4, .timeMs = 117},
        {.note = SILENCE, .timeMs = 70},
        {.note = A_SHARP_3, .timeMs = 187},
        {.note = G_SHARP_3, .timeMs = 187},
        {.note = A_SHARP_3, .timeMs = 187},
        {.note = F_3, .timeMs = 140},
        {.note = SILENCE, .timeMs = 46},
        {.note = F_3, .timeMs = 39},
        {.note = SILENCE, .timeMs = 54},
        {.note = F_3, .timeMs = 39},
        {.note = SILENCE, .timeMs = 54},
        {.note = F_4, .timeMs = 187},
        {.note = F_3, .timeMs = 140},
        {.note = SILENCE, .timeMs = 46},
        {.note = F_3, .timeMs = 39},
        {.note = SILENCE, .timeMs = 54},
        {.note = G_SHARP_3, .timeMs = 39},
        {.note = SILENCE, .timeMs = 54},
        {.note = G_SHARP_4, .timeMs = 187},
        {.note = G_SHARP_3, .timeMs = 187},
        {.note = A_3, .timeMs = 187},
        {.note = A_SHARP_3, .timeMs = 187},
        {.note = A_SHARP_4, .timeMs = 187},
        {.note = A_SHARP_3, .timeMs = 39},
        {.note = SILENCE, .timeMs = 54},
        {.note = A_SHARP_4, .timeMs = 39},
        {.note = SILENCE, .timeMs = 54},
        {.note = A_SHARP_4, .timeMs = 117},
        {.note = SILENCE, .timeMs = 70},
        {.note = A_SHARP_4, .timeMs = 117},
        {.note = SILENCE, .timeMs = 70},
        {.note = A_SHARP_3, .timeMs = 187},
        {.note = G_SHARP_3, .timeMs = 93},
        {.note = A_SHARP_3, .timeMs = 93},
        {.note = C_4, .timeMs = 187},
        {.note = F_3, .timeMs = 140},
        {.note = SILENCE, .timeMs = 46},
        {.note = F_3, .timeMs = 39},
        {.note = SILENCE, .timeMs = 54},
        {.note = F_3, .timeMs = 39},
        {.note = SILENCE, .timeMs = 54},
        {.note = F_4, .timeMs = 187},
        {.note = F_3, .timeMs = 140},
        {.note = SILENCE, .timeMs = 46},
        {.note = F_3, .timeMs = 39},
        {.note = SILENCE, .timeMs = 54},
        {.note = G_SHARP_3, .timeMs = 39},
        {.note = SILENCE, .timeMs = 54},
        {.note = G_SHARP_4, .timeMs = 187},
        {.note = G_SHARP_3, .timeMs = 187},
        {.note = A_3, .timeMs = 187},
        {.note = A_SHARP_3, .timeMs = 187},
        {.note = A_SHARP_4, .timeMs = 187},
        {.note = A_SHARP_3, .timeMs = 39},
        {.note = SILENCE, .timeMs = 54},
        {.note = A_SHARP_4, .timeMs = 39},
        {.note = SILENCE, .timeMs = 54},
        {.note = A_SHARP_4, .timeMs = 117},
        {.note = SILENCE, .timeMs = 70},
        {.note = A_SHARP_4, .timeMs = 117},
        {.note = SILENCE, .timeMs = 70},
        {.note = A_SHARP_3, .timeMs = 187},
        {.note = G_SHARP_3, .timeMs = 187},
        {.note = A_SHARP_3, .timeMs = 187},
        {.note = F_3, .timeMs = 140},
        {.note = SILENCE, .timeMs = 46},
        {.note = F_3, .timeMs = 39},
        {.note = SILENCE, .timeMs = 54},
        {.note = F_3, .timeMs = 39},
        {.note = SILENCE, .timeMs = 54},
        {.note = F_4, .timeMs = 187},
        {.note = F_3, .timeMs = 140},
        {.note = SILENCE, .timeMs = 46},
        {.note = F_3, .timeMs = 39},
        {.note = SILENCE, .timeMs = 54},
        {.note = G_SHARP_3, .timeMs = 39},
        {.note = SILENCE, .timeMs = 54},
        {.note = G_SHARP_4, .timeMs = 187},
        {.note = G_SHARP_3, .timeMs = 187},
        {.note = A_3, .timeMs = 187},
        {.note = C_SHARP_4, .timeMs = 93},
        {.note = F_4, .timeMs = 93},
        {.note = A_SHARP_4, .timeMs = 93},
        {.note = C_SHARP_5, .timeMs = 93},
        {.note = C_4, .timeMs = 93},
        {.note = D_SHARP_4, .timeMs = 93},
        {.note = G_SHARP_4, .timeMs = 93},
        {.note = C_5, .timeMs = 93},
        {.note = A_SHARP_3, .timeMs = 93},
        {.note = D_SHARP_4, .timeMs = 93},
        {.note = G_4, .timeMs = 93},
        {.note = A_SHARP_4, .timeMs = 93},
        {.note = G_SHARP_3, .timeMs = 187},
        {.note = G_3, .timeMs = 187},
        {.note = F_3, .timeMs = 187},
        {.note = F_5, .timeMs = 93},
        {.note = SILENCE, .timeMs = 93},
        {.note = F_4, .timeMs = 187},
        {.note = F_3, .timeMs = 187},
        {.note = G_SHARP_5, .timeMs = 93},
        {.note = SILENCE, .timeMs = 93},
        {.note = G_SHARP_4, .timeMs = 187},
        {.note = A_5, .timeMs = 93},
        {.note = SILENCE, .timeMs = 93},
        {.note = A_SHARP_5, .timeMs = 93},
        {.note = SILENCE, .timeMs = 93},
        {.note = A_SHARP_3, .timeMs = 187},
        {.note = F_5, .timeMs = 93},
        {.note = SILENCE, .timeMs = 93},
        {.note = A_SHARP_4, .timeMs = 187},
        {.note = F_6, .timeMs = 93},
        {.note = SILENCE, .timeMs = 93},
        {.note = A_SHARP_5, .timeMs = 93},
        {.note = SILENCE, .timeMs = 93},
        {.note = A_SHARP_3, .timeMs = 187},
        {.note = G_SHARP_3, .timeMs = 187},
        {.note = A_SHARP_3, .timeMs = 187},
        {.note = F_3, .timeMs = 187},
        {.note = F_5, .timeMs = 93},
        {.note = SILENCE, .timeMs = 93},
        {.note = F_4, .timeMs = 187},
        {.note = F_3, .timeMs = 187},
        {.note = G_SHARP_5, .timeMs = 93},
        {.note = SILENCE, .timeMs = 93},
        {.note = G_SHARP_4, .timeMs = 187},
        {.note = A_5, .timeMs = 93},
        {.note = SILENCE, .timeMs = 93},
        {.note = A_SHARP_5, .timeMs = 93},
        {.note = SILENCE, .timeMs = 93},
        {.note = A_SHARP_3, .timeMs = 187},
        {.note = F_5, .timeMs = 93},
        {.note = SILENCE, .timeMs = 93},
        {.note = A_SHARP_4, .timeMs = 187},
        {.note = F_6, .timeMs = 93},
        {.note = SILENCE, .timeMs = 93},
        {.note = G_5, .timeMs = 187},
        {.note = G_SHARP_5, .timeMs = 187},
        {.note = G_5, .timeMs = 187},
        {.note = G_SHARP_5, .timeMs = 187},
        {.note = F_3, .timeMs = 187},
        {.note = F_5, .timeMs = 93},
        {.note = SILENCE, .timeMs = 93},
        {.note = F_4, .timeMs = 187},
        {.note = F_3, .timeMs = 187},
        {.note = G_SHARP_5, .timeMs = 93},
        {.note = SILENCE, .timeMs = 93},
        {.note = G_SHARP_4, .timeMs = 187},
        {.note = A_5, .timeMs = 93},
        {.note = SILENCE, .timeMs = 93},
        {.note = A_SHARP_5, .timeMs = 93},
        {.note = SILENCE, .timeMs = 93},
        {.note = A_SHARP_3, .timeMs = 187},
        {.note = F_5, .timeMs = 93},
        {.note = SILENCE, .timeMs = 93},
        {.note = A_SHARP_4, .timeMs = 187},
        {.note = F_6, .timeMs = 93},
        {.note = SILENCE, .timeMs = 93},
        {.note = A_SHARP_5, .timeMs = 93},
        {.note = SILENCE, .timeMs = 93},
        {.note = A_SHARP_3, .timeMs = 187},
        {.note = G_SHARP_3, .timeMs = 187},
        {.note = A_SHARP_3, .timeMs = 187},
        {.note = F_3, .timeMs = 187},
        {.note = F_5, .timeMs = 93},
        {.note = SILENCE, .timeMs = 93},
        {.note = F_4, .timeMs = 187},
        {.note = F_3, .timeMs = 187},
        {.note = G_SHARP_5, .timeMs = 93},
        {.note = SILENCE, .timeMs = 93},
        {.note = G_SHARP_4, .timeMs = 187},
        {.note = A_5, .timeMs = 93},
        {.note = SILENCE, .timeMs = 93},
        {.note = A_SHARP_5, .timeMs = 93},
        {.note = SILENCE, .timeMs = 93},
        {.note = A_SHARP_3, .timeMs = 187},
        {.note = F_5, .timeMs = 93},
        {.note = SILENCE, .timeMs = 93},
        {.note = A_SHARP_5, .timeMs = 93},
        {.note = SILENCE, .timeMs = 93},
        {.note = F_5, .timeMs = 93},
        {.note = SILENCE, .timeMs = 93},
        {.note = C_6, .timeMs = 187},
        {.note = D_SHARP_6, .timeMs = 187},
        {.note = F_6, .timeMs = 46},
        {.note = E_6, .timeMs = 23},
        {.note = D_SHARP_6, .timeMs = 23},
        {.note = C_SHARP_6, .timeMs = 23},
        {.note = C_6, .timeMs = 23},
        {.note = A_5, .timeMs = 23},
        {.note = G_5, .timeMs = 23},
        {.note = F_SHARP_5, .timeMs = 23},
        {.note = E_5, .timeMs = 23},
        {.note = D_SHARP_5, .timeMs = 23},
        {.note = C_SHARP_5, .timeMs = 23},
        {.note = C_5, .timeMs = 23},
        {.note = A_4, .timeMs = 23},
        {.note = G_4, .timeMs = 23},
        {.note = F_SHARP_4, .timeMs = 19},

    },
    .numNotes = 425,
    .shouldLoop = true
};

/*static const song_t sndGameStart =
{
    .notes =
    {
        {C_4, 50},{SILENCE, 50},{C_4, 100},{SILENCE, 200},{C_5, 100},
        {SILENCE, 100},{E_4, 100},{SILENCE, 100},{E_4, 100},{SILENCE, 100},
        {G_4, 100},{C_5, 100},{SILENCE, 100},{G_4, 100}
    },
    .numNotes = 14,
    .shouldLoop = false
};*/

static const song_t sndDie =
{
    .notes =
    {
        {C_SHARP_5, 100},{A_SHARP_4, 100},{G_SHARP_4, 200},{F_SHARP_4, 100},
        {D_SHARP_4, 100},{SILENCE, 100},{D_4, 100},{SILENCE, 100},
        {C_SHARP_4, 100}
    },
    .numNotes = 9,
    .shouldLoop = false
};

static const song_t sndMenuSelect =
{
    .notes =
    {
        {C_5, 50},{C_4, 50}
    },
    .numNotes = 2,
    .shouldLoop = false
};

static const song_t sndMenuConfirm =
{
    .notes =
    {
        {C_6, 50},{C_5, 50},{C_4, 50}
    },
    .numNotes = 2,
    .shouldLoop = false
};

static const song_t sndMenuDeny =
{
    .notes =
    {
        {C_3, 50},{SILENCE, 50},{C_3, 50}
    },
    .numNotes = 3,
    .shouldLoop = false
};

static const song_t sndPause =
{
    .notes = 
    {
        {G_6, 80},{E_6, 80},{G_6, 80},{E_6, 80}
    },
    .numNotes = 4,
    .shouldLoop = false
};

static const song_t sndHit =
    {
        .notes =
            {
                {C_4, 25}, {C_5, 25}},
        .numNotes = 2,
        .shouldLoop = false
    };

static const song_t sndSquish =
    {
        .notes =
            {
                {740, 10}, {840, 10}, {940, 10}},
        .numNotes = 3,
        .shouldLoop = false
    };

static const song_t sndBreak =
    {
        .notes =
            {
                {C_5, 25}, {C_4, 25}, {A_4, 25}, {A_SHARP_3, 25}, {A_3, 25}},
        .numNotes = 5,
        .shouldLoop = false
    };

static const song_t sndCoin =
    {
        .notes =
            {
                {1000, 50}, {1200, 100}},
        .numNotes = 2,
        .shouldLoop = false
    };

static const song_t sndPowerUp =
    {
        .notes =
            { 
                {C_4, 100}, {G_4, 100}, {E_4, 100}, {C_5, 100}, {G_4, 100}, {D_5, 100}, {C_5, 100}},
        .numNotes = 7,
        .shouldLoop = false
    };

static const song_t sndJump1 =
    {
        .notes =
            { 
                {C_5, 50}, {E_5, 50}, {C_5, 50}},
        .numNotes = 2,
        .shouldLoop = false
    };         

static const song_t sndJump2 =
    {
        .notes =
            { 
                {E_5, 50}, {G_5, 50}, {E_5, 50}},
        .numNotes = 2,
        .shouldLoop = false
    };         

static const song_t sndJump3 =
    {
        .notes =
            { 
                {G_5, 50}, {C_6, 50}, {G_5, 50}},
        .numNotes = 2,
        .shouldLoop = false
    };

static const song_t sndWarp =
    {
        .notes =
            { 
                {D_5, 50}, {A_4, 50}, {E_4, 50},{D_5, 50}, {A_4, 50}, {E_4, 50},{D_5, 50}, {A_4, 50}, {E_4, 50}
            },
        .numNotes = 9,
        .shouldLoop = false
    };

static const song_t sndHurt =
    {
        .notes =
            { 
                {E_4, 50}, {D_SHARP_4, 50}, {D_4, 50},{C_SHARP_4, 50}, {C_4, 50}, {C_5, 50}, {C_4, 50}
            },
        .numNotes = 6,
        .shouldLoop = false
    };   

static const song_t sndWaveBall =
{
    .notes =
    {
        {D_4, 50},{D_5, 50},{A_6, 50},{A_5, 50}
    },
    .numNotes = 4,
    .shouldLoop = false
};    

static const song_t snd1up =
{
    .notes = 
    {
        {G_7, 40},{D_6, 40},{B_5, 80}
    },
    .numNotes = 3,
    .shouldLoop = false
};

#endif