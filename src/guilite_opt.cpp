/*
 * guilite_opt.cpp
 *
 *  Created on: Dec 17, 2019
 *      Author: aron566
 */

#include "guilite_opt.h"
gui_hardware_info_t gui_hardware_info_value;
/***************************GUILite***********************************/
extern "C" void start_GUILite(void* phy_fb, int width, int height, int color_bytes, struct EXTERNAL_GFX_OP* gfx_op);

static void* get_embeded_fb_in_display_app(int shared_id);
static void* get_dev_fb(char* path, int &width, int &height, int &color_bytes);
static const char* s_welcome =
"-------------------------------------------------------------------\n"
"*  GuiLite Screen code [Version 1.6]                              *\n"
"-------------------------------------------------------------------\n"
"How to run inside QT APP?\n"
"If x64: ./display-x64 | ./GNNC_SCREEN shared-fb\n"
"If ARM: ./display-arm | ./GNNC_SCREEN shared-fb\n\n"

"How to run on Linux?\n"
"Type command: ./GNNC_SCREEN /dev/fb0\n\n"
"-------------------- Help ------------------------\n"
"exit: 	Exit the App.\n"
"--------------------------------------------------\n";

enum FRAMEBUFFER_MODE
{
	FB_NULL_MODE,
	FB_APP_MODE,
	FB_DEV_MODE
};

int GuiLite_Init(void)//(int argc, char** argv)
{
	printf(s_welcome);
	system("chmod 777 .sync_build.sh");
	system("./.sync_build.sh GNNC_SCREEN");

	int color_bytes = 2;
	int screen_width = 240;
	int screen_height = 320;

	FRAMEBUFFER_MODE fb_mode = FB_NULL_MODE;
	char *fb_dev_path = NULL;
	int share_id = 1;//should be same with display app.
	unsigned char argc = 2;//代替默认参数
	if(argc == 2)
	{
		char* argument = "/dev/fb0";//argv[1];
		if(strcmp(argument,"shared-fb") == 0)
		{
			fb_mode = FB_APP_MODE;
		}
		else if(strncmp(argument, "/dev", 4) == 0)
		{
			fb_dev_path = argument;
			fb_mode = FB_DEV_MODE;
		}
		else
		{
			printf("Invalid argument, should be: shared-fb, or framebuffer path\n");
			return -1;
		}
	}

	void* phy_fb = NULL;
	switch(fb_mode)
	{
		case FB_APP_MODE:
			phy_fb = get_embeded_fb_in_display_app(share_id);//获取共享内存返回的第一个字节指针
		break;
		case FB_DEV_MODE:
			phy_fb = get_dev_fb(fb_dev_path, screen_width, screen_height, color_bytes);//获得映射到内存的操作指针
		break;
		default:
			phy_fb = calloc(screen_width * screen_height, color_bytes);//动态申请内存并初始化类似与malloc（malloc需要计算大小，这个只需要填入数量和类型大小自动计算，并初始化0）
		break;
	}
	gui_hardware_info_value.phy_fb = phy_fb;
	gui_hardware_info_value.screen_width = screen_width;
	gui_hardware_info_value.screen_height = screen_height;
	gui_hardware_info_value.color_bytes = color_bytes;
    return 0;
}

static void* get_embeded_fb_in_display_app(int shared_id)
{
	void* ret = NULL;
	while(ret == NULL)
	{
		int shmid = shmget(shared_id, 0, SHM_R | SHM_W);//创建共享内存
		if (shmid >= 0)
		{
			if(ret = (unsigned char*)shmat(shmid, 0, 0))//访问
			{
				break;
			}
			printf("shmat failed! run display app first.\n");
		}
		else
		{
			printf("shmget failed! run display app first\n");
		}
		sleep(1);
	}
	return ret;
}
/*
 * 嵌入式设备使用
 * 用户层调用获取屏幕设备信息（open，ioctl：FBIOGET_VSCREENINFO命令获取参数信息：vinfo存放参数区域，）
 * mmap将屏幕设备映射到应用进程的内存空间中，通过直接的内存操作对设备读写
 * */
static void* get_dev_fb(char* path, int &width, int &height, int &color_bytes)
{
	int fd = open (path, O_RDWR);//嵌入式用 ？ 打开/dev/fbx
	if(0 > fd)
	{
		printf("%s, open fb failed!\n", path);
		_exit(-1);
	}

	struct fb_var_screeninfo vinfo;//创建屏幕信息 包含可见分辨率等
	if (0 > ioctl(fd, FBIOGET_VSCREENINFO, &vinfo))//获取当前屏幕信息->vinfo
	{
		printf("get fb info failed!\n");
		_exit(-1);
    }

    width = vinfo.xres;//设置宽x方向
    height = vinfo.yres;//设置高y方向
    color_bytes = vinfo.bits_per_pixel / 8;//将颜色深度位转为字节
    if (width & 0x3)//判断显示区域大小
    {
        printf("Warning: vinfo.xres should be divided by 4!\nChange your display resolution to meet the rule.\n");
    }
    printf("vinfo.xres=%d\n",vinfo.xres);//打印当前屏幕信息
    printf("vinfo.yres=%d\n",vinfo.yres);
    printf("vinfo.bits_per_pixel=%d\n",vinfo.bits_per_pixel);
    /*将打开的/dev/xx设备映射到内存，通过内存直接操作，不指定映射地址，指定映射大小（显示面积*颜色深度）*/
	void* fbp = mmap(0, (vinfo.xres * vinfo.yres * color_bytes), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if(0 > fbp)
	{
		printf("mmap fb failed!\n");
		_exit(-1);
	}
	memset(fbp, 0, (vinfo.xres * vinfo.yres * color_bytes));//映射成功--清空操作
	return fbp;
}
/************************************************************/
void *guilite_display_opt(void *data)
{
	gui_hardware_info_t *gui_data = (gui_hardware_info_t *)data;
	/*将内存映射的操作地址，获取到的实际屏幕XY 参数和颜色深度传递*/
	start_GUILite(gui_data->phy_fb, gui_data->screen_width, gui_data->screen_height, gui_data->color_bytes, NULL);//never return;
	return NULL;
}
