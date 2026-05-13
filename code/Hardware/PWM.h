#ifndef __PWM_H
#define __PWM_H

void PWM_Init(void);
void PWM_Duty_Set(uint16_t Compare);
void PWM_Interrupt_on(void);
void PWM_Interrupt_off(void);

#endif




