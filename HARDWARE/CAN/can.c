#include "can.h"
#include "delay.h"
#include "includes.h"
PTC_ELECT_DATA PTCElectData;
SET_PTC_DATA SetPTCData;
//CAN��ʼ��
//tsjw:����ͬ����Ծʱ�䵥Ԫ.��Χ:CAN_SJW_1tq~ CAN_SJW_4tq
//tbs2:ʱ���2��ʱ�䵥Ԫ.   ��Χ:CAN_BS2_1tq~CAN_BS2_8tq;
//tbs1:ʱ���1��ʱ�䵥Ԫ.   ��Χ:CAN_BS1_1tq ~CAN_BS1_16tq
//brp :�����ʷ�Ƶ��.��Χ:1~1024; tq=(brp)*tpclk1
//������=Fpclk1/((tbs1+1+tbs2+1+1)*brp);
//mode:CAN_Mode_Normal,��ͨģʽ;CAN_Mode_LoopBack,�ػ�ģʽ;
//Fpclk1��ʱ���ڳ�ʼ����ʱ������Ϊ42M,�������CAN1_Mode_Init(CAN_SJW_1tq,CAN_BS2_6tq,CAN_BS1_7tq,6,CAN_Mode_LoopBack);
//������Ϊ:42M/((6+7+1)*6)=500Kbps
//����ֵ:0,��ʼ��OK;
//����,��ʼ��ʧ��; 


//u16 CRC16(u8 *puchMsg, u16 usDataLen);
//void InvertUint8(u8 *dBuf, u8 *srcBuf);
//void InvertUint16(u16 *dBuf,u16 *srcBuf);

void Can_ReadData(CanRxMsg msg);
u16 CRC16_XMODEM(u8 *puchMsg, u16 usDataLen);
u32 canid_tx=0, canid_rx=0;
u8 flag_canrev=0;


void canid_init(void)
{
	
	canid_tx=PRIORITY<<28 | CSU_TYPE<<21 | CSU_ADDR<<14 | PTC_TYPE<<7 | PTC_ADDR;
	canid_rx=PRIORITY<<28 | PTC_TYPE<<21 | PTC_ADDR<<14 | CSU_TYPE<<7 | CSU_ADDR;
	
//u8 crc_test[12]={0xE0, 0x01,0x01, 0x28, 00, 00, 0x85, 00, 00, 00, 00, 00};
//	u16 crc_test1=0;
//	crc_test1=CRC16_XMODEM(crc_test,12);
}


#if CANTEST
u8 CAN1_Mode_Init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode)
{
  	GPIO_InitTypeDef GPIO_InitStructure; 
	  CAN_InitTypeDef        CAN_InitStructure;
  	CAN_FilterInitTypeDef  CAN_FilterInitStructure;
		NVIC_InitTypeDef  NVIC_InitStructure;
    //ʹ�����ʱ��
	  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//ʹ��PORTAʱ��	                   											 

  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);//ʹ��CAN1ʱ��	
	
    //��ʼ��GPIO
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11| GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
    GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��PA11,PA12
	
	  //���Ÿ���ӳ������
	  GPIO_PinAFConfig(GPIOA,GPIO_PinSource11,GPIO_AF_CAN1); //GPIOA11����ΪCAN1
	  GPIO_PinAFConfig(GPIOA,GPIO_PinSource12,GPIO_AF_CAN1); //GPIOA12����ΪCAN1
	  
  	//CAN��Ԫ����
   	CAN_InitStructure.CAN_TTCM=DISABLE;	//��ʱ�䴥��ͨ��ģʽ   
  	CAN_InitStructure.CAN_ABOM=ENABLE;	//����Զ����߹���	  
  	CAN_InitStructure.CAN_AWUM=DISABLE;//˯��ģʽͨ���������(���CAN->MCR��SLEEPλ)
  	CAN_InitStructure.CAN_NART=ENABLE;	//��ֹ�����Զ����� 
  	CAN_InitStructure.CAN_RFLM=DISABLE;	//���Ĳ�����,�µĸ��Ǿɵ�  
  	CAN_InitStructure.CAN_TXFP=DISABLE;	//���ȼ��ɱ��ı�ʶ������ 
  	CAN_InitStructure.CAN_Mode= mode;	 //ģʽ���� 
  	CAN_InitStructure.CAN_SJW=tsjw;	//����ͬ����Ծ���(Tsjw)Ϊtsjw+1��ʱ�䵥λ CAN_SJW_1tq~CAN_SJW_4tq
  	CAN_InitStructure.CAN_BS1=tbs1; //Tbs1��ΧCAN_BS1_1tq ~CAN_BS1_16tq
  	CAN_InitStructure.CAN_BS2=tbs2;//Tbs2��ΧCAN_BS2_1tq ~	CAN_BS2_8tq
  	CAN_InitStructure.CAN_Prescaler=brp;  //��Ƶϵ��(Fdiv)Ϊbrp+1	
  	CAN_Init(CAN1, &CAN_InitStructure);   // ��ʼ��CAN1 
    
		//���ù�����
 	  CAN_FilterInitStructure.CAN_FilterNumber=0;	  //������0
  	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; 
  	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit; //32λ 
  	CAN_FilterInitStructure.CAN_FilterIdHigh=0x0000;////32λID
  	CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
  	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x0000;//32λMASK
  	CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;
   	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;//������0������FIFO0
  	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE; //���������0
  	CAN_FilterInit(&CAN_FilterInitStructure);//�˲�����ʼ��
		
	  CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);//FIFO0��Ϣ�Һ��ж�����.		    
  
  	NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;     // �����ȼ�Ϊ1
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;            // �����ȼ�Ϊ0
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);

	return 0;
} 

