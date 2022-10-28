

#include "usart.h"
#include "can.h"
#include "alarm.h"

////////////////////////////////////////////////////////////////////////////////// 	 
//如果使用ucos,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos 使用	  
#endif


u8 check_sum(void *si, u8 count);
void screen_cmd_analysis(u8* buf);

u8 usart2_tx_buf[USART2_TX_LEN];
u8 usart2_rx_buf[USART2_RX_LEN];

u8 usart3_tx_buf[USART3_TX_LEN];
u8 usart3_rx_buf[USART3_RX_LEN];

u8 uart4_tx_buf[UART4_TX_LEN];
u8 uart4_rx_buf[UART4_RX_LEN];

u8 usart5_tx_buf[USART5_TX_LEN];
u8 usart5_rx_buf[USART5_RX_LEN];

u8 usart6_tx_buf[USART6_TX_LEN];
u8 usart6_rx_buf[USART6_RX_LEN];

//////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{ 	
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
	USART1->DR = (u8) ch;      
	return ch;
}
#endif
 
void uart1_debug_init(u32 bound)
{
   //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //使能GPIOA时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//使能USART1时钟
 
	//串口1对应引脚复用映射
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1); //GPIOA9复用为USART1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1); //GPIOA10复用为USART1
	
	//USART1端口配置
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; //GPIOA9与GPIOA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOA,&GPIO_InitStructure); //初始化PA9，PA10

   //USART1 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
  USART_Init(USART1, &USART_InitStructure); //初始化串口1
	
  USART_Cmd(USART1, ENABLE);  //使能串口1 
	
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启相关中断

	//Usart1 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//串口1中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器、
	
}


void usart2_screen_init(u32 bound)
{
	//GPIO端口设置
	GPIO_InitTypeDef  GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef  NVIC_InitStructure;
	DMA_InitTypeDef   DMA_InitStructure;
 
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE); 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1,ENABLE);
 
	
	//引脚复用映射
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource5,GPIO_AF_USART2); //GPIOD5复用为USART2
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource6,GPIO_AF_USART2); //GPIOD6复用为USART2
 
	//USART2端口配置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 |GPIO_Pin_7; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOD,&GPIO_InitStructure); 
	 //485控制
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	 
	//USART2 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
	USART_Init(USART2, &USART_InitStructure); //初始化串口2
 
	USART_Cmd(USART2, ENABLE);  //使能串口2
 
	USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);//开启相关中断
 
	//Usart2 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;//串口2中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器、
 
	USART_DMACmd(USART2,USART_DMAReq_Tx,ENABLE);  //使能串口2的DMA发送   
	USART_DMACmd(USART2,USART_DMAReq_Rx,ENABLE);  //使能串口2的DMA接收
 
	//****************************配置UART2发送
	DMA_DeInit(DMA1_Stream6);
	while (DMA_GetCmdStatus(DMA1_Stream6) != DISABLE);//等待DMA可配置 
	/* 配置 DMA Stream */
	DMA_InitStructure.DMA_Channel = DMA_Channel_4;  //通道选择
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&USART2->DR;//DMA外设地址
	DMA_InitStructure.DMA_Memory0BaseAddr = (u32)usart2_tx_buf;//DMA 存储器0地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;//存储器到外设模式
	DMA_InitStructure.DMA_BufferSize = USART2_TX_LEN;//数据传输量 
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//外设非增量模式
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//存储器增量模式
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;//外设数据长度:8位
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;//存储器数据长度:8位
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;// 使用普通模式 
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;//中等优先级
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;//存储器突发单次传输
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;//外设突发单次传输
	DMA_Init(DMA1_Stream6, &DMA_InitStructure);//初始化DMA Stream
 
	//****************************配置UART2接收
	DMA_DeInit(DMA1_Stream5);
	while (DMA_GetCmdStatus(DMA1_Stream5) != DISABLE);//等待DMA可配置 
	/* 配置 DMA Stream */
	DMA_InitStructure.DMA_Channel = DMA_Channel_4;  //通道选择
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&USART2->DR;//DMA外设地址
	DMA_InitStructure.DMA_Memory0BaseAddr = (u32)usart2_rx_buf;//DMA 存储器0地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory ;//外设到存储器模式
	DMA_InitStructure.DMA_BufferSize = USART2_RX_LEN;//数据传输量 
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//外设非增量模式
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//存储器增量模式
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;//外设数据长度:8位
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;//存储器数据长度:8位
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;// 使用普通模式 
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;//中等优先级
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;//存储器突发单次传输
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;//外设突发单次传输
	DMA_Init(DMA1_Stream5, &DMA_InitStructure);//初始化DMA Stream
        
	DMA_Cmd(DMA1_Stream5, ENABLE);  //开启DMA传输 
	SCREEN_485_RX;   ////

}


