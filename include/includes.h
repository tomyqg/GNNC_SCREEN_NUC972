/*
 * includes.h
 *
 *  Created on: Mar 18, 2019
 *      Author: root
 */

#ifndef INCLUDES_H_
#define INCLUDES_H_
#ifdef __cplusplus //使用ｃ编译
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <stdarg.h>
#include <sys/time.h>
#include <pthread.h>
#include "mysql.h"
#include "sqlite3.h"


#define FALSE 1
#define TRUE 0
#ifdef __cplusplus //使用ｃ编译
}
#endif
#endif /* INCLUDES_H_ */
