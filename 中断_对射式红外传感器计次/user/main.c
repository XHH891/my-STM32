#include "stm32f10x.h"                  // Device header
#include "OLED.h"

void countsensor_init(void);
uint16_t retur(void);

uint16_t t=0;

int main(void)
{
	countsensor_init();
	OLED_Init();
	
	OLED_ShowString(1,1,"count:");
	
	while(1)
	{
		OLED_ShowNum(1,7,retur(),5); 
	}
}

uint16_t retur(void)
{
	return t;
}

void EXTI15_10_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line14) == SET)//由于他会监测10~15的所有引脚，因此必须判断我们规定的引脚是否被触发。
	{
		t++;
		EXTI_ClearITPendingBit(EXTI_Line14);//系统不会自动将标注为变0 如果不手动中断将它变为0，系统会一直执行中断
	}
}

void countsensor_init(void)//中断初始化
{
	//STM32 的外设（GPIO、AFIO）默认时钟是关闭的，必须先开启时钟才能配置寄存器；
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);		//开启GPIOB的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);		//AFIO（复用功能 IO）负责将 GPIO 引脚和 EXTI 线绑定（比如把 B14 映射到 EXTI14 线）
																//是外部中断的 “桥梁”
	
	/*GPIO初始化*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//上拉输入 → 引脚默认电平为高，当外部事件（比如按键按下）发生时，引脚电平拉低，满足 “下降沿触发” 的条件；
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);						//将PB14引脚初始化为上拉输入
	
	/*AFIO选择中断引脚*/
	/*STM32 的 EXTI（外部中断控制器）有 16 条中断线（EXTI0~EXTI15），每条线可以和任意 GPIO 端口的对应引脚绑定（比如 EXTI14 可以绑定 PA14、PB14、PC14 等）；
	这行代码就是告诉 AFIO：将 GPIOB 的 14 号引脚，映射到 EXTI 的 14 号中断线；
	对应中断流程：这是 “外设触发中断请求” 的关键一步 —— 只有完成绑定，B14 引脚的电平变化才能被 EXTI 检测到，并向 NVIC 发送中断请求。*/
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource14);//将外部中断的14号线映射到GPIOB，即选择PB14为外部中断引脚
	
	/*EXTI初始化*/
	EXTI_InitTypeDef EXTI_InitStructure;						//定义结构体变量
	EXTI_InitStructure.EXTI_Line = EXTI_Line14;					//选择配置外部中断的14号线
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;					//开启这条中断线 —— 如果关闭，即使 B14 电平变化，EXTI 也不会发送请求
	/*EXTI 有两种模式：
	中断模式（Interrupt）：会向 NVIC 发送中断请求，打断 CPU 执行；
	事件模式（Event）：仅触发硬件事件（如 DMA），不向 NVIC 发请求，CPU 不中断*/
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;			//指定外部中断线为中断模式
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;		//指定外部中断线为下降沿触发
	EXTI_Init(&EXTI_InitStructure);								//将结构体变量交给EXTI_Init，配置EXTI外设
	
	/*NVIC中断分组*/
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);				//配置NVIC为分组2
																//即抢占优先级范围：0~3，响应优先级范围：0~3
																//此分组配置在整个工程中仅需调用一次
																//若有多个中断，可以把此代码放在main函数内，while循环之前
																//若调用多次配置分组的代码，则后执行的配置会覆盖先执行的配置
	
	/*NVIC配置*/
	NVIC_InitTypeDef NVIC_InitStructure;						//定义结构体变量
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;		//选择配置NVIC的EXTI15_10线
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//使能该 NVIC 中断通道 —— 如果关闭，即使 EXTI 发送了请求，NVIC 也会拒绝向 CPU 转发，中断无法响应；
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	//指定NVIC线路的抢占优先级为1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;			//指定NVIC线路的响应优先级为1
	NVIC_Init(&NVIC_InitStructure);								//将结构体变量交给NVIC_Init，配置NVIC外设

}