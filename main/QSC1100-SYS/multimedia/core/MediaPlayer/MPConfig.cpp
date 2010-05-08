/*=============================================================================
  FILE: MPConfig.c

  SERVICES:class obbject Implementations

  GENERAL DESCRIPTION:

  PUBLIC CLASSES AND STATIC FUNCTIONS:

  INITIALIZATION AND SEQUENCING REQUIREMENTS:

  Copyright @2007 QUALCOMM Incorporated. All Rights Reserved.
  QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

Revision:
  $Header: //depot/asic/msmshared/apps/MediaPlayer/MPConfig.cpp#4 $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------

===========================================================================*/

#include "MPConfig.h"
#include "AEEStdLib.h"
#include "AEEShell.h"
#include "MediaPlayer.h"

#define MPCONFIGF_SECTION_SYMBOL_LEFT     "["
#define MPCONFIGF_SECTION_SYMBOL_RIGHT    "]"
#define MPCONFIGF_SYMBOL_EQUAL            "="
#define MPCONFIGF_NEWLINE_CHAR            "\r\n"

#define MPCONFIGF_TRUE                    "TRUE"
#define MPCONFIGF_FALSE                   "FALSE"


//------------------------------------------------------------------------------
// CStringReader implementation
//------------------------------------------------------------------------------
CStringReader::CStringReader():
  m_pIFile(NULL), m_pIShell(NULL), m_pstrBuf(NULL), 
  m_nBufSize(0), m_bInUse(FALSE)
{
}

CStringReader::~CStringReader()
{
  if(m_pstrBuf)
  {
    delete [] m_pstrBuf;
  }
}

int CStringReader::Init(IShell *pIShell, IFile* pIFile)
{
  if(pIShell && pIFile)
  {
    m_pIShell = pIShell;
    m_pIFile  = pIFile;
    m_bInUse = FALSE;

    if(m_pstrBuf)
    {
      delete [] m_pstrBuf;
      m_pstrBuf = NULL;
    }
    m_nBufSize = 0;

    return SUCCESS;
  }

  return EFAILED;
}

// This function reads a data string from the currently opened ini file
//   pSection, section name string
//   pItem, item name string
// the return is the const pointer to data string, NULL if the requested item does not exist
const char* CStringReader::Read(const char* pSection, const char* pItem)
{
  ISourceUtil *pISourceUtil  = NULL;
  ISource     *pISource  = NULL;
  IGetLine    *pIGetLine = NULL;
  GetLine      line;
  char*        pBuf = NULL;
  boolean      bSection = FALSE;

  if(NULL == pItem)
  {
    DBGPRINTF_ERROR("MP: Invalid parm, pItem is NULL.");
    return NULL;
  }
  if(NULL == m_pIFile)
  {
    DBGPRINTF_ERROR("MP: m_pIFile is NULL.");
  }
  if(NULL == m_pIShell)
  {
    DBGPRINTF_ERROR("MP: m_pIShell is NULL.");
  }

  if(m_pIFile)
  {
    if(SUCCESS != ISHELL_CreateInstance(m_pIShell, AEECLSID_SOURCEUTIL,(void **)&pISourceUtil))
    {
      DBGPRINTF_ERROR("MP: create AEECLSID_SOURCEUTIL instance failed.");
      pISourceUtil = NULL;
      goto DONE;
    }

    if(SUCCESS != ISOURCEUTIL_SourceFromAStream(pISourceUtil,(IAStream*)m_pIFile, &pISource))
    {
      DBGPRINTF_ERROR("MP: ISOURCEUTIL_SourceFromAStream failed.");
      pISource = NULL;
      goto DONE;
    }

    if(SUCCESS != ISOURCEUTIL_GetLineFromSource(pISourceUtil, pISource,5000, &pIGetLine))
    {
      DBGPRINTF_ERROR("MP: ISOURCEUTIL_GetLineFromSource failed.");
      pIGetLine = NULL;
      goto DONE;
    }
    
    while(TRUE)
    {
      bSection = FALSE;
      // read a line
      if(SUCCESS != ReadLine(pIGetLine, &line))
      {
        // end
        break;
      }

      if(SUCCESS == HasSectionString(line.psz, pSection))
      {
        // contains section name
        bSection = TRUE;
      }

      if(TRUE == bSection)
      {
        // found section, read all lines under this section
        while(TRUE)
        {
          // read next line
          if(SUCCESS != ReadLine(pIGetLine, &line))
          {
            // end
            goto DONE;
          }

          if(NULL != (pBuf = GetItemDataString(line.psz, pItem)))
          {
            // found what we are looking for
            goto DONE;
          }

          if(SUCCESS == HasSectionString(line.psz, pSection))
          {
            // found next section name string.
            // we should return NULL
            goto DONE;

          }
        }
      }
    }
  }
  else
  {
    DBGPRINTF_ERROR("MP: m_pIFile is NULL");
  }


DONE:
  if (NULL != pIGetLine)
    IGETLINE_Release(pIGetLine);
  if (NULL != pISource)
    ISOURCE_Release(pISource);
  if (NULL != pISourceUtil)
    ISOURCEUTIL_Release(pISourceUtil);

  return pBuf;
}

