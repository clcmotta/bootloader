#include "flash.h"
#include "usart.h"
#include "delay.h"

u16 FLASH_ReadHalfWord(u32 faddr)
{
	return *(vu16*)faddr;
}

void FLASH_Read(u32 addr, u16 *pbuff, u16 numToRead)
{
	u16 i;
	for(i=0; i<numToRead; i++){
		pbuff[i] = FLASH_ReadHalfWord(addr);
		addr += 2;
	}
}

void FLASH_Write_NoCheck(u32 WriteAddr, u16* pbuff, u16 numtoWrite)
{
	u16 i;
	for(i=0; i<numtoWrite; i++){
		FLASH_ProgramHalfWord(WriteAddr, pbuff[i]);
		WriteAddr+=2;
	}
}

#if STM_FLASH_SIZE<256
#define STM_SECTOR_SIZE		1024
#else 
#define STM_SECTOR_SIZE		2048
#endif

u16 STMFLASH_BUF[STM_SECTOR_SIZE/2];

void STMFLASH_Write(u32 writeAddr, u16 *pbuff, u16 numToWrite)	
{
	u32 secpos;
	u16 secoff;
	u16 secremain;
	u16 i;
	u32 offaddr;
	
	if(writeAddr<STM_FLASH_BASE||(writeAddr>=(STM_FLASH_BASE+1024*STM_FLASH_SIZE))){
		return ;
	}
	
	FLASH_Unlock();
	
	offaddr = writeAddr - STM_FLASH_BASE;
	secpos = offaddr/STM_SECTOR_SIZE;
	secoff = (offaddr%STM_SECTOR_SIZE)/2;
	secremain = STM_SECTOR_SIZE/2-secoff;
	if(numToWrite <= secremain) secremain = numToWrite;
	while(1)
	{
		FLASH_Read(secpos*STM_SECTOR_SIZE+STM_FLASH_BASE, STMFLASH_BUF, STM_SECTOR_SIZE/2);
		for(i=0; i<secremain; i++){
			if(STMFLASH_BUF[secoff+i] != 0xffff)break;
		}
		if(i<secremain){
			FLASH_ErasePage(secpos*STM_SECTOR_SIZE+STM_FLASH_BASE);
			for(i=0;i<secremain;i++){
				STMFLASH_BUF[i+secoff] = pbuff[i];
			}
			FLASH_Write_NoCheck(secpos*STM_SECTOR_SIZE+STM_FLASH_BASE, STMFLASH_BUF, STM_SECTOR_SIZE/2);
		}
		else{
			FLASH_Write_NoCheck(writeAddr, pbuff, secremain);
		}		   
		if(numToWrite == secremain)break;
		else{
			secpos++;
			secoff = 0;
			pbuff += secremain;
			writeAddr += secremain;
			numToWrite -= secremain;
			if(numToWrite>(STM_SECTOR_SIZE/2)) secremain = STM_SECTOR_SIZE/2;
			else secremain = numToWrite;
		}
	}
	
	FLASH_Lock();
}

void Test_Write(u32 WriteAddr, u16 writeData)
{
	STMFLASH_Write(WriteAddr, &writeData, 1);
}
