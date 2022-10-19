#include <stdint.h>

#define WIFI_NAME "hello_world"			//要连接的wifi热点名
#define WIFI_PWD "87654321"					//热点密码
#define SERVER_ADDR "192.168.43.73"		//服务器ip地址
#define SERVER_PORT "5656"						//端口号

#define AT_ERROR "at指令错误 \r\n"
#define MODE_ERROR "工作模式设置错误\r\n"
#define CONECT_WIFI_ERROR "无法连接wifi\r\n"
#define CONECT_SERVER_ERROR "无法连接服务器\r\n"

// #define DEBUG 1			//debug模式下设置为1


#define buffsize 200

extern  u8 rx_buff[buffsize];
extern  u8 rx_count;

extern u8 my_re_buf1[2000];
extern u8 my_re_buf2[2000];
extern u8 send_buf[512];
extern u16 pt_w1;

void wifi_config(u8 usart_periph);
int wifi_init(u32 usart_periph);
u8 Send_AT_commend(u32 usart_periph, char *at_commend, char *re_commend, uint16_t time_out);
void clear_buf();
u8 find_str(char *a);
u8 server_str();
u8 wifi_str();
void wifi_send(u32 usart_periph,u8 *buf,int len);
void itoa(int n,char s[]);