// this function reads a line from ini file
int CStringReader::ReadLine(IGetLine *pIGetLine, GetLine *pline)
{
  int nRet = EFAILED;
  int n;

  n = IGETLINE_GetLine(pIGetLine, pline, IGETLINE_LF);
  if(TRUE == IGETLINE_Exhausted(n) )
  {
    //source is exhausted (no more data).
    return nRet;
  }

  if(FALSE == IGETLINE_LineComplete(n) )
  {
    DBGPRINTF_ERROR("MP: failed to read complete line.");
    return nRet;
  }

  nRet = SUCCESS;

  return nRet;
}

// return TRUE if the pstr satisfies the following
//  string starts with [
//  string ends with ]
//  pstr contains only "[" + pszSection + "]"
//  note, space is allowed

int CStringReader::HasSectionString(const char* pstr, const char* pszSection)
{
  int   nRet = EFAILED;
  int   nStr = 0;
  char *ptmp = (char*)pstr;
  char *p0   = (char*)pstr;
  char *p1;
  int   i;

  if(NULL == pstr || NULL == pszSection)
  {
    DBGPRINTF_ERROR("MP: invalid input, pstr or pszSection is NULL.");
    nRet = EBADPARM;
    return nRet;
  }

  nStr = STRLEN(pstr);
  for(i=0; i < nStr; i++)
  {
    // remove leading space
    if(ptmp[i] != 0x20)
    {
      break;
    }
  }

  if(i == nStr || ptmp[i] != '[')
  {
    // pstr does not start with '['
    nRet = EFAILED;
    return nRet;
  }

  p0 = &ptmp[i];
  ptmp = STRSTR(p0, pszSection);
  if(NULL != ptmp)
  {
    p1 = ptmp;
    i = (ptmp - p0);
    ptmp = p0;
    if(i>0)
    {
      i--;
      // string contains section name
      for(; i >=0; i--)
      {
        // remove space
        if(ptmp[i] != 0x20)
        {
          break;
        }
      }
    }

    if(ptmp[i] != '[')
    {
      nRet = EFAILED;
      return nRet;
    }

    i = (p1 - p0) + STRLEN(pszSection) - 1;
    p1 = p0;
    nStr = STRLEN(p1);
    while(TRUE)
    {
      i++;
      // remove trailing space
      if(i >= (nStr - 1) || p1[i] != 0x20)
      {
        break;
      }
    }

    if(p1[i] != ']')
    {
      nRet = EFAILED;
      return nRet;
    }

    nRet = SUCCESS;
  }
  else
  {
    // does not contain requested section name
    nRet = EFAILED;
  }

  return nRet;
}
// this function retrieve the datastring
char* CStringReader::GetItemDataString(const char* pstr, const char* pszItem)
{
  int n;
  int nLen;
  const char* p0   = pstr;
  const char* p1   = pstr;
  char*       pBuf = NULL;

  if(NULL == pstr )
  {
    DBGPRINTF_ERROR("MP: invalid input, pstr is NULL.");
    return NULL;
  }

  nLen = STRLEN(p0);
  p1 = STRSTR(p0, MPCONFIGF_SYMBOL_EQUAL);
  if(p1)
  {
    n = p1 - p0; // length of item string
    if(n)
    {
      pBuf = GetBuffer(n + 1);
      if(pBuf)
      {
        STRLCPY(pBuf, p0, n + 1); //plus end of sring
        (void)RemoveLeadingTrailingSpace(pBuf);
        // compare item string
        if(0 == STRCMP(pBuf, pszItem))
        {
          p1++;//move over '='
          ReleaseBuffer();
          pBuf = GetBuffer(nLen-n+1);
          STRLCPY(pBuf, p1, nLen-n);
          ReleaseBuffer();
        }
        else
        {
          pBuf = NULL;
          ReleaseBuffer();
        }
      }
    }
  }

  return pBuf;
}

