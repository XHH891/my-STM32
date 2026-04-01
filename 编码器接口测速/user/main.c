#include "stm32f10x.h"                  // Device header
#include "OLED.h"
#include "Delay.h"

int16_t num;

void timer_init(void);//定时器初始化

void encoder_init(void);
int16_t encoder_get(void);

int main(void)
{
	OLED_Init(); 
	timer_init();
	encoder_init();
	
	OLED_ShowString(1,1,"cnt:");
	
	while(1)
	{
		OLED_ShowSignedNum(1,7,num,5);
	}
}

void encoder_init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);

	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Pin =GPIO_Pin_6 | GPIO_Pin_7;  
	GPIO_InitStruct.GPIO_Speed =GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);	
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;				
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;// 时钟分频1			
	TIM_TimeBaseInitStructure.TIM_Period = 65536 - 1;				
	TIM_TimeBaseInitStructure.TIM_Prescaler = 1 - 1;				
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;			
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);		

	TIM_ICInitTypeDef TIM_ICInitStructure;
	TIM_ICStructInit(&TIM_ICInitStructure);
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;// 选择通道1
	TIM_ICInitStructure.TIM_ICFilter = 0xF;// 输入滤波器：最大滤波（16个采样周期）
	TIM_ICInit(TIM3,&TIM_ICInitStructure);
	
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;// 选择通道1
	TIM_ICInitStructure.TIM_ICFilter = 0xF;// 输入滤波器：最大滤波（16个采样周期）
	TIM_ICInit(TIM3,&TIM_ICInitStructure);
	
	TIM_EncoderInterfaceConfig(TIM3,TIM_EncoderMode_TI12,TIM_ICPolarity_Rising,TIM_ICPolarity_Rising);
	
	TIM_Cmd(TIM3,ENABLE);
}

int16_t encoder_get(void)
{
	int16_t temp;
	temp = TIM_GetCounter(TIM3);
	TIM_SetCounter(TIM3,0);
	return temp;
}

void timer_init(void)//定时器初始化
{
	/*开启时钟*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);			//开启TIM2的时钟
	
	/*配置时钟源*/
	TIM_InternalClockConfig(TIM2);		//选择TIM2为内部时钟，若不调用此函数，TIM默认也为内部时钟
	
	/*时基单元初始化*/
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;				//定义结构体变量
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;		//时钟分频，选择不分频，此参数用于配置滤波器时钟，不影响时基单元功能
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;	//计数器模式，选择向上计数
	TIM_TimeBaseInitStructure.TIM_Period = 10000 - 1;				//计数周期，即ARR的值
	TIM_TimeBaseInitStructure.TIM_Prescaler = 7200 - 1;				//预分频器，即PSC的值
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;			//重复计数器，高级定时器才会用到
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);				//将结构体变量交给TIM_TimeBaseInit，配置TIM2的时基单元	
	
	/*中断输出配置*/
	TIM_ClearFlag(TIM2, TIM_FLAG_Update);						//清除定时器更新标志位
																//TIM_TimeBaseInit函数末尾，手动产生了更新事件
																//若不清除此标志位，则开启中断后，会立刻进入一次中断
																//如果不介意此问题，则不清除此标志位也可
	
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);					//开启TIM2的更新中断
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);				
	
	/*NVIC配置*/
	NVIC_InitTypeDef NVIC_InitStructure;						
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;		
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;	
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;			
	NVIC_Init(&NVIC_InitStructure);		

	TIM_Cmd(TIM2,ENABLE);
}

void TIM2_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
	{
		num = encoder_get();
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	}
}