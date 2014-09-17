/*
 * Notify_View.c
 *
 *  Created on: 2014-9-10
 *      Author: yunshansimon
 */
#include <pebble.h>
#include "Constants.h"
#include "Notify_View.h"
#include "Draw_Bitmap.h"
#include "Msg_Info.h"
//---------------notify_layer--------------------------------
static NotifyView notifyview;



int init_notifyview(uint8_t char_scale, uint32_t notify_delay, uint32_t id, bool whitebg , void (* callback)(void *data)){
	if (notifyview.base_window==NULL) {
//		APP_LOG(APP_LOG_LEVEL_DEBUG, "Init notifyview");
        notifyview.base_window=window_create();
        window_set_fullscreen(notifyview.base_window,true);
		GRect fb=layer_get_frame(window_get_root_layer(notifyview.base_window));
/*		APP_LOG(APP_LOG_LEVEL_DEBUG, "the common screen x:%d,y:%d,w:%d,h:%d",
				fb.origin.x,
				fb.origin.y,
				fb.size.w,
				fb.size.h);
*/

		notifyview.title_layer=text_layer_create(GRect (0,0,fb.size.w,16));
		text_layer_set_background_color(notifyview.title_layer,GColorBlack);
		text_layer_set_text_color(notifyview.title_layer,GColorWhite);
		text_layer_set_text_alignment(notifyview.title_layer,GTextAlignmentCenter);
//		APP_LOG(APP_LOG_LEVEL_DEBUG, "inited title_layer.");

		text_layer_set_font(notifyview.title_layer,fonts_get_system_font(FONT_KEY_GOTHIC_14));
		notifyview.icon_layer=bitmap_layer_create(GRect (127,0,16,16));
		bitmap_layer_set_compositing_mode(notifyview.icon_layer,GCompOpSet);

		notifyview.bitmap_yes_next=gbitmap_create_with_resource(RESOURCE_ID_IMAGE_YES_NEXT);
		notifyview.bitmap_no_next=gbitmap_create_with_resource(RESOURCE_ID_IMAGE_NO_NEXT);

//		APP_LOG(APP_LOG_LEVEL_DEBUG, "Add time service.");

//		APP_LOG(APP_LOG_LEVEL_DEBUG, "inited icon_layer");

		GRect wb=GRect (0,16,fb.size.w,fb.size.h-16);
		notifyview.ascii_layer=text_layer_create(wb);
		notifyview.unicode_layer=bitmap_layer_create(wb);
		notifyview.ascii_buff=malloc(CHAR_MAX_BUFF);
		if (notifyview.ascii_buff==NULL) return 0;
		notifyview.charscale=malloc(sizeof(CharScale));
		if (notifyview.charscale==NULL) return 0;
		text_layer_set_background_color(notifyview.ascii_layer,GColorBlack);
		text_layer_set_text_color(notifyview.ascii_layer,GColorWhite);
		bitmap_layer_set_compositing_mode(notifyview.unicode_layer,GCompOpOr);
		const GBitmap *bitmap=gbitmap_create_blank(wb.size);
		set_bitmap_to_black(bitmap);
		bitmap_layer_set_bitmap(notifyview.unicode_layer, bitmap);

		notifyview.pages=1;
		notifyview.pagenum=1;
		notifyview.is_white=whitebg;
		if (notifyview.is_white) {
            notifyview.invert_layer= inverter_layer_create(wb);
		}
	    window_set_click_config_provider (notifyview.base_window,handle_notify_click);
	    notifyview.charscale->scale=char_scale;
		switch(char_scale){
			case MESSAGE_SCALE_SMALL:
				notifyview.charscale->h=CHAR_SMALL_HEIGHT_BIT;
				notifyview.charscale->w=CHAR_SMALL_WIDTH_BIT;
				notifyview.charscale->rows=SMALL_LINES;
				notifyview.font=fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ANONY_15));
			break;
			case MESSAGE_SCALE_MID:
				notifyview.charscale->h=CHAR_MID_HEIGHT_BIT;
				notifyview.charscale->w=CHAR_MID_WIDTH_BIT;
				notifyview.charscale->rows=MID_LINES;
				notifyview.font=fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ANONY_20));
			break;
			case MESSAGE_SCALE_LARGE:
				notifyview.charscale->h=CHAR_LARGE_HEIGHT_BIT;
				notifyview.charscale->w=CHAR_LARGE_WIDTH_BIT;
				notifyview.charscale->rows=LARGE_LINES;
				notifyview.font=fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ANONY_26));
			break;
		}
		text_layer_set_font(notifyview.ascii_layer,notifyview.font);
	}
	notifyview.id=id;
	notifyview.delay=notify_delay;
	notifyview.callback=callback;
    return 1;
}
void show_notifyview(){
    if (window_stack_contains_window(notifyview.base_window)){
        window_stack_remove(notifyview.base_window, false);
    }
    text_layer_set_text(notifyview.ascii_layer, notifyview.ascii_buff);
//    APP_LOG(APP_LOG_LEVEL_DEBUG, "show the notify");

    Layer *root_layer=window_get_root_layer(notifyview.base_window);
    if(notifyview.pages>notifyview.pagenum){
    	bitmap_layer_set_bitmap(notifyview.icon_layer, notifyview.bitmap_yes_next);
    }else{
    	bitmap_layer_set_bitmap(notifyview.icon_layer, notifyview.bitmap_no_next);
    }

    window_stack_push(notifyview.base_window, true);
    layer_add_child(root_layer, text_layer_get_layer(notifyview.title_layer));
    layer_add_child(root_layer, bitmap_layer_get_layer(notifyview.icon_layer));
    layer_add_child	(root_layer,text_layer_get_layer(notifyview.ascii_layer));
	layer_add_child(root_layer,bitmap_layer_get_layer(notifyview.unicode_layer));

    if (notifyview.is_white){
        layer_add_child(root_layer,inverter_layer_get_layer(notifyview.invert_layer));
    }
  if (notifyview.delay>0){
	  if (notifyview.delay_timer==NULL){
		  notifyview.delay_timer=app_timer_register(notifyview.delay, notifyview.callback, NULL);
	  }else{
		  app_timer_reschedule(notifyview.delay_timer,notifyview.delay);
	  }
    }

}