// To avoid constantly allocating and deleting the memory, GetBuffer can be 
// used to retrieve a shared buffer pointer. The function sets a lock if ptr
// is returned successfully.
// If the buffer is in use (locked), returns NULL.
// Note, ReleaseBuffer needs to be called to release the lock.
char* CStringReader::GetBuffer(int nSize)
{
  char* pBuf = NULL;

  if(!nSize)
  {
    DBGPRINTF_ERROR("MP: requesting 0 size buffer.");
    return NULL;
  }

  if(m_bInUse)
  {
    DBGPRINTF_ERROR("MP: buffer is in use.");
    return NULL;
  }

  m_bInUse = TRUE;
  if(NULL != m_pstrBuf)
  {
    if(nSize < MPCONFIGF_DEFAULT_STRING_LEN)
    {
      // we have allocated buffer larger than default size, set it back now
      delete [] m_pstrBuf;
      m_pstrBuf = NULL;

      m_nBufSize = MPCONFIGF_DEFAULT_STRING_LEN;
      pBuf = m_strBuf;
    }
    else
    {
      if(nSize > m_nBufSize)
      {
        delete [] m_pstrBuf;
        m_pstrBuf = new char[nSize + 1];
        if(m_pstrBuf)
        {
          m_nBufSize = nSize;
          pBuf = m_pstrBuf;
        }
        else
        {
          m_nBufSize = 0;
        }
      }
      else
      {
        pBuf = m_pstrBuf;
      }
    }
  }
  else
  {
    if(nSize < MPCONFIGF_DEFAULT_STRING_LEN)
    {
      m_nBufSize = MPCONFIGF_DEFAULT_STRING_LEN;
      pBuf = m_strBuf;
    }
    else
    {
      delete [] m_pstrBuf;
      m_pstrBuf = new char[nSize + 1];
      if(m_pstrBuf)
      {
        pBuf = m_pstrBuf;
        m_nBufSize = MPCONFIGF_DEFAULT_STRING_LEN;
      }
      else
      {
        m_nBufSize = 0;
      }
    }
  }

  if(NULL == pBuf)
  {
    m_bInUse = FALSE;
  }
  return pBuf;
}

// this function sets flag to release internal buffer
void CStringReader::ReleaseBuffer()
{
  if(m_bInUse)
  {
    m_bInUse = FALSE;
  }
}

int CStringReader::RemoveLeadingTrailingSpace(char* pszString)
{
  int   nStr;
  char* ptmp = (char*)pszString;
  int   i = 0;
  int   n = 0;

  nStr = STRLEN(ptmp);
  for(i=0; i < nStr; i++)
  {
    if(ptmp[i] != 0x20)
    {
      break;
    }
  }

  // if no leading spaces, above code will break out with i eq to 0
  if(i > 0)                
  {
    n = i;
    //remove leading space
    for(i=0; i < nStr; i++)
    {
      ptmp[i] = ptmp[i + n];
    }
    ptmp[nStr - n] = '\0';
  }

  for(i = nStr - n - 1; i >=0; i--)
  {
    if(ptmp[i] != 0x20)
    {
      break;
    }
  }

  ptmp[i + 1] = '\0';

  return SUCCESS;
}


//------------------------------------------------------------------------------
// CStringWriter implementation
//------------------------------------------------------------------------------
CStringWriter::CStringWriter():m_pIFile(NULL), m_pIShell(NULL),
  m_pszSection(NULL)
{
}

CStringWriter::~CStringWriter()
{
  if(m_pszSection)
  {
    delete [] m_pszSection;
  }
}
int CStringWriter::Init(IShell *pIShell, IFile* pIFile)
{
  if(pIShell && pIFile)
  {
    m_pIShell = pIShell;
    m_pIFile  = pIFile;

    if(m_pszSection)
    {
      delete [] m_pszSection;
      m_pszSection = NULL;
    }

    return SUCCESS;
  }
  return EFAILED;
}

