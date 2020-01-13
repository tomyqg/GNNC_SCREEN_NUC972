/*
 * serial_opt.h
 *
 *  Created on: Dec 17, 2019
 *      Author: aron566
 */

#ifndef INCLUDE_SERIAL_OPT_H_
#define INCLUDE_SERIAL_OPT_H_
#ifdef __cplusplus //使用ｃ编译
extern "C" {
#endif

#include "CircularQueue.h"//环形队列管理

#define USART_RX_ONE_TIME   128//单次至多接收128

#define UART_SEND_BUFF_MAX  256
#define UART_NUM_MAX		10

//extern int file_id_1;//串口ttyS6


extern int file_id[];

typedef struct circular_buffer {
    void *ptr;
    int fd;
    uint32_t count;
    uint32_t read_offset;
    uint32_t write_offset;
    CQ_handleTypeDef seq_manager;//备用环形队列
} circular_buffer;

uint32_t cb_bytes_can_read(circular_buffer *cb);
void cb_read_offset_inc(circular_buffer *cb, unsigned long  cnt);
void cbClear(circular_buffer *cb);
uint32_t CQ_get_buff_Data(circular_buffer *cb, uint8_t *targetBuf, uint32_t len);

circular_buffer *cb_create(char *path,unsigned long order);


int usart_init(void);
void usart_cleanup(void);
int usart_tx(int fd,unsigned char* msg , int len);
int usart_rx(int fd,circular_buffer *cb, int len);
int usart_discard(circular_buffer *cb);
void usart_sig_hander(int signo);

void* usart1_rx_start(void* data);
void* usart2_rx_start(void* data);
void* usart3_rx_start(void* data);
void* usart4_rx_start(void* data);
void* usart5_rx_start(void* data);
void* usart6_rx_start(void* data);
void* usart7_rx_start(void* data);
void* usart8_rx_start(void* data);
void* usart9_rx_start(void* data);
void* usart10_rx_start(void* data);

/*对外切换串口运行模式接口*/
uint8_t switch_running_mode(uint8_t mode);


#ifdef __cplusplus //使用ｃ编译
}
#endif
#endif /* INCLUDE_SERIAL_OPT_H_ */
