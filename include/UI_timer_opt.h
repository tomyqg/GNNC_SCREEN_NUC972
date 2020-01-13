/*
 * UI_timer_opt.h
 *
 *  Created on: Dec 20, 2019
 *      Author: aron566
 */

#ifndef INCLUDE_UI_TIMER_OPT_H_
#define INCLUDE_UI_TIMER_OPT_H_
#include "timer_manager.h"
#include "GuiLite.h"
#include <string.h>
extern bool logo_display_onoff;

void *key_cursor(void* ptmr);
void *user_cursor(void* ptmr);
void *start_company_logo_flash(void* ptmr);
void *display_time(void* ptmr);//首页时间显示
void start_company_logo_flashx(void* ptmr, void* parg);

void *helloworld(void* ptmr);
void *display_time_test(void* ptmr);
//绘制三态按钮，响应任务处理
class user_button :public c_button
{
public:

	virtual c_wnd* clone() { return new user_button(); }
	void check_paramater() { std::cout << "\n＋＋！当前使用为默认参数！＋＋\n";}
	void set_on_pain_style(bool style = false);//default rect
	void draw_button(int state);
	void set_push_task(pfunc pf);
	void set_on_ico_style(const BITMAP_INFO* pnormal,const BITMAP_INFO* pfocus,const BITMAP_INFO* ppushed,int x0, int y0,c_surface* surface, int z_order = Z_ORDER_LEVEL_0, unsigned int mask_rgb = DEFAULT_MASK_COLOR);
	void set_on_ico_style(const char* str,int x0 , int y0 , unsigned int normal_rgb,unsigned int focus_rgb,unsigned int push_rgb , int z_order = Z_ORDER_LEVEL_0);
	void set_tips_str(const char* str = "default") { user_tips_str = str; }
	virtual void on_key(KEY_TYPE key)//2019.12.30分支更新后无返回值
	{
		switch(key)
		{
			case KEY_ENTER:
				std::cout << "***按钮使能***\n";
				//任务
				draw_button(2);
				pfunc_runing(NULL);
				m_status = STATUS_PUSHED;
				m_parent->set_child_focus(this);
				std::cout << "\n***！！！已设置按下状态！！！***\n";
				break;
			case KEY_LEAVE:
				std::cout << "***释放按钮***\n";
				//任务
				draw_button(1);
				m_status = STATUS_FOCUSED;
				notify_parent(GL_BN_CLICKED, 0);
				break;
				default:
				m_status = STATUS_NORMAL;
				break;
		}
		return c_wnd::on_key(key);
	}
	virtual void on_paint()
	{
		c_rect rect;
		get_screen_rect(rect);
		switch (m_status)
		{
		case STATUS_NORMAL:
			std::cout << "***按钮正常***\n";
			draw_button(0);
			break;
		case STATUS_FOCUSED:
			std::cout << "***按钮聚焦***\n";
			draw_button(1);
			break;
		case STATUS_PUSHED:
			std::cout << "***按钮按下***\n";
			draw_button(2);
			break;
		default:
			std::cout << "***按钮error***\n";
			ASSERT(false);
			break;
		}
	}
protected:

	bool on_pain_style = false;//默认矩形图案
	int rect_x0 = 0;//擦除与绘制区域
	int rect_x1 = 36;
	int rect_y0 = 0;
	int rect_y1 = 20;
	const char* user_tips_str;
	int on_pain_zorder = 0;
	unsigned int rect_rgb_color_normal = GL_RGB(0, 0, 255);
	unsigned int rect_rgb_color_focus = GL_RGB(0, 255, 0);
	unsigned int rect_rgb_color_push = GL_RGB(255, 0, 0);
	unsigned int mask_rgb_color = DEFAULT_MASK_COLOR;
	int rect_z_order = Z_ORDER_LEVEL_0;
	c_surface* current_surface;
	const BITMAP_INFO* p_normal_Bitmap = NULL;
	const BITMAP_INFO* p_focus_Bitmap = NULL;
	const BITMAP_INFO* p_pushed_Bitmap = NULL;
	pfunc pfunc_runing;
};

class user_edit_box : public c_edit
{
public://（类外）可以访问
	static c_keyboard  s_keyboard;
private://只能由类成员（类内）和友元访问,派生类无法访问

protected://可以被派生类对象访问
	void on_paint()
	{
		c_rect rect;
		get_screen_rect(rect);
		c_rect empty_rect;
		empty_rect.Empty();
		switch(m_status)
		{
		case STATUS_NORMAL:
			if (m_z_order > m_parent->get_z_order())
			{
				s_keyboard.disconnect();
				m_surface->set_frame_layer_visible_rect(empty_rect, s_keyboard.get_z_order());
				m_z_order = m_parent->get_z_order();
				m_attr = (WND_ATTRIBUTION)(ATTR_VISIBLE | ATTR_FOCUS);
			}
			m_surface->fill_rect(rect, c_theme::get_color(COLOR_WND_NORMAL), m_z_order);
			c_word::draw_string_in_rect(m_surface, m_parent->get_z_order(), m_str, rect, m_font_type, m_font_color, c_theme::get_color(COLOR_WND_NORMAL), ALIGN_HCENTER | ALIGN_VCENTER);
			break;
		case STATUS_FOCUSED:
			if (m_z_order > m_parent->get_z_order())
			{
				s_keyboard.disconnect();
				m_surface->set_frame_layer_visible_rect(empty_rect, s_keyboard.get_z_order());
				m_z_order = m_parent->get_z_order();
				m_attr = (WND_ATTRIBUTION)(ATTR_VISIBLE | ATTR_FOCUS);
			}
			m_surface->fill_rect(rect, c_theme::get_color(COLOR_WND_FOCUS), m_z_order);
			c_word::draw_string_in_rect(m_surface, m_parent->get_z_order(), m_str, rect, m_font_type, m_font_color, c_theme::get_color(COLOR_WND_FOCUS), ALIGN_HCENTER | ALIGN_VCENTER);
			break;
		case STATUS_PUSHED:
			if (m_z_order == m_parent->get_z_order())
			{
				m_z_order++;
				m_attr = (WND_ATTRIBUTION)(ATTR_VISIBLE | ATTR_FOCUS | ATTR_PRIORITY);
				show_keyboard();
			}
			m_surface->fill_rect(rect.m_left, rect.m_top, rect.m_right, rect.m_bottom, c_theme::get_color(COLOR_WND_PUSHED), m_parent->get_z_order());
			m_surface->draw_rect(rect.m_left, rect.m_top, rect.m_right, rect.m_bottom, c_theme::get_color(COLOR_WND_BORDER), m_parent->get_z_order(), 2);
			strlen(m_str_input) ? c_word::draw_string_in_rect(m_surface, m_parent->get_z_order(), m_str_input, rect, m_font_type, m_font_color, c_theme::get_color(COLOR_WND_PUSHED), ALIGN_HCENTER | ALIGN_VCENTER) :
			c_word::draw_string_in_rect(m_surface, m_parent->get_z_order(), m_str, rect, m_font_type, m_font_color, c_theme::get_color(COLOR_WND_PUSHED), ALIGN_HCENTER | ALIGN_VCENTER);
			break;
		default:
			ASSERT(false);
		}
	}
};

#endif /* INCLUDE_UI_TIMER_OPT_H_ */
