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
static void init_notifyview(uint8_t char_scale, uint32_t notify_delay, uint32_t id);
static void show_notifyview();
static void destory_notifyview();
static void set_bitmap_to_black(const GBitmap *target);
static void main_menu_onclick(int index, void *context);
static void draw_data_to_bitmap(int colpix, int rowpix, int width, int size, int scale ,const GBitmap *bitmap, const uint8_t *data);
static void read_notify();
static void next_notify_page();
static void handle_notify_click(void *context);


typedef struct {
	Window *base_window;
    Layer *base_layer;
	GRect frame;
	BitmapLayer *unicode_layer;
	TextLayer *ascii_layer;
    uint32_t notify_delay;
    uint32_t id;
    uint8_t row_scale;
    uint8_t col_scale;
    uint8_t char_scale;
    uint8_t page_rows;
	InverterLayer *invert_white_layer;
    uint8_t pages;
    uint8_t pagenum;
    char ascii_buff[200];
} NotifyView;

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
