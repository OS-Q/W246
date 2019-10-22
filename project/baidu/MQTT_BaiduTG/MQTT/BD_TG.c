#include "stm32f10x.h"

#include "BD_TG.h"
#include "mqttkit.h"
#include "socket.h"
#include "W5500_conf.h"
#include "w5500.h"
#include "tcp_demo.h"

#include "utility.h"

#include <string.h>
#include <stdio.h>

#define PROID		"mqtt_test/w5500"																	// 设备全称

#define AUTH_INFO	"IHJwyNJidsF3ku1v7A3b3a7ApQUlXgyYURUBvJ+ahVA="	// API密钥

#define DEVID		"1"																								// 身份

unsigned char w5500_buf[128];
unsigned char publish_buf[64]; 

//==========================================================
//	函数名称：	BD_TG_DevLink

//	函数功能：	与百度天工创建连接

//	入口参数：	无

//	返回参数：	1-成功	0-失败

//	说明：		与百度天工平台建立连接
//==========================================================


void BD_TG_DevLink(void)
{
	
	int len;
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};					//协议包

	unsigned char *dataPtr;
	

	printf( "百度天工_DevLink\r\nPROID: %s,	AUIF: %s,	DEVID:%s\r\n",PROID, AUTH_INFO, DEVID);
	
	if(MQTT_PacketConnect(PROID, AUTH_INFO, DEVID, 256, 0, MQTT_QOS_LEVEL0, NULL, NULL, 0, &mqttPacket) == 0)
	{
			
		
		send(SOCK_TCPC,mqttPacket._data,mqttPacket._len);
		
		delay_ms(100);  //等待平台响应

		while( w5500_buf[0] != 0x20)
		{	
			len=getSn_RX_RSR(SOCK_TCPC);		
			recv(SOCK_TCPC,w5500_buf,len); 
		}
		
		dataPtr = w5500_buf;	
		if(dataPtr != NULL)
		{
			if(MQTT_UnPacketRecv(dataPtr) == MQTT_PKT_CONNACK)
			{
				switch(MQTT_UnPacketConnectAck(dataPtr))
				{
					case 0:printf( "Tips:	连接成功\r\n");break;
					
					case 1:printf("WARN:	连接失败：协议错误\r\n");break;
					case 2:printf("WARN:	连接失败：非法的clientid\r\n");break;
					case 3:printf("WARN:	连接失败：服务器失败\r\n");break;
					case 4:printf("WARN:	连接失败：用户名或密码错误\r\n");break;
					case 5:printf("WARN:	连接失败：非法链接(比如token非法)\r\n");break;		
					default:printf("ERR:	连接失败：未知错误\r\n");break;
				}
			}
		}
		
		MQTT_DeleteBuffer(&mqttPacket);								//删包
	}
	else
	printf( "WARN:	MQTT_PacketConnect Failed\r\n");
	

}


void MQTT_UnSubscribe(const char *topics[], uint8 topics_cnt)
{

	int len;
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};					//协议包

	if(MQTT_PacketUnSubscribe(10, topics,topics_cnt, &mqttPacket) == 0)
	{
	   send(SOCK_TCPC,mqttPacket._data,mqttPacket._len);
	   delay_ms(100);  //等待平台响应
		 len=getSn_RX_RSR(SOCK_TCPC);
     recv(SOCK_TCPC,w5500_buf,len);
		 if ((w5500_buf[0] >> 4) == MQTT_PKT_UNSUBACK)
		    printf("成功取消订阅\r\n");					
	}	

	

}
//==========================================================
//	函数名称：	BD_TG_Subscribe

//	函数功能：	订阅

//	入口参数：	topics：订阅的topic
//				topic_cnt：topic个数

//	返回参数：	SEND_TYPE_OK-成功	SEND_TYPE_SUBSCRIBE-需要重发

