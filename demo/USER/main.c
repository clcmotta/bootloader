#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "iap.h"
#include "led.h"
#include <stdio.h>
#include <string.h>

#define BOOTLOADER_ADDR		0x08070000

iapfun jump2app; 

void iap_load_app(u32 appxaddr)
{
	if(((*(vu32*)appxaddr)&0x2FFE0000)==0x20000000)	//���ջ����ַ�Ƿ�Ϸ�.
	{ 
		jump2app=(iapfun)*(vu32*)(appxaddr+4);		//�û��������ڶ�����Ϊ����ʼ��ַ(��λ��ַ)		
		MSR_MSP(*(vu32*)appxaddr);					//��ʼ��bootloader��ջָ��(�û��������ĵ�һ�������ڴ��ջ����ַ)
		jump2app();									//��ת��bootloader.
	}
}	

int main(void)
{
	LED_Init();
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 
	uart_init(115200);	//���ڳ�ʼ��Ϊ115200
	delay_init();	   	 	//��ʱ��ʼ�� 
	printf("demo running\r\n");
	
	while(1)
	{	if(USART_RX_CNT > 2){
			printf("recv str is %s\r\n",USART_RX_BUF);
			if(strcmp((char *)USART_RX_BUF,"OK")){
				iap_load_app(BOOTLOADER_ADDR);
			}
		}
		GPIO_ResetBits(GPIOB,GPIO_Pin_5);
		delay_ms(100);
	}
}


 

