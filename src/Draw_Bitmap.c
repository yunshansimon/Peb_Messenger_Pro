/*
 * Draw_Bitmap.c
 *
 *  Created on: 2014-9-10
 *      Author: yunshansimon
 */
#include <pebble.h>
#include "Constants.h"
#include "Draw_Bitmap.h"

//-------------main_draw_function---------------------------
void set_bitmap_to_black(const GBitmap *target){
    memset(target->addr, 0, target->row_size_bytes * (uint16_t)((target->bounds).size.h));
}
void draw_data_to_bitmap(int colpix, int rowpix, int width, int size, int scale , const GBitmap *bitmap, const uint8_t *source_data){
	APP_LOG(APP_LOG_LEVEL_DEBUG,"Bitmap addr:%p", bitmap->addr);
	uint8_t *target_data=(bitmap->addr);

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
 //           APP_LOG(APP_LOG_LEVEL_DEBUG,"s_by_off:%d,s_bt_off:%d, s_bit:%d, d_by_off:%d, d_bt_off:%d"
 //           		, s_byte_offset, s_bit_offset, s_bit, t_byte_offset, t_bit_offset);
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
