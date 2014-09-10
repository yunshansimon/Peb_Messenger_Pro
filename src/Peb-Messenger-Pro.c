#include <pebble.h>
#include "Constants.h"
#include "Peb-Messenger-Pro.h"


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
static bool is_white_background;

	//views
NotifyView notifyview;
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
	app_message_deregister_callbacks();
    
}

static void init(void) {
    window = window_create();
    //init communicat system
	is_self_close=true;
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
                uint32_t delay=20000;
                int8_t scale=0;
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
                init_notifyview(scale, delay, id);       
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
                            notifyview.pages=pages;
                        break;
                        case ID_PAGE_NUM:
                            pagenum=tuple->value->uint8;
                            notifyview.pagenum=pagenum;
                        break;
                        case ID_TOTAL_PACKAGES:
                            packages=tuple->value->uint8;
                        break;
                        case ID_PACKAGE_NUM:
                            packagenum=tuple->value->uint8;
                        if (packagenum==1) {
                            notifyview.ascii_buff[0]='\0';
                            set_bitmap_to_black(bitmap_layer_get_bitmap(notifyview.unicode_layer));
                        }
                        break;
                        case ID_ASCSTR:
                        {
                            strcat(notifyview.ascii_buff,tuple->value->cstring);
                            APP_LOG(APP_LOG_LEVEL_DEBUG, "Get string:%s", tuple->value->cstring);
                        }
                        break;
                        case ID_UNICHR_WIDTH:
                        {
                            int colpix,rowpix,width;
                            width=(int) tuple->value->uint8;
                            tuple=dict_read_next(received);
                            colpix=((int) tuple->value->data[1]-1)* notifyview.col_scale;
                            rowpix=((int) tuple->value->data[0]-(notifyview.pagenum-1)*notifyview.page_rows-1)* notifyview.row_scale;
                            tuple=dict_read_next(received);
                            draw_data_to_bitmap(colpix, rowpix, width, tuple->length, notifyview.char_scale ,bitmap_layer_get_bitmap(notifyview.unicode_layer), tuple->value->data);
                        }
                        break;
                    }
                }
                if (packages==packagenum){
                    //show the message
                    
                    show_notifyview();
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
    
    app_timer_register(FIRST_VIEW_DELAY, destory_first_view, NULL);                    
}

static void destory_first_view(void *data){
    if(firstview.base_layer!=NULL){
        layer_remove_from_parent(firstview.base_layer);
        text_layer_destroy(firstview.app_name_layer);
        text_layer_destroy(firstview.copy_right_layer);
        layer_destroy(firstview.base_layer);
    }
    firstview.base_layer=NULL;
    show_main_menu(window_layer);
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
			break;
		case MAIN_MENU_CALL_INDEX:
			break;
		case MAIN_MENU_MUSIC_INDEX:
			break;
		case MAIN_MENU_CAMERA:
			break;
		default:
			break;
	}
}
//---------------notify_layer--------------------------------

static void init_notifyview(uint8_t char_scale, uint32_t notify_delay, uint32_t id){
	if (notifyview.base_window==NULL) {
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Init notifyview");
        notifyview=(NotifyView){
            .base_window=window_create(),
            .base_layer=window_get_root_layer(notifyview.base_window),
            .frame=window_bounds,
            .unicode_layer=bitmap_layer_create(window_bounds),
            .ascii_layer= text_layer_create(window_bounds),
            .notify_delay=notify_delay,
            .char_scale=char_scale,
            .pages=1,
            .pagenum=1,
            .id=id
		};
        
        switch(char_scale){
            case MESSAGE_SCALE_SMALL:
                notifyview.row_scale=CHAR_SMALL_HEIGHT_BIT;
                notifyview.col_scale=CHAR_SMALL_WIDTH_BIT;
                notifyview.page_rows=SMALL_LINES;
            break;
            case MESSAGE_SCALE_MID:
                notifyview.row_scale=CHAR_MID_HEIGHT_BIT;
                notifyview.col_scale=CHAR_MID_WIDTH_BIT;
                notifyview.page_rows=MID_LINES;
            break;
            case MESSAGE_SCALE_LARGE:
                notifyview.row_scale=CHAR_LARGE_HEIGHT_BIT;
                notifyview.col_scale=CHAR_LARGE_WIDTH_BIT;
                notifyview.page_rows=LARGE_LINES;
            break;
        }
		text_layer_set_background_color(notifyview.ascii_layer,GColorBlack);
		text_layer_set_text_color(notifyview.ascii_layer,GColorWhite);
        
		
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Append text and bitmap layer");
		if (is_white_background) {
            notifyview.invert_white_layer= inverter_layer_create(window_bounds);			
		}
				
	}
    window_set_click_config_provider (notifyview.base_window,handle_notify_click);
	
}
static void show_notifyview(){
    if (window_stack_contains_window(notifyview.base_window)){
        window_stack_remove(notifyview.base_window, false);
    }
    text_layer_set_text(notifyview.ascii_layer, notifyview.ascii_buff);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "show the notify");
    window_stack_push(notifyview.base_window, true);
    layer_add_child	(notifyview.base_layer,text_layer_get_layer(notifyview.ascii_layer));
	layer_add_child(notifyview.base_layer,bitmap_layer_get_layer(notifyview.unicode_layer));
    
    if (is_white_background){
        layer_add_child(notifyview.base_layer,inverter_layer_get_layer(notifyview.invert_white_layer));
    }
}