int CStringWriter::Write(const char* pSection, const char* pItem, const char* pData)
{
  int nRet = EBADPARM;

  if(NULL == pItem)
  {
    DBGPRINTF_ERROR("MP: Invalid parm, pItem is NULL.");
    return NULL;
  }

  if(NULL == m_pIFile)
  {
    DBGPRINTF_ERROR("MP: m_pIFile is NULL.");
  }

  if(m_pIFile)
  {
    // create new section only if it does not exist or if it is different than previous
    if(NULL == m_pszSection || 0 != STRCMP(m_pszSection, pSection))
    {
      int len;
      // new section
      len = STRLEN(pSection) + 1;
      if(0 == len)
      {
        DBGPRINTF_ERROR("MP: empty section string.");
        return nRet;
      }

      //write new section string
      if(SUCCESS != (nRet = WriteSectionString(pSection)))
      {
        DBGPRINTF_ERROR("MP: failed to write config section string.");
        return nRet;
      }

      // save section string
      if(m_pszSection)
      {
        delete [] m_pszSection;
      }

      m_pszSection = new char[len];
      if(m_pszSection)
      {
        STRLCPY(m_pszSection, pSection, len);
      }
      else
      {
        DBGPRINTF_ERROR("MP: failed to allocate memory.");
        nRet = EFAILED;
        return nRet;
      }
    }

    // write item string
    if(SUCCESS != (nRet = WriteItemString(m_pIFile, pItem, pData)))
    {
      DBGPRINTF_ERROR("MP: failed to write config item string.");
      return nRet;
    }
  }
  else
  {
    DBGPRINTF_ERROR("MP: open file error.");
    nRet = EFAILED;
  }

  return nRet;
}

// this function writes string to the pFile
// it returns number of chars
uint32 CStringWriter::WriteLine(IFile* pFile, const char* pSting)
{
  uint32 dwWriteNum = 0;
  uint32 dwCount = 0;

  if (NULL == pFile || NULL == pSting)
  {
    DBGPRINTF_ERROR("MP: Invalid parm.");
    return 0;
  }

  dwCount = STRLEN(pSting);
  if (0 == dwCount)
  {
    DBGPRINTF_ERROR("MP: empty string.");
    return 0;
  }

  dwWriteNum = IFILE_Write(pFile, pSting, dwCount);
  if(dwWriteNum != dwCount)
  {
    DBGPRINTF_ERROR("MP: write file failed, %d bytes written", dwWriteNum);
  }
  return dwWriteNum;
}

int CStringWriter::WriteSectionString(const char* pDataString)
{
  int nRet = SUCCESS;

  if(NULL == m_pIFile)
  {
    DBGPRINTF_ERROR("MP: m_pIFile is NULL.");
    nRet = EFAILED;
    return nRet;
  }

  if(0 == WriteLine(m_pIFile, MPCONFIGF_SECTION_SYMBOL_LEFT))
  {
    DBGPRINTF_ERROR("MP: failed to write file, %s.", MPCONFIGF_SECTION_SYMBOL_LEFT);
    nRet = EFAILED;
    return nRet;
  }
  if(0 == WriteLine(m_pIFile, pDataString))
  {
    DBGPRINTF_ERROR("MP: failed to write file, section = %s.", pDataString);
    nRet = EFAILED;
    return nRet;
  }
  if(0 == WriteLine(m_pIFile, MPCONFIGF_SECTION_SYMBOL_RIGHT))
  {
    DBGPRINTF_ERROR("MP: failed to write file, %s.", MPCONFIGF_SECTION_SYMBOL_RIGHT);
    nRet = EFAILED;
    return nRet;
  }
  if(0 == WriteLine(m_pIFile, MPCONFIGF_NEWLINE_CHAR))
  {
    DBGPRINTF_ERROR("MP: failed to write file, end char.");
    nRet = EFAILED;
    return nRet;
  }

  return nRet;
}

int CStringWriter::WriteItemString(IFile* pFile, const char* pItemSting, const char* pDataString)
{
  int nRet = SUCCESS;

  if(NULL == pFile)
  {
    DBGPRINTF_ERROR("MP: Invalid parm.");
    nRet = EFAILED;
    return nRet;
  }

  if(0 == WriteLine(pFile, pItemSting))
  {
    DBGPRINTF_ERROR("MP: failed to write file, pItemSting = %s.", pItemSting);
    nRet = EFAILED;
    return nRet;
  }
  if(0 == WriteLine(pFile, MPCONFIGF_SYMBOL_EQUAL))
  {
    DBGPRINTF_ERROR("MP: failed to write file, %s.", MPCONFIGF_SYMBOL_EQUAL);
    nRet = EFAILED;
    return nRet;
  }
  if(0 == WriteLine(pFile, pDataString))
  {
    DBGPRINTF_ERROR("MP: failed to write file, data string = %s.", pDataString);
    nRet = EFAILED;
    return nRet;
  }
  if(0 == WriteLine(pFile, MPCONFIGF_NEWLINE_CHAR))
  {
    DBGPRINTF_ERROR("MP: failed to write file, end char.");
    nRet = EFAILED;
    return nRet;
  }

  return nRet;
}


