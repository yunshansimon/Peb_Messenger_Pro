/*
 * Draw_Bitmap.h
 *
 *  Created on: 2014-9-10
 *      Author: yunshansimon
 */

#ifndef DRAW_BITMAP_H_
#define DRAW_BITMAP_H_
	void set_bitmap_to_black(const GBitmap *target);
	void draw_data_to_bitmap(int colpix, int rowpix, int width, int size, int scale , const GBitmap *bitmap, const uint8_t *data);

#endif /* DRAW_BITMAP_H_ */
