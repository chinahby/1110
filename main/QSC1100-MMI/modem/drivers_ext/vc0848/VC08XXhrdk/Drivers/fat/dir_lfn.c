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

#include "dir_lfn.h"
#include "util.h"
#include "volume.h"
#include "drv.h"
#include "file.h"
#include "fat.h"

#if F_LONGFILENAME

/* Characters that are undesirable in an short directory entry name */
//static unsigned short bad_chars[] = {
	/*  `*'     `?'     `<'    `>'      `|'     `"'     `:'     `/' */
	//0x002A, 0x003F, 0x003C, 0x003E, 0x007C, 0x0022, 0x003A, 0x002F,
	/*  `\' */
	//0x005C, 0,
//};
/* Characters that will be replaced with '_' in an short directory entry name */
static unsigned short replace_chars[] = {
	/*  `['     `]'    `;'     `,'     `+'      `=' */
	0x005B, 0x005D, 0x003B, 0x002C, 0x002B, 0x003D, 0,
};
/* Characters that will be skipped in an short directory entry name */
static unsigned short skip_chars[] = {
	/*  `.'     ` ' */
	0x002E, 0x0020, 0,
};

//#define F_IS_BADCHAR(uni)	(_f_unistrchr(bad_chars, (uni)) != NULL)
#define F_IS_REPLACECHAR(uni)	(_f_unistrchr(replace_chars, (uni)) != NULL)
#define F_IS_SKIPCHAR(uni) \
	((unsigned short)(uni) == skip_chars[0] || (unsigned short)(uni) == skip_chars[1])
#define F_IS_ALPHABET(uni) (((uni)>='A' && (uni)<='Z') || ((uni)>='a' && (uni)<='z'))
#define F_IS_UPPERCASE(uni) ((uni)>='A' && (uni)<='Z')


/****************************************************************************
 *
 * _f_unistrchr
 *
 * locates the first occurrence of the Unicode char in the Unicode string.
 *
 * INPUTS
 *
 * c - the Unicode char to be located
 * s - the Unicode source string
 *
 * RETURNS
 *
 * returns a pointer to the Unicode char, or a null pointer if the char was not found.
 *
 * REMARKS
 * 
 * Unicode supported
 *
 ***************************************************************************/
static unsigned short *_f_unistrchr(const unsigned short *s, const unsigned short c)
{
	for(; *s != c; ++s) 
	{
		if (*s == 0) return NULL;
	}
	return (unsigned short *) s;
}

/****************************************************************************
 *
 * _f_getnextstring
 *
 * finding in a string a string until / or \ termination
 *
 * INPUTS
 *
 * dest - where to store next string
 * sou - original text
 *
 * RETURNS
 *
 * new start position in the original
 *
 * REMARKS
 *
 * Unicode supported
 *
 ***************************************************************************/
#if F_DIRECTORIES
static char *_f_getnextstring(char *dest, char *sou) 
{
	for (;;) 
	{
		char cl = *sou;
		char ch = *(sou+1);
		if (!cl && !ch) break;
		sou+=2;
		if (!ch && (cl=='/' || cl=='\\')) break;
		
		*dest++ = cl;
		*dest++ = ch;
	}
	*dest++ = 0;
	*dest = 0; /*add termination*/
	
	return sou;
}
#endif


/****************************************************************************
 *
 * _f_getdirsector
 *
 * read a directory sector
 *
 * INPUTS
 *
 * sector - which sector is needed
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/
unsigned char _f_getdirsector(unsigned long sector) 
{
	unsigned char ret = F_NO_ERROR;
	ret = _f_readsector(gl_volume.direntry, sector);
	if(!ret) gl_volume.direntrysector = sector;
	else gl_volume.direntrysector = (unsigned long)(-1);
	return ret;
}

/****************************************************************************
 *
 * _f_writedirsector
 *
 * Write directory sector, which was last read
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/
#if F_WRITING
unsigned char _f_writedirsector(void) 
{
	if (gl_volume.direntrysector != (unsigned long)(-1)) 
	{
		return _f_writesector(gl_volume.direntry, gl_volume.direntrysector);
	}
	return F_ERR_INVALIDSECTOR;
}
#endif

/****************************************************************************
 *
 * _f_getfilename
 *
 * create a complete 8.3 filename string from name and extension
 *
 * INPUTS
 *
 * dest - where to store filename, a 8.3 filename string
 * name - name of the file
 * ext - extension of the file
 * ntres -name and extension lower case flag
 *
 ***************************************************************************/
static void _f_getfilename(char *dest, char *name, char *ext, unsigned char ntres) 
{
	unsigned char a, len;
	char *ptr = dest;

	for (len=a=F_MAXNAME; a; a--,len--) 
	{
		if (name[a-1] != ' ') break;
	}
	for (a=0; a<len; a++) 
	{
		*ptr++ = *name++;
	}
	*ptr = 0;
	if(ntres & NTRES_NAME_LOWER)
	{
		_f_strtolower(dest);
	}	

	for (len=a=F_MAXEXT; a; a--,len--) 
	{
		if (ext[a-1] != ' ') break;
	}
	if (len) *ptr++ = '.';

	dest = ptr;

	for (a=0; a<len; a++) 
	{
		*ptr++ = *ext++;
	}
	*ptr = 0; /*terminateit*/

	if(ntres & NTRES_EXT_LOWER)
	{
		_f_strtolower(dest);
	}	
}

/****************************************************************************
 *
 * _f_addlfn
 *
 * add next directory entry as long filename entry, this routine will build
 * the next file name, even if its long filename or short filename
 *
 * INPUTS
 *
 * de - directory entry to add
 * plfn - pointer to lfn structure where to build
 * num - number of directory entry
 *
 * REMARKS
 *
 * Unicode supported
 *
 ***************************************************************************/
void _f_addlfn(F_DIRENTRY *de, F_LFNINT *plfn, unsigned long num) 
{
	F_LFN *lfn=(F_LFN *)de;
	long pos;
	char cpos[26] = {1,2,3,4,5,6,7,8,9,10,14,15,16,17,18,19,20,21,22,23,24,25,28,29,30,31};
	unsigned char *ptr;
	unsigned char i, chksum = 0;

	if ((!de->name[0]) || ((unsigned char)(de->name[0])==0xe5)) 
	{
		plfn->state = F_LFNSTATE_INV;
		return;
	}

	if (de->attr == F_ATTR_LFN) 
	{
		if (lfn->clusterlo) 
		{  
			/*this is always zero in lfn!!*/
			plfn->state = F_LFNSTATE_INV;
			return;
		}

		pos = (unsigned long)lfn->ord & 0x3f;

		if (pos<1 || pos>20) 
		{
			plfn->state = F_LFNSTATE_INV;
			return;
		}

		pos--;
		pos *= 26; /*get current pos in Unicode char stream;*/	
		ptr = (unsigned char *)lfn;

		if (lfn->ord & 0x40) 
		{  
			/*if its the end of lfn (reverse order!)*/
			plfn->ord = lfn->ord;
			plfn->state = F_LFNSTATE_NEXT;
			plfn->chksum = lfn->chksum;
			plfn->start = num;
			plfn->end = num;			
			for (i = 0; i < 26; i+=2)
			{
				plfn->name[pos+i] = *(ptr+cpos[i]);
				plfn->name[pos+i+1] = *(ptr+cpos[i+1]);
				if(!plfn->name[pos+i] && !plfn->name[pos+i+1]) 
					break; /* meet double-null, end of LFN */
			}			
			if(plfn->name[pos+i-2] || plfn->name[pos+i-1])
			{
				plfn->name[pos+i] = 0; 
				plfn->name[pos+i+1] = 0;	/*set double-null terminate*/
			}
		}
		else 
		{

			if (plfn->chksum != lfn->chksum) 
			{
				plfn->state=F_LFNSTATE_INV;
				return;
			}

			if (plfn->state == F_LFNSTATE_NEXT) 
			{
				if (lfn->ord != ((plfn->ord & 0x3f)-1)) 
				{ 
					/*if not the next*/
					plfn->state = F_LFNSTATE_INV;
					return;
				}

				plfn->ord = lfn->ord;
			}
			else 
			{
				plfn->state = F_LFNSTATE_INV;
				return;
			}
			
			for (i = 0; i < 26; i++)
			{
				plfn->name[pos+i] = *(ptr+cpos[i]);
			}
		}
	}
	else 
	{ 
		/*check sum check*/
		if (plfn->state == F_LFNSTATE_NEXT) 
		{
			if ((plfn->ord & 0x3f) == 1) 
			{
				ptr = (unsigned char *)de;
				for (i = 0; i < 11; i++)
				{
					chksum = (((chksum&1)<<7) | (( chksum & 0xfe) >>1)) + *ptr++;
				}      

				if (chksum == plfn->chksum) 
				{
					plfn->state = F_LFNSTATE_LFN;
					plfn->end = num;
					return;
				}
			}
		}

		plfn->start = plfn->end = num; /*only short name*/
		plfn->ntres = de->ntres;
		_f_getfilename(cpos, de->name, de->ext, de->ntres);
		_f_strasc2uni(plfn->name, cpos);
		
		plfn->state = F_LFNSTATE_SFN;
	}
}


