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
	static void destroy_first_view();
	static void init_com();


	void out_sent_handler(DictionaryIterator *sent, void *context);
	void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context);
	void in_received_handler(DictionaryIterator *received, void *context);
	void in_dropped_handler(AppMessageResult reason, void *context);





	static void show_main_menu(Layer *baseLayer);
	static void init_main_menu();


	static void main_menu_onclick(int index, void *context);
	static void close_app(void *data);
	static void send_command(uint8_t cmd);
	static void show_progress(int per);
	static void show_time(struct tm *tick_time, TimeUnits units_changed);
	static void send_im_free(void *data);

	typedef struct {
		Layer *base_layer;
		TextLayer *app_name_layer;
		TextLayer *copy_right_layer;
	} FirstView;


	typedef struct{
			GRect bar_frame;
			InverterLayer *bg;
			InverterLayer *gr;
			int pre_int;
		}ProgressBar;

#endif
