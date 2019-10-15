
#include "usart.h"
#include "config.h"
#include "device.h"
#include "spi2.h"
#include "socket.h"
#include "w5500.h"
//#include "24c16.h"
#include "ult.h"
#include "md5.h"
#include "string.h"
#include "ntp.h"
#include <stdio.h>
#include "tcp_client.h"
#include "timer.h"
#include "usart.h"
#include "dns.h"


extern uint8 txsize[];
extern uint8 rxsize[];
extern tstamp Total_Seconds;
//extern const	uint8 str1; 
uint8 buffer[2048]={"TKKMt4nMF8U.iot-as-mqtt.cn-shanghai.aliyuncs.com"};/*定义一个2KB的缓存*/
																																				/*DNS解析域名*/

int main()
{
	uint8 *domain_name;
	uint8 dns_retry_cnt=0;
  	uint8 dns_ok=0;
	uint8 len,dns_flag=0;				// 定义串口输入的数据长度、初始化DNS标志位
			
	Systick_Init(72);/* 初始化Systick工作时钟*/ 

	GPIO_Configuration();/* 配置GPIO*/
		
	USART1_Init(); /*初始化串口通信:115200@8-n-1*/
		
	//at24c16_init();/*初始化eeprom*/
		
	TIM2_NVIC_Configuration();
	TIM2_Configuration();
	printf("W5500 EVB initialization over.\r\n");

	Reset_W5500();/*硬重启W5500*/
	WIZ_SPI_Init();/*初始化SPI接口*/

	set_default(); 	
	set_network();
	
	ntpclient_init(); 
	printf("W5500 Init Complete!\r\n");
 	printf("Start DNS Test!\r\n");	
	printf("\r\n[ %s ]'s IP Address is:\r\n",buffer);
  
	 while(1)
  	{	
		dns_flag=1;													// DNS标志位置1
		if(dns_flag==1)
		{
			
			if(dns_num>=6)											// DNS次数≥6
			{
				dns_flag=0;												// DNS标志位清0
				dns_num=0;												// dns_num清0
			}
			else
				do_dns(buffer);										// DNS过程			
		}
	}
}