//	说明：		
//==========================================================
void BD_TG_Subscribe(const char *topics[], unsigned char topic_cnt)
{
	
	unsigned char i = 0;
	uint16  len;
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};							//协议包
	
	for(; i < topic_cnt; i++)
		printf( "Subscribe Topic: %s\r\n", topics[i]);
	
	if(MQTT_PacketSubscribe(MQTT_SUBSCRIBE_ID, MQTT_QOS_LEVEL0, topics, topic_cnt, &mqttPacket) == 0)
	{
		send(SOCK_TCPC,mqttPacket._data, mqttPacket._len);					//向平台发送订阅请求
		
		MQTT_DeleteBuffer(&mqttPacket);											//删包
	}
	
	delay_ms(100);  //等待平台响应
	len=getSn_RX_RSR(SOCK_TCPC);
	recv(SOCK_TCPC,w5500_buf,len); 
	switch(w5500_buf[4])
	{
			case 0x00:
				printf("订阅确认， QoS等级0\r\n");
			  break;
			case 0x01:
				printf("订阅确认， QoS等级1\r\n");
			  break;
		  case 0x02:
				printf("订阅确认， QoS等级2 \r\n");
			  break;
			case 0x80:
				printf("订阅失败 \r\n");
			  break;
		}
	 

}

//==========================================================
//	函数名称：	BD_TG_Publish
//
//	函数功能：	发布消息
//
//	入口参数：	topic：发布的主题
//				msg：消息内容
//
//	返回参数：	SEND_TYPE_OK-成功	SEND_TYPE_PUBLISH-需要重送
//
//	说明：		
//==========================================================
void BD_TG_Publish(const char *topic, const char *msg)
{

	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};							//协议包
	
	printf("Publish Topic: %s, Msg: %s\r\n", topic, msg);
	
	if(MQTT_PacketPublish(MQTT_PUBLISH_ID, topic, msg, strlen(msg), MQTT_QOS_LEVEL0, 0, 1, &mqttPacket) == 0)
	{
		send(SOCK_TCPC,mqttPacket._data, mqttPacket._len);					//向平台发送订阅请求
		
		MQTT_DeleteBuffer(&mqttPacket);											//删包
	}

}

