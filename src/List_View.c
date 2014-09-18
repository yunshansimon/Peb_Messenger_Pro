/*
 * List_View.c
 *
 *  Created on: 2014年9月18日
 *      Author: yunshansimon
 */

#include <pebble.h>
#include "List_View.h"
#include "Table.h"
#include "Constants.h"

ListView listview;

void init_listview(enum ListType listtype, uint16_t buffsize, void(*get_content)(char *index)){
	if(listview.base_window==NULL){
		listview.listtype=listtype;
		listview.base_window=window_create();
		window_set_click_config_provider(listview.base_window,listview_window_click_provider);
		listview.list_menu_layer=menu_layer_create(layer_get_frame(window_get_root_layer(listview.base_window)));
		listview.first_row=malloc(sizeof(Row));
	}else{
		free(listview.buff);
		remove_rows(listview.first_row);
		gbitmap_destroy(listview.new_icon);
		listview.new_icon=NULL;
		gbitmap_destroy(listview.old_icon);
		listview.old_icon=NULL;
	}
	listview.buff=malloc(buffsize);
	listview.buff[0]='\0';
	if(listview.listtype==MESSAGE_LIST){
		listview.new_icon=gbitmap_create_with_resource(RESOURCE_ID_IMAGE_NEW_MSG);
		listview.old_icon=gbitmap_create_with_resource(RESOURCE_ID_IMAGE_OLD_MSG);
	}else{
		listview.new_icon=gbitmap_create_with_resource(RESOURCE_ID_IMAGE_NEW_CALL);
		listview.new_icon=gbitmap_create_with_resource(RESOURCE_ID_IMAGE_OLD_CALL);
	}
	listview.get_content=get_content;
	listview.rows_num=0;
}

void destroy_listview(){
	if(listview.base_window!=NULL){
		window_stack_pop(true);
		listview.rows_num=0;
		listview.get_content=NULL;
		remove_rows(listview.first_row);
		gbitmap_destroy(listview.new_icon);
		listview.new_icon=NULL;
		gbitmap_destroy(listview.old_icon);
		listview.old_icon=NULL;
		remove_rows(listview.first_row);
		free(listview.buff);
		listview.buff=NULL;
		menu_layer_destroy(listview.list_menu_layer);
		listview.list_menu_layer=NULL;
		window_destroy(listview.base_window);
		listview.base_window=NULL;
	}
}

void append_buff_listview(char *newstr){
	strcat(listview.buff,newstr);
}
void show_listview(){
	if(listview.base_window==NULL) return;
	split_buff_to_rows(listview.first_row,listview.buff);
	menu_layer_set_callbacks(listview.list_menu_layer,NULL,
			(MenuLayerCallbacks){
		.draw_row=draw_row,
		.get_num_rows=get_num_rows,
		.select_click=select_click,
	});
	window_stack_push(listview.base_window,true);
	layer_add_child(window_get_root_layer(listview.base_window),menu_layer_get_layer(listview.list_menu_layer));
	menu_layer_set_click_config_onto_window(listview.list_menu_layer,listview.base_window);
}

static void draw_row(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *callback_context){
	Row *cur_row=get_row_by_index(listview.first_row,cell_index->row);
	menu_cell_basic_draw(ctx,cell_layer,cur_row->title,cur_row->time,
			(cur_row->icon[0]=='!')?listview.new_icon:listview.old_icon);
}

static uint16_t get_num_rows(struct MenuLayer *menu_layer, uint16_t section_index, void *callback_context){
	return listview.rows_num;
}

static void select_click(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context){
	Row *cur_row=get_row_by_index(listview.first_row,cell_index->row);
	listview.get_content(cur_row->index);
}

static void split_buff_to_rows(Row *begin,char *buff){
	if (begin==NULL || buff==NULL) return;
	char *prt;
	Row *cur_row,*next_row;
	cur_row=begin;
	prt=buff;
	listview.rows_num=0;
	while(*prt!='\0'){
		prt=set_row(cur_row,prt);
		listview.rows_num++;
		if(prt!='\0'){
			next_row=new_row(cur_row);
			cur_row->next_row=next_row;
			cur_row=next_row;
		}
	};
}
static void listview_window_click_provider(void *context){
	window_single_click_subscribe(BUTTON_ID_BACK,handler_click_back);
}
static void handler_click_back(ClickRecognizerRef recognizer, void *context){
	destroy_listview();
}
