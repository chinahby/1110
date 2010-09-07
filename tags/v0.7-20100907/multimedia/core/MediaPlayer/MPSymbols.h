/*=============================================================================
  FILE: MPSymbols.h

  SERVICES: this file defines symbol base objects
 
  GENERAL DESCRIPTION:


  PUBLIC CLASSES AND STATIC FUNCTIONS:

  INITIALIZATION AND SEQUENCING REQUIREMENTS:

  Copyright ?2005 QUALCOMM Incorporated. All Rights Reserved. 
  QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

Revision:
  $Header: //depot/asic/msmshared/apps/MediaPlayer/MPSymbols.h#13 $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------

===========================================================================*/

#ifndef MPSYMBOLS_H
#define MPSYMBOLS_H

#include "AEEStdLib.h"
#include "AEEGraphics.h"
#include "AEEText.h"
#include "MPObjects.h"
#include "MPDefines.h"

#ifdef FEATURE_MP_EQ_UI
#include "MPEqualizerData.h"
#endif

#include "MPSettings.h"
#ifdef __cplusplus

// return AECHAR string in an allocated memory  
AECHAR* MP_ToAEEChar(const char* pName);
// draw frame into given graphic context
boolean MP_DrawFrame(IGraphics* pg, AEERect* pRc, AEEFrameType type, RGBVAL clr);

/* ------------------------------------------------------------------------
  CMPSymbol Class, a base class for all MP symbol objects
 ------------------------------------------------------------------------ */
class CMPSymbol 
{
public:
  CMPSymbol();
  CMPSymbol(void *pOwner);
  CMPSymbol(void *pOwner, CMPRect& rc);
  virtual ~CMPSymbol();

  virtual boolean Draw(IGraphics* ){return FALSE;};
  virtual boolean OnEvent(AEEEvent /*eCode*/, uint16 /*wParam8*/, uint32 /*dwParam*/){return FALSE;}

  // set/get item to have input focus
  virtual void SetFocus(boolean bMode);
  virtual boolean GetFocus();

  // set/get item to have selection (or high light)
  virtual void SetSelection(boolean bMode);
  virtual boolean GetSelection();

  // set and get frame rectangle
  virtual void SetFrameRect(CMPRect& rc);
  virtual boolean GetFrameRect(CMPRect* rc);
  virtual void SetFrameRect(int x, int y, int dx, int dy);
  virtual void GetFrameRect(int* x, int* y, int* dx, int* dy);  
  
  // set/get center pos
  void SetPos(int x, int y);
  boolean GetPos(int* x, int* y);
  // set/get size
  void SetSize(int w, int h);
  boolean GetSize(int* w, int* h);

  // get colors
  CMPColor* GetBkColor();   // background color
  CMPColor* GetFgColor();   // foreground color
  CMPColor* GetFlColor();   // fill color
  CMPColor* GetSdColor();   // shadow color

  // set/get id
  void  SetID(int16 id);
  int16 GetID();
	
  void SetIndentLevel(int16 level); // object indent level
  int16 GetIndentLevel();

  virtual void  SetOwnerPtr(void* pOwner);
  virtual void* GetOwnerPtr();

  void  SetData(int32 data);
  int32 GetData();
protected:
  void*  m_pOwner;    // stores owner pointer
  int16  m_nID;       // stores id
  int32  m_data;      // can be used to save anything
  int16  m_byIntLevel; // indent level

  CMPRect m_FrameRect;// frame rect

  CMPColor m_bkClr;   // background/fill color
  CMPColor m_fgClr;   // drawing color
  CMPColor m_fillClr; // fill color
  CMPColor m_sdClr;   // shadow color

  boolean  m_Focus;   // has focus
  boolean  m_bSelect; // been selected
};

/* ------------------------------------------------------------------------
  CMPPanel Class, can be used as symbol object holder
  such as slider bars, buttons and labels
 ------------------------------------------------------------------------ */
class CMPPanel : public CMPSymbol 
{
public:
  CMPPanel(void* pOwner) : CMPSymbol(pOwner){}
  CMPPanel(){}
  virtual ~CMPPanel(){}
  
  virtual boolean Init(AEERect /*rc*/) {return FALSE;}
  virtual boolean Init(int /*x*/, int /*y*/, int /*dx*/, int /*dy*/) {return FALSE;}

