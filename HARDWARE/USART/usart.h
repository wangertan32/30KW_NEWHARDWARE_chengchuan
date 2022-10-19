#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "stm32f4xx_conf.h"
#include "sys.h" 
#include "includes.h"
//////////////////////////////////////////////////////////////////////////////////	 

#define USART_REC_LEN  			200  	//定义最大接收字节数 200
#define EN_USART1_RX 			1		//使能（1）/禁止（0）串口1接收


#define  USART2_RX_LEN  256
#define  USART2_TX_LEN  256

#define  USART3_RX_LEN  256
#define  USART3_TX_LEN  256

#define  UART4_RX_LEN  256
#define  UART4_TX_LEN  256


#define  USART5_RX_LEN  256
#define  USART5_TX_LEN  256

#define  USART6_RX_LEN  256
#define  USART6_TX_LEN  256


#define  ADDR_CSU    0x02       //CSU源地址
#define  ADDR_SCREEN 0X01       //屏源地址

#define  SCREEN_SOF1  0x77      //CSU-屏 帧头
#define  SCREEN_SOF2  0x88      //CSU-屏 帧头
#define  SCREEN_POWERON  0x10   //开机
#define  SCREEN_POWEROFF  0x11  //关机
#define  SCREEN_SEEK  0x12      //运行
#define  SCREEN_CONFIG  0x13    //配置

#define  UART_TEST 0x20


#define  SCREEN_ACKON  0x20     //开机
#define  SCREEN_ACKOFF  0x21    //关机
#define  SCREEN_ACKDATA  0x22   //运行
#define  SCREEN_ACKCONFIG  0x23 //配置




#define UART_SCREEN 1   //UART2  触摸屏通信  RS485-A
#define UART_METER  2   //UART6   电表通信  RS485-B
#define UART_4G     3
#define UART_WIFI  4    //UART5   WIFI通信  
#define UART_FAILOVER 5 //UART3   失效转移通信 RS485-D
#define UART_OUT 6      //UART4  485对外接口   RS485-C

#define SCREEN_485_RX   GPIOD->BSRRL = GPIO_Pin_7   //触摸屏通信  RS485-A
#define SCREEN_485_TX   GPIOD->BSRRH = GPIO_Pin_7 

#define FAILOVER_485_RX   GPIOB->BSRRL = GPIO_Pin_12//失效转移通信 RS485-D
#define FAILOVER_485_TX   GPIOB->BSRRH = GPIO_Pin_12

#define METER_485_RX   GPIOG->BSRRL = GPIO_Pin_8//电表通信  RS485-B
#define METER_485_TX   GPIOG->BSRRH = GPIO_Pin_8

#define OUT_485_RX   GPIOA->BSRRL = GPIO_Pin_3   //485对外接口   RS485-C
#define OUT_485_TX   GPIOA->BSRRH = GPIO_Pin_3



extern OS_TCB ScreenTaskTCB;

extern u8 usart2_tx_buf[USART2_TX_LEN];
extern u8 usart2_rx_buf[USART2_RX_LEN];

extern u8 usart3_tx_buf[USART3_TX_LEN];
extern u8 usart3_rx_buf[USART3_RX_LEN];

extern u8 uart4_tx_buf[UART4_TX_LEN];
extern u8 uart4_rx_buf[UART4_RX_LEN];

extern u8 usart5_tx_buf[USART5_TX_LEN];
extern u8 usart5_rx_buf[USART5_RX_LEN];

extern u8 usart6_tx_buf[USART6_TX_LEN];
extern u8 usart6_rx_buf[USART6_RX_LEN];
	  	

extern u8 usart5_buf[20];


void uart1_debug_init(u32 bound);
void usart2_screen_init(u32 bound);
void usart3_failover_init(u32 bound);
void uart4_out_init(u32 bound);
void usart5_wifi_init(u32 bound);
void usart6_meter_init(u32 bound);

void screen_ack(u8 order);
void screen_datasend(u8 order);	

void uart_send(u8 usart_periph, u8 *addr, u8 len);

void uart_send_dma(u8 usart_periph, u8* send_buf, u16 send_len);

#endif