//------------------------------------------------------------------------------
// CMPConfig implementation
//------------------------------------------------------------------------------
CMPConfig::CMPConfig():
  m_pIShell(NULL), m_pIFileMgr(NULL), m_pIFile(NULL), m_pstrFile(NULL)
{
}

CMPConfig::~CMPConfig()
{
  Release();
}

void CMPConfig::Release()
{
  // Release the buffered I/O cache
  if(NULL != m_pIFile)
  {
      (void) IFILE_SetCacheSize(m_pIFile, 0);
  }

  if(NULL != m_pIFileMgr)
  {
    (void)IFILEMGR_Release(m_pIFileMgr);
    m_pIFileMgr = NULL;
  }

  if(NULL != m_pIFile)
  {
    (void)IFILE_Release(m_pIFile);
    m_pIFile = NULL;
  }

  if(m_pstrFile)
  {
    delete [] m_pstrFile;
    m_pstrFile = NULL;
  }
}

int CMPConfig::Init(IShell* pIShell, const char* pFile)
{
  Release();
  m_pIShell = pIShell;

  if(SUCCESS != ISHELL_CreateInstance(pIShell, AEECLSID_FILEMGR, (void **)&m_pIFileMgr))
  {
    DBGPRINTF_ERROR("MP: failed to create CMPConfig file manager.");
    return EFAILED;
  }

  m_pstrFile = new char[STRLEN(pFile) + 1];
  if(NULL == m_pstrFile)
  {
    DBGPRINTF_ERROR("MP: failed to allocate memory.");
    Release();
    return EFAILED;
  }
  (void)STRLCPY(m_pstrFile, pFile, STRLEN(pFile)+1);

  if(NULL == m_pIFile)
  {
    if(SUCCESS == IFILEMGR_Test(m_pIFileMgr, m_pstrFile))
    {
      // Config file exists, open it for appending.
      // Since we only support appending, this is fine. We need to make it read/write mode in the future
      m_pIFile = IFILEMGR_OpenFile(m_pIFileMgr, m_pstrFile, _OFM_APPEND);
    }
    else
    {
      // config file does not exist, create one
      m_pIFile = IFILEMGR_OpenFile(m_pIFileMgr, m_pstrFile, _OFM_CREATE);
    }

    if(NULL == m_pIFile)
    {
      DBGPRINTF_ERROR("MP: failed to open file, %s.", m_pstrFile);
      Release();
      return EFAILED;
    }
  }
  
  // Set the I/O type to buffered I/O
  if (0 >= IFILE_SetCacheSize(m_pIFile, SCS_DEFAULT)) 
  {
      DBGPRINTF_ERROR("MP: failed to set cache size for %s.", m_pstrFile);
      Release();
      return EFAILED;
  }

  (void)m_Reader.Init(pIShell, m_pIFile);
  (void)m_Writer.Init(pIShell, m_pIFile);

  return SUCCESS;
}

int CMPConfig::WriteString(const char* pSection, const char* pItem, const char* pData)
{
  int nRet = EBADPARM;

  if(NULL == pItem || NULL == pData)
  {
    DBGPRINTF_ERROR("MP: Invalid parm.");
    return nRet;
  }

  if(NULL == m_pstrFile)
  {
    DBGPRINTF_ERROR("MP: NULL config file name.");
    return EFAILED;
  }

  if(NULL == m_pIFile)
  {
    if(SUCCESS == IFILEMGR_Test(m_pIFileMgr, m_pstrFile))
    {
      // Config file exists, open for appending
      // since we only support appending, this is fine. we need to make it read/write mode in the future
      m_pIFile = IFILEMGR_OpenFile(m_pIFileMgr, m_pstrFile, _OFM_APPEND);
    }
    else
    {
      // config file does not exist, create one
      m_pIFile = IFILEMGR_OpenFile(m_pIFileMgr, m_pstrFile, _OFM_CREATE);
    }

    if(NULL == m_pIFile)
    {
      DBGPRINTF_ERROR("MP: failed to open file, %s.", m_pstrFile);
      return EFAILED;
    }
  }

  nRet = m_Writer.Write(pSection, pItem, pData);

  return nRet;
}
const char* CMPConfig::ReadString(const char* pSection, const char* pItem)
{
  const char* pstr = NULL;

  if(NULL == m_pIFile)
  {
    m_pIFile = IFILEMGR_OpenFile(m_pIFileMgr, m_pstrFile, _OFM_READ);
    if(NULL == m_pIFile)
    {
      DBGPRINTF_ERROR("MP: failed to open file, %s.", m_pstrFile);
      return pstr;
    }
  }
  else
  {
    // seek to the begining of the file
    (void)Seek(_SEEK_START, 0);
  }

  pstr = m_Reader.Read(pSection, pItem);

  return pstr;
}

