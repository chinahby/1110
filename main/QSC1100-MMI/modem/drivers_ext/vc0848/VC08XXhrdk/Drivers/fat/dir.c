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

#include "dir.h"
#include "util.h"
#include "volume.h"
#include "drv.h"
#include "file.h"
#include "fat.h"


#if !(F_LONGFILENAME)

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
	unsigned char ret=F_NO_ERROR;
	if (gl_volume.direntrysector!=sector) 
	{
		ret=_f_readsector(gl_volume.direntry,sector);

		if (!ret) gl_volume.direntrysector=sector;
		else gl_volume.direntrysector=(unsigned long)(-1);
	}
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
	if (gl_volume.direntrysector!=(unsigned long)(-1)) 
	{
		return _f_writesector(gl_volume.direntry,gl_volume.direntrysector);
	}
	return F_ERR_INVALIDSECTOR;
}

#endif


/****************************************************************************
 *
 * _f_findfilewc
 *
 * internal function to finding file in directory entry with or without
 * wildcard
 *
 * INPUTS
 *
 * name - filename
 * ext - fileextension
 * pos - where to start searching, and contains current position
 * pde - store back the directory entry pointer
 * wc - wildcard checking?
 *
 * RETURNS
 *
 * 0 - if file was not found
 * 1 - if file was found
 *
 ***************************************************************************/
#if F_FINDING
unsigned char _f_findfilewc(char *name,char *ext,F_POS *pos, F_DIRENTRY **pde, unsigned char wc)
#else
unsigned char _f_findfilewc(char *name,char *ext,F_POS *pos, F_DIRENTRY **pde) 
#endif
{

	while (pos->cluster<F_CLUSTER_RESERVED) 
	{

		for (;pos->sector<pos->sectorend; pos->sector++) 
		{
			F_DIRENTRY *de=(F_DIRENTRY*)(gl_volume.direntry+sizeof(F_DIRENTRY)*pos->pos);

			if (_f_getdirsector(pos->sector)) return 0; /*not found*/

			for (; pos->pos<F_SECTOR_SIZE/sizeof(F_DIRENTRY);de++,pos->pos++) 
			{
				unsigned char b,ok;

				if (!de->name[0]) return 0;						 /*empty*/
				if ((unsigned char)(de->name[0])==0xe5) continue; /*deleted*/
				if (de->attr & F_ATTR_VOLUME) continue;

#if F_FINDING
				if (wc)
				{
					for (b=0,ok=1; b<sizeof(de->name); b++) 
					{
						if (name[b]=='*') break;

						if (name[b]!='?') 
						{
							if (de->name[b]!=name[b]) 
							{
								ok=0;
								break;
							}
						}
					}

					if (ok)
					{
						for (b=0,ok=1; b<sizeof(de->ext); b++) 
						{
							if (ext[b]=='*') 
							{
								if (pde) *pde=de;
								return 1;
							}

							if (ext[b]!='?') 
							{
								if (de->ext[b]!=ext[b]) 
								{
									ok=0;
									break;
								}
							}

						}

						if (ok) 
						{
							if (pde) *pde=de;
							return 1;
						}
					}
				}
				else
#endif
				{
					for (b=0,ok=1; b<sizeof(de->name); b++) 
					{
						if (de->name[b]!=name[b]) 
						{
							ok=0;
							break;
						}
					}

					if (ok)
					{
						for (b=0,ok=1; b<sizeof(de->ext); b++) 
						{
							if (de->ext[b]!=ext[b]) 
							{
								ok=0;
								break;
							}
						}

						if (ok) 
						{
							if (pde) *pde=de;
							return 1;
						}
					}
				}
			}

			pos->pos=0;
		}

		if (!pos->cluster) 
		{
#if F_FAT32
			if (gl_volume.mediatype==F_FAT32_MEDIA) 
			{
				pos->cluster=gl_volume.bootrecord.rootcluster;
			}
			else 
#endif
				return 0; /*root directory*/
		}

		{
			unsigned long nextcluster;
			if (_f_getclustervalue(pos->cluster,&nextcluster)) return 0; /*not found*/

			if (nextcluster>=F_CLUSTER_RESERVED) return 0; /*eof*/

			_f_clustertopos(nextcluster,pos);
		}
	}

	return 0;
}


