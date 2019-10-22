#ifndef __LED_H
#define	__LED_H

#include "stm32f10x.h"
#include "W5500_conf.h"

/** the macro definition to trigger the led on or off 
* 1 - off
* 0 - on
*/
//#define ON  0
//#define OFF 1

/* ���κ꣬��������������һ��ʹ�� */
#define LED1(a)	if (a)	\
					GPIO_SetBits(GPIOB,GPIO_Pin_0);\
					else		\
					GPIO_ResetBits(GPIOB,GPIO_Pin_0)

#define LED2(a)	if (a)	\
					GPIO_SetBits(GPIOC,GPIO_Pin_4);\
					else		\
					GPIO_ResetBits(GPIOC,GPIO_Pin_4)

#define LED3(a)	if (a)	\
					GPIO_SetBits(GPIOC,GPIO_Pin_3);\
					else		\
					GPIO_ResetBits(GPIOC,GPIO_Pin_3)

/* ֱ�Ӳ����Ĵ����ķ�������IO */
#define	digitalHi(p,i)			  {p->BSRR=i;}			  //����Ϊ�ߵ�ƽ		
#define digitalLo(p,i)			  {p->BRR=i;}				  //����͵�ƽ
#define digitalToggle(p,i)		{p->ODR ^=i;}			  //�����ת״̬

/* �������IO�ĺ� */
#define LED1_TOGGLE		digitalToggle(GPIOB,GPIO_Pin_0)
#define LED1_OFF		digitalHi(GPIOB,GPIO_Pin_0)
#define LED1_ON		digitalLo(GPIOB,GPIO_Pin_0)

#define LED2_TOGGLE		digitalToggle(GPIOC,GPIO_Pin_4)
#define LED2_OFF		digitalHi(GPIOC,GPIO_Pin_4)
#define LED2_ON		digitalLo(GPIOC,GPIO_Pin_4)

#define LED3_TOGGLE		digitalToggle(GPIOC,GPIO_Pin_3)
#define LED3_OFF		digitalHi(GPIOC,GPIO_Pin_3)
#define LED3_ON		digitalLo(GPIOC,GPIO_Pin_3)

#define LED_ALL_OFF GPIO_SetBits(GPIOA, GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3)
#define LED_ALL_ON  GPIO_ResetBits(GPIOA, GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3)

#define KEY_4  GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4)
#define KEY_5  GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_5)
#define KEY_6  GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6)

void LED_GPIO_Config(void);

#endif /* __LED_H */
