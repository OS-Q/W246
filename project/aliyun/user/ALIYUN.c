#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "ALIYUN.h"

///****************Æ´½Óaliyun hamacsha1×Ö·û´®**********************/
void hamacsha1(char* buffer)
{
	//MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
	//char buffer[1024];
	//char mesbuf[1024];
	char *clientId = ClientID;
	char *deviceName = DeviceName;
	char *productKey = ProductKey;
	//char *secret = "05U7YhMnnSjHpg9Amz4OVlyEiWzWH4ZF";
	int timestap = 789;
	sprintf(buffer,"clientId%sdeviceName%sproductKey%stimestap%d",clientId,deviceName,productKey,timestap);
	printf("%s\r\n",buffer);
}

