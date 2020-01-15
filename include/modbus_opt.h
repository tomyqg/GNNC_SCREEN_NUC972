/*
 * modbus_opt.h
 *
 *  Created on: Dec 17, 2019
 *      Author: aron566
 */

#ifndef INCLUDE_MODBUS_OPT_H_
#define INCLUDE_MODBUS_OPT_H_
#ifdef __cplusplus //使用ｃ编译
extern "C" {
#endif
#include "serial_opt.h"
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <stdint.h>

//单通道气体参数 ---寄存器列表---
#define CHANNEL_STATE_H					40000U//通道状态_H
#define CHANNEL_STATE_L					40001U//通道状态_L
#define REAL_TIME_CONSISTENCE_INT_H		40002U//实时浓度整形 int*1000
#define REAL_TIME_CONSISTENCE_INT_L		40003U
#define REAL_TIME_CONSISTENCE_FLOAT_H	40004U//实时浓度浮点型
#define REAL_TIME_CONSISTENCE_FLOAT_L	40005U
#define GAS_AD_VALUE					40006U//气体AD值
#define REAL_TIME_TEMP_INT_H			40007U//实时温度带符号整形 温度值*1000 单位摄氏度
#define REAL_TIME_TEMP_INT_L			40008U
#define TEMP_AD_VALUE					40009U//温度AD值
#define GAS_TYPE						40016U//气体类型
#define ALARM_FLAG						40017U//报警标志\分辨率\物理单位
#define DISPLAY_UNIT                    40018U//显示单位 0:%VOL 1:%LEL 2:PPM 3:PPB
#define INIT_TIME						40019U//初始化时长--设置,回避初期不稳定数值
#define SENSOR_BRAND_H					40020U//传感器品牌-起始字符
#define SENSOR_BRAND_L					40021U//传感器品牌-剩余字符
#define GAS_FEED_RANGE_H				40022U//气体量程H
#define GAS_FEED_RANGE_L				40023U//气体量程L
#define GAS_SET_ZERO_VALUE_H			40024U//气体标零的零点值H
#define GAS_SET_ZERO_VALUE_L			40025U//气体标零的零点值L
#define GAS_SET_ZERO_RANGE_H			40026U//设置气体零值范围,此范围内数值,对外显示0
#define GAS_SET_ZERO_RANGE_L			40027U
#define SET_ALARM_VALUE_H				40028U//设置报警值
#define SET_ALARM_VALUE_L				40029U
#define SET_HIGH_ALARM_VALUE_H			40030U//设置高报警值
#define SET_HIGH_ALARM_VALUE_L			40031U
#define SET_STEL_VALUE_H				40032U//设置STEL
#define SET_STEL_VALUE_L				40033U
#define SET_TWA_VALUE_H					40034U//设置TWA
#define SET_TWA_VALUE_L					40035U
#define SET_BIND_LOW_ALARM_RELAY_H		40036U//绑定低报继电器
#define SET_BIND_LOW_ALARM_RELAY_L		40037U
#define SET_BIND_HIGH_ALARM_RELAY_H		40038U//绑定高报继电器
#define SET_BIND_HIGH_ALARM_RELAY_L		40039U
#define SET_BIND_STEL_RELAY_H			40040U//绑定STEL继电器
#define SET_BIND_STEL_RELAY_L			40041U
#define SET_BIND_TWA_RELAY_H			40042U//绑定TWA继电器
#define SET_BIND_TWA_RELAY_L			40043U
#define SET_ZERO_CONSISTENCE_H			40048U//设置零点浓度值
#define SET_ZERO_CONSISTENCE_L			40049U
#define SET_ZERO_AD						40050U//设置零点AD值
#define SET_GAS_PONT1_CONSISTENCE_H		40051U//设置气点1 浓度值
#define SET_GAS_PONT1_CONSISTENCE_L		40052U
#define SET_GAS_PONT1_AD				40053U//设置气点1 AD值
#define SET_GAS_PONT2_CONSISTENCE_H		40054U//设置气点2 浓度值
#define SET_GAS_PONT2_CONSISTENCE_L		40055U
#define SET_GAS_PONT2_AD				40056U//设置气点2 AD值
#define SET_UNDER20_COMPENSATE			40060U//设置-20度温度补偿系数
#define SET_UNDER18_COMPENSATE			40061U//设置-18度温度补偿系数
#define SET_UNDER16_COMPENSATE			40062U//设置-16度温度补偿系数
#define SET_UNDER14_COMPENSATE			40063U//设置-14度温度补偿系数
#define SET_UNDER12_COMPENSATE			40064U//设置-12度温度补偿系数
#define SET_UNDER10_COMPENSATE			40065U//设置-10度温度补偿系数
#define SET_UNDER8_COMPENSATE			40066U//设置-8度温度补偿系数
#define SET_UNDER6_COMPENSATE			40067U//设置-6度温度补偿系数
#define SET_UNDER4_COMPENSATE			40068U//设置-4度温度补偿系数
#define SET_UNDER2_COMPENSATE			40069U//设置-2度温度补偿系数
#define SET_ZERO_COMPENSATE				40070U//设置0度温度补偿系数
#define SET_ON_2_COMPENSATE				40071U//设置2度温度补偿系数
#define SET_ON_4_COMPENSATE				40072U//设置4度温度补偿系数
#define SET_ON_6_COMPENSATE				40073U//设置6度温度补偿系数
#define SET_ON_8_COMPENSATE				40074U//设置8度温度补偿系数
#define SET_ON_10_COMPENSATE			40075U//设置10度温度补偿系数
#define SET_ON_12_COMPENSATE			40076U//设置12度温度补偿系数
#define SET_ON_14_COMPENSATE			40077U//设置14度温度补偿系数
#define SET_ON_16_COMPENSATE			40078U//设置16度温度补偿系数
#define SET_ON_18_COMPENSATE			40079U//设置18度温度补偿系数
#define SET_ON_20_COMPENSATE			40080U//设置20度温度补偿系数
#define SET_ON_22_COMPENSATE			40081U//设置22度温度补偿系数
#define SET_ON_24_COMPENSATE			40082U//设置24度温度补偿系数
#define SET_ON_26_COMPENSATE			40083U//设置26度温度补偿系数 20度标准值的千分之x,计算结果*该补偿 再除以1000
#define SET_ON_28_COMPENSATE			40084U//设置28度温度补偿系数
#define SET_ON_30_COMPENSATE			40085U//设置30度温度补偿系数
#define SET_ON_32_COMPENSATE			40086U//设置32度温度补偿系数
#define SET_ON_34_COMPENSATE			40087U//设置34度温度补偿系数
#define SET_ON_36_COMPENSATE			40088U//设置36度温度补偿系数
#define SET_ON_38_COMPENSATE			40089U//设置38度温度补偿系数
#define SET_ON_40_COMPENSATE			40090U//设置40度温度补偿系数
#define SET_ON_42_COMPENSATE			40091U//设置42度温度补偿系数
#define SET_ON_44_COMPENSATE			40092U//设置44度温度补偿系数
#define SET_ON_46_COMPENSATE			40093U//设置46度温度补偿系数
#define SET_ON_48_COMPENSATE			40094U//设置48度温度补偿系数
#define SET_ON_50_COMPENSATE			40095U//设置50度温度补偿系数
#define SEND_CMD						40096U//发送命令 0:标零命令无附带参数 1:标气+ 参数1:浓度增加值 2:标气- 参数1:浓度减少值
#define SEND_CMD_PAR1					40097U//发送命令参数1
#define SEND_CMD_PAR2					40098U//发送命令参数2
#define ACCESS_VIRTUAL_CHNNEL_NUM		40099U//虚拟访问通道号

//新增寄存器
#define SET_UART_MODE					49000U//上位机设置本机串口模式
#define SET_MQTT_SERVER_H				49001U//16进制显示:高字节区域:192 168
#define SET_MQTT_SERVER_L				49002U//16进制显示:低字节区域:1 37
#define SET_TCP_SERVER_H				49003U//16进制显示:高字节区域:192 168
#define SET_TCP_SERVER_L				49004U//16进制显示:低字节区域: 1   37
#define SET_TCP_PORT					49005U//TCP端口号
#define SET_READ_DATA_TIME_H			49006U//上位机设置读取数据库数据时间起点
#define SET_READ_DATA_TIME_L			49007U//上位机设置读取数据库数据时间起点
#define READ_DATA_FROM_DB				49008U//上位机读取数据库数据
#define SET_REPORT_CYCLE				49009U//上位机设置本机数据上报周期MQTT 依据通道设置 发送至通道11则所有通道设定

//终端参数 ---寄存器列表---
#define MODBUS_PROTOL 					50000U//modbus协议 回复0230
#define PRODUCKT_TYPE 					50001U//产品类型
#define CPU_ID_1	  					50002U
#define CPU_ID_2	 					50003U
#define CPU_ID_3	  					50004U
#define CPU_ID_4	  					50005U
#define CPU_ID_5						50006U
#define CPU_ID_6						50007U
#define HARD_VERSION					50008U//硬件版本号
#define SOFTWARE_VERSION				50009U//软件版本号
#define PARAMETER_VERSION				50010U//参数版本号
#define REGISTER_YEAR					50011U//出厂年
#define REGISTER_MONTH					50012U//出厂月
#define REGISTER_DATE					50013U//出厂日
#define COMMUNICATION_ID				50016U//通信ID
#define SERIAL_BAUD_RATE				50017U//通讯波特率
#define RELAY_NUM_1						50018U//继电器1 bit15:0-电平 1-脉冲
#define RELAY_NUM_2						50019U//继电器2 bit14-bit12:0-低报\高报都动作 1-低报动作 2-高报动作 3-不动作 4-运行指示
#define RELAY_NUM_3						50020U//继电器3 bit11-bit6 脉冲宽度时间(100ms倍数)
#define RELAY_NUM_4						50021U//继电器4 bit5-bit0 脉冲间隔时间(10s倍数)
//#define UNSUED_1
//#define UNUSED_2
#define CURRENT_YEAR					50040U//年
#define CURRENT_MONTH					50041U//月
#define CURRENT_DATE					50042U//日
#define CURRENT_HOUR					50043U//时
#define CURRENT_MINUTE					50044U//分
#define CURRENT_SECOND					50045U//秒
#define SYSTICK_H						50046U//systick h
#define SYSTICK_L						50047U
#define RUNNING_SECONDS_H				50048U//运行时间秒
#define RUNNING_SECONDS_L				50049U
#define RELAY_COUNT_NUM					50060U//继电器数量
#define RELAY_A_TYPE					50061U//继电器A类型 0-常开 1-手拉电关
#define RELAY_P_TYPE					50062U//继电器P类型
#define RELAY_A_PULSE_MS				50063U//继电器A脉冲时长ms
#define RELAY_P_PULSE_MS				50064U//继电器P脉冲时长ms
#define RELAY_A_REACTION_DELAY_SECOND	50065U//继电器A动作重复标志与间隔 bit15:是否重复;bit0-bit14:间隔秒数
#define RELAY_P_REACTION_DELAY_SECOND	50066U//继电器P动作重复标志与间隔
#define RESERVED_1						50088U//保留
#define RESERVED_2						50089U//
#define RESERVED_3						50090U//
#define RESERVED_4						50091U//
#define IO_INPUT_TEST_H					50092U//io输入测试 只读
#define IO_INPUT_TEST_L					50093U//
#define IO_OUTPUT_TEST_H				50094U//io输出测试 只写
#define IO_OUTPUT_TEST_L				50095U//
#define ENDPOINT_CMD					50096U//cmd bit0:保留 1-终端复位 2-备份出厂 3-恢复出厂   4-CL<		5-CL>      6-强制电流环输出  		8-强制输出                                                                                                                         0x7177-获取厂家信息
#define ENDPOINT_PAR_1					50097U//命令参数1         80F1      80F2     80F3		80F4		80F5		80F6		 高字节代表继电器号:0x01-0x04;低字节代表继电器开关;高字节代表LED:0x11-0x13;低字节代表开关LED;高字节0x99电流环输出;低字节电流环输出大小(0-240对应0-24.0mA)		无
#define ENDPOINT_PAR_2					50098U//命令参数2		   延时s复位     无       无	  电流环up步长  电流环DOWN步长	输出电流mA         输出时间s秒																																无
#define CHANNNEL_NUM					50099U//通道号-要访问的通道号

typedef uint16_t (*p_modbus_process)(void *data);

typedef struct modbus_process_
{
	uint16_t register_num;
	p_modbus_process func;
}modbus_process_t;

typedef struct modbus_master_rec_
{
	int rec_data;//实时数据
	uint8_t *data_addr;//数据指针-处理多个寄存器使用
	uint16_t channnel;//数据来自通道
	uint8_t cmd;//modbus命令
}modbus_master_rec_t;


typedef struct modbus_polling_slave
{
	uint16_t access_reg_addr;
	uint16_t access_num;
}modbus_polling_slave_t;

/*独立运行模式下主动轮寻从站寄存器列表*/
extern modbus_polling_slave_t access_reg_arr[];

/*寄存器--处理handle 建立映射关系*/
extern modbus_process_t reg_process_map[];

/*本机作为主机解码*/
void* modbus_master_decode_start(void* data);

/*本机作为从机解码*/
void* modbus_slave_decode_start(void* data);

/*独立运行模式*/
int8_t modbus_master_decode_mannul(void* data);

typedef struct
{
	volatile uint16_t read_count;
	volatile uint16_t read_reg;
	volatile uint16_t read_channel;
	circular_buffer *cb;
}polling_msg_t;
/*独立运行模式本机轮寻*/
extern polling_msg_t polling_msg[];


/*CRC16 校验*/
unsigned short CRC_Return(unsigned char *Crc_Buf, unsigned short Crc_Len);

/*调试打印*/
void debug_print(uint8_t *msg,uint8_t msg_len);



/*线程锁+信号量*/
extern pthread_mutex_t GNNC_Decode_mutex_lock;
extern pthread_cond_t GNNC_Decode_cond;

extern pthread_mutex_t GNNC_REC_Data_mutex_lock;
extern pthread_cond_t GNNC_REC_cond;
//struct timeval now;
//struct timespec outtime;
//if(uart_mode == REC_NOT_THROUGH_MODE)
//{
//	pthread_mutex_lock(&GNNC_REC_Data_mutex_lock);
//}
//if(polling_msg.read_count <= cb->write_offset || polling_msg.crc_state == 1 || uart_mode == REC_THROUGH_MODE)//透传 crc错误 立刻执行解析
//{
//	//发信号 解析
//	printf("OK 可解析！ 需读%u  可读%lu\n",polling_msg.read_count,cb->write_offset);
//	pthread_cond_signal(&GNNC_REC_cond);//激活线程
//	pthread_mutex_unlock(&GNNC_REC_Data_mutex_lock);
//}
//pthread_mutex_lock(&GNNC_REC_Data_mutex_lock);
//gettimeofday(&now, NULL);
//outtime.tv_sec = now.tv_sec + 0;
//outtime.tv_nsec = now.tv_usec * 1000;
//outtime.tv_nsec = now.tv_usec + (30*1000);//不能超过1秒
//if(ETIMEDOUT == pthread_cond_timedwait(&GNNC_REC_cond,&GNNC_REC_Data_mutex_lock,&outtime))
//{
//	printf("------------等待超时----------\n");
//}
//pthread_cond_wait(&GNNC_Decode_cond, &GNNC_Decode_mutex_lock);
//pthread_mutex_unlock(&GNNC_REC_Data_mutex_lock);
#ifdef __cplusplus //使用ｃ编译
}
#endif
#endif /* INCLUDE_MODBUS_OPT_H_ */
