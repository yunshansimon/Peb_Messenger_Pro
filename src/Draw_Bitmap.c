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
void set_bitmap_to_black(GBitmap *target){
    memset(target->addr, 0, target->row_size_bytes * (uint16_t)((target->bounds).size.h));
}
void draw_data_to_bitmap(int colpix, int rowpix, int width, int size, int scale , GBitmap *bitmap, const uint8_t *source_data){
//	APP_LOG(APP_LOG_LEVEL_DEBUG,"Bitmap addr:%p", bitmap->addr);
	uint8_t *target_data=(bitmap->addr);

    int t_byte_offset,t_bit_offset,s_byte_offset,s_bit_offset;
    uint8_t s_bit=0,t_bit=0;
    for (int row_offset=0;row_offset<size/width;row_offset++){

    	for(int col_offset=0;col_offset<8*width;col_offset++){
            s_byte_offset=row_offset*width+(int)(col_offset/8);
            s_bit_offset=col_offset%8;
            s_bit=source_data[s_byte_offset]>>s_bit_offset & (uint8_t) 1;
            int tmp=colpix+col_offset+(int)(col_offset*scale/2);
            t_byte_offset=(rowpix+row_offset+(int)(row_offset*scale/2))*(bitmap->row_size_bytes)+(int)(tmp/8);
            t_bit_offset=tmp%8;
   //         APP_LOG(APP_LOG_LEVEL_DEBUG,"s_by_off:%d,s_bt_off:%d, s_bit:%d, d_by_off:%d, d_bt_off:%d"
   //         		, s_byte_offset, s_bit_offset, s_bit, t_byte_offset, t_bit_offset);
            if (scale==MESSAGE_SCALE_LARGE){
                if (s_bit){
                    if (t_bit_offset==7){
                    	t_bit=s_bit<<7;
                        target_data[t_byte_offset] |= t_bit;
                        target_data[t_byte_offset+bitmap->row_size_bytes] |= t_bit;
                        t_bit=s_bit;
                        target_data[t_byte_offset+1] |= t_bit;
                        target_data[t_byte_offset+bitmap->row_size_bytes+1] |= t_bit;
                    }else{
                        t_bit=(uint8_t) 3<<(t_bit_offset);
                        target_data[t_byte_offset] |= t_bit;
                        target_data[t_byte_offset+bitmap->row_size_bytes] |= t_bit;
                    }
                }
            }else if(scale==MESSAGE_SCALE_MID ){

				if (s_bit){
					if (t_bit_offset==7){
						t_bit=s_bit<<7;
						target_data[t_byte_offset] |= t_bit;
						if(row_offset%2>0) target_data[t_byte_offset+bitmap->row_size_bytes] |= t_bit;
						t_bit=s_bit;
						if(col_offset%2>0) target_data[t_byte_offset+1] |= t_bit;
						if(row_offset%2>0) target_data[t_byte_offset+bitmap->row_size_bytes+1] |= t_bit;
					}else{
						t_bit=(uint8_t) 1<<(t_bit_offset);
						target_data[t_byte_offset] |= t_bit;
						if(row_offset%2>0) target_data[t_byte_offset+bitmap->row_size_bytes] |= t_bit;
						if(col_offset%2>0) target_data[t_byte_offset] |= (t_bit<<1);
						if(row_offset%2>0) target_data[t_byte_offset+bitmap->row_size_bytes] |= (t_bit<<1);
					}
				}

            }else{
                if (s_bit){
                    t_bit=s_bit<<t_bit_offset;
                    target_data[t_byte_offset] |= t_bit;
                }

            }
        }
    }
}

void draw_data_mid(int colpix, int rowpix, int width, int size,GBitmap *bitmap, const uint8_t *data){
	uint8_t sou[2][2],des[3][3];
	uint8_t *target_data=(bitmap->addr);
	int s_byte_offset,s_bit_offset,t_byte_offset,t_bit_offset;
	for (uint8_t sou_row=0;sou_row<size/width;sou_row+=2){
		for(uint8_t sou_col=0;sou_col<8*width;sou_col+=2){
			s_byte_offset=sou_row*width+sou_col/8;
			s_bit_offset=sou_col%8;
			sou[0][0]=data[s_byte_offset]>>s_bit_offset & (uint8_t) 1;
			sou[0][1]=data[s_byte_offset]>>(s_bit_offset+1) & (uint8_t) 1;
			sou[1][0]=data[s_byte_offset+width]>>s_bit_offset & (uint8_t) 1;
			sou[1][1]=data[s_byte_offset+width]>>(s_bit_offset+1) & (uint8_t) 1;
			des[0][0]=sou[0][0];
			des[0][1]=sou[0][0];
			des[0][2]=sou[0][1];
			des[2][0]=sou[1][0];
			des[2][1]=sou[1][0];
			des[2][2]=sou[1][1];
			des[1][0]=sou[0][0];
			des[1][1]=des[0][1];
			des[1][2]=des[0][2];
            int tmp=colpix+sou_col/2*3;
			t_byte_offset=(rowpix+sou_row/2*3)*(bitmap->row_size_bytes);
			t_bit_offset=tmp%8;
			for (uint8_t des_row=0;des_row<3;des_row++){
				for (uint8_t des_col=0;des_col<3;des_col++){
					if (des[des_row][des_col]){
						t_bit_offset=(tmp+des_col)%8;
						target_data[t_byte_offset+des_row*(bitmap->row_size_bytes)+(tmp+des_col)/8] |=(des[des_row][des_col]<<t_bit_offset);
					}
				}
			}
		}
	}
}

