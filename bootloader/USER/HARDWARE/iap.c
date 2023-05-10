#include "iap.h"
#include "flash.h"	  
#include "sys.h"

iapfun jump2app;
u16 iapbuf[1024];

void iap_write_appbin(u32 appxaddr, u8 *appbuf, u32 appsize)
{
	u16 t;
	u16 i=0;
	u16 temp;
	u32 fwaddr = appxaddr; 
	u8 *dfu = appbuf;
	for(t=0; t<appsize; t+=2){
		temp = (u16)dfu[1]<<8;
		temp += (u16)dfu[0];
		dfu += 2;
		iapbuf[i++] = temp;
		
		if(i == 1024){
			i = 0;
			STMFLASH_Write(fwaddr, iapbuf, 1024);
			fwaddr += 2048;
		}
	}
	if(i)STMFLASH_Write(fwaddr, iapbuf, i);
}

void iap_load_app(u32 appxaddr)
{
	if(((*(vu32*)appxaddr)&0x2ffe0000) == 0x20000000){
		jump2app = (iapfun)*(vu32*)(appxaddr+4);
		MSR_MSP(*(vu32*)appxaddr);
		jump2app();
	}
}

