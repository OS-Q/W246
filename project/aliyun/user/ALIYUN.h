#ifndef __ALIYUN_H_
#define __ALIYUN_H_
#include "types.h"

#define ProductKey 		"a1NOItbBML2"
#define MQTTServer 		ProductKey".iot-as-mqtt.cn-shanghai.aliyuncs.com"

#define DeviceName 		"STM32"
#define ClientID 		"JIAN24"
#define HASH_STR	    "clientId"ClientID"deviceName"DeviceName"productKey"ProductKey"timestamp%s"
#define SignString      "fa5888273e56bef5b75062357f23e0c2d553a772"
#define STATUS_TOPIC 	"/"ProductKey"/"DeviceName"/user/status"

//char topic[100]={"/a1NOItbBML2/STM32/user/status"};//设置发布订阅主题

void hamacsha1(char* buffer);

#endif
