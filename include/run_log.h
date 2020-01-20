/*
 * run_log.h
 *
 *  Created on: Jan 3, 2020
 *      Author: aron566
 */

#ifndef INCLUDE_RUN_LOG_H_
#define INCLUDE_RUN_LOG_H_
#ifdef __cplusplus //使用ｃ编译
extern "C" {
#endif

#define GNNC_DEBUG_INFO(fmt, args...) if(GNNC_TEMP_log != NULL)									\
									{															\
										GNNC_cur_time = time(NULL);								\
										GNNC_get_timestamp(GNNC_buf_date, 20, GNNC_cur_time);	\
										sprintf(GNNC_TEMP_log,									\
										"LOG_%s-FILE:%s-%s(%d)--INFOR:"#fmt"\n", 				\
										GNNC_buf_date,__FILE__,__FUNCTION__,__LINE__,##args);	\
										GNNC_log_ww(GNNC_TEMP_log);								\
									}
/*日志文件输出处理*/
int GNNC_log_init(void);
void GNNC_log_ww(char *str);
char *GNNC_get_timestamp(char *buf, int len, time_t cur_time);
int cat_mmp(void);
extern char GNNC_TEMP_log[];
extern char GNNC_LOG_File_TIME[];
extern int GNNC_Origin_Day;
extern char GNNC_buf_date[];
extern time_t GNNC_cur_time;

#ifdef __cplusplus //使用ｃ编译
}
#endif
#endif /* INCLUDE_RUN_LOG_H_ */
