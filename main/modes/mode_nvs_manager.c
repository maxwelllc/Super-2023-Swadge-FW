/*
 * mode_nvs_manager.c
 *
 *  Created on: 3 Dec 2022
 *      Author: bryce and dylwhich
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

#include "bresenham.h"
#include "display.h"
#include "embeddednf.h"
#include "embeddedout.h"
#include "esp_timer.h"
#include "led_util.h"
#include "linked_list.h"
#include "meleeMenu.h"
#include "mode_main_menu.h"
#include "mode_test.h"
#include "nvs_manager.h"
#include "settingsManager.h"
#include "touch_sensor.h"
#if defined(EMU)
#include "emu_main.h"
#endif

#include "mode_nvs_manager.h"

/*==============================================================================
 * Defines
 *============================================================================*/

#define CORNER_OFFSET 14
//#define TOP_TEXT_X_MARGIN CORNER_OFFSET / 2
#define LINE_BREAK_Y 5
#define ENTRIES_BUF_SIZE 14

/// Helper macro to return an integer clamped within a range (MIN to MAX)
//#define CLAMP(X, MIN, MAX) ( ((X) > (MAX)) ? (MAX) : ( ((X) < (MIN)) ? (MIN) : (X)) )
//#define lengthof(x) (sizeof(x) / sizeof(x[0]))

/*==============================================================================
 * Enums
 *============================================================================*/



/*==============================================================================
 * Prototypes
 *============================================================================*/

void nvsManagerEnterMode(display_t* disp);
void nvsManagerExitMode(void);
void nvsManagerButtonCallback(buttonEvt_t* evt);
void nvsManagerTouchCallback(touch_event_t* evt);
void nvsManagerMainLoop(int64_t elapsedUs);
void nvsManagerSetUpTopMenu(bool resetPos);
void nvsManagerTopLevelCb(const char* opt);
void nvsManagerSetUpManageDataMenu(bool resetPos);
void nvsManagerManageDataCb(const char* opt);

/*==============================================================================
 * Structs
 *============================================================================*/

/// @brief Defines each separate screen in the NVS manager mode.
typedef enum
{
    // Top menu
    NVS_MENU,
    // Summary of used, free, and total entries in NVS
    NVS_SUMMARY,
    // Manage key/value pairs in NVS
    NVS_MANAGE_DATA,
} nvsScreen_t;

/*==============================================================================
 * Variables
 *============================================================================*/

// The swadge mode
swadgeMode modeNvsManager =
{
    .modeName = "Save Data Mgr",
    .fnEnterMode = nvsManagerEnterMode,
    .fnExitMode = nvsManagerExitMode,
    .fnButtonCallback = nvsManagerButtonCallback,
    .fnTouchCallback = nvsManagerTouchCallback,
    .fnMainLoop = nvsManagerMainLoop,
    .wifiMode = NO_WIFI,
    .fnEspNowRecvCb = NULL,
    .fnEspNowSendCb = NULL,
    .fnAccelerometerCallback = NULL,
    .fnAudioCallback = NULL,
    .overrideUsb = false
};

// The state data
typedef struct
{
    display_t* disp;
    font_t ibm_vga8;
    font_t radiostars;
    font_t mm;

    // Menu
    meleeMenu_t* menu;
    uint8_t topLevelPos;
    uint16_t manageDataPos;
    // The screen within NVS manager that the user is in
    nvsScreen_t screen;
    bool eraseDataSelected;
    bool eraseDataConfirm;

    // Track touch
    bool touchHeld;
    int32_t touchPosition;
    int32_t touchIntensity;

    // NVS
    nvs_stats_t nvsStats;
    nvs_entry_info_t* nvsKeys;
} nvsManager_t;

nvsManager_t* nvsManager;

/*==============================================================================
 * Const Variables
 *============================================================================*/

const char str_summary[] = "Summary";
const char str_manage_data[] = "Manage Data";
const char str_factory_reset[] = "Factory Reset";
const char str_confirm_no[] = "Confirm: No!";
const char str_confirm_yes[] = "Confirm: Yes";
const char str_back[] = "Back";
const char str_exit[] = "Exit";

