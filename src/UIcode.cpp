#include "GuiLite.h"

#if defined(__linux__) || defined(_WIN32) || defined(WIN32) || defined(__linux__) || defined(__APPLE__)
	//#include "GuiLite.cpp"
#else
	//#include "GuiLite-unknow.cpp"//In keil, new operator will crash the program; but could work well in library
#endif
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "resource.h"
#include <cwchar>
#include <iostream>
#include "UI_timer_opt.h"
#include "timer_manager.h"
#include "string.h"

#define UI_WIDTH 480
#define UI_HEIGHT 272
#define EMITTER_X 120
#define EMITTER_Y 250
#define ACC_X 0
#define ACC_Y 1
#define PARTICAL_WITH	3
#define PARTICAL_HEIGHT 3

char company_name[]= "更佳电子科技有限公司";
static c_surface* s_surface;
enum WND_ID
{
	ID_ROOT = 1,
	UNLOCK_BUTTON,
	LOGIN_USER_LABEL1,
	LOGIN_KEY_LABEL1,
	LOGIN_USER_EDIT,
	LOGIN_KEY_EDIT,
	DISPLAY_MENU,
	ID_DIALOG_EXIT,
	ID_DIALOG_BUTTON_EXIT,
	LOGIN_KEY_ERROR,
	ID_PAGE1,
	ID_PAGE2,
	ID_PAGE3,
	ID_PAGE4,
	ID_PAGE5
};

class c_page : public c_wnd//－－测试
{
	virtual c_wnd* clone() { return new c_page(); }
	virtual void on_paint();
	void draw_menuconfig_background()//绘制菜单页面背景
	{
		c_rect rect;
		get_screen_rect(rect);
		unsigned int color = GL_RGB_16_to_32(((int)31727>>16)|31727);
		m_surface->fill_rect(0, 0, UI_WIDTH, UI_HEIGHT, GL_RGB(255, 255, 255), rect.m_top);

		c_bitmap::draw_bitmap(m_surface, m_z_order, c_theme::get_bmp(BITMAP_CUSTOM17), 275, 230);
		c_bitmap::draw_bitmap(m_surface, m_z_order, c_theme::get_bmp(BITMAP_CUSTOM8), 78, 65+20);
		c_bitmap::draw_bitmap(m_surface, m_z_order, c_theme::get_bmp(BITMAP_CUSTOM9), 78+50, 65+20);
		m_surface->fill_rect(74, 65, UI_WIDTH, 65+5, color, rect.m_top);
		m_surface->fill_rect(74, 65*2, UI_WIDTH, 65*2+5, color, rect.m_top);
		m_surface->fill_rect(74, 65*3, UI_WIDTH, 65*3+5, color, rect.m_top);
		m_surface->fill_rect(64, 106, 64+5, UI_HEIGHT, color, rect.m_top);
	}
};

void c_page::on_paint()
{
	c_rect rect;
	get_screen_rect(rect);
	const BITMAP_INFO* bmp = NULL;
	switch (m_id)
	{
	case ID_PAGE2:
		std::cout << "绘制页面2\n";
		bmp = c_theme::get_bmp(BITMAP_CUSTOM17);
		draw_menuconfig_background();
		break;
	case ID_PAGE3:
		std::cout << "绘制页面3\n";
		bmp = c_theme::get_bmp(BITMAP_CUSTOM16);
		  break;
	case ID_PAGE4:
		std::cout << "绘制页面4\n";
		bmp = c_theme::get_bmp(BITMAP_CUSTOM9);
		break;
	case ID_PAGE5:
		std::cout << "绘制页面5\n";
		bmp = c_theme::get_bmp(BITMAP_CUSTOM10);
		break;
	default:
		break;
	}

//	s_surface->fill_rect(0, 0, UI_WIDTH, UI_HEIGHT, GL_RGB(255, 255, 255), Z_ORDER_LEVEL_0);//擦除区域　底色图形会没
	c_bitmap::draw_bitmap(m_surface, m_z_order, bmp, rect.m_left+5, rect.m_top);
}
c_page s_page1, s_page2, s_page3, s_page4, s_page5;
static c_slide_group s_root;

