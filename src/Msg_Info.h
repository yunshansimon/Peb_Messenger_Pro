/*
 * Msg_Info.h
 *
 *  Created on: Sep 14, 2014
 *      Author: yunshan
 */

#ifndef MSG_INFO_H_
#define MSG_INFO_H_
	typedef struct {
		Layer *base_layer;
		TextLayer *title_layer;
		TextLayer *msg_layer;
		} MsgView;
	void display_message(Layer *baseLayer ,const char *title_str, const char *body_str, const int sec);
	void destory_message(void *data);

#endif /* MSG_INFO_H_ */