const paletteColor_t color_summary_text = c555;
const paletteColor_t color_summary_h_rule = c222;
const paletteColor_t color_summary_used = c134;
const paletteColor_t color_summary_free = c333;
const char str_non_volatile_storage[] = "Non-Volatile Storage (nvs:)";
const char str_nvs[] = "NVS";
const char str_type[] = "Type:";
const char str_local_flash_part[] = "Local Flash Partition";
const char str_file_system[] = "File system:";
const char str_used_space[] = "Used space:";
const char str_namespaces[] = "Namespaces:";
const char str_free_space[] = "Free space:";
const char str_capacity[] = "Capacity:";
const char str_entries_format[] = "%zu entries";

/*============================================================================
 * Functions
 *==========================================================================*/

/**
 * Initializer for nvsManager
 */
void  nvsManagerEnterMode(display_t* disp)
{
    // Allocate zero'd memory for the mode
    nvsManager = calloc(1, sizeof(nvsManager_t));

    nvsManager->disp = disp;

    loadFont("ibm_vga8.font", &nvsManager->ibm_vga8);
    loadFont("radiostars.font", &nvsManager->radiostars);
    loadFont("mm.font", &nvsManager->mm);

    // TODO: handle errors
    readAllNvsEntryInfos(&nvsManager->nvsStats, &nvsManager->nvsKeys);

    // Initialize the menu
    nvsManager->menu = initMeleeMenu(modeNvsManager.modeName, &nvsManager->mm, nvsManagerTopLevelCb);
    nvsManagerSetUpTopMenu(true);
}

/**
 * Called when nvsManager is exited
 */
void  nvsManagerExitMode(void)
{
    deinitMeleeMenu(nvsManager->menu);

    freeFont(&nvsManager->ibm_vga8);
    freeFont(&nvsManager->radiostars);
    freeFont(&nvsManager->mm);

    if(nvsManager->nvsKeys != NULL)
    {
        free(nvsManager->nvsKeys);
    }

    free(nvsManager);
}

/**
 * This function is called when a button press is pressed. Buttons are
 * handled by interrupts and queued up for this callback, so there are no
 * strict timing restrictions for this function.
 *
 * @param evt The button event that occurred
 */
void  nvsManagerTouchCallback(touch_event_t* evt)
{
    nvsManager->touchHeld = evt->state != 0;
    nvsManager->touchPosition = roundf((evt->position * nvsManager->disp->w) / 255);
}

/**
 * @brief Button callback function, plays notes and switches parameters
 *
 * @param evt The button event that occurred
 */
void  nvsManagerButtonCallback(buttonEvt_t* evt)
{
    switch (nvsManager->screen)
    {
        case NVS_MENU:
        {
            if (evt->down)
            {
                const char* selectedOption = nvsManager->menu->rows[nvsManager->menu->selectedRow];

                switch (evt->button)
                {
                    case LEFT:
                    case RIGHT:
                    {
                        if (str_confirm_no == selectedOption)
                        {
                            nvsManager->eraseDataConfirm = true;
                            nvsManagerSetUpTopMenu(false);
                        }
                        else if (str_confirm_yes == selectedOption)
                        {
                            nvsManager->eraseDataConfirm = false;
                            nvsManagerSetUpTopMenu(false);
                        }

                        break;
                    }
                    case BTN_B:
                    {
                        if (nvsManager->eraseDataSelected)
                        {
                            nvsManager->eraseDataSelected = false;
                            nvsManagerSetUpTopMenu(false);
                        }

                        break;
                    }
                    default:
                    {
                        meleeMenuButton(nvsManager->menu, evt->button);
                        selectedOption = nvsManager->menu->rows[nvsManager->menu->selectedRow];
                        break;
                    }
                }

                if (nvsManager->eraseDataSelected && str_confirm_no != selectedOption &&
                    str_confirm_yes != selectedOption)
                {
                    // If the confirm-erase option is not selected, reset eraseDataConfirm and redraw the menu
                    nvsManager->topLevelPos = nvsManager->menu->selectedRow;
                    nvsManager->eraseDataSelected = false;
                    nvsManager->eraseDataConfirm = false;
                    nvsManagerSetUpTopMenu(false);
                }
            }

            break;
        }
        case NVS_SUMMARY:
        {
            if(evt->down && evt->button == BTN_B)
            {
                nvsManagerSetUpTopMenu(false);
            }

            break;
        }
        case NVS_MANAGE_DATA:
        {
            if(evt->down)
            {
                if(evt->button == BTN_B)
                {
                    nvsManagerSetUpTopMenu(false);
                    break;
                }

                meleeMenuButton(nvsManager->menu, evt->button);
            }
            
            break;
        }
        default:
        {
            break;
        }
    }
}