//********************************************************************显示建立开始
class c_myUI : public c_wnd
{
	virtual c_wnd* clone() { return new c_myUI(); }
	virtual void on_paint(void);
	virtual void on_init_children()
	{
		c_edit* edit = (c_edit*)get_wnd_ptr(LOGIN_USER_EDIT);
		edit->set_keyboard_style(STYLE_ALL_BOARD);
		edit = (c_edit*)get_wnd_ptr(LOGIN_KEY_EDIT);
		edit->set_keyboard_style(STYLE_NUM_BOARD);
//		c_list_box  *list_box = (c_list_box*)get_wnd_ptr(ID_LIST_BOX);
//		list_box->clear_item();
//		list_box->add_item((char*)"Item 0");
//		list_box->add_item((char*)"Item 1");
//		list_box->add_item((char*)"Item 2");
//		list_box->select_item(0);
//
//		c_spin_box  *spin_box = (c_spin_box*)get_wnd_ptr(ID_SPIN_BOX);
//		spin_box->set_max_min(9, 0);
//		spin_box->set_step(1);
//		spin_box->set_value(5);
	}
	void on_button_clicked(unsigned int ctrl_id)
	{
		c_dialog::open_dialog((c_dialog*)get_wnd_ptr(ID_DIALOG_EXIT));
	}
	GL_DECLARE_MESSAGE_MAP()//delcare message
};
GL_BEGIN_MESSAGE_MAP(c_myUI)//为这个类，关联事件消息
ON_GL_BN_CLICKED(c_myUI::on_button_clicked)
//ON_SPIN_CONFIRM(ID_SPIN_BOX, c_my_ui::on_spinbox_confirm)
//ON_SPIN_CHANGE(ID_SPIN_BOX, c_my_ui::on_spinbox_change)
//ON_LIST_CONFIRM(ID_LIST_BOX, c_my_ui::on_listbox_confirm)
GL_END_MESSAGE_MAP()
/*登录界面*/
void c_myUI::on_paint()
{
	c_rect rect;
	get_screen_rect(rect);
	//c_bitmap::draw_bitmap(m_surface, m_z_order, c_theme::get_bmp(BITMAP_CUSTOM1), rect.m_left, rect.m_top);
	c_bitmap::draw_bitmap(m_surface, m_z_order, &background1_bmp, 0, 0,GL_RGB(0xFF, 0xFF, 0xFF));
	int step = 1;
	for (int i = 0; i < clock_bmp.height; i += step)//慢慢向下铺展显示，每次铺开10piex
	{
		thread_sleep(5);
		c_bitmap::draw_bitmap(m_surface, m_z_order, &clock_bmp, rect.m_left+70, rect.m_top + i, 0, i, clock_bmp.width, step, GL_RGB(0xFF, 0xFF, 0xFF));//绘制图形，GL_RGB(0,0,0)为mask区域白色颜色
	}
	c_bitmap::draw_bitmap(m_surface, m_z_order, &shield_bmp, (UI_WIDTH/2)-20, 64,GL_RGB(0xFF, 0xFF, 0xFF));//盾牌
	c_bitmap::draw_bitmap(m_surface, m_z_order, &key_bmp, 100, 140,GL_RGB(0xFF, 0xFF, 0xFF));//钥匙图标
	c_bitmap::draw_bitmap(m_surface, m_z_order, &RTC_bmp, rect.m_left+70+60, 0,GL_RGB(0xFF, 0xFF, 0xFF));//时间
	//创建字符动画线程
	c_timer_manager[0].timer_on_off = true;
	//register_timer(100,start_company_logo_flashx);//内部管理，效果差
	c_timer_manager[3].timer_on_off = true;
}
static c_myUI s_desktop;
//*****************************************时间显示--测试
void *display_time_test(void* ptmr)
{
	T_TIME t_local = {0};
	char time_data[30];
	while(1){
		t_local = get_time();
		sprintf(time_data,"%04d年 %02d 月 %02d 日 %02d:%02d:%02d",t_local.year,t_local.month,t_local.day,t_local.hour,t_local.minute,t_local.second);
		std::cout << "***logo显示中**" << time_data << "\n";
		s_surface->fill_rect(200+50, 24+180, 378+50, 35+180, GL_RGB(255, 255, 255), Z_ORDER_LEVEL_0);//擦除区域
		c_word::draw_string(s_surface, Z_ORDER_LEVEL_0, time_data, 200+50, 20, c_theme::get_font(FONT_DEFAULT), GL_RGB(0, 0, 0), GL_ARGB(0, 0, 0, 0));
		thread_sleep(c_timer_manager[3].interval_seconds);
	}
	return 0;
}
//设置绘制图形样式，默认普通矩形填充
void user_button::set_on_pain_style(bool style)
{
	on_pain_style = (style == on_pain_style)?on_pain_style:style;
}
//设置矩形默认颜色，矩形按钮起始地址,默认左上角按钮
void user_button::set_on_ico_style(const char* str,int x0 , int y0, unsigned int normal_rgb,unsigned int focus_rgb,unsigned int push_rgb, int z_order)
{
	int temp = 0;
	set_tips_str(str);//设置矩形框内字符串，覆盖connect时赋值的ｍ_str
	if(x0 != rect_x0 || y0 != rect_y0)
	{
		temp = x0-rect_x0;
		rect_x0 = x0;
		rect_x1 = rect_x0+temp;
		temp = y0-rect_y0;
		rect_y0 = y0;
		rect_y1 = rect_y0+temp;
	}
	rect_rgb_color_normal = (normal_rgb == rect_rgb_color_normal)?rect_rgb_color_normal:normal_rgb;
	rect_rgb_color_focus = (focus_rgb == rect_rgb_color_focus)?rect_rgb_color_focus:focus_rgb;
	rect_rgb_color_push = (push_rgb == rect_rgb_color_push)?rect_rgb_color_push:push_rgb;
	rect_z_order = (rect_z_order == z_order)?rect_z_order:z_order;
}

