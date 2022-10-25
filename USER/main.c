//TEXT
#include "main.h"
uint8 First_Start;//�״��ϵ��־
SYSTEM_FIXED systemFixedData;//ϵͳ�̶����� ����ID����Ӳ���汾

void start_task(void *pdata);
void plc_task(void *p_arg);
void can_task(void *p_arg);
void meter_task(void *pdata);
void screen_task(void *pdata); 
void scan_task(void *pdata);
void handle_task(void *pdata);

CPU_STK	SCAN_TASK_STK[SCAN_STK_SIZE];
CPU_STK	CAN_TASK_STK[CAN_STK_SIZE];
CPU_STK	PLC_TASK_STK[PLC_STK_SIZE];
CPU_STK SCREEN_TASK_STK[SCREEN_STK_SIZE];
CPU_STK METER_TASK_STK[METER_STK_SIZE];
CPU_STK HANDLE_TASK_STK[HANDLE_STK_SIZE];

CPU_STK START_TASK_STK[START_STK_SIZE];

OS_TCB ScanTaskTCB;
OS_TCB CanTaskTCB;
OS_TCB ScreenTaskTCB;
OS_TCB MeterTaskTCB;
OS_TCB PlcTaskTCB;
OS_TCB HandleTaskTCB;
OS_TCB StartTaskTCB;

OS_TMR 	tmr1;		//��ʱ��1
void tmr1_callback(void *p_tmr, void *p_arg); 	//��ʱ��1�ص�����

OS_TMR 	tmr2;		//��ʱ��1
void tmr2_callback(void *p_tmr, void *p_arg); 	//��ʱ��2�ص�����

static void sys_init(void);
static void poweron(void);
static void poweroff(void);
static void Set_ptcdata(void);
static void waring_poweroff(void);
int main(void)
{ 

	OS_ERR err;
	CPU_SR_ALLOC();

	delay_init(168);       	//��ʱ��ʼ��
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//�жϷ�������
	
	mymem_init(SRAMIN);  	//��ʼ���ڲ��ڴ��
	mymem_init(SRAMEX);  	//��ʼ���ⲿ�ڴ��
	mymem_init(SRAMCCM); 	//��ʼ��CCM�ڴ��
	
	sys_init();
	delay_ms(100);
	
	OSInit(&err); 					//UCOSIII��ʼ��
#if ETHERNET	
u8 lwip_init_cnt=0;
u8 lwip_ctr_cnt=0;
  while(lwip_comm_init()) 		//lwip��ʼ��
	{
		delay_ms(500); 
		lwip_init_cnt++;
		if(lwip_init_cnt>10)
		break;	
	}
	if(lwip_init_cnt<10)
	{
		lwip_init_cnt=0;
		printf("ethernet init finished!\r\n");
	}
	
	while(udp_ctr_init()) 		//(����udp_ctr�߳�)
	{
		delay_ms(500);
		lwip_ctr_cnt++;
		if(lwip_ctr_cnt>10)
		break;
	}
	if(lwip_ctr_cnt<10)
	{
		lwip_ctr_cnt=0;
		printf("ethernet build udp connect!\r\n");
	}
#endif		
	OS_CRITICAL_ENTER();//�����ٽ���
	//������ʼ����
	OSTaskCreate((OS_TCB 	* )&StartTaskTCB,		//������ƿ�
				 (CPU_CHAR	* )"start task", 		//��������
                 (OS_TASK_PTR )start_task, 			//������
                 (void		* )0,					//���ݸ��������Ĳ���
                 (OS_PRIO	  )START_TASK_PRIO,     //�������ȼ�
                 (CPU_STK   * )&START_TASK_STK[0],	//�����ջ����ַ
                 (CPU_STK_SIZE)START_STK_SIZE/10,	//�����ջ�����λ
                 (CPU_STK_SIZE)START_STK_SIZE,		//�����ջ��С
                 (OS_MSG_QTY  )0,					//�����ڲ���Ϣ�����ܹ����յ������Ϣ��Ŀ,Ϊ0ʱ��ֹ������Ϣ
                 (OS_TICK	  )0,					//��ʹ��ʱ��Ƭ��תʱ��ʱ��Ƭ���ȣ�Ϊ0ʱΪĬ�ϳ��ȣ�
                 (void   	* )0,					//�û�����Ĵ洢��
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, //����ѡ��
                 (OS_ERR 	* )&err);				//��Ÿú�������ʱ�ķ���ֵ
	OS_CRITICAL_EXIT();	//�˳��ٽ���

	OSStart(&err); 		//����UCOS
}

