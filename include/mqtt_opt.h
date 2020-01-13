/*
 * mqtt_opt.h
 *
 *  Created on: Jan 4, 2020
 *      Author: aron566
 */

#ifndef INCLUDE_MQTT_OPT_H_
#define INCLUDE_MQTT_OPT_H_

#ifdef __cplusplus //使用ｃ编译
extern "C" {
#endif

void *start_mqtt(void *data);
/*切换mqtt服务器 -- reconnect*/
int switch_mqtt_server(void);
/*终结线程*/
void mqtt_sig_hander(int signo);
extern struct mosquitto *mosq;

#ifdef __cplusplus //使用ｃ编译
}
#endif


#endif /* INCLUDE_MQTT_OPT_H_ */