void user_button::draw_button(int state)
{
	c_rect rect;
	get_screen_rect(rect);
	if(on_pain_style)//使用图形
	{
		switch(state)
		{
		case 0://normal
			s_surface->fill_rect(rect_x0, rect_y0, rect_x1, rect_y1, GL_RGB(255, 255, 255), rect_z_order);
			c_bitmap::draw_bitmap(current_surface, rect_z_order, p_normal_Bitmap, rect_x0, rect_y0,mask_rgb_color);
			break;
		case 1://focus
			s_surface->fill_rect(rect_x0, rect_y0, rect_x1, rect_y1, GL_RGB(255, 255, 255), rect_z_order);
			c_bitmap::draw_bitmap(current_surface, rect_z_order, p_focus_Bitmap, rect_x0, rect_y0,mask_rgb_color);
			break;
		case 2://pushed
			s_surface->fill_rect(rect_x0, rect_y0, rect_x1, rect_y1, GL_RGB(255, 255, 255), rect_z_order);
			c_bitmap::draw_bitmap(current_surface, rect_z_order, p_pushed_Bitmap, rect_x0, rect_y0,mask_rgb_color);
			break;
		}
	}
	else//使用矩形
	{
		switch(state)
		{
		case 0://normal
			//m_surface->fill_rect(rect, c_theme::get_color(COLOR_WND_NORMAL), m_z_order);//相对坐标，相对于connect设置时坐标
			s_surface->fill_rect(rect_x0, rect_y0, rect_x1, rect_y1, rect_rgb_color_normal, rect_z_order);
			if (m_str)
			{
				c_word::draw_string_in_rect(m_surface, m_z_order, user_tips_str, rect, m_font_type, m_font_color, c_theme::get_color(COLOR_WND_NORMAL), ALIGN_HCENTER | ALIGN_VCENTER);
			}
			break;
		case 1://focus
			//m_surface->fill_rect(rect, c_theme::get_color(COLOR_WND_NORMAL), m_z_order);
			s_surface->fill_rect(rect_x0, rect_y0, rect_x1, rect_y1,rect_rgb_color_focus, rect_z_order);
			if (m_str)
			{
				c_word::draw_string_in_rect(m_surface, m_z_order, user_tips_str, rect, m_font_type, m_font_color, c_theme::get_color(COLOR_WND_NORMAL), ALIGN_HCENTER | ALIGN_VCENTER);
			}
			break;
		case 2://pushed
			//m_surface->fill_rect(rect, c_theme::get_color(COLOR_WND_NORMAL), m_z_order);
			s_surface->fill_rect(rect_x0, rect_y0, rect_x1, rect_y1, rect_rgb_color_push, rect_z_order);
			if (m_str)
			{
				c_word::draw_string_in_rect(m_surface, m_z_order, user_tips_str, rect, m_font_type, m_font_color, c_theme::get_color(COLOR_WND_NORMAL), ALIGN_HCENTER | ALIGN_VCENTER);
			}
			break;
		}
	}
}

//设置按钮图形起始地址，图层等参数
void user_button::set_on_ico_style(const BITMAP_INFO* pnormal,const BITMAP_INFO* pfocus,const BITMAP_INFO* ppushed,int x0, int y0,c_surface* surface, int z_order, unsigned int mask_rgb)
{
	int temp = 0;
	if(x0 != rect_x0 || y0 != rect_y0)
	{
		temp = x0-rect_x0;
		rect_x0 = x0;
		rect_x1 = rect_x0+temp;
		temp = y0-rect_y0;
		rect_y0 = y0;
		rect_y1 = rect_y0+temp;
	}
	p_normal_Bitmap = pnormal;
	p_focus_Bitmap = pfocus;
	p_pushed_Bitmap = ppushed;
	current_surface = surface;
	rect_z_order = (rect_z_order == z_order)?rect_z_order:z_order;
	mask_rgb_color = (mask_rgb == mask_rgb_color)?mask_rgb_color:mask_rgb;
}
void user_button::set_push_task(pfunc pf)
{
	pfunc_runing = pf;
}
//*******************************初始化光标状态显示***********************

unsigned short edit_box_key_x0 = (UI_WIDTH/2)+20+4+c_timer_manager[2].offset;
unsigned short edit_box_key_x1 = (UI_WIDTH/2)+20+6+c_timer_manager[2].offset;
unsigned short edit_box_key_y0 = 187;
unsigned short edit_box_key_y1 = 187+16;

unsigned short edit_box_user_x0 = (UI_WIDTH/2)+20+4+c_timer_manager[1].offset;
unsigned short edit_box_user_x1 = (UI_WIDTH/2)+20+6+c_timer_manager[1].offset;
unsigned short edit_box_user_y0 = 157;
unsigned short edit_box_user_y1 = 157+16;
void *key_cursor(void* ptmr)
{
	static bool trigger_state = true;
	//绘图
	while(1){
		(trigger_state)? s_surface->fill_rect(edit_box_key_x0, edit_box_key_y0, edit_box_key_x1, edit_box_key_y1, 0, Z_ORDER_LEVEL_0):s_surface->fill_rect(edit_box_key_x0, edit_box_key_y0, edit_box_key_x1, edit_box_key_y1, 0xFFFFFFFF, Z_ORDER_LEVEL_0);
		(trigger_state)? std::cout << "***key光标停止中**\n":std::cout << "***key光标闪烁中***\n";
		trigger_state = !trigger_state;
		std::cout << "***key is alive**\n";
		thread_sleep(c_timer_manager[2].interval_seconds);
	}
	return 0;
}
void *user_cursor(void* ptmr)
{
	static bool trigger_state = true;
	//绘图
	while(1){
		(trigger_state)? s_surface->fill_rect(edit_box_user_x0, edit_box_user_y0, edit_box_user_x1, edit_box_user_y1, 0, Z_ORDER_LEVEL_0):s_surface->fill_rect(edit_box_user_x0, edit_box_user_y0, edit_box_user_x1, edit_box_user_y1, 0xFFFFFFFF, Z_ORDER_LEVEL_0);
		(trigger_state)? std::cout << "***user光标停止中**\n":std::cout << "***user光标闪烁中***\n";
		trigger_state = !trigger_state;
		std::cout << "***user is alive**\n";
		thread_sleep(c_timer_manager[1].interval_seconds);
	}
	return 0;
}
//**************************************************光标状态管理结束

//**************************************************logo动画显示线程管理

#define START_X		24
#define START_Y		270
#define FONT_SIZE	16

