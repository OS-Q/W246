#include <stdio.h> 
#include <string.h>
#include "w5500_conf.h"
#include "bsp_i2c_ee.h"
#include "bsp_spi_flash.h"
#include "utility.h"
#include "w5500.h"
#include "dhcp.h"
#include "timer.h"
#include "flash.h"

CONFIG_MSG  ConfigMsg;																/*配置结构体*/
EEPROM_MSG_STR EEPROM_MSG;															/*EEPROM存储信息结构体*/


uint16 local_port=8080;	                    		/*定义本地端口*/

/*定义远端IP信息*/
uint8  remote_ip[4]={119,23,42,226};										/*远端IP地址*/
uint16 remote_port=7777;													/*远端端口号*/

/*IP配置方法选择，请自行选择*/
uint8  ip_from = IP_FROM_FLASH;						//IP_FROM_DHCP;				

uint8  dhcp_ok=0;															/*dhcp成功获取IP*/
uint32 ms=0;																/*毫秒计数*/
uint32 dhcp_time= 0;														/*DHCP运行计数*/
vu8    ntptimer = 0;								/*NPT秒计数*/

/*MAC地址首字节必须为偶数*/

uint8 mac[6]={0x00,0x08,0xdc,0x11,0x11,0x11};   	/*如果多块W5500网络适配板在同一现场工作，请使用不同的MAC地址*/
/*定义默认IP信息*/
uint8 local_ip[4]={192,168,1,25};										/*定义W5500默认IP地址*/
uint8 subnet[4]={255,255,255,0};										/*定义W5500默认子网掩码*/
uint8 gateway[4]={192,168,1,1};											/*定义W5500默认网关*/
uint8 dns_server[4]={114,114,114,114};									/*定义W5500默认DNS*/
/**
*@brief		配置W5500的IP地址
*@param		无
*@return	无
*/
#include "stm32f10x_iwdg.h"

void dog_init(u8 prer,u16 rlr)
{
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable); //使能对IWDG->PR IWDG->RLR的写
    IWDG_SetPrescaler(prer); //设置IWDG分频系数
    IWDG_SetReload(rlr); //设置IWDG装载值
    IWDG_ReloadCounter(); //reload?//重载看门狗计数值
    IWDG_Enable(); //使能看门狗
}

void dog_feed(void)
{
    IWDG_ReloadCounter(); //reload?//重载看门狗计数值
}
/**
*@brief		配置W5500的IP地址
*@param		无
*@return	无
*/
void config_w5500(void)
{	
	/*将IP配置信息写入W5500相应寄存器*/	
	setSIPR(ConfigMsg.lip);
	setSUBR(ConfigMsg.sub);
	setGAR(ConfigMsg.gw);
	setSHAR(ConfigMsg.mac);	
	setRTR(2000);/*设置溢出时间值*/
  	setRCR(3);/*设置最大重新发送次数*/
  	
	getSHAR(mac);
	printf("W5500 MAC : %X.%X.%X.%X.%X.%X\r\n", mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);	
	getSIPR (local_ip);			
	printf("W5500 地址 : %d.%d.%d.%d\r\n", local_ip[0],local_ip[1],local_ip[2],local_ip[3]);
	getSUBR(subnet);
	printf("W5500 掩码 : %d.%d.%d.%d\r\n", subnet[0],subnet[1],subnet[2],subnet[3]);
	getGAR(gateway);
	printf("W5500 网关 : %d.%d.%d.%d\r\n", gateway[0],gateway[1],gateway[2],gateway[3]);	
}

/**
*@brief		配置W5500的MAC地址
*@param		无
*@return	无
*/
void set_mac(void)
{
	uint8 *id = (vu8 *)UID_ADDR;	
	memcpy(ConfigMsg.mac,id+6,6);
	if((ConfigMsg.mac[1]==ConfigMsg.mac[0])||(ConfigMsg.mac[2]==ConfigMsg.mac[3])||(ConfigMsg.mac[4]==ConfigMsg.mac[5]))
	{
		//uint8 *id = (uint8 *)UID_ADDR;	
		memcpy(ConfigMsg.mac,mac,6);
		//memcpy(DHCP_GET.mac, mac, 6);
		//set_default();
		//printf("ConfigMsg.mac\r\n");
	}
	setSHAR(ConfigMsg.mac);	/**/	
	getSHAR(mac);
	//printf("INIT MAC: %X.%X.%X.%X.%X.%X\r\n", mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
	//write_MAC_to_flash();
	//read_mac_from_flash(imac);
	//printf("ConfigMsg.mac: %X.%X.%X.%X.%X.%X\r\n", ConfigMsg.mac[0],ConfigMsg.mac[1],ConfigMsg.mac[2],ConfigMsg.mac[3],ConfigMsg.mac[4],ConfigMsg.mac[5]);
}

