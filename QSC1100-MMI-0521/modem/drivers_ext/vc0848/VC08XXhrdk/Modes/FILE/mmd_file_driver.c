/*******************************************************************************
*  All rights reserved, Copyright (C) vimicro LIMITED 2008													 
* ------------------------------------------------------------------------------
* [File Name]		 	 : mmd_file_driver.c
* [Description]		 : File API defined for file access operation
* [Author]			 : chenjingen
* [Date Of Creation]	 : 2008-04-15
* [Platform]			 : Platform independent
* [Note]				 : None
*
* Modification History : 
* ------------------------------------------------------------------------------
* Date				 Author 			 Modifications
* ------------------------------------------------------------------------------
* 2008-04-15		 	chenjingen 		 Modified for VC0848 SDK
*******************************************************************************/
/*******************************************************************************
* This source code has been made available to you by VIMICRO on an
* AS-IS basis. Anyone receiving this source code is licensed under VIMICRO
* copyrights to use it in any way he or she deems fit, including copying it,
* modifying it, compiling it, and redistributing it either with or without
* modifications. Any person who transfers this source code or any derivative
* work must include the VIMICRO copyright notice and this paragraph in
* the transferred software.
*******************************************************************************/


/*
 * INCLUDE FILE                                              
 */
#include "mmd_file_driver.h"
#include "..\..\Drivers\fat\fat.h"
#include "..\..\Drivers\fat\drv.h"
#include "mmd_sd_driver.h"

#include "AEEStdlib.h"

/*
 * INTERNAL MACRO DEFINITION
 */
//#define MMD_FILE_DEBUG_INFO
#define MMD_FILE_DEBUG_ERR
#define MMD_FILE_DEBUG_NOTE
#define MMD_FILE_DEBUG_WARNING

#ifdef MMD_FILE_DEBUG_INFO
    #define PRINTF(WriteVal)    MMD_PRINTF(WriteVal) //general print out
#else
    #define PRINTF(WriteVal)
#endif

#ifdef MMD_FILE_DEBUG_ERR
    #define PRINTFE(WriteVal)   MMD_PRINTFE(WriteVal) //error
#else
    #define PRINTFE(WriteVal)
#endif

#ifdef MMD_FILE_DEBUG_NOTE
    #define PRINTFN(WriteVal)   MMD_PRINTFN(WriteVal) //note
#else
    #define PRINTFN(WriteVal)
#endif

#ifdef MMD_FILE_DEBUG_WARNING
    #define PRINTFW(WriteVal)   MMD_PRINTFW(WriteVal) //warning
#else
    #define PRINTFW(WriteVal)
#endif

/*
 * INTERNAL ENUM DEFINITION
 */

/*
 * INTERNAL TYPE DEFINITION                                             
 */
typedef struct {
	F_FILE *fsrc; 			/* source file handle */
	F_FILE *fdst;			/* destionation file handle */
	MMD_U08 wr_flag;		/* 0 - idle; 1 - read; 2 - write */
	MMD_U08 work_mode; 		/* 0 - idle; 1 - running; 2 - paused */
	MMD_U32 nbytes;			/* size of current read or write operation */
	F_COREMEM core_mem;		/* VC0848 Core memory information */
	F_CALLBACK callback; 	/* callback fuction when fast copy completed */
}F_FASTCOPY;
/*
 * GLOBAL VARIABLES DEFINITION                                          
 */
F_FASTCOPY g_FILE_FastCopy;
extern volatile MMD_U32 g_Ack_MinCmd;
extern volatile MMD_U32 g_Ack_OP1;
extern volatile MMD_U32 g_Ack_OP2;
extern SD_DEV g_SdDev;

//unsigned int g_FILE_DbgCnt = 0;

/*
 * FUNCTION (Local routine, all should be defined as static, only for internal use ) 
 */
static MMD_U08 MMD_FILE_GetSectorChain(F_FILE *psFile, MMD_U32 *pdChain, MMD_U32 dTotal);
static MMD_U08 MMD_FILE_AllocSectorChain(F_FILE *psFile, MMD_U32 *pdChain, MMD_U32 dTotal);
static void MMD_FILE_FastCopyEnd(MMD_U32 dRetCode);
static void MMD_FILE_FastCopyTisr(void);

static MMD_U08 MMD_FILE_GetSectorChain(F_FILE *psFile, MMD_U32 *pdChain, MMD_U32 dTotal)
{
	unsigned char ret;
	unsigned long cluster, prevcluster;
	unsigned int num = 0;
	F_POS pos;

	/* backup current pos */ 
	pos.cluster = psFile->pos.cluster;
	pos.sector = psFile->pos.sector;
	pos.sectorend = psFile->pos.sectorend;
	pos.pos = psFile->pos.pos;
	prevcluster = psFile->prevcluster;

	while(num<dTotal)
	{
		if (psFile->pos.sector==psFile->pos.sectorend)
		{
			/* end of cluster */
			ret=_f_getclustervalue(psFile->pos.cluster,&cluster);
			if (ret) break;
			if (cluster>=(unsigned long )F_CLUSTER_RESERVED) break;
			psFile->prevcluster=psFile->pos.cluster;
			_f_clustertopos(cluster,&psFile->pos);							
		}

		*pdChain++ = psFile->pos.sector;
		num++;
		psFile->pos.sector++;
	}

	if(num != dTotal)
	{
		/* resume the pos */
		psFile->pos.cluster = pos.cluster;
		psFile->pos.sector = pos.sector;
		psFile->pos.sectorend = pos.sectorend;
		psFile->pos.pos = pos.pos;
		psFile->prevcluster = prevcluster;
		return MMD_FILE_ERR_GETSECTORCHAIN;
	}
	
	psFile->pos.sector--; /* move back to current sector */

	return MMD_FILE_NO_ERROR;
}

static MMD_U08 MMD_FILE_AllocSectorChain(F_FILE *psFile, MMD_U32 *pdChain, MMD_U32 dTotal)
{
	unsigned char ret = MMD_FILE_NO_ERROR;
	unsigned long cluster, prevcluster;
	unsigned int num = 0;
	F_POS pos;

	/* backup current pos */ 
	pos.cluster = psFile->pos.cluster;
	pos.sector = psFile->pos.sector;
	pos.sectorend = psFile->pos.sectorend;
	pos.pos = psFile->pos.pos;
	prevcluster = psFile->prevcluster;

	while (num < dTotal) {
		if (psFile->pos.sector == psFile->pos.sectorend) {
			if (!psFile->startcluster) {
				ret = _f_alloccluster(&psFile->startcluster);
				if(ret) 
					break;
				
				_f_clustertopos(psFile->startcluster, &psFile->pos);

				/* update backup */
				pos.cluster = psFile->pos.cluster;
				pos.sector = psFile->pos.sector;
				pos.sectorend = psFile->pos.sectorend;
				pos.pos = psFile->pos.pos;
				prevcluster = psFile->prevcluster;

				ret = _f_setclustervalue(psFile->startcluster, F_CLUSTER_LAST);
				if(ret) 
					break;
			} else {
				/* end of cluster */
				ret = _f_getclustervalue(psFile->pos.cluster, &cluster);
				if (ret) 
					break;
				if (cluster >= (unsigned long )F_CLUSTER_RESERVED) {
					ret = _f_alloccluster(&cluster);
					if (ret)
						break;
					ret = _f_setclustervalue(cluster, F_CLUSTER_LAST);
					if (ret)
						break;
					ret = _f_setclustervalue(psFile->pos.cluster, cluster);
					if (ret)
						break;
				}
				psFile->prevcluster = psFile->pos.cluster;
				_f_clustertopos(cluster, &psFile->pos);	
			}
		}

		*pdChain++ = psFile->pos.sector;
		num++;
		psFile->pos.sector++;
	}

	if (num != dTotal || ret) {
		/* resume the pos */
		psFile->pos.cluster = pos.cluster;
		psFile->pos.sector = pos.sector;
		psFile->pos.sectorend = pos.sectorend;
		psFile->pos.pos = pos.pos;
		psFile->prevcluster = prevcluster;
		if (ret)
			return ret;
		else
			return MMD_FILE_ERR_ALLOCSECTORCHAIN;
	}
	
	psFile->pos.sector--; /* move back to current sector */

	/* empty the current buffer first */
	if (psFile->modified) {
		if (pos.sector >= pos.sectorend)
				PRINTFE(("\r\n ####ERR: pos.sector = %d, pos.sectorend = %d \r\n", pos.sector, pos.sectorend));
		ret = _f_writesector(psFile->_tdata, pos.sector);
		if (ret) 
			return MMD_FILE_ERR_ALLOCSECTORCHAIN;
		psFile->modified = 0;
	}

	return MMD_FILE_NO_ERROR;
}