void *start_company_logo_flash(void* ptmr)
{
	char one_word[4];
	unsigned int color = GL_RGB_16_to_32(((int)8484>>16)|8484);
	//绘图
	while(1){
		static int begin_y = START_Y;
		static unsigned short char_index = 0;
		for(;char_index < sizeof(company_name)-1;char_index+=3)//去掉几个尾部字符
		{
			for(int i = begin_y;i > 0;i--)//y坐标偏移，向上
			{
				unsigned short tmp = char_index;
				//int erase_current_y = 0;
				for(int y = i;y < UI_HEIGHT;y+=FONT_SIZE)//y坐标偏移字符串垂直间距,从底部开始向下刷一列字符串
				{
					if(tmp >= sizeof(company_name) || y > UI_HEIGHT)
					{
						break;
					}
					memset(one_word, 0, sizeof(one_word));
					memcpy(one_word, company_name+tmp, 3);
					s_surface->fill_rect(START_X, y, START_X+11, y+FONT_SIZE, color, Z_ORDER_LEVEL_0);
					c_word::draw_string(s_surface, Z_ORDER_LEVEL_0, one_word, START_X, y, c_theme::get_font(FONT_CUSTOM1), GL_RGB(255, 255, 255), GL_ARGB(0, 0, 0, 0));
					//std::cout << "***logo显示中**" << one_word << "\n";
					//erase_current_y = y;
					tmp += 3;
				}
				thread_sleep(c_timer_manager[0].interval_seconds);
				//s_surface->fill_rect(START_X, erase_current_y, START_X+11, i, color, Z_ORDER_LEVEL_0);
			}
			s_surface->fill_rect(START_X, 0, START_X+11, FONT_SIZE, color, Z_ORDER_LEVEL_0);//擦除最后一个
			begin_y = FONT_SIZE;//y起始坐标设s为去掉一个字符高度
		}
		char_index = 0;
		begin_y = UI_HEIGHT;
	}
	return 0;
}

//-----------------------------------------------------定时器管理
bool logo_display_onoff = true;
void start_company_logo_flashx(void* ptmr, void* parg)
{
	char one_word[4];
	unsigned int color = GL_RGB_16_to_32(((int)8484>>16)|8484);
	//绘图
	if(logo_display_onoff == true)/*16长*/
	{
		static int begin_y = START_Y;
		static unsigned short char_index = 0;
		for(;char_index < sizeof(company_name)-1;char_index+=3)//去掉几个尾部字符
		{
			for(int i = begin_y;i > 0;i--)//y坐标偏移，向上
			{
				unsigned short tmp = char_index;
				//int erase_current_y = 0;
				for(int y = i;y < UI_HEIGHT;y+=FONT_SIZE)//y坐标偏移字符串垂直间距,从底部开始向下刷一列字符串
				{
					if(tmp >= sizeof(company_name) || y > UI_HEIGHT)
					{
						break;
					}
					memset(one_word, 0, sizeof(one_word));
					memcpy(one_word, company_name+tmp, 3);
					s_surface->fill_rect(START_X, y, START_X+11, y+FONT_SIZE, color, Z_ORDER_LEVEL_0);
					c_word::draw_string(s_surface, Z_ORDER_LEVEL_0, one_word, START_X, y, c_theme::get_font(FONT_CUSTOM1), GL_RGB(255, 255, 255), GL_ARGB(0, 0, 0, 0));
					//std::cout << "***logo显示中**" << one_word << "\n";
					//erase_current_y = y;
					tmp += 3;
				}
				thread_sleep(100);
				//s_surface->fill_rect(START_X, erase_current_y, START_X+11, i, color, Z_ORDER_LEVEL_0);
			}
			s_surface->fill_rect(START_X, 0, START_X+11, FONT_SIZE, color, Z_ORDER_LEVEL_0);//擦除最后一个
			begin_y = FONT_SIZE;//y起始坐标设s为去掉一个字符高度
		}
		char_index = 0;
		begin_y = UI_HEIGHT;
	}
	else
	{
		thread_sleep(10);
	}
}
//**************************************************logo动画显示线程管理结束

//*****************************************时间显示
void *display_time(void* ptmr)
{
	T_TIME t_local = {0};
	char time_data[30];
	while(1){
		t_local = get_time();
		sprintf(time_data,"%04d年 %02d 月 %02d 日 %02d:%02d:%02d",t_local.year,t_local.month,t_local.day,t_local.hour,t_local.minute,t_local.second);
		//std::cout << "***logo显示中**" << time_data << "\n";
		s_surface->fill_rect(200, 24, 378, 35, GL_RGB(255, 255, 255), Z_ORDER_LEVEL_0);//擦除区域
		c_word::draw_string(s_surface, Z_ORDER_LEVEL_0, time_data, 200, 20, c_theme::get_font(FONT_DEFAULT), GL_RGB(0, 0, 0), GL_ARGB(0, 0, 0, 0));
		thread_sleep(c_timer_manager[3].interval_seconds);
	}
	return 0;
}


class c_particle {
public:
	c_particle(){
		initialize();
	}
	void initialize() {
		m_x = EMITTER_X; m_y = EMITTER_Y;//emitter 发射 范围120,250
		m_x_velocity = (rand() % 7) - 3; m_y_velocity = -15 - (rand() % 4);//速率，落差
	}
	void move() {
		s_surface->fill_rect(m_x, m_y, m_x + PARTICAL_WITH - 1, m_y + PARTICAL_HEIGHT - 1, 0xFFFFFFFF, Z_ORDER_LEVEL_0);//clear previous image
		m_x_velocity += ACC_X; m_y_velocity += ACC_Y;//每次高度增加1，由负到零再到正
		m_x += m_x_velocity; m_y += m_y_velocity;//x随机数确定后始终一个偏移方向，y由大到小（显示效果：由低到高，再到低）
		if (m_x < 0 || (m_x + PARTICAL_WITH - 1) >= UI_WIDTH || m_y < 0 || (m_y + PARTICAL_HEIGHT - 1) >= UI_HEIGHT) {
			initialize();
		}
	}
	void draw() {
		int red = rand() % 5 * 63;
		int green = rand() % 5 * 63;
		int blue = rand() % 5 * 63;
		s_surface->fill_rect(m_x, m_y, m_x + PARTICAL_WITH - 1, m_y + PARTICAL_HEIGHT - 1, GL_RGB(red, green, blue), Z_ORDER_LEVEL_0);//draw current image
	}
	int m_x, m_y, m_x_velocity, m_y_velocity;
};
c_particle particle_array[100];
////////////////////////////////////////////////////////////////


