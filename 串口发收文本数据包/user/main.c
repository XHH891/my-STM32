#include "stm32f10x.h"                  // Device header
#include "OLED.h"
#include "Delay.h"
#include <string.h>

char Serial_Rxpacket[100];	
uint8_t Serial_RxFlag = 0;	

uint8_t Serial_GetRxFlag(void);

void LED_init(void);
void led1_on(void);//打开LED1
void led1_off(void);//关闭LED1

void serial_init();//初始化
void serial_sendbyte(uint8_t byte);//发送单个字节
void serial_sendarray(uint8_t *array,uint16_t l);//发送一个数组
void serial_sendstring(char *string);//发送字符串
void serial_sendnumber(uint32_t number, uint8_t l);//发送数字
uint32_t pow(uint32_t X, uint32_t Y);//X的Y次方

int main(void)
{
	
	OLED_Init(); 
	serial_init();
	LED_init();
	
	OLED_ShowString(1,1,"TxPacket");
	OLED_ShowString(3,1,"RxPacket");
	
	while(1)
	{
		if(Serial_GetRxFlag() == 1)
		{
			OLED_ShowString(4,1,"				");
			OLED_ShowString(4,1,Serial_Rxpacket);
			
			if(strcmp(Serial_Rxpacket,"LED_ON") == 0)
			{
				led1_on();
				serial_sendstring("LED_ON_OK\r\n");
				OLED_ShowString(2,1,"				");
				OLED_ShowString(2,1,"LED_ON_OK");
			}
			else if(strcmp(Serial_Rxpacket,"LED_OFF") == 0)
			{
				led1_off();
				serial_sendstring("LED_OFF_OK\r\n");
				OLED_ShowString(2,1,"				");
				OLED_ShowString(2,1,"LED_OFF_OK");
			}
			else
			{
				serial_sendstring("ERROR_COMMAND\r\n");
				OLED_ShowString(2,1,"				");
				OLED_ShowString(2,1,"ERROR_COMMAND");
			}
		}
	}
}

uint32_t pow(uint32_t X, uint32_t Y)//X的Y次方
{
	uint32_t Result = 1;	
	while (Y --)			
	{
		Result *= X;		
	}
	return Result;
}

void serial_sendnumber(uint32_t number, uint8_t l)//串口发送数字
{
	uint8_t i;
	for (i = 0; i < l; i ++)		//根据数字长度遍历数字的每一位
	{
		serial_sendbyte(number / pow(10, l - i - 1) % 10 + '0');	//依次调用Serial_SendByte发送每位数字
	}
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

void serial_sendbyte(uint8_t byte)//发送单个字节
{
	USART_SendData(USART1,byte);
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE) == RESET);
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
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Pin =GPIO_Pin_10;
	GPIO_InitStruct.GPIO_Speed =GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	USART_InitTypeDef USART_Inits;
	USART_Inits.USART_BaudRate = 9600;// 波特率
	USART_Inits.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//硬件流控制
	USART_Inits.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;//串口模式 发送还是接收
	USART_Inits.USART_Parity = USART_Parity_No;//校验位 奇校验或偶校验或无
	USART_Inits.USART_StopBits = USART_StopBits_1;//停止位
	USART_Inits.USART_WordLength = USART_WordLength_8b;//字长 有校验为9位无为8位
	USART_Init(USART1,&USART_Inits);
	
	//可以使用中断来读取数据
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);//开启RXNE标志位到NVIC中断的输出
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitTypeDef NVIC_InitStructure;						
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;		
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;			
	NVIC_Init(&NVIC_InitStructure);		
	
	USART_Cmd(USART1,ENABLE);
}

void USART1_IRQHandler(void)
{
	static uint8_t s = 0;//状态变量用于判断数据的包头包尾和数据
	static uint8_t ppacket = 0;//指示接收到哪一个数据
	uint8_t data;//用于接受数据
	if(USART_GetITStatus(USART1,USART_IT_RXNE) == SET)
	{
		data = USART_ReceiveData(USART1);
		
		if(s==0)//等待包头
		{
			if(data == '@')
			{
				s = 1;
				ppacket = 0;
			}
		}
		else if(s==1)
		{
			if(data == '\r')
			{
				s=2;
			}
			else
			{
				Serial_Rxpacket[ppacket] = data;
				ppacket++;
			}	
		}
		else if(s==2)
		{
			if(data == '\n')
			{
				s = 0;
				Serial_Rxpacket[ppacket] = '\0';
				Serial_RxFlag = 1;
			}
		}
	}
}

uint8_t Serial_GetRxFlag(void)
{
	if (Serial_RxFlag == 1)			
	{
		Serial_RxFlag = 0;
		return 1;					
	}
	return 0;					
}
void LED_init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin =GPIO_Pin_1;
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
	