//start����
void start_task(void *p_arg)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	p_arg = p_arg;

	CPU_Init();
#if OS_CFG_STAT_TASK_EN > 0u
   OSStatTaskCPUUsageInit(&err);  	//ͳ������                
#endif
	
#ifdef CPU_CFG_INT_DIS_MEAS_EN		//���ʹ���˲����жϹر�ʱ��
    CPU_IntDisMeasMaxCurReset();	
#endif

#if	OS_CFG_SCHED_ROUND_ROBIN_EN  //��ʹ��ʱ��Ƭ��ת��ʱ��
	 //ʹ��ʱ��Ƭ��ת���ȹ���,ʱ��Ƭ����Ϊ1��ϵͳʱ�ӽ��ģ���1*5=5ms
	OSSchedRoundRobinCfg(DEF_ENABLED,1,&err);  
#endif		
	
#if LWIP_DHCP
	lwip_comm_dhcp_creat(); //����DHCP����
#endif


//������ʱ��1
	OSTmrCreate((OS_TMR		*)&tmr1,		//��ʱ��1
                (CPU_CHAR	*)"tmr1",		//��ʱ������
                (OS_TICK	 )0,			//
                (OS_TICK	 )100,          //tmr_rate 500ms
                (OS_OPT		 )OS_OPT_TMR_PERIODIC, //����ģʽ
                (OS_TMR_CALLBACK_PTR)tmr1_callback,//��ʱ��1�ص�����
								(void	    *)0,			//����Ϊ0
                (OS_ERR	    *)&err);		//���صĴ�����

	OSTmrCreate((OS_TMR		*)&tmr2,		//��ʱ��2
                (CPU_CHAR	*)"tmr2",		//��ʱ������
                (OS_TICK	 )0,			//
                (OS_TICK	 )300,          //tmr_rate 3s
                (OS_OPT		 )OS_OPT_TMR_PERIODIC, //����ģʽ
                (OS_TMR_CALLBACK_PTR)tmr2_callback,//��ʱ��1�ص�����
								(void	    *)0,			//����Ϊ0
                (OS_ERR	    *)&err);		//���صĴ�����
								
	OS_CRITICAL_ENTER();	//�����ٽ���  
	
	//����CAN���� 16
	OSTaskCreate((OS_TCB 	* )&CanTaskTCB,		
				 (CPU_CHAR	* )"Can task", 		
                 (OS_TASK_PTR )can_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )CAN_TASK_PRIO,                  
                 (CPU_STK   * )&CAN_TASK_STK[0],	
                 (CPU_STK_SIZE)CAN_STK_SIZE/10,	
                 (CPU_STK_SIZE)CAN_STK_SIZE,		
                 (OS_MSG_QTY  )TASK_CAN_NUM,		//����Msgdis_task��Ҫʹ���ڽ���Ϣ���У���Ϣ���г���Ϊ4					
                 (OS_TICK	  )10,  					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);
	//����scan����		14												 	 			 
	OSTaskCreate((OS_TCB 	* )&ScanTaskTCB,		
				 (CPU_CHAR	* )"scan task", 		
                 (OS_TASK_PTR )scan_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )SCAN_TASK_PRIO,     
                 (CPU_STK   * )&SCAN_TASK_STK[0],	
                 (CPU_STK_SIZE)SCAN_STK_SIZE/10,	
                 (CPU_STK_SIZE)SCAN_STK_SIZE,		
                 (OS_MSG_QTY  )TASK_SCAN_NUM,		//����Msgdis_task��Ҫʹ���ڽ���Ϣ���У���Ϣ���г���Ϊ4					
                 (OS_TICK	  )10,  					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);
								 
	//����handle����		15												 	 			 
	OSTaskCreate((OS_TCB 	* )&HandleTaskTCB,		
				 (CPU_CHAR	* )"handle task", 		
                 (OS_TASK_PTR )handle_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )HANDLE_TASK_PRIO,     
                 (CPU_STK   * )&HANDLE_TASK_STK[0],	
                 (CPU_STK_SIZE)HANDLE_STK_SIZE/10,	
                 (CPU_STK_SIZE)HANDLE_STK_SIZE,		
                 (OS_MSG_QTY  )TASK_HANDLE_NUM,		//����Msgdis_task��Ҫʹ���ڽ���Ϣ���У���Ϣ���г���Ϊ4					
                 (OS_TICK	  )10,  					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);
								 
	//�����������		12					 								 
	OSTaskCreate((OS_TCB 	* )&MeterTaskTCB,		
				 (CPU_CHAR	* )"meter task", 		 
                 (OS_TASK_PTR )meter_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )METER_TASK_PRIO,     
                 (CPU_STK   * )&METER_TASK_STK[0],	
                 (CPU_STK_SIZE)METER_STK_SIZE/10,	
                 (CPU_STK_SIZE)METER_STK_SIZE,		
                 (OS_MSG_QTY  )TASK_METER_NUM,		//����Msgdis_task��Ҫʹ���ڽ���Ϣ���У���Ϣ���г���Ϊ4					
                 (OS_TICK	  )10,  					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);	
								 
	//������Ļ��������			17												 
	OSTaskCreate((OS_TCB 	* )&ScreenTaskTCB,		
				 (CPU_CHAR	* )"screen task", 		
                 (OS_TASK_PTR )screen_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )SCREEN_TASK_PRIO,     
                 (CPU_STK   * )&SCREEN_TASK_STK[0],	
                 (CPU_STK_SIZE)SCREEN_STK_SIZE/10,	
                 (CPU_STK_SIZE)SCREEN_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )10,					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);	
	//����Զ�̽�������    13  
	OSTaskCreate((OS_TCB 	* )&PlcTaskTCB,		
				 (CPU_CHAR	* )"plc task", 		
                 (OS_TASK_PTR )plc_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )PLC_TASK_PRIO,     	
                 (CPU_STK   * )&PLC_TASK_STK[0],	
                 (CPU_STK_SIZE)PLC_STK_SIZE/10,	
                 (CPU_STK_SIZE)PLC_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )10,					
                 (void   	* )0,				
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, 
                 (OS_ERR 	* )&err);	
							 
								 
		OS_TaskSuspend((OS_TCB*)&StartTaskTCB,&err);		//����ʼ����	
					
		OS_CRITICAL_EXIT();	//�����ٽ���							 
								 
}

