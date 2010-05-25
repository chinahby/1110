#include <stdio.h>
#include "mmp_rdk.h"
#include "thin_usr.h"
#include "fat_thin.h"

#if (AASP_FAT_ENABLE)
//Module Name
#define MOD_NAME "FAT_MODULE"
//Module Version
#define MOD_VER	"0.1"
//Module Description
#define MOD_DESC "FAT test"
//Module Owner
#define MOD_OWNER "Chen Jing'en"

#define PRINTF MMD_PRINTF

#define MALLOC_LEN  10*1024+50
#define MALLOC_DATA 0x5a

MMD_U08 write_buf[MALLOC_LEN];
MMD_U08 buf[MALLOC_LEN]={0};
MMD_U08 unicode_name[F_MAXPATH];

int fat_help(int argc, char *argv[]);
int fat_exit(int argc, char *argv[]);

int fat_mount(int argc, char *argv[]);
int fat_open(int argc, char *argv[]);
int fat_close(int argc, char *argv[]);
int fat_read(int argc, char *argv[]);
int fat_write(int argc, char *argv[]);
int fat_seek(int argc, char *argv[]);
int fat_delete(int argc, char *argv[]);
int fat_rename(int argc, char *argv[]);
int fat_format(int argc, char *argv[]);
int fat_mkdir(int argc, char *argv[]);
int fat_rmdir(int argc, char *argv[]);
int fat_chdir(int argc, char *argv[]);
int fat_setlabel(int argc, char *argv[]);
int fat_getlabel(int argc, char *argv[]);
int fat_getspace(int argc, char *argv[]);
int fat_truncate(int argc, char *argv[]);
int fat_filelength(int argc, char *argv[]);
int fat_list(int argc, char *argv[]);
int fat_settime(int argc, char *argv[]);
int fat_gettime(int argc, char *argv[]);

Subcmd fat_cmds[]={
	{
		"mount",
		"mount FAT file system",
		fat_mount
	},
	{
		"seek",
		"seek a file",
		fat_seek
	},
	{
		"read",
		"read from file",
		fat_read
	},
	{
		"write",
		"write to file",
		fat_write
	},
	{
		"open",
		"open a file",
		fat_open
	},
	{
		"close",
		"close file",
		fat_close
	},
	{
		"getspace",
		"get current disk space",
		fat_getspace
	},
	{
		"list",
		"find all files and dirs",
		fat_list
	},
	{
		"truncate",
		"truncate a file",
		fat_truncate
	},
	{
		"settime",
		"set time and date of file",
		fat_settime
	},
	{
		"gettime",
		"get time and date of file",
		fat_gettime
	},
	{
		"rename",
		"rename a file",
		fat_rename
	},
	{
		"mkdir",
		"make dir",
		fat_mkdir
	},
	{
		"rmdir",
		"remove dir",
		fat_rmdir
	},
	{
		"chdir",
		"change dir",
		fat_chdir
	},
	{
		"delete",
		"delete a file",
		fat_delete
	},
	{
		"filelen",
		"get a file length",
		fat_filelength
	},
	{
		"format",
		"format  ",
		fat_format
	},
	{
		"setlabel",
		"set label to disk ",
		fat_setlabel
	},
	{
		"getlabel",
		"get label of disk  ",
		fat_getlabel
	},
	{
		"help",
		"Help and help?",
		fat_help
	},
	{
		"exit",
		"Exit from this sub Shell",
		fat_exit
	},
	{
		0,
		0,
		0
	}
};  
int do_fat(int argc, char *argv[])
{
	
    char *mem_shell_name="fat";

    AASP_CMDWAIT(fat_cmds, mem_shell_name);
   
	return 0;
}

int fat_exit(int argc, char *argv[])
{
    EXIT_FROM_CMDWAIT = 1;	
	return 1;
}

int fat_help(int argc, char *argv[])
{
	Subcmd *cmd = fat_cmds;
	while (cmd->str) 
	{
		MSG(("%s\n %s\n", cmd->help, cmd->str));
		cmd++;
	}
    return 0;
}

int fat_mount(int argc, char *argv[])
{
	int ret = 0;

	ret = f_initvolume();
	
	if(ret)
	{
		PRINTF(("\r\nFail to mount, %d\r\n", ret));
		return -1;
	}
	else
	{	
		PRINTF(("\r\nMount OK\r\n"));	
	}
	return 1;
}