static void MMD_FILE_FastCopyEnd(MMD_U32 dRetCode)
{
	F_FASTCOPY *p_fastcopy = &g_FILE_FastCopy;
	F_CALLBACK callback;

	MMD_USER_Timer_Stop();
	MMD_USER_Timer_Release();			
	f_close(p_fastcopy->fdst);
	f_close(p_fastcopy->fsrc);
	MMD_SYS_Free(p_fastcopy->core_mem.addr);
	callback = p_fastcopy->callback;
	MMD_MemSet(p_fastcopy, 0, sizeof(F_FASTCOPY));
	if (callback)
		callback(dRetCode);
	return;
}

static void MMD_FILE_FastCopyTisr(void)
{
	MMD_U32 status, ret;
	MMD_U32 nbytes;
	F_FASTCOPY *p_fastcopy = &g_FILE_FastCopy;	
	
	MMD_BIU_Get_Device_Status(IDX_FILE_STATUS, &status);
	if (status & 0x1) {
		PRINTFN(("\r\nSD Card busy..."));
		return;	/* SD sector rw processing in VC0848 Core */
	}	
	if (status >> 16) {
		/* error occur during SD sector rw in VC0848 Core */
		PRINTFE(("\r\nVC0848 Core SD WR error in FastCopyTisr, status = 0x%08X", status));
		MMD_FILE_FastCopyEnd(MMD_FILE_ERR_CORESDRW);
		return;
	}

	switch (p_fastcopy->wr_flag) {
	case 1:
		/* last operation is reading */
		ret = MMD_FILE_WriteFrCore(p_fastcopy->fdst, p_fastcopy->core_mem, p_fastcopy->nbytes, &nbytes);
		if (!ret) 
			ret = MMD_FILE_StartWriteFrCore();
		if (ret) {
			PRINTFE(("\r\nWrite in FastCopyTisr, ret = %d", ret));
			MMD_FILE_FastCopyEnd(ret);
			return;
		}
		p_fastcopy->wr_flag = 2;
		p_fastcopy->nbytes = nbytes;
		break;
	case 2:
		/* last operation is writing */
		if (p_fastcopy->fdst->filesize == p_fastcopy->fsrc->filesize) {
			/* end of copy */
			MMD_FILE_FastCopyEnd(MMD_FILE_NO_ERROR);
			return;
		}
		ret = MMD_FILE_Read2Core(p_fastcopy->fsrc, p_fastcopy->core_mem, p_fastcopy->nbytes, &nbytes);
		if (!ret) 
			ret = MMD_FILE_StartRead2Core();
		if (ret) {
			PRINTFE(("\r\nRead in FastCopyTisr, ret = %d", ret));
			MMD_FILE_FastCopyEnd(ret);
			return;
		}
		p_fastcopy->wr_flag = 1;
		p_fastcopy->nbytes = nbytes;
		break;
	default:
		break;
	}
}

/*
 * FUNCTION (API)
 */

/*************************************************
  Function: 	  
	MMD_Mode_File_Lisr
  Description:	  
	file mode command process call back function, called in LISR
  Input:
	pMsg:	Host to device cmd msg
  Output:		 
	NULL	
  Return:		  
	Error code 
  History:		  
	  <author>			<time>			<desc>
	  Chen Jing'en			07/04/24		create this function  
*************************************************/
MMD_U32 MMD_Mode_File_Lisr(MMD_U32 cmd)
{
	MMD_MSG msg;
	MMD_U08 min;

	// get command and parameter 
	MMD_BIU_Get_D2H_Ack(&msg);
	MMD_ASSERT(msg.CMD==cmd);
	
	min = msg.CMD & CMD_MASK_MIN;
	
	g_Ack_MinCmd = min;
	g_Ack_OP1 = msg.OP1;
	g_Ack_OP2 = msg.OP2;

	switch(min)
	{
		default:
			break;
	}

	return MMD_RET_NOERR;
}

/*************************************************
  Function: 	  
	MMD_File_Sys_Hisr
  Description:	  
	file mode command process call back function, called in HISR
  Input:
	pMsg:	Host to device cmd msg
  Output:		 
	NULL	
  Return:		  
	Error code 
  History:		  
	  <author>			<time>			<desc>
	  Chen Jing'en			07/04/24		create this function  
*************************************************/
MMD_U32 MMD_Mode_File_Hisr(MMD_U32 cmd)
{
	MMD_MSG msg;
	MMD_U08 min;

	// get command and parameter 
	MMD_BIU_Get_D2H_Ack(&msg);
	MMD_ASSERT(msg.CMD==cmd);
	
	min = msg.CMD & CMD_MASK_MIN;

	switch(min)
	{	
		default:
			break;
	}
	
	return MMD_RET_NOERR;
}

/*-----------------------------------------------------------------------------
[FUNCTION NAME]
 	MMD_FILE_Mount
[DESCRIPTION]
 	This function is used to initialize the volume. 
[PARAMETERS]
	None.
[RETURN VALUE]
 	MMD_FILE_NO_ERROR 	: 	Success
 	Others 				: 	Error Code
-----------------------------------------------------------------------------*/
MMD_U32 MMD_FILE_Mount(void)
{
	MMD_U08 ret;
    FARF(MSG,("MMD_FILE_Mount"));
	ret = f_initvolume();
	if (ret)
		return ret;

	MMD_MemSet(&g_FILE_FastCopy, 0, sizeof(g_FILE_FastCopy));

	return MMD_FILE_InitCore();
}

/*-----------------------------------------------------------------------------
[FUNCTION NAME]
	MMD_FILE_Format
[DESCRIPTION]
	Formats the volume with specified FAT type.
[PARAMETERS]
	bFATType(IN)	:	type of FAT format. This can be one of 
					the value listed below: 	 
				       F_FAT12_MEDIA(= 1), support disk size < 4MB
				       F_FAT16_MEDIA(= 2), support disk size 4MB ~ 2GB
				       F_FAT32_MEDIA(= 3), support disk size > 32MB
	bHardFlag(IN)	:	0 - normal format
					others - hard format
[RETURN VALUE]
 	MMD_FILE_NO_ERROR 	: 	Success
 	Others 				: 	Error Code
 [REMARKS]
	For normal format, any existing Master Boot Record will be unaffected and the
	Boot Sector information will be re-created.
	For hard format, any existing Master Boot Record or Boot Sector information 
	will be destroyed. The new volume will be formatted without a Master Boot
	Record and start with re-created Boot Sector Information.	
-----------------------------------------------------------------------------*/
MMD_U32 MMD_FILE_Format(MMD_U08 bFATType, MMD_U08 bHardFlag)
{
	if(bHardFlag)
		return f_hardformat(bFATType);
	else
		return f_format(bFATType);
}

/*-----------------------------------------------------------------------------
[FUNCTION NAME]
	MMD_FILE_GetLabel
[DESCRIPTION]
	Retrives the current label of volume. 
[PARAMETERS]
	pbLabel(OUT)	:	where to store the name of label
[RETURN VALUE]
 	MMD_FILE_NO_ERROR 	: 	Success
 	Others 				: 	Error Code
[REMARKS]
	pbLabel should be capable of holding the 12 bytes, and the name of label 
	is a 11-character string with NULL terminator. 	
-----------------------------------------------------------------------------*/
MMD_U32 MMD_FILE_GetLabel(MMD_U08 *pbLabel)
{
	return f_getlabel((char *)pbLabel, 12);
}

/*-----------------------------------------------------------------------------
[FUNCTION NAME]
	MMD_FILE_SetLabel
[DESCRIPTION]
	Sets the volume label.
[PARAMETERS]
	pbLabel(IN) 	: 	where to store the desired name of label.
[RETURN VALUE]
	MMD_FILE_NO_ERROR	:	Success
	Others			  	:	Error Code
[REMARKS]
	Different from the other file APIs, the parameter pbLabel of this function should be 
	a string with NULL terminator instead of the Unicode byte stream.
	The max length of the string is 12 bytes that including the NULL terminator.	
-----------------------------------------------------------------------------*/
MMD_U32 MMD_FILE_SetLabel(MMD_U08 *pbLabel)
{
    return f_setlabel((char *)pbLabel);
}

/*-----------------------------------------------------------------------------
[FUNCTION NAME]
	MMD_FILE_GetSpace
[DESCRIPTION]
	Retrieves the information about the size of total space, free space, used space and bad (damaged) space.
[PARAMETERS]
	psSpace(OUT) 	: 	where to store space information of volume
[RETURN VALUE]
 	MMD_FILE_NO_ERROR  	:   Success
  	Others			  	:   Error Code
[REMARKS]
	To avoid overflow of the space that is equal or larger than 4 Gbytes, the unit of each member of F_SPACE is 
	in sector, i.e. the value 100 means (100*F_SECTOR_SIZE) bytes.	
-----------------------------------------------------------------------------*/
MMD_U32 MMD_FILE_GetSpace(F_SPACE *psSpace)
{
    return f_getfreespace(psSpace);
}

