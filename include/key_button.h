/*
 * key_button.h
 *
 *  Created on: Dec 17, 2019
 *      Author: aron566
 */

#ifndef INCLUDE_KEY_BUTTON_H_
#define INCLUDE_KEY_BUTTON_H_
#ifdef __cplusplus //使用ｃ编译
extern "C" {
#endif
#include <stdio.h>
//#include <pthread.h>
#include <linux/input.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

void* key_check_loop(void* param);
#ifdef __cplusplus //使用ｃ编译
}
#endif
#endif /* INCLUDE_KEY_BUTTON_H_ */

