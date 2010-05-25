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

#include "volume.h"
#include "util.h"
#include "drv.h"
#include "fat.h"


#if F_LONGFILENAME
#include "dir_lfn.h"
#else
#include "dir.h"
#endif

typedef struct {
	unsigned long total_sector;
	unsigned char sector_per_cluster;
}F_CLUSTERSIZE;

F_VOLUME gl_volume;	/*only one volume*/

#if F_LONGFILENAME
F_FILE gl_files[F_MAXFILES+1];
#else
F_FILE gl_files[F_MAXFILES];
#endif

F_CLUSTERSIZE gl_clustersize_tbl_FAT16[ ] = {
	{8400, 0},		/* disks up to 4.1 MB, the 0 value for sector_per_cluster trips an error */
	{32680, 2},		/* disks up to 16 MB, 1k cluster */
	{262080, 4},	/* disks up to 128 MB, 2k cluster */
	{524160, 8},	/* disks up to 256 MB, 4k cluster */
	{1048320, 16},	/* disks up to 512 MB, 8k cluster */
	/* The entries after this point are not used unless FAT16 is forced */
	{2096640, 32},	/* disks up to 1 GB, 16k cluster */
	{4193280, 64},	/* disks up to 2 GB, 32k cluster */
	{0xFFFFFFFF, 0} /* any disk greater than 2GB, 0 value for sector_per_cluster trips an error */
};

F_CLUSTERSIZE gl_clustersize_tbl_FAT32[ ] = {
	{66600, 0},		/* disks up to 32.5 MB, the 0 value for sector_per_cluster trips an error */
	{131072, 1},	/* disks up to 64 MB, .5k cluster */
	{262144, 2},	/* disks up to 128MB, 1k cluster */
	{524288, 4},	/* disks up to 256 MB, 2k cluster */
	{4194304, 8},	/* disks up to 2 GB, 4k cluster */
	{16777216, 16},	/* disks up to 8 GB, 8k cluster */
	{67108864, 32}, /* disks up to 32 GB, 16k cluster */
	{0xFFFFFFFF, 64}/* disks greater than 32GB, 32k cluster */
};

#if (F_FORMATTING && F_WRITING)
/****************************************************************************
 *
 * _f_disksz2clustersz
 *
 * calculate appropriate cluster size from the size of disk
 *
 ***************************************************************************/
static unsigned char _f_disksz2clustersz(unsigned long disksize, unsigned char fattype, unsigned char *p_clustersize) 
{
	unsigned long max_clusters, cluster_size;
	unsigned int i;

	*p_clustersize = 0;
	
	switch (fattype) {
#if F_FAT12
	case F_FAT12_MEDIA: 
		max_clusters = F_CLUSTER_RESERVED & 0xfff; /* 12 bits */
		cluster_size = 1;
		while (cluster_size) {
			if (disksize / cluster_size < max_clusters) 
				break;
			cluster_size <<= 1;
		}
		break;
#endif
#if F_FAT16
	case F_FAT16_MEDIA:
		//max_clusters = F_CLUSTER_RESERVED & 0xffff; /* 16 bits */
		if (disksize <= gl_clustersize_tbl_FAT16[0].total_sector)
			return F_ERR_MEDIATOOSMALL;
		else {
			for (i = 1; ; i++) {
				if (disksize < gl_clustersize_tbl_FAT16[i].total_sector) {
					cluster_size = gl_clustersize_tbl_FAT16[i].sector_per_cluster;
					break;
				}
			}
		}
		break;
#endif
#if F_FAT32
	case F_FAT32_MEDIA: 
		//max_clusters = F_CLUSTER_RESERVED; /* 28 bits */
		if (disksize <= gl_clustersize_tbl_FAT32[0].total_sector)
			return F_ERR_MEDIATOOSMALL;
		else {
			for (i = 1; ; i++) {
				if (disksize < gl_clustersize_tbl_FAT32[i].total_sector) {
					cluster_size = gl_clustersize_tbl_FAT32[i].sector_per_cluster;
					break;
				}
			}
		}
		break;
#endif
	default: 
		return F_ERR_INVFATTYPE;
	}

	
	if (!cluster_size) 
		return F_ERR_MEDIATOOLARGE;

	*p_clustersize = cluster_size;
	return F_NO_ERROR;	
}

/****************************************************************************
 *
 * _f_buildsectors
 *
 * calculate relative sector position from boot record
 *
 ***************************************************************************/
static unsigned char _f_buildsectors(void) 
{
	gl_volume.mediatype=F_UNKNOWN_MEDIA;

	gl_volume.firstfat.sector=gl_volume.bootrecord.reserved_sectors+gl_volume.bootrecord.number_of_hidden_sectors; /*maybe partioned*/

	if(gl_volume.bootrecord.sector_per_FAT) 
	{
		gl_volume.firstfat.num=gl_volume.bootrecord.sector_per_FAT;

		gl_volume.root.sector=gl_volume.firstfat.sector+(gl_volume.firstfat.num*(unsigned long)(gl_volume.bootrecord.number_of_FATs));
		gl_volume.root.num=((gl_volume.bootrecord.max_root_entry)*sizeof(F_DIRENTRY))/F_SECTOR_SIZE;

		gl_volume._tdata.sector=gl_volume.root.sector+gl_volume.root.num;
		gl_volume._tdata.num=0; /*??*/
	}
	else 
	{
#if F_FAT32
		gl_volume.firstfat.num=gl_volume.bootrecord.sector_per_FAT32;

		gl_volume._tdata.sector=gl_volume.firstfat.sector;
		gl_volume._tdata.sector+=gl_volume.firstfat.num*(unsigned long)(gl_volume.bootrecord.number_of_FATs);
		gl_volume._tdata.num=0; /*??*/

		{
			unsigned long sectorcou=gl_volume.bootrecord.sector_per_cluster;
			gl_volume.root.sector=((gl_volume.bootrecord.rootcluster-2)*sectorcou)+gl_volume._tdata.sector;
			gl_volume.root.num=gl_volume.bootrecord.sector_per_cluster;
		}
#else
		return F_ERR_INVALIDMEDIA; /*unknown media*/
#endif
	}

	{   
		unsigned long maxcluster;
		maxcluster=gl_volume.bootrecord.number_of_sectors_less32;
		if (!maxcluster) maxcluster=gl_volume.bootrecord.number_of_sectors;
		maxcluster-=gl_volume._tdata.sector;
		maxcluster+=gl_volume.bootrecord.number_of_hidden_sectors; /*out of calc!*/
		maxcluster/=gl_volume.bootrecord.sector_per_cluster;
		gl_volume.maxcluster=maxcluster;
	}

	if(gl_volume.maxcluster<(F_CLUSTER_RESERVED&0xfff)) 
	{
#if F_FAT12
		gl_volume.mediatype=F_FAT12_MEDIA;
#else
		return F_ERR_INVALIDMEDIA;
#endif
	}
	else if(gl_volume.maxcluster<(F_CLUSTER_RESERVED&0xffff)) 
	{
#if F_FAT16
		gl_volume.mediatype=F_FAT16_MEDIA;
#else
		return F_ERR_INVALIDMEDIA;
#endif
	}
	else 
	{
#if F_FAT32
		gl_volume.mediatype=F_FAT32_MEDIA;
#else
		return F_ERR_INVALIDMEDIA;
#endif
	}

	return F_NO_ERROR;
}

