/*=============================================================================
  FILE: MPConfig.h

  SERVICES: this file defines CMPConfig objects and functions to handle MediaPlayer 
            configuration read and write operations

  GENERAL DESCRIPTION:

    Configure file format:
     [SECTION1]
     item1=DataString
     item2=DataString
     [SECTION2]
     item1=DataString
     item2=DataString

  PUBLIC CLASSES AND STATIC FUNCTIONS:

  INITIALIZATION AND SEQUENCING REQUIREMENTS:

  Copyright @2007 QUALCOMM Incorporated. All Rights Reserved.
  QUALCOMM Proprietary/GTDR
===========================================================================*/
/*===========================================================================
                        EDIT HISTORY FOR MODULE

Revision:
  $Header: //depot/asic/msmshared/apps/MediaPlayer/MPConfig.h#3 $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------

===========================================================================*/

#ifndef MPCONFIGF_H
#define MPCONFIGF_H

#include "AEEFile.h"
#include "AEESource.h"

#ifdef __cplusplus

class CStringReader
{
public:
  enum DEFAULT_STR_LENGTH
  {
    MPCONFIGF_DEFAULT_STRING_LEN = 128
  };

  CStringReader();
  virtual ~CStringReader();

  int   Init(IShell *pIShell, IFile* pIFile);
  //read from configure file
  //return NULL if request does not exist
  const char* Read(const char* pSection, const char* pItem);

protected:
  // point to owner objects
  IFile     *m_pIFile; 
  IShell    *m_pIShell;

  // shared string buffer
  char       m_strBuf[MPCONFIGF_DEFAULT_STRING_LEN + 1];
  char      *m_pstrBuf;
  int        m_nBufSize;
  boolean    m_bInUse;

  //read a line from file
  int   ReadLine(IGetLine *pIGetLine, GetLine *pline);
  //return TRUE if string contain "[" + pszSection + "]"
  int   HasSectionString(const char* pstr, const char* pszSection);
  //if string is "item" + "=" + datastring, return datastring
  char* GetItemDataString(const char* pstr, const char* pszItem);
  //remove leading and trailing spaces for a given string
  int   RemoveLeadingTrailingSpace(char* pszString);

  // to prevent constantly allocate and delete the memory, GetBuffer can be 
  // used to retrieve a shared buffer pointer. the function set a lock if ptr
  // returned usefully
  char* GetBuffer(int nSize);
  // this function release the lock on shared buffer
  void  ReleaseBuffer();
};

class CStringWriter
{
public:
  CStringWriter();
  virtual ~CStringWriter();
  
  int Init(IShell *pIShell, IFile* pIFile);
  //write a line
  int Write(const char* pSection, const char* pItem, const char* pData);

protected:
  // point to owner objects
  IFile     *m_pIFile;
  IShell    *m_pIShell;

  char*      m_pszSection; // section name

  uint32 WriteLine(IFile* pFile, const char* pSting);
  int    WriteItemString(IFile* pFile, const char* pItemSting, const char* pDataString);
  int    WriteSectionString(const char* pDataString);

};

class CMPConfig
{
public:
  CMPConfig();
  virtual ~CMPConfig();

  int Init(IShell* pIShell, const char* pFile);
  int WriteString(const char* pSection, const char* pItem, const char* pData);
  const char* ReadString(const char* pSection, const char* pItem);

  uint32 Seek(FileSeekType seekType, int32 moveDistance);
protected:
  CStringReader        m_Reader;
  CStringWriter        m_Writer;
  IShell*              m_pIShell;
  IFileMgr*            m_pIFileMgr;
  IFile*               m_pIFile;
  char*                m_pstrFile; // config file name

  void Release();
};


#else
  typedef struct CMPConfig CMPConfig;
#endif


#ifdef __cplusplus
extern "C" {
#endif

CMPConfig* MPConfigMake(IShell* pIShell, const char* pFile);
void MPConfigFree(CMPConfig** pConfig);
#define MPCONFIG_FREEIF(p) MPConfigFree((CMPConfig **)&(p))
uint32  MPConfigSeek(CMPConfig* pConfig, FileSeekType seekType, int32 moveDistance);


//write date string into config file
// limitation:  items in same section have to be written together, 
//              otherwise, writer will create multiple sections with same name
int MPConfigWriteString(CMPConfig* pConfig, const char* pSection, const char* pItem, const char* pData);
int MPConfigWriteBoolean(CMPConfig* pConfig, const char* pSection, const char* pItem, boolean value);
int MPConfigWriteInt(CMPConfig* pConfig, const char* pSection, const char* pItem, int32 value);

// read data string from config file by given section and item name
const char* MPConfigReadString(CMPConfig* pConfig, const char* pSection, const char* pItem);
boolean MPConfigReadBoolean(CMPConfig* pConfig, const char* pSection, const char* pItem, boolean defaultValue);
int MPConfigReadInt(CMPConfig* pConfig, const char* pSection, const char* pItem, int32 defaultValue);

#ifdef __cplusplus
}
#endif

#endif //MPCONFIGF_H
