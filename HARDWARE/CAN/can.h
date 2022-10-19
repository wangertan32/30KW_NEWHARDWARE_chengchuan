#ifndef __CAN_H
#define __CAN_H	 
#include "sys.h"	    
#define CANTEST 1
/**系统参数结构体定义**/
/*版本信息*/
typedef struct _PTC_VERSION_INF{
	u8 SerialNum[4];//生产序列号
	u8 HardwareVer[28];//硬件版本
	u8 PCBVer[28];//硬件PCB版本
	u8 SoftwareVer[28];//软件版本
	u8 Producer[28];//生产厂商
} PTC_VERSION_INF; 
extern PTC_VERSION_INF PTCVersionINF;
/*监控参数*/
/*
电压精度：0.1V。譬如实际输出电压100V，发送报文为1000。
电流精度：0. 01A。譬如实际输出电流10A，发送报文为1000。
频率精度：0.01KHZ。譬如实际逆变频率10KHz，发送报文为1000。
温度精度：实际温度+40度。譬如实际温度为25度，则发送报文为65。
*/
typedef struct _PTC_ELECT_DATA{
	u16 Input_v;//输入电压
	u16 Input_i;//输入电流
	u16 Rectifier_output_v;//整流输出电压
	u16 PFCInductor1_i;//PFC电感电流1
	u16 PFCInductor2_i;//PFC电感电流2
	u16 BUSoutput_v;//BUS输出电压
	u16 Coil_i;//线圈电流
	u16 INVInductor_i;//逆变电感电流
	u16 INV_fre;//逆变工作频率
	u8 	Temp_envir;//环境温度
	u8 	Temp_rectify;//整流器温度
	u8 	Temp_pfc_mos;//PFC功率管温度
	u8 	Temp_INV_mos1;//逆变功率管温度1
	u8 	Temp_INV_mos2;//逆变功率管温度2
	u16 Output_v;//输出电压
	u16 Output_i;//输出电流
	u32 Output_p;//输出功率
	u16 Output_MAX_i;//额定最大输出电流
	u16 g_PTC_State;//PTC上传系统状态
	u16 g_pfc_State;//PTC上传PFC状态量
	u16 g_inv_State;//PTC上传INV状态量
	u16 g_PFCAlarm_State;//PTC上传PFC告警量
	u16 g_INVAlarm_State;//PTC上传INV告警量	
} PTC_ELECT_DATA; 
extern PTC_ELECT_DATA PTCElectData;
/*设置参数*/
/*
电压精度: 单位0.1V，例如8100表示810V
频率精度:	单位0.1Hz，例如500表示50Hz
*/
typedef struct _SET_PTC_DATA{
	u16 s_output_v;//输出电压
	u16 s_output_i;//输出电流
	u16 s_Bus_v;//母线电压       
	u16 s_INV_fre;//逆变工作频率 
	FlagStatus  s_set_statu;//设置状态
} SET_PTC_DATA; 
extern SET_PTC_DATA SetPTCData;
/**CAN帧结构说明**/
#define PROTOCOL_NOR   0xE0			//正常通讯
#define PROTOCOL_UPLOAD   0xE1	//在线更新
#define CSU_TYPE 			 0x7D			//CSU设备类型
#define PTC_TYPE       0x01			//PTC设备类型
#define CSU_ADDR       0x78			//CSU地址
#define PTC_ADDR       0x01     //PTC地址
#define PRIORITY       0x1

#define FUNC_SEEK     0x20      //后台查询呼叫命令 (PTC应答时将模拟量、状态量、告警量等数据上送给CSU)
#define FUNC_SETNUM   0x24      //配置机号(CSU设置PTC机号)
#define FUNC_CONFIG   0x26      //配置参数(CSU下发PTC运行参数、默认参数、开机时间)
#define FUNC_LINK     0x28      //建立链接 (便于CSU管理PTC)
#define FUNC_RCONTROL 0x29			//遥控帧(CSU下发休眠、唤醒、风扇调速禁止、允许)
#define FUNC_GET_INF 	0x31			//获取厂家版本
#define FUNC_SEEKVALUE     0x32     //查询参数值
#define FUNC_SEEKSTAT      0x33     //查询状态值
//CANTOOL查询
#define TOOL_INF     			0x30 				//获取信息
#define TOOL_INF_CODE1     0x01 			//获取厂家信息附加码
#define TOOL_INF_CODE2     0x02 			//获取版本命令附加码

//遥控命令附加码
#define APPEND_CODEH   			0x24		//遥控命令附加码高字节
#define APPEND_CODEL_STOP   0x04		//PTC休眠
#define APPEND_CODEL_START  0x05		//PTC唤醒
#define APPEND_CODEL_FANS   0x08		//风扇调速允许
#define APPEND_CODEL_FANP   0x09		//风扇调速禁止

#define RTN_CODE      0x80  //返回码
#define SHEET_SN      0x01  //数据表序号
#define REV_DATA0     0x0   //预留字节
#define REV_DATA1     0x0		//预留字节

#define SegNum0     0x0	//分段字节

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
u8 CAN2_Receive_Msg(u8 *buf);							//接收数据

u8 PreProcess_CanOrder(u8 order);
u8 Can2Send_MultiMessages(u32 exid, u8 *ptr, u16 len);
u8 can2_data_packet(u8 order, u8 append_codel, u8 len_data ,u8 *data);			
void canid_init(void);

u8 CAN1_Mode_Init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode);

u8 CAN_ETH_Init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode) ;
void can_send_eth(void);

#endif

