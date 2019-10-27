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

CONFIG_MSG  ConfigMsg;																/*���ýṹ��*/
EEPROM_MSG_STR EEPROM_MSG;															/*EEPROM�洢��Ϣ�ṹ��*/


uint16 local_port=8080;	                    		/*���屾�ض˿�*/

/*����Զ��IP��Ϣ*/
uint8  remote_ip[4]={119,23,42,226};										/*Զ��IP��ַ*/
uint16 remote_port=7777;													/*Զ�˶˿ں�*/

/*IP���÷���ѡ��������ѡ��*/
uint8  ip_from = IP_FROM_FLASH;						//IP_FROM_DHCP;				

uint8  dhcp_ok=0;															/*dhcp�ɹ���ȡIP*/
uint32 ms=0;																/*�������*/
uint32 dhcp_time= 0;														/*DHCP���м���*/
vu8    ntptimer = 0;								/*NPT�����*/

/*MAC��ַ���ֽڱ���Ϊż��*/

uint8 mac[6]={0x00,0x08,0xdc,0x11,0x11,0x11};   	/*������W5500�����������ͬһ�ֳ���������ʹ�ò�ͬ��MAC��ַ*/
/*����Ĭ��IP��Ϣ*/
uint8 local_ip[4]={192,168,1,25};										/*����W5500Ĭ��IP��ַ*/
uint8 subnet[4]={255,255,255,0};										/*����W5500Ĭ����������*/
uint8 gateway[4]={192,168,1,1};											/*����W5500Ĭ������*/
uint8 dns_server[4]={114,114,114,114};									/*����W5500Ĭ��DNS*/
/**
*@brief		����W5500��IP��ַ
*@param		��
*@return	��
*/
#include "stm32f10x_iwdg.h"

void dog_init(u8 prer,u16 rlr)
{
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable); //ʹ�ܶ�IWDG->PR IWDG->RLR��д
    IWDG_SetPrescaler(prer); //����IWDG��Ƶϵ��
    IWDG_SetReload(rlr); //����IWDGװ��ֵ
    IWDG_ReloadCounter(); //reload?//���ؿ��Ź�����ֵ
    IWDG_Enable(); //ʹ�ܿ��Ź�
}

void dog_feed(void)
{
    IWDG_ReloadCounter(); //reload?//���ؿ��Ź�����ֵ
}
/**
*@brief		����W5500��IP��ַ
*@param		��
*@return	��
*/
void config_w5500(void)
{	
	/*��IP������Ϣд��W5500��Ӧ�Ĵ���*/	
	setSIPR(ConfigMsg.lip);
	setSUBR(ConfigMsg.sub);
	setGAR(ConfigMsg.gw);
	setSHAR(ConfigMsg.mac);	
	setRTR(2000);/*�������ʱ��ֵ*/
  	setRCR(3);/*����������·��ʹ���*/
  	
	getSHAR(mac);
	printf("W5500 MAC : %X.%X.%X.%X.%X.%X\r\n", mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);	
	getSIPR (local_ip);			
	printf("W5500 ��ַ : %d.%d.%d.%d\r\n", local_ip[0],local_ip[1],local_ip[2],local_ip[3]);
	getSUBR(subnet);
	printf("W5500 ���� : %d.%d.%d.%d\r\n", subnet[0],subnet[1],subnet[2],subnet[3]);
	getGAR(gateway);
	printf("W5500 ���� : %d.%d.%d.%d\r\n", gateway[0],gateway[1],gateway[2],gateway[3]);	
}

/**
*@brief		����W5500��MAC��ַ
*@param		��
*@return	��
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
	/*����DHCP��ȡ��������Ϣ�����ýṹ��*/
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
			printf("���´洢��Ϣ\r\n");
			//read_config_from_flash(conf);
		}
	}
	else
	{
		printf(" DHCP�ӳ���δ����,����ִ��ʧ��\r\n");
	}
}

