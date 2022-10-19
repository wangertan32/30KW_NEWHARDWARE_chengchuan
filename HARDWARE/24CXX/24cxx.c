#include "24cxx.h" 
#include "delay.h" 				 
//初始化IIC接口
void AT24CXX_Init(void)
{
	IIC_Init();//IIC初始化
}
//在AT24CXX指定地址读出一个数据
//ReadAddr:开始读数的地址  
//返回值  :读到的数据
u8 AT24CXX_ReadOneByte(u16 ReadAddr)
{				  
	u8 temp=0;		  	    																 
    IIC_Start();  
	if(EE_TYPE>AT24C16)
	{
		IIC_Send_Byte(0XA0);	   //发送写命令
		IIC_Wait_Ack();
		IIC_Send_Byte(ReadAddr>>8);//发送高地址	    
	}else IIC_Send_Byte(0XA0+((ReadAddr/256)<<1));   //发送器件地址0XA0,写数据 	   
	IIC_Wait_Ack(); 
    IIC_Send_Byte(ReadAddr%256);   //发送低地址
	IIC_Wait_Ack();	    
	IIC_Start();  	 	   
	IIC_Send_Byte(0XA1);           //进入接收模式			   
	IIC_Wait_Ack();	 
    temp=IIC_Read_Byte(0);		   
    IIC_Stop();//产生一个停止条件	    
	return temp;
}
//在AT24CXX指定地址写入一个数据
//WriteAddr  :写入数据的目的地址    
//DataToWrite:要写入的数据
void AT24CXX_WriteOneByte(u16 WriteAddr,u8 DataToWrite)
{				   	  	    																 
    IIC_Start();  
	if(EE_TYPE>AT24C16)
	{
		IIC_Send_Byte(0XA0);	    //发送写命令
		IIC_Wait_Ack();
		IIC_Send_Byte(WriteAddr>>8);//发送高地址	  
	}else IIC_Send_Byte(0XA0+((WriteAddr/256)<<1));   //发送器件地址0XA0,写数据 	 
	IIC_Wait_Ack();	   
    IIC_Send_Byte(WriteAddr%256);   //发送低地址
	IIC_Wait_Ack(); 	 										  		   
	IIC_Send_Byte(DataToWrite);     //发送字节							   
	IIC_Wait_Ack();  		    	   
    IIC_Stop();//产生一个停止条件 
	delay_ms(10);	 
}
//在AT24CXX里面的指定地址开始写入长度为Len的数据
//该函数用于写入16bit或者32bit的数据.
//WriteAddr  :开始写入的地址  
//DataToWrite:数据数组首地址
//Len        :要写入数据的长度2,4
void AT24CXX_WriteLenByte(u16 WriteAddr,u32 DataToWrite,u8 Len)
{  	
	u8 t;
	for(t=0;t<Len;t++)
	{
		AT24CXX_WriteOneByte(WriteAddr+t,(DataToWrite>>(8*t))&0xff);
	}												    
}

//在AT24CXX里面的指定地址开始读出长度为Len的数据
//该函数用于读出16bit或者32bit的数据.
//ReadAddr   :开始读出的地址 
//返回值     :数据
//Len        :要读出数据的长度2,4
u32 AT24CXX_ReadLenByte(u16 ReadAddr,u8 Len)
{  	
	u8 t;
	u32 temp=0;
	for(t=0;t<Len;t++)
	{
		temp<<=8;
		temp+=AT24CXX_ReadOneByte(ReadAddr+Len-t-1); 	 				   
	}
	return temp;												    
}
//检查AT24CXX是否正常
//这里用了24XX的最后一个地址(255)来存储标志字.
//如果用其他24C系列,这个地址要修改
//返回1:检测失败
//返回0:检测成功
u8 AT24CXX_Check(void)
{
	u8 temp;
	temp=AT24CXX_ReadOneByte(255);//避免每次开机都写AT24CXX			   
	if(temp==0X55)return 0;		   
	else//排除第一次初始化的情况
	{
		AT24CXX_WriteOneByte(255,0X55);
	    temp=AT24CXX_ReadOneByte(255);	  
		if(temp==0X55)return 0;
	}
	return 1;											  
}

