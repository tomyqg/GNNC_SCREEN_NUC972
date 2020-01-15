
#include "timer_manager.h"//定时器管理
#ifdef __cplusplus //使用ｃ编译
extern "C" {
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h> // using the termios.h library
#include <signal.h>
#include <sys/types.h>
#include <semaphore.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <errno.h>
#include <pthread.h>
#include <sys/epoll.h>
#include "serial_opt.h"
#include "modbus_opt.h"
#include "run_log.h"
#include "parameter_setting.h"//参数设置区


static void usart1_init(void);//串口初始化
static void usart2_init(void);
static void usart3_init(void);
static void usart4_init(void);
static void usart5_init(void);
static void usart6_init(void);
static void usart7_init(void);
static void usart8_init(void);
static void usart9_init(void);
static void usart10_init(void);
/*初始化uart运行模式*/
static void uart_running_mode_init(void);
/*本机运行在主动查询模式--独立工作*/
static void *uart_send_data_loop(void* ptmr);
static void call_c_timer_task_opt_func(c_timer_task_opt *timer ,c_timer_manager_t *task ,bool on_off);
//int file_id_1 = -1;//串口ttyS6
//int file_id_2 = -1;//串口ttyS7


int file_id[UART_NUM_MAX+1] = {0};


c_timer_manager_t modbus_read_data_poll =
{
	.interval_seconds = 1,
	.timer_on_off = true,
	.run_state = false,
	.p_task = uart_send_data_loop,//建立任务
	.pid = 0,
	.offset = 0,
	.data = NULL
};

c_timer_manager_t modbus_default_read_data_1_poll =
{
	.interval_seconds = 1,
	.timer_on_off = true,
	.run_state = false,
	.p_task = modbus_master_decode_start,//建立任务
	.pid = 0,
	.offset = 0,
	.data = NULL
};

c_timer_manager_t modbus_default_read_data_2_poll =
{
	.interval_seconds = 1,
	.timer_on_off = true,
	.run_state = false,
	.p_task = modbus_master_decode_start,//建立任务
	.pid = 0,
	.offset = 0,
	.data = NULL
};
circular_buffer *cb_create(char *path,unsigned long order)
{
	int fd = 0, status = 0;
	void *address = NULL;
//	char path[] = "/dev/shm/gnnc_cache_XXXXXX";
	circular_buffer *cb = (circular_buffer *)malloc(sizeof(circular_buffer));//分配一个新的内存空间。cb_struct_pointer
	if (NULL == cb)
	{
		return NULL;
	}
//	order = (order <= 11 ? 11 : order);
	order = (order <= 11 ? order : 11);
	cb->count = 1UL << order;//左移order位,1UL表示无符号长整形1 左移11次 2KB
	cb->read_offset = 0;
	cb->write_offset = 0;
	cb->fd = -1;
	cb->ptr = mmap(NULL, cb->count+(cb->count>>1), PROT_READ | PROT_WRITE, MAP_ANONYMOUS |MAP_SHARED, -1, 0);
	if (MAP_FAILED == cb->ptr)
	{
		printf("map failed \n");
		abort();
	}
	fd = mkstemp((char *)path);
	if (0 > fd)
	{
		printf("mkstemp failed \n");
		abort();
	}
	/*
	status = unlink(path);
	if (0 != status)
	{
		printf("unlink failed \n");
		abort();
	}*/
	status = ftruncate(fd, cb->count);//设置大小
	if (0 != status)
	{
		printf("ftruncate failed \n");
		abort();
	}
	address = mmap(cb->ptr , cb->count, PROT_READ | PROT_WRITE, MAP_FIXED | MAP_SHARED, fd, 0);
	if (address != cb->ptr)
	{
		printf("map1 failed \n");
		printf("errno = %d \n" , errno);
		abort();
	}
//	address = mmap(cb->ptr + cb->count, cb->count>>2, PROT_READ | PROT_WRITE, MAP_FIXED | MAP_SHARED, fd, 0);
//	if (address != cb->ptr + cb->count)
//	{
//		printf("map2 failed \n");
//		printf("errno = %d \n" , errno);
//		abort();
//	}
	//初始化环形队列
	CQ_init(&cb->seq_manager ,(uint8_t *)cb->ptr, (uint16_t)cb->count);
	status = close(fd);
	if (0 != status)
	{
		printf("close failed \n");
		abort();
	}

	return cb;
}
uint32_t cb_bytes_can_read(circular_buffer *cb)
{
#ifdef USE_OLD_CQ
	return cb->write_offset - cb->read_offset;
#else
	return CQ_getLength(&cb->seq_manager);
#endif
}

void cb_read_offset_inc(circular_buffer *cb, unsigned long  cnt)
{
#ifdef USE_OLD_CQ
	cb->read_offset += cnt ;//读取统计地址+1
//	if (cb->read_offset > cb->count)//大于则说明读到顶
//	{
//		cb->read_offset -= cb->count;
//		cb->write_offset -= cb->count;
//	}
#else
	uint8_t temp_buff[USART_RX_ONE_TIME];
	CQ_getData(&cb->seq_manager, (uint8_t *)temp_buff, cnt);//使用get偏移
#endif
}

void cbClear(circular_buffer *cb)
{
#ifdef USE_OLD_CQ
	cb->read_offset = 0;
	cb->write_offset = 0;
#else
	CQ_emptyData(&cb->seq_manager);
#endif
}
/*
*环形缓冲区读走数据：
*CircularQueue作为环形冲区的记录器，是个结构体
*targetBuf 为临时数据处理处
*len 为本次数据读取长度
*使用写入长度-读取的长度 == 剩余可读  ，要读   取小值
*/
uint32_t CQ_get_buff_Data(circular_buffer *cb, uint8_t *targetBuf, uint32_t len)
{
    uint32_t size = 0;

    /*此次读取的实际大小，取 缓存事件数据大小 和 目标读取数量  两个值小的那个*/
    len = GET_MIN(len, cb->write_offset - cb->read_offset);// 假设总大小10  写入了5 - 已读4  == 1 未读   要读5个   返回1
    /*原理雷同存入*/
    size = GET_MIN(len, cb->count - (cb->read_offset & (cb->count - 1)));//10 - 0 > 1 返回1
    memcpy(targetBuf, (uint8_t *)cb->ptr + (cb->read_offset & (cb->count - 1)), size);
    memcpy(targetBuf + size, (uint8_t *)cb->ptr, len - size);// 存储区偏移0个字节
    /*利用无符号数据的溢出特性*/
//    cb->read_offset += len;//取出数据加 len 记录

    return len;
}
/* 串口初始化函数 */
int usart_init(void)
{
	usart1_init();
	usart2_init();
	usart3_init();
	usart4_init();
	usart5_init();
	usart6_init();
	usart7_init();
	usart8_init();
	usart9_init();
	usart10_init();
	uart_running_mode_init();
	return 0;
}
static void usart1_init(void)
{
	if ((file_id[1] = open("/dev/ttyS6", O_RDWR |  O_NOCTTY |  O_NDELAY))<0)//打开串口1
	{
		perror("USART: Failed to open the ttyS6 port! \n");//该函数将错误消息打印到流stderr错误流
		return ;
	}
	struct termios options; // the termios structure is vital//串口配置结构体
	tcgetattr(file_id[1], &options); // sets the parameters associated with file 将第一个的状态输入第二个参数

	//Set To Raw Mode
	options.c_lflag  &= ~(ICANON | ECHO | ECHOE | ISIG);  /*Input*///输入模式标志
	options.c_oflag  &= ~OPOST;   /*Output*///输出模式标志
	//控制模式标志
	// 9600 baud, 8-bit, enable receiver, no modem control lines 9600波特，8位，使能接收器
	options.c_cflag = B9600 | CS8 | CREAD | CLOCAL;
	//本地模式标志
	options.c_iflag = IGNPAR | ICRNL; // ignore partity errors, CR -> newline

	tcflush(file_id[1], TCIFLUSH); // discard file information not transmitted刷清输入、输出队列
	tcsetattr(file_id[1], TCSANOW, &options); // changes occur immmediately立即执行而不等待数据发送或者接受完成。
	printf("UART:S6--FD:%d\n",file_id[1]);
}
static void usart2_init(void)
{
	if ((file_id[2] = open("/dev/ttyS7", O_RDWR |  O_NOCTTY |  O_NDELAY))<0)//打开串口2
	{
		perror("USART: Failed to open the ttyS7 port! \n");
		return ;
	}
	struct termios options;
	tcgetattr(file_id[2], &options);
	options.c_lflag  &= ~(ICANON | ECHO | ECHOE | ISIG);
	options.c_oflag  &= ~OPOST;
	options.c_cflag = B9600 | CS8 | CREAD | CLOCAL;
	options.c_iflag = IGNPAR | ICRNL;
	tcflush(file_id[2], TCIFLUSH);
	tcsetattr(file_id[2], TCSANOW, &options);
	printf("UART:S7--FD:%d\n",file_id[2]);
}

static void usart3_init(void)
{
	if ((file_id[3] = open("/dev/ttyS1", O_RDWR |  O_NOCTTY |  O_NDELAY))<0)//打开串口3
	{
		perror("USART: Failed to open the ttyS1 port! \n");
		return ;
	}
	struct termios options;
	tcgetattr(file_id[3], &options);
	options.c_lflag  &= ~(ICANON | ECHO | ECHOE | ISIG);
	options.c_oflag  &= ~OPOST;
	options.c_cflag = B9600 | CS8 | CREAD | CLOCAL;
	options.c_iflag = IGNPAR | ICRNL;
	tcflush(file_id[3], TCIFLUSH);
	tcsetattr(file_id[3], TCSANOW, &options);
	printf("UART:S1--FD:%d\n",file_id[3]);
}
static void usart4_init(void)
{
	if ((file_id[4] = open("/dev/ttyS4", O_RDWR |  O_NOCTTY |  O_NDELAY))<0)//打开串口4
	{
		perror("USART: Failed to open the ttyS4 port! \n");
		return ;
	}
	struct termios options;
	tcgetattr(file_id[4], &options);
	options.c_lflag  &= ~(ICANON | ECHO | ECHOE | ISIG);
	options.c_oflag  &= ~OPOST;
	options.c_cflag = B9600 | CS8 | CREAD | CLOCAL;
	options.c_iflag = IGNPAR | ICRNL;
	tcflush(file_id[4], TCIFLUSH);
	tcsetattr(file_id[4], TCSANOW, &options);
	printf("UART:S4--FD:%d\n",file_id[4]);
}
static void usart5_init(void)
{
//	if ((file_id[5] = open("/dev/ttyS0", O_RDWR |  O_NOCTTY |  O_NDELAY))<0)//打开串口5
//	{
//		perror("USART: Failed to open the ttyS4 port! \n");
//		return ;
//	}
//	struct termios options;
//	tcgetattr(file_id[5], &options);
//	options.c_lflag  &= ~(ICANON | ECHO | ECHOE | ISIG);
//	options.c_oflag  &= ~OPOST;
//	options.c_cflag = B9600 | CS8 | CREAD | CLOCAL;
//	options.c_iflag = IGNPAR | ICRNL;
//	tcflush(file_id[5], TCIFLUSH);
//	tcsetattr(file_id[5], TCSANOW, &options);
}
static void usart6_init(void)
{

}
static void usart7_init(void)
{

}
static void usart8_init(void)
{

}
static void usart9_init(void)
{

}
static void usart10_init(void)
{

}
void usart_cleanup(void)
{
	close(file_id[1]);//关闭串口1号串口
	close(file_id[2]);
	close(file_id[3]);
	close(file_id[4]);
	close(file_id[5]);
	close(file_id[6]);
	close(file_id[7]);
	close(file_id[8]);
	close(file_id[9]);
	close(file_id[10]);
}


static void *uart_send_data_loop(void* ptmr)
{
	uint8_t CRC_value_L ,CRC_value_H;
	uint16_t crc_ret = 0;
	uint8_t send_data_buff[8] = {0xFF ,0x03};
	uint8_t time_out = 0;
	uint8_t fd_count = 1;
	while(1){

	//循环轮寻从站寄存器
	for(int index = 0;access_reg_arr[index].access_num != 0;index++)
	{
		printf("============当前读取寄存器：%d --数量：%d============\n",access_reg_arr[index].access_reg_addr,access_reg_arr[index].access_num);
		send_data_buff[2] = (access_reg_arr[index].access_reg_addr>>8)&0xFF;//起始地址寄存器高字节
		send_data_buff[3] =	(access_reg_arr[index].access_reg_addr&0xFF);//寄存器低字节
		send_data_buff[4] =	(access_reg_arr[index].access_num>>8)&0xFF;//读取数量高字节
		send_data_buff[5] =	(access_reg_arr[index].access_num&0xFF);
		crc_ret = CRC_Return(send_data_buff ,6);
		send_data_buff[6] =	CRC_value_L =(uint8_t)(crc_ret &0x00FF);//有无符号重要！
		send_data_buff[7] =	CRC_value_H = (uint8_t)((crc_ret>>8)&0x00FF);
		//对每个串口轮寻此寄存器
		fd_count = 1;
		cbClear(polling_msg[fd_count].cb);
		for(;fd_count < UART_NUM_MAX+1;)
		{
			if(file_id[fd_count] != 0 && file_id[fd_count] != -1)
			{
				printf("当前发送：");
				debug_print((uint8_t*)send_data_buff,8);
				//发送本次需要轮寻的寄存器
				usart_tx(file_id[fd_count] ,(uint8_t *)send_data_buff ,8);
				//封装本次轮寻状态
				polling_msg[fd_count].read_count = ((access_reg_arr[index].access_num*2)+5);
				polling_msg[fd_count].read_channel = fd_count;
				polling_msg[fd_count].read_reg = access_reg_arr[index].access_reg_addr;

				printf("当前需读取到%d字节，通道：%d，reg：%d\n",polling_msg[fd_count].read_count,polling_msg[fd_count].read_channel,polling_msg[fd_count].read_reg);
				printf("->->->->发送一包至通道%d 挂起发送,等接收完成.......！\n",fd_count);
				//解析
				int8_t ret = modbus_master_decode_mannul(polling_msg[fd_count].cb);
				switch(ret)
				{
					case 0:
						fd_count++;
						break;
					case -1:
						if(time_out < 1)
						{
							time_out++;
							printf("重试！\n");
						}
						else
						{
							printf("跳过\n");
							fd_count++;
							time_out = 0;
						}
						break;
					default:
						fd_count++;
						printf("???！\n");
						break;
				}
				cbClear(polling_msg[fd_count].cb);
			}
			else
			{
				fd_count++;
			}
		}
	}
	sleep(1);
	printf("is alived !\n");
  }
  printf("跳出！\n");
  return NULL;
}
static void call_c_timer_task_opt_func(c_timer_task_opt *timer ,c_timer_manager_t *task ,bool on_off)
{
	if(on_off)
	{
		timer->add_timer_task(*task);
		printf("参数地址：%p\n",(circular_buffer*)task->data);
		timer->set_timer_onff(*task,on_off);
	}
	else
	{
		printf("删除前PID：%ld\n",task->pid);
		timer->remove_timer(*task);
	}
}
/*初始化uart运行模式*/
static void uart_running_mode_init(void)
{
	printf("初始化串口模式：%d\n",uart_mode);
	switch_running_mode(uart_mode);
}
uint8_t switch_running_mode(uint8_t mode)
{
//	ptr_uart_task_config = new c_timer_task_opt;
	if(mode == REC_NOT_THROUGH_MODE) //运行在独立模式 --开启任务
	{
		printf("独立模式运行中,轮寻从机数据. . .\n");
		call_c_timer_task_opt_func(&main_timer_task ,&modbus_default_read_data_1_poll ,false);
		call_c_timer_task_opt_func(&main_timer_task ,&modbus_default_read_data_2_poll ,false);

		call_c_timer_task_opt_func(&main_timer_task ,&modbus_read_data_poll ,true);
	}
	else if(mode == REC_THROUGH_MODE) //运行在透传模式 --关闭任务
	{
		printf("透传模式运行中,等待主机数据. . .\n");
		//关闭独立工作
		call_c_timer_task_opt_func(&main_timer_task ,&modbus_read_data_poll ,false);
		//启动端口1
		modbus_default_read_data_1_poll.data = polling_msg[1].cb;
		call_c_timer_task_opt_func(&main_timer_task ,&modbus_default_read_data_1_poll ,true);
		//启动端口2
		modbus_default_read_data_2_poll.data = polling_msg[2].cb;
		call_c_timer_task_opt_func(&main_timer_task ,&modbus_default_read_data_2_poll ,true);
	}
	return 0;
}
/* 串口发送 */
int usart_tx(int fd,unsigned char* msg , int len)
{
	int count = write(fd, msg, len) ;
	if (count < 0)//返回写入的值＜0则错误
	{
		perror("Failed to write to the output.\n");//显示错误信息，到错误流
		return  -1;
	}
	printf("FD:%d->Send! Byte:%d\n",fd,count);
	return count ;//正确，则返回写入的字节数
}

/* 串口接收 */
int usart_rx(int fd,circular_buffer *cb, int len)
{
#ifdef USE_OLD_CQ
    uint32_t size = 0;
    uint32_t read_len = len;
    /*此次存入的实际大小，取 剩余空间 和 目标存入数量  两个值小的那个*/
    read_len = GET_MIN(read_len, cb->count - cb->write_offset + cb->read_offset);

    /*&(size-1)代替取模运算，同上原理，得到此次存入队列入口到末尾的大小*/
    size = GET_MIN(read_len, cb->count - (cb->write_offset & (cb->count - 1)));

    //第一次存在到buff尾部
	int count = read(fd,(uint8_t*)cb->ptr + (cb->write_offset & (cb->count - 1)),size);//初次buffer存储地址为cache的分配的起始地址,ptr为void *型需要转换真实类型进行加减

	if (count < 0)
	{
		perror("Failed to read from the input.\n");
		return -1;
	}
	//第二次从开头存
	int count_2 = read(fd,cb->ptr,read_len - size);//写入的数据必须尽快读出，否则后面的数据将无法继续写入
	if (count_2 >= 0)
	{
		count += count_2;
	}
	if(cb->fd == -1)
	{
		cb->fd = fd;
	}
	printf("FD:%d->write_offset:%u,read_offset:%u\n",fd,cb->write_offset,cb->read_offset);
	cb->write_offset += count;//写入的数据统计=读取的数据长度+初始分配地址    是个全局变量 假设发送0-5数据地址=则长度为6个字节 6作为下一次写入地址起始位OK

	return count ;
#else
	uint8_t temp_buff[USART_RX_ONE_TIME];
	int count = read(fd,temp_buff,len);//初次buffer存储地址为cache的分配的起始地址,ptr为void *型需要转换真实类型进行加减
	DQ_putData(&cb->seq_manager, (uint8_t *)temp_buff, count);//加入数据至缓冲区
	if (count < 0)
	{
		perror("Failed to read from the input.\n");
		return -1;
	}
	if(cb->fd == -1)
	{
		cb->fd = fd;
	}
	printf("FD:%d->write_offset:%d,read_offset:%ld\n",fd,cb->seq_manager.entrance,cb->seq_manager.exit);
	return count ;
#endif
}

/* 数据丢弃 */
int usart_discard(circular_buffer *cb)
{
	cbClear(cb);
	return 0;
}

/* SIGKILL信号处理 */
void usart_sig_hander(int signo)
{
    printf("Usart thread was killed\n");
	usart_cleanup();
	pthread_exit(0);
}

/*  串口1接收的LOOP */
void* usart1_rx_start(void* data)
{
	circular_buffer *cb = (circular_buffer *)(data);
	signal(SIGKILL,usart_sig_hander);
	int  fs_sel;
	fd_set fs_read;
	FD_ZERO(&fs_read);//清除一个文件描述符集
	FD_SET(file_id[1],&fs_read);//将一个文件描述符加入文件描述符集中
	while(1)
	{
//		FD_ZERO(&fs_read);//清除一个文件描述符集
//		FD_SET(file_id[1],&fs_read);//将一个文件描述符加入文件描述符集中
		fs_sel = select(file_id[1]+1,&fs_read,NULL,NULL,NULL);//返回准备好的文件描述符的数目　０超时　－１出错
		if(fs_sel)
		{
			if(cb->write_offset > 1024 && cb->read_offset < 512)//
			{
				printf("Buffer will overflow , discard!\n");//数据溢出
				usart_discard(cb);
				continue;//丢弃一次记录后，退出本次if，若达到条件再次进入
			}
			printf("串口1接收中．．．\n");
			usart_rx(file_id[1],cb,USART_RX_ONE_TIME);//128字节
		}
		else
		{
			printf("Usart receive err .\n");
			break;
		}
	}
	return NULL;
}
/*  串口2接收的LOOP */
void* usart2_rx_start(void* data)
{
	circular_buffer *cb = (circular_buffer *)(data);
	signal(SIGKILL,usart_sig_hander);
	int  fs_sel;
	fd_set fs_read;
	FD_ZERO(&fs_read);//清除一个文件描述符集
	FD_SET(file_id[2],&fs_read);//将一个文件描述符加入文件描述符集中
	while(1)
	{
//		FD_ZERO(&fs_read);//清除一个文件描述符集
//		FD_SET(file_id[2],&fs_read);//将一个文件描述符加入文件描述符集中
		fs_sel = select(file_id[2]+1,&fs_read,NULL,NULL,NULL);//返回准备好的文件描述符的数目　０超时　－１出错
		if(fs_sel)
		{
			if(cb->write_offset > 1024 && cb->read_offset < 512)//写入大于3K并且,读取小于2K则属于溢出
			{
				printf("Buffer will overflow , discard!\n");//数据溢出
				usart_discard(cb);
				continue;//丢弃一次记录后，退出本次if，若达到条件再次进入
			}
			printf("串口２接收中．．．\n");
			usart_rx(file_id[2],cb,USART_RX_ONE_TIME);//128字节
		}
		else
		{
			printf("Usart receive err .\n");
			break;
		}
	}
	return NULL;
}

/*  串口3接收的LOOP */
void* usart3_rx_start(void* data)
{
//	circular_buffer *cb = (circular_buffer *)(data);
//	signal(SIGKILL,usart_sig_hander);
//	int  fs_sel;
//	fd_set fs_read;
//	FD_ZERO(&fs_read);//清除一个文件描述符集
//	FD_SET(file_id_3,&fs_read);//将一个文件描述符加入文件描述符集中
//	while(1)
//	{
//		fs_sel = select(file_id_3+1,&fs_read,NULL,NULL,NULL);//返回准备好的文件描述符的数目　０超时　－１出错
//		if(fs_sel)
//		{
//			if(cb->write_offset > 512 && cb->read_offset < 512)//写入大于3K并且,读取小于2K则属于溢出
//			{
//				printf("Buffer will overflow , discard!\n");//数据溢出
//				usart_discard(cb);
//				continue;//丢弃一次记录后，退出本次if，若达到条件再次进入
//			}
//			printf("串口3接收中．．．\n");
//			usart_rx(file_id_3,cb,USART_RX_ONE_TIME);//256字节
//		}
//		else
//		{
//			printf("Usart receive err .\n");
//			break;
//		}
//	}
	return NULL;
}


/*  串口4接收的LOOP */
void* usart4_rx_start(void* data)
{
//	circular_buffer *cb = (circular_buffer *)(data);
//	signal(SIGKILL,usart_sig_hander);
//	int  fs_sel;
//	fd_set fs_read;
//	FD_ZERO(&fs_read);//清除一个文件描述符集
//	FD_SET(file_id_4,&fs_read);//将一个文件描述符加入文件描述符集中
//	while(1)
//	{
//		fs_sel = select(file_id_4+1,&fs_read,NULL,NULL,NULL);//返回准备好的文件描述符的数目　０超时　－１出错
//		if(fs_sel)
//		{
//			if(cb->write_offset > 512 && cb->read_offset < 512)//写入大于3K并且,读取小于2K则属于溢出
//			{
//				printf("Buffer will overflow , discard!\n");//数据溢出
//				usart_discard(cb);
//				continue;//丢弃一次记录后，退出本次if，若达到条件再次进入
//			}
//			usart_rx(file_id_4,cb,USART_RX_ONE_TIME);//256字节
//		}
//		else
//		{
//			printf("Usart receive err .\n");
//			break;
//		}
//	}
	return NULL;
}

/*  串口5接收的LOOP */
void* usart5_rx_start(void* data)
{
//	circular_buffer *cb = (circular_buffer *)(data);
//	signal(SIGKILL,usart_sig_hander);
//	int  fs_sel;
//	fd_set fs_read;
//	FD_ZERO(&fs_read);//清除一个文件描述符集
//	FD_SET(file_id_5,&fs_read);//将一个文件描述符加入文件描述符集中
//	while(1)
//	{
//		fs_sel = select(file_id_5+1,&fs_read,NULL,NULL,NULL);//返回准备好的文件描述符的数目　０超时　－１出错
//		if(fs_sel)
//		{
//			if(cb->write_offset > 1024 && cb->read_offset < 512)//写入大于3K并且,读取小于2K则属于溢出
//			{
//				printf("Buffer will overflow , discard!\n");//数据溢出
//				usart_discard(cb);
//				continue;//丢弃一次记录后，退出本次if，若达到条件再次进入
//			}
//			usart_rx(file_id_5,cb,USART_RX_ONE_TIME);//256字节
//		}
//		else
//		{
//			printf("Usart receive err .\n");
//			break;
//		}
//	}
	return NULL;
}

/*  串口6接收的LOOP */
void* usart6_rx_start(void* data)
{
//	circular_buffer *cb = (circular_buffer *)(data);
//	signal(SIGKILL,usart_sig_hander);
//	int  fs_sel;
//	fd_set fs_read;
//	FD_ZERO(&fs_read);//清除一个文件描述符集
//	FD_SET(file_id_6,&fs_read);//将一个文件描述符加入文件描述符集中
//	while(1)
//	{
//		fs_sel = select(file_id_6+1,&fs_read,NULL,NULL,NULL);//返回准备好的文件描述符的数目　０超时　－１出错
//		if(fs_sel)
//		{
//			if(cb->write_offset > 1024 && cb->read_offset < 512)//写入大于3K并且,读取小于2K则属于溢出
//			{
//				printf("Buffer will overflow , discard!\n");//数据溢出
//				usart_discard(cb);
//				continue;//丢弃一次记录后，退出本次if，若达到条件再次进入
//			}
//			usart_rx(file_id_6,cb,USART_RX_ONE_TIME);//256字节
//		}
//		else
//		{
//			printf("Usart receive err .\n");
//			break;
//		}
//	}
	return NULL;
}

/*  串口7接收的LOOP */
void* usart7_rx_start(void* data)
{
//	circular_buffer *cb = (circular_buffer *)(data);
//	signal(SIGKILL,usart_sig_hander);
//	int  fs_sel;
//	fd_set fs_read;
//	FD_ZERO(&fs_read);//清除一个文件描述符集
//	FD_SET(file_id_7,&fs_read);//将一个文件描述符加入文件描述符集中
//	while(1)
//	{
//		fs_sel = select(file_id_7+1,&fs_read,NULL,NULL,NULL);//返回准备好的文件描述符的数目　０超时　－１出错
//		if(fs_sel)
//		{
//			if(cb->write_offset > 512 && cb->read_offset < 512)//写入大于3K并且,读取小于2K则属于溢出
//			{
//				printf("Buffer will overflow , discard!\n");//数据溢出
//				usart_discard(cb);
//				continue;//丢弃一次记录后，退出本次if，若达到条件再次进入
//			}
//			usart_rx(file_id_7,cb,USART_RX_ONE_TIME);//256字节
//		}
//		else
//		{
//			printf("Usart receive err .\n");
//			break;
//		}
//	}
	return NULL;
}

/*  串口8接收的LOOP */
void* usart8_rx_start(void* data)
{
//	circular_buffer *cb = (circular_buffer *)(data);
//	signal(SIGKILL,usart_sig_hander);
//	int  fs_sel;
//	fd_set fs_read;
//	FD_ZERO(&fs_read);//清除一个文件描述符集
//	FD_SET(file_id_8,&fs_read);//将一个文件描述符加入文件描述符集中
//	while(1)
//	{
//		fs_sel = select(file_id_8+1,&fs_read,NULL,NULL,NULL);//返回准备好的文件描述符的数目　０超时　－１出错
//		if(fs_sel)
//		{
//			if(cb->write_offset > 512 && cb->read_offset < 512)//写入大于3K并且,读取小于2K则属于溢出
//			{
//				printf("Buffer will overflow , discard!\n");//数据溢出
//				usart_discard(cb);
//				continue;//丢弃一次记录后，退出本次if，若达到条件再次进入
//			}
//			usart_rx(file_id_8,cb,USART_RX_ONE_TIME);//256字节
//		}
//		else
//		{
//			printf("Usart receive err .\n");
//			break;
//		}
//	}
	return NULL;
}
/*  串口9接收的LOOP */
void* usart9_rx_start(void* data)
{
//	circular_buffer *cb = (circular_buffer *)(data);
//	signal(SIGKILL,usart_sig_hander);
//	int  fs_sel;
//	fd_set fs_read;
//	FD_ZERO(&fs_read);//清除一个文件描述符集
//	FD_SET(file_id_9,&fs_read);//将一个文件描述符加入文件描述符集中
//	while(1)
//	{
//		fs_sel = select(file_id_9+1,&fs_read,NULL,NULL,NULL);//返回准备好的文件描述符的数目　０超时　－１出错
//		if(fs_sel)
//		{
//			if(cb->write_offset > 512 && cb->read_offset < 512)//写入大于3K并且,读取小于2K则属于溢出
//			{
//				printf("Buffer will overflow , discard!\n");//数据溢出
//				usart_discard(cb);
//				continue;//丢弃一次记录后，退出本次if，若达到条件再次进入
//			}
//			usart_rx(file_id_9,cb,USART_RX_ONE_TIME);//256字节
//		}
//		else
//		{
//			printf("Usart receive err .\n");
//			break;
//		}
//	}
	return NULL;
}

/*  串口10接收的LOOP */
void* usart10_rx_start(void* data)
{
//	circular_buffer *cb = (circular_buffer *)(data);
//	signal(SIGKILL,usart_sig_hander);
//	int  fs_sel;
//	fd_set fs_read;
//	FD_ZERO(&fs_read);//清除一个文件描述符集
//	FD_SET(file_id_10,&fs_read);//将一个文件描述符加入文件描述符集中
//	while(1)
//	{
//		fs_sel = select(file_id_10+1,&fs_read,NULL,NULL,NULL);//返回准备好的文件描述符的数目　０超时　－１出错
//		if(fs_sel)
//		{
//			if(cb->write_offset > 512 && cb->read_offset < 512)//写入大于3K并且,读取小于2K则属于溢出
//			{
//				printf("Buffer will overflow , discard!\n");//数据溢出
//				usart_discard(cb);
//				continue;//丢弃一次记录后，退出本次if，若达到条件再次进入
//			}
//			usart_rx(file_id_10,cb,USART_RX_ONE_TIME);//256字节
//		}
//		else
//		{
//			printf("Usart receive err .\n");
//			break;
//		}
//	}
	return NULL;
}

#ifdef __cplusplus
}
#endif