int fat_open(int argc, char *argv[])
{
	F_FILE *f;
	int ret = 0;
	int i=0;
	//int j=0;

	char filename[256]={0};
	char mode[8]={0};
	
	Input mode_input = {toString, 8, NULL};

	Input file_input = {toString, 256, NULL};

	Param arguments[] = 
	{
		{ "*1", OPTION_INPUT, required, NULL, "Please input the filename", 0 },
		{ "*2", OPTION_INPUT, required, NULL, "Please input the mode", 0 },
		{ 0, 0, 0, 0, 0 }
	};

	file_input.result = &filename;
	arguments[0].type = &file_input;	

	mode_input.result = &mode;
	arguments[1].type = &mode_input;

	//ENTER();

	ret = AASP_REQUESTMAIN(arguments, argc, argv);
	if(ret == -1)
	{
		return 0;
	}
	
	PRINTF(("\r\n Opening file '%s' with '%s' mode...", filename, mode));

	AASP_GbkStr2UnicodeStr(unicode_name, (unsigned char *)filename);
	
	f = f_open((char *)unicode_name, mode);

	if (!f)
	{
		PRINTF(("FAIL! \n"));
		return -1;
	}
	else
	{
		PRINTF(("OK! \n"));
		PRINTF((" The handle is %d\n", (unsigned int)f));
	}

	return 1;
}
int fat_close(int argc, char *argv[])
{
#if 0
	int ret = 0;
	//UINT8 filename[FILE_MAXNAME];
	UINT32 fileid;


	Input file_input = { toInt, sizeof(UINT32), NULL };
	
	Param arguments[] = 
	{
		{ "*1", OPTION_INPUT,  required, NULL,    "Please input the file id", 0 },
		{ 0, 0, 0, 0, 0 }
	};

	file_input.result = &fileid;
	arguments[0].type = &file_input;


	//ENTER();

	ret = AASP_REQUESTMAIN( arguments, argc, argv );
	if( ret == -1 )
	{
		RETURN(0);
	}
	
	MSG(("fileid %d\n",fileid));
	#ifdef FILE_MODULE_CONTROL_BY_BASEBAND
	ret = f_close((F_FILE *)fileid);
	#else
	ret = RDK_FILE_Close(fileid);
	#endif
	if ( ret )
	{
		MSG(("close file error\n"));
		RETURN(-1);
	}
	else
	{
		MSG(("close file OK\n"));
	}
#endif	
	return 1;
}

int fat_read(int argc, char *argv[])
{
	int ret = 0;
	int i;
	unsigned int handle;
	unsigned int len = 0, readlen = 0;
	unsigned long offset;
	Input file_handle = {toInt, sizeof(unsigned int), NULL};
	Input file_len = {toInt, sizeof(unsigned int), NULL};
	
	Param arguments[] = 
	{
		{ "*1", OPTION_INPUT, required, NULL, "Please input the file handle", 0 },
		{ "*2", OPTION_INPUT, required, NULL, "Please input the length to read", 0 },
		{ 0, 0, 0, 0, 0 }
	};

	file_handle.result = &handle;
	arguments[0].type = &file_handle;

	file_len.result = &len;
	arguments[1].type = &file_len;

	//ENTER();

	ret = AASP_REQUESTMAIN( arguments, argc, argv );
	if( ret == -1 )
	{
		return 0;
	}

	offset = f_tell((F_FILE *)handle);
	
	PRINTF(("\r\nfile handle is %d", handle));
	PRINTF(("\r\ncurrent file offset is %d", offset));
	PRINTF(("\r\nthe length to read is %d\r\n", len));

	ret = f_read(buf, 1, len, (F_FILE *)handle);

	if (ret)
	{		
		i = 0;
		while (i < ret+offset%16) 
		{
			if(i%16 == 0)
			{
				PRINTF(("\r\n0x%08xh:  ", ((i/16)<<4)+offset/16*16));
			}
			if(i<offset%16)
			{
				PRINTF(("   "));
			}
			else
			{
				PRINTF(("%02x ", buf[i-offset%16]));
			}
			i++;

		}
		PRINTF(("\r\n"));	
		PRINTF(("read length is %d\n",ret));
	}
	else
	{
		PRINTF(("read file len is 0\n"));

	}
	
	ret = f_tell((F_FILE *)handle);
	PRINTF(("current file offset is %d\n",ret));

	return 1;
}

int fat_write(int argc, char *argv[])
{
	int ret = 0, i;
	unsigned int handle, offset;
	unsigned int len = 0, wrlen = 0;

	Input file_handle = {toInt, sizeof(unsigned int), NULL };
	Input file_len = {toInt, sizeof(unsigned int), NULL};

	Param arguments[] = 
	{
		{ "*1", OPTION_INPUT, required, NULL, "Please input the file handle", 0 },
		{ "*2", OPTION_INPUT, required, NULL, "Please input the length to write", 0 },
		{ 0, 0, 0, 0, 0 }
	};


	file_handle.result = &handle;
	arguments[0].type = &file_handle;
	
	file_len.result = &len;
	arguments[1].type = &file_len;

	for(i=0; i<MALLOC_LEN; i++)
		write_buf[i] = i;

	//ENTER();

	ret = AASP_REQUESTMAIN(arguments, argc, argv);
	if(ret == -1)
	{
		return 0;
	}
	
	offset = f_tell((F_FILE *)handle);
	
	PRINTF(("\r\nfile handle is %d", handle));
	PRINTF(("\r\ncurrent file position is %d", offset));
	PRINTF(("\r\nthe length to write is %d\r\n", len));
	
	ret = f_write(write_buf, 1, MALLOC_LEN, (F_FILE *)handle);

	if (ret)
	{
		PRINTF(("\r\nwritten length is %d\r\n",ret));
	}
	else
	{
		PRINTF(("\r\nwritten length is %d\r\n",ret));
		return -1;
	}
	
	offset = f_tell((F_FILE *)handle);
	PRINTF(("\r\nfile position after writing is %d", offset));
	
	return 1;
}

