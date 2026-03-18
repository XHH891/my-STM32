#include "stm32f10x.h"                  // Device header
#include "Delay.h"

int main(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin =GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStruct.GPIO_Speed =GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	while(1)
	{
		GPIO_Write(GPIOA,~0x0001); 
		Delay_ms(100);
		GPIO_Write(GPIOA,~0x0002);
		Delay_ms(100);
		GPIO_Write(GPIOA,~0x0004);
		Delay_ms(100);
		GPIO_Write(GPIOA,~0x0008);
		Delay_ms(100);
		GPIO_Write(GPIOA,~0x0010);
		Delay_ms(100);
		GPIO_Write(GPIOA,~0x0020);
		Delay_ms(100);
		GPIO_Write(GPIOA,~0x0040);
		Delay_ms(100);
		GPIO_Write(GPIOA,~0x0080);
		Delay_ms(100);
	}
}