void get_dhcp_ip(void)						
{
	uint8 conf[32];	
	uint8 *id = (vu8 *)UID_ADDR;	
	/*复制DHCP获取的配置信息到配置结构体*/
	if(dhcp_ok==1)
	{
		//printf(" IP from DHCP\r\n");		 
		memcpy(ConfigMsg.lip,DHCP_GET.lip, 4);
		memcpy(ConfigMsg.sub,DHCP_GET.sub, 4);
		memcpy(ConfigMsg.gw,DHCP_GET.gw, 4);
		memcpy(ConfigMsg.dns,DHCP_GET.dns,4);			
		memcpy(ConfigMsg.mac,id+6,6);
		//memcpy(ConfigMsg.mac,(vu8 *)UID_ADDR+6,6);
		read_config_from_flash(conf);
		//printf(" FLASH DHCP : %d.%d.%d.%d\r\n", conf[0],conf[1],conf[2],conf[3]);
		//printf(" W5500 DHCP : %d.%d.%d.%d\r\n", ConfigMsg.lip[0],ConfigMsg.lip[1],ConfigMsg.lip[2],ConfigMsg.lip[3]);
		if((ConfigMsg.lip[0]!=conf[0])||(ConfigMsg.lip[1]!=conf[1])||(ConfigMsg.lip[2]!=conf[2])||(ConfigMsg.lip[3]!=conf[3]))
		{				
			write_config_to_flash();
			printf("更新存储信息\r\n");
			//read_config_from_flash(conf);
		}
	}
	else
	{
		printf(" DHCP子程序未运行,或者执行失败\r\n");
	}
}

int dhcp_init(void)								 		
{	
	uint32 flag = 0;
	printf("执行 DHCP\r\n");
	while(dhcp_ok==0)								 		
  	{	
			dog_feed();	
		do_dhcp();
		if(flag == 0)
		{		
			printf("\r\n获取DHCP地址 ");
		}
		else
		{
			if(flag%100 == 0 ) printf(".");
			delay_ms(10);
		} 
		flag++;
	}
	printf("\r\nDHCP完成\r\n");
	return 0;
}

