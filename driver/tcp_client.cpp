#ifdef __cplusplus //使用ｃ编译
extern "C" {
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include "tcp_client.h"
#include <netinet/tcp.h>//keep_alive
#include "parameter_setting.h"//tcp_相关参数设定
#include "run_log.h"

#define MAXLINE 4096
#define TCP_RX_ONE_TIME 256

static int tcp_client_rx(int fd,circular_buffer *cb, int len);

static int tcp_discard(circular_buffer *cb);
static void tcp_client_sig_hander(int signo);
static void tcp_client_cleanup(void);
static void socket_connect(void);
static int tcp_client_rx_test(int fd,circular_buffer *cb, int len);
int sockfd = -1;

void* tcp_clent_start(void* data) {
	circular_buffer *cb = (circular_buffer *)(data);

	signal(SIGKILL,tcp_client_sig_hander);

    int  fs_sel;
	fd_set fs_read;

	socket_connect();
//    printf("TCP connected! \n");

	FD_ZERO(&fs_read);//清除一个文件描述符集
	FD_SET(sockfd,&fs_read);//将一个文件描述符加入文件描述符集中
	while(1)
	{
		FD_SET(sockfd,&fs_read);//将一个文件描述符加入文件描述符集中
		fs_sel = select(sockfd+1,&fs_read,NULL,NULL,NULL);//返回准备好的文件描述符的数目　０超时　－１出错
		if(fs_sel)
		{
			if(cb->write_offset > 4096 && cb->read_offset < 2048)//写入大于4K并且,读取小于2K则属于溢出
			{
				printf("Buffer will overflow , discard!\n");//数据溢出
				tcp_discard(cb);
				continue;//丢弃一次记录后，退出本次if，若达到条件再次进入
			}
			tcp_client_rx(sockfd,cb,TCP_RX_ONE_TIME);//256字节
			printf("TCP_Rec:data...\n");
		}
		else
		{
			printf("TCP receive err .\n");
		}
	}
    return NULL;
}

/* 数据丢弃 */
static int tcp_discard(circular_buffer *cb)
{
	cbClear(cb);
	return 0;
}
/* SIGKILL信号处理 */
static void tcp_client_sig_hander(int signo)
{
    printf("Tcp_Client thread was killed\n");
    tcp_client_cleanup();
	pthread_exit(0);
}
static void tcp_client_cleanup(void)
{
	close(sockfd);//关闭socket
}
/* tcp发送 */
int tcp_client_tx(int fd,unsigned char* msg , int len)
{
	int count = send(sockfd, (unsigned char*)msg, len, 0) ;
	if (count < 0)//返回写入的值＜0则错误
	{
		printf("send msg error: %s(errno: %d)\n", strerror(errno), errno);
		perror("Failed to write to the output.\n");//显示错误信息，到错误流
		return  -1;
	}
	return count ;//正确，则返回写入的字节数
}

/* tcp接收 */
static int tcp_client_rx_test(int fd,circular_buffer *cb, int len)
{
//    struct sockaddr temp_seraddr;
//    socklen_t temp_len;
	int count = recv(fd,(char*)cb->ptr + cb->write_offset,len,0);//初次buffer存储地址为cache的分配的起始地址,ptr为void *型需要转换真实类型进行加减
//	if (count < 0)
//	{
//		perror("Failed to read from the input.\n");
//		return -1;
//	}
	if(count == 0 || count < 0)
	{
		if (count < 0)
		{
			perror("Failed to read from the input.\n");
//			return -1;
		}
//		if(getpeername(fd,&temp_seraddr,&temp_len))
//		{
//			printf("The TCP is connected! \n");
//			usleep(5000);
//		}
//		else
//		{
			printf("Reconnect　TCP．．．! \n");
			GNNC_DEBUG_INFO("Reconnect　TCP．．．!");//调试用
			socket_connect();
			sleep(3);
//		}
	}
	else
	{
		printf("FD:%d->write_offset:%d,read_offset:%d\n",fd,cb->write_offset,cb->read_offset);

//		tcp_client_tx(fd,((unsigned char*)cb->ptr + cb->read_offset) , count);
		cb->write_offset += count;//写入的数据统计=读取的数据长度+初始分配地址    是个全局变量 假设发送0-5数据地址=则长度为6个字节 6作为下一次写入地址起始位OK
	}
	return count ;
}

/* tcp接收 */
static int tcp_client_rx(int fd,circular_buffer *cb, int len)
{
	uint32_t size = 0;
	uint32_t read_len = len;
	/*此次存入的实际大小，取 剩余空间 和 目标存入数量  两个值小的那个*/
	read_len = GET_MIN(read_len, cb->count - cb->write_offset + cb->read_offset);

	/*&(size-1)代替取模运算，同上原理，得到此次存入队列入口到末尾的大小*/
	size = GET_MIN(read_len, cb->count - (cb->write_offset & (cb->count - 1)));

	int count = recv(fd,(uint8_t*)cb->ptr + (cb->write_offset & (cb->count - 1)),size,MSG_DONTWAIT);//初次buffer存储地址为cache的分配的起始地址,ptr为void *型需要转换真实类型进行加减
	if(count == 0 || count < 0)
	{
		if (count < 0)
		{
			perror("Failed to read from the input.\n");
			return -1;
		}
			printf("Reconnect　TCP．．．! \n");
			GNNC_DEBUG_INFO("Reconnect　TCP．．．!");//调试用
			close(sockfd);
			socket_connect();
			sleep(3);
	}
	else
	{
		//第二次从开头存
		int count_2 = recv(fd,(uint8_t*)cb->ptr,read_len - size,MSG_DONTWAIT);
		if (count_2 > 0)
		{
			count += count_2;
		}
		printf("FD:%d->write_offset:%d,read_offset:%d\n",fd,cb->write_offset,cb->read_offset);
		cb->write_offset += count;//写入的数据统计=读取的数据长度+初始分配地址    是个全局变量 假设发送0-5数据地址=则长度为6个字节 6作为下一次写入地址起始位OK
	}
	return count ;
}

static void socket_connect(void)
{
    struct sockaddr_in  servaddr;
    char parameter_cmd[32];
    sprintf(parameter_cmd,"%s","serverip");
    GNNC_read_config(parameter_cmd,server_ip,para_string);//读取配置服务器ip配置及端口配置信息

    sprintf(parameter_cmd,"%s","port_num");
    GNNC_read_config(parameter_cmd,&port_num,para_int);//读取配置服务器ip配置及端口配置信息

    if( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        printf("create socket error: %s(errno: %d)\n", strerror(errno),errno);
        return;
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port_num);//转为网络字节
    if( inet_pton(AF_INET, server_ip, &servaddr.sin_addr) <= 0){//IP地址转换函数: 转换格式　源字符串　转换后存储区域
        printf("inet_pton error for %s\n",server_ip);
        return;
    }

    if( connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0){
        printf("connect error: %s(errno: %d)\n",strerror(errno),errno);
        return;
    }
    GNNC_DEBUG_INFO("connected　TCP  OK!");//调试用
}

/*
 * 强制重新连接socket
 * */
int socket_force_reconnect(void)
{
	socket_connect();
	return 0;
}

/*
 * fd:网络连接描述符
 *
 * start:首次心跳侦测包发送之间的空闲时间
 *
 * interval:两次心跳侦测包之间的间隔时间
 *
 * count:探测次数，即将几次探测失败判定为TCP断开
 */

int set_tcp_keepAlive(int fd, int start, int interval, int count)
{
    int keepAlive = 1;
    if (fd < 0 || start < 0 || interval < 0 || count < 0) return -1;  //入口参数检查 ，编程的好习惯。
    //启用心跳机制，如果您想关闭，将keepAlive置零即可
    if(setsockopt(fd,SOL_SOCKET,SO_KEEPALIVE,(void*)&keepAlive,sizeof(keepAlive)) == -1)
    {
        perror("setsockopt");
        return -1;
    }
    //启用心跳机制开始到首次心跳侦测包发送之间的空闲时间
    if(setsockopt(fd,SOL_TCP,TCP_KEEPIDLE,(void *)&start,sizeof(start)) == -1)
    {
        perror("setsockopt");
        return -1;
    }
    //两次心跳侦测包之间的间隔时间
    if(setsockopt(fd,SOL_TCP,TCP_KEEPINTVL,(void *)&interval,sizeof(interval)) == -1)
    {
        perror("setsockopt");
        return -1;
    }
    //探测次数，即将几次探测失败判定为TCP断开
    if(setsockopt(fd,SOL_TCP,TCP_KEEPCNT,(void *)&count,sizeof(count)) == -1)
    {
        perror("setsockopt");
        return -1;
    }
    return 0;
}
#ifdef __cplusplus //使用ｃ编译
}
#endif
