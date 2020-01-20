/*
 * timer_manager.cpp
 *
 *  Created on: Dec 21, 2019
 *      Author: aron566
 */

#include "timer_manager.h"
#include "UI_timer_opt.h"
#include <unistd.h>
#include <iostream>
#include <pthread.h>
#include <signal.h>
#include "run_log.h"
//#include <algorithm>
//定时任务初始化
c_timer_manager_t c_timer_manager[] =
{
	{
		.interval_seconds = 100,
		.timer_on_off = false,
		.run_state = false,
		.p_task = start_company_logo_flash,//建立logo显示任务
		.pid = 0,
		.offset = 0,
	},
	{
		.interval_seconds = 500,
		.timer_on_off = false,
		.run_state = false,
		.p_task = user_cursor,//建立光标显示任务
		.pid = 0,
		.offset = 1
	},
	{
		.interval_seconds = 500,
		.timer_on_off = false,
		.run_state = false,
		.p_task = key_cursor,//建立光标显示任务
		.pid = 0,
		.offset = 1
	},
	{
		.interval_seconds = 1000,
		.timer_on_off = false,
		.run_state = false,
		.p_task = display_time,//建立首页时间显示任务
		.pid = 0,
		.offset = 1
	},
	{
		.interval_seconds = 0,
		.timer_on_off = NULL,
		.run_state = NULL,
		.p_task = NULL,
		.pid = 0,
		.offset = 1
	}
};

//线程中实例化，循环执行判断
void *misc_run_loop(void *data)
{
	static unsigned int systic_ms = 0;
	int err = 0;
	for(;;)
	{
		for(int i = 0; c_timer_manager[i].p_task != NULL;i++)
		{
			if((c_timer_manager[i].timer_on_off == true) && c_timer_manager[i].run_state == false)
			{
				err = pthread_create(&c_timer_manager[i].pid, NULL, c_timer_manager[i].p_task, NULL);
				if(err)
				{
					std::cout << "***线程创建错误！***\n";
					continue;
				}
				else
				{
					c_timer_manager[i].run_state = true;
					std::cout << "***线程创建成功！***\n";
				}
			}
			if((c_timer_manager[i].timer_on_off == false) && c_timer_manager[i].run_state == true)
			{
				//pthread_kill(c_timer_manager[i].pid, SIGKILL);
				std::cout << "***线程killed！***\n";
				pthread_cancel(c_timer_manager[i].pid);
				c_timer_manager[i].run_state = false;
			}
		}
		systic_ms++;
		//std::cout << "***线程状态检查中！***\n";
		sleep(1);
	}
	return 0;
}


/*开启或关闭任务*/
void c_timer_task_opt::set_timer_onff(c_timer_manager_t &timer_stuct,bool timer_on_off,unsigned int index)
{
	c_timer_manager_dynamic.at(index).timer_on_off = timer_on_off;
	print_check_info(index);
	if(timer_on_off)
	{
		timer_stuct.pid = realy_timer_start(true,index);
	}
	else
	{
		timer_stuct.pid = realy_timer_start(false,index);
	}
}
/*设置任务运行间隔*/
void c_timer_task_opt::set_timer_interval(unsigned int interval_seconds,unsigned int index)
{
	c_timer_manager_dynamic.at(index).interval_seconds = interval_seconds;
}
/*设置运行任务*/
void c_timer_task_opt::set_timer_pfun(pfunc p_task,unsigned int index)
{
	c_timer_manager_dynamic.at(index).p_task = p_task;
}
/*设置属性-针对光标*/
void c_timer_task_opt::set_offset(unsigned short offset,unsigned int index)
{
	c_timer_manager_dynamic.at(index).offset = offset;
}
void c_timer_task_opt::add_timer_task(c_timer_manager_t &timer_stuct)
{
	std::cout << "===================================添加前向量数目：";
	std::cout << c_timer_manager_dynamic.size();
	std::cout << "\n===================================延时时间:";
	std::cout << c_timer_manager_dynamic.at(0).interval_seconds;
	std::cout << "\n";
	c_timer_manager_dynamic.insert(c_timer_manager_dynamic.begin(),timer_stuct);
	std::cout << "===================================添加后向量数目：";
	std::cout << c_timer_manager_dynamic.size();
	std::cout << "\n";
}
/*获取任务线程id
 *
 * 输入函数指针
 *
 * 返回如果容器中多个重复函数 则返回0不确定 ，否则返回真PID
 * */