#if (F_DELETE || F_RMDIR || F_WRITING || F_DIRECTORIES || F_GETLABEL || F_SETLABEL)
/****************************************************************************
 *
 * _f_opendir
 *
 * open a directory as file
 *
 * INPUTS
 * cluster - directory start cluster
 *
 * RETURNS
 * file pointer or zero if any error
 *
 ***************************************************************************/
F_FILE *_f_opendir(unsigned long cluster) 
{
	F_FILE *f = 0;

	f = &gl_files[F_MAXFILES]; //_f_getintfile();

	_memset((char *)f, 0, 24);
	if(!f) return 0; /*no more file could be opened */

	f->startcluster = cluster;

	_f_clustertopos(f->startcluster, &f->pos);

	if (_f_readsector(f->_tdata, f->pos.sector)) return 0;

	f->mode = F_FILE_RD;
	return f;
}


/****************************************************************************
 *
 * _f_nextdirsector
 *
 * check and go to next directory sector
 *
 * INPUTS
 * f - filepointer
 *
 * RETURNS
 * error code or zero if successfull
 *
 ***************************************************************************/
static unsigned char _f_nextdirsector (F_FILE *f) 
{
	unsigned char ret;

	if (f->relpos == F_SECTOR_SIZE) 
	{
#if F_WRITING
		if(f->modified) 
		{
			ret = _f_writesector(f->_tdata, f->pos.sector);
			if(ret) return ret;
			f->modified = 0;
		}
#endif
		if((f->pos.sector + 1) == f->pos.sectorend) 
		{
			if(f->pos.cluster >= F_CLUSTER_RESERVED) return F_ERR_EOF;

			if(!f->pos.cluster) 
			{
#if F_FAT32
				if(gl_volume.mediatype == F_FAT32_MEDIA) 
				{
					f->pos.cluster = gl_volume.bootrecord.rootcluster;
				}
				else 
#endif
					return F_ERR_EOF; /*root directory*/
			}

			{
				unsigned long nextcluster;
				if(_f_getclustervalue(f->pos.cluster, &nextcluster)) return F_ERR_EOF; /*not found*/
				if(nextcluster >= F_CLUSTER_RESERVED) return F_ERR_EOF;
				_f_clustertopos(nextcluster, &f->pos);
			}
		}
		else 
		{
			f->pos.sector++;	  
		}

		ret=_f_readsector(f->_tdata, f->pos.sector);
		if (ret) return ret;

		f->relpos = 0;
		f->abspos += F_SECTOR_SIZE;
	}

	return F_NO_ERROR;
}

/****************************************************************************
 *
 * _f_seekdir
 *
 * seek into directory
 *
 * INPUTS
 *
 * file - filepointer
 * offset - number of directory entries
 * whence - how to seek
 *
 * RETURNS
 *
 * error code or zero if successfull
 *
 ***************************************************************************/
unsigned char _f_seekdir(F_FILE *f, long offset, unsigned char whence) 
{
	unsigned char ret;

	if(!f) return F_ERR_NOTOPEN;

	if(offset == -1 && whence == F_SEEK_CUR && f->relpos) 
	{
		f->relpos -= sizeof(F_DIRENTRY);
		return F_NO_ERROR;
	}

	offset *= sizeof(F_DIRENTRY);

	switch(whence) 
	{
		case F_SEEK_CUR:  offset = f->abspos + f->relpos + offset; break;
		case F_SEEK_SET:  break;
		default: return F_ERR_NOTUSEABLE;
	}

#if F_WRITING
	if(f->modified) 
	{
		ret = _f_writesector(f->_tdata, f->pos.sector);
		if(ret) return ret;
		f->modified = 0;
	}
#endif
	f->relpos = 0;
	f->abspos = 0;

	_f_clustertopos(f->startcluster, &f->pos);

	while(1) 
	{
		if (offset < F_SECTOR_SIZE) 
		{
			f->relpos = offset;

			ret = _f_readsector(f->_tdata, f->pos.sector);
			if(ret) return ret;
			return F_NO_ERROR;
		}

		offset -= F_SECTOR_SIZE;
		f->abspos += F_SECTOR_SIZE;

		if((f->pos.sector + 1) == f->pos.sectorend) 
		{

			if(f->pos.cluster >= F_CLUSTER_RESERVED) return F_ERR_EOF;

			if(!f->pos.cluster) 
			{
#if F_FAT32
				if(gl_volume.mediatype == F_FAT32_MEDIA) 
				{
					f->pos.cluster = gl_volume.bootrecord.rootcluster;
				}
				else 
#endif
					return F_ERR_EOF; /*root directory*/
			}

			{
				unsigned long nextcluster;
				if(_f_getclustervalue(f->pos.cluster, &nextcluster)) return F_ERR_EOF; /*not found*/
				if(nextcluster >= F_CLUSTER_RESERVED) return F_ERR_EOF;
				_f_clustertopos(nextcluster, &f->pos);
			}
		}
		else 
		{
			f->pos.sector++;	  
		}
	}
}


/****************************************************************************
 *
 * _f_writedir
 *
 * write directory entry into directory
 *
 * INPUTS
 * de - directory entry needs written
 * file - filepointer
 *
 * RETURNS
 * error code or zero if successfull
 *
 ***************************************************************************/
unsigned char _f_writedir(F_DIRENTRY *de, F_FILE *f) 
{
	unsigned char ret;

	if(!f) return F_ERR_NOTOPEN;

	ret = _f_nextdirsector(f);
	if(ret) return ret;

	_memcpy(f->_tdata + f->relpos, de, sizeof(F_DIRENTRY));
	f->relpos += sizeof(F_DIRENTRY);

	f->modified = 1;

	return F_NO_ERROR;
}

/****************************************************************************
 *
 * _f_readdir
 *
 * read next directory entry
 *
 * INPUTS
 * de - where to store information
 * file - filepointer
 *
 * RETURNS
 * error code or zero if successfull
 *
 ***************************************************************************/
unsigned char _f_readdir(F_DIRENTRY *de, F_FILE *f) 
{
	unsigned char ret;

	if(!f) return F_ERR_NOTOPEN;

	ret = _f_nextdirsector(f);
	if(ret) return ret;

	_memcpy(de, f->_tdata + f->relpos, sizeof(F_DIRENTRY));
	f->relpos += sizeof(F_DIRENTRY);

	return F_NO_ERROR;
}


/****************************************************************************
 *
 * _f_closedir
 *
 * close a directory, and carry on error code 
 *
 * INPUTS
 * file - file pointer
 * errcode - errorcode 
 *
 * RETURNS
 * error code or zero if successfull
 *
 ***************************************************************************/
unsigned char _f_closedir(F_FILE *f, unsigned char errcode) 
{

	if(!f) 
	{
		if(errcode) return errcode;
		return F_ERR_NOTOPEN;
	}

	f->mode = F_FILE_CLOSE;

#if F_WRITING
	if(f->modified) 
	{
		int ret;
		ret = _f_writesector(f->_tdata, f->pos.sector);
		if(ret) 
		{
			if(errcode) return errcode;
			return ret;
		}
		f->modified = 0;
	}
#endif
	if(errcode) return errcode;
	return F_NO_ERROR;
}
#endif

