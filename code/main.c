#include "stm32f10x.h"		// Device header
#include "OLED.h"
#include "Timer.h"
#include "ADTIM.h"
#include "AD.h"
#include "PWM12.h"
#include "PWM34.h"
#include "PID.h"
#include "protection.h"
#include "define.h"
#include "Serial.h"
#include "Delay.h"
#include "Key.h"
#include "environment_test.h"
#include "Encoder.h"



//	主函数：coding 执行入口


int main(void)
{
	/*模块初始化*/

	Protection_Init();
	Serial_Init();
	AD_Init();
	ADTIM_Init();
	OLED_Init();		//OLED初始化
	PWM12_Init();		//PWM输出初始化(PA0输出PWM_12)
	PWM34_Init();		//PWM输出初始化(PA8输出PWM_34)
	Timer_Init();		//定时中断初始化
	Key_Init();
	Encoder_Init();
	
	count_avg = 800;
	charge_percent = 0;

//采样次数
	ad_count = 10 ;
	
//8F超级电容采样接收位置
	v_8f_ad = 0;
	i_8f_ad = 0;
	v_8f_ad_sum = 0;
	i_8f_ad_sum = 0;

//other采样接收位置
	v_other_ad = 0;
	i_other_ad = 0;
	v_other_ad_sum = 0;
	i_other_ad_sum = 0;

//两个PWM的占空比
	duty12 = 0;
	duty34 = 0;

//平均值（直流）
	v_8f_avg = 0;
	i_8f_avg = 0;
	p_8f_avg = 0;
	v_other_avg = 0;
	i_other_avg = 0;
	p_other_avg = 0;
	
	v_8f_avg_sum = 0;
	i_8f_avg_sum = 0;
	v_other_avg_sum = 0;
	v_other_avg_sum = 0;
	
//放电收敛电压
	v_other_compare = 300;
	
//	/*显示静态字符串*/
//	OLED_ShowString(1, 1, "Voltage: 00.00 V");
//	OLED_ShowString(2, 1, "Current: 00.00 A");
//	OLED_ShowString(3, 1, "Power:   00.00 W");
//	OLED_ShowString(4, 1, "Mode: ");


	OLED_ShowString(1, 3, "Please press");
	OLED_ShowString(2, 7, "SET");
	OLED_ShowString(3, 3, "to start...");
	
//	主循环：待机ing…
//	刷新显示

	while (1)
	{
		
		//按键接收
		u8 key_get = Key_GetNum();
		if(key_get == set && charge_state == no_charge_state)
		{
			//环境检测，自动匹配适应的模式(充电/放电)
			environment_test();
			charge_state_test:
			if(v_other_avg >= 2200)
			{charge_state = charge ; ad_count = 10;
			OLED_Clear();OLED_ShowString(2,5,"Charge");OLED_ShowString(3,6,"Mode");Delay_ms(2000);OLED_Clear();}
			else
			{charge_state = discharge ; ad_count = 10;
			OLED_Clear();OLED_ShowString(2,4,"Discharge");OLED_ShowString(3,7,"Mode");Delay_ms(2000);OLED_Clear();}
		}
		
		
		
		
		//不同状态下的OLED显示
		//静态显示
		OLED_Clear();
		OLED_ShowString(1, 1, "Voltage: 00.00 V");
		OLED_ShowString(2, 1, "Current: 00.00 A");
		OLED_ShowString(3, 1, "Power:   00.00 W");
		OLED_ShowString(4, 1, "Mode: ");
		while(1)
		{
			if( charge_state == charge)
			{
				//充电百分比静态显示
				OLED_ShowString(4, 13, " 00%");

				while(1)
				{//动态显示
					
				
				if((charge_percent / 100) == 0){OLED_ShowString(4, 13, " ");OLED_ShowNum(4,14,charge_percent,2);}
				else if((charge_percent) / 100 != 0)OLED_ShowNum(4,13,charge_percent,3);
				//脱离while的条件					
				if(v_other_avg < 2200){OLED_Clear();goto charge_state_test;}
				}
			}
			else if(charge_state == discharge)
			{//动态显示
				
				u8 key = Key_GetNum();
				//脱离while的条件
				if(key == mode ){OLED_Clear();goto Vref_set;}
				if(v_other_avg >= 2200){OLED_Clear();goto charge_state_test;}
			}
		}//OLED显示下的while
		
		
		
		
		//设置参考电压
		Vref_set:
			//静态显示
				OLED_Clear();
				OLED_ShowString(1,1,"Operate the");
				OLED_ShowString(2,1,"Encoder to Set");
				OLED_ShowString(3,1,"Vref: 03.00 V");
				while(1)
			{
				//转动旋转编码器，调节收敛电压
				u8 getnum = Encoder_Get();
				v_other_compare += (getnum * 50);
				if(v_other_compare >= 2000)	v_other_compare = 2000 ; 
GPIO_WriteBit(GPIOB,GPIO_Pin_5,Bit_SET);	Delay_ms(100); 	GPIO_WriteBit(GPIOB,GPIO_Pin_5,Bit_RESET);//蜂鸣器提示
				if(v_other_compare <= 300)	v_other_compare = 300 ;
GPIO_WriteBit(GPIOB,GPIO_Pin_5,Bit_SET);	Delay_ms(100); 	GPIO_WriteBit(GPIOB,GPIO_Pin_5,Bit_RESET);
				//动态刷新显示
				OLED_ShowNum(3,7,v_other_compare/100,2);
				OLED_ShowNum(3,7,v_other_compare%100,2);
				//按下SET确认退出循环，同时给收敛值赋值
				u8 keyy = Key_GetNum();
				if(keyy == set)
				{OLED_Clear();break;}
			}//内层while(1)

		
	}//最外层while
}



