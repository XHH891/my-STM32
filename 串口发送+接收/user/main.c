#include "stm32f10x.h"                  // Device header
#include "OLED.h"

uint8_t Serial_RxData;		//定义串口接收的数据变量
uint8_t Serial_RxFlag;		//定义串口接收的标志位变量
uint8_t Serial_GetRxData(void);
uint8_t Serial_GetRxFlag(void);

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
	
	uint8_t data;
	
	while(1)
	{
		if(USART_GetFlagStatus(USART1,USART_FLAG_RXNE) == SET)
		{
			data = USART_ReceiveData(USART1);
			OLED_ShowHexNum(1,1,data,2);
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
	if(USART_GetITStatus(USART1,USART_IT_RXNE) == SET)
	{
		Serial_RxData = USART_ReceiveData(USART1);				
		Serial_RxFlag = 1;										
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
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

uint8_t Serial_GetRxData(void)
{
	return Serial_RxData;			
}