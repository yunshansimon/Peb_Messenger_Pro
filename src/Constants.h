//
//  Constants.h
//  
//
//  Created by 曹 阳 on 14-6-25.
//
//

#ifndef _Constants_h
	#define _Constants_h
	#define APP_NAME "Pebble\nMessenger\nPro"
	#define APP_AUTHOR "Yang Tsao Software"

	#define FIRST_VIEW_DELAY 3000

	//Define the message to communicate the phone app
	#define ID_COMMAND          0
	#define EXCUTE_NEW_MESSAGE      1
	#define EXCUTE_NEW_CALL         2
	#define EXCUTE_CONTINUE_MESSAGE 3
	#define EXCUTE_CONTINUE_CALL    4
	#define DISPLAY_MESSAGE_TABLE   5
	#define DISPLAY_CALL_TABLE      6
	#define EXCUTE_CALL_END         7
	#define DISPLAY_CONTINUE        8
	#define ID_TOTAL_PAGES      1
	#define ID_PAGE_NUM         2
	#define ID_TOTAL_PACKAGES   3
	#define ID_PACKAGE_NUM      4
	#define ID_ASCSTR           5
	#define ID_UNICHR_BYTES     6
	#define ID_UNICHR_WIDTH     7
	#define ID_UNICHR_POS       8
	#define ID_CLOSE_DELAY_SEC  9
	#define ID_CHAR_SCALE       10
	#define ID_INFO_ID          11
	#define MESSAGE_SCALE_SMALL 0
	#define MESSAGE_SCALE_MID   1
	#define MESSAGE_SCALE_LARGE 2


	//Define remote control phone by transaction id
	#define ID_MAIN    0
	#define REQUEST_TRANSID_MESSAGE     1
	#define REQUEST_TRANSID_CALL        2
	#define REQUEST_TRANSID_MESSAGE_TABLE   3
	#define REQUEST_TRANSID_CALL_TABLE  4
	#define REQUEST_TRANSID_CLOSE_APP   5
	#define REQUEST_TRANSID_PICKUP_PHONE    6
	#define REQUEST_TRANSID_HANGOFF_PHONE   7
	#define REQUEST_TRANSID_HANGOFF_SMS1    8
	#define REQUEST_TRANSID_HANGOFF_SMS2    9
	#define REQUEST_TRANSID_NEXTPAGE        10
	#define REQUEST_TRANSID_READ_NOTIFY     11
	#define ID_EXTRA_DATA   2
	#define REQUEST_EXTRA_SPEAKER_ON    1
	#define REQUEST_EXTRA_SPEAKER_OFF    2


	//Defind char bit length
	#define CHAR_MAX_BUFF 200
	#define CHAR_SMALL_WIDTH_BIT 9
	#define CHAR_SMALL_HEIGHT_BIT 16
	#define CHAR_MID_WIDTH_BIT 12
	#define CHAR_MID_HEIGHT_BIT 24
	#define CHAR_LARGE_WIDTH_BIT 16
	#define CHAR_LARGE_HEIGHT_BIT 32
	#define SMALL_LINES 9
	#define MID_LINES 8
	#define LARGE_LINES 6


	//define main_menu_index
	#define MAIN_MENU_MESSAGE_INDEX 0
	#define MAIN_MENU_CALL_INDEX 1
	#define MAIN_MENU_MUSIC_INDEX 2
	#define MAIN_MENU_CAMERA 3

		//define main_menu_title and subtitle
	#define MAIN_MENU_MESSAGE_TITLE			"Messages"
	#define MAIN_MENU_MESSAGE_SUBTITLE		"Message's history"
	#define MAIN_MENU_CALL_TITLE			"Calls"
	#define MAIN_MENU_CALL_SUBTITLE			"Incoming call's history"
	#define MAIN_MENU_MUSIC_TITLE			"Music"
	#define MAIN_MENU_MUSIC_SUBTITLE		"Play music"
	#define MAIN_MENU_CAMERA_TITLE			"Camera"
	#define MAIN_MENU_CAMERA_SUBTITLE		"Take a shot"

	#define INDICATOR_STR "----------"

#endif