//在AT24CXX里面的指定地址开始读出指定个数的数据
//ReadAddr :开始读出的地址 对24c02为0~255
//pBuffer  :数据数组首地址
//NumToRead:要读出数据的个数
void AT24CXX_Read(u16 ReadAddr,u8 *pBuffer,u16 NumToRead)
{
	while(NumToRead)
	{
		*pBuffer++=AT24CXX_ReadOneByte(ReadAddr++);	
		NumToRead--;
	}
}  
//在AT24CXX里面的指定地址开始写入指定个数的数据
//WriteAddr :开始写入的地址 对24c02为0~255
//pBuffer   :数据数组首地址
//NumToWrite:要写入数据的个数
void AT24CXX_Write(u16 WriteAddr,u8 *pBuffer,u16 NumToWrite)
{
	while(NumToWrite--)
	{
		AT24CXX_WriteOneByte(WriteAddr,*pBuffer);
		WriteAddr++;
		pBuffer++;
	}
}

void eeprom_buffer_write(u8* p_buffer, u16 write_address, u16 number_of_byte)
{
    u8 number_of_page = 0, number_of_single = 0, address = 0, count = 0;
    
    address = write_address % I2C_PAGE_SIZE;
    count = I2C_PAGE_SIZE - address;
    number_of_page =  number_of_byte / I2C_PAGE_SIZE;
    number_of_single = number_of_byte % I2C_PAGE_SIZE;
    
    /* if write_address is I2C_PAGE_SIZE aligned  */
    if(0 == address){
        while(number_of_page--){
            AT24CXX_Write(write_address,p_buffer, I2C_PAGE_SIZE); 
            write_address +=  I2C_PAGE_SIZE;
            p_buffer += I2C_PAGE_SIZE;
        }
        if(0 != number_of_single){
            AT24CXX_Write(write_address,p_buffer,  number_of_single);
        }      
    }else{
        /* if write_address is not I2C_PAGE_SIZE aligned */
        if(number_of_byte < count){ 
            AT24CXX_Write(write_address,p_buffer, number_of_byte);
        }else{
            number_of_byte -= count;
            number_of_page =  number_of_byte / I2C_PAGE_SIZE;
            number_of_single = number_of_byte % I2C_PAGE_SIZE;
            
            if(0 != count){
                AT24CXX_Write(write_address, p_buffer,  count);
                write_address += count;
                p_buffer += count;
            } 
            /* write page */
            while(number_of_page--){
                AT24CXX_Write(write_address, p_buffer,  I2C_PAGE_SIZE);
                write_address +=  I2C_PAGE_SIZE;
                p_buffer += I2C_PAGE_SIZE;
            }
            /* write single */
            if(0 != number_of_single){
                AT24CXX_Write(write_address, p_buffer,  number_of_single); 
            }
        }
    }  
}
#define EEPROM_DEV_ADDR 			0xA0
#define EEPROM_DEV_ADDR_Page 	0xB0
#define I2C_WR 	0x00
#define I2C_RD 	0x01
#define EEPROM_PAGE_SIZE	64//M24256有256kbit空间，每一页的大小Page Size =64，所以计算可得页的取值范围为：256*1024/8/64 = 512 页。
uint8_t W24256_Byte_Write(uint16_t address,uint8_t WriteData)
{
    uint16_t usAddr;
    usAddr = address; 
	
    /* 第1步：发起I2C总线启动信号 */
    IIC_Start();
	
    /* 第2步：发起控制字节，高7bit是地址，bit0是读写控制位，0表示写，1表示读 */
    IIC_Send_Byte(EEPROM_DEV_ADDR | I2C_WR);    /* 此处是写指令 */
	
    /*第3步：等待ACK信号*/
    if (IIC_Wait_Ack() != 0)
    {
        goto cmd_fail;  /* EEPROM器件无应答 */
    }

    /* 第4步：发送字节地址，发送地址直接高八位*/
    IIC_Send_Byte((uint8_t)(usAddr>> 8));

    /*第5步：等待ACK信号*/
    if (IIC_Wait_Ack() != 0)
    {
        goto cmd_fail;  /* EEPROM器件无应答 */

    }

    /* 第6步：发送字节地址，发送地址直接低八位*/
    IIC_Send_Byte((uint8_t)(usAddr&0x00ff));

    /*第7步：等待ACK信号*/
    if (IIC_Wait_Ack() != 0)
    {
        goto cmd_fail;  /* EEPROM器件无应答 */
    }

    /* 第8步：开始写入数据 */
    IIC_Send_Byte(WriteData);

    /*第9步：等待ACK信号*/
    if (IIC_Wait_Ack() != 0)
    {
        goto cmd_fail;  /* EEPROM器件无应答 */
    }
		
    IIC_Stop();

    return 1;

cmd_fail: /* 命令执行失败后，切记发送停止信号，避免影响I2C总线上其他设备 */

    /* 发送I2C总线停止信号 */
    IIC_Stop();
		
    return 0;
}
    /*

     * 写串行EEPROM不像读操作可以连续读取很多字节，每次写操作只能在同一个page。

     * 对于24xx02，page size = 8

     * 简单的处理方法为：按字节写操作模式，没写1个字节，都发送地址

     * 为了提高连续写的效率: 本函数采用page wirte操作。

     */
