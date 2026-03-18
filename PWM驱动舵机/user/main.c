#include "stm32f10x.h"                  // Device header
#include "OLED.h"
#include "Delay.h"

void PWM_init(void);
void PWM_setccr(uint16_t compare);//更改CCR的值
void servo_setangle(float angle);//
void key_init(void);//按键初始化
uint8_t key_getnum(void);//判断按键是否按下

int main(void)
{
	uint8_t k,a=0;
	OLED_Init(); 
	PWM_init();
	key_init();
	
	OLED_ShowString(1,1,"Angle:");
	while(1)
	{
		//k = key_getnum();
		//if(k == 1)
		//{
			//a += 30;
			//if(a>180)
				//a = 0;
		//}
		
		for(a = 0;a<=180;a++)
		{
			servo_setangle(a);
			Delay_ms(5);
			OLED_ShowNum(1,7,a,3);
		}
			
		for(a = 0;a<=180;a++)
		{
			servo_setangle(180 - a);
			Delay_ms(5);
			OLED_ShowNum(1,7,180-a,3);
		}
		
	}
}

void PWM_init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);	//开启TIM2的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//开启GPIOA外设的时钟

	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Pin =GPIO_Pin_1;
	GPIO_InitStruct.GPIO_Speed =GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);//将配置参数写入GPIOA寄存器，使配置生效
	
	/*配置时钟源*/
	TIM_InternalClockConfig(TIM2);		//选择TIM2为内部时钟，若不调用此函数，TIM默认也为内部时钟
	
	/*时基单元初始化*/
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;				//定义结构体变量
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;		//时钟分频，选择不分频，此参数用于配置滤波器时钟，不影响时基单元功能
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;	//计数器模式，选择向上计数
	TIM_TimeBaseInitStructure.TIM_Period = 20000 - 1;				//计数周期，即ARR的值
	TIM_TimeBaseInitStructure.TIM_Prescaler = 72 - 1;				//预分频器，即PSC的值
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;			
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);				//将结构体变量交给TIM_TimeBaseInit，配置TIM2的时基单元	
	
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_OCStructInit(&TIM_OCInitStructure);
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;                               //CCR
	TIM_OC2Init(TIM2,&TIM_OCInitStructure);
	
	TIM_Cmd(TIM2,ENABLE);
	/*PWM频率： freq = CK_PSC/(PSC+1)/(ARR+1) 
	 *PWM占空比：duty = CCR/(ARR+1)
	 *PWM分辨率：reso = 1/(ARR+1)      */
}

void PWM_setccr(uint16_t compare)//更改CCR的值
{
	TIM_SetCompare2(TIM2,compare);
}

void servo_setangle(float angle)
{
	PWM_setccr(angle / 180*2000 + 500);
}

void key_init(void)//按键初始化
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;//上拉输入
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;
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
	
	return keynum;
}