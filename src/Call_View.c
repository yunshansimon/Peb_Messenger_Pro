/*
 * Call_View.c
 *
 *  Created on: Sep 15, 2014
 *      Author: yunshan
 */

#include <pebble.h>
#include "Call_View.h"
#include "Draw_Bitmap.h"
#include "Constants.h"
#include "Msg_Info.h"
static const char ON_THE_LINE[]="On The Line";
static const char TITLE_MSG[]="HANG UP CALL";
static CallView callview;

void init_callview (const char *name, const char *phonenum, uint32_t id , void (* callback)(void *data)){
	if(callview.base_window==NULL){
		callview.base_window=window_create();

		callview.title_text_layer=text_layer_create(GRect(0,0,124,30));
		text_layer_set_font(callview.title_text_layer,fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
		text_layer_set_text_alignment(callview.title_text_layer, GTextAlignmentCenter);

		GRect frame=GRect(0,30,128,96);
		callview.name_text_layer=text_layer_create(frame);
		callview.custom_font=fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ANONY_26));
		text_layer_set_font(callview.name_text_layer,callview.custom_font);
		text_layer_set_background_color(callview.name_text_layer,GColorBlack);
		text_layer_set_text_color(callview.name_text_layer,GColorWhite);

		callview.name_bitmap_layer=bitmap_layer_create(frame);
		bitmap_layer_set_compositing_mode(callview.name_bitmap_layer,GCompOpOr);
		const GBitmap *bitmap=gbitmap_create_blank(frame.size);
		set_bitmap_to_black(bitmap);
		bitmap_layer_set_bitmap(callview.name_bitmap_layer,bitmap);

		callview.inverter_layer=inverter_layer_create(frame);

		callview.phone_text_layer=text_layer_create(GRect(0,128,124,32));
		text_layer_set_font(callview.phone_text_layer,fonts_get_system_font(FONT_KEY_GOTHIC_24));

		callview.action_bar = action_bar_layer_create();
		action_bar_layer_set_icon(callview.action_bar, BUTTON_ID_UP, gbitmap_create_with_resource (RESOURCE_ID_IMAGE_MENU_PICKUP));
		action_bar_layer_set_icon(callview.action_bar,BUTTON_ID_SELECT, gbitmap_create_with_resource (RESOURCE_ID_IMAGE_MENU_SMS));
		action_bar_layer_set_icon(callview.action_bar, BUTTON_ID_DOWN, gbitmap_create_with_resource (RESOURCE_ID_IMAGE_MENU_HANGOUT));
		action_bar_layer_set_click_config_provider(callview.action_bar, click_config_provider);

		callview.phonenum=malloc(16);
		callview.name=malloc(28);
		callview.begin_time=0;
	}
	callview.id=id;
	callview.callback=callback;
	strcpy(callview.name,name);
	strcpy(callview.phonenum,phonenum);
}

void show_callview(){
	if(window_stack_contains_window(callview.base_window)){
		window_stack_remove(callview.base_window,false);
	}
	text_layer_set_text(callview.title_text_layer,TITLE_INFO);
	Layer *rootlayer=window_get_root_layer(callview.base_window);
	layer_add_child(rootlayer,text_layer_get_layer(callview.title_text_layer));
	layer_add_child(rootlayer,text_layer_get_layer(callview.name_text_layer));
	layer_add_child(rootlayer,bitmap_layer_get_layer(callview.name_bitmap_layer));
	layer_add_child(rootlayer,inverter_layer_get_layer(callview.inverter_layer));
	layer_add_child(rootlayer,text_layer_get_layer(callview.phone_text_layer));
	action_bar_layer_add_to_window(callview.action_bar, callview.base_window);

	window_stack_push(callview.base_window,true);
}

void destroy_callview(void *data){
	if(callview.base_window!=NULL){
		if(window_stack_contains_window(callview.base_window)){
			window_stack_remove(callview.base_window,false);
		}
		action_bar_layer_destroy(callview.action_bar);
		text_layer_destroy(callview.phone_text_layer);
		inverter_layer_destroy(callview.inverter_layer);
		bitmap_layer_destroy(callview.name_bitmap_layer);
		text_layer_destroy(callview.name_text_layer);
		text_layer_destroy(callview.title_text_layer);
		free(callview.name);
		free(callview.phonenum);
		window_destroy(callview.base_window);
		callview.base_window=NULL;
		if(callview.begin_time>0) tick_timer_service_unsubscribe();
		callview.begin_time=0;
		if(callview.callback!=NULL) callview.callback(NULL);
	}
	callview.base_window=NULL;
}