/*-----------------------------------------------------------------------------
[FUNCTION NAME]
	MMD_FILE_GetClusterSize
[DESCRIPTION]
	Retrieves the cluster size of current volume.
[PARAMETERS]
	pdSize(OUT) 	: 	where to store the cluster size
[RETURN VALUE]
 	MMD_FILE_NO_ERROR  	:   Success
  	Others			  	:   Error Code		
-----------------------------------------------------------------------------*/
MMD_U32 MMD_FILE_GetClusterSize(MMD_U32 *pdSize)
{
	MMD_U32 cluster_size;
	cluster_size = f_getclustersize();
	if (!cluster_size)
		return MMD_FILE_ERR_ONDRIVE;

	*pdSize = cluster_size;
	return MMD_FILE_NO_ERROR;
}

/*-----------------------------------------------------------------------------
[FUNCTION NAME]
 	MMD_FILE_FindFirst
[DESCRIPTION]
	Find first file or subdirectory in specified directory. First call MMD_FILE_FindFirst
	function and if file was found get the next file with MMD_FILE_FindNext function.
	Files with the system attribute set will be ignored.
[PARAMETERS]
	pbPath(IN) 	: 	the path of file name (wildcard supported) to find, the little-endian Unicode stream 
 					with double null terminator is required. i.e. {'*', 0 , '.', 0, 'm', 0, 'p', 0, '3', 0, 0, 0}
	psFind(OUT) 	: 	where to store find information
[RETURN VALUE]
 	MMD_FILE_NO_ERROR  	:   Success
  	Others			  	:   Error Code	
[REMARKS]
	Wildcard '*' and '?' are supported and complied with following rules: 
	1. {'*', 0, 0, 0} or {'*', 0, '.', 0, '*', 0, 0, 0} means all files. 
	2. Character matching is case insensitive.
	3. The metacharacter '*' matches any sequence of zero or more characters.
	4. The metacharacter '?' matches exactly one character unless that character is a period ('.').
-----------------------------------------------------------------------------*/
MMD_U32 MMD_FILE_FindFirst(MMD_U08 *pbPath, F_FIND *psFind)
{
    FARF(MSG,("MMD_FILE_FindFirst"));
    return f_findfirst((char *)pbPath, psFind);
}

/*-----------------------------------------------------------------------------
[FUNCTION NAME]
	MMD_FILE_FindNext
[DESCRIPTION]
 	Find the next file or subdirectory in a specified directory after a previous call to 
 	MMD_FILE_FindFirst or MMD_FILE_FindNext. First call MMD_FILE_FindFirst function and if file 
 	was found get the rest of the matching files by repeated calls to the MMD_FILE_FindNext 
 	function. Files with the system attribute set will be ignored.
[PARAMETERS]
	psFind(OUT) 	: 	where to store find information
[RETURN VALUE]
 	MMD_FILE_NO_ERROR  	:   Success
  	Others			  	:   Error Code
-----------------------------------------------------------------------------*/
MMD_U32 MMD_FILE_FindNext(F_FIND *psFind)
{
    FARF(MSG,("MMD_FILE_FindNext"));
    return f_findnext(psFind);
}


/*-----------------------------------------------------------------------------
[FUNCTION NAME]
	MMD_FILE_MkDir
[DESCRIPTION]
	Creates a new directory.
[PARAMETERS]
	pbPath(IN) 	: 	the path of directory to be created, the little-endian Unicode stream 
 					with double null terminator is required. i.e. {'n', 0, 'e', 0, 'w', 0, 0, 0}   
[RETURN VALUE]
 	MMD_FILE_NO_ERROR  	:   Success
  	Others			  	:   Error Code
-----------------------------------------------------------------------------*/
MMD_U32 MMD_FILE_MkDir(MMD_U08 *pbPath)
{
    FARF(MSG,("MMD_FILE_MkDir"));
    return f_mkdir((char *)pbPath);
}

/*-----------------------------------------------------------------------------
[FUNCTION NAME]
	MMD_FILE_RmDir
[DESCRIPTION]
	Removes a directory. 
[PARAMETERS]
	pbPath(IN) 	: 	the path of directory to be removed, the little-endian Unicode stream 
 					with double null terminator is required. i.e. {'d', 0, 'i', 0, 'r', 0, 0, 0}   
[RETURN VALUE]
 	MMD_FILE_NO_ERROR  	:   Success
  	Others			  	:   Error Code
[REMARKS]
	If the target directory not empty or it is read-only then this
	returns with an error.  	
-----------------------------------------------------------------------------*/
MMD_U32 MMD_FILE_RmDir(MMD_U08 *pbPath)
{
     FARF(MSG,("MMD_FILE_RmDir"));
    return f_rmdir((char *)pbPath);
}

/*-----------------------------------------------------------------------------
[FUNCTION NAME]
	MMD_FILE_ChDir
[DESCRIPTION]
	Changes the current working directory to the specified directory.
[PARAMETERS]
	pbPath(IN) 	: 	the path of new working directory, , the little-endian Unicode stream 
 					with double null terminator is required. i.e. {'d', 0, 'i', 0, 'r', 0, 0, 0} 
[RETURN VALUE]
 	MMD_FILE_NO_ERROR  	:   Success
  	Others			  	:   Error Code
-----------------------------------------------------------------------------*/
MMD_U32 MMD_FILE_ChDir(MMD_U08 *pbPath)
{
    FARF(MSG,("MMD_FILE_ChDir"));
    return f_chdir((char *)pbPath);
}

/*-----------------------------------------------------------------------------
[FUNCTION NAME]
	MMD_FILE_GetCWD
[DESCRIPTION]
	Retrives the name of current working directory.
[PARAMETERS]
	pbPath(OUT) 	: 	where to store the path of current working directory 
[RETURN VALUE]
 	MMD_FILE_NO_ERROR  	:   Success
  	Others			  	:   Error Code
-----------------------------------------------------------------------------*/
MMD_U32 MMD_FILE_GetCWD(MMD_U08 *pbPath)
{
    return f_getcwd((char *)pbPath, F_MAXPATH);
}

/*-----------------------------------------------------------------------------
[FUNCTION NAME]
	MMD_FILE_Rename
[DESCRIPTION]
	Renames  a file or directory 
[PARAMETERS]
	pbName(IN)		: 	target file or directory name with/without path
	pbNewName(IN)	:	new name of file or directory without path
[RETURN VALUE]
	MMD_FILE_NO_ERROR   :   Success
 	Others			  	:   Error Code
[REMARKS]
	The little-endian Unicode streams with double null terminator are required for
	these two parameters. i.e. {'a', 0 , '.', 0, 't', 0, 'x', 0, 't', 0, 0, 0}     	
-----------------------------------------------------------------------------*/
MMD_U32 MMD_FILE_Rename(MMD_U08 *pbName, MMD_U08 *pbNewName)
{
    return f_rename((char *)pbName, (char *)pbNewName);
}

/*-----------------------------------------------------------------------------
[FUNCTION NAME]
	MMD_FILE_GetTimeDate
[DESCRIPTION]
	Retrives the time and date of a file or directory.
[PARAMETERS]
	pbPath(IN) 		: 	the path of directory or file to be set,  the little-endian Unicode stream with 
						double null terminator is required. i.e. {'a', 0 , '.', 0, 't', 0, 'x', 0, 't', 0, 0, 0}    
	psDate(IN) 		: 	the date of the directory or file
	psTime(IN) 		: 	the time of the directory or file
[RETURN VALUE]
 	MMD_FILE_NO_ERROR  	:   Success
  	Others			  	:   Error Code
-----------------------------------------------------------------------------*/
MMD_U32 MMD_FILE_GetTimeDate(MMD_U08 *pbPath, MMD_DATE *psDate, MMD_TIME *psTime)
{
	MMD_U16 *pctime, *pcdate;
	MMD_U32 ret;

	ret = f_gettimedate((char *)pbPath, pctime, pcdate);
	if (ret)
		return ret;

	psDate->year = ((*pcdate & F_CDATE_YEAR_MASK) >> F_CDATE_YEAR_SHIFT) + 1980;
	psDate->month = (*pcdate & F_CDATE_MONTH_MASK) >> F_CDATE_MONTH_SHIFT;
	psDate->day = *pcdate & F_CDATE_DAY_MASK;

	psTime->hour = (*pctime & F_CTIME_HOUR_MASK) >> F_CTIME_HOUR_SHIFT;
	psTime->minute = (*pctime & F_CTIME_MIN_MASK) >> F_CTIME_MIN_SHIFT;
	psTime->second = (*pctime & F_CTIME_SEC_MASK) << 1;
		
	return MMD_FILE_NO_ERROR;
}


