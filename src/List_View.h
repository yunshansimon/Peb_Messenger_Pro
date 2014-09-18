/*
 * List_View.h
 *
 *  Created on: 2014年9月18日
 *      Author: yunshansimon
 */
#include "Table.h"
#ifndef LIST_VIEW_H_
#define LIST_VIEW_H_

enum ListType{MESSAGE_LIST,CALL_LIST};
typedef struct{
	enum ListType listtype;
	Window *base_window;
	MenuLayer *list_menu_layer;
	char *buff;
	Row *first_row;
	GBitmap *old_icon;
	GBitmap *new_icon;
	void (*get_content)(char *index);
	uint16_t rows_num;
}ListView;

void init_listview(enum ListType listtype, uint16_t buffsize, void(*get_content)(char *index));
void append_buff_listview(char *newstr);
void show_listview();
void destroy_listview();

static void split_buff_to_rows(Row *begin,char *buff);
static void draw_row(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *callback_context);
static uint16_t get_num_rows(struct MenuLayer *menu_layer, uint16_t section_index, void *callback_context);
static void select_click(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context);

static void listview_window_click_provider(void *context);
static void handler_click_back(ClickRecognizerRef recognizer, void *context);
#endif /* LIST_VIEW_H_ */
