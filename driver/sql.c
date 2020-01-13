/*
 * sql.c
 *
 *  Created on: Mar 20, 2019
 *      Author: max
 *
 */
#ifdef __cplusplus //使用ｃ编译
extern "C" {
#endif
#include "includes.h"

static MYSQL* mysql = NULL;

int sql_connect()
{
	if(NULL == mysql)
	{
		mysql = mysql_init(NULL);
		mysql_library_init(0, NULL, NULL);
		unsigned int timeout = 1000;
		mysql_options(mysql, MYSQL_OPT_CONNECT_TIMEOUT, &timeout);
	}

	if(mysql_real_connect(mysql, "112.74.167.243", "max", "better", "gas_leak_monitor_db", 3306, NULL, 0) == NULL)
	{
		LOG("connect failed: %s\n", mysql_error(mysql));
		return -1;
	}

	LOG("mysql connect success.\n");
	return 0;
}

void sql_disconnect()
{
	mysql_close(mysql);
	mysql_library_end();
	mysql = NULL;
}

static int sql_cmd_send(char* cmd_str)
{
	if (mysql_query(mysql, cmd_str))
	{
		LOG("sql failed: %s[%s]\n", mysql_error(mysql),cmd_str);
		return -1;
	}
	return 0;
}

int sql_realtime_data_update(st_sql_data* data)
{
	char cmd[MAX_SQL_CMD_LEN] = {0};
	sprintf(cmd,"replace into gas_leak_realtime_data(id,time,gatewayID,devID,dataID,value) values(%d,%d,'%s',%d,%d,%f);",
			0,data->time,data->gatewayID,data->devID,data->dataID,data->value);

	return sql_cmd_send(cmd);
}

int sql_history_data_update(st_sql_data* data)
{
	char cmd[MAX_SQL_CMD_LEN] = {0};
	sprintf(cmd,"replace into gas_leak_history_data(time,gatewayID,devID,dataID,value) values(%d,'%s',%d,%d,%f);",
			data->time,data->gatewayID,data->devID,data->dataID,data->value);

	return sql_cmd_send(cmd);
}

int sql_realtime_image_update(st_sql_image* data)
{
	char cmd[MAX_SQL_CMD_LEN] = {0};
	sprintf(cmd,"replace into gas_leak_realtime_image(time,gatewayID,devID,dataID,value) values(%d,'%s',%d,%d,'%s');",
			data->time,data->gatewayID,data->devID,data->dataID,data->image);

	return sql_cmd_send(cmd);
}

int sql_history_image_update(st_sql_image* data)
{
	char cmd[MAX_SQL_CMD_LEN] = {0};
	sprintf(cmd,"replace into gas_leak_history_image(time,gatewayID,devID,dataID,value) values(%d,'%s',%d,%d,'%s');",
			data->time,data->gatewayID,data->devID,data->dataID,data->image);

	return sql_cmd_send(cmd);
}
//
int sql_realtime_select()
{
	char cmd[MAX_SQL_CMD_LEN] = {0};
	sprintf(cmd,"select * from realtime_data order by id DESC limit 1;");
	LOG("command select\n");
	if (mysql_query(mysql, cmd))
	{
		LOG("query failed: %s\n", mysql_error(mysql));
		return -1;
	}
	MYSQL_RES *result = mysql_use_result(mysql);
	if(result==NULL)
	{
		return -1;
	}
	int num_fields=mysql_num_fields(result);
	printf("mysql_use_result列：%d\n",num_fields);

	MYSQL_ROW row;
	while(NULL!=(row = mysql_fetch_row(result)))
	{
		int i = 0;
		for(i = 0; i < num_fields; i++)
		{
			printf("|%s|\t",row[i]);
		}
		printf("\n");
	}
	mysql_free_result(result);

	return 0;
}
#ifdef __cplusplus //使用ｃ编译
}
#endif