/*-----------------------------------------------------------------------------
[FUNCTION NAME]
	MMD_FILE_SetTimeDate
[DESCRIPTION]
	Sets the time and date of a file or directory.
[PARAMETERS]
	pbPath(IN) 		: 	the path of directory or file to be set,  the little-endian Unicode stream with 
						double null terminator is required. i.e. {'a', 0 , '.', 0, 't', 0, 'x', 0, 't', 0, 0, 0}    
	psDate(IN) 		: 	the date to set
	psTime(IN) 		: 	the time to set
[RETURN VALUE]
 	MMD_FILE_NO_ERROR  	:   Success
  	Others			  	:   Error Code
[REMARKS]
	The valid value range of year is 1980 - 2107.  	
-----------------------------------------------------------------------------*/
MMD_U32 MMD_FILE_SetTimeDate(MMD_U08 *pbPath, MMD_DATE *psDate, MMD_TIME *psTime)
{
	MMD_U16 ctime, cdate;

	ctime = ((psTime->hour << F_CTIME_HOUR_SHIFT) & F_CTIME_HOUR_MASK) |
			((psTime->minute << F_CTIME_MIN_SHIFT) & F_CTIME_MIN_MASK) |
			(((psTime->second >> 1) << F_CTIME_SEC_SHIFT) & F_CTIME_SEC_MASK);

	cdate = (((psDate->year-1980) << F_CDATE_YEAR_SHIFT) & F_CDATE_YEAR_MASK) |
			((psDate->month << F_CDATE_MONTH_SHIFT) & F_CDATE_MONTH_MASK) |
			((psDate->day << F_CDATE_DAY_SHIFT) & F_CDATE_DAY_MASK);
		
	return f_settimedate((char *)pbPath, ctime, cdate);
}

/*-----------------------------------------------------------------------------
[FUNCTION NAME]
	MMD_FILE_GetAttr
[DESCRIPTION]
	Retrieves the arrtibutes of a file or directory
[PARAMETERS]
	pbPath(IN) 	: 	the path of file or directory to be get,   the little-endian Unicode stream with 
					double null terminator is required. i.e. {'a', 0 , '.', 0, 't', 0, 'x', 0, 't', 0, 0, 0}  
	pbAttr(OUT) 	: 	The attributes of the file or directory. This can be one or more of 
					the attributes listed below: 
					F_ATTR_ARC
					F_ATTR_DIR
					F_ATTR_VOLUME
					F_ATTR_SYSTEM
					F_ATTR_HIDDEN
					F_ATTR_READONLY
[RETURN VALUE]
	MMD_FILE_NO_ERROR   :   Success
	Others			  	:   Error Code
-----------------------------------------------------------------------------*/
MMD_U32 MMD_FILE_GetAttr(MMD_U08 *pbPath, MMD_U08 *pbAttr)
{
	return f_getattr((char *)pbPath, pbAttr);
}

/*-----------------------------------------------------------------------------
[FUNCTION NAME]
	MMD_FILE_SetAttr
[DESCRIPTION]
	Sets the arrtibutes of a file or directory
[PARAMETERS]
	pbPath(IN) 	: 	the path of file or directory to be set,   the little-endian Unicode stream with 
					double null terminator is required. i.e. {'a', 0 , '.', 0, 't', 0, 'x', 0, 't', 0, 0, 0}  
	bAttr(IN) 		: 	The desired attributes of the file or directory. This can be one or more of 
					the attributes listed below: 
					F_ATTR_ARC
					F_ATTR_SYSTEM
					F_ATTR_HIDDEN
					F_ATTR_READONLY
[RETURN VALUE]
	MMD_FILE_NO_ERROR   :   Success
	Others			  	:   Error Code
[REMARKS]
	To specify multiple attributes, separate them with | symbols. 	
-----------------------------------------------------------------------------*/
MMD_U32 MMD_FILE_SetAttr(MMD_U08 *pbPath, MMD_U08 bAttr)
{
	return f_setattr((char *)pbPath, bAttr);
}

/*-----------------------------------------------------------------------------
[FUNCTION NAME]
 	MMD_FILE_Open
[DESCRIPTION]
 	Open a file for reading/writing/appending
[PARAMETERS]
 	pbPath (IN) 	: 	the path of file to open or create, the little-endian Unicode stream 
 					with double null terminator is required. i.e. {'a', 0 , '.', 0, 't', 0, 'x', 0, 't', 0, 0, 0}
 	pbMode (IN) 	: 	open mode.
					"r", opens for reading. If the file does not exist or cannot be found, return fail. 
					"w", opens an empty file for writing. If the given file exists, its contents are destroyed. 
					"a", opens for writing at the end of the file (appending), creates the file first if it doesn't exist. 
					"r+", opens for both reading and writing. (The file must exist.) 
					"w+", opens an empty file for both reading and writing. If the given file exists, its contents are destroyed. 
					"a+", opens for reading and appending; creates the file first if it doesn't exist. 					
 	psFile (OUT) 	: 	the handle of opened file
[RETURN VALUE]
 	MMD_FILE_NO_ERROR   :   Success
 	Others			  	:   Error Code
-----------------------------------------------------------------------------*/
MMD_U32 MMD_FILE_Open(MMD_U08 *pbPath, MMD_U08 *pbMode, F_FILE **ppFile)
{
	*ppFile = f_open((char *)pbPath, (char *)pbMode);
	if(*ppFile == NULL)
		return MMD_FILE_ERR_NOMOREHANDLE;
	
	return MMD_FILE_NO_ERROR;
}

/*-----------------------------------------------------------------------------
[FUNCTION NAME]
	MMD_FILE_Write
[DESCRIPTION]
	Writes data to an opened file.	
[PARAMETERS]
	psFile(IN) 		: 	the handle of file
	pdData(IN) 		: 	where to store data to write
	dLength(IN) 		: 	the number of bytes to write
	pdWrLen(OUT) 	: 	the number of actually written bytes
[RETURN VALUE]
 	MMD_FILE_NO_ERROR   :   Success
 	Others			  	:   Error Code
-----------------------------------------------------------------------------*/
MMD_U32 MMD_FILE_Write(F_FILE *psFile, MMD_U08 *pbData, MMD_U32 dLength, MMD_U32 *pdWrLen)
{
#ifdef MMD_FILE_DEBUG_INFO
	int i;
	PRINTF(("\r\n## %03d: dLength = %d", g_FILE_DbgCnt++, dLength));
	PRINTF(("\r\n         # filesize = %d, abspos = %d, relpos = %d", psFile->filesize, psFile->abspos, psFile->relpos));
	PRINTF(("\r\n         # cluster = %d, sector = %d, sectorend = %d", psFile->pos.cluster, psFile->pos.sector, psFile->pos.sectorend));
	PRINTF(("\r\n		  # "));
	for (i = 0; i < dLength; i++) {
		if (i == 4) {
			PRINTF(("... %02X, %02X, %02X, %02X", pbData[dLength-3], pbData[dLength-2], pbData[dLength-1], pbData[dLength]));
			break;
		}
		PRINTF(("%02X, ", pbData[i]));
	}
#endif

	*pdWrLen = f_write(pbData, 1, dLength, psFile);

#ifdef MMD_FILE_DEBUG_INFO
	PRINTF(("\r\n         * filesize = %d, abspos = %d, relpos = %d", psFile->filesize, psFile->abspos, psFile->relpos));
	PRINTF(("\r\n         * cluster = %d, sector = %d, sectorend = %d", psFile->pos.cluster, psFile->pos.sector, psFile->pos.sectorend));	
#endif

	if (*pdWrLen < dLength)
		return MMD_FILE_ERR_NOMORESPACE;
	
	return MMD_FILE_NO_ERROR;
}

/*-----------------------------------------------------------------------------
[FUNCTION NAME]
 	MMD_FILE_Read
[DESCRIPTION]
 	Reads the opened file
[PARAMETERS]
 	psFile (IN) 		: 	the handle of opened file
 	pbData (IN) 		: 	user-specified buffer to store read data
 	dLength (IN) 		:  	the number of bytes to read
 	pdRdLen (OUT) 	: 	where to store the number of read bytes
[RETURN VALUE]
	MMD_FILE_NO_ERROR	:	Success
	Others				:	Error Code
-----------------------------------------------------------------------------*/
MMD_U32 MMD_FILE_Read(F_FILE *psFile, MMD_U08 *pbData, MMD_U32 dLength, MMD_U32 *pdRdLen)
{
	*pdRdLen = f_read(pbData, 1, dLength, psFile);
	if (*pdRdLen < dLength)
		return MMD_FILE_ERR_NOMOREDATA;
	
	return MMD_FILE_NO_ERROR;
}

/*-----------------------------------------------------------------------------
[FUNCTION NAME]
	MMD_FILE_Close
[DESCRIPTION]
 	Closes the opened file.
[PARAMETERS]
	psFile (IN) 	: 	the handle of opened file
[REMARKS]

[RETURN VALUE]
	MMD_FILE_NO_ERROR   :   Success
 	Others			  	:   Error Code
-----------------------------------------------------------------------------*/
MMD_U32 MMD_FILE_Close(F_FILE *psFile)
{
	return f_close(psFile);
}