//显示屏数据接收中断
u8 usart2_buf[20];
void USART2_IRQHandler(void)
{
  u16 num;
	OS_ERR err;
	OSIntEnter();    
  if(USART_GetITStatus(USART2,USART_IT_IDLE) != RESET)
  {
    DMA_Cmd(DMA1_Stream5, DISABLE); //关闭DMA,防止处理其间有数据
    num = USART2->SR;
    num = USART2->DR;
    num =USART2_RX_LEN - DMA_GetCurrDataCounter(DMA1_Stream5);
    if(num !=0)  
		{
			memcpy(usart2_buf,usart2_rx_buf,num); 
			if(num==8 || num==15)//依据长度判断
			{
				OSTaskQPost(&ScreenTaskTCB,(void *)usart2_buf,num,OS_OPT_POST_FIFO,&err);				
			}
	  }
		
    DMA_ClearFlag(DMA1_Stream5,DMA_FLAG_TCIF5 | DMA_FLAG_FEIF5 | DMA_FLAG_DMEIF5 | DMA_FLAG_TEIF5 | DMA_FLAG_HTIF5);
    DMA_SetCurrDataCounter(DMA1_Stream5, USART2_RX_LEN);
    DMA_Cmd(DMA1_Stream5, ENABLE);     //打开DMA
		OSIntExit();    	//退出中断
  }
}


void usart3_failover_init(u32 bound)
{
	//GPIO端口设置
	GPIO_InitTypeDef  GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef  NVIC_InitStructure;
	DMA_InitTypeDef   DMA_InitStructure;
 
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE); //使能GPIOA时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);//使能USART1时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1,ENABLE);//DMA2时钟使能 
 
	//串口1对应引脚复用映射
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource10,GPIO_AF_USART3); //GPIOA9复用为USART1
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource11,GPIO_AF_USART3); //GPIOA10复用为USART1
 
	//USART1端口配置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11; //GPIOA9与GPIOA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOB,&GPIO_InitStructure); //初始化PA9，PA10
 	 //485控制
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	//USART1 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
	USART_Init(USART3, &USART_InitStructure); //初始化串口1
 
	USART_Cmd(USART3, ENABLE);  //使能串口1 
 
	//USART_ClearFlag(USART1, USART_FLAG_TC);
	USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);//开启相关中断
 
	//Usart1 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;//串口1中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器、
 
	USART_DMACmd(USART3,USART_DMAReq_Tx,ENABLE);  //使能串口1的DMA发送   
	USART_DMACmd(USART3,USART_DMAReq_Rx,ENABLE);  //使能串口1的DMA接收
 
	//****************************配置UART3发送
	DMA_DeInit(DMA1_Stream3);
	while (DMA_GetCmdStatus(DMA1_Stream3) != DISABLE);//等待DMA可配置 
	/* 配置 DMA Stream */
	DMA_InitStructure.DMA_Channel = DMA_Channel_4;  //通道选择
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&USART3->DR;//DMA外设地址
	DMA_InitStructure.DMA_Memory0BaseAddr = (u32)usart3_tx_buf;//DMA 存储器0地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;//存储器到外设模式
	DMA_InitStructure.DMA_BufferSize = USART3_TX_LEN;//数据传输量 
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//外设非增量模式
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//存储器增量模式
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;//外设数据长度:8位
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;//存储器数据长度:8位
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;// 使用普通模式 
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;//中等优先级
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;//存储器突发单次传输
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;//外设突发单次传输
	DMA_Init(DMA1_Stream3, &DMA_InitStructure);//初始化DMA Stream
 
	//****************************配置UART1接收
	DMA_DeInit(DMA1_Stream1);
	while (DMA_GetCmdStatus(DMA1_Stream1) != DISABLE);//等待DMA可配置 
	/* 配置 DMA Stream */
	DMA_InitStructure.DMA_Channel = DMA_Channel_4;  //通道选择
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&USART3->DR;//DMA外设地址
	DMA_InitStructure.DMA_Memory0BaseAddr = (u32)usart3_rx_buf;//DMA 存储器0地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory ;//外设到存储器模式
	DMA_InitStructure.DMA_BufferSize = USART3_RX_LEN;//数据传输量 
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//外设非增量模式
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//存储器增量模式
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;//外设数据长度:8位
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;//存储器数据长度:8位
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;// 使用普通模式 
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;//中等优先级
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;//存储器突发单次传输
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;//外设突发单次传输
	DMA_Init(DMA1_Stream1, &DMA_InitStructure);//初始化DMA Stream
        
	DMA_Cmd(DMA1_Stream1, ENABLE);  //开启DMA传输 	
	
  FAILOVER_485_RX;   /////

}