int fat_seek(int argc, char *argv[])
{
	int ret = 0;
	unsigned int handle;
	unsigned long offset, curoffset;

	Input file_handle = {toInt, sizeof(unsigned int), NULL};
	Input file_offset = {toInt, sizeof(unsigned int), NULL};

	Param arguments[] = 
	{
		{ "*1", OPTION_INPUT, required, NULL, "Please input the file handle", 0 },
		{ "*2", OPTION_INPUT, required, NULL, "Please input the position to seek", 0 },
		{ 0, 0, 0, 0, 0 }
	};
	
	file_handle.result = &handle;
	arguments[0].type = &file_handle;
	file_offset.result = &offset;
	arguments[1].type = &file_offset;

	//ENTER();

	ret = AASP_REQUESTMAIN(arguments, argc, argv);
	if(ret == -1)
	{
		return 0;
	}
	
	curoffset = f_tell((F_FILE *)handle);
	
	PRINTF(("\r\nfile handle is %d", handle));
	PRINTF(("\r\ncurrent file position is %d", curoffset));

	ret = f_seek((F_FILE *)handle, 0, F_SEEK_END);
	if (!ret)
	{
		PRINTF(("\r\nseek file ok, postion is %d", f_tell((F_FILE *)handle)));
	}
	else
	{
		PRINTF(("\r\nseek file fail"));
		return -1;
	}	

	ret = f_seek((F_FILE *)handle, 0, F_SEEK_SET);
	if (!ret)
	{
		PRINTF(("\r\nseek file ok, postion is %d", f_tell((F_FILE *)handle)));
	}
	else
	{
		PRINTF(("\r\nseek file fail"));
		return -1;
	}

	ret = f_seek((F_FILE *)handle, 234, F_SEEK_SET);
	if (!ret)
	{
		PRINTF(("\r\nseek file ok, postion is %d", f_tell((F_FILE *)handle)));
	}
	else
	{
		PRINTF(("\r\nseek file fail"));
		return -1;
	}

	ret = f_seek((F_FILE *)handle, 7088, F_SEEK_SET);
	if (!ret)
	{
		PRINTF(("\r\nseek file ok, postion is %d", f_tell((F_FILE *)handle)));
	}
	else
	{
		PRINTF(("\r\nseek file fail"));
		return -1;
	}

	ret = f_seek((F_FILE *)handle, 10354, F_SEEK_SET);
	if (!ret)
	{
		PRINTF(("\r\nseek file ok, postion is %d", f_tell((F_FILE *)handle)));
	}
	else
	{
		PRINTF(("\r\nseek file fail"));
		return -1;
	}

	ret = f_seek((F_FILE *)handle, 566, F_SEEK_SET);
	if (!ret)
	{
		PRINTF(("\r\nseek file ok, postion is %d", f_tell((F_FILE *)handle)));
	}
	else
	{
		PRINTF(("\r\nseek file fail"));
		return -1;
	}

	ret = f_seek((F_FILE *)handle, 8000, F_SEEK_SET);
	if (!ret)
	{
		PRINTF(("\r\nseek file ok, postion is %d", f_tell((F_FILE *)handle)));
	}
	else
	{
		PRINTF(("\r\nseek file fail"));
		return -1;
	}

	return 1;
}

int fat_delete(int argc, char *argv[])
{
#if 0
	int ret = 0;
	int i=0;	
	//UINT8 filename[FILE_MAXNAME];
	INT8 filename[256]={0};
	INT8 pathfile[64]={0};	
//	FILE_MODE bMode=F_READ_OP;
	//UINT32  fileid;
	Input file_input = { toString, 256, NULL };
	//Input dir_input  = { toString, 64, NULL };


	Param arguments[] = 
	{
		//{ "*1", OPTION_INPUT,  required, NULL,    "Please input the position", 0 },
		{ "*1",  OPTION_INPUT , required, NULL, "Please input the filename",   0 },
		{ 0, 0, 0, 0, 0 }
	};

	file_input.result = &filename;
	//dir_input.result = &dirname;

	//arguments[0].type = &dir_input;
	arguments[0].type = &file_input;




	//ENTER();

	ret = AASP_REQUESTMAIN( arguments, argc, argv );
	if( ret == -1 )
	{
		RETURN(0);
	}
	
	memset(pathfile,0,sizeof(pathfile));
		#if 0
		while(dirname[i]!=0)
		{
			if(dirname[i]=='?'){
				pathfile[j++] = 0;
				i++;
			}
			else{
				pathfile[j++] = dirname[i++];	
			}
		
		}
		#endif
		i=0;
	while((filename[i]!=0)||(filename[i+1]!=0))
	{
		if(filename[i]=='|'){
			filename[i] = 0;				
		}
		i++;
	}
		filename[i++]=0;
		filename[i++]=0;
	#ifdef FILE_MODULE_CONTROL_BY_BASEBAND
	#ifdef USE_ORG_FILE_SYSTEM_IF
	ret = f_delete(filename);
	#else
	ret = fat_delete(filename);
	#endif
	#else
	ret = RDK_FILE_Delete((MMD_U08*)filename);
	#endif
	if (ret)
	{
		MSG(("delete file error!return value is %d \n",ret));
		RETURN(-1);
	}
	else
	{		
		MSG(("delete file OK\n"));
	}
#endif	
	return 1;
}

