#include <string.h>
#include <stdio.h>
#include "timer.h"
#include "utility.h"
#include "socket.h"
#include "w5500.h"
//#include "md5.h"
#include "tcp_client.h"
#include "mqtt.h"
#include "MQTTPacket.h"
#include "cJSON.h"
#include "ALIYUN.h"
#include "msg_command.h"
#include "w5500_conf.h"

extern uint8 server_ip[4];			// 配置远程服务器IP地址
uint16 server_port=1883;			// 配置远程服务器端口
//uint16 local_port=6000;			// 初始化一个本地端口
uint16 len=0;
uint32 ping_delay_s=5;

uint8  msgbuf[500];
extern uint8 buffer[2048];
uint8 g_con_flag=1;
uint8 g_sub_flag = 1;
uint8 g_pub_flag = 1;
char ser_cmd[240];

uint32 g_con_cnt;


unsigned char dup;
int qos;
unsigned char retained;
unsigned short mssageid;
int payloadlen_in;
unsigned char* payload_in;
MQTTString receivedTopic;
char topic[100]={"/a1NOItbBML2/STM32/user/status"};//设置发布订阅主题
//char topic[100]={"/TKKMt4nMF8U/MQTT1/mqtt"};//设置发布订阅主题
char new_topic[100];

char rebuf[1024];

void connect_ali_login(void)
{
	char buffer[300];				
	make_con_msg("123456|securemode=3,signmethod=hmacsha1,timestamp=789|",180, 1,
	"STM32&a1NOItbBML2","fa5888273e56bef5b75062357f23e0c2d553a772",msgbuf,sizeof(msgbuf));
	// make_con_msg("192.168.207.115|securemode=3,signmethod=hmacsha1,timestamp=789|",180, 1,
	// "MQTT1&TKKMt4nMF8U","9076b0ebc04dba8a8ebba1f0003552dbc862c9b9",msgbuf,sizeof(msgbuf));
	//printf("test_HASH_STR : %s\r\n",HASH_STR);
	printf("STATUS_TOPIC : %s\r\n",STATUS_TOPIC);
	hamacsha1(buffer);//hamacsha1字符串连接
	//passwor = hamacsha1("secret","clientId192.168.207.115deviceNameMQTT1productKeyTKKMt4nMF8Utimestap789");
	//printf("server_ip: %d.%d.%d.%d\r\n", server_ip[0],server_ip[1],server_ip[2],server_ip[3]);
	//printf("connect ALY\r\n");
	send(0,msgbuf,sizeof(msgbuf));
	delay_s(2);	
}

/**
*@brief		DHCP定时初始化
*@param		无
*@return	无
*/

void mqtt_Sub_handle(void)
{
	memset(msgbuf,0,sizeof(msgbuf));					 
	make_sub_msg(STATUS_TOPIC,msgbuf,sizeof(msgbuf));
	send(0,msgbuf,sizeof(msgbuf));	// 接收到数据后再回给服务器，完成数据回环
	delay_s(2);
}
/**
*@brief		DHCP定时初始化
*@param		无
*@return	无
*/

void mqtt_pub_handle(char *pub_topic,char*msg)
{
	if(strlen(msg) > 0 && strlen(msg) < sizeof(msgbuf))
	{
		printf("pub_topic: %s\r\n",pub_topic);
		printf("pub_message : %s\r\n",msg);
		memset(msgbuf,0,sizeof(msgbuf));
		make_pub_msg(pub_topic,msgbuf,sizeof(msgbuf),msg);
		send(0,msgbuf,sizeof(msgbuf));	
	}
}

/**
*@brief		ping连接测试人物
*@param		无
*@return	执行结果
*/

int ping_task(void)
{				
	make_ping_msg(msgbuf,sizeof(msgbuf));
	send(0,msgbuf,sizeof(msgbuf));	
	while((len=getSn_RX_RSR(0))==0);
	recv(0,msgbuf,len);
	if(len>2)
	{
		printf("ping: message %s\r\n",msgbuf);		
	}
	else if(len==2) printf("ping ok\r\n");	
	else return 1;
	return 0;
}