u8 usart3_buf[256];
void USART3_IRQHandler(void)
{
  u16 num;
	OS_ERR err;
	OSIntEnter();    
  if(USART_GetITStatus(USART3,USART_IT_IDLE) != RESET)
  {
    DMA_Cmd(DMA1_Stream1, DISABLE); //关闭DMA,防止处理其间有数据
    num = USART3->SR;
    num = USART3->DR;
    num =USART3_RX_LEN - DMA_GetCurrDataCounter(DMA1_Stream1);
    if(num !=0)  
		{
			memcpy(usart3_buf,usart3_rx_buf,num); 
			if(num==8)
			{
				OSTaskQPost(&ScreenTaskTCB,(void *)usart3_buf,num,OS_OPT_POST_FIFO,&err);				
			}
	  }
    DMA_ClearFlag(DMA1_Stream1,DMA_FLAG_TCIF1 | DMA_FLAG_FEIF1 | DMA_FLAG_DMEIF1 | DMA_FLAG_TEIF1 | DMA_FLAG_HTIF1);//清除DMA2_Steam7传输完成标志
    DMA_SetCurrDataCounter(DMA1_Stream1, USART3_RX_LEN);
    DMA_Cmd(DMA1_Stream1, ENABLE);     //打开DMA
		OSIntExit();    	//退出中断
  }
}



void uart4_out_init(u32 bound)
{
	//GPIO端口设置
	GPIO_InitTypeDef  GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef  NVIC_InitStructure;
	DMA_InitTypeDef   DMA_InitStructure;
 
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //使能GPIOA时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE);//使能USART1时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1,ENABLE);//DMA1时钟使能 
 
	//串口1对应引脚复用映射
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource0,GPIO_AF_UART4); //PA0  TX
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource1,GPIO_AF_UART4); //PA1  RX
 
	//USART1端口配置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 |GPIO_Pin_1; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOA,&GPIO_InitStructure); //初始化PA9，PA10
	
  //485控制
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	//USART1 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
	USART_Init(UART4, &USART_InitStructure); //初始化串口4
 
	USART_Cmd(UART4,ENABLE);  //使能串口4
	USART_ITConfig(UART4, USART_IT_IDLE, ENABLE);//开启相关中断
// 
//	//Usart5 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;//串口5中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=4;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器、
 
	USART_DMACmd(UART4,USART_DMAReq_Tx,ENABLE);  //使能串口5的DMA发送   
	USART_DMACmd(UART4,USART_DMAReq_Rx,ENABLE);  //使能串口5的DMA接收
 	
	//****************************配置UART1发送
	DMA_DeInit(DMA1_Stream4);
	while (DMA_GetCmdStatus(DMA1_Stream4) != DISABLE);//等待DMA可配置 
	/* 配置 DMA Stream */
	DMA_InitStructure.DMA_Channel = DMA_Channel_4;  //通道选择
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&UART4->DR;//DMA外设地址
	DMA_InitStructure.DMA_Memory0BaseAddr = (u32)uart4_tx_buf;//DMA 存储器0地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;//存储器到外设模式
	DMA_InitStructure.DMA_BufferSize = UART4_TX_LEN;//数据传输量 
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//外设非增量模式
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//存储器增量模式
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;//外设数据长度:8位
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;//存储器数据长度:8位
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;// 使用普通模式 
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;//中等优先级
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;//存储器突发单次传输
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;//外设突发单次传输
	DMA_Init(DMA1_Stream4, &DMA_InitStructure);//初始化DMA Stream
        
	//****************************配置UART1接收
	DMA_DeInit(DMA1_Stream2);
	while (DMA_GetCmdStatus(DMA1_Stream2) != DISABLE);//等待DMA可配置 
	/* 配置 DMA Stream */
	DMA_InitStructure.DMA_Channel = DMA_Channel_4;  //通道选择
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&UART4->DR;//DMA外设地址
	DMA_InitStructure.DMA_Memory0BaseAddr = (u32)uart4_rx_buf;//DMA 存储器0地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory ;//外设到存储器模式
	DMA_InitStructure.DMA_BufferSize = UART4_RX_LEN;//数据传输量 
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//外设非增量模式
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//存储器增量模式
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;//外设数据长度:8位
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;//存储器数据长度:8位
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;// 使用普通模式 
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;//中等优先级
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;//存储器突发单次传输
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;//外设突发单次传输
	DMA_Init(DMA1_Stream2, &DMA_InitStructure);//初始化DMA Stream
        
	DMA_Cmd(DMA1_Stream2, ENABLE);  //开启DMA传输 
		
	OUT_485_RX;       ////////
}