/**
 * Update the display by drawing the current state of affairs
 */
void  nvsManagerMainLoop(int64_t elapsedUs)
{
    switch(nvsManager->screen)
    {
        case NVS_MENU:
        case NVS_MANAGE_DATA:
        {
            // Draw the menu
            drawMeleeMenu(nvsManager->disp, nvsManager->menu);
            break;
        }
        case NVS_SUMMARY:
        {
            nvsManager->disp->clearPx();

            led_t leds[NUM_LEDS];
            memset(leds, 0, NUM_LEDS * sizeof(led_t));
            setLeds(leds, NUM_LEDS);

            char buf[ENTRIES_BUF_SIZE];

            // Partition name
            int16_t yOff = CORNER_OFFSET;
            drawText(nvsManager->disp, &nvsManager->ibm_vga8, color_summary_text, str_non_volatile_storage, CORNER_OFFSET, yOff);

            yOff += nvsManager->ibm_vga8.h + LINE_BREAK_Y + 1;
            plotLine(nvsManager->disp, CORNER_OFFSET, yOff, nvsManager->disp->w - CORNER_OFFSET, yOff, color_summary_h_rule, 0);

            // Partition type
            yOff += LINE_BREAK_Y + 1;
            drawText(nvsManager->disp, &nvsManager->ibm_vga8, color_summary_text, str_type, CORNER_OFFSET, yOff);
            drawText(nvsManager->disp, &nvsManager->ibm_vga8, color_summary_text, str_local_flash_part, nvsManager->disp->w - textWidth(&nvsManager->ibm_vga8, str_local_flash_part) - CORNER_OFFSET, yOff);

            // Partition file system
            yOff += nvsManager->ibm_vga8.h + LINE_BREAK_Y;
            drawText(nvsManager->disp, &nvsManager->ibm_vga8, color_summary_text, str_file_system, CORNER_OFFSET, yOff);
            drawText(nvsManager->disp, &nvsManager->ibm_vga8, color_summary_text, str_nvs, nvsManager->disp->w - textWidth(&nvsManager->ibm_vga8, str_nvs) - CORNER_OFFSET, yOff);

            yOff += nvsManager->ibm_vga8.h + LINE_BREAK_Y + 1;
            plotLine(nvsManager->disp, CORNER_OFFSET, yOff, nvsManager->disp->w - CORNER_OFFSET, yOff, color_summary_h_rule, 0);

            // Used space
            yOff += LINE_BREAK_Y + 1;
            fillDisplayArea(nvsManager->disp, CORNER_OFFSET, yOff, CORNER_OFFSET + nvsManager->ibm_vga8.h, yOff + nvsManager->ibm_vga8.h, color_summary_used);
            drawText(nvsManager->disp, &nvsManager->ibm_vga8, color_summary_text, str_used_space, CORNER_OFFSET + nvsManager->ibm_vga8.h + LINE_BREAK_Y, yOff);
            snprintf(buf, ENTRIES_BUF_SIZE, str_entries_format, nvsManager->nvsStats.used_entries);
            drawText(nvsManager->disp, &nvsManager->ibm_vga8, color_summary_text, buf, nvsManager->disp->w - textWidth(&nvsManager->ibm_vga8, buf) - CORNER_OFFSET, yOff);
            
            // Namespaces
            yOff += nvsManager->ibm_vga8.h + LINE_BREAK_Y;
            drawText(nvsManager->disp, &nvsManager->ibm_vga8, color_summary_text, str_namespaces, CORNER_OFFSET + nvsManager->ibm_vga8.h + LINE_BREAK_Y, yOff);
            snprintf(buf, ENTRIES_BUF_SIZE, str_entries_format, nvsManager->nvsStats.namespace_count);
            int16_t tWidth = textWidth(&nvsManager->ibm_vga8, buf);
            snprintf(buf, ENTRIES_BUF_SIZE, "%zu", nvsManager->nvsStats.namespace_count);
            drawText(nvsManager->disp, &nvsManager->ibm_vga8, color_summary_text, buf, nvsManager->disp->w - tWidth - CORNER_OFFSET, yOff);

            // Free space
            yOff += nvsManager->ibm_vga8.h + LINE_BREAK_Y;
            fillDisplayArea(nvsManager->disp, CORNER_OFFSET, yOff, CORNER_OFFSET + nvsManager->ibm_vga8.h, yOff + nvsManager->ibm_vga8.h, color_summary_free);
            drawText(nvsManager->disp, &nvsManager->ibm_vga8, color_summary_text, str_free_space, CORNER_OFFSET + nvsManager->ibm_vga8.h + LINE_BREAK_Y, yOff);
            snprintf(buf, ENTRIES_BUF_SIZE, str_entries_format, nvsManager->nvsStats.free_entries);
            drawText(nvsManager->disp, &nvsManager->ibm_vga8, color_summary_text, buf, nvsManager->disp->w - textWidth(&nvsManager->ibm_vga8, buf) - CORNER_OFFSET, yOff);

            yOff += nvsManager->ibm_vga8.h + LINE_BREAK_Y + 1;
            plotLine(nvsManager->disp, CORNER_OFFSET, yOff, nvsManager->disp->w - CORNER_OFFSET, yOff, color_summary_h_rule, 0);

            // Capacity
            yOff += LINE_BREAK_Y + 1;
            drawText(nvsManager->disp, &nvsManager->ibm_vga8, color_summary_text, str_capacity, CORNER_OFFSET + nvsManager->ibm_vga8.h + LINE_BREAK_Y, yOff);
            snprintf(buf, ENTRIES_BUF_SIZE, str_entries_format, nvsManager->nvsStats.total_entries);
            drawText(nvsManager->disp, &nvsManager->ibm_vga8, color_summary_text, buf, nvsManager->disp->w - textWidth(&nvsManager->ibm_vga8, buf) - CORNER_OFFSET, yOff);

            yOff += nvsManager->ibm_vga8.h + LINE_BREAK_Y;
            int16_t xStart = CORNER_OFFSET + nvsManager->ibm_vga8.h + LINE_BREAK_Y;
            int16_t xEnd = nvsManager->disp->w - CORNER_OFFSET - nvsManager->ibm_vga8.h - LINE_BREAK_Y;
            fillDisplayArea(nvsManager->disp, xStart, yOff, xStart + roundf((float_t)nvsManager->nvsStats.used_entries / nvsManager->nvsStats.total_entries * (xEnd - xStart)), yOff + nvsManager->ibm_vga8.h, color_summary_used);
            fillDisplayArea(nvsManager->disp, xEnd - roundf((float_t)nvsManager->nvsStats.free_entries / nvsManager->nvsStats.total_entries * (xEnd - xStart)), yOff, xEnd, yOff + nvsManager->ibm_vga8.h, color_summary_free);

            break;
        }
    }
}

