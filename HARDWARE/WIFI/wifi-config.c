
#include "wifi-config.h"
#include <string.h>
#include "delay.h"
#include "usart.h"

u8 rx_buff[buffsize]={0};
u8 rx_count=0;
char temp[50] = "";

void wifi_config(u8 usart_periph)
{
	switch(wifi_init(usart_periph))
			{								 
//				case -1:  uart_send(usart_periph, (u8 *)AT_ERROR, strlen(AT_ERROR)); break;
//				case -2:	uart_send(usart_periph, (u8 *)MODE_ERROR, strlen(MODE_ERROR));break;
//				case -3:	uart_send(usart_periph, (u8 *)CONECT_WIFI_ERROR, strlen(CONECT_WIFI_ERROR));break;
//				case -4:	uart_send(usart_periph, (u8 *)CONECT_SERVER_ERROR, strlen(CONECT_SERVER_ERROR));break;
//				default :	uart_send(usart_periph, (u8 *)"wifi连接成功\r\n", 14);break;
				
				case -1:  printf("\r\nAT_ERROR\r\n");	break;
				case -2:	printf("\r\nMODE_ERROR\r\n"); break;
				case -3:	printf("\r\nCONECT_WIFI_ERROR\r\n");break;
				case -4:	printf("\r\nCONECT_SERVER_ERROR\r\n");break;
				default :	printf("\r\nwifi连接成功\r\n");break;			
			}	
}


/*wifi模块初始化*/
int wifi_init(u32 usart_periph)
{
	u8 ret=1;
	
	//依次测试wifi模块功能及连接wifi、服务器
//	ret = Send_AT_commend(usart_periph,"AT", "OK", 100);
	if(!ret)
		return -1;
	#ifdef DEBUG
		Send_AT_commend(usart_periph,"ATE1", "", 100);
	#else
		Send_AT_commend(usart_periph,"ATE0", "", 100);
	#endif
	ret = Send_AT_commend(usart_periph,"AT+CWMODE=1", "OK", 100);  //1-station, 2-AP
	if(!ret)
		return -2;
	wifi_str();
	ret = Send_AT_commend(usart_periph,temp, "OK", 9000); 			//AT+CWJAP="pxc002","?????"
	if(!ret)
		return -3;
	server_str();
	ret = Send_AT_commend(usart_periph,temp, "OK", 3000);			//AT+CIPSTART="TCP","192.168.31.32",3456
	if(!ret)
		return -4;
	return 1;
}
uint8_t wifi_str()
{
	int i=0;
	while(temp[i])temp[i++]=0x00;
	strcat(temp,"AT+CWJAP=");
	strcat(temp,"\"");
	strcat(temp,WIFI_NAME);
	strcat(temp,"\",\"");
	strcat(temp,WIFI_PWD);
	strcat(temp,"\"");
}
uint8_t server_str()
{
	int i=0;
	while(temp[i])temp[i++]=0x00;
	strcat(temp,"AT+CIPSTART=\"TCP\",\"");
	strcat(temp,SERVER_ADDR);
	strcat(temp,"\",");
	strcat(temp,SERVER_PORT);
}

/*发送AT指令
*传入参数:at指令，期待接收的数据，超时时间
*/
u8 Send_AT_commend(u32 usart_periph,char *at_commend, char *re_commend, u16 time_out)
{
	u8 i=0;
	for(i=0;i<3;i++)
	{
		clear_buf();
		uart_send(usart_periph, (uint8_t *)at_commend, strlen(at_commend));
		uart_send(usart_periph, (uint8_t *)"\r\n", 2);  //自动发送回车换行
			
		delay_ms(time_out);
		if(find_str(re_commend))
			return 1;
		i++;
	}
	return 0;
}


/*清空wifi接收缓冲区*/
void clear_buf()
{
    memset(rx_buff,0,rx_count);	
	  rx_count=0;

}


/*查找字符串
*传入参数：需要匹配的字符串
*/
uint8_t find_str(char *a)
{
	
	char *addr = strstr((char *)rx_buff,a);
	if(addr)
		return 1;
	return 0;
	
}

/*wifi发送
*传入参数：需要发送的字符串数组，发送的字节数
*/
void wifi_send(u32 usart_periph, u8 *buf, int len)
{
	char len_str[]="",temp1[256]="";
	itoa(len, len_str);
	strcat(temp1,"AT+CIPSEND=");
	strcat(temp1,len_str);
	
	uart_send(usart_periph, (u8 *)"发送数据\r\n", 10);
	if(Send_AT_commend(usart_periph,temp1, ">", 300))
		if(Send_AT_commend(usart_periph,(char *)buf, "SEND OK", 300))
			uart_send(usart_periph,(uint8_t *)"发送成功！！！\r\n", 16);
}

void itoa(int n,char s[])
{
	int i,j,sign,m=0;
	char temp[]="";
	if((sign=n)<0)
	n=-n;
	i=0;
	do{
				 s[i++]=n%10+'0';
	}
	while ((n/=10)>0);
	if(sign<0)
	s[i++]='-';
	s[i]='\0';
	for(j=i-1;j>=0;j--)    
		temp[m++]=s[j];
	for(i=0;i<strlen(temp);i++)
		s[i]=temp[i];
}