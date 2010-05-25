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

#include "file.h"
#include "util.h"
#include "volume.h"
#include "drv.h"
#include "fat.h"
#include "tcache.h"

#if F_LONGFILENAME
#include "dir_lfn.h"
#else
#include "dir.h"
#endif

#include "mmd_data_type.h"
#include "aeestdlib.h"
/****************************************************************************
 *
 * _f_checklocked
 *
 * check if a given file is locked or not
 *
 * INPUTS
 * pos - pos structure contains directory entry pos
 *
 * RETURNS
 * zero - if not locked
 * F_ERR_LOCKED - if locked
 *
 ***************************************************************************/
#if F_WRITING
static unsigned char _f_checklocked(F_POS *pos) 
{
	unsigned char a;
	F_FILE *f;
	
	for (a=0; a<F_MAXFILES; a++) 
	{
		f=&gl_files[a];

		if (f->mode!=F_FILE_CLOSE) 		
		{
			if (f->dirpos.sector==pos->sector && f->dirpos.pos==pos->pos) 
				return F_ERR_LOCKED;
		}
	}
	return F_NO_ERROR;
}
#endif

/****************************************************************************
 * _f_updatefileentry
 * Updated a file directory entry or removes the entry 
 * and the fat chain belonging to it.
 ***************************************************************************/
#if F_WRITING
static unsigned char _f_updatefileentry (F_FILE *f, int remove)
{
	F_DIRENTRY *de;

	de=(F_DIRENTRY*)(gl_volume.direntry+sizeof(F_DIRENTRY)*f->dirpos.pos);

	if (_f_getdirsector(f->dirpos.sector) || remove) 
	{
		(void)_f_removechain(f->startcluster); 
		_f_setdecluster(de,0);
		_f_setlong(&de->filesize,0);
		(void)_f_writedirsector();
		return F_ERR_WRITE;
	}

	_f_setdecluster(de,f->startcluster);
	_f_setlong(&de->filesize,f->filesize);
	_f_setword(&de->cdate,f_getdate());  /*if there is realtime clock then creation date could be set from*/
	_f_setword(&de->ctime,f_gettime());  /*if there is realtime clock then creation time could be set from*/

#if F_FAT32
	if (gl_volume.mediatype==F_FAT32_MEDIA) 
	{
		_f_setword(&de->lastaccessdate,f_getdate());  /*if there is realtime clock then creation date could be set from*/
	}
#endif
	return _f_writedirsector();
}
#endif

/****************************************************************************
 *
 * _f_emptywritebuffer
 *
 * empty write buffer if it contains unwritten data
 *
 * INPUTS
 *
 * f - internal file pointer
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/
#if F_WRITING
static unsigned char _f_emptywritebuffer(F_FILE *f, void *ptr) 
{
	unsigned char ret;
	
	if (!f->modified) return F_NO_ERROR; /*nothing to write*/
	if (f->pos.sector >= f->pos.sectorend) 
	{
		
		if (!f->startcluster) /* empty file */
		{
			ret = _f_alloccluster(&f->startcluster);
			if (ret) return ret;
			
			_f_clustertopos(f->startcluster, &f->pos);

			ret=_f_setclustervalue(f->startcluster, F_CLUSTER_LAST);
			if (ret) return ret;
		}
		else /* end of cluster */
		{
			unsigned long value;
			
			ret = _f_getclustervalue(f->pos.cluster, &value);
			if (ret) return ret;

			if (value < F_CLUSTER_RESERVED) 
			{  
				/*we are in chain*/
				f->prevcluster = f->pos.cluster;
				_f_clustertopos(value, &f->pos); /*go to next cluster*/
			}
			else 
			{
				unsigned long nextcluster;

				ret = _f_alloccluster(&nextcluster);
				if (ret) return ret;

				ret = _f_setclustervalue(nextcluster, F_CLUSTER_LAST);
				if (ret) return ret;

				ret = _f_setclustervalue(f->pos.cluster, nextcluster);
				if (ret) return ret;

				f->prevcluster = f->pos.cluster;

				_f_clustertopos(nextcluster, &f->pos);
			}
		}
	}

	if (ptr == NULL) ptr = f->_tdata;
	
write_again:
	ret = _f_writesector(ptr, f->pos.sector);
	if (ret) 
	{
		if (gl_volume.state != F_STATE_WORKING) return ret; /*check if still working volume*/

		ret = _f_dobadblock(f);
		if (ret) return ret;

		if (!f->prevcluster) 
		{
			f->startcluster = f->pos.cluster;
		}
		else 
		{
			ret = _f_setclustervalue(f->prevcluster, f->pos.cluster);
			if (ret) return ret;
		}

		goto write_again;
	}

	f->modified = 0;
	f->cached = 0;
	return F_NO_ERROR;
}
#endif

/****************************************************************************
 *
 * _f_emptymultibuffer
 *
 * empty buffer of unwritten data by writting multi-sector
 *
 * INPUTS
 *
 * f - internal file pointer
 * ptr - pointer to buffer of  unwritten data
 * max - maximum sectors to write
 * size - where to store number of written bytes
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/
#if (F_MULTIWRITE && F_WRITING)
static unsigned char _f_emptymultibuffer(F_FILE *f, void *ptr, unsigned int max, unsigned long *size) 
{
	unsigned char ret;
	unsigned long cluster, precluster;
	unsigned int num = 0;
	F_POS pos;
	
	if(!f->modified) return F_NO_ERROR; /*nothing to write*/

	if(ptr == NULL) 
	{	
		/* flush the f->_tdata buffer */
		ptr = f->_tdata;
		max = 1;
	}

	while(num < max)
	{
		if(f->pos.sector >= f->pos.sectorend) 
		{
			/* end of cluster */
			if(!f->startcluster) 
			{
				ret = _f_alloccluster(&f->startcluster);
				if(ret) return ret;
				
				_f_clustertopos(f->startcluster, &f->pos);

				ret = _f_setclustervalue(f->startcluster, F_CLUSTER_LAST);
				if(ret) return ret;
			}
			else 
			{
				ret = _f_getclustervalue(f->pos.cluster, &cluster);
				if(ret) return ret;

				if(cluster < F_CLUSTER_RESERVED) 
				{  
					/*we are in chain*/
					if(num && (f->pos.cluster != cluster-1)) break;

					f->prevcluster = f->pos.cluster;
					_f_clustertopos(cluster, &f->pos); /*go to next cluster*/
				}
				else 
				{
					ret = _f_alloccluster(&cluster);
					if(ret) return ret;
					
					if(num && (f->pos.cluster != cluster-1))
					{
						gl_volume.lastalloccluster = cluster;
						break;
					}
					
					ret = _f_setclustervalue(cluster, F_CLUSTER_LAST);
					if(ret) return ret;

					ret = _f_setclustervalue(f->pos.cluster, cluster);
					if(ret) return ret;

					f->prevcluster = f->pos.cluster;

					_f_clustertopos(cluster, &f->pos);					
				}
			}
		}

		if(!num && (max > 1)) 
		{
			/* backup current pos only for multi-sector write */
			_memcpy(&pos, &f->pos, sizeof(F_POS));
			precluster = f->prevcluster;
		}

		num++;		
		f->pos.sector++;
	}

	f->pos.sector--; /* move back to current sector */