u8 CAN1Send_Msg(u8* msg,u8 len,u32 exid)
{	
  u8 mbox;
  u16 i=0;
  CanTxMsg TxMessage;
  TxMessage.ExtId=exid;	 // ������չ��ʾ����29λ��

	TxMessage.IDE=4;		  // ʹ����չ��ʶ��;	
  TxMessage.RTR=0;		  // ��Ϣ����Ϊ����֡��һ֡8λ
  TxMessage.DLC=len;							 
  for(i=0;i<len;i++)
  TxMessage.Data[i]=msg[i];				      
  mbox= CAN_Transmit(CAN1, &TxMessage);   
  i=0;
  while((CAN_TransmitStatus(CAN1, mbox)==CAN_TxStatus_Failed)&&(i<0XFFF))i++;	//�ȴ����ͽ���
  if(i>=0XFFF)return 1;
  return 0;			
}

void CAN1_RX0_IRQHandler(void)
{	
  	CanRxMsg RxMessage;

	  OSIntEnter();
    CAN_Receive(CAN1, 0, &RxMessage);
	  
	  switch(RxMessage.ExtId)
		{
			case  0x10207EF8: 
				Can_ReadData(RxMessage);
				flag_canrev=1;
			break;
			
			default:
			break;
		}				
	  	OSIntExit();    	//�˳��ж�
}

#endif