uint8_t W24256_Page_Write(uint8_t *WriteBuf, uint16_t Page, uint16_t Size)
{
    uint16_t i,m;
    uint16_t usAddr;
    usAddr = Page * 0x0040;

    for (i = 0; i < Size; i++)
    {
        /* 当发送第1个字节或是页面首地址时，需要重新发起启动信号和地址 */

        if ((i == 0) || (usAddr & (EEPROM_PAGE_SIZE - 1)) == 0)
        {

//          /*　第０步：发停止信号，启动内部写操作　*/

//          IIC_Stop();

              

            /* 通过检查器件应答的方式，判断内部写操作是否完成, 一般小于 10ms            

                CLK频率为200KHz时，查询次数为30次左右

            */
            for (m = 0; m < 1000; m++)
            {   
							
                /* 第1步：发起I2C总线启动信号 */
                IIC_Start();

                /* 第2步：发起控制字节，高7bit是地址，bit0是读写控制位，0表示写，1表示读 */
                IIC_Send_Byte(EEPROM_DEV_ADDR | I2C_WR);    /* 此处是写指令 */

                /* 第3步：发送一个时钟，判断器件是否正确应答 */
                if (IIC_Wait_Ack() == 0)
                {
                    break;
                }
            }
            if (m  == 1000)
            {
                goto cmd_fail;  /* EEPROM器件写超时 */
            }

            /* 第4步：发送字节地址，发送地址直接高八位*/
            IIC_Send_Byte((uint8_t)(usAddr>> 8));

            /*第五步：等待ACK信号*/
            if (IIC_Wait_Ack() != 0)
            {
                goto cmd_fail;  /* EEPROM器件无应答 */
            }

            /* 第6步：发送字节地址，发送地址直接低八位*/
            IIC_Send_Byte((uint8_t)(usAddr&0x00ff));

            /* 第5步：等待ACK */
            if (IIC_Wait_Ack() != 0)
            {
                goto cmd_fail;  /* EEPROM器件无应答 */
            }

        }

        /* 第6步：开始写入数据 */
        IIC_Send_Byte(WriteBuf[i]);

        /* 第7步：发送ACK */
        if (IIC_Wait_Ack() != 0)
        {
            goto cmd_fail;  /* EEPROM器件无应答 */
        }
        usAddr++;   /* 地址增1 */      
    }

      

    /* 命令执行成功，发送I2C总线停止信号 */
    IIC_Stop();
		
    return 1;
cmd_fail: /* 命令执行失败后，切记发送停止信号，避免影响I2C总线上其他设备 */

    /* 发送I2C总线停止信号 */

    IIC_Stop();

    return 0;

}
uint8_t W24256_Byte_Read( uint16_t Address)
{
    uint16_t uAddress;
    uint8_t ReadData;
    uAddress = Address;
    /* 第1步：发起I2C总线启动信号 */
    IIC_Start();    

    /* 第2步：发起控制字节，高7bit是地址，bit0是读写控制位，0表示写，1表示读 */
    IIC_Send_Byte(EEPROM_DEV_ADDR | I2C_WR);    /* 此处是写指令 */  

    /* 第3步：等待ACK */
    if (IIC_Wait_Ack() != 0)
    {
        IIC_Stop(); /* EEPROM器件无应答 */
    }

    /* 第4步：发送字节地址高8位 */
    IIC_Send_Byte((uint8_t)(uAddress>>8));  

    /* 第5步：等待ACK */
    if (IIC_Wait_Ack() != 0)
    {
        IIC_Stop(); /* EEPROM器件无应答 */
    }  

    /* 第6步：发送字节地址低八位 */
    IIC_Send_Byte((uint8_t)uAddress&0xff);  
    
    /* 第7步：等待ACK */
    if (IIC_Wait_Ack() != 0)
    {
        IIC_Stop(); /* EEPROM器件无应答 */
    }

    /* 第8步：重新启动I2C总线。前面的代码的目的向EEPROM传送地址，下面开始读取数据 */
    IIC_Start();    

    /* 第9步：发起控制字节，高7bit是地址，bit0是读写控制位，0表示写，1表示读 */
    IIC_Send_Byte(EEPROM_DEV_ADDR | I2C_RD);    /* 此处是读指令 */
      
    /* 第10步：发送ACK */
    if (IIC_Wait_Ack() != 0)
    {
       IIC_Stop(); /* EEPROM器件无应答 */
    } 
		
    /* 第11步：读取数据并保存*/
    ReadData = IIC_Read_Byte(0); /* 读1个字节 */
		
    /*第12步：发送NACK命令*/    
    IIC_NAck(); /* 最后1个字节读完后，CPU产生NACK信号(驱动SDA = 1) */

    /*第13步：发送I2C总线停止信号 */
    IIC_Stop();

    return ReadData;    /* 执行成功 */
}
/**

* 函数功能: 从串行EEPROM指定地址处开始读取若干数据

* 输入参数: ReadBuf : 存放读到的数据的缓冲区指针

*           Address : 起始地址  

*           Size : 数据长度，单位为字节

* 返 回 值:  0 表示失败，1表示成功

* 说    明：无

*/

