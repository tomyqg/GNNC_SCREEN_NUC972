/*
 * guilite_opt.h
 *
 *  Created on: Dec 17, 2019
 *      Author: aron566
 */

#ifndef INCLUDE_GUILITE_OPT_H_
#define INCLUDE_GUILITE_OPT_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <errno.h>
#include <sys/stat.h>
typedef struct gui_hardware_info{
	void *phy_fb;
	int color_bytes;
	int screen_width;
	int screen_height;
}gui_hardware_info_t;
extern gui_hardware_info_t gui_hardware_info_value;
int GuiLite_Init(void);
void *guilite_display_opt(void *data);


#endif /* INCLUDE_GUILITE_OPT_H_ */
