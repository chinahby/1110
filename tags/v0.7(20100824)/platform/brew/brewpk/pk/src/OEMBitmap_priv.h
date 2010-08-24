/*=====================================================================
FILE:  OEMBitmap_priv.h

SERVICES:  OEMBitmap private header file

DESCRIPTION: Reference IBitmap implementation private header file

PUBLIC CLASSES: Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS: Not Applicable
        
        Copyright © 2003 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================================*/


//////////////////////////////////////////////////////////////////////////////////
// ! WARNING ! WARNING ! WARNING ! WARNING ! WARNING ! WARNING ! WARNING !
// This file should absolutely not be included from anywhere except OEMBitmapN.c,
// OEMDisp.c, and OEMDisplayDev.c.
//////////////////////////////////////////////////////////////////////////////////


#ifndef OEMBITMAP_PRIV_H
#define OEMBITMAP_PRIV_H

// If defined, implement ITransform
#define IMPLEMENT_ITRANSFORM

#include "AEEBitmap.h"
#include "OEMDisp.h"
#include "OEMDisplayDev.h"
#include "AEEBase.h"
#include "AEEBitmapRot.h"
#include "customer.h"

#ifdef IMPLEMENT_ITRANSFORM
#include "AEETransform.h"
#endif

typedef struct OEMBitmap OEMBitmap;

// This is just for type safe casting of an OEMBitmap* to an IBitmap*.
static __inline IBitmap *OEMBITMAP_TO_IBITMAP(const OEMBitmap *pOEMBitmap)
{
   return (IBitmap*)(void*)pOEMBitmap;
}

typedef struct OEMBitmapCtl {
   AEEBASE_INHERIT(IBitmapCtl, OEMBitmap);   // IBitmapCtl is an AEEBase (can use AEEBase_... methods)
} OEMBitmapCtl;

typedef struct OEMBitmapDev {
   AEEBASE_INHERIT(IBitmapDev, OEMBitmap);
} OEMBitmapDev;

typedef struct OEMBitmapRot {
   AEEBASE_INHERIT(IBitmapRot, OEMBitmap);
} OEMBitmapRot;

#ifdef IMPLEMENT_ITRANSFORM
typedef struct OEMTransform {
   AEEBASE_INHERIT(ITransform, OEMBitmap);
} OEMTransform;
#endif


// Our own private interface ID.  This must be replaced with a new IID if the structure
// below is changed in any way.
#define AEEIID_OEMBITMAP3   0x0106c870

struct OEMBitmap {
   // IDIB overlays IBitmap.
   //
   // The vtable (pvt) pointer of m_dib is used as the vtable pointer for the
   // IBitmap instance.
   IDIB        m_dib;

   uint32      m_uRefs;

   PFNREALLOC  m_pfnRealloc;

   uint16      m_cxEffective;    // stores effective cx (0 if disabled, otherwise equal to m_dib.cx)
   IDisplayDev *m_pDisplayDev;   // display interface if this is the device bitmap
   IBitmap     *m_pParent;       // parent IBitmap, for handling update
   IBitmapDev  *m_pParentDev;    // parent IBitmapDev, for handling update

   int         m_nFullWidth;     // full width and height of bitmap, not taking
   int         m_nFullHeight;    // restriction and rotation into account

   // area of bitmap that is dirty (needs to be updated to screen)
   int         m_xDirty;
   int         m_yDirty;
   int         m_x2Dirty;
   int         m_y2Dirty;
   boolean     m_bDisableDirty;

   int         m_xOrigin;        // our 0,0 corresonds to this x,y in our parent
   int         m_yOrigin;

   int         m_nRotation;

   AEECallback *m_pcbEnable;     // callbacks to fire when bitmap is enabled or disabled
   AEECallback *m_pcbRelease;    // callbacks to fire when last reference to this bitmap is released

   // declare IBitmapCtl interface
   OEMBitmapCtl m_IBitmapCtl;

   // declare IBitmapDev interface
   OEMBitmapDev m_IBitmapDev;

   OEMBitmapRot m_IBitmapRot;

#ifdef IMPLEMENT_ITRANSFORM
   // declare ITransform interface
   OEMTransform m_ITransform;
#endif

};

//
// Dirty rectangle macros
//

#define EXPAND_DIRTY_RECT(pMe, x, y, dx, dy)                                        \
do {                                                                                \
   (pMe)->m_xDirty  = MIN((pMe)->m_xDirty,  (x));                                   \
   (pMe)->m_x2Dirty = MAX((pMe)->m_x2Dirty, (x) + (dx));                            \
   (pMe)->m_yDirty  = MIN((pMe)->m_yDirty,  (y));                                   \
   (pMe)->m_y2Dirty = MAX((pMe)->m_y2Dirty, (y) + (dy));                            \
} while (0)

#define IS_OUTSIDE_DIRTY_RECT(pMe, x, y, dx, dy)                                    \
   ((x) < (pMe)->m_xDirty || (x) + (dx) > (pMe)->m_x2Dirty ||                       \
    (y) < (pMe)->m_yDirty || (y) + (dy) > (pMe)->m_y2Dirty)


#endif /* OEMBITMAP_PRIV_H */

