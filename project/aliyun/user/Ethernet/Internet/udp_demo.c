/**
******************************************************************************
* @file    			utility.c                                                 
* @version 			V1.2										  		
* @date    			2017-11-01								 
* @brief 				UDP��ʾ����    			  	
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
#include "w5500_conf.h"
#include "w5500.h"
#include "socket.h"
#include "utility.h"
#include "udp_demo.h"
#include "mqtt.h"

uint16 UDP_port = 5000;

/**
*@brief		UDP���Գ���
*@param		��
*@return	��
*/
uint8 UDPbuff[2048];                                                         /*����һ��2KB�Ļ���*/
uint16 UDPlen=0;
void do_udp(void)
{                                                              
	switch(getSn_SR(SOCK_UDPS))                                                /*��ȡsocket��״̬*/
	{
		case SOCK_CLOSED:                                                        /*socket���ڹر�״̬*/
			socket(SOCK_UDPS,Sn_MR_UDP,UDP_port,0);                              /*��ʼ��socket*/
		break;		
		case SOCK_UDP:                                                           /*socket��ʼ�����*/
			if(getSn_IR(SOCK_UDPS) & Sn_IR_RECV)
			{
				setSn_IR(SOCK_UDPS, Sn_IR_RECV);                                     /*������ж�*/
			}
			if((UDPlen=getSn_RX_RSR(SOCK_UDPS))>0)                                    /*���յ�����*/
			{
				UDPbuff[UDPlen-8]=0x00; 																								 /*����ַ���������*/
				recvfrom(SOCK_UDPS,UDPbuff, UDPlen, remote_ip,&UDP_port);            /*W5500���ռ����������������*/                                                   
				printf("%s\r\n",UDPbuff);                                            /*��ӡ���ջ���*/ 
				sendto(SOCK_UDPS,UDPbuff,UDPlen-8, remote_ip, UDP_port);             /*W5500�ѽ��յ������ݷ��͸�Remote*/
			}
			break;	
	}
}