/**
*@brief		配置W5500的IP地址
*@param		无
*@return	无
*/
void data_init(void)
{	
	uint8 conf[32];	
	//uint8 *id = (vu8 *)UID_ADDR;	
	/*使用存储的IP参数*/	
	if(ip_from==IP_FROM_FLASH)
	{
		/*从EEPROM中读取IP配置信息*/
		//read_config_from_eeprom();		
		read_config_from_flash(conf);
		/*如果读取EEPROM中MAC信息,如果已配置，则可使用*/		
		if( (conf[8] !=0xFF) && (conf[9] !=0xFF) && (conf[10] !=0xFF) && (conf[11] !=0xFF))		
		{
			//printf("IP from FLASH\r\n");
			/*复制EEPROM配置信息到配置的结构体变量*/
			memcpy(ConfigMsg.sw_ver, conf,2);
			memcpy(ConfigMsg.mac,conf+2, 6);
			memcpy(ConfigMsg.lip,conf+8, 4);				
			memcpy(ConfigMsg.sub,conf+12,4);
			memcpy(ConfigMsg.gw, conf+16,4);
			memcpy(ConfigMsg.dns, conf+20,4);
			memcpy(ConfigMsg.rip, conf+24,4);
			// for(i = 0; i < 6; i++) {
			// 	printf("%x ", ConfigMsg.mac[i]);
			// } 
			// ConfigMsg.dhcp=0;
			// ConfigMsg.debug=1;
			// ConfigMsg.fw_len=0;			
			//ConfigMsg.state=NORMAL_STATE;
			//uint8 *id = (vu8 *)UID_ADDR;	
			//memcpy(ConfigMsg.mac,(vu8 *)UID_ADDR+6,6);
		}
		else
		{
			ip_from=IP_FROM_DHCP;
			ConfigMsg.dhcp=1;
			dhcp_init();
			//memcpy(ConfigMsg.mac,(vu8 *)UID_ADDR+6,6);
			printf(" FLASH未配置,自动使用DHCP功能\r\n");
			//write_config_to_flash();
		}			
	}
	else if(ip_from==IP_FROM_EEPROM)
	{
		/*从EEPROM中读取IP配置信息*/
		read_config_from_eeprom();		
		//read_config_from_flash(conf);
		/*如果读取EEPROM中MAC信息,如果已配置，则可使用*/		
		if( (conf[0] !=0xFF) && (conf[1] !=0xFF) && (conf[2] !=0xFF) && (conf[3] !=0xFF))		
		{
			printf("IP from FLASH\r\n");
			/*复制EEPROM配置信息到配置的结构体变量*/
			memcpy(ConfigMsg.lip,conf, 4);				
			memcpy(ConfigMsg.sub,conf+4, 4);
			memcpy(ConfigMsg.gw, conf+4, 4);
			//uint8 *id = (vu8 *)UID_ADDR;	
			//memcpy(ConfigMsg.mac,(vu8 *)UID_ADDR+6,6);
		}
		else
		{
			printf(" EEPROM未配置,使用定义的IP信息配置W5500,并写入EEPROM\r\n");
			//write_config_to_eeprom();	/*使用默认的IP信息，并初始化EEPROM中数据*/
		}			
	}		
	else if(ip_from==IP_FROM_DHCP)			/*使用DHCP获取IP参数，需调用DHCP子函数*/							
	{
		dhcp_init();
		get_dhcp_ip();
	}
	else
	{
		/*复制定义的配置信息到配置结构体*/
		memcpy(ConfigMsg.mac,mac, 6);
		memcpy(ConfigMsg.lip,local_ip,4);
		memcpy(ConfigMsg.sub,subnet,4);
		memcpy(ConfigMsg.gw,gateway,4);
		memcpy(ConfigMsg.dns,dns_server,4);
		/*以下配置信息，根据需要选用*/	
		ConfigMsg.sw_ver[0]=FW_VER_HIGH;
		ConfigMsg.sw_ver[1]=FW_VER_LOW;	
		if(ip_from==IP_FROM_DEFINE)	 printf("使用定义的IP信息配置W5500\r\n");		
	}
	config_w5500();
}

/**
*@brief		配置W5500的GPIO接口
*@param		无
*@return	无
*/
void gpio_for_w5500_config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_FLASH_Init();																		 /*初始化STM32 SPI接口*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOE|RCC_APB2Periph_GPIOG, ENABLE);
		
  /*定义RESET引脚*/	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;              /*选择要控制的GPIO引脚*/		 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;      /*设置引脚速率为50MHz*/		
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       /*设置引脚模式为通用推挽输出*/		
  GPIO_Init(GPIOB, &GPIO_InitStructure);                 /*调用库函数，初始化GPIO*/
  GPIO_SetBits(GPIOB, GPIO_Pin_1);		
  /*定义INT引脚*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;              /*选择要控制的GPIO引脚*/		 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;      /*设置引脚速率为50MHz */		
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;          /*设置引脚模式为通用推挽模拟上拉输入*/	
  GPIO_Init(GPIOC, &GPIO_InitStructure);                 /*调用库函数，初始化GPIO*/
}

/**
*@brief		W5500复位设置函数
*@param		无
*@return	无
*/
void reset_w5500(void)
{
  GPIO_ResetBits(GPIOB, GPIO_Pin_1);
  delay_us(50);  
  GPIO_SetBits(GPIOB, GPIO_Pin_1);
  delay_us(50);
}

