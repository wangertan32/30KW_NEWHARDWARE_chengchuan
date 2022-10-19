#include "tcp_client_demo.h"
#include "lwip/opt.h"
#include "lwip_comm.h"
#include "lwip/lwip_sys.h"
#include "lwip/api.h"
#include "includes.h"
#include "key.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ���������ɣ��������������κ���;
//ALIENTEK STM32F407������
//NETCONN API��̷�ʽ��TCP�ͻ��˲��Դ���	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/8/15
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) �������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//*******************************************************************************
//�޸���Ϣ
//��
////////////////////////////////////////////////////////////////////////////////// 	   
extern OS_TCB EthcTaskTCB;  

struct netconn *tcp_clientconn;					//TCP CLIENT�������ӽṹ��
u8 tcp_client_recvbuf[TCP_CLIENT_RX_BUFSIZE];	//TCP�ͻ��˽������ݻ�����
u8 *tcp_client_sendbuf="Explorer STM32F407 NETCONN TCP Client send data\r\n";	//TCP�ͻ��˷������ݻ�����
u8 flag_charge=0;

u8 tcp_client_flag=0;
//TCP�ͻ�������
#define TCPCLIENT_PRIO		6
//�����ջ��С
#define TCPCLIENT_STK_SIZE	300
//������ƿ�
OS_TCB	TcpClientTaskTCB;
//�����ջ
CPU_STK TCPCLIENT_TASK_STK[TCPCLIENT_STK_SIZE];

//tcp�ͻ���������
static void tcp_client_thread(void *arg)
{
	CPU_SR_ALLOC();
	OS_ERR err1,err2;
	u32 data_len = 0;
	struct pbuf *q;
	err_t err,recv_err;
	static ip_addr_t server_ipaddr,loca_ipaddr;
	static u16_t 		 server_port,loca_port;

	LWIP_UNUSED_ARG(arg);
	server_port = REMOTE_PORT;
	IP4_ADDR(&server_ipaddr, lwipdev.remoteip[0],lwipdev.remoteip[1], lwipdev.remoteip[2],lwipdev.remoteip[3]);
	
	while (1) 
	{
		tcp_clientconn = netconn_new(NETCONN_TCP);  //����һ��TCP����
		err = netconn_connect(tcp_clientconn,&server_ipaddr,server_port);//���ӷ�����
		if(err != ERR_OK)  
		{
			netconn_delete(tcp_clientconn); //����ֵ������ERR_OK,ɾ��tcp_clientconn����
			OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err1); //��ʱ1s
		}
		else if (err == ERR_OK)    //���������ӵ�����
		{ 
			struct netbuf *recvbuf;
			tcp_clientconn->recv_timeout = 10;
			netconn_getaddr(tcp_clientconn,&loca_ipaddr,&loca_port,1); //��ȡ����IP����IP��ַ�Ͷ˿ں�
		//	printf("�����Ϸ�����%d.%d.%d.%d,�����˿ں�Ϊ:%d\r\n",lwipdev.remoteip[0],lwipdev.remoteip[1], lwipdev.remoteip[2],lwipdev.remoteip[3],loca_port);
			while(1)
			{
				if((tcp_client_flag & LWIP_SEND_DATA) == LWIP_SEND_DATA) //������Ҫ����
				{
					if(flag_charge==1)
					err = netconn_write(tcp_clientconn,eth_onbuf,5,NETCONN_COPY);
					else 	err = netconn_write(tcp_clientconn ,eth_offbuf,5,NETCONN_COPY);
					if(err != ERR_OK)
					{
						//printf("����ʧ��\r\n");
					}
					tcp_client_flag &= ~LWIP_SEND_DATA;
				}
					
				if((recv_err = netconn_recv(tcp_clientconn,&recvbuf)) == ERR_OK)  //���յ�����
				{	
					OS_CRITICAL_ENTER(); //���ж�
					memset(tcp_client_recvbuf,0,TCP_CLIENT_RX_BUFSIZE);  //���ݽ��ջ���������
					for(q=recvbuf->p;q!=NULL;q=q->next)  //����������pbuf����
					{
						//�ж�Ҫ������TCP_CLIENT_RX_BUFSIZE�е������Ƿ����TCP_CLIENT_RX_BUFSIZE��ʣ��ռ䣬�������
						//�Ļ���ֻ����TCP_CLIENT_RX_BUFSIZE��ʣ�೤�ȵ����ݣ�����Ļ��Ϳ������е�����
						if(q->len > (TCP_CLIENT_RX_BUFSIZE-data_len)) memcpy(tcp_client_recvbuf+data_len,q->payload,(TCP_CLIENT_RX_BUFSIZE-data_len));//��������
						else memcpy(tcp_client_recvbuf+data_len,q->payload,q->len);
						data_len += q->len;  	
						if(data_len > TCP_CLIENT_RX_BUFSIZE) break; //����TCP�ͻ��˽�������,����	
					}
					OSTaskQPost(&EthcTaskTCB,(void *)tcp_client_recvbuf,data_len ,OS_OPT_POST_FIFO,&err2);
					
					OS_CRITICAL_EXIT();  //���ж�
					data_len=0;  //������ɺ�data_lenҪ���㡣					
					//printf("%s\r\n",tcp_client_recvbuf);
					
					netbuf_delete(recvbuf);
				}else if(recv_err == ERR_CLSD)  //�ر�����
				{
					netconn_close(tcp_clientconn);
					netconn_delete(tcp_clientconn);
					//printf("������%d.%d.%d.%d�Ͽ�����\r\n",lwipdev.remoteip[0],lwipdev.remoteip[1], lwipdev.remoteip[2],lwipdev.remoteip[3]);
					break;
				}
			}
		}
	}
}

//����TCP�ͻ����߳�
//����ֵ:0 TCP�ͻ��˴����ɹ�
//		���� TCP�ͻ��˴���ʧ��
u8 tcp_client_init(void)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	
	OS_CRITICAL_ENTER();//�����ٽ���
	//����TCP�ͻ�������
	OSTaskCreate((OS_TCB 	* )&TcpClientTaskTCB,		
				 (CPU_CHAR	* )"tcp_client task", 		
                 (OS_TASK_PTR )tcp_client_thread, 			
                 (void		* )0,					
                 (OS_PRIO	  )TCPCLIENT_PRIO,     
                 (CPU_STK   * )&TCPCLIENT_TASK_STK[0],	
                 (CPU_STK_SIZE)TCPCLIENT_STK_SIZE/10,	
                 (CPU_STK_SIZE)TCPCLIENT_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);
	OS_CRITICAL_EXIT();	//�˳��ٽ���
	return err;
}