static void destory_notify_layer(){
	if (notifyview.base_window!=NULL) {
		window_stack_remove(notifyview.base_window, false);
		if (notifyview.invert_white_layer!=NULL) {
			inverter_layer_destroy(notifyview.invert_white_layer);
		}
		bitmap_layer_destroy(notifyview.unicode_layer);
		text_layer_destroy(notifyview.ascii_layer);
		window_destroy(notifyview.base_window);
	}
	notifyview.base_window= NULL;
}

static void set_bitmap_to_black( const GBitmap *target){
    memset(target->addr, 0, target->row_size_bytes * (uint16_t)((target->bounds).size.h));
}

static void handle_notify_click(void * context){
     window_single_click_subscribe (BUTTON_ID_SELECT, read_notify);
     window_single_click_subscribe (BUTTON_ID_DOWN, next_notify_page);
}

static void next_notify_page(){
    DictionaryIterator *iter=NULL;
    app_message_outbox_begin (&iter);
    dict_write_uint8 (iter, ID_MAIN, REQUEST_TRANSID_MESSAGE);
    app_message_outbox_send();
}

static void read_notify(){
    DictionaryIterator *iter=NULL;
    app_message_outbox_begin(&iter);
    dict_write_uint8 (iter, (uint32_t)ID_MAIN, (uint8_t) REQUEST_TRANSID_READ_NOTIFY);
    app_message_outbox_send();
}
//-------------main_draw_function---------------------------
static void draw_data_to_bitmap(int colpix, int rowpix, int width, int size, int scale , const GBitmap *bitmap, const uint8_t *source_data){
    uint8_t *target_data=bitmap->addr;
    int t_byte_offset,t_bit_offset,s_byte_offset,s_bit_offset;
    uint8_t s_bit=0,t_bit=0;
    for (int row_offset=0;row_offset<size/width;row_offset++){
        for(int col_offset=0;col_offset<8*width;col_offset++){
            s_byte_offset=row_offset*width+col_offset/8;
            s_bit_offset=8*width-1-col_offset;
            s_bit=source_data[s_byte_offset]>>s_bit_offset & (uint8_t) 1;
            int tmp=colpix+col_offset+(int) (col_offset*scale/2);
            t_byte_offset=(rowpix+row_offset+row_offset*scale/2)*(bitmap->row_size_bytes)+(int)(tmp/8);
            t_bit_offset=7-(tmp-((int)(tmp/8))*8);
            if ((scale==MESSAGE_SCALE_MID && (col_offset/2)>(int) (col_offset/2)) || scale==MESSAGE_SCALE_LARGE){
                if (s_bit){
                    if (t_bit_offset==0){
                        t_bit=s_bit;
                        target_data[t_byte_offset] |= t_bit;
                        target_data[t_byte_offset+bitmap->row_size_bytes] |= t_bit;
                        t_bit=s_bit<<7;
                        target_data[t_byte_offset+1] |= t_bit;
                        target_data[t_byte_offset+bitmap->row_size_bytes+1] |= t_bit;
                    }else{
                        t_bit=(uint8_t) 3<<(t_bit_offset-1);
                        target_data[t_byte_offset] |= t_bit;
                        target_data[t_byte_offset+bitmap->row_size_bytes] |= t_bit;
                    }
                }else{
                    if(t_bit_offset==0){
                        t_bit=0xff -1;
                        target_data[t_byte_offset] &= t_bit;
                        target_data[t_byte_offset+bitmap->row_size_bytes] &= t_bit;
                        t_bit=(uint8_t)0xff<<1>>1;
                        target_data[t_byte_offset+1] &= t_bit;
                        target_data[t_byte_offset+bitmap->row_size_bytes+1] &= t_bit;
                    }else{
                        t_bit=(uint8_t) 3<<(t_bit_offset-1);
                        t_bit &=target_data[t_byte_offset];
                        target_data[t_byte_offset] ^=t_bit;
                        t_bit &=target_data[t_byte_offset+bitmap->row_size_bytes];
                        target_data[t_byte_offset+bitmap->row_size_bytes] ^=t_bit;
                    }
                }
            }else{
                if (s_bit){
                    t_bit=s_bit<<t_bit_offset;
                    target_data[t_byte_offset] |= t_bit;
                }else{
                    t_bit=(uint8_t) 1<<t_bit_offset;
                    t_bit &= target_data[t_byte_offset];
                    target_data[t_byte_offset] ^=t_bit;
                }
            }
        }
    }
    
}