u8 test_buf[10]={1,2,3,4,5,6,7,8,9};
void screen_task(void *pdata)
{				
	u8 *p;
	OS_ERR err; 
	OS_MSG_SIZE size;
	u8 screen_txbuf[5]={0};
	OSTmrStart(&tmr1,&err);	//��ʱ�ϴ�����
	OSTmrStart(&tmr2,&err);	//����CSU-PTC������
	while(1)
	{
		//������Ϣ  ��������ָ��
		p=OSTaskQPend((OS_TICK		)0,
                      (OS_OPT		)OS_OPT_PEND_BLOCKING,
                      (OS_MSG_SIZE*	)&size,
                      (CPU_TS*		)0,
                      (OS_ERR*      )&err );		
		if(err == OS_ERR_NONE)
		{	
				if(p[0]==0x55&&p[1]==0x66)   ///// ��-csu
				 {					 
				 		switch(p[4])
						{
							case SCREEN_POWERON:  /////// ����ָ��
							
								SCREEN_485_TX;  // 
								delay_ms(10);
								screen_ack(SCREEN_ACKON );
								delay_ms(50);
								SCREEN_485_RX;
							
							  screen_txbuf[0]=MSG_CAN_POWERON;    //// ��can������Ϣ ����
								OSTaskQPost(&CanTaskTCB,(void *)screen_txbuf,3,OS_OPT_POST_FIFO,&err);
								OSTimeDlyHMSM(0,0,0,50,OS_OPT_TIME_PERIODIC,&err);   //��ʱ50ms
							
							break;
							
							case SCREEN_POWEROFF:   ///// �ػ�ָ��
//// �й����·��ػ�ָ��				
							 SYSTEM_POWER_=POWER_OFF; 
								poweroff();

							
							break;
							
							case SCREEN_CONFIG:    ///// ��������
								
								SetPTCData.s_Bus_v = (getu16(p[7],p[6]))*10;    ////// ĸ�ߵ�ѹ
								SetPTCData.s_INV_fre = (getu16(p[9],p[8]))*10;  ////  ��乤��Ƶ��
							
							screen_txbuf[0]=MSG_CAN_CONFIG_DATA;    //// ��������ָ��
								OSTaskQPost(&CanTaskTCB,(void *)screen_txbuf,3,OS_OPT_POST_FIFO,&err);
								OSTimeDlyHMSM(0,0,0,50,OS_OPT_TIME_PERIODIC,&err);   //��ʱ50ms
							break;
							
							case SCREEN_SEEK:     ///// �������� ����������Ҫ���� ��ʱû�õ�
									SCREEN_485_TX;
									delay_ms(10);
									screen_datasend(SCREEN_ACKDATA);   //////  csu�ϴ����ݸ���
									delay_ms(10);
									SCREEN_485_RX;	
							break;
							//��������485��
							case UART_TEST:
								 	switch(p[5])
									{
										case 1:
												METER_485_TX;
												delay_ms(10);
												uart_send_dma(UART_METER,test_buf, 10);
												delay_ms(50);
												METER_485_RX;	
										break;
										
										case 2:
												FAILOVER_485_TX;
												delay_ms(10);
												uart_send_dma(UART_FAILOVER,test_buf, 10);
												delay_ms(50);
												FAILOVER_485_RX;	
									  break;
										
										case 3:
												OUT_485_TX;
												delay_ms(10);
												uart_send_dma(UART_OUT,test_buf, 10);
												delay_ms(50);
												OUT_485_RX;		
										break;
										
										case 4:
												SCREEN_485_TX;
												delay_ms(10);
												uart_send_dma(UART_SCREEN,test_buf, 10);
												delay_ms(50);
												SCREEN_485_RX;	
										break;
										default: break;
									}
						
							break;
							
							default:
							break;
						}								 
				 }
		}
		delay_ms(500);
		IWDG_Feed();//ι��
	}
	
}