int fat_rename(int argc, char *argv[])
{
#if 0
	int ret = 0;
	int i=0;
	//int j=0;
	//INT8 oldfile[512]={0};
	//INT8 newfile[512]={0};
	INT8 oldfilename[512]={0};
	INT8 newfilename[512]={0};

	Input old_file = { toString, 512, NULL };
	Input new_file  = { toString, 512, NULL };
	//Input name_type1 = {toInt, sizeof(UINT32), NULL };
	//Input name_type2 = {toInt, sizeof(UINT32), NULL };
	
	Param arguments[] = 
	{
		{ "*1", OPTION_INPUT,  required, NULL,    "Please input the name of file you want to change", 0 },
		{ "*2",  OPTION_INPUT , required, NULL, "Please input a new name for that file",   0 },
		{ 0, 0, 0, 0, 0 }
	};

	old_file.result = &oldfilename;
	arguments[0].type = &old_file;



	new_file.result = &newfilename;
	arguments[1].type = &new_file;

	//ENTER();

	ret = AASP_REQUESTMAIN( arguments, argc, argv );
	if( ret == -1 )
	{
		RETURN(0);
	}
	i=0;
	while((oldfilename[i]!=0)||(oldfilename[i+1]!=0))
	{
		if(oldfilename[i]=='|'){
			oldfilename[i] = 0;			
		}
		i++;
	}
	oldfilename[i++] = 0;
	oldfilename[i++] = 0;
	i=0;		
	while((newfilename[i]!=0)||(newfilename[i+1]!=0))
	{
		if(newfilename[i]=='|'){
			newfilename[i] = 0;			
		}	
		i++;
	}
	newfilename[i++] = 0;
	newfilename[i++] = 0;	
	#ifdef FILE_MODULE_CONTROL_BY_BASEBAND
	#ifdef USE_ORG_FILE_SYSTEM_IF
	ret = f_rename(oldfilename,newfilename);
	#else
	ret = fat_rename(oldfilename,newfilename);
	#endif
	#else
	ret = RDK_FILE_ReName((MMD_U08*)oldfilename,(MMD_U08*)newfilename);
	#endif
	if ( ret )
	{
		MSG(("file rename error\n"));
		RETURN(-1);
	}
	else
	{
		MSG(("file rename OK\n"));
	}
#endif	
	return 1;
}

int fat_format(int argc, char *argv[])
{
#if 0
	int ret = 0;
	UINT32	bType=3;
	UINT8 volume[1]={0};
	//UINT8 disk=0;
	Input type_input = { toInt, sizeof(int),NULL };
	Input volume_input = { toString, 1, NULL };
	
	/* Register User Interfaces use your program start here */
	Param arguments[] =  {
		
		 {"*1", OPTION_INPUT, optional, NULL, "FAT12:1, FAT16:2,FAT32:3",0},	
		 { "*2", OPTION_INPUT,  required, NULL,    "Please input the volume", 0 },
		 {0,0,0,0,0}
		};//Run Just in Use
	type_input.result = &bType;
	arguments[0].type = &type_input;
	volume_input.result = &volume;
	arguments[1].type = &volume_input;

	//ENTER();

	ret = AASP_REQUESTMAIN( arguments, argc, argv );
	if( ret == -1 )
	{
		RETURN(0);
	}

	#ifdef FILE_MODULE_CONTROL_BY_BASEBAND
	#ifdef USE_ORG_FILE_SYSTEM_IF
	ret = f_format(bType);
	#else
	ret = fat_format(volume[0],bType);
	#endif
	#else
	ret = RDK_FILE_Format(volume[0],bType);
	#endif
	if ( ret !=0 )
	{
		MSG(("format fail\n"));
		RETURN(-1);
	}
	else{
		MSG(("format OK\n"));
	}
#endif	
	return 1;
}