u8 can2_dsp_Init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode)  //CAN2 PB5 PB6
{
  	GPIO_InitTypeDef GPIO_InitStructure; 
	  CAN_InitTypeDef        CAN_InitStructure;
  	CAN_FilterInitTypeDef  CAN_FilterInitStructure;
   	NVIC_InitTypeDef  NVIC_InitStructure;
	
    //ʹ�����ʱ��
	  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN2, ENABLE);//ʹ��CAN1ʱ��	

    //��ʼ��GPIO
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
    GPIO_Init(GPIOB, &GPIO_InitStructure);
	
		 //���Ÿ���ӳ������
	  GPIO_PinAFConfig(GPIOB, GPIO_PinSource5,GPIO_AF_CAN2);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource6,GPIO_AF_CAN2);
  	//CAN��Ԫ����
   	CAN_InitStructure.CAN_TTCM=DISABLE;	//��ʱ�䴥��ͨ��ģʽ   
  	CAN_InitStructure.CAN_ABOM=ENABLE;	//����Զ����߹���	  
  	CAN_InitStructure.CAN_AWUM=DISABLE;//˯��ģʽͨ���������(���CAN->MCR��SLEEPλ)
  	CAN_InitStructure.CAN_NART=ENABLE;	//��ֹ�����Զ����� 
  	CAN_InitStructure.CAN_RFLM=DISABLE;	//���Ĳ�����,�µĸ��Ǿɵ�  
  	CAN_InitStructure.CAN_TXFP=DISABLE;	//���ȼ��ɱ��ı�ʶ������ 
  	CAN_InitStructure.CAN_Mode= mode;	 //ģʽ���� 
  	CAN_InitStructure.CAN_SJW=tsjw;	//����ͬ����Ծ���(Tsjw)Ϊtsjw+1��ʱ�䵥λ CAN_SJW_1tq~CAN_SJW_4tq
  	CAN_InitStructure.CAN_BS1=tbs1; //Tbs1��ΧCAN_BS1_1tq ~CAN_BS1_16tq
  	CAN_InitStructure.CAN_BS2=tbs2;//Tbs2��ΧCAN_BS2_1tq ~	CAN_BS2_8tq
  	CAN_InitStructure.CAN_Prescaler=brp;  //��Ƶϵ��(Fdiv)Ϊbrp+1	
  	CAN_Init(CAN2, &CAN_InitStructure);   // ��ʼ��CAN1 
    
		//���ù�����
 	  CAN_FilterInitStructure.CAN_FilterNumber=15;	  //������0
  	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; 
  	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit; //32λ 
  	CAN_FilterInitStructure.CAN_FilterIdHigh=0x0000;////32λID
  	CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
  	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x0000;//32λMASK
  	CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;
   	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;//������0������FIFO0
  	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE; //���������0
  	CAN_FilterInit(&CAN_FilterInitStructure);//�˲�����ʼ��
		
	
	  CAN_ITConfig(CAN2,CAN_IT_FMP0,ENABLE);//FIFO0��Ϣ�Һ��ж�����.		    
  
  	NVIC_InitStructure.NVIC_IRQChannel = CAN2_RX0_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;     // �����ȼ�Ϊ1
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;            // �����ȼ�Ϊ0
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);

	return 0;
}   

/*************************************************************************
 * ��������CAN2Send_Msg()
 * ����  ����֡����
 * ����  ��
 * ���  : return 1 ���ͳɹ�  ��return 0  ����ʧ��
 **************************************************************************/		

u8 CAN2Send_Msg(u8* msg,u8 len,u32 exid)
{	
  u8 mbox;
  u16 i=0;
  CanTxMsg TxMessage;
  TxMessage.ExtId=exid;	 // ������չ��ʾ����29λ��
	
	TxMessage.IDE=4;	//0-��׼֡   4-��չ֡
  TxMessage.RTR=0;	// ��Ϣ����Ϊ����֡��һ֡8λ
  TxMessage.DLC=len;							 // ������֡��Ϣ
  for(i=0;i<len;i++)
  TxMessage.Data[i]=msg[i];				 // ��һ֡��Ϣ          
  mbox= CAN_Transmit(CAN2, &TxMessage);   
  i=0;
  while((CAN_TransmitStatus(CAN2, mbox)==CAN_TxStatus_Failed)&&(i<0XFFF))i++;	//�ȴ����ͽ���
  if(i>=0XFFF)return 1;
  return 0;		

}
u16 getu16(u8 highByte, u8 lowByte)
{
    return (((u16)highByte) << 8u) | (u16)lowByte;
}


/**************************************************************
 ** ������ :Candata_analysis()
 ** ����   :����CAN��������
 ** ����   :
 ** ���   :��
***************************************************************/
u8 revframe_num=0;
u8 revdata_len=0;

u16 stat_sys=0;
u16 stat_pfc=0;
u16 stat_inver=0;
u16 warn_pfc=0;
u16 warn_inver=0;
u16 warn_other=0;

