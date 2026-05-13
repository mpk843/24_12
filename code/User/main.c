#include "stm32f10x.h"		// Device header
#include <stdlib.h>
#include "RCC.h"
#include "OLED.h"
#include "Timer.h"
#include "ADTIM.h"
#include "AD.h"
#include "PWM.h"
#include "protection.h"
#include "define.h"
#include "Serial.h"
#include "Delay.h"
#include "Key.h"
#include "state_ctrl.h"

typedef enum {
	STATE_IDLE,
	STATE_RUNNING,
	STATE_VREF_SET
} SystemState;

static void enter_running(void)
{
	OLED_Clear();
	OLED_ShowString(24, 24, "Loading...", OLED_8X16);
	OLED_Update();

	work_continue();

	OLED_Clear();
	OLED_ShowChinese(0, 0, "输");OLED_ShowChinese(16, 0, "出");OLED_ShowChinese(32, 0, "电");OLED_ShowChinese(48, 0, "压");
	OLED_ShowString(64, 0, " 00.00 V", OLED_8X16);
	OLED_ShowChinese(0, 16, "输");OLED_ShowChinese(16, 16, "出");OLED_ShowChinese(32, 16, "电");OLED_ShowChinese(48, 16, "流");
	OLED_ShowString(64, 16, " 00.00 A", OLED_8X16);
	OLED_ShowChinese(0, 32, "输");OLED_ShowChinese(16, 32, "入");OLED_ShowChinese(32, 32, "电");OLED_ShowChinese(48, 32, "压");
	OLED_ShowString(64, 32, " 00.00 V", OLED_8X16);
	OLED_ShowChinese(0, 48, "输");OLED_ShowChinese(16, 48, "入");OLED_ShowChinese(32, 48, "电");OLED_ShowChinese(48, 48, "流");
	OLED_ShowString(64, 48, " 00.00 A", OLED_8X16);
	OLED_Update();
}

static void enter_vref_set(void)
{
	OLED_Clear();
	OLED_ShowChinese(16, 24, "载");OLED_ShowChinese(32, 24, "入");OLED_ShowChinese(48, 24, "中");
	OLED_ShowString(64, 24, "......", OLED_8X16);
	OLED_Update();

	work_suspend();

	OLED_Clear();
	OLED_ShowChinese(4, 0, "按");OLED_ShowString(20, 0, "\"ADD\"\"REDUCE\"", OLED_8X16);
	OLED_ShowChinese(8, 16, "以");OLED_ShowChinese(24, 16, "调");OLED_ShowChinese(40, 16, "整");
	OLED_ShowChinese(56, 16, "输");OLED_ShowChinese(72, 16, "出");OLED_ShowChinese(88, 16, "电");OLED_ShowChinese(104, 16, "压");
	OLED_ShowString(0, 32, "Vref: ", OLED_8X16);
	OLED_ReverseArea(place[i], 48, 8, 16);
	OLED_ShowImage(16, 48, 16, 16, right_arrow);
	OLED_ShowString(48, 48, "00.00 V", OLED_8X16);
	OLED_Update();
}

//	主函数：coding 执行入口