void can_task(void *p_arg)
{
	u8 num_frame=0;
	u8 *p;
	OS_ERR err; 
	OS_MSG_SIZE size;
	u8 data[6]={0};
	while(1)
	{
		//������Ϣ  �ȴ���Ϣ����
		p=OSTaskQPend((OS_TICK		)0,    /// һֱ�ȴ�
                      (OS_OPT		)OS_OPT_PEND_BLOCKING,     //// ��û��Ϣ����һֱ�ȵ�����Ϣ
                      (OS_MSG_SIZE*	)&size,       ////������ʾ���յ�����Ϣ����(�ֽ�)
                      (CPU_TS*		)0,     ///// ��Ҫ��ʱ���
                      (OS_ERR*      )&err ); ///// ���ô˺������صĴ�����
		if(err == OS_ERR_NONE)
		{	
			  switch(p[0])
				{
					case MSG_CAN_LINK: //����						
						num_frame=can2_data_packet(FUNC_LINK,0, 0 ,0);
						Can2Send_MultiMessages(canid_tx, can2_tbuf, num_frame*8);
					break;
					
					case MSG_CAN_SEARCH:   //��̨��ѯ
						num_frame=can2_data_packet(FUNC_SEEK, 0, 0 ,0);
						Can2Send_MultiMessages(canid_tx, can2_tbuf, num_frame*8);
					break;
										
					case MSG_CAN_POWERON:   //��
						 poweron();									
					break;

					case MSG_CAN_POWEROFF:   //��
						 poweroff();
					break;
					
					case MSG_CAN_CONFIG_NUM:   //���û���
					{
						num_frame=can2_data_packet(FUNC_SETNUM, 0, 6 ,0);
						Can2Send_MultiMessages(canid_tx, can2_tbuf, num_frame*8);						
					}
					break;
					
					case MSG_CAN_CONFIG_DATA:   //���ò���
						Set_ptcdata();
					break;
					
					case MSG_CAN_CONFIG_SEARCH_DATA:   //��ѯ����ֵ 100ms
							if(System_State == WARNING)
							{
//								waring_poweroff();
							}
							screen_datasend(SCREEN_ACKDATA);   //// ��ʱ��������
					break;
									
					case MSG_CAN_CONFIG_SEARCH_STATE:   //��ѯ״ֵ̬  300ms
					{
						if(System_State == STANDBY)     ////// ����
						{
							data[0] = 0x01;
						}else if(System_State == RUNNING)    //// ����
						{
							data[0] = 0x02;
						}else if(System_State == WARNING)     ////�澯
						{
							data[0] = 0x04;
						}else if(System_State == SHUTDOWN)     ////  ��ͣ
						{
							data[0] = 0x08;
						}                                   /////  ����0xfc 1111 1100   //// ����0x0f 0000 1111
						data[1] = 0;
						data[2] = (statio.all&0xff);
						data[3] = statio.io.emo;
						data[4] = 0x00;
//						data[3] = ((statio.all>>8)&0xff);
//						data[4] = ((statio.all>>16)&0xff);
						data[5] = 0x00;
						num_frame=can2_data_packet(FUNC_SEEKSTAT,0,14,data);        /// can�������
						Can2Send_MultiMessages(canid_tx, can2_tbuf, num_frame*8);   //// can�������� ��ptc
						
						Write_Log();      //// ��¼��sd
					}
					
					break;
					
					default:
					break;
				}
		}
		delay_ms(500);
		IWDG_Feed();//ι��
	}
}