#if ((F_DELETE || F_RMDIR) && F_WRITING)
/****************************************************************************
 *
 * _f_removelfn
 *
 * remove lfn entry from directory
 *
 * INPUTS
 * cluster - cluster of directory
 * lfn - long file name to remove (findfile fills starts and ends)
 *
 * RETURNS
 * 0 - if successfully
 * or error code
 *
 ***************************************************************************/
unsigned char _f_removelfn(unsigned long cluster, F_LFNINT *lfn) 
{
	F_DIRENTRY de;
	unsigned long a;
	F_FILE *file;
	unsigned char ret;

	if (lfn->state != F_LFNSTATE_LFN && lfn->state != F_LFNSTATE_SFN) 
	{
		return F_ERR_INVALIDNAME;
	}

	file = _f_opendir(cluster);
	if(!file) return F_ERR_INVALIDDIR;

	ret = _f_seekdir(file, lfn->start, F_SEEK_SET);
	if(ret) return	_f_closedir(file, ret);

	for(a = lfn->start; a <= lfn->end; a++) 
	{
		ret = _f_readdir(&de, file);		
		if(ret) return	_f_closedir(file, ret);

		ret = _f_seekdir(file, -1, F_SEEK_CUR);
		if(ret) return	_f_closedir(file, ret);

		de.name[0] = (char)0xe5;

		ret = _f_writedir(&de, file);		
		if(ret) return	_f_closedir(file, ret);
	}

	return _f_closedir(file, F_NO_ERROR);
}
#endif

/****************************************************************************
 *
 * _f_findlfilewc
 *
 * internal function to finding file in directory entry with or
 * without wildcard.
 *
 * INPUTS
 * lname - filename
 * pos - where to start searching, and contains current position
 * pde - store back the directory entry pointer
 * lfn - long file name structure
 * wc - use wildcard checking?
 *
 * RETURNS
 * 0 - if file was not found
 * 1 - if file was found
 *
 * REMARKS
 *
 * Unicode supported
 *
 ***************************************************************************/
#if F_FINDING
unsigned char _f_findlfilewc(char *lname,F_POS *pos, F_DIRENTRY **pde, F_LFNINT *lfn, unsigned char wc)
#else
unsigned char _f_findlfilewc(char *lname,F_POS *pos, F_DIRENTRY **pde, F_LFNINT *lfn) 
#endif
{
	unsigned long denum = 0;

	lfn->state = F_LFNSTATE_INV;

	while(pos->cluster < F_CLUSTER_RESERVED) 
	{
		for(; pos->sector < pos->sectorend; pos->sector++) 
		{
			F_DIRENTRY *de = (F_DIRENTRY*)(gl_volume.direntry + sizeof(F_DIRENTRY)*pos->pos);

			if(_f_getdirsector(pos->sector)) return 0; /*not found*/

			for(; pos->pos < F_SECTOR_SIZE / sizeof(F_DIRENTRY); de++, pos->pos++) 
			{

				_f_addlfn(de, lfn, denum);
				denum++;

				if(!de->name[0]) return 0; 					 /*empty*/
				if((unsigned char)(de->name[0]) == 0xe5) continue; /*deleted*/
				if(de->attr & F_ATTR_VOLUME) continue;

#if F_FINDING
				if(wc)
				{
							
					if(_f_checkmatchwc(lname, lfn->name)) 
					{
						if (pde) *pde = de;
						return 1;
					}
				}
				else
#endif
				{
					unsigned short b;
					char *buf = lfn->name;
					
					for (b = 0; ; b += 2)
					{
						char cl = buf[b];
						char ch = buf[b+1];
						if(!lname[b+1] && !ch)
						{
							if (_f_toupper(cl) != _f_toupper(lname[b])) break; /*differs*/
						}
						else
						{
							if(cl != lname[b] || ch != lname[b+1]) break; /*differs*/
						}
						if (!ch && !cl) 
						{
							if (pde) *pde=de;
							return 1;
						}
					}
				}
			}

			pos->pos = 0;
		}

		if(!pos->cluster) 
		{
#if F_FAT32
			if(gl_volume.mediatype == F_FAT32_MEDIA) 
			{
				pos->cluster = gl_volume.bootrecord.rootcluster;
			}
			else
#endif
			return 0; /*root directory*/
		}

		{
			unsigned long nextcluster;
			if(_f_getclustervalue(pos->cluster, &nextcluster)) return 0; /*not found*/
			if(nextcluster >= F_CLUSTER_RESERVED) return 0; /*not found*/
			_f_clustertopos(nextcluster, pos);
		}
	}

	return 0;
}

/****************************************************************************
 *
 * _f_getdecluster
 *
 * get a directory entry structure start cluster value
 *
 * INPUTS
 * de - directory entry
 *
 * RETURNS
 *
 * directory entry cluster value
 *
 ***************************************************************************/
unsigned long _f_getdecluster(F_DIRENTRY *de) 
{
#if F_FAT32
	unsigned long cluster;
	if(gl_volume.mediatype == F_FAT32_MEDIA) 
	{
		cluster = _f_getword(&de->clusterhi);
		cluster <<= 16;
		cluster |= _f_getword(&de->clusterlo);
		return cluster;
	}
#endif
	return _f_getword(&de->clusterlo);
}

/****************************************************************************
 *
 * _f_setdecluster
 *
 * set a directory entry structure start cluster value
 *
 * INPUTS
 * de - directory entry
 * cluster - value of the start cluster
 *
 ***************************************************************************/
#if F_WRITING
void _f_setdecluster(F_DIRENTRY *de, unsigned long cluster) 
{
	_f_setword(&de->clusterlo, (unsigned short)(cluster&0xffff));
#if F_FAT32
	if (gl_volume.mediatype == F_FAT32_MEDIA) 
	{
		_f_setword(&de->clusterhi, (unsigned short)(cluster>>16));
	}
	else 
#endif
	{
		_f_setword(&de->clusterhi, (unsigned short)0);
	}
}
#endif


/****************************************************************************
 *
 * _f_findpath
 *
 * finding out if path is valid in F_NAME and
 * correct path info with absolute path (removes relatives)
 *
 * INPUTS
 * fsname - filled structure with path,drive
 * pos - where to start searching, and contains current position
 *
 * RETURNS
 * 0 - if path was not found or invalid
 * 1 - if path was found
 *
 * REMARKS
 *
 * Unicode supported
 *
 ***************************************************************************/
unsigned char _f_findpath(F_NAME *fsname, F_POS *pos) 
{
#if F_DIRECTORIES
	F_LFNINT *plfn = NULL;
	char *currname = NULL;
	char *oripath = NULL;
	F_DIRENTRY *de;
	char *path = fsname->path;
	unsigned short pathpos = 0;
	unsigned short a;

	currname = (char *)f_malloc(F_MAXLNAME);
	oripath = (char *)f_malloc(F_MAXPATH);
	plfn = (F_LFNINT *)f_malloc(sizeof(F_LFNINT));
	
	if(!currname || !oripath || !plfn)
	{
		if(currname) f_free(currname);
		if(oripath) f_free(oripath);
		if(plfn) f_free(plfn);
		return F_ERR_NULLPOINTER;
	}
	
	oripath[0] = 0;
	oripath[1] = 0;

	_f_clustertopos(0, pos);
	
	for(;*path || *(path+1);) 
	{
		path = _f_getnextstring(currname,path);

		if((pos->cluster == 0) && (currname[0] == '.') && (!currname[1]) && (!currname[2]) && (!currname[3])) 
		{
			_f_clustertopos(0, pos);
			continue;
		}
		else 
		{
#if F_FINDING
			if(!_f_findlfilewc(currname, pos, &de, plfn, 0))
#else
			if(!_f_findlfilewc(currname, pos, &de, plfn))
#endif
			{
				if(currname) f_free(currname);
				if(oripath) f_free(oripath);
				if(plfn) f_free(plfn);
				return 0;
			}
			if(!(de->attr & F_ATTR_DIR )) 
			{
				if(currname) f_free(currname);
				if(oripath) f_free(oripath);
				if(plfn) f_free(plfn);
				return 0;
			}
			_f_clustertopos(_f_getdecluster(de), pos);
		}
		
		if(plfn->name[0] == '.' && (!plfn->name[1]))
		{
			if((!plfn->name[2]) && (!plfn->name[3])) 
			{
				continue; /*nothing to modify*/
			}
			else if(plfn->name[2]=='.' && (!plfn->name[3])) 
			{
				if(plfn->name[4] || plfn->name[5]) 
				{
					if(currname) f_free(currname);
					if(oripath) f_free(oripath);
					if(plfn) f_free(plfn);
					return 0; /*more than 2 dots*/
				}
				if(!pathpos) 
				{
					if(currname) f_free(currname);
					if(oripath) f_free(oripath);
					if(plfn) f_free(plfn);
					return 0; /*we are on the top*/
				}
				for(;pathpos;) 
				{		
					/*set back with one level*/
					pathpos -= 2;
					if (oripath[pathpos] == '/' && !oripath[pathpos+1]) break;
				}
			}
		}
		else 
		{
			if(pathpos) 
			{
				oripath[pathpos++] = '/';
				oripath[pathpos++] = 0;
			}
			for(a=0; ; ) 
			{
				char cl = plfn->name[a++];
				char ch = plfn->name[a++];
				if(!cl && !ch) break;
				oripath[pathpos++] = cl;
				oripath[pathpos++] = ch;
			}
		}
	}

	oripath[pathpos++] = 0; 
	oripath[pathpos] = 0;	/*terminates it;*/
	
	_memcpy(fsname->path, oripath, F_MAXPATH);

	if(currname) f_free(currname);
	if(oripath) f_free(oripath);
	if(plfn) f_free(plfn);
	return 1; /*found*/
#else	
	_f_clustertopos(0, pos);
	return 1;
#endif
}

