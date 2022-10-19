#ifndef __LED_H
#define __LED_H
#include "sys.h"
#include "includes.h"
#include "usart.h"
//BSRRH 表示BSRR寄存器高16位（BRy），哪一个BRy置1，引脚输出低电平
//BSRRL 表示BSRR寄存器低16位（BSy），哪一个BRy置1，引脚输出高电平

#define   RCC_ALL_IO 	(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC |RCC_AHB1Periph_GPIOD| RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOG)

//输入状态

#define REMOTE_RUN	GPIO_Pin_14			//PD14    远程控制运行信号-输入   1-正常，0-按下
#define REMOTE_STOP	GPIO_Pin_13			//PD13    远程控制停止信号-输入 	1-正常，0-按下
#define EXT_RESET		GPIO_Pin_9			//PG9  		外部复位信号-输入    1-正常，0-复位
#define EMO_STAT   	GPIO_Pin_4     	//PG4  		急停开关状态-输入    1-正常，0-按下

#define NFB_STAT		GPIO_Pin_10			//PG10 无熔丝开关状态-输入  		1-正常，0-闭合
#define MFUSE_STAT	GPIO_Pin_3      //PG3 主功率保险丝状态-输入  		1-正常，0-损坏
#define PPC_STAT   	GPIO_Pin_10     //PD10  副边PPC故障    					1-正常，0-故障 
#define POWER_STAT  GPIO_Pin_15   	//PA15 掉电检测									1-正常，0-掉电

#define MC1_STAT   	GPIO_Pin_6     	//PG6  主功率接触器状态-输入    1-断开，0-闭合
#define MC2_STAT   	GPIO_Pin_5     	//PG5  预充接触器状态-输入      1-断开，0-闭合
#define SR1_STAT   	GPIO_Pin_8    	//PA8 主功率继电器状态-输入   	1-断开，0-闭合
#define SR2_STAT   	GPIO_Pin_7    	//PG7 预充继电器状态-输入      	1-断开，0-闭合


#define TH3 			GPIO_Pin_2 			//PG2  1-报警，0-正常 ，利兹线接线铜盘温度75°
#define TH7 			GPIO_Pin_15 		//PD15  1-报警，0-正常 ，谐振电容温度75°
#define TH60 				GPIO_Pin_12 		//PD12  0-报警，1-正常 ，主功率器件散热器温度60°
#define TH80 				GPIO_Pin_11 		//PD11  0-报警，1-正常 ，主功率器件散热器温度80°

#define EXT_OHD			GPIO_Pin_4 			//PD4    1-正常，0-报警 ，外部感温线
#define INT_OHD			GPIO_Pin_1 			//PD1   1-正常，0-报警 ，内部感温线
#define SMOKE_STAT  GPIO_Pin_9      //PD9 烟感  1-正常，0-报警
#define MFAN_STAT   GPIO_Pin_8			//PD8 主风扇   1-正常，0-报警

//    #define TERMINAL_STAT   GPIO_Pin_			//接线端子过温

//输出控制
#define RESET 			GPIO_Pin_10		//PE10  安全单元复位 
//#define RELAY_MC1
//#define RELAY_MC2
//#define OUTPUT_RELAY

#define LED_LAM					GPIO_Pin_11   //PE11
#define LED_Run   			GPIO_Pin_12		//PE12
#define LED_Fault 			GPIO_Pin_13		//PE13
#define LED_Stop 				GPIO_Pin_9    //PE9

#define  	LEDRun_ON()      GPIOE->BSRRL = LED_Run
#define  	LEDRun_OFF()     GPIOE->BSRRH = LED_Run
#define  	LEDRun_TOGGLE()  GPIOE->ODR ^=  LED_Run

#define  	LEDStop_ON()      GPIOE->BSRRL = LED_Stop
#define  	LEDStop_OFF()     GPIOE->BSRRH = LED_Stop
#define  	LEDStop_TOGGLE()  GPIOE->ODR ^=  LED_Stop

#define  	LEDFault_ON()      GPIOE->BSRRL = LED_Fault
#define  	LEDFault_OFF()     GPIOE->BSRRH = LED_Fault
#define  	LEDFault_TOGGLE()  GPIOE->ODR ^=  LED_Fault

#define  	LEDLAM_ON()      GPIOE->BSRRL = LED_LAM
#define  	LEDLAM_OFF()     GPIOE->BSRRH = LED_LAM

#define  	RESET_ON()      GPIOE->BSRRL = RESET
#define  	RESET_OFF()     GPIOE->BSRRH = RESET


