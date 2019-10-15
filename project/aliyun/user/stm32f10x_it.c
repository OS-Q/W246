
#include "stm32f10x_it.h"
extern int count;
void NMI_Handler(void)
{
}

void HardFault_Handler(void)
{
  while (1)
  {
  }
}


void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}


void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

void SVC_Handler(void)
{
}


void DebugMon_Handler(void)
{
}


void PendSV_Handler(void)
{
}

void SysTick_Handler(void)
{
}
/*
void EXTI9_5_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line6) != RESET)
	{
		ISR();//中断处理

		EXTI_ClearITPendingBit(EXTI_Line6);       
	}                  
}
*/

/************
*@brief:timer2 中断处理函数
*@param:无
*@return:无
 *******************/
void TIM2_IRQHandler(void)
{	
	if ( TIM_GetITStatus(TIM2 , TIM_IT_Update) != RESET ) 
	{		
     count++;		
		TIM_ClearITPendingBit(TIM2 , TIM_FLAG_Update);  		 
	}		 	
}
