#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"

void environment_test(void)
{
	u8	load_time = 5;
	OLED_Clear();
	OLED_ShowString(2, 3, "Environment",OLED_8X16);
	OLED_ShowString(3, 5, "Test",OLED_8X16);
	while(load_time --)
	{
		Delay_ms(200);
		OLED_ShowString(3, 10, ".",OLED_8X16);
		Delay_ms(250);
		OLED_ShowString(3, 11, ".",OLED_8X16);
		Delay_ms(250);
		OLED_ShowString(3, 12, ".",OLED_8X16);
		Delay_ms(250);
		OLED_ShowString(3, 10, "   ",OLED_8X16);
	}
	OLED_Clear();
}


