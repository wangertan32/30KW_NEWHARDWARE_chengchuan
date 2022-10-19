

#include "io.h"
#include "alarm.h"


STAT_IO statio={0};

void io_Init(void)
{    	 
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_ALL_IO, ENABLE);//ʹ��ʱ��

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����

	GPIO_InitStructure.GPIO_Pin = LED_LAM | LED_Run |LED_Fault |LED_Stop |RESET;
	GPIO_Init(GPIOE, &GPIO_InitStructure);//��ʼ��
	
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//��ͨ����ģʽ
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
	
	GPIO_InitStructure.GPIO_Pin = POWER_STAT | SR1_STAT;	
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = REMOTE_RUN | REMOTE_STOP | SMOKE_STAT | MFAN_STAT |PPC_STAT |TH7 | TH60 |TH80 |EXT_OHD |INT_OHD ;	
  GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = EXT_RESET | NFB_STAT | EMO_STAT | MC1_STAT | MC2_STAT | MFUSE_STAT | SR2_STAT | TH3;
  GPIO_Init(GPIOG, &GPIO_InitStructure);

///////////�ڴ˴����ӽ��߶���io��ʼ��  TERMINAL_STAT
//	GPIO_InitStructure.GPIO_Pin = TERMINAL_STAT;
//  GPIO_Init(   , &GPIO_InitStructure);