u8 scan_txbuf[5]={0};
void scan_task(void *pdata)
{		
	u8 screen_txbuf[5]={0};	
	OS_ERR err; 
/////////
	while(1)
	{
		read_io();   ////// ��io״̬
		
		if(SYSTEM_POWER_==POWER_ON)		 /// ��������
		{
		 screen_txbuf[0]=MSG_CAN_POWERON;    //// ��can������Ϣ ����
		 OSTaskQPost(&CanTaskTCB,(void *)screen_txbuf,3,OS_OPT_POST_FIFO,&err);
		 OSTimeDlyHMSM(0,0,0,50,OS_OPT_TIME_PERIODIC,&err);   //��ʱ50ms
			
		}	
		if(SYSTEM_POWER_==POWER_OFF)		 /// �ػ�����
		{
		 screen_txbuf[0]=MSG_CAN_POWEROFF;    //// ��can������Ϣ ����
		 OSTaskQPost(&CanTaskTCB,(void *)screen_txbuf,3,OS_OPT_POST_FIFO,&err);
		 OSTimeDlyHMSM(0,0,0,50,OS_OPT_TIME_PERIODIC,&err);   //��ʱ50ms
			
		}	
		
		delay_ms(500);    ////// 
		IWDG_Feed();//ι��
	}
}

void handle_task(void *pdata)
{				

	while(1)
	{
		handle_led();    ////// ���Ƶ�
		handle_alarm();  ///// ����
		handle_fault();  ///// ����
		delay_ms(500);
		IWDG_Feed();//ι��
	}
}

void plc_task(void *pdata)
{
				
	u8 *p;
	OS_ERR err; 
	OS_MSG_SIZE size;
	while(1)
	{
		//������Ϣ
		p=OSTaskQPend((OS_TICK		)0,
                      (OS_OPT		)OS_OPT_PEND_BLOCKING,
                      (OS_MSG_SIZE*	)&size,
                      (CPU_TS*		)0,
                      (OS_ERR*      )&err );		
		if(err == OS_ERR_NONE)
		{	

		}
	}
}

void meter_task(void *pdata)
{
				
	u8 *p;
	OS_ERR err; 
	OS_MSG_SIZE size;
	while(1)
	{
		//������Ϣ
		p=OSTaskQPend((OS_TICK		)0,
                      (OS_OPT		)OS_OPT_PEND_BLOCKING,
                      (OS_MSG_SIZE*	)&size,
                      (CPU_TS*		)0,
                      (OS_ERR*      )&err );		
		if(err == OS_ERR_NONE)
		{	

		}
	}
}


