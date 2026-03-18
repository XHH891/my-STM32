#include "stm32f10x.h"                  // Device header
#include "Delay.h"

/***********光敏电阻**********/
void lightsensor_init(void);
uint8_t lightsensor_get(void);

/***********蜂鸣器************/
void buzzer_init(void);//蜂鸣器初始化函数
void buzzer_off(void);//关闭
void buzzer_on(void);//打开

int main(void)
{
	uint8_t k;
	
	buzzer_init();
	lightsensor_init();
	
	while(1)
	{
		if(lightsensor_get() == 1)
			buzzer_on();
		else
			buzzer_off();
	}
}

void lightsensor_init(void)//光敏电阻初始化函数
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Pin =GPIO_Pin_13;
	GPIO_InitStruct.GPIO_Speed =GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOB,&GPIO_InitStruct);
}

uint8_t lightsensor_get(void)
{
	return GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_13);
}

void buzzer_init(void)//蜂鸣器初始化函数
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin =GPIO_Pin_12;
	GPIO_InitStruct.GPIO_Speed =GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOB,&GPIO_InitStruct);
}

void buzzer_off(void)//关闭
{
	GPIO_SetBits(GPIOB,GPIO_Pin_12);
}
void buzzer_on(void)//打开
{
	GPIO_ResetBits(GPIOB,GPIO_Pin_12);
}

