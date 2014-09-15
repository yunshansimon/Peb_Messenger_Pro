#include <pebble.h>
#include "Constants.h"
#include "Peb_Messenger_Pro.h"
#include "Notify_View.h"
#include "Call_View.h"


static Window *window;
static Layer *window_layer;
static GRect window_bounds;




static SimpleMenuLayer *main_menu;
SimpleMenuItem main_buttons[4];
static SimpleMenuSection main_section;
MenuLayer *sub_menu;
static TextLayer *title_layer;


//---------------------------
static bool is_self_close;
static long close_delay;
static bool is_white_background;

	//views
AppTimer *firstrun_timer;
FirstView firstview;


ProgressBar progressbar;

static char clock_buff[6];

static void window_load(Window *window) {
    
    window_layer = window_get_root_layer(window);
    GRect wb=layer_get_bounds(window_layer);
    window_bounds = GRect (wb.origin.x,
    		wb.origin.y+16,
    		wb.size.w,
    		wb.size.h-16);
    init_first_view(window_layer);
/*    APP_LOG(APP_LOG_LEVEL_DEBUG, "the common screen x:%d,y:%d,w:%d,h:%d", window_bounds.origin.x
    		,window_bounds.origin.y,
    		window_bounds.size.w,
    		window_bounds.size.h);
    		*/
    init_com();

 //   APP_LOG(APP_LOG_LEVEL_DEBUG, "Init com");
    
}

