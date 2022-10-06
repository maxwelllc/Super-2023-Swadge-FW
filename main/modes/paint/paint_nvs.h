#ifndef _PAINT_NVS_H_
#define _PAINT_NVS_H_

#include <stdint.h>
#include <stdbool.h>
#include <malloc.h>

#include "nvs_manager.h"

#include "paint_common.h"
#include "paint_type.h"

void paintLoadIndex(void);
void paintSaveIndex(void);
void paintResetStorage(void);
bool paintGetSlotInUse(uint8_t slot);
void paintClearSlot(uint8_t slot);
void paintSetSlotInUse(uint8_t slot);
bool paintGetAnySlotInUse(void);
uint8_t paintGetRecentSlot(void);
void paintSetRecentSlot(uint8_t slot);
bool paintSave(const paintCanvas_t* canvas, uint8_t slot);
bool paintLoad(paintCanvas_t* canvas, uint8_t slot);
bool paintLoadDimensions(paintCanvas_t* canvas, uint8_t slot);
uint8_t paintGetPrevSlotInUse(uint8_t slot);
uint8_t paintGetNextSlotInUse(uint8_t slot);


#endif