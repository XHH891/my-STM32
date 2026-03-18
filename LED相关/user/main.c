#include "stm32f10x.h"                  // Device header
#include "Delay.h"

int main(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//开启GPIOA外设的时钟
	
	GPIO_InitTypeDef GPIO_InitStruct;//定义GPIO初始化结构体变量
	//创建一个用于配置 GPIO 参数的 “容器”。
	//GPIO_InitTypeDef是 STM32 标准库预先定义好的结构体，
	//里面包含了 GPIO 模式、引脚、速度等所有配置项，
	//我们需要先定义这个结构体变量，再给它的成员赋值。
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;//配置GPIO模式为推挽输出 如果是开漏输出，参数是GPIO_Mode_Out_OD
	GPIO_InitStruct.GPIO_Pin =GPIO_Pin_0;//配置要操作的GPIO引脚
	GPIO_InitStruct.GPIO_Speed =GPIO_Speed_50MHz;//设置 PA0 引脚的输出速度为 50MHz
	
	/*
	*GPIO_Speed_2MHz：低速，电磁干扰小，适合驱动 LED 等低速设备；
	*GPIO_Speed_10MHz：中速；
	*GPIO_Speed_50MHz：高速，驱动能力强，适合 SPI、串口等高速通信。
	*/
	
	GPIO_Init(GPIOA,&GPIO_InitStruct);//将配置参数写入GPIOA寄存器，使配置生效
	
	//GPIO_ResetBits(GPIOA,GPIO_Pin_0);
	//GPIO_SetBits(GPIOA,GPIO_Pin_0);
	GPIO_WriteBit(GPIOA,GPIO_Pin_0,Bit_SET);
	
	while(1)
	{
		GPIO_WriteBit(GPIOA,GPIO_Pin_0,Bit_RESET);
		Delay_ms(100);
		GPIO_WriteBit(GPIOA,GPIO_Pin_0,Bit_SET);
		Delay_ms(100);
	}
}