u8 uart4_buf[20];
void UART4_IRQHandler(void)
{
  u16 num;
	OS_ERR err;
	OSIntEnter();    
  if(USART_GetITStatus(UART4,USART_IT_IDLE) != RESET)
  {
    DMA_Cmd(DMA1_Stream2, DISABLE); //关闭DMA,防止处理其间有数据
    num = UART4->SR;
    num = UART4->DR;
    num =UART4_RX_LEN - DMA_GetCurrDataCounter(DMA1_Stream2);
    if(num !=0)  
		{
			memcpy(uart4_buf,uart4_rx_buf,num); 
			if(num==8)
			{
				OSTaskQPost(&ScreenTaskTCB,(void *)uart4_buf,num,OS_OPT_POST_FIFO,&err);				
			}
	  }
    DMA_ClearFlag(DMA1_Stream2,DMA_FLAG_TCIF2 | DMA_FLAG_FEIF2 | DMA_FLAG_DMEIF2 | DMA_FLAG_TEIF2 | DMA_FLAG_HTIF2);
    DMA_SetCurrDataCounter(DMA1_Stream2, UART4_RX_LEN);
    DMA_Cmd(DMA1_Stream2, ENABLE);     //打开DMA
		OSIntExit();    	//退出中断
  }
}

void usart5_wifi_init(u32 bound)
{
	//GPIO端口设置
	GPIO_InitTypeDef  GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef  NVIC_InitStructure;
	DMA_InitTypeDef   DMA_InitStructure;
 
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE); //使能GPIOA时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE); //使能GPIOA时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5,ENABLE);//使能USART1时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1,ENABLE);//DMA2时钟使能 
 
	//串口1对应引脚复用映射
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource12,GPIO_AF_UART5); //GPIOC12复用为UART5  TX
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource2,GPIO_AF_UART5); //GPIOA10复用为UART5  RX
 
	//USART1端口配置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12; //GPIOA9与GPIOA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOC,&GPIO_InitStructure); //初始化PA9，PA10
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //GPIOA9与GPIOA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOD,&GPIO_InitStructure); //初始化PA9，PA10
 
	//USART1 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
	USART_Init(UART5, &USART_InitStructure); //初始化串口1
 
	USART_Cmd(UART5,ENABLE);  //使能串口5 
	USART_ITConfig(UART5, USART_IT_IDLE, ENABLE);//开启相关中断
// 
//	//Usart5 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;//串口5中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器、
 
	USART_DMACmd(UART5,USART_DMAReq_Tx,ENABLE);  //使能串口5的DMA发送   
	USART_DMACmd(UART5,USART_DMAReq_Rx,ENABLE);  //使能串口5的DMA接收
 	
	//****************************配置UART1发送
	DMA_DeInit(DMA1_Stream7);
	while (DMA_GetCmdStatus(DMA1_Stream7) != DISABLE);//等待DMA可配置 
	/* 配置 DMA Stream */
	DMA_InitStructure.DMA_Channel = DMA_Channel_4;  //通道选择
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&UART5->DR;//DMA外设地址
	DMA_InitStructure.DMA_Memory0BaseAddr = (u32)usart5_tx_buf;//DMA 存储器0地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;//存储器到外设模式
	DMA_InitStructure.DMA_BufferSize = USART5_TX_LEN;//数据传输量 
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//外设非增量模式
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//存储器增量模式
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;//外设数据长度:8位
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;//存储器数据长度:8位
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;// 使用普通模式 
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;//中等优先级
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;//存储器突发单次传输
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;//外设突发单次传输
	DMA_Init(DMA1_Stream7, &DMA_InitStructure);//初始化DMA Stream
        
	//****************************配置UART1接收
	DMA_DeInit(DMA1_Stream0);
	while (DMA_GetCmdStatus(DMA1_Stream0) != DISABLE);//等待DMA可配置 
	/* 配置 DMA Stream */
	DMA_InitStructure.DMA_Channel = DMA_Channel_4;  //通道选择
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&UART5->DR;//DMA外设地址
	DMA_InitStructure.DMA_Memory0BaseAddr = (u32)usart5_rx_buf;//DMA 存储器0地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory ;//外设到存储器模式
	DMA_InitStructure.DMA_BufferSize = USART5_RX_LEN;//数据传输量 
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//外设非增量模式
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//存储器增量模式
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;//外设数据长度:8位
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;//存储器数据长度:8位
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;// 使用普通模式 
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;//中等优先级
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;//存储器突发单次传输
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;//外设突发单次传输
	DMA_Init(DMA1_Stream0, &DMA_InitStructure);//初始化DMA Stream
        
	DMA_Cmd(DMA1_Stream0, ENABLE);  //开启DMA传输 
	//DMA_ITConfig(DMA2_Stream5,DMA_IT_TC,ENABLE);
}