u16 crcdata = 0;
void Candata_analysis(u8 *data)
{
	if( (data[0]==PROTOCOL_NOR)&&(data[1]==PTC_TYPE)&&(data[2]==PTC_ADDR) )
	 {
			switch(data[3])
				{
					case FUNC_SEEK:
						
					break;
								
					case FUNC_SETNUM:
						revframe_num=3;
					break;
					
					case FUNC_CONFIG:
						SetPTCData.s_set_statu =SET;
						revframe_num=2;
					break;
					
					case FUNC_LINK: 
						PTCElectData.Output_MAX_i = getu16(data[17], data[18]);
					break;
					
					case FUNC_RCONTROL:
						revframe_num=3;
					break;
					
					case FUNC_SEEKVALUE:
						//System_State=1;
					crcdata = CRC16_XMODEM(data,40); 
					//if(crcdata== getu16(data[40],data[41]))
					if(1)
					{
						revdata_len= getu16(data[10], data[11]);
						PTCElectData.Rectifier_output_v = getu16(data[12], data[13]);
						PTCElectData.Input_v = PTCElectData.Rectifier_output_v;
						PTCElectData.PFCInductor1_i = getu16(data[14], data[15]);
						PTCElectData.Input_i = PTCElectData.PFCInductor1_i;
						PTCElectData.PFCInductor2_i = getu16(data[16], data[17]);
						PTCElectData.BUSoutput_v = getu16(data[18], data[19]);
						PTCElectData.Coil_i = getu16(data[20], data[21]);
						PTCElectData.Output_i = PTCElectData.Coil_i;
						PTCElectData.INVInductor_i = getu16(data[22], data[23]);
						PTCElectData.INV_fre = getu16(data[24], data[25]);
						PTCElectData.Temp_envir = data[28] - 40;
						PTCElectData.Temp_rectify = data[29] - 40;
						PTCElectData.Temp_pfc_mos = data[30] - 40;
						PTCElectData.Temp_INV_mos1 = data[31] - 40;
						PTCElectData.Temp_INV_mos2 = data[32] - 40;					
					}
					break;
					
					case FUNC_SEEKSTAT:
					//revframe_num=6;
					if(CRC16_XMODEM(data,33) == getu16(data[33],data[34]))
					{
						PTCElectData.g_PTC_State = getu16(data[13], data[14]);
						PTCElectData.g_pfc_State = getu16(data[15], data[16]);
						PTCElectData.g_inv_State = getu16(data[17], data[18]);
						PTCElectData.g_PFCAlarm_State = getu16(data[22], data[23]);
						PTCElectData.g_INVAlarm_State = getu16(data[24], data[25]);
					}
					break;
										
					default:
					break;
				}		 
	 }
}


/**************************************************************
 ** ������ :Can_ReadData
 ** ����   :����CAN���ݣ���������
 ** ����   :CanRxMsg msg��CAN���ݰ�
 ** ���   :��
***************************************************************/
u8 can_temp[50]={0};

u8 revframe_pos=0;

void Can_ReadData(CanRxMsg msg)
{
     
	if( flag_canrev==1)
	{
			flag_canrev=0;
			if(msg.Data[0] == 0x00)  
			{
				memcpy(can_temp,&msg.Data[1],7);
				revframe_pos=1;
			}
			else if( (revframe_pos>=1)&&(revframe_pos<0x80) )
      {		
					revframe_pos++;
					memcpy(&can_temp[(revframe_pos-1)*7],&msg.Data[1],7);	
			}	
     if( (msg.Data[0]>=0x80)&&(revframe_pos>=1)&&(revframe_pos < 8))
		  {
					revframe_pos++;
					memcpy(&can_temp[(revframe_pos-1)*7],&msg.Data[1],7);
					Candata_analysis(can_temp);
					revframe_pos=0;	
					memset(can_temp,0,sizeof(can_temp));
			}
	}
	
}

void CAN2_RX0_IRQHandler(void)
{	
  	CanRxMsg RxMessage;

	  OSIntEnter();
    CAN_Receive(CAN2, 0, &RxMessage);
	
	  switch(RxMessage.ExtId)
		{
			case  0x10207EF8: 
				Can_ReadData(RxMessage);
				flag_canrev=1;
			break;
			
			default:
			break;
		}				
	  OSIntExit();    	//�˳��ж�
}