static void window_unload(Window *window) {
	
	if (progressbar.bg!=NULL){
        show_progress(10);
    }
	if (title_layer!=NULL) text_layer_destroy(title_layer);
	if (main_menu!=NULL) simple_menu_layer_destroy(main_menu);
	send_command(REQUEST_TRANSID_CLOSE_APP);
	app_comm_set_sniff_interval(SNIFF_INTERVAL_NORMAL);
	app_message_deregister_callbacks();
	tick_timer_service_unsubscribe();
	destroy_notifyview();
	destroy_callview(NULL);
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
    window_set_fullscreen(window,true);
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
   //             APP_LOG(APP_LOG_LEVEL_DEBUG, "Get a msg.");
            	app_comm_set_sniff_interval(SNIFF_INTERVAL_REDUCED);
                if (firstrun_timer!=NULL){

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
   //             APP_LOG(APP_LOG_LEVEL_DEBUG, "scale:%u , delay:%lu , id:%lu ", scale, delay, id);
                close_delay=delay;
                init_notifyview(scale, (is_self_close? delay:0), id , is_white_background,
                		(is_self_close? close_app : send_im_free));

            }
            break;
            case EXCUTE_NEW_CALL:
            {
            	app_comm_set_sniff_interval(SNIFF_INTERVAL_REDUCED);
            	if (firstrun_timer!=NULL){
            		is_self_close=true;
            	}else{
            		is_self_close=false;
            	}
            	uint8_t packages=1,packagenum=1;
            	char phone[16];
            	char name[28];
            	uint32_t id=0;
            	tuple=dict_read_next(received);
            	while(tuple!=NULL){
            		switch(tuple->key){
            		case ID_TOTAL_PACKAGES:
            			packages=tuple->value->uint8;
            			break;
            		case ID_PACKAGE_NUM:
            			packagenum=tuple->value->uint8;
            			break;
            		case ID_ASCSTR:
            			strcpy(name,tuple->value->cstring);
            			break;
            		case ID_PHONE_NUM:
            			strcpy(phone,tuple->value->cstring);
            		case ID_INFO_ID:
            			id=tuple->value->uint32;
            		default:
            			break;
            		}
            		tuple=dict_read_next(received);
            	}
            	init_callview(name,phone,id, (is_self_close? close_app : send_im_free));
            	if (packages==packagenum) show_callview();
            }
            break;
            case EXCUTE_CONTINUE_MESSAGE:
            {
                APP_LOG(APP_LOG_LEVEL_DEBUG, "Get continue msg.");
                tuple=dict_read_next(received);
                uint8_t pages=2,pagenum=1,packages=2,packagenum=1,width=1;
                uint8_t pos[2]={0};
                uint8_t *data=NULL;
                uint16_t length=0;
                while(tuple!=NULL){
                    switch(tuple->key){
                        case ID_TOTAL_PAGES:
                            pages=tuple->value->uint8;
//                            APP_LOG(APP_LOG_LEVEL_DEBUG, "pages: %d", pages);
                            set_pages_notifyview(pages);
                        break;
                        case ID_PAGE_NUM:
                            pagenum=tuple->value->uint8;
  //                          APP_LOG(APP_LOG_LEVEL_DEBUG, "pagenum: %d", pagenum);
                            set_pagenum_notifyview(pagenum);
                        break;
                        case ID_TOTAL_PACKAGES:
                            packages=tuple->value->uint8;
  //                          APP_LOG(APP_LOG_LEVEL_DEBUG, "packages: %d", packages);
                        break;
                        case ID_PACKAGE_NUM:
                            packagenum=tuple->value->uint8;
  //                          APP_LOG(APP_LOG_LEVEL_DEBUG, "packagenum: %d", packagenum);
                        if (packagenum==1) {
                            clean_notifyview();
                            hide_notifyview();
  //                          APP_LOG(APP_LOG_LEVEL_DEBUG,"Clean notifyview");
                        }
                        break;
                        case ID_ASCSTR:
                        {
 //                           APP_LOG(APP_LOG_LEVEL_DEBUG, "Get string:[%s]", tuple->value->cstring);
                        	append_str_notifyview(tuple->value->cstring);
                        }
                        break;
                        case ID_UNICHR_WIDTH:
                        {
                        	width=tuple->value->uint8;
                        }
                        break;
                        case ID_UNICHR_POS:
                        	pos[0]=tuple->value->data[0];
							pos[1]=tuple->value->data[1];
//							APP_LOG(APP_LOG_LEVEL_DEBUG, "row:%u, col:%u", pos[0], pos[1]);
						break;
                        case ID_UNICHR_BYTES:
                        	length=tuple->length;
                        	data=malloc(length);

                        	memcpy(data, tuple->value->data, length);
                        	break;
                    }
                    tuple=dict_read_next(received);
                }
                if (length>0){
 //               	APP_LOG(APP_LOG_LEVEL_DEBUG, "length:%u, width:%u", length, width);
                	append_bitmap_notifyview(data,length,pos,width);
                	free(data);
                }
 //               APP_LOG(APP_LOG_LEVEL_DEBUG, "Show the progress.packagenum:%d", packagenum);
                show_progress(packagenum*10/packages);
                if (packages==packagenum){
                    //show the message
 //                   APP_LOG(APP_LOG_LEVEL_DEBUG, "Show the notify view.");
                    app_comm_set_sniff_interval(SNIFF_INTERVAL_NORMAL);
                    show_notifyview();
                    set_notifyview_time(clock_buff);
                    vibes_short_pulse();

                }
                
                
            }
               
                
            break;
            case EXCUTE_CONTINUE_CALL:
            {
            	uint8_t packages=3,packagenum=2,width=1;
            	uint8_t pos[2]={0};
            	uint8_t *data=NULL;
            	uint16_t length=0;
            	tuple=dict_read_next(received);
            	while(tuple!=NULL){
                	switch(tuple->key){
                	case ID_TOTAL_PACKAGES:
                		packages=tuple->value->uint8;
                		break;
                	case ID_PACKAGE_NUM:
                		packagenum=tuple->value->uint8;
                		break;
                	case ID_UNICHR_BYTES:
                		length=tuple->length;
                		data=malloc(length);
                		memcpy(data,tuple->value->data,length);
                		break;
                	case ID_UNICHR_WIDTH:
                		width=tuple->value->uint8;
                		break;
                	case ID_UNICHR_POS:
                		pos[0]=tuple->value->data[0];
                		pos[1]=tuple->value->data[1];
                		break;
                	default:
                		break;
                	}
            		tuple=dict_read_next(received);
            	}
            	if (length>0){
            		append_bitmap_callview(data,length,pos,width);
            		free(data);
            	}
            	show_progress(packagenum*10/packages);
            	if(packagenum==packages){
            		app_comm_set_sniff_interval(SNIFF_INTERVAL_NORMAL);
            		show_callview();
            		vibes_double_pulse();
            	}
            }
            break;
            case DISPLAY_MESSAGE_TABLE:
            break;
            case DISPLAY_CALL_TABLE:
            break;
            case DISPLAY_CONTINUE:
            break;
            case EXCUTE_CALL_END:
            break;
            case EXCUTE_EMPTY:
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
    
    firstrun_timer=app_timer_register(FIRST_VIEW_DELAY, destroy_first_view, NULL);
}

