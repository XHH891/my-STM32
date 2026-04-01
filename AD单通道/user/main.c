#include "stm32f10x.h"                  // Device header
#include "OLED.h"

void ad_init()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);//指定分频
	
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStruct.GPIO_Pin =GPIO_Pin_0;
	GPIO_InitStruct.GPIO_Speed =GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	                                                 //转换时间
	ADC_RegularChannelConfig(ADC1,ADC_Channel_0,1,ADC_SampleTime_55Cycles5);//填充菜单 在规则组的第一个位置写入通道0
	
	ADC_InitTypeDef ADC_init;
	ADC_init.ADC_Mode = ADC_Mode_Independent;//单通道还是双通道
	ADC_init.ADC_DataAlign = ADC_DataAlign_Right;//数据对齐 是左对齐还是右对齐
	ADC_init.ADC_NbrOfChannel = 1;//通道数目指定在扫描模式下总共会用几个通道
	ADC_init.ADC_ScanConvMode = DISABLE;//扫描转换模式 是扫描转换模式还是单次转换模式
	ADC_init.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//选择外部源
	ADC_init.ADC_ContinuousConvMode = DISABLE;//连续转换模式 是连续转换还是单次转换
	ADC_Init(ADC1,&ADC_init);
	
	ADC_Cmd(ADC1,ENABLE);
	
	ADC_ResetCalibration(ADC1);//开启校准，内部自带
	while(ADC_GetCalibrationStatus(ADC1) == SET);//没校准完成就一直等待
	ADC_StartCalibration(ADC1);
	while(ADC_GetSoftwareStartConvStatus(ADC1) == SET);
	
	//ADC_SoftwareStartConvCmd(ADC1,ENABLE);//为连续转换时
}

uint16_t ad_getvalue(void)//返回转换结果 
{
	ADC_SoftwareStartConvCmd(ADC1,ENABLE);//若为连续转换，则将这句放在ad_init()即可，只要触发一次
	while(ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC) == RESET);//等待转换完成 若为连续转换，则不需要这一句
	return ADC_GetConversionValue(ADC1);
}

int main(void)
{
	uint16_t a;
	OLED_Init(); 
	ad_init();
	OLED_ShowString(1,1,"ad:");
	OLED_ShowString(2,1,"V:0.00V");
	
	while(1)
	{
		a = ad_getvalue();
		OLED_ShowNum(1,4,a,4);
		OLED_ShowNum(2,3,(float)a/4095*3.3,1);
		OLED_ShowNum(2,5,(uint16_t)((float)a/4095*3.3*100)%100,2);
	}
}