//	PWM12 中断函数：

void TIM2_IRQHandler(void)	
{
	if ( TIM_GetITStatus(TIM2, TIM_IT_Update) == SET )
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
		
		if(charge_state == charge)
		{
			v_8f_ad = AD_Value[3] * 100;
			i_8f_ad = AD_Value[2] * 100;
			switch(charge_mode)
			{
				case current_mode:
					break;
				case power_mode:
					break;
				case voltage_mode:
					if(v_8f_ad < v_8f_compare)duty12 ++ ;
					if(v_8f_ad > v_8f_compare)duty12 -- ;
					if(v_8f_ad == v_8f_compare)duty12 = duty12 ;
					PWM_Duty_Set_12(duty12);
					break;
			}
		}//charge_state==charge
		
		else if(charge_state == discharge )
		{
			if( discharge_mode == boost )
			{
				v_other_ad = AD_Value[0] * 100;
				i_other_ad = AD_Value[1] * 100;

			}
		}

	//以下两个“}”为TIM中断自带	
	}	
}


//	PWM34 中断函数：


void TIM1_UP_IRQHandler(void)	
{
	if ( TIM_GetITStatus(TIM1, TIM_IT_Update) == SET )
	{
		TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
		
		//charge_state == discharge && discharge_mode == buck
		v_other_ad = AD_Value[0] * 100;
		i_other_ad = AD_Value[1] * 100;
		
		
		


	//以下两个“}”为TIM中断自带	
	}	
}

//采样数值中断(送入反馈网络)

void TIM3_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) == SET)
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
		
		ad_count --;
			switch(charge_state)
			{
				case charge:v_8f_ad_sum += AD_Value[3];i_8f_ad_sum += AD_Value[2];
					if(ad_count == 0)
					{ad_count = 10;
					v_8f_ad = v_8f_ad_sum /ad_count;
					i_8f_ad = i_8f_ad_sum /ad_count;
					v_8f_ad_sum = 0; i_8f_ad_sum = 0;}break;
				case discharge:v_other_ad_sum += AD_Value[0];i_other_ad_sum += AD_Value[1];
					if(ad_count == 0)
					{ad_count = 10;
					v_other_ad = v_other_ad_sum / ad_count;
					i_other_ad = i_other_ad_sum / ad_count;
					v_other_ad_sum = 0; i_other_ad_sum = 0;}break;

			}	
	}//定时中断的if

}

