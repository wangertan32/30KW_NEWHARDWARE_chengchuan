#ifndef __BQ32000_H
#define __BQ32000_H
#include "myiic.h"  
/*ʱ�����*/
typedef struct _CSU_TIME{
	u16 g_years;//��
	u8 	g_month;//��
	u8 	g_date;//��
	u8 	g_hours;//ʱ
	u8 	g_minutes;//��
	u8 	g_seconds;//��
} CSU_TIME; 
extern CSU_TIME CSUtime;

/*Normal Registers*/
#define 	BQ_SECONDS				0x00		//ʱ��������STOPλ
#define 	BQ_MINUTES				0x01		//ʱ�ӷ���
#define 	BQ_CENT_HOURS			0x02		//ʱ��Сʱ�����ͺ�CENT_ENλ
#define 	BQ_DAY						0x03		//ʱ������
#define 	BQ_DATE						0x04		//ʱ������
#define 	BQ_MONTH					0x05		//ʱ���·�
#define 	BQ_YEARS					0x06		//ʱ����
#define 	BQ_CAL_CFG1				0x07		//У׼������
#define 	BQ_TCH2						0x08		//�������ʽ���
#define 	BQ_CFG2						0x09		//
/*Special Function Registers*/
#define 	BQ_SF_KEY_1				0x20		//
#define 	BQ_SF_KEY_2				0x21		//
#define 	BQ_SFR						0x22		//

void bq32000_Init(void);
u8 bq32000_ReadOneByte(u8 ReadAddr);
void bq32000_WriteOneByte(u8 WriteAddr,u8 DataToWrite);
u8 bq32000_ReadSeconds(void);
u8 bq32000_ReadMinutes(void);
u8 bq32000_ReadHours(void);
u8 bq32000_ReadDay(void);
u8 bq32000_ReadDate(void);
u8 bq32000_ReadMonth(void);
u16 bq32000_ReadYears(void);
void bq32000_ConfigureSeconds(u8 temp);
void bq32000_ConfigureMinutes(u8 temp);
void bq32000_ConfigureHours(u8 temp);
void bq32000_ConfigureDay(u8 temp);
void bq32000_ConfigureDate(u8 temp);
void bq32000_ConfigureMonth(u8 temp);
void bq32000_ConfigureYears(u16 temp);
void Set_CSU_time(void);
void Get_CSU_time(void);
#endif

