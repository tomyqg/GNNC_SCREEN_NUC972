/*
 * Parameter_setting.h
 *
 *  Created on: Dec 27, 2019
 *      Author: aron566
 */

#ifndef INCLUDE_PARAMETER_SETTING_H_
#define INCLUDE_PARAMETER_SETTING_H_
#ifdef __cplusplus //使用ｃ编译
extern "C" {
#endif
#define PRINT_ERRMSG(STR) fprintf(stderr,"line:%d,msg:%s,eMsg:%s\n", __LINE__, STR, strerror(errno))


/*设置全局串口与上位机通讯模式*/
#define REC_THROUGH_MODE 0x01U
#define REC_NOT_THROUGH_MODE 0x00U
extern volatile uint16_t uart_mode;

/*启动参数配置*/
void startup_config(int argc ,char *argv[]);
/*设置工装板名称*/
extern char board_name[];

/*设置全局服务器TCP ip地址及端口号*/
extern char server_ip[];
extern unsigned short port_num;

 /* mqtt参数设置*/
extern char mqtt_client_id[];
extern char mqtt_user[];
extern char mqtt_key[];
extern char mqtt_server[];
extern char mqtt_sub_topic[];
extern char mqtt_pub_topic[];

/*初始化参数键值对*/
void GNNC_init_config(void);

typedef enum{
	para_int,
	para_double,
	para_string
}para_data_type_t;

/*参数修改－＿－修改键值对*/
int GNNC_midfield_config(char *identifier,void *value,para_data_type_t data_type);
/*参数读取－＿－读取键值对*/
int GNNC_read_config(char *identifier,void *value,para_data_type_t data_type);

/*设置工装板名称*/
int pat_set_board_name(char *name);
/*设置串口工作模式*/
uint8_t par_set_uart_mode(unsigned char mode);
/*设置MQTT IP*/
int par_set_mqtt_server(char *ip_addr);
/*设置MQTT client id*/
int par_set_mqtt_clent_id(char *id);
/*设置MQTT user*/
int par_set_mqtt_clent_user(char *user);
/*设置MQTT user key*/
int par_set_mqtt_clent_key(char *key);
/*设置MQTT user subtopic*/
int par_set_mqtt_clent_sub(char *subtopic);
/*设置MQTT user pubtopic*/
int par_set_mqtt_clent_pub(char *pubtopic);

/*设置TCP IP*/
int pat_set_tcp_server(char *ip_addr);
/*设置TCP IP port*/
int pat_set_tcp_server_port(uint16_t port);

#ifdef __cplusplus //使用ｃ编译
}
#endif
#endif /* INCLUDE_PARAMETER_SETTING_H_ */