uint32 CMPConfig::Seek(FileSeekType seekType, int32 moveDistance)
{
  if(NULL == m_pIFile)
  {
    DBGPRINTF_ERROR("MP: m_pIFile is NULL.");
    return EFAILED;
  }
  
  return IFILE_Seek(m_pIFile, seekType, moveDistance);
}

//------------------------------------------------------------------------------------------
CMPConfig* MPConfigMake(IShell* pIShell, const char* pFile)
{
  CMPConfig* pConfig = NULL;

  if(NULL == pIShell || NULL == pFile)
  {
    DBGPRINTF_ERROR("MP: Invalid parm.");
    return pConfig;
  }

  pConfig = new CMPConfig;  
  if(NULL != pConfig)
  {
    if(SUCCESS != pConfig->Init(pIShell, pFile))
    {
      delete pConfig;
      pConfig = NULL;
      DBGPRINTF_ERROR("MP: failed to initialize CMPConfig.");
    }
  }
  else
  {
    DBGPRINTF_ERROR("MP: failed to create CMPConfig.");
  }

  return pConfig;
}

void MPConfigFree(CMPConfig** ppConfig)
{
  if(ppConfig && *ppConfig)
  {
    delete (*ppConfig);
    *ppConfig = NULL;
  }
}

uint32  MPConfigSeek(CMPConfig* pConfig, FileSeekType seekType, int32 moveDistance)
{
  if(NULL == pConfig)
  {
    DBGPRINTF_ERROR("MP: Invalid parm.");
    return EBADPARM;
  }
  return pConfig->Seek(seekType, moveDistance);
}

int MPConfigWriteString(CMPConfig* pConfig, const char* pSection, const char* pItem, const char* pData)
{
  int nRet = EBADPARM;

  if(NULL == pConfig || NULL == pItem || NULL == pData)
  {
    DBGPRINTF_ERROR("MP: Invalid parm.");
    return nRet;
  }

  nRet = pConfig->WriteString(pSection, pItem, pData);

  return nRet;
}


int MPConfigWriteBoolean(CMPConfig* pConfig, const char* pSection, const char* pItem, boolean value)
{
  return MPConfigWriteString(pConfig, pSection, pItem, value ? (const char*)MPCONFIGF_TRUE : (const char*)MPCONFIGF_FALSE);
}

int MPConfigWriteInt(CMPConfig* pConfig, const char* pSection, const char* pItem, int32 value)
{
  char pbuf[25];

  (void)MEMSET(pbuf, 0, 25);
  (void)SNPRINTF(pbuf, 24, "%d", value);
  return MPConfigWriteString(pConfig, pSection, pItem, pbuf);
}

boolean MPConfigReadBoolean(CMPConfig* pConfig, const char* pSection, const char* pItem, boolean defaultValue)
{
  boolean data = defaultValue;
  const char *value = NULL;

  if (NULL == pSection || NULL == pItem)
  {
    DBGPRINTF_ERROR("MP: Invalid parm.");
    return data;
  }

  value = MPConfigReadString(pConfig, pSection, pItem);
  if (NULL == value)
  {
    data = defaultValue;
  }
  else
  {
    if (0 == STRICMP(value, MPCONFIGF_TRUE))
    {
      data = TRUE;
    }
    else
    {
      data = FALSE;
    }
  }

  return data;
}

int MPConfigReadInt(CMPConfig* pConfig, const char* pSection, const char* pItem, int32 defaultValue)
{
  int data = defaultValue;
  const char *value = NULL;

  if (NULL == pSection || NULL == pItem)
  {
    return data;
  }

  value = MPConfigReadString(pConfig, pSection, pItem);
  if (NULL == value)
  {
    data = defaultValue;
  }
  else
  {
    data = ATOI(value);
  }

  return data;
}

const char* MPConfigReadString(CMPConfig* pConfig, const char* pSection, const char* pItem)
{
  if(pConfig)
  {
    return pConfig->ReadString(pSection, pItem);
  }
  else
  {
    return NULL;
  }
}