//----------------click function--------------------------------
static void click_config_provider(void *context) {
	window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
	window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
	window_long_click_subscribe	(BUTTON_ID_UP,
			500,
			up_long_press_handler,
			NULL
	);
	window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
	window_long_click_subscribe	(BUTTON_ID_SELECT,
			500,
			select_long_press_handler,
			NULL
	);
	window_single_click_subscribe(BUTTON_ID_BACK, back_click_handler);
}
static void send_command(uint8_t cmd){
	DictionaryIterator *iter=NULL;
	app_message_outbox_begin (&iter);
	dict_write_uint8 (iter, ID_MAIN, cmd);
	app_message_outbox_send();
}
static void send_command_string(uint8_t cmd, const char *data){
	DictionaryIterator *iter=NULL;
	app_message_outbox_begin (&iter);
	dict_write_uint8 (iter, ID_MAIN, cmd);
	dict_write_cstring(iter,ID_EXTRA_DATA, data);
	app_message_outbox_send();
}
static void send_command_uint(uint8_t cmd, uint8_t data){
	DictionaryIterator *iter=NULL;
	app_message_outbox_begin (&iter);
	dict_write_uint8 (iter, ID_MAIN, cmd);
	dict_write_uint8(iter,ID_EXTRA_DATA,data);
	app_message_outbox_send();
}
static void up_click_handler(ClickRecognizerRef recognizer, void *context){
	if(callview.begin_time>0) return;
	text_layer_set_text(callview.title_text_layer,ON_THE_LINE);
	callview.begin_time=time(NULL);
	send_command_uint(REQUEST_TRANSID_PICKUP_PHONE,REQUEST_EXTRA_SPEAKER_ON);
	tick_timer_service_subscribe(SECOND_UNIT,update_time);
}
static void update_time(struct tm *tick_time, TimeUnits units_changed){
	time_t passed_time=time(NULL)-callview.begin_time;
	snprintf(callview.phonenum,sizeof(callview.phonenum),"%02d:%02d",(int)(passed_time/60),(int)(passed_time%60));
	text_layer_set_text(callview.phone_text_layer,callview.phonenum);
}
static void up_long_press_handler(ClickRecognizerRef recognizer, void *context){
	if(callview.begin_time>0) return;
	text_layer_set_text(callview.title_text_layer,ON_THE_LINE);
	callview.begin_time=time(NULL);
	send_command_uint(REQUEST_TRANSID_PICKUP_PHONE,REQUEST_EXTRA_SPEAKER_OFF);
	tick_timer_service_subscribe(SECOND_UNIT,update_time);
}
static void select_click_handler(ClickRecognizerRef recognizer, void *context){
	if(callview.begin_time>0) return;
	send_command_string(REQUEST_TRANSID_HANGOFF_SMS1,callview.phonenum);
	display_message(window_get_root_layer(callview.base_window),TITLE_MSG,"You just hang up the phone and send No.1 SMS to the contact.",3);
	app_timer_register(3500,destroy_callview,NULL);
}
static void select_long_press_handler(ClickRecognizerRef recognizer, void *context){
	if(callview.begin_time>0) return;
	send_command_string(REQUEST_TRANSID_HANGOFF_SMS2,callview.phonenum);
	display_message(window_get_root_layer(callview.base_window),TITLE_MSG,"You just hang up the phone and send No.2 SMS to the contact.",3);
	app_timer_register(3500,destroy_callview,NULL);
}
static void down_click_handler(ClickRecognizerRef recognizer, void *context){
	send_command(REQUEST_TRANSID_HANGOFF_PHONE);
	display_message(window_get_root_layer(callview.base_window),TITLE_MSG,"You just hang up the phone",3);
	app_timer_register(3500,destroy_callview,NULL);
}
static void back_click_handler(ClickRecognizerRef recognizer, void *context){
	destroy_callview(NULL);
}

void append_bitmap_callview(const uint8_t *src, uint16_t length , uint8_t pos[2] , uint8_t width){
	int rowpix,colpix;
	rowpix=((int) pos[0]-1)* CHAR_LARGE_HEIGHT_BIT;
	colpix=((int) pos[1]-1)* CHAR_LARGE_WIDTH_BIT * width;
	//	APP_LOG(APP_LOG_LEVEL_DEBUG, "rowpix:%d, colpix:%d", rowpix, colpix );
	draw_data_to_bitmap( colpix, rowpix,(int) width,(int) length,
			MESSAGE_SCALE_LARGE,
			bitmap_layer_get_bitmap(callview.name_bitmap_layer),
			src);
}