u8 CAN2_Receive_Msg(u8 *buf)
{		   		   
 	u32 i;
	CanRxMsg RxMessage;
    if( CAN_MessagePending(CAN2,CAN_FIFO0)==0)return 0;		//û�н��յ�����,ֱ���˳� 
    CAN_Receive(CAN2, CAN_FIFO0, &RxMessage);//��ȡ����	
    for(i=0;i<RxMessage.DLC;i++)
    buf[i]=RxMessage.Data[i];  
	return RxMessage.DLC;	
}

/**************************************************************
 ** ������ :CRC16_XMODEM()
 ** ����   :x16+x12+x5+1��0x1021�� ��ʼֵ0x0000  ��λ�ں󣬸�λ��ǰ   ��0x0000���
 ** ����   :
 ** ���   :
***************************************************************/
u16 CRC16_XMODEM(u8 *puchMsg, u16 usDataLen)  
{  
  u16 wCRCin = 0x0000;  
  u16 wCPoly = 0x1021;  
  u8 wChar = 0;  
    
  while (usDataLen--)     
  {  
        wChar = *(puchMsg++);  
        wCRCin ^= (wChar << 8);  
        for(int i = 0;i < 8;i++)  
        {  
          if(wCRCin & 0x8000)  
            wCRCin = (wCRCin << 1) ^ wCPoly;  
          else  
            wCRCin = wCRCin << 1;  
        }  
  }  
  return (wCRCin) ;  
}

