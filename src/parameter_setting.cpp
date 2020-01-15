/*
 * Parameter_setting.cpp
 *
 *  Created on: Dec 27, 2019
 *      Author: aron566
 */

#ifdef __cplusplus //使用ｃ编译
extern "C" {
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/time.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <ctype.h>
#include <fcntl.h>
#include "parameter_setting.h"
#include "cJSON.h"
#include "run_log.h"
#include "tcp_client.h"//用于设置tcp连接信息
#include "mqtt_opt.h"//用于设置mqtt连接信息
#include "serial_opt.h"
#include "db_opt.h"

#define GS_ERROR_INVAILD_PARAM -1
#define GS_SUCCESS 				0

static void cJSON_add_string_to_object(cJSON * const object, const char * const name, const char * const string);
static bool GNNC_strip_comments(char *string, char comment);
static void starup_read_config(void);


int GNNC_get_config_size(const char *module_name);


/*
 * 串口参数设置
 * */
volatile uint16_t uart_mode = REC_THROUGH_MODE;//REC_THROUGH_MODE;//配置全局透传模式

const char * config_file_path = "/root/GNNC_cnf.ini";


/*
 * 工装板名称配置
 * */
char board_name[64] = "GNNC_Factory";
/*
 *TCP_Client参数设置
 * */
char server_ip[16] = "192.168.1.37";
uint16_t port_num = 50000;

/*
 * mqtt参数设置
 * */
char mqtt_client_id[64] = "client_id";
char mqtt_user[64] = "mqtt_user1";
char mqtt_key[64] = "mqtt_user1";
char mqtt_server[32] = "192.168.1.62";
char mqtt_sub_topic[32] = "RCV";
char mqtt_pub_topic[32] = "SEND";


/**
* 去掉字符串内所有空白
* 且忽略注释部分
* 最终得到没有空白的字符串
**/
static bool GNNC_strip_comments(char *string, char comment)
{
  if (NULL == string || '\n' == *string || '\r' == *string) {
    return false; /* 第一个字符为回车或换行,表示空行 */
  }

  char *p, *q; /* 下面去掉字符串中所有空白字符 */
  for (p = q = string; *p != '\0' && *p != comment; p++) {
    if (0 == isspace(*p)) {
      *q++ = *p; /* 不是空白字符则重写字符串 */
    }
  }
  *q = '\0';

  return 0 != strlen(string); /* 字符串长度不为0,表示数据可用 */
}
/*
 * 初始化cjson结构配置数据
 *
 * */
void GNNC_init_config(void)
{
	GNNC_log_init();
	FILE  *fp 	= 	NULL;
	cJSON *root = 	NULL;

	if(access(config_file_path,F_OK | W_OK | R_OK) != -1)//如果配置文件存在，则读取文件配置项
	{
		GNNC_DEBUG_INFO("/root/GNNC_cnf.ini 文件已存在读取中...");//调试用
		starup_read_config();
	}
	else //初始化配置
	{
		GNNC_DEBUG_INFO("/root/GNNC_cnf.ini 文件不存在,创建中...");//调试用
		fp = fopen(config_file_path, "w+");
		if(NULL == fp)
		{
			PRINT_ERRMSG("fopen");
			exit(errno); /* 读文件错误直接按照错误码退出 */
		}
		root = cJSON_CreateObject();
		cJSON_AddStringToObject(root,"board_name",board_name);
		cJSON_AddNumberToObject(root,"port_num",port_num);
		cJSON_AddStringToObject(root,"serverip",server_ip);
		cJSON_AddNumberToObject(root,"uart_mode",uart_mode);
		cJSON_AddStringToObject(root,"mqtt_client_id",mqtt_client_id);
		cJSON_AddStringToObject(root,"mqtt_user",mqtt_user);
		cJSON_AddStringToObject(root,"mqtt_key",mqtt_key);
		cJSON_AddStringToObject(root,"mqtt_server",mqtt_server);
		cJSON_AddStringToObject(root,"mqtt_sub_topic",mqtt_sub_topic);
		cJSON_AddStringToObject(root,"mqtt_pub_topic",mqtt_pub_topic);
		cJSON_AddNumberToObject(root,"mqtt_report_cycle_channel1",5);
		cJSON_AddNumberToObject(root,"mqtt_report_cycle_channel2",5);
		cJSON_AddNumberToObject(root,"mqtt_report_cycle_channel3",5);
		cJSON_AddNumberToObject(root,"mqtt_report_cycle_channel4",5);
		cJSON_AddNumberToObject(root,"mqtt_report_cycle_channel5",5);
		cJSON_AddNumberToObject(root,"mqtt_report_cycle_channel6",5);
		cJSON_AddNumberToObject(root,"mqtt_report_cycle_channel7",5);
		cJSON_AddNumberToObject(root,"mqtt_report_cycle_channel8",5);
		cJSON_AddNumberToObject(root,"mqtt_report_cycle_channel9",5);
		cJSON_AddNumberToObject(root,"mqtt_report_cycle_channel10",5);
		char *out = cJSON_Print(root);
		GNNC_DEBUG_INFO("首次获取配置文件：%s",out);
		fprintf(fp,"%s",out);
		fclose(fp);//关闭才保存
		cJSON_free(out);
	}
	if (NULL != root)
	{
		cJSON_Delete(root);
	}
}

/*读取配置信息*/
static void starup_read_config(void)
{
	char parameter_cmd[32];
	int uart_mode_temp = 0;
	int report_cycle_temp = 0;
    sprintf(parameter_cmd,"%s","serverip");
    GNNC_read_config(parameter_cmd,server_ip,para_string);//读取配置服务器ip配置

    sprintf(parameter_cmd,"%s","port_num");
    GNNC_read_config(parameter_cmd,&port_num,para_int);//读取配置服务器端口配置信息

    sprintf(parameter_cmd,"%s","board_name");
    GNNC_read_config(parameter_cmd,board_name,para_string);//读取工装板信息

    sprintf(parameter_cmd,"%s","uart_mode");
    GNNC_read_config(parameter_cmd,&uart_mode_temp,para_int);//读取串口模式
    uart_mode = uart_mode_temp;

    sprintf(parameter_cmd,"%s","mqtt_server");
    GNNC_read_config(parameter_cmd,mqtt_server,para_string);//读取配置mqtt server ip

    sprintf(parameter_cmd,"%s","mqtt_client_id");
    GNNC_read_config(parameter_cmd,mqtt_client_id,para_string);//读取配置mqtt客户端id

    sprintf(parameter_cmd,"%s","mqtt_user");
    GNNC_read_config(parameter_cmd,mqtt_user,para_string);//读取配置mqtt 用户名

    sprintf(parameter_cmd,"%s","mqtt_key");
    GNNC_read_config(parameter_cmd,mqtt_key,para_string);//读取配置mqtt key

    sprintf(parameter_cmd,"%s","mqtt_sub_topic");
    GNNC_read_config(parameter_cmd,mqtt_sub_topic,para_string);//读取配置mqtt_sub_topic

    sprintf(parameter_cmd,"%s","mqtt_pub_topic");
    GNNC_read_config(parameter_cmd,mqtt_pub_topic,para_string);//读取配置mqtt_pub_topic

    sprintf(parameter_cmd,"%s","mqtt_report_cycle_channel1");
    GNNC_read_config(parameter_cmd,&report_cycle_temp,para_int);//读取配置mqtt上报周期
    gnnc_device_v_info[1].report_period = report_cycle_temp;

    sprintf(parameter_cmd,"%s","mqtt_report_cycle_channel2");
    GNNC_read_config(parameter_cmd,&report_cycle_temp,para_int);//读取配置mqtt上报周期
    gnnc_device_v_info[2].report_period = report_cycle_temp;

    sprintf(parameter_cmd,"%s","mqtt_report_cycle_channel3");
    GNNC_read_config(parameter_cmd,&report_cycle_temp,para_int);//读取配置mqtt上报周期
    gnnc_device_v_info[3].report_period = report_cycle_temp;

    sprintf(parameter_cmd,"%s","mqtt_report_cycle_channel4");
    GNNC_read_config(parameter_cmd,&report_cycle_temp,para_int);//读取配置mqtt上报周期
    gnnc_device_v_info[4].report_period = report_cycle_temp;

    sprintf(parameter_cmd,"%s","mqtt_report_cycle_channel5");
    GNNC_read_config(parameter_cmd,&report_cycle_temp,para_int);//读取配置mqtt上报周期
    gnnc_device_v_info[5].report_period = report_cycle_temp;

    sprintf(parameter_cmd,"%s","mqtt_report_cycle_channel6");
    GNNC_read_config(parameter_cmd,&report_cycle_temp,para_int);//读取配置mqtt上报周期
    gnnc_device_v_info[6].report_period = report_cycle_temp;

    sprintf(parameter_cmd,"%s","mqtt_report_cycle_channel7");
    GNNC_read_config(parameter_cmd,&report_cycle_temp,para_int);//读取配置mqtt上报周期
    gnnc_device_v_info[7].report_period = report_cycle_temp;

    sprintf(parameter_cmd,"%s","mqtt_report_cycle_channel8");
    GNNC_read_config(parameter_cmd,&report_cycle_temp,para_int);//读取配置mqtt上报周期
    gnnc_device_v_info[8].report_period = report_cycle_temp;

    sprintf(parameter_cmd,"%s","mqtt_report_cycle_channel9");
    GNNC_read_config(parameter_cmd,&report_cycle_temp,para_int);//读取配置mqtt上报周期
    gnnc_device_v_info[9].report_period = report_cycle_temp;

    sprintf(parameter_cmd,"%s","mqtt_report_cycle_channel10");
    GNNC_read_config(parameter_cmd,&report_cycle_temp,para_int);//读取配置mqtt上报周期
    gnnc_device_v_info[10].report_period = report_cycle_temp;
}

/*启动参数配置*/
void startup_config(int argc ,char *argv[])
{
	if(argc == 3)
	{
		printf("配置参数：%s 键值：%s\n",argv[1],argv[2]);
		if((!strcmp(argv[1],"port_num")) || (!strcmp(argv[1],"uart_mode")))
		{
			int data = atoi(argv[2]);
			GNNC_midfield_config(argv[1],&data,para_int);
		}
		else
		{
			GNNC_midfield_config(argv[1],argv[2],para_string);
		}
	}
	//重新读取配置
	starup_read_config();
}

/*设置工装板名称*/
int pat_set_board_name(char *name)
{
	char parameter_cmd[32];
	char temp_board[32];
	sprintf(parameter_cmd,"board_name");
	GNNC_read_config(parameter_cmd,temp_board,para_string);//读取配置
	if(strcmp(board_name,temp_board))
	{
		sprintf(board_name,"%s",name);
		GNNC_midfield_config(parameter_cmd,board_name,para_string);
		return 0;//刷新显示
	}
	else
	{
		printf("Warning 参数相同\n");
	}
	return -1;
}

/*
 * 设置串口工作模式
 *
 * 参数: mode:
 * */
uint8_t par_set_uart_mode(unsigned char mode)
{
	char parameter_cmd[32];
	int mode_temp = mode;
	if(mode_temp != uart_mode)
	{
		sprintf(parameter_cmd,"uart_mode");
		GNNC_midfield_config(parameter_cmd,&mode_temp,para_int);
		uart_mode = mode;
		printf("改模式-当前模式：%d\n",uart_mode);
		return switch_running_mode(uart_mode);
	}
	printf("未修改模式-当前模式：%d\n",uart_mode);
	return 0;
}


/*设置TCP IP*/
int pat_set_tcp_server(char *ip_addr)
{
	char parameter_cmd[32];
	if(ip_addr != NULL)
	{
		sprintf(parameter_cmd,"serverip");
		sprintf(server_ip,"%s",ip_addr);
		GNNC_midfield_config(parameter_cmd,server_ip,para_string);
		return socket_force_reconnect();
	}
	return -1;
}

/*设置TCP IP port*/
int pat_set_tcp_server_port(uint16_t port)
{
	char parameter_cmd[32];
	if(port != 0 && port != port_num)
	{
		sprintf(parameter_cmd,"port_num");
		port_num = port;
		GNNC_midfield_config(parameter_cmd,&port_num,para_int);
		printf("port_num = %u\n",port_num);
		return socket_force_reconnect();
	}
	return -1;
}
/*设置MQTT IP*/
int par_set_mqtt_server(char *ip_addr)
{
	char parameter_cmd[32];
	char temp_server[32];

	if(ip_addr != NULL)
	{
		sprintf(parameter_cmd,"mqtt_server");
		GNNC_read_config(parameter_cmd,temp_server,para_string);//读取配置mqtt server ip
		if(strcmp(ip_addr,temp_server))
		{
			sprintf(mqtt_server,"%s",ip_addr);
			GNNC_midfield_config(parameter_cmd,mqtt_server,para_string);
			return switch_mqtt_server();
		}
		else
		{
			printf("Warning 参数相同\n");
		}
	}
	return -1;
}

/*设置MQTT client id*/
int par_set_mqtt_client_id(char *id)
{
	char parameter_cmd[32];
	if(id != NULL)
	{
		sprintf(parameter_cmd,"mqtt_client_id");
		sprintf(mqtt_client_id,"%s",id);
		GNNC_midfield_config(parameter_cmd,mqtt_client_id,para_string);
		return switch_mqtt_server();
	}
	return -1;
}

/*设置MQTT user*/
int par_set_mqtt_client_user(char *user)
{
	char parameter_cmd[32];
	if(user != NULL)
	{
		sprintf(parameter_cmd,"mqtt_user");
		sprintf(mqtt_user,"%s",user);
		GNNC_midfield_config(parameter_cmd,mqtt_user,para_string);
		return switch_mqtt_server();
	}
	return -1;
}

/*设置MQTT user key*/
int par_set_mqtt_client_key(char *key)
{
	char parameter_cmd[32];
	if(key != NULL)
	{
		sprintf(parameter_cmd,"mqtt_key");
		sprintf(mqtt_key,"%s",key);
		GNNC_midfield_config(parameter_cmd,mqtt_key,para_string);
		return switch_mqtt_server();
	}
	return -1;
}

/*设置MQTT user subtopic*/
int par_set_mqtt_client_sub(char *subtopic)
{
	char parameter_cmd[32];
	if(subtopic != NULL)
	{
		sprintf(parameter_cmd,"mqtt_sub_topic");
		sprintf(mqtt_sub_topic,"%s",subtopic);
		GNNC_midfield_config(parameter_cmd,mqtt_sub_topic,para_string);
		return switch_mqtt_server();
	}
	return -1;
}

/*设置MQTT user pubtopic*/
int par_set_mqtt_client_pub(char *pubtopic)
{
	char parameter_cmd[32];
	if(pubtopic != NULL)
	{
		sprintf(parameter_cmd,"mqtt_pub_topic");
		sprintf(mqtt_pub_topic,"%s",pubtopic);
		GNNC_midfield_config(parameter_cmd,mqtt_pub_topic,para_string);
		return switch_mqtt_server();
	}
	return -1;
}

/*设置mqtt上报周期*/
int par_set_mqtt_report_cycle(uint16_t cycle ,uint8_t channel_num)
{
	char parameter_cmd[32];
	int interval = cycle;
	if(channel_num > UART_NUM_MAX)
	{
		printf("无效参数\n");
		return -1;
	}
	if(gnnc_device_v_info[channel_num].report_period != cycle)
	{
		sprintf(parameter_cmd,"mqtt_report_cycle_channel%u",channel_num);
		GNNC_midfield_config(parameter_cmd,&interval,para_int);
		gnnc_device_v_info[channel_num].report_period = (interval&0xFFFF);
		printf("改通道%u周期-当前周期：%d\n",channel_num ,gnnc_device_v_info[channel_num].report_period);
		return 0;
	}
	printf("未修改通道%u周期-当前周期：%d\n",channel_num ,gnnc_device_v_info[channel_num].report_period);
	return 0;
}
/*
 * 读取配置文件中指定键值对
 *
 * 参数identifier：键值名称
 *
 * value：读取的值写入
 *
 * data_type数值类型：０代表整数，１代表double，２代表字符串
 *
 * 返回０读取成功，－１读取失败
 * */
int GNNC_read_config(char *identifier,void *value,para_data_type_t data_type)
{
	FILE  *fp 	= 	NULL;
	cJSON *root = 	NULL;
	char str[1024] = {0};
	char txt[64] = {0};
	fp = fopen(config_file_path, "r");
	if(NULL == fp) {
		PRINT_ERRMSG("fopen");
		exit(errno); /* 读文件错误直接按照错误码退出 */
	}

//	GNNC_DEBUG_INFO("/root/GNNC_cnf.ini 读取配置文件中...");//调试用
	while((fgets(txt,64,fp))!=NULL)//循环读取64字节,如果没有数据则退出循环
	{ /* 去掉字符串所有空白,注释也忽略 */
		if (GNNC_strip_comments(txt, ' '))
		{
			strcat(str,txt);//拼接字符串
		} /* end SNAI_strip_comments */
	}
//	printf("配置文件大小:\%dByte\n",strlen(str));
	root = cJSON_Parse(str);
	cJSON *item = cJSON_GetObjectItem(root,identifier);
	switch(data_type)
	{
	case para_int:
		if(item != NULL)
		{
			if(item->type == cJSON_Number)
			{
				*(int*)value = item->valueint;
				printf("获取数值：%d\n",item->valueint);
			}
		}
		break;
	case para_double:
		if(item != NULL)
		{
			if(item->type == cJSON_Number)
			{
				*(double*)value = item->valuedouble;
			}
		}
		break;
	case para_string:
		if(item != NULL)
		{
			if(item->type == cJSON_String)
			{
				strcpy((char*)value,item->valuestring);
				printf("获取参数：%s\n",(char*)value);
			}
		}
		break;
	default:
		printf("读取ERROR!\n");
		return -1;
		break;
	}
	fclose(fp);//关闭才保存
	if (NULL != root)
	{
		cJSON_Delete(root);
	}
	return 0;
}
/*
 * 修改配置文件中键值对
 *
 * 参数identifier：键值名称
 *
 * value：值
 *
 * data_type数值类型：０代表整数，１代表double，２代表字符串
 * */
int GNNC_midfield_config(char *identifier,void *value,para_data_type_t data_type)
{
	FILE  *fp 	= 	NULL;
	cJSON *root = 	NULL;
	char str[1024] = {0};
	char txt[64] = {0};
	int int_value;
	double float_value;
	char *char_value = NULL;
	fp = fopen(config_file_path, "r");
	if(NULL == fp) {
		PRINT_ERRMSG("fopen");
		exit(errno); /* 读文件错误直接按照错误码退出 */
	}

	GNNC_DEBUG_INFO("/root/GNNC_cnf.ini 修改配置文件中...");//调试用

	while((fgets(txt,64,fp))!=NULL)//循环读取64字节,如果没有数据则退出循环
	{ /* 去掉字符串所有空白,注释也忽略 */
		if (GNNC_strip_comments(txt, ' '))
		{
			strcat(str,txt);//拼接字符串
		} /* end SNAI_strip_comments */
	}
	fclose(fp);//关闭才保存
	usleep(100);
	fp = fopen(config_file_path, "w+");
	root = cJSON_Parse(str);
	cJSON *item = cJSON_GetObjectItem(root,identifier);
	switch(data_type)
	{
	case para_int:
//		cJSON_DeleteItemFromObject(root, identifier);
		int_value = *(int*)value;
		printf("设置值：%d\n",int_value);
		cJSON_SetIntValue(item, int_value);
//		cJSON_AddNumberToObject(root,identifier,int_value);
		break;
	case para_double:
//		cJSON_DeleteItemFromObject(root, identifier);
		float_value = *(double*)value;
		printf("设置值：%.1f\n",float_value);
		cJSON_SetIntValue(item, float_value);
//		cJSON_AddNumberToObject(root,identifier,float_value);
		break;
	case para_string:
//		cJSON_DeleteItemFromObject(root, identifier);
		char_value = (char*)value;
		printf("设置值：%s\n",char_value);
//		cJSON_AddStringToObject(root,identifier,char_value);
		cJSON_add_string_to_object(root, identifier, char_value);
		break;
	default:
		printf("设置ERROR!\n");
		return -1;
		break;
	}
	char *out = cJSON_Print(root);
	fprintf(fp,"%s",out);
	fclose(fp);//关闭才保存
//	printf("关闭文件！\n");
	GNNC_DEBUG_INFO("修改后获取配置文件：%s",out);
	cJSON_free(out);
	if (NULL != root)
	{
		cJSON_Delete(root);
	}
//	printf("释放！\n");
	return 0;
}

static void cJSON_add_string_to_object(cJSON * const object, const char * const name, const char * const string)
{
    cJSON *item;

    if (object == NULL || name == NULL || string == NULL)
    {
        return;
    }

    item = cJSON_GetObjectItemCaseSensitive(object, name);
    // item not exist
    if (item == NULL)
    {
        cJSON_AddStringToObject(object, name, string);  // just addstring
    }
    // item exist
    else
    {
        cJSON_free(item->valuestring); // free present valuestring
        item->valuestring = strdup(string); // malloc and init new valuestring
    }
}



#define UNICODE_VALID(Char)                   \
    ((Char) < 0x110000 &&                     \
     (((Char) & 0xFFFFF800) != 0xD800))
#define UTF8_GET(Result, Chars, Count, Mask, Len)   \
    (Result) = (Chars)[0] & (Mask);                 \
    for((Count) = 1; (Count) < (Len); ++(Count))    \
    {									            \
        if (((Chars)[(Count)] & 0xc0) != 0x80)     \
	    {                                           \
	        (Result) = -1;                          \
	        break;							        \
	    }                                           \
        (Result) <<= 6;                             \
        (Result) |= ((Chars)[(Count)] & 0x3f);      \
    }
#define UTF8_COMPUTE(Char, Mask, Len)         \
    if (Char < 128)                           \
    {                                         \
      Len = 1;                                \
      Mask = 0x7f;                            \
    }                                         \
    else if ((Char & 0xe0) == 0xc0)           \
    {                                         \
      Len = 2;                                \
      Mask = 0x1f;                            \
    }                                         \
    else if ((Char & 0xf0) == 0xe0)           \
    {                                         \
      Len = 3;                                \
      Mask = 0x0f;                            \
    }          	                              \
    else if ((Char & 0xf8) == 0xf0)           \
    {									      \
      Len = 4;                                \
      Mask = 0x07;                            \
    }									      \
    else if ((Char & 0xfc) == 0xf8)           \
    {									      \
      Len = 5;                                \
      Mask = 0x03;                            \
    }									      \
    else if ((Char & 0xfe) == 0xfc)           \
    {									      \
      Len = 6;                                \
      Mask = 0x01;                            \
    }									      \
    else                                      \
    {                                         \
      Len = 0;                                \
      Mask = 0;                               \
    }
#define UTF8_LENGTH(Char)              \
  ((Char) < 0x80 ? 1 :                 \
   ((Char) < 0x800 ? 2 :               \
    ((Char) < 0x10000 ? 3 :            \
     ((Char) < 0x200000 ? 4 :          \
      ((Char) < 0x4000000 ? 5 : 6)))))

int leda_string_validate_utf8(const char *str, int len)
{
    unsigned char *p;
    unsigned char *end;

    p = (unsigned char*)str;
    end = p + len;

    while(p < end)
    {
        int i, mask, char_len;
        uint32_t result;

        /* nul bytes considered invalid */
        if (*p == '\0')
        {
            break;
        }
        if(*p < 128)
        {
            ++p;
            continue;
        }

        UTF8_COMPUTE (*p, mask, char_len);

        if(char_len == 0)  /* ASCII: char_len == 1 */
        {
            break;
        }

        if((end - p) < char_len)
        {
            break;
        }
        UTF8_GET (result, p, i, mask, char_len);

        if(UTF8_LENGTH (result) != char_len) /* ASCII: UTF8_LENGTH == 1 */
        {
            break;
        }
        if(!UNICODE_VALID (result)) /* ASCII: always valid */
        {
            break;
        }

        p += char_len;
    }
    if(p != end)
    {
        return GS_ERROR_INVAILD_PARAM;
    }
    else
    {
        return GS_SUCCESS;
    }
}
//
//static int _leda_get_config_info(leda_data_info_e type, const char *key, char **value)
//{
//    int                 ret = LE_SUCCESS;
//    leda_retinfo_t      retinfo;
//    char                *request_key = NULL;
//    DBusMessage         *msg_call = NULL;
//
//    if (INFO_TYPE_DEVICE_CONFIG == type)
//    {
//        request_key = malloc(strlen(key) + strlen(CONFIGMANAGER_DEVICE_HEADER) + 1);
//        if (NULL == request_key)
//        {
//            log_e(LEDA_TAG_NAME, "no memory\r\n");
//            return LE_ERROR_ALLOCATING_MEM;
//        }
//
//        memset(request_key, 0, (strlen(key) + strlen(CONFIGMANAGER_DEVICE_HEADER) + 1));
//        snprintf(request_key, (strlen(key) + strlen(CONFIGMANAGER_DEVICE_HEADER) + 1), "%s%s", CONFIGMANAGER_DEVICE_HEADER, key);
//    }
//    else if (INFO_TYPE_PRODUCT_TSL == type)
//    {
//        request_key = malloc(strlen(key) + strlen(CONFIGMANAGER_TSL_HEADER) + 1);
//        if (NULL == request_key)
//        {
//            log_e(LEDA_TAG_NAME, "no memory\r\n");
//            return LE_ERROR_ALLOCATING_MEM;
//        }
//
//        memset(request_key, 0, (strlen(key) + strlen(CONFIGMANAGER_TSL_HEADER) + 1));
//        snprintf(request_key, (strlen(key) + strlen(CONFIGMANAGER_TSL_HEADER) + 1), "%s%s", CONFIGMANAGER_TSL_HEADER, key);
//    }
//    else
//    {
//        return LE_ERROR_UNKNOWN;
//    }
//
//    msg_call = _leda_create_methodcall(DMP_CONFIGMANAGER_WELL_KNOW_NAME, DMP_CONFIGMANAGER_METHOD_GET);
//    if (NULL == msg_call)
//    {
//        log_e(LEDA_TAG_NAME, "create method call failed\r\n");
//        free(request_key);
//        return LE_ERROR_UNKNOWN;
//    }
//
//    dbus_message_append_args(msg_call, DBUS_TYPE_STRING, &request_key, DBUS_TYPE_INVALID);
//    ret = _leda_send_with_replay(msg_call, 10000, &retinfo);
//    dbus_message_unref(msg_call);
//    free(request_key);
//
//    if (LE_SUCCESS != ret || ((LE_SUCCESS != retinfo.code) || (NULL == retinfo.params) || (!strcmp(retinfo.params, ""))))
//    {
//        log_e(LEDA_TAG_NAME, "call method failed, ret:%d code:%d\r\n", ret, retinfo.code);
//        leda_retinfo_free(&retinfo);
//        return LE_ERROR_UNKNOWN;
//    }
//
//    *value = malloc(strlen(retinfo.params) + 1);
//    if (NULL == *value)
//    {
//        log_e(LEDA_TAG_NAME, "no memory\r\n");
//        leda_retinfo_free(&retinfo);
//        return LE_ERROR_UNKNOWN;
//    }
//    strcpy(*value, retinfo.params);
//    leda_retinfo_free(&retinfo);
//
//    return ret;
//}
///*
// * 获取驱动配置信息长度.
// *
// * module_name:  驱动模块名称
// *
// * 阻塞接口, 成功返回config长度, 失败返回0.
// */
//int GNNC_get_config_size(const char *module_name)
//{
//    int     ret     = 0;
//    char    *value  = NULL;
//
//    if ((NULL == module_name) ||
//        (GS_SUCCESS != leda_string_validate_utf8(module_name, strlen(module_name))))
//    {
//    	GNNC_DEBUG_INFO(LEDA_TAG_NAME, "module_name is invalid\r\n");
//        return 0;
//    }
//
//    ret = _leda_get_config_info(INFO_TYPE_DEVICE_CONFIG, module_name, &value);
//    if ((GS_SUCCESS == ret) && (NULL != value))
//    {
//        ret = strlen(value) + 1;
//        free(value);
//
//        return ret;
//    }
//
//    return 0;
//}
//
///*
// * 获取驱动配置信息.
// *
// * module_name:  驱动模块名称
// * config:       配置信息, 需要提前申请好内存传入.
// * size:         config长度, 该长度通过leda_get_config_size接口获取, 如果传入config比实际配置内容长度短, 会返回LE_ERROR_INVAILD_PARAM.
// *
// * 阻塞接口, 成功返回LE_SUCCESS, 失败返回错误码.
// */
//int GNNC_get_config(const char *module_name, char *config, int size)
//{
//    int     ret     = GS_SUCCESS;
//    char    *value  = NULL;
//
//    if ((NULL == module_name) ||
//        (GS_SUCCESS != leda_string_validate_utf8(module_name, strlen(module_name))))
//    {
//    	GNNC_DEBUG_INFO( "module_name is invalid\r\n");
//        return GS_ERROR_INVAILD_PARAM;
//    }
//
//    if (NULL == config)
//    {
//    	GNNC_DEBUG_INFO("config is invalid\r\n");
//        return GS_ERROR_INVAILD_PARAM;
//    }
//
//    if (0 >= size)
//    {
//    	GNNC_DEBUG_INFO("size is invalid\r\n");
//        return GS_ERROR_INVAILD_PARAM;
//    }
//
//    ret = _leda_get_config_info(INFO_TYPE_DEVICE_CONFIG, module_name, &value);
//    if ((GS_SUCCESS == ret) && (NULL != value))
//    {
//        if (size < (strlen(value) + 1))
//        {
//        	GNNC_DEBUG_INFO("size is less than real config info\r\n");
//            free(value);
//            return GS_ERROR_INVAILD_PARAM;
//        }
//
//        strncpy(config, value, (strlen(value) + 1));
//        free(value);
//    }
//
//    return ret;
//}



//int filesize = 0;
//将文件指针移到末尾
//fseek(fp,0,SEEK_END);
//filesize = ftell(fp);//通过ftell函数获得指针到文件头的偏移字节数。
//str=(char *)malloc(filesize);//动态分配str内存
//fseek(fp,0,SEEK_SET);
//rewind(fp);
#ifdef __cplusplus //使用ｃ编译
}
#endif

