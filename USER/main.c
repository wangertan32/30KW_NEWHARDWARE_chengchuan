//TEXT
#include "main.h"
uint8 First_Start;//首次上电标志
SYSTEM_FIXED systemFixedData;//系统固定参数 主机ID和软硬件版本

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

OS_TMR 	tmr1;		//定时器1
void tmr1_callback(void *p_tmr, void *p_arg); 	//定时器1回调函数

OS_TMR 	tmr2;		//定时器1
void tmr2_callback(void *p_tmr, void *p_arg); 	//定时器2回调函数

static void sys_init(void);
static void poweron(void);
static void poweroff(void);
static void Set_ptcdata(void);
static void waring_poweroff(void);
int main(void)
{ 

	OS_ERR err;
	CPU_SR_ALLOC();

	delay_init(168);       	//延时初始化
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//中断分组配置
	
	mymem_init(SRAMIN);  	//初始化内部内存池
	mymem_init(SRAMEX);  	//初始化外部内存池
	mymem_init(SRAMCCM); 	//初始化CCM内存池
	
	sys_init();
	delay_ms(100);
	
	OSInit(&err); 					//UCOSIII初始化
#if ETHERNET	
u8 lwip_init_cnt=0;
u8 lwip_ctr_cnt=0;
  while(lwip_comm_init()) 		//lwip初始化
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
	
	while(udp_ctr_init()) 		//(创建udp_ctr线程)
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
	OS_CRITICAL_ENTER();//进入临界区
	//创建开始任务
	OSTaskCreate((OS_TCB 	* )&StartTaskTCB,		//任务控制块
				 (CPU_CHAR	* )"start task", 		//任务名字
                 (OS_TASK_PTR )start_task, 			//任务函数
                 (void		* )0,					//传递给任务函数的参数
                 (OS_PRIO	  )START_TASK_PRIO,     //任务优先级
                 (CPU_STK   * )&START_TASK_STK[0],	//任务堆栈基地址
                 (CPU_STK_SIZE)START_STK_SIZE/10,	//任务堆栈深度限位
                 (CPU_STK_SIZE)START_STK_SIZE,		//任务堆栈大小
                 (OS_MSG_QTY  )0,					//任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
                 (OS_TICK	  )0,					//当使能时间片轮转时的时间片长度，为0时为默认长度，
                 (void   	* )0,					//用户补充的存储区
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, //任务选项
                 (OS_ERR 	* )&err);				//存放该函数错误时的返回值
	OS_CRITICAL_EXIT();	//退出临界区

	OSStart(&err); 		//开启UCOS
}

//start任务
void start_task(void *p_arg)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	p_arg = p_arg;

	CPU_Init();
#if OS_CFG_STAT_TASK_EN > 0u
   OSStatTaskCPUUsageInit(&err);  	//统计任务                
#endif
	
#ifdef CPU_CFG_INT_DIS_MEAS_EN		//如果使能了测量中断关闭时间
    CPU_IntDisMeasMaxCurReset();	
#endif

#if	OS_CFG_SCHED_ROUND_ROBIN_EN  //当使用时间片轮转的时候
	 //使能时间片轮转调度功能,时间片长度为1个系统时钟节拍，既1*5=5ms
	OSSchedRoundRobinCfg(DEF_ENABLED,1,&err);  
#endif		
	
#if LWIP_DHCP
	lwip_comm_dhcp_creat(); //创建DHCP任务
#endif