//********************************************************************显示建立结束
//********************************************************************登录输入开始
class c_login_user_edit : public c_edit
{
public:
	virtual c_wnd* clone() { return new c_login_user_edit(); }
	virtual void on_paint(void);
	virtual void on_key(KEY_TYPE key)
	{
		switch(key)
		{
			case KEY_ENTER:
				std::cout << "***用户名登录使能***\n";
				//任务
				if(c_timer_manager[1].timer_on_off == false)
				{
					c_timer_manager[1].timer_on_off = true;
				}
				break;
//			case KEY_FORWARD:
//				std::cout << "***输入前进***\n";
//				break;
//			case KEY_BACKWARD:
//				std::cout << "***输入后退***\n";
//				break;
			case KEY_LEAVE:
				std::cout << "***用户名登录失能***\n";
				//任务
				break;
				default:
				break;
		}

	}
};
void c_login_user_edit::on_paint(void)//建立 user输入框初始图形
{
	c_rect rect;
	get_screen_rect(rect);
	set_keyboard_style(STYLE_ALL_BOARD);
	switch (m_status)
	{
	case STATUS_NORMAL:
		std::cout << "***输入user正常***\n";
		c_bitmap::draw_bitmap(m_surface, m_z_order, &editbox_bmp, (UI_WIDTH/2)+20, 150,GL_RGB(0xFF, 0xFF, 0xFF));//login usr
		c_timer_manager[1].timer_on_off = false;
		s_surface->fill_rect(edit_box_user_x0, edit_box_user_y0, edit_box_user_x1, edit_box_user_y1, 0xFFFFFFFF, Z_ORDER_LEVEL_0);//消除光标
		break;
	case STATUS_FOCUSED://聚焦状态
		std::cout << "***输入user聚焦***\n";
		c_bitmap::draw_bitmap(m_surface, m_z_order, &editbox_focus_bmp, (UI_WIDTH/2)+20, 150,GL_RGB(0xFF, 0xFF, 0xFF));//login usr
		break;
	case STATUS_PUSHED:
		std::cout << "***输入user按下***\n";
		//光标闪
		break;
	default:
		ASSERT(false);
		break;
	}

}
//static c_login_user_edit s_login_user_edit;
static c_edit s_login_user_edit;
//-----------------------------------------------------------------------------
class c_login_key_edit : public c_edit
{
public:
	virtual c_wnd* clone() { return new c_login_key_edit(); }
	virtual void on_paint(void);
	virtual void on_key(KEY_TYPE key)
	{
		switch(key)
		{
			case KEY_ENTER:
				std::cout << "***用户密码登录使能***\n";
				//任务
				if(c_timer_manager[2].timer_on_off == false)
				{
					c_timer_manager[2].timer_on_off = true;
				}

//			case KEY_FORWARD:
//				std::cout << "***输入前进***\n";
//				break;
//			case KEY_BACKWARD:
//				std::cout << "***输入后退***\n";
//				break;
				break;
			case KEY_LEAVE:
				std::cout << "***用户密码登录失能***\n";
				//任务
				break;
				default:
				break;
		}

	}
};
void c_login_key_edit::on_paint(void)//建立 key输入框初始图形
{
	c_rect rect;
	get_screen_rect(rect);
	set_keyboard_style(STYLE_NUM_BOARD);

	switch (m_status)
	{
	case STATUS_NORMAL:
		std::cout << "***输入key正常***\n";
		c_bitmap::draw_bitmap(m_surface, m_z_order, &editbox_bmp, (UI_WIDTH/2)+20, 180,GL_RGB(0xFF, 0xFF, 0xFF));//login KEY
		c_timer_manager[2].timer_on_off = false;
		s_surface->fill_rect(edit_box_key_x0, edit_box_key_y0, edit_box_key_x1, edit_box_key_y1, 0xFFFFFFFF, Z_ORDER_LEVEL_0);//消除光标
		break;
	case STATUS_FOCUSED://聚焦状态
		std::cout << "***输入key聚焦***\n";
		c_bitmap::draw_bitmap(m_surface, m_z_order, &editbox_focus_bmp, (UI_WIDTH/2)+20, 180,GL_RGB(0xFF, 0xFF, 0xFF));//login KEY
		break;
	case STATUS_PUSHED:
		std::cout << "***输入key按下***\n";
		//光标闪
		break;
	default:
		ASSERT(false);
		break;
	}

}
static c_login_key_edit  s_login_key_edit;
//********************************************************************登录输入结束

