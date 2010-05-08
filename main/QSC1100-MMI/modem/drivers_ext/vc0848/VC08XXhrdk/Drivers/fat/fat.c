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

#include "fat.h"
#include "util.h"
#include "volume.h"
#include "drv.h"

#include "mmd_data_type.h"


/****************************************************************************
 *
 * _f_writefatsector
 *
 * writing fat sector into volume, this function check if fat was modified
 * and writes data
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/
#if F_WRITING
unsigned char _f_writefatsector(void) 
{
	unsigned char a;
	if(gl_volume.fatmodified) 
	{
		unsigned long fatsector = gl_volume.firstfat.sector + gl_volume.fatsector;

		if (gl_volume.fatsector >= gl_volume.firstfat.num) 
		{
			return F_ERR_INVALIDSECTOR;
		}

		for (a = 0; a < gl_volume.bootrecord.number_of_FATs; a++) 
		{
			unsigned char ret;
			ret = _f_writesector(gl_volume.fat,fatsector);
			if (ret) return ret;
			fatsector += gl_volume.firstfat.num;
		}

		gl_volume.fatmodified = 0;

	}

	return F_NO_ERROR;
}
#endif



/****************************************************************************
 *
 * _f_getfatsector
 *
 * read a fat sector from media
 *
 * INPUTS
 *
 * sector - which fat sector is needed, this sector number is zero based
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/
unsigned char _f_getfatsector(unsigned long sector) 
{
	unsigned char a;
	if (gl_volume.fatsector!=sector) 
	{
		unsigned long fatsector;


#if F_WRITING
		if (gl_volume.fatmodified) 
		{
			unsigned char ret;
			ret=_f_writefatsector();
			if (ret) return ret;
		}
#endif

		gl_volume.fatsector=sector;

		if (gl_volume.fatsector>=gl_volume.firstfat.num) 
		{
			return F_ERR_INVALIDSECTOR;
		}

		fatsector=gl_volume.firstfat.sector+gl_volume.fatsector;

		for (a=0; a<gl_volume.bootrecord.number_of_FATs; a++) 
		{
			if (!_f_readsector(gl_volume.fat,fatsector)) 
			{
				return F_NO_ERROR;
			}
			fatsector+=gl_volume.firstfat.num;
		}
		return F_ERR_READ;
	}

	return F_NO_ERROR;
}



/****************************************************************************
 *
 * _f_setclustervalue
 *
 * set a cluster value in the FAT
 *
 * INPUTS
 *
 * cluster - which cluster's value need to be modified
 * data - new value of the cluster
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

#if F_WRITING

unsigned char _f_setclustervalue(unsigned long cluster, unsigned long _tdata) 
{
	unsigned char ret;

	switch (gl_volume.mediatype)
	{
#if F_FAT16
		case F_FAT16_MEDIA:
		{
			unsigned long sector=cluster;
			unsigned short s_data=(unsigned short)(_tdata & 0xffff); /*keep 16 bit only*/

			sector/=(F_SECTOR_SIZE/2);
			cluster-=sector*(F_SECTOR_SIZE/2);

			ret=_f_getfatsector(sector);
			if (ret) return ret;

			if (_f_getword(&gl_volume.fat[cluster<<1])!=s_data) 
			{
				_f_setword(&gl_volume.fat[cluster<<1],s_data);
				gl_volume.fatmodified = 1;
			}
		}
		break;
