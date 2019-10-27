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
	systick_init(72);				            		/*��ʼ��Systick����ʱ��*/
	USART1_Config(); 				            		/*��ʼ������ͨ��:115200@8-n-1*/
	dog_init(IWDG_Prescaler_256,2000);					//12s
	//i2c_CfgGpio();				    	        	/*��ʼ��eeprom*/
	w5500_init();
	timer_init();										/*��ʼ����ʱ��*/

	return 0;
}


int net_init(void)
{ 	
	socket_buf_init(txsize, rxsize);					/*��ʼ��8��Socket�ķ��ͽ��ջ����С*/
	PHY_check();										/*��������Ƿ����*/   
	//set_mac();										/*����MAC��ַ*/
	data_init();										/*����IP��ַ*/
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
		PHY_check();					// ���߼��	
		connect_ali();                   //MQTT����
		// do_ntp_client(); 
		// do_tcp_server();                    /*TCP_Server ���ݻػ����Գ���*/
		// do_tcp_client();					/*TCP_Client ���ݻػ����Գ���*/
		// do_udp();							/*UDP        ���ݻػ����Գ���*/
  	}
} 