void ping1_task(void)
{					
	make_ping_msg(msgbuf,sizeof(msgbuf));
	send(0,msgbuf,sizeof(msgbuf));	
	while((len=getSn_RX_RSR(0))==0);
	recv(0,msgbuf,len);
	printf("rev ping len: %d\r\n",len);
	if(len>2)
	{
		if(PUBLISH == mqtt_decode_msg(msgbuf+2))
		{
			MQTTDeserialize_publish(&dup, &qos, &retained, &mssageid, &receivedTopic,&payload_in, &payloadlen_in, msgbuf+2, len-2);							
			if(payloadlen_in> 0)
			{
				printf("topic:%s\r\n",topic);
				printf("ping message arrived %d : %s\r\n", payloadlen_in, payload_in);
				memset(topic,0,sizeof(topic));
				memset(ser_cmd,0,sizeof(ser_cmd));
				memcpy(topic,receivedTopic.lenstring.data,receivedTopic.lenstring.len);
				replace_string(new_topic,topic,"request","response");
				printf("topic:%s\r\n",topic);
				strcpy(ser_cmd,(const char *)payload_in);
				//parse_topic(ser_cmd);
				printf("cmd message : %s\r\n",ser_cmd);
				memset(msgbuf,0,sizeof(msgbuf));
				make_pub_msg(new_topic,msgbuf,sizeof(msgbuf),"ping qitas");
				//send(0,msgbuf,sizeof(msgbuf));	
			}
		}
		else {
			printf("ping get message : %s\r\n",msgbuf);
		}
	}
	
}


void mqtt_con_init(void)
{	
	//int len;
	//int type;					
	if(g_con_flag == 1 )
	{		
		printf("****** connect aliyun ******\r\n");						
		connect_ali_login();
		while((len=getSn_RX_RSR(0))==0)
		{
			send(0,msgbuf,sizeof(msgbuf));
			delay_s(2);	
		};
		recv(0,msgbuf,len);
		while(mqtt_decode_msg(msgbuf)!=CONNACK)	//判断是不是CONNACK
		{
			printf("wait ack\r\n");
			delay_s(2);
		}
		g_con_flag=0;
		printf("connect ack done\r\n");
	}
	if(g_sub_flag == 1)
	{	
		printf("****** sub topic ******\r\n");	
		mqtt_Sub_handle();
		g_sub_flag = 0;		
		while((len=getSn_RX_RSR(0))==0)  //重发机制
		{
			send(0,msgbuf,sizeof(msgbuf));
			delay_s(2);	
		};
		recv(0,msgbuf,len);
		while(mqtt_decode_msg(msgbuf)!=SUBACK) //判断是不是SUBACK
		{
			printf("wait suback\r\n");
			delay_ms(100);	
		}
		g_con_cnt = 0;
		printf("mqtt sub done\r\n");						
	}
	if(g_pub_flag == 1)
	{
		printf("****** pub topic ******\r\n");	
		memset(msgbuf,0,sizeof(msgbuf));					 
		//make_sub_msg(topic,msgbuf,sizeof(msgbuf));
		make_pub_msg(topic,msgbuf,sizeof(msgbuf),"hello qitas pub init");
		send(0,msgbuf,sizeof(msgbuf));	
		delay_s(2);
		g_pub_flag = 0;
		printf("mqtt topic pub init done\r\n");	
		printf("g_con_cnt: %d\r\n",g_con_cnt);
	}				 			 				
}
/**
*@brief		DHCP定时初始化
*@param		无
*@return	无
*/

void mqtt_con1_init(void)
{		
	uint16 len=0;	
	memset(msgbuf,0,sizeof(msgbuf));				
	if(g_con_flag == 1 )
	{		
		printf("****** connect aliyun ******\r\n");		
		do{
			connect_ali_login();
			delay_s(2);	
		} while((len=getSn_RX_RSR(0))==0);							
		recv(0,msgbuf,len);
		while(mqtt_decode_msg(msgbuf)!=CONNACK)	//判断是不是CONNACK
		{
			printf("wait ack\r\n");
			delay_s(2);
		}
		//g_con_cnt = 0;
		g_con_flag = 0;
		printf("connect ack %d :%s\r\n",len,msgbuf);
	}
	if(g_sub_flag == 1)
	{	
		printf("****** sub topic ******\r\n");					 	
		do{
			make_sub_msg(topic,msgbuf,sizeof(msgbuf));	
			send(0,msgbuf,sizeof(msgbuf));		// 接收到数据后再回给服务器，完成数据回环
			delay_s(2);
		} while((len=getSn_RX_RSR(0))==0);
		recv(0,msgbuf,len);
		while(mqtt_decode_msg(msgbuf)!=SUBACK) //判断是不是SUBACK
		{
			printf("wait suback\r\n");
			delay_ms(100);	
		}
		//g_con_cnt = 0;
		g_sub_flag = 0;
		printf("mqtt sub done:%s\r\n",msgbuf);						
	}
	if(g_pub_flag == 1)
	{
		printf("****** pub topic ******\r\n");					 
		make_pub_msg(topic,msgbuf,sizeof(msgbuf),"qitas pub init");
		send(0,msgbuf,sizeof(msgbuf));	
		delay_s(2);
		g_pub_flag = 0;
		g_con_cnt=0;
		printf("topic pub:%s\r\n",msgbuf);
	}					 			 				
}

