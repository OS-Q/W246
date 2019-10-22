/**
******************************************************************************
* @file    			main.c                                              
* @version 			V1.2										  		
* @date    			2017-11-01								 
* @brief 				��3.5.0�汾�⽨�Ĺ���ģ��						  	
* @attention		�������ԣ��뱣֤W5500��IP�����PC����ͬһ�����ڣ��Ҳ���ͻ         
*								����������߸�PC��ֱ����������PC���������ӵ�ַIPΪ��̬IP
*				
* @company  		��������Ƽ����޹�˾
* @information  WIZnet W5500�ٷ������̣�ȫ�̼���֧�֣��۸�������ƣ�
* @website  		www.wisioe.com																							 	 				
* @forum        www.w5500.cn																								 		
* @qqGroup      383035001																										 
******************************************************************************
*/

/*W5500��STM32�� SPI1Ӳ�����߶���*/
//	W5500_SCS    --->     STM32_GPIOA4                /*W5500��Ƭѡ�ܽ�*/
//	W5500_SCLK	 --->     STM32_GPIOA5                /*W5500��ʱ�ӹܽ�*/
//  W5500_MISO	 --->     STM32_GPIOA6                /*W5500��MISO�ܽ�*/
//	W5500_MOSI	 --->     STM32_GPIOA7                /*W5500��MOSI�ܽ�*/
//	W5500_RESET	 --->     STM32_GPIOB1                /*W5500��RESET�ܽ�*/
//	W5500_INT    --->     STM32_GPIOC5                /*W5500��INT�ܽ�*/

/*W5500��STM32�� SPI2Ӳ�����߶���*/
//	W5500_SCS    --->     STM32_GPIOB12               /*W5500��Ƭѡ�ܽ�*/
//	W5500_SCLK	 --->     STM32_GPIOB13               /*W5500��ʱ�ӹܽ�*/
//  W5500_MISO	 --->     STM32_GPIOB14               /*W5500��MISO�ܽ�*/
//	W5500_MOSI	 --->     STM32_GPIOB15               /*W5500��MOSI�ܽ�*/
//	W5500_RESET	 --->     STM32_GPIOB1                /*W5500��RESET�ܽ�*/
//	W5500_INT    --->     STM32_GPIOC5                /*W5500��INT�ܽ�*/

#include <stdio.h>
#include <string.h>
#include "stm32f10x.h"
#include "bsp_usart1.h"
#include "bsp_i2c_ee.h"
#include "bsp_i2c_gpio.h"
#include "bsp_led.h"
#include "w5500.h"
#include "W5500_conf.h"
#include "socket.h"
#include "utility.h"
#include "tcp_demo.h"
#include "MqttKit.h"

int main(void)
{ 	 
	systick_init(72);				            	/*��ʼ��Systick����ʱ��*/
	USART1_Config(); 				            	/*��ʼ������ͨ��:115200@8-n-1*/
	i2c_CfgGpio();				    	        	/*��ʼ��eeprom*/
	timer2_init();	                      /*��ʼ����ʱ��*/
	LED_GPIO_Config();
	printf(" ����Ƽ�--WIZnet W5500�ٷ������̣�ȫ�̼���֧�֣��۸�������ƣ�\r\n");
	gpio_for_w5500_config();							/*��ʼ��MCU�������*/
	reset_w5500();												/*Ӳ��λW5500*/
	PHY_check();													/*��������Ƿ����*/   
	set_w5500_mac();											/*����MAC��ַ*/
	set_w5500_ip();												/*����IP��ַ*/
	socket_buf_init(txsize, rxsize);			/*��ʼ��8��Socket�ķ��ͽ��ջ����С*/
	
	while(1)                							/*ѭ��ִ�еĺ���*/ 
	{
		do_tcp_client();                    /*TCP_Client ���ݻػ����Գ���*/

		delay_ms(100);

		if(KEY_4 == 0) //����
		{
			MQTT_STATE = MQTT_PKT_SUBSCRIBE;
		}
		else if(KEY_5 == 0)//ȡ������
		{
			MQTT_STATE = MQTT_PKT_UNSUBSCRIBE;
		}
		else if(KEY_6 == 0)//����
		{
			MQTT_STATE =MQTT_PKT_PUBLISH;
		}	
	}
} 
