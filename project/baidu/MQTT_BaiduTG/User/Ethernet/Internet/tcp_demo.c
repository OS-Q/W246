/**
******************************************************************************
* @file    			tcp_demo.c                                               
* @version 			V1.2										  		
* @date    			2017-11-01								 
* @brief 			  TCP 演示函数     					        
*
* @company  		深圳炜世科技有限公司
* @information  WIZnet W5500官方代理商，全程技术支持，价格绝对优势！
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
uint8 buff[2048];				                             /*定义一个2KB的缓存*/
uint8  BD_TG_server_ip[4] = {163,177,150,12};        //BD_TG服务器IP地址
uint16 BD_TG_server_port  = 1883;                   //BD_TG服务器端口号


int MQTT_STATE = MQTT_PKT_CONNECT;   //连接
const char *topics[] = {"test"};
uint8 BD_TG_ping_pak[2] = {0xC0,0x00};

unsigned char *data_ptr = NULL;
/**
*@brief		TCP Client回环演示函数。
*@param		无
*@return	无
*/
void do_tcp_client(void)
{	
	uint16 len=0;	

	switch(getSn_SR(SOCK_TCPC))								  				         /*获取socket的状态*/
	{
		case SOCK_CLOSED:											        		         /*socket处于关闭状态*/
			socket(SOCK_TCPC,Sn_MR_TCP,local_port++,Sn_MR_ND);
		break;

		case SOCK_INIT:													        	         /*socket处于初始化状态*/
			connect(SOCK_TCPC,BD_TG_server_ip,BD_TG_server_port);                /*socket连接服务器*/ 
		break;

		case SOCK_ESTABLISHED: 												             /*socket处于连接建立状态*/
			if(getSn_IR(SOCK_TCPC) & Sn_IR_CON)
			{
				setSn_IR(SOCK_TCPC, Sn_IR_CON); 							         /*清除接收中断标志位*/
			}

			len=getSn_RX_RSR(SOCK_TCPC); 								  	         /*定义len为已接收数据的长度*/
			if(len>0)
			{
				recv(SOCK_TCPC,buff,len); 							   		         /*接收来自Server的数据*/			
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
				/*MQTT协议连接BD_TG代理平台*/
				case MQTT_PKT_CONNECT:
					BD_TG_DevLink();
					MQTT_STATE = MQTT_PKT_PINGREQ;
				break;
				/*订阅主题*/
				case MQTT_PKT_SUBSCRIBE:
					BD_TG_Subscribe(topics,1);
					MQTT_STATE = MQTT_PKT_PINGREQ;
				break;	
				/*Qos2级别发布消息*/
				case MQTT_PKT_PUBLISH:			
					
					BD_TG_Publish(*topics, "MQTT Publish Test"); //发布消息
					delay_ms(300);                                 //等待平台响应    
					/*接收平台发送的PubRec并回复PubRel响应*/		 
					len=getSn_RX_RSR(SOCK_TCPC); 
					recv(SOCK_TCPC,buff,len); 
					data_ptr = buff;
					if(data_ptr != NULL)
						BD_TG_RevPro(data_ptr);
					delay_ms(100);                                 //PubRel响应等待平台响应
					len=getSn_RX_RSR(SOCK_TCPC); 
					recv(SOCK_TCPC,buff,len); 
					data_ptr = buff;
					if(data_ptr != NULL)
						BD_TG_RevPro(data_ptr);
					
					MQTT_STATE = MQTT_PKT_PINGREQ;
				/*120秒发送一次Ping响应保持长连接*/
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

	case SOCK_CLOSE_WAIT: 											    	         /*socket处于等待关闭状态*/
		close(SOCK_TCPC);
		break;
	}
}


