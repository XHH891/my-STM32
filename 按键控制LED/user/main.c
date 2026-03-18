#include "stm32f10x.h"                  // Device header
#include "Delay.h"

/*********按键相关*********/
void key_init(void);//按键初始化
uint8_t key_getnum(void);//判断按键是否按下


/*********led相关**********/
void led_init(void);//led初始化函数
void led2_on(void);//打开LED2
void led2_off(void);//关闭LED2	
void led1_on(void);//打开LED1
void led1_off(void);//关闭LED1

int main(void)
{
	uint8_t k;
	
	key_init();
	led_init();

	
	while(1)
	{
		k = key_getnum();
		if(k == 1)
		{
			led1_on();
		}
		if(k == 2)
		{
			led1_off();
		}
	}
}


void key_init(void)//按键初始化
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;//上拉输入
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_11;
	GPIO_InitStruct.GPIO_Speed =GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOB,&GPIO_InitStruct);
}

uint8_t key_getnum(void)//判断按键是否按下
{
	uint8_t keynum = 0;
	
	if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1) == 0)
	{
		Delay_ms(20);
		while(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1) == 0);
		Delay_ms(20);
		keynum = 1;
	}
	
	if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_11) == 0)
	{
		Delay_ms(20);
		while(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_11) == 0);
		Delay_ms(20);
		keynum = 2;
	}
	
	return keynum;
}

void led_init(void)//led初始化函数
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2;
	GPIO_InitStruct.GPIO_Speed =GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOA,&GPIO_InitStruct);
}

void led1_off(void)//关闭LED1
{
	GPIO_SetBits(GPIOA,GPIO_Pin_1);
}
void led1_on(void)//打开LED1
{
	GPIO_ResetBits(GPIOA,GPIO_Pin_1);
}
void led2_off(void)//关闭LED2
{
	GPIO_SetBits(GPIOA,GPIO_Pin_2);
}
void led2_on(void)//打开LED2
{
	GPIO_ResetBits(GPIOA,GPIO_Pin_2);
}