/*-----------------------------------------------------------------------------
[FUNCTION NAME]
	MMD_FILE_Seek
[DESCRIPTION]
	Moves position to specified offset of an opened file
[PARAMETERS]
	psFile (IN) 		: 	the handle of opened file
	dOffset (IN) 		: 	number of bytes from whence
	bWhence (IN) 		: 	initial position to calculate offset, must be one of the following constants, 
						F_SEEK_SET, beginning of file
						F_SEEK_CUR, current position of file pointer
						F_SEEK_END  end of file
[RETURN VALUE]
	MMD_FILE_NO_ERROR   :   Success
 	Others			  	:   Error Code
-----------------------------------------------------------------------------*/
MMD_U32 MMD_FILE_Seek(F_FILE *psFile, MMD_U32 dOffset, MMD_U08 bWhence)
{
	return f_seek(psFile, dOffset, bWhence);
}

/*-----------------------------------------------------------------------------
[FUNCTION NAME]
  	MMD_FILE_Tell
[DESCRIPTION]
  	Gets current position from the beginning of opened file
[PARAMETERS]
	psFile (IN) 	: 	the handle of opened file
	pdPos (OUT) 	: 	where to store the position
[RETURN VALUE]
	MMD_FILE_NO_ERROR   :   Success
	Others			  	:   Error Code
-----------------------------------------------------------------------------*/
MMD_U32 MMD_FILE_Tell(F_FILE *psFile, MMD_U32 *pdPos)
{
	*pdPos = f_tell(psFile);
	
	return MMD_FILE_NO_ERROR;
}

/*-----------------------------------------------------------------------------
[FUNCTION NAME]
  	MMD_FILE_SetEOF
[DESCRIPTION]
  	Sets end of opened file
[PARAMETERS]
	psFile (IN) 	: 	the handle of opened file
	dSize (IN) 	: 	the desired size of file
[RETURN VALUE]
	MMD_FILE_NO_ERROR   :   Success
	Others			  	:   Error Code
-----------------------------------------------------------------------------*/
MMD_U32 MMD_FILE_SetEOF(F_FILE *psFile, MMD_U32 dSize)
{
	return f_seteof(psFile, dSize);
}

/*-----------------------------------------------------------------------------
[FUNCTION NAME]
	MMD_FILE_Delete
[DESCRIPTION]
	Deletes a file. A read-only or opened file cannot be deleted.
[PARAMETERS]
	pbPath (IN)	:	the path of file to be deleted, the little-endian Unicode stream 
 					with double null terminator is required. i.e. {'a', 0 , '.', 0, 't', 0, 'x', 0, 't', 0, 0, 0}    
[RETURN VALUE]
 	MMD_FILE_NO_ERROR   :   Success
 	Others			  	:   Error Code
-----------------------------------------------------------------------------*/
MMD_U32 MMD_FILE_Delete(MMD_U08 *pbPath)
{
	//g_FILE_DbgCnt = 0;
	return f_delete((char *)pbPath);
}

/*-----------------------------------------------------------------------------
[FUNCTION NAME]
	MMD_FILE_Copy
[DESCRIPTION]
	Copies file to specified destination.
[PARAMETERS]
	pbSrcPath(IN)		:	the path of source file to copy, the little-endian Unicode stream 
 						with double null terminator is required. i.e. {'a', 0 , '.', 0, 't', 0, 'x', 0, 't', 0, 0, 0}
	pbDestPath(IN)	:	the path of destination, the little-endian Unicode stream 
 						with double null terminator is required. i.e. {'b', 0 , '.', 0, 't', 0, 'x', 0, 't', 0, 0, 0}
[RETURN VALUE]
	MMD_FILE_NO_ERROR	:	Success
	Others				:	Error Code
[REMARKS]
	If source and destination are in the same path, the file name of destination should 	be differ from
	that of source. 
	It means in current working directory if no path is specified in parameters.	
-----------------------------------------------------------------------------*/
MMD_U32 MMD_FILE_Copy(MMD_U08 *pbSrcPath, MMD_U08 *pbDestPath)
{
	F_FILE *fsrc = NULL, *fdest = NULL;
	MMD_U08 *ptr = NULL;
	MMD_U32 rdsize = 0, wrsize = 0;

	/* open source file */
	fsrc = f_open((char *)pbSrcPath, "r");
	if(!fsrc)
		return MMD_FILE_ERR_CANNOTOPEN;
	/* check if destination duplicated, not allow to copy source to an existed file */
	fdest = f_open((char *)pbDestPath, "r");
	if(fdest) {
		f_close(fsrc);
		f_close(fdest);
		return MMD_FILE_ERR_DUPLICATED;
	}
	/* create destination file */
	fdest = f_open((char *)pbDestPath, "w");
	if(!fdest) {
		f_close(fsrc);
		return MMD_FILE_ERR_CANNOTOPEN;
	}
	/* malloc data buffer to exchange data */
	ptr = MMD_Malloc(MMD_FILE_COPY_BUFFER_SIZE);
	if(!ptr) {
		f_close(fsrc);
		f_close(fdest);
		return MMD_FILE_ERR_NULLPOINTER;
	}
	/* start copying */
	while(fdest->filesize < fsrc->filesize) {
		rdsize = f_read(ptr, 1, MMD_FILE_COPY_BUFFER_SIZE, fsrc);
		wrsize = f_write(ptr, 1, rdsize, fdest);
		if(wrsize != rdsize || !rdsize) //error or no data
			break;		
	}

	MMD_Free(ptr);
	
	if(fdest->filesize != fsrc->filesize) {
		f_close(fsrc);
		f_close(fdest);
		return MMD_FILE_ERR_WRITE;
	}

	f_close(fsrc);
	f_close(fdest);
	
	return MMD_FILE_NO_ERROR;
}

/*-----------------------------------------------------------------------------
[FUNCTION NAME]
	MMD_FILE_StartFastCopy
[DESCRIPTION]
	Starts a fast copy.
[PARAMETERS]
	pbSrcPath(IN)		:	the path of source file to copy, the little-endian Unicode stream 
 						with double null terminator is required. i.e. {'a', 0 , '.', 0, 't', 0, 'x', 0, 't', 0, 0, 0}
	pbDestPath(IN)	:	the path of destination, the little-endian Unicode stream 
 						with double null terminator is required. i.e. {'b', 0 , '.', 0, 't', 0, 'x', 0, 't', 0, 0, 0}
 	pfCallback(IN)		:	pointer to user-specified callback function
[RETURN VALUE]
	MMD_FILE_NO_ERROR	:	Success
	Others				:	Error Code
[REMARKS]
	The callback fuction will be called in fast copy timer ISR when copy finished or error occured. 
	The parameter of callback fuction is passed by timer ISR while MMD_FILE_NO_ERROR means copy 
	finished successfully and others mean error occured.	
-----------------------------------------------------------------------------*/
MMD_U32 MMD_FILE_StartFastCopy(MMD_U08 *pbSrcPath, MMD_U08 *pbDestPath, F_CALLBACK pfCallback)
{
	MMD_U32 rdsize = 0, ret, status;
	F_FASTCOPY *p_fastcopy = &g_FILE_FastCopy;

	/* check if there is already a fast copy processing */
	if (p_fastcopy->fsrc && p_fastcopy->fdst)
		return MMD_FILE_ERR_FASTCOPYBUSY; /* only one fast copy process allowed */
	
	/* open source file */
	p_fastcopy->fsrc = f_open((char *)pbSrcPath, "r");
	if(!p_fastcopy->fsrc)
		return MMD_FILE_ERR_CANNOTOPEN;
	/* check if destination duplicated, not allow to copy source to an existed file */
	p_fastcopy->fdst = f_open((char *)pbDestPath, "r");
	if(p_fastcopy->fdst) {
		f_close(p_fastcopy->fsrc);
		f_close(p_fastcopy->fdst);
		MMD_MemSet(p_fastcopy, 0, sizeof(F_FASTCOPY));
		return MMD_FILE_ERR_DUPLICATED;
	}
	/* create destination file */
	p_fastcopy->fdst = f_open((char *)pbDestPath, "w");
	if(!p_fastcopy->fdst) {
		f_close(p_fastcopy->fsrc);
		MMD_MemSet(p_fastcopy, 0, sizeof(F_FASTCOPY));
		return MMD_FILE_ERR_CANNOTOPEN;
	}

	if (!p_fastcopy->fsrc->filesize) {
			f_close(p_fastcopy->fdst);
			f_close(p_fastcopy->fsrc);
			MMD_MemSet(p_fastcopy, 0, sizeof(F_FASTCOPY));
			if (pfCallback)
				pfCallback(MMD_FILE_NO_ERROR);
			return MMD_FILE_NO_ERROR;
	}
	
	/* malloc data buffer in VC0848 SRAM to exchange data */
	p_fastcopy->core_mem.addr = MMD_SYS_Malloc(MMD_FILE_FASTCOPY_BUFFER_SIZE);
	if(!p_fastcopy->core_mem.addr) {
		f_close(p_fastcopy->fsrc);
		f_close(p_fastcopy->fdst);
		MMD_MemSet(p_fastcopy, 0, sizeof(F_FASTCOPY));
		return MMD_FILE_ERR_DEVMALLOCFAIL;
	}

	p_fastcopy->core_mem.reg = IDX_AUD_STATUS;
	p_fastcopy->core_mem.bit = 0;
	ret = MMD_FILE_Read2Core(p_fastcopy->fsrc, p_fastcopy->core_mem, MMD_FILE_FASTCOPY_BUFFER_SIZE, &rdsize);
	if (!ret) 
		ret = MMD_FILE_StartRead2Core();
	if (ret) {
		f_close(p_fastcopy->fsrc);
		f_close(p_fastcopy->fdst);		
		MMD_SYS_Free(p_fastcopy->core_mem.addr);
		MMD_MemSet(p_fastcopy, 0, sizeof(F_FASTCOPY));
		return ret;
	}

	p_fastcopy->work_mode = 1;
	p_fastcopy->wr_flag = 1;
	p_fastcopy->nbytes = rdsize;
	p_fastcopy->callback = pfCallback;
	
	/* start copying */
	MMD_USER_Timer_Set(MMD_FILE_FastCopyTisr, MMD_FILE_FASTCOPY_TIMER_INTERVAL);
	MMD_BIU_Get_Device_Status(IDX_FILE_STATUS, &status);	
	status &= (~0x1); /* clear busy bit */
	status &= 0xFFFF; /* clear error bits */
	MMD_BIU_Set_Device_Status(IDX_FILE_STATUS, status);	
	MMD_USER_Timer_Start();
	
	return MMD_FILE_NO_ERROR;
}