int fat_mkdir(int argc, char *argv[])
{
#if 0
	int ret = 0;
	int i=0;
	//int j=0;
	MMD_U32 stime,etime;
	INT8 dirname[256]={0};
	//F_FILE* fileid;
	//INT8 diskname[8]={0};
	#if 1
	Input dir_input  = { toString, 256, NULL };
	//Input disk_input  = { toString, 64, NULL };
	//INT8 * pcurpath;
	//INT8 pathname[512]={0};
	Param arguments[] = 
	{
		//{ "*1", OPTION_INPUT,  required, NULL,    "Please input the position", 0 },
		{ "*1",  OPTION_INPUT , required, NULL, "Please input the name of dir",   0 },
		{ 0, 0, 0, 0, 0 }
	};

	//disk_input.result = &diskname;
	//arguments[0].type = &disk_input;
	
	dir_input.result = &dirname;
	arguments[0].type = &dir_input;	

	//ENTER();

	ret = AASP_REQUESTMAIN( arguments, argc, argv );
	if( ret == -1 )
	{
		RETURN(0);
	}

	#endif
	#if 1
	i=0;
	while((dirname[i]!=0)||(dirname[i+1]!=0))
	{
		if(dirname[i]=='|'){
			dirname[i] = 0;				
		}
		i++;
	}
		dirname[i++]=0;
		dirname[i++]=0;
	stime = VIM_TIM2_GetValue();
	#ifdef FILE_MODULE_CONTROL_BY_BASEBAND
	#ifdef USE_ORG_FILE_SYSTEM_IF
	ret = f_mkdir(dirname);
	#else
	ret = fat_mkdir(dirname);
	#endif
	#else
	ret = RDK_FILE_MakeDir((MMD_U08*)dirname);
	#endif
	etime = VIM_TIM2_GetValue();
	MSG(("make dir time is %d\n",(etime-stime)));
	if ( ret )
	{
		MSG(("make dir error,return value is %d\n",ret));
		RETURN(-1);
	}
	else
	{
		MSG(("make dir OK\n"));
	}		
#else
	#if 1
	 MMD_S08 str1[32] = {'/',0,'a',0,0,0};
	MMD_S08 str2[32] = {'/',0,'a',0,'/',0,'b',0,0,0};
	MMD_S08 str3[32] = {'/',0,'a',0,'/',0,'b',0,'/',0,'c',0,0,0};
	#else
	MMD_S08 str1[32] = {'a',0,'b',0,0,0};
	MMD_S08 str2[32] = {'b',0,0,0};
	MMD_S08 str3[32] = {'c',0,0,0};
	#endif
	MMD_S08 filename[32] = {'a',0,'b',0,'.',0,'t',0,0,0};
	ret = f_mkdir(str1);
	if(ret)
	{
		MSG(("makedir str1 error,ret=%d\n",ret));
	}
	ret = f_chdir(str1);
	if(ret)
	{
		MSG(("change dir str1 error,ret=%d\n",ret));
	}
	fileid = f_open(filename,"w");
	if(fileid==0)
		MSG(("open file error\n"));
	ret = f_close(fileid);
	if(ret)
	{
		MSG(("close file error\n"));
	}
	
 	ret = f_mkdir(str2);
	if(ret)
	{
		MSG(("makedir str2 error,ret=%d\n",ret));
	}
	ret = f_chdir(str2);
	if(ret)
	{
		MSG(("change dir str2 error,ret=%d\n",ret));
	}
	fileid = f_open(filename,"w");
	if(fileid==0)
		MSG(("open file error\n"));
	ret = f_close(fileid);
	if(ret)
	{
		MSG(("close file error\n"));
	}
	
	ret = f_mkdir(str3);
	if(ret)
	{
		MSG(("makedir str3 error,ret=%d\n",ret));
	}
	ret = f_chdir(str3);
	if(ret)
	{
		MSG(("change dir str3 error,ret=%d\n",ret));
	}
	ret = f_chdir("..");
	if(ret)
	{
		MSG(("change dir .. error,ret=%d\n",ret));
	}
	ret = f_delete(filename);
	if(ret)
		MSG(("delete file error\n"));
	ret = f_chdir("..");
	if(ret)
	{
		MSG(("change dir .. error,ret=%d\n",ret));
	}
	ret = f_delete(filename);
	if(ret)
		MSG(("delete file error\n"));//stop
	ret = f_chdir(str2);
	if(ret)
	{
		MSG(("change dir str2 error,ret=%d\n",ret));
	}
	ret = f_rmdir(str3);
	if(ret)
	{
		MSG(("remove dir str3 error,ret=%d\n",ret));
	}
	ret = f_chdir("..");
	if(ret)
	{
		MSG(("change dir .. error,ret=%d\n",ret));
	}
	ret = f_chdir("..");
	if(ret)
	{
		MSG(("change dir .. error,ret=%d\n",ret));
	}
	ret = f_chdir(str1);
	if(ret)
	{
		MSG(("change dir str1 error,ret=%d\n",ret));
	}
	ret = f_rmdir(str2);
	if(ret)
	{
		MSG(("remove dir str2 error,ret=%d\n",ret));
	}
	ret = f_chdir("..");
	if(ret)
	{
		MSG(("change dir .. error,ret=%d\n",ret));
	}
	ret = f_rmdir(str1);
	if(ret)
	{
		MSG(("remove dir str1 error,ret=%d\n",ret));
	}
#endif
#endif
	return 1;
}

int fat_rmdir(int argc, char *argv[])
{
#if 0
	int ret = 0;
	int i=0;	
	INT8 dirname[256]={0};
	//INT8 diskname[8]={0};
	Input dir_input  = { toString, 256, NULL };
	//Input disk_input  = { toString, 64, NULL };
	//INT8 * pcurpath;
	//INT8 pathname[512]={0};
	Param arguments[] = 
	{
		//{ "*1", OPTION_INPUT,  required, NULL,    "Please input the position", 0 },
		{ "*1",  OPTION_INPUT , required, NULL, "Please input the name of dir",   0 },
		{ 0, 0, 0, 0, 0 }
	};

	//disk_input.result = &diskname;
	//arguments[0].type = &disk_input;
	
	dir_input.result = &dirname;
	arguments[0].type = &dir_input;	

	//ENTER();

	ret = AASP_REQUESTMAIN( arguments, argc, argv );
	if( ret == -1 )
	{
		RETURN(0);
	}	
		#if 0
		while(diskname[i]!=0)
		{
			if(diskname[i]=='|'){
				pathname[j++] = 0;
				i++;
			}
			else{
				pathname[j++] = diskname[i++];	
			}
		
		}
		#endif
		i=0;
	while((dirname[i]!=0)||(dirname[i+1]!=0))
	{
		if(dirname[i]=='|'){
			dirname[i] = 0;				
		}
		i++;
	}
		dirname[i++]=0;
		dirname[i++]=0;
	#ifdef FILE_MODULE_CONTROL_BY_BASEBAND
	#ifdef USE_ORG_FILE_SYSTEM_IF
	ret = f_rmdir(dirname);
	#else
	ret = fat_rmdir(dirname);
	#endif
	#else
	ret = RDK_FILE_DelDir((MMD_U08*)dirname);
	#endif
	if ( ret )
	{
		MSG(("delete dir error,return value is %d\n",ret));
		RETURN(-1);
	}
	else
	{
		MSG(("delete dir OK\n"));
	}		
#endif	
	return 1;
}

int fat_chdir(int argc, char *argv[])
{
	int ret = 0;
	char dirname[256]={0};
	
	Input dir_input  = {toString, 256, NULL };		
	Param arguments[] = 
	{
		{ "*1",  OPTION_INPUT , required, NULL, "Please input the name of dir",   0 },
		{ 0, 0, 0, 0, 0 }
	};

	dir_input.result = &dirname;
	arguments[0].type = &dir_input;	

	//ENTER();

	ret = AASP_REQUESTMAIN( arguments, argc, argv );
	if( ret == -1 )
	{
		RETURN(0);
	}
	
	PRINTF(("\r\n Changing current dir to \"%s\"...", dirname));

	AASP_GbkStr2UnicodeStr(unicode_name, (unsigned char *)dirname);

	ret = f_chdir((char *)unicode_name);
	if (ret)
	{
		PRINTF(("FAIL! ret = %d\r\n", ret));
		RETURN(-1);
	}
	else
	{
		PRINTF(("OK! \r\n"));
	}		

	return 1;
}