u8 tmr1_txbuf[5]={0};
//��ʱ��1�Ļص�����
void tmr1_callback(void *p_tmr, void *p_arg)
{
	OS_ERR err; 
	static u16 tmr1_num=0;
		
	tmr1_txbuf[0]=MSG_CAN_CONFIG_SEARCH_DATA;  //��ʱ��ѯ����
	/////                    Ҫ���͵���Ϣ    ��Ϣ�ֽ���    ��������Ϣ���е����
	OSTaskQPost(&CanTaskTCB,(void *)tmr1_txbuf,3,OS_OPT_POST_FIFO,&err);  ///// ����һ����Ϣ��
	OSTimeDlyHMSM(0,0,0,50,OS_OPT_TIME_PERIODIC,&err);   //��ʱ50ms
 	tmr1_num++;		//��ʱ��1ִ�д�����1					
}


u8 tmr2_txbuf[3]={0};
//��ʱ��2�Ļص�����
void tmr2_callback(void *p_tmr, void *p_arg)
{
	OS_ERR err; 
	static u16 tmr2_num=0;
	
	tmr2_txbuf[0]=MSG_CAN_CONFIG_SEARCH_STATE;  //��ʱ��ѯ״̬
	OSTaskQPost(&CanTaskTCB,(void *)tmr2_txbuf,3,OS_OPT_POST_FIFO,&err);
	OSTimeDlyHMSM(0,0,0,50,OS_OPT_TIME_PERIODIC,&err);   //��ʱ50ms
	
 	tmr2_num++;		//��ʱ��1ִ�д�����1					
}

