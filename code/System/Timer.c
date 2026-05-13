#include "stm32f10x.h"		// Device header
#include "Timer.h"
#include "ADTIM.h"
#include "PWM.h"
#include "define.h"
#include "Delay.h"
#include "protection.h"
#include "AD.h"

/**
  * 函    数：定时中断初始化
  * 参    数：无
  * 返 回 值：无
  */
void Timer_Init(void)
{
	/*开启时钟*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);			//开启TIM4的时钟
	
	/*配置时钟源*/
	TIM_InternalClockConfig(TIM4);		//选择TIM4为内部时钟，若不调用此函数，TIM默认也为内部时钟
	
	/*时基单元初始化*/
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;				//定义结构体变量
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;		//时钟分频，选择不分频，此参数用于配置滤波器时钟，不影响时基单元功能
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;	//计数器模式，选择向上计数
	TIM_TimeBaseInitStructure.TIM_Period = 1000 - 1;				//计数周期，即ARR的值
	TIM_TimeBaseInitStructure.TIM_Prescaler = 36 - 1;				//预分频器，即PSC的值(周期T=0.5ms)
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;			//重复计数器，高级定时器才会用到
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStructure);				//将结构体变量交给TIM_TimeBaseInit，配置TIM3的时基单元	
	
	/*中断输出配置*/
	TIM_ClearFlag(TIM4, TIM_FLAG_Update);						//清除定时器更新标志位
																//TIM_TimeBaseInit函数末尾，手动产生了更新事件
																//若不清除此标志位，则开启中断后，会立刻进入一次中断
																//如果不介意此问题，则不清除此标志位也可
	//TIM4的更新中断
	TIM_ITConfig(TIM4, TIM_IT_Update, DISABLE);					
	
	/*NVIC中断分组*/
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);				//配置NVIC为分组2
																//即抢占优先级范围：0~3，响应优先级范围：0~3
																//此分组配置在整个工程中仅需调用一次
																//若有多个中断，可以把此代码放在main函数内，while循环之前
																//若调用多次配置分组的代码，则后执行的配置会覆盖先执行的配置
	
	/*NVIC配置*/
	NVIC_InitTypeDef NVIC_InitStructure;						//定义结构体变量
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;				//选择配置NVIC的TIM4线
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//指定NVIC线路使能
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	//指定NVIC线路的抢占优先级为1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;			//指定NVIC线路的响应优先级为1
	NVIC_Init(&NVIC_InitStructure);								//将结构体变量交给NVIC_Init，配置NVIC外设
	
	/*TIM使能*/
	TIM_Cmd(TIM4, ENABLE);			//使能TIM4，定时器开始运行
}

//定时器中断开启和关闭函数
void Timer_Interrupt_on(void)
{
	TIM_ClearITPendingBit(TIM4, TIM_IT_Update);	
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
}

void Timer_Interrupt_off(void)
{
	TIM_ClearITPendingBit(TIM4, TIM_IT_Update);	
	TIM_ITConfig(TIM4, TIM_IT_Update, DISABLE);
}



/* 定时器中断函数，可以复制到使用它的地方

  * 函    数：TIM4中断函数
  * 参    数：无
  * 返 回 值：无
  * 注意事项：此函数为中断函数，无需调用，中断触发后自动执行
  *           函数名为预留的指定名称，可以从启动文件复制
  *           请确保函数名正确，不能有任何差异，否则中断函数将不能进入
*/
void TIM4_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM4, TIM_IT_Update) == SET)
	{
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
		
		
		count_avg -- ;
		v_in_avg_sum += (AD_Value[0] * 3300 / 4096) ;	//最大采样33V
		i_in_avg_sum += (AD_Value[1] * 660 / 4096) ;	//最大采样3.3A
		v_out_avg_sum += (AD_Value[3] * 3300 / 4096) ;
		i_out_avg_sum += (AD_Value[2] * 660 / 4096) ;
		
		if(count_avg == 0)
		{
			count_avg = 800 ;
			//采样数据的平均值
			v_out_avg = v_out_avg_sum / count_avg ;
			i_out_avg = i_out_avg_sum / count_avg - 330;
			p_out_avg = v_out_avg * i_out_avg / 100 ;
			v_in_avg = v_in_avg_sum / count_avg ;
			i_in_avg = i_in_avg_sum / count_avg - 330;
			p_in_avg = v_in_avg * i_in_avg / 100 ;
			

			//采样数据求和部分清零
			v_out_avg_sum = 0;
			i_out_avg_sum = 0;
			v_in_avg_sum = 0;
			i_in_avg_sum = 0;		
			
			//平均值线性校准
			v_out_avg = v_out_avg * 1.0011 + 7.891;
			i_out_avg = i_out_avg * 1.1458 + 1.0552;
			v_in_avg = v_in_avg * 1.0011 + 7.891;
			i_in_avg = i_in_avg * 1.155 - 0.1078;
		}

	}
}