  // set focus to an item
  virtual void    SetItem(int /*idx*/){}
  // return item id which has focus
  virtual int     GetItem(){return -1;}
  // set focus to next item
  virtual boolean SetNext(){return FALSE;}
  // set focus to previous item
  virtual boolean SetPrevious(){return FALSE;}
  // set selection on an item
  virtual void    SetSelectItem(int /*idx*/){}
  // return item id which has selection
  virtual int     GetSelectItem(){return -1;}
  // return item count
  virtual int     GetItemCount(){return -1;}
};

/* ------------------------------------------------------------------------
  CMPLabel Class, show text
 ------------------------------------------------------------------------ */
class CMPLabel : public CMPSymbol
{
public:
  CMPLabel();
  CMPLabel(void* pOwner);
  virtual ~CMPLabel();

  virtual boolean Draw(IGraphics *pg);

  void      Enable(boolean bEnable);
  boolean   Enable();
  
  // name should be allocated and deleted by user
  void      SetName(AECHAR* name);
  AECHAR*   GetName();
  // label text color
  CMPColor* GetTextColor();
  void      SetAllignText(int nAlignMode);
protected:
  AECHAR*      m_strName;   // store label name string ptr
  boolean      m_bEnable;   // label is seen only when it is enabled
  CMPColor     m_TextClr;   // text color
  int          m_nAlignMode;
};

/* ------------------------------------------------------------------------
  CMPTextPanel Class, display multline text in panel
 ------------------------------------------------------------------------ */
class CMPTextPanel : public CMPSymbol
{
public:
  CMPTextPanel();
  CMPTextPanel(void* pOwner);
  virtual ~CMPTextPanel();

  virtual boolean Draw(IGraphics *pg);
  
  void SetRect(int x, int y, int dx, int dy);
  boolean   SetText(const char* pstrTitle, const char* pstrText, 
                    AEEFont titleFnt = AEE_FONT_NORMAL | AEE_FONT_BOLD, 
                    AEEFont textFnt = AEE_FONT_NORMAL);

  void      SetTextColor(uint8 r, uint8 g, uint8 b);
  CMPColor* GetTextColor();

protected:
  IStatic     *m_pIText;
  AECHAR      *m_pwstrTitle;  // title string
  AECHAR      *m_pwstrText;   // text string
  CMPColor     m_TextClr;     // text color
};
/* ------------------------------------------------------------------------
  CMPLabel Class, show text
 ------------------------------------------------------------------------ */
class CMPButton : public CMPLabel
{
public:
  CMPButton();
  CMPButton(void* pOwner);
  virtual ~CMPButton();

  virtual boolean Draw(IGraphics *pg);
};
/* ------------------------------------------------------------------------
  CMPRadioButton Class,  a label with check box
 ------------------------------------------------------------------------ */
class CMPRadioButton : public CMPLabel
{
public:
  CMPRadioButton();
  CMPRadioButton(void* pOwner);
  virtual ~CMPRadioButton();

  virtual boolean Draw(IGraphics *pg);
};

/* ------------------------------------------------------------------------
  CMPCheckBox Class, a label with selectable rectangle box
 ------------------------------------------------------------------------ */
class CMPCheckBox : public CMPLabel
{
public:
  CMPCheckBox();
  CMPCheckBox(void* pOwner);
  virtual ~CMPCheckBox();

  virtual boolean Draw(IGraphics *pg);
};
/* ------------------------------------------------------------------------
  CMPSrollBar Class, vertical scroll bar
 ------------------------------------------------------------------------ */
class CMPSrollBar : public CMPLabel
{
public: 
  CMPSrollBar();
  CMPSrollBar(void* pOwner);
  virtual ~CMPSrollBar();

  virtual boolean Draw(IGraphics *pg);
};

// **************************************************************
/* ------------------------------------------------------------------------
  CMPNumEditCtl Class, numerical edit ctrl, 
 ------------------------------------------------------------------------ */
class CMPNumEdit : public CMPLabel
{
public: 

enum {
  TEXT_OFFSET  = 12
};

  CMPNumEdit();
  CMPNumEdit(void* pOwner);
  virtual ~CMPNumEdit();

