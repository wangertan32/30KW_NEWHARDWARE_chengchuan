

#include "io.h"
#include "alarm.h"


STAT_IO statio={0};

void io_Init(void)
{    	 
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_ALL_IO, ENABLE);//使能时钟

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉

	GPIO_InitStructure.GPIO_Pin = LED_LAM | LED_Run |LED_Fault |LED_Stop |RESET;
	GPIO_Init(GPIOE, &GPIO_InitStructure);//初始化
	
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//普通输入模式
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	
	GPIO_InitStructure.GPIO_Pin = POWER_STAT | SR1_STAT;	
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = REMOTE_RUN | REMOTE_STOP | SMOKE_STAT | MFAN_STAT |PPC_STAT |TH7 | TH60 |TH80 |EXT_OHD |INT_OHD ;	
  GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = EXT_RESET | NFB_STAT | EMO_STAT | MC1_STAT | MC2_STAT | MFUSE_STAT | SR2_STAT | TH3;
  GPIO_Init(GPIOG, &GPIO_InitStructure);

///////////在此处增加接线端子io初始化  TERMINAL_STAT
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
io读的故障
1    利兹线接线铜盘温度75°  
2    谐振电容温度75°            v
3    主功率器件散热器温度60°    v
4    主功率器件散热器温度80°    v
5    外部感温线                 v
6    内部感温线
7    烟感 
8    主风扇                   v
9    远程控制运行信号-输入
10   远程控制停止信号-输入
11   外部复位信号-输入
12   急停开关状态-输入         v
13   无熔丝开关状态-输入
14   主功率保险丝状态-输入       v
15   副边PPC故障  
16   掉电检测
17   主功率接触器状态-输入      v
18   预充接触器状态-输入        v
19   主功率继电器状态-输入
20   预充继电器状态-输入 
ADD
21   接线端子过温
**************************/
void read_io(void)
{
//	u8 io_temp=0; 
		u16   io_temp=0; 
////////////////////
// 假设接线端子1 报警 / 0 正常	
//	if(READ_TERMINAL_STAT == 1)    //接线端子温度80℃
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
	if(READ_TH3 == 1)    //利兹线接线铜盘温度75°
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
	
	if(READ_TH7 == 1)    ////谐振电容温度75°
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
	if(READ_TH60 == 0)      //////// 主功率器件散热器温度60°
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
	
	
	
	
	if(READ_TH80 == 0)         //// 主功率器件散热器温度80°
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
	
	if(READ_EXT_OHD == 0)         //// 外部感温线
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
	
	if(READ_INT_OHD == 0)         //// 内部感温线
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
	
	if(READ_SMOKE_STAT == 0)           //// 烟感
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
	
	if(READ_MFAN_STAT == 0)      //// 主风扇
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
	
	if(READ_REMOTE_RUN == 0)      /// 远程控制运行信号-输入
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
	
	if(READ_REMOTE_STOP == 0)      //// 远程控制停止信号-输入
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
	
	if(READ_EXT_RESET == 0)     //// 外部复位信号-输入
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
				statio.io.emo = 0;    //// 急停开关状态-输入
		}
	}else if(READ_EMO_STAT == 1)
	{
			delay_ms(100);
		if(READ_EMO_STAT == 1)
		{
				statio.io.emo = 1;
		}	
	}		
	
	if(READ_NFB_STAT == 0)         //// 无熔丝开关状态-输入
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

	if(READ_MFUSE_STAT == 0)    //// 主功率保险丝状态-输入 
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

	if(READ_PPC_STAT == 0)    /// 副边PPC故障  
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

	if(READ_POWER_STAT == 0)   //// 掉电检测	
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

	if(READ_MC1_STAT == 0)   //// 主功率接触器状态-输入 
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

	if(READ_MC2_STAT == 0)        // 预充接触器状态-输入 
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

	if(READ_SR1_STAT == 0)      // 主功率继电器状态-输入
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

	if(READ_SR2_STAT == 0)      // 预充继电器状态-输入  
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
	
//	io_temp=statio.all&0xff;//正常0xfc
	
//	io_ctr=(statio.all>>8)&0xff;//0x0f
//	io_status=(statio.all>>16)&0xff;//0XCF
/**温度处理**/	 
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
		io_temp=statio.all&0xffff;// 正常0x0ffc
if(io_temp!=0x0ffc)	   ///// 有故障发生
{
//////// 温度信号		  注意高低信号
//////// 似乎也不需要 在屏端解析io信号	
				if(statio.io.extohd==0)  ////外部感温线故障   waring
				{
					warning_code=OVEXT;
					System_State = WARNING;
				}		
				if(statio.io.th7==1)  ////谐振电容75°过温    waring 
				{
					warning_code=OVTH7 ;
					System_State = WARNING;
				}
//				if(statio.io.TERMINAL_STAT==0)  ////接线端子80°过温  waring
//				{
//					warning_code= OVTERMINAL ;
//				}
				if(statio.io.th80==0)  ////散热器80°过温  waring
				{
					warning_code=OVTH80  ;
					System_State = WARNING;
				}
				if(statio.io.th60==0)  ////散热器60°过温  waring
				{
					warning_code= OVTH60 ;
					System_State = WARNING;
				}
				if(statio.io.fan==0)  ////风扇故障   waring
				{
					warning_code=MFAN  ;
					System_State = WARNING;
				}				
				
//////状态信号				
				if(statio.io.mfuse==0)  ////主功率保险断开   fault
				{
					fault_code=MFUSE_BROKEN  ;
					System_State = FAULT;
				}	
				if(statio.io.mc2==0)  ////预充接触器粘连故障   fault
				{
					fault_code=MC2_OPEN  ;
					System_State = FAULT;
				}
				if(statio.io.mc1==0)  ////主功率接触器粘连故障  fault
				{
					fault_code=MC1_OPEN  ;
					System_State = FAULT;
				}				
				
///// 控制信号	急停发生 测试
		  if(statio.io.emo==0)
			 {
					fault_code = EMERGENCY_STOP;
					System_State = SHUTDOWN;				 
			 }

}		
		
		
		
		
		
//////////////////
/**控制信号**/	
////////////// 机柜急停
//		  if(statio.io.emo==0)
//			 {
//					fault_code = EMERGENCY_STOP;
//					System_State = SHUTDOWN;				 
//			 }
	 
}



