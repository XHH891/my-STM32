#include "stm32f10x.h"                  // Device header
#include "OLED.h"

uint8_t dataa[] = {0x01,0x02,0x03,0x04};
uint8_t datab[] = {0,0,0,0};

void mydma_init(uint32_t addra,uint32_t addrb,uint16_t size)
{
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
	
	DMA_InitTypeDef dma_init;
	dma_init.DMA_PeripheralBaseAddr = addra;// 外设寄存器的起始地址
	dma_init.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;// 外设传输数据宽度（8/16/32位）
	dma_init.DMA_PeripheralInc = DMA_PeripheralInc_Enable;// 外设地址是否自动增加
	dma_init.DMA_MemoryBaseAddr = addrb;// 内存缓冲区的起始地址
	dma_init.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;// 内存传输数据宽度（8/16/32位）
	dma_init.DMA_MemoryInc = DMA_MemoryInc_Enable;// 内存地址是否自动增加
	dma_init.DMA_DIR = DMA_DIR_PeripheralSRC;// 数据传输方向（外设→内存/内存→外设）
	dma_init.DMA_BufferSize = size;// 缓冲区大小
	dma_init.DMA_Mode = DMA_Mode_Normal;// DMA工作模式是否使用自动重装（单次/循环传输）
	dma_init.DMA_M2M = DMA_M2M_Enable;// 是否开启内存到内存传输硬件触发还是软件触发
	dma_init.DMA_Priority =DMA_Priority_Medium;// DMA通道的优先级（低/中/高/极高）
	DMA_Init(DMA1_Channel1,&dma_init);
	
	DMA_Cmd(DMA1_Channel1,ENABLE);
}

void mydma_transfer(void)
{
	
}

int main(void)
{
	OLED_Init(); 
	OLED_ShowHexNum(1,1,dataa[0],2);
	OLED_ShowHexNum(1,4,dataa[1],2);
	OLED_ShowHexNum(1,7,dataa[2],2);
	OLED_ShowHexNum(1,10,dataa[3],2);
	OLED_ShowHexNum(2,1,datab[0],2);
	OLED_ShowHexNum(2,4,datab[1],2);
	OLED_ShowHexNum(2,7,datab[2],2);
	OLED_ShowHexNum(2,10,datab[3],2);
	mydma_init((uint32_t)dataa,(uint32_t)datab,4);
	
	OLED_ShowHexNum(3,1,dataa[0],2);
	OLED_ShowHexNum(3,4,dataa[1],2);
	OLED_ShowHexNum(3,7,dataa[2],2);
	OLED_ShowHexNum(3,10,dataa[3],2);
	OLED_ShowHexNum(4,1,datab[0],2);
	OLED_ShowHexNum(4,4,datab[1],2);
	OLED_ShowHexNum(4,7,datab[2],2);
	OLED_ShowHexNum(4,10,datab[3],2);
	while(1)
	{
		
	}
}