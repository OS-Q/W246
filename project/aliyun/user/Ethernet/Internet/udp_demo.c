/**
******************************************************************************
* @file    			utility.c                                                 
* @version 			V1.2										  		
* @date    			2017-11-01								 
* @brief 				UDP演示函数    			  	
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
#include "w5500_conf.h"
#include "w5500.h"
#include "socket.h"
#include "utility.h"
#include "udp_demo.h"
#include "mqtt.h"

uint16 UDP_port = 5000;

/**
*@brief		UDP测试程序
*@param		无
*@return	无
*/
uint8 UDPbuff[2048];                                                         /*定义一个2KB的缓存*/
uint16 UDPlen=0;
void do_udp(void)
{                                                              
	switch(getSn_SR(SOCK_UDPS))                                                /*获取socket的状态*/
	{
		case SOCK_CLOSED:                                                        /*socket处于关闭状态*/
			socket(SOCK_UDPS,Sn_MR_UDP,UDP_port,0);                              /*初始化socket*/
		break;		
		case SOCK_UDP:                                                           /*socket初始化完成*/
			if(getSn_IR(SOCK_UDPS) & Sn_IR_RECV)
			{
				setSn_IR(SOCK_UDPS, Sn_IR_RECV);                                     /*清接收中断*/
			}
			if((UDPlen=getSn_RX_RSR(SOCK_UDPS))>0)                                    /*接收到数据*/
			{
				UDPbuff[UDPlen-8]=0x00; 																								 /*添加字符串结束符*/
				recvfrom(SOCK_UDPS,UDPbuff, UDPlen, remote_ip,&UDP_port);            /*W5500接收计算机发送来的数据*/                                                   
				printf("%s\r\n",UDPbuff);                                            /*打印接收缓存*/ 
				sendto(SOCK_UDPS,UDPbuff,UDPlen-8, remote_ip, UDP_port);             /*W5500把接收到的数据发送给Remote*/
			}
			break;	
	}
}