/****************************************************************************
 *
 * _f_readbootrecord
 *
 * read boot record from a volume, it detects if there is MBR on the media
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/
static unsigned char _f_readbootrecord(void) 
{
	unsigned char ret;
	unsigned char *ptr=gl_files[0]._tdata;
	unsigned char _n=0;
	unsigned long first_sector=0;

	ret=_f_readsector(ptr,0);
   if (ret) return ret;


   if ((ptr[0x1fe]!=0x55) || (ptr[0x1ff]!=0xaa)) return F_ERR_NOTFORMATTED; /*??*/

	if ((ptr[0]!=0xeb) && (ptr[0]!=0xe9)) 
	{
		first_sector=_f_getlong(&ptr [0x08+0x1be] ); /*start sector for 1st partion*/

       ret=_f_readsector(ptr,first_sector); /* MBR */
	   if (ret) return ret;
	   /* Partition Table */
	   if ((ptr[0x1fe]!=0x55) || (ptr[0x1ff]!=0xaa)) return F_ERR_NOTFORMATTED; 

	   if ((ptr[0]!=0xeb) && (ptr[0]!=0xe9)) return F_ERR_NOTFORMATTED;
   }

	/*ptr+=sizeof(gl_volume.bootrecord.jump_code)+sizeof(gl_volume.bootrecord.OEM_name);*/
	ptr=_getchar(gl_volume.bootrecord.jump_code,sizeof(gl_volume.bootrecord.jump_code),ptr);
	ptr=_getchar(gl_volume.bootrecord.OEM_name,sizeof(gl_volume.bootrecord.OEM_name),ptr);

   /* BPB */
	gl_volume.bootrecord.bytes_per_sector=_f_getword(ptr); ptr+=2;
   if (gl_volume.bootrecord.bytes_per_sector!=F_SECTOR_SIZE) return F_ERR_NOTSUPPSECTORSIZE;
	gl_volume.bootrecord.sector_per_cluster=*ptr++;
	gl_volume.bootrecord.reserved_sectors=_f_getword(ptr); ptr+=2;
	gl_volume.bootrecord.number_of_FATs=*ptr++;
	gl_volume.bootrecord.max_root_entry=_f_getword(ptr); ptr+=2;
	gl_volume.bootrecord.number_of_sectors_less32=_f_getword(ptr); ptr+=2;
	gl_volume.bootrecord.media_descriptor=*ptr++;
	gl_volume.bootrecord.sector_per_FAT=_f_getword(ptr); ptr+=2;
	gl_volume.bootrecord.sector_per_Track=_f_getword(ptr); ptr+=2;
	gl_volume.bootrecord.number_of_heads=_f_getword(ptr); ptr+=2;
	gl_volume.bootrecord.number_of_hidden_sectors=_f_getlong(ptr); ptr+=4;
	if (gl_volume.bootrecord.number_of_hidden_sectors<first_sector) 
	{
		gl_volume.bootrecord.number_of_hidden_sectors=first_sector;
	}
	gl_volume.bootrecord.number_of_sectors=_f_getlong(ptr); ptr+=4;


	if (gl_volume.bootrecord.sector_per_FAT==0) 
	{
#if F_FAT32
		gl_volume.bootrecord.sector_per_FAT32=_f_getlong(ptr); ptr+=4;
		gl_volume.bootrecord.extflags=_f_getword(ptr); ptr+=2;
		gl_volume.bootrecord.fsversion=_f_getword(ptr); ptr+=2;
		gl_volume.bootrecord.rootcluster=_f_getlong(ptr); ptr+=4;
		gl_volume.bootrecord.fsinfo=_f_getword(ptr); ptr+=2;
		gl_volume.bootrecord.bkbootsec=_f_getword(ptr); ptr+=2;
		ptr=_getchar(gl_volume.bootrecord.reserved,sizeof(gl_volume.bootrecord.reserved),ptr); 
		_n=28;
#else
		gl_volume.mediatype=F_UNKNOWN_MEDIA;
		return F_ERR_INVALIDMEDIA;
#endif
	}

	gl_volume.bootrecord.logical_drive_num=_f_getword(ptr); ptr+=2;
	gl_volume.bootrecord.extended_signature=*ptr++;
	gl_volume.bootrecord.serial_number=_f_getlong(ptr); ptr+=4;
	ptr=_getchar(gl_volume.bootrecord.volume_name,sizeof(gl_volume.bootrecord.volume_name),ptr);
	ptr=_getchar(gl_volume.bootrecord.FAT_name,sizeof(gl_volume.bootrecord.FAT_name),ptr);
	ptr+=(448-_n);
	ptr=_getchar(gl_volume.bootrecord.executable_marker,sizeof(gl_volume.bootrecord.executable_marker),ptr);

   ret=_f_buildsectors();
   if (ret) return ret;

	if (gl_volume.bootrecord.media_descriptor!=0xf8) 
	{     /*fixdrive*/
	   if (gl_volume.bootrecord.media_descriptor!=0xf0) { /*removable*/
		   return F_ERR_NOTFORMATTED; /*??*/
	   }
   }

   return F_NO_ERROR;
}
#else
/****************************************************************************
 *
 * _f_readbootrecord
 *
 * read boot record from a volume, it detects if there is MBR on the media
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/
static unsigned char _f_readbootrecord(void) {
	unsigned char ret;
	unsigned char *ptr=gl_files[0]._tdata;
	unsigned long maxcluster,_n;
	unsigned long first_sector=0;

	gl_volume.mediatype=F_UNKNOWN_MEDIA;

	ret=_f_readsector(ptr,0);
   
   if (ret) return ret;


   if ((ptr[0x1fe]!=0x55) || (ptr[0x1ff]!=0xaa)) return F_ERR_NOTFORMATTED; /*??*/

	if ((ptr[0]!=0xeb) && (ptr[0]!=0xe9)) 
	{
	   first_sector=_f_getlong(&ptr [0x08+0x1be] ); /*start sector for 1st partioon*/

       ret=_f_readsector(ptr,first_sector);
	   if (ret) return ret;

	   if ((ptr[0x1fe]!=0x55) || (ptr[0x1ff]!=0xaa)) return F_ERR_NOTFORMATTED; /*??*/

	   if ((ptr[0]!=0xeb) && (ptr[0]!=0xe9)) return F_ERR_NOTFORMATTED; /*??*/
   }

   ptr+=11;
   if (_f_getword(ptr)!=F_SECTOR_SIZE) return F_ERR_NOTSUPPSECTORSIZE; 
	ptr+=2;
	gl_volume.bootrecord.sector_per_cluster=*ptr++;
	gl_volume.firstfat.sector=_f_getword(ptr); ptr+=2;
	gl_volume.bootrecord.number_of_FATs=*ptr++;
	gl_volume.root.num=_f_getword(ptr); ptr+=2;
	gl_volume.root.num*=sizeof(F_DIRENTRY);
	gl_volume.root.num/=F_SECTOR_SIZE;
	maxcluster=_f_getword(ptr); ptr+=2;
	gl_volume.bootrecord.media_descriptor=*ptr++;
	gl_volume.firstfat.num=_f_getword(ptr); ptr+=6;
	_n=_f_getlong(ptr); ptr+=4;
	if (_n<first_sector) _n=first_sector;
	gl_volume.firstfat.sector+=_n;
	if (!maxcluster) maxcluster=_f_getlong(ptr); ptr+=4;


	if (gl_volume.firstfat.num)
	{
		gl_volume.root.sector=gl_volume.firstfat.sector+(gl_volume.firstfat.num*gl_volume.bootrecord.number_of_FATs);
		gl_volume._tdata.sector=gl_volume.root.sector+gl_volume.root.num;
		gl_volume._tdata.num=0;
		ptr+=7;
	}
	else
	{
#if F_FAT32
		gl_volume.firstfat.num=_f_getlong(ptr); ptr+=8;
		gl_volume._tdata.sector=gl_volume.firstfat.sector;
		gl_volume._tdata.sector+=gl_volume.firstfat.num*gl_volume.bootrecord.number_of_FATs;
		gl_volume._tdata.num=0;
		gl_volume.bootrecord.rootcluster=_f_getlong(ptr); ptr+=27;
		gl_volume.root.num=gl_volume.bootrecord.sector_per_cluster;
		gl_volume.root.sector=((gl_volume.bootrecord.rootcluster-2)*gl_volume.root.num)+gl_volume._tdata.sector;
#else
		return F_ERR_INVALIDMEDIA;
#endif
	}

