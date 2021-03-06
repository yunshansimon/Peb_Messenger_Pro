/*
 * Notify_View.h
 *
 *  Created on: 2014-9-10
 *      Author: yunshansimon
 */

#ifndef NOTIFY_VIEW_H_
#define NOTIFY_VIEW_H_
	void read_notify();
	void next_notify_page();
	void handle_notify_click(void *context);
	int init_notifyview(uint8_t char_scale, uint32_t notify_delay, uint32_t id , bool whitebg, void (* callback)(void *data));
	void show_notifyview();
	void destroy_notifyview();
	void set_pages_num_notifyview(uint8_t pages, uint8_t nums);
	void append_str_notifyview(const char *src);
	void append_bitmap_notifyview(const uint8_t *src, uint16_t length , uint8_t pos[2] , uint8_t width);
	void clean_notifyview();
	void set_notifyview_time(const char *clock);
	void hide_notifyview();
	typedef struct {
			uint8_t w;
			uint8_t h;
			uint8_t rows;
			uint8_t scale;
	} CharScale;
	typedef struct {
		Window *base_window;
		TextLayer *title_layer;
		BitmapLayer *icon_layer;
		BitmapLayer *unicode_layer;
		Layer *ascii_layer;
		InverterLayer *invert_layer;
		uint32_t delay;
		void (* callback)(void *data);
		uint32_t id;
		uint8_t pages;
		uint8_t pagenum;
		char *ascii_buff;
		CharScale *charscale;
		bool is_white;
		GFont font;
		AppTimer *delay_timer;
		GBitmap *bitmap_no_next;
		GBitmap *bitmap_yes_next;
		GBitmap *unicode_bitmap;
	} NotifyView;

	void draw_text_on_layer(Layer *layer, GContext *ctx);

	void load_font(uint8_t char_scale);
	void stop_notify_close_timer();
#endif /* NOTIFY_VIEW_H_ */
