#ifndef __TCP_DEMO_H
#define __TCP_DEMO_H
#include "types.h"

extern uint16 W5500_tcp_server_port;
extern int MQTT_STATE;
void do_tcp_server(void);//TCP Server回环演示函数
void do_tcp_client(void);//TCP Clinet回环演示函数
#endif 