//********************************************************************登录按钮开始
class c_unlock_button : public c_button
{
	virtual c_wnd* clone() { return new c_unlock_button(); }
	virtual void on_key(KEY_TYPE key)
	{
		switch(key)
		{
			case KEY_ENTER:
				std::cout << "***按钮使能***\n";
				//任务
				m_parent->set_child_focus(this);
				m_status = STATUS_PUSHED;
				on_paint();//添加
				c_dialog::open_dialog((c_dialog*)get_wnd_ptr(ID_DIALOG_EXIT));
				std::cout << "\n***！！！已打开对话框！！！***\n";
				break;
			case KEY_LEAVE:
				std::cout << "***释放按钮***\n";
				//任务
				c_dialog::close_dialog(m_surface);
				m_status = STATUS_FOCUSED;
				on_paint();//添加
				notify_parent(GL_BN_CLICKED, 0);
				break;
				default:
				break;
		}

	}
	virtual void on_paint()
	{
		c_rect rect;
		get_screen_rect(rect);

		switch (m_status)
		{
		case STATUS_NORMAL:
			std::cout << "***按钮正常***\n";
			s_surface->fill_rect(UI_WIDTH-70, UI_HEIGHT-70, UI_WIDTH-6, UI_HEIGHT-6, GL_RGB(255, 255, 255), Z_ORDER_LEVEL_0);//擦除区域
			c_bitmap::draw_bitmap(m_surface, m_z_order, &enter_bmp, UI_WIDTH-70, UI_HEIGHT-70,GL_RGB(0xFF, 0xFF, 0xFF));//解锁键
			break;
		case STATUS_FOCUSED://聚焦状态
			std::cout << "***按钮聚焦***\n";
			s_surface->fill_rect(UI_WIDTH-70, UI_HEIGHT-70, UI_WIDTH-6, UI_HEIGHT-6, GL_RGB(255, 255, 255), Z_ORDER_LEVEL_0);//擦除区域
			c_bitmap::draw_bitmap(m_surface, m_z_order, &enter_focus_bmp, UI_WIDTH-70, UI_HEIGHT-70,GL_RGB(0xFF, 0xFF, 0xFF));//解锁键
			break;
		case STATUS_PUSHED:
			std::cout << "***按钮按下***\n";
			s_surface->fill_rect(UI_WIDTH-70, UI_HEIGHT-70, UI_WIDTH-6, UI_HEIGHT-6, GL_RGB(255, 255, 255), Z_ORDER_LEVEL_0);//擦除区域
			c_bitmap::draw_bitmap(m_surface, m_z_order, &enter_push_bmp, UI_WIDTH-70, UI_HEIGHT-70,GL_RGB(0xFF, 0xFF, 0xFF));//解锁键
			break;
		default:
			std::cout << "***按钮error***\n";
			ASSERT(false);
			break;
		}
	}
};

static c_unlock_button s_unlock_button;//构造函数会调用paint去建立图形
//static c_button s_unlock_button;
//static user_button s_unlock_button;
//********************************************************************登录按钮结束

//***************************************************************建立一个弹窗中按钮
class c_diglog_button : public c_button
{
	virtual c_wnd* clone() { return new c_diglog_button(); }
	virtual void on_key(KEY_TYPE key)
	{
		std::cout << "***！！！进入！！！***\n";
		switch(key)
		{
			case KEY_ENTER:
				std::cout << "！！！弹窗按钮使能！！！\n";
				//任务
				s_surface->fill_rect(262, 181, 262+36, 181+20, GL_RGB(0, 255, 0), Z_ORDER_LEVEL_2);//相对于屏幕坐标
				c_dialog::close_dialog(m_surface);
				break;
			case KEY_LEAVE:
				std::cout << "！！！弹窗按钮释放！！！\n";
				//任务
				s_surface->fill_rect(262, 181, 262+36, 181+20, GL_RGB(255, 255, 255), Z_ORDER_LEVEL_2);
				break;
				default:
				break;
		}
	}
	virtual void on_paint()
	{
		c_rect rect;
		get_screen_rect(rect);
		std::cout << "！！！绘制进入！！！***\n";
		switch (m_status)
		{
		case STATUS_NORMAL:
			std::cout << "！！！绘制正常！！！\n";
			s_surface->fill_rect(262, 181, 262+36, 181+20, GL_RGB(55, 55, 55), Z_ORDER_LEVEL_2);//擦除区域
			break;
		case STATUS_FOCUSED://聚焦状态
			std::cout << "！！！绘制聚焦！！！\n";
			s_surface->fill_rect(262, 181,262+36, 181+20, GL_RGB(255, 55, 255), Z_ORDER_LEVEL_2);//擦除区域
			break;
		case STATUS_PUSHED:
			std::cout << "！！！绘制按下！！！\n";
			s_surface->fill_rect(262, 181, 262+36, 181+20, GL_RGB(255, 255, 55), Z_ORDER_LEVEL_2);//擦除区域
			break;
		default:
			std::cout << "***按钮error***\n";
			ASSERT(false);
			break;
		}
	}
};
//************************************************************一个弹窗中按钮建立结束

//static c_diglog_button s_diglog_exit_button;
static c_button s_diglog_exit_button;

//********************************************************************提示话框开始
class c_tips_dialog : public c_dialog
{
	virtual c_wnd* clone() { return new c_tips_dialog(); }
	void on_button_clicked(unsigned int ctrl_id)
	{
		std::cout << "\n\n！！！关闭对话框！！！***\n\n";
		c_dialog::close_dialog(m_surface);
	}
	virtual void on_paint(void)
	{
		std::cout << "\n\n！！！绘制对话框进入！！！***\n\n";
		c_wnd::set_active_child(&s_diglog_exit_button);

		unsigned int color = GL_RGB_16_to_32(((int)16936>>16)|16936);
		s_surface->fill_rect(160, 77, 160+240-1, 77+135-1, color, Z_ORDER_LEVEL_1);
	}
	GL_DECLARE_MESSAGE_MAP()//delcare message
};
GL_BEGIN_MESSAGE_MAP(c_tips_dialog)//为这个类关联，事件处理方法
ON_GL_BN_CLICKED(c_tips_dialog::on_button_clicked)//建立对话框中LOGIN_KEY_ERROR事件处理
GL_END_MESSAGE_MAP()
static c_tips_dialog tips_diglog;
void *helloworld(void* ptmr)
{
	std::cout << "this is test program!\n";
	tips_diglog.open_dialog((c_dialog*)tips_diglog.get_wnd_ptr(ID_DIALOG_EXIT));
	return 0;
}
WND_TREE w_tips_widgets[] =
{
	{&s_diglog_exit_button,ID_DIALOG_BUTTON_EXIT,"EXIT",102, 104, 36, 20,NULL},//相对坐标
	{NULL, 0 , 0, 0, 0, 0, 0}
};