//////////
	LEDRun_ON();
	LEDStop_ON();  
	LEDFault_ON();
	LEDLAM_ON();
	RESET_ON();
	
  delay_ms(1000);
	
	LEDRun_OFF();
	LEDStop_OFF();  
	LEDFault_OFF();
	LEDLAM_OFF();
	RESET_OFF();	
}
/****************************
io���Ĺ���
1    �����߽���ͭ���¶�75��  
2    г������¶�75��            v
3    ����������ɢ�����¶�60��    v
4    ����������ɢ�����¶�80��    v
5    �ⲿ������                 v
6    �ڲ�������
7    �̸� 
8    ������                   v
9    Զ�̿��������ź�-����
10   Զ�̿���ֹͣ�ź�-����
11   �ⲿ��λ�ź�-����
12   ��ͣ����״̬-����         v
13   ����˿����״̬-����
14   �����ʱ���˿״̬-����       v
15   ����PPC����  
16   ������
17   �����ʽӴ���״̬-����      v
18   Ԥ��Ӵ���״̬-����        v
19   �����ʼ̵���״̬-����
20   Ԥ��̵���״̬-���� 
ADD
21   ���߶��ӹ���
**************************/
void read_io(void)
{
//	u8 io_temp=0; 
		u16   io_temp=0; 
////////////////////
// ������߶���1 ���� / 0 ����	
//	if(READ_TERMINAL_STAT == 1)    //���߶����¶�80��
//	{
//		delay_ms(100);
//		if(READ_TERMINAL_STAT == 1)
//		{
//			delay_ms(100);
//			if(READ_TERMINAL_STAT == 1)
//			{
//				statio.io.TERMINAL_STAT = 1;
//			}
//		}
//	}else if(READ_TERMINAL_STAT == 0)
//	{
//				statio.io.TERMINAL_STAT = 0;
//	}
	
///////////
	if(READ_TH3 == 1)    //�����߽���ͭ���¶�75��
	{
		delay_ms(100);
		if(READ_TH3 == 1)
		{
			delay_ms(100);
			if(READ_TH3 == 1)
			{
				statio.io.th3 = 1;
			}
		}
	}else if(READ_TH3 == 0)
	{
				statio.io.th3 = 0;
	}
	
	if(READ_TH7 == 1)    ////г������¶�75��
	{
		delay_ms(100);
		if(READ_TH7 == 1)
		{
			delay_ms(100);
			if(READ_TH7 == 1)
			{
				statio.io.th7 = 1;
			}
		}
	}else if(READ_TH7 == 0)
	{
		statio.io.th7 = 0;
	}	
///////// pd12 0 waring / 1 normal
	
	
//	statio.io.th60 = 0; 
	if(READ_TH60 == 0)      //////// ����������ɢ�����¶�60��
	{
		delay_ms(100);
		if(READ_TH60 == 0)
		{
			delay_ms(100);
			if(READ_TH60 == 0)
			{
				statio.io.th60 = 0;     ///waring 
			}
		}
	}else if(READ_TH60 == 1)
	{
		statio.io.th60 = 1;
	}
	
	
	
	
	if(READ_TH80 == 0)         //// ����������ɢ�����¶�80��
	{
		delay_ms(100);
		if(READ_TH80 == 0)
		{
			delay_ms(100);
			if(READ_TH80 == 0)
			{
				statio.io.th80 = 0;
			}
		}
	}else if(READ_TH80 == 1)
	{
		statio.io.th80 = 1;
	}	
	
	if(READ_EXT_OHD == 0)         //// �ⲿ������
	{
		delay_ms(100);
		if(READ_EXT_OHD == 0)
		{
			delay_ms(100);
			if(READ_EXT_OHD == 0)
			{
				statio.io.extohd = 0;
			}
		}
	}else if(READ_EXT_OHD == 1)
	{
		statio.io.extohd = 1;
	}	
	
	if(READ_INT_OHD == 0)         //// �ڲ�������
	{
		delay_ms(100);
		if(READ_INT_OHD == 0)
		{
			delay_ms(100);
			if(READ_INT_OHD == 0)
			{
				statio.io.intohd = 0;
			}
		}
	}else if(READ_INT_OHD == 1)
	{
		statio.io.intohd = 1;
	}		
	
	if(READ_SMOKE_STAT == 0)           //// �̸�
	{
		delay_ms(100);
		if(READ_SMOKE_STAT == 0)
		{
			delay_ms(100);
			if(READ_SMOKE_STAT == 0)
			{
				statio.io.smoke = 0;
			}
		}
	}else if(READ_SMOKE_STAT == 1)
	{
		statio.io.smoke = 1;
	}		
	
	if(READ_MFAN_STAT == 0)      //// ������
	{
		delay_ms(100);
		if(READ_MFAN_STAT == 0)
		{
			delay_ms(100);
			if(READ_MFAN_STAT == 0)
			{
				statio.io.fan = 0;
			}
		}
	}else if(READ_MFAN_STAT == 1)
	{
		statio.io.fan = 1;
	}		
	
	if(READ_REMOTE_RUN == 0)      /// Զ�̿��������ź�-����
	{
			delay_ms(100);
		if(READ_REMOTE_RUN == 0)
		{
				statio.io.remote_run = 0;
		}
	}else if(READ_REMOTE_RUN == 1)
	{
			delay_ms(100);
		if(READ_REMOTE_RUN == 1)
		{
				statio.io.remote_run = 1;
		}	
	}
	
	if(READ_REMOTE_STOP == 0)      //// Զ�̿���ֹͣ�ź�-����
	{
			delay_ms(100);
		if(READ_REMOTE_STOP == 0)
		{
				statio.io.remote_stop = 0;
		}
	}else if(READ_REMOTE_STOP == 1)
	{
			delay_ms(100);
		if(READ_REMOTE_STOP == 1)
		{
				statio.io.remote_stop = 1;
		}	
	}	
	
	if(READ_EXT_RESET == 0)     //// �ⲿ��λ�ź�-����
	{
			delay_ms(100);
		if(READ_EXT_RESET == 0)
		{
				statio.io.ext_reset = 0;
		}
	}else if(READ_EXT_RESET == 1)
	{
			delay_ms(100);
		if(READ_EXT_RESET == 1)
		{
				statio.io.ext_reset = 1;
		}	
	}		
	
	if(READ_EMO_STAT == 0)
	{
			delay_ms(100);
		if(READ_EMO_STAT == 0)
		{
				statio.io.emo = 0;    //// ��ͣ����״̬-����
		}
	}else if(READ_EMO_STAT == 1)
	{
			delay_ms(100);
		if(READ_EMO_STAT == 1)
		{
				statio.io.emo = 1;
		}	
	}		
	
	if(READ_NFB_STAT == 0)         //// ����˿����״̬-����
	{
			delay_ms(100);
		if(READ_NFB_STAT == 0)
		{
				statio.io.nfb = 0;
		}
	}else if(READ_NFB_STAT == 1)
	{
			delay_ms(100);
		if(READ_NFB_STAT == 1)
		{
				statio.io.nfb = 1;
		}	
	}	

	if(READ_MFUSE_STAT == 0)    //// �����ʱ���˿״̬-���� 
	{
			delay_ms(100);
		if(READ_MFUSE_STAT == 0)
		{
				statio.io.mfuse = 0;
		}
	}else if(READ_MFUSE_STAT == 1)
	{
			delay_ms(100);
		if(READ_MFUSE_STAT == 1)
		{
				statio.io.mfuse = 1;
		}	
	}	

	if(READ_PPC_STAT == 0)    /// ����PPC����  
	{
			delay_ms(100);
		if(READ_PPC_STAT == 0)
		{
				statio.io.ppc = 0;
		}
	}else if(READ_PPC_STAT == 1)
	{
			delay_ms(100);
		if(READ_PPC_STAT == 1)
		{
				statio.io.ppc = 1;
		}	
	}	

	if(READ_POWER_STAT == 0)   //// ������	
	{
			delay_ms(100);
		if(READ_POWER_STAT == 0)
		{
				statio.io.power = 0;
		}
	}else if(READ_POWER_STAT == 1)
	{
			delay_ms(100);
		if(READ_POWER_STAT == 1)
		{
				statio.io.power = 1;
		}	
	}

	if(READ_MC1_STAT == 0)   //// �����ʽӴ���״̬-���� 
	{
			delay_ms(100);
		if(READ_MC1_STAT == 0)
		{
				statio.io.mc1 = 0;
		}
	}else if(READ_MC1_STAT == 1)
	{
			delay_ms(100);
		if(READ_MC1_STAT == 1)
		{
				statio.io.mc1 = 1;
		}	
	}

	if(READ_MC2_STAT == 0)        // Ԥ��Ӵ���״̬-���� 
	{
			delay_ms(100);
		if(READ_MC2_STAT == 0)
		{
				statio.io.mc2 = 0;
		}
	}else if(READ_MC2_STAT == 1)
	{
			delay_ms(100);
		if(READ_MC2_STAT == 1)
		{
				statio.io.mc2 = 1;
		}	
	}

	if(READ_SR1_STAT == 0)      // �����ʼ̵���״̬-����
	{
			delay_ms(100);
		if(READ_SR1_STAT == 0)
		{
				statio.io.sr1 = 0;
		}
	}else if(READ_SR1_STAT == 1)
	{
			delay_ms(100);
		if(READ_SR1_STAT == 1)
		{
				statio.io.sr1 = 1;
		}	
	}

	if(READ_SR2_STAT == 0)      // Ԥ��̵���״̬-����  
	{
			delay_ms(100);
		if(READ_SR2_STAT == 0)
		{
				statio.io.sr2 = 0;
		}
	}else if(READ_SR2_STAT == 1)
	{
			delay_ms(100);
		if(READ_SR2_STAT == 1)
		{
				statio.io.sr2 = 1;
		}	
	}
	
//	io_temp=statio.all&0xff;//����0xfc
	
//	io_ctr=(statio.all>>8)&0xff;//0x0f
//	io_status=(statio.all>>16)&0xff;//0XCF
/**�¶ȴ���**/	 
//		 if(io_temp != 0xfc)
//		 {
//				if(statio.io.extohd==0)
//				{
//					warning_code=OVEXT;
//				}
//					
//				if(statio.io.intohd==0)
//				{
//					warning_code=OVINT;
//				}
//					
//				if(statio.io.smoke==0)
//				{
//					warning_code=SMOKE;
//				}
//					
//				if(statio.io.fan==0)
//				{
//					warning_code=MFAN;	
//				}
//							 
//				if(statio.io.th3==1)
//				{
//					warning_code=OVTH3;
//				}
//				
//				if(statio.io.th7==1)
//				{
//					warning_code=OVTH7;
//				}
//					
//				if(statio.io.th60==0)
//				{
//					warning_code=OVTH60;
//				}
//				
//				if(statio.io.th80==0)
//				{
//					warning_code=OVTH80;
//				}			 
//		 }
//			if(io_temp == 0xfc)
//			{
//				warning_code = ALARM_CODE_NULL;
//			}else{
//					System_State = WARNING;
//			}
//////////add
		io_temp=statio.all&0xffff;// ����0x0ffc
if(io_temp!=0x0ffc)	   ///// �й��Ϸ���
{
//////// �¶��ź�		  ע��ߵ��ź�
//////// �ƺ�Ҳ����Ҫ �����˽���io�ź�	
				if(statio.io.extohd==0)  ////�ⲿ�����߹���   waring
				{
					warning_code=OVEXT;
					System_State = WARNING;
				}		
				if(statio.io.th7==1)  ////г�����75�����    waring 
				{
					warning_code=OVTH7 ;
					System_State = WARNING;
				}
//				if(statio.io.TERMINAL_STAT==0)  ////���߶���80�����  waring
//				{
//					warning_code= OVTERMINAL ;
//				}
				if(statio.io.th80==0)  ////ɢ����80�����  waring
				{
					warning_code=OVTH80  ;
					System_State = WARNING;
				}
				if(statio.io.th60==0)  ////ɢ����60�����  waring
				{
					warning_code= OVTH60 ;
					System_State = WARNING;
				}
				if(statio.io.fan==0)  ////���ȹ���   waring
				{
					warning_code=MFAN  ;
					System_State = WARNING;
				}				
				
//////״̬�ź�				
				if(statio.io.mfuse==0)  ////�����ʱ��նϿ�   fault
				{
					fault_code=MFUSE_BROKEN  ;
					System_State = FAULT;
				}	
				if(statio.io.mc2==0)  ////Ԥ��Ӵ���ճ������   fault
				{
					fault_code=MC2_OPEN  ;
					System_State = FAULT;
				}
				if(statio.io.mc1==0)  ////�����ʽӴ���ճ������  fault
				{
					fault_code=MC1_OPEN  ;
					System_State = FAULT;
				}				
				
///// �����ź�	��ͣ���� ����
		  if(statio.io.emo==0)
			 {
					fault_code = EMERGENCY_STOP;
					System_State = SHUTDOWN;				 
			 }

}		
		
		
		
		
		
//////////////////
/**�����ź�**/	
////////////// ����ͣ
//		  if(statio.io.emo==0)
//			 {
//					fault_code = EMERGENCY_STOP;
//					System_State = SHUTDOWN;				 
//			 }
	 
}



