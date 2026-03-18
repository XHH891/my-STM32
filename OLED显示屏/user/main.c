#include "stm32f10x.h"                  // Device header
#include "OLED.h"

int main(void)
{
	OLED_Init(); 
	
	OLED_ShowString(1,3,"I'm happy!");
	
	while(1)
	{
		
	}
}