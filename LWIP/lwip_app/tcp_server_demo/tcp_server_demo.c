#include "tcp_server_demo.h"
#include "lwip/opt.h"
#include "lwip_comm.h"
#include "lwip/lwip_sys.h"
#include "lwip/api.h"
#include "includes.h"
#include "key.h"

#include "tcp.h"


//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//NETCONN API��̷�ʽ��TCP���������Դ���	   
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
 
extern OS_TCB EthsTaskTCB;

u8 *tcp_server_sendbuf="Explorer STM32F407 NETCONN TCP Server send data\r\n";	
u8 tcp_server_flag;								//TCP���������ݷ��ͱ�־λ
u8 tcp_server_recvbuf[TCP_SERVER_RX_BUFSIZE]={0};
//TCP�ͻ�������
#define TCPSERVER_PRIO		7
//�����ջ��С
#define TCPSERVER_STK_SIZE	1024
//������ƿ�
OS_TCB	TcpServerTaskTCB;
//�����ջ
CPU_STK TCPSERVER_TASK_STK[TCPSERVER_STK_SIZE];

u8 eth_onbuf[13]={8,00,00,00,0x5a,0xaa,05,01,00,00,00,00,0x78};
u8 eth_offbuf[13]={8,00,00,00,0x5a,0xaa,05,02,00,00,00,00,0x79};

struct name *name={0};

static u32 get_pbuf(struct pbuf* p,u8* pBuffer, u32 maxlen)
{
	u32 remainlen;
	u32 readlen=0;
	for(;p!=NULL;p=p->next)
	{
			remainlen=maxlen-readlen;
			if(p->len >=remainlen)
			{
				memcpy(&pBuffer[readlen],p->payload,remainlen);
				readlen+=remainlen;
				break;			
			}
			
			memcpy(&pBuffer[readlen],p->payload,p->len);
			readlen+=p->len;
	}

	return readlen;
}

