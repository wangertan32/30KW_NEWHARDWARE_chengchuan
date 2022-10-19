#include "udp_demo.h"
#include "lwip/opt.h"
#include "lwip_comm.h"
#include "lwip/lwip_sys.h"
#include "lwip/api.h"
#include "includes.h"
#include "string.h"
#include "main.h"
#include "can.h"

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
 
//TCP�ͻ�������
#define UDP_PRIO		12
//�����ջ��С
#define UDP_STK_SIZE	300
//�����ջ
CPU_STK UDP_TASK_STK[UDP_STK_SIZE];
//������ƿ�
OS_TCB	UdpTaskTCB;


u8 udp_ctr_recvbuf[UDP_CTR_RX_BUFSIZE];	//UDP�������ݻ�����

//UDP������������
u8 udp_sendbuf[20]={0xAA,0x14,0,0};
u8 udp_flag;		//UDP���ݷ��ͱ�־λ

//udp������
static void udp_ctr_thread(void *arg)
{
	CPU_SR_ALLOC();
	OS_ERR err;
	static struct netconn *udpconn;
	static struct netbuf  *recvbuf;
	static struct netbuf  *sentbuf;
	struct ip_addr destipaddr;
	u32 data_len = 0;
	struct pbuf *q;
	
	LWIP_UNUSED_ARG(arg);
	udpconn = netconn_new(NETCONN_UDP);  //����һ��UDP����
	udpconn->recv_timeout = 10;  		
	
	if(udpconn != NULL)  //����UDP���ӳɹ�
	{
			
		err = netconn_bind(udpconn,IP_ADDR_ANY,UDP_CTR_PORT); 
		IP4_ADDR(&destipaddr,lwipdev.remoteip[0],lwipdev.remoteip[1], lwipdev.remoteip[2],lwipdev.remoteip[3]); //����Ŀ��IP��ַ
		netconn_connect(udpconn,&destipaddr,UDP_LOCAL_PORT); 	//���ӵ�Զ������
		if(err == ERR_OK)//�����
		{
			while(1)
			{

				if((udp_flag & LWIP_SEND_DATA) == LWIP_SEND_DATA) //��ʱ������δ�ϵ�
				{

						sentbuf = netbuf_new();
						netbuf_alloc(sentbuf,20);
						memcpy(sentbuf->p->payload,(void*)(udp_sendbuf),20);
						err = netconn_send(udpconn,sentbuf);  	//��netbuf�е����ݷ��ͳ�ȥ	
 
					if(err != ERR_OK)
					{
					//	printf("����ʧ��\r\n");
						netbuf_delete(sentbuf);      //ɾ��buf
					}
					udp_flag &= ~LWIP_SEND_DATA;	//������ݷ��ͱ�־
					netbuf_delete(sentbuf);      	//ɾ��buf
				}	
								
				netconn_recv(udpconn,&recvbuf); //��������
				if(recvbuf != NULL)          //���յ�����
				{ 
					OS_CRITICAL_ENTER(); //���ж�
					memset(udp_ctr_recvbuf,0,UDP_CTR_RX_BUFSIZE);  //���ݽ��ջ���������
					for(q=recvbuf->p;q!=NULL;q=q->next)  //����������pbuf����
					{
						//�ж�Ҫ������UDP_DEMO_RX_BUFSIZE�е������Ƿ����UDP_DEMO_RX_BUFSIZE��ʣ��ռ䣬�������
						//�Ļ���ֻ����UDP_DEMO_RX_BUFSIZE��ʣ�೤�ȵ����ݣ�����Ļ��Ϳ������е�����
						if(q->len > (UDP_CTR_RX_BUFSIZE-data_len)) memcpy(udp_ctr_recvbuf+data_len,q->payload,(UDP_CTR_RX_BUFSIZE-data_len));//��������
						else memcpy(udp_ctr_recvbuf+data_len,q->payload,q->len);
						data_len += q->len;  	
						if(data_len > UDP_CTR_RX_BUFSIZE) break; //����TCP�ͻ��˽�������,����	
					}

					udp_flag |= LWIP_SEND_DATA;	
					OS_CRITICAL_EXIT();  //���ж�
					data_len=0;  //������ɺ�data_lenҪ���㡣
				//	printf("%s\r\n",udp_demo_recvbuf);  //��ӡ���յ�������
					netbuf_delete(recvbuf);      //ɾ��buf
				}else {}; 
			}
		}
	}
}


//����UDP_ctr�̣߳�������������
//����ֵ:0 UDP�����ɹ�
//		���� UDP����ʧ��
u8 udp_ctr_init(void)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	
	OS_CRITICAL_ENTER();//�����ٽ���
	OSTaskCreate((OS_TCB 	* )&UdpTaskTCB,		
				 (CPU_CHAR	* )"udp task", 		
                 (OS_TASK_PTR )udp_ctr_thread, 			
                 (void		* )0,					
                 (OS_PRIO	  )UDP_PRIO,     
                 (CPU_STK   * )&UDP_TASK_STK[0],	
                 (CPU_STK_SIZE)UDP_STK_SIZE/10,	
                 (CPU_STK_SIZE)UDP_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);
	OS_CRITICAL_EXIT();	//�˳��ٽ���
	return err;
}