int main(void)
{
	/*模块初始化*/

	SystemClock_Config();
	Protection_Init();
	Serial_Init();
	OLED_Init();
	Key_Init();
	AD_Init();
	ADTIM_Init();
	PWM_Init();
	Timer_Init();

	turn_zero();

	work_off();
	Vref = (Vtarget * 4096) / 3300 ;
	Iref = (Itarget * 4096) / 660 ;

	OLED_ShowChinese(25, 10, "按");OLED_ShowChinese(45, 10, "下");OLED_ShowString(66, 10, "\"SET\"", OLED_8X16);
	OLED_ShowChinese(37, 37, "以");OLED_ShowChinese(57, 37, "启");OLED_ShowChinese(77, 37, "动");
	OLED_Update();

	SystemState state = STATE_IDLE;
	u8 state_enter = 1;

//	主循环
	while (1)
	{
		if (state_enter)
		{
			state_enter = 0;
			if (state == STATE_RUNNING)       enter_running();
			else if (state == STATE_VREF_SET) enter_vref_set();
		}

		u8 key = Key_GetNum();

		switch (state)
		{
		//	待机：等待按下SET启动
		case STATE_IDLE:
			if (key == set)
			{
				OLED_Clear();
				Delay_ms(100);
				OLED_ShowChinese(24, 24, "输");OLED_ShowChinese(44, 24, "出");
				OLED_Update();

				Delay_ms(500);
				OLED_ShowChinese(64, 24, "设");OLED_ShowChinese(84, 24, "置");
				OLED_Update();

				work_first_time_on();

				Delay_ms(1000);
				state = STATE_VREF_SET;
				state_enter = 1;
			}
			break;

		//	运行：动态监测显示
		case STATE_RUNNING:
			OLED_ShowNum(72, 0, v_out_avg / 100, 2, OLED_8X16);
			OLED_ShowNum(96, 0, v_out_avg % 100, 2, OLED_8X16);

			OLED_ShowSignedNum(64, 16, i_out_avg / 100, 2, OLED_8X16);
			OLED_ShowNum(96, 16, (abs(i_out_avg)) % 100, 2, OLED_8X16);

			OLED_ShowNum(72, 32, v_in_avg / 100, 2, OLED_8X16);
			OLED_ShowNum(96, 32, v_in_avg % 100, 2, OLED_8X16);

			OLED_ShowSignedNum(64, 48, i_in_avg / 100, 2, OLED_8X16);
			OLED_ShowNum(96, 48, (abs(i_in_avg)) % 100, 2, OLED_8X16);

			OLED_Update();

			//	OCP 过流保护
			if ((i_filter * 330 / 2048) > ocp_10e2)
			{
				protect();
				state = STATE_RUNNING;
				state_enter = 1;
				break;
			}

			//	切换至 Vref 设置
			if (key == mode)
			{
				state = STATE_VREF_SET;
				state_enter = 1;
			}
			break;

		//	参考电压设置
		case STATE_VREF_SET:
			if (key == mode)
			{
				OLED_ShowNum(48, 48, Vtarget / 100, 2, OLED_8X16);
				OLED_ShowNum(72, 48, Vtarget % 100, 2, OLED_8X16);
				OLED_Update();
				i--;
				if (i <= -1) i = 3;
				OLED_ReverseArea(place[i], 48, 8, 16);
				OLED_Update();
			}

			if (key == add)
			{
				if (i == 3) Vtarget += 1;
				if (i == 2) Vtarget += 10;
				if (i == 1) Vtarget += 100;
				if (i == 0) Vtarget += 1000;
				if (Vtarget >= 1900) { Vtarget = 1900; buzzer_on; Delay_ms(70); buzzer_off; }
			}

			if (key == reduce)
			{
				if (i == 3) Vtarget -= 1;
				if (i == 2) Vtarget -= 10;
				if (i == 1) Vtarget -= 100;
				if (i == 0) Vtarget -= 1000;
				if (Vtarget <= 500) { Vtarget = 500; buzzer_on; Delay_ms(70); buzzer_off; }
			}

			//	动态刷新
			OLED_ShowNum(48, 48, Vtarget / 100, 2, OLED_8X16);
			OLED_ShowNum(72, 48, Vtarget % 100, 2, OLED_8X16);
			OLED_ReverseArea(place[i], 48, 8, 16);
			OLED_Update();

			//	按下SET确认，保存Vref并返回运行状态
			if (key == set)
			{
				Vref = (Vtarget * 4096) / 3300;
				Vref = 1.0033 * Vref - 10.669;
				state = STATE_RUNNING;
				state_enter = 1;
			}
			break;
		}
	}
}

//			if(power_write_times >= 500){return;}
//			if(v_8f_avg >= 550)
//			{power[power_write_times] = (AD_Value[2] * 1650 / 4096)*(AD_Value[3] * 3300 / 4096);power_write_times++;}
//			{power[power_write_times] = AD_Value[2] * 1650 / 4096 ; power_write_times++;}
//			{power[power_write_times] = AD_Value[3] * 3300 / 4096 ; power_write_times++;}
//			{power[power_write_times] = AD_Value[0] * 3300 / 4096 ; power_write_times++;}
//			{power[power_write_times] = AD_Value[1] * 1650 / 4096 ; power_write_times++;}
