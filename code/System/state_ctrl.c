#include "stm32f10x.h"		// Device header
#include "Timer.h"
#include "ADTIM.h"
#include "PWM.h"
#include "define.h"
#include "Delay.h"
#include "protection.h"

void work_first_time_on (void)
{
	turn_zero();
	sw_in_on;	sw_out_on;		
}

void work_on (void)
{
	sw_in_on;	sw_out_on;	pwm_sd_on;
	ADTIM_Interrupt_on();
	Timer_Interrupt_on();
	PWM_Duty_Set(0);
	PWM_Interrupt_on();
	
	turn_zero();
}

void work_suspend (void)
{
	PWM_Interrupt_off();
	PWM_Duty_Set(0);Delay_ms(500);
	ADTIM_Interrupt_off();
	Timer_Interrupt_off();
	pwm_sd_off;
	
	turn_zero();
}

void work_continue (void)
{
	pwm_sd_on;
	ADTIM_Interrupt_on();
	Timer_Interrupt_on();
	PWM_Duty_Set(0);Delay_ms(500);
	PWM_Interrupt_on();
	
	turn_zero();
}

void work_off (void)
{
	PWM_Interrupt_off();
	PWM_Duty_Set(0);Delay_ms(500);
	ADTIM_Interrupt_off();
	Timer_Interrupt_off();
	pwm_sd_off;
	sw_out_off;		sw_in_off;
	
	turn_zero();
}



