#include <pebble.h>
#include "Constants.h"
#include "Peb_Messenger_Pro.h"
#include "Notify_View.h"
#include "Call_View.h"
#include "List_View.h"


static Window *window;
static Layer *window_layer;
static GRect window_bounds;




static SimpleMenuLayer *main_menu;
SimpleMenuItem main_buttons[2];
static SimpleMenuSection main_section;
static TextLayer *title_layer;


//---------------------------
static bool is_self_close;
static bool is_white_background;

	//views
AppTimer *firstrun_timer;
FirstView firstview;


ProgressBar progressbar;

static char clock_buff[6];
static char version[10];

static void window_load(Window *window) {
    
    window_layer = window_get_root_layer(window);
    GRect wb=layer_get_bounds(window_layer);
    window_bounds = GRect (wb.origin.x,
    		wb.origin.y+16,
    		wb.size.w,
    		wb.size.h-16);
    init_com();
    switch(launch_reason()){
    case APP_LAUNCH_PHONE:
    	is_self_close=true;
    	break;
    default:
    	is_self_close=false;
    }
    bluetooth_connection_service_subscribe(handle_pebble_disconnect);
    	init_first_view(window_layer);
/*    APP_LOG(APP_LOG_LEVEL_DEBUG, "the common screen x:%d,y:%d,w:%d,h:%d", window_bounds.origin.x
    		,window_bounds.origin.y,
    		window_bounds.size.w,
    		window_bounds.size.h);
    		*/

 //   APP_LOG(APP_LOG_LEVEL_DEBUG, "Init com");
    
}

static void window_unload(Window *window) {
	destroy_first_view();
	if (progressbar.bg!=NULL){
        show_progress(10);
    }
	if (title_layer!=NULL) text_layer_destroy(title_layer);
	if (main_menu!=NULL) {
		gbitmap_destroy(main_buttons[0].icon);
		gbitmap_destroy(main_buttons[1].icon);
		simple_menu_layer_destroy(main_menu);
	}
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

	is_white_background=true;
	snprintf(version,10,"%d.%d.%d",MAJOR_VERSION,MINOR_VERSION,POINT_VERSION);
    window_set_window_handlers(window, (WindowHandlers) {
        .load = window_load,
        .unload = window_unload,
        .appear=show_title
    });
    window_set_fullscreen(window,true);
    const bool animated = true;
    window_stack_push(window, animated);
}

static void show_title(Window *window){
	if(title_layer!=NULL){
		layer_add_child(window_layer,text_layer_get_layer(title_layer));
	}
}