u8 usart5_buf[20];
void UART5_IRQHandler(void)
{
  u16 num;
	OS_ERR err;
	OSIntEnter();    
  if(USART_GetITStatus(UART5,USART_IT_IDLE) != RESET)
  {
    DMA_Cmd(DMA1_Stream0, DISABLE); //关闭DMA,防止处理其间有数据
    num = UART5->SR;
    num = UART5->DR;
    num =USART5_RX_LEN - DMA_GetCurrDataCounter(DMA1_Stream0);
    if(num !=0)  
		{
			memcpy(usart5_buf,usart5_rx_buf,num); 
			if(num==8)
			{
				OSTaskQPost(&ScreenTaskTCB,(void *)usart5_buf,num,OS_OPT_POST_FIFO,&err);				
			}
	  }
    DMA_ClearFlag(DMA1_Stream0,DMA_FLAG_TCIF0 | DMA_FLAG_FEIF0 | DMA_FLAG_DMEIF0 | DMA_FLAG_TEIF0 | DMA_FLAG_HTIF0);
    DMA_SetCurrDataCounter(DMA1_Stream0, USART5_RX_LEN);
    DMA_Cmd(DMA1_Stream0, ENABLE);     //打开DMA
		OSIntExit();    	//退出中断
  }
}


void usart6_meter_init(u32 bound)
{
	//GPIO端口设置
	GPIO_InitTypeDef  GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef  NVIC_InitStructure;
	DMA_InitTypeDef   DMA_InitStructure;
 
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE); 
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG,ENABLE); 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE);
 
	//引脚复用映射
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource6,GPIO_AF_USART6); //GPIOC6复用为USART6
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource7,GPIO_AF_USART6); //GPIOC7复用为USART6
 
	//USART6端口配置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOC,&GPIO_InitStructure); 
 
  //485控制
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_Init(GPIOG, &GPIO_InitStructure);
 
	//USART6 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
	USART_Init(USART6, &USART_InitStructure); //初始化串口2
 
	USART_Cmd(USART6, ENABLE);  //使能串口6
 
	USART_ITConfig(USART6, USART_IT_IDLE, ENABLE);//开启相关中断
 
	//Usart6 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn;//串口6中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=4;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器、
 
	USART_DMACmd(USART6,USART_DMAReq_Tx,ENABLE);  //使能串口6的DMA发送   
	USART_DMACmd(USART6,USART_DMAReq_Rx,ENABLE);  //使能串口6的DMA接收
 
	//****************************配置UART6发送
	DMA_DeInit(DMA2_Stream6);
	while (DMA_GetCmdStatus(DMA2_Stream6) != DISABLE);//等待DMA可配置 
	/* 配置 DMA Stream */
	DMA_InitStructure.DMA_Channel = DMA_Channel_5;  //通道选择
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&USART6->DR;//DMA外设地址
	DMA_InitStructure.DMA_Memory0BaseAddr = (u32)usart6_tx_buf;//DMA 存储器0地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;//存储器到外设模式
	DMA_InitStructure.DMA_BufferSize = USART6_TX_LEN;//数据传输量 
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//外设非增量模式
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//存储器增量模式
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;//外设数据长度:8位
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;//存储器数据长度:8位
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;// 使用普通模式 
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;//中等优先级
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;//存储器突发单次传输
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;//外设突发单次传输
	DMA_Init(DMA2_Stream6, &DMA_InitStructure);//初始化DMA Stream
 
	//****************************配置UART6接收
	DMA_DeInit(DMA2_Stream1);
	while (DMA_GetCmdStatus(DMA2_Stream1) != DISABLE);//等待DMA可配置 
	/* 配置 DMA Stream */
	DMA_InitStructure.DMA_Channel = DMA_Channel_5;  //通道选择
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&USART6->DR;//DMA外设地址
	DMA_InitStructure.DMA_Memory0BaseAddr = (u32)usart6_rx_buf;//DMA 存储器0地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory ;//外设到存储器模式
	DMA_InitStructure.DMA_BufferSize = USART6_RX_LEN;//数据传输量 
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//外设非增量模式
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//存储器增量模式
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;//外设数据长度:8位
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;//存储器数据长度:8位
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;// 使用普通模式 
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;//中等优先级
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;//存储器突发单次传输
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;//外设突发单次传输
	DMA_Init(DMA2_Stream1, &DMA_InitStructure);//初始化DMA Stream
        
	DMA_Cmd(DMA2_Stream1, ENABLE);  //开启DMA传输 

	METER_485_RX;    //// 
	
}

//电表数据接收中断
u8 usart6_buf[20];
void USART6_IRQHandler(void)
{		 
  u16 num;
	OS_ERR err;
	OSIntEnter();    
  if(USART_GetITStatus(USART6,USART_IT_IDLE) != RESET)
  {
    DMA_Cmd(DMA2_Stream1, DISABLE); //关闭DMA,防止处理其间有数据
    num = USART6->SR;
    num = USART6->DR;
    num =USART6_RX_LEN - DMA_GetCurrDataCounter(DMA2_Stream1);
    if(num !=0)  
		{
			memcpy(usart6_buf,usart6_rx_buf,num); 
			if(num==8)
			{
				OSTaskQPost(&ScreenTaskTCB,(void *)usart6_buf,num,OS_OPT_POST_FIFO,&err);				
			}
	  }
		
    DMA_ClearFlag(DMA2_Stream1,DMA_FLAG_TCIF1 | DMA_FLAG_FEIF1 | DMA_FLAG_DMEIF1 | DMA_FLAG_TEIF1 | DMA_FLAG_HTIF1);
    DMA_SetCurrDataCounter(DMA2_Stream1, USART6_RX_LEN);
    DMA_Cmd(DMA2_Stream1, ENABLE);     //打开DMA
		OSIntExit();    	//退出中断
  }
}

