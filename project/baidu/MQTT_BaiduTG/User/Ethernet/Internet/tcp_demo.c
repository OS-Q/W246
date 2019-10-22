/**
******************************************************************************
* @file    			tcp_demo.c                                               
* @version 			V1.2										  		
* @date    			2017-11-01								 
* @brief 			  TCP ��ʾ����     					        
*
* @company  		��������Ƽ����޹�˾
* @information  WIZnet W5500�ٷ������̣�ȫ�̼���֧�֣��۸�������ƣ�
* @website  		www.wisioe.com																							 	 				
* @forum        www.w5500.cn																								 		
* @qqGroup      383035001																										 
******************************************************************************
*/

#include <stdio.h>
#include <string.h>
#include "tcp_demo.h"
#include "W5500_conf.h"
#include "w5500.h"
#include "socket.h"
#include "BD_TG.h"
#include "MqttKit.h"
#include "utility.h"
#include "bsp_led.h"
uint8 buff[2048];				                             /*����һ��2KB�Ļ���*/
uint8  BD_TG_server_ip[4] = {163,177,150,12};        //BD_TG������IP��ַ
uint16 BD_TG_server_port  = 1883;                   //BD_TG�������˿ں�


int MQTT_STATE = MQTT_PKT_CONNECT;   //����
const char *topics[] = {"test"};
uint8 BD_TG_ping_pak[2] = {0xC0,0x00};

unsigned char *data_ptr = NULL;
/**
*@brief		TCP Client�ػ���ʾ������
*@param		��
*@return	��
*/
void do_tcp_client(void)
{	
	uint16 len=0;	

	switch(getSn_SR(SOCK_TCPC))								  				         /*��ȡsocket��״̬*/
	{
		case SOCK_CLOSED:											        		         /*socket���ڹر�״̬*/
			socket(SOCK_TCPC,Sn_MR_TCP,local_port++,Sn_MR_ND);
		break;

		case SOCK_INIT:													        	         /*socket���ڳ�ʼ��״̬*/
			connect(SOCK_TCPC,BD_TG_server_ip,BD_TG_server_port);                /*socket���ӷ�����*/ 
		break;

		case SOCK_ESTABLISHED: 												             /*socket�������ӽ���״̬*/
			if(getSn_IR(SOCK_TCPC) & Sn_IR_CON)
			{
				setSn_IR(SOCK_TCPC, Sn_IR_CON); 							         /*��������жϱ�־λ*/
			}

			len=getSn_RX_RSR(SOCK_TCPC); 								  	         /*����lenΪ�ѽ������ݵĳ���*/
			if(len>0)
			{
				recv(SOCK_TCPC,buff,len); 							   		         /*��������Server������*/			
				data_ptr = buff;
				if(data_ptr != NULL)
					BD_TG_RevPro(data_ptr);
				if(publish_buf[0] == 0x31)
				{	
					LED_ALL_ON;
				}			
				else if(publish_buf[0] == 0x32)	
				{	
					LED_ALL_OFF;
				}	
			}	
		
			switch(MQTT_STATE)
			{
				/*MQTTЭ������BD_TG����ƽ̨*/
				case MQTT_PKT_CONNECT:
					BD_TG_DevLink();
					MQTT_STATE = MQTT_PKT_PINGREQ;
				break;
				/*��������*/
				case MQTT_PKT_SUBSCRIBE:
					BD_TG_Subscribe(topics,1);
					MQTT_STATE = MQTT_PKT_PINGREQ;
				break;	
				/*Qos2���𷢲���Ϣ*/
				case MQTT_PKT_PUBLISH:			
					
					BD_TG_Publish(*topics, "MQTT Publish Test"); //������Ϣ
					delay_ms(300);                                 //�ȴ�ƽ̨��Ӧ    
					/*����ƽ̨���͵�PubRec���ظ�PubRel��Ӧ*/		 
					len=getSn_RX_RSR(SOCK_TCPC); 
					recv(SOCK_TCPC,buff,len); 
					data_ptr = buff;
					if(data_ptr != NULL)
						BD_TG_RevPro(data_ptr);
					delay_ms(100);                                 //PubRel��Ӧ�ȴ�ƽ̨��Ӧ
					len=getSn_RX_RSR(SOCK_TCPC); 
					recv(SOCK_TCPC,buff,len); 
					data_ptr = buff;
					if(data_ptr != NULL)
						BD_TG_RevPro(data_ptr);
					
					MQTT_STATE = MQTT_PKT_PINGREQ;
				/*120�뷢��һ��Ping��Ӧ���ֳ�����*/
				break;
				case MQTT_PKT_UNSUBSCRIBE:
					MQTT_UnSubscribe(topics,1);
					MQTT_STATE = MQTT_PKT_PINGREQ;
				break;
				case MQTT_PKT_PINGREQ:
					if(BD_TG_ping_time > 120)
					{
						send(SOCK_TCPC,BD_TG_ping_pak,2);
						BD_TG_ping_time = 0;
					}
				break;
			}	
			
		
    break;

	case SOCK_CLOSE_WAIT: 											    	         /*socket���ڵȴ��ر�״̬*/
		close(SOCK_TCPC);
		break;
	}
}