#if (F_GETLABEL)
	ptr=_getchar(gl_volume.bootrecord.volume_name,sizeof(gl_volume.bootrecord.volume_name),ptr); /*getlabel+format*/
#endif

	maxcluster-=gl_volume._tdata.sector;
	maxcluster+=_n;
	gl_volume.maxcluster=maxcluster/gl_volume.bootrecord.sector_per_cluster;

	if (gl_volume.maxcluster<(F_CLUSTER_RESERVED&0xfff)) 
	{
#if F_FAT12
		gl_volume.mediatype=F_FAT12_MEDIA;
#else
		return F_ERR_INVALIDMEDIA;
#endif
	}
	else if (gl_volume.maxcluster<(F_CLUSTER_RESERVED&0xffff)) 
	{
#if F_FAT16
		gl_volume.mediatype=F_FAT16_MEDIA;
#else
		return F_ERR_INVALIDMEDIA;
#endif
	}
	else 
	{
#if F_FAT32
		gl_volume.mediatype=F_FAT32_MEDIA;
#else
		return F_ERR_INVALIDMEDIA;
#endif
	}

	if (gl_volume.bootrecord.media_descriptor!=0xf8) 
	{     
		/*fixdrive*/
		if (gl_volume.bootrecord.media_descriptor!=0xf0) 
		{ 
			/*removable*/
			return F_ERR_NOTFORMATTED; /*??*/
		}
	}
	
	return F_NO_ERROR;
}
#endif


/****************************************************************************
 *
 * _f_writebootrecord
 *
 * writing boot record onto a volume, it uses number of hidden sector variable
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/
#if (F_FORMATTING && F_WRITING)
static unsigned char _f_writebootrecord(void) 
{
	unsigned char _n = 0;
	unsigned char *ptr = gl_files[0]._tdata;
	unsigned char *bptr = ptr;

#if F_FAT32
	unsigned char ret;

	if (gl_volume.mediatype==F_FAT32_MEDIA) 
	{ 
		/*write FS_INFO*/
		unsigned short a;

		_memset(bptr,0,F_SECTOR_SIZE);

		for (a=0; a<gl_volume.bootrecord.reserved_sectors; a++) 
		{
			ret=_f_writesector(bptr,gl_volume.bootrecord.number_of_hidden_sectors+a); /*erase reserved area*/
			if (ret) 
			{
				return ret;
			}
		}

		ptr=_setlong(0x41615252,ptr); /*signature*/
		ptr=_setcharzero(480,ptr);        /*reserved*/
		ptr=_setlong(0x61417272,ptr); /*signature  */
		ptr=_setlong(0xffffffff,ptr); /*no last*/
		ptr=_setlong(0xffffffff,ptr); /*no hint*/
		ptr=_setcharzero(12,ptr);         /*reserved*/
		ptr=_setlong(0xaa550000,ptr); /*trail*/

		ret=_f_writesector(bptr,gl_volume.bootrecord.number_of_hidden_sectors+gl_volume.bootrecord.fsinfo); /*write FSINFO*/
		if (ret)
		{
			return ret;
		}

		ret=_f_writesector(bptr,gl_volume.bootrecord.number_of_hidden_sectors+gl_volume.bootrecord.fsinfo+gl_volume.bootrecord.bkbootsec); /*write FSINFO*/
		if (ret)
		{
			return ret;
		}
	}
