#ifndef __ALARM_H
#define __ALARM_H	 
#include "sys.h" 

typedef enum  
{
				ALARM_CODE_NULL = 0,
  	    OVTH3,//利兹线接线铜盘温度75° 
				OVTH7,  //谐振电容温度75°
        OVTH60, //主功率器件散热器温度60°
        OVTH80, //主功率器件散热器温度80°				
	      OVEXT,  //外部感温线 
	      OVINT,  // 内部感温线    
	      SMOKE,  // 烟感  
				MFAN,   //主风扇
				RETAIN_WARNING,//PTC警告
	      OVTERMINAL,   //接线端子过温
}ALARM_CODE;
extern ALARM_CODE warning_code;
typedef enum  
{
				ERROR_CODE_NULL = 0,
  	    NFB_OPEN,//无熔丝开关状态
				MFUSE_BROKEN,//主功率保险丝状态  
        PPC_FAULT,  //副边PPC故障
        POWER_DROP, //掉电检测	
	      MC1_OPEN,   //主功率接触器状态  
	      MC2_OPEN,   //预充接触器状态  
	      SR1_OPEN,   //主功率继电器状态 
				SR2_OPEN,   //预充继电器状态
				EMERGENCY_STOP,//急停
}ERROR_CODE;
extern ERROR_CODE fault_code;
typedef enum  
{
	STANDBY = 0,
	RUNNING,
	WARNING,//CSU告警
	FAULT,//PTC告警
	SHUTDOWN,
}SYSTEM_STATE;
extern SYSTEM_STATE System_State;
void handle_led(void);
void handle_alarm(void);
void handle_fault(void);

#endif

