static void sys_init(void)
{
	u8 num_frame=0;
	
	io_Init();
	uart1_debug_init(115200);
	usart3_failover_init(115200);  //ʧЧת�ƽӿ�
	usart2_screen_init(9600);  //485����ͨ��
	uart4_out_init(115200);			//�ⲿRS485�ӿ�
	usart6_meter_init(9600);		//���ӿ�
	canid_init();
	delay_ms(100);

	
#if CANTEST
//������Ϊ:42M/((6+7+1)*6)=500Kbps
		CAN1_Mode_Init(CAN_SJW_1tq,CAN_BS2_6tq,CAN_BS1_7tq,6,CAN_Mode_Normal);
#else
	//can2_dsp_Init(CAN_SJW_1tq,CAN_BS2_6tq,CAN_BS1_7tq,6,CAN_Mode_Normal);//������500Kbps   
#endif
  
	for(u8 i=0; i<3; i++) //����
	 {
	 		num_frame=can2_data_packet(FUNC_LINK,0, 0 ,0);
			Can2Send_MultiMessages(canid_tx, can2_tbuf, num_frame*8);	
			delay_ms(500); 
	 }


	


#if M24256	
	 AT24CXX_Init();	
	 delay_ms(500);	
	 AT24CXX_Check();
	 Variable_Eeprom();	
#endif	 
#if BQ32000	
	 bq32000_Init();
	 Set_CSU_time();//��һ���ϵ�ʱ�䲻�ԣ���Ҫ����
	 Get_CSU_time();
	 delay_ms(100);
#endif		 



#if SDCARD	
	u8 sd_init_cnt=0;
 	while(SD_Init())//��ⲻ��SD��
	{
		delay_ms(500);
		sd_init_cnt++;
		if(sd_init_cnt>10)
		break;
	}
	exfuns_init();							//Ϊfatfs��ر��������ڴ�				 
	f_mount(fs[0],"0:",1); 					//����SD��
	Write_Log();
#endif

#if WIFI

	usart5_wifi_init(115200);
	delay_ms(100);
	wifi_config(UART_WIFI);
#endif

	//IWDG_Init(6,2500); //
}
ErrorStatus Write_TFCard(void)
{
		FIL fil;
		u8 retSD = 0;
		ErrorStatus  flag = SUCCESS;
	
		char filename[5]={0};
		char str_zt[512] ={0};
		
		sprintf(filename,"Log");
		
		sprintf(str_zt,"TIME:20%02d%02d%02d %02d:%02d:%02d\r\nϵͳ״̬:%02d �¸�״̬:%02d IO״̬:%02d PTC״̬:%x PFC״̬:%x INV״̬:%x PFC�澯��:%x INV�澯��:%x\r\n���������ѹ:%5dV PFC��е���1:%5dA PFC��е���2:%5dA BUS�����ѹ:%5dV ��Ȧ����:%5dA ����е���:%5dA ��乤��Ƶ��:%5dKHz\r\n�����¶�:%3d�� �������¶�:%3d�� PFC���ʹ��¶�:%3d�� ��书�ʹ��¶�1:%3d�� ��书�ʹ��¶�2:%3d��\r\n",
		CSUtime.g_years,CSUtime.g_month,CSUtime.g_date,CSUtime.g_hours,CSUtime.g_minutes,CSUtime.g_seconds,
		System_State,warning_code,fault_code,PTCElectData.g_PTC_State,PTCElectData.g_pfc_State,PTCElectData.g_inv_State,PTCElectData.g_PFCAlarm_State,PTCElectData.g_INVAlarm_State,
		(PTCElectData.Rectifier_output_v/10),(PTCElectData.PFCInductor1_i/100),(PTCElectData.PFCInductor2_i/100),
		(PTCElectData.BUSoutput_v/10),(PTCElectData.Coil_i/100),(PTCElectData.INVInductor_i/100),(PTCElectData.INV_fre/100),
		PTCElectData.Temp_envir,PTCElectData.Temp_rectify,PTCElectData.Temp_pfc_mos,PTCElectData.Temp_INV_mos1,PTCElectData.Temp_INV_mos2);
 
		/*-1- �����ļ�ϵͳ*/
		retSD = f_mount(fs[0],"0:",1); 					//����SD�� 
		if(retSD)
		{
				flag = ERROR;
		}
		 
		/*-2-�����µ��ļ���д������*/
		retSD = f_open(&fil, filename, FA_OPEN_ALWAYS | FA_WRITE);		//���ļ���Ȩ�ް���������д�����û�и��ļ����ᴴ�����ļ���
		if(retSD)															//����ֵ��Ϊ0���������⣩
		{
				flag = ERROR;
		}

		/*-3- ƫ��ָ�뵽ĩβ��*/	   
		f_lseek(&fil,fil.fsize);

		/*-4- ��txt�ļ�β��д����*/	

		retSD = f_write(&fil, str_zt, strlen(str_zt), (void *)&bw);	//���ļ���д��wtext�ڵ�����	
		
		if(retSD)															//����ֵ��Ϊ0���������⣩
		{
				flag = ERROR;
		}
	
		/*-5- �ر�txt�ļ�*/
		retSD = f_close(&fil);												//�رո��ļ�
		if(retSD)															//����ֵ��Ϊ0���������⣩
		{
				flag = ERROR;
		}
		
		return flag;
}
ErrorStatus Write_Log(void)
{		
		ErrorStatus flag;
	
		Get_CSU_time();
		flag = Write_TFCard();

		return flag;
}		
#if M24256	
//��ʼ������
void Variable_Eeprom(void)
{
	eeprom_buffer_read((uint8 *)&First_Start,FIRST_START_FLAG_ADDR, sizeof(First_Start)); 
	if(First_Start == FIRST_START_FLAG){
		eeprom_buffer_read((uint8 *)&systemFixedData,SYSTEM_FIXED_ADDR, sizeof(systemFixedData)); 
	}else{
		First_Start = FIRST_START_FLAG;
		eeprom_buffer_write((uint8 *)&First_Start,FIRST_START_FLAG_ADDR, sizeof(First_Start)); 
		
		systemFixedData.uDeviceId = 0x01;
		strcpy((char*)systemFixedData.VSRSION, "20220905_H01_S01");
		eeprom_buffer_write((uint8 *)&systemFixedData,SYSTEM_FIXED_ADDR, sizeof(systemFixedData)); 
		
		delay_ms(10);

	}
}
#endif