#endif
#if F_FAT12
		case F_FAT12_MEDIA:
		{
			unsigned char f12new[2];
			unsigned long sector=cluster;
			unsigned short pos;
			unsigned short s_data=(unsigned short)(_tdata & 0x0fff); /*keep 12 bit only*/

			if (cluster&1) s_data <<= 4;

			_f_setword(f12new,s_data); /*create new data*/

			sector+=sector/2;		  /*1.5 bytes*/
			pos=sector%F_SECTOR_SIZE;
			sector/=F_SECTOR_SIZE;

			ret=_f_getfatsector(sector);
			if (ret) return ret;

			if (cluster&1) 
			{
				f12new[0] |= gl_volume.fat[pos] & 0x0f;
			}

			if (gl_volume.fat[pos]!=f12new[0]) 
			{
				gl_volume.fat[pos]=f12new[0];
				gl_volume.fatmodified=1;
			}

			pos++;
			if (pos>=512) 
			{
				ret=_f_getfatsector(sector+1);
				if (ret) return ret;
				pos=0;
			}

			if (!(cluster&1)) 
			{
				f12new[1] |= gl_volume.fat[pos] & 0xf0;
			}

			if (gl_volume.fat[pos]!=f12new[1]) 
			{
				gl_volume.fat[pos]=f12new[1];
				gl_volume.fatmodified=1;
			}
		}
		break;
#endif
#if F_FAT32
		case F_FAT32_MEDIA:
		{
			unsigned long sector=cluster;
			unsigned long oldv;

			sector/=(F_SECTOR_SIZE/4);
			cluster-=sector*(F_SECTOR_SIZE/4);

			ret=_f_getfatsector(sector);
			if (ret) return ret;

			oldv=_f_getlong(&gl_volume.fat[cluster<<2]);

			_tdata&=0x0fffffff;
			_tdata|=oldv & 0xf0000000; /*keep 4 top bits*/

			if (_tdata!=oldv) 
			{
				_f_setlong(&gl_volume.fat[cluster<<2],_tdata);
				gl_volume.fatmodified=1;
			}
		}
		break;
#endif
		default: return F_ERR_INVALIDMEDIA;
	}

	return F_NO_ERROR;
}

#endif

/****************************************************************************
 *
 * _f_getclustervalue
 *
 * get a cluster value from FAT
 *
 * INPUTS
 *
 * cluster - which cluster value is requested
 * pvalue - where to store data 
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/
unsigned char _f_getclustervalue(unsigned long cluster,unsigned long *pvalue) 
{
	unsigned long val;
	unsigned char ret;
	
	switch(gl_volume.mediatype)
	{
#if F_FAT16
		case F_FAT16_MEDIA:
		{
			unsigned long sector=cluster;
			sector/=(F_SECTOR_SIZE/2);
			cluster-=sector*(F_SECTOR_SIZE/2);

			ret=_f_getfatsector(sector);
			if (ret) return ret;

			val=_f_getword(&gl_volume.fat[cluster<<1]);
			if (val>=(F_CLUSTER_RESERVED&0xffff)) val|=0x0fff0000;	 /*extends it*/

			if (pvalue) *pvalue=val;
		}
		break;
#endif
#if F_FAT12
		case F_FAT12_MEDIA:
		{
			unsigned char dataf12[2];
			unsigned long sector=cluster;
			unsigned short pos;

			sector+=sector/2;		  /*1.5 bytes*/
			pos=sector % F_SECTOR_SIZE;
			sector/=F_SECTOR_SIZE;

			ret=_f_getfatsector(sector);
			if (ret) return ret;

			dataf12[0]=gl_volume.fat[pos++];

			if (pos>=512) 
			{
				ret=_f_getfatsector(sector+1);
				if (ret) return ret;
				pos=0;
			}

			dataf12[1]=gl_volume.fat[pos];

			val=_f_getword(dataf12);

			if (cluster&1) 
			{
				val = val >> 4;
			}
			else 
			{
				val = val & 0xfff;
			}

			if (val>=(F_CLUSTER_RESERVED&0xfff)) val|=0x0ffff000;	/*extends it*/

			if (pvalue) *pvalue=val;
		}
		break;
#endif
#if F_FAT32
		case F_FAT32_MEDIA:
		{
			unsigned long sector=cluster;
			sector/=(F_SECTOR_SIZE/4);
			cluster-=sector*(F_SECTOR_SIZE/4);

			ret=_f_getfatsector(sector);
			if (ret) return ret;

			if (pvalue) *pvalue=_f_getlong(&gl_volume.fat[cluster<<2]) & 0x0fffffff; /*28bit*/
		}
		break;