uint8_t W24256_Page_Read(uint8_t *ReadBuf, uint16_t Page, uint16_t Size)
{
    uint16_t i;
    uint16_t uAddress;

    /* 采用串行EEPROM随即读取指令序列，连续读取若干字节 */
    uAddress = Page * 0x0040;

    /* 第1步：发起I2C总线启动信号 */
    IIC_Start();    

    /* 第2步：发起控制字节，高7bit是地址，bit0是读写控制位，0表示写，1表示读 */
    IIC_Send_Byte(EEPROM_DEV_ADDR_Page | I2C_WR);   /* 此处是写指令 */  

    /* 第3步：等待ACK */
    if (IIC_Wait_Ack() != 0)
    {
        goto cmd_fail;  /* EEPROM器件无应答 */
    }

    /* 第4步：发送字节地址高8位 */
    IIC_Send_Byte((uint8_t)(uAddress>>8));  

    /* 第5步：等待ACK */
    if (IIC_Wait_Ack() != 0)
    {
        goto cmd_fail;  /* EEPROM器件无应答 */
    }  

    /* 第6步：发送字节地址低八位 */
    IIC_Send_Byte((uint8_t)uAddress&0xff);  
    
    /* 第7步：等待ACK */
    if (IIC_Wait_Ack() != 0)
    {
        goto cmd_fail;  /* EEPROM器件无应答 */
    }

    /* 第8步：重新启动I2C总线。前面的代码的目的向EEPROM传送地址，下面开始读取数据 */
    IIC_Start();    

    /* 第7步：发起控制字节，高7bit是地址，bit0是读写控制位，0表示写，1表示读 */
    IIC_Send_Byte(EEPROM_DEV_ADDR_Page | I2C_RD);   /* 此处是读指令 */

    /* 第8步：发送ACK */
    if (IIC_Wait_Ack() != 0)
    {
        goto cmd_fail;  /* EEPROM器件无应答 */
    }  

    /* 第9步：循环读取数据 */
    for (i = 0; i < Size; i++)
    {
        ReadBuf[i] = IIC_Read_Byte(0);   /* 读1个字节 */
        /* 每读完1个字节后，需要发送Ack， 最后一个字节不需要Ack，发Nack */
        if (i != Size - 1)
        {
            IIC_Ack();  /* 中间字节读完后，CPU产生ACK信号(驱动SDA = 0) */
        }
        else
        {
            IIC_NAck(); /* 最后1个字节读完后，CPU产生NACK信号(驱动SDA = 1) */
        }

    }

    /* 发送I2C总线停止信号 */

    IIC_Stop();

    return 1;   /* 执行成功 */

   

cmd_fail: /* 命令执行失败后，切记发送停止信号，避免影响I2C总线上其他设备 */

    /* 发送I2C总线停止信号 */

    IIC_Stop();

    return 0;

}
//在W24256里面的指定地址开始写入指定个数的数据
//WriteAddr :开始写入的地址 
//pBuffer   :数据数组首地址
//NumToWrite:要写入数据的个数
void W24256_Write(u16 WriteAddr,u8 *pBuffer,u16 NumToWrite)
{
	while(NumToWrite--)
	{
		W24256_Byte_Write(WriteAddr,*pBuffer);
		WriteAddr++;
		pBuffer++;
	}
}
//在W24256里面的指定地址开始读出指定个数的数据
//ReadAddr :开始读出的地址 
//pBuffer  :数据数组首地址
//NumToRead:要读出数据的个数
void W24256_Read(u16 ReadAddr,u8 *pBuffer,u16 NumToRead)
{
	while(NumToRead)
	{
		*pBuffer++=W24256_Byte_Read(ReadAddr++);	
		NumToRead--;
	}
}  
/*!
    \brief      read data from the EEPROM
    \param[in]  p_buffer: pointer to the buffer that receives the data read from the EEPROM
    \param[in]  read_address: EEPROM's internal address to start reading from
    \param[in]  number_of_byte: number of bytes to reads from the EEPROM
    \param[out] none
    \retval     none
*/
void eeprom_buffer_read(uint8_t* p_buffer, uint8_t read_address, uint16_t number_of_byte)
{  
		W24256_Read(read_address, p_buffer, number_of_byte); 
}


