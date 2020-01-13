/*
 * tcp_client.h
 *
 *  Created on: Dec 26, 2019
 *      Author: aron566
 */

#ifndef INCLUDE_TCP_CLIENT_H_
#define INCLUDE_TCP_CLIENT_H_
#ifdef __cplusplus //使用ｃ编译
extern "C" {
#endif
#include "serial_opt.h"

void* tcp_clent_start(void* data);
int tcp_client_tx(int fd,unsigned char* msg , int len);
int socket_force_reconnect(void);
extern int sockfd;

#ifdef __cplusplus //使用ｃ编译
}
#endif
#endif /* INCLUDE_TCP_CLIENT_H_ */