int dhcp_init(void)								 		
{	
	uint32 flag = 0;
	printf("ִ�� DHCP\r\n");
	while(dhcp_ok==0)								 		
  	{	
			dog_feed();	
		do_dhcp();
		if(flag == 0)
		{		
			printf("\r\n��ȡDHCP��ַ ");
		}
		else
		{
			if(flag%100 == 0 ) printf(".");
			delay_ms(10);
		} 
		flag++;
	}
	printf("\r\nDHCP���\r\n");
	return 0;
}

/**
*@brief		����W5500��IP��ַ
*@param		��
*@return	��
*/
void data_init(void)
{	
	uint8 conf[32];	
	//uint8 *id = (vu8 *)UID_ADDR;	
	/*ʹ�ô洢��IP����*/	
	if(ip_from==IP_FROM_FLASH)
	{
		/*��EEPROM�ж�ȡIP������Ϣ*/
		//read_config_from_eeprom();		
		read_config_from_flash(conf);
		/*�����ȡEEPROM��MAC��Ϣ,��������ã����ʹ��*/		
		if( (conf[8] !=0xFF) && (conf[9] !=0xFF) && (conf[10] !=0xFF) && (conf[11] !=0xFF))		
		{
			//printf("IP from FLASH\r\n");
			/*����EEPROM������Ϣ�����õĽṹ�����*/
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
			printf(" FLASHδ����,�Զ�ʹ��DHCP����\r\n");
			//write_config_to_flash();
		}			
	}
	else if(ip_from==IP_FROM_EEPROM)
	{
		/*��EEPROM�ж�ȡIP������Ϣ*/
		read_config_from_eeprom();		
		//read_config_from_flash(conf);
		/*�����ȡEEPROM��MAC��Ϣ,��������ã����ʹ��*/		
		if( (conf[0] !=0xFF) && (conf[1] !=0xFF) && (conf[2] !=0xFF) && (conf[3] !=0xFF))		
		{
			printf("IP from FLASH\r\n");
			/*����EEPROM������Ϣ�����õĽṹ�����*/
			memcpy(ConfigMsg.lip,conf, 4);				
			memcpy(ConfigMsg.sub,conf+4, 4);
			memcpy(ConfigMsg.gw, conf+4, 4);
			//uint8 *id = (vu8 *)UID_ADDR;	
			//memcpy(ConfigMsg.mac,(vu8 *)UID_ADDR+6,6);
		}
		else
		{
			printf(" EEPROMδ����,ʹ�ö����IP��Ϣ����W5500,��д��EEPROM\r\n");
			//write_config_to_eeprom();	/*ʹ��Ĭ�ϵ�IP��Ϣ������ʼ��EEPROM������*/
		}			
	}		
	else if(ip_from==IP_FROM_DHCP)			/*ʹ��DHCP��ȡIP�����������DHCP�Ӻ���*/							
	{
		dhcp_init();
		get_dhcp_ip();
	}
	else
	{
		/*���ƶ����������Ϣ�����ýṹ��*/
		memcpy(ConfigMsg.mac,mac, 6);
		memcpy(ConfigMsg.lip,local_ip,4);
		memcpy(ConfigMsg.sub,subnet,4);
		memcpy(ConfigMsg.gw,gateway,4);
		memcpy(ConfigMsg.dns,dns_server,4);
		/*����������Ϣ��������Ҫѡ��*/	
		ConfigMsg.sw_ver[0]=FW_VER_HIGH;
		ConfigMsg.sw_ver[1]=FW_VER_LOW;	
		if(ip_from==IP_FROM_DEFINE)	 printf("ʹ�ö����IP��Ϣ����W5500\r\n");		
	}
	config_w5500();
}

/**
*@brief		����W5500��GPIO�ӿ�
*@param		��
*@return	��
*/
void gpio_for_w5500_config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_FLASH_Init();																		 /*��ʼ��STM32 SPI�ӿ�*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOE|RCC_APB2Periph_GPIOG, ENABLE);
		
  /*����RESET����*/	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;              /*ѡ��Ҫ���Ƶ�GPIO����*/		 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;      /*������������Ϊ50MHz*/		
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       /*��������ģʽΪͨ���������*/		
  GPIO_Init(GPIOB, &GPIO_InitStructure);                 /*���ÿ⺯������ʼ��GPIO*/
  GPIO_SetBits(GPIOB, GPIO_Pin_1);		
  /*����INT����*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;              /*ѡ��Ҫ���Ƶ�GPIO����*/		 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;      /*������������Ϊ50MHz */		
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;          /*��������ģʽΪͨ������ģ����������*/	
  GPIO_Init(GPIOC, &GPIO_InitStructure);                 /*���ÿ⺯������ʼ��GPIO*/
}