static void init_title(){
	title_layer=text_layer_create(GRect (0,0,window_bounds.size.w,16));
	text_layer_set_background_color(title_layer,GColorBlack);
	text_layer_set_text_color(title_layer,GColorWhite);
	text_layer_set_text_alignment(title_layer,GTextAlignmentCenter);
	text_layer_set_font(title_layer,fonts_get_system_font(FONT_KEY_GOTHIC_14));
	time_t now=time(NULL);
	show_time(localtime(&now),MINUTE_UNIT);
	tick_timer_service_subscribe(MINUTE_UNIT, show_time);
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
    const uint32_t outbound_size = 64;
//    APP_LOG(APP_LOG_LEVEL_DEBUG, "Inbox buffer:%lu",inbound_size);
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
    Tuple *tuple=find_tuple_by_id(ID_COMMAND ,received);
    if (tuple==NULL){
    	APP_LOG(APP_LOG_LEVEL_DEBUG, "Bad receive, there no command at all.");
    	return;
    }
        switch (tuple->value->uint8){
            case EXCUTE_NEW_MESSAGE:
            {
   //             APP_LOG(APP_LOG_LEVEL_DEBUG, "Get a msg.");
          //  	app_comm_set_sniff_interval(SNIFF_INTERVAL_REDUCED);

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
                        case ID_WHITE_BACKGROUND:
                        	if(tuple->value->uint8>0){
                        		is_white_background=true;
                        	}else{
                        		is_white_background=false;
                        	}

                        break;
                    }
                    tuple=dict_read_next(received);
                };
   //             APP_LOG(APP_LOG_LEVEL_DEBUG, "scale:%u , delay:%lu , id:%lu ", scale, delay, id);
                init_notifyview(scale, delay, id , is_white_background,
                		(is_self_close? (call_view_on_top() ? hide_notifyview:close_app) : send_im_free));
                app_comm_set_sniff_interval(SNIFF_INTERVAL_REDUCED);
            }
            break;
            case EXCUTE_NEW_CALL:
            {
 //               APP_LOG(APP_LOG_LEVEL_DEBUG, "Get a call.");

            	if (is_self_close) stop_notify_close_timer();
            	uint8_t packages=1,packagenum=1;
            	char phone[16];
            	char name[28];
            	uint32_t id=0;
            	tuple=dict_read_next(received);
            	while(tuple!=NULL){
            		switch(tuple->key){
            		case ID_PAGE_INFO:
            		{
            			uint8_t *packinfo=tuple->value->data;
            			packages=packinfo[2];
            			packagenum=packinfo[3];
            		}
            			break;
            		case ID_ASCSTR:
             //           APP_LOG(APP_LOG_LEVEL_DEBUG, "get ascstr:%s", tuple->value->cstring);
            			strcpy(name,tuple->value->cstring);
            			break;
            		case ID_PHONE_NUM:
               //         APP_LOG(APP_LOG_LEVEL_DEBUG, "get ascstr:%s", tuple->value->cstring);
            			strcpy(phone,tuple->value->cstring);
            		case ID_INFO_ID:
            			id=tuple->value->uint32;
            		case ID_WHITE_BACKGROUND:
            			if(tuple->value->uint8>0){
							is_white_background=true;
						}else{
							is_white_background=false;
						}
            		default:
            			break;
            		}
            		tuple=dict_read_next(received);
            	}
            	init_callview(name,phone,id, is_white_background , (is_self_close? close_app : send_im_free));
            	if (packages==packagenum) {
            		show_callview();
            		send_im_free(NULL);
            	}
            	app_comm_set_sniff_interval(SNIFF_INTERVAL_REDUCED);
            }
            break;
            case EXCUTE_CONTINUE_MESSAGE:
            {
   //             APP_LOG(APP_LOG_LEVEL_DEBUG, "Get continue msg.");
                tuple=dict_read_next(received);
                uint8_t packages=2,packagenum=1,width=1;
                uint8_t pos[2]={0};
                uint8_t *posextra=NULL;
                uint8_t extranum=0;
                uint8_t *data=NULL;
                uint16_t length=0;
                while(tuple!=NULL){
  //              	APP_LOG(APP_LOG_LEVEL_DEBUG, "Msg id:%lu",tuple->key);
                    switch(tuple->key){
                        case ID_PAGE_INFO:
                        {
                			uint8_t *packinfo=tuple->value->data;
                            set_pages_num_notifyview(packinfo[0],packinfo[1]);
                            packages=packinfo[2];
                            packagenum=packinfo[3];
          //                  APP_LOG(APP_LOG_LEVEL_DEBUG, "Packages:%u, PackageNum:%u",packages,packagenum);

                        }
                        break;
                        case ID_ASCSTR:
                        {
        //                    APP_LOG(APP_LOG_LEVEL_DEBUG, "Get string:[%s]", tuple->value->cstring);
                        	append_str_notifyview(tuple->value->cstring);
                        }
                        break;
                        case ID_UNICHR_INFO:
                        {
                        	uint8_t *uniinfo=tuple->value->data;
                        	width=uniinfo[0];
                        	pos[0]=uniinfo[1];
                        	pos[1]=uniinfo[2];
                        }
                        break;
                        case ID_UNICHR_BYTES:
                        	length=tuple->length;
                        	data=malloc(length);

                        	memcpy(data, tuple->value->data, length);
                        	break;
                        case ID_EXTRA_POS_NUM:
                        	extranum=tuple->value->data[0];
                        	posextra=malloc(tuple->length);
                        	memcpy(posextra,tuple->value->data, tuple->length);
          //              	APP_LOG(APP_LOG_LEVEL_DEBUG, "Sizeof posextra:%u, pos_num:%u, sizeof pos:%u", sizeof(posextra),tuple->value->uint8,sizeof(pos));
                        	break;

                    }
                    tuple=dict_read_next(received);
                }
                if (length>0){
 //               	APP_LOG(APP_LOG_LEVEL_DEBUG, "length:%u, width:%u", length, width);
                	append_bitmap_notifyview(data,length,pos,width);
                	if (extranum>0){
                		for(int i=1;i<= (int) extranum;i++){
                			pos[0]=(15 & posextra[i])+1;
                			pos[1]=(posextra[i]>>4)+1;
                			append_bitmap_notifyview(data,length,pos,width);
                		}
                		free(posextra);
                	}
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

                }
    /*            if (packagenum==1 && pagenum>1){
					hide_notifyview();
					clean_notifyview();
                }
      */
                
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
                	case ID_PAGE_INFO:
                	{
            			uint8_t *packinfo=tuple->value->data;
                        packages=packinfo[2];
                        packagenum=packinfo[3];
                	}
                		break;
                	case ID_UNICHR_BYTES:
                		length=tuple->length;
                		data=malloc(length);
                		memcpy(data,tuple->value->data,length);
                		break;
                	case ID_UNICHR_INFO:
                	{
                    	uint8_t *uniinfo=tuple->value->data;
                    	width=uniinfo[0];
                    	pos[0]=uniinfo[1];
                    	pos[1]=uniinfo[2];
                	}
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
            		send_im_free(NULL);
            		show_callview();
            	}
            }
            break;
            case DISPLAY_MESSAGE_TABLE:
            {
            	uint16_t buffsize;
            	uint8_t packages,packagenum;
            	Tuple *tup=find_tuple_by_id(ID_PAGE_INFO,received);
            	packages=tup->value->data[2];
            	buffsize=packages*MAX_CHARS_PACKAGE_CONTAIN;
            	packagenum=tup->value->data[3];
//            	APP_LOG(APP_LOG_LEVEL_DEBUG, "Message,%d,%u,%u",buffsize,packages,packagenum);
            	show_progress(packagenum*10/packages);
            	init_listview(MESSAGE_LIST,buffsize,send_command_get_msg_by_index,title_layer);
            	tup=find_tuple_by_id(ID_ASCSTR,received);
  //          	APP_LOG(APP_LOG_LEVEL_DEBUG, "append string:\n%s",tup->value->cstring);

            	append_buff_listview(tup->value->cstring);
            	if(packages==packagenum){
            		show_listview();
            	}
            }
            break;
            case DISPLAY_CALL_TABLE:
            {
            	uint16_t buffsize;
            	uint8_t packages,packagenum;
            	Tuple *tup=find_tuple_by_id(ID_PAGE_INFO,received);
            	packages=tup->value->data[2];
            	buffsize=packages*MAX_CHARS_PACKAGE_CONTAIN;
            	packagenum=tup->value->data[3];
            	show_progress(packagenum*10/packages);
            	init_listview(CALL_LIST,buffsize,send_command_get_call_by_index,title_layer);
            	tup=find_tuple_by_id(ID_ASCSTR,received);

            	append_buff_listview(tup->value->cstring);
            	if(packages==packagenum){
            		show_listview();
            	}
            }
            break;
            case DISPLAY_CONTINUE:
            {

            	uint8_t packages,packagenum;
            	Tuple *tup=find_tuple_by_id(ID_PAGE_INFO,received);
            	packages=tup->value->data[2];
            	packagenum=tup->value->data[3];
            	show_progress(packagenum*10/packages);
            	tup=find_tuple_by_id(ID_ASCSTR,received);
  //          	APP_LOG(APP_LOG_LEVEL_DEBUG, "Display_continue,%u,%u/\n String:\n%s",packages,packagenum,tup->value->cstring);

            	append_buff_listview(tup->value->cstring);
            	if(packages==packagenum){
            		show_listview();
            	}
            }
            break;
            case EXCUTE_CALL_END:
  //          	APP_LOG(APP_LOG_LEVEL_DEBUG, "Excute call end.");
            	destroy_callview(NULL);
            break;

            case EXCUTE_CALL_HOOK:
   //         	APP_LOG(APP_LOG_LEVEL_DEBUG, "Excute call hook.");
            	call_hook();
            break;
            case EXCUTE_EMPTY:
            	break;
            case EXCUTE_TEST:
            {

            	send_command_with_str_extra(REQUEST_TRANSID_VERSION,version);
            }
            	break;

		break;
            default:
   //         	APP_LOG(APP_LOG_LEVEL_DEBUG, "Unknown command:%u",tuple->value->uint8);
            break;
        }    

}


