/*
 * main.c
 *
 *  Created on: Dec 17, 2019
 *      Author: aron566
 */
#include <signal.h>
#include <pthread.h>
#include <string.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <stdio.h>
#include "guilite_opt.h"//独立线程
#include "key_button.h"//独立线程
#include "timer_manager.h"//独立线程
#include "modbus_opt.h"//独立线程
#include "tcp_client.h"//独立线程
#include "serial_opt.h"//独立线程
#include "parameter_setting.h"//参数初始化
#include "CircularQueue.h"//环形队列管理
#include "db_opt.h"
#include "mqtt_opt.h"
typedef struct pthread_opt
{
	void* (*pthread_opt_func)(void *data);
	void*  data;
	pthread_t pthread_id;
}pthread_opt_user;
pthread_opt_user* opt_seq_ptr = NULL;

void pthread_opt_seq_kill(pthread_opt_user *op)
{
	int i = 0;
	while(op[i].pthread_opt_func!=NULL)
	{
		pthread_kill(op[i].pthread_id, SIGKILL);
		i++;
	}
}

int pthread_opt_seq_exec(pthread_opt_user *op)
{
	int i = 0;
	int err = 0;
	while(op[i].pthread_opt_func!=NULL)
	{
		err = pthread_create(&op[i].pthread_id,NULL,op[i].pthread_opt_func, op[i].data);
		if(err)
			break;
		i++ ;
	}
	i--;
	printf("The create ok!\r\n");
	while(i >= 0)
	{
		err = pthread_join(op[i].pthread_id, NULL) ;
		if(err)
			break;
		i-- ;
	}
    printf("The process over!\r\n");
	if(err)
		pthread_opt_seq_kill(op);

	return  err;
}

void main_thread_hander(int signo)
{
	printf("game over!\r\n");
	pthread_mutex_destroy(&GNNC_Decode_mutex_lock);
	pthread_cond_destroy(&GNNC_Decode_cond);
	pthread_opt_seq_kill(opt_seq_ptr);
}
int main(int argc, char *argv[])
{
	//GUILITE初始化
	GuiLite_Init();

	/* 2K rx cache for usart rx*/
	char path_0[] = "/dev/shm/gnnc0_cache_XXXXXX";
	char path_1[] = "/dev/shm/gnnc1_cache_XXXXXX";
	char path_2[] = "/dev/shm/gnnc2_cache_XXXXXX";
	char path_3[] = "/dev/shm/gnnc3_cache_XXXXXX";
	char path_4[] = "/dev/shm/gnnc4_cache_XXXXXX";
	char path_5[] = "/dev/shm/gnnc5_cache_XXXXXX";
	char path_6[] = "/dev/shm/gnnc6_cache_XXXXXX";
	char path_7[] = "/dev/shm/gnnc7_cache_XXXXXX";
	char path_8[] = "/dev/shm/gnnc8_cache_XXXXXX";
	char path_9[] = "/dev/shm/gnnc9_cache_XXXXXX";
	char path_10[] = "/dev/shm/gnnc10_cache_XXXXXX";
	circular_buffer* cb_tcpip_rx = cb_create(path_0,11);//2KB
    circular_buffer* cb_usart1_rx = cb_create(path_1,9);//512Byte
    circular_buffer* cb_usart2_rx = cb_create(path_2,9);
    circular_buffer* cb_usart3_rx = cb_create(path_3,9);
    circular_buffer* cb_usart4_rx = cb_create(path_4,9);
    circular_buffer* cb_usart5_rx = cb_create(path_5,9);
    circular_buffer* cb_usart6_rx = cb_create(path_6,9);
    circular_buffer* cb_usart7_rx = cb_create(path_7,9);
    circular_buffer* cb_usart8_rx = cb_create(path_8,9);
    circular_buffer* cb_usart9_rx = cb_create(path_9,9);
    circular_buffer* cb_usart10_rx = cb_create(path_10,9);
    polling_msg[0].cb = cb_tcpip_rx;
    polling_msg[1].cb = cb_usart1_rx;
    polling_msg[2].cb = cb_usart2_rx;
    polling_msg[3].cb = cb_usart3_rx;
    polling_msg[4].cb = cb_usart4_rx;
    polling_msg[5].cb = cb_usart5_rx;
    polling_msg[6].cb = cb_usart6_rx;
    polling_msg[7].cb = cb_usart7_rx;
    polling_msg[8].cb = cb_usart8_rx;
    polling_msg[9].cb = cb_usart9_rx;
    polling_msg[10].cb = cb_usart10_rx;
    /*建立线程*/
	pthread_opt_user pthread_user_seq[] =
	{
		{usart1_rx_start,  cb_usart1_rx},
		{usart2_rx_start,  cb_usart2_rx},
		{usart3_rx_start,  cb_usart3_rx},
		{usart4_rx_start,  cb_usart4_rx},
		{usart5_rx_start,  cb_usart5_rx},
		{usart6_rx_start,  cb_usart6_rx},
		{usart7_rx_start,  cb_usart7_rx},
		{usart8_rx_start,  cb_usart8_rx},
		{usart9_rx_start,  cb_usart9_rx},
		{usart10_rx_start, cb_usart10_rx},
//		{tcp_clent_start,  cb_tcpip_rx},
//		{modbus_master_decode_start, cb_usart1_rx},
//		{modbus_master_decode_start, cb_usart2_rx},
		{modbus_slave_decode_start, cb_tcpip_rx},
		{status_minitor,&gnnc_device_v_info},
		//{stdin_thread,NULL},
		{key_check_loop,NULL},
		{guilite_display_opt,&gui_hardware_info_value},/*图形显示*/
		{misc_run_loop,NULL},//定时器任务管理
		{start_mqtt ,NULL},//启动mqtt
		{NULL ,NULL}//
	};
	opt_seq_ptr = pthread_user_seq;

	signal(SIGINT , main_thread_hander);
	signal(SIGKILL, main_thread_hander);
	signal(SIGTERM, main_thread_hander);
	//初始化数据库
	init_database();
	//初始化配置
	GNNC_init_config();
	//读取启动参数
	startup_config(argc ,argv);
	//初始化串口
	usart_init();
	//tcp初始化
	tcp_connect_start_init();
	/*启动线程*/
	pthread_opt_seq_exec(pthread_user_seq);//启动线程
	printf("clean up!\r\n");
	return 0;
}