//显示所需的数据支持、监控（protection）, 模式配置


void TIM4_IRQHandler(void)	
{
	if (TIM_GetITStatus(TIM4, TIM_IT_Update) == SET)
	{
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update) ;
		
		v_other_avg_sum += AD_Value[0];
		i_other_avg_sum += AD_Value[1];
		v_8f_avg_sum += AD_Value[2];
		i_8f_avg_sum += AD_Value[3];
		
		count_avg -- ;
		if(count_avg == 0)
		{
			count_avg = 800 ;
			//采样数据的平均值
			v_other_avg = v_other_avg_sum * 3300 / (4096 * count_avg );
			i_other_avg = i_other_avg_sum * 1650 / (4096 * count_avg );
			p_other_avg = v_other_avg * i_other_avg / 100;
			v_8f_avg = v_8f_avg_sum * 3300 / (4096 * count_avg );
			i_8f_avg = i_8f_avg_sum * 1650 / (4096 * count_avg );
			p_8f_avg = v_8f_avg * i_8f_avg / 100;
			
			//采样数据求和部分清零&舍弃次数清零
			v_other_avg_sum = 0;
			i_other_avg_sum = 0;
			v_8f_avg_sum = 0;
			i_8f_avg_sum = 0;
			
		
		//充电状态配置
		if(charge_state == charge)
		{	
			PWM_Duty_Set_34(3601);
			PWM34_Interrupt_turn_down();
			PWM12_Interrupt_turn_on();
			//充电模式选择
			if(v_8f_avg >= v_target_entry)charge_mode = voltage_mode;
			if(p_8f_avg >= p_target_entry)charge_mode = power_mode;
			else charge_mode = current_mode;
			//充电百分比数据支持->OLED
			charge_percent = v_8f_avg *100 / (u16)(Vtarget_8f * 100);
			
				//	OCP, OVP, OPP
//			if( v_8f_avg > ovp_8f_10e2 || 
//				i_8f_avg > ocp_8f_10e2 || 
//				p_8f_avg > opp_8f_10e2 )
//			{ protect(); }
		}
		
		//放电状态配置
		else if(charge_state == discharge)
		{
			//判断设定的other端收敛值和8F端当前值的大小差距，确定使用buck还是boost
			//BOOST
			if(v_other_compare > v_8f_avg)
			{
				PWM_Duty_Set_34(3601);
				PWM34_Interrupt_turn_down();
				PWM12_Interrupt_turn_on();
				discharge_mode = boost;
			}
			//BUCK
			else
			{
				PWM_Duty_Set_12(3601);
				PWM12_Interrupt_turn_down();
				PWM34_Interrupt_turn_on();
				discharge_mode = buck;
			}
			// OVP, OCP, OPP
//			if( v_other_avg > ovp_other_10e2 || 
//				i_other_avg > ocp_other_10e2 || 
//				p_other_avg > opp_other_10e2 )
//			{ protect(); }

		}
		
		else if(charge_state == no_charge_state)
		{
			PWM_Duty_Set_34(0);
			PWM_Duty_Set_12(0);	//电感放电
			
		}
		
		else if(charge_state == suspend){}//暂停状态，本次代码不做使用

		
			
			/*
			//	电压 AD 传入数组
			if( Voltage_AD_WriteIn_Times >= Voltage_AD_WriteIn_Array_Length)
			{return ;}
			Voltage_AD_WriteIn[ Voltage_AD_WriteIn_Times ] 
			= v_8f_avg ;
			Voltage_AD_WriteIn_Times ++ ;
			
			//	电流 AD 传入数组
			if( Current_AD_WriteIn_Times >= Current_AD_WriteIn_Array_Length)
			{return ;}
			Current_AD_WriteIn[ Current_AD_WriteIn_Times ] 
			= i_8f_avg ;
			Current_AD_WriteIn_Times ++ ;
			*/
			
		}
		
		/*
		

		*/
		
		
	//以下两个“}”为TIM中断自带		
	}

}
