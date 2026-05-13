#include "stm32f10x.h"                  // Device header
#include "define.h"

//u32 duty12_writein[1500];
//u16 duty12times=0;

//目标收敛值
int32_t Vtarget	= 1200;
int32_t Itarget	= 200;

//直接用于比较
int32_t Vref = 0;
int32_t Iref = 0;
int32_t Iref_final = 0;

//数据处理部分
u32 v_out = 0;
int32_t i_out = 0;
u32 v_in = 0;
int32_t i_in = 0;

//控制部分
u16 voltage_count = 0;
u16 current_count = 0;
int32_t	v_filter = 0;
int32_t	i_filter = 0;

int32_t Iref_final_p = 0;
int32_t	Iref_final_integral = 0;
int32_t duty_p = 0;
int32_t duty_integral = 0;

//PWM的占空比
int32_t duty = 0;


//平均数处理部分
u16 count_avg = 800;
u16 abandon_times = 0;
u32 v_out_avg = 0;
int32_t i_out_avg = 0;
int32_t p_out_avg = 0;
u32 v_in_avg = 0;
int32_t i_in_avg = 0;
int32_t p_in_avg = 0;

u32 v_out_avg_sum = 0;
u32 i_out_avg_sum = 0;
u32 v_in_avg_sum = 0;
u32 i_in_avg_sum = 0;


//保护部分
u32	ovp_10e2 = ovp * 100;
int32_t	ocp_10e2 = ocp * 100;
int32_t	opp_10e2 = opp * 100;


//无充电
u8 no_charge_state_symb = 1;
u8 basic = 0;
//串口
u32 power[500];
u16 power_write_times = 0;

//归零
void turn_zero(void)
{
	//PID部分
	voltage_count = 0;	current_count = 0;	duty = 0;
	Iref_final = 0;
	v_filter = 0;		i_filter = 0;	
	Iref_final_p = 0;		Iref_final_integral = 0;
	duty_p = 0;		duty_integral = 0;

	
	//平均数处理部分
	count_avg = 800;
	v_out_avg = 0;	i_out_avg = 0;	p_out_avg = 0;
	v_in_avg = 0;	i_in_avg = 0;	p_in_avg = 0;
	v_out_avg_sum = 0;	i_out_avg_sum = 0;
	v_in_avg_sum = 0;	i_in_avg_sum = 0;
	
}

//Vref 显示调节
u8 place[4] = {48,56,72,80};
int8_t i=3;