/**
*@brief		W5500��λ���ú���
*@param		��
*@return	��
*/
void reset_w5500(void)
{
  GPIO_ResetBits(GPIOB, GPIO_Pin_1);
  delay_us(50);  
  GPIO_SetBits(GPIOB, GPIO_Pin_1);
  delay_us(50);
}

/**
*@brief		����W5500��GPIO�ӿ�
*@param		��
*@return	��
*/
void w5500_init(void)
{
	gpio_for_w5500_config();							/*��ʼ��MCU�������*/
	reset_w5500();										/*Ӳ��λW5500*/
}
/**
*@brief		W5500Ƭѡ�ź����ú���
*@param		val: Ϊ��0����ʾƬѡ�˿�Ϊ�ͣ�Ϊ��1����ʾƬѡ�˿�Ϊ��
*@return	��
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
*@brief		����W5500��Ƭѡ�˿�SCSnΪ��
*@param		��
*@return	��
*/
void iinchip_csoff(void)
{
	wiz_cs(LOW);
}

/**
*@brief		����W5500��Ƭѡ�˿�SCSnΪ��
*@param		��
*@return	��
*/
void iinchip_cson(void)
{	
   wiz_cs(HIGH);
}


/**
*@brief		STM32 SPI1��д8λ����
*@param		dat��д���8λ����
*@return	��
*/
uint8  IINCHIP_SpiSendData(uint8 dat)
{
	return(SPI_FLASH_SendByte(dat));
}

/**
*@brief		д��һ��8λ���ݵ�W5500
*@param		addrbsb: д�����ݵĵ�ַ
*@param   data��д���8λ����
*@return	��
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
*@brief		��W5500����һ��8λ����
*@param		addrbsb: д�����ݵĵ�ַ
*@param   data����д��ĵ�ַ����ȡ����8λ����
*@return	��
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
*@brief		��W5500д��len�ֽ�����
*@param		addrbsb: д�����ݵĵ�ַ
*@param   buf��д���ַ���
*@param   len���ַ�������
*@return	len�������ַ�������
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
*@brief		��W5500����len�ֽ�����
*@param		addrbsb: ��ȡ���ݵĵ�ַ
*@param 	buf����Ŷ�ȡ����
*@param		len���ַ�������
*@return	len�������ַ�������
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
*@brief		д������Ϣ��EEPROM
*@param		��
*@return	��
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
*@brief		��EEPROM��������Ϣ
*@param		��
*@return	��
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
*@brief		dhcp�õ��Ķ�ʱ����ʼ��
*@param		��
*@return	��
*/
void timer_init(void)
{
	TIM2_NVIC_Configuration();
	TIM2_Config(1000);										/* TIM2 ��ʱ���� */
	TIM2_ON();															
}

/**
*@brief		��ʱ��2�жϺ���
*@param		��
*@return	��
*/
void timer2_isr(void)
{
  	ms++;	
	if(ms>=1000)
	{
		ms=0;
		g_con_cnt++;
		#ifndef	_DNS_H_
		dhcp_time++;			/*DHCP��ʱ��1S*/
		#endif
		#ifndef	__NTP_H__
		ntptimer++;				/*NTP����ʱ���1S*/
		#endif
	}

}
/**
*@brief		STM32ϵͳ��λ����
*@param		��
*@return	��
*/
void reboot(void)
{
  pFunction Jump_To_Application;
  uint32 JumpAddress;
  printf(" ϵͳ�����С���\r\n");
  JumpAddress = *(vu32*) (0x00000004);
  Jump_To_Application = (pFunction) JumpAddress;
  Jump_To_Application();
}

void setKPALVTR(SOCKET i,uint8 val)
{
  IINCHIP_WRITE(Sn_KPALVTR(i),val);
}
