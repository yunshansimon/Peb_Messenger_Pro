/*
 * Msg_Info.c
 *
 *  Created on: Sep 14, 2014
 *      Author: yunshan
 */
#include <pebble.h>
#include "Msg_Info.h"

static MsgView msgview;
//--------------system_message_view------------------------
void destory_message(void *data) {
    if (msgview.base_layer != NULL){
        text_layer_destroy(msgview.title_layer);
        text_layer_destroy(msgview.msg_layer);
    }
    msgview.base_layer=NULL;
}

void display_message(Layer *baselayer , const char *title_str, const char *body_str, int sec) {
    if (msgview.base_layer == NULL){
        msgview.base_layer= baselayer;

        msgview.title_layer=text_layer_create(GRect(0,0,144,30));
        text_layer_set_font(msgview.title_layer,fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
        text_layer_set_text_alignment(msgview.title_layer, GTextAlignmentCenter);
        text_layer_set_text_color(msgview.title_layer, GColorWhite);
        text_layer_set_background_color(msgview.title_layer, GColorBlack);
        text_layer_set_text(msgview.title_layer, title_str);

        msgview.msg_layer=text_layer_create(GRect(0,30,144,138));
        text_layer_set_font(msgview.msg_layer,fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
        text_layer_set_text_alignment(msgview.msg_layer, GTextAlignmentLeft);
        text_layer_set_text(msgview.msg_layer, body_str);
    }
    layer_add_child(msgview.base_layer, text_layer_get_layer(msgview.title_layer));
    layer_add_child(msgview.base_layer, text_layer_get_layer(msgview.msg_layer));
    vibes_short_pulse();
    if (sec>0){
        app_timer_register((uint32_t) sec*1000, destory_message, NULL);
    }
}
