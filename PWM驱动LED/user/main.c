#include "stm32f10x.h"                  // Device header
#include "OLED.h"
#include "Delay.h"

void PWM_init(void);
void PWM_setccr(uint16_t compare);//更改CCR的值

int main(void)
{
	uint8_t i;
	
	OLED_Init(); 
	PWM_init();
	
	OLED_ShowString(1,3,"I'm happy!");
	
	while(1)
	{
		for(i=0;i<=100;i++)
		{
			PWM_setccr(i);
			Delay_ms(10);
		}
		for(i=0;i<=100;i++)
		{
			PWM_setccr(100 - i);
			Delay_ms(10);
		}
		
	}
}

void PWM_init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);	//开启TIM2的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//开启GPIOA外设的时钟
	
	/*GPIO重映射这行代码能将原先的 A0引脚印射到 A15引脚。如果A0引脚要做别的事情，无法输出PWM波形可以将它映射到 A15引脚。*/
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);			//开启AFIO的时钟，重映射必须先开启AFIO的时钟
//	GPIO_PinRemapConfig(GPIO_PartialRemap1_TIM2, ENABLE);			//将TIM2的引脚部分重映射，具体的映射方案需查看参考手册
//	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);		//将JTAG引脚失能，作为普通GPIO引脚使用

	
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Pin =GPIO_Pin_0;  //GPIO_Pin_15;
	GPIO_InitStruct.GPIO_Speed =GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);//将配置参数写入GPIOA寄存器，使配置生效
	
	/*配置时钟源*/
	TIM_InternalClockConfig(TIM2);		//选择TIM2为内部时钟，若不调用此函数，TIM默认也为内部时钟
	
	/*时基单元初始化*/
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;				//定义结构体变量
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;		//时钟分频，选择不分频，此参数用于配置滤波器时钟，不影响时基单元功能
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;	//计数器模式，选择向上计数
	TIM_TimeBaseInitStructure.TIM_Period = 100 - 1;				//计数周期，即ARR的值
	TIM_TimeBaseInitStructure.TIM_Prescaler = 720 - 1;				//预分频器，即PSC的值
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;			
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);				//将结构体变量交给TIM_TimeBaseInit，配置TIM2的时基单元	
	
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_OCStructInit(&TIM_OCInitStructure);
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;                               //CCR
	
	TIM_OC1Init(TIM2,&TIM_OCInitStructure);
	
	TIM_Cmd(TIM2,ENABLE);
	/*PWM频率： freq = CK_PSC/(PSC+1)/(ARR+1) 
	 *PWM占空比：duty = CCR/(ARR+1)
	 *PWM分辨率：reso = 1/(ARR+1)      */
}

void PWM_setccr(uint16_t compare)//更改CCR的值
{
	TIM_SetCompare1(TIM2,compare);
}