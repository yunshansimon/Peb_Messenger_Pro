/*
 * Table.h
 *
 *  Created on: 2014年9月18日
 *      Author: yunshansimon
 */

#ifndef TABLE_H_
#define TABLE_H_
typedef struct row_link *link;
typedef struct row_link{
	char *index;
	char *title;
	char *time;
	char *icon;
	link pre_row;
	link next_row;
}Row;

//create a new row structure, the parent link will be set. NULL is allowed.
Row* new_row(Row *pre);
//provide target row, and the first char point at the begin of the line, return filled target.
//the link will never changed. The line will split by \0.
//return the next line begin point Or NULL if no other string.
char* set_row(Row *target, char *index);
//connect the new row to present row. Return next row.
Row* connect_next_row(Row *base, Row *next);

char* get_index(Row *base);
char* get_title(Row *base);
char* get_time(Row *base);
char* get_icon(Row *base);

Row* get_next_row(Row *base);
Row* get_pre_row(Row *base);

//remove this row and all rows after this row.
//if the row has preview row, the preview row's next_row will set to NULL.
void remove_rows(Row *base);

Row *get_row_by_index(Row *first_row, uint16_t index);

#endif /* TABLE_H_ */
