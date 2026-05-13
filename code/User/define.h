#ifndef __DEFINE_H
#define __DEFINE_H

//收敛目标值
extern int32_t Vtarget;
extern int32_t Itarget;

//直接用于比较
extern int32_t Vref ;
extern int32_t Iref ;
extern int32_t Iref_final;

#define		ovp		32.00f
#define		ocp		2.20f
#define		opp		100.00f


//蜂鸣器开启&关闭
#define		buzzer_on	GPIO_WriteBit(GPIOA,GPIO_Pin_11,Bit_SET)
#define		buzzer_off	GPIO_WriteBit(GPIOA,GPIO_Pin_11,Bit_RESET)

//PWM_SD & SW继电器开关
#define		pwm_sd_on	GPIO_WriteBit(GPIOA,GPIO_Pin_5,Bit_SET)
#define		pwm_sd_off	GPIO_WriteBit(GPIOA,GPIO_Pin_5,Bit_RESET)

#define		sw_in_on	GPIO_WriteBit(GPIOA,GPIO_Pin_4,Bit_SET)
#define		sw_in_off	GPIO_WriteBit(GPIOA,GPIO_Pin_4,Bit_RESET)

#define		sw_out_on	GPIO_WriteBit(GPIOB,GPIO_Pin_10,Bit_SET)
#define		sw_out_off	GPIO_WriteBit(GPIOB,GPIO_Pin_10,Bit_RESET)


//数据处理部分
extern u32 v_out;
extern int32_t i_out;
extern u32 v_in;
extern int32_t i_in;


//控制部分
extern u16 voltage_count;
extern u16 current_count;
extern int32_t	v_filter;
extern int32_t	i_filter;

extern int32_t Iref_final_p;
extern int32_t	Iref_final_integral;
extern int32_t duty_p;
extern int32_t duty_integral;

//PWM的占空比
extern int32_t duty;




//平均数处理部分
extern u16 count_avg;
extern u16 abandon_times;
extern u32 v_out_avg;
extern int32_t i_out_avg;
extern int32_t p_out_avg;
extern u32 v_in_avg;
extern int32_t i_in_avg;
extern int32_t p_in_avg;


extern u32 v_out_avg_sum;
extern u32 i_out_avg_sum;
extern u32 v_in_avg_sum;
extern u32 i_in_avg_sum;




//保护部分
extern u32	ovp_10e2 ;
extern int32_t	ocp_10e2 ;
extern int32_t	opp_10e2 ;


//串口
extern u32 power[500];
extern u16 power_write_times;

//归零
void turn_zero(void);

//Vref 显示调节
extern u8 place[4];
extern int8_t i;


#endif





