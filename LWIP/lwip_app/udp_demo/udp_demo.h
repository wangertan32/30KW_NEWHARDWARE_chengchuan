#ifndef __UDP_DEMO_H
#define __UDP_DEMO_H
#include "sys.h"
#include "includes.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//NETCONN API��̷�ʽ��UDP���Դ���	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/8/15
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//*******************************************************************************
//�޸���Ϣ
//��
////////////////////////////////////////////////////////////////////////////////// 	   
 
 
#define UDP_CTR_RX_BUFSIZE		10	//����udp���������ݳ���
#define UDP_CTR_PORT			2000	//����udp���ӵķ������˿ں�
#define UDP_LOCAL_PORT    2183
#define LWIP_SEND_DATA			0X80    //���������ݷ���


extern u8 udp_flag;


u8 udp_ctr_init(void);
u8 udp_s_init(void);
#endif

