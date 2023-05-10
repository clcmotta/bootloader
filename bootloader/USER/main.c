#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "flash.h"
#include "iap.h"
#include <stdio.h>

int main(void)
{
	u8 t;
	u16 oldcount=0;				
	u16 applength=0;				
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 
	uart_init(115200);	//串口初始化为115200
	delay_init();	   	 	//延时初始化 
	printf("bootloader running\r\n");
	
	while(1)
	{	
		if(USART_RX_CNT){
			if(oldcount == USART_RX_CNT){
				applength = USART_RX_CNT;
				oldcount = 0;
				USART_RX_CNT = 0;
				printf("successful\r\n");
			}
			else{
				oldcount = USART_RX_CNT;
			}
		}
		t++;
		delay_ms(10);
		if(applength){
			printf("firmware begin\r\n");
			printf("applength is %d\r\n",applength);
			if(((*(vu32*)(0x20001000+4))&0xff000000) == 0x08000000){
				iap_write_appbin(FLASH_APP1_ADDR, USART_RX_BUF, applength);
				printf("firmware successful\r\n");
				iap_load_app(FLASH_APP1_ADDR);
			}
			else {
				printf("no flash\r\n");
			}
		}
		else {
			printf("no firmware\r\n");
		}
	}
}


 