#endif

	ptr=bptr;

	ptr=_setchar(gl_volume.bootrecord.jump_code,sizeof(gl_volume.bootrecord.jump_code),ptr);
	ptr=_setchar(gl_volume.bootrecord.OEM_name,sizeof(gl_volume.bootrecord.OEM_name),ptr);
	ptr=_setword(F_SECTOR_SIZE,ptr);
	*ptr++=gl_volume.bootrecord.sector_per_cluster;
	ptr=_setword(gl_volume.bootrecord.reserved_sectors,ptr);
	*ptr++=gl_volume.bootrecord.number_of_FATs;
	ptr=_setword(gl_volume.bootrecord.max_root_entry,ptr);
	ptr=_setword(gl_volume.bootrecord.number_of_sectors_less32,ptr);
	*ptr++=gl_volume.bootrecord.media_descriptor;
	ptr=_setword(gl_volume.bootrecord.sector_per_FAT,ptr);
	ptr=_setword(gl_volume.bootrecord.sector_per_Track,ptr);
	ptr=_setword(gl_volume.bootrecord.number_of_heads,ptr);
	ptr=_setlong(gl_volume.bootrecord.number_of_hidden_sectors,ptr);
	ptr=_setlong(gl_volume.bootrecord.number_of_sectors,ptr);

#if F_FAT32
	if (gl_volume.mediatype==F_FAT32_MEDIA) 
	{
		ptr=_setlong(gl_volume.bootrecord.sector_per_FAT32,ptr);
		ptr=_setword(gl_volume.bootrecord.extflags,ptr);   /*?*/
		ptr=_setword(gl_volume.bootrecord.fsversion,ptr);  /*?*/
		ptr=_setlong(gl_volume.bootrecord.rootcluster,ptr); /*?*/
		ptr=_setword(gl_volume.bootrecord.fsinfo,ptr);      /*?*/
		ptr=_setword(gl_volume.bootrecord.bkbootsec,ptr);   /*?*/
		ptr=_setchar(gl_volume.bootrecord.reserved,sizeof(gl_volume.bootrecord.reserved),ptr); /*?*/
		_n=28;
	}
#endif

	{
		ptr=_setword(gl_volume.bootrecord.logical_drive_num,ptr);
		*ptr++=gl_volume.bootrecord.extended_signature;
		ptr=_setlong(gl_volume.bootrecord.serial_number,ptr);
		ptr=_setchar(gl_volume.bootrecord.volume_name,sizeof(gl_volume.bootrecord.volume_name),ptr);
		ptr=_setchar(gl_volume.bootrecord.FAT_name,sizeof(gl_volume.bootrecord.FAT_name),ptr);
		ptr=_setchar(0,448-_n,ptr); 
		ptr=_setchar(gl_volume.bootrecord.executable_marker,sizeof(gl_volume.bootrecord.executable_marker),ptr);
	}

#if F_FAT32
	if (_n)
	{
		ret=_f_writesector(bptr,gl_volume.bootrecord.number_of_hidden_sectors+gl_volume.bootrecord.bkbootsec); /*write backup*/
		if (ret)
		{
			return ret;
		}
	}
#endif

	return _f_writesector(bptr,gl_volume.bootrecord.number_of_hidden_sectors); /*write bootrecord*/
}
#endif

/****************************************************************************
 *
 * _f_getvolume
 *
 * getting back a volume info structure of a given drive, it try to mounts 
 * drive if it was not mounted before
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
unsigned char _f_getvolume(void) 
{
	switch (gl_volume.state) 
	{
		case F_STATE_NONE: 
			return F_ERR_ONDRIVE;
		case F_STATE_WORKING: 
			if (!_f_checkstatus()) return F_NO_ERROR;
			/* here we don't stop case flow,  */
			/* because we have to clean up this volume! */
		case F_STATE_NEEDMOUNT: 
		{
			unsigned char a;
#if F_WRITING
			gl_volume.fatmodified=0;
			gl_volume.lastalloccluster=0; 
#endif
			gl_volume.fatsector=(unsigned long)(-1);
			gl_volume.direntrysector=(unsigned long)(-1);
			for (a=0; a<F_MAXFILES; gl_files[a++].mode = F_FILE_CLOSE);

#if F_DIRECTORIES
			gl_volume.cwd[0]=0;
			gl_volume.cwd[1]=0; /*reset cwd*/
#endif
			gl_volume.mediatype=F_UNKNOWN_MEDIA;

#ifdef drv_getstatus
			if (drv_getstatus() & F_ST_MISSING) 
			{
				gl_volume.state=F_STATE_NEEDMOUNT; /*card missing*/
				return F_ERR_CARDREMOVED;
			}        
#endif

			if (!_f_readbootrecord()) 
			{
				gl_volume.state=F_STATE_WORKING;
				return F_NO_ERROR;
			}

			gl_volume.mediatype=F_UNKNOWN_MEDIA;
			return F_ERR_NOTFORMATTED;
		}
	}

	return F_ERR_ONDRIVE;
}


