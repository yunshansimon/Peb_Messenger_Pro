/*
 * Call_View.h
 *
 *  Created on: Sep 15, 2014
 *      Author: yunshan
 */

#ifndef CALL_VIEW_H_
#define CALL_VIEW_H_
#define TITLE_INFO "Incoming call"
typedef struct{
	char *phonenum;
	char *name;
	Window *base_window;
	TextLayer *title_text_layer;
	TextLayer *name_text_layer;
	BitmapLayer *name_bitmap_layer;
	InverterLayer *inverter_layer;
	TextLayer *phone_text_layer;
	ActionBarLayer *action_bar;
	GFont custom_font;
	uint32_t id;
	time_t begin_time;
	void (* callback)(void *data);
}CallView;
void init_callview (const char *name, const char *phonenum, uint32_t id, void (* callback)(void *data));
void show_callview();
void destroy_callview(void *data);
void append_bitmap_callview(const uint8_t *src, uint16_t length , uint8_t pos[2] , uint8_t width);
void call_hook();
static void click_config_provider(void *context);
static void up_click_handler(ClickRecognizerRef recognizer, void *context);
static void up_long_press_handler(ClickRecognizerRef recognizer, void *context);
static void select_click_handler(ClickRecognizerRef recognizer, void *context);
static void select_long_press_handler(ClickRecognizerRef recognizer, void *context);
static void down_click_handler(ClickRecognizerRef recognizer, void *context);
static void back_click_handler(ClickRecognizerRef recognizer, void *context);
static void send_command(uint8_t cmd);
static void send_command_string(uint8_t cmd, const char *data);
static void send_command_uint(uint8_t cmd, uint8_t data, const char *phonenum);
static void update_time(struct tm *tick_time, TimeUnits units_changed);

#endif /* CALL_VIEW_H_ */
