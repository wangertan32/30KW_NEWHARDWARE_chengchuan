#ifndef __ALARM_H
#define __ALARM_H	 
#include "sys.h" 

typedef enum  
{
				ALARM_CODE_NULL = 0,
  	    OVTH3,//�����߽���ͭ���¶�75�� 
				OVTH7,  //г������¶�75��
        OVTH60, //����������ɢ�����¶�60��
        OVTH80, //����������ɢ�����¶�80��				
	      OVEXT,  //�ⲿ������ 
	      OVINT,  // �ڲ�������    
	      SMOKE,  // �̸�  
				MFAN,   //������
				RETAIN_WARNING,//PTC����
	      OVTERMINAL,   //���߶��ӹ���
}ALARM_CODE;
extern ALARM_CODE warning_code;
typedef enum  
{
				ERROR_CODE_NULL = 0,
  	    NFB_OPEN,//����˿����״̬
				MFUSE_BROKEN,//�����ʱ���˿״̬  
        PPC_FAULT,  //����PPC����
        POWER_DROP, //������	
	      MC1_OPEN,   //�����ʽӴ���״̬  
	      MC2_OPEN,   //Ԥ��Ӵ���״̬  
	      SR1_OPEN,   //�����ʼ̵���״̬ 
				SR2_OPEN,   //Ԥ��̵���״̬
				EMERGENCY_STOP,//��ͣ
}ERROR_CODE;
extern ERROR_CODE fault_code;
typedef enum  
{
	STANDBY = 0,
	RUNNING,
	WARNING,//CSU�澯
	FAULT,//PTC�澯
	SHUTDOWN,
}SYSTEM_STATE;
extern SYSTEM_STATE System_State;
void handle_led(void);
void handle_alarm(void);
void handle_fault(void);

#endif

