/****************************************************************************
 *
 * _f_initentry
 *
 * init directory entry, this function is called if a new entry is coming
 *
 * INPUTS
 *
 * de - directory entry which needs to be initialized
 * name - fil ename  (8)
 * ext - file extension (3)
 *
 ***************************************************************************/
#if F_WRITING
void _f_initentry(F_DIRENTRY *de,char *name, char *ext) 
{
	_memset(de, 0, sizeof(F_DIRENTRY)); /*reset all entries*/

	_memcpy(de->name, name, sizeof(de->name));
	_memcpy(de->ext, ext, sizeof(de->ext));

	_f_setword(&de->cdate, f_getdate());  /*if there is realtime clock then creation date could be set from*/
	_f_setword(&de->ctime, f_gettime());  /*if there is realtime clock then creation time could be set from*/
}
#endif

/****************************************************************************
 *
 * _f_createlfn
 *
 * Create a raw lfn entry and short name,extension from long filename
 *
 * INPUTS
 *
 * lname - original long file name
 * lfn - lfn structure where to create raw lfn entry
 * name - where to create 8 bytes short name 
 * ext - where to create 3 bytes short extension 
 *
 * REMARKS
 *
 * Unicode supported
 *
 ***************************************************************************/
#if F_WRITING
static void _f_createlfn(char *lname, F_LFNINT *lfn, char *name, char *ext) 
{
	unsigned char a,b, flag = 0, alphabet = 0;
	unsigned short unichar, oemchar;	

	lfn->state = F_LFNSTATE_INV;
	lfn->ntres = 0;
	lfn->start = 0;
	lfn->end = 0;

	while(lname[lfn->end] || lname[lfn->end+1])
	{
		lfn->end += 2;
	}

	_memset(name, ' ', F_MAXNAME);
	_memset(ext, ' ', F_MAXEXT);

	for(a = lfn->end; a > 0; a -= 2) 
	{
		if(!lname[a-1] && lname[a-2] == '.') 
		{
			lfn->start = a - 2;
			for(b=0;;a+=2) 
			{
				/* create extension name */				
				unichar = lname[a] + ((unsigned short)lname[a+1]<<8);
				if(!unichar) break;
				if(F_IS_SKIPCHAR(unichar)) 
				{	
					flag = 1;
					continue;
				}
				if(F_IS_REPLACECHAR(unichar))
				{
					unichar = '_';
					flag = 1;
				}
				oemchar = f_unichar2oem(unichar);
				if(oemchar != unichar) flag = 1;
				if(oemchar>>8)
				{
					//double-byte character 
					if(b+2>F_MAXEXT) break;
					ext[b++] = oemchar >> 8;
					ext[b++] = oemchar & 0xFF;
				}
				else
				{
					ext[b++] = _f_toupper(oemchar & 0xFF);						
				}
				if(b == F_MAXEXT) 
				{
					if(!flag)
					{
						unichar = lname[a+2] + ((unsigned short)lname[a+3]<<8);
						if(unichar) flag = 1;
					}
					break;
				}
			}
			
			break;
		}
	}

	if(!lfn->start) lfn->start = lfn->end;
	
	for(a = b = 0; a < lfn->start; a += 2)
	{		
		/* create main name */ 
		unichar = lname[a] + ((unsigned short)lname[a+1]<<8);

		if(F_IS_SKIPCHAR(unichar)) 
		{	
			flag = 1;
			continue;
		}
		if(F_IS_REPLACECHAR(unichar)) 
		{
			unichar = '_';
			flag = 1;
		}
		oemchar = f_unichar2oem(unichar);
		if(oemchar != unichar) flag = 1;
		if(oemchar >> 8)
		{
			//double-byte character 
			if(b + 2 > F_MAXNAME) break;
			name[b++] = oemchar >> 8;
			name[b++] = oemchar & 0xFF;
		}
		else
		{
			name[b++] = _f_toupper(oemchar & 0xFF);
		}
		if (b == F_MAXNAME)
		{
			if(!flag)
			{
				if(a + 2 != lfn->start) flag = 1;
			}
			break;
		}
	}

	if(!flag)
	{
		for(a = 0; a < lfn->end; a+=2) 
		{
			if(a == lfn->start)
			{
				if(alphabet && !flag)
					lfn->ntres = lfn->ntres | NTRES_NAME_LOWER;
				alphabet = 0;
				flag = 0;
			}
			
			unichar = lname[a] + ((unsigned short)lname[a+1]<<8);

			if(F_IS_ALPHABET(unichar))
			{
				if(!alphabet) 
				{
					alphabet = 1;
					flag = F_IS_UPPERCASE(unichar);
				}
				else 
				{
					if(flag != F_IS_UPPERCASE(unichar)) break;				
				}
			}
		}

		if(a == lfn->end)
		{
			if (alphabet && !flag) {
				if(lfn->end == lfn->start) /* no ext */
					lfn->ntres = lfn->ntres | NTRES_NAME_LOWER;
				else
					lfn->ntres = lfn->ntres | NTRES_EXT_LOWER;
			}
			lfn->state = F_LFNSTATE_SFN;
			lfn->ord = 0; /*no lfn needed*/		
			return;
		}
	}

	lfn->state = F_LFNSTATE_LFN;
	if(lfn->end % 26)
	{
		lfn->end += 2; //double-null	
		lfn->ord = (unsigned char)((lfn->end+25)/26);
	}
	else
	{
		lfn->ord = (unsigned char)(lfn->end/26);
	}
}
#endif

/****************************************************************************
 * num2str
 *
 * Converts a number to string and puts '~' before it.
 * Needed to generate short filename that belongs to a long filename.
 *
 * INPUTS
 *
 * a - the number to convert
 * str - pointer to the string where to store the result
 *
 * OUTPUT
 *
 * length of the string
 *
 ***************************************************************************/
#if F_WRITING
static unsigned char num2str (unsigned short a, char *str)
{
	unsigned char b, c, rc;

	*str++ = '~';
	for(rc = 2, b = 100, c = a; b > 1;b /= 10)
	{
		unsigned char r = c / b;
		if(a / b) 
		{
			*str++ = r + '0';
			++rc;
			c -= r * b;
		}
	}
	*str = c + '0';
	return rc;
}
#endif

