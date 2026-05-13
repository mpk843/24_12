#include "stm32f10x.h"		// Device header
#include "Timer.h"
#include "ADTIM.h"
#include "PWM.h"
#include "define.h"
#include "Delay.h"
#include "protection.h"
#include "OLED.h"
#include "state_ctrl.h"

void Protection_Init()
{
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOA ,ENABLE);
	
	//定义 GPIO输出结构体 变量 GPIO_InitStructure
	GPIO_InitTypeDef GPIO_InitStructure;
	
	//配置输出模式，pin口，输出频率	
	GPIO_InitStructure.GPIO_Mode= GPIO_Mode_Out_PP;	//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	//PWM_SD
	GPIO_InitStructure.GPIO_Pin= GPIO_Pin_5;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	//SW_IN
	GPIO_InitStructure.GPIO_Pin= GPIO_Pin_4;
	GPIO_Init(GPIOA,&GPIO_InitStructure);

	//SW_OUT
	GPIO_InitStructure.GPIO_Pin= GPIO_Pin_10;
	GPIO_Init(GPIOB,&GPIO_InitStructure);

	
	//蜂鸣器I/O引脚配置
	GPIO_InitStructure.GPIO_Pin= GPIO_Pin_11;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
		

	//默认 PWM驱动 关闭  继电器开关 关闭
	pwm_sd_off;		sw_in_off;	 sw_out_off;
	
	//蜂鸣器默认关闭状态
	buzzer_off;
}

void protect(void)	//PWM_SD_12 && PWM_SD_34
{	
	work_off();
	
	//显示过载警告
	OLED_Clear();
//	OLED_ShowString(1, 5, "WARNING");
//	OLED_ShowString(4, 5, "WARNING");
	OLED_ShowChinese(26, 24, "输");OLED_ShowChinese(46, 24, "出");OLED_ShowChinese(66, 24, "过");OLED_ShowChinese(86, 24, "载");
	OLED_Update();

	//蜂鸣器警报	
	buzzer_on;
	Delay_ms(2000);
	while(	(i_filter * 330 / 2048) > ocp_10e2 - 150	);
	//蜂鸣器关闭
	buzzer_off;
	
	work_on();
	
//	//OLED显示重启提示
//	OLED_Clear();
//	OLED_ShowString(1, 5, "WARNING",OLED_8X16);
//	OLED_ShowString(2, 1, "  The Mode had  ",OLED_8X16);
//	OLED_ShowString(3, 1, "  Shut Down...  ",OLED_8X16);
//	OLED_ShowString(4, 1, "  Please RESET  ",OLED_8X16);
//	while(1);
}