int fat_setlabel(int argc, char *argv[])
{
#if 0
	int ret = 0;

	UINT8 labelname[12]={0};
	INT8 volume[1]={0};
	//UINT8 disk=0;
	Input label_input = { toString, 12, NULL };
	Input volume_input = { toString, 1, NULL };
	
	Param arguments[] = 
	{
		{ "*1", OPTION_INPUT,  required, NULL,    "Please input the new label", 0 },
		{ "*2", OPTION_INPUT,  required, NULL,    "Please input the volume", 0 },
		{ 0, 0, 0, 0, 0 }
	};

	label_input.result = &labelname;
	arguments[0].type = &label_input;

	volume_input.result = &volume;
	arguments[1].type = &volume_input;


	//ENTER();

	ret = AASP_REQUESTMAIN( arguments, argc, argv );
	if( ret == -1 )
	{
		RETURN(0);
	}		
	#ifdef FILE_MODULE_CONTROL_BY_BASEBAND	
	#ifdef USE_ORG_FILE_SYSTEM_IF
	ret = f_setlabel((INT8*)labelname);
	#else
	ret = fat_setlabel(volume[0],(INT8*)labelname);
	#endif
	#else
	ret = RDK_FILE_SetLabel(volume[0],labelname);
	#endif
	if (ret)
	{
		MSG(("Set label fail\n"));
		RETURN(-1);
	}
	else
	{
		MSG(("Set label OK\n"));
	}
#endif	
	return 1;
}

int fat_getlabel(int argc, char *argv[])
{
#if 0
	int ret = 0;
	UINT8 volume[1]={0};
	//UINT8 disk=0;
	UINT8 labelname[12]={0};
	#if 1
	Input volume_input = { toString, 1, NULL };

	Param arguments[] = 
	{
		{ "*1", OPTION_INPUT,  required, NULL,    "Please input the volume", 0 },
		{ 0, 0, 0, 0, 0 }
	};

	volume_input.result = &volume;
	arguments[0].type = &volume_input;

	//ENTER();

	ret = AASP_REQUESTMAIN( arguments, argc, argv );

	if( ret == -1 )
	{
		RETURN(0);
	}
	#endif
	#ifdef FILE_MODULE_CONTROL_BY_BASEBAND	
	#ifdef USE_ORG_FILE_SYSTEM_IF
	ret = f_getlabel((INT8*)labelname,12);
	#else
	ret = fat_getlabel(volume[0],(INT8*)labelname,12);
	#endif
	#else
	ret = RDK_FILE_GetLabel(volume[0],labelname);
	#endif
	if ( ret )
	{
		MSG(("getlabel fail\n"));
		RETURN(-1);
	}
	else
	{
		MSG(("labelname %s\n",labelname));
	}
#endif	
	return 1;
}

int fat_getspace(int argc, char *argv[])
{
#if 0
	int ret = 0;
	UINT8 volume[1]={0};
	//UINT8 disk=0;
	F_SPACE space;
	
	Input volume_input = { toString, 1, NULL };

	Param arguments[] = 
	{
		{ "*1", OPTION_INPUT,  required, NULL,    "Please input the volume", 0 },
		{ 0, 0, 0, 0, 0 }
	};

	volume_input.result = &volume;
	arguments[0].type = &volume_input;

	//ENTER();

	ret = AASP_REQUESTMAIN( arguments, argc, argv );

	if( ret == -1 )
	{
		RETURN(0);
	}


	#ifdef FILE_MODULE_CONTROL_BY_BASEBAND
	#ifdef USE_ORG_FILE_SYSTEM_IF
	ret = f_getfreespace(&space);
	#else
	ret = fat_getspace(volume[0],&space);
	#endif
	#else
	ret = RDK_FILE_GetSpace(volume[0],(MMD_U32*)&(space.total),(MMD_U32*)&(space.used),(MMD_U32*)&(space.free),(MMD_U32*)&(space.bad));
	#endif
	if ( ret )
	{
		MSG(("getspace fail\n"));
		RETURN(-1);
	}
	else
	{
		MSG(("getspace ok\n"));
		MSG(("maxbyte %d\n",(UINT32)space.total));
		MSG(("usedbyte %d\n",(UINT32)space.used));
		MSG(("badbyte %d\n",(UINT32)space.bad));
		MSG(("freebyte %d\n",(UINT32)space.free));
	}
#endif	
	return 1;
}