void uart_send_dma(u8 usart_periph, u8* send_buf, u16 send_len)
{
			if(usart_periph==UART_SCREEN)  //UART2
			 {
					DMA_Cmd(DMA1_Stream6, DISABLE);                      //关闭DMA传输 
					while (DMA_GetCmdStatus(DMA1_Stream6) != DISABLE){}	//确保DMA可以被设置  
					DMA_ClearFlag(DMA1_Stream6,DMA_FLAG_TCIF6);
					DMA1_Stream6->M0AR =(u32)send_buf;
					DMA1_Stream6->NDTR = send_len; //数据传输量       
					DMA_Cmd(DMA1_Stream6, ENABLE);   //开启DMA传输  
					while(DMA_GetFlagStatus(DMA1_Stream6,DMA_FLAG_TCIF6)!=RESET)
					{
						DMA_ClearFlag(DMA1_Stream6,DMA_FLAG_TCIF6);
					}//等待DMA1_Stream6传输完成	
			 }
			else if(usart_periph==UART_WIFI)  //UART5
			 {
					DMA_Cmd(DMA1_Stream7, DISABLE);                      
					while (DMA_GetCmdStatus(DMA1_Stream7) != DISABLE){}	
					DMA_ClearFlag(DMA1_Stream7,DMA_FLAG_TCIF7);
					DMA1_Stream7->M0AR =(u32)send_buf;
					DMA1_Stream7->NDTR = send_len;     
					DMA_Cmd(DMA1_Stream7, ENABLE);
					while(DMA_GetFlagStatus(DMA1_Stream7,DMA_FLAG_TCIF7)!=RESET)
					{
						DMA_ClearFlag(DMA1_Stream7,DMA_FLAG_TCIF7);
					}//等待DMA1_Stream7传输完成							
			 }
			else if(usart_periph==UART_METER)  //UART6
			 {
					DMA_Cmd(DMA2_Stream6, DISABLE);                      
					while (DMA_GetCmdStatus(DMA2_Stream6) != DISABLE){}	 
					DMA_ClearFlag(DMA2_Stream6,DMA_FLAG_TCIF6);
					DMA2_Stream6->M0AR =(u32)send_buf;
					DMA2_Stream6->NDTR = send_len;    
					DMA_Cmd(DMA2_Stream6, ENABLE);
					while(DMA_GetFlagStatus(DMA2_Stream6,DMA_FLAG_TCIF6)!=RESET)
					{
						DMA_ClearFlag(DMA2_Stream6,DMA_FLAG_TCIF6);
					}//等待DMA2_Stream6传输完成							
			 }
			else if(usart_periph==UART_FAILOVER)  //UART3
			 {
					DMA_Cmd(DMA1_Stream3, DISABLE);                      //关闭DMA传输 
					while (DMA_GetCmdStatus(DMA1_Stream3) != DISABLE){}	//确保DMA可以被设置  
					DMA_ClearFlag(DMA1_Stream3,DMA_FLAG_TCIF3);
					DMA1_Stream3->M0AR =(u32)send_buf;
					DMA1_Stream3->NDTR = send_len; //数据传输量       
					DMA_Cmd(DMA1_Stream3, ENABLE);   //开启DMA传输
					while(DMA_GetFlagStatus(DMA1_Stream3,DMA_FLAG_TCIF3)!=RESET)
					{
						DMA_ClearFlag(DMA1_Stream3,DMA_FLAG_TCIF3);
					}//等待DMA1_Stream3传输完成								
			 } 
			else if(usart_periph==UART_OUT)  //UART4
			 {
					DMA_Cmd(DMA1_Stream4, DISABLE);                      //关闭DMA传输 
					while (DMA_GetCmdStatus(DMA1_Stream4) != DISABLE){}	//确保DMA可以被设置  
					DMA_ClearFlag(DMA1_Stream4,DMA_FLAG_TCIF4);
					DMA1_Stream4->M0AR =(u32)send_buf;
					DMA1_Stream4->NDTR = send_len; //数据传输量       
					DMA_Cmd(DMA1_Stream4, ENABLE);   //开启DMA传输
					while(DMA_GetFlagStatus(DMA1_Stream4,DMA_FLAG_TCIF4)!=RESET)
					{
						DMA_ClearFlag(DMA1_Stream4,DMA_FLAG_TCIF4);
					}//等待DMA1_Stream4传输完成								
			 } 			 
}


