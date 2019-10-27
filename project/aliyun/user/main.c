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

#include "w5500.h"
#include "W5500_conf.h"
#include "socket.h"
#include "utility.h"
#include "dhcp.h"

#include "tcp_client.h"
#include "ntp.h"
#include "dns.h"


int hal_init(void)
{ 	
	systick_init(72);				            		/*初始化Systick工作时钟*/
	USART1_Config(); 				            		/*初始化串口通信:115200@8-n-1*/
	dog_init(IWDG_Prescaler_256,2000);					//12s
	//i2c_CfgGpio();				    	        	/*初始化eeprom*/
	w5500_init();
	timer_init();										/*初始化定时器*/

	return 0;
}


int net_init(void)
{ 	
	socket_buf_init(txsize, rxsize);					/*初始化8个Socket的发送接收缓存大小*/
	PHY_check();										/*检查网线是否接入*/   
	//set_mac();										/*配置MAC地址*/
	data_init();										/*配置IP地址*/
	//dhcp_init();	
	return 0;
}

int main(void)
{ 
 	hal_init();
	net_init();
	dns_init();
	ntp_init(); 
	do_ntp_client();
	while(1)								 		
	{	
		PHY_check();					// 断线检测	
		connect_ali();                   //MQTT配置
		// do_ntp_client(); 
		// do_tcp_server();                    /*TCP_Server 数据回环测试程序*/
		// do_tcp_client();					/*TCP_Client 数据回环测试程序*/
		// do_udp();							/*UDP        数据回环测试程序*/
  	}
} 