#define READ_REMOTE_RUN  GPIO_ReadInputDataBit(GPIOD,REMOTE_RUN)	
#define READ_REMOTE_STOP  GPIO_ReadInputDataBit(GPIOD,REMOTE_STOP)
#define READ_EXT_RESET  GPIO_ReadInputDataBit(GPIOG,EXT_RESET)
#define READ_NFB_STAT  GPIO_ReadInputDataBit(GPIOG,NFB_STAT)
#define READ_EMO_STAT  GPIO_ReadInputDataBit(GPIOG,EMO_STAT)
#define READ_MC1_STAT  GPIO_ReadInputDataBit(GPIOG,MC1_STAT)
#define READ_MC2_STAT  GPIO_ReadInputDataBit(GPIOG,MC2_STAT)
#define READ_MFUSE_STAT  GPIO_ReadInputDataBit(GPIOG,MFUSE_STAT)
#define READ_SR1_STAT  GPIO_ReadInputDataBit(GPIOA,SR1_STAT)
#define READ_SR2_STAT  GPIO_ReadInputDataBit(GPIOG,SR2_STAT)
#define READ_SMOKE_STAT  GPIO_ReadInputDataBit(GPIOD,SMOKE_STAT)
#define READ_MFAN_STAT  GPIO_ReadInputDataBit(GPIOD,MFAN_STAT)
#define READ_PPC_STAT  GPIO_ReadInputDataBit(GPIOD,PPC_STAT)
#define READ_TH3  GPIO_ReadInputDataBit(GPIOG,TH3)
#define READ_TH7  GPIO_ReadInputDataBit(GPIOD,TH7)
#define READ_TH60  GPIO_ReadInputDataBit(GPIOD,TH60)
#define READ_TH80  GPIO_ReadInputDataBit(GPIOD,TH80)
#define READ_EXT_OHD  GPIO_ReadInputDataBit(GPIOD,EXT_OHD)
#define READ_INT_OHD  GPIO_ReadInputDataBit(GPIOD,INT_OHD)
#define READ_POWER_STAT  GPIO_ReadInputDataBit(GPIOA,POWER_STAT)


// #define READ_TERMINAL_STAT  GPIO_ReadInputDataBit( ,TERMINAL_STAT)  ////接线端子


typedef union{
			u32 all;
			struct{
			/*温度0xf0*/	
					u8 th3:1; //PG2  1-报警，0-正常 ，利兹线接线铜盘温度75°
					u8 th7:1;	//PD15  1-报警，0-正常 ，谐振电容温度75°
					u8 th60:1; //PD12  0-报警，1-正常 ，主功率器件散热器温度60°
					u8 th80:1;  //PD11  0-报警，1-正常 ，主功率器件散热器温度80°
					u8 extohd:1; //PD4    1-正常，0-报警 ，外部感温线
					u8 intohd:1;//PD1   1-正常，0-报警 ，内部感温线
					u8 smoke:1;//PD9 烟感  1-正常，0-报警
					u8 fan:1;//PD8 主风扇   1-正常，0-报警

		/*控制信号0x0f*/
					u8 remote_run:1; //PD14    远程控制运行信号-输入   1-正常，0-按下
					u8 remote_stop:1;//PD13    远程控制停止信号-输入 	1-正常，0-按下	
					u8 ext_reset:1;//PG9  		外部复位信号-输入    1-正常，0-复位 
					u8 emo:1; //PG4  		急停开关状态-输入    1-正常，0-按下    
				  /////
				  u8 TERMINAL_STAT:1 ;   //// 接线端子温度
				 ////
					u8 reserve:3;

		/*状态0xff*/
					u8 nfb:1; //PG10 无熔丝开关状态-输入  		1-正常，0-闭合
					u8 mfuse:1;//PG3 主功率保险丝状态-输入  		1-正常，0-损坏
					u8 ppc:1;//PD10  副边PPC故障    					1-正常，0-故障 
					u8 power:1;//PA15 掉电检测									1-正常，0-掉电
					u8 mc1:1;	//PG6  主功率接触器状态-输入    1-断开，0-闭合
					u8 mc2:1; //PG5  预充接触器状态-输入      1-断开，0-闭合					   
					u8 sr1:1; //PA8 主功率继电器状态-输入   	1-断开，0-闭合
					u8 sr2:1;//PG7 预充继电器状态-输入      	1-断开，0-闭合
			}io;
}STAT_IO;
extern STAT_IO statio;


extern OS_TCB CanTaskTCB;

void io_Init(void);
void read_io(void);
#endif

