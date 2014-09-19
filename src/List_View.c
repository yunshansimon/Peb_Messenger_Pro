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

void init_listview(enum ListType listtype, uint16_t buffsize, void(*get_content)(char *index),TextLayer *txt_layer){
	if(listview.base_window==NULL){
		listview.listtype=listtype;
		listview.base_window=window_create();
		window_set_fullscreen(listview.base_window,true);
		GRect fb=layer_get_frame(window_get_root_layer(listview.base_window));
		GRect wb=GRect (0,16,fb.size.w,fb.size.h-16);
		window_set_window_handlers(listview.base_window,(WindowHandlers){
			.unload=list_view_close
		});
		listview.list_menu_layer=menu_layer_create(wb);
		listview.first_row=new_row(NULL);
		listview.title_layer=txt_layer;
	}else{
		free(listview.buff);
		remove_rows(listview.first_row);
		gbitmap_destroy(listview.new_icon);
		listview.new_icon=NULL;
		gbitmap_destroy(listview.old_icon);
		listview.old_icon=NULL;
	}
	if(listview.listtype==MESSAGE_LIST){
		listview.new_icon=gbitmap_create_with_resource(RESOURCE_ID_IMAGE_NEW_MSG);
		listview.old_icon=gbitmap_create_with_resource(RESOURCE_ID_IMAGE_OLD_MSG);
	}else{
		listview.new_icon=gbitmap_create_with_resource(RESOURCE_ID_IMAGE_NEW_CALL);
		listview.new_icon=gbitmap_create_with_resource(RESOURCE_ID_IMAGE_OLD_CALL);
	}
	listview.get_content=get_content;
	listview.rows_num=0;
	listview.buff=malloc(buffsize);
	listview.buff[0]='\0';
//	APP_LOG(APP_LOG_LEVEL_DEBUG, "Buff begin point:%p", listview.buff);
}

void destroy_listview(){
//	APP_LOG(APP_LOG_LEVEL_DEBUG, "Close listview.");

	if(listview.base_window!=NULL){
		listview.rows_num=0;
		listview.get_content=NULL;
		gbitmap_destroy(listview.new_icon);
		listview.new_icon=NULL;
		gbitmap_destroy(listview.old_icon);
		listview.old_icon=NULL;

		free(listview.buff);
		listview.buff=NULL;
		listview.title_layer=NULL;
	//	listview.buff[0]='\0';
		menu_layer_destroy(listview.list_menu_layer);
		listview.list_menu_layer=NULL;
		remove_rows(listview.first_row);
		window_destroy(listview.base_window);

		listview.base_window=NULL;
	}
}

void append_buff_listview(const char *newstr){
//	APP_LOG(APP_LOG_LEVEL_DEBUG, "Newstr length:%u",strlen(newstr));
	strcat(listview.buff,newstr);
//	APP_LOG(APP_LOG_LEVEL_DEBUG, "Buff point:%p", listview.buff);
//	APP_LOG(APP_LOG_LEVEL_DEBUG, "Buff length:%u",strlen(listview.buff));
}
void show_listview(){
	if(listview.base_window==NULL) return;
//	APP_LOG(APP_LOG_LEVEL_DEBUG, "All string:\n%s\n length:%u",listview.buff ,strlen(listview.buff));

	split_buff_to_rows(listview.first_row,listview.buff);
//	APP_LOG(APP_LOG_LEVEL_DEBUG, "First row:%s,%s,%s",listview.first_row->index,listview.first_row->title,listview.first_row->time);

	menu_layer_set_callbacks(listview.list_menu_layer,NULL,
			(MenuLayerCallbacks){
		.draw_row=draw_row,
		.get_num_rows=get_num_rows,
		.select_click=select_click,
	});
	window_stack_push(listview.base_window,true);
	layer_add_child(window_get_root_layer(listview.base_window),text_layer_get_layer(listview.title_layer));
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
	Row *currow,*nextrow;
	currow=begin;
	prt=buff;
	listview.rows_num=0;

	while(*prt!='\0'){
//		APP_LOG(APP_LOG_LEVEL_DEBUG, "Source:\n%s", prt);
		prt=set_row(currow,prt);
//		APP_LOG(APP_LOG_LEVEL_DEBUG, "%u row:%s,%s,%s,%s",listview.rows_num,currow->index,currow->title,currow->time,currow->icon);
		listview.rows_num++;
		if(prt!='\0'){
			nextrow=new_row(currow);
			currow->next_row=nextrow;
			currow=nextrow;
		}
	}
//	APP_LOG(APP_LOG_LEVEL_DEBUG, "There %u rows.", listview.rows_num);
//	APP_LOG(APP_LOG_LEVEL_DEBUG, "Buff end point:%p", listview.buff);

}

static void list_view_close(Window *window){
	destroy_listview();
}

