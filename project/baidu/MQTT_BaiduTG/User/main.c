/**
******************************************************************************
* @file    			main.c                                              
* @version 			V1.2										  		
* @date    			2017-11-01								 
* @brief 				用3.5.0版本库建的工程模板						  	
* @attention		内网测试，请保证W5500的IP与测试PC机在同一网段内，且不冲突         
*								如果是用网线跟PC机直连，请设置PC机本地连接地址IP为静态IP
*				
* @company  		深圳炜世科技有限公司
* @information  WIZnet W5500官方代理商，全程技术支持，价格绝对优势！
* @website  		www.wisioe.com																							 	 				
* @forum        www.w5500.cn																								 		
* @qqGroup      383035001																										 
******************************************************************************
*/

/*W5500接STM32的 SPI1硬件接线定义*/
//	W5500_SCS    --->     STM32_GPIOA4                /*W5500的片选管脚*/
//	W5500_SCLK	 --->     STM32_GPIOA5                /*W5500的时钟管脚*/
//  W5500_MISO	 --->     STM32_GPIOA6                /*W5500的MISO管脚*/
//	W5500_MOSI	 --->     STM32_GPIOA7                /*W5500的MOSI管脚*/
//	W5500_RESET	 --->     STM32_GPIOB1                /*W5500的RESET管脚*/
//	W5500_INT    --->     STM32_GPIOC5                /*W5500的INT管脚*/

/*W5500接STM32的 SPI2硬件接线定义*/
//	W5500_SCS    --->     STM32_GPIOB12               /*W5500的片选管脚*/
//	W5500_SCLK	 --->     STM32_GPIOB13               /*W5500的时钟管脚*/
//  W5500_MISO	 --->     STM32_GPIOB14               /*W5500的MISO管脚*/
//	W5500_MOSI	 --->     STM32_GPIOB15               /*W5500的MOSI管脚*/
//	W5500_RESET	 --->     STM32_GPIOB1                /*W5500的RESET管脚*/
//	W5500_INT    --->     STM32_GPIOC5                /*W5500的INT管脚*/

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
	systick_init(72);				            	/*初始化Systick工作时钟*/
	USART1_Config(); 				            	/*初始化串口通信:115200@8-n-1*/
	i2c_CfgGpio();				    	        	/*初始化eeprom*/
	timer2_init();	                      /*初始化定时器*/
	LED_GPIO_Config();
	printf(" 炜世科技--WIZnet W5500官方代理商，全程技术支持，价格绝对优势！\r\n");
	gpio_for_w5500_config();							/*初始化MCU相关引脚*/
	reset_w5500();												/*硬复位W5500*/
	PHY_check();													/*检查网线是否接入*/   
	set_w5500_mac();											/*配置MAC地址*/
	set_w5500_ip();												/*配置IP地址*/
	socket_buf_init(txsize, rxsize);			/*初始化8个Socket的发送接收缓存大小*/
	
	while(1)                							/*循环执行的函数*/ 
	{
		do_tcp_client();                    /*TCP_Client 数据回环测试程序*/

		delay_ms(100);

		if(KEY_4 == 0) //订阅
		{
			MQTT_STATE = MQTT_PKT_SUBSCRIBE;
		}
		else if(KEY_5 == 0)//取消订阅
		{
			MQTT_STATE = MQTT_PKT_UNSUBSCRIBE;
		}
		else if(KEY_6 == 0)//发布
		{
			MQTT_STATE =MQTT_PKT_PUBLISH;
		}	
	}
} 