/****************************************************************************
 *
 * _f_addentry
 *
 * Add a new directory entry into driectory list
 *
 * INPUTS
 *
 * fsname - filled F_NAME structure 
 * pos - where directory cluster chains starts
 * pde - F_DIRENTRY pointer where to store the entry when it was added
 * plfn - F_LFNINT pointer where to store the long file name structure
 *
 * RETURNS
 *
 * 0 - if successfully added
 * other - if any error (see FS_xxx errorcodes)
 *
 * REMARKS
 *
 * Unicode supported
 *
 ***************************************************************************/
#if F_WRITING
unsigned char _f_addentry(F_NAME *fsname, F_POS *pos, F_DIRENTRY **pde, F_LFNINT *plfn) 
{
#if F_FAT32
	unsigned char fat32root = 0;
#endif
	char name[8];
	char ext[3];
	unsigned char ret;
	F_FILE *f;
	long num = 0;
	unsigned char len = 0;
	long start = -1;
	F_DIRENTRY de;
	
	if(!fsname->lname[0] && !fsname->lname[1]) return F_ERR_INVALIDNAME;
	if(fsname->lname[0] == '.'  && !fsname->lname[1]) return F_ERR_INVALIDNAME;

	_f_createlfn(fsname->lname, plfn, name, ext);

	if(plfn->state == F_LFNSTATE_INV) return F_ERR_INVALIDNAME;

#if F_FAT32
	if((!pos->cluster) && (gl_volume.mediatype==F_FAT32_MEDIA)) fat32root = 1;
#endif

	f = _f_opendir(pos->cluster);
	if(!f) return F_ERR_INVALIDDIR;

	while(1) 
	{
		ret = _f_readdir(&de, f);
		if(ret == F_ERR_EOF) 
		{
			unsigned short a;
			unsigned long cluster;

			if(!f || (!f->pos.cluster) || (f->pos.cluster >= F_CLUSTER_RESERVED))
			{
				return _f_closedir(f, F_ERR_NOMOREENTRY); /*EOF!! root FAT16,FAT12;*/
			}

			ret = _f_alloccluster(&cluster);	/*get a new one*/
			if(ret) return _f_closedir(f, ret);

			if(cluster < F_CLUSTER_RESERVED) 
			{
#if F_FAT32			
				if(fat32root) 
					ret = _f_setclustervalue(cluster, F_CLUSTER_LASTF32R);
				else 
#endif
					ret = _f_setclustervalue(cluster, F_CLUSTER_LAST);

				if(ret) return _f_closedir(f, ret);

				ret = _f_setclustervalue(f->pos.cluster, cluster);
				if(ret) return _f_closedir(f, ret);

				ret = _f_writefatsector();
				if(ret) return _f_closedir(f, ret);
			}
			else return _f_closedir(f, F_ERR_NOMOREENTRY);

			_memset(&de, 0, sizeof(F_DIRENTRY));

			a = gl_volume.bootrecord.sector_per_cluster;
			a *= F_SECTOR_SIZE/sizeof(F_DIRENTRY);
			while(a--) 
			{
				ret = _f_writedir(&de, f);
				if(ret) return _f_closedir(f, ret);
			}

			ret = _f_seekdir(f,num, F_SEEK_SET);
			if(ret) return _f_closedir(f, ret);

			ret = _f_readdir(&de, f); /*read new entry*/
		}

		if(ret) return _f_closedir(f, ret);

		if((!de.name[0]) || ((unsigned char)(de.name[0]) == 0xe5)) 
		{
			if(start == -1) 
			{
				start = num;
				len = 0;
			}
			len++;

			if(len - 1 == plfn->ord) 
			{
				if(plfn->state == F_LFNSTATE_SFN) 
				{
					/* only short directory entry */
					ret = _f_seekdir(f, start, F_SEEK_SET);
					if(ret) return _f_closedir(f, ret);

					_f_initentry(&de, name, ext);

					de.ntres = plfn->ntres;
					
#if F_FAT32
					if(gl_volume.mediatype == F_FAT32_MEDIA) 
					{
						_f_setword(&de.crtdate, f_getdate());  /*if there is realtime clock then creation date could be set from*/
						_f_setword(&de.crttime, f_gettime());  /*if there is realtime clock then creation time could be set from*/
						_f_setword(&de.lastaccessdate, f_getdate());  /*if there is realtime clock then creation date could be set from*/
					}
#endif
					ret = _f_writedir(&de, f);
					if(ret) return _f_closedir(f, ret);
				}
				else 
				{
					unsigned short a;
					unsigned char b, c, ok;
					F_LFN *plde = (F_LFN *)(&de);

					for(c = 8; c > 0; c--)
					{
						if(name[c-1] != ' ') break;
					}

					if(c > 6)
					{
						c = 6;
#if F_DBCS
						if(name[c-1] > 0x80) c -= 1;
#endif
					}
					

					for(a = 1; ; a++) 
					{
						unsigned char n;
						char str_n[4];

						if(a > 999) return _f_closedir(f, F_ERR_NOMOREENTRY);
						n = num2str(a, str_n);
						if(c > (unsigned long)(8-n)) 
						{
							c = 8-n;
#if F_DBCS
							if(name[c-1] > 0x80) c -= 1;
#endif
						}
						
						_memcpy(name+c, str_n, n);

						ret = _f_seekdir(f, 0, F_SEEK_SET);
						if(ret) return _f_closedir(f, ret);

						for ( ; ; ) 
						{
							ok = 1;
							ret = _f_readdir(&de, f);
							if(ret == F_ERR_EOF) break;
							if(ret) return _f_closedir(f, ret);

							if((!de.name[0]) || ((unsigned char)(de.name[0]) == 0xe5)) continue;

							for(b = 0; b < 8; b++) 
							{
								if(de.name[b] != name[b]) 
								{
									ok = 0;
									break;
								}
							}
							if(ok) 
							{
								for(b = 0; b < 3; b++) 
								{
									if (de.ext[b] != ext[b]) 
									{
										ok = 0;
										break;
									}
								}
							}

							if(ok) 
							{ 
								/*if its found*/
								ok = 0; /*name is not ok*/
								break;
							}
						}

						if(ok) break;
					}

					ret = _f_seekdir(f, start, F_SEEK_SET);
					if(ret) return _f_closedir(f, ret);

					{
						unsigned char chksum = 0;

						for(a = 0; a < 8; a++) 
						{	
							chksum = (((chksum&1)<<7) | (( chksum & 0xfe) >>1)) + name[a];
						}	   

						for(a = 0; a < 3; a++) 
						{	
							chksum = (((chksum&1)<<7) | (( chksum & 0xfe) >>1)) + ext[a];
						}	   
						plfn->chksum = chksum;
					}

					for(a = plfn->ord; a > 0; a--) 
					{
						unsigned long npos = (a-1)*26;
						_memset(plde, 0, sizeof(F_LFN));

						if(a == plfn->ord) plde->ord = (unsigned char)(a + 0x40);
						else plde->ord = (unsigned char)a;

						plde->chksum = plfn->chksum;

						{
							unsigned char cpos[26] = {1,2,3,4,5,6,7,8,9,10,14,15,16,17,18,19,20,21,22,23,24,25,28,29,30,31};
							char *bb = &fsname->lname[npos];
							unsigned char i;

							for(i = 0; i < 26; i++)
							{
								char *cc = (char *)plde + cpos[i];
								if(npos + i < plfn->end) *cc = *bb++;
								else *cc = (char)0xFF;
							}
						}

						plde->attr = F_ATTR_LFN;

						ret = _f_writedir(&de,f);
						if(ret) return _f_closedir(f,ret);
					}

					_f_initentry(&de, name, ext); 
#if F_FAT32
					if (gl_volume.mediatype == F_FAT32_MEDIA) 
					{
						_f_setword(&de.crtdate, f_getdate());  /*if there is realtime clock then creation date could be set from*/
						_f_setword(&de.crttime, f_gettime());  /*if there is realtime clock then creation time could be set from*/
						_f_setword(&de.lastaccessdate, f_getdate());  /*if there is realtime clock then creation date could be set from*/
					}
#endif
					ret = _f_writedir(&de, f);
					if(ret) return _f_closedir(f, ret);
				}

				if (pde) 
				{
					if(!f) return _f_closedir(f, F_ERR_NOTOPEN);

					if(f->relpos >= sizeof(F_DIRENTRY)) 
					{
						gl_volume.direntrysector = f->pos.sector;
						_memcpy(gl_volume.direntry, f->_tdata, F_SECTOR_SIZE);
						*pde = (F_DIRENTRY*)(gl_volume.direntry + f->relpos - sizeof(F_DIRENTRY));

						pos->cluster = f->pos.cluster;
						pos->pos = f->relpos / sizeof(F_DIRENTRY) - 1;
						pos->sector = f->pos.sector;
						pos->sectorend = f->pos.sectorend;
					}
					else return _f_closedir(f, F_ERR_NOTOPEN);
				}

				return _f_closedir(f, F_NO_ERROR); /*if ok*/
			}
		}
		else start = -1;

		num++;
	}
}
#endif