void in_dropped_handler(AppMessageResult reason, void *context) {
    // incoming message dropped
	switch(reason){
	case APP_MSG_BUSY:
		APP_LOG(APP_LOG_LEVEL_DEBUG, "There are pending (in or outbound) messages that need to be processed first before new ones can be received or sent.");
		break;
	case APP_MSG_BUFFER_OVERFLOW :
		APP_LOG(APP_LOG_LEVEL_DEBUG, "The buffer was too small to contain the incoming message.");
		break;
	case APP_MSG_OUT_OF_MEMORY:
		APP_LOG(APP_LOG_LEVEL_DEBUG, "The support library did not have sufficient application memory to perform the requested operation");

		break;
	case APP_MSG_CLOSED:
		APP_LOG(APP_LOG_LEVEL_DEBUG, "App message was closed.");

		break;
	case APP_MSG_INTERNAL_ERROR:
		APP_LOG(APP_LOG_LEVEL_DEBUG, "An internal OS error prevented APP_MSG from completing an operation.");

		break;
	default:
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Other error.");

	}
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
    
        text_layer_set_font(firstview.copy_right_layer,fonts_get_system_font(FONT_KEY_GOTHIC_18));
        text_layer_set_text_alignment(firstview.copy_right_layer,GTextAlignmentRight );
        text_layer_set_text(firstview.copy_right_layer, version);
        layer_add_child(firstview.base_layer, text_layer_get_layer(firstview.copy_right_layer));
    }
    
    layer_add_child(parent_layer, firstview.base_layer);
    if (!is_self_close){
    	firstrun_timer=app_timer_register(FIRST_VIEW_DELAY, show_main_menu, NULL);
    }else{
    	init_title();
    }
}

