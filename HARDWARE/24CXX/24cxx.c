#include "24cxx.h" 
#include "delay.h" 				 
//��ʼ��IIC�ӿ�
void AT24CXX_Init(void)
{
	IIC_Init();//IIC��ʼ��
}
//��AT24CXXָ����ַ����һ������
//ReadAddr:��ʼ�����ĵ�ַ  
//����ֵ  :����������
u8 AT24CXX_ReadOneByte(u16 ReadAddr)
{				  
	u8 temp=0;		  	    																 
    IIC_Start();  
	if(EE_TYPE>AT24C16)
	{
		IIC_Send_Byte(0XA0);	   //����д����
		IIC_Wait_Ack();
		IIC_Send_Byte(ReadAddr>>8);//���͸ߵ�ַ	    
	}else IIC_Send_Byte(0XA0+((ReadAddr/256)<<1));   //����������ַ0XA0,д���� 	   
	IIC_Wait_Ack(); 
    IIC_Send_Byte(ReadAddr%256);   //���͵͵�ַ
	IIC_Wait_Ack();	    
	IIC_Start();  	 	   
	IIC_Send_Byte(0XA1);           //�������ģʽ			   
	IIC_Wait_Ack();	 
    temp=IIC_Read_Byte(0);		   
    IIC_Stop();//����һ��ֹͣ����	    
	return temp;
}
//��AT24CXXָ����ַд��һ������
//WriteAddr  :д�����ݵ�Ŀ�ĵ�ַ    
//DataToWrite:Ҫд�������
void AT24CXX_WriteOneByte(u16 WriteAddr,u8 DataToWrite)
{				   	  	    																 
    IIC_Start();  
	if(EE_TYPE>AT24C16)
	{
		IIC_Send_Byte(0XA0);	    //����д����
		IIC_Wait_Ack();
		IIC_Send_Byte(WriteAddr>>8);//���͸ߵ�ַ	  
	}else IIC_Send_Byte(0XA0+((WriteAddr/256)<<1));   //����������ַ0XA0,д���� 	 
	IIC_Wait_Ack();	   
    IIC_Send_Byte(WriteAddr%256);   //���͵͵�ַ
	IIC_Wait_Ack(); 	 										  		   
	IIC_Send_Byte(DataToWrite);     //�����ֽ�							   
	IIC_Wait_Ack();  		    	   
    IIC_Stop();//����һ��ֹͣ���� 
	delay_ms(10);	 
}
//��AT24CXX�����ָ����ַ��ʼд�볤��ΪLen������
//�ú�������д��16bit����32bit������.
//WriteAddr  :��ʼд��ĵ�ַ  
//DataToWrite:���������׵�ַ
//Len        :Ҫд�����ݵĳ���2,4
void AT24CXX_WriteLenByte(u16 WriteAddr,u32 DataToWrite,u8 Len)
{  	
	u8 t;
	for(t=0;t<Len;t++)
	{
		AT24CXX_WriteOneByte(WriteAddr+t,(DataToWrite>>(8*t))&0xff);
	}												    
}

//��AT24CXX�����ָ����ַ��ʼ��������ΪLen������
//�ú������ڶ���16bit����32bit������.
//ReadAddr   :��ʼ�����ĵ�ַ 
//����ֵ     :����
//Len        :Ҫ�������ݵĳ���2,4
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
//���AT24CXX�Ƿ�����
//��������24XX�����һ����ַ(255)���洢��־��.
//���������24Cϵ��,�����ַҪ�޸�
//����1:���ʧ��
//����0:���ɹ�
u8 AT24CXX_Check(void)
{
	u8 temp;
	temp=AT24CXX_ReadOneByte(255);//����ÿ�ο�����дAT24CXX			   
	if(temp==0X55)return 0;		   
	else//�ų���һ�γ�ʼ�������
	{
		AT24CXX_WriteOneByte(255,0X55);
	    temp=AT24CXX_ReadOneByte(255);	  
		if(temp==0X55)return 0;
	}
	return 1;											  
}