/****************************************************************************
 *
 * fn_getcwd
 *
 * getting a current working directory of current drive
 *
 * INPUTS
 *
 * buffer - where to store current working folder
 * maxlen - buffer length (possible size is F_MAXPATH)
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 * REMARKS
 * 
 * Unicode supported
 *
 ***************************************************************************/
#if F_DIRECTORIES
unsigned char fn_getcwd(char *buffer, unsigned long maxlen, char root) 
{
	unsigned char a;

	if(!maxlen) return F_NO_ERROR;

	maxlen -= 2;	/*need for termination*/
	if(root && maxlen)
	{
		*buffer++ = '/';
		*buffer++ = 0;
		maxlen -= 2;
	}

	for(a = 0; a < maxlen; a += 2) 
	{
		char cl = gl_volume.cwd[a];
		char ch = gl_volume.cwd[a+1];
		buffer[a] = cl;
		buffer[a+1] = ch;
		if(!cl && !ch) break;
	}

	buffer[a] = 0;	
	buffer[a+1] = 0;	/*add terminator at the end*/

	return F_NO_ERROR;
}

#endif

/****************************************************************************
 *
 * fn_findfirst
 *
 * find a file(s) or directory(s) in directory
 *
 * INPUTS
 *
 * filename - filename (with or without wildcards)
 * find - where to store found file information
 *
 * RETURNS
 *																			 
 * error code or zero if successful
 *																			 
 ***************************************************************************/
#if F_FINDING
unsigned char fn_findfirst(const char *filename, F_FIND *find) 
{
	unsigned char ret;

	if(_f_setfsname(filename, &find->findfsname)) return F_ERR_INVALIDNAME; /*invalid name*/
	
#if F_CHECKNAME
	if(_f_checknamewc(find->findfsname.lname, 0)) return F_ERR_INVALIDNAME;/*invalid name, wildcard is ok*/
#endif

	ret = _f_getvolume();
	if(ret) return ret;

	if(!_f_findpath(&find->findfsname, &find->pos)) return F_ERR_INVALIDDIR; /*search for path*/

	return fn_findnext(find);
}
#endif

/****************************************************************************
 *
 * fn_findnext
 *
 * find further file(s) or directory(s) in directory
 *
 * INPUTS
 *
 * find - where to store found file information (findfirst should call 1st)
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/
#if F_FINDING
unsigned char fn_findnext(F_FIND *find) 
{
	F_LFNINT lfn;
	F_DIRENTRY *de;
	unsigned char a;
	unsigned char ret;

	ret = _f_getvolume();
	if(ret) return ret;

	if(!_f_findlfilewc(find->findfsname.lname, &find->pos, &de, &lfn, 1)) return F_ERR_NOTFOUND;

	for(a = 0; a < F_MAXNAME; a++) find->name[a] = de->name[a];
	for(a = 0; a < F_MAXEXT; a++) find->ext[a] = de->ext[a];

	_memcpy(find->filename, lfn.name, F_MAXLNAME);

	find->attr = de->attr;
	find->cdate = _f_getword(&de->cdate);
	find->ctime = _f_getword(&de->ctime);
	find->filesize = _f_getlong(&de->filesize);
	find->cluster = _f_getdecluster(de);
	find->pos.pos++; /*goto next position*/

	return 0;
}
#endif

/****************************************************************************
 *
 * fn_chdir
 *
 * change current working directory
 *
 * INPUTS
 *
 * dirname - new working directory name
 *
 * RETURNS
 *
 * 0 - if successfully
 * other - if any error 
 *
 * REMARKS
 *
 * Unicode supported
 *
 ***************************************************************************/
#if (F_CHDIR && F_DIRECTORIES)
unsigned char fn_chdir(const char *dirname) 
{
	F_POS pos;
	F_NAME *fsname = NULL;
	unsigned short len;
	unsigned short a;
	unsigned char ret;

	ret = _f_getvolume();
	if(ret) return ret;

	fsname = (F_NAME *)f_malloc(sizeof(F_NAME));
	if(!fsname) return F_ERR_NULLPOINTER;
	
	ret = _f_setfsname(dirname, fsname);
	if(ret == 1) 
	{
		if(fsname) f_free(fsname);
		return F_ERR_INVALIDNAME; /*invalid name*/
	}
#if F_CHECKNAME
	if(_f_checknamewc(fsname->lname, 1)) 
	{
		if(fsname) f_free(fsname);
		return F_ERR_INVALIDNAME; /*invalid name*/
	}
#endif

	for(len = 0; fsname->path[len] || fsname->path[len+1]; ) len += 2;
	if(len) 
	{
		fsname->path[len++] = '/';
		fsname->path[len++] = 0;
	}

	for(a=0; ; ) 
	{
		char cl = fsname->lname[a++];
		char ch = fsname->lname[a++];
		fsname->path[len++] = cl;
		fsname->path[len++] = ch;
		if(!ch && !cl) break;
	}

	if(!(_f_findpath(fsname, &pos))) 
	{
		if(fsname) f_free(fsname);
		return F_ERR_NOTFOUND;
	}

	for(a = 0; a < F_MAXPATH; a++) gl_volume.cwd[a] = fsname->path[a];

	if(fsname) f_free(fsname);
	
	return F_NO_ERROR;
}
#endif

/****************************************************************************
 *
 * fn_mkdir
 *
 * making a new directory
 *
 * INPUTS
 *
 * dirname - new directory name
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 * REMARKS
 *
 * Unicode supported
 *
 ***************************************************************************/
#if (F_MKDIR && F_DIRECTORIES && F_WRITING)
unsigned char fn_mkdir(const char *dirname) 
{
	F_LFNINT *plfn = NULL;
	F_NAME *fsname = NULL;
	F_FILE *f;
	F_POS posdir;
	F_POS pos;
	F_DIRENTRY *de;	
	unsigned long cluster;
	unsigned char ret;

	ret = _f_getvolume();
	if(ret) return ret;

	plfn = (F_LFNINT *)f_malloc(sizeof(F_LFNINT));
	fsname = (F_NAME *)f_malloc(sizeof(F_NAME));
	if(!plfn || !fsname)
	{
		if(plfn) f_free(plfn);
		if(fsname) f_free(fsname);
		return F_ERR_NULLPOINTER;
	}

	if(_f_setfsname(dirname, fsname)) 
	{
		if(plfn) f_free(plfn);
		if(fsname) f_free(fsname);
		return F_ERR_INVALIDNAME; /*invalid name*/
	}
	
#if F_CHECKNAME
	if(_f_checknamewc(fsname->lname, 1)) 
	{
		if(plfn) f_free(plfn);
		if(fsname) f_free(fsname);
		return F_ERR_INVALIDNAME;/*invalid name*/
	}
	if(fsname->lname[0] == '.' && !fsname->lname[1]) 
	{
		if(plfn) f_free(plfn);
		if(fsname) f_free(fsname);
		return F_ERR_INVALIDNAME;
	}
#endif

	if(!_f_findpath(fsname, &posdir)) 
	{
		if(plfn) f_free(plfn);
		if(fsname) f_free(fsname);
		return F_ERR_INVALIDDIR;
	}

	pos = posdir;

#if F_FINDING
	if(_f_findlfilewc(fsname->lname, &pos, &de, plfn, 0)) 
#else
	if(_f_findlfilewc(fsname->lname, &pos, &de, plfn))
#endif
	{
		if(plfn) f_free(plfn);
		if(fsname) f_free(fsname);
		return F_ERR_DUPLICATED;
	}

	pos = posdir;

	ret = _f_addentry(fsname, &pos, &de, plfn);
	if(plfn) f_free(plfn);
	if(fsname) f_free(fsname);
	if(ret) return ret;	
	
	de->attr |= F_ATTR_DIR; 	/*set as directory*/

	f = _f_getintfile();

	if(!f) return F_ERR_NOMOREENTRY;

	ret = _f_alloccluster(&cluster);
	if(ret) return ret;

	_f_clustertopos(cluster, &f->pos);

	_f_setdecluster(de, cluster); /*new dir*/

	de = (F_DIRENTRY *)(f->_tdata);

	_f_initentry(de, ".       ", "   ");
	de->attr = F_ATTR_DIR;		/*set as directory*/
	_f_setdecluster(de, cluster);	 /*current*/
	de++;

	_f_initentry(de, "..      ", "   ");
	de->attr = F_ATTR_DIR;		/*set as directory*/
	_f_setdecluster(de, posdir.cluster); /*parent*/
	de++;

	_memset(de, 0, F_SECTOR_SIZE - 2 * sizeof(F_DIRENTRY));

	ret = _f_writesector(f->_tdata, f->pos.sector);
	if(ret) return ret;

	f->pos.sector++;

	_memset(f->_tdata, 0, 2 * sizeof(F_DIRENTRY));

	while(f->pos.sector < f->pos.sectorend) 
	{
write_again:
		ret = _f_writesector(f->_tdata, f->pos.sector);
		if(ret) 
		{
			if(gl_volume.state != F_STATE_WORKING) return ret; /*check if still working volume*/

			ret = _f_dobadblock(f);
			if(ret) return ret;

			goto write_again;
		}

		f->pos.sector++;
	}

	ret = _f_setclustervalue(f->pos.cluster, F_CLUSTER_LAST);
	if(ret) return ret;

	ret = _f_writefatsector();
	if(ret) return ret;

	return _f_writedirsector(); /*writing  direntry*/
}
#endif