void destroy_notifyview(){
	if(notifyview.delay_timer!=NULL) {
		app_timer_cancel(notifyview.delay_timer);
		notifyview.delay_timer=NULL;
	}
	if (notifyview.base_window!=NULL) {
		window_stack_remove(notifyview.base_window, false);
		if (notifyview.invert_layer!=NULL) {
			inverter_layer_destroy(notifyview.invert_layer);
			notifyview.invert_layer=NULL;
		}
		fonts_unload_custom_font(notifyview.font);
		gbitmap_destroy(notifyview.bitmap_no_next);
		notifyview.bitmap_no_next=NULL;
		gbitmap_destroy(notifyview.bitmap_yes_next);
		notifyview.bitmap_yes_next=NULL;
		text_layer_destroy(notifyview.title_layer);
		notifyview.title_layer=NULL;
		bitmap_layer_destroy(notifyview.icon_layer);
		notifyview.icon_layer=NULL;
		bitmap_layer_destroy(notifyview.unicode_layer);
		notifyview.unicode_layer=NULL;
		text_layer_destroy(notifyview.ascii_layer);
		notifyview.ascii_layer=NULL;
		free(notifyview.ascii_buff);
		notifyview.ascii_buff=NULL;
		free(notifyview.charscale);
		notifyview.charscale=NULL;
		window_destroy(notifyview.base_window);
		notifyview.base_window= NULL;
		if (notifyview.callback!=NULL) notifyview.callback(NULL);
		notifyview.callback=NULL;
	}

}

void append_str_notifyview(const char *src){
	strcat(notifyview.ascii_buff, src);
}

void append_bitmap_notifyview(const uint8_t *src, uint16_t length , uint8_t pos[2] , uint8_t width){
	int rowpix,colpix;
	rowpix=((int) pos[0]-((int)(notifyview.pagenum)-1)*((int)(notifyview.charscale->rows))-1)* ((int)(notifyview.charscale->h));
	colpix=((int) pos[1]-1)* (int)(notifyview.charscale->w);
//	APP_LOG(APP_LOG_LEVEL_DEBUG, "rowpix:%d, colpix:%d", rowpix, colpix );
	draw_data_to_bitmap( colpix, rowpix,(int) width,(int) length,
			notifyview.charscale->scale,
			bitmap_layer_get_bitmap(notifyview.unicode_layer),
			src);
}

void set_pages_notifyview(uint8_t pages){
	notifyview.pages=pages;
}

void set_pagenum_notifyview(uint8_t pagenum){
	notifyview.pagenum=pagenum;
}

void clean_notifyview(){
	if(notifyview.base_window!=NULL){
		notifyview.ascii_buff[0]='\0';
		set_bitmap_to_black(bitmap_layer_get_bitmap(notifyview.unicode_layer));
	}
}

static void handle_notify_click(void * context){
     window_single_click_subscribe (BUTTON_ID_SELECT, read_notify);
     window_single_click_subscribe (BUTTON_ID_DOWN, next_notify_page);
     window_single_click_subscribe (BUTTON_ID_BACK,destroy_notifyview);
}

static void next_notify_page(){

	if(notifyview.delay_timer!=NULL) {
		app_timer_cancel(notifyview.delay_timer);
		notifyview.delay_timer=NULL;
	}
	if(notifyview.pagenum<notifyview.pages){
		DictionaryIterator *iter=NULL;
		hide_notifyview();
		app_message_outbox_begin (&iter);
		dict_write_uint8 (iter, ID_MAIN, REQUEST_TRANSID_NEXTPAGE);
		app_message_outbox_send();
	}else{

	}
}

static void read_notify(){
    DictionaryIterator *iter=NULL;
    app_message_outbox_begin(&iter);
    dict_write_uint8 (iter, (uint32_t)ID_MAIN, (uint8_t) REQUEST_TRANSID_READ_NOTIFY);
    dict_write_uint32(iter, (uint32_t)ID_EXTRA_DATA,notifyview.id);
    app_message_outbox_send();
    display_message(window_get_root_layer(notifyview.base_window),"Read Message","Send Read Command To Your Phone, Please Wait...",3);
}

void set_notifyview_time(const char *clock){
	if(notifyview.title_layer!=NULL) text_layer_set_text(notifyview.title_layer,clock);
}

void hide_notifyview(){
	if (window_stack_contains_window(notifyview.base_window)) window_stack_remove(notifyview.base_window,true);
}