/*-----------------------------------------------------------------------------
[FUNCTION NAME]
	MMD_FILE_AbortFastCopy
[DESCRIPTION]
	Aborts the current copy.
[RETURN VALUE]
	MMD_FILE_NO_ERROR	:	Success
	Others				:	Error Code
-----------------------------------------------------------------------------*/
MMD_U32 MMD_FILE_AbortFastCopy(void)
{
	MMD_U32 status;
	F_FASTCOPY *p_fastcopy = &g_FILE_FastCopy;

	if (!p_fastcopy->fsrc || !p_fastcopy->fdst)
		return MMD_FILE_ERR_NOTOPEN;

	if (p_fastcopy->work_mode == 2) {
		/* for fast copy paused */
		f_close(p_fastcopy->fdst);
		f_close(p_fastcopy->fsrc);		
		MMD_MemSet(p_fastcopy, 0, sizeof(F_FASTCOPY));
	} else {	
		MMD_USER_Timer_Stop();
		
		do {
			/* wait for SD card read or write operation in VC0848 Core to finish */
			MMD_BIU_Get_Device_Status(IDX_FILE_STATUS, &status);
		} while (status & 0x1);

		if (status >> 16)
			MMD_SD_Reset();

		f_close(p_fastcopy->fdst);
		f_close(p_fastcopy->fsrc);
		MMD_SYS_Free(p_fastcopy->core_mem.addr);
		MMD_MemSet(p_fastcopy, 0, sizeof(F_FASTCOPY));

		MMD_USER_Timer_Release();
	}
	return MMD_FILE_NO_ERROR;
}

/*-----------------------------------------------------------------------------
[FUNCTION NAME]
	MMD_FILE_PauseFastCopy
[DESCRIPTION]
	Pauses a current fast copy.
[RETURN VALUE]
	MMD_FILE_NO_ERROR	:	Success
	Others				:	Error Code
[REMARKS]
	If returns error code, current fast copy will be aborted.	
-----------------------------------------------------------------------------*/
MMD_U32 MMD_FILE_PauseFastCopy(void)
{
	MMD_U32 status, ret;
	F_FASTCOPY *p_fastcopy = &g_FILE_FastCopy;

	if (!p_fastcopy->fsrc || !p_fastcopy->fdst)
		return MMD_FILE_ERR_NOTOPEN;

	if (p_fastcopy->work_mode == 2)
		return MMD_FILE_NO_ERROR; /* paused already */
	
	MMD_USER_Timer_Stop();
	
	do {
		/* wait for SD card read or write operation in VC0848 Core to finish */
		MMD_BIU_Get_Device_Status(IDX_FILE_STATUS, &status);
	} while (status & 0x1);

	ret = MMD_FILE_NO_ERROR;
	if (status >> 16) {
		MMD_SD_Reset();
		if (p_fastcopy->wr_flag == 2) {
			/* seek back & set eof */
			ret = f_seteof(p_fastcopy->fdst, (p_fastcopy->fdst->filesize - p_fastcopy->nbytes));
			p_fastcopy->wr_flag = 1; /* read again when resume */
			MMD_BIU_Set_Device_Status(IDX_FILE_STATUS, (status & 0xFFFF)); /* clear error bit */
		}
	}

	MMD_SYS_Free(p_fastcopy->core_mem.addr);

	MMD_USER_Timer_Release();

	p_fastcopy->work_mode = 2;

	if (ret) {
		f_close(p_fastcopy->fdst);
		f_close(p_fastcopy->fsrc);		
		MMD_MemSet(p_fastcopy, 0, sizeof(F_FASTCOPY));
	}

	return ret;
}

/*-----------------------------------------------------------------------------
[FUNCTION NAME]
	MMD_FILE_ResumeFastCopy
[DESCRIPTION]
	Resumes a paused fast copy.
[RETURN VALUE]
	MMD_FILE_NO_ERROR	:	Success
	Others				:	Error Code
[REMARKS]
	If returns error code, current fast copy will be aborted.
-----------------------------------------------------------------------------*/
MMD_U32 MMD_FILE_ResumeFastCopy(void)
{
	MMD_U32 pos, ret, size;
	F_FASTCOPY *p_fastcopy = &g_FILE_FastCopy;

	if (!p_fastcopy->fsrc || !p_fastcopy->fdst)
		return MMD_FILE_ERR_NOTOPEN;

	if (p_fastcopy->work_mode != 2)
		return MMD_FILE_ERR_FASTCOPYNOTPAUSE;

	/* malloc data buffer in VC0848 SRAM to exchange data */
	p_fastcopy->core_mem.addr = MMD_SYS_Malloc(MMD_FILE_FASTCOPY_BUFFER_SIZE);
	if(!p_fastcopy->core_mem.addr) {
		f_close(p_fastcopy->fsrc);
		f_close(p_fastcopy->fdst);
		MMD_MemSet(p_fastcopy, 0, sizeof(F_FASTCOPY));
		return MMD_FILE_ERR_DEVMALLOCFAIL;
	}

	ret = MMD_FILE_NO_ERROR;
	if (p_fastcopy->wr_flag == 1) {
		/* seek back */
		pos = f_tell(p_fastcopy->fsrc);
		pos -= p_fastcopy->nbytes;
		ret = f_seek(p_fastcopy->fsrc, pos, F_SEEK_SET);
	}

	if (!ret)
		ret = MMD_FILE_Read2Core(p_fastcopy->fsrc, p_fastcopy->core_mem, MMD_FILE_FASTCOPY_BUFFER_SIZE, &size);
	if (!ret) 
		ret = MMD_FILE_StartRead2Core();
	if (ret) {
		f_close(p_fastcopy->fsrc);
		f_close(p_fastcopy->fdst);		
		MMD_SYS_Free(p_fastcopy->core_mem.addr);
		MMD_MemSet(p_fastcopy, 0, sizeof(F_FASTCOPY));
		return ret;
	}
	
	p_fastcopy->work_mode = 1;
	p_fastcopy->wr_flag = 1;
	p_fastcopy->nbytes = size;

	/* resume copying */
	MMD_USER_Timer_Set(MMD_FILE_FastCopyTisr, MMD_FILE_FASTCOPY_TIMER_INTERVAL);
	MMD_USER_Timer_Start();

	return MMD_FILE_NO_ERROR;
}

/*-----------------------------------------------------------------------------
[FUNCTION NAME]
	MMD_FILE_GetFastCopyProgress
[DESCRIPTION]
	Retrieves the progress percentage of current fast copy.
[RETURN VALUE]
	Progress percentage of current fast copy(0~100).
[REMARKS]
	If no fast copy processing, the return value is 100.
-----------------------------------------------------------------------------*/
MMD_U08 MMD_FILE_GetFastCopyProgress(void)
{
	MMD_U08 percent = 0;
	MMD_U32 src_size, dst_size;
	F_FASTCOPY *p_fastcopy = &g_FILE_FastCopy;

	if (p_fastcopy->fsrc && p_fastcopy->fdst) {
		src_size = p_fastcopy->fsrc->filesize;
		dst_size = p_fastcopy->fdst->filesize;
		if (src_size > 100)
			percent = dst_size / (src_size / 100);
		else
			percent = (dst_size * 100) / src_size;
		if (percent >= 100)
			percent = 99;		
	} else
		percent = 100;

	return percent;
}