  virtual boolean Draw(IGraphics *pg);
  virtual boolean OnEvent(AEEEvent eCode, uint16 wParam8, uint32 dwParam);

  // insert a char at current position
  boolean AddChar(const char* pChar);
  // delete a char from current position
  boolean DeleteChar();
  // return TURE, if it is editable, otherwise return FALSE
  boolean GetEditMode();
  // set editable
  void    SetEditMode(boolean bMode);

  // set/get min and max value
  void SetRange(int nMin, int nMax);
  void GetRange(int* nMin, int* nMax);

  void SetIncStep(int nValue);

  // set/get ctrl name, if will be shown at left of the ctrl
  void SetLabelName(const char* strName);
  const AECHAR* GetLabelName();
  
  // set/get ctrl value
  void SetValue(int nValue);
  int  GetValue();

  // return TRUE, if value is changed
  boolean GetDirty();

  // set/get editable rect offset
  void SetRectOffset(int nOffset);
  int  GetRectOffset();

  // this width will be used if it is smaller than
  // the editable rect offset width
  void SetWidth(int nWidth);
  void SetDirty(boolean bMode);
protected:
  //CMPSrollBar m_ScrollBar;

  CMPColor m_clrBkEdit;
  int      m_nMaxLength;
  int      m_nCurrentPos;
  boolean  m_bEdit;

  AECHAR*  m_strLabelName;
  int      m_nValue;
  int      m_nRectOffset;      // editable rect offset
  int      m_nRectWidth;       // editable rect width. this width will 
                               // be used if it is smaller the 
                               // editable rect offset width
  boolean m_bDirty;            // true, if ctl is modified
  int     m_nSavedData; 
  unsigned short m_previousEvent;
  int     m_nMin;
  int     m_nMax;
  int     m_nStep;

  void DrawTickBar(IGraphics *pg, CMPRect& rc);
  void DrawCursor(IGraphics *pg, AEERect* pAEERc);

  boolean OnKeyEvent(AEEEvent eCode, uint16 wParam, uint32 dwParam);
};

/* ------------------------------------------------------------------------
  CMPBar, show rectagle bar graph peak hold information 
  for analyzer display 
 ------------------------------------------------------------------------ */
class CMPBar : public CMPSymbol
{
public:
  CMPBar();
  CMPBar(void* pOwner);
  virtual ~CMPBar();
  // set data
  void    SetData(int32 data);
  virtual boolean Draw(IGraphics *pg);

  // set/get peak hold buffer size
  boolean SetBufSize(int nSize);
  int     GetBufSize();

  // clean peak hold data
  void    CleanPeakHold();
protected:
  int32* m_pDataBuffer;     // data buffer for peak hold calculation
  int    m_nReqBufferSize;  // saved buffersize
  int    m_nBufferSize;     // buffer size
  int    m_nIndex;          // current pos
  int32  m_nMaxValue;       // max value  
  int    m_nMaxValueIndex;  // pos of max value in the buffer

  int  GetNextIndex();
  void ComputeMaxValue();
};

/* ------------------------------------------------------------------------
  CMPThumb, this is slider bar thumb
 ------------------------------------------------------------------------ */
class CMPThumb : public CMPSymbol
{
public:
  CMPThumb(void* pOwner);
  virtual ~CMPThumb();
  virtual boolean Draw(IGraphics *pg);

  void SetThumbSize(int dx, int dy);
};

/* ------------------------------------------------------------------------
  CMPSlider, vertical slider bar with thumb
 ------------------------------------------------------------------------ */
class CMPSlider : public CMPSymbol
{
public:
  enum{HORZIONTAL = 0, VERTICAL};
  CMPSlider();
  CMPSlider(void* pOwner);
  virtual ~CMPSlider();

  virtual boolean Draw(IGraphics *pg);

  // set/get thumb position
  boolean SetThumbPos(int value);
  int  GetThumbPos();
  // set thumb size
  void SetThumbSize(int w, int h);

  // set/get upper and lower bounds
  void SetUpperBound(int value);
  int  GetUpperBound();
  void SetLowerBound(int value);
  int  GetLowerBound();

