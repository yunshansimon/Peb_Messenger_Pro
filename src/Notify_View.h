/*
 * Notify_View.h
 *
 *  Created on: 2014-9-10
 *      Author: yunshansimon
 */

#ifndef NOTIFY_VIEW_H_
#define NOTIFY_VIEW_H_
	static void read_notify();
	static void next_notify_page();
	static void handle_notify_click(void *context);
	int init_notifyview(uint8_t char_scale, uint32_t notify_delay, uint32_t id , bool whitebg);
	void show_notifyview();
	void destory_notifyview(void *data);
	void set_pages_notifyview(uint8_t pages);
	void set_pagenum_notifyview(uint8_t pagenum);
	void append_str_notifyview(const char *src);
	void append_bitmap_notifyview(const uint8_t *src, uint16_t length , uint8_t pos[2] , uint8_t width);
	void clean_notifyview();
	typedef struct {
			uint8_t w;
			uint8_t h;
			uint8_t rows;
			uint8_t scale;
	} CharScale;
	typedef struct {
		Window *base_window;
		BitmapLayer *unicode_layer;
		TextLayer *ascii_layer;
		InverterLayer *invert_layer;
		uint32_t delay;
		uint32_t id;
		uint8_t pages;
		uint8_t pagenum;
		char *ascii_buff;
		CharScale *charscale;
		bool is_white;
		GFont font;
	} NotifyView;



#endif /* NOTIFY_VIEW_H_ */