write_again:
	if(num > 1)
	{
		ret = _f_writemultisector(ptr, pos.sector, num);
		if(ret)
		{
			/* restore backup pos */
			f->prevcluster = precluster;
			_memcpy(&f->pos, &pos, sizeof(F_POS));
			num = 1;
			ret = _f_writesector(ptr, f->pos.sector);
		}
	}
	else
	{
		ret = _f_writesector(ptr, f->pos.sector);
	}
	
	if(ret) 
	{
		if(gl_volume.state != F_STATE_WORKING) return ret; /*check if still working volume*/

		ret = _f_dobadblock(f);
		if(ret) return ret;

		if(!f->prevcluster) 
		{
			f->startcluster = f->pos.cluster;
		}
		else 
		{
			ret = _f_setclustervalue(f->prevcluster, f->pos.cluster);
			if(ret) return ret;
		}

		goto write_again;
	}

	f->modified = 0;
	f->cached = 0;

	if(size) *size = num * F_SECTOR_SIZE;

	return F_NO_ERROR;
}
#endif

/****************************************************************************
 *
 * _f_extend
 *
 * Extend file with a certain size from current EOF
 *
 ***************************************************************************/
#if (F_WRITING && (F_SEEK || F_SETEOF))
static unsigned char _f_extend(F_FILE *f, long extend)
{
	unsigned long relpos;
	unsigned char ret;
	long size = extend;
	
	_memset(f->_tdata + f->relpos, 0, F_SECTOR_SIZE - f->relpos);
	if (f->relpos + size <= F_SECTOR_SIZE) 
	{
		relpos = f->relpos + size;
		size = 0;
	}
	else
	{
		size -= (F_SECTOR_SIZE - f->relpos);
	}

	for (;;)
	{
		if (size)
		{
			f->modified = 1;
			ret = _f_emptywritebuffer(f, NULL);
			if (ret) return ret;
			++f->pos.sector;
			f->abspos += F_SECTOR_SIZE;
		}
		else break;
		_memset(f->_tdata, 0, F_SECTOR_SIZE);
		relpos = (size > F_SECTOR_SIZE) ? F_SECTOR_SIZE : size;
		size -= relpos;
	}
	
	f->relpos = relpos;
	f->filesize += extend;
	f->modified = 1;

	return F_NO_ERROR;
}

#if 0
static unsigned char _f_extend (F_FILE *f, long size)
{
	unsigned long relpos;
	long _size;
	unsigned char rc;
	/******************************/
	unsigned long sector_num=0;
	unsigned long cluster_num=0;
	unsigned long next_cluster;
	/******************************/
	size -= f->filesize;
	_size = size;

	if(f->relpos+f->abspos < f->filesize)
	{
		if(f->modified)
		{
			rc=_f_emptywritebuffer(f, NULL);
			if (rc) return rc;
			//f->modified = 0;
		}
		sector_num = f->pos.sectorend-f->pos.sector;
		while(1)
		{
			rc=_f_getclustervalue(f->pos.cluster,&next_cluster);
			if (rc) return rc;
			if(next_cluster>=F_CLUSTER_RESERVED)
				break;
			cluster_num++;
			f->prevcluster=f->pos.cluster;
			_f_clustertopos(next_cluster,&f->pos);
		}
		if(((f->filesize/F_SECTOR_SIZE-sector_num)%gl_volume.bootrecord.sector_per_cluster)==0)
			f->pos.sector+=gl_volume.bootrecord.sector_per_cluster;
		else
			f->pos.sector+=(f->filesize/F_SECTOR_SIZE-sector_num)%gl_volume.bootrecord.sector_per_cluster;	
		f->relpos = f->filesize%F_SECTOR_SIZE;
		f->abspos = f->filesize-f->relpos;
		rc = _f_getcurrsector(f,NULL);
	}	
	/*****************************/
	
	if (f->relpos==F_SECTOR_SIZE)
	{
		f->modified=1;
		rc=_f_emptywritebuffer(f, NULL);
		if (rc) return rc;
	}

	_memset(f->_tdata+f->relpos,0,F_SECTOR_SIZE-f->relpos);
	if (f->relpos+_size<=F_SECTOR_SIZE) 
	{
		relpos=f->relpos+_size;
		_size=0;
	}
	else
	{
		_size-=(F_SECTOR_SIZE-f->relpos);
	}

	for (;;)
	{
		if (_size)
		{
			f->modified=1;
 			rc=_f_emptywritebuffer(f, NULL);
			if (rc) return rc;
			++f->pos.sector;
			f->abspos+=F_SECTOR_SIZE;
		}
		else break;
		_memset(f->_tdata,0,F_SECTOR_SIZE);
		relpos=(_size>F_SECTOR_SIZE)?F_SECTOR_SIZE:_size;
		_size-=relpos;
	}

	f->relpos=relpos;
	f->filesize+=size;
	f->modified=1;

	return F_NO_ERROR;
}
#endif
#endif


/****************************************************************************
 *
 * _f_fseek
 *
 * subfunction for f_seek it moves position into given offset and read
 * the current sector
 *
 * INPUTS
 * f - F_FILEINT structure which file position needed to be modified
 * offset - position from start
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 * REMARKS
 * 
 * seek cache enabled
 *
 ***************************************************************************/
static unsigned char _f_fseek(F_FILE *f, long offset) 
{
	unsigned long cluster;
	unsigned long clustersize;
	unsigned char ret;
#if (F_WRITING && F_SEEK)	
	unsigned long extend = 0;
#endif	
#if F_SEEKCACHE
	SLOT current;
#endif

	if(offset < 0) offset = 0;
	if((unsigned long)offset > f->filesize) 
	{
#if (F_WRITING && F_SEEK)
		if(f->mode & (F_FILE_WR|F_FILE_RDP|F_FILE_WRP))
			extend = (unsigned long)offset - f->filesize;
#endif
		offset = f->filesize;
	}

	if((f->relpos < F_SECTOR_SIZE) && (offset >= f->abspos) && (offset < f->abspos+F_SECTOR_SIZE))
	{
		/* in current sector */
		f->relpos = offset - f->abspos;
#if (F_WRITING && F_SEEK)
		goto seek_extend;
#else
		return 0;
#endif
	}

#if F_WRITING
	if(f->modified) 
	{
		ret = _f_emptywritebuffer(f, NULL); 
		if(ret) 
		{
			f->mode=F_FILE_CLOSE; /*cant accessed any more*/
			return ret;
		}

		ret = _f_writefatsector();
		if(ret) 
		{
			f->mode = F_FILE_CLOSE;
			return ret;
		}
	}
#endif

#if F_SEEKCACHE
	if(f->filesize > (f->abspos+f->relpos)) 
	{
		current.v = f->abspos;
		current.currcluster = f->pos.cluster;
		current.prevcluster = f->prevcluster;
		if(current.v) _f_setcache(&current, f->slots, MAX_SLOTS);
	}
#endif


	if(!f->startcluster) return F_NO_ERROR;

	clustersize = gl_volume.bootrecord.sector_per_cluster;
	clustersize *= F_SECTOR_SIZE;
	
	f->relpos = 0;
#if F_SEEKCACHE
	current.v = offset;
	if (_f_getcache(&current, f->slots, MAX_SLOTS) == 0)
	{
		/* found match cache */				
		f->abspos = current.v - current.v%clustersize;
		offset -= f->abspos;
		f->prevcluster = current.prevcluster;
		f->pos.cluster = current.currcluster;
	}
	else
#endif
	{
		/* seek from beginning */
		f->abspos = 0;
		f->prevcluster = 0;
		f->pos.cluster = f->startcluster;
	}

	/*calc cluster*/
	while ((unsigned long)offset >= clustersize) 
	{
		ret = _f_getclustervalue(f->pos.cluster,&cluster);
		if (ret) 
		{
			f->mode = F_FILE_CLOSE;
			return ret;
		}

		offset -= clustersize;
		f->abspos += clustersize;
		if (cluster >= F_CLUSTER_RESERVED) 
		{
			if (!offset) 
			{
				_f_clustertopos(f->pos.cluster, &f->pos);
				f->pos.sector = f->pos.sectorend;
				return F_NO_ERROR; /*but eof!*/
			}
			f->mode = F_FILE_CLOSE;
			MMDPrintf("\r\n F_ERR_INVALIDSECTOR, cluster = 0x%x, f->prevcluster = 0x%x, f->pos.cluster = 0x%x, f->startcluster = 0x%x, offset = %d",(unsigned int) cluster, (unsigned int)f->prevcluster, (unsigned int)f->pos.cluster,(unsigned int) f->startcluster,(unsigned int)offset);
			return F_ERR_INVALIDSECTOR;
		}

		f->prevcluster = f->pos.cluster;
		f->pos.cluster = cluster;
	}

	_f_clustertopos(f->pos.cluster, &f->pos);

	while (offset >= F_SECTOR_SIZE) 
	{
		f->pos.sector++;
		offset -= F_SECTOR_SIZE;
		f->abspos += F_SECTOR_SIZE;
	}

	f->relpos = offset;
	ret = _f_getcurrsector(f,NULL);
	if(ret) return ret;
	
#if (F_WRITING && F_SEEK)
seek_extend:
	if(extend)
	{
		ret = _f_extend(f, extend);
		if(ret) return ret;
	}
#endif
	return F_NO_ERROR;
}	