static void destroy_first_view(){
    if(firstview.base_layer!=NULL){
        layer_remove_from_parent(firstview.base_layer);
        text_layer_destroy(firstview.app_name_layer);
        firstview.app_name_layer=NULL;
        text_layer_destroy(firstview.copy_right_layer);
        firstview.copy_right_layer=NULL;
        layer_destroy(firstview.base_layer);
        firstview.base_layer=NULL;
    }
 //   app_timer_cancel(firstrun_timer);
    firstrun_timer=NULL;
    firstview.base_layer=NULL;

}







//--------------main_menu-----------------------------------
void show_main_menu(void *data){
	destroy_first_view();
    if (main_menu==NULL) {
        init_main_menu();
    }
	layer_add_child(window_layer,simple_menu_layer_get_layer(main_menu));
	init_title();
	show_title(window);
}

static void init_main_menu(){

    main_buttons[0] = (SimpleMenuItem){
			.icon=gbitmap_create_with_resource(RESOURCE_ID_IMAGE_MAIN_NOTIFI),
			.subtitle=MAIN_MENU_MESSAGE_SUBTITLE,
			.title=MAIN_MENU_MESSAGE_TITLE,
			.callback=main_menu_onclick
		};
    main_buttons[1] = (SimpleMenuItem){
			.icon=gbitmap_create_with_resource(RESOURCE_ID_IMAGE_MAIN_CALL),
			.subtitle=MAIN_MENU_CALL_SUBTITLE,
			.title=MAIN_MENU_CALL_TITLE,
			.callback=main_menu_onclick
		};
  /*  main_buttons[2] = (SimpleMenuItem){
			.icon=gbitmap_create_with_resource(RESOURCE_ID_IMAGE_MAIN_MUSIC),
			.subtitle=MAIN_MENU_MUSIC_SUBTITLE,
			.title=MAIN_MENU_MUSIC_TITLE
		};
	main_buttons[3] = (SimpleMenuItem){
			.icon=gbitmap_create_with_resource(RESOURCE_ID_IMAGE_MAIN_CAMERA),
			.subtitle=MAIN_MENU_CAMERA_SUBTITLE,
			.title=MAIN_MENU_CAMERA_TITLE
		};
		*/
	main_section=(SimpleMenuSection){
		.items=main_buttons,
		.num_items=2,
		.title=NULL
	};
    main_menu=simple_menu_layer_create(
									   window_bounds,
									   window,
									   &main_section,
                                        1,
									   NULL
									   );
}

static void main_menu_onclick(int index, void *context){
//	APP_LOG(APP_LOG_LEVEL_DEBUG, "Get a command:%d", index);
    switch (index) {
		case MAIN_MENU_MESSAGE_INDEX:
			send_command(REQUEST_TRANSID_MESSAGE_TABLE);
			break;
		case MAIN_MENU_CALL_INDEX:
			send_command(REQUEST_TRANSID_CALL_TABLE);
			break;
/*		case MAIN_MENU_MUSIC_INDEX:
break;
case MAIN_MENU_CAMERA:
break;
 */
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
	if (progressbar.bg!=NULL) return;
	DictionaryIterator *iter=NULL;
	app_message_outbox_begin (&iter);
	dict_write_uint8 (iter, ID_MAIN, REQUEST_TRANSID_IM_FREE);
	dict_write_uint8(iter,ID_EXTRA_DATA,(is_self_close?REQUEST_EXTRA_DELAY_ON:REQUEST_EXTRA_DELAY_OFF));
	app_message_outbox_send();
	app_comm_set_sniff_interval(SNIFF_INTERVAL_NORMAL);

}
static void send_command_with_str_extra(uint8_t command, char *extra){
	DictionaryIterator *iter=NULL;
		app_message_outbox_begin (&iter);
		dict_write_uint8 (iter, ID_MAIN, command);
		dict_write_cstring(iter,ID_EXTRA_DATA, extra);
		app_message_outbox_send();
}
static void send_command_get_msg_by_index(char *index){
	send_command_with_str_extra(REQUEST_TRANSID_MESSAGE, index);
}
static void send_command_get_call_by_index(char *index){
	send_command_with_str_extra(REQUEST_TRANSID_CALL, index);
}

static Tuple* find_tuple_by_id(uint32_t id,  DictionaryIterator *iter){
	Tuple *tuple;
	tuple=dict_read_first(iter);
	while (tuple!=NULL && tuple->key!=id){
		tuple=dict_read_next(iter);
	}
	return tuple;
}

void handle_pebble_disconnect(bool connected){
	if (!connected){
		close_app(NULL);
	}
}
