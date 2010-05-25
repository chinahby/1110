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


#if F_LONGFILENAME

/****************************************************************************
 *
 * _f_strtolower
 *
 * converts a string into lower case
 *
 * INPUTS
 *
 * s - input string to convert
 *
 ***************************************************************************/
void _f_strtolower(char *s) 
{
	for (;;) 
	{
		char ch=*s;
		if (!ch) break;
		if (ch>='A' && ch<='Z') *s=ch-'A'+'a';
		s++;
	}
}

/****************************************************************************
 *
 * _f_strasc2uni
 *
 * converts an ASCII string into Unicode char stream
 *
 * INPUTS
 *
 * dest - where to store coverted Unicode char stream
 * src - input string to convert
 *
 ***************************************************************************/
void _f_strasc2uni(char *dest, char *src)
{
	for (;;) 
	{
		char ch=*src;
		*dest = ch;
		*(dest+1) = 0;
		if(!ch) break;
		src++;
		dest += 2;
	}
}

#if F_CHECKNAME
/****************************************************************************
 *
 * _f_checknamewc
 *
 * checking filename for wildcard character
 *
 * INPUTS
 * lname - filename (e.g.: filename)
 * wc - check for wildcards too?
 *
 * RETURNS
 *
 * 0 - if no contains wildcard character (? or *)
 * other - if contains any wildcard character
 *
 * REMARKS
 * 
 * Unicode supported
 *
 ***************************************************************************/
unsigned char _f_checknamewc(const char *lname, char wc) 
{
	for(;;) 
	{
		char cl = *lname++;
		char ch = *lname++;
		if(!cl && !ch) return 0;
		if(wc && !ch && (cl == '?' || cl == '*')) return 1; 
		if(!ch && (cl=='|' || cl=='<' || cl=='>' || cl=='/' || cl=='\\' || cl==':')) return 1;
	}
}

#endif


#if F_FINDING
/****************************************************************************
 *
 * _f_checkmatchwc
 *
 * checking if given Unicode char stream match with the pattern Unicode 
 * char stream that contains wildcard
 *
 * INPUTS
 * pat - the pattern Unicode char stream that contains wildcard
 * src - the given Unicode char stream
 *
 * RETURNS
 *
 * 0 - if not match
 * other - if match
 *
 * REMARKS
 * 
 * Unicode supported
 *
 ***************************************************************************/
unsigned char _f_checkmatchwc(char *pat, char *src) 
{
	char *s, *p;
	char all[8] = {'*', 0, '.', 0, '*', 0, 0, 0};
	unsigned char star = 0;
	unsigned char i;

	for(i = 0; i < 8; i++)
	{
		if(pat[i] != all[i]) goto loop_start;
	}

	all[2] = 0;
	pat = (char *)all; // "*.*" equals to "*"

loop_start:
	for (s = src, p = pat; *s || *(s+1); s += 2, p += 2) 
	{
		unsigned short unichar = *p + ((unsigned short)*(p+1)<<8);
		switch (unichar) 
		{
			case '?':
				if (*s == '.' && !*(s+1)) goto star_check;
				break;
			case '*':
				star = 1;
				src = s, pat = p;
				do { pat+=2; } while (*pat == '*' && !*(pat+1));
				if (!*pat && !*(pat+1)) return 1;
				goto loop_start;
			default:
				if(!*(s+1) && !*(p+1))
				{
					if(_f_toupper(*s) != _f_toupper(*p)) goto star_check;
				}
				else
				{
					if(*(s+1) != *(p+1) || *s != *p) goto star_check;
				}
				break;
		} /* endswitch */
	} /* endfor */
	while (*p == '*' && !*(p+1)) p += 2;
	return (!*p && !*(p+1));

star_check:
	if (!star) return 0;
	src += 2;
	goto loop_start;
}
#endif

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
 * REMARKS
 * 
 * Unicode supported
 *
 ***************************************************************************/
unsigned char _f_setfsname(const char *name, F_NAME *fsname) 
{
	unsigned short namepos=0;
#if F_DIRECTORIES
	unsigned short pathpos=0;
	unsigned short a;
#endif

	if (!name[0] && !name[1]) return 1; /*no name*/

	if (name[2]==':' && name[3]==0) name+=4; 

#if F_DIRECTORIES
	if (name[0]!='/' && name[0]!='\\' || name[1]) 
	{
		/* relative path */
		if (fn_getcwd(fsname->path, F_MAXPATH, 0)) return 1; /*error*/
		for (pathpos = 0; fsname->path[pathpos] || fsname->path[pathpos+1];) 
		{
			pathpos+=2;
		}
	}
#endif
	for (;;) 
	{
		char cl = *name++;
		char ch = *name++;
		
		if (!cl && !ch) break;

		if (cl==':' && !ch) return 1; /*not allowed*/

		if ((cl=='/' || cl=='\\') && !ch) 
		{
#if F_DIRECTORIES
			if (pathpos) 
			{
				if (fsname->path[pathpos-2]=='/' && !fsname->path[pathpos-1]) return 1; /*not allowed double */

				if (pathpos>=F_MAXPATH-4) return 1; /*path too long*/
				fsname->path[pathpos++] = '/';
				fsname->path[pathpos++] = 0;
			}

			for (;namepos;) 
			{
				if (fsname->lname[namepos-2]==' ' && !fsname->path[namepos-1])
					namepos-=2;		  /*remove end spaces*/
				else
					break;
				
			}

			for (a=0; a<namepos; a+=2) 
			{
				if (pathpos>=F_MAXPATH-4) return 1; /*path too long*/
				fsname->path[pathpos++]=fsname->lname[a];
				fsname->path[pathpos++]=fsname->lname[a+1];
			}
			namepos = 0;
			continue;
#else
			if (namepos) return 1;
#endif
		}

		if ((cl==' ' && !ch) && (!namepos)) continue; /*remove start spaces*/

		if (namepos>=F_MAXLNAME-2) return 1; /*name too long*/

		fsname->lname[namepos++]=cl;
		fsname->lname[namepos++]=ch;
	}

	fsname->lname[namepos]=0;
	fsname->lname[namepos+1]=0; 	/*terminates it*/
#if F_DIRECTORIES
	fsname->path[pathpos]=0;  
	fsname->path[pathpos+1]=0; 	/*terminates it*/
#endif
	for (;namepos;) 
	{
		if (fsname->lname[namepos-2] == ' ' && !fsname->lname[namepos-1]) /* namepos & (namepos+1) are null */
		{
			fsname->lname[namepos-2] = 0; /*remove end spaces*/
			namepos-=2;
		}
		else
			break;
	}


	if (!namepos) return 2; /*no name*/
	return 0;
}

#endif /* F_LONGFILENAME */