WND_TREE w_unlock_button_diglog[] =
{
	{(c_wnd*)&tips_diglog, ID_DIALOG_EXIT, 0, 160, 77, 240, 135, w_tips_widgets},//120 67相对于父窗口的坐标-其父为unlock_button
	//{(c_wnd*)&tips_diglog, ID_DIALOG_EXIT, 0, 120, 67, 240, 135, NULL},
	//{(c_wnd*)&s_diglog_exit_button,ID_DIALOG_BUTTON_EXIT,"EXIT",102, 104, 36, 20,NULL},//绝对坐标
	{NULL, 0 , 0, 0, 0, 0, 0}
};

//********************************************************************提示话框结束



//*********************************************************************对话框开始
class c_main_menu : public c_dialog
{
	virtual c_wnd* clone() { return new c_main_menu(); }
	virtual void on_paint(void);
};
void c_main_menu::on_paint(void)//建立 登录初始图形
{
	c_rect rect;
	get_screen_rect(rect);
	std::cout << "***翻页***\n";
	//菜单显示区域c_bitmap::draw_bitmap(m_surface, m_z_order, &enter_bmp, UI_WIDTH-70, UI_HEIGHT-70,GL_RGB(0xFF, 0xFF, 0xFF));//解锁键登录
}
//static c_main_menu s_main_menu;
//*********************************************************************对话框结束

static c_label s_login_user_label1, s_login_key_label1;//无图形只有文字
static c_edit xx,xx1;
//static c_slide_group s_root;//切换页面根
static WND_TREE s_desktop_children[] = //建立树形控件关系并绘制控件
{
	//{(c_wnd*)&s_desktop, ID_ROOT, 0, 0, 0, UI_WIDTH, UI_HEIGHT, NULL},
	{(c_wnd*)&s_login_user_label1, LOGIN_USER_LABEL1, "USER:", 220, 154, 40, 20, NULL},//登录用户名标签
	{(c_wnd*)&s_login_key_label1, LOGIN_KEY_LABEL1,   "KEY  :", 220, 184, 40, 20, NULL},//登录用户密码标签

	//{(c_wnd*)&xx, LOGIN_USER_EDIT, 0, (UI_WIDTH/2)+20, 150, 107, 28, NULL},//登录用户输入编辑框
	//{(c_wnd*)&xx1, LOGIN_KEY_EDIT,  0, (UI_WIDTH/2)+20, 180, 107, 28, NULL},//登录用户密码输入编辑框
	{(c_wnd*)&s_login_user_edit, LOGIN_USER_EDIT, 0, (UI_WIDTH/2)+20, 150, 107, 28, NULL},//登录用户输入编辑框
	{(c_wnd*)&s_login_key_edit, LOGIN_KEY_EDIT,  0, (UI_WIDTH/2)+20, 180, 107, 28, NULL},//登录用户密码输入编辑框

	//{(c_wnd*)&s_unlock_button, UNLOCK_BUTTON, 0, 410, 202, 64, 64, w_unlock_button_diglog},//解锁位置x 410 y202 长64宽64 范围x410-474 y202-266
	{(c_wnd*)&s_unlock_button, UNLOCK_BUTTON, 0, 0, 0, UI_WIDTH, UI_HEIGHT, w_unlock_button_diglog},//对的　＋　作为弹窗的相对坐标
	//{(c_wnd*)&s_unlock_button, UNLOCK_BUTTON, 0, 410, 202, 64, 64, NULL},
	//{(c_wnd*)&s_unlock_button, UNLOCK_BUTTON, 0, 0, 0, UI_WIDTH, UI_HEIGHT, NULL},

	//{(c_wnd*)&tips_diglog, ID_DIALOG_EXIT, 0, 120, 100, 120, 100, w_tips_widgets},

	{NULL,0,0,0,0,0,0}
};
//**********************************************************************标签开始

//**********************************************************************标签结束
//********************************************************************建立实例开始


//********************************************************************建立实例结束
//////////////////////// start UI ////////////////////////

void load_resource() {
	c_theme::add_font(FONT_DEFAULT, &Microsoft_YaHei_20);//将字体作为--默认FONT_DEFAULT
	c_theme::add_font(FONT_CUSTOM1, &songti_8);
	c_theme::add_color(COLOR_WND_FONT, GL_RGB(0, 0, 0));//GL_RGB(255, 255, 243)
	c_theme::add_color(COLOR_WND_NORMAL, GL_RGB(59, 75, 94));
	c_theme::add_color(COLOR_WND_PUSHED, GL_RGB(33, 42, 53));
	c_theme::add_color(COLOR_WND_FOCUS, GL_RGB(78, 198, 76));
	c_theme::add_color(COLOR_WND_BORDER, GL_RGB(46, 59, 73));
	c_theme::add_bitmap(BITMAP_CUSTOM1, &background1_bmp);//加载资源，左侧黑背
	c_theme::add_bitmap(BITMAP_CUSTOM2, &clock_bmp);//加载资源，时钟
	c_theme::add_bitmap(BITMAP_CUSTOM3, &key_bmp);//加载资源，钥匙
	c_theme::add_bitmap(BITMAP_CUSTOM5, &shield_bmp);//加载资源，盾牌
	c_theme::add_bitmap(BITMAP_CUSTOM6, &siren_bmp);//加载资源，报警
	c_theme::add_bitmap(BITMAP_CUSTOM7, &system_bmp);//加载资源，菜单设置
	c_theme::add_bitmap(BITMAP_CUSTOM8, &usb_bmp);//加载资源，usb
	c_theme::add_bitmap(BITMAP_CUSTOM9, &worldwide_bmp);//加载资源，网络
	c_theme::add_bitmap(BITMAP_CUSTOM10, &enter_bmp);//加载资源，解锁
	c_theme::add_bitmap(BITMAP_CUSTOM11, &enter_focus_bmp);//加载资源，解锁聚焦
	c_theme::add_bitmap(BITMAP_CUSTOM12, &enter_push_bmp);//加载资源，解锁按下
	c_theme::add_bitmap(BITMAP_CUSTOM13, &editbox_bmp);//加载资源，login
	c_theme::add_bitmap(BITMAP_CUSTOM14, &RTC_bmp);//加载资源，RTC时间显示窗口
	c_theme::add_bitmap(BITMAP_CUSTOM15, &editbox_focus_bmp);//加载资源，登录输入框聚焦状态
	c_theme::add_bitmap(BITMAP_CUSTOM16, &MAIN_BACKGROUND_bmp);//菜单背景
	c_theme::add_bitmap(BITMAP_CUSTOM17, &logoGNNC_bmp);//公司ｌｏｇｏ
}