  void SetOrientation(int nOrientation);
  // return thumb object
  CMPThumb* GetThumb();
  virtual void SetOwnerPtr(void* ptr);
protected:
  CMPThumb  m_Thumb;           // thumb
  int m_bOrientation;    // HORZIONTAL or VERTICAL
  int m_nThombPos;           // current thumb pos
                             // y for VERTICAL, x for HORZIONTAL
  int m_nUpperBound;         // slider max value
  int m_nLowerBound;         // slider min value
  
  // for VERTICAL slider
  int GetThumbPhysicalPos(int log, int Or);
  int GetThumbLogicalPos(int log, int Or);
};

/* ------------------------------------------------------------------------
  CMPSliderH, 
  contains a text label on the left side and 
  a horizontal slider bar on right size
 ------------------------------------------------------------------------ */
class CMPSliderH : public CMPPanel
{
public: 
  CMPSliderH(void* pOwner);
  virtual ~CMPSliderH();

  // initialization
  virtual boolean Init(AEERect rc) { return Init(rc.x, rc.y, rc.dx, rc.dy); }
  virtual boolean Init(int x, int y, int dx, int dy);
  
  // set text string for the label
  void SetLabelName(const char* pszSting);
  AECHAR* GetNameString();

  CMPLabel*  GetLabel();
  CMPSlider* GetSlider();

  void SetSliderStep(int nStep);
  void SetSliderBarHight(int nHight);
  void SetSliderThumbSize(int w, int h);

  int GetThumbValue();

  // this function will force the slider to be in given offset
  // set it back to 0, will removing the forcing mode, and the 
  // offset will be calculated base on the label string length
  // Init() should be called to take effect after setting
  void SetSliderOffset(int x);
public:
  virtual boolean Draw(IGraphics *pg);
  virtual boolean OnEvent(AEEEvent eCode, uint16 wParam, uint32 dwParam);

  virtual void SetFrameRect(CMPRect& rc);
  virtual void SetFrameRect(int x, int y, int dx, int dy);
  virtual void SetFocus(boolean bMode);
  virtual boolean GetFocus();
protected:
  enum Items { SLIDERBAR_HIGH = 4, VALUE_STEP = 1, THUMB_WIDTH = 6, THUMB_HIGHT = 12};
  CMPLabel    m_Label;
  AECHAR*     m_pNameString;
  CMPSlider   m_pSlider;
  boolean     m_bForceSliderOffset;
  int         m_nSliderOffset;
  int         m_nStep;
  int         m_SliderHight;
  int         m_szThumbW;
  int         m_szThumbH;
};

// This structure contains reference objects
struct MPDeviceInfo
{
#ifdef FEATURE_MP_EQ
  MPDeviceInfo():m_pShell(NULL), m_pDisplay(NULL), m_pSetting(NULL),m_pFilters(NULL){}
#else
  MPDeviceInfo():m_pShell(NULL), m_pDisplay(NULL){
#ifdef FEATURE_MP_EQ_UI
  m_pSetting = NULL;
#endif //FEATURE_MP_EQ_UI
  }
  //MPDeviceInfo(IShell* pShell, IDisplay* pDisp):m_pShell(pShell), m_pDisplay(pDisp){}
#endif

  IShell*    m_pShell;
  IDisplay*  m_pDisplay;

#ifdef FEATURE_MP_EQ_UI
  MPSetting*   m_pSetting;
#ifdef FEATURE_MP_EQ
  CMPEQData* m_pFilters;
#endif
#endif
};

// contains MPDeviceInfo, can be used by top level panel
class CMPObject : public CMPSymbol
{
public:
  CMPObject(){}
  CMPObject(void *pOwner):CMPSymbol(pOwner){};
  virtual ~CMPObject(){}

  MPDeviceInfo* GetMPInfo(){return &m_MPInfo;}

public:
  MPDeviceInfo m_MPInfo;
};

// convert from CMPSymbol owner ptr to MPDeviceInfo
// note, the owner ptr must be pointed to 
// an class object derived from CMPObject
template <class T>
inline MPDeviceInfo* MP_GetInfo (T* p)
{
  CMPObject* pOwner = (CMPObject*)(p->GetOwnerPtr());
  if(pOwner)
    return pOwner->GetMPInfo();
  else
    return NULL;
}

#endif //__cplusplus

#endif //MPSYMBOLS_H