/****************************************************************************
 *
 * _f_getintfile
 *
 * allocate an internal file handler
 *
 * RETURNS
 *
 * internal file pointer if successful
 * zero if there was no free file pointer (see F_MAXFILES)
 *
 ***************************************************************************/
F_FILE *_f_getintfile(void) 
{
	unsigned char a;
	for(a = 0; a < F_MAXFILES; a++) 
	{
		if(gl_files[a].mode == F_FILE_CLOSE) 
		{
			_memset(&gl_files[a], 0, 24);
			return &gl_files[a];
		}
	}
	return 0;		/*no more file could be opened */
}

/****************************************************************************
 *
 * fn_rename
 *
 * Rename file or directory
 *
 * INPUTS
 *
 * filename - filename or directory name (with or without path)
 * newname - new name of the file or directory (without path)
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
#if (F_RENAME && F_WRITING)
#if F_LONGFILENAME
unsigned char fn_rename(const char *filename, const char *newname) 
{
	F_LFNINT *plfn = NULL;
	F_NAME *fsname = NULL;
	F_NAME *fsnewname = NULL;
	
	F_POS posdir;
	F_POS pos;
	F_DIRENTRY *de;
	F_DIRENTRY deold;
	unsigned char ret;

	ret = _f_getvolume();
	if(ret) return ret;

	plfn = (F_LFNINT *)f_malloc(sizeof(F_LFNINT));
	fsname = (F_NAME *)f_malloc(sizeof(F_NAME));
	fsnewname = (F_NAME *)f_malloc(sizeof(F_NAME));
	if(!plfn || !fsname || !fsnewname)
	{
		if(plfn) f_free(plfn);
		if(fsname) f_free(fsname);
		if(fsnewname) f_free(fsnewname);
		return F_ERR_NULLPOINTER;
	}

	if(_f_setfsname(filename, fsname)) 
	{
		if(plfn) f_free(plfn);
		if(fsname) f_free(fsname);
		if(fsnewname) f_free(fsnewname);
		return F_ERR_INVALIDNAME; /*invalid name*/
	}
#if F_CHECKNAME
	if(_f_checknamewc(fsname->lname, 1)) 
	{
		if(plfn) f_free(plfn);
		if(fsname) f_free(fsname);
		if(fsnewname) f_free(fsnewname);
		return F_ERR_INVALIDNAME;/*invalid name*/
	}
	if(fsname->lname[0] == '.' && !fsname->lname[1]) 
	{	
		if(plfn) f_free(plfn);
		if(fsname) f_free(fsname);
		if(fsnewname) f_free(fsnewname);
		return F_ERR_NOTFOUND;
	}
#endif


	if(!(_f_findpath(fsname, &posdir))) 
	{	
		if(plfn) f_free(plfn);
		if(fsname) f_free(fsname);
		if(fsnewname) f_free(fsnewname);
		return F_ERR_INVALIDDIR;
	}

	pos = posdir;

#if F_FINDING
	if(!_f_findlfilewc(fsname->lname, &pos, &de, plfn, 0))
#else
	if(!_f_findlfilewc(fsname->lname, &pos, &de, plfn)) 
#endif
	{
		if(plfn) f_free(plfn);
		if(fsname) f_free(fsname);
		if(fsnewname) f_free(fsnewname);
		return F_ERR_NOTFOUND;
	}

	if(de->attr & F_ATTR_READONLY) 
	{	
		if(plfn) f_free(plfn);
		if(fsname) f_free(fsname);
		if(fsnewname) f_free(fsnewname);
		return F_ERR_ACCESSDENIED;      /*readonly*/
	}

	if(_f_checklocked(&pos)) 
	{	
		if(plfn) f_free(plfn);
		if(fsname) f_free(fsname);
		if(fsnewname) f_free(fsnewname);
		return F_ERR_LOCKED;
	}

	_memcpy(&deold, de, sizeof(F_DIRENTRY)); /*store all properties*/

#if F_CHECKNAME
	if(_f_checknamewc(newname, 1)) 
	{	
		if(plfn) f_free(plfn);
		if(fsname) f_free(fsname);
		if(fsnewname) f_free(fsnewname);
		return F_ERR_INVALIDNAME;/*invalid name no drive,no path*/
	}
#endif
	if(_f_setfsname(newname, fsnewname)) 
	{	
		if(plfn) f_free(plfn);
		if(fsname) f_free(fsname);
		if(fsnewname) f_free(fsnewname);
		return F_ERR_INVALIDNAME; /*invalid name*/
	}
#if F_CHECKNAME
	if(_f_checknamewc(fsnewname->lname, 1)) 
	{	
		if(plfn) f_free(plfn);
		if(fsname) f_free(fsname);
		if(fsnewname) f_free(fsnewname);
		return F_ERR_INVALIDNAME;/*invalid name*/
	}
	if(fsnewname->lname[0]=='.' && !fsnewname->lname[1]) 
	{	
		if(plfn) f_free(plfn);
		if(fsname) f_free(fsname);
		if(fsnewname) f_free(fsnewname);
		return F_ERR_INVALIDNAME;
	}
#endif

	_memcpy(fsnewname->path, fsname->path, F_MAXPATH);

	pos = posdir;

#if F_FINDING
	if(_f_findlfilewc(fsnewname->lname, &pos, 0, plfn, 0)) 
#else
	if(_f_findlfilewc(fsnewname->lname, &pos, 0, plfn)) 
