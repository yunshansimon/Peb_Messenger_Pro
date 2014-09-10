#include <pebble.h>
#include "Constants.h"
#include "Peb_Messenger_Pro.h"
#include "Notify_View.h"


static Window *window;
static Layer *window_layer;
static GRect window_bounds;

static Window *notify_list_window;
static Window *call_list_window;
static Window *notify_window;
static Window *call_window;


static SimpleMenuLayer *main_menu;
SimpleMenuItem main_buttons[4];
static SimpleMenuSection main_section;
MenuLayer *sub_menu;

static GFont small_custom_font;
static GFont mid_custom_font;
static GFont large_custom_font;

//---------------------------
static bool is_self_close;
static long close_delay;
static bool is_white_background;

	//views
AppTimer *firstrun_timer;
FirstView firstview;
IndicatorView indicatorview;
MsgView msgview;



static void window_load(Window *window) {
    
    window_layer = window_get_root_layer(window);
    window_bounds = layer_get_bounds(window_layer);
    init_first_view(window_layer);
 //   APP_LOG(APP_LOG_LEVEL_DEBUG, "Show first view");
    init_com();

 //   APP_LOG(APP_LOG_LEVEL_DEBUG, "Init com");
    
}

static void window_unload(Window *window) {
	
	if (indicatorview.indicator_layer!=NULL){
        display_indicator(window_layer, 10); 
    }
	send_command(REQUEST_TRANSID_CLOSE_APP);
	app_message_deregister_callbacks();
    
}

static void init(void) {
    window = window_create();
    //init communicat system
	is_self_close=false;
	is_white_background=true;
  
    window_set_window_handlers(window, (WindowHandlers) {
        .load = window_load,
        .unload = window_unload
    });
    const bool animated = true;
    window_stack_push(window, animated);
}

static void deinit(void) {
  window_destroy(window);
}

int main(void) {
  init();

  //APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

  app_event_loop();
  deinit();
}

static void init_com() {
    const uint32_t inbound_size = app_message_inbox_size_maximum();
    const uint32_t outbound_size = 32;
    app_message_register_inbox_received(in_received_handler);
    app_message_register_inbox_dropped(in_dropped_handler);
    app_message_register_outbox_sent(out_sent_handler);
    app_message_register_outbox_failed(out_failed_handler);
    app_message_open(inbound_size, outbound_size);
}

void out_sent_handler(DictionaryIterator *sent, void *context) {
    // outgoing message was delivered
    
}


void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context) {
    // outgoing message failed
    
}

void in_received_handler(DictionaryIterator *received, void *context) {
    // incoming message received
    Tuple *tuple=dict_read_first(received);
    if (tuple->key==ID_COMMAND){
        switch (tuple->value->uint8){
            case EXCUTE_NEW_MESSAGE:
            {
                APP_LOG(APP_LOG_LEVEL_DEBUG, "Get a msg.");
                if (firstrun_timer!=NULL){
                	app_timer_cancel(firstrun_timer);
                	is_self_close=true;

                }else{
                	is_self_close=false;
                }
                uint32_t delay=20000;
                uint8_t scale=0;
                uint32_t id=0;
                tuple=dict_read_next(received);
                while(tuple !=NULL){
                    switch (tuple->key){
                        case ID_CLOSE_DELAY_SEC:
                            delay=tuple->value->uint8 * 1000;
                        break;
                        case ID_CHAR_SCALE:
                            scale=tuple->value->uint8;
                        break;
                        case ID_INFO_ID:
                            id=tuple->value->uint32;
                        break;
                    }
                    tuple=dict_read_next(received);
                };
                APP_LOG(APP_LOG_LEVEL_DEBUG, "scale:%u , delay:%lu , id:%lu ", scale, delay, id);
                close_delay=delay;
                init_notifyview(scale, (is_self_close? delay:0), id , is_white_background);
            }
            break;
            case EXCUTE_NEW_CALL:
            break;
            case EXCUTE_CONTINUE_MESSAGE:
            {
                APP_LOG(APP_LOG_LEVEL_DEBUG, "Get continue msg.");
                tuple=dict_read_next(received);
                uint8_t pages=2,pagenum=1,packages=2,packagenum=1;
                while(tuple!=NULL){
                    switch(tuple->key){
                        case ID_TOTAL_PAGES:
                            pages=tuple->value->uint8;
                            APP_LOG(APP_LOG_LEVEL_DEBUG, "pages: %d", pages);
                            set_pages_notifyview(pages);
                        break;
                        case ID_PAGE_NUM:
                            pagenum=tuple->value->uint8;
                            APP_LOG(APP_LOG_LEVEL_DEBUG, "pagenum: %d", pagenum);
                            set_pagenum_notifyview(pagenum);
                        break;
                        case ID_TOTAL_PACKAGES:
                            packages=tuple->value->uint8;
                        break;
                        case ID_PACKAGE_NUM:
                            packagenum=tuple->value->uint8;
                        if (packagenum==1) {
                            clean_notifyview();
                        }
                        break;
                        case ID_ASCSTR:
                        {
                            APP_LOG(APP_LOG_LEVEL_DEBUG, "Get string:%s", tuple->value->cstring);
                        	append_str_notifyview(tuple->value->cstring);
                        }
                        break;
                        case ID_UNICHR_WIDTH:
                        {
                            int width=(int) tuple->value->uint8;
                            tuple=dict_read_next(received);
                            uint8_t pos[2]={tuple->value->data[0],tuple->value->data[1]};
                            tuple=dict_read_next(received);
                            append_bitmap_notifyview(tuple->value->data,tuple->length,pos,width);
                        }
                        break;
                    }
                    tuple=dict_read_next(received);
                }
                display_indicator(window_layer, packagenum*10/packages);
                if (packages==packagenum){
                    //show the message
                    APP_LOG(APP_LOG_LEVEL_DEBUG, "Show the notify view.");

                    show_notifyview();
                    if (is_self_close){
                    	app_timer_register(close_delay, close_app, NULL);
                    }
                }
                
                
            }
               
                
            break;
            case EXCUTE_CONTINUE_CALL:
            break;
            case DISPLAY_MESSAGE_TABLE:
            break;
            case DISPLAY_CALL_TABLE:
            break;
            case DISPLAY_CONTINUE:
            break;
            case EXCUTE_CALL_END:
            break;
        }    
    }    
}


