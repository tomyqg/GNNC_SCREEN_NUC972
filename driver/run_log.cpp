/*
 * run_log.cpp
 *
 *  Created on: Jan 3, 2020
 *      Author: aron566
 */
#ifdef __cplusplus //使用ｃ编译
extern "C" {
#endif
#include <sys/time.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <linux/unistd.h>     /* 包含调用 _syscallX 宏等相关信息*/
#include <linux/kernel.h>     /* 包含sysinfo结构体信息*/
#include <sys/sysinfo.h>
#include <linux/sysinfo.h>
#include <fcntl.h>
#include "run_log.h"

/*日志文件输出处理*/
char GNNC_TEMP_log[2048] = {0};
char GNNC_LOG_File_TIME[40] = {0};
int GNNC_Origin_Day = -1;
char GNNC_buf_date[20] = {0};
time_t GNNC_cur_time;
static int GNNC_log_id = -1;
char *GNNC_get_timestamp(char *buf, int len, time_t cur_time)
{
    struct tm tm_time;
    localtime_r(&cur_time, &tm_time);

    snprintf(buf, len, "%d-%u-%d-%d:%d:%d",
             1900 + tm_time.tm_year, 1 + tm_time.tm_mon,
             tm_time.tm_mday, tm_time.tm_hour,
             tm_time.tm_min, tm_time.tm_sec);
    if(GNNC_Origin_Day != tm_time.tm_mday && tm_time.tm_hour == 0)
	{
		int status = unlink(GNNC_LOG_File_TIME);//清除前一天日志文件
		if (0 != status)
		{
			printf("unlink failed \n");
			//abort();
		}
		GNNC_log_init();//new day
	}
    return buf;
}
int GNNC_log_init(void)
{
	struct tm  *tp;
	time_t t = time(NULL);
	tp = localtime(&t);
	GNNC_Origin_Day = tp->tm_mday;//首次
    sprintf(GNNC_LOG_File_TIME,"/dev/shm/GNNC_LOG-%d-%u-%d-%d-%d.txt",tp->tm_year+1900,tp->tm_mon+1,tp->tm_mday
   			,tp->tm_hour,tp->tm_min);
	if((GNNC_log_id = open(GNNC_LOG_File_TIME,O_CREAT | O_RDWR | O_APPEND | O_NONBLOCK,S_IWGRP | S_IROTH ))<0)//创建日志文件
	{
		GNNC_DEBUG_INFO("log_file open failed");
		perror("LOG: Failed to open the SNAI_log! \n");
		printf("log_file open failed \n");
		return -1;
	}
	return 1;
}
void GNNC_log_ww(char *str)
{
	if(GNNC_log_id != -1)
	{
		write(GNNC_log_id,str,strlen(str));
	}
	else
	{
		GNNC_log_init();//重启
	}
}

int cat_mmp(void)
{
	struct sysinfo s_info;
	int error;

	error = sysinfo(&s_info);
	GNNC_DEBUG_INFO("<<<Uptime = %lds>>>" "RAM: |total %lu|used = %lu|free = %lu",
			s_info.uptime,
			s_info.totalram/1024,(s_info.totalram/1024-s_info.freeram/1024),s_info.freeram/1024);
	return 0;
}
//struct sysinfo {
//long uptime;
///* 启动到现在经过的时间 */
//unsigned long loads[3];
///* 1, 5, and 15 minute load averages */
//unsigned long totalram;  /* 总的可用的内存大小 */
//unsigned long freeram;   /* 还未被使用的内存大小 */
//unsigned long sharedram; /* 共享的存储器的大小 */
//unsigned long bufferram; /* 缓冲区大小 */
//unsigned long totalswap; /* 交换区大小 */
//unsigned long freeswap;  /* 还可用的交换区大小 */
//unsigned short procs;    /* 当前进程数目 */
//char _f[22];         /* 64字节的补丁结构 */

#ifdef __cplusplus //使用ｃ编译
}
#endif