int fat_filelength(int argc, char *argv[])
{
#if 0
	int ret = 0;
	int i=0;
	//int j=0;
	//UINT8 filename[FILE_MAXNAME];
	INT8 filename[256]={0};
	//INT8 dirname[64]={0};
	//INT8 pathfile[512]={0};
//	FILE_MODE bMode=F_READ_OP;
	//UINT32  fileid;
	Input file_input = { toString, 256, NULL };
	//Input dir_input  = { toString, 64, NULL };


	Param arguments[] = 
	{
		//{ "*1", OPTION_INPUT,  required, NULL,    "Please input the position", 0 },
		{ "*1",  OPTION_INPUT , required, NULL, "Please input the filename",   0 },
		{ 0, 0, 0, 0, 0 }
	};

	file_input.result = &filename;
	//dir_input.result = &dirname;

	//arguments[0].type = &dir_input;
	arguments[0].type = &file_input;



	//ENTER();

	ret = AASP_REQUESTMAIN( arguments, argc, argv );
	if( ret == -1 )
	{
		RETURN(0);
	}
		#if 0
		while(dirname[i]!=0)
		{
			if(dirname[i]=='?'){
				pathfile[j++] = 0;
				i++;
			}
			else{
				pathfile[j++] = dirname[i++];	
			}
		
		}
		#endif
		i=0;
		while((filename[i]!=0)||(filename[i+1]!=0))
		{
			if(filename[i]=='|'){
				filename[i] = 0;				
			}
			i++;
		}
		filename[i++]=0;
		filename[i++]=0;

	MSG(("filename %s\n",filename));


	ret = fat_filelength(filename);
	if (ret)
	{
		MSG(("file length is %d \n",ret));		
	}
	else
	{		
		MSG(("filelength is 0\n"));
		RETURN(-1);
	}
#endif
	return 1;
}

int fat_truncate(int argc, char *argv[])
{
#if 0
	int ret = 0;
	//int i=0;
	//int j=0;
	//UINT8 filename[FILE_MAXNAME];
	//INT8 filename[256]={0};
	//INT8 dirname[64]={0};
	//INT8 pathfile[512]={0};
//	FILE_MODE bMode=F_READ_OP;
	UINT32  fileid;
	UINT32 truncatelen=0;
	Input file_input = { toInt, sizeof(UINT32), NULL };
	//Input dir_input  = { toString, 64, NULL };
	Input truncate_len = {toInt, sizeof(UINT32), NULL };

	Param arguments[] = 
	{
		{ "*1", OPTION_INPUT,  required, NULL,    "Please input the file id", 0 },
		{ "*2", OPTION_INPUT,  required, NULL,    "Please input the length", 0 },
		{ 0, 0, 0, 0, 0 }
	};

	file_input.result = &fileid;
	arguments[0].type = &file_input;

	truncate_len.result = &truncatelen;
	arguments[1].type = &truncate_len;

	//ENTER();

	ret = AASP_REQUESTMAIN( arguments, argc, argv );
	if( ret == -1 )
	{
		RETURN(0);
	}
	


	#ifdef FILE_MODULE_CONTROL_BY_BASEBAND
	#ifdef USE_ORG_FILE_SYSTEM_IF
	ret = f_truncate((F_FILE *)fileid,truncatelen);
	#else
	ret = fat_truncate((F_FILE *)fileid,truncatelen);
	#endif
	#else
	ret = RDK_FILE_Truncate(fileid,truncatelen);
	#endif
	if (!ret)
	{
		MSG(("truncate file OK! \n"));
		
	}
	else
	{		
		MSG(("truncate file fail\n"));
		RETURN(-1);
	}
#endif
	return 1;
}

int fat_list(int argc, char *argv[])
{
	int i=0, ret;
	unsigned char rc;
	//char listname[]={'/', 0, '*', 0, '.', 0,'*', 0, 0, 0};
	char listname[256];
	F_FIND find;
	Input name_input = {toString, 256, NULL};

	Param arguments[] = 
	{
		{ "*1", OPTION_INPUT, required, NULL, "Please input the filename", 0 },
		{ 0, 0, 0, 0, 0 }
	};

	name_input.result = &listname;
	arguments[0].type = &name_input;	

	//ENTER();

	ret = AASP_REQUESTMAIN(arguments, argc, argv);
	if(ret == -1)
	{
		return 0;
	}
	
		
	PRINTF(("\r\n Listing file matched with \"%s\"...\r\n\r\n", listname));

	AASP_GbkStr2UnicodeStr(unicode_name, (unsigned char *)listname);

	rc = f_findfirst((char *)unicode_name, &find);
	if(rc==0) 
	{
		do
		{
			if(find.attr & F_ATTR_DIR) 
			{
				PRINTF(("<DIR>	   "));
				//PRINTF(("%s",find.filename));
				AASP_PrintUnicodeStr((unsigned char *)find.filename);
				PRINTF(("\n"));				
			}
			else
			{
				PRINTF(("     	   "));
				//PRINTF(("%s",find.filename));
				AASP_PrintUnicodeStr((unsigned char *)find.filename);
				PRINTF(("\n"));
			}
		}while (!f_findnext(&find));
	}

	PRINTF(("\r\n"));
	
	return 1;
}