#endif
	{	
		if(plfn) f_free(plfn);
		if(fsname) f_free(fsname);
		if(fsnewname) f_free(fsnewname);
		return F_ERR_DUPLICATED;
	}

	pos = posdir;

	ret = _f_addentry(fsnewname, &pos, &de, plfn);
	if(ret) 
	{	
		if(plfn) f_free(plfn);
		if(fsname) f_free(fsname);
		if(fsnewname) f_free(fsnewname);
		return ret;
	}

	_memcpy(&deold, de, F_MAXNAME+F_MAXEXT); /* update name and ext */
	deold.ntres = de->ntres; /* update ntres */
	_memcpy(de, &deold, sizeof(F_DIRENTRY)); /*restore all properties*/

	ret = _f_writedirsector();
	if(ret) return ret;

	pos = posdir;

#if F_FINDING
	if(!_f_findlfilewc(fsname->lname, &pos, &de, plfn, 0)) 
#else
	if(!_f_findlfilewc(fsname->lname, &pos, &de, plfn)) 
#endif
	{
		if(plfn) f_free(plfn);
		if(fsname) f_free(fsname);
		if(fsnewname) f_free(fsnewname);
		return F_ERR_NOTFOUND; /*try to find again to remove*/
	}

	ret = _f_removelfn(posdir.cluster, plfn);
	if(plfn) f_free(plfn);
	if(fsname) f_free(fsname);
	if(fsnewname) f_free(fsnewname);
	return ret;
}
#else
unsigned char fn_rename(const char *filename, const char *newname) 
{
	F_POS posdir;
	F_POS pos;
	F_DIRENTRY *de;
	F_NAME fsname;
	char name[F_MAXNAME];
	char ext[F_MAXEXT];
	unsigned char a;
	unsigned char ret;

	if(_f_setfsname(filename, &fsname)) return F_ERR_INVALIDNAME; /*invalid name*/
#if F_CHECKNAME
	if(_f_checknamewc(fsname.filename, fsname.fileext)) return F_ERR_INVALIDNAME;/*invalid name*/
#endif
	ret = _f_getvolume();
	if(ret) return ret;

	if(!(_f_findpath(&fsname, &posdir))) return F_ERR_INVALIDDIR;

	pos = posdir;

	if(fsname.filename[0] == '.') return F_ERR_NOTFOUND;

#if F_FINDING
	if(!_f_findfilewc(fsname.filename, fsname.fileext, &pos, &de, 0)) return F_ERR_NOTFOUND;
#else
	if(!_f_findfilewc(fsname.filename, fsname.fileext, &pos, &de)) return F_ERR_NOTFOUND;
#endif

	if(de->attr & F_ATTR_READONLY) return F_ERR_ACCESSDENIED;      /*readonly*/

	if(_f_checklocked(&pos)) return F_ERR_LOCKED;

	if(!_f_setnameext((char *)newname, name, ext)) return 0; /*no length invalid name*/
#if F_CHECKNAME
	if(_f_checknamewc(name,ext)) return 0;/*invalid name*/
	if(name[0] == '.' || name[0] == ' ') return F_ERR_INVALIDNAME;
#endif
#if F_FINDING
	if(_f_findfilewc(name, ext, &posdir, 0, 0)) return F_ERR_DUPLICATED;
#else
	if(_f_findfilewc(name, ext, &posdir, 0)) return F_ERR_DUPLICATED;
#endif

	ret = _f_getdirsector(pos.sector);
	if(ret) return ret;

	for(a = 0; a < F_MAXNAME; a++) de->name[a]=name[a];
	for(a = 0; a < F_MAXEXT; a++) de->ext[a]=ext[a];

	return _f_writedirsector();
}
#endif
#endif


/****************************************************************************
 *
 * fn_filelength
 *
 * Get a file length
 *
 * INPUTS
 *
 * filename - file whose length is needed
 *
 * RETURNS
 *
 * length of the file
 *
 ***************************************************************************/
#if F_FILELENGTH
long fn_filelength(const char *filename) 
{
#if F_LONGFILENAME
	F_LFNINT lfn;
#endif
	F_NAME *fsname = NULL;
	F_POS pos;
	F_DIRENTRY *de;

	if(_f_getvolume()) return 0; /*can't get the size*/

	fsname = f_malloc(sizeof(F_NAME));
	if(!fsname) return 0;
	
	if(_f_setfsname(filename, fsname)) 
	{	
		if(fsname) f_free(fsname);
 		return 0; /*invalid name*/
	}
	
#if F_CHECKNAME
#if F_LONGFILENAME
	if(_f_checknamewc(fsname->lname, 1))
#else
	if(_f_checknamewc(fsname->filename, fsname->fileext)) 
#endif
#endif
	{
		if(fsname) f_free(fsname);
		return 0;/*invalid name*/
	}

	if(!_f_findpath(fsname, &pos)) 
	{	
		if(fsname) f_free(fsname);
		return 0;
	}
#if F_LONGFILENAME
#if F_FINDING
	if(!_f_findlfilewc(fsname->lname, &pos, &de, &lfn, 0)) 
#else
	if(!_f_findlfilewc(fsname->lname, &pos, &de, &lfn))
#endif
	{
		if(fsname) f_free(fsname);
		return 0;
	}
#else
#if F_FINDING
	if(!_f_findfilewc(fsname->filename, fsname->fileext, &pos, &de, 0)) 
#else
	if(!_f_findfilewc(fsname->filename, fsname->fileext, &pos, &de))
#endif
	{
		if(fsname) f_free(fsname);
		return 0;
	}
#endif

	if(fsname) f_free(fsname);
	if (de->attr & F_ATTR_DIR) return 0;				/*directory*/	
	
	return _f_getlong(&de->filesize);
}
#endif

/****************************************************************************
 *
 * fn_open
 *
 * open a file for reading/writing/appending
 *
 * INPUTS
 *
 * filename - which file need to be opened
 * mode - string how to open ("r"-read, "w"-write, "w+"-overwrite, "a"-append
 *
 * RETURNS
 *
 * F_FILE pointer if successfully
 * 0 - if any error
 *
 ***************************************************************************/