void in_dropped_handler(AppMessageResult reason, void *context) {
    // incoming message dropped
}

//------------firstview------------------------------------
static void init_first_view(Layer *parent_layer) {
    if (firstview.base_layer==NULL){
        firstview=(FirstView){
            .base_layer=layer_create(window_bounds),
            .app_name_layer=text_layer_create(GRect(0,0,144,140)),
            .copy_right_layer=text_layer_create(GRect(0,130,144,18))
        };
        text_layer_set_font(firstview.app_name_layer,fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
        text_layer_set_text(firstview.app_name_layer, APP_NAME);
        layer_add_child(firstview.base_layer, text_layer_get_layer(firstview.app_name_layer));
    
        text_layer_set_font(firstview.copy_right_layer,fonts_get_system_font(FONT_KEY_GOTHIC_14));
        text_layer_set_text(firstview.copy_right_layer, APP_AUTHOR);
        layer_add_child(firstview.base_layer, text_layer_get_layer(firstview.copy_right_layer));
    }
    
    layer_add_child(parent_layer, firstview.base_layer);
    
    firstrun_timer=app_timer_register(FIRST_VIEW_DELAY, destory_first_view, NULL);
}

static void destory_first_view(void *data){
    if(firstview.base_layer!=NULL){
        layer_remove_from_parent(firstview.base_layer);
        text_layer_destroy(firstview.app_name_layer);
        text_layer_destroy(firstview.copy_right_layer);
        layer_destroy(firstview.base_layer);
    }
    firstrun_timer=NULL;
    firstview.base_layer=NULL;
    if (data==NULL){
    	show_main_menu(window_layer);
    }
}




//--------------system_message_view------------------------
static void destory_message(void *data) {
    if (msgview.base_layer != NULL){
        text_layer_destroy(msgview.title_layer);
        text_layer_destroy(msgview.msg_layer);
        msgview.title[0]='\0';
        msgview.msg[0]='\0';
    }
    msgview.base_layer=NULL;
}

static void display_message(Layer *baselayer , const char *title_str, const char *body_str, int sec) {
    if (msgview.base_layer == NULL){
        msgview.base_layer= baselayer;
        msgview.title[0]='\0';
        msgview.msg[0]='\0';
        strcpy(msgview.title,title_str);
        strcpy(msgview.msg,body_str);
        msgview.title_layer=text_layer_create(GRect(0,0,144,30));
        text_layer_set_font(msgview.title_layer,fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
        text_layer_set_text_alignment(msgview.title_layer, GTextAlignmentCenter);
        text_layer_set_text_color(msgview.title_layer, GColorWhite);
        text_layer_set_background_color(msgview.title_layer, GColorBlack);
        text_layer_set_text(msgview.title_layer, msgview.title);
        
        msgview.msg_layer=text_layer_create(GRect(0,30,144,130));
        text_layer_set_font(msgview.msg_layer,fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
        text_layer_set_text_alignment(msgview.msg_layer, GTextAlignmentLeft);
        text_layer_set_text(msgview.msg_layer, msgview.msg);
    }
    layer_add_child(msgview.base_layer, text_layer_get_layer(msgview.title_layer));
    layer_add_child(msgview.base_layer, text_layer_get_layer(msgview.msg_layer));
    vibes_short_pulse();
    if (sec>0){
        app_timer_register((uint32_t) sec*1000, destory_message, NULL);
    }    
}


//---------------display_indicator---------------------------
static void display_indicator(Layer *baselayer, int indicator) {   
    if (indicatorview.indicator_layer==NULL) {
        indicatorview.base_layer=baselayer;
        indicatorview.indicator_layer=text_layer_create(GRect(0,100,144,20));
        text_layer_set_font(indicatorview.indicator_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
        text_layer_set_text_alignment(indicatorview.indicator_layer, GTextAlignmentCenter);
        indicatorview.indicator_now=0;
        strcpy(indicatorview.indicator_str, INDICATOR_STR);
	}
    if (indicator>9) {
        layer_remove_from_parent(text_layer_get_layer(indicatorview.indicator_layer));
        text_layer_destroy(indicatorview.indicator_layer);
        indicatorview.base_layer=NULL;
        indicatorview.indicator_layer=NULL;
        return;
    }
    if (indicator==indicatorview.indicator_now){
        return;
    }else{
        indicatorview.indicator_now=indicator;
    }
    for (int i=0; i<indicatorview.indicator_now; i++) {
        indicatorview.indicator_str[i]='*';
    }
 //   APP_LOG(APP_LOG_LEVEL_DEBUG, "indicator:%s", indicator_str);
    text_layer_set_text(indicatorview.indicator_layer,indicatorview.indicator_str);
    layer_add_child(indicatorview.base_layer,text_layer_get_layer(indicatorview.indicator_layer));
}
//--------------main_menu-----------------------------------
static void show_main_menu(Layer *baseLayer){
    if (main_menu==NULL) {
        init_main_menu();
    }
	layer_add_child(baseLayer,simple_menu_layer_get_layer(main_menu));
}

static void init_main_menu(){
    main_buttons[0] = (SimpleMenuItem){
			.icon=gbitmap_create_with_resource(RESOURCE_ID_IMAGE_MAIN_NOTIFI),
			.subtitle=MAIN_MENU_MESSAGE_SUBTITLE,
			.title=MAIN_MENU_MESSAGE_TITLE
		};
    main_buttons[1] = (SimpleMenuItem){
			.icon=gbitmap_create_with_resource(RESOURCE_ID_IMAGE_MAIN_CALL),
			.subtitle=MAIN_MENU_CALL_SUBTITLE,
			.title=MAIN_MENU_CALL_TITLE
		};
    main_buttons[2] = (SimpleMenuItem){
			.icon=gbitmap_create_with_resource(RESOURCE_ID_IMAGE_MAIN_MUSIC),
			.subtitle=MAIN_MENU_MUSIC_SUBTITLE,
			.title=MAIN_MENU_MUSIC_TITLE
		};
	main_buttons[3] = (SimpleMenuItem){
			.icon=gbitmap_create_with_resource(RESOURCE_ID_IMAGE_MAIN_CAMERA),
			.subtitle=MAIN_MENU_CAMERA_SUBTITLE,
			.title=MAIN_MENU_CAMERA_TITLE
		};
	main_section=(SimpleMenuSection){
		.items=main_buttons,
		.num_items=4,
		.title=NULL
	};
    main_menu=simple_menu_layer_create(
									   window_bounds,
									   window,
									   &main_section,
                                        1,
									   main_menu_onclick
									   );
}

static void main_menu_onclick(int index, void *context){
	is_self_close=false;
    switch (index) {
		case MAIN_MENU_MESSAGE_INDEX:
			send_command(REQUEST_TRANSID_MESSAGE_TABLE);
			break;
		case MAIN_MENU_CALL_INDEX:
			send_command(REQUEST_TRANSID_CALL_TABLE);
			break;
		case MAIN_MENU_MUSIC_INDEX:
			break;
		case MAIN_MENU_CAMERA:
			break;
		default:
			break;
	}
}
static void send_command(uint8_t cmd){
	DictionaryIterator *iter=NULL;
	app_message_outbox_begin (&iter);
	dict_write_uint8 (iter, ID_MAIN, cmd);
	app_message_outbox_send();
}


static void close_app(void *data){
	send_command(REQUEST_TRANSID_CLOSE_APP);
	window_stack_pop_all(true);
}


