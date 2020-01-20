/*
 * db_opt.h
 *
 *  Created on: Jan 3, 2020
 *      Author: aron566
 */

#ifndef INCLUDE_DB_OPT_H_
#define INCLUDE_DB_OPT_H_
#ifdef __cplusplus //使用ｃ编译
extern "C" {
#endif
#include <stdint.h>
#define DEVICE_TAB_COUNT_NUM 	10
#define MAX_DB_TIME_COUNT          720U//最大记录时间720min
#define RECORD_DELAY_S         	   2U //每通道记录延时2s
#define MAX_RECORD_MESSAGE_NUM (MAX_DB_TIME_COUNT*60/RECORD_DELAY_S) //最大记录消息数720*60/2

typedef void (*pquery)(char** pr , int row , int column );

typedef struct db_device
{
	uint16_t device_count_num;//每个通道设备记录数目　　０－９
	uint16_t report_period;//每个通道设备上报周期
	uint16_t current_channel_state_H;//每个通道状态
	uint16_t current_channel_state_L;//每个通道状态
	uint16_t current_consisitence_int_H;//每个通道浓度整形
	uint16_t current_consisitence_int_L;
	uint16_t current_consisitence_float_H;//每个通道浓度浮点
	uint16_t current_consisitence_float_L;
	uint16_t current_value_H;//每个通道设备当前值H gas ad
	uint16_t current_value_L;//每个通道设备当前值L
	uint16_t current_temp_int_H;//每个通道温度整形
	uint16_t current_temp_int_L;
	uint16_t current_temp_ad;//每个通道温度AD值
	uint16_t device_type;
	long long max_value;//每个通道设备记录的最大值
	long long min_value;//每个通道设备记录的最小值
	long long average_value;//每个通道设备记录的平均值
	int device_state;//每个通道设备状态
	long int running_time;//每个通道设备运行时间
	int read_data_time;//每个通道查询时间设置
	long long device_id_H;//每个通道设备唯一ID高８字节
	long long device_id_L;//每个通道设备唯一ID低８字节
}db_device_tab_t;

extern db_device_tab_t gnnc_device_v_info[];
/*每个表名*/
extern const char* device_tab_name_list[];

void init_database(void);

/*表中通道，准备支持所有通道设备集中在一张表中，提供查询筛选*/

//数据记录
int record_insert(const char* device_tab_name,int channel ,long int timestamp ,db_device_tab_t *device_tab);


//状态查询
void* status_minitor(void* data);
//查询大于此时间的数据记录
int record_query_up_time_region(const char *device_tab_name ,long int current_time ,int channel ,pquery pf);
//查询后发送至上位机
void process_query_send(char** pr, int row, int column);

//更新设备数据--使用
int update_table_data(const char *device_tab_name ,long int timestamp ,int channel ,db_device_tab_t *device_tab);
//更新上报周期
int record_update_report_period(const char *device_tab_name ,int channel,db_device_tab_t *device_tab);
//更新设备状态
int record_update_state(const char *device_tab_name,int channel ,db_device_tab_t *device_tab);

/*清空暂存数据信息*/
void device_info_clear(uint8_t channel_num);
//删除表-暂未使用
int record_delete(const char *device_tab_name ,int channel);

#ifdef __cplusplus //使用ｃ编译
}
#endif
#endif /* INCLUDE_DB_OPT_H_ */
