/*
 * Call_View.h
 *
 *  Created on: Sep 15, 2014
 *      Author: yunshan
 */

#ifndef CALL_VIEW_H_
#define CALL_VIEW_H_
#define TITLE_INFO "Incoming call"
typedef struct{
	char *phonenum;
	char *name;
	TextLayer *name_text_layer;
	BitmapLayer *name_bitmap_layer;
	TextLayer *phone_text_layer;



}CallView;


#endif /* CALL_VIEW_H_ */