/**
 * Set up the top level menu
 *
 * @param resetPos true to reset the position to 0, false to leave it where it is
 */
void nvsManagerSetUpTopMenu(bool resetPos)
{
    // Set up the menu
    resetMeleeMenu(nvsManager->menu, modeNvsManager.modeName, nvsManagerTopLevelCb);
    addRowToMeleeMenu(nvsManager->menu, str_summary);
    addRowToMeleeMenu(nvsManager->menu, str_manage_data);

    // Add the row for factory resetting the Swadge
    if (nvsManager->eraseDataSelected)
    {
        if (nvsManager->eraseDataConfirm)
        {
            addRowToMeleeMenu(nvsManager->menu, str_confirm_yes);
        }
        else
        {
            addRowToMeleeMenu(nvsManager->menu, str_confirm_no);
        }
    }
    else
    {
        addRowToMeleeMenu(nvsManager->menu, str_factory_reset);
    }

    addRowToMeleeMenu(nvsManager->menu, str_exit);

    // Set the position
    if(resetPos)
    {
        nvsManager->topLevelPos = 0;
    }
    nvsManager->menu->selectedRow = nvsManager->topLevelPos;
    nvsManager->menu->usePerRowXOffsets = true;

    nvsManager->screen = NVS_MENU;
}

