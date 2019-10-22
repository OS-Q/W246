#ifndef _BD_TG_H_
#define _BD_TG_H_
#include "types.h"




void  BD_TG_DevLink(void);

void BD_TG_Subscribe(const char *topics[], unsigned char topic_cnt);

void BD_TG_Publish(const char *topic, const char *msg);

void BD_TG_RevPro(unsigned char *cmd);
void MQTT_UnSubscribe(const char *topics[], uint8 topic_cnt);

extern unsigned char publish_buf[64];
#endif