/****************************************************************************
 *
 * _f_getfilename
 *
 * create a complete filename from name and extension
 *
 * INPUTS
 *
 * dest - where to store filename
 * name - name of the file
 * ext - extension of the file
 *
 ***************************************************************************/
static void _f_getfilename(char *dest, char *name,char *ext) 
{
	unsigned char a,len;

	for (len=a=F_MAXNAME; a; a--,len--) 
	{
		if (name[a-1]!=' ') break;
	}
	for (a=0; a<len; a++) *dest++=*name++;


	for (len=a=F_MAXEXT; a; a--,len--) 
	{
		if (ext[a-1]!=' ') break;
	}
	if (len) *dest++='.';

	for (a=0; a<len; a++) *dest++=*ext++;

	*dest=0; /*terminateit*/
}


/****************************************************************************
 *
 * _f_getdecluster
 *
 * get a directory entry structure start cluster value
 *
 * INPUTS
 *
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
	if (gl_volume.mediatype==F_FAT32_MEDIA) 
	{
		cluster=_f_getword(&de->clusterhi);
		cluster<<=16;
		cluster|=_f_getword(&de->clusterlo);
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
 *
 * de - directory entry
 * cluster - value of the start cluster
 *
 ***************************************************************************/

#if F_WRITING

void _f_setdecluster(F_DIRENTRY *de,unsigned long cluster) 
{
	_f_setword(&de->clusterlo,(unsigned short)(cluster&0xffff));

#if F_FAT32
	if (gl_volume.mediatype==F_FAT32_MEDIA) 
	{
		_f_setword(&de->clusterhi,(unsigned short)(cluster>>16));
	}
	else
#endif
	{
		_f_setword(&de->clusterhi,(unsigned short)0);
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
 *
 * fsname - filled structure with path,drive
 * pos - where to start searching, and contains current position
 *
 * RETURNS
 *
 * 0 - if path was not found or invalid
 * 1 - if path was found
 *
 ***************************************************************************/

unsigned char _f_findpath(F_NAME *fsname,F_POS *pos) 
{
#if F_DIRECTORIES
	char *path=fsname->path;
	char *mpath=path;
	F_DIRENTRY *de;

	_f_clustertopos(0,pos);

	for (;*path;) 
	{
		char name[F_MAXNAME];
		char ext[F_MAXEXT];

		unsigned char len=_f_setnameext(path,name,ext);

		if (pos->cluster==0 && len==1 && name[0]=='.') 
		{
			_f_clustertopos(0,pos);
		}
		else 
		{
#if F_FINDING
			if (!_f_findfilewc(name,ext,pos,&de,0)) return 0;
#else
			if (!_f_findfilewc(name,ext,pos,&de)) return 0;
#endif
			if (!(de->attr & F_ATTR_DIR)) return 0;

			_f_clustertopos(_f_getdecluster(de),pos);
		}


		if (name[0]=='.') 
		{
			if (len==1) 
			{
				path+=len;

				if (!(*path)) 
				{
					if (mpath!=fsname->path) mpath--; /*if we are now at the top*/
						break;
				}
				path++;
				continue;
			}

			if (name[1]!='.') return 0; /*invalid name*/
			if (len!=2) return 0; /*invalid name !*/

			path+=len;

			if (mpath==fsname->path) return 0; /*we are in the top*/

			mpath--; /*no on separator*/
			for (;;) 
			{
				if (mpath==fsname->path) break; /*we are now at the top*/
				mpath--;
				if (*mpath=='/') 
				{
					mpath++;
					break;
				}
			}

			if (!(*path)) 
			{
				if (mpath!=fsname->path) mpath--; /*if we are now at the top*/
				break;
			}
			path++;
			continue;
		}
		else 
		{
			if (path==mpath) 
			{							
				/*if no was dots just step*/
				path+=len;
				mpath+=len;
			}
			else 
			{
				unsigned char a;
				for (a=0; a<len;a++) *mpath++=*path++;	 /*copy if in different pos*/
			}
		}

		if (!(*path)) break;
		path++;
		*mpath++='/'; /*add separator*/
	}

	*mpath=0; /*terminate it*/
	return 1;
#else
	_f_clustertopos(0,pos);
	return 1;
#endif
}


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
 ***************************************************************************/

#if F_DIRECTORIES

unsigned char fn_getcwd(char *buffer, unsigned char maxlen, char root ) 
{
	unsigned char a;

	if (!maxlen) return F_NO_ERROR;

	maxlen--;	/*need for termination*/

	if (root && maxlen)
	{
		*buffer++='/';
		maxlen--;
	}

	for (a=0; a<maxlen; a++) 
	{
		char ch=gl_volume.cwd[a];
		buffer[a]=ch;
		if (!ch) break;
	}

	buffer[a]=0;	/*add terminator at the end*/

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

unsigned char fn_findfirst(const char *filename,F_FIND *find) 
{
	unsigned char ret;
	if (_f_setfsname(filename,&find->findfsname)) return F_ERR_INVALIDNAME; /*invalid name*/
#if F_CHECKNAME
	if (_f_checkname(find->findfsname.filename,find->findfsname.fileext)) return F_ERR_INVALIDNAME;/*invalid name, wildcard is ok*/
#endif

	ret=_f_getvolume();
	if (ret) return ret;

	if (!_f_findpath(&find->findfsname,&find->pos)) return F_ERR_INVALIDDIR; /*search for path*/

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
	F_DIRENTRY *de;
	unsigned char a;
	unsigned char ret;

	ret=_f_getvolume();
	if (ret) return ret;

	if (!_f_findfilewc(find->findfsname.filename,find->findfsname.fileext,&find->pos,&de,1)) return F_ERR_NOTFOUND;

	for (a=0;a<F_MAXNAME; a++) find->name[a]=de->name[a];
	for (a=0;a<F_MAXEXT; a++) find->ext[a]=de->ext[a];

	_f_getfilename(find->filename,de->name,de->ext);

	find->attr=de->attr;
	find->cdate=_f_getword(&de->cdate);
	find->ctime=_f_getword(&de->ctime);
	find->filesize=_f_getlong(&de->filesize);
	find->cluster=_f_getdecluster(de);
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
 ***************************************************************************/

#if (F_CHDIR && F_DIRECTORIES)

unsigned char fn_chdir(const char *dirname) 
{
	F_POS pos;
	F_NAME fsname;
	unsigned char len;
	unsigned char a;
	unsigned char ret;

	ret=_f_setfsname(dirname,&fsname);

	if (ret==1) return F_ERR_INVALIDNAME; /*invalid name*/
#if F_CHECKNAME
	if (_f_checknamewc(fsname.filename,fsname.fileext)) return F_ERR_INVALIDNAME;/*invalid name*/
#endif
	ret=_f_getvolume();
	if (ret) return ret;

	for (len=0;fsname.path[len];) len++;
	if (len && (fsname.filename[0]!=32 || fsname.fileext[0]!=32)) fsname.path[len++]='/';

	_f_getfilename(fsname.path+len,fsname.filename,fsname.fileext);

	if (!(_f_findpath(&fsname,&pos))) return F_ERR_NOTFOUND;

	for (a=0; a<F_MAXPATH;a++) gl_volume.cwd[a]=fsname.path[a];

	return F_NO_ERROR;
}

#endif



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

static void _f_initentry(F_DIRENTRY *de,char *name, char *ext) 
{

	_memset (de,0,sizeof(F_DIRENTRY)); /*reset all entries*/

	_memcpy (de->name,name,sizeof(de->name));
	_memcpy (de->ext,ext,sizeof(de->ext));

	_f_setword(&de->cdate,f_getdate());  /*if there is realtime clock then creation date could be set from*/
	_f_setword(&de->ctime,f_gettime());  /*if there is realtime clock then creation time could be set from*/

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
 * fs_name - filled structure what to add into directory list
 * pos - where directory cluster chains starts
 * pde - F_DIRENTRY pointer where to store the entry where it was added
 *
 * RETURNS
 *
 * 0 - if successfully added
 * other - if any error (see FS_xxx errorcodes)
 *
 ***************************************************************************/

#if F_WRITING

unsigned char _f_addentry(F_NAME *fsname,F_POS *pos,F_DIRENTRY **pde) 
{
	unsigned char ret;

	if (!fsname->filename[0]) return F_ERR_INVALIDNAME;
	if (fsname->filename[0]=='.') return F_ERR_INVALIDNAME;

	while (pos->cluster<F_CLUSTER_RESERVED) 
	{

		for (;pos->sector<pos->sectorend; pos->sector++) 
		{
			F_DIRENTRY *de=(F_DIRENTRY*)(gl_volume.direntry+sizeof(F_DIRENTRY)*pos->pos);

			ret=_f_getdirsector(pos->sector);
			if (ret) return ret;

			for (; pos->pos<F_SECTOR_SIZE/sizeof(F_DIRENTRY);de++,pos->pos++) 
			{
				if ((!de->name[0]) || ((unsigned char)(de->name[0])==0xe5)) 
				{
					_f_initentry(de,fsname->filename,fsname->fileext);

#if F_FAT32
					if (gl_volume.mediatype==F_FAT32_MEDIA) 
					{
						_f_setword(&de->crtdate,f_getdate());  /*if there is realtime clock then creation date could be set from*/
						_f_setword(&de->crttime,f_gettime());  /*if there is realtime clock then creation time could be set from*/
						_f_setword(&de->lastaccessdate,f_getdate());  /*if there is realtime clock then creation date could be set from*/
					}
#endif
					if (pde) *pde=de;
					return F_NO_ERROR;
				}
			}

			pos->pos=0;
		}

		if (!pos->cluster) 
		{
#if F_FAT32
			if (gl_volume.mediatype==F_FAT32_MEDIA) 
			{
				pos->cluster=gl_volume.bootrecord.rootcluster;
			}
			else 
#endif
				return F_ERR_NOMOREENTRY; /*root directory, cannot continue*/
		}

		{
			unsigned long cluster;

			ret=_f_getclustervalue(pos->cluster,&cluster); /*try to get next cluster*/
			if (ret) return ret;
	
			if (cluster<F_CLUSTER_RESERVED) 
			{
				_f_clustertopos(cluster,pos);
			}
			else 
			{
				ret=_f_alloccluster(&cluster);	/*get a new one*/
				if (ret) return ret;

				if (cluster<F_CLUSTER_RESERVED) 
				{
					F_FILE *f=_f_getintfile();
					if (!f) return F_ERR_NOMOREENTRY;

					_f_clustertopos(cluster,&f->pos);


					_memset(f->_tdata,0,F_SECTOR_SIZE);
					while(f->pos.sector<f->pos.sectorend) 
					{
write_again:
						ret=_f_writesector(f->_tdata,f->pos.sector);
						if (ret) 
						{
							if (gl_volume.state!=F_STATE_WORKING) return ret; /*check if still working volume*/

							ret=_f_dobadblock(f);
							if (ret) return ret;

							goto write_again;
						}
						f->pos.sector++;
					}

					ret=_f_setclustervalue(f->pos.cluster,F_CLUSTER_LAST);
					if (ret) return ret;

					ret=_f_setclustervalue(pos->cluster,f->pos.cluster);
					if (ret) return ret;

					ret=_f_writefatsector();
					if (ret) return ret;

					_f_clustertopos(f->pos.cluster,pos);
				}
				else 
					return F_ERR_NOMOREENTRY;

			}
		}
	}

	return F_ERR_NOMOREENTRY;
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
 ***************************************************************************/

#if (F_MKDIR && F_DIRECTORIES && F_WRITING)

unsigned char fn_mkdir(const char *dirname) 
{
	F_FILE *f;
	F_POS posdir;
	F_POS pos;
	F_DIRENTRY *de;
	F_NAME fsname;
	unsigned long cluster;
	unsigned char ret;

	if (_f_setfsname(dirname,&fsname)) return F_ERR_INVALIDNAME; /*invalid name*/
#if F_CHECKNAME
	if (_f_checknamewc(fsname.filename,fsname.fileext)) return F_ERR_INVALIDNAME;/*invalid name*/
#endif
	ret=_f_getvolume();
	if (ret) return ret;

	if (!_f_findpath(&fsname,&posdir)) return F_ERR_INVALIDDIR;

	pos=posdir;

	if (fsname.filename[0]=='.') return F_ERR_NOTFOUND;
#if F_FINDING
	if (_f_findfilewc(fsname.filename,fsname.fileext,&pos,&de,0)) return F_ERR_DUPLICATED;
#else
	if (_f_findfilewc(fsname.filename,fsname.fileext,&pos,&de)) return F_ERR_DUPLICATED;
#endif

	pos=posdir;

	ret=_f_alloccluster(&cluster);
	if (ret) return ret;

	ret=_f_addentry(&fsname,&pos,&de);
	if (ret) return ret;

	de->attr |= F_ATTR_DIR;		/*set as directory*/

	f=_f_getintfile();

	if (!f) return F_ERR_NOMOREENTRY;

	_f_clustertopos(cluster,&f->pos);

	_f_setdecluster(de,cluster); /*new dir*/


	de=(F_DIRENTRY *)(f->_tdata);

	_f_initentry(de,".       ","   ");
	de->attr = F_ATTR_DIR;		/*set as directory*/
	_f_setdecluster(de,cluster);	 /*current*/
	de++;

	_f_initentry(de,"..      ","   ");
	de->attr = F_ATTR_DIR;		/*set as directory*/
	_f_setdecluster(de,posdir.cluster); /*parent*/
	de++;

	_memset(de,0,F_SECTOR_SIZE-2*sizeof(F_DIRENTRY));


	ret=_f_writesector(f->_tdata,f->pos.sector);
	if (ret) return ret;

	f->pos.sector++;

	_memset(f->_tdata,0,2*sizeof(F_DIRENTRY));


	while(f->pos.sector<f->pos.sectorend) 
	{
write_again:
		ret=_f_writesector(f->_tdata,f->pos.sector);
		if (ret) 
		{
			if (gl_volume.state!=F_STATE_WORKING) return ret; /*check if still working volume*/

			ret=_f_dobadblock(f);
			if (ret) return ret;

			goto write_again;
		}

		f->pos.sector++;
	}

	ret=_f_setclustervalue(f->pos.cluster,F_CLUSTER_LAST);
	if (ret) return ret;

	ret=_f_writefatsector();
	if (ret) return ret;

	return _f_writedirsector(); /*writing  direntry*/
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

unsigned char fn_settimedate(const char *filename,unsigned short ctime,unsigned short cdate) 
{
	F_POS pos;
	F_DIRENTRY *de;
	F_NAME fsname;
	unsigned char ret;

	if (_f_setfsname(filename,&fsname)) return F_ERR_INVALIDNAME; /*invalid name*/
#if F_CHECKNAME
	if (_f_checknamewc(fsname.filename,fsname.fileext)) return F_ERR_INVALIDNAME;/*invalid name*/
#endif
	ret=_f_getvolume();
	if (ret) return ret;

	if (!_f_findpath(&fsname,&pos)) return F_ERR_INVALIDDIR;
#if F_FINDING
	if (!_f_findfilewc(fsname.filename,fsname.fileext,&pos,&de,0)) return F_ERR_NOTFOUND;
#else
	if (!_f_findfilewc(fsname.filename,fsname.fileext,&pos,&de)) return F_ERR_NOTFOUND;
#endif

	_f_setword(&de->ctime,ctime);
	_f_setword(&de->cdate,cdate);

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

unsigned char fn_gettimedate(const char *filename,unsigned short *pctime,unsigned short *pcdate) 
{
	F_POS pos;
	F_DIRENTRY *de;
	F_NAME fsname;
	unsigned char ret;

	if (_f_setfsname(filename,&fsname)) return F_ERR_INVALIDNAME; /*invalid name*/
#if F_CHECKNAME
	if (_f_checknamewc(fsname.filename,fsname.fileext)) return F_ERR_INVALIDNAME;/*invalid name*/
#endif
	ret=_f_getvolume();
	if (ret) return ret;

	if (!_f_findpath(&fsname,&pos)) return F_ERR_INVALIDDIR;
#if F_FINDING
	if (!_f_findfilewc(fsname.filename,fsname.fileext,&pos,&de,0)) return F_ERR_NOTFOUND;
#else
	if (!_f_findfilewc(fsname.filename,fsname.fileext,&pos,&de)) return F_ERR_NOTFOUND;
#endif

	if (pctime) *pctime=_f_getword(&de->ctime);
	if (pcdate) *pcdate=_f_getword(&de->cdate);

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

unsigned char fn_getattr(const char *filename,unsigned char *attr) 
{
	F_POS pos;
	F_DIRENTRY *de;
	F_NAME fsname;
	unsigned char ret;

	if (_f_setfsname(filename,&fsname)) return F_ERR_INVALIDNAME; /*invalid name*/
#if F_CHECKNAME
	if (_f_checknamewc(fsname.filename,fsname.fileext)) return F_ERR_INVALIDNAME;/*invalid name*/
#endif
	ret=_f_getvolume();
	if (ret) return ret;

	if (!_f_findpath(&fsname,&pos)) return F_ERR_INVALIDDIR;
#if F_FINDING
	if (!_f_findfilewc(fsname.filename,fsname.fileext,&pos,&de,0)) return F_ERR_NOTFOUND;
#else
	if (!_f_findfilewc(fsname.filename,fsname.fileext,&pos,&de)) return F_ERR_NOTFOUND;
#endif

	if (attr) *attr=de->attr;

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
	F_POS pos;
	F_DIRENTRY *de;
	F_NAME fsname;
	unsigned char ret;

	if (_f_setfsname(filename,&fsname)) return F_ERR_INVALIDNAME; /*invalid name*/
#if F_CHECKNAME
	if (_f_checknamewc(fsname.filename,fsname.fileext)) return F_ERR_INVALIDNAME;/*invalid name*/
#endif
	ret=_f_getvolume();
	if (ret) return ret;

	if (!_f_findpath(&fsname,&pos)) return F_ERR_INVALIDDIR;
#if F_FINDING
	if (!_f_findfilewc(fsname.filename,fsname.fileext,&pos,&de,0)) return F_ERR_NOTFOUND;
#else
	if (!_f_findfilewc(fsname.filename,fsname.fileext,&pos,&de)) return F_ERR_NOTFOUND;
#endif

	attr&=F_ATTR_HIDDEN|F_ATTR_READONLY|F_ATTR_ARC|F_ATTR_SYSTEM; /*keep only valid bits*/

	de->attr&=F_ATTR_DIR|F_ATTR_VOLUME; /*keep some original*/

	attr|=de->attr;	/*add old settings, (dir,volume)*/

	if (de->attr==attr) return F_NO_ERROR; /*its the same*/

	de->attr=attr; 

	return _f_writedirsector();
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
 ***************************************************************************/

#if (F_RMDIR && F_DIRECTORIES && F_WRITING)

unsigned char fn_rmdir(const char *dirname) 
{
	unsigned char ret;
	F_POS pos;
	F_DIRENTRY *de;
	F_NAME fsname;
	F_FILE *f;
	int a;

	if (_f_setfsname(dirname,&fsname)) return F_ERR_INVALIDNAME; /*invalid name*/
#if F_CHECKNAME
	if (_f_checknamewc(fsname.filename,fsname.fileext)) return F_ERR_INVALIDNAME;/*invalid name*/
	if (fsname.filename[0]=='.') return F_ERR_NOTFOUND;
#endif
	ret=_f_getvolume();
	if (ret) return ret;

	if (!(_f_findpath(&fsname,&pos))) return F_ERR_INVALIDDIR;
#if F_FINDING
	if (!_f_findfilewc(fsname.filename,fsname.fileext,&pos,&de,0)) return F_ERR_NOTFOUND;
#else
	if (!_f_findfilewc(fsname.filename,fsname.fileext,&pos,&de)) return F_ERR_NOTFOUND;
#endif

	if (!(de->attr & F_ATTR_DIR)) return F_ERR_INVALIDDIR;		   /*not a directory*/


	f=_f_getintfile();

	if (!f) return F_ERR_NOMOREENTRY;

	_f_clustertopos(_f_getdecluster(de),&f->pos);

	while (1) {
	F_DIRENTRY *de2;
	ret=_f_getcurrsector(f,NULL);

	if (ret==F_ERR_EOF) break;
	if (ret) return ret;

	de2=(F_DIRENTRY *)(f->_tdata);

	for (a=0; a<(int)(F_SECTOR_SIZE/sizeof(F_DIRENTRY)); a++,de2++) {
	char ch=de2->name[0];
	if (!ch) break;
	if ((unsigned char)ch==0xe5) continue;
	if (ch=='.') continue;
	return F_ERR_NOTEMPTY; /*something is there*/
	}
	f->pos.sector++;
	}

	de->name[0]=(char)0xe5;

	ret=_f_writedirsector();
	if (ret) return ret;

	return _f_removechain(_f_getdecluster(de));
}

#endif


#endif /* !(F_LONGFILENAME) */