/**************************************************************
 ** ������ :can2_data_packet
 ** ����   :�����������
 ** ����   :order-ָ�� �����룬append_codel-��������ֽڣ�len_data-���ݳ��ȣ�data-��������
 ** ���   :��
 ** ����   :��֡��
***************************************************************/
u8 can2_tbuf[50]={0};  //0x33     0             14      
u8 can2_data_packet(u8 order, u8 append_codel, u8 len_data ,u8 *data)
{
		u32 crc=0;
    u8 FrameNum=0;//����֡����
		u8 crc_table[50]={0};
	
		memset(can2_tbuf,0,sizeof(can2_tbuf));
		
		can2_tbuf[FrameNum*8+0]=SegNum0;     /// 0x00
		can2_tbuf[FrameNum*8+1]=PROTOCOL_NOR;  // 0xe0
		can2_tbuf[FrameNum*8+2]=CSU_TYPE;       //0x7d
		can2_tbuf[FrameNum*8+3]=CSU_ADDR;      // 0x78
		can2_tbuf[FrameNum*8+4]=order;        // 0x33
		
		switch(order)
		{
			case FUNC_RCONTROL: 
			{
				can2_tbuf[FrameNum*8+5]=APPEND_CODEH;  //ң��֡
				can2_tbuf[FrameNum*8+6]=append_codel;	  // ���߻���
				can2_tbuf[FrameNum*8+7]=RTN_CODE;				// 	������
			}				
			break;
			case FUNC_SEEKSTAT:	   ///// ��ѯ״ֵ̬
			{
				can2_tbuf[FrameNum*8+5]=0x00;
				can2_tbuf[FrameNum*8+6]=0x01;
				can2_tbuf[FrameNum*8+7]=0x00;				
			}				
			break;			
			default:
			{
				can2_tbuf[FrameNum*8+5]=0;
				can2_tbuf[FrameNum*8+6]=0;
				can2_tbuf[FrameNum*8+7]=RTN_CODE;
			}
			break;
		}
		
		FrameNum=PreProcess_CanOrder(order);      //// �������ݳ��� 4   ң���淵��3
		
		switch(FrameNum)
		{
			case 2: 
			{
				can2_tbuf[(FrameNum-1)*8+0]=0x80+FrameNum-1;
				can2_tbuf[(FrameNum-1)*8+1]=SHEET_SN;
				can2_tbuf[(FrameNum-1)*8+2]=REV_DATA1;
				can2_tbuf[(FrameNum-1)*8+3]=REV_DATA0;
				can2_tbuf[(FrameNum-1)*8+4]=len_data>>8;  
				can2_tbuf[(FrameNum-1)*8+5]=len_data&0xff;  
				
				memcpy(&crc_table,&can2_tbuf[(FrameNum-2)*8+1],7);
				memcpy(&crc_table[7],&can2_tbuf[(FrameNum-1)*8+1],5);
				crc=CRC16_XMODEM(crc_table,12);
				
				can2_tbuf[(FrameNum-1)*8+6]=crc>>8;   	 //crc-16 Msb;
				can2_tbuf[(FrameNum-1)*8+7]=crc&0xff;   //crc-16 Lsb;			
			}				
			break;
			case 3:	
			{
				can2_tbuf[(FrameNum-2)*8+0]=0x1;
				can2_tbuf[(FrameNum-2)*8+1]=SHEET_SN;
				can2_tbuf[(FrameNum-2)*8+2]=REV_DATA1;
				can2_tbuf[(FrameNum-2)*8+3]=REV_DATA0;
				can2_tbuf[(FrameNum-2)*8+4]=len_data>>8;
				can2_tbuf[(FrameNum-2)*8+5]=len_data&0xff;
				
				can2_tbuf[(FrameNum-2)*8+6]=*data;   	
				can2_tbuf[(FrameNum-2)*8+7]=*(data+1);   	
				
				can2_tbuf[(FrameNum-1)*8+0]=0x80+FrameNum-1;;
				can2_tbuf[(FrameNum-1)*8+1]=*(data+2); 
				can2_tbuf[(FrameNum-1)*8+2]=*(data+3); 
				can2_tbuf[(FrameNum-1)*8+3]=*(data+4); 
				can2_tbuf[(FrameNum-1)*8+4]=*(data+5);

				memcpy(&crc_table,&can2_tbuf[(FrameNum-3)*8+1],7);
				memcpy(&crc_table[7],&can2_tbuf[(FrameNum-2)*8+1],7);
				memcpy(&crc_table[14],&can2_tbuf[(FrameNum-1)*8+1],4);
				crc=CRC16_XMODEM(crc_table,18);
				
				can2_tbuf[(FrameNum-1)*8+5]= crc>>8;   	
				can2_tbuf[(FrameNum-1)*8+6]= crc&0xff; 
				can2_tbuf[(FrameNum-1)*8+7]=0x0;		
			}				
			break;
			case 4:	        //  FrameNum=4                          //// ��ѯ״ֵ̬
			{
				can2_tbuf[(FrameNum-3)*8+0]=FrameNum-3;   // ��8�ֽ�����
				can2_tbuf[(FrameNum-3)*8+1]=SHEET_SN;
				can2_tbuf[(FrameNum-3)*8+2]=REV_DATA1;
				can2_tbuf[(FrameNum-3)*8+3]=REV_DATA0;
				can2_tbuf[(FrameNum-3)*8+4]=0;
				can2_tbuf[(FrameNum-3)*8+5]=len_data;   //14
				
				can2_tbuf[(FrameNum-3)*8+6]=*data;   	  // 14λ     data 0
				can2_tbuf[(FrameNum-3)*8+7]=*(data+1);   // 15λ    data1
				
				can2_tbuf[(FrameNum-2)*8+0]=FrameNum-2;     
				can2_tbuf[(FrameNum-2)*8+1]=*(data+2);      // data2
				can2_tbuf[(FrameNum-2)*8+2]=*(data+3);
				can2_tbuf[(FrameNum-2)*8+3]=*(data+4);
				can2_tbuf[(FrameNum-2)*8+4]=*(data+5);		  //data5		
				
				can2_tbuf[(FrameNum-1)*8+0]=0x80+FrameNum-1;	
				
				memcpy(&crc_table,&can2_tbuf[(FrameNum-4)*8+1],7);
				memcpy(&crc_table[7],&can2_tbuf[(FrameNum-3)*8+1],7);
				memcpy(&crc_table[14],&can2_tbuf[(FrameNum-2)*8+1],7);
				memcpy(&crc_table[21],&can2_tbuf[(FrameNum-1)*8+1],5);
				
				crc=CRC16_XMODEM(crc_table,26);
								
				can2_tbuf[(FrameNum-1)*8+6]= crc>>8;   
				can2_tbuf[(FrameNum-1)*8+7]= crc&0xff;
			}				
			break;
			case 5:	
			{
				can2_tbuf[(FrameNum-4)*8+0]=FrameNum-4;
				can2_tbuf[(FrameNum-4)*8+1]=SHEET_SN;
				can2_tbuf[(FrameNum-4)*8+2]=REV_DATA1;
				can2_tbuf[(FrameNum-4)*8+3]=REV_DATA0;
				can2_tbuf[(FrameNum-4)*8+4]=len_data>>8;
				can2_tbuf[(FrameNum-4)*8+5]=len_data&0xff;
				
				can2_tbuf[(FrameNum-4)*8+6]=*data;   	
				can2_tbuf[(FrameNum-4)*8+7]=*(data+1);   	
				
				can2_tbuf[(FrameNum-3)*8+0]=FrameNum-3;
				can2_tbuf[(FrameNum-3)*8+1]=*(data+2);
				can2_tbuf[(FrameNum-3)*8+2]=*(data+3);
				
				can2_tbuf[(FrameNum-2)*8+0]=FrameNum-2;
				can2_tbuf[(FrameNum-1)*8+0]=0x80+FrameNum-1;
				
				memcpy(&crc_table,&can2_tbuf[(FrameNum-5)*8+1],7);
				memcpy(&crc_table[7],&can2_tbuf[(FrameNum-4)*8+1],7);
				memcpy(&crc_table[14],&can2_tbuf[(FrameNum-3)*8+1],7);
				memcpy(&crc_table[21],&can2_tbuf[(FrameNum-2)*8+1],7);
				memcpy(&crc_table[28],&can2_tbuf[(FrameNum-1)*8+1],5);
				crc=CRC16_XMODEM(crc_table,33);
								
				can2_tbuf[(FrameNum-1)*8+6]= crc>>8;   
				can2_tbuf[(FrameNum-1)*8+7]= crc&0xff; 		
			}				
			break;			
			default:
			{
			}
			break;
		}
		return FrameNum;
}	