//��AT24CXX�����ָ����ַ��ʼ����ָ������������
//ReadAddr :��ʼ�����ĵ�ַ ��24c02Ϊ0~255
//pBuffer  :���������׵�ַ
//NumToRead:Ҫ�������ݵĸ���
void AT24CXX_Read(u16 ReadAddr,u8 *pBuffer,u16 NumToRead)
{
	while(NumToRead)
	{
		*pBuffer++=AT24CXX_ReadOneByte(ReadAddr++);	
		NumToRead--;
	}
}  
//��AT24CXX�����ָ����ַ��ʼд��ָ������������
//WriteAddr :��ʼд��ĵ�ַ ��24c02Ϊ0~255
//pBuffer   :���������׵�ַ
//NumToWrite:Ҫд�����ݵĸ���
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
#define EEPROM_PAGE_SIZE	64//M24256��256kbit�ռ䣬ÿһҳ�Ĵ�СPage Size =64�����Լ���ɵ�ҳ��ȡֵ��ΧΪ��256*1024/8/64 = 512 ҳ��
uint8_t W24256_Byte_Write(uint16_t address,uint8_t WriteData)
{
    uint16_t usAddr;
    usAddr = address; 
	
    /* ��1��������I2C���������ź� */
    IIC_Start();
	
    /* ��2������������ֽڣ���7bit�ǵ�ַ��bit0�Ƕ�д����λ��0��ʾд��1��ʾ�� */
    IIC_Send_Byte(EEPROM_DEV_ADDR | I2C_WR);    /* �˴���дָ�� */
	
    /*��3�����ȴ�ACK�ź�*/
    if (IIC_Wait_Ack() != 0)
    {
        goto cmd_fail;  /* EEPROM������Ӧ�� */
    }

    /* ��4���������ֽڵ�ַ�����͵�ֱַ�Ӹ߰�λ*/
    IIC_Send_Byte((uint8_t)(usAddr>> 8));

    /*��5�����ȴ�ACK�ź�*/
    if (IIC_Wait_Ack() != 0)
    {
        goto cmd_fail;  /* EEPROM������Ӧ�� */

    }

    /* ��6���������ֽڵ�ַ�����͵�ֱַ�ӵͰ�λ*/
    IIC_Send_Byte((uint8_t)(usAddr&0x00ff));

    /*��7�����ȴ�ACK�ź�*/
    if (IIC_Wait_Ack() != 0)
    {
        goto cmd_fail;  /* EEPROM������Ӧ�� */
    }

    /* ��8������ʼд������ */
    IIC_Send_Byte(WriteData);

    /*��9�����ȴ�ACK�ź�*/
    if (IIC_Wait_Ack() != 0)
    {
        goto cmd_fail;  /* EEPROM������Ӧ�� */
    }
		
    IIC_Stop();

    return 1;

cmd_fail: /* ����ִ��ʧ�ܺ��мǷ���ֹͣ�źţ�����Ӱ��I2C�����������豸 */

    /* ����I2C����ֹͣ�ź� */
    IIC_Stop();
		
    return 0;
}
    /*

     * д����EEPROM�������������������ȡ�ܶ��ֽڣ�ÿ��д����ֻ����ͬһ��page��

     * ����24xx02��page size = 8

     * �򵥵Ĵ�����Ϊ�����ֽ�д����ģʽ��ûд1���ֽڣ������͵�ַ

     * Ϊ���������д��Ч��: ����������page wirte������

     */
