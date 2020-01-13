/*
 * key_button.c
 *
 *  Created on: Dec 17, 2019
 *      Author: aron566
 */

/*****************************************************************
Filename:	keybutton.c
Author:		lisl
Date:		2016-09-18
Description:	nuc97x linux keybutton
******************************************************************/
#include "key_button.h"

extern void sendTouch(int x, int y, bool is_down);
#ifdef c_plusplus
extern "c"{
#endif
extern "C" void sendKeyNoTouch(unsigned int key);
typedef struct input_event INPUT_EVENT;

//#define DEVICE_NAME "/dev/input/event1"
#define DEVICE_NAME "/dev/input/event0"

void* key_check_loop(void* param)
{
	static int count = 0;
	int fd;
	int num;
	INPUT_EVENT event;

	fd = open( DEVICE_NAME , O_RDONLY, 0);
	if (fd < 0)
	{
		perror("Can't open button device...\n");
		return 0;
	}
	while(1)
	{
		num = read(fd, &event, sizeof(INPUT_EVENT));
		if (sizeof(INPUT_EVENT) != num)
		{
			printf("read data error\n");
			return 0;
		}

		if(event.type == EV_KEY)
		{
			//printf("event type is key_event\n");
			// keybutton code
			switch (event.code)
			{
				case KEY_LEFT:
					// keybutton status
					if(event.value)
					{
						sendKeyNoTouch(1);//backward pressed
					}
					printf("LEFT keybutton,the code is: %d",event.code);
					break;

				case KEY_RIGHT:
					if(event.value)
					{
						sendKeyNoTouch(0);//forward pressed
					}
					printf("RIGHT keybutton,the code is: %d",event.code);
					break;

				case KEY_UP:
					if(event.value)
					{
						//sendTouch(165, 185, true);
						sendKeyNoTouch(4);
						if(count > 4)
						{
							sendTouch(10, 200, true);
							sendTouch(50, 200, true);
							sendTouch(100, 200, true);
							sendTouch(200, 200, true);
							sendTouch(200, 200, false);
							printf("Right slide.");
						}
						if(count < 4)
						{
							sendTouch(200, 200, true);
							sendTouch(100, 200, true);
							sendTouch(50, 200, true);
							sendTouch(10, 200, true);
							sendTouch(10, 200, false);
							printf("Left slide.");
						}
						if(count < 8)
						{
							count++;
							if(count >= 8)
							{
								count=0;
							}
						}
//						printf("＊＊＊＊＊按下上键＊＊＊＊＊");
//						printf("UP  keybutton,the code is: %d",event.code);
					}
					else
					{
						//sendTouch(165, 185, false);
						//sendKeyNoTouch(5);
						printf("＊＊＊＊＊释放上键＊＊＊＊＊");
					}
					break;

				case KEY_DOWN:
					if(event.value)
					{
						//sendTouch(411, 101, true);
						sendKeyNoTouch(2);//KEY_ENTER
					}
					else
					{
						printf("    press up\n");
						//sendTouch(411, 101, false);
						sendKeyNoTouch(3);//KEY_LEAVE
					}
					printf("DOWN  keybutton,the code is: %d",event.code);
					break;
			}
			printf("\n");
		}
		usleep(600);
	}
	return 0;
}
#ifdef c_plusplus
}
#endif


