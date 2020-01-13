/*
 * timer_manager.h
 *
 *  Created on: Dec 21, 2019
 *      Author: aron566
 */

#ifndef INCLUDE_TIMER_MANAGER_H_
#define INCLUDE_TIMER_MANAGER_H_

#include <pthread.h>
#include <vector>
#include <iostream>
typedef void *(*pfunc)(void* ptmr);

void *misc_run_loop(void *data);
/*
 * 设置运行间隔，索引，执行的任务
 * */
typedef struct
{
	unsigned int interval_seconds;
	bool timer_on_off;
	bool run_state;
	pfunc p_task;
	pthread_t pid;
	unsigned short offset;
	void *data;
}c_timer_manager_t;

extern c_timer_manager_t c_timer_manager[];

//自成一体：类方式管理

class c_timer_task_opt
{
public:
	c_timer_task_opt()
	{
		property_info.interval_seconds = 10;
		property_info.timer_on_off = false;
		property_info.run_state = false;
		property_info.p_task = NULL;//建立任务
		property_info.pid = 0;
		property_info.offset = 0;
		property_info.data = NULL;
		c_timer_manager_dynamic.insert(c_timer_manager_dynamic.begin(),property_info);
		std::cout << "\n **The Timer init OK!**\n";
	}
	c_timer_manager_t property_info;
	//std::vector<c_timer_manager_t> c_timer_manager_dynamic(c_timer_manager,c_timer_manager+4);//建立管理向量
	std::vector<c_timer_manager_t> c_timer_manager_dynamic;
	std::vector<c_timer_manager_t>::iterator it;
	void print_check_info(unsigned int index = 0);
	void set_timer_onff(c_timer_manager_t &timer_stuct,bool timer_on_off = false,unsigned int index = 0);
	void set_timer_interval(unsigned int interval_seconds = 500,unsigned int index = 0);
	void set_timer_pfun(pfunc p_task = NULL,unsigned int index = 0);
	void set_offset(unsigned short offset = 1,unsigned int index = 0);
	void add_timer_task(c_timer_manager_t &timer_stuct);
	pthread_t get_timer_task_pthread_id(pfunc pf);
	unsigned int get_timer_task_arry_index(pthread_t pid);
	void get_timer_run_state(bool &run_state);
	void remove_timer(c_timer_manager_t &timer_stuct);
private:
	pthread_t realy_timer_start(bool timer_on_off = false,unsigned int index = 0);
};
extern c_timer_task_opt main_timer_task;

#endif /* INCLUDE_TIMER_MANAGER_H_ */