/**
*@brief		DHCP定时初始化
*@param		无
*@return	无
*/

void mqtt_decode_handle(void)
{
	//uint16 len=0;
	recv(0,msgbuf,len);
	switch(mqtt_decode_msg(msgbuf))					
	{
		case CONNECT:						
			printf("recv msg 1: CONNECT \r\n");
			//g_con_flag = 0;
			break;
		case CONNACK:			                    
			printf("recv msg 2: CONNACK \r\n");
			g_con_flag = 0;
			break;
		case PUBLISH:	
			printf("recv msg: PUBLISH \r\n");		                    
			MQTTDeserialize_publish(&dup, &qos, &retained, &mssageid, &receivedTopic,&payload_in, &payloadlen_in, msgbuf, len);
			if(payloadlen_in > 0)
			{
				printf("topic:%s\r\n",topic);
				printf("publish [%d]: %s\n\r", payloadlen_in, payload_in);
				
				memset(topic,0,sizeof(topic));
				memcpy(topic,receivedTopic.lenstring.data,receivedTopic.lenstring.len);
				replace_string(new_topic,topic,"request","response");	
				//memset(ser_cmd,0,sizeof(ser_cmd));
				//memcpy(ser_cmd,(const char *)payload_in,strlen((char*)payload_in));						
				//parse_topic(ser_cmd);	
				//mqtt_pub_handle(topic,"qitas test");
			}				
			break;
		case PUBACK:						
			printf("recv msg 4: PUBACK \r\n");
			break;
		case PUBREC:			                    
			printf("recv msg 5: PUBREC \r\n");
			break;
		case PUBREL:						

			break;
		case PUBCOMP:			                    
				
			break;
		case SUBSCRIBE:						

			break;
		case SUBACK:
			printf("recv msg: SUBACK \r\n");			                    
			g_sub_flag = 0;
			break;
		case UNSUBSCRIBE:						

			break;
		case PINGRESP:						
			printf("msg: PINGRESP \r\n");
			break;
		case PINGREQ:			                    
			printf("msg: PINGREQ \r\n");
			break;
		case DISCONNECT:						
			printf("msg: DISCONNECT \r\n");
			break;
	}
}
/**
*@brief		DHCP定时初始化
*@param		无
*@return	无
*/

int sock_conn_status(void)
{
	switch(getSn_SR(0))								// 获取socket0的状态
	{
		case SOCK_INIT:			                    // Socket处于初始化完成(打开)状态
			connect(0, server_ip,server_port);		// 配置Sn_CR为CONNECT，并向TCP服务器发出连接请求				
			break;
		case SOCK_CLOSE_WAIT:						// Socket处于等待关闭状态
			close(0);								// 关闭Socket0
			break;
		case SOCK_CLOSED:							// Socket处于关闭状态
			socket(0,Sn_MR_TCP,local_port,Sn_MR_ND);// 打开Socket0，并配置为TCP无延时模式，打开一个本地端口
			break;
		case SOCK_ESTABLISHED:						// Socket处于连接建立状态
			if(getSn_IR(0) & Sn_IR_CON)   					
			{
				setSn_IR(0, Sn_IR_CON);				// Sn_IR的CON位置1，通知W5500连接已建立
			}
			return 0;	
			//break;
		default :
			break;
	}
	return 1;		
}

void connect_ali(void)
{
	if(sock_conn_status()==0)
	{	
		memset(msgbuf,0,sizeof(msgbuf));	
		len=getSn_RX_RSR(0);
		if(len == 0)
		{					
			mqtt_con_init(); 
		}	
		else if(len > 0)
		{	
			//printf("g_con_cnt: %d \r\n", g_con_cnt);
			mqtt_decode_handle();
			delay_s(1);	
		}

		if(g_con_cnt>ping_delay_s) //心跳机制，配置时间s
		{
			g_con_cnt=0;
			dog_feed();			
			ping_task();					
		}
	}
}


/********字符串替换********/

void replace_string(char *result, char *source, char* s1, char *s2)
{
	char *q=NULL;
	char *p=NULL;
	p=source;
	while((q=strstr(p, s1))!=NULL)
	{
		strncpy(result, p, q-p);
		result[q-p]= '\0';	//very important, must attention!
		strcat(result, s2);
		strcat(result, q+strlen(s1));
		strcpy(p,result);
	}
	strcpy(result, p);
}