void uart_send(u8 usart_periph, u8 *addr, u8 len)
{
	 if(usart_periph==UART_WIFI)
	 { 
	   while( (len--)!=0 )  
	    {            
            while (RESET == USART_GetFlagStatus(UART5, USART_FLAG_TC));
							USART_SendData(UART5, *addr++);  
	    }
	 }
	if(usart_periph==UART_METER)
	 { 
	   while( (len--)!=0 )  
	    {            
            while (RESET == USART_GetFlagStatus(USART6, USART_FLAG_TC));
							USART_SendData(USART6, *addr++);  
	    }
	 } 
	 
}
///////////////////////// msc        6        0
static void AppendTwoBytes(u8 *pBuf, u16 *pPos, u16 data)
{

    pBuf[*pPos] = (u8)((data >> 8) & 0xFF);
    pBuf[*pPos+1] = (u8)(data & 0xFF);
    *pPos += 2;
}


int HexToAscii(unsigned char *pHexStr,unsigned char *pAscStr,int Len)
{
	char Nibble[2];
	unsigned char Buffer[256];
	int i = 0;
	int j = 0;

	for(i=0;i<Len;i++)
	{
		Nibble[0]=pHexStr[i] >> 4 & 0X0F;
		Nibble[1]=pHexStr[i] & 0x0F;
		for(j=0;j<2;j++)
		{
			if(Nibble[j]<10)
			{
				Nibble[j]=Nibble[j]+0x30;
			}
			else if(Nibble[j]<16)
			{
				Nibble[j]=Nibble[j]-10+'A';
			}
			else
			{
				return 0;
			}
		}
		memcpy(Buffer+i*2,Nibble,2);
	}
	Buffer[2*Len]=0x00;
	memcpy(pAscStr,Buffer,2*Len);
	pAscStr[2*Len]=0x00;
	return 1;
}

u8 mcgs_buf[256] = {SCREEN_SOF1,SCREEN_SOF2,ADDR_CSU,ADDR_SCREEN,0};
u8 sendbuff[256]={0};
// 0x22  运行
void screen_datasend(u8 order)
{	
//测试变量
	static u8 flag_tri=0;
//
	u16 len=6, size=0;
	memset(mcgs_buf,0,sizeof(mcgs_buf));
	memset(sendbuff,0,sizeof(sendbuff));
	mcgs_buf[0]= SCREEN_SOF1;
	mcgs_buf[1]= SCREEN_SOF2;
	mcgs_buf[2]= ADDR_CSU;
	mcgs_buf[3]= ADDR_SCREEN;
	mcgs_buf[4]= order;
	
	AppendTwoBytes(mcgs_buf, &len, System_State);	   ///系统状态 待机 运行 故障 告警 急停
	AppendTwoBytes(mcgs_buf, &len, warning_code);    ///警告信息 温度 风扇 烟感等
	AppendTwoBytes(mcgs_buf, &len, fault_code);      ///错误信息  功率开关 急停等
	
	
//////// 输入假数据
//	AppendTwoBytes(mcgs_buf, &len, System_State);	
//	if(flag_tri==0)
//	{
//	   AppendTwoBytes(mcgs_buf, &len, 3);
//		flag_tri=1;
//	}
//	else{
//		   AppendTwoBytes(mcgs_buf, &len, 4);	
//		flag_tri=0;
//	}
//	AppendTwoBytes(mcgs_buf, &len, fault_code);
   
////////////
	AppendTwoBytes(mcgs_buf, &len, PTCElectData.Input_v);    //输入电压
	AppendTwoBytes(mcgs_buf, &len, PTCElectData.Input_i/10);   //输入电流
	AppendTwoBytes(mcgs_buf, &len, PTCElectData.Output_i/10); //输出电流
	
	PTCElectData.Output_p = (PTCElectData.Rectifier_output_v*PTCElectData.PFCInductor1_i)/1000;
	AppendTwoBytes(mcgs_buf, &len, PTCElectData.Output_p);   //输出功率

	AppendTwoBytes(mcgs_buf, &len, PTCElectData.Rectifier_output_v); //整流输出电压
	AppendTwoBytes(mcgs_buf, &len, PTCElectData.PFCInductor1_i/10);   //PFC电感电流1
	AppendTwoBytes(mcgs_buf, &len, PTCElectData.PFCInductor2_i/10);
	AppendTwoBytes(mcgs_buf, &len, PTCElectData.BUSoutput_v);     //BUS输出电压

	AppendTwoBytes(mcgs_buf, &len, PTCElectData.Coil_i/10);		  //线圈电流
	AppendTwoBytes(mcgs_buf, &len, PTCElectData.INVInductor_i/10); //逆变电感电流


	AppendTwoBytes(mcgs_buf, &len, PTCElectData.INV_fre/10);		//逆变工作频率
	AppendTwoBytes(mcgs_buf, &len, PTCElectData.Temp_envir);	  //环境温度
	AppendTwoBytes(mcgs_buf, &len, PTCElectData.Temp_rectify);	//整流器温度
	AppendTwoBytes(mcgs_buf, &len, PTCElectData.Temp_pfc_mos);	//PFC功率管温度
	AppendTwoBytes(mcgs_buf, &len, PTCElectData.Temp_INV_mos1);	 //逆变功率管温度1
	AppendTwoBytes(mcgs_buf, &len, PTCElectData.Temp_INV_mos2);


	AppendTwoBytes(mcgs_buf, &len, PTCElectData.g_PTC_State);  //PTC上传系统状态
	
	AppendTwoBytes(mcgs_buf, &len, PTCElectData.g_pfc_State); //PTC上传PFC状态量
	
	AppendTwoBytes(mcgs_buf, &len, PTCElectData.g_inv_State);  //PTC上传INV状态量
	

	AppendTwoBytes(mcgs_buf, &len, PTCElectData.g_PFCAlarm_State);   //PTC上传PFC告警量

	AppendTwoBytes(mcgs_buf, &len, PTCElectData.g_INVAlarm_State);  //PTC上传INV告警量	
	////// 
	AppendTwoBytes(mcgs_buf, &len, SYSTEM_POWER_);      ///上传开机关机信息	
	
	if((PTCElectData.g_pfc_State>>8)&0x08)
	{
		warning_code = RETAIN_WARNING;
		System_State = FAULT;
	}
	if((PTCElectData.g_inv_State>>8)&0x08)
	{
		warning_code = RETAIN_WARNING;
		System_State = FAULT;
	}
	
	mcgs_buf[5]=len-6;
  mcgs_buf[len]=check_sum(&mcgs_buf[6],len-6);
	size=len+1;
	HexToAscii(mcgs_buf,sendbuff,size);
	uart_send_dma(UART_SCREEN,sendbuff,size*2);	
	if(warning_code == ALARM_CODE_NULL&&fault_code == ERROR_CODE_NULL)
	{
		if((PTCElectData.g_PTC_State>>8)&0x01)
		{
			System_State = RUNNING;
		}
		if((PTCElectData.g_PTC_State>>8)&0x02)
		{
			System_State = STANDBY;
		}	
	}	
}


