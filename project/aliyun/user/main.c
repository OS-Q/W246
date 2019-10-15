
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
uint8 buffer[2048]={"TKKMt4nMF8U.iot-as-mqtt.cn-shanghai.aliyuncs.com"};/*����һ��2KB�Ļ���*/
																																				/*DNS��������*/

int main()
{
	uint8 *domain_name;
	uint8 dns_retry_cnt=0;
  	uint8 dns_ok=0;
	uint8 len,dns_flag=0;				// ���崮����������ݳ��ȡ���ʼ��DNS��־λ
			
	Systick_Init(72);/* ��ʼ��Systick����ʱ��*/ 

	GPIO_Configuration();/* ����GPIO*/
		
	USART1_Init(); /*��ʼ������ͨ��:115200@8-n-1*/
		
	//at24c16_init();/*��ʼ��eeprom*/
		
	TIM2_NVIC_Configuration();
	TIM2_Configuration();
	printf("W5500 EVB initialization over.\r\n");

	Reset_W5500();/*Ӳ����W5500*/
	WIZ_SPI_Init();/*��ʼ��SPI�ӿ�*/

	set_default(); 	
	set_network();
	
	ntpclient_init(); 
	printf("W5500 Init Complete!\r\n");
 	printf("Start DNS Test!\r\n");	
	printf("\r\n[ %s ]'s IP Address is:\r\n",buffer);
  
	 while(1)
  	{	
		dns_flag=1;													// DNS��־λ��1
		if(dns_flag==1)
		{
			
			if(dns_num>=6)											// DNS������6
			{
				dns_flag=0;												// DNS��־λ��0
				dns_num=0;												// dns_num��0
			}
			else
				do_dns(buffer);										// DNS����			
		}
	}
}