F_FILE *fn_open(const char *filename,const char *mode) 
{
#if F_LONGFILENAME
	F_LFNINT lfn;
#endif
	F_NAME *fsname = NULL;
	F_DIRENTRY *de;	
	unsigned char m_mode = F_FILE_CLOSE;
	F_FILE *f = 0;

	if (_f_getvolume()) return 0; /*cant open any*/
	
	fsname = f_malloc(sizeof(F_NAME));
	if(!fsname) return 0;	

	if(mode[1] == 0)
	{
		if(mode[0] == 'r') m_mode = F_FILE_RD;
#if F_WRITING
		if(mode[0] == 'w') m_mode = F_FILE_WR;
		if(mode[0] == 'a') m_mode = F_FILE_A;
	}
	if(mode[1] == '+' && mode[2] == 0)
	{
		if(mode[0] == 'r') m_mode = F_FILE_RDP;
		if(mode[0] == 'w') m_mode = F_FILE_WRP;
		if(mode[0] == 'a') m_mode = F_FILE_AP;
#endif
	}

	if(m_mode == F_FILE_CLOSE) 
	{	
		if(fsname) f_free(fsname);
		return 0; 			/*invalid mode*/
	}
	
	if(_f_setfsname(filename, fsname)) 
	{	
		if(fsname) f_free(fsname);
		return 0; /*invalid name*/
	}
#if F_CHECKNAME
#if F_LONGFILENAME
	if(_f_checknamewc(fsname->lname, 1)) 
	{	
		if(fsname) f_free(fsname);
		return 0;/*invalid name*/
	}
	if(fsname->lname[0] == '.' && !fsname->lname[1]) 
	{	
		if(fsname) f_free(fsname);
		return 0;
	}
#else
	if(_f_checknamewc(fsname.filename, fsname.fileext)) 
	{	
		if(fsname) f_free(fsname);
		return 0;/*invalid name*/
	}
	if(fsname.filename[0] == '.') 
	{	
		if(fsname) f_free(fsname);
		return 0;
	}
#endif
#endif
	
	f = _f_getintfile();
	
	if(!f) 
	{	
		if(fsname) f_free(fsname);
		return 0; /*no more file could be opened */
	}

	if(!_f_findpath(fsname, &f->dirpos)) 
	{	
		if(fsname) f_free(fsname);
		return 0;
	}

#if F_SEEKCACHE
	_f_initcache(f->slots, MAX_SLOTS);
#endif
	
	switch (m_mode) 
	{
#if F_WRITING
		case F_FILE_RDP: /*r+*/
#endif
		case F_FILE_RD: /*r*/
#if F_LONGFILENAME
#if F_FINDING
			if(!_f_findlfilewc(fsname->lname, &f->dirpos, &de, &lfn, 0)) 
#else
			if(!_f_findlfilewc(fsname->lname, &f->dirpos, &de, &lfn))
#endif
#else
#if F_FINDING
			if(!_f_findfilewc(fsname->filename, fsname->fileext, &f->dirpos, &de, 0))
#else
			if(!_f_findfilewc(fsname->filename, fsname->fileext, &f->dirpos, &de))
#endif
#endif
			{	
				if(fsname) f_free(fsname);
				return 0;	
			}

			if(fsname) f_free(fsname);

			if(de->attr & F_ATTR_DIR) return 0;		   /*directory*/

			f->startcluster = _f_getdecluster(de);

			if(f->startcluster)
			{
				_f_clustertopos(f->startcluster, &f->pos);
				f->filesize = _f_getlong(&de->filesize);
				f->abspos = (unsigned long)-F_SECTOR_SIZE;
				if(_f_fseek(f, 0)) return 0;
			}
			else
			{
				f->pos.sector = 0;
				f->pos.sectorend = 0;
			}

			if ((m_mode & F_FILE_RDP) && _f_checklocked(&f->dirpos)) return 0;
			
			break;

#if F_WRITING
		case F_FILE_AP: /*a+*/
		case F_FILE_A: /*a*/
			_memcpy(&(f->pos), &(f->dirpos), sizeof(F_POS));
#if F_LONGFILENAME
#if F_FINDING
			if(_f_findlfilewc(fsname->lname, &f->dirpos, &de, &lfn, 0))
#else
			if(_f_findlfilewc(fsname->lname, &f->dirpos, &de, &lfn))
#endif
#else
#if F_FINDING
			if(_f_findfilewc(fsname->filename, fsname->fileext, &f->dirpos, &de, 0))
#else
			if(_f_findfilewc(fsname->filename, fsname->fileext, &f->dirpos, &de))
#endif
#endif
			{
				if(fsname) f_free(fsname);

				if(de->attr & (F_ATTR_DIR|F_ATTR_READONLY)) return 0;

				if(_f_checklocked(&f->dirpos)) return 0; /*locked*/


				f->startcluster = _f_getdecluster(de);
				f->filesize = _f_getlong(&de->filesize);
				if(f->startcluster) 
				{
					_f_clustertopos(f->startcluster, &f->pos);
					f->abspos = (unsigned long)-F_SECTOR_SIZE; /*forcing seek to read 1st sector! abspos=0;*/
					if(_f_fseek(f, f->filesize)) 
					{
						f->mode = F_FILE_CLOSE;
						return 0;
					}
				}
				else 
				{
					f->pos.sector = 0;
					f->pos.sectorend = 0;
					f->modified = 0;
					f->abspos = 0;
					f->relpos = 0;
					f->filesize = 0;
					f->prevcluster = 0;
					//_memset(f,0,21);
				}
			}
			else 
			{
				_memcpy(&(f->dirpos), &(f->pos), sizeof(F_POS));

				_f_clustertopos(f->dirpos.cluster, &f->pos);

				if(_f_addentry(fsname, &f->dirpos, &de, &lfn)) 
				{	
					if(fsname) f_free(fsname);
					return 0;	/*couldnt be added*/
				}

				if(fsname) f_free(fsname);

				de->attr |= F_ATTR_ARC; 	/*set as archiv*/
				if(_f_writedirsector()) return 0;

				f->pos.sector = 0;
				f->pos.sectorend = 0;
				f->modified = 0;
				f->abspos = 0;
				f->relpos = 0;
				f->filesize = 0;
				f->prevcluster = 0;
				f->startcluster = 0;
				//_memset(f, 0, 21);
			}
			break;

		case F_FILE_WR: /*w*/
		case F_FILE_WRP: /*w+*/
			_f_clustertopos(f->dirpos.cluster, &f->pos);
#if F_LONGFILENAME
#if F_FINDING
			if(_f_findlfilewc(fsname->lname, &f->pos, &de, &lfn, 0))
#else
			if(_f_findlfilewc(fsname->lname, &f->pos, &de, &lfn))
#endif
#else
#if F_FINDING
			if(_f_findfilewc(fsname->filename, fsname->fileext, &f->pos, &de, 0))
#else
			if(_f_findfilewc(fsname->filename, fsname->fileext, &f->pos, &de))
#endif
#endif
			{
				unsigned long cluster = _f_getdecluster(de);	   /*exist*/

				if(fsname) f_free(fsname);
				
				if(de->attr & (F_ATTR_DIR|F_ATTR_READONLY)) return 0;

				_memcpy(&(f->dirpos), &(f->pos), sizeof(F_POS));

				if(_f_checklocked(&f->dirpos)) return 0; /*locked*/

				de->filesize = 0; 				/*reset size;*/
				de->attr |= F_ATTR_ARC; 	/*set as archiv*/
				de->clusterlo = 0;					/*no points anywhere*/
				de->clusterhi = 0;					/*no points anywhere*/

#if F_FAT32
				if(gl_volume.mediatype == F_FAT32_MEDIA) 
				{
					_f_setword(&de->crtdate, f_getdate());  /*if there is realtime clock then creation date could be set from*/
					_f_setword(&de->crttime, f_gettime());  /*if there is realtime clock then creation time could be set from*/
					_f_setword(&de->lastaccessdate, f_getdate());  /*if there is realtime clock then creation date could be set from*/
				}
#endif
				if(_f_writedirsector()) return 0;

				if(_f_removechain(cluster)) return 0; /*remove */
			}
			else 
			{
				if(_f_addentry(fsname, &f->dirpos, &de, &lfn)) 
				{	
					if(fsname) f_free(fsname);
					return 0;	/*couldnt be added*/
				}
				
				if(fsname) f_free(fsname);

				f->pos.sector = 0;
				f->pos.sectorend = 0;
				f->modified = 0;
				f->abspos = 0;
				f->relpos = 0;
				f->filesize = 0;
				f->prevcluster = 0;
				f->startcluster = 0;
				//_memset(f, 0, 21);
				de->attr |= F_ATTR_ARC; 	/*set as archiv*/
				if(_f_writedirsector()) return 0;
			}

			f->startcluster = 0;
			f->pos.sector = 0;
			f->pos.sectorend = 0;
			break;
#endif

		default: return 0; /*invalid mode*/
	}

	f->mode = m_mode; /* lock it */
	return f;
}

