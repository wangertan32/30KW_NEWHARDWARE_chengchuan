#include "bq32000.h" 
#include "delay.h" 
CSU_TIME CSUtime;
//��ʼ��IIC�ӿ�
void bq32000_Init(void)
{
	IIC_Init();//IIC��ʼ��
}
//��bq32000ָ����ַ����һ������
//ReadAddr:��ʼ�����ĵ�ַ  
//����ֵ  :����������
u8 bq32000_ReadOneByte(u8 ReadAddr)
{				  
	u8 temp=0;		  	    																 
	IIC_Start();  
	
	IIC_Send_Byte(0XD0);	   //����д����
	IIC_Wait_Ack();
	IIC_Send_Byte(ReadAddr);//���͵�ַ	    

	IIC_Wait_Ack();	    
	IIC_Start();  	 	   
	IIC_Send_Byte(0XD1);           //�������ģʽ			   
	IIC_Wait_Ack();	 
	temp=IIC_Read_Byte(0);		   
	IIC_Stop();//����һ��ֹͣ����	    
	return temp;
}
//��bq32000ָ����ַд��һ������
//WriteAddr  :д�����ݵ�Ŀ�ĵ�ַ    
//DataToWrite:Ҫд�������
void bq32000_WriteOneByte(u8 WriteAddr,u8 DataToWrite)
{				   	  	    																 
		IIC_Start();  

		IIC_Send_Byte(0XD0);	    //����д����
		IIC_Wait_Ack();
		IIC_Send_Byte(WriteAddr);//���͵�ַ
		IIC_Wait_Ack();	
		IIC_Send_Byte(DataToWrite);     //�����ֽ�							   
		IIC_Wait_Ack();  		    	   
		IIC_Stop();//����һ��ֹͣ���� 
		delay_ms(10);	 
}
//��ȡbq32000ʱ������
//����ֵ  :����������
u8 bq32000_ReadSeconds(void)
{
	u8 temp;
	u8 TenDigit;
	u8 SingleDigit;
	temp = bq32000_ReadOneByte(BQ_SECONDS);
	TenDigit = ((temp>>4)&0X07)*10;
	SingleDigit = temp&0X0F;
	temp = TenDigit + SingleDigit;
	return temp;
}
//��ȡbq32000ʱ�ӷ���
//����ֵ  :����������
u8 bq32000_ReadMinutes(void)
{
	u8 temp;
	u8 TenDigit;
	u8 SingleDigit;
	temp = bq32000_ReadOneByte(BQ_MINUTES);
	TenDigit = ((temp>>4)&0X07)*10;
	SingleDigit = temp&0X0F;
	temp = TenDigit + SingleDigit;
	return temp;
}
//��ȡbq32000ʱ��Сʱ
//����ֵ  :����������
u8 bq32000_ReadHours(void)
{
	u8 temp;
	u8 TenDigit;
	u8 SingleDigit;
	temp = bq32000_ReadOneByte(BQ_CENT_HOURS);
	TenDigit = ((temp>>4)&0X03)*10;
	SingleDigit = temp&0X0F;
	temp = TenDigit + SingleDigit;
	return temp;
}
//��ȡbq32000ʱ������
//����ֵ  :����������
u8 bq32000_ReadDay(void)
{
	u8 temp;
	u8 SingleDigit;
	temp = bq32000_ReadOneByte(BQ_DAY);
	SingleDigit = temp&0X07;
	temp = SingleDigit;
	return temp;
}
//��ȡbq32000ʱ������
//����ֵ  :����������
u8 bq32000_ReadDate(void)
{
	u8 temp;
	u8 TenDigit;
	u8 SingleDigit;
	temp = bq32000_ReadOneByte(BQ_DATE);
	TenDigit = ((temp>>4)&0X03)*10;
	SingleDigit = temp&0X0F;
	temp = TenDigit + SingleDigit;
	return temp;
}
//��ȡbq32000ʱ���·�
//����ֵ  :����������
u8 bq32000_ReadMonth(void)
{
	u8 temp;
	u8 TenDigit;
	u8 SingleDigit;
	temp = bq32000_ReadOneByte(BQ_MONTH);
	TenDigit = ((temp>>4)&0X01)*10;
	SingleDigit = temp&0X0F;
	temp = TenDigit + SingleDigit;
	return temp;
}
//��ȡbq32000ʱ����
//����ֵ  :����������
u16 bq32000_ReadYears(void)
{
	u8 temp;
	u16 data;
	u16 TenDigit;
	u8 SingleDigit;
	temp = bq32000_ReadOneByte(BQ_YEARS);
	TenDigit = ((temp>>4)&0X0f)*10;
	SingleDigit = temp&0X0F;
	data = TenDigit + SingleDigit;
	return data;
}
//����bq32000ʱ������
//temp:0~59 
void bq32000_ConfigureSeconds(u8 temp)
{
	u8 TenDigit;
	u8 SingleDigit;
	TenDigit = (temp/10)<<4;
	SingleDigit = (temp%10);
	temp = TenDigit | SingleDigit;
	bq32000_WriteOneByte(BQ_SECONDS,temp);
}
//����bq32000ʱ�ӷ���
//temp:0~59 
void bq32000_ConfigureMinutes(u8 temp)
{
	u8 TenDigit;
	u8 SingleDigit;
	TenDigit = (temp/10)<<4;
	SingleDigit = (temp%10);
	temp = TenDigit | SingleDigit;
	bq32000_WriteOneByte(BQ_MINUTES,temp);
}
//����bq32000ʱ��Сʱ
//temp:0~24 
void bq32000_ConfigureHours(u8 temp)
{
	u8 TenDigit;
	u8 SingleDigit;
	TenDigit = (temp/10)<<4;
	SingleDigit = (temp%10);
	temp = TenDigit | SingleDigit;
	bq32000_WriteOneByte(BQ_CENT_HOURS,temp);
}
//����bq32000ʱ������
//temp:1~7 
void bq32000_ConfigureDay(u8 temp)
{
	u8 SingleDigit;
	SingleDigit = temp;
	temp = SingleDigit;
	bq32000_WriteOneByte(BQ_DAY,temp);
}
//����bq32000ʱ������
//temp:1~31 
void bq32000_ConfigureDate(u8 temp)
{
	u8 TenDigit;
	u8 SingleDigit;
	TenDigit = (temp/10)<<4;
	SingleDigit = (temp%10);
	temp = TenDigit | SingleDigit;
	bq32000_WriteOneByte(BQ_DATE,temp);
}
//����bq32000ʱ���·�
//temp:1~12 
void bq32000_ConfigureMonth(u8 temp)
{
	u8 TenDigit;
	u8 SingleDigit;
	TenDigit = (temp/10)<<4;
	SingleDigit = (temp%10);
	temp = TenDigit | SingleDigit;
	bq32000_WriteOneByte(BQ_MONTH,temp);
}
//����bq32000ʱ����
//temp:
void bq32000_ConfigureYears(u16 temp)
{
	u8 data;
	u8 TenDigit;
	u8 SingleDigit;
	TenDigit = (temp/10)<<4;
	SingleDigit = (temp%10);
	data = TenDigit | SingleDigit;
	bq32000_WriteOneByte(BQ_YEARS,data);
}
void Set_CSU_time(void)
{
		 bq32000_WriteOneByte(BQ_CFG2,0X0A);//�������
//	 bq32000_ConfigureYears(22);
//	 bq32000_ConfigureMonth(9);
//	 bq32000_ConfigureDate(6);
//	 bq32000_ConfigureHours(16);
//	 bq32000_ConfigureMinutes(27);
//	 bq32000_ConfigureSeconds(0);
}
void Get_CSU_time(void)
{
	 CSUtime.g_years = bq32000_ReadYears();
	 CSUtime.g_month = bq32000_ReadMonth();
	 CSUtime.g_date = bq32000_ReadDate();
	 CSUtime.g_hours = bq32000_ReadHours();
	 CSUtime.g_minutes = bq32000_ReadMinutes();
	 CSUtime.g_seconds = bq32000_ReadSeconds();
}