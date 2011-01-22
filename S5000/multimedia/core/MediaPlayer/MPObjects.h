/*=============================================================================
  FILE: MPObjects.h

  SERVICES: this file defines base objects used in MediaPlayer
 
  GENERAL DESCRIPTION:


  PUBLIC CLASSES AND STATIC FUNCTIONS:

  INITIALIZATION AND SEQUENCING REQUIREMENTS:

  Copyright ?2005 QUALCOMM Incorporated. All Rights Reserved. 
  QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

Revision:
  $Header: //depot/asic/msmshared/apps/MediaPlayer/MPObjects.h#6 $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------

===========================================================================*/

#ifndef MPOBJECTS_H
#define MPOBJECTS_H

#include "MPColor.h"

#ifndef UINT
typedef unsigned int UINT;
#endif 

/*
#ifndef FEATURE_MEMUTILS_NEW_DELETE
void* operator new(UINT size){return MALLOC(size);}
void operator delete(void * ptr){FREE(ptr);}
void* operator new[](UINT size){return MALLOC(size);}
void operator delete[](void * ptr){FREE(ptr);}
#endif
*/

typedef struct MPPOINT
{
  int   x;
  int   y;
}MPPoint;

typedef struct MPSIZE 
{ 
  int cx; 
  int cy; 
}MPSize; 

typedef struct MPRECT {
  int left;
  int top;
  int right;
  int bottom;
}MPRect;

#ifdef __cplusplus
class CMPPoint   
{
public:
  CMPPoint();
  ~CMPPoint();
  CMPPoint(int x, int y);
  CMPPoint(MPPOINT& pt);

  void Set(int x, int y);
  int GetX() const;
  int GetY() const;

  const MPPOINT* GetPoint() const;
protected:
  MPPOINT m_pt;
};

class CMPSize
{
public:
  CMPSize(int w, int h){
    m_sz.cx = w;
    m_sz.cy = h;
  }
  void Set(int w, int h){
    m_sz.cx = w;
    m_sz.cy = h;
  }
  const MPSIZE* GetSize() const{
    return &m_sz;
  }
protected:
  MPSIZE m_sz;
};

class CMPRect 
{
public:
  CMPRect();
  CMPRect(AEERect& rc);
  CMPRect(int left, int top, int width, int height);
  ~CMPRect();

  CMPRect& operator=(const AEERect& rc);

  void Set(int left, int top, int width, int height);
  void Set(CMPRect& rc);
  boolean Get(CMPRect* rc);
  AEERect* GetRect();
  //boolean GetRect(AEERect* rc);
  boolean GetRect(int16* x, int16* y, int16* dx, int16* dy);

  void SetPos(int x, int y);
  boolean GetPos(int* x, int* y);

  void SetSize(int w, int h);
  boolean GetSize(int* w, int* h);

  boolean IsInitialize();
  static void Resize(AEERect* pRc, int x, int h);
protected:
  AEERect m_rc;
};

#endif //__cplusplus

#endif //MPOBJECTS_H