/****************************************************************************
 *
 * _f_prepareformat
 *
 * preparing boot record for formatting, it sets and calculates values
 *
 * INPUTS
 *
 * phy - media physical descriptor
 * f_bootrecord - which bootrecord need to be prepare
 * number_of_hidden_sectors - where boot record starts
 * fattype - one of this definitions F_FAT12_MEDIA,F_FAT16_MEDIA,F_FAT32_MEDIA
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

#if (F_FORMATTING && F_WRITING)
static unsigned char _f_prepareformat(F_PHY *phy,unsigned long number_of_hidden_sectors,unsigned char fattype) 
{
	unsigned char ret;
	if (!phy->number_of_sectors) return F_ERR_INVALIDSECTOR;

	gl_volume.bootrecord.jump_code[0]=0xeb;
	gl_volume.bootrecord.jump_code[1]=0x3c;
	gl_volume.bootrecord.jump_code[2]=0x90;

	_memcpy(gl_volume.bootrecord.OEM_name,"MSDOS5.0",8);
	gl_volume.bootrecord.bytes_per_sector=F_SECTOR_SIZE;

	gl_volume.bootrecord.number_of_FATs=2;

#if F_FAT32
	if (fattype==F_FAT32_MEDIA) 
	{
		gl_volume.bootrecord.reserved_sectors=32; 
		gl_volume.bootrecord.max_root_entry=0;
	}
	else
#endif
	{
		gl_volume.bootrecord.reserved_sectors=1; 
		gl_volume.bootrecord.max_root_entry=512;
	}

	gl_volume.bootrecord.media_descriptor=0xf0;  /*0xf0 for removable. 0xf8 for fix*/

	gl_volume.bootrecord.sector_per_Track=phy->sector_per_track;
	gl_volume.bootrecord.number_of_heads=phy->number_of_heads;

	gl_volume.bootrecord.number_of_hidden_sectors=number_of_hidden_sectors; /*if this is not partioned*/
	gl_volume.bootrecord.logical_drive_num=0;

	gl_volume.bootrecord.extended_signature=0x29;

	_memcpy(gl_volume.bootrecord.volume_name,"NO NAME    ",11);

	gl_volume.bootrecord.executable_marker[0]=0x55;
	gl_volume.bootrecord.executable_marker[1]=0xaa;

	gl_volume.bootrecord.serial_number=f_getrand(gl_volume.bootrecord.serial_number^phy->number_of_sectors);

	if (phy->number_of_sectors<0x10000 && (fattype==F_FAT16_MEDIA || fattype==F_FAT12_MEDIA))
	{
		gl_volume.bootrecord.number_of_sectors_less32=(unsigned short)(phy->number_of_sectors);
	}
	else 
	{
		gl_volume.bootrecord.number_of_sectors_less32=0;
		gl_volume.bootrecord.number_of_sectors=phy->number_of_sectors;
	}

	if (!phy->number_of_sectors) 
		return F_ERR_INVALIDMEDIA;
	ret = _f_disksz2clustersz(phy->number_of_sectors, fattype, &gl_volume.bootrecord.sector_per_cluster);
	if (ret)
		return ret;

	{
		long secpercl=gl_volume.bootrecord.sector_per_cluster;
		long nfat=gl_volume.bootrecord.number_of_FATs;
		unsigned long roots;
		unsigned long fatsec;

		roots=(gl_volume.bootrecord.max_root_entry*sizeof(F_DIRENTRY))/F_SECTOR_SIZE;

		switch(fattype)
		{
#if F_FAT32
			case F_FAT32_MEDIA:
			{
				unsigned long _n=128*secpercl+nfat;
				fatsec=(phy->number_of_sectors-gl_volume.bootrecord.reserved_sectors+2*secpercl);
				fatsec+=(_n-1);
				fatsec/=_n;

				gl_volume.bootrecord.sector_per_FAT32=fatsec;
				gl_volume.bootrecord.sector_per_FAT=0;

				gl_volume.bootrecord.extflags=0;
				gl_volume.bootrecord.fsversion=0;
				gl_volume.bootrecord.rootcluster=2;
				gl_volume.bootrecord.fsinfo=1;
				gl_volume.bootrecord.bkbootsec=6;

				_memset(gl_volume.bootrecord.reserved,0,sizeof(gl_volume.bootrecord.reserved));
			}
			break;
#endif
#if F_FAT16
			case F_FAT16_MEDIA:
			{
				unsigned long _n=256*secpercl+nfat;
				fatsec=(phy->number_of_sectors-gl_volume.bootrecord.reserved_sectors-roots+2*secpercl);
				fatsec+=(_n-1);
				fatsec/=_n;
				gl_volume.bootrecord.sector_per_FAT=(unsigned short)(fatsec);
			}
			break;
#endif
#if F_FAT12
			case F_FAT12_MEDIA:
			{
				unsigned long _n=1024*secpercl+3*nfat;
				fatsec=(phy->number_of_sectors-gl_volume.bootrecord.reserved_sectors-roots+2*secpercl);
				fatsec*=3;
				fatsec+=(_n-1); 
				fatsec/=_n;
				gl_volume.bootrecord.sector_per_FAT=(unsigned short)(fatsec);
			}
			break;
#endif
			default: return F_ERR_INVALIDMEDIA;
		}
		return F_NO_ERROR;
	}
}

#endif

/****************************************************************************
 *
 * _f_postformat
 *
 * erase fats, erase root directory, reset variables after formatting
 *
 * INPUTS
 *
 * fattype - one of this definitions F_FAT12_MEDIA,F_FAT16_MEDIA,F_FAT32_MEDIA
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/

#if (F_FORMATTING && F_WRITING)
static unsigned char _f_postformat(unsigned char fattype) 
{
	unsigned char ret;
	unsigned char *ptr;
	unsigned char n = 1;
	unsigned char j = 2;
	unsigned long i;

	ret=_f_buildsectors(); /*get positions*/
	if (ret) return ret;

	if (gl_volume.mediatype!=fattype) return F_ERR_MEDIATOOSMALL;

	switch (gl_volume.mediatype)
	{
#if F_FAT12
		case F_FAT12_MEDIA: _memcpy(gl_volume.bootrecord.FAT_name,"FAT12   ",8); break;
#endif
#if F_FAT16
		case F_FAT16_MEDIA: _memcpy(gl_volume.bootrecord.FAT_name,"FAT16   ",8); break;
#endif
#if F_FAT32
		case F_FAT32_MEDIA: _memcpy(gl_volume.bootrecord.FAT_name,"FAT32   ",8); break;
#endif
		default: return F_ERR_INVALIDMEDIA;
	}

	gl_volume.fatmodified=0;
	gl_volume.fatsector=(unsigned long)(-1);
	gl_volume.direntrysector=(unsigned long)(-1);

#if F_MULTIWRITE
	n = 20;
	ptr = f_malloc(F_SECTOR_SIZE*n);
	if (!ptr) {
		ptr = gl_files[0]._tdata;
		n = 1;
	}		
#endif

	_memset(ptr, 0, F_SECTOR_SIZE*n);

	switch (gl_volume.mediatype)
	{
#if F_FAT16
		case F_FAT16_MEDIA: j = 3; break;
#endif
#if F_FAT32
		case F_FAT32_MEDIA: j = 7; break;
#endif
	}

	*ptr = 0xf8;
	_memset(ptr+1, 0xff, j);
	(void)_f_writesector(ptr,gl_volume.firstfat.sector);
	(void)_f_writesector(ptr,gl_volume.firstfat.sector + gl_volume.firstfat.num);
	_memset(ptr, 0, j+1);