/****************************************************************************
 *
 * fn_rmdir
 *
 * Remove directory, only could be removed if empty
 *
 * INPUTS
 *
 * dirname - which directory needed to be removed
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 * REMARKS
 *
 * Unicode supported
 *
 ***************************************************************************/
#if (F_RMDIR && F_DIRECTORIES && F_WRITING)
unsigned char fn_rmdir(const char *dirname) 
{
	F_LFNINT *plfn = NULL;
	F_NAME *fsname = NULL;
	F_POS dirpos,pos;
	F_DIRENTRY *pde;
	F_FILE *file;
	unsigned long dircluster;
	unsigned char ret;
	unsigned short a;
	char *pcwd;

	ret = _f_getvolume();
	if(ret) return ret;

	plfn = (F_LFNINT *)f_malloc(sizeof(F_LFNINT));
	fsname = (F_NAME *)f_malloc(sizeof(F_NAME));
	if(!plfn || !fsname)
	{
		if(plfn) f_free(plfn);
		if(fsname) f_free(fsname);
		return F_ERR_NULLPOINTER;
	}

	if(_f_setfsname(dirname, fsname)) 
	{
		if(plfn) f_free(plfn);
		if(fsname) f_free(fsname);
		return F_ERR_INVALIDNAME; /*invalid name*/
	}
	
#if F_CHECKNAME
	if(_f_checknamewc(fsname->lname, 1)) 
	{	
		if(plfn) f_free(plfn);
		if(fsname) f_free(fsname);
		return F_ERR_INVALIDNAME; /*invalid name*/
	}
	if(fsname->lname[0] == '.' && !fsname->lname[1]) 
	{	
		if(plfn) f_free(plfn);
		if(fsname) f_free(fsname);
		return F_ERR_NOTFOUND;
	}
#endif

	/* check if is cwd */
	for(a = 0, pcwd = gl_volume.cwd; ; a += 2, pcwd += 2)
	{
		char cl = fsname->path[a];
		char ch = fsname->path[a+1];
		char cwdcl = *pcwd;
		char cwdch = *(pcwd+1);

		if(!ch)
			cl = _f_toupper(cl);
		if(!cwdch)
			cwdcl = _f_toupper(cwdcl);			
		
		if (cl != cwdcl || ch != cwdch) {
			if ((!cl && !ch) && ((cwdcl == '/' && !cwdch) || a == 0)) {
				/* compare fsname->lname with residual part of cwd */
				unsigned short b;
				if(a)
					pcwd += 2; /* skip slash */
				for (b = 0; ; b += 2, pcwd += 2) {
					cl = fsname->lname[b];
					ch = fsname->lname[b+1];
					cwdcl = *pcwd;
					cwdch = *(pcwd+1);

					if(!ch)
						cl = _f_toupper(cl);
					if(!cwdch)
						cwdcl = _f_toupper(cwdcl);
					if (cl != cwdcl || ch != cwdch) {
						break;
					}

					if (!cl && !ch) {
						if (plfn) 
							f_free(plfn);
						if (fsname) 
							f_free(fsname);
						return F_ERR_LOCKED; /*can not remove if is cwd */
					}				
				}
			}
			else
				break;
		}
	}

	if(!(_f_findpath(fsname, &dirpos))) 
	{	
		if(plfn) f_free(plfn);
		if(fsname) f_free(fsname);
		return F_ERR_INVALIDDIR;
	}

	_memcpy(&pos, &dirpos, sizeof(F_POS)); /*copy directory pos*/

#if F_FINDING
	if(!_f_findlfilewc(fsname->lname, &pos, &pde, plfn, 0)) 
#else
	if(!_f_findlfilewc(fsname->lname, &pos, &pde, plfn)) 
#endif
	{
		if(plfn) f_free(plfn);
		if(fsname) f_free(fsname);
		return F_ERR_NOTFOUND;
	}

	if(!(pde->attr & F_ATTR_DIR)) 
	{
		if(plfn) f_free(plfn);
		if(fsname) f_free(fsname);
		return F_ERR_INVALIDDIR; 	   /*not a directory*/
	}

	dircluster = _f_getdecluster(pde);

	file = _f_opendir(dircluster);
	if(!file) 
	{
		if(plfn) f_free(plfn);
		if(fsname) f_free(fsname);
		return F_ERR_INVALIDDIR;
	}

	while(1) 
	{
		/*check empty*/
		F_DIRENTRY de;
		char ch;

		ret = _f_readdir(&de, file);
		if(ret == F_ERR_EOF) break;
		if(ret) 
		{	
			if(plfn) f_free(plfn);
			if(fsname) f_free(fsname);
			return _f_closedir(file, ret);
		}
		ch = de.name[0];
		if(!ch) break;  /*last entry*/
		if((unsigned char)ch == 0xe5) continue; /*deleted*/
		if(ch == '.') continue;
		
		if(plfn) f_free(plfn);
		if(fsname) f_free(fsname);
		return _f_closedir(file, F_ERR_NOTEMPTY);/*something is there*/
	}

	ret = _f_closedir(file, F_NO_ERROR);
	if(ret) 
	{	
		if(plfn) f_free(plfn);
		if(fsname) f_free(fsname);
		return ret;
	}

	ret = _f_removelfn(dirpos.cluster, plfn);
	if(plfn) f_free(plfn);
	if(fsname) f_free(fsname);
	if(ret) return ret;

	return _f_removechain(dircluster);
}
#endif

/****************************************************************************
 *
 * fn_settimedate
 *
 * set a file time and date
 *
 * INPUTS
 *
 * filename - which file time and date wanted to be set
 * ctime - new ctime of the file
 * cdate - new cdate of the file
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/
#if (F_SETTIMEDATE && F_WRITING)
unsigned char fn_settimedate(const char *filename, unsigned short ctime, unsigned short cdate) 
{
	F_LFNINT *plfn = NULL;
	F_NAME *fsname = NULL;
	F_POS pos;
	F_DIRENTRY *de;	
	unsigned char ret;

	ret = _f_getvolume();
	if(ret) return ret;

	plfn = (F_LFNINT *)f_malloc(sizeof(F_LFNINT));
	fsname = (F_NAME *)f_malloc(sizeof(F_NAME));
	if(!plfn || !fsname)
	{
		if(plfn) f_free(plfn);
		if(fsname) f_free(fsname);
		return F_ERR_NULLPOINTER;
	}

	if(_f_setfsname(filename, fsname)) 
	{	
		if(plfn) f_free(plfn);
		if(fsname) f_free(fsname);
		return F_ERR_INVALIDNAME; /*invalid name*/
	}
