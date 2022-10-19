
#include "alarm.h"
#include "io.h"

ALARM_CODE warning_code = ALARM_CODE_NULL;
ERROR_CODE fault_code = ERROR_CODE_NULL;
SYSTEM_STATE System_State=STANDBY;  

void handle_led(void)
{
	switch(System_State)
	 {
		 case STANDBY:
			 LEDRun_OFF();
			 LEDStop_ON();
			 LEDFault_OFF();
		 break;
		 
	 	 case RUNNING:
			 LEDRun_ON();
			 LEDStop_OFF();
			 LEDFault_OFF();
		 break;
		 
		 case WARNING:
			 LEDRun_OFF();
			 LEDStop_OFF();
			 LEDFault_ON();
		 break;
				 	 	 		 
		 case FAULT:
			 LEDRun_OFF();
			 LEDStop_OFF();
			 LEDFault_ON();
		 break;
		 
		 case SHUTDOWN:
			 LEDRun_OFF();
			 LEDStop_ON();
			 LEDFault_OFF();
		 break;
				 
			default:
			break;
	 }
}

void handle_alarm(void)
{
		switch(warning_code)
		 {
		 		 case OVTH3:
				 break;
	
				 case OVTH7:
				 break;
				
				 case OVTH60:
				 break;
		 				 
				 case OVTH80:
				 break;
				 
				 case OVEXT:
				 break;
				 
				 case OVINT:
				 break;
					 					 
				 case SMOKE:
				 break;
					 
				 case MFAN:
				 break;
				 
		 		default:
				break;
		 }

}
	
void handle_fault(void)
{

		switch(fault_code)
		 {
		 		 case NFB_OPEN:
				 break;
	
				 case MFUSE_BROKEN:
				 break;
				
				 case PPC_FAULT:
				 break;
		 				 
				 case POWER_DROP:
				 break;
				 
				 case MC1_OPEN:
				 break;
				 
				 case MC2_OPEN:
				 break;
					 					 
				 case SR1_OPEN:
				 break;
					 
				 case SR2_OPEN:
				 break;
				 
		 		default:
				break;
		 }





}