#if F_MULTIWRITE
	for (i = 1; i < gl_volume.firstfat.num; ) {
		if (gl_volume.firstfat.num - i >= n) {
			(void)_f_writemultisector(ptr, (gl_volume.firstfat.sector + i), n);
			(void)_f_writemultisector(ptr, (gl_volume.firstfat.sector + i + gl_volume.firstfat.num), n);
			i += n;
		} else {
			(void)_f_writemultisector(ptr, (gl_volume.firstfat.sector + i), (gl_volume.firstfat.num - i));
			(void)_f_writemultisector(ptr, (gl_volume.firstfat.sector + i + gl_volume.firstfat.num), (gl_volume.firstfat.num - i));
			i += (gl_volume.firstfat.num - i);
		}

	}
#else
	for (i = 1; i < gl_volume.firstfat.num; i++) {
		(void)_f_writesector(ptr, (gl_volume.firstfat.sector + i));
		(void)_f_writesector(ptr, (gl_volume.firstfat.sector + i + gl_volume.firstfat.num));
	}
#endif


#if F_FAT32
	if (gl_volume.mediatype == F_FAT32_MEDIA) 
	{
		ret = _f_setclustervalue(gl_volume.bootrecord.rootcluster, F_CLUSTER_LAST);
		if (ret) return ret;

		ret = _f_writefatsector();
		if (ret) return ret;
	}
#endif


	for (i = 0; i < gl_volume.root.num; i++) 
	{ 
		/*reset root direntries*/
		ret = _f_writesector(ptr, (gl_volume.root.sector + i));
  		if (ret) return ret;
	}


#if F_MULTIWRITE
	if (n > 1)
		f_free(ptr);
#endif

	return _f_writebootrecord();
}
#endif

/****************************************************************************
 *
 * fn_hardformat
 *
 * Making a complete format on media, independently from master boot record,
 * according to media physical
 *
 * INPUTS
 *
 * fattype - one of this definitions F_FAT12_MEDIA,F_FAT16_MEDIA,F_FAT32_MEDIA
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/
#if (F_FORMATTING && F_WRITING)
unsigned char fn_hardformat(unsigned char fattype) 
{
	unsigned char ret;
	F_PHY phy;

	ret=_f_getvolume();
	if (ret && ret!=F_ERR_NOTFORMATTED) return ret;

	gl_volume.state=F_STATE_NEEDMOUNT;

	_memset(&phy,0,sizeof(F_PHY));

#ifndef drv_getphy
	return F_ERR_INITFUNC;
#else
	ret=drv_getphy(&phy);
	if (ret) return F_ERR_ONDRIVE;
#endif

	ret=_f_prepareformat(&phy,0,fattype); /*no partition*/
	if (ret) return ret;

	return _f_postformat(fattype);
}
#endif

/****************************************************************************
 *
 * fn_format
 *
 * format a media, 1st it checks existing formatting, then master boot record,
 * then media physical
 *
 * INPUTS
 *
 * fattype - one of this definitions F_FAT12_MEDIA,F_FAT16_MEDIA,F_FAT32_MEDIA
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/
#if (F_FORMATTING && F_WRITING)
unsigned char fn_format(unsigned char fattype) 
{
	unsigned char ret;
	F_PHY phy;

	ret = _f_getvolume();
	if(ret && ret != F_ERR_NOTFORMATTED) return ret;

	gl_volume.state = F_STATE_NEEDMOUNT;

	if(!ret) 
	{ 
		/*we have existing formating*/
		phy.number_of_heads = gl_volume.bootrecord.number_of_heads;
		phy.sector_per_track = gl_volume.bootrecord.sector_per_Track;

		phy.number_of_sectors = gl_volume.bootrecord.number_of_sectors_less32;
		if(!phy.number_of_sectors) phy.number_of_sectors = gl_volume.bootrecord.number_of_sectors;


		ret = _f_prepareformat(&phy, gl_volume.bootrecord.number_of_hidden_sectors, fattype); 

		if(ret) return ret;
	}
	else if(ret == F_ERR_NOTFORMATTED) 
	{ 
		/*we dont have existing formatting*/
		unsigned long startsec=0;
		unsigned char *ptr=gl_files[0]._tdata;

		_memset(&gl_volume.bootrecord,0,sizeof(F_BOOTRECORD));
		_memset(&phy,0,sizeof(F_PHY));

#ifndef drv_getphy
		return F_ERR_INITFUNC;
#else
		ret=drv_getphy(&phy);
		if (ret)
		{
			return F_ERR_ONDRIVE;
		}
#endif

		ret=_f_readsector(ptr,0); /*check MBR*/
		if (ret)
		{
			return ret;
		}

		if ((ptr[0x1fe]==0x55) && (ptr[0x1ff]==0xaa)) 
		{ 
			/*check valid*/
			if ((ptr[0]!=0xeb) && (ptr[0]!=0xe9)) 
			{   
				/*check MBR*/
				if (ptr[0x1be]==0x80) 
				{				  
					/*check active*/
					startsec=_f_getlong(&ptr[0x08+0x1be]); /*start sector for 1st partioon*/
					phy.number_of_sectors=_f_getlong(&ptr[0x0c+0x1be]); /*number of sectors*/
				}
			}
		}

		ret=_f_prepareformat(&phy,startsec,fattype); /*no partition*/
		if (ret) return ret;
	}

	return _f_postformat(fattype);
}
#endif


/****************************************************************************
 *
 * fn_getfreespace
 *
 * get total/free/used/bad diskspace
 *
 * INPUTS
 * pspace - pointer where to store the information
 *
 * RETURNS
 * error code
 *
 * REMARKS
 * the unit of space is F_SECTOR_SIZE-byte, i.e. the value 100 means (100 * F_SECTOR_SIZE) bytes
 *
 ***************************************************************************/
