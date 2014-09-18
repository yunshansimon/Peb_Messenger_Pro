/*
 * Table.c
 *
 *  Created on: 2014年9月18日
 *      Author: yunshansimon
 */
#include <pebble.h>
#include "Table.h"

Row* new_row(Row *pre){
	Row *new=malloc(sizeof(Row));
	new->pre_row=pre;
	return new;
}

char* set_row(Row *target, char *index){
	target->index=index;
	while(*index!='|'){
		index++;
	}
	*index='\0';
	target->time=index+1;
	while(*index!='|'){
			index++;
		}
	*index='\0';
	target->title=index+1;
	while(*index!='|'){
		index++;
	}
	*index='\0';
	target->icon=index+1;
	while(*index!='\n'){
			index++;
		}
	*index='\0';
	return index++;
}

Row* connect_next_row(Row *base, Row *next){
	if (base==NULL) return NULL;
	if (next==NULL) return NULL;
	base->next_row=next;
	return next;
}

char* get_index(Row *base){
	return base->index;
}

char* get_title(Row *base){
	return base->title;
}

char* get_time(Row *base){
	return base->time;
}

char* get_icon(Row *base){
	return base->icon;
}

Row* get_next_row(Row *base){
	return base->next_row;
}

Row* get_pre_row(Row *base){
	return base->pre_row;
}

void remove_rows(Row *base){
	if(base==NULL) return;
	if(base->pre_row!=NULL) base->pre_row->next_row=NULL;
	Row *next=base->next_row;
	free(base);
	base=NULL;
	if(next!=NULL) next->pre_row=NULL;
	remove_rows(next);
}

Row *get_row_by_index(Row *first_row, uint16_t index){
	Row *cur_row=first_row;
	for (uint16_t i=1;i<=index;i++){
		cur_row=cur_row->next_row;
	}
	return cur_row;
}

