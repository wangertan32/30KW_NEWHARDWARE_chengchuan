#include "tcp_server_demo.h"
#include "lwip/opt.h"
#include "lwip_comm.h"
#include "lwip/lwip_sys.h"
#include "lwip/api.h"
#include "includes.h"
#include "key.h"

#include "tcp.h"


//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//NETCONN API编程方式的TCP服务器测试代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/8/15
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//*******************************************************************************
//修改信息
//无
////////////////////////////////////////////////////////////////////////////////// 	   
 
extern OS_TCB EthsTaskTCB;

u8 *tcp_server_sendbuf="Explorer STM32F407 NETCONN TCP Server send data\r\n";	
u8 tcp_server_flag;								//TCP服务器数据发送标志位
u8 tcp_server_recvbuf[TCP_SERVER_RX_BUFSIZE]={0};
//TCP客户端任务
#define TCPSERVER_PRIO		7
//任务堆栈大小
#define TCPSERVER_STK_SIZE	1024
//任务控制块
OS_TCB	TcpServerTaskTCB;
//任务堆栈
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
//tcp服务器任务
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

	conn = netconn_new(NETCONN_TCP);  //创建一个TCP链接
	netconn_bind(conn,IP_ADDR_ANY,TCP_SERVER_PORT);  //绑定端口 8号端口
	netconn_listen(conn);  		//进入监听模式
	conn->recv_timeout = 10;  	//禁止阻塞线程 等待10ms
	while (1) 
	{
		err = netconn_accept(conn,&newconn);  //接收连接请求
		if(err==ERR_OK)newconn->recv_timeout = 10;

		if (err == ERR_OK)    //处理新连接的数据
		{ 
			struct netbuf *recvbuf;

			netconn_getaddr(newconn,&ipaddr,&port,0); //获取远端IP地址和端口号
			
			remot_addr[3] = (uint8_t)(ipaddr.addr >> 24); 
			remot_addr[2] = (uint8_t)(ipaddr.addr>> 16);
			remot_addr[1] = (uint8_t)(ipaddr.addr >> 8);
			remot_addr[0] = (uint8_t)(ipaddr.addr);
		//	printf("主机%d.%d.%d.%d连接上服务器,主机端口号为:%d\r\n",remot_addr[0], remot_addr[1],remot_addr[2],remot_addr[3],port);
			
			while(1)
			{
				if((tcp_server_flag & LWIP_SEND_DATA) == LWIP_SEND_DATA) //有数据要发送
				{
					if(flag_charge==1) err = netconn_write(newconn,eth_onbuf,13,NETCONN_COPY);
					else 	err = netconn_write(newconn ,eth_offbuf,13,NETCONN_COPY);
				//	err = netconn_write(newconn ,tcp_server_sendbuf,strlen((char*)tcp_server_sendbuf),NETCONN_COPY); //发送tcp_server_sendbuf中的数据
					if(err != ERR_OK)
					{
		//				printf("发送失败\r\n");
					}
					tcp_server_flag &= ~LWIP_SEND_DATA;
				}
				
				if((recv_err = netconn_recv(newconn,&recvbuf)) == ERR_OK)  	//接收到数据
				{		
					OS_CRITICAL_ENTER(); //关中断
					memset(tcp_server_recvbuf,0,TCP_SERVER_RX_BUFSIZE);  //数据接收缓冲区清零
					for(q=recvbuf->p;q!=NULL;q=q->next)  //遍历完整个pbuf链表
					{ 
						//判断要拷贝到TCP_SERVER_RX_BUFSIZE中的数据是否大于TCP_SERVER_RX_BUFSIZE的剩余空间，如果大于
						//的话就只拷贝TCP_SERVER_RX_BUFSIZE中剩余长度的数据，否则的话就拷贝所有的数据
						if(q->len > (TCP_SERVER_RX_BUFSIZE-data_len)) memcpy(tcp_server_recvbuf+data_len,q->payload,(TCP_SERVER_RX_BUFSIZE-data_len));//拷贝数据
						else memcpy(tcp_server_recvbuf+data_len,q->payload,q->len);
						data_len += q->len;  	
						if(data_len > TCP_SERVER_RX_BUFSIZE) break; //超出TCP客户端接收数组,跳出	
					} 				
     
					OS_CRITICAL_EXIT();  //开中断				
					data_len=0;  //复制完成后data_len要清零	
					OSTaskQPost(&EthsTaskTCB,(void *)tcp_server_recvbuf,13,OS_OPT_POST_FIFO,&err1);
					OSTimeDlyHMSM(0,0,0,10,OS_OPT_TIME_PERIODIC,&err1);   //延时50ms
//				printf("%s\r\n",tcp_server_recvbuf);  //通过串口发送接收到的数据
					netbuf_delete(recvbuf);
				}else if(recv_err == ERR_CLSD)  //关闭连接
				{
					netconn_close(newconn);
					netconn_delete(newconn);
		//			printf("主机:%d.%d.%d.%d断开与服务器的连接\r\n",remot_addr[0], remot_addr[1],remot_addr[2],remot_addr[3]);
					break;
				}
			}
		}
	}
}


//创建TCP服务器线程
//返回值:0 TCP服务器创建成功
//		其他 TCP服务器创建失败
u8 tcp_server_init(void)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	
	OS_CRITICAL_ENTER();//进入临界区
	//创建TCP客户端任务
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
	OS_CRITICAL_EXIT();	//退出临界区
	return err;
}



