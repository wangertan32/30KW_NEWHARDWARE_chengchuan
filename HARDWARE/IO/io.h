#ifndef __LED_H
#define __LED_H
#include "sys.h"
#include "includes.h"
#include "usart.h"
//BSRRH ��ʾBSRR�Ĵ�����16λ��BRy������һ��BRy��1����������͵�ƽ
//BSRRL ��ʾBSRR�Ĵ�����16λ��BSy������һ��BRy��1����������ߵ�ƽ

#define   RCC_ALL_IO 	(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC |RCC_AHB1Periph_GPIOD| RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOG)

//����״̬

#define REMOTE_RUN	GPIO_Pin_14			//PD14    Զ�̿��������ź�-����   1-������0-����
#define REMOTE_STOP	GPIO_Pin_13			//PD13    Զ�̿���ֹͣ�ź�-���� 	1-������0-����
#define EXT_RESET		GPIO_Pin_9			//PG9  		�ⲿ��λ�ź�-����    1-������0-��λ
#define EMO_STAT   	GPIO_Pin_4     	//PG4  		��ͣ����״̬-����    1-������0-����

#define NFB_STAT		GPIO_Pin_10			//PG10 ����˿����״̬-����  		1-������0-�պ�
#define MFUSE_STAT	GPIO_Pin_3      //PG3 �����ʱ���˿״̬-����  		1-������0-��
#define PPC_STAT   	GPIO_Pin_10     //PD10  ����PPC����    					1-������0-���� 
#define POWER_STAT  GPIO_Pin_15   	//PA15 ������									1-������0-����

#define MC1_STAT   	GPIO_Pin_6     	//PG6  �����ʽӴ���״̬-����    1-�Ͽ���0-�պ�
#define MC2_STAT   	GPIO_Pin_5     	//PG5  Ԥ��Ӵ���״̬-����      1-�Ͽ���0-�պ�
#define SR1_STAT   	GPIO_Pin_8    	//PA8 �����ʼ̵���״̬-����   	1-�Ͽ���0-�պ�
#define SR2_STAT   	GPIO_Pin_7    	//PG7 Ԥ��̵���״̬-����      	1-�Ͽ���0-�պ�


#define TH3 			GPIO_Pin_2 			//PG2  1-������0-���� �������߽���ͭ���¶�75��
#define TH7 			GPIO_Pin_15 		//PD15  1-������0-���� ��г������¶�75��
#define TH60 				GPIO_Pin_12 		//PD12  0-������1-���� ������������ɢ�����¶�60��
#define TH80 				GPIO_Pin_11 		//PD11  0-������1-���� ������������ɢ�����¶�80��

#define EXT_OHD			GPIO_Pin_4 			//PD4    1-������0-���� ���ⲿ������
#define INT_OHD			GPIO_Pin_1 			//PD1   1-������0-���� ���ڲ�������
#define SMOKE_STAT  GPIO_Pin_9      //PD9 �̸�  1-������0-����
#define MFAN_STAT   GPIO_Pin_8			//PD8 ������   1-������0-����

//    #define TERMINAL_STAT   GPIO_Pin_			//���߶��ӹ���

//�������
#define RESET 			GPIO_Pin_10		//PE10  ��ȫ��Ԫ��λ 
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


// #define READ_TERMINAL_STAT  GPIO_ReadInputDataBit( ,TERMINAL_STAT)  ////���߶���


typedef union{
			u32 all;
			struct{
			/*�¶�0xf0*/	
					u8 th3:1; //PG2  1-������0-���� �������߽���ͭ���¶�75��
					u8 th7:1;	//PD15  1-������0-���� ��г������¶�75��
					u8 th60:1; //PD12  0-������1-���� ������������ɢ�����¶�60��
					u8 th80:1;  //PD11  0-������1-���� ������������ɢ�����¶�80��
					u8 extohd:1; //PD4    1-������0-���� ���ⲿ������
					u8 intohd:1;//PD1   1-������0-���� ���ڲ�������
					u8 smoke:1;//PD9 �̸�  1-������0-����
					u8 fan:1;//PD8 ������   1-������0-����

		/*�����ź�0x0f*/
					u8 remote_run:1; //PD14    Զ�̿��������ź�-����   1-������0-����
					u8 remote_stop:1;//PD13    Զ�̿���ֹͣ�ź�-���� 	1-������0-����	
					u8 ext_reset:1;//PG9  		�ⲿ��λ�ź�-����    1-������0-��λ 
					u8 emo:1; //PG4  		��ͣ����״̬-����    1-������0-����    
				  /////
				  u8 TERMINAL_STAT:1 ;   //// ���߶����¶�
				 ////
					u8 reserve:3;

		/*״̬0xff*/
					u8 nfb:1; //PG10 ����˿����״̬-����  		1-������0-�պ�
					u8 mfuse:1;//PG3 �����ʱ���˿״̬-����  		1-������0-��
					u8 ppc:1;//PD10  ����PPC����    					1-������0-���� 
					u8 power:1;//PA15 ������									1-������0-����
					u8 mc1:1;	//PG6  �����ʽӴ���״̬-����    1-�Ͽ���0-�պ�
					u8 mc2:1; //PG5  Ԥ��Ӵ���״̬-����      1-�Ͽ���0-�պ�					   
					u8 sr1:1; //PA8 �����ʼ̵���״̬-����   	1-�Ͽ���0-�պ�
					u8 sr2:1;//PG7 Ԥ��̵���״̬-����      	1-�Ͽ���0-�պ�
			}io;
}STAT_IO;
extern STAT_IO statio;


extern OS_TCB CanTaskTCB;

void io_Init(void);
void read_io(void);
#endif