/**
*@brief		配置W5500的GPIO接口
*@param		无
*@return	无
*/
void w5500_init(void)
{
	gpio_for_w5500_config();							/*初始化MCU相关引脚*/
	reset_w5500();										/*硬复位W5500*/
}
/**
*@brief		W5500片选信号设置函数
*@param		val: 为“0”表示片选端口为低，为“1”表示片选端口为高
*@return	无
*/
void wiz_cs(uint8_t val)
{
	#ifdef  STM32_SPI1
		if (val == LOW) 
		{
			GPIO_ResetBits(GPIOA, GPIO_Pin_4); 
		}
		else if (val == HIGH)
		{
			GPIO_SetBits(GPIOA, GPIO_Pin_4); 
		}
	#endif
	#ifdef STM32_SPI2
		if (val == LOW) 
		{
			GPIO_ResetBits(GPIOB, GPIO_Pin_12); 
		}
		else if (val == HIGH)
		{
			GPIO_SetBits(GPIOB, GPIO_Pin_12); 
		}
	#endif
}

/**
*@brief		设置W5500的片选端口SCSn为低
*@param		无
*@return	无
*/
void iinchip_csoff(void)
{
	wiz_cs(LOW);
}

/**
*@brief		设置W5500的片选端口SCSn为高
*@param		无
*@return	无
*/
void iinchip_cson(void)
{	
   wiz_cs(HIGH);
}


/**
*@brief		STM32 SPI1读写8位数据
*@param		dat：写入的8位数据
*@return	无
*/
uint8  IINCHIP_SpiSendData(uint8 dat)
{
	return(SPI_FLASH_SendByte(dat));
}

/**
*@brief		写入一个8位数据到W5500
*@param		addrbsb: 写入数据的地址
*@param   data：写入的8位数据
*@return	无
*/
void IINCHIP_WRITE( uint32 addrbsb,  uint8 data)
{
	iinchip_csoff();                              		
	IINCHIP_SpiSendData( (addrbsb & 0x00FF0000)>>16);	
	IINCHIP_SpiSendData( (addrbsb & 0x0000FF00)>> 8);
	IINCHIP_SpiSendData( (addrbsb & 0x000000F8) + 4);  
	IINCHIP_SpiSendData(data);                   
	iinchip_cson();                            
}

/**
*@brief		从W5500读出一个8位数据
*@param		addrbsb: 写入数据的地址
*@param   data：从写入的地址处读取到的8位数据
*@return	无
*/
uint8 IINCHIP_READ(uint32 addrbsb)
{
	uint8 data = 0;
	iinchip_csoff();                            
	IINCHIP_SpiSendData( (addrbsb & 0x00FF0000)>>16);
	IINCHIP_SpiSendData( (addrbsb & 0x0000FF00)>> 8);
	IINCHIP_SpiSendData( (addrbsb & 0x000000F8))    ;
	data = IINCHIP_SpiSendData(0x00);            
	iinchip_cson();                               
	return data;    
}

/**
*@brief		向W5500写入len字节数据
*@param		addrbsb: 写入数据的地址
*@param   buf：写入字符串
*@param   len：字符串长度
*@return	len：返回字符串长度
*/
uint16 wiz_write_buf(uint32 addrbsb,uint8* buf,uint16 len)
{
	uint16 idx = 0;
	if(len == 0) printf(" Unexpected2 length 0\r\n");
	iinchip_csoff();                               
	IINCHIP_SpiSendData( (addrbsb & 0x00FF0000)>>16);
	IINCHIP_SpiSendData( (addrbsb & 0x0000FF00)>> 8);
	IINCHIP_SpiSendData( (addrbsb & 0x000000F8) + 4); 
	for(idx = 0; idx < len; idx++)
	{
		IINCHIP_SpiSendData(buf[idx]);
	}
	iinchip_cson();                           
	return len;  
}

/**
*@brief		从W5500读出len字节数据
*@param		addrbsb: 读取数据的地址
*@param 	buf：存放读取数据
*@param		len：字符串长度
*@return	len：返回字符串长度
*/
uint16 wiz_read_buf(uint32 addrbsb, uint8* buf,uint16 len)
{
  uint16 idx = 0;
  if(len == 0)
  {
    printf(" Unexpected2 length 0\r\n");
  }
  iinchip_csoff();                                
  IINCHIP_SpiSendData( (addrbsb & 0x00FF0000)>>16);
  IINCHIP_SpiSendData( (addrbsb & 0x0000FF00)>> 8);
  IINCHIP_SpiSendData( (addrbsb & 0x000000F8));    
  for(idx = 0; idx < len; idx++)                   
  {
			buf[idx] = IINCHIP_SpiSendData(0x00);
  }
  iinchip_cson();                                  
  return len;
}