#if F_CHECKNAME
	if(_f_checknamewc(fsname->lname, 1)) 
	{	
		if(plfn) f_free(plfn);
		if(fsname) f_free(fsname);
		return F_ERR_INVALIDNAME;/*invalid name*/
	}
#endif

	if(!_f_findpath(fsname, &pos)) 
	{	
		if(plfn) f_free(plfn);
		if(fsname) f_free(fsname);
		return F_ERR_INVALIDDIR;
	}
	
#if F_FINDING
	if(!_f_findlfilewc(fsname->lname, &pos, &de, plfn, 0)) 
#else
	if(!_f_findlfilewc(fsname->lname, &pos, &de, plfn))
#endif
	{	
		if(plfn) f_free(plfn);
		if(fsname) f_free(fsname);
		return F_ERR_NOTFOUND;
	}

	_f_setword(&de->ctime, ctime);
	_f_setword(&de->cdate, cdate);

	if(plfn) f_free(plfn);
	if(fsname) f_free(fsname);

	return _f_writedirsector();
}
#endif

/****************************************************************************
 *
 * fn_gettimedate
 *
 * get a file time and date
 *
 * INPUTS
 *
 * filename - which file time and date wanted to be retrive
 * pctime - ctime of the file where to store
 * pcdate - cdate of the file where to store
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/
#if F_GETTIMEDATE
unsigned char fn_gettimedate(const char *filename, unsigned short *pctime, unsigned short *pcdate) 
{
	F_LFNINT *plfn = NULL;
	F_NAME *fsname = NULL;	
	F_POS pos;
	F_DIRENTRY *de;
	unsigned char ret;
	
	ret = _f_getvolume();
	if(ret) return ret;

	plfn = (F_LFNINT *)f_malloc(sizeof(F_LFNINT));
	fsname = (F_NAME *)f_malloc(sizeof(F_NAME));
	if(!plfn || !fsname)
	{
		if(plfn) f_free(plfn);
		if(fsname) f_free(fsname);
		return F_ERR_NULLPOINTER;
	}

	if(_f_setfsname(filename, fsname)) 
	{	
		if(plfn) f_free(plfn);
		if(fsname) f_free(fsname);
		return F_ERR_INVALIDNAME; /*invalid name*/
	}
#if F_CHECKNAME
	if(_f_checknamewc(fsname->lname, 1)) 
	{	
		if(plfn) f_free(plfn);
		if(fsname) f_free(fsname);
		return F_ERR_INVALIDNAME;/*invalid name*/
	}
#endif

	if(!_f_findpath(fsname, &pos)) 
	{	
		if(plfn) f_free(plfn);
		if(fsname) f_free(fsname);
		return F_ERR_INVALIDDIR;
	}
#if F_FINDING
	if(!_f_findlfilewc(fsname->lname, &pos, &de, plfn, 0)) 
#else
	if(!_f_findlfilewc(fsname->lname, &pos, &de, plfn)) 
#endif
	{
		if(plfn) f_free(plfn);
		if(fsname) f_free(fsname);
		return F_ERR_NOTFOUND;
	}

	if(pctime) *pctime = _f_getword(&de->ctime);
	if(pcdate) *pcdate = _f_getword(&de->cdate);

	if(plfn) f_free(plfn);
	if(fsname) f_free(fsname);

	return F_NO_ERROR;
}
#endif

/****************************************************************************
 *
 * fn_getattr
 *
 * get file attribute
 *
 * INPUTS
 *
 * filename - which file attribute is needed
 * attr - pointer to a characterter where to store attribute information
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/
#if F_GETATTR
unsigned char fn_getattr(const char *filename, unsigned char *attr) 
{
	F_LFNINT *plfn = NULL;
	F_NAME *fsname = NULL;
	F_POS pos;
	F_DIRENTRY *de;	
	unsigned char ret;
	
	ret = _f_getvolume();
	if(ret) return ret;

	plfn = (F_LFNINT *)f_malloc(sizeof(F_LFNINT));
	fsname = (F_NAME *)f_malloc(sizeof(F_NAME));
	if(!plfn || !fsname)
	{
		if(plfn) f_free(plfn);
		if(fsname) f_free(fsname);
		return F_ERR_NULLPOINTER;
	}

	if(_f_setfsname(filename, fsname)) 
	{	
		if(plfn) f_free(plfn);
		if(fsname) f_free(fsname);
		return F_ERR_INVALIDNAME; /*invalid name*/
	}
	
#if F_CHECKNAME
	if(_f_checknamewc(fsname->lname, 1)) 
	{	
		if(plfn) f_free(plfn);
		if(fsname) f_free(fsname);
		return F_ERR_INVALIDNAME;/*invalid name*/
	}
#endif

	if(!_f_findpath(fsname, &pos)) 
	{	
		if(plfn) f_free(plfn);
		if(fsname) f_free(fsname);
		return F_ERR_INVALIDDIR;
	}
#if F_FINDING
	if(!_f_findlfilewc(fsname->lname, &pos, &de, plfn, 0))
#else
	if(!_f_findlfilewc(fsname->lname, &pos, &de, plfn)) 
#endif
	{
		if(plfn) f_free(plfn);
		if(fsname) f_free(fsname);
		return F_ERR_NOTFOUND;
	}

	if(attr) *attr=de->attr;

	if(plfn) f_free(plfn);
	if(fsname) f_free(fsname);

	return F_NO_ERROR;
}

#endif

/****************************************************************************
 *
 * fn_setattr
 *
 * set attribute of a file
 *
 * INPUTS
 *
 * filename - which file attribute set
 * attr - new attribute of the file
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/
#if (F_SETATTR && F_WRITING)
unsigned char fn_setattr(const char *filename,unsigned char attr) 
{
	F_LFNINT *plfn = NULL;
	F_NAME *fsname = NULL;
	F_POS pos;
	F_DIRENTRY *de;	
	unsigned char ret;

	ret = _f_getvolume();
	if(ret) return ret;

	plfn = (F_LFNINT *)f_malloc(sizeof(F_LFNINT));
	fsname = (F_NAME *)f_malloc(sizeof(F_NAME));
	if(!plfn || !fsname)
	{
		if(plfn) f_free(plfn);
		if(fsname) f_free(fsname);
		return F_ERR_NULLPOINTER;
	}

	if(_f_setfsname(filename, fsname)) 
	{	
		if(plfn) f_free(plfn);
		if(fsname) f_free(fsname);
		return F_ERR_INVALIDNAME; /*invalid name*/
	}
#if F_CHECKNAME
	if(_f_checknamewc(fsname->lname, 1)) 
	{	
		if(plfn) f_free(plfn);
		if(fsname) f_free(fsname);
		return F_ERR_INVALIDNAME;/*invalid name*/
	}
#endif

	if(!_f_findpath(fsname, &pos)) 
	{
		if(plfn) f_free(plfn);
		if(fsname) f_free(fsname);
		return F_ERR_INVALIDDIR;
	}

#if F_FINDING
	if(!_f_findlfilewc(fsname->lname, &pos, &de, plfn, 0)) 
#else
	if(!_f_findlfilewc(fsname->lname, &pos, &de, plfn)) 
#endif
	{
		if(plfn) f_free(plfn);
		if(fsname) f_free(fsname);
		return F_ERR_NOTFOUND;
	}

	if(plfn) f_free(plfn);
	if(fsname) f_free(fsname);

	attr &= F_ATTR_HIDDEN | F_ATTR_READONLY | F_ATTR_ARC | F_ATTR_SYSTEM; /*keep only valid bits*/

	de->attr &= F_ATTR_DIR | F_ATTR_VOLUME; /*keep some original*/

	attr |= de->attr;	/*add old settings, (dir,volume)*/

	if(de->attr == attr) return F_NO_ERROR; /*its the same*/

	de->attr = attr; 

	return _f_writedirsector();
}
#endif

#endif   /* F_LONGFILENAME */