/*-----------------------------------------------------------------------------
[FUNCTION NAME]
 	MMD_FILE_DupHandle
[DESCRIPTION]
 	Duplicate a new handle from a given handle, only support 'r' mode
[PARAMETERS]
 	psFileSrc (IN) 	: 	the given handle to duplicate. 
 	pFile (OUT) 	: 	where to store the duplicated handle
[RETURN VALUE]
 	MMD_FILE_NO_ERROR   :   Success
 	Others			  	:   Error Code
-----------------------------------------------------------------------------*/
MMD_U32 MMD_FILE_DupHandle(F_FILE *psFileSrc, F_FILE **ppFile)
{
	*ppFile = f_duphandle(psFileSrc);
	if(*ppFile == NULL)
		return MMD_FILE_ERR_NOMOREHANDLE;
	
	return MMD_FILE_NO_ERROR;
}

/*-----------------------------------------------------------------------------
[FUNCTION NAME]
 	MMD_FILE_IsFileExisted
[DESCRIPTION]
 	Check if the specified file is existed.
[PARAMETERS]
 	pbPath (IN) 	: 	the path of file to check, the little-endian Unicode stream 
 					with double null terminator is required. i.e. {'a', 0 , '.', 0, 't', 0, 'x', 0, 't', 0, 0, 0}
 	pbFlag (OUT) 	: 	0, file is not existed; 1, file is existed.
[RETURN VALUE]
 	MMD_FILE_NO_ERROR   :   Success
 	Others			  	:   Error Code
-----------------------------------------------------------------------------*/
MMD_U32 MMD_FILE_IsFileExisted(MMD_U08 *pbPath, MMD_U08 *pbFlag)
{
	F_FILE *f;

	f = f_open((char *)pbPath, (char *)"r");
	if(f != NULL) {
		*pbFlag = 1;
		return f_close(f);
	}

	*pbFlag = 0;
	return MMD_FILE_NO_ERROR;
}

/*-----------------------------------------------------------------------------
[FUNCTION NAME]
 	MMD_FILE_InitCore
[DESCRIPTION]
 	Initials global parameters in VC0848 core.
[PARAMETERS]
	None.
[RETURN VALUE]
	MMD_FILE_NO_ERROR	:	Success
	Others				:	Error Code
[REMARKS]
 	This routine should be called once after mode switch if MMD_File_Read2Core or 
 	MMD_FILE_WriteFrCore	will be invoked in the new mode.
-----------------------------------------------------------------------------*/
MMD_U32 MMD_FILE_InitCore(void)
{
	MMD_U32 timeout = 1000;
	MMD_MSG msg;
	MMD_U08 socket;

	socket = g_SdDev.current_socket;
	socket &= 0x1;

	/* send command to init Mode_File */
	g_Ack_MinCmd = MIN_RESERVED;
	msg.CMD = MAJ_FIL | MIN_FIL_INIT;
	msg.OP1 = g_SdDev.capacitymode[socket]; // SD capacity mode, High-Capacity or Standard-Capacity
	MMD_BIU_Send_H2D_Cmd(&msg);
		
	while (timeout) {
		if(g_Ack_MinCmd == MIN_RESERVED)
			MMD_Check_Irq_Staus_And_Wait(1);
		else
			break;
		timeout--;
	}

	if (!timeout) {
		return MMD_FILE_ERR_ACKTIMEOUT;
	}
	return MMD_FILE_NO_ERROR;
}

/*-----------------------------------------------------------------------------
[FUNCTION NAME]
 	MMD_FILE_Read2Core
[DESCRIPTION]
 	Reads data from the opened file to VC0848 core
[PARAMETERS]
 	psFile (IN) 		: 	the handle of opened file
 	sCoreMem (IN) 	: 	the F_COREMEM structure 	
 	dLength (IN) 		:  	the number of bytes to read
 	pdRdLen (OUT) 	: 	where to store the number of read bytes
[RETURN VALUE]
	MMD_FILE_NO_ERROR	:	Success
	Others				:	Error Code
[REMARKS]
 	The F_COREMEM is used to specify the address of SRAM in VC0848 to store read data, 
 	and the bit of register to clear when finished reading;
 	The actual reading operation will be started after calling MMD_FILE_StartRead2Core	
-----------------------------------------------------------------------------*/
MMD_U32 MMD_FILE_Read2Core(F_FILE *psFile, F_COREMEM sCoreMem, MMD_U32 dLength, MMD_U32 *pdRdLen)
{
	MMD_U32 relpos, rdlen, nsectors, paralen, para_addr, ret;
	MMD_U32 timeout = 2000;
	MMD_U32 *ptr;
	MMD_MSG msg;

	relpos = psFile->relpos;
	rdlen = dLength;
	*pdRdLen = 0;
	//psFile->empty = 1;
		
	PRINTF(("\r\n current file pos: 0x%x", psFile->abspos + psFile->relpos));
	PRINTF(("\r\n file len: %d", dLength));

	if(relpos == F_SECTOR_SIZE)
	{
		psFile->abspos += psFile->relpos;
		psFile->relpos = 0;
		psFile->pos.sector++;	/* goto next */
		relpos = 0;		
	}
	else
	{
		psFile->relpos = 0;
		rdlen += relpos;
	}

	if (rdlen + psFile->relpos + psFile->abspos >= psFile->filesize)
	{	/* read len longer than the file */
		rdlen = (psFile->filesize) - (psFile->relpos) - (psFile->abspos);  /* calculate new size */
	}

	if(!rdlen) return MMD_FILE_NO_ERROR;

	nsectors = (rdlen + F_SECTOR_SIZE - 1) / F_SECTOR_SIZE;
	paralen = nsectors + 6; // 6 parameters + sector numbers
	paralen *= 4; // number of bytes

	ret = MMD_FILE_NO_ERROR;
	
	para_addr = MMD_SYS_Malloc(paralen);
	if(para_addr == NULL)
	{
		ret = MMD_FILE_ERR_DEVMALLOCFAIL;
	}
	ptr = MMD_Malloc(paralen);
	if(ptr == NULL)
	{
		MMD_SYS_Free(para_addr);
		ret = MMD_FILE_ERR_NULLPOINTER;
	}
	if(ret)
	{
		psFile->relpos = relpos;
		return ret;
	}

	ptr[0] = sCoreMem.reg;
	ptr[1] = sCoreMem.bit;
	ptr[2] = sCoreMem.addr; // VC0848 SRAM addr. to store read data
	ptr[3] = relpos;
	ptr[4] = rdlen;
	ptr[5] = nsectors;
	
	ret = MMD_FILE_GetSectorChain(psFile, ptr + 6, nsectors);
	if(ret)
	{
		psFile->relpos = relpos;
		MMD_SYS_Free(para_addr);
		MMD_Free(ptr);
		return ret;
	}

	/* write parameters to VC0848 SRAM for reading SD sectors */
	Vm_WrSram(para_addr, (MMD_S08 *)ptr, paralen);
	MMD_Free(ptr);

	/* send reading SD sectors command  */
	g_Ack_MinCmd = MIN_RESERVED;
	msg.CMD = MAJ_FIL|MIN_FIL_READSECTORS;
	msg.OP1 = para_addr; // VC0848 SRAM addr. to store parameters, will be freed in VC0848 Core
	MMD_BIU_Send_H2D_Cmd(&msg);
		
	while(timeout)
	{
		if(g_Ack_MinCmd == MIN_RESERVED)
			MMD_Check_Irq_Staus_And_Wait(1);
		else
			break;
		timeout--;
	}

	if(!timeout)
	{
		return MMD_FILE_ERR_ACKTIMEOUT;
	}
	
	*pdRdLen = rdlen - relpos;
	psFile->relpos = rdlen%F_SECTOR_SIZE; // offset in current sector
	if(psFile->relpos == 0) psFile->relpos = F_SECTOR_SIZE;
	psFile->abspos += rdlen - psFile->relpos; // ensure abspos is multiple of F_SECTOR_SIZE
	psFile->cached = 0;

	return MMD_FILE_NO_ERROR;
}

/*-----------------------------------------------------------------------------
[FUNCTION NAME]
 	MMD_FILE_StartRead2Core
[DESCRIPTION]
 	Starts reading data to VC0848 core according to the parameters set in MMD_FILE_Read2Core
[RETURN VALUE]
	MMD_FILE_NO_ERROR	:	Success
	Others				:	Error Code
[REMARKS]
	This function must be called after MMD_FILE_Read2Core	
-----------------------------------------------------------------------------*/
MMD_U32 MMD_FILE_StartRead2Core(void)
{
	MMD_U32 timeout = 2000;
	MMD_MSG msg;

	/* send command to start reading SD sectors */
	g_Ack_MinCmd = MIN_RESERVED;
	msg.CMD = MAJ_FIL|MIN_FIL_STARTTOREAD;
	MMD_BIU_Send_H2D_Cmd(&msg);
		
	while(timeout)
	{
		if(g_Ack_MinCmd == MIN_RESERVED)
			MMD_Check_Irq_Staus_And_Wait(1);
		else
			break;
		timeout--;
	}
	
	
	
	if(!timeout)
	{
		return MMD_FILE_ERR_ACKTIMEOUT;
	}
	
	return MMD_FILE_NO_ERROR;
}

