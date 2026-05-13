#include "stm32f10x.h"		// Device header
#include "Timer.h"
#include "ADTIM.h"
#include "PWM.h"
#include "define.h"
#include "Delay.h"
#include "protection.h"
#include "AD.h"

/**
  * 函    数：PWM初始化
  * 参    数：无
  * 返 回 值：无
  */
void PWM_Init(void)
{
	/*开启时钟*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);			//开启TIM3的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);			//开启GPIOA的时钟
	
	/*GPIO重映射*/
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);			//开启AFIO的时钟，重映射必须先开启AFIO的时钟
//	GPIO_PinRemapConfig(GPIO_PartialRemap1_TIM4, ENABLE);			//将TIM4的引脚部分重映射，具体的映射方案需查看参考手册
//	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);		//将JTAG引脚失能，作为普通GPIO引脚使用
	
	/*GPIO初始化*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;		
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);							//将PA6引脚初始化为复用推挽输出	
																	//受外设控制的引脚，均需要配置为复用模式		
	
	/*配置时钟源*/
	TIM_InternalClockConfig(TIM3);		//选择TIM3为内部时钟
	
	/*时基单元初始化*/
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;				//定义结构体变量
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;     //时钟分频，选择不分频，此参数用于配置滤波器时钟，不影响时基单元功能
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up; //计数器模式
	TIM_TimeBaseInitStructure.TIM_Period = 3600 - 1;					//计数周期，即ARR的值(由于中心对齐, ARR和PSC有一个要减半)
	TIM_TimeBaseInitStructure.TIM_Prescaler = 1 - 1;				//预分频器，即PSC的值,(ARR+1)*(PSC+1)=72M/FreqTarget
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;            //重复计数器，高级定时器才会用到
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);             //将结构体变量交给TIM_TimeBaseInit，配置TIM3的时基单元
	
	/*中断输出配置*/
	TIM_ClearFlag(TIM3, TIM_FLAG_Update);						//清除定时器更新标志位
																//TIM_TimeBaseInit函数末尾，手动产生了更新事件
																//若不清除此标志位，则开启中断后，会立刻进入一次中断
																//如果不介意此问题，则不清除此标志位也可
	
	/*输出比较初始化*/
	TIM_OCInitTypeDef TIM_OCInitStructure;							//定义结构体变量
	TIM_OCStructInit(&TIM_OCInitStructure);							//结构体初始化，若结构体没有完整赋值
																	//则最好执行此函数，给结构体所有成员都赋一个默认值
																	//避免结构体初值不确定的问题
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;				//输出比较模式，选择PWM模式1
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;		//输出极性，选择为高，若选择极性为低，则输出高低电平取反
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	//输出使能
	TIM_OCInitStructure.TIM_Pulse = 0;								//初始的CCR值为0
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);						//将结构体变量交给TIM_OC1Init，配置TIM2的输出比较通道1(PA6)
	
	//预装载, 可以让 CCR 和 ARR 修改值在下一个计数(中断)周期生效
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);
	TIM_ARRPreloadConfig(TIM3, ENABLE);
	
	//更新中断机制
	TIM_ITConfig(TIM3, TIM_IT_Update, DISABLE);
	
	//设置中断分组
	/*NVIC配置*/
	NVIC_InitTypeDef NVIC_InitStructure;						//定义结构体变量
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;				//选择配置NVIC的TIM3线
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//指定NVIC线路使能
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	//指定NVIC线路的抢占优先级为0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;			//指定NVIC线路的响应优先级为0
	NVIC_Init(&NVIC_InitStructure);								//将结构体变量交给NVIC_Init，配置NVIC外设
	
	/*TIM使能*/
	TIM_Cmd(TIM3, ENABLE);			//使能TIM3，定时器开始运行
}

/**
  * 函    数：PWM设置CCR
  * 参    数：Compare 要写入的CCR的值，范围：0 ~ CCR_MAX
  * 返 回 值：无
  * 注意事项：CCR和ARR共同决定占空比，此函数仅设置CCR的值，并不直接是占空比
  *           占空比Duty = CCR / (ARR + 1)
  *			  这里由于PSC = 1, 故 ARR = CCR_MAX
  */
void PWM_Duty_Set(uint16_t Compare)
{
	TIM_SetCompare1(TIM3, Compare);		//设置CCR1的值
}


//定时器中断开启和关闭函数
void PWM_Interrupt_on(void)
{
	TIM_ClearITPendingBit(TIM3, TIM_IT_Update);	
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
}

void PWM_Interrupt_off(void)
{
	TIM_ClearITPendingBit(TIM3, TIM_IT_Update);	
	TIM_ITConfig(TIM3, TIM_IT_Update, DISABLE);
}


/* 定时器中断函数，可以复制到使用它的地方

  * 函    数：TIM3中断函数
  * 参    数：无
  * 返 回 值：无
  * 注意事项：此函数为中断函数，无需调用，中断触发后自动执行
  *           函数名为预留的指定名称，可以从启动文件复制
  *           请确保函数名正确，不能有任何差异，否则中断函数将不能进入
*/
void TIM3_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) == SET)
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
		

		//电压环
		voltage_count ++;
		if(voltage_count == 10)
		{	voltage_count = 0;
		//Pv
					Iref_final_p = (Vref - v_filter) * 37 / 250; // 0.148
	
		//Iv
					if(v_filter < Vref - 2)	Iref_final_integral ++ ;

					if(v_filter > Vref - 2)	Iref_final_integral -- ;

					else;
			
		//积分限幅
					if(Iref_final_integral >= Iref) Iref_final_integral = Iref;
					if(Iref_final_integral <= -Iref) Iref_final_integral = -Iref;								

					
			Iref_final = Iref_final_p + Iref_final_integral;
					

		//总输出限幅保护，避免 Iref_final 溢出
			if(Iref_final >= Iref) Iref_final = Iref;
			if(Iref_final <= -Iref) Iref_final = -Iref;								
		//参考值线性校准
			Iref_final = 0.8696 * Iref_final + 0.7517;
		}//		if(voltage_count == 10)				
				
			//test
//		Iref_final = Iref;
			
		//以下代码（电流环）完全正确，可以正常使用，主要问题不在这			
						
						
		//电流环
		//Pi
					duty_p = (Iref_final - i_filter) * 78 / 100 ; //0.78
						
		//Ii		
					if(i_filter > Iref_final + 5)	duty_integral -- ;

					if(i_filter < Iref_final + 5)	duty_integral ++ ;

					else;

		//积分限幅
					if(duty_integral >= 3200) duty_integral = 3200;	
					if(duty_integral <= -360) duty_integral = -360;	
					
			duty = duty_p + duty_integral;

		
		//总输出限幅保护，避免 PWM占空比 溢出
			if(duty >= 3200) duty = 3200;
			if(duty <= 0) duty = 0;
			
			PWM_Duty_Set(duty);

		
	}
}






