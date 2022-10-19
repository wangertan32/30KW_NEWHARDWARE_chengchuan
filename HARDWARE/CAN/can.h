#ifndef __CAN_H
#define __CAN_H	 
#include "sys.h"	    
#define CANTEST 1
/**ϵͳ�����ṹ�嶨��**/
/*�汾��Ϣ*/
typedef struct _PTC_VERSION_INF{
	u8 SerialNum[4];//�������к�
	u8 HardwareVer[28];//Ӳ���汾
	u8 PCBVer[28];//Ӳ��PCB�汾
	u8 SoftwareVer[28];//����汾
	u8 Producer[28];//��������
} PTC_VERSION_INF; 
extern PTC_VERSION_INF PTCVersionINF;
/*��ز���*/
/*
��ѹ���ȣ�0.1V��Ʃ��ʵ�������ѹ100V�����ͱ���Ϊ1000��
�������ȣ�0. 01A��Ʃ��ʵ���������10A�����ͱ���Ϊ1000��
Ƶ�ʾ��ȣ�0.01KHZ��Ʃ��ʵ�����Ƶ��10KHz�����ͱ���Ϊ1000��
�¶Ⱦ��ȣ�ʵ���¶�+40�ȡ�Ʃ��ʵ���¶�Ϊ25�ȣ����ͱ���Ϊ65��
*/
typedef struct _PTC_ELECT_DATA{
	u16 Input_v;//�����ѹ
	u16 Input_i;//�������
	u16 Rectifier_output_v;//���������ѹ
	u16 PFCInductor1_i;//PFC��е���1
	u16 PFCInductor2_i;//PFC��е���2
	u16 BUSoutput_v;//BUS�����ѹ
	u16 Coil_i;//��Ȧ����
	u16 INVInductor_i;//����е���
	u16 INV_fre;//��乤��Ƶ��
	u8 	Temp_envir;//�����¶�
	u8 	Temp_rectify;//�������¶�
	u8 	Temp_pfc_mos;//PFC���ʹ��¶�
	u8 	Temp_INV_mos1;//��书�ʹ��¶�1
	u8 	Temp_INV_mos2;//��书�ʹ��¶�2
	u16 Output_v;//�����ѹ
	u16 Output_i;//�������
	u32 Output_p;//�������
	u16 Output_MAX_i;//�����������
	u16 g_PTC_State;//PTC�ϴ�ϵͳ״̬
	u16 g_pfc_State;//PTC�ϴ�PFC״̬��
	u16 g_inv_State;//PTC�ϴ�INV״̬��
	u16 g_PFCAlarm_State;//PTC�ϴ�PFC�澯��
	u16 g_INVAlarm_State;//PTC�ϴ�INV�澯��	
} PTC_ELECT_DATA; 
extern PTC_ELECT_DATA PTCElectData;
/*���ò���*/
/*
��ѹ����: ��λ0.1V������8100��ʾ810V
Ƶ�ʾ���:	��λ0.1Hz������500��ʾ50Hz
*/
typedef struct _SET_PTC_DATA{
	u16 s_output_v;//�����ѹ
	u16 s_output_i;//�������
	u16 s_Bus_v;//ĸ�ߵ�ѹ       
	u16 s_INV_fre;//��乤��Ƶ�� 
	FlagStatus  s_set_statu;//����״̬
} SET_PTC_DATA; 
extern SET_PTC_DATA SetPTCData;
/**CAN֡�ṹ˵��**/
#define PROTOCOL_NOR   0xE0			//����ͨѶ
#define PROTOCOL_UPLOAD   0xE1	//���߸���
#define CSU_TYPE 			 0x7D			//CSU�豸����
#define PTC_TYPE       0x01			//PTC�豸����
#define CSU_ADDR       0x78			//CSU��ַ
#define PTC_ADDR       0x01     //PTC��ַ
#define PRIORITY       0x1

#define FUNC_SEEK     0x20      //��̨��ѯ�������� (PTCӦ��ʱ��ģ������״̬�����澯�����������͸�CSU)
#define FUNC_SETNUM   0x24      //���û���(CSU����PTC����)
#define FUNC_CONFIG   0x26      //���ò���(CSU�·�PTC���в�����Ĭ�ϲ���������ʱ��)
#define FUNC_LINK     0x28      //�������� (����CSU����PTC)
#define FUNC_RCONTROL 0x29			//ң��֡(CSU�·����ߡ����ѡ����ȵ��ٽ�ֹ������)
#define FUNC_GET_INF 	0x31			//��ȡ���Ұ汾
#define FUNC_SEEKVALUE     0x32     //��ѯ����ֵ
#define FUNC_SEEKSTAT      0x33     //��ѯ״ֵ̬
//CANTOOL��ѯ
#define TOOL_INF     			0x30 				//��ȡ��Ϣ
#define TOOL_INF_CODE1     0x01 			//��ȡ������Ϣ������
#define TOOL_INF_CODE2     0x02 			//��ȡ�汾�������

//ң���������
#define APPEND_CODEH   			0x24		//ң�����������ֽ�
#define APPEND_CODEL_STOP   0x04		//PTC����
#define APPEND_CODEL_START  0x05		//PTC����
#define APPEND_CODEL_FANS   0x08		//���ȵ�������
#define APPEND_CODEL_FANP   0x09		//���ȵ��ٽ�ֹ

#define RTN_CODE      0x80  //������
#define SHEET_SN      0x01  //���ݱ����
#define REV_DATA0     0x0   //Ԥ���ֽ�
#define REV_DATA1     0x0		//Ԥ���ֽ�

#define SegNum0     0x0	//�ֶ��ֽ�

extern u32 canid_tx;
extern u32 canid_rx;





//typedef  union {
//			u32 all;
//			struct	{
//								u8 target_add:7;
//								u8 target_type:7;
//								u8 source_add:7;
//								u8 source_type:7;
//								u8 priority:1;
//								u8 reserve:3;
//					}ID_bit;
//			}ID;
//extern ID  canid_tx,canid_rx;

extern u8 can2_tbuf[50];
u16 getu16(u8 highByte, u8 lowByte);
u8 can2_dsp_Init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode);

u8 CAN2Send_Msg(u8* msg,u8 len,u32 exid);
u8 CAN2_Receive_Msg(u8 *buf);							//��������

u8 PreProcess_CanOrder(u8 order);
u8 Can2Send_MultiMessages(u32 exid, u8 *ptr, u16 len);
u8 can2_data_packet(u8 order, u8 append_codel, u8 len_data ,u8 *data);			
void canid_init(void);

u8 CAN1_Mode_Init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode);

u8 CAN_ETH_Init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode) ;
void can_send_eth(void);

#endif