#endif
		default: return F_ERR_INVALIDMEDIA;
	}
	
	return(F_NO_ERROR);
}


/****************************************************************************
 *
 * _f_clustertopos
 *
 * convert a cluster position into physical sector position
 *
 * INPUTS
 *
 * cluster - original cluster position
 * pos - position structure to fills the position
 *
 ***************************************************************************/
void _f_clustertopos(unsigned long cluster, F_POS *pos) 
{
	pos->cluster=cluster;

	if (!cluster) 
	{
		pos->sector=gl_volume.root.sector;
		pos->sectorend=pos->sector+gl_volume.root.num;
	}
	else 
	{
		unsigned long sectorcou=gl_volume.bootrecord.sector_per_cluster;
		pos->sector=(pos->cluster-2)*sectorcou+gl_volume._tdata.sector;
		pos->sectorend=pos->sector+sectorcou;
	}

	if (cluster>=F_CLUSTER_RESERVED) 
	{
		pos->sectorend=0;
	}

	pos->pos=0;
}




/****************************************************************************
 *
 * _f_getcurrsector
 *
 * read current sector according in file structure
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
unsigned char _f_getcurrsector(F_FILE *f,void *ptr) 
{
	unsigned char ret, cached = 0;//empty = 1;
	unsigned long cluster;

	if(f->pos.sector == f->pos.sectorend)
	{
		/* end of cluster */
		ret = _f_getclustervalue(f->pos.cluster, &cluster);
		if(ret) return ret;
		if(cluster >= F_CLUSTER_RESERVED) return F_ERR_EOF;
		f->prevcluster = f->pos.cluster;		
		_f_clustertopos(cluster, &f->pos); /* get next cluster */
	}

	if(!ptr) 
	{	
		ptr = f->_tdata;
		//empty = 0;
		cached = 1;
	}
	ret = _f_readsector(ptr, f->pos.sector);
	if(ret) return ret;

	//if(!empty) f->empty = empty;
	if(cached) f->cached = cached;

	return F_NO_ERROR;
}


/****************************************************************************
 *
 * _f_getmultisector
 *
 * read multi consecutive sectors according in file structure, added by Chen Jing'en
 *
 * INPUTS
 *
 * f - internal file pointer
 * ptr - data pointer where store data
 * max - max. number of sectors to read
 * size - where to store number of read bytes
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/
unsigned char _f_getmultisector(F_FILE *f, void *ptr, unsigned int max, unsigned long *size) 
{
	unsigned char ret;
	unsigned long cluster, sector;
	unsigned int num = 0;

	while(num < max)
	{
		if (f->pos.sector == f->pos.sectorend)
		{
			/* end of cluster */
			ret=_f_getclustervalue(f->pos.cluster, &cluster);
			if (ret) return ret;
			
			if (cluster >= (unsigned long)F_CLUSTER_RESERVED) break;
			
			if(num && (f->pos.cluster != cluster-1)) break;

			f->prevcluster = f->pos.cluster;
			_f_clustertopos(cluster, &f->pos);			
		}

		if(!num) sector = f->pos.sector; /* start sector */

		num++;		
		f->pos.sector++;
	}

	if(num) f->pos.sector--; /* move back to current sector */
	if(size) *size = num * F_SECTOR_SIZE;

	if(num > 1)
	{
		return _f_readmultisector(ptr, sector, num);
	}
	else if(num == 1)
	{
		return _f_readsector(ptr, f->pos.sector);
	}
	else
	{
		return F_ERR_EOF;
	}
}


