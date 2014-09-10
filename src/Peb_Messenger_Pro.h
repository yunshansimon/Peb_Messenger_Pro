//
//  Peb-Messenger-Pro.h
//  
//
//  Created by 曹 阳 on 14-6-25.
//
//

#ifndef _Peb_Messenger_Pro_h
#define _Peb_Messenger_Pro_h
	static void init_first_view(Layer *parent_layer);
	static void destory_first_view();
	static void init_com();


	void out_sent_handler(DictionaryIterator *sent, void *context);
	void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context);
	void in_received_handler(DictionaryIterator *received, void *context);
	void in_dropped_handler(AppMessageResult reason, void *context);


	static void display_message(Layer *baseLayer ,const char *title_str, const char *body_str, const int sec);
	static void destory_message(void *data);
	static void display_indicator(Layer *baseLayer, int indicator);

	static void show_main_menu(Layer *baseLayer);
	static void init_main_menu();


	static void main_menu_onclick(int index, void *context);
	static void close_app(void *data);
	static void send_command(uint8_t cmd);

	typedef struct {
		Layer *base_layer;
		TextLayer *app_name_layer;
		TextLayer *copy_right_layer;
	} FirstView;

	typedef struct {
		Layer *base_layer;
		TextLayer *title_layer;
		char title[20];
		char msg[50];
		TextLayer *msg_layer;
	} MsgView;

	typedef struct {
		Layer *base_layer;
		TextLayer *indicator_layer;
		char indicator_str[11];
		int  indicator_now;
	} IndicatorView;

#endif