void screen_ack(u8 order)
{
	memset(mcgs_buf,0,sizeof(mcgs_buf));
	memset(sendbuff,0,sizeof(sendbuff));
	mcgs_buf[0]= SCREEN_SOF1;
	mcgs_buf[1]= SCREEN_SOF2;
	mcgs_buf[2]= ADDR_CSU;
	mcgs_buf[3]= ADDR_SCREEN;
	if( (order==SCREEN_ACKON)||(order== SCREEN_ACKOFF))
	{
		mcgs_buf[4]=order;
		mcgs_buf[5]=0;
		mcgs_buf[6]=0;
		mcgs_buf[7]=0;
		
	}
  else if(order==SCREEN_ACKCONFIG)
	 {
	 		mcgs_buf[4]=order;
			mcgs_buf[5]=1;
			mcgs_buf[6]=1;  //1-配置成功，0-配置失败
			mcgs_buf[7]=0;	 	 
	 }
		
		HexToAscii(mcgs_buf,sendbuff,8);
		uart_send_dma(UART_SCREEN,sendbuff,16);	

}


u8 check_sum(void *si, u8 count)
{
		u16 sum=0;
	  u8 *ptr=(u8*)si;
	  while(count-->0)
		{
		  sum=sum+(*ptr++);		
		}
		return (sum&0xff);
}




/*
void usart2_send(u8* send_buf, u16 send_len)
{
        DMA_Cmd(DMA1_Stream6, DISABLE);                      //关闭DMA传输 
	while (DMA_GetCmdStatus(DMA1_Stream6) != DISABLE){}	//确保DMA可以被设置  
        DMA_ClearFlag(DMA1_Stream6,DMA_FLAG_TCIF6);
        DMA1_Stream6->M0AR =(u32)send_buf;
        DMA1_Stream6->NDTR = send_len; //数据传输量       
				DMA_Cmd(DMA1_Stream6, ENABLE);   //开启DMA传输      
}

void usart3_send(u8* send_buf, u16 send_len)
{
        DMA_Cmd(DMA1_Stream3, DISABLE);                      //关闭DMA传输 
	while (DMA_GetCmdStatus(DMA1_Stream3) != DISABLE){}	//确保DMA可以被设置  
        DMA_ClearFlag(DMA1_Stream3,DMA_FLAG_TCIF3);
        DMA1_Stream3->M0AR =(u32)send_buf;
        DMA1_Stream3->NDTR = send_len; //数据传输量       
				DMA_Cmd(DMA1_Stream3, ENABLE);   //开启DMA传输      
}



*/

