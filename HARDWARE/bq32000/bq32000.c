#include "bq32000.h" 
#include "delay.h" 
CSU_TIME CSUtime;
//初始化IIC接口
void bq32000_Init(void)
{
	IIC_Init();//IIC初始化
}
//在bq32000指定地址读出一个数据
//ReadAddr:开始读数的地址  
//返回值  :读到的数据
u8 bq32000_ReadOneByte(u8 ReadAddr)
{				  
	u8 temp=0;		  	    																 
	IIC_Start();  
	
	IIC_Send_Byte(0XD0);	   //发送写命令
	IIC_Wait_Ack();
	IIC_Send_Byte(ReadAddr);//发送地址	    

	IIC_Wait_Ack();	    
	IIC_Start();  	 	   
	IIC_Send_Byte(0XD1);           //进入接收模式			   
	IIC_Wait_Ack();	 
	temp=IIC_Read_Byte(0);		   
	IIC_Stop();//产生一个停止条件	    
	return temp;
}
//在bq32000指定地址写入一个数据
//WriteAddr  :写入数据的目的地址    
//DataToWrite:要写入的数据
void bq32000_WriteOneByte(u8 WriteAddr,u8 DataToWrite)
{				   	  	    																 
		IIC_Start();  

		IIC_Send_Byte(0XD0);	    //发送写命令
		IIC_Wait_Ack();
		IIC_Send_Byte(WriteAddr);//发送地址
		IIC_Wait_Ack();	
		IIC_Send_Byte(DataToWrite);     //发送字节							   
		IIC_Wait_Ack();  		    	   
		IIC_Stop();//产生一个停止条件 
		delay_ms(10);	 
}
//读取bq32000时钟秒数
//返回值  :读到的数据
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
//读取bq32000时钟分钟
//返回值  :读到的数据
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
//读取bq32000时钟小时
//返回值  :读到的数据
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
//读取bq32000时钟星期
//返回值  :读到的数据
u8 bq32000_ReadDay(void)
{
	u8 temp;
	u8 SingleDigit;
	temp = bq32000_ReadOneByte(BQ_DAY);
	SingleDigit = temp&0X07;
	temp = SingleDigit;
	return temp;
}
//读取bq32000时钟日期
//返回值  :读到的数据
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
//读取bq32000时钟月份
//返回值  :读到的数据
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
//读取bq32000时钟年
//返回值  :读到的数据
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
//配置bq32000时钟秒数
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
//配置bq32000时钟分钟
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
//配置bq32000时钟小时
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
//配置bq32000时钟星期
//temp:1~7 
void bq32000_ConfigureDay(u8 temp)
{
	u8 SingleDigit;
	SingleDigit = temp;
	temp = SingleDigit;
	bq32000_WriteOneByte(BQ_DAY,temp);
}
//配置bq32000时钟日期
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
//配置bq32000时钟月份
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
//配置bq32000时钟年
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
		 bq32000_WriteOneByte(BQ_CFG2,0X0A);//开启电池
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