/*-----------------------------------------------------------------------------
[FUNCTION NAME]
 	MMD_FILE_WriteFrCore
[DESCRIPTION]
 	Writes data from VC0848 core to the opened file in SD card. 
[PARAMETERS]
 	psFile (IN) 		: 	the handle of opened file
 	sCoreMem (IN) 	: 	the F_COREMEM structure 	
 	dLength (IN) 		:  	the number of bytes to write
 	pdWrLen (OUT) 	: 	where to store the number of written bytes
[RETURN VALUE]
	MMD_FILE_NO_ERROR	:	Success
	Others				:	Error Code
[REMARKS]
 	The F_COREMEM is used to specify the address of SRAM, that is the source data, in VC0848 core, 
 	and the bit of register to clear when finished writting;
 	The actual writting operation will be started after calling MMD_FILE_StartWriteFrCore	
-----------------------------------------------------------------------------*/
MMD_U32 MMD_FILE_WriteFrCore(F_FILE *psFile, F_COREMEM sCoreMem, MMD_U32 dLength, MMD_U32 *pdWrLen)
{
	MMD_U32 relpos, wrlen, nsectors, paralen, para_addr, ret;
	MMD_U32 timeout = 2000;
	MMD_U32 *ptr;
	MMD_MSG msg;

	relpos = psFile->relpos;
	wrlen = dLength;
	*pdWrLen = 0;
	//psFile->empty = 1;

	PRINTF(("\r\n current file pos: 0x%x", psFile->abspos + psFile->relpos));
	PRINTF(("\r\n file len: %d", dLength));

	if(relpos == F_SECTOR_SIZE)
	{
		psFile->abspos += psFile->relpos;
		psFile->relpos = 0;
		psFile->pos.sector++;	/* goto next */
		relpos = 0;		
	}
	else
	{
		psFile->relpos = 0;
		wrlen += relpos;
	}

	if(!wrlen) return MMD_FILE_NO_ERROR;

	nsectors = (wrlen + F_SECTOR_SIZE - 1) / F_SECTOR_SIZE;
	paralen = nsectors + 6; // 6 parameters + sector numbers
	paralen *= 4; // number of bytes

	ret = MMD_FILE_NO_ERROR;
	
	para_addr = MMD_SYS_Malloc(paralen);
	if(para_addr == NULL)
	{
		ret = MMD_FILE_ERR_DEVMALLOCFAIL;
	}
	ptr = MMD_Malloc(paralen);
	if(ptr == NULL)
	{
		MMD_SYS_Free(para_addr);
		ret = MMD_FILE_ERR_NULLPOINTER;
	}
	if(ret)
	{
		psFile->relpos = relpos;
		return ret;
	}

	ptr[0] = sCoreMem.reg;
	ptr[1] = sCoreMem.bit;
	ptr[2] = sCoreMem.addr; // VC0848 SRAM addr. that stored data to write;
	ptr[3] = relpos;
	ptr[4] = wrlen;
	ptr[5] = nsectors;
	
	ret = MMD_FILE_AllocSectorChain(psFile, ptr + 6, nsectors);
	if(ret)
	{
		psFile->relpos = relpos;
		MMD_SYS_Free(para_addr);
		MMD_Free(ptr);
		return ret;
	}

	/* write parameters to VC0848 SRAM for writting SD sectors */
	Vm_WrSram(para_addr, (MMD_S08 *)ptr, paralen);
	MMD_Free(ptr);

	/* send reading SD sectors command  */
	g_Ack_MinCmd = MIN_RESERVED;
	msg.CMD = MAJ_FIL|MIN_FIL_WRITESECTORS;
	msg.OP1 = para_addr; // VC0848 SRAM addr. to store parameters, will be freed in VC0848 Core
	MMD_BIU_Send_H2D_Cmd(&msg);
		
	while(timeout)
	{
		if(g_Ack_MinCmd == MIN_RESERVED)
			MMD_Check_Irq_Staus_And_Wait(3);
		else
			break;
		timeout--;
	}

	if(!timeout)
	{
		return MMD_FILE_ERR_ACKTIMEOUT;
	}
	
	*pdWrLen = wrlen - relpos;
	psFile->relpos = wrlen%F_SECTOR_SIZE; // offset in current sector
	if(psFile->relpos == 0) psFile->relpos = F_SECTOR_SIZE;
	psFile->abspos += wrlen - psFile->relpos; // ensure abspos is multiple of F_SECTOR_SIZE
	//psFile->filesize += *pdWrLen;
	if (psFile->filesize < psFile->abspos + psFile->relpos)
		psFile->filesize = psFile->abspos + psFile->relpos;

	psFile->cached = 0;

	return MMD_FILE_NO_ERROR;
}

/*-----------------------------------------------------------------------------
[FUNCTION NAME]
 	MMD_FILE_StartWriteFrCore
[DESCRIPTION]
 	Starts writing data from VC0848 core to SD card acording to the parameters setted in MMD_FILE_WriteFrCore
[RETURN VALUE]
	MMD_FILE_NO_ERROR	:	Success
	Others				:	Error Code
[REMARKS]
	This function must be called after MMD_FILE_WriteFrCore	
-----------------------------------------------------------------------------*/
MMD_U32 MMD_FILE_StartWriteFrCore(void)
{
	MMD_U32 timeout = 2000;
	MMD_MSG msg;

	/* send command to start writing SD sectors */
	g_Ack_MinCmd = MIN_RESERVED;
	msg.CMD = MAJ_FIL|MIN_FIL_STARTTOWRITE;
	MMD_BIU_Send_H2D_Cmd(&msg);
		
	while(timeout)
	{
		if(g_Ack_MinCmd == MIN_RESERVED)
			MMD_Check_Irq_Staus_And_Wait(1);
		else
			break;
		timeout--;
	}

	if(!timeout)
	{
		return MMD_FILE_ERR_ACKTIMEOUT;
	}
	
	return MMD_FILE_NO_ERROR;
}


/*-----------------------------------------------------------------------------
[FUNCTION NAME]
	MMD_FILE_Combine
[DESCRIPTION]
	Combines two files
[PARAMETERS]
	pbPath1(IN) 	: 	the path of the first file to be combined,   the little-endian Unicode stream with 
					double null terminator is required. i.e. {'a', 0 , '.', 0, 't', 0, 'x', 0, 't', 0, 0, 0}  
	pbPath2(IN) 	: 	the path of the second file to be combined,   the little-endian Unicode stream with 
					double null terminator is required. i.e. {'b', 0 , '.', 0, 't', 0, 'x', 0, 't', 0, 0, 0} 
[RETURN VALUE]
	MMD_FILE_NO_ERROR   :   Success
	Others			  	:   Error Code
[REMARKS]
	After combination, the first file will have the content of both two files, and the second file will be removed. 
	The data after EOF but in the last cluster of the first file before combination will be regarded as valid after combination.
-----------------------------------------------------------------------------*/
MMD_U32 MMD_FILE_Combine(MMD_U08 *pbPath1, MMD_U08 *pbPath2)
{
	return f_combine((char *)pbPath1, (char *)pbPath2);
}

/********************************************************************
 [FUNCTION NAME]
 	MMD_FILE_ReadSDSectors
 [DESCRIPTION]
 	Read data from sectors in SD card.
 [PARAMETERS]
	pbDataBuf: 		Pointer to the user-supplied buffer that is to receive the data read from card.
	dSecAddr:		The start sector address to read.
	dNum:			The number of sector to read				
 [RETURN VALUE]
	 MMD_FILE_NO_ERROR	:	Success
	 Others				:	Error Code		 		
*********************************************************************/
MMD_U32 MMD_FILE_ReadSDSectors(MMD_U08 *pbDataBuf, MMD_U32 dSecAddr, MMD_U32 dNum)
{
	MMD_U32 ret;

	ret = MMD_SD_ReadMultiSector(pbDataBuf, dSecAddr, dNum);
	if (ret) 
		return MMD_FILE_ERR_READ;

	return MMD_FILE_NO_ERROR;	
}

/********************************************************************
 [FUNCTION NAME]
 	MMD_FILE_WriteSDSectors
 [DESCRIPTION]
 	Write data into sectors in sectors.
 [PARAMETERS]
	pBuf: 	Pointer to the user-supplied buffer that contains the data to be written to the card.
	dSec:	The start sector address to write
	dNum:	The number of sector to write.
 [RETURN VALUE]
	 MMD_FILE_NO_ERROR	:	Success
	 Others				:	Error Code		 		
*********************************************************************/
MMD_U32 MMD_FILE_WriteSDSectors(MMD_U08 *pbDataBuf, MMD_U32 dSecAddr, MMD_U32 dNum)
{
	MMD_U32 ret;
	
	ret = MMD_SD_WriteMultiSector(pbDataBuf, dSecAddr, dNum);
	if (ret)
		return MMD_FILE_ERR_WRITE;

	return MMD_FILE_NO_ERROR;
}