/*************************************************************************
 * ��������Can2Send_MultiMessages()
 * ����  ����֡����
 * ����  �����ݳ���Ϊ8�ı���
 * ���  : return 1 ���ͳɹ�  ��return 0  ����ʧ��
 **************************************************************************/	
u8 Can2Send_MultiMessages(u32 exid, u8 *ptr, u16 len)
{
	u8 i=0;
	
	while(( len - 8 * i) > 0 )
	{		
		#if CANTEST
		CAN1Send_Msg(ptr+i*8,8,exid);  //����8���ֽ� 
		#else
		CAN2Send_Msg(ptr+i*8,8,exid);  //����8���ֽ� 
		#endif
		
		delay_ms(20);
		i++;
	}	
	return 0;
}


/**************************************************************
 ** ������ :PreProcess_CanOrder()
 ** ����   :���·���CANָ�����Ԥ������ȡ���ݳ���
 ** ����   :order
 ** ���   :��
***************************************************************/

u8 PreProcess_CanOrder(u8 order)
{
	u8 len=0;
	u8 num=0;
	
	switch(order)
				{
					case FUNC_SEEK:
							len=0;
					break;

					case FUNC_SETNUM:
							len=6;
					break;
					
					case FUNC_CONFIG:
							len=21;
					break;
					
					case FUNC_LINK:						
							len=0;				
					break;
					
					case FUNC_RCONTROL:
							len=6;
					break;
					
					case FUNC_SEEKVALUE:
							len=0;
					break;
										
					case FUNC_SEEKSTAT:    /////
							len=14;
					break;
					
					default:
					break;
				}
				
		if(((len+14)%7)!=0)
	  num=(len+14)/7+1;
		else 
		num=(len+14)/7;			
		
		return num;
}