static void destroy_first_view(void *data){
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







//--------------main_menu-----------------------------------
static void show_main_menu(Layer *baseLayer){
    if (main_menu==NULL) {
        init_main_menu();
    }
	layer_add_child(baseLayer,simple_menu_layer_get_layer(main_menu));
	layer_add_child(window_layer,text_layer_get_layer(title_layer));

}

static void init_main_menu(){
	title_layer=text_layer_create(GRect (0,0,window_bounds.size.w,16));
	text_layer_set_background_color(title_layer,GColorBlack);
	text_layer_set_text_color(title_layer,GColorWhite);
	text_layer_set_text_alignment(title_layer,GTextAlignmentCenter);
	text_layer_set_font(title_layer,fonts_get_system_font(FONT_KEY_GOTHIC_14));
	time_t now=time(NULL);
	show_time(localtime(&now),MINUTE_UNIT);
	tick_timer_service_subscribe(MINUTE_UNIT, show_time);
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

	if (progressbar.bg!=NULL) return;
	DictionaryIterator *iter=NULL;
	app_message_outbox_begin (&iter);
	dict_write_uint8 (iter, ID_MAIN, cmd);
	app_message_outbox_send();
}


static void close_app(void *data){
	window_stack_pop_all(true);
}

//-----------show_progress----------------------------
void show_progress(int per){
	if (per<1 ||per>10 || title_layer==NULL || per==progressbar.pre_int) return;
	if (progressbar.bg==NULL){
		progressbar.bar_frame=GRect (104,6,22,5);
		progressbar.bg=inverter_layer_create(progressbar.bar_frame);
		progressbar.gr=inverter_layer_create(GRect(progressbar.bar_frame.origin.x+1,
				progressbar.bar_frame.origin.y+1,
				1,
				2));
	}
	progressbar.pre_int=per;
	if (per==10 && progressbar.bg!=NULL){
		inverter_layer_destroy(progressbar.gr);
		inverter_layer_destroy(progressbar.bg);
		progressbar.bg=NULL;
		progressbar.pre_int=0;
		return;
	}
	layer_add_child(text_layer_get_layer(title_layer),inverter_layer_get_layer(progressbar.bg));
	layer_set_frame(inverter_layer_get_layer(progressbar.gr), GRect (
			progressbar.bar_frame.origin.x+1,
			progressbar.bar_frame.origin.y+1,
			2*per,
			2));
	layer_add_child(text_layer_get_layer(title_layer),inverter_layer_get_layer(progressbar.gr));

}

static void show_time(struct tm *tick_time, TimeUnits units_changed){
	snprintf(clock_buff,6,"%02d:%02d",tick_time->tm_hour,tick_time->tm_min);
	text_layer_set_text(title_layer,clock_buff);
	set_notifyview_time(clock_buff);
}
static void send_im_free(void *data){

	send_command(REQUEST_TRANSID_IM_FREE);
}