/**
*@brief		写配置信息到EEPROM
*@param		无
*@return	无
*/
void write_config_to_eeprom(void)
{
	uint16 dAddr=0;
	ee_WriteBytes(ConfigMsg.mac,dAddr,(uint8)EEPROM_MSG_LEN);				
	delay_ms(10);																							
}

void write_config_to_flash(void)
{
	uint8 i=0,ret[32]={0};
	//uint8 crc[2]={2};
	memcpy(ret+i,ConfigMsg.sw_ver,2);
	i+=2;
	memcpy(ret+i,ConfigMsg.mac,6);
	i+=6;
	memcpy(ret+i,ConfigMsg.lip,4);
	i+=4;
	memcpy(ret+i,ConfigMsg.sub,4);
	i+=4;
	memcpy(ret+i,ConfigMsg.gw,4);
	i+=4;
	memcpy(ret+i,ConfigMsg.dns,4);
	 i+=4;
	memcpy(ret+i,ConfigMsg.rip,4);
	//ret = (vu8*)(MAC_ADDR);
	flash_Write(MAC_ADDR,(u32*)(ret),32);	
	for(i = 0; i < 32; i++) {
		printf("%x ",  *((vu8*)(MAC_ADDR)+i));
	} 
	printf("\r\n"); 																					
}

// void write_MAC_to_flash(void)
// {

// 	uint8 i,ret[8];
// 	memcpy(ret,ConfigMsg.mac,6);
// 	ret[6]=1;
// 	ret[7]++;
// 	flash_Write(MAC_ADDR,(u32*)(ret),8);
// 	// for(i=0;i<8;i++){
// 	// 	printf("%x ",  *((vu8*)(MAC_ADDR)+i));
// 	// }
// 	// printf("MAC\r\n");
// 	delay_ms(10);																							
// }

/**
*@brief		从EEPROM读配置信息
*@param		无
*@return	无
*/
void read_config_from_eeprom(void)
{
	ee_ReadBytes(EEPROM_MSG.mac,0,EEPROM_MSG_LEN);
	delay_us(10);
}

void read_config_from_flash(uint8 *buff)
{
	uint8 *ret = (vu8*)(MAC_ADDR);	
	memcpy(buff,ret,32);
	// for(i = 0; i < 32; i++){
	// 	printf("%d ", buff[i]);
	// }
	// printf("\r\n");
	//memcpy(buff,(vu8*)(LIP_ADDR),8);
	// for(i = 0; i < 12; i++) {
	// 	printf("%x ", buff[i]);
	// } 
	// printf("\r\n"); 
}


/**
*@brief		dhcp用到的定时器初始化
*@param		无
*@return	无
*/
void timer_init(void)
{
	TIM2_NVIC_Configuration();
	TIM2_Config(1000);										/* TIM2 定时配置 */
	TIM2_ON();															
}

/**
*@brief		定时器2中断函数
*@param		无
*@return	无
*/
void timer2_isr(void)
{
  	ms++;	
	if(ms>=1000)
	{
		ms=0;
		g_con_cnt++;
		#ifndef	_DNS_H_
		dhcp_time++;			/*DHCP定时加1S*/
		#endif
		#ifndef	__NTP_H__
		ntptimer++;				/*NTP重试时间加1S*/
		#endif
	}

}
/**
*@brief		STM32系统软复位函数
*@param		无
*@return	无
*/
void reboot(void)
{
  pFunction Jump_To_Application;
  uint32 JumpAddress;
  printf(" 系统重启中……\r\n");
  JumpAddress = *(vu32*) (0x00000004);
  Jump_To_Application = (pFunction) JumpAddress;
  Jump_To_Application();
}

void setKPALVTR(SOCKET i,uint8 val)
{
  IINCHIP_WRITE(Sn_KPALVTR(i),val);
}
