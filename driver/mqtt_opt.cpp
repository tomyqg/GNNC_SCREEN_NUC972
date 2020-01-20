/*
 * mqtt_opt.cpp
 *
 *  Created on: Jan 4, 2020
 *      Author: aron566
 */
#include "timer_manager.h"//定时器管理
#ifdef __cplusplus //使用ｃ编译
extern "C" {
#endif


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include "mosquitto.h"
#include "mqtt_opt.h"
#include "parameter_setting.h"

//#define HOST "localhost"

#define PORT  1883
#define KEEP_ALIVE 60
#define MSG_MAX_SIZE  512


static void *mqtt_reconnect_poll_loop(void* ptmr);
static void call_c_timer_task(c_timer_task_opt *timer ,c_timer_manager_t *task ,bool on_off);
int demo_publisher(void);
int demo_subscribe(void);


bool session = true;
struct mosquitto *mosq = NULL;

c_timer_manager_t mqtt_reconnect_poll =
{
	.interval_seconds = 1,
	.timer_on_off = true,
	.run_state = false,
	.p_task = mqtt_reconnect_poll_loop,//建立任务
	.pid = 0,
	.offset = 0,
	.data = NULL
};

/* SIGKILL信号处理 */
void mqtt_sig_hander(int signo)
{
    printf("mqtt thread was killed\n");
    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();
	pthread_exit(0);
}

void *start_mqtt(void *data)
{
	signal(SIGKILL,mqtt_sig_hander);
	demo_subscribe();
	//demo_publisher();
	return NULL;
}


void my_message_callback(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *message)
{
    if(message->payloadlen){
        printf("%s %s\n", message->topic, (char*)message->payload);
    }else{
        printf("%s (null)\n", message->topic);
    }
    fflush(stdout);
}

void my_connect_callback(struct mosquitto *mosq, void *userdata, int result)
{
    if(!result){
        /* Subscribe to broker information topics on successful connect. */
        mosquitto_subscribe(mosq, NULL, mqtt_sub_topic, 1);//订阅主题
    }else{
        fprintf(stderr, "Connect failed\n");
        printf( "MQTT Connect failed\n");
    }
}

void my_subscribe_callback(struct mosquitto *mosq, void *userdata, int mid, int qos_count, const int *granted_qos)
{
    int i;
    printf("Subscribed (mid: %d): %d", mid, granted_qos[0]);
    for(i=1; i<qos_count; i++){
        printf(", %d", granted_qos[i]);
    }
    printf("\n");
}

void my_log_callback(struct mosquitto *mosq, void *userdata, int level, const char *str)
{
    /* Pring all log messages regardless of level. */
    printf("%s\n", str);
}

int demo_subscribe()
{
    //libmosquitto 库初始化
    mosquitto_lib_init();
    //创建mosquitto客户端
    mosq = mosquitto_new(mqtt_client_id,session,NULL);//client id
    if(!mosq){
        printf("create client failed..\n");
        mosquitto_lib_cleanup();
        return 1;
    }
    //设置回调函数，需要时可使用
    //mosquitto_log_callback_set(mosq, my_log_callback);
    mosquitto_connect_callback_set(mosq, my_connect_callback);
    mosquitto_message_callback_set(mosq, my_message_callback);
    //mosquitto_subscribe_callback_set(mosq, my_subscribe_callback);
    //客户端连接服务器
    while(1)
    {
        if(mosquitto_connect(mosq, mqtt_server, PORT, KEEP_ALIVE)){
            fprintf(stderr, "Unable to connect.\n");
            sleep(3);
            fprintf(stderr, "Reconfirm to connect.\n");
        }
        else
        {
        	break;
        }
    }
    //循环处理网络消息
    mosquitto_loop_forever(mosq, -1, 1);
//    mosquitto_loop_start(mosq);//开线程持续运行

    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();

    return 0;
}

/*
 * 发布功能
 * */

int demo_publisher()
{
    char buff[MSG_MAX_SIZE];
    struct mosquitto *mosq = NULL;
    //libmosquitto 库初始化
    mosquitto_lib_init();
    //创建mosquitto客户端
    mosq = mosquitto_new(NULL,session,NULL);
    if(!mosq){
        printf("create client failed..\n");
        mosquitto_lib_cleanup();
        return 1;
    }
    //连接服务器
    if(mosquitto_connect(mosq, mqtt_server, PORT, KEEP_ALIVE)){
        fprintf(stderr, "Unable to connect.\n");
        return 1;
    }
    //开启一个线程，在线程里不停的调用 mosquitto_loop() 来处理网络信息
    int loop = mosquitto_loop_start(mosq);
    if(loop != MOSQ_ERR_SUCCESS)
    {
        printf("mosquitto loop error\n");
        return 1;
    }
    int xx = 0;
    //while(fgets(buff, MSG_MAX_SIZE, stdin) != NULL)
    while(1)
    {
    	xx++;
		/*发布消息*/
		mosquitto_publish(mosq,NULL,"RCV",strlen(buff)+1,buff,0,0);
		if(xx > 1000)
		{
			xx = 0;
			memset(buff,0,sizeof(buff));
		}
		sprintf(buff , "send : %d",xx);
		sleep(3);
    }
    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();
    return 0;
}

static void *mqtt_reconnect_poll_loop(void* ptmr)
{
	signal(SIGKILL,mqtt_sig_hander);
	mosquitto_username_pw_set(mosq, mqtt_user, mqtt_key);
    while(1)
    {
    	mosquitto_disconnect(mosq);
    	sleep(1);
        if(mosquitto_connect(mosq, mqtt_server, PORT, KEEP_ALIVE)){
            fprintf(stderr, "Checkout faild .Unable to connect.\n");
            sleep(3);
            fprintf(stderr, "Reconfirm to connect.\n");
        }
        else
        {
        	printf("MQTT is Connected !\n");
        	return NULL;
        }
    }
}

/*切换MQtt服务器IP*/
int switch_mqtt_server(void)
{
	if(mqtt_reconnect_poll.p_task != 0)
	{
		call_c_timer_task(&main_timer_task ,&mqtt_reconnect_poll ,false);
		call_c_timer_task(&main_timer_task ,&mqtt_reconnect_poll ,true);
	}
	else
	{
		call_c_timer_task(&main_timer_task ,&mqtt_reconnect_poll ,true);
	}
	return 0;
}

static void call_c_timer_task(c_timer_task_opt *timer ,c_timer_manager_t *task ,bool on_off)
{
	if(on_off)
	{
		timer->add_timer_task(*task);
		timer->set_timer_onff(*task,on_off);
	}
	else
	{
		printf("删除前PID：%ld\n",task->pid);
		timer->remove_timer(*task);
	}
}
#ifdef __cplusplus //使用ｃ编译
}
#endif
