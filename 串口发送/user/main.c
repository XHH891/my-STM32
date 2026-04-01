#include "stm32f10x.h"                  // Device header
#include "OLED.h"

void serial_init();//初始化
void serial_sendbyte(uint8_t byte);//发送单个字节
void serial_sendarray(uint8_t *array,uint16_t l);//发送一个数组
void serial_sendstring(char *string);//发送字符串
void serial_sendnumber(uint32_t number,uint16_t l);//发送一个数字

int main(void)
{
	OLED_Init(); 
	serial_init();
	
	//uint8_t a[] = {0x42,0x43,0x44,0x45};
	//serial_sendarray(a,4);
	
	serial_sendstring("12345");
	
	while(1)
	{
		
	}
}

void serial_sendnumber(uint32_t number,uint16_t l)//发送一个大数字比如1234567
{
	
}

void serial_sendstring(char *string)
{
	uint8_t i;
	for(i=0;string[i]!='\0';i++)
	{
		serial_sendbyte(string[i]);
	}
}

void serial_sendarray(uint8_t *array,uint16_t l)//发送一个数组
{
	uint16_t i;
	for(i=0;i<l;i++)
	{
		serial_sendbyte(array[i]);
	}
}

void serial_init()//初始化
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Pin =GPIO_Pin_9;
	GPIO_InitStruct.GPIO_Speed =GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	USART_InitTypeDef USART_Inits;
	USART_Inits.USART_BaudRate = 9600;// 波特率
	USART_Inits.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//硬件流控制
	USART_Inits.USART_Mode = USART_Mode_Tx;//串口模式 发送还是接收
	USART_Inits.USART_Parity = USART_Parity_No;//校验位 奇校验或偶校验或无
	USART_Inits.USART_StopBits = USART_StopBits_1;//停止位
	USART_Inits.USART_WordLength = USART_WordLength_8b;//字长 有校验为9位无为8位
	USART_Init(USART1,&USART_Inits);
	
	USART_Cmd(USART1,ENABLE);
}

void serial_sendbyte(uint8_t byte)//发送单个字节
{
	USART_SendData(USART1,byte);
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE) == RESET);
}