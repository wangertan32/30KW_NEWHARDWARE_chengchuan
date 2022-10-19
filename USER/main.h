#ifndef __MAIN_H
#define __MAIN_H	 

#include "includes.h"

#include "io.h"
#include "alarm.h"
#include "can.h"
#include "timer.h"
#include "sram.h"
#include "malloc.h"
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "iwdg.h"

#define FIRST_START_FLAG    0XAA   //第一次上电标志
/*系统固定参数*/
typedef  struct{
	uint8 uDeviceId;//主机ID：
	uint8 VSRSION[20];//系统版本号："220610_H01_S01" 年 月 日 硬件版本 软件版本 
	
}SYSTEM_FIXED;

#define START_TASK_PRIO		18
#define START_STK_SIZE		128

#define TASK_SCREEN_NUM	      10
#define SCREEN_TASK_PRIO 		17
#define SCREEN_STK_SIZE		256	

#define TASK_CAN_NUM	      10		//发任务内建消息队列的长度
#define CAN_TASK_PRIO	      16
#define CAN_STK_SIZE		    1024

#define TASK_HANDLE_NUM	      10
#define HANDLE_TASK_PRIO 		15
#define HANDLE_STK_SIZE		128

#define TASK_SCAN_NUM	      10
#define SCAN_TASK_PRIO 		14
#define SCAN_STK_SIZE		128

#define TASK_PLC_NUM	      10
#define PLC_TASK_PRIO 		13
#define PLC_STK_SIZE		128

#define TASK_METER_NUM	      10
#define METER_TASK_PRIO 		12
#define METER_STK_SIZE		128
// ==========================================================================
// 消息定义
// ==========================================================================
#define MSG_CAN_LINK                    0x00   //建立链接
#define MSG_CAN_SEARCH                  0x01   //后台查询
#define MSG_CAN_POWERON                 0x02   //CSU-PTC开机
#define MSG_CAN_POWEROFF                0x03   //CSU-PTC关机
#define MSG_CAN_CONFIG_NUM              0x04   //CSU-PTC配置机号
#define MSG_CAN_CONFIG_DATA             0x05   //CSU-PTC配置参数
#define MSG_CAN_CONFIG_SEARCH_DATA             0x06   //查询参数值
#define MSG_CAN_CONFIG_SEARCH_STATE             0x07   //查询状态

//条件编译其他外围资源
#define M24256    		1
#define BQ32000    		1
#define ETHERNET 			0
#define SDCARD        1
#define WIFI					0

#if ETHERNET
#include "lwipopts.h"
#include "udp_demo.h"
#include "lwip_comm.h"
#include "lwip_comm.h"
#include "LAN8720.h"
#endif

#if M24256	
#include "24cxx.h"
void Variable_Eeprom(void);
#endif	 
#if BQ32000	
#include "bq32000.h"  	 
#endif		 


#if SDCARD
#include "sdio_sdcard.h"
#include "ff.h"  
#include "exfuns.h"   
ErrorStatus Write_TFCard(void);
ErrorStatus Write_Log(void);
#endif

#if WIFI
#include "wifi-config.h"
#endif

#endif
