int fat_settime(int argc, char *argv[])
{
#if 0
	int ret = 0;
	int i=0;
	
	//UINT8 filename[FILE_MAXNAME];
	UINT8 filename[256]={0};
	#ifdef FILE_MODULE_CONTROL_BY_BASEBAND	
	UINT32 stime=0;
	UINT32 sdate=0;
	#else
	FILE_TIME file_time;
	#endif
	UINT32 sec,minute,hour,day,month,year;
	
//	FILE_MODE bMode=F_READ_OP;
	//UINT32  fileid;
	Input file_input = { toString, 256, NULL };	
//	Input stime_input = {toInt,sizeof(UINT32),NULL};
//	Input sdate_input = {toInt,sizeof(UINT32),NULL};
	Input sec_input = {toInt,sizeof(UINT32),NULL};
	Input minute_input = {toInt,sizeof(UINT32),NULL};
	Input hour_input = {toInt,sizeof(UINT32),NULL};
	Input day_input = {toInt,sizeof(UINT32),NULL};
	Input month_input = {toInt,sizeof(UINT32),NULL};
	Input year_input = {toInt,sizeof(UINT32),NULL};
	Param arguments[] = 
	{		
		{ "*1",  OPTION_INPUT , required, NULL, "Please input the filename",   0 },
		{ "*2",  OPTION_INPUT , required, NULL, "Please input the sec",   0 },
		{ "*3",  OPTION_INPUT , required, NULL, "Please input the minute",   0 },
		{ "*4",  OPTION_INPUT , required, NULL, "Please input the hour",   0 },
		{ "*5",  OPTION_INPUT , required, NULL, "Please input the day",   0 },
		{ "*6",  OPTION_INPUT , required, NULL, "Please input the month",   0 },
		{ "*7",  OPTION_INPUT , required, NULL, "Please input the year",   0 },
		{ 0, 0, 0, 0, 0 }
	};

	file_input.result = &filename;
	

	
	arguments[0].type = &file_input;

	sec_input.result = &sec;
	arguments[1].type = &sec_input;

	minute_input.result = &minute;
	arguments[2].type = &minute_input;
	hour_input.result = &hour;
	arguments[3].type = &hour_input;
	
	day_input.result = &day;
	arguments[4].type = &day_input;
	month_input.result = &month;
	arguments[5].type = &month_input;
	year_input.result = &year;
	arguments[6].type = &year_input;


	//ENTER();

	ret = AASP_REQUESTMAIN( arguments, argc, argv );
	if( ret == -1 )
	{
		RETURN(0);
	}	
	
	i=0;
	while((filename[i]!=0)||(filename[i+1]!=0))
	{
		if(filename[i]=='|'){
			filename[i] = 0;				
		}
		i++;
	}
	filename[i++]=0;
	filename[i++]=0;
	
	#ifdef FILE_MODULE_CONTROL_BY_BASEBAND
	#ifdef USE_ORG_FILE_SYSTEM_IF
	stime = (sec>>1)|(minute<<5)|(hour<<11);
	sdate = day|(month<<5)|((year-1980)<<9);
	ret = f_settimedate((INT8*)filename,(UINT16)stime,(UINT16)sdate);
	#else
	ret = fat_settimedate((INT8*)filename,(UINT16)stime,(UINT16)sdate);
	#endif
	#else
	file_time.minute = minute;
	file_time.sec = sec;
	file_time.hour = hour;
	file_time.day = day;
	file_time.month = month;
	file_time.year  = year;
	ret = RDK_FILE_SetTime(filename,&file_time);
	#endif
	if (ret)
	{
		MSG(("set file time error!return value is %d \n",ret));
		RETURN(-1);
	}
	else
	{		
		MSG(("set file time OK\n"));
	}
#endif
	return 1;
}

int fat_gettime(int argc, char *argv[])
{
#if 0
	int ret = 0;
	int i=0;
	int j=0;
	//UINT8 filename[FILE_MAXNAME];
	INT8 filename[256]={0};
	INT8 dirname[64]={0};
	INT8 pathfile[512]={0};
	UINT16 stime=0;
	UINT16 sdate=0;
	UINT16 sec,minute,hour,day,month,year;
//	FILE_MODE bMode=F_READ_OP;
	//UINT32  fileid;
	Input file_input = { toString, 256, NULL };
	Input dir_input  = { toString, 64, NULL };
	Param arguments[] = 
	{
		{ "*1", OPTION_INPUT,  required, NULL,    "Please input the position", 0 },
		{ "*2",  OPTION_INPUT , required, NULL, "Please input the filename",   0 },

		{ 0, 0, 0, 0, 0 }
	};

	file_input.result = &filename;
	dir_input.result = &dirname;

	arguments[0].type = &dir_input;
	arguments[1].type = &file_input;




	//ENTER();

	ret = AASP_REQUESTMAIN( arguments, argc, argv );
	if( ret == -1 )
	{
		RETURN(0);
	}
	
	memset(pathfile,0,sizeof(pathfile));
		#if 0
		while(dirname[i]!=0)
		{
			if(dirname[i]=='?'){
				pathfile[j++] = 0;
				i++;
			}
			else{
				pathfile[j++] = dirname[i++];	
			}
		
		}
		#endif
		i=0;
		while(filename[i]!=0)
		{
			if(filename[i]=='|'){
				pathfile[j++] = 0;
				i++;
			}
			else{
				pathfile[j++] = filename[i++];	
			}
		
		}
		pathfile[j++]=0;
		pathfile[j++]=0;

	MSG(("pathfile %s\n",pathfile));


	ret = f_gettimedate(pathfile,&stime,&sdate);
	if (ret)
	{
		MSG(("set file time error!return value is %d \n",ret));
		RETURN(-1);
	}
	else
	{		
		MSG(("get file time OK\n"));
		MSG(("file time is 0x%x\n",stime));
		MSG(("file date is 0x%x\n",sdate));
		sec = ((stime&0x1f)<<1);
		minute = ((stime&0x7e0)>>5);
		hour  = ((stime&0xf800)>>11);
		day = (sdate&0x1f);
		month = ((sdate&0x1e0)>>5);
		year = 1980+((sdate&0xfe00)>>9);
		MSG(("Time:%d:%d:%d\n",hour,minute,sec));
		MSG(("Date:%d.%d.%d\n",year,month,day));
		
	}
#endif
	return 1;
}
#endif