/**
 * Callback for the top level menu
 *
 * @param opt The menu option which was selected
 */
void nvsManagerTopLevelCb(const char* opt)
{
    // Save the position
    nvsManager->topLevelPos = nvsManager->menu->selectedRow;

    // Handle the option
    if(str_summary == opt)
    {
        nvsManager->screen = NVS_SUMMARY;
    }
    else if(str_manage_data == opt)
    {
        nvsManagerSetUpManageDataMenu(true);
    }
    else if(str_factory_reset == opt)
    {
        nvsManager->eraseDataSelected = true;
        nvsManagerSetUpTopMenu(false);
    }
    else if (str_confirm_yes == opt)
    {
        if(eraseNvs())
        {
#ifdef EMU
            emu_quit();
#else
            switchToSwadgeMode(&modeTest);
#endif
        }
        else
        {
#ifdef EMU
            exit(1);
#else
            switchToSwadgeMode(&modeMainMenu);
#endif
        }
        nvsManager->eraseDataConfirm = false;
        nvsManager->eraseDataSelected = false;
    }
    else if (str_confirm_no == opt)
    {
        nvsManager->eraseDataSelected = false;
        nvsManager->eraseDataConfirm = false;
        nvsManagerSetUpTopMenu(false);
    }
    else if(str_exit == opt)
    {
        switchToSwadgeMode(&modeMainMenu);
    }
}

/**
 * Set up the data management menu
 *
 * @param resetPos true to reset the position to 0, false to leave it where it is
 */
void nvsManagerSetUpManageDataMenu(bool resetPos)
{
    // Set up the menu
    resetMeleeMenu(nvsManager->menu, modeNvsManager.modeName, nvsManagerManageDataCb);

    for(size_t i = 0; i < nvsManager->nvsStats.used_entries; i++)
    {
        if(nvsManager->nvsKeys[i].key[0] != '\0')
        {
            addRowToMeleeMenu(nvsManager->menu, nvsManager->nvsKeys[i].key);
        }
    }

    addRowToMeleeMenu(nvsManager->menu, str_back);

    // Set the position
    if(resetPos)
    {
        nvsManager->manageDataPos = 0;
    }
    nvsManager->menu->selectedRow = nvsManager->manageDataPos;
    nvsManager->menu->usePerRowXOffsets = false;

    nvsManager->screen = NVS_MANAGE_DATA;
}

/**
 * Callback for the data management menu
 *
 * @param opt The menu option which was selected
 */
void nvsManagerManageDataCb(const char* opt)
{
    // Save the position
    nvsManager->manageDataPos = nvsManager->menu->selectedRow;

    // Handle the option
    if(str_manage_data == opt)
    {
        nvsManagerSetUpManageDataMenu(true);
    }
    else if(str_back == opt)
    {
        nvsManagerSetUpTopMenu(false);
    }
}