static void poweron(void)
{
		u8 num_frame=0;
		u8 data_poweron[6]={0,0,0x12,0xd0,0x00,0x00};
		u8 i = 0;
		
	while((((PTCElectData.g_PTC_State >>8)&0x01) != 0x01)&&(i<10))
	{
			num_frame=can2_data_packet(FUNC_RCONTROL, APPEND_CODEL_START, 6 ,data_poweron);
			Can2Send_MultiMessages(canid_tx, can2_tbuf, num_frame*8);	
			delay_ms(100); 
			i++;
	} 
	if((PTCElectData.g_PTC_State >>8)&0x01)
	{
			SCREEN_485_TX;
			delay_ms(10);
			screen_ack(SCREEN_ACKON );
			delay_ms(500);
			SCREEN_485_RX;
			delay_ms(10);
			System_State = RUNNING;
	}
}	
	
static void poweroff(void)
{
		u8 num_frame=0;
		u8 data_powerff[6]={0,0,0x12,0xd0,0x00,0x00};
		u8 i = 0;
	
	while((((PTCElectData.g_PTC_State >>8)&0x02) != 0x02)&&(i<10))
	{
			num_frame=can2_data_packet(FUNC_RCONTROL,  APPEND_CODEL_STOP, 6 ,data_powerff);
			Can2Send_MultiMessages(canid_tx, can2_tbuf, num_frame*8);	
			delay_ms(100); 
			i++;
	} 
	if((PTCElectData.g_PTC_State >>8)&0x02)
	{
			SCREEN_485_TX;
			delay_ms(10);
			screen_ack(SCREEN_ACKOFF);
			delay_ms(10);
			SCREEN_485_RX;
		
		warning_code=ALARM_CODE_NULL;
	  fault_code=ERROR_CODE_NULL;
		System_State=STANDBY; 
	}

}	
static void waring_poweroff(void)
{
	u8 num_frame=0;
	u8 data_powerff[6]={0,0,0x12,0xd0,0x00,0x00};
	u8 i = 0;

	while((((PTCElectData.g_PTC_State >>8)&0x02) != 0x02)&&(i<10))
	{
			num_frame=can2_data_packet(FUNC_RCONTROL,  APPEND_CODEL_STOP, 6 ,data_powerff);
			Can2Send_MultiMessages(canid_tx, can2_tbuf, num_frame*8);	
			delay_ms(100); 
			i++;
	} 
}	
static void Set_ptcdata(void)
{
		u8 num_frame=0;
		u8 i = 0;
		u8 data[4]={
			SetPTCData.s_Bus_v>> 8u,
			SetPTCData.s_Bus_v& 0xff,
			SetPTCData.s_INV_fre>> 8u,
			SetPTCData.s_INV_fre& 0xff,
		};
		
		SetPTCData.s_set_statu = RESET;   //// 
		while((i<10)&&(SetPTCData.s_set_statu != SET))
		{
			num_frame=can2_data_packet(FUNC_CONFIG, 0, 21 ,data);
			Can2Send_MultiMessages(canid_tx, can2_tbuf, num_frame*8);		
			delay_ms(10);
			i++;
		}

		if(SetPTCData.s_set_statu == SET)
		{
			delay_ms(10);
			screen_ack(SCREEN_ACKCONFIG);
			delay_ms(12);
			screen_ack(SCREEN_ACKCONFIG);
			delay_ms(14);
			screen_ack(SCREEN_ACKCONFIG);
			delay_ms(16);
			screen_ack(SCREEN_ACKCONFIG);
			delay_ms(18);
			screen_ack(SCREEN_ACKCONFIG);
			delay_ms(20);
			screen_ack(SCREEN_ACKCONFIG);		
		}
			delay_ms(10);
			screen_ack(SCREEN_ACKCONFIG);
			delay_ms(12);
			screen_ack(SCREEN_ACKCONFIG);
			delay_ms(14);
			screen_ack(SCREEN_ACKCONFIG);
			delay_ms(16);
			screen_ack(SCREEN_ACKCONFIG);
			delay_ms(18);
			screen_ack(SCREEN_ACKCONFIG);
			delay_ms(20);
			screen_ack(SCREEN_ACKCONFIG);		
		
}	









