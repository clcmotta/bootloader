#ifndef __FLASH_H__
#define __FLASH_H__

#include "stm32f10x.h"

#define STM_FLASH_SIZE			512

#define STM_FLASH_BASE			0X08000000

void FLASH_Read(u32 addr, u16 *pbuff, u16 numToRead);
void STMFLASH_Write(u32 writeAddr, u16 *pbuff, u16 numToWrite);

#endif