uint8_t W24256_Page_Write(uint8_t *WriteBuf, uint16_t Page, uint16_t Size)
{
    uint16_t i,m;
    uint16_t usAddr;
    usAddr = Page * 0x0040;

    for (i = 0; i < Size; i++)
    {
        /* �����͵�1���ֽڻ���ҳ���׵�ַʱ����Ҫ���·��������źź͵�ַ */

        if ((i == 0) || (usAddr & (EEPROM_PAGE_SIZE - 1)) == 0)
        {

//          /*���ڣ�������ֹͣ�źţ������ڲ�д������*/

//          IIC_Stop();

              

            /* ͨ���������Ӧ��ķ�ʽ���ж��ڲ�д�����Ƿ����, һ��С�� 10ms            

                CLKƵ��Ϊ200KHzʱ����ѯ����Ϊ30������

            */
            for (m = 0; m < 1000; m++)
            {   
							
                /* ��1��������I2C���������ź� */
                IIC_Start();

                /* ��2������������ֽڣ���7bit�ǵ�ַ��bit0�Ƕ�д����λ��0��ʾд��1��ʾ�� */
                IIC_Send_Byte(EEPROM_DEV_ADDR | I2C_WR);    /* �˴���дָ�� */

                /* ��3��������һ��ʱ�ӣ��ж������Ƿ���ȷӦ�� */
                if (IIC_Wait_Ack() == 0)
                {
                    break;
                }
            }
            if (m  == 1000)
            {
                goto cmd_fail;  /* EEPROM����д��ʱ */
            }

            /* ��4���������ֽڵ�ַ�����͵�ֱַ�Ӹ߰�λ*/
            IIC_Send_Byte((uint8_t)(usAddr>> 8));

            /*���岽���ȴ�ACK�ź�*/
            if (IIC_Wait_Ack() != 0)
            {
                goto cmd_fail;  /* EEPROM������Ӧ�� */
            }

            /* ��6���������ֽڵ�ַ�����͵�ֱַ�ӵͰ�λ*/
            IIC_Send_Byte((uint8_t)(usAddr&0x00ff));

            /* ��5�����ȴ�ACK */
            if (IIC_Wait_Ack() != 0)
            {
                goto cmd_fail;  /* EEPROM������Ӧ�� */
            }

        }

        /* ��6������ʼд������ */
        IIC_Send_Byte(WriteBuf[i]);

        /* ��7��������ACK */
        if (IIC_Wait_Ack() != 0)
        {
            goto cmd_fail;  /* EEPROM������Ӧ�� */
        }
        usAddr++;   /* ��ַ��1 */      
    }

      

    /* ����ִ�гɹ�������I2C����ֹͣ�ź� */
    IIC_Stop();
		
    return 1;
cmd_fail: /* ����ִ��ʧ�ܺ��мǷ���ֹͣ�źţ�����Ӱ��I2C�����������豸 */

    /* ����I2C����ֹͣ�ź� */

    IIC_Stop();

    return 0;

}
uint8_t W24256_Byte_Read( uint16_t Address)
{
    uint16_t uAddress;
    uint8_t ReadData;
    uAddress = Address;
    /* ��1��������I2C���������ź� */
    IIC_Start();    

    /* ��2������������ֽڣ���7bit�ǵ�ַ��bit0�Ƕ�д����λ��0��ʾд��1��ʾ�� */
    IIC_Send_Byte(EEPROM_DEV_ADDR | I2C_WR);    /* �˴���дָ�� */  

    /* ��3�����ȴ�ACK */
    if (IIC_Wait_Ack() != 0)
    {
        IIC_Stop(); /* EEPROM������Ӧ�� */
    }

    /* ��4���������ֽڵ�ַ��8λ */
    IIC_Send_Byte((uint8_t)(uAddress>>8));  

    /* ��5�����ȴ�ACK */
    if (IIC_Wait_Ack() != 0)
    {
        IIC_Stop(); /* EEPROM������Ӧ�� */
    }  

    /* ��6���������ֽڵ�ַ�Ͱ�λ */
    IIC_Send_Byte((uint8_t)uAddress&0xff);  
    
    /* ��7�����ȴ�ACK */
    if (IIC_Wait_Ack() != 0)
    {
        IIC_Stop(); /* EEPROM������Ӧ�� */
    }

    /* ��8������������I2C���ߡ�ǰ��Ĵ����Ŀ����EEPROM���͵�ַ�����濪ʼ��ȡ���� */
    IIC_Start();    

    /* ��9������������ֽڣ���7bit�ǵ�ַ��bit0�Ƕ�д����λ��0��ʾд��1��ʾ�� */
    IIC_Send_Byte(EEPROM_DEV_ADDR | I2C_RD);    /* �˴��Ƕ�ָ�� */
      
    /* ��10��������ACK */
    if (IIC_Wait_Ack() != 0)
    {
       IIC_Stop(); /* EEPROM������Ӧ�� */
    } 
		
    /* ��11������ȡ���ݲ�����*/
    ReadData = IIC_Read_Byte(0); /* ��1���ֽ� */
		
    /*��12��������NACK����*/    
    IIC_NAck(); /* ���1���ֽڶ����CPU����NACK�ź�(����SDA = 1) */

    /*��13��������I2C����ֹͣ�ź� */
    IIC_Stop();

    return ReadData;    /* ִ�гɹ� */
}
/**

* ��������: �Ӵ���EEPROMָ����ַ����ʼ��ȡ��������

* �������: ReadBuf : ��Ŷ��������ݵĻ�����ָ��

*           Address : ��ʼ��ַ  

*           Size : ���ݳ��ȣ���λΪ�ֽ�

* �� �� ֵ:  0 ��ʾʧ�ܣ�1��ʾ�ɹ�

* ˵    ������

*/

