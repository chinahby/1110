//	2002.11.23
//				copy from kdphoto projects

#include "stdafx.h"

int	GetChar( char ** str )
{
    int code = **((unsigned char **)str);
    if ( !str ) return 0;
    if ( !*str ) return 0;
    if ( !code ) return 0;

    (*str) ++;
    if ( code > 0x80 )
    {
        code *= 256;
        code += **((unsigned char **)str);
        (*str) ++;
    }
    return code;
};

int SetChar( char **str, int chr )
{
    int  byte = 1;

    if ( !str ) return 0;
    if ( !*str ) return 0;

    if ( chr < 256 )
    {
        **((unsigned char **)str) = chr % 256;
        (*str) ++;
    }
    else
    {
        **((unsigned char **)str) = chr / 256;
        (*str) ++;
        **((unsigned char **)str) = chr % 256;
        (*str) ++;
        byte ++;
    }

    return byte;
}

int PeekChar( char **str )
{
    int code = **((unsigned char **)str);

    if ( !str ) return 0;
    if ( !*str ) return 0;
    if ( !code ) return 0;

    if ( code > 0x80 )
    {
        code *= 256;
        code += *((unsigned char *)(*str + 1));
    }

    return code;
}

int CatChar( char *str, int cc)
{
	int len = 0;
	while( *str ) 
	{
		GetChar( &str );
		len++;
	}
	SetChar( &str, cc );
	*str = 0;
	return (++len);
}


char *whitespaces = " \t\n";

///去掉空格的函数
void trim( char *buf, char *result )
{
	char * p;
	if ( !buf || !result )
		return;
	p = strlen( buf ) + buf;
	while ( (*buf) && strchr( whitespaces, *buf) ) buf++;
	while( (p >= buf) && strchr( whitespaces, *p) ) p--;
	while ( buf <= p ) *result++ = *buf++;
	*result = 0;
}

char *next_token( char *buf, char *token, char *stopchars )
{
	if ( !stopchars )
		stopchars = whitespaces;
	int cc;
	char *p = token;

	while ( (cc = GetChar(&buf)) && (!strchr( stopchars, cc))  )
		SetChar( &token, cc);			
	SetChar( &token, 0);		
	trim( p, p );

	return buf;
}


CString GetPathName( const char * filename )
{
/*
	CString path = filename;
	char *file = path.GetBuffer(512);
	char *p = file + strlen( file );
	for ( ; *p != '\\' && *p != '/' && p > file; p-- );
	if ( *p == '\\' || *p == '/') p++;
	*p = 0;	
	path.ReleaseBuffer();
*/
	char path[MAX_PATH];
	strcpy( path, filename );
	char *p = path + strlen( path );
	for ( ; *p != '\\' && *p != '/' && p > path; p-- );
	if ( *p == '\\' || *p == '/') p++;
	*p = 0;		
	return CString(path);	
}

///获取文件扩展名
const char * GetExt( const char * filename )
{
	const char *p = strlen(filename) + filename;
	while( p > filename && *p != '.' ) p--;
	if ( *p != '.' )
		return "\0";
	else
		return p;
}

///获取文件扩展名
CString GetFileName( const char * filename, int ext = 0)
{
/*
	CString path = filename;
	char *file = path.GetBuffer(512);
	char *p = file + strlen( file );
	for ( ; *p != '\\' && *p != '/' && p > file; p-- );
	if ( *p == '\\' || *p == '/') p++;
	if ( !ext )
	{
		char *p1 = p;
		while( *p1 && *p1 != '.' ) p1++;
		*p1 = 0;	
	}
	path.ReleaseBuffer();
	return p;	
*/
	char path[MAX_PATH];
	strcpy( path, filename );
	char *p  = path + strlen(path);
	for ( ; *p != '\\' && *p != '/' && p > path; p-- );
	if ( *p == '\\' || *p == '/') p++;
	if ( !ext )
	{
		char *p1 = p;
		while( *p1 && *p1 != '.' ) p1++;
		*p1 = 0;	
	}
	return CString(p);
}

typedef  HRESULT  (_stdcall *RegFunc)(void);

BOOL	RegisterControl( const char * file, int reg )
{
	HMODULE hmod = LoadLibrary( file );
	if ( !hmod )
		return FALSE;

	RegFunc func;
	if ( reg )
		func = (RegFunc)GetProcAddress( hmod, "DllRegisterServer" );
	else
		func = (RegFunc)GetProcAddress( hmod, "DllUnregisterServer" );
	if ( func )
	{
		HRESULT hr = func();
		FreeLibrary( hmod );
		return S_OK == hr;
	}
	FreeLibrary( hmod );
	return FALSE;	
}


//0:win9x
//1:winnt
//2:win2000/winxp
//3:win32s
int GetWindowVersion()
{
	DWORD ret;
	DWORD dwVersion = GetVersion();
	
	// Get the Windows version.
	
	DWORD dwWindowsMajorVersion =  (DWORD)(LOBYTE(LOWORD(dwVersion)));
	DWORD dwWindowsMinorVersion =  (DWORD)(HIBYTE(LOWORD(dwVersion)));
	
	// Get the build number.
	DWORD dwBuild;
	if (dwVersion < 0x80000000)              // Windows NT/2000/XP
	{
		dwBuild = (DWORD)(HIWORD(dwVersion));
		if ( dwBuild == 5 )
			ret = 2;
		else
			ret = 1;
	}
	else if (dwWindowsMajorVersion < 4)      // Win32s
	{
		dwBuild = (DWORD)(HIWORD(dwVersion) & ~0x8000);
		ret = 3;
	}
	else                                     // Windows 95/98/Me
	{
		dwBuild =  0;
		ret = 0;
	}
	return ret;
}


CString GetSystemPath()
{
	CString systempath;
	::GetWindowsDirectory(systempath.GetBuffer(MAX_PATH), MAX_PATH);
	systempath.ReleaseBuffer();		
	BOOL bWin9x = ( GetWindowVersion() == 0 );
	if (systempath.Right(1) != _T("\\"))
	{
		//is win9x
		if ( bWin9x )
			systempath += _T("\\System\\");	
		else			
			systempath += _T("\\System32\\");	
	}	
	return systempath;
}


int debug_printf(char *fmt, ...);
//be careful!
void RecursiveDelete(CString szPath)
{
	CFileFind ff;
	CString path = szPath;
	
	if(path.Right(1) != "\\")
		path += "\\";
	
	path += "*.*";
	
	BOOL res = ff.FindFile(path);
	
	///debug_printf("delete dir!:%s\n", (LPCSTR)szPath);
	
	while(res)
	{
		res = ff.FindNextFile();
		if (!ff.IsDots() && !ff.IsDirectory())
		{
			DeleteFile(ff.GetFilePath());
			
		}
		else if (ff.IsDirectory() && !ff.IsDots() )
		{
			path = ff.GetFilePath();
			RecursiveDelete(path);
			RemoveDirectory(path);
		}
	}
}