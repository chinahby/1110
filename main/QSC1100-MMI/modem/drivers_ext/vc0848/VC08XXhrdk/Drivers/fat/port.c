#ifndef _PORT_C_
#define _PORT_C_

/****************************************************************************
 *
 *            Copyright (c) 2003 by HCC Embedded 
 *
 * This software is copyrighted by and is the sole property of 
 * HCC.  All rights, title, ownership, or other interests
 * in the software remain the property of HCC.  This
 * software may only be used in accordance with the corresponding
 * license agreement.  Any unauthorized use, duplication, transmission,  
 * distribution, or disclosure of this software is expressly forbidden.
 *
 * This Copyright notice may not be removed or modified without prior
 * written consent of HCC.
 *
 * HCC reserves the right to modify this software without notice.
 *
 * HCC Embedded
 * Budapest 1132
 * Victor Hugo Utca 11-15
 * Hungary
 *
 * Tel:  +36 (1) 450 1302
 * Fax:  +36 (1) 450 1303
 * http: www.hcc-embedded.com
 * email: info@hcc-embedded.com
 *
 ***************************************************************************/

#include "thin_usr.h"
#include "port.h"

/* user depend APIs of VC0848 */
#include "mmd_common_typedef.h"
#include "mmd_user_depend.h"
#include	"mmd_mm.h"
/****************************************************************************
 *
 * f_getrand
 *
 * This function should be ported. It has to return a different 32bit
 * random number whenever it is called. Random number generator could be
 * get from system time, this algorith below is just a simple random 
 * number generator
 * 
 * INPUTS
 *
 * rand - a number which could be used for random number generator
 * 
 * RETURNS
 *
 * 32bit random number
 *
 ***************************************************************************/
#if (F_FORMATTING && F_WRITING)
static unsigned long dwrand=0x729a8fb3;
unsigned long f_getrand(unsigned long rand) 
{
	long a;

	dwrand^=f_gettime();

	for (a=0; a<32; a++) 
	{
		if (rand&1) 
		{
			dwrand^=0x34098bc2;
		}
		if (dwrand&0x8000000) 
		{
			dwrand<<=1;
			dwrand|=1;
		}
		else dwrand<<=1;
		rand>>=1;
	}

	return dwrand;
}

#endif 

/****************************************************************************
 *
 * f_getdate
 *
 * need to be ported depending on system, it retreives the
 * current date in DOS format
 *
 * RETURNS
 *
 * current date
 *
 ***************************************************************************/
#if F_WRITING
unsigned short f_getdate(void) 
{
	//unsigned short year = 1980;
	//unsigned short month = 1;
	//unsigned short day = 1;
	unsigned short pcdate;	
	MMD_DATE curdate;

	MMD_USER_GetDate(&curdate);

	pcdate = (((curdate.year - 1980) << F_CDATE_YEAR_SHIFT) & F_CDATE_YEAR_MASK) |
			 ((curdate.month << F_CDATE_MONTH_SHIFT) & F_CDATE_MONTH_MASK) |
			 ((curdate.day << F_CDATE_DAY_SHIFT) & F_CDATE_DAY_MASK);
	
	return pcdate;
}
#endif

/****************************************************************************
 *
 * f_gettime
 *
 * need to be ported depending on system, it retreives the
 * current time in DOS format
 *
 * RETURNS
 *
 * current time
 *
 ***************************************************************************/
#if F_WRITING
unsigned short f_gettime(void) 
{
	//unsigned short hour=12;
	//unsigned short min=0;
	//unsigned short sec=0;
	unsigned short pctime;
	MMD_TIME curtime;

	MMD_USER_GetTime(&curtime);

	pctime = ((curtime.hour << F_CTIME_HOUR_SHIFT) & F_CTIME_HOUR_MASK) |
			 ((curtime.minute << F_CTIME_MIN_SHIFT) & F_CTIME_MIN_MASK) |
			 (((curtime.second >> 1) << F_CTIME_SEC_SHIFT) & F_CTIME_SEC_MASK);
	
	return pctime;
}
#endif

/****************************************************************************
 *
 * f_uchar2oem
 *
 * need to be ported depending on system charset setting, it coverts character 
 * from Unicode to OEM/ANSI code page character set.
 *
 * INPUTS
 * 
 * unichar - the Unicode char to convert
 *
 * RETURNS
 *
 * 2-byte(16-bit) character of OEM/ANSI code page
 *
 * REMARKS
 * 
 * for DBSC(Double-Byte Character Set), return the 16-bit char with
 * the lead byte in high byte and the trail byte in low byte;
 * for SBSC(Single-Byte Character Set), return the 16-bit char with the
 * result of conversion in low byte and 0 in high byte.
 *
 ***************************************************************************/
#if F_LONGFILENAME
unsigned short f_unichar2oem(unsigned short unichar) 
{
	unsigned short oemchar;

	oemchar = '_';
	
	if(unichar < 0x80) 
	{
		//ASCII?		
		oemchar =  unichar;
	}
	else 
	{
		/* at least convert to a valid ASCII char if not specified any codepage */
		oemchar = (unichar & 0xFF) + (unichar >> 8);
		oemchar = oemchar % 26 + 'A';
	}

	return oemchar;
}
#endif

/****************************************************************************
 *
 * f_malloc
 *
 * need to be ported depending on system, it allocates memory blocks of specified size. 
 *
 * INPUTS
 * 
 * size - bytes to allocate
 *
 * RETURNS
 *
 * Pointer to the allocated space, or NULL if there is insufficient memory available
 *
 ***************************************************************************/
void *f_malloc(unsigned int size)
{
	return MMD_Malloc(size);
}

/****************************************************************************
 *
 * f_free
 *
 * need to be ported depending on system, it frees previously allocated memory block. 
 *
 * INPUTS
 * 
 * ptr - pointer to previously allocated memory block
 *
 ***************************************************************************/
void f_free(void * ptr)
{
	MMD_Free(ptr);
}

/****************************************************************************
 *
 * end of port.c
 *
 ***************************************************************************/

#endif /*_PORT_C_*/
