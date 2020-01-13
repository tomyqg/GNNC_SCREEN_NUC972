/*
 * sql.h
 *
 *  Created on: Mar 20, 2019
 *      Author: max
 */

#ifndef DATA_SQL_H_
#define DATA_SQL_H_
#ifdef __cplusplus //使用ｃ编译
extern "C" {
#endif
#define MAX_DATA_LEN 32
#define MAX_SQL_CMD_LEN 512


typedef struct
{
	int time;
	char gatewayID[16];
	int devID;
	int dataID;
	float value;
}st_sql_data;

typedef struct
{
	int time;
	char gatewayID[16];
	int devID;
	int dataID;
	char image[16];
}st_sql_image;



void sql_disconnect();
int sql_connect();
int sql_realtime_data_update(st_sql_data* data);
int sql_history_data_update(st_sql_data* data);
int sql_realtime_image_update(st_sql_image* data);
int sql_history_image_update(st_sql_image* data);
#ifdef __cplusplus //使用ｃ编译
}
#endif
#endif /* DATA_SQL_H_ */