//创建定时器1
	OSTmrCreate((OS_TMR		*)&tmr1,		//定时器1
                (CPU_CHAR	*)"tmr1",		//定时器名字
                (OS_TICK	 )0,			//
                (OS_TICK	 )100,          //tmr_rate 500ms
                (OS_OPT		 )OS_OPT_TMR_PERIODIC, //周期模式
                (OS_TMR_CALLBACK_PTR)tmr1_callback,//定时器1回调函数
								(void	    *)0,			//参数为0
                (OS_ERR	    *)&err);		//返回的错误码

	OSTmrCreate((OS_TMR		*)&tmr2,		//定时器2
                (CPU_CHAR	*)"tmr2",		//定时器名字
                (OS_TICK	 )0,			//
                (OS_TICK	 )300,          //tmr_rate 3s
                (OS_OPT		 )OS_OPT_TMR_PERIODIC, //周期模式
                (OS_TMR_CALLBACK_PTR)tmr2_callback,//定时器1回调函数
								(void	    *)0,			//参数为0
                (OS_ERR	    *)&err);		//返回的错误码
								
	OS_CRITICAL_ENTER();	//进入临界区  
	
	//创建CAN任务 16
	OSTaskCreate((OS_TCB 	* )&CanTaskTCB,		
				 (CPU_CHAR	* )"Can task", 		
                 (OS_TASK_PTR )can_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )CAN_TASK_PRIO,                  
                 (CPU_STK   * )&CAN_TASK_STK[0],	
                 (CPU_STK_SIZE)CAN_STK_SIZE/10,	
                 (CPU_STK_SIZE)CAN_STK_SIZE,		
                 (OS_MSG_QTY  )TASK_CAN_NUM,		//任务Msgdis_task需要使用内建消息队列，消息队列长度为4					
                 (OS_TICK	  )10,  					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);
	//创建scan任务		14												 	 			 
	OSTaskCreate((OS_TCB 	* )&ScanTaskTCB,		
				 (CPU_CHAR	* )"scan task", 		
                 (OS_TASK_PTR )scan_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )SCAN_TASK_PRIO,     
                 (CPU_STK   * )&SCAN_TASK_STK[0],	
                 (CPU_STK_SIZE)SCAN_STK_SIZE/10,	
                 (CPU_STK_SIZE)SCAN_STK_SIZE,		
                 (OS_MSG_QTY  )TASK_SCAN_NUM,		//任务Msgdis_task需要使用内建消息队列，消息队列长度为4					
                 (OS_TICK	  )10,  					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);
								 
	//创建handle任务		15												 	 			 
	OSTaskCreate((OS_TCB 	* )&HandleTaskTCB,		
				 (CPU_CHAR	* )"handle task", 		
                 (OS_TASK_PTR )handle_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )HANDLE_TASK_PRIO,     
                 (CPU_STK   * )&HANDLE_TASK_STK[0],	
                 (CPU_STK_SIZE)HANDLE_STK_SIZE/10,	
                 (CPU_STK_SIZE)HANDLE_STK_SIZE,		
                 (OS_MSG_QTY  )TASK_HANDLE_NUM,		//任务Msgdis_task需要使用内建消息队列，消息队列长度为4					
                 (OS_TICK	  )10,  					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);
								 
	//创建电表任务		12					 								 
	OSTaskCreate((OS_TCB 	* )&MeterTaskTCB,		
				 (CPU_CHAR	* )"meter task", 		 
                 (OS_TASK_PTR )meter_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )METER_TASK_PRIO,     
                 (CPU_STK   * )&METER_TASK_STK[0],	
                 (CPU_STK_SIZE)METER_STK_SIZE/10,	
                 (CPU_STK_SIZE)METER_STK_SIZE,		
                 (OS_MSG_QTY  )TASK_METER_NUM,		//任务Msgdis_task需要使用内建消息队列，消息队列长度为4					
                 (OS_TICK	  )10,  					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);	
								 
	//创建屏幕处理任务			17												 
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
	//创建远程接收任务    13  
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
							 
								 
		OS_TaskSuspend((OS_TCB*)&StartTaskTCB,&err);		//挂起开始任务	
					
		OS_CRITICAL_EXIT();	//进入临界区							 
								 
}

