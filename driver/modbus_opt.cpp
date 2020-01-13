/*
 * modbus_opt.c
 *
 *  Created on: Dec 17, 2019
 *      Author: aron566
 */
//#include "timer_manager.h"//定时器管理
#ifdef __cplusplus //使用ｃ编译
extern "C" {
#endif
#include <stdint.h>
#include <semaphore.h>
#include <sys/time.h>
#include <errno.h>
#include "modbus_opt.h"
#include "parameter_setting.h"
#include "tcp_client.h"
#include "db_opt.h"
#include "run_log.h"

static unsigned short Crc_Cal(unsigned short Data, unsigned short GenPoly, unsigned short CrcData);
static void decode_sig_hander(int signo);
static void broker_send_message_to_slave(int msg_device_addr,uint8_t* msg,int len);
static void return_rw_message(int fd,uint8_t *ptr,uint16_t msglen);
static void broker_rw_message(int channel,uint8_t *ptr,uint16_t msglen);
static uint8_t get_end_point(int fd);


#define MAX_REG_COUNT 					13U//定义最大寄存器处理数量

pthread_mutex_t GNNC_Decode_mutex_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t GNNC_Decode_cond = PTHREAD_COND_INITIALIZER;

pthread_mutex_t GNNC_REC_Data_mutex_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t GNNC_REC_cond = PTHREAD_COND_INITIALIZER;

modbus_master_rec_t self_analysis_temp;//数据解析打包发送至处理函数


/*索引*/
static int Check_Modbus_Addr(uint16_t Reg_start_addr);
static void rec_slave_data(modbus_master_rec_t *rec_struct ,uint8_t len);
static void rec_master_data(modbus_master_rec_t *rec_struct ,uint16_t reg_begin_addr ,uint16_t len);
static void rec_master_data_ack(uint8_t *msg ,uint16_t len);
/*处理函数*/
static uint16_t Real_Time_Consistence_Int_Get(void *data);
static uint16_t Gas_Ad_Value_Get(void *data);
static uint16_t Cpu_Id_1_Get(void *data);
static uint16_t Cpu_Id_2_Get(void *data);
static uint16_t Cpu_Id_3_Get(void *data);
static uint16_t Cpu_Id_4_Get(void *data);
static uint16_t Cpu_Id_5_Get(void *data);
static uint16_t Cpu_Id_6_Get(void *data);
static uint16_t Running_Sec_H_Get(void *data);
static uint16_t Running_Sec_L_Get(void *data);
static uint16_t Read_Data_From_Db_Get(void *data);
static uint16_t Set_Uart_Mode(void *data);
static uint16_t Unused_Ack(void *data);

static int Ret_master_Read_Ack(void);

/*轮寻信号 标志*/
polling_msg_t polling_msg[UART_NUM_MAX+1];
/*独立运行模式下 对从站轮寻寄存器地址信息 配置*/
modbus_polling_slave_t access_reg_arr[] =
{
	{
		.access_reg_addr = GAS_AD_VALUE,//AD
		.access_num = 1
	},
	{
		.access_reg_addr = CPU_ID_1,//CPU ID
		.access_num = 6
	},
	{
		.access_reg_addr = RUNNING_SECONDS_H,//RUNNING TIME
		.access_num = 2
	},
	{
		.access_reg_addr = 0,//END
		.access_num = 0
	}
};


/*寄存器对应处理映射 --每增加索引列表 需更改 MAX_REG_COUNT 值 */
modbus_process_t reg_process_map[MAX_REG_COUNT] =
{
	{REAL_TIME_CONSISTENCE_INT_H,Real_Time_Consistence_Int_Get},
	{GAS_AD_VALUE,Gas_Ad_Value_Get},
	{CPU_ID_1 ,Cpu_Id_1_Get},
	{CPU_ID_2 ,Cpu_Id_2_Get},
	{CPU_ID_3 ,Cpu_Id_3_Get},
	{CPU_ID_4 ,Cpu_Id_4_Get},
	{CPU_ID_5 ,Cpu_Id_5_Get},
	{CPU_ID_6 ,Cpu_Id_6_Get},
	{RUNNING_SECONDS_H ,Running_Sec_H_Get},
	{RUNNING_SECONDS_L ,Running_Sec_L_Get},
	{READ_DATA_FROM_DB ,Read_Data_From_Db_Get},
	{SET_UART_MODE ,Set_Uart_Mode},
	{0xFFFF,Unused_Ack}
};

/* 未能找到对应寄存器处理方法 */
static uint16_t Unused_Ack(void *data)
{
  return 0;
}

/* 查找对应寄存器处理索引 */
static int Check_Modbus_Addr(uint16_t Reg_start_addr)
{
	int ret = 0;
	uint8_t index = 0;
	for(;index < MAX_REG_COUNT;index++)
	{
		if(reg_process_map[index].register_num == Reg_start_addr)
		{
			ret = index;
			return ret;
		}
	}
	return MAX_REG_COUNT-1;/*未找到,使用默认处理*/
}
/*处理获得的实时浓度 - - 整形*/
static uint16_t Real_Time_Consistence_Int_Get(void *data)
{
	modbus_master_rec_t *temp_data = (modbus_master_rec_t *)data;

	return 0;
}
/*处理获得的气体AD值 - -
 *
 * 存入数据库
 * */
static uint16_t Gas_Ad_Value_Get(void *data)
{
	modbus_master_rec_t *temp_data = (modbus_master_rec_t *)data;
	static uint16_t device_record_count[UART_NUM_MAX+1] = {0};
	struct tm *p_tm;
	time_t timestamp;
	timestamp = time((time_t *) NULL);
	p_tm = localtime(&timestamp);
	printf( "AD值记录时间:%02d-%02d-%02d-%02d-%02d\n", p_tm->tm_year + 1900,
			p_tm->tm_mon + 1, p_tm->tm_mday,p_tm->tm_hour,p_tm->tm_min);
	//录入状态
	gnnc_device_v_info[temp_data->channnel].device_state = 1;
	//录入当前值
	gnnc_device_v_info[temp_data->channnel].current_value_H = 0;
	gnnc_device_v_info[temp_data->channnel].current_value_L = temp_data->rec_data;
	printf("录入数据:%d--%d\n",gnnc_device_v_info[temp_data->channnel].current_value_H,gnnc_device_v_info[temp_data->channnel].current_value_L);
	if(temp_data->cmd == 0x03)
	{
		//记录数据
		if(device_record_count[temp_data->channnel] > MAX_RECORD_MESSAGE_NUM)
		{
			//更新记录--更新数据
			gnnc_device_v_info[temp_data->channnel].device_count_num = (gnnc_device_v_info[temp_data->channnel].device_count_num >= MAX_RECORD_MESSAGE_NUM)?0:gnnc_device_v_info[temp_data->channnel].device_count_num++;
		}
		else
		{
			//增加记录--加入数据
			record_insert(device_tab_name_list[temp_data->channnel],temp_data->channnel ,timestamp ,&gnnc_device_v_info[temp_data->channnel]);
			gnnc_device_v_info[temp_data->channnel].device_count_num++;
			device_record_count[temp_data->channnel]++;
		}
		return (temp_data->rec_data&0xFFFF);
	}
	return 0;
}

static uint16_t Cpu_Id_1_Get(void *data)
{
	modbus_master_rec_t *temp_data = (modbus_master_rec_t *)data;
	if(temp_data->cmd == 0x03)//来自从站
	{
		gnnc_device_v_info[temp_data->channnel].device_id_H = (temp_data->rec_data&0xFFFF);
		return (temp_data->rec_data&0xFFFF);
	}
	return 0;
}

static uint16_t Cpu_Id_2_Get(void *data)
{
	modbus_master_rec_t *temp_data = (modbus_master_rec_t *)data;
	if(temp_data->cmd == 0x03)//来自从站
	{
		gnnc_device_v_info[temp_data->channnel].device_id_H <<= 16;
		gnnc_device_v_info[temp_data->channnel].device_id_H |= (temp_data->rec_data&0xFFFF);
		printf("获取通道：%d CPUID1-2：%0*lX\n",temp_data->channnel ,16,gnnc_device_v_info[temp_data->channnel].device_id_H);
		return (temp_data->rec_data&0xFFFF);
	}
	return 0;
}

static uint16_t Cpu_Id_3_Get(void *data)
{
	modbus_master_rec_t *temp_data = (modbus_master_rec_t *)data;
	if(temp_data->cmd == 0x03)//来自从站
	{
		gnnc_device_v_info[temp_data->channnel].device_id_L |= (temp_data->rec_data&0xFFFF);
		gnnc_device_v_info[temp_data->channnel].device_id_L <<= 16;
		return (temp_data->rec_data&0xFFFF);
	}
	return 0;
}

static uint16_t Cpu_Id_4_Get(void *data)
{
	modbus_master_rec_t *temp_data = (modbus_master_rec_t *)data;
	if(temp_data->cmd == 0x03)//来自从站
	{
		gnnc_device_v_info[temp_data->channnel].device_id_L |= (temp_data->rec_data&0xFFFF);
		gnnc_device_v_info[temp_data->channnel].device_id_L <<= 16;
		return (temp_data->rec_data&0xFFFF);
	}
	return 0;
}

static uint16_t Cpu_Id_5_Get(void *data)
{
	modbus_master_rec_t *temp_data = (modbus_master_rec_t *)data;
	if(temp_data->cmd == 0x03)//来自从站
	{
		gnnc_device_v_info[temp_data->channnel].device_id_L |= (temp_data->rec_data&0xFFFF);
		gnnc_device_v_info[temp_data->channnel].device_id_L <<= 16;
		return (temp_data->rec_data&0xFFFF);
	}
	return 0;

}

static uint16_t Cpu_Id_6_Get(void *data)
{
	modbus_master_rec_t *temp_data = (modbus_master_rec_t *)data;
	if(temp_data->cmd == 0x03)//来自从站
	{
		gnnc_device_v_info[temp_data->channnel].device_id_L |= (temp_data->rec_data&0xFFFF);
		printf("获取通道：%d CPUID3-6：%lX\n",temp_data->channnel ,gnnc_device_v_info[temp_data->channnel].device_id_L);
		return (temp_data->rec_data&0xFFFF);
	}
	return 0;
}

static uint16_t Running_Sec_H_Get(void *data)
{
	modbus_master_rec_t *temp_data = (modbus_master_rec_t *)data;
	if(temp_data->cmd == 0x03)//来自从站
	{
		gnnc_device_v_info[temp_data->channnel].running_time = (temp_data->rec_data&0xFFFF);
		return (temp_data->rec_data&0xFFFF);
	}
	return 0;
}

static uint16_t Running_Sec_L_Get(void *data)
{
	modbus_master_rec_t *temp_data = (modbus_master_rec_t *)data;
	if(temp_data->cmd == 0x03)//来自从站
	{
		gnnc_device_v_info[temp_data->channnel].running_time <<= 16;
		gnnc_device_v_info[temp_data->channnel].running_time |= (temp_data->rec_data&0xFFFF);
		printf("获取通道：%d 运行时间：%ld\n",temp_data->channnel ,gnnc_device_v_info[temp_data->channnel].running_time);
		return (temp_data->rec_data&0xFFFF);
	}
	return 0;
}

/*响应上位机查询需求*/
static uint16_t Read_Data_From_Db_Get(void *data)
{
	modbus_master_rec_t *temp_data = (modbus_master_rec_t *)data;
	if(temp_data->cmd == 0x03)
	{
		Ret_master_Read_Ack();
	}
	//上位机读取起始时间设置
	if(temp_data->cmd == 0x10)
	{
//		temp_data->rec_data;
	}
	return 0;
}

static uint16_t Set_Uart_Mode(void *data)
{
	modbus_master_rec_t *temp_data = (modbus_master_rec_t *)data;
	//返回串口模式数据
	if(temp_data->cmd == 0x03)
	{
		return uart_mode;
	}
	//上位机设置串口模式
	if(temp_data->cmd == 0x10)
	{
		printf("接收通道:%d 控制UART模式数据:%d\n",temp_data->channnel,temp_data->rec_data);
		par_set_uart_mode(temp_data->rec_data&0x01);
	}
	return 0;
}
/* 返回数据库数据给上位机 */
static int Ret_master_Read_Ack(void)
{
	for(int i = 0; i < DEVICE_TAB_COUNT_NUM; i++)
	{
		//查询符合时间范围内的数据
		record_query_up_time_region(device_tab_name_list[i],time((time_t *)NULL ),i ,process_query_send);
	}

	return 0;
}
/* SIG_KILL 处理函数 */
static void decode_sig_hander(int signo)
{
	pthread_exit(0);
}
/* modbus解码的LOOP
 *
 * 接收slave包并解析【本机作为master】
 *
 * uart_mode:０正常解析模式,解析slave数据包存至数据库对应通道表，１透传模式，转发slave包至上级master
 *
 * 缓冲区Uart_Send_master_BUFF
 * */
void* modbus_master_decode_start(void* data) {
	circular_buffer *cb = (circular_buffer *) (data);
	uint8_t msg_device_addr,msg_cmd;
	uint8_t CRC_value_L,CRC_value_H,CRC_value_L_temp,CRC_value_H_temp;
	uint16_t crc_ret = 0,msg_len = 0;
	uint8_t error_flag = 0;
	uint32_t read_offset = 0;
	uint8_t tmp_buff[128];
	//主动解析
	signal(SIGKILL, decode_sig_hander);
	while (1) {
#ifdef USE_OLD_CQ
		if (cb_bytes_can_read(cb) >= 7) //比较当前可读数据长度=当前写入长度-已读长度）大于8即可进入,符合modbus协议最小长度
		{
			msg_device_addr = *((uint8_t*) ((uint8_t*)cb->ptr + (cb->read_offset%cb->count))); //第一个字节为设备地址
			read_offset = ((cb->read_offset + 1)%cb->count);
			msg_cmd = *(((uint8_t*)cb->ptr + read_offset)); //第二个字节为功能码

			if(msg_cmd == 0x03)//slave从机返回寄存器值
			{
				read_offset = ((cb->read_offset + 2)%cb->count);
				msg_len = (*((uint8_t*)cb->ptr + read_offset))+3;//计算字节：addr+cmd+len
				while((msg_len+2) > cb_bytes_can_read(cb))
				{
					usleep(1000);
				}
				printf("需要长度：%d\n",msg_len+2);
				CQ_get_buff_Data(cb, (uint8_t *)tmp_buff, msg_len+2);
				crc_ret = CRC_Return(tmp_buff,msg_len);
				CRC_value_L_temp = tmp_buff[msg_len];
				CRC_value_H_temp = tmp_buff[msg_len+1];
				CRC_value_L =(uint8_t)(crc_ret &0x00FF);//有无符号重要！
				CRC_value_H = (uint8_t)((crc_ret>>8)&0x00FF);
				if(CRC_value_L == CRC_value_L_temp && CRC_value_H == CRC_value_H_temp)
				{
					if(uart_mode == REC_THROUGH_MODE)//透传模式－－CRC计算修改ID后发送给master
					{
						return_rw_message(cb->fd,(uint8_t*)tmp_buff,msg_len);
					}
					if(uart_mode == REC_NOT_THROUGH_MODE)//本机解析模式
					{
						printf("进入解析存储!\n");
						read_offset = ((cb->read_offset + 3)%cb->count);
						uint8_t *reg_begin_data = ((uint8_t*)cb->ptr + read_offset);
//						rec_slave_data(cb->fd ,reg_begin_data ,*((uint8_t*)((uint8_t*)cb->ptr + cb->read_offset + 2)));
					}
					cb_read_offset_inc(cb, msg_len+2);//偏移一帧报文
				}
				else
				{
					if(error_flag > 3)
					{
						cb_read_offset_inc(cb, 1);//丢弃１字节
						printf("master : modbus_0x03　return　CRC error!\n");
					}
					else
					{
						printf("数据错误 %d\n",error_flag);
						debug_print((uint8_t*)cb->ptr + (cb->read_offset%cb->count),msg_len+2);
						debug_print(tmp_buff,msg_len+2);
						error_flag++;
					}
				}
			}
			else if(msg_cmd == 0x10)//slave从机返回控制寄存器地址及数量
			{
				msg_len = 6;//不含CRC字节数
				CQ_get_buff_Data(cb, (uint8_t *)tmp_buff, msg_len+2);
				crc_ret = CRC_Return(tmp_buff,msg_len);
				CRC_value_L =(uint8_t)(crc_ret &0x00FF);//有无符号重要！
				CRC_value_H = (uint8_t)((crc_ret>>8)&0x00FF);
				CRC_value_L_temp = tmp_buff[msg_len];
				CRC_value_H_temp = tmp_buff[msg_len+1];
				if(CRC_value_L == CRC_value_L_temp && CRC_value_H == CRC_value_H_temp)//校验正确
				{
					if(uart_mode == REC_THROUGH_MODE)//透传模式－－CRC计算修改ID后发送给master
					{
						return_rw_message(cb->fd,(uint8_t*)tmp_buff,msg_len);
					}
					if(uart_mode == REC_NOT_THROUGH_MODE)//本机解析模式
					{

					}
					cb_read_offset_inc(cb, msg_len+2);//偏移一帧报文
				}
				else
				{
					if(error_flag > 3)
					{
						cb_read_offset_inc(cb, 1);//丢弃１字节
						printf("master : modbus_0x10　return　CRC error!\n");
					}
					else
					{
						error_flag++;
					}
				}
			}
			else{
				cb_read_offset_inc(cb, 1);//什么都不是－>丢弃１字节
				printf("master : modbus_0x**　return　CRC error!\n");
			}
		}
#else

#endif
		printf("Wait ...\n");
		usleep(50*1000);
	}
	return NULL;
}

/* 透传 模式下
 * 给上位机返回读写数据 0x03 0x10 结果
 * */
static void return_rw_message(int fd,uint8_t *ptr,uint16_t msglen)
{
	uint8_t CRC_value_L,CRC_value_H;
	uint16_t crc_ret = 0;
	uint8_t Uart_Send_master_BUFF[UART_SEND_BUFF_MAX];

	//复制数据
	memcpy(Uart_Send_master_BUFF,(uint8_t*)ptr,msglen);
	//修改数据
	Uart_Send_master_BUFF[0] = get_end_point(fd);//更换固定头部设备ID
	crc_ret = CRC_Return(Uart_Send_master_BUFF,msglen);
	CRC_value_L =(uint8_t)(crc_ret &0x00FF);//有无符号重要！
	CRC_value_H = (uint8_t)((crc_ret>>8)&0x00FF);
	Uart_Send_master_BUFF[msglen] = CRC_value_L;
	Uart_Send_master_BUFF[msglen+1] = CRC_value_H;
	//打印数据
	debug_print((uint8_t*)ptr,msglen+2);
	printf("master : 转换!\n");
	debug_print((uint8_t*)Uart_Send_master_BUFF,msglen+2);
	//发送数据
	tcp_client_tx(sockfd, Uart_Send_master_BUFF, msglen+2);//发送至本机上级
}

/* modbus解码的LOOP
 *
 * 接收master包并解析【本机作为slave】
 *
 * uart_mode:０正常解析模式,解析master命令,配置运行模式,返回数据，１透传模式，转发master包至下级slave
 *
 * 缓冲区Uart【1】_Send_slave_BUFF
 * */
void* modbus_slave_decode_start(void* data) {
	circular_buffer *cb = (circular_buffer *) (data);
	uint8_t msg_device_addr ,msg_cmd ,data_len;
	uint8_t CRC_value_L,CRC_value_H,CRC_value_L_temp,CRC_value_H_temp;
	uint16_t crc_ret = 0,msg_len = 0;
	uint8_t error_flag = 0;
	uint32_t read_offset = 0;
	uint8_t tmp_buff[128];
	//独立运行模式
	uint16_t reg_start_addr = 0;
	uint16_t reg_len = 0;
	modbus_master_rec_t rec_data_temp;
	signal(SIGKILL, decode_sig_hander);
	while (1) {
		if (cb_bytes_can_read(cb) >= 7) //比较当前可读数据长度=当前写入长度-已读长度）大于7即可进入,符合modbus协议最小长度
		{
			msg_device_addr = *(((uint8_t*)cb->ptr + (cb->read_offset%cb->count))); //第一个字节为设备地址
			read_offset = ((cb->read_offset + 1)%cb->count);
			msg_cmd = *(((uint8_t*)cb->ptr + read_offset)); //第二个字节为功能码

			if(msg_cmd == 0x03)//master要读取寄存器值
			{
				msg_len = 6;//不含CRC字节数
				CQ_get_buff_Data(cb, tmp_buff, msg_len+2);
				crc_ret = CRC_Return(tmp_buff,msg_len);
				CRC_value_L_temp = tmp_buff[msg_len];
				CRC_value_H_temp = tmp_buff[msg_len+1];
				CRC_value_L =(uint8_t)(crc_ret &0x00FF);//有无符号重要！
				CRC_value_H = (uint8_t)((crc_ret>>8)&0x00FF);
				if(CRC_value_L == CRC_value_L_temp && CRC_value_H == CRC_value_H_temp)
				{
					if(uart_mode == REC_THROUGH_MODE)//透传模式－－CRC计算修改ID后发送给slave
					{
						broker_rw_message(msg_device_addr,tmp_buff,msg_len);
					}
					if(uart_mode == REC_NOT_THROUGH_MODE)//本机解析模式
					{
						rec_data_temp.channnel = msg_device_addr;
						rec_data_temp.cmd = msg_cmd;
					}
					cb_read_offset_inc(cb, msg_len+2);//偏移一帧报文
				}
				else
				{
					if(error_flag > 3)
					{
						cb_read_offset_inc(cb, 1);//丢弃１字节
						printf("slave : modbus_0x03　return　CRC error!\n");
					}
					else
					{
						error_flag++;
					}
				}
			}
			else if(msg_cmd == 0x10)//master要控制的寄存器地址及数量
			{
				read_offset = ((cb->read_offset + 6)%cb->count);
				data_len = (*((uint8_t*)cb->ptr + read_offset));
				msg_len = data_len+7;//计算字节：第七元素数据字节数＋前面７个字节数
				CQ_get_buff_Data(cb, (uint8_t *)tmp_buff, msg_len+2);
				crc_ret = CRC_Return(tmp_buff,msg_len);
				CRC_value_L_temp = tmp_buff[msg_len];
				CRC_value_H_temp = tmp_buff[msg_len+1];
				CRC_value_L =(uint8_t)(crc_ret &0x00FF);//有无符号重要！
				CRC_value_H = (uint8_t)((crc_ret>>8)&0x00FF);
				if(CRC_value_L == CRC_value_L_temp && CRC_value_H == CRC_value_H_temp)//校验正确
				{
					/*获取控制起始地址*/
					reg_start_addr = tmp_buff[2];
					reg_start_addr <<= 8;
					reg_start_addr |= tmp_buff[3];
					if(reg_start_addr == SET_UART_MODE)
					{
						goto _set_uart;
					}
					if(uart_mode == REC_THROUGH_MODE)//透传模式－－修改ID CRC计算后发送给master
					{
						broker_rw_message(msg_device_addr,tmp_buff,msg_len);
					}
					if(uart_mode == REC_NOT_THROUGH_MODE)//本机解析模式
					{
						_set_uart:
						reg_start_addr = tmp_buff[2];
						reg_start_addr <<= 8;
						reg_start_addr |= tmp_buff[3];
						reg_len = tmp_buff[4];
						reg_len <<= 8;
						reg_len |= tmp_buff[5];
						rec_data_temp.channnel = msg_device_addr;
						rec_data_temp.cmd = msg_cmd;
						rec_data_temp.rec_data = 0;
						rec_data_temp.data_addr = &tmp_buff[7];
						printf("控制命令:0x%02X-0x%02X\n",tmp_buff[7],tmp_buff[8]);
						printf("接收控制命令:");
						debug_print(tmp_buff,msg_len+2);
						rec_master_data(&rec_data_temp ,reg_start_addr ,reg_len);
					}
					cb_read_offset_inc(cb, msg_len+2);//偏移一帧报文
				}
				else
				{
					if(error_flag > 3)
					{
						cb_read_offset_inc(cb, 1);//丢弃１字节
						printf("slave : modbus_0x10　return　CRC error!\n");
					}
					else
					{
						error_flag++;
					}
					cb_read_offset_inc(cb, 1);//丢弃１字节
				}
			}
			else
			{
				debug_print((uint8_t*)((uint8_t*)cb->ptr + cb->read_offset),8);
				cb_read_offset_inc(cb, 1);//什么都不是－>丢弃１字节
				printf("slave : modbus_0x**　return　CRC error!\n");
			}

		}
		usleep(100*1000);
	}
	return NULL;
}

/* 透传 模式下
 * 转发读写数据 0x03 0x10
 * */
static void broker_rw_message(int channel,uint8_t *ptr,uint16_t msglen)
{
	uint8_t CRC_value_L,CRC_value_H;
	uint16_t crc_ret = 0;
	uint8_t Uart_Send_slave_BUFF[UART_SEND_BUFF_MAX];
	//复制数据
	memcpy(Uart_Send_slave_BUFF,(uint8_t*)ptr,msglen);
	//修改数据
	Uart_Send_slave_BUFF[0] = 0XFF;//更换固定头部设备ID
	crc_ret = CRC_Return(Uart_Send_slave_BUFF,msglen);
	CRC_value_L =(uint8_t)(crc_ret &0x00FF);//有无符号重要！
	CRC_value_H = (uint8_t)((crc_ret>>8)&0x00FF);
	Uart_Send_slave_BUFF[msglen] = CRC_value_L;
	Uart_Send_slave_BUFF[msglen+1] = CRC_value_H;
	//打印数据
	debug_print((uint8_t*)ptr,msglen+2);
	printf("slave : 转换!\n");
	debug_print((uint8_t*)Uart_Send_slave_BUFF,msglen+2);
	//发送数据
	broker_send_message_to_slave(channel,Uart_Send_slave_BUFF,msglen+2);
}

/*
 * 解析master数据转发至正确端口slave
 *
 * msg_device_addr数据头部
 *
 * msg消息体，len数据长度
 * */
static void broker_send_message_to_slave(int msg_device_addr,uint8_t* msg,int len)
{
	switch(msg_device_addr)
	{
		case 0x01:
		case 0x02:
		case 0x03:
		case 0x04:
		case 0x05:
		case 0x06:
		case 0x07:
		case 0x08:
		case 0x09:
		case 0x0A:
			usart_tx(file_id[msg_device_addr],msg , len);
			break;
		default:
			break;
	}
}

/*
 * 解析slave数据来自的端口　－－　添加至数据包头　－－　tcp转发至master
 *
 * fd所属端口
 *
 * 返回所属端口号
 * */
static uint8_t get_end_point(int fd)
{
	uint8_t uart_endpoint = 0;
	for(uint8_t i = 0;i < UART_NUM_MAX+1;i++)
	{
		if(file_id[i] == fd)
		{
			uart_endpoint = i;
			break;
		}
	}
	return uart_endpoint;
}

/*
 * 本机作为master独立解析从站数据 --存入数据库
 *
 * 参数:rec_struct 包含所需cmd datapointer channel_num
 * 参数:len cmd 0x03时字节长度  cmd 0x10时ack消息
 * */
static void rec_slave_data(modbus_master_rec_t *rec_struct ,uint8_t len)
{
	uint16_t data_temp = 0;
	//数据地址
	uint8_t *data_addr = rec_struct->data_addr;
	//获取来自通道信息
	uint8_t channel_num = rec_struct->channnel;
	//数据对应起始寄存器地址
	uint16_t reg_addr = polling_msg[channel_num].read_reg;
	//解析从站返回的读操作数据
	if(rec_struct->cmd == 0x03)
	{
		printf("接收通道：%u 数据长度:%d\n",channel_num,len);
		for(uint16_t read_offset = 0; read_offset < len;read_offset += 2)
		{
			//准备数据
			data_temp = (*(data_addr+read_offset))&0xFF;
			data_temp <<= 8;
			data_temp |= (*(data_addr+read_offset+1))&0xFF;
			rec_struct->rec_data = data_temp;
			//存入数据
			printf("接收数据:%04X---%d\n",data_temp,data_temp);
			//调用处理对应寄存器
			reg_process_map[Check_Modbus_Addr(reg_addr)].func(rec_struct);
			//下一个寄存器
			reg_addr++;
		}
	}
	//解析从站返回的写操作ACK
	if(rec_struct->cmd == 0x10)
	{

	}
}

/*
 * 本机作为slave独立解析主站数据包 --响应 控制和读取操作
 *
 * 参数:reg_begin_addr 起始寄存器地址
 * 参数:len 寄存器数量长度
 * */
static void rec_master_data(modbus_master_rec_t *rec_struct ,uint16_t reg_begin_addr ,uint16_t len)
{
	uint8_t ack_data_buff[128] = {0};
	uint16_t read_offset = 0;
	uint16_t reg_addr = reg_begin_addr;
	uint16_t reg_end = reg_addr+len;
	printf("通道:%02X-命令:%02X-数据:%02X-数据:%02X\n",rec_struct->channnel,rec_struct->cmd,*rec_struct->data_addr,*(rec_struct->data_addr+1));
	uint8_t *data_addr = rec_struct->data_addr;

	for(;reg_addr < reg_end ;reg_addr++)
	{
		rec_struct->rec_data = (*data_addr+read_offset)&0xFF;
		rec_struct->rec_data <<= 8;
		rec_struct->rec_data |= (*(data_addr+read_offset+1))&0xFF;
		reg_process_map[Check_Modbus_Addr(reg_addr)].func(rec_struct);
		printf("处理寄存器值:%d,长度:%d\n",reg_addr,len);
		read_offset += 2;
	}
}

/*
 * 本机作为slave接受主站控制，回复响应数据
 *
 * 参数 msg 回复消息地址
 *
 * len 消息数量
 * */
static void rec_master_data_ack(uint8_t *msg ,uint16_t len)
{


}

/*
 * modbus CRC校验
 * */
static unsigned short Crc_Cal(unsigned short Data, unsigned short GenPoly, unsigned short CrcData)
{
	unsigned short TmpI;
	Data *= 2;
	for (TmpI = 8; TmpI > 0; TmpI--) {
		Data = Data / 2;
		if ((Data ^ CrcData) & 1)
			CrcData = (CrcData / 2) ^ GenPoly;
		else
			CrcData /= 2;
	}
	return CrcData;
}

unsigned short CRC_Return(uint8_t *Crc_Buf, unsigned short Crc_Len)
{
        unsigned short temp;
        unsigned short CRC_R = 0xFFFF;
        for (temp = 0; temp < Crc_Len; temp++)
        {
                CRC_R = Crc_Cal(Crc_Buf[temp], 0xA001, CRC_R);
        }
        return CRC_R;
}


void debug_print(uint8_t *msg,uint8_t msg_len)
{
	for (int i = 0; i < msg_len; i++)
	printf("%02X ",
			msg[i]); //打印2位的16进制数，不足位0补齐。
	printf("\n");
}


/* modbus解码
 *
 * 接收slave包并解析【本机作为master】
 *
 * uart_mode:０正常解析模式,解析slave数据包存至数据库对应通道表，１透传模式，转发slave包至上级master
 *
 * 缓冲区Uart_Send_master_BUFF
 * */
int8_t modbus_master_decode_test(void* data) {
	circular_buffer *cb = (circular_buffer *) (data);
	uint8_t msg_device_addr,msg_cmd ,device_channel;
	uint8_t CRC_value_L,CRC_value_H,CRC_value_L_temp,CRC_value_H_temp;
	uint16_t crc_ret = 0,msg_len = 0;
	uint16_t read_offset = 0;
	uint8_t tmp_buff[128];
	//透传模式下：缓冲区数据发送至下级slave
	uint8_t Uart_Send_master_BUFF[UART_SEND_BUFF_MAX];
	//独立运行模式：主动解析
	modbus_master_rec_t rec_data_temp;
	uint16_t time_delay = 0;
	uint8_t error_times = 0;
	signal(SIGKILL, decode_sig_hander);
	while(1){
		if (cb_bytes_can_read(cb) >= 7){ //比较当前可读数据长度=当前写入长度-已读长度）大于8即可进入,符合modbus协议最小长度
		//检测包是否完整，不完整则等待
		if(polling_msg[get_end_point(cb->fd)].read_count <= cb_bytes_can_read(cb))
		{
			msg_device_addr = *((uint8_t*) ((uint8_t*)cb->ptr + (cb->read_offset%cb->count))); //第一个字节为设备地址
			read_offset = ((cb->read_offset + 1)%cb->count);
			msg_cmd = *(((uint8_t*)cb->ptr + read_offset)); //第二个字节为功能码

			if(msg_cmd == 0x03)//slave从机返回寄存器值
			{
				read_offset = ((cb->read_offset + 2)%cb->count);
				msg_len = (*((uint8_t*)cb->ptr + read_offset))+3;//计算字节：addr+cmd+len
				if((unsigned long)(msg_len+2) > cb_bytes_can_read(cb))
				{
					usleep(100*1000);
				}
				CQ_get_buff_Data(cb, (uint8_t *)tmp_buff, msg_len+2);
				crc_ret = CRC_Return(tmp_buff,msg_len);
				CRC_value_L_temp = tmp_buff[msg_len];
				CRC_value_H_temp = tmp_buff[msg_len+1];
				CRC_value_L =(uint8_t)(crc_ret &0x00FF);//有无符号重要！
				CRC_value_H = (uint8_t)((crc_ret>>8)&0x00FF);
				if(CRC_value_L == CRC_value_L_temp && CRC_value_H == CRC_value_H_temp)
				{
					device_channel = get_end_point(cb->fd);//更换固定头部设备ID
					if(uart_mode == REC_THROUGH_MODE)//透传模式－－CRC计算修改ID后发送给master
					{
						memcpy(Uart_Send_master_BUFF,(uint8_t*) ((uint8_t*)cb->ptr + cb->read_offset+1),msg_len);
						Uart_Send_master_BUFF[0] = device_channel;//添加通道号作为ID
						crc_ret = CRC_Return(Uart_Send_master_BUFF,msg_len);
						CRC_value_L =(uint8_t)(crc_ret &0x00FF);//有无符号重要！
						CRC_value_H = (uint8_t)((crc_ret>>8)&0x00FF);
						Uart_Send_master_BUFF[msg_len] = CRC_value_L;
						Uart_Send_master_BUFF[msg_len+1] = CRC_value_H;
						debug_print((uint8_t*)((uint8_t*)cb->ptr + cb->read_offset),msg_len+2);
						printf("master : 转换!\n");
						debug_print((uint8_t*)Uart_Send_master_BUFF,msg_len+2);
						tcp_client_tx(sockfd, Uart_Send_master_BUFF, msg_len+2);//发送至本机上级
					}
					if(uart_mode == REC_NOT_THROUGH_MODE)//本机解析模式
					{
						printf("进入!\n");
						read_offset = ((cb->read_offset + 2)%cb->count);
						//数据字节长度
						uint8_t data_len = *(((uint8_t*)cb->ptr + read_offset));
						read_offset = ((cb->read_offset + 3)%cb->count);
						//数据起始地址
						uint8_t *reg_begin_data_H = ((uint8_t*)cb->ptr + read_offset);
						rec_data_temp.channnel = device_channel;
						rec_data_temp.cmd = msg_cmd;
						rec_data_temp.data_addr = reg_begin_data_H;
						printf("进入解析寄存器：%d. . .\n",polling_msg[device_channel].read_reg);
						rec_slave_data(&rec_data_temp ,data_len);
					}
					cb_read_offset_inc(cb, msg_len+2);//偏移一帧报文
					printf("当前通道：%d ，可读：%u\n",device_channel,cb_bytes_can_read(cb));
					return 0;
				}
				else
				{
					cb_read_offset_inc(cb, 1);//丢弃１字节
					printf("master : modbus_0x03　return　CRC error!\n");

				}
			}
			else if(msg_cmd == 0x10)//slave从机返回控制寄存器地址及数量
			{
				msg_len = 6;//不含CRC字节数
				crc_ret = CRC_Return((uint8_t*) ((uint8_t*)cb->ptr + cb->read_offset),msg_len);
				CRC_value_L =(uint8_t)(crc_ret &0x00FF);//有无符号重要！
				CRC_value_H = (uint8_t)((crc_ret>>8)&0x00FF);
				CRC_value_L_temp = *((uint8_t*) ((uint8_t*)cb->ptr + cb->read_offset + msg_len));
				CRC_value_H_temp = *((uint8_t*) ((uint8_t*)cb->ptr + cb->read_offset + msg_len+1));
				if(CRC_value_L == CRC_value_L_temp && CRC_value_H == CRC_value_H_temp)//校验正确
				{
					device_channel = get_end_point(cb->fd);//更换固定头部设备ID
					if(uart_mode == REC_THROUGH_MODE)//透传模式－－CRC计算修改ID后发送给master
					{
						memcpy(Uart_Send_master_BUFF,(uint8_t*) ((uint8_t*)cb->ptr + cb->read_offset+1),msg_len);
						Uart_Send_master_BUFF[0] = device_channel;//添加通道号作为ID
						crc_ret = CRC_Return(Uart_Send_master_BUFF,msg_len);
						CRC_value_L =(uint8_t)(crc_ret &0x00FF);//有无符号重要！
						CRC_value_H = (uint8_t)((crc_ret>>8)&0x00FF);
						Uart_Send_master_BUFF[msg_len] = CRC_value_L;
						Uart_Send_master_BUFF[msg_len+1] = CRC_value_H;
						debug_print((uint8_t*)((uint8_t*)cb->ptr + cb->read_offset),msg_len+2);
						printf("master : 转换!\n");
						debug_print((uint8_t*)Uart_Send_master_BUFF,msg_len+2);
						tcp_client_tx(sockfd, Uart_Send_master_BUFF, msg_len+2);//发送至本机上级
					}
					if(uart_mode == REC_NOT_THROUGH_MODE)//本机解析模式
					{

					}
//					cb_read_offset_inc(cb, msg_len+2);//偏移一帧报文
					return 0;
				}
				else
				{
					//失败次数大于1 则丢弃
					if(error_times > 1)
					{
						cb_read_offset_inc(cb, 1);//丢弃１字节
						printf("master : modbus_0x10　return　CRC error!\n");
					}
					else
					{
						error_times++;
					}
				}
			}
			else
			{
				debug_print(((uint8_t*) ((uint8_t*)cb->ptr + cb->read_offset)),msg_len+2);
				cb_read_offset_inc(cb, 1);//什么都不是－>丢弃１字节
				printf("master : modbus_0x**　return　CRC error!\n");
			}
		}
	}
		usleep(50*1000);
		time_delay++;
		if(time_delay > 2)//200ms out
		{
			printf("直接退出！\n");
			return -1;
		}
	}
}
#ifdef __cplusplus //使用ｃ编译
}
#endif

