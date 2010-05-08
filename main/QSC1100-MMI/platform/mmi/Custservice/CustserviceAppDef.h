#ifndef _MORSE_APP_DEF_H_
#define _MORSE_APP_DEF_H_
/*******************************************************************
*	Copyright(c)2008-2009 TCL CDMA SW developement Team Reserved
*
********************************************************************/
/********************************************************************

	Team		: 	CDMA JRD SW
	Author		: 	Qiong.chen Create 
	Version 	:	1.0.0
	Function Name 	:
	Change Time 	:
		Qiong.chen Create 2008/12/18
		
*********************************************************************/
//#include "Morse_priv.h"

/*max count of list for morse code*/
#define MAX_MORSE_CODE_COUNT  20

/*max char buffer*/
#define MAX_SPECIAL_CHAR_BUF  3

/*max length of morse code for letter*/
#define MAX_MORSE_CODE_LEN 5

/*count of char*/
#define MAX_CHAR_COUNT 27

typedef struct
{
    char  letter;
    char  morseStr[MAX_MORSE_CODE_LEN];  
} MORSE_CODE_STRUCT;

typedef struct
{
    uint8 level;
    uint16 duration_time;   
} TORCH_PATTERN_STRUCT;



#endif
