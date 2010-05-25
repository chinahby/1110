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

#if !(F_LONGFILENAME)

#if F_CHECKNAME
/****************************************************************************
 *
 * _f_checknameprim
 *
 * checking a string if could be valid
 *
 * INPUTS
 *
 * ptr - pointer to name or extension
 * len - number max char of name or extension
 *
 * RETURNS
 *
 ***************************************************************************/

static unsigned char _f_checknameprim(char *ptr,unsigned char len) 
{
	unsigned char inspace=0;
	while (len--) 
	{
		char ch=*ptr++;
		if (!inspace) 
		{
			if (ch==' ') inspace=1;
			if (ch=='|' || ch=='[' || ch==']' || ch=='<' || ch=='>' || ch=='/' || ch=='\\' || ch==':') 
			{
			  return 1;
			}
		}
		else if (ch!=' ') return 1; /*no inspace allowed*/
	}

	return 0;
}

/****************************************************************************
 *
 * _f_checkname
 *
 * checking filename and extension for special characters
 *
 * INPUTS
 *
 * name - filename (e.g.: filename)
 * ext - extension of file (e.g.: txt)
 *
 * RETURNS
 *
 * 0 - if no contains invalid character
 * other - if contains any invalid character
 *
 ***************************************************************************/

unsigned char _f_checkname(char *name, char *ext) 
{
   if (_f_checknameprim(name,F_MAXNAME)) return 1;
   if (_f_checknameprim(ext,F_MAXEXT)) return 1;

   return 0;
}

/****************************************************************************
 *
 * _f_checknamewc
 *
 * checking filename and extension for wildcard character
 *
 * INPUTS
 *
 * name - filename (e.g.: filename)
 * ext - extension of file (e.g.: txt)
 *
 * RETURNS
 *
 * 0 - if no contains wildcard character (? or *)
 * other - if contains any wildcard character
 *
 ***************************************************************************/
unsigned char _f_checknamewc(const char *name,const char *ext) 
{
	unsigned char a=0;
	for (a=0; a<F_MAXNAME; a++) 
	{
		char ch=name[a];
		if (ch=='?' || ch=='*') return 1;
	}
	for (a=0; a<F_MAXEXT; a++) 
	{
		char ch=ext[a];
		if (ch=='?' || ch=='*') return 1;
	}
	return _f_checkname((char *)name,(char *)ext);
}
#endif

/****************************************************************************
 *
 * _f_setnameext
 *
 * convert a string into filename and extension separatelly, the terminator
 * character could be zero char, '/' or '\'
 *
 * INPUTS
 *
 * s - source string (e.g.: hello.txt)
 * name - where to store name (this array size has to be F_MAXNAME (8))
 * ext - where to store extension (this array size has to be F_MAXEXT (3))
 *
 * RETURNS
 *
 * length of the used bytes from source string array
 *
 ***************************************************************************/
unsigned char _f_setnameext(char *s, char *name, char *ext) 
{
	unsigned char len,extlen=0;
	unsigned char a;
	unsigned char setext=1;

	for (len=0;;) 
	{
		unsigned char ch=s[len];
		if (ch==0 || ch=='\\' || ch=='/') break;
		len++;					/*calculate len*/
	}

	if (len && s[0]=='.') 
	{
	/*		if (len==1 || (s[1]=='.' && len==2)) goto dots;		*/
		if (len==1 || s[1]=='.') goto dots;
	}

	for (a=len; a; a--) 
	{
		if (s[a-1]=='.') 
		{
			unsigned char b;

			extlen=len-a+1;
			len=a-1;

			for (b=0; b<F_MAXEXT; b++) 
			{
				if (b<extlen-1) ext[b]=_f_toupper(s[a++]);
				else ext[b]=' ';
			}
			setext=0;
			break;
		}
	}

dots:
	if (setext) 
	{
		for (a=0; a<F_MAXEXT; a++) ext[a]=' '; 
	}

	for (a=0; a<F_MAXNAME; a++) 
	{
		if (a<len) name[a]=_f_toupper(s[a]);
		else name[a]=' ';
	}

	return len+extlen;
}



/****************************************************************************
 *
 * _f_setfsname
 *
 * convert a single string into F_NAME structure
 *
 * INPUTS
 *
 * name - combined name with drive,path,filename,extension used for source
 * fsname - where to fill this structure with separated drive,path,name,ext
 *
 * RETURNS
 *
 * 0 - if successfully
 * other - if name contains invalid path or name
 *
 ***************************************************************************/

unsigned char _f_setfsname(const char *name,F_NAME *fsname) 
{
	char s[F_MAXPATH];
	unsigned char namepos=0;
#if F_DIRECTORIES
	unsigned char pathpos=0;
	unsigned char a;
#endif

	s[0]=0;

	if (!name[0]) return 1; /*no name*/

	if (name[1]==':') name+=2;

#if F_DIRECTORIES
	if (name[0]!='/' && name[0]!='\\') 
	{
		if (fn_getcwd(fsname->path,F_MAXPATH,0)) return 1; /*error*/
		for (pathpos=0; fsname->path[pathpos];) pathpos++;
	}
#endif

	for (;;) 
	{
		char ch=_f_toupper(*name++); 
		if (!ch) break;
		if (ch==':') return 1; /*not allowed*/

		if (ch=='/' || ch=='\\') 
		{
#if F_DIRECTORIES
			if (pathpos) 
			{
				if (fsname->path[pathpos-1]=='/') return 1; /*not allowed double */
				if (pathpos>=F_MAXPATH-2) return 1; /*path too long*/
				fsname->path[pathpos++]='/';
			}

			for (;namepos;) 
			{
				if (s[namepos-1]!=' ') break;
				namepos--;		  /*remove end spaces*/
			}

			for (a=0; a<namepos; a++) 
			{
				if (pathpos>=F_MAXPATH-2) return 1; /*path too long*/
				fsname->path[pathpos++]=s[a];
			}
			namepos=0;
			continue;
#else
			if (namepos) return 1; /*no directories allowed*/
#endif
		}

		if (ch==' ' && (!namepos)) continue; /*remove start spaces*/

		if (namepos>=(sizeof(s)-2)) return 1; /*name too long*/
		s[namepos++]=ch;
	}

	s[namepos]=0; /*terminates it*/
#if F_DIRECTORIES
	fsname->path[pathpos]=0;  /*terminates it*/
#endif

	for (;namepos;) 
	{
		if (s[namepos-1]!=' ') break;
		s[namepos-1]=0; /*remove end spaces*/
		namepos--;
	}

	if (!_f_setnameext(s,fsname->filename,fsname->fileext)) return 2; /*no name*/

	if (fsname->filename[0]==' ') return 1; /*cannot be*/

	return 0;
}


#endif /* !(F_LONGFILENAME) */

