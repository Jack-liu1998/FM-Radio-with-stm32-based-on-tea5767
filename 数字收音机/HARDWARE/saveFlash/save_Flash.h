#ifndef _SAVE_FLASH_H_
#define _SAVA_FLASH_H_
#include "stm32f10x.h"
#define Base_Address 0x0807f800
#define End_Address 0x0807ffff
void save(uint32_t * data);
void load(uint32_t* data);
#endif