uint8_t W24256_Page_Read(uint8_t *ReadBuf, uint16_t Page, uint16_t Size)
{
    uint16_t i;
    uint16_t uAddress;

    /* ���ô���EEPROM�漴��ȡָ�����У�������ȡ�����ֽ� */
    uAddress = Page * 0x0040;

    /* ��1��������I2C���������ź� */
    IIC_Start();    

    /* ��2������������ֽڣ���7bit�ǵ�ַ��bit0�Ƕ�д����λ��0��ʾд��1��ʾ�� */
    IIC_Send_Byte(EEPROM_DEV_ADDR_Page | I2C_WR);   /* �˴���дָ�� */  

    /* ��3�����ȴ�ACK */
    if (IIC_Wait_Ack() != 0)
    {
        goto cmd_fail;  /* EEPROM������Ӧ�� */
    }

    /* ��4���������ֽڵ�ַ��8λ */
    IIC_Send_Byte((uint8_t)(uAddress>>8));  

    /* ��5�����ȴ�ACK */
    if (IIC_Wait_Ack() != 0)
    {
        goto cmd_fail;  /* EEPROM������Ӧ�� */
    }  

    /* ��6���������ֽڵ�ַ�Ͱ�λ */
    IIC_Send_Byte((uint8_t)uAddress&0xff);  
    
    /* ��7�����ȴ�ACK */
    if (IIC_Wait_Ack() != 0)
    {
        goto cmd_fail;  /* EEPROM������Ӧ�� */
    }

    /* ��8������������I2C���ߡ�ǰ��Ĵ����Ŀ����EEPROM���͵�ַ�����濪ʼ��ȡ���� */
    IIC_Start();    

    /* ��7������������ֽڣ���7bit�ǵ�ַ��bit0�Ƕ�д����λ��0��ʾд��1��ʾ�� */
    IIC_Send_Byte(EEPROM_DEV_ADDR_Page | I2C_RD);   /* �˴��Ƕ�ָ�� */

    /* ��8��������ACK */
    if (IIC_Wait_Ack() != 0)
    {
        goto cmd_fail;  /* EEPROM������Ӧ�� */
    }  

    /* ��9����ѭ����ȡ���� */
    for (i = 0; i < Size; i++)
    {
        ReadBuf[i] = IIC_Read_Byte(0);   /* ��1���ֽ� */
        /* ÿ����1���ֽں���Ҫ����Ack�� ���һ���ֽڲ���ҪAck����Nack */
        if (i != Size - 1)
        {
            IIC_Ack();  /* �м��ֽڶ����CPU����ACK�ź�(����SDA = 0) */
        }
        else
        {
            IIC_NAck(); /* ���1���ֽڶ����CPU����NACK�ź�(����SDA = 1) */
        }

    }

    /* ����I2C����ֹͣ�ź� */

    IIC_Stop();

    return 1;   /* ִ�гɹ� */

   

cmd_fail: /* ����ִ��ʧ�ܺ��мǷ���ֹͣ�źţ�����Ӱ��I2C�����������豸 */

    /* ����I2C����ֹͣ�ź� */

    IIC_Stop();

    return 0;

}
//��W24256�����ָ����ַ��ʼд��ָ������������
//WriteAddr :��ʼд��ĵ�ַ 
//pBuffer   :���������׵�ַ
//NumToWrite:Ҫд�����ݵĸ���
void W24256_Write(u16 WriteAddr,u8 *pBuffer,u16 NumToWrite)
{
	while(NumToWrite--)
	{
		W24256_Byte_Write(WriteAddr,*pBuffer);
		WriteAddr++;
		pBuffer++;
	}
}
//��W24256�����ָ����ַ��ʼ����ָ������������
//ReadAddr :��ʼ�����ĵ�ַ 
//pBuffer  :���������׵�ַ
//NumToRead:Ҫ�������ݵĸ���
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