u8 CAN_ETH_Init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode)  //CAN2 PB5 PB6
{
  	GPIO_InitTypeDef GPIO_InitStructure; 
	  CAN_InitTypeDef        CAN_InitStructure;
  	CAN_FilterInitTypeDef  CAN_FilterInitStructure;

   	NVIC_InitTypeDef  NVIC_InitStructure;
    //ʹ�����ʱ��
	  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN2, ENABLE);//ʹ��CAN1ʱ��	

    //��ʼ��GPIO
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
    GPIO_Init(GPIOB, &GPIO_InitStructure);
	
		 //���Ÿ���ӳ������
	  GPIO_PinAFConfig(GPIOB, GPIO_PinSource5,GPIO_AF_CAN2);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource6,GPIO_AF_CAN2);
  	//CAN��Ԫ����
   	CAN_InitStructure.CAN_TTCM=DISABLE;	//��ʱ�䴥��ͨ��ģʽ   
  	CAN_InitStructure.CAN_ABOM=ENABLE;	//����Զ����߹���	  
  	CAN_InitStructure.CAN_AWUM=DISABLE;//˯��ģʽͨ���������(���CAN->MCR��SLEEPλ)
  	CAN_InitStructure.CAN_NART=ENABLE;	//��ֹ�����Զ����� 
  	CAN_InitStructure.CAN_RFLM=DISABLE;	//���Ĳ�����,�µĸ��Ǿɵ�  
  	CAN_InitStructure.CAN_TXFP=DISABLE;	//���ȼ��ɱ��ı�ʶ������ 
  	CAN_InitStructure.CAN_Mode= mode;	 //ģʽ���� 
  	CAN_InitStructure.CAN_SJW=tsjw;	//����ͬ����Ծ���(Tsjw)Ϊtsjw+1��ʱ�䵥λ CAN_SJW_1tq~CAN_SJW_4tq
  	CAN_InitStructure.CAN_BS1=tbs1; //Tbs1��ΧCAN_BS1_1tq ~CAN_BS1_16tq
  	CAN_InitStructure.CAN_BS2=tbs2;//Tbs2��ΧCAN_BS2_1tq ~	CAN_BS2_8tq
  	CAN_InitStructure.CAN_Prescaler=brp;  //��Ƶϵ��(Fdiv)Ϊbrp+1	
  	CAN_Init(CAN2, &CAN_InitStructure);   // ��ʼ��CAN1 
    
		//���ù�����
 	  CAN_FilterInitStructure.CAN_FilterNumber=15;	  //������0
  	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; 
  	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit; //32λ 
  	CAN_FilterInitStructure.CAN_FilterIdHigh=0x0000;////32λID
  	CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
  	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x0000;//32λMASK
  	CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;
   	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;//������0������FIFO0
  	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE; //���������0
  	CAN_FilterInit(&CAN_FilterInitStructure);//�˲�����ʼ��
			
	  CAN_ITConfig(CAN2,CAN_IT_FMP0,ENABLE);//FIFO0��Ϣ�Һ��ж�����.		    
  
  	NVIC_InitStructure.NVIC_IRQChannel = CAN2_RX0_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;     // �����ȼ�Ϊ1
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;            // �����ȼ�Ϊ0
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);

	return 0;
}   

static u8 CAN2_Send_Msg(u8* msg,u8 len,u32 exid)
{	
  u8 mbox;
  u16 i=0;
  CanTxMsg TxMessage;
  TxMessage.StdId=0;	 // ��׼��ʶ��Ϊ0
  TxMessage.ExtId=0x3a;	 // ������չ��ʾ����29λ��
	TxMessage.IDE=0;	//0-��׼֡
  TxMessage.RTR=0;		  // ��Ϣ����Ϊ����֡��һ֡8λ
  TxMessage.DLC=len;							 // ������֡��Ϣ
  for(i=0;i<len;i++)
  TxMessage.Data[i]=msg[i];				 // ��һ֡��Ϣ          
  mbox= CAN_Transmit(CAN2, &TxMessage);   
  i=0;
  while((CAN_TransmitStatus(CAN2, mbox)==CAN_TxStatus_Failed)&&(i<0XFFF))i++;	//�ȴ����ͽ���
  if(i>=0XFFF)return 1;
  return 0;		

}

u8  can2_buf[8]={0};
void can_send_eth(void)
{ 
	 	 
 	 can2_buf[0]=1;
   can2_buf[1]=2;
   can2_buf[2]=3;
	 can2_buf[3]=4;
	 can2_buf[4]=5; 
   can2_buf[5]=6;
   can2_buf[6]=7; 
	 can2_buf[7]=8;
	 CAN2_Send_Msg(can2_buf,8,0); 

}

