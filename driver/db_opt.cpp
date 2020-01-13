/*
 * db_opt.cpp
 *
 *  Created on: Jan 2, 2020
 *      Author: aron566
 */

#ifdef __cplusplus //使用ｃ编译
extern "C" {
#endif
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/time.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include "db_opt.h"
#include "sqlite3.h"
#include "run_log.h"

//--------------------------database_SQL----------------------------------------

sqlite3* db = NULL;
const char* file_database_path = "/dev/shm/GNNC_Device_info.db";
const char* sql_query_data = "SELECT count(*) FROM sqlite_master device_info WHERE type='table' AND name='device_info';";

//为通道０－９清空表数据
/*
 * 参数：表名
 * */
const char* sql_empty_data = "delete from %s;";
//未使用
const char* sql_create_data_unused = "CREATE TABLE IF NOT EXISTS device_info(sequence_num UNSIGNED ,channel_num UNSIGNED ,%s UNSIGNED PRIMARY KEY ,report_period UNSIGNED,value_H INTEGER ,value_L INTEGER ,state INTEGER);";

//为通道０－９建表
/*
 * 表0：device_channel_0
 * 表1：device_channel_1
 * */
const char* device_tab_name_list[DEVICE_TAB_COUNT_NUM+1] =
{
	"device_channel_0",//未使用
	"device_channel_1",
	"device_channel_2",
	"device_channel_3",
	"device_channel_4",
	"device_channel_5",
	"device_channel_6",
	"device_channel_7",
	"device_channel_8",
	"device_channel_9",
	"device_channel_10"
};
/*
 * 参数：　表名，唯一关键字段
 * */
const char* sql_create_data = "CREATE TABLE IF NOT EXISTS %s(sequence_num UNSIGNED ,channel_num UNSIGNED ,%s UNSIGNED PRIMARY KEY,report_period UNSIGNED ,value_H INTEGER ,value_L INTEGER ,state INTEGER);";

//建立数据
/*
 * 参数：表名，序号，通道，更新时间，上传周期，数值Ｈ，数值Ｌ，状态
 * */
const char* sql_insert_data = "INSERT OR REPLACE INTO MAIN.%s VALUES('%d', '%d', '%ld', '%d','%d','%d','%d');";//序号，通道，更新时间，上传周期，数值Ｈ，数值Ｌ，状态

//删除数据
/*
 * 参数：表名，状态
 * */
const char* sql_delete_data = "DELETE FROM MAIN.%s WHERE state = %d;";//属于离线踢出

//未使用
/*更新记录*/
const char* sql_update_data_time_field = "UPDATE MAIN.device_info SET update_time = %ld where sequence_num = %d AND channel_num = %d;";//更新记录时间

/*本次更新使用*/
/*
 * 参数：表名，记录时间，上传周期，数值Ｈ，数值Ｌ，状态
 * */
const char* sql_update_data_channel_field = "UPDATE MAIN.%s SET update_time = %ld,report_period = %d,value_H = %d,value_L = %d,state = %d where sequence_num = %d AND channel_num = %d;";//更新记录时间，上传周期，数值Ｈ，数值Ｌ，状态

/*索引异常设备*/
/*
 * 参数：表名，记录时间，上传周期，通道号
 * */
const char* sql_search_data = "SELECT * FROM MAIN.%s WHERE update_time <= %ld-(report_period*1) AND channel_num = %d;";

/*索引指定记录范围设备*/
/*
 * 参数：表名，记录数，通道号
 * */
const char* sql_search_net = "SELECT * FROM MAIN.%s WHERE sequence_num <= %d AND channel_num = %d;";//搜索－－匹配记录数和通道号

/*索引指定记录 [<时间]范围设备*/
/*
 * 参数：表名，更新时间，通道号
 * */
const char* sql_search_down_time = "SELECT * FROM MAIN.%s WHERE update_time <= %ld AND channel_num = %d;";//搜索－－匹配时间和通道号

/*索引指定记录 [>时间]范围设备*/
/*
 * 参数：表名，更新时间，通道号
 * */
const char* sql_search_up_time = "SELECT * FROM MAIN.%s WHERE update_time >= %ld AND channel_num = %d;";//搜索－－匹配时间和通道号

/*更新设备上报周期*/
/*
 * 参数：表名，通道号
 * */
const char* sql_update_data = "UPDATE MAIN.%s SET report_period = %d WHERE sequence_num = %d AND channel_num = %d;";//更新数据－－匹配记录数和通道号

/*更新设备状态*/
/*
 * 参数：表名，通道号
 * */
const char* sql_update_state = "UPDATE MAIN.%s SET state = %d WHERE channel_num = %d;";
//-------------------------database_SQL_END-------------------------------------


//-----------------------database_funcation_declaration-------------------------
//数据库建立+数据表建立
static int filedb_create(const char* device_tab_name);

//查询设备
static int record_query(const char *device_tab_name ,long int current_time ,int channel, pquery pf);
//查询设备离线超时
static void process_query_result(char** pr, int row, int column);

//查询设备时间范围内所有数值
static int record_query_time_region(const char *device_tab_name ,long int current_time ,int channel ,pquery pf);
//查询设备时间范围内所有数值平均值,最大值,最小值
static void process_query_math(char** pr, int row, int column);


static void minitor_sig_hander(int signo);

int time_field_update(int addr, long int timestamp);
//----------------------database_function_define--------------------------------

db_device_tab_t gnnc_device_v_info[DEVICE_TAB_COUNT_NUM+1];
//{
//	.device_count_num = {0},//每个通道设备记录数目  录入 和 静态变量 一起使用 静态变量作为本通道的记录的最大数是否达到，达到则循环更新
//	.current_value_H = {0},//每个通道设备当前值H   录入
//	.current_value_L = {0},//每个通道设备当前值L  录入
//	.max_value = {0},//每个通道设备记录的最大值    一直存在
//	.min_value = {0},//每个通道设备记录的最小值     一直存在
//	.report_period = {0},//每个通道设备上报周期    录入
//	.average_value = {0},//每个通道设备记录的平均值 一直存在
//	.device_state = {0},//每个通道设备状态		   录入
//	.running_time = {0},//每个通道设备运行时间     一直存在
//	.device_id_H = {0},//每个通道设备唯一ID高８字节 一直存在
//	.device_id_L = {0}//每个通道设备唯一ID低８字节
//}

void init_database(void)
{
	//初始化数据库建立表
	for(int i = 1;i < DEVICE_TAB_COUNT_NUM+1;i++)
	{
		if(filedb_create(device_tab_name_list[i]))
		{
			printf("create tab error!\n");
			GNNC_DEBUG_INFO("create [%d] tab error!",i);//调试用
			return;
		}
		GNNC_DEBUG_INFO("Create table【%s】Successful！",device_tab_name_list[i]);//调试用
	}
	//初始化结构体
	memset(&gnnc_device_v_info,0,sizeof(gnnc_device_v_info));
}


static int filedb_create(const char* device_tab_name)
{
	char* errMsg= NULL;
	int rc = 0;
	char sqlcmd[512] = {0};

	rc = sqlite3_open(file_database_path, &db);
	if(rc != SQLITE_OK)
	{
		fprintf(stderr, "can't open database:%s\n", sqlite3_errmsg(db));
		GNNC_DEBUG_INFO("can't open database:%s", sqlite3_errmsg(db));
		sqlite3_close(db);
		return -1;
	}
	GNNC_DEBUG_INFO("Open database【%s】Successful！",file_database_path);//调试用
	snprintf(sqlcmd, sizeof(sqlcmd), sql_create_data,device_tab_name,"update_time");
#if ENABLE_DB_DEBUG
	printf("%s\n",sqlcmd);
#endif
	rc = sqlite3_exec(db, sqlcmd, NULL, NULL, &errMsg);
	if(SQLITE_OK != rc)
	{
		fprintf(stderr, "can't create file database :%s\n", errMsg);
		sqlite3_close(db);
		return -1;
	}
//	GNNC_DEBUG_INFO("Create table【%s】Successful！",file_database_path);//调试用
	snprintf(sqlcmd, sizeof(sqlcmd), sql_empty_data,device_tab_name);
#if ENABLE_DB_DEBUG
	printf("%s\n",sqlcmd);
#endif
	rc = sqlite3_exec(db, sqlcmd, NULL, NULL, &errMsg);
	if(SQLITE_OK != rc)
	{
		fprintf(stderr, "can't empty file database :%s\n", errMsg);
		sqlite3_close(db);
		return -1;
	}
	return 0;
}

//序号，通道，更新时间，上传周期，数值Ｈ，数值Ｌ，状态
/*
 * 数据存入
 *
 * 返回:-1异常 0正常写入
 * */
int record_insert(const char* device_tab_name,int channel ,long int timestamp ,db_device_tab_t *device_tab)
{
	int      rc              =  0;
	char*    errMsg          =  NULL;
	char     sqlcmd[512]     =  {0};

	snprintf(sqlcmd, sizeof(sqlcmd) ,sql_insert_data ,device_tab_name ,device_tab->device_count_num ,channel ,timestamp ,device_tab->report_period ,device_tab->current_value_H ,device_tab->current_value_L ,device_tab->device_state);
#if ENABLE_DB_DEBUG
	printf("%s\n",sqlcmd);
#endif
	rc = sqlite3_exec(db, sqlcmd, NULL, NULL, &errMsg);
	if (SQLITE_OK != rc)
	{
		fprintf(stderr, "can't add record to memory database, sqlcmd=%s, err:%s\n", sqlcmd, errMsg);
		return -1;
	}

	return 0;
}
/*更新设备上报周期*/
/*
 * 参数：表名，通道号
 * */
int record_update_report_period(const char *device_tab_name ,int channel,db_device_tab_t *device_tab)
{
	int      rc              =  0;
	char*    errMsg          =  NULL;
	char     sqlcmd[512]     =  {0};

	snprintf(sqlcmd, sizeof(sqlcmd), sql_update_data,device_tab_name,device_tab->report_period,device_tab->device_count_num,channel);
#if ENABLE_DB_DEBUG
	printf("%s\n",sqlcmd);
#endif
	rc = sqlite3_exec(db, sqlcmd, NULL, NULL, &errMsg);
	if (SQLITE_OK != rc)
	{
		fprintf(stderr, "can't update record to memory database, sqlcmd=%s, err:%s\n", sqlcmd, errMsg);
		return -1;
	}

	return 0;
}

/*更新设备状态*/
/*
 * 参数：表名，通道号
 * */
int record_update_state(const char *device_tab_name,int channel ,db_device_tab_t *device_tab)
{
	int      rc              =  0;
	char*    errMsg          =  NULL;
	char     sqlcmd[512]     =  {0};

	snprintf(sqlcmd, sizeof(sqlcmd), sql_update_state ,device_tab_name ,device_tab->device_state ,channel);
#if ENABLE_DB_DEBUG
	printf("%s\n",sqlcmd);
#endif
	rc = sqlite3_exec(db, sqlcmd, NULL, NULL, &errMsg);
	if (SQLITE_OK != rc)
	{
		fprintf(stderr, "can't update record to memory database, sqlcmd=%s, err:%s\n", sqlcmd, errMsg);
		return -1;
	}

	return 0;
}

int record_delete(const char *device_tab_name ,int channel)
{
	int      rc              =  0;
	char*    errMsg          =  NULL;
	char     sqlcmd[512]     =  {0};

	snprintf(sqlcmd, sizeof(sqlcmd), sql_delete_data,device_tab_name, channel);
#if ENABLE_DB_DEBUG
	printf("%s\n",sqlcmd);
#endif
	rc = sqlite3_exec(db, sqlcmd, NULL, NULL, &errMsg);
	if (SQLITE_OK != rc)
	{
		fprintf(stderr, "can't delete record %s\n", errMsg);
		return -1;
	}

	return 0;
}

/*查找上报超时数据，视为异常
 *
 * */
static int record_query(const char *device_tab_name ,long int current_time ,int channel, pquery pf)//记录查询
{
	int      rc              = 0;
    char     *errMsg         = NULL;
    char**   pRecord         = NULL;
    int      row             = 0;
    int      column          = 0;
    char     sqlcmd[512]     = {0};

    if(db != NULL)
    {
		snprintf(sqlcmd, sizeof(sqlcmd), sql_search_data,device_tab_name ,current_time ,channel);//update_time <= %d-(period*3)  这里%d为当前时间current_time，两个合在一起
#if ENABLE_DB_DEBUG
	printf("%s\n",sqlcmd);
#endif
		rc = sqlite3_get_table(db, sqlcmd, &pRecord, &row, &column, &errMsg);//得到一維數組pRecord，此数据符合：记录的时间《=最迟记录时间 也就是迟于允许时间
		if (SQLITE_OK != rc) {
			fprintf(stderr, "can't get table %s\n",  errMsg);
			return -1;
		}
		pf(pRecord,row,column);//调用alarm process_query_result，带入一维数组数据
    }

	return 0 ;

}

/*  时间戳更新 */

int time_field_update(int addr, long int timestamp)
{
	int      rc              = 0;
	char*    errMsg          = NULL;
	char     sqlCmd[512]  = {0};

	snprintf(sqlCmd, sizeof(sqlCmd), sql_update_data_time_field, timestamp, addr);
	rc = sqlite3_exec(db, sqlCmd, NULL, NULL, &errMsg);
	if (SQLITE_OK != rc)
	{
		fprintf(stderr, "cat't update record %s\n", errMsg);
		return -1;
	}

	return 0;
}

/* 指定通道和序号的数据表　更新 */

int update_table_data(const char *device_tab_name ,long int timestamp ,int channel ,db_device_tab_t *device_tab)
{
	int      rc              = 0;
	char*    errMsg          = NULL;
	char     sqlCmd[512]  = {0};

	snprintf(sqlCmd, sizeof(sqlCmd), sql_update_data_channel_field,device_tab_name ,timestamp,device_tab->report_period,device_tab->current_value_H,device_tab->current_value_L,device_tab->device_state,device_tab->device_count_num,channel);
#if ENABLE_DB_DEBUG
	printf("%s\n",sqlCmd);
#endif
	rc = sqlite3_exec(db, sqlCmd, NULL, NULL, &errMsg);
	if (SQLITE_OK != rc)
	{
		fprintf(stderr, "cat't update record %s\n", errMsg);
		return -1;
	}

	return 0;
}

void filedb_close(void)
{
	sqlite3_close(db);
}
//查询设备>时间范围内所有数值
int record_query_up_time_region(const char *device_tab_name ,long int current_time ,int channel ,pquery pf)
{
	int      rc              = 0;
    char     *errMsg         = NULL;
    char**   pRecord         = NULL;
    int      row             = 0;
    int      column          = 0;
    char     sqlcmd[512]     = {0};

    if(db != NULL)
    {
		snprintf(sqlcmd, sizeof(sqlcmd), sql_search_up_time,device_tab_name ,current_time ,channel);//>=当前时间内地数据全部列出
#if ENABLE_DB_DEBUG
	printf("%s\n",sqlcmd);
#endif
		rc = sqlite3_get_table(db, sqlcmd, &pRecord, &row, &column, &errMsg);
		if (SQLITE_OK != rc) {
			fprintf(stderr, "can't get table %s\n",  errMsg);
			return -1;
		}
		pf(pRecord,row,column);//调用process_query_math 带入一维数组数据
    }

	return 0 ;
}
//查询设备时间范围内所有数值
static int record_query_time_region(const char *device_tab_name ,long int current_time ,int channel ,pquery pf)
{
	int      rc              = 0;
    char     *errMsg         = NULL;
    char**   pRecord         = NULL;
    int      row             = 0;
    int      column          = 0;
    char     sqlcmd[512]     = {0};

    if(db != NULL)
    {
		snprintf(sqlcmd, sizeof(sqlcmd), sql_search_down_time,device_tab_name ,current_time ,channel);//≤当前时间内地数据全部列出
#if ENABLE_DB_DEBUG
	printf("%s\n",sqlcmd);
#endif
		rc = sqlite3_get_table(db, sqlcmd, &pRecord, &row, &column, &errMsg);
		if (SQLITE_OK != rc) {
			fprintf(stderr, "can't get table %s\n",  errMsg);
			return -1;
		}
		pf(pRecord,row,column);//调用process_query_math 带入一维数组数据
    }

	return 0 ;
}
//查询大于时间的设备数据值--发送至上位机
void process_query_send(char** pr, int row, int column)
{

}
//查询设备时间范围内所有数值平均值,最大值,最小值
static void process_query_math(char** pr, int row, int column)
{
	int i, j;
	struct tm *p_tm;
	time_t timestamp;
	timestamp = time((time_t *) NULL);
	p_tm = localtime(&timestamp);
	int device_channel_num = 0;
	int device_state = 0;
//	int sequence_num = 0;
	char datavalue[30];
	/*序号，通道，更新时间，上传周期，数值Ｈ，数值Ｌ，状态 = 7列*/
	long long temp_result[10] = {0};//临时存放数据
	long double result[10] = {0};
	int channel_device_count[10] = {0};//每通道设备数量

	for (i = 1; i <= row; i++) //每一行查找
	{
		j = i * column; //得到每一行的首地址
		device_state = atoi(pr[j + 6]); //获取当前行的设备状态值(首地址+7相当于数组名+7)
//		sequence_num = atoi(pr[j]); //记录数目序号
		device_channel_num = atoi(pr[j + 1]); //设备通道
		temp_result[device_channel_num] = atoi(pr[j + 4]);
		temp_result[device_channel_num] <<= 32;
		temp_result[device_channel_num] |= (atoi(pr[j + 5])&&0xFFFFFFFF);//算出即时数据
		//求平均值
		channel_device_count[device_channel_num]++;
		if(channel_device_count[device_channel_num] <= 1)
		{
			gnnc_device_v_info[device_channel_num].average_value = temp_result[device_channel_num];
			result[device_channel_num] = temp_result[device_channel_num];
		}
		else
		{
			gnnc_device_v_info[device_channel_num].average_value = (result[device_channel_num]+temp_result[device_channel_num])/2;//旧数值+即时值 /2 =平均
		}
		//求最大值
		gnnc_device_v_info[device_channel_num].max_value = (gnnc_device_v_info[device_channel_num].max_value < temp_result[device_channel_num])?temp_result[device_channel_num]:gnnc_device_v_info[device_channel_num].max_value;
		//求最小值
		gnnc_device_v_info[device_channel_num].min_value = (gnnc_device_v_info[device_channel_num].max_value > temp_result[device_channel_num])?temp_result[device_channel_num]:gnnc_device_v_info[device_channel_num].max_value;
	}

}


//---------------------database_function_define_end-----------------------------


//-------------------------------Alarm------------------------------------------
static void process_query_result(char** pr, int row, int column)
{
	int i, j;
	int device_channel_num = 0;
	int device_state = 0;
//	int sequence_num = 0;
	char datavalue[30];
	for (i = 1; i <= row; i++) //每一行查找
	{
		j = i * column; //得到每一行的首地址
		device_state = atoi(pr[j + 6]); //获取当前行的设备状态值(首地址+7相当于数组名+7)
//		sequence_num = atoi(pr[j]); //记录数目序号
		device_channel_num = atoi(pr[j + 1]); //设备通道
		if(device_state == 1)
		{
			struct tm *p_tm;
			time_t timestamp;
			timestamp = time((time_t *) NULL);
			p_tm = localtime(&timestamp);
			sprintf(datavalue, "%02d%02d%02d", p_tm->tm_year + 1900,
					p_tm->tm_mon + 1, p_tm->tm_mday);
			GNNC_DEBUG_INFO("Device【%d】The offline time:%02d:%02d:%02d\n\n",device_channel_num, p_tm->tm_hour,
					p_tm->tm_min, p_tm->tm_sec);
			gnnc_device_v_info[device_channel_num].device_state = 0;//离线
			record_update_state(device_tab_name_list[device_channel_num] ,device_channel_num ,&gnnc_device_v_info[device_channel_num]);
		}
	}
}

/* SIGKILL信号处理 */
static void minitor_sig_hander(int signo)
{
    printf("db_minitor thread was killed\n");

	pthread_exit(0);
}

/* 每隔10s，轮询一下数据库 各表，判断是否有离线设备 */
/* 每隔2s,轮寻一下数据库 各表 ,判断 平均 ,最大 ,最小值*/

void* status_minitor(void* data)
{
	signal(SIGKILL,minitor_sig_hander);
	static unsigned int time_count = 0;
	while(1)
	{
		time_count = (time_count > 1000)?1:time_count+1;
		for(int i = 1; i < DEVICE_TAB_COUNT_NUM+1; i++)
		{
			//查询离线
			if(time_count % 10)
			{
				record_query(device_tab_name_list[i],time((time_t *)NULL ),i ,process_query_result);//查询
			}
			//查询最大,最小,平均
			if(time_count % 2)
			{
				record_query_time_region(device_tab_name_list[i],time((time_t *)NULL ),i ,process_query_math);
			}
		}
		sleep(1);
	}
	return NULL;
}

//-----------------------------Alarm——end---------------------------------------




//--------------------------database_SQL_NEW------------------------------------
sqlite3* db_new = NULL;
const char* file_database_path_new = "/root/SNAI.db";
const char* sql_query_data_new = "SELECT count(*) FROM sqlite_master device_info WHERE type='table' AND name='device_info';";
const char* sql_empty_data_new = "delete from %s;";
const char* sql_create_data_new = "CREATE TABLE IF NOT EXISTS device_info(id INTEGER , %s INTEGER PRIMARY KEY, device_handle INTEGER, update_time UNSIGNED,period INTEGER,value INTEGER,value1 INTEGER,state INTEGER);";
const char* sql_insert_data_new = "INSERT OR REPLACE INTO MAIN.device_info VALUES('%d', '%d', '%d', '%ld','%d','%d','%d',%d);";
const char* sql_delete_data_new = "DELETE FROM MAIN.device_info WHERE addr = %d;";
const char* sql_update_data_time_field_new = "UPDATE MAIN.device_info SET update_time = %ld where addr = %d;";
const char* sql_search_data_new = "SELECT * FROM MAIN.device_info WHERE update_time <= %ld-(period*1);";
const char* sql_search_net_new = "SELECT * FROM MAIN.device_info WHERE addr = %d;";
const char* sql_update_data_new = "UPDATE MAIN.device_info SET period = %d,value = %d WHERE addr = %d;";
const char* sql_update_state_new = "UPDATE MAIN.device_info SET state = %d WHERE addr = %d;";
//-------------------------database_SQL_END-------------------------------------

#ifdef __cplusplus //使用ｃ编译
}
#endif