/****************************************************************************
 *
 * fn_close
 *
 * close a previously opened file
 *
 * INPUTS
 *
 * filehandle - which file needs to be closed
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/
unsigned char fn_close(F_FILE *f) 
{
	unsigned char ret;

	if (!f) return F_ERR_NOTOPEN;

	ret=_f_getvolume();
	
	if (ret)
	{
		return ret;
	}

	if (f->mode == F_FILE_CLOSE)
	{
		return F_ERR_NOTOPEN;
	}
	else if (f->mode == F_FILE_RD)
	{
		f->mode = F_FILE_CLOSE;
		return F_NO_ERROR;
	}
#if F_WRITING
	else
	{
		f->mode = F_FILE_CLOSE;

		if (_f_emptywritebuffer(f, NULL)) 
		{
			(void)_f_updatefileentry(f, 1); 
			return F_ERR_WRITE;
		}

		if (_f_writefatsector()) 
		{
			(void)_f_updatefileentry(f, 1); 
			return F_ERR_WRITE;
		}
		return _f_updatefileentry(f, 0);
	}
#endif
}


/****************************************************************************
 *
 * fn_read
 *
 * read data from file
 *
 * INPUTS
 *
 * buf - where the store data 
 * size - size of items to be read
 * _size_t - number of items need to be read
 * filehandle - file where to read from
 *
 * RETURNS
 *
 * with the number of read bytes
 *
 ***************************************************************************/
long fn_read(void *buf,long size,long _size_st,F_FILE *f) 
{
	char *buffer = (char*)buf;
	long retsize;
	unsigned char direct, ret;

	if(!f) return 0;

	if((f->mode & (F_FILE_RD|F_FILE_RDP|F_FILE_WRP|F_FILE_AP)) == 0) return 0;

	retsize = size;
	size *= _size_st;
	_size_st = retsize;
	retsize = 0;

	/* size: number of bytes to read, _size_st: size of item, retsize: number of read bytes */

	if(_f_getvolume()) return 0; /*cant read any*/
	
	if(size + f->relpos + f->abspos >= f->filesize)
	{	/*read len longer than the file*/
		size = (f->filesize) - (f->relpos) - (f->abspos);  /*calculate new size*/
	}

	if(size <= 0) return 0;

	while(size) 
	{
		unsigned long rdsize = 0;

		if(f->relpos == F_SECTOR_SIZE) 
		{
			/* all data in one sector have been read */
			f->abspos += f->relpos;
			f->relpos = 0;
			f->cached = 0;

#if F_WRITING
			if(f->modified) 
			{
				ret = _f_emptywritebuffer(f, NULL); /*empty write buffer automatically goes to next sector*/
				if(ret) 
				{
					f->mode = F_FILE_CLOSE; /*no more read allowed*/
					return retsize;
				}

				if(_f_writefatsector()) 
				{
					f->mode = F_FILE_CLOSE;
					return retsize;
				}

			}
#endif
			f->pos.sector++;	/*goto next*/

			if(size >= F_SECTOR_SIZE)
			{
				direct = 1;
#if F_MULTIREAD
				ret = _f_getmultisector(f, buffer, size/F_SECTOR_SIZE, &rdsize);
#else
				rdsize = F_SECTOR_SIZE;
				ret = _f_getcurrsector(f, buffer);				
#endif
			}
			else 
			{
				direct = 0;
				rdsize = size;
				ret = _f_getcurrsector(f, NULL);
			}
			if(ret == F_ERR_EOF && (!size)) return retsize;

			if(ret) 
			{
				f->mode = F_FILE_CLOSE; /*no more read allowed*/
				return retsize / _size_st;
			}
		}

		if(rdsize == 0)
		{
			direct = 0;
			rdsize = F_SECTOR_SIZE-f->relpos;
			if(rdsize > size) rdsize = size;
		}

		if(!direct) 
		{	
			//if(f->empty) 
			if(!f->cached) 
			{
#if F_WRITING			
				if (f->modified) {
					if (_f_emptywritebuffer(f, NULL)) {
						f->mode = F_FILE_CLOSE;

						if (f->startcluster) {
							if (_f_updatefileentry(f, 0) == 0) 
								return retsize / _size_st;
						} else {
							(void)_f_updatefileentry(f, 1);
						}
						return 0;
					}
				}
#endif
				ret = _f_getcurrsector(f, NULL);
				if(ret) 
				{
					f->mode = F_FILE_CLOSE; /*no more read allowed*/
					return retsize / _size_st;
				}
			}
			_memcpy(buffer, f->_tdata+f->relpos, (int)(rdsize)); /*always less than 512*/
		}

		buffer += rdsize;
		f->relpos += rdsize;
		
#if F_MULTIREAD
		if(f->relpos > F_SECTOR_SIZE)
		{
			f->abspos += f->relpos - F_SECTOR_SIZE;
			f->relpos = F_SECTOR_SIZE;
		}
#endif
		size -= rdsize;
		retsize += rdsize;
	}

	return retsize / _size_st;

}

/****************************************************************************
 *
 * fn_write
 *
 * write data into file
 *
 * INPUTS
 *
 * buf - where the store data 
 * size - size of items to be read
 * size_t - number of items need to be read
 * filehandle - file where to read from
 *
 * RETURNS
 *
 * with the number of read bytes
 *
 ***************************************************************************/
#if (F_WRITE && F_WRITING)
long fn_write(const void *buf, long size, long _size_st, F_FILE *f) 
{
	char *buffer = (char*)buf;
	long retsize;
	long ret = 0;
	unsigned long cluster;

	if (!f) 
		return 0;

	if ((f->mode & (F_FILE_WR | F_FILE_A | F_FILE_RDP | F_FILE_WRP | F_FILE_AP)) == 0) 
		return 0;

	retsize = size;
	size *= _size_st;
	_size_st = retsize;
	retsize = 0;
	/* size: number of bytes to write, _size_st: size of item, retsize: number of written bytes */

	if (_f_getvolume()) 
		return 0; /*can't write*/

	if (f->mode & (F_FILE_A | F_FILE_AP)) {
		/*seek to the end of file to write*/
		if (_f_fseek(f, f->filesize)) {
			f->mode = F_FILE_CLOSE;
			return 0;
		}
	}

	if (size > (0xFFFFFFFF - (f->abspos + f->relpos))) {
		/* filesize > 0xFFFFFFFF is not allowed */
		return 0;
	}

	while (size) {
		unsigned long wrsize = size;

		if (f->relpos == F_SECTOR_SIZE) { 
			/*now full*/
			if (f->modified) {
				if (_f_emptywritebuffer(f, NULL)) {
					f->mode = F_FILE_CLOSE;

					if (f->startcluster) {
						if (_f_updatefileentry(f, 0) == 0) 
							return retsize / _size_st;
					} else {
						(void)_f_updatefileentry(f, 1);
					}
					return 0;
				}
			}

			f->abspos += f->relpos;
			f->relpos = 0;
			f->cached = 0;

			f->pos.sector++;	/*goto next*/

			if (size < F_SECTOR_SIZE) {
				ret = _f_getcurrsector(f, NULL);

				if (ret) {
					if (ret != F_ERR_EOF) {
						f->mode = F_FILE_CLOSE; /*no more read allowed*/
						return retsize / _size_st;
					}
				}
			}
		}

		if (wrsize >= F_SECTOR_SIZE - f->relpos) {
			wrsize = F_SECTOR_SIZE - f->relpos; /* wrsize <= F_SECTOR_SIZE */
		}

		if (wrsize < F_SECTOR_SIZE) {
			/* wrsize is not beyond a sector */
			if (!f->startcluster) {
				if (_f_alloccluster(&f->startcluster)) {
					f->mode = F_FILE_CLOSE;
					return 0;
				}
				
				_f_clustertopos(f->startcluster, &f->pos);

				if (_f_setclustervalue(f->startcluster, F_CLUSTER_LAST)) {
					f->mode = F_FILE_CLOSE;
					return 0;
				}
			} else {
				//if (f->empty) {
				if (!f->cached) {
					if (f->modified) {
						if (_f_emptywritebuffer(f, NULL)) {
							f->mode = F_FILE_CLOSE;

							if (f->startcluster) {
								if (_f_updatefileentry(f, 0) == 0) 
									return retsize / _size_st;
							} else {
								(void)_f_updatefileentry(f, 1);
							}
							return 0;
						}
					}
					ret = _f_getcurrsector(f, NULL);				
					if (ret) {
						if (ret != F_ERR_EOF) {
							f->mode = F_FILE_CLOSE; /*no more read allowed*/
							return retsize / _size_st;
						}
					}
				}
			}
			_memcpy(f->_tdata + f->relpos, buffer, (int)(wrsize));
			f->modified = 1;	 /*sector is modified*/
		} else {
			/* wrsize == F_SECTOR_SIZE and f->relpos == 0 */
			f->modified = 1;
			cluster = f->pos.cluster;
#if F_MULTIWRITE
			if (_f_emptymultibuffer(f, buffer, size/F_SECTOR_SIZE, &wrsize)) 
#else
			if (_f_emptywritebuffer(f, buffer))
#endif
			{
				/* if write fail, close file and remove all cluster chain allocated */
				f->mode = F_FILE_CLOSE;				
				(void)_f_removechain(f->startcluster); /*dont care with return value of _f_removechain*/
				return 0;
			}
		}		

		buffer += wrsize;
		f->relpos += wrsize;
#if F_MULTIWRITE
		if (f->relpos > F_SECTOR_SIZE) {
			f->abspos += f->relpos - F_SECTOR_SIZE;
			f->relpos = F_SECTOR_SIZE;
		}
#endif
		size -= wrsize;
		retsize += wrsize;

		if (f->filesize < f->abspos + f->relpos) {
			f->filesize = f->abspos + f->relpos;
		}
	}

	return retsize / _size_st;
}