c_display* display;
c_timer_task_opt main_timer_task;//定时器管理类
static WND_TREE s_root_children[] =
{
	{ NULL,0,0,0,0,0,0 }
};
void create_ui(void* phy_fb, int screen_width, int screen_height, int color_bytes, struct EXTERNAL_GFX_OP* gfx_op) {
	load_resource();//载入字体
	/*创建实例*/
	display = new c_display(phy_fb, screen_width, screen_height, UI_WIDTH, UI_HEIGHT, color_bytes,  (1 + 5)/*1 root + 5 pages*/, gfx_op);
	s_surface = display->alloc_surface(Z_ORDER_LEVEL_2);//设置层数
	s_surface->set_active(true);//写入标志
	s_surface->fill_rect(0, 0, UI_WIDTH , UI_HEIGHT , 0xFFFFFF, Z_ORDER_LEVEL_0);//白底

	//s_unlock_button.set_on_pain_style(true);//使用图形
	//s_unlock_button.set_push_task(helloworld);//task
	//s_unlock_button.set_on_ico_style(&enter_bmp,&enter_focus_bmp,&enter_push_bmp,410,202,s_surface,Z_ORDER_LEVEL_0,GL_RGB(255, 255, 255));
	//s_unlock_button.set_on_ico_style(400 , 200, GL_RGB(0, 0, 255),GL_RGB(0, 255, 0),GL_RGB(255, 0, 0), Z_ORDER_LEVEL_0);

	c_timer_manager_t xx;
	xx.interval_seconds =2000;
	xx.offset = 1;
	xx.p_task = display_time_test;
	xx.timer_on_off = true;
	xx.pid = 0;
	xx.run_state = false;
	main_timer_task.add_timer_task(xx);
	main_timer_task.set_timer_onff(xx,false);
	main_timer_task.set_timer_onff(xx,false);
/*********************************MENU***************************************/



/***********************每个display可创建6个surface*******************************
 *static c_surface* s_surface;存储surface、指针
 *通过s_surface = display->alloc_surface(Z_ORDER_LEVEL_xx)为每个surface可分配0-2层代号对应不同控件，分配的是最大层数
 *通过s_surface->set_active(true);激活surface，用来最终绘制图形 确认 是否写入 真实 映射内存区域，如何调用s_surface->fill_rect(0, 50, UI_WIDTH - 1, UI_HEIGHT - 1, 0xFFFFFFFF, Z_ORDER_LEVEL_1)
 *
 * *********************************/
	s_desktop.set_surface(s_surface);
	s_desktop.connect(NULL, ID_ROOT, 0, 0, 0, UI_WIDTH, UI_HEIGHT, s_desktop_children);//定义界面元素关系：s_desktop这个c_wnd其子类s_desktop_children，在这个界面所有信号发到这个树形关系去找！
	s_desktop.show_window();

	s_root.set_surface(s_surface);
	s_root.connect(&s_desktop, ID_PAGE1, 0, 0, 0, UI_WIDTH, UI_HEIGHT, s_root_children);//只要是窗口就使用connect去连接竖形结构初始化
	//将ID_PAGE1与s_desktop(c_wnd类)绑定并调用s_page2的和children的on_pain进行绘制
//	s_root.add_slide(&s_desktop, ID_ROOT, 0, 0, UI_WIDTH, UI_HEIGHT, s_desktop_children);
	s_root.add_slide(&s_page2, ID_PAGE2, 0, 0, UI_WIDTH, UI_HEIGHT, NULL);
	s_root.add_slide(&s_page3, ID_PAGE3, 0, 0, UI_WIDTH, UI_HEIGHT, NULL);
	s_root.add_slide(&s_page4, ID_PAGE4, 0, 0, UI_WIDTH, UI_HEIGHT, NULL);
	s_root.add_slide(&s_page5, ID_PAGE5, 0, 0, UI_WIDTH, UI_HEIGHT, NULL);
	s_root.set_active_slide(0);
	std::cout << "ADD OK!\n";
//	s_root.show_window();
/***********************************************************************/

//	while(1) {
//		for (int i = 0; i < sizeof(particle_array)/sizeof(c_particle); i++) {
//			particle_array[i].move();
//			particle_array[i].draw();
//		}
//		thread_sleep(50);
//	}
}

//////////////////////// interface for all platform ////////////////////////
extern "C" void start_GUILite(void* phy_fb, int width, int height, int color_bytes, struct EXTERNAL_GFX_OP* gfx_op) {
	create_ui(phy_fb, width, height, color_bytes, gfx_op);
}

extern "C" void* get_ui_GUILite()
{
    return display->get_updated_fb(0, 0, false);
}


int captureUi()
{
	return display->snap_shot("snap_short.bmp");
}
void sendTouch(int x, int y, bool is_down)
{
	is_down ? s_desktop.on_touch(x, y, TOUCH_DOWN) : s_desktop.on_touch(x, y, TOUCH_UP);
}
extern "C" void sendKeyNoTouch(unsigned int key)
{
	s_desktop.on_key(KEY_TYPE(key));
}
