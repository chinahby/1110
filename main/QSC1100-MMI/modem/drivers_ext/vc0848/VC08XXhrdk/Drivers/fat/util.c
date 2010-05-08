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
#include "util.h"


/****************************************************************************
 *
 * fn_getversion
 *
 * returns with the filesystem version string
 *
 * RETURNS
 *
 * string pointer with version number
 *
 ***************************************************************************/
#if F_GETVERSION
#if F_LONGFILENAME
char *fn_getversion(void) 
{
	return (char*)("HCC_FAT_THIN_LFN ver: 1.75, Unicode mod: 0.9");
}
#else
char *fn_getversion(void) 
{
	return (char*)("HCC_FAT_THIN ver:1.75");
}
#endif
#endif

/****************************************************************************
 *
 * _f_getword
 *
 * get a word 16bit number from a memory (it uses LITTLE ENDIAN mode always)
 *
 * INPUTS
 *
 * ptr - pointer where data is
 *
 * RETURNS
 *
 * word number
 *
 ***************************************************************************/

unsigned short _f_getword(void *ptr) 
{
	unsigned char *sptr=(unsigned char*)ptr;
	unsigned short ret;	
	ret=sptr[1];
	ret<<=8;
	ret+=sptr[0];
	return ret;
}

/****************************************************************************
 *
 * _f_setword
 *
 * set a word 16bit number into a memory (it uses LITTLE ENDIAN mode always)
 *
 * INPUTS
 *
 * ptr - where to store data
 * num - 16 bit number to store
 *
 ***************************************************************************/
#if F_WRITING
void _f_setword(void *ptr,unsigned short num) 
{
	unsigned char *sptr=(unsigned char*)ptr;
	sptr[1]=(unsigned char)(num>>8);
	sptr[0]=(unsigned char)(num);
}
#endif

/****************************************************************************
 *
 * _f_getlong
 *
 * get a long 32bit number from a memory (it uses LITTLE ENDIAN mode always)
 *
 * INPUTS
 *
 * ptr - pointer where data is
 *
 * RETURNS
 *
 * long number
 *
 ***************************************************************************/
unsigned long _f_getlong(void *ptr) 
{
	unsigned char *sptr=(unsigned char*)ptr;
	unsigned long ret;	
	ret=sptr[3];
	ret<<=8;
	ret+=sptr[2];
	ret<<=8;
	ret+=sptr[1];
	ret<<=8;
	ret+=sptr[0];
	return ret;
}

/****************************************************************************
 *
 * _f_setlong
 *
 * set a long 32bit number into a memory (it uses LITTLE ENDIAN mode always)
 *
 * INPUTS
 *
 * ptr - where to store data
 * num - 32 bit number to store
 *
 ***************************************************************************/
#if F_WRITING
void _f_setlong(void *ptr,unsigned long num) 
{
	unsigned char *sptr=(unsigned char*)ptr;
	sptr[3]=(unsigned char)(num>>24);
	sptr[2]=(unsigned char)(num>>16);
	sptr[1]=(unsigned char)(num>>8);
	sptr[0]=(unsigned char)(num);
}
#endif

/****************************************************************************
 *
 * _getchar
 *
 * get characters from memory
 *
 * INPUTS
 *
 * array - where to store read data
 * num - number of characters
 * ptr - where to read from
 *
 * RETURNS
 *
 * read pointer new position
 *
 ***************************************************************************/
#if (F_GETLABEL || F_SETLABEL || F_FORMATTING || F_FAT32)
unsigned char *_getchar(unsigned char *array,int num,unsigned char *ptr) 
{

	if (!array) return ptr+num;

	while (num--) 
	{
		*array++=*ptr++;
	}
	return ptr;
}

#endif




/****************************************************************************
 *
 * _setcharzero
 *
 * fills with zero charater to memory
 *
 * INPUTS
 *
 * num - number of characters
 * ptr - where to store data
 *
 * RETURNS
 *
 * last write position
 *
 ***************************************************************************/

#if (F_FORMATTING & F_WRITING)
unsigned char *_setcharzero(int num,unsigned char *ptr) 
{
	while (num--) 
	{
		*ptr++=0;
	}
	return ptr;
}

#endif


/****************************************************************************
 *
 * _setchar
 *
 * copy a charater string to memory
 *
 * INPUTS
 *
 * array - original code what to copy
 * num - number of characters
 * ptr - where to store data
 *
 * RETURNS
 *
 * last write position
 *
 ***************************************************************************/

#if (F_FORMATTING & F_WRITING)
unsigned char *_setchar(unsigned char *array,int num,unsigned char *ptr) 
{

	if (!array) return _setcharzero(num,ptr);

	while (num--) 
	{
		*ptr++=*array++;
	}
	return ptr;
}
#endif



/****************************************************************************
 *
 * _setword
 *
 * store a 16bit word into memory
 *
 * INPUTS
 *
 * num - 16bit number to store
 * ptr - where to store data
 *
 * RETURNS
 *
 * last write position
 *
 ***************************************************************************/
#if (F_FORMATTING & F_WRITING)
unsigned char *_setword(unsigned short num,unsigned char *ptr) 
{
	_f_setword(ptr,num);
	return ptr+2;
}
#endif

/****************************************************************************
 *
 * _setlong
 *
 * store a 32bit long number into memory
 *
 * INPUTS
 *
 * num - 32bit number to store
 * ptr - where to store data
 *
 * RETURNS
 *
 * last write position
 *
 ***************************************************************************/
#if (F_FORMATTING & F_WRITING)
unsigned char *_setlong(unsigned long num,unsigned char *ptr)
{
	_f_setlong(ptr,num);
	return ptr+4;
}

#endif

#ifdef INTERNAL_MEMFN
/****************************************************************************
 *
 * _f_memcpy
 *
 * standard memory copy
 *
 * INPUTS
 *
 * d - destination address
 * s - source address
 * len - length of copied data
 *
 ***************************************************************************/

void _f_memcpy(void *d, void *s, int len) 
{
	unsigned char *dc=(unsigned char*)d;
	unsigned char *sc=(unsigned char*)s;
	while(len--) 
	{
		*dc++=*sc++;
	}
}

/****************************************************************************
 *
 * _f_memset
 *
 * standard memory set into a value
 *
 * INPUTS
 *
 * d - destination address
 * fill - what char used to fill
 * len - length of copied data
 *
 ***************************************************************************/
void _f_memset(void *d, unsigned char fill, int len) 
{
	unsigned char *dc=(unsigned char*)d;
	while(len--) 
	{
		*dc++=fill;
	}
}

/****************************************************************************
 *
 * _f_memcmp
 *
 * standard memory compare
 *
 * INPUTS
 *
 * s1 - first buffer
 * s2 - second buffer
 * len - length of bytes to compare
 *
 * RETURNS
 * 0 - s1 is equal to s2
 * > 0 - s1 is greater than s2
 * < 0 - s1 is less than s2.
 *
 ***************************************************************************/
int _f_memcmp(void *s1, void *s2, int len) 
{
	if (!len) return 0;

	while(--len && *(char *)s1 == *(char *)s2)
	{
		s1 = (char *)s1 + 1;	
		s2 = (char *)s2 + 1;	
	}
	return(*((unsigned char *)s1) - *((unsigned char *)s2));
}

#endif


/****************************************************************************
 *
 * _f_toupper
 *
 * convert a char into lower case
 *
 * INPUTS
 *
 * s - input string to convert
 *
 ***************************************************************************/
char _f_toupper(char ch) 
{
	if (ch>='a' && ch<='z') return ch-'a'+'A';
	return ch;
}