static err_t tcp_server_recv(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err)
{
    struct pbuf *q;
    struct name *name = (struct name *)arg;
    int done;
    char *c;
    int i;

    /* we perform here any necessary processing on the pbuf */
    if (p != NULL){    
        /* we call this function to tell the LwIp that we have processed the data */
        /* this lets the stack advertise a larger window, so more data can be received*/
        tcp_recved(pcb, p->tot_len);

        /* check the name if NULL, no data passed, return withh illegal argument error */
        if(!name){ 
            pbuf_free(p);
            return ERR_ARG;
        }

     //   done = 0;
        for(q=p; q != NULL; q = q->next){ 
            c = q->payload;
            for(i=0; i<q->len ; i++){
                if(name->length < MAX_NAME_SIZE){
                    /* save the received data to name->bytes */
                    name->bytes[name->length++] = c[i];
                }
            } 
        }
        name->length = MAX_NAME_SIZE;
				tcp_client_flag|=LWIP_SEND_DATA; 
				
    //    tcp_write(pcb, name->bytes, name->length, TCP_WRITE_FLAG_COPY);
 

        /* end of processing, we free the pbuf */
        pbuf_free(p);
        
    }else if (err == ERR_OK){  
        /* when the pbuf is NULL and the err is ERR_OK, the remote end is closing the connection. */
        /* we free the allocated memory and we close the connection */
        mem_free(name);
       
        return tcp_close(pcb);
    }
    return ERR_OK;
}
//tcp����������
static void tcp_server_thread(void *arg)
{
	CPU_SR_ALLOC();
	OS_ERR err1;
	u32 data_len = 0;
	struct pbuf *q;
	err_t err,recv_err;
	u8 remot_addr[4];
	struct netconn *conn, *newconn;
	static ip_addr_t ipaddr;
	static u16_t 			port;

	
	LWIP_UNUSED_ARG(arg);

	conn = netconn_new(NETCONN_TCP);  //����һ��TCP����
	netconn_bind(conn,IP_ADDR_ANY,TCP_SERVER_PORT);  //�󶨶˿� 8�Ŷ˿�
	netconn_listen(conn);  		//�������ģʽ
	conn->recv_timeout = 10;  	//��ֹ�����߳� �ȴ�10ms
	while (1) 
	{
		err = netconn_accept(conn,&newconn);  //������������
		if(err==ERR_OK)newconn->recv_timeout = 10;

		if (err == ERR_OK)    //���������ӵ�����
		{ 
			struct netbuf *recvbuf;

			netconn_getaddr(newconn,&ipaddr,&port,0); //��ȡԶ��IP��ַ�Ͷ˿ں�
			
			remot_addr[3] = (uint8_t)(ipaddr.addr >> 24); 
			remot_addr[2] = (uint8_t)(ipaddr.addr>> 16);
			remot_addr[1] = (uint8_t)(ipaddr.addr >> 8);
			remot_addr[0] = (uint8_t)(ipaddr.addr);
		//	printf("����%d.%d.%d.%d�����Ϸ�����,�����˿ں�Ϊ:%d\r\n",remot_addr[0], remot_addr[1],remot_addr[2],remot_addr[3],port);
			
			while(1)
			{
				if((tcp_server_flag & LWIP_SEND_DATA) == LWIP_SEND_DATA) //������Ҫ����
				{
					if(flag_charge==1) err = netconn_write(newconn,eth_onbuf,13,NETCONN_COPY);
					else 	err = netconn_write(newconn ,eth_offbuf,13,NETCONN_COPY);
				//	err = netconn_write(newconn ,tcp_server_sendbuf,strlen((char*)tcp_server_sendbuf),NETCONN_COPY); //����tcp_server_sendbuf�е�����
					if(err != ERR_OK)
					{
		//				printf("����ʧ��\r\n");
					}
					tcp_server_flag &= ~LWIP_SEND_DATA;
				}
				
				if((recv_err = netconn_recv(newconn,&recvbuf)) == ERR_OK)  	//���յ�����
				{		
					OS_CRITICAL_ENTER(); //���ж�
					memset(tcp_server_recvbuf,0,TCP_SERVER_RX_BUFSIZE);  //���ݽ��ջ���������
					for(q=recvbuf->p;q!=NULL;q=q->next)  //����������pbuf����
					{ 
						//�ж�Ҫ������TCP_SERVER_RX_BUFSIZE�е������Ƿ����TCP_SERVER_RX_BUFSIZE��ʣ��ռ䣬�������
						//�Ļ���ֻ����TCP_SERVER_RX_BUFSIZE��ʣ�೤�ȵ����ݣ�����Ļ��Ϳ������е�����
						if(q->len > (TCP_SERVER_RX_BUFSIZE-data_len)) memcpy(tcp_server_recvbuf+data_len,q->payload,(TCP_SERVER_RX_BUFSIZE-data_len));//��������
						else memcpy(tcp_server_recvbuf+data_len,q->payload,q->len);
						data_len += q->len;  	
						if(data_len > TCP_SERVER_RX_BUFSIZE) break; //����TCP�ͻ��˽�������,����	
					} 				
     
					OS_CRITICAL_EXIT();  //���ж�				
					data_len=0;  //������ɺ�data_lenҪ����	
					OSTaskQPost(&EthsTaskTCB,(void *)tcp_server_recvbuf,13,OS_OPT_POST_FIFO,&err1);
					OSTimeDlyHMSM(0,0,0,10,OS_OPT_TIME_PERIODIC,&err1);   //��ʱ50ms
//				printf("%s\r\n",tcp_server_recvbuf);  //ͨ�����ڷ��ͽ��յ�������
					netbuf_delete(recvbuf);
				}else if(recv_err == ERR_CLSD)  //�ر�����
				{
					netconn_close(newconn);
					netconn_delete(newconn);
		//			printf("����:%d.%d.%d.%d�Ͽ��������������\r\n",remot_addr[0], remot_addr[1],remot_addr[2],remot_addr[3]);
					break;
				}
			}
		}
	}
}


//����TCP�������߳�
//����ֵ:0 TCP�����������ɹ�
//		���� TCP����������ʧ��
u8 tcp_server_init(void)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	
	OS_CRITICAL_ENTER();//�����ٽ���
	//����TCP�ͻ�������
	OSTaskCreate((OS_TCB 	* )&TcpServerTaskTCB,		
				 (CPU_CHAR	* )"tcp_server task", 		
                 (OS_TASK_PTR )tcp_server_thread, 			
                 (void		* )0,					
                 (OS_PRIO	  )TCPSERVER_PRIO,     
                 (CPU_STK   * )&TCPSERVER_TASK_STK[0],	
                 (CPU_STK_SIZE)TCPSERVER_STK_SIZE/10,	
                 (CPU_STK_SIZE)TCPSERVER_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);
	OS_CRITICAL_EXIT();	//�˳��ٽ���
	return err;
}