#endif

/****************************************************************************
 *
 * fn_seek
 *
 * moves position into given offset in given file
 *
 * INPUTS
 *
 * filehandle - F_FILE structure which file position needed to be modified
 * offset - relative position 
 * whence - where to calculate position (F_SEEK_SET,F_SEEK_CUR,F_SEEK_END)
 *
 * RETURNS
 *
 * 0 - if successfully
 * other - if any error 
 *
 ***************************************************************************/
#if (F_SEEK || F_REWIND)
unsigned char fn_seek(F_FILE *f,long offset,unsigned char whence) 
{
	unsigned char ret;

	if(!f) return F_ERR_NOTOPEN;

	if((f->mode & (F_FILE_RD|F_FILE_WR|F_FILE_A|F_FILE_RDP|F_FILE_WRP|F_FILE_AP)) == 0) 
	{	
		return F_ERR_NOTOPEN;
	}

	ret = _f_getvolume();
	if(ret) return ret;

	if(whence == F_SEEK_CUR)  return _f_fseek(f, f->abspos + f->relpos + offset);
	else if(whence == F_SEEK_END)  return _f_fseek(f, f->filesize + offset);
	else if(whence == F_SEEK_SET)  return _f_fseek(f, offset);

	return F_ERR_NOTUSEABLE;
}
#endif

/****************************************************************************
 *
 * fn_tell
 *
 * Tells the current position of opened file
 *
 * INPUTS
 *
 * filehandle - which file needs the position
 *
 * RETURNS
 *
 * position in the file from start
 *
 ***************************************************************************/
#if F_TELL
long fn_tell(F_FILE *f) 
{
	if(!f) return 0;

	if((f->mode & (F_FILE_RD|F_FILE_WR|F_FILE_A|F_FILE_RDP|F_FILE_WRP|F_FILE_AP)) == 0) 
	{	
		return 0;
	}

	return (f->abspos + f->relpos);
}
#endif


/****************************************************************************
 *
 * fn_eof
 *
 * Tells if the current position is end of file or not
 *
 * INPUTS
 *
 * filehandle - which file needs the checking
 *
 * RETURNS
 *
 * 0 - if not EOF
 * other - if EOF or invalid file handle
 *
 ***************************************************************************/
#if F_EOF
unsigned char fn_eof(F_FILE *f) 
{
	if(!f) return F_ERR_NOTOPEN;  /*if error*/
	if(f->abspos + f->relpos < f->filesize) return 0;
	return F_ERR_EOF;/*EOF*/
}
#endif


/****************************************************************************
 *
 * fn_rewind
 *
 * set the fileposition in the opened file to the begining
 *
 * INPUTS
 *
 * filehandle - which file needs to be rewinded
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/
#if F_REWIND
unsigned char fn_rewind(F_FILE *filehandle) 
{
	return fn_seek(filehandle, 0L, F_SEEK_SET);
}
#endif


/****************************************************************************
 *
 * fn_putc
 *
 * write a character into file
 *
 * INPUTS
 *
 * ch - what to write into file
 * filehandle - file where to write
 *
 * RETURNS
 *
 * with the character written or -1 if not successfully
 *
 ***************************************************************************/
#if (F_PUTC && F_WRITING)
int fn_putc(int ch, F_FILE *filehandle) 
{
	unsigned char tmpch = (unsigned char)ch;
	if(fn_write(&tmpch, 1, 1, filehandle)) return ch;
	else return -1;
}
#endif


/****************************************************************************
 *
 * fn_getc
 *
 * get a character from file
 *
 * INPUTS
 *
 * filehandle - file where to read from
 *
 * RETURNS
 *
 * with the read character or -1 if read was not successfully 
 *
 ***************************************************************************/
#if F_GETC
int fn_getc(F_FILE *filehandle) 
{
	unsigned char ch;
	if(!fn_read(&ch, 1, 1 ,filehandle)) return -1;
	
	return (int)ch;
}
#endif


/****************************************************************************
 *
 * fn_delete
 *
 * delete a file 
 *
 * INPUTS
 *
 * filename - file which wanted to be deleted (with or without path)
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/
#if (F_DELETE && F_WRITING)
unsigned char fn_delete(const char *filename) 
{
#if F_LONGFILENAME
	F_LFNINT *plfn = NULL;
	F_POS dirpos;
#endif
	F_NAME *fsname = NULL;
	F_POS pos;
	F_DIRENTRY *de;
	unsigned char ret;

	ret = _f_getvolume();
	if(ret) return ret;

	fsname = f_malloc(sizeof(F_NAME));
	if(!fsname) return F_ERR_NULLPOINTER;	

	if(_f_setfsname(filename, fsname)) 
	{	
		if(fsname) f_free(fsname);
		return F_ERR_INVALIDNAME; /*invalid name*/
	}
#if F_CHECKNAME
#if F_LONGFILENAME
	if(_f_checknamewc(fsname->lname, 1)) 
	{	
		if(fsname) f_free(fsname);
		return F_ERR_INVALIDNAME;/*invalid name*/
	}
	if(fsname->lname[0] == '.' && !fsname->lname[1]) 
	{	
		if(fsname) f_free(fsname);
		return F_ERR_NOTFOUND;
	}