#if F_GETFREESPACE
unsigned char fn_getfreespace(F_SPACE *pspace) 
{ 
	unsigned char ret;
	unsigned long a;


	ret = _f_getvolume();
	if (ret) 
		return ret;

	_memset(pspace, 0, sizeof(F_SPACE));

#if (F_MULTIREAD && F_FAT32)
	if (gl_volume.mediatype == F_FAT32_MEDIA) {
		unsigned char *ptr;
		unsigned long n, fatsector, i, j;
		unsigned long value;
		
#if F_WRITING
		if (gl_volume.fatmodified) {
			ret = _f_writefatsector();
			if (ret) 
				return ret;
		}
#endif	

		n = 20;
		ptr = f_malloc(F_SECTOR_SIZE*n);
		if (!ptr) {
			ptr = gl_volume.fat;
			n = 1;
		}
		
		ret = F_NO_ERROR;
		for (i = 0; i < gl_volume.firstfat.num; i += n) {
			fatsector = gl_volume.firstfat.sector + i;
			for (j = 0; j < gl_volume.bootrecord.number_of_FATs; j++) {
				if (gl_volume.firstfat.num < (i + n))
					ret = _f_readmultisector(ptr, fatsector, (gl_volume.firstfat.num - i));
				else
					ret = _f_readmultisector(ptr, fatsector, n);	

				if (!ret) {
					a = (i == 0)? 2 : 0;
					while (a < (F_SECTOR_SIZE * n / 4)) {
						value = _f_getlong(&ptr[a<<2]) & 0x0fffffff; /*28bit*/
						if (!value) 
							++(pspace->free);
						else if (value == F_CLUSTER_BAD) 
							++(pspace->bad);
						else 
							++(pspace->used);
					
						if (++(pspace->total) >= gl_volume.maxcluster)
							break;

						a++;
					}
					break;
				}
				fatsector += gl_volume.firstfat.num;
			}				
		}
		if (n > 1)
			f_free(ptr);

		if (ret)
			return ret;
	} else
#endif
	{
		pspace->total = gl_volume.maxcluster;
		for (a = 2; a < gl_volume.maxcluster + 2; a++) {
			unsigned long value;

			ret = _f_getclustervalue(a, &value);
			if (ret) 
				return ret;

			if (!value) 
				++(pspace->free);
			else if (value == F_CLUSTER_BAD) 
				++(pspace->bad);
			else 
				++(pspace->used);
		}
	}

	a = gl_volume.bootrecord.sector_per_cluster;
	
	pspace->total *= a;
	pspace->free *= a;
	pspace->used *= a;
	pspace->bad *= a;

	return F_NO_ERROR;
}
#endif

/****************************************************************************
 *
 * fn_getlabel
 *
 * get a label of a media
 *
 * INPUTS
 * label - char pointer where to store label
 * len - length of label buffer
 *
 * RETURNS
 * error code or zero if successful
 *
 ***************************************************************************/
#if F_GETLABEL
#if F_LONGFILENAME
unsigned char fn_getlabel(char *label, unsigned char len) 
{
	char tmplabel[11];
	long a;
	unsigned char ret;
	F_FILE *file;

	if (!len) 
		return F_NO_ERROR; /*how to store volume name?*/

	ret = _f_getvolume();
	if (ret) 
		return ret;

	_memcpy(tmplabel, gl_volume.bootrecord.volume_name, 11);

	file = _f_opendir(0); /*open root;*/
	if (!file) 
		return F_ERR_READ;

	while (1) 
	{
		F_DIRENTRY de;

		ret=_f_readdir(&de, file);
		if (ret==F_ERR_EOF) 
			break;
		if (ret) 
			break;

		if (!de.name[0]) 
			continue; /*empty*/
		if ((unsigned char)(de.name[0]) == 0xe5) 
			continue; /*deleted*/

		if (de.attr == F_ATTR_VOLUME) 
		{
			_memcpy(tmplabel, de.name, 8);
			_memcpy(tmplabel+8, de.ext, 3);
			break;
		}

	}

	ret = _f_closedir(file, F_NO_ERROR);
	if (ret) 
		return ret;

	len--;
	for (a = 0; a < 11 && a < len; a++) 
	{
		label[a]=tmplabel[a];
	}
	label[a] = 0; /*terminate it, this is ok because of len--!*/

	for (; a >= 0; a--) 
	{ 
		/*cut the end spaces*/
		if (label[a] <= 32) 
			label[a] = 0;
		else break;
	}

	return F_NO_ERROR;
}
#else
unsigned char fn_getlabel(char *label, unsigned char len) 
{
	char tmplabel[11];
	unsigned char a;
	F_POS pos;
	unsigned char ret;
	if (!len) return F_NO_ERROR; /*how to store volume name?*/

	ret=_f_getvolume();
	if (ret) return ret;

	_f_clustertopos(0,&pos);
	_memcpy(tmplabel,gl_volume.bootrecord.volume_name,11);

	while (pos.cluster<F_CLUSTER_RESERVED) 
	{
		for (;pos.sector<pos.sectorend;pos.sector++) 
		{
			F_DIRENTRY *de=(F_DIRENTRY*)(gl_volume.direntry);

			ret=_f_getdirsector(pos.sector);
			if (ret) return ret;

			for (pos.pos=0; pos.pos<F_SECTOR_SIZE/sizeof(F_DIRENTRY);de++,pos.pos++) 
			{
				if (!de->name[0]) continue;						 /*empty*/
				if ((unsigned char)(de->name[0])==0xe5) continue; /*deleted*/
				if (de->attr == F_ATTR_VOLUME) 
				{
					_memcpy(tmplabel,de->name,8);
					_memcpy(tmplabel+8,de->ext,3);
					goto vege;
				}
			}
		}

		if (!pos.cluster) 
		{
#if F_FAT32
			if (gl_volume.mediatype==F_FAT32_MEDIA) 
			{
				pos.cluster=gl_volume.bootrecord.rootcluster;
			}
			else 
#endif
			goto vege;
		}

		{
			unsigned long nextcluster;
			if (_f_getclustervalue(pos.cluster,&nextcluster)) goto vege;

			if (nextcluster>=F_CLUSTER_RESERVED) goto vege;

			_f_clustertopos(nextcluster,&pos);
		}
	}

vege:
	len--;
	for (a=0; a<11 && a<len; a++) 
	{
		label[a]=tmplabel[a];
	}
	label[a]=0; /*terminate it, this is ok because of len--!*/

	for (;a>0; a--) 
	{ 
		/*cut the spaces*/
		if (label[a-1]<=32) label[a-1]=0;
		else break;
	}
	
	return F_NO_ERROR;
}
#endif
#endif