/****************************************************************************
 *
 * _f_alloccluster
 *
 * allocate cluster from FAT
 *
 * INPUTS
 * pcluster - where to store the allocated cluster number
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/
#if F_WRITING
unsigned char _f_alloccluster(unsigned long *pcluster) 
{
	unsigned long maxcluster = gl_volume.maxcluster;
	unsigned long cou;
	unsigned long cluster = gl_volume.lastalloccluster;
	unsigned long value;
	unsigned char ret;
	
	for (cou = 0; cou < maxcluster; cou++) 	
	{

		//if (cluster >= maxcluster) cluster = 0;
		if (cluster >= (maxcluster + 2) || cluster < 2) cluster = 2;

		ret = _f_getclustervalue(cluster, &value);
		if (ret) return ret;

		if (!value) 
		{
			gl_volume.lastalloccluster = cluster+1; /*set next one*/
			*pcluster = cluster;

			return F_NO_ERROR;
		}

		cluster++;
	}

	return F_ERR_NOMOREENTRY;
}
#endif



/****************************************************************************
 *
 * _f_dobadblock
 *
 * Bad block handler, if a given block is bad, this routine mark the cluster
 * as bad, and change the file pointer position values
 *
 * INPUTS
 * f - internal file pointer which contains bad block
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/
#if F_WRITING
unsigned char _f_dobadblock(F_FILE *f) 
{
	F_POS posold;
	F_POS pos;
	unsigned long cluster;
	unsigned long oldclustervalue;
	unsigned long currdif = f->pos.sectorend - f->pos.sector;
	unsigned char ret;

	ret = _f_getclustervalue(f->pos.cluster, &oldclustervalue); /*get old value*/
	if (ret) return ret;

	ret = _f_setclustervalue(f->pos.cluster, F_CLUSTER_BAD); /*set signal as bad*/
	if (ret) return ret;

write_again:
	ret = _f_alloccluster(&cluster);						  /*get a new one*/
	if (ret) return ret;

	ret = _f_setclustervalue(cluster, oldclustervalue);	/*set old value back*/
	if (ret) return ret;

	_f_clustertopos(cluster, &pos);			   /*set new data position*/
	_f_clustertopos(f->pos.cluster, &posold); /*set old data position*/

	(void)_f_writefatsector();			/* write actual fat sector */
	gl_volume.fatsector = (unsigned long)-1;			/* and set actual fat sector to unknown */

	for (;pos.sector < pos.sectorend; pos.sector++, posold.sector++) 
	{
		ret = _f_readsector(gl_volume.fat,posold.sector);		/* gl_volume.fat is just a temporary buffer */
		if (ret) return ret; /*cannot read!!!*/

		ret = _f_writesector(gl_volume.fat,pos.sector);
		if (ret) 
		{
			if (gl_volume.state != F_STATE_WORKING) return ret; /*not working*/

			ret = _f_setclustervalue(cluster, F_CLUSTER_BAD); /*set signal as bad also*/
			if (ret) return ret;

			goto write_again; /*this will be bad sector as well*/
		}
	}

	_f_clustertopos(cluster, &f->pos);
	f->pos.sector = f->pos.sectorend - currdif; /*lets go back to current sector*/

	return F_NO_ERROR;
}
#endif




/****************************************************************************
 *
 * _f_removechain
 *
 * remove cluster chain from fat
 *
 * INPUTS
 * cluster - first cluster in the cluster chain
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/
#if F_WRITING

unsigned char _f_removechain(unsigned long cluster) 
{

	if (cluster < gl_volume.lastalloccluster) 
	{ 
		/*this could be the begining of alloc*/
		gl_volume.lastalloccluster = cluster;
	}

	while (cluster < F_CLUSTER_RESERVED && cluster >= 2) 
	{
		unsigned long nextcluster;

		unsigned char ret = _f_getclustervalue(cluster, &nextcluster);
		if (ret) return ret;

		ret = _f_setclustervalue(cluster, F_CLUSTER_FREE);
		if (ret) return ret;

		cluster = nextcluster;
	}	

	return _f_writefatsector();
}

#endif