#else
	if(_f_checknamewc(fsname->filename, fsname->fileext)) 
	{	
		if(fsname) f_free(fsname);
		return F_ERR_INVALIDNAME;/*invalid name*/
	}
	if(fsname->filename[0] == '.') 
	{	
		if(fsname) f_free(fsname);
		return F_ERR_NOTFOUND;
	}
#endif
#endif


#if F_LONGFILENAME
	if(!(_f_findpath(fsname, &dirpos))) 
	{	
		if(fsname) f_free(fsname);
		return F_ERR_INVALIDDIR;
	}
	_memcpy(&pos, &dirpos, sizeof(F_POS)); /*copy directory pos*/

	plfn = f_malloc(sizeof(F_LFNINT));
	if(!plfn) 
	{
		if(fsname) f_free(fsname);
		return F_ERR_NULLPOINTER;
	}
	
#if F_FINDING
	if(!_f_findlfilewc(fsname->lname, &pos, &de, plfn, 0))
#else
	if(!_f_findlfilewc(fsname->lname, &pos, &de, plfn)) 
#endif
	{
		if(fsname) f_free(fsname);
		if(plfn) f_free(plfn);
		return F_ERR_NOTFOUND;
	}
#else
	if(!(_f_findpath(fsname, &pos)))
	{	
		if(fsname) f_free(fsname);
		return F_ERR_INVALIDDIR;
	}
#if F_FINDING
	if(!_f_findfilewc(fsname->filename, fsname->fileext, &pos, &de, 0))
#else
	if(!_f_findfilewc(fsname->filename, fsname->fileext, &pos, &de)) 
#endif
	{
		if(fsname) f_free(fsname);
		return F_ERR_NOTFOUND;
	}
#endif

	if(fsname) f_free(fsname);

	ret = F_NO_ERROR;

	if(de->attr & F_ATTR_DIR) 
	{
		ret = F_ERR_INVALIDDIR; 	   /*directory*/
	}
	else if(de->attr & F_ATTR_READONLY) 
	{
		ret = F_ERR_ACCESSDENIED;		/*readonly*/
	}
	else if(_f_checklocked(&pos)) 
	{
		ret = F_ERR_LOCKED;
	}

	if(ret)
	{
#if F_LONGFILENAME
		if(plfn) f_free(plfn);
#endif
		return ret;
	}

#if F_LONGFILENAME
	ret = _f_removelfn(dirpos.cluster, plfn);
	if(plfn) f_free(plfn);
#else
	de->name[0] = (char)0xe5; /*removes it*/
	ret = _f_writedirsector();
#endif

	if(ret) return ret;

	return _f_removechain(_f_getdecluster(de));
}
#endif


/****************************************************************************
 *
 * fn_seteof
 *
 * Set end of file
 *
 * INPUT:	f - file pointer
 *		filesize - required new size
 * RETURN:	F_NO_ERROR - on success
 *		other if error
 *
 ***************************************************************************/
#if (F_SETEOF && F_WRITING)
unsigned char fn_seteof (F_FILE *f, long filesize)
{
	unsigned char rc = F_NO_ERROR;

	if(!f) return F_ERR_NOTOPEN;  /*if error*/
	if(filesize < f->filesize)
	{
		rc = _f_emptywritebuffer(f, NULL);
		if(rc == F_NO_ERROR)
		{
			rc = _f_fseek(f, filesize);
			if(rc == F_NO_ERROR)
			{
				unsigned long cluster;
				rc = _f_getclustervalue(f->pos.cluster, &cluster);
				if(rc == F_NO_ERROR)
				{
					if(cluster != F_CLUSTER_LAST)
					{
						rc = _f_removechain(cluster);
						if(rc) return rc;
						rc = _f_setclustervalue(f->pos.cluster, F_CLUSTER_LAST);
						if(rc) return rc;
					}
					f->filesize = filesize;
				}      
			}
		}
	}
	else if(filesize > f->filesize)
	{
		rc = _f_emptywritebuffer(f, NULL);
		if(rc == F_NO_ERROR) rc = _f_fseek(f, filesize);
	}
	return rc;
}
#endif

/****************************************************************************
 *
 * fn_truncate
 *
 * Open a file and set end of file
 *
 * INPUT:	filename - name of the file 
 *		filesize - required new size
 * RETURN:	NULL on error, otherwise file pointer
 *
 ***************************************************************************/
#if (F_TRUNCATE && F_SETEOF && F_WRITING)
F_FILE *fn_truncate (const char *filename, long filesize)
{
	F_FILE *f = fn_open(filename, "r+");
	unsigned char rc;
	if (f != NULL) 
	{
		rc = _f_fseek(f, f->filesize);
		if (rc == F_NO_ERROR)
		{
			rc = fn_seteof(f, filesize);
		}
		if (rc)
		{
			fn_close(f);
			f = NULL;
		}
	}
	return f;
}
#endif

/****************************************************************************
 *
 * fn_duphandle
 *
 * duplicate a new handle from a given handle
 *
 * INPUTS
 *
 * fsrc - the given handle to duplicate
 *
 * RETURNS
 *
 * F_FILE pointer if successfully
 * 0 - if any error
 *
 ***************************************************************************/
#if F_DUPHANDLE
F_FILE *fn_duphandle(F_FILE *fsrc)
{
	F_FILE *f;
	if(fsrc == NULL) return 0; /* not opened */
	if(fsrc->mode & F_FILE_RD) /* only for 'r' mode */
	{
		f=_f_getintfile();
		if (!f) return 0; /*no more file could be opened */
		_memcpy(f, fsrc, sizeof(F_FILE));
		return f;
	}

	return 0;	
}
#endif

/****************************************************************************
 *
 * fn_combine
 *
 * combine two files 
 *
 * INPUTS
 *
 * filename1, filename2 - files to combine (with or without path)
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

#if (F_COMBINE && F_SEEK && F_WRITING)
unsigned char fn_combine (const char *filename1, const char *filename2)
{
	unsigned char ret;
	unsigned long cluster_size;
	F_FILE *f1, *f2;

	f1 = fn_open(filename1, "r");
	f2 = fn_open(filename2, "r");
	if (!f1 || !f2) {
		ret = F_ERR_NOTFOUND;
		goto combine_return;
	}

	ret = _f_fseek(f1, f1->filesize);
	if (ret) 
		goto combine_return;

	if (f1->startcluster) {
		if (f2->startcluster) {
			/* last cluster of file1 = start cluster of file2 */
			ret = _f_setclustervalue(f1->pos.cluster, f2->startcluster);
			if (ret)
				goto combine_return;

			cluster_size = fn_getclustersize();
			if (!cluster_size) {
				ret = F_ERR_ONDRIVE;
				goto combine_return;
			}
			f1->filesize = (f1->filesize + cluster_size - 1) / cluster_size * cluster_size;
			f1->filesize += f2->filesize;			
		}
	} else {
		f1->startcluster = f2->startcluster;
		f1->filesize = f2->filesize;
	}

	ret = _f_updatefileentry(f1, 0);
	if (ret)
		goto combine_return;

	f2->startcluster = 0;
	f2->filesize = 0;
	ret = _f_updatefileentry(f2, 0);
	if (ret)
		goto combine_return;

	ret = fn_close(f2);
	if (ret) 
		goto combine_return;

	f2 = NULL;

	ret = fn_delete(filename2);

combine_return:
	if (f1)
		fn_close(f1);
	if (f2)
		fn_close(f2);
	
	return ret;
}
#endif


