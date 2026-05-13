#include "stm32f10x.h"                  // Device header

void SystemClock_Config(void)
{
  RCC_DeInit(); // 复位RCC寄存器到默认值
  
  // 1. 开启外部晶振并等待稳定
  RCC_HSEConfig(RCC_HSE_ON);
  if (RCC_WaitForHSEStartUp() != SUCCESS)
  {
    while(1); // HSE启动失败，死循环提示
  }
  
  // 2. 配置Flash延迟（72MHz时需要2个等待周期）
  FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
  FLASH_SetLatency(FLASH_Latency_2);
  
  // 3. 配置分频器
  RCC_HCLKConfig(RCC_SYSCLK_Div1); // AHB分频=1
  RCC_PCLK1Config(RCC_HCLK_Div2);  // APB1分频=2
  RCC_PCLK2Config(RCC_HCLK_Div1);  // APB2分频=1
  
  // 4. 配置PLL（HSE×9=72MHz）
  RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
  RCC_PLLCmd(ENABLE);
  while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET); // 等待PLL稳定
  
  // 5. 选择PLL作为系统时钟
  RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
  while(RCC_GetSYSCLKSource() != 0x08); // 确认系统时钟已切换到PLL
}


