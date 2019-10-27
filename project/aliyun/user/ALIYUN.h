#ifndef __ALIYUN_H_
#define __ALIYUN_H_
#include "types.h"

#define ProductKey 		"TKKMt4nMF8U"
#define MQTTServer 		ProductKey".iot-as-mqtt.cn-shanghai.aliyuncs.com"

#define DeviceName 		"STM32"
#define ClientID 		"JIAN24"
#define HASH_STR	    "clientId"ClientID"deviceName"DeviceName"productKey"ProductKey"timestamp%s"
#define SignString      "fa5888273e56bef5b75062357f23e0c2d553a772"

#define USER_TOPIC 	    "/"ProductKey"/"DeviceName"/user/"
#define STATUS_TOPIC 	USER_TOPIC"status"
#define GET_TOPIC 	    USER_TOPIC"get"
#define UPDATE_TOPIC 	USER_TOPIC"update"
#define ERROR_TOPIC 	USER_TOPIC"update/error"

#define TEST_TOPIC 	"/TKKMt4nMF8U/MQTT1/mqtt"

//char topic[100]={"/a1NOItbBML2/STM32/user/status"};//设置发布订阅主题

void hamacsha1(char* buffer);

#endif
