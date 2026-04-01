#include "stm32f10x.h"                  // Device header
#include "OLED.h"

uint16_t ad_value[4];

void ad_init()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);//指定分频为6  72/6
	
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;// 模拟输入（必须）
	GPIO_InitStruct.GPIO_Pin =GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;
	GPIO_InitStruct.GPIO_Speed =GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	// 4. 配置ADC规则通道：4个通道，扫描顺序0→1→2→3，采样时间55.5个周期
	ADC_RegularChannelConfig(ADC1,ADC_Channel_0,1,ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC1,ADC_Channel_1,2,ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC1,ADC_Channel_2,3,ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC1,ADC_Channel_3,4,ADC_SampleTime_55Cycles5);
		
	ADC_InitTypeDef ADC_init;
	ADC_init.ADC_Mode = ADC_Mode_Independent;//单通道还是双通道
	ADC_init.ADC_DataAlign = ADC_DataAlign_Right;//数据对齐 是左对齐还是右对齐
	ADC_init.ADC_NbrOfChannel = 4;//通道数目指定在扫描模式下总共会用几个通道
	ADC_init.ADC_ScanConvMode = ENABLE;//扫描转换模式 是扫描转换模式还是单次转换模式
	ADC_init.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//选择外部源
	ADC_init.ADC_ContinuousConvMode = ENABLE;//连续转换模式 是连续转换还是单次转换
	ADC_Init(ADC1,&ADC_init);
	
	/*DMA初始化*/
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
	
	DMA_InitTypeDef dma_init;
	dma_init.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;// 外设寄存器的起始地址
	dma_init.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;// 外设传输数据宽度（8/16/32位）
	dma_init.DMA_PeripheralInc = DMA_PeripheralInc_Disable;// 外设地址是否自动增加
	dma_init.DMA_MemoryBaseAddr = (uint32_t)ad_value;// 内存缓冲区的起始地址
	dma_init.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;// 内存传输数据宽度（8/16/32位）
	dma_init.DMA_MemoryInc = DMA_MemoryInc_Enable;// 内存地址是否自动增加
	dma_init.DMA_DIR = DMA_DIR_PeripheralSRC;// 数据传输方向（外设→内存/内存→外设）
	dma_init.DMA_BufferSize = 4;// 缓冲区大小
	dma_init.DMA_Mode = DMA_Mode_Circular;// DMA工作模式是否使用自动重装（单次/循环传输）
	dma_init.DMA_M2M = DMA_M2M_Disable;// 是否开启内存到内存传输硬件触发还是软件触发
	dma_init.DMA_Priority =DMA_Priority_Medium;// DMA通道的优先级（低/中/高/极高）
	DMA_Init(DMA1_Channel1,&dma_init);
	
	DMA_Cmd(DMA1_Channel1,ENABLE);
	/***********************************/
	ADC_DMACmd(ADC1,ENABLE);
	ADC_Cmd(ADC1,ENABLE);
	
	ADC_ResetCalibration(ADC1);//开启校准，内部自带
	while(ADC_GetCalibrationStatus(ADC1) == SET);//没校准完成就一直等待
	ADC_StartCalibration(ADC1);
	while(ADC_GetSoftwareStartConvStatus(ADC1) == SET);
	
	ADC_SoftwareStartConvCmd(ADC1,ENABLE);//为连续转换时
}

void ad_getvalue(void)//返回转换结果 
{
	// 重新配置DMA传输（单次模式下，传输完成后需重置计数器）
	DMA_Cmd(DMA1_Channel1,DISABLE);// 先禁用DMA
	DMA_SetCurrDataCounter(DMA1_Channel1,4);// 重置传输计数器为4
	DMA_Cmd(DMA1_Channel1,ENABLE);// 重新使能DMA
	
	//ADC_SoftwareStartConvCmd(ADC1,ENABLE);//若为连续转换，则将这句放在ad_init()即可，只要触发一次
	
	while(DMA_GetFlagStatus(DMA1_FLAG_TC1) == RESET);
	DMA_ClearFlag(DMA1_FLAG_TC1);
}

int main(void)
{
	OLED_Init(); 
	ad_init();
	OLED_ShowString(1,1,"ad0:");
	OLED_ShowString(2,1,"ad1:");
	OLED_ShowString(3,1,"ad2:");
	OLED_ShowString(4,1,"ad3:");
	
	while(1)
	{
		//ad_getvalue();若配置为ADC连续扫描+DMA循环转运则不需要ad_getvalue()
		OLED_ShowNum(1,5,ad_value[0],4);
		OLED_ShowNum(2,5,ad_value[1],4);
		OLED_ShowNum(3,5,ad_value[2],4);
		OLED_ShowNum(4,5,ad_value[3],4);
	}
}