u8 i2c_m24256_test(void)
{
    u16 i;
    u8 i2c_buffer_write[BUFFER_SIZE];
    u8 i2c_buffer_read[BUFFER_SIZE];
    

    
    /* initialize i2c_buffer_write */
    for(i = 0;i < BUFFER_SIZE;i++){ 
        i2c_buffer_write[i]=i;
        if(15 == i%16){
        }
    }
		
			eeprom_buffer_write(i2c_buffer_write,EEP_FIRST_PAGE, BUFFER_SIZE); 
			AT24CXX_Read(EEP_FIRST_PAGE,i2c_buffer_read, BUFFER_SIZE); 
		
    /* EEPROM data write */
//    W24256_Write(EEP_FIRST_PAGE,i2c_buffer_write, BUFFER_SIZE); 
//		W24256_Page_Write(i2c_buffer_write,EEP_FIRST_PAGE,BUFFER_SIZE);
    /* EEPROM data read */
//		W24256_Page_Read(i2c_buffer_read,EEP_FIRST_PAGE,BUFFER_SIZE);
//    W24256_Read(EEP_FIRST_PAGE, i2c_buffer_read, BUFFER_SIZE); 
    /* compare the read buffer and write buffer */
    for(i = 0;i < BUFFER_SIZE;i++){
        if(i2c_buffer_read[i] != i2c_buffer_write[i]){
            return I2C_FAIL;
        }
        if(15 == i%16){
        }
    }
    return I2C_OK;
}