pthread_t c_timer_task_opt::get_timer_task_pthread_id(pfunc pf)
{
	unsigned int i = 0;
	pthread_t id = 0 ;
	int times = 0;
	for(; i < c_timer_manager_dynamic.size(); i++)
	{
		if(c_timer_manager_dynamic[i].p_task == pf)
		{
			id = c_timer_manager_dynamic[i].pid;
			times++;
		}
	}
	return (times == 1)?id:0;
}
/*任务所在索引 -不严谨 队列中禁止相同函数*/
unsigned int c_timer_task_opt::get_timer_task_arry_index(pthread_t pid)
{
	unsigned int index = 0;
	for(; index < c_timer_manager_dynamic.size(); index++)
	{
		if(c_timer_manager_dynamic[index].pid == pid)
		{
			break;
		}
	}
	return index;
}

/*获得运行状态*/
void c_timer_task_opt::get_timer_run_state(bool &run_state)
{
	run_state = property_info.run_state;
}
void c_timer_task_opt::print_check_info(unsigned int index)
{
	if(c_timer_manager_dynamic.at(index).p_task == NULL)
	{
		std::cout << "the pthread pointer is unset！\n";
		c_timer_manager_dynamic.at(index).timer_on_off = false;
	}
}
/*启动或关闭真正任务*/
pthread_t c_timer_task_opt::realy_timer_start(bool timer_on_off,unsigned int index)
{
	int err = 0;
	if(timer_on_off)
	{
		if((c_timer_manager_dynamic.at(index).timer_on_off == true) && c_timer_manager_dynamic.at(index).run_state == false)
		{
			err = pthread_create(&c_timer_manager_dynamic[index].pid, NULL, c_timer_manager_dynamic.at(index).p_task, c_timer_manager_dynamic.at(index).data);
			if(err)
			{
				std::cout << "pthread create failed ！\n";
				return 0;
			}
			else
			{
				//处于分离状态--线程终止时底层资源立即被回收
				pthread_detach(c_timer_manager_dynamic[index].pid);
				c_timer_manager_dynamic.at(index).run_state = true;
				return c_timer_manager_dynamic[index].pid;
			}
		}
	}
	else if(timer_on_off == false)
	{
		if((c_timer_manager_dynamic.at(index).timer_on_off == false) && c_timer_manager_dynamic.at(index).run_state == true)
		{
			//pthread_kill(property_info.pid, SIGKILL);
			std::cout << "***线程pid " << c_timer_manager_dynamic.at(index).pid << " killed！***\n";
			pthread_cancel(c_timer_manager_dynamic.at(index).pid);
			c_timer_manager_dynamic.at(index).run_state = false;
			return 0;
		}
		return 0;
	}
	return 0;
}
//删除定时器
void c_timer_task_opt::remove_timer(c_timer_manager_t &timer_stuct)
{
	it = c_timer_manager_dynamic.begin();
	int index = 0;
	if(timer_stuct.pid == 0)
	{
		std::cout << "The pthread is no exist\n";
		return;
	}
	for(;it != c_timer_manager_dynamic.end();)
	{
		if(it->pid == timer_stuct.pid){
			//删除指定元素，返回指向删除元素的下一个元素的位置的迭代器
			set_timer_onff(timer_stuct,false ,index);
			std::cout << "The pthread is Off！\n";
			it = c_timer_manager_dynamic.erase(it);
			std::cout << "The pthread is Erase！\n";
			return;
		}
		else
		{
			//迭代器指向下一个元素位置
			++it;
			++index;
		}
	  }
	std::cout << "can't remove the pthread is not exist\n";
//	c_timer_manager_dynamic.erase(remove(c_timer_manager_dynamic.begin(), c_timer_manager_dynamic.end(), pf), c_timer_manager_dynamic.end());//删除vector中所有值为pf的元素
}
