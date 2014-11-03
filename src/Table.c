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
	new->next_row=NULL;
	return new;
}

char* set_row(Row *target,char *index){
	char *tmpindex;
	tmpindex=index;
	target->index=tmpindex;
	while(*tmpindex!='|'){
		tmpindex++;
	}
	*tmpindex='\0';
	target->passtime=tmpindex+1;
	while(*tmpindex!='|'){
			tmpindex++;
		}
	*tmpindex='\0';
	target->title=tmpindex+1;
	while(*tmpindex!='|'){
		tmpindex++;
	}
	*tmpindex='\0';
	target->icon=tmpindex+1;
	while(*tmpindex!='\n'){
			tmpindex++;
		}
	*tmpindex='\0';
	return tmpindex+1;
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

char* get_passtime(Row *base){
	return base->passtime;
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
	Row *next=base->next_row;
//	APP_LOG(APP_LOG_LEVEL_DEBUG, "Remove one row:%p.next:%p",base,next);
	free(base);
	base=NULL;
	remove_rows(next);
}

Row *get_row_by_index(Row *first_row, uint16_t index){
	Row *cur_row=first_row;
	for (uint16_t i=1;i<=index;i++){
		cur_row=cur_row->next_row;
	}
	return cur_row;
}