//==========================================================
//	函数名称：	BD_TG_RevPro
//
//	函数功能：	平台返回数据检测
//
//	入口参数：	dataPtr：平台返回的数据
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void BD_TG_RevPro(unsigned char *cmd)
{
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};								//协议包
	
	char *req_payload = NULL;
	char *cmdid_topic = NULL;
	unsigned char type = 0;
	unsigned char qos = 0;
	static unsigned short pkt_id = 0;
	
	short result = 0;

	char *dataPtr = NULL;
	char numBuf[10];
	int num = 0;
	
	type = MQTT_UnPacketRecv(cmd);
	switch(type)
	{
		case MQTT_PKT_CMD:															//命令下发
			
			result = MQTT_UnPacketCmd(cmd, &cmdid_topic, &req_payload);				//解出topic和消息体
			if(result == 0)
			{
				printf("cmdid: %s, req: %s\r\n", cmdid_topic, req_payload);
				
				if(MQTT_PacketCmdResp(cmdid_topic, req_payload, &mqttPacket) == 0)	//命令回复组包
				{
				 printf( "Tips:	Send CmdResp\r\n");
					
					send(SOCK_TCPC,mqttPacket._data, mqttPacket._len);			//回复命令
					MQTT_DeleteBuffer(&mqttPacket);									//删包
				}
			}
		
		break;
			
		case MQTT_PKT_PUBLISH:														//接收的Publish消息
		
			result = MQTT_UnPacketPublish(cmd, &cmdid_topic, &req_payload, &qos, &pkt_id);
			if(result == 0)
			{
				printf("topic: %s\r\npayload: %s\r\n", cmdid_topic, req_payload);
				memcpy(publish_buf,req_payload,strlen(req_payload));
				switch(qos)
				{
					case 1:													//收到publish的qos为1，设备需要回复Ack
					
						if(MQTT_PacketPublishAck(pkt_id, &mqttPacket) == 0)
						{
							printf( "Tips:	Send PublishAck\r\n");
							send(SOCK_TCPC,mqttPacket._data, mqttPacket._len);
							MQTT_DeleteBuffer(&mqttPacket);
						}
					
					break;
					
					case 2:													//收到publish的qos为2，设备先回复Rec
																					//平台回复Rel，设备再回复Comp
						if(MQTT_PacketPublishRec(pkt_id, &mqttPacket) == 0)
						{
							printf("Tips:	Send PublishRec\r\n");
							send(SOCK_TCPC,mqttPacket._data, mqttPacket._len);
							MQTT_DeleteBuffer(&mqttPacket);
						}
					
					break;
					
					default:
						break;
				}
			}
		
		break;
			
		case MQTT_PKT_PUBACK:														//发送Publish消息，平台回复的Ack
		
			if(MQTT_UnPacketPublishAck(cmd) == 0)
				printf("Tips:	MQTT Publish Send OK\r\n");
			
		break;
			
		case MQTT_PKT_PUBREC:														//发送Publish消息，平台回复的Rec，设备需回复Rel消息
		
			if(MQTT_UnPacketPublishRec(cmd) == 0)
			{
				printf("Tips:	Rev PublishRec\r\n");
				if(MQTT_PacketPublishRel(MQTT_PUBLISH_ID, &mqttPacket) == 0)
				{
					 printf ("Tips:	Send PublishRel\r\n");
					send(SOCK_TCPC,mqttPacket._data, mqttPacket._len);
					MQTT_DeleteBuffer(&mqttPacket);
				}
			}
		
		break;
			
		case MQTT_PKT_PUBREL:														//收到Publish消息，设备回复Rec后，平台回复的Rel，设备需再回复Comp
			
			if(MQTT_UnPacketPublishRel(cmd, pkt_id) == 0)
			{
				printf("Tips:	Rev PublishRel\r\n");
				if(MQTT_PacketPublishComp(MQTT_PUBLISH_ID, &mqttPacket) == 0)
				{
					printf( "Tips:	Send PublishComp\r\n");
					send(SOCK_TCPC,mqttPacket._data, mqttPacket._len);
					MQTT_DeleteBuffer(&mqttPacket);
				}
			}
		
		break;
		
		case MQTT_PKT_PUBCOMP:														//发送Publish消息，平台返回Rec，设备回复Rel，平台再返回的Comp
		
			if(MQTT_UnPacketPublishComp(cmd) == 0)
			{
			 printf("Tips:	Rev PublishComp\r\n");
			}
		
		break;
			
		case MQTT_PKT_SUBACK:														//发送Subscribe消息的Ack
		
			if(MQTT_UnPacketSubscribe(cmd) == 0)
				printf("Tips:	MQTT Subscribe OK\r\n");
			else
				printf("Tips:	MQTT Subscribe Err\r\n");
		
		break;
			
		case MQTT_PKT_UNSUBACK:														//发送UnSubscribe消息的Ack
		
			if(MQTT_UnPacketUnSubscribe(cmd) == 0)
				printf( "Tips:	MQTT UnSubscribe OK\r\n");
			else
				printf("Tips:	MQTT UnSubscribe Err\r\n");
		
		break;
		
		default:
			result = -1;
		break;
	}
	
	//ESP8266_Clear();									//清空缓存
	
	if(result == -1)
		return;
	
	dataPtr = strchr(req_payload, '}');					//搜索'}'

	if(dataPtr != NULL && result != -1)					//如果找到了
	{
		dataPtr++;
		
		while(*dataPtr >= '0' && *dataPtr <= '9')		//判断是否是下发的命令控制数据
		{
			numBuf[num++] = *dataPtr++;
		}
		
		num = atoi((const char *)numBuf);				//转为数值形式
		
	}

	if(type == MQTT_PKT_CMD || type == MQTT_PKT_PUBLISH)
	{
		MQTT_FreeBuffer(cmdid_topic);
		MQTT_FreeBuffer(req_payload);
	}
}