u8 test_buf[10]={1,2,3,4,5,6,7,8,9};
void screen_task(void *pdata)
{				
	u8 *p;
	OS_ERR err; 
	OS_MSG_SIZE size;
	u8 screen_txbuf[5]={0};
	OSTmrStart(&tmr1,&err);	//定时上传数据
	OSTmrStart(&tmr2,&err);	//启动CSU-PTC心跳包
	while(1)
	{
		//请求消息  接收屏的指令
		p=OSTaskQPend((OS_TICK		)0,
                      (OS_OPT		)OS_OPT_PEND_BLOCKING,
                      (OS_MSG_SIZE*	)&size,
                      (CPU_TS*		)0,
                      (OS_ERR*      )&err );		
		if(err == OS_ERR_NONE)
		{	
				if(p[0]==0x55&&p[1]==0x66)   ///// 屏-csu
				 {					 
				 		switch(p[4])
						{
							case SCREEN_POWERON:  /////// 开机指令
							
								SCREEN_485_TX;  // 
								delay_ms(10);
								screen_ack(SCREEN_ACKON );
								delay_ms(50);
								SCREEN_485_RX;
							
							  screen_txbuf[0]=MSG_CAN_POWERON;    //// 给can发送消息 开机
								OSTaskQPost(&CanTaskTCB,(void *)screen_txbuf,3,OS_OPT_POST_FIFO,&err);
								OSTimeDlyHMSM(0,0,0,50,OS_OPT_TIME_PERIODIC,&err);   //延时50ms
							
							break;
							
							case SCREEN_POWEROFF:   ///// 关机指令
//// 有故障下发关机指令				
							 SYSTEM_POWER_=POWER_OFF; 
								poweroff();

							
							break;
							
							case SCREEN_CONFIG:    ///// 配置命令
								
								SetPTCData.s_Bus_v = (getu16(p[7],p[6]))*10;    ////// 母线电压
								SetPTCData.s_INV_fre = (getu16(p[9],p[8]))*10;  ////  逆变工作频率
							
							screen_txbuf[0]=MSG_CAN_CONFIG_DATA;    //// 发送配置指令
								OSTaskQPost(&CanTaskTCB,(void *)screen_txbuf,3,OS_OPT_POST_FIFO,&err);
								OSTimeDlyHMSM(0,0,0,50,OS_OPT_TIME_PERIODIC,&err);   //延时50ms
							break;
							
							case SCREEN_SEEK:     ///// 运行命令 屏发送命令要数据 暂时没用到
									SCREEN_485_TX;
									delay_ms(10);
									screen_datasend(SCREEN_ACKDATA);   //////  csu上传数据给屏
									delay_ms(10);
									SCREEN_485_RX;	
							break;
							//测试其他485口
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
		IWDG_Feed();//喂狗
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
		//请求消息  等待消息队列
		p=OSTaskQPend((OS_TICK		)0,    /// 一直等待
                      (OS_OPT		)OS_OPT_PEND_BLOCKING,     //// 若没消息，则一直等到有消息
                      (OS_MSG_SIZE*	)&size,       ////用来表示接收到的消息长度(字节)
                      (CPU_TS*		)0,     ///// 无要求时间戳
                      (OS_ERR*      )&err ); ///// 调用此函数返回的错误码
		if(err == OS_ERR_NONE)
		{	
			  switch(p[0])
				{
					case MSG_CAN_LINK: //建链						
						num_frame=can2_data_packet(FUNC_LINK,0, 0 ,0);
						Can2Send_MultiMessages(canid_tx, can2_tbuf, num_frame*8);
					break;
					
					case MSG_CAN_SEARCH:   //后台查询
						num_frame=can2_data_packet(FUNC_SEEK, 0, 0 ,0);
						Can2Send_MultiMessages(canid_tx, can2_tbuf, num_frame*8);
					break;
										
					case MSG_CAN_POWERON:   //开
						 poweron();									
					break;

					case MSG_CAN_POWEROFF:   //关
						 poweroff();
					break;
					
					case MSG_CAN_CONFIG_NUM:   //配置机号
					{
						num_frame=can2_data_packet(FUNC_SETNUM, 0, 6 ,0);
						Can2Send_MultiMessages(canid_tx, can2_tbuf, num_frame*8);						
					}
					break;
					
					case MSG_CAN_CONFIG_DATA:   //配置参数
						Set_ptcdata();
					break;
					
					case MSG_CAN_CONFIG_SEARCH_DATA:   //查询参数值 100ms
							if(System_State == WARNING)
							{
//								waring_poweroff();
							}
							screen_datasend(SCREEN_ACKDATA);   //// 定时发送数据
					break;
									
					case MSG_CAN_CONFIG_SEARCH_STATE:   //查询状态值  300ms
					{
						if(System_State == STANDBY)     ////// 待机
						{
							data[0] = 0x01;
						}else if(System_State == RUNNING)    //// 运行
						{
							data[0] = 0x02;
						}else if(System_State == WARNING)     ////告警
						{
							data[0] = 0x04;
						}else if(System_State == SHUTDOWN)     ////  急停
						{
							data[0] = 0x08;
						}                                   /////  正常0xfc 1111 1100   //// 正常0x0f 0000 1111
						data[1] = 0;
						data[2] = (statio.all&0xff);
						data[3] = statio.io.emo;
						data[4] = 0x00;
//						data[3] = ((statio.all>>8)&0xff);
//						data[4] = ((statio.all>>16)&0xff);
						data[5] = 0x00;
						num_frame=can2_data_packet(FUNC_SEEKSTAT,0,14,data);        /// can打包数据
						Can2Send_MultiMessages(canid_tx, can2_tbuf, num_frame*8);   //// can发送数据 给ptc
						
						Write_Log();      //// 记录到sd
					}
					
					break;
					
					default:
					break;
				}
		}
		delay_ms(500);
		IWDG_Feed();//喂狗
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
		read_io();   ////// 读io状态
		
		if(SYSTEM_POWER_==POWER_ON)		 /// 开机命令
		{
		 screen_txbuf[0]=MSG_CAN_POWERON;    //// 给can发送消息 开机
		 OSTaskQPost(&CanTaskTCB,(void *)screen_txbuf,3,OS_OPT_POST_FIFO,&err);
		 OSTimeDlyHMSM(0,0,0,50,OS_OPT_TIME_PERIODIC,&err);   //延时50ms
			
		}	
		if(SYSTEM_POWER_==POWER_OFF)		 /// 关机命令
		{
		 screen_txbuf[0]=MSG_CAN_POWEROFF;    //// 给can发送消息 开机
		 OSTaskQPost(&CanTaskTCB,(void *)screen_txbuf,3,OS_OPT_POST_FIFO,&err);
		 OSTimeDlyHMSM(0,0,0,50,OS_OPT_TIME_PERIODIC,&err);   //延时50ms
			
		}	
		
		delay_ms(500);    ////// 
		IWDG_Feed();//喂狗
	}
}

void handle_task(void *pdata)
{				

	while(1)
	{
		handle_led();    ////// 控制灯
		handle_alarm();  ///// 暂无
		handle_fault();  ///// 暂无
		delay_ms(500);
		IWDG_Feed();//喂狗
	}
}

void plc_task(void *pdata)
{
				
	u8 *p;
	OS_ERR err; 
	OS_MSG_SIZE size;
	while(1)
	{
		//请求消息
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
		//请求消息
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
//定时器1的回调函数
void tmr1_callback(void *p_tmr, void *p_arg)
{
	OS_ERR err; 
	static u16 tmr1_num=0;
		
	tmr1_txbuf[0]=MSG_CAN_CONFIG_SEARCH_DATA;  //定时查询数据
	/////                    要发送的消息    消息字节数    发布在消息队列的最后
	OSTaskQPost(&CanTaskTCB,(void *)tmr1_txbuf,3,OS_OPT_POST_FIFO,&err);  ///// 发送一个消息给
	OSTimeDlyHMSM(0,0,0,50,OS_OPT_TIME_PERIODIC,&err);   //延时50ms
 	tmr1_num++;		//定时器1执行次数加1					
}


u8 tmr2_txbuf[3]={0};
//定时器2的回调函数
void tmr2_callback(void *p_tmr, void *p_arg)
{
	OS_ERR err; 
	static u16 tmr2_num=0;
	
	tmr2_txbuf[0]=MSG_CAN_CONFIG_SEARCH_STATE;  //定时查询状态
	OSTaskQPost(&CanTaskTCB,(void *)tmr2_txbuf,3,OS_OPT_POST_FIFO,&err);
	OSTimeDlyHMSM(0,0,0,50,OS_OPT_TIME_PERIODIC,&err);   //延时50ms
	
 	tmr2_num++;		//定时器1执行次数加1					
}

static void sys_init(void)
{
	u8 num_frame=0;
	
	io_Init();
	uart1_debug_init(115200);
	usart3_failover_init(115200);  //失效转移接口
	usart2_screen_init(9600);  //485威纶通屏
	uart4_out_init(115200);			//外部RS485接口
	usart6_meter_init(9600);		//电表接口
	canid_init();
	delay_ms(100);

	
#if CANTEST
//则波特率为:42M/((6+7+1)*6)=500Kbps
		CAN1_Mode_Init(CAN_SJW_1tq,CAN_BS2_6tq,CAN_BS1_7tq,6,CAN_Mode_Normal);
#else
	//can2_dsp_Init(CAN_SJW_1tq,CAN_BS2_6tq,CAN_BS1_7tq,6,CAN_Mode_Normal);//波特率500Kbps   
#endif
  
	for(u8 i=0; i<3; i++) //建链
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
	 Set_CSU_time();//第一次上电时间不对，需要设置
	 Get_CSU_time();
	 delay_ms(100);
#endif		 



#if SDCARD	
	u8 sd_init_cnt=0;
 	while(SD_Init())//检测不到SD卡
	{
		delay_ms(500);
		sd_init_cnt++;
		if(sd_init_cnt>10)
		break;
	}
	exfuns_init();							//为fatfs相关变量申请内存				 
	f_mount(fs[0],"0:",1); 					//挂载SD卡
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
		
		sprintf(str_zt,"TIME:20%02d%02d%02d %02d:%02d:%02d\r\n系统状态:%02d 温感状态:%02d IO状态:%02d PTC状态:%x PFC状态:%x INV状态:%x PFC告警量:%x INV告警量:%x\r\n整流输出电压:%5dV PFC电感电流1:%5dA PFC电感电流2:%5dA BUS输出电压:%5dV 线圈电流:%5dA 逆变电感电流:%5dA 逆变工作频率:%5dKHz\r\n环境温度:%3d℃ 整流器温度:%3d℃ PFC功率管温度:%3d℃ 逆变功率管温度1:%3d℃ 逆变功率管温度2:%3d℃\r\n",
		CSUtime.g_years,CSUtime.g_month,CSUtime.g_date,CSUtime.g_hours,CSUtime.g_minutes,CSUtime.g_seconds,
		System_State,warning_code,fault_code,PTCElectData.g_PTC_State,PTCElectData.g_pfc_State,PTCElectData.g_inv_State,PTCElectData.g_PFCAlarm_State,PTCElectData.g_INVAlarm_State,
		(PTCElectData.Rectifier_output_v/10),(PTCElectData.PFCInductor1_i/100),(PTCElectData.PFCInductor2_i/100),
		(PTCElectData.BUSoutput_v/10),(PTCElectData.Coil_i/100),(PTCElectData.INVInductor_i/100),(PTCElectData.INV_fre/100),
		PTCElectData.Temp_envir,PTCElectData.Temp_rectify,PTCElectData.Temp_pfc_mos,PTCElectData.Temp_INV_mos1,PTCElectData.Temp_INV_mos2);
 
		/*-1- 挂载文件系统*/
		retSD = f_mount(fs[0],"0:",1); 					//挂载SD卡 
		if(retSD)
		{
				flag = ERROR;
		}
		 
		/*-2-创建新的文件并写入数据*/
		retSD = f_open(&fil, filename, FA_OPEN_ALWAYS | FA_WRITE);		//打开文件，权限包括创建、写（如果没有该文件，会创建该文件）
		if(retSD)															//返回值不为0（出现问题）
		{
				flag = ERROR;
		}

		/*-3- 偏移指针到末尾处*/	   
		f_lseek(&fil,fil.fsize);

		/*-4- 在txt文件尾续写数据*/	

		retSD = f_write(&fil, str_zt, strlen(str_zt), (void *)&bw);	//在文件内写入wtext内的内容	
		
		if(retSD)															//返回值不为0（出现问题）
		{
				flag = ERROR;
		}
	
		/*-5- 关闭txt文件*/
		retSD = f_close(&fil);												//关闭该文件
		if(retSD)															//返回值不为0（出现问题）
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
//初始化变量
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