/****************************************************************************
 *
 * fn_setlabel
 *
 * set a label of a media
 *
 * INPUTS
 * label - new label for the media
 *
 * RETURNS
 * error code or zero if successful
 *
 ***************************************************************************/
#if (F_SETLABEL && F_WRITING)
#if F_LONGFILENAME
unsigned char fn_setlabel(const char *label) 
{
	F_NAME *fsname = NULL;
	F_LFNINT *plfn = NULL;

	unsigned char ret;
	char tmplabel[11];
	long a,b;
	F_FILE *file;
	F_POS pos;
	F_DIRENTRY *pde;

	ret = _f_getvolume();
	if (ret) 
		return ret;

	for (a = b = 0; a < 11; a++) 
	{
		char ch = _f_toupper(label[b]);
		if (ch >= ' ') 
			b++;
		else 
			ch = ' ';
		tmplabel[a] = ch;
	}

	file = _f_opendir(0); /*open root;*/
	if (!file) 
		return F_ERR_READ;

	while (1) 
	{
		F_DIRENTRY de;

		ret = _f_readdir(&de,file);
		if (ret == F_ERR_EOF) 
			break;
		if (ret) 
			return _f_closedir(file,ret);

		if (!de.name[0]) 
			continue; /*empty*/
		if ((unsigned char)(de.name[0]) == 0xe5) 
			continue; /*deleted*/

		if (de.attr == F_ATTR_VOLUME) 
		{
			_memcpy(de.name, tmplabel, 8);
			_memcpy(de.ext, tmplabel + 8, 3);

			ret = _f_seekdir(file, -1, F_SEEK_CUR);
			if (ret) 
				return _f_closedir(file, ret);

			ret = _f_writedir(&de, file);
			if (ret) 
				return _f_closedir(file, ret);

			return _f_closedir(file, F_NO_ERROR);
		}
	}

	ret = _f_closedir(file, F_NO_ERROR);
	if (ret) 
		return ret;

	_f_clustertopos(0, &pos);

	fsname = f_malloc(sizeof(F_NAME));
	plfn = f_malloc(sizeof(F_LFNINT));
	if(!fsname || !plfn)
	{
		if(fsname) f_free(fsname);
		if(plfn) f_free(plfn);
		return F_ERR_NULLPOINTER;
	}

	fsname->path[0] = 0;
	fsname->path[1] = 0;
	fsname->lname[0] = 'X';  /*for dummy*/
	fsname->lname[1] = 0;
	fsname->lname[2] = 0;
	fsname->lname[3] = 0;

	if (_f_addentry(fsname, &pos, &pde, plfn)) 
	{
		if(fsname) f_free(fsname);
		if(plfn) f_free(plfn);
		return F_ERR_NOMOREENTRY;  /*couldnt be added*/
	}

	if(fsname) f_free(fsname);
	if(plfn) f_free(plfn);
	
	_f_initentry(pde, tmplabel, tmplabel + 8);
	pde->attr = F_ATTR_VOLUME;

	return _f_writedirsector();
}
#else	/* F_LONGFILENAME */
unsigned char fn_setlabel(const char *label) 
{
	F_NAME fsname;
	unsigned char ret;
	char tmplabel[11];
	unsigned char a,b;
	F_POS pos;
	F_DIRENTRY *de;

	ret=_f_getvolume();
	if (ret) return ret;

	_f_clustertopos(0,&pos);

	for (a=b=0; a<11; a++) 
	{
		char ch=_f_toupper(label[b]);

		if (ch>=32) b++;
		else ch=32;

		tmplabel[a]=ch;
	}

	while (pos.cluster<F_CLUSTER_RESERVED) 
	{
		for (;pos.sector<pos.sectorend;pos.sector++) 
		{
			de=(F_DIRENTRY*)(gl_volume.direntry);

			ret=_f_getdirsector(pos.sector);
			if (ret) return ret;

			for (pos.pos=0; pos.pos<F_SECTOR_SIZE/sizeof(F_DIRENTRY);de++,pos.pos++) 
			{

				if (!de->name[0]) continue;						 /*empty*/
				if ((unsigned char)(de->name[0])==0xe5) continue; /*deleted*/
				if (de->attr == F_ATTR_VOLUME) 
				{
					_memcpy(de->name,tmplabel,8);
					_memcpy(de->ext,tmplabel+8,3);
					return _f_writedirsector();
				}
			}
		}

		if (!pos.cluster) 
		{
#if F_FAT32
			if (gl_volume.mediatype==F_FAT32_MEDIA) 
			{
				pos.cluster=gl_volume.bootrecord.rootcluster;
			}
			else 
#endif
				break;
		}

		{
			unsigned long nextcluster;
			if (_f_getclustervalue(pos.cluster,&nextcluster)) break;

			if (nextcluster>=F_CLUSTER_RESERVED) break;

			_f_clustertopos(nextcluster,&pos);
		}
	}

	_f_clustertopos(0,&pos);

	_memcpy(fsname.filename,tmplabel,F_MAXNAME);
	_memcpy(fsname.fileext,tmplabel+F_MAXNAME,F_MAXEXT);

	if (_f_addentry(&fsname,&pos,&de)) return F_ERR_NOMOREENTRY;  /*couldnt be added*/

	de->attr=F_ATTR_VOLUME;
	return _f_writedirsector();
}
#endif
#endif


/****************************************************************************
 *
 * fn_initvolume
 *
 * initiate a volume, this function has to be called 1st to set physical
 * driver function to a given volume
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/
unsigned char fn_initvolume(void) 
{

	gl_volume.state=F_STATE_NONE;

#ifdef drv_initfunc
   	if (drv_initfunc()) return F_ERR_INITFUNC;
#endif

	gl_volume.state=F_STATE_NEEDMOUNT;

	return _f_getvolume();
}

/****************************************************************************
 *
 * fn_getclustersize
 *
 * get the size of cluster in byte.
 *
 *
 * RETURNS
 *
 * zero for error, otherwise the size of cluster
 *
 ***************************************************************************/
unsigned long fn_getclustersize(void)
{
	if(_f_getvolume()) return 0; /*cant read any*/
	return (gl_volume.bootrecord.bytes_per_sector * gl_volume.bootrecord.sector_per_cluster);
}



