/*
  ========================================================================

  FILE:  AEEResFile.h
  
  SERVICES: 

  GENERAL DESCRIPTION: Defines a resource file 

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/
#ifndef __AEERESFILE_H__
#define __AEERESFILE_H__

#include "AEEShell.h"
#include "AEESource.h"
#include "AEEComUtil.h"
#include "AEEStdLib.h"

// IResFile interface ids
#define AEEIID_RESFILE_1    	0x01022738
#define AEEIID_RESFILE_2    	0x0102ec58

#define AEEIID_RESFILE		   AEEIID_RESFILE_2

#define RESTYPE_DICT     0x5001
#define RESTYPE_WPROPS   0x5002

typedef struct IResFile IResFile;
#define INHERIT_IResFile(iname) \
  INHERIT_IQueryInterface(iname); \
   int   (*Open)            (iname *, const char *fname);\
   void  (*Close)           (iname *);\
   int   (*Get)             (iname *, uint32 resid, uint16 restype, void *pbuf, uint32 *plen);\
   int   (*GetSource)       (iname *, uint32 resid, uint16 restype, ISource **ppo);\
   int   (*GetObject)       (iname *, uint32 resid, AEECLSID clsid, void **ppo);\
   int   (*GetNamedIdV)     (iname *, uint16 restype, uint32 *presid, VaListPtrType args);\
   int   (*GetNamedV)       (iname *, uint16 restype, void *pbuf, uint32 *plen, VaListPtrType args);\
   int   (*GetNamedObjectV) (iname *, AEECLSID clsid, void **ppo, VaListPtrType args);\
   int   (*GetNamedSourceV) (iname *, uint16 restype, ISource **ppo, VaListPtrType args);\
   int   (*GetNamedChildrenV)(iname*, void *buf, uint32 *plen, uint32 *pnrElem, VaListPtrType args);\
   int   (*GetNamedTypeV)   (iname *, uint16* ptype, VaListPtrType args)

AEEINTERFACE(IResFile) {
   INHERIT_IResFile(IResFile);
};

static __inline uint32 IRESFILE_AddRef(IResFile *po) {
   return AEEGETPVTBL(po, IResFile)->AddRef(po);
}

static __inline uint32 IRESFILE_Release(IResFile *po) {
   return AEEGETPVTBL(po, IResFile)->Release(po);
}

static __inline int IRESFILE_QueryInterface(IResFile *po, AEECLSID clsid, void **ppo) {
   return AEEGETPVTBL(po, IResFile)->QueryInterface(po, clsid, ppo);
}

static __inline int IRESFILE_Open(IResFile *po, const char *f) {
   return AEEGETPVTBL(po,IResFile)->Open(po, f);
}

static __inline void IRESFILE_Close(IResFile *po) {
   AEEGETPVTBL(po, IResFile)->Close(po);
}

static __inline int IRESFILE_Get(IResFile *po, uint32 resid, uint16 restype, void *pbuf, uint32 *plen) {
   return AEEGETPVTBL(po, IResFile)->Get(po, resid, restype, pbuf, plen);
}

static __inline int IRESFILE_GetSource(IResFile *po, uint32 resid, uint16 restype, ISource **ppo) {
   return AEEGETPVTBL(po, IResFile)->GetSource(po, resid, restype, ppo);
}

static __inline int IRESFILE_GetObject(IResFile *po, uint32 resid, AEECLSID clsid, void **ppo) {
   return AEEGETPVTBL(po, IResFile)->GetObject(po, resid, clsid, ppo);
}

static __inline int IRESFILE_GetNamedIdV(IResFile *po, uint16 restype, uint32 *presid, VaListPtrType args) {
   return AEEGETPVTBL(po, IResFile)->GetNamedIdV(po, restype, presid, args);
}

static __inline int IRESFILE_GetNamedV(IResFile *po, uint16 restype, void *pbuf, uint32 *plen, VaListPtrType args) {
   return AEEGETPVTBL(po, IResFile)->GetNamedV(po, restype, pbuf, plen, args);
}

static __inline int IRESFILE_GetNamedObjectV(IResFile *po, AEECLSID clsid, void **ppo, VaListPtrType args) {
   return AEEGETPVTBL(po, IResFile)->GetNamedObjectV(po, clsid, ppo, args);
}

static __inline int IRESFILE_GetNamedSourceV(IResFile *po, uint16 restype, ISource **ppo, VaListPtrType args) {
   return AEEGETPVTBL(po, IResFile)->GetNamedSourceV(po, restype, ppo, args);
}

static __inline int IRESFILE_GetNamedChildrenV(IResFile *po, void *pbuf, uint32 *plen, uint32 *prnElem, VaListPtrType args) {
   return AEEGETPVTBL(po, IResFile)->GetNamedChildrenV(po, pbuf, plen, prnElem, args);
}

static __inline int IRESFILE_GetNamedTypeV(IResFile *po, uint16 *ptype, VaListPtrType args) {
   return AEEGETPVTBL(po, IResFile)->GetNamedTypeV(po, ptype, args);
}

static __inline int IRESFILE_GetString(IResFile *po, uint32 resid, AECHAR *buf, uint32 *plen) {
   return IRESFILE_Get(po, resid, RESTYPE_STRING, (void*)buf, plen);
}

static __inline int IRESFILE_GetNamed(IResFile *po, uint16 restype, void *pbuf, uint32 *plen, ...) {
   int result;
   va_list args;
   va_start(args, plen);
   result = IRESFILE_GetNamedV(po, restype, pbuf, plen, VaListPtr_From_va_list(&args));
   va_end(args);
   return result;
}

static __inline int IRESFILE_GetNamedObject(IResFile *po, AEECLSID clsid, void **ppo, ...) {
   int result;
   va_list args;
   va_start(args, ppo);
   result = IRESFILE_GetNamedObjectV(po, clsid, ppo, VaListPtr_From_va_list(&args));
   va_end(args);
   return result;
}


static __inline int IRESFILE_GetNamedChildren(IResFile *po, void *buf, uint32 *plen, uint32 *pnrElem, ...) {
   int result;
   va_list args;
   va_start(args, pnrElem);
   result = IRESFILE_GetNamedChildrenV(po, buf, plen, pnrElem, VaListPtr_From_va_list(&args));
   va_end(args);
   return result;
}


static __inline int IRESFILE_GetNamedSource(IResFile *po, uint16 restype, ISource **ppo, ...) {
   int result;
   va_list args;
   va_start(args, ppo);
   result = IRESFILE_GetNamedSourceV(po, restype, ppo, VaListPtr_From_va_list(&args));
   va_end(args);
   return result;
}

static __inline int IRESFILE_GetNamedId(IResFile *po, uint16 restype, uint32 *presid, ...) {
   int result;
   va_list args;
   va_start(args, presid);
   result = IRESFILE_GetNamedIdV(po, restype, presid, VaListPtr_From_va_list(&args));
   va_end(args);
   return result;
}

static __inline int IRESFILE_GetNamedImageV(IResFile *po, void **ppo, VaListPtrType args) {
   return IRESFILE_GetNamedObjectV(po, HTYPE_VIEWER, ppo, args);
}

static __inline int IRESFILE_GetNamedImage(IResFile *po, void **ppo, ...) {
   int result;
   va_list args;
   va_start(args, ppo);
   result = IRESFILE_GetNamedImageV(po, ppo, VaListPtr_From_va_list(&args));
   va_end(args);
   return result;
}

static __inline int IRESFILE_GetNamedType(IResFile *po, uint16* ptype, ...) {
   int result;
   va_list args;
   va_start(args, ptype);
   result = IRESFILE_GetNamedTypeV(po, ptype, VaListPtr_From_va_list(&args));
   va_end(args);
   return result;
}

static __inline int IRESFILE_GetNamedStringV(IResFile *po, AECHAR *buf, uint32 *plen, VaListPtrType args) {
   return IRESFILE_GetNamedV(po, RESTYPE_STRING, (void*)buf, plen, args);
}

static __inline int IRESFILE_GetNamedString(IResFile *po, AECHAR *buf, uint32 *plen, ...) {
   int result;
   va_list args;
   va_start(args, plen);
   result = IRESFILE_GetNamedStringV(po, buf, plen, VaListPtr_From_va_list(&args));
   va_end(args);
   return result;
}

#include "bid/AEECLSID_RESFILE.bid"

/////////////////////////////////////////////////////////////////////////////

/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================
ResFile Interface and Class IDs

Description: 

   Resource files are used a lot in BREW to separate configuration
   items from the actual code. Common uses are strings, images, colors
   and anything else that should be configurable for the app.

   IResFile allows for a single interface for retrieving resources
   from BREW resource files. Additionally, IResFile supports resource
   files that contain named resources. Note that named resources are
   not supported by the standard BREW compiler but are generated by
   the theme compiler that is part of the UI toolkit
   
   The class ID used to identify the ResFile interface and
   implementation are defined below.

Definition:

Identifier          Definition
----------          -----------------------------------
AEECLSID_RESFILE:   Identifies the class ID of the ResFile 
AEEIID_RESFILE:     Identifies the interface id of the ResFile interface
   
Comments:
   None

See Also:
   ISHELL_CreateInstance()
   
===========================================================================
 INTERFACES DOCUMENTATION
===========================================================================

IResFile Interface

Description: 

   IResFile is an interface that can be used to retrieve resources
   from BREW resource files. The interface is intended to be used
   instead of the ISHELL_LoadRes equivalents, but uses the same
   constants as the ISHELL equivalents.

   In addition, IResFile has APIs to help form and widget based applications 
   apply theme information, used with named entries in
   resource files which can be created using the theme compiler.
   
   For more information about themes, and a list of default names
   used with common forms, please see IROOTFORM_ApplyWProps() and 
   IFORM_SetThemeBaseName().

   The following APIs are provided to support an object reference counting mechanism for
   objects to manage their own memory instances.
===pre>
       IRESFILE_AddRef()
       IRESFILE_Release()
===/pre>

   The following APIs are provided to support the ability to query
   other interfaces from an object.
===pre>
       IRESFILE_QueryInterface()
===/pre>

   The following APIs are provided to open and close resource files. 
===pre>
   IRESFILE_Open()
   IRESFILE_Close() 
===/pre>

   The following APIs are provided to get resources from an open
   resource file
===pre>
   IRESFILE_Get()
   IRESFILE_GetObject()
===/pre>

   The following APIs are provided to get named resources from an open
   resource file
===pre>
   IRESFILE_GetNamed()
   IRESFILE_GetNamedObject()
===/pre>

   The following APIs are provided to get the id for a named resources
   from an open resource file
===pre>
   IRESFILE_GetNamedId()
===/pre>

   IResFile supports standard resource file types listed in AEEShell.h (ResType enum).  In addition, IResFile 
   supports the following:   

===pre>   
   RESTYPE_WPROPS: A collection of properties (ID / value pairs), 8 bytes each (4 bytes ID, 4 bytes value)
===/pre>

   IRESFILE_GetNamed() can be used to retrieve objects of type RESTYPE_WPROPS

===========================================================================

IRESFILE_Open()

Description:
   Opens a given resource file. 

Prototype:
   int   IRESFILE_Open(IResFile *p, const char *f);

Parameters:
===pre>   
   p:        A pointer to an IResFile object.
   f:        The name of the resource file to open
===/pre>   
   
Return Value:
===pre>
   SUCCESS, if at least 1 file was successfully opened
   EFILEOPEN, if given IResFile is already opened
   EFILENOEXISTS, if given file name does not exist
   EBADFILENAME, if the given filename is invalid
===/pre>

Comments:
   None

Side Effects: 
   Associates the IResFile instance with the given resource
   file(s). Subsequent resource access functions will use the given
   resource file to load resources.

   This function accepts multiple resource files, each of which will be
   opened and used as a search path on future resource accesses.
   Files are separated by semicolons.

   For example:
===pre>
   IRESFILE_Open(pRes, "customtheme.bar;basetheme.bar");
===/pre>

   This will initialize both resource files for access.  Future access functions
   will start with "customtheme.bar", followed by "basetheme.bar", returning
   the first valid entry found.  

   See IRESFILE access functions for more information.   
   
See Also:
    IResFile Interface
    IRESFILE_GetNamed()
    IRESFILE_GetNamedObject()
    IRESFILE_GetNamedId()
    IRESFILE_GetObject()
    IRESFILE_Get()

===========================================================================

IRESFILE_Close()

Description:
   Closes the IResFile

Prototype:
   void   IRESFILE_Close(IResFile *p);

Parameters:
===pre>   
   p:        A pointer to an IResFile object.
===/pre>   
   
Return Value:
   None

Comments:
   None

Side Effects: 
   Closes the IResFile. Subsequent calls to resource access functions
   will fail.
   
See Also:
    IResFile Interface
    IRESFILE_Open()

===========================================================================

IRESFILE_Get()

Description:
   Gets a resource with given id and type from the IResFile. This
   function can also be used to request the size of a given resource
   in the resource file

Prototype:
   int   IRESFILE_Get(IResFile *p, uint32 i, uint16 t, void *pb, uint32 *pl);

Parameters:
===pre>   
   p:        A pointer to an IResFile object.
   i:        id of the resource to be loaded
   t:        type of the resource to be loaded
   pb:       pointer to a buffer where the resource data is to be loaded into 
   pl:       pointer to where the size of the loaded resource can be stored
===/pre>   
   
Return Value:
   SUCCESS, if the resource was found and loaded
   EFAILED, if the resource could not be found or failed to load

Comments:
   If the pointer to the given buffer is set to -1 the size of the
   resource will be returned in the size field

Side Effects: 
   None
   
See Also:
    IResFile Interface
    IRESFILE_GetObject()
    ISHELL_LoadResDataEx()

===========================================================================

IRESFILE_GetSource()

Description:
   Gets an ISource stream for the data of the given resource.

Prototype:
   int   IRESFILE_GetSource(IResFile *p, uint32 i, uint16 t, ISource **ppo);

Parameters:
===pre>   
   p:        A pointer to an IResFile object.
   i:        id of the resource to be loaded
   t:        type of the resource to be loaded
   ppo:      pointer to a destination for the resulting ISource pointer
===/pre>   
   
Return Value:
   SUCCESS, if the resource was found and loaded
   EFAILED, if the resource could not be found or failed to load
   ENOTSUPPORT, if sources are not supported for this resource file
   ENOMEMORY, if memory could not be allocated for the source result

Comments:

Side Effects: 
   None
   
See Also:
    IResFile Interface
    ISource Interface
    IRESFILE_Get()

===========================================================================

IRESFILE_GetObject()

Description:
   Gets a resource object with given class from the resource
   file. This function can be used to easily load BREW interface based
   objects, such as images and sounds from a resource file using the
   HTYPE constants defined in AEEShell.h

Prototype:
   int   IRESFILE_GetObject(IResFile *p, uint32 i, AEECLSID c, void **ppo);

Parameters:
===pre>   
   p:        A pointer to an IResFile object
   i:        the resource id of the object
   c:        the class id or HTYPE of the object to be loaded
   ppo:      pointer to the destination pointer for the object
===/pre>   
   
Return Value:
   SUCCESS, if the object was loaded 
   EFAILED, if the object was not found or failed to load correctly

Comments:
   The function accepts HTYPE and classid for the classid field. 

Side Effects: 
   None
   
See Also:
    IResFile Interface
    IRESFILE_GetObject()
    ISHELL_LoadResObject

===========================================================================

IRESFILE_GetString()

Description:
   Retrieves a string resource with given id from IResFile. This
   function can also be used to request the size of a string resource
   in the resource file

If successful, the number of bytes copied with be stored in len.

Prototype:
   int   IRESFILE_GetString(IResFile *po, uint32 resid, AECHAR *buf, uint32 *plen) {

Parameters:
===pre>   
   po:       A pointer to an IResFile object.
   resid:    id of the string resource to be loaded   
   buf:      pointer to a buffer where the resource data is to be loaded into 
   plen:     Pointer to size of resource or input buffer
===/pre>   
   
Return Value:
   SUCCESS, if the resource was found
   EFAILED, if the resource could not be found or failed to load

Comments:
   If the pointer to the given buffer is set to -1 the size of the
   resource will be returned in the size field.

   This function is a macro for calling IRESFILE_Get() with RESTYPE_STRING.

Side Effects: 
   None
   
See Also:
    IResFile Interface
    IRESFILE_Get()
    ISHELL_LoadResString()

===========================================================================

IRESFILE_GetNamed()

Description:

   Gets a named resource from a resource file. Names for a resource
   can be given as a variable set of, null terminated, string
   arguments. Each string specifies one or more levels in the naming
   tree. A simple string without '.' characters specifies a single
   level in the naming tree where as a dot separated set of names in a
   string specify a series of names in the naming tree.

   Loading a named string, for example, could look like:

===pre>
      IRESFILE_GetNamed(piResFile, RESTYPE_STRING, pbuf, &size, "Main", "Menu", "Open", 0))
===/pre>

   or could look like:

===pre>
      IRESFILE_GetNamed(piResFile, RESTYPE_STRING, pbuf, &size, "Main.Menu.Open", 0))
===/pre>

   or a mix of the above:

===pre>
      IRESFILE_GetNamed(piResFile, RESTYPE_STRING, pbuf, &size, "Main.Menu", "Open", 0))
===/pre>

Prototype:
   int   IRESFILE_GetNamed(IResFile *p, uint16 t, void *pb, uint32 *pl, ...);

Parameters:
===pre>   
   p:        A pointer to an IResFile object.
   t:        type of the resource to be loaded
   pb:       pointer to a buffer where the resource data is to be loaded into 
   pl:       pointer to where the size of the loaded resource can be stored
===/pre>   
   
Return Value:
   SUCCESS, if the named resource was found and loaded
   EBADSTATE, if this resource file is not open 
   EUNSUPPORTED, if this resource file does not support named resources
   EUNABLETOLOAD, if the named resource could not be resolved or loaded

Comments:
   This function is a shortcut function for the actual interface
   function IRESFILE_GetNamedV which takes an actual vararg parameter.

Side Effects: 
   None
   
See Also:
    IResFile Interface
    IRESFILE_GetObject()
    ISHELL_LoadResObject


===========================================================================

IRESFILE_GetNamedImage()


Description:

   Gets a named image from a resource file. Names for a resource
   can be given as a variable set of, null terminated, string
   arguments. Each string specifies one or more levels in the naming
   tree. A simple string without '.' characters specifies a single
   level in the naming tree where as a dot separated set of names in a
   string specify a series of names in the naming tree.

   Loading a image, for example, could look like:

===pre>
      IRESFILE_GetNamedImage(piResFile, (void**)&piImage, "Main", "Background", "Image", 0))
===/pre>

   or could look like:

===pre>
      IRESFILE_GetNamedImage(piResFile, (void**)&piImage, "Main.Background.Image", 0))
===/pre>

   or a mix of the above:

===pre>
      IRESFILE_GetNamedImage(piResFile, (void**)&piImage, "Main", "Background.Image", 0))
===/pre>

Prototype:  
   int IRESFILE_GetNamedImage(IResFile *po, void **ppo, ...)

Parameters:
===pre>   
   po:       A pointer to an IResFile object.
   ppo:      Pointer to an IImage pointer
   ...:      null terminated string arguments (see above)
===/pre>   
   
Return Value:
   SUCCESS, if the named resource was found and loaded
   EBADSTATE, if this resource file is not open 
   EUNSUPPORTED, if this resource file does not support named resources
   EUNABLETOLOAD, if the named resource could not be resolved or loaded

Comments:
   This function is a shortcut function for the interface
   function IRESFILE_GetNamedImageV which takes an actual vararg parameter.

  IRESFILE_GetNamedImageV is actually a shortcut for the IRESFILE_GetNamedObject function
  with HTYPE_VIEWER.

Side Effects: 
   None
   
See Also:
    IResFile Interface
    IRESFILE_GetNamedObject()
    IRESFILE_GetNamed()

===========================================================================
  


IRESFILE_GetNamedObject()

Description:

   Gets a named object from a resource file. Names for a resource
   can be given as a variable set of, null terminated, string
   arguments. Each string specifies one or more levels in the naming
   tree. A simple string without '.' characters specifies a single
   level in the naming tree where as a dot separated set of names in a
   string specify a series of names in the naming tree.

   Loading a image, for example, could look like:

===pre>
      IRESFILE_GetNamedObject(piResFile, HTYPE_VIEWER, &piImage, "Main", "Background", "Image", 0))
===/pre>

   or could look like:

===pre>
      IRESFILE_GetNamedObject(piResFile, HTYPE_VIEWER, &piImage, "Main.Background.Image", 0))
===/pre>

   or a mix of the above:

===pre>
      IRESFILE_GetNamedObject(piResFile, HTYPE_VIEWER, pbuf, &size, "Main", "Background.Image", 0))
===/pre>

Prototype:
   int IRESFILE_GetNamedObject(IResFile *po, AEECLSID clsid, void **ppo, ...)

Parameters:
===pre>   
   po:        A pointer to an IResFile object.
   clsid:     Class ID of the handler (see ISHELL_LoadResObject(), AEEHandlerType) 
   ppo:       Pointer to the object pointer where object will be loaded
   ...:       null terminated string list (see above)
===/pre>   
   
Return Value:
   SUCCESS, if the named resource was found and loaded
   EBADSTATE, if this resource file is not open 
   EUNSUPPORTED, if this resource file does not support named resources
   EUNABLETOLOAD, if the named resource could not be resolved or loaded

Comments:
   This function is a shortcut function for the actual interface
   function IRESFILE_GetNamedObjectV which takes an actual vararg parameter.

Side Effects: 
   None
   
See Also:
    IResFile Interface
    IRESFILE_GetNamed()

===========================================================================

IRESFILE_GetNamedChildren()

Description:

   Gets the names of all children of the provided parent.  The names will be
   returned in a null terminated set of null terminated strings.  Also returned
   will be the total number of children found.

   If buf is NULL, the number of children found will be returned in pnrElem, and the total
   size of the buffer required for all of the names will be returned in plen.

   If plen is NULL, only the number of children found will be returned in pnrElem.

   The parent name can be given as a variable set of null terminated string
   arguments. Each string specifies one or more levels in the naming
   tree. A simple string without '.' characters specifies a single
   level in the naming tree where as a dot separated set of names in a
   string specify a series of names in the naming tree.  

   To specify the 'Root' of the dictionary in order to retrieve the names of the 
   top-level children, use a string containing a single dot ( "." ) for the parent name.

   Retrieving the names of children for "Main.Intro.Properties" could be accomplished
   by the following sequence of commands:

===pre>
      IRESFILE_GetNamedChildren(piResFile, NULL, pLength, pNumChildren, "Main.Intro.Properties", 0));
===/pre>

   after this command, the value "pLength" can be used to allocate a buffer.  A second command:

===pre>      
      IRESFILE_GetNamedChildren(piResFile, pNewBuf, pLength, pNumChildren, "Main.Intro.Properties", 0));
===/pre>

   would then return the names of all children.  
   
   Note that this buffer contains a number of null terminated strings, 
   and a final null terminating character marking the end of the set. 
   So a parent which has 2 children named "Child1" and "Child2", respectively, 
   would return the following buffer:
   
   {'C','h','i','l','d','1',\0,'C','h','i','l','d','2',\0,\0}

   The number of elements would be 2, and the buffer size would be 15.

   Retrieving the names of the top-level children (those under the root level), 
   could be accomplished by the following commands: 

===pre>
      IRESFILE_GetNamedChildren(piResFile, NULL, &nLength, &nNumChildren, ".", 0));

      // -- allocate buffer 'pBuffer' here --

      IRESFILE_GetNamedChildren(piResFile, pBuffer, &nLength, &nNumChildren, ".", 0));
===/pre>

   Which would fill the buffer 'pBuffer' with the names of the top-level children.


Prototype:
   int   IRESFILE_GetNamedChildren(IResFile *p, void *buf, uint32 *plen, uint32 *pnrElem, ...);

Parameters:
===pre>   
   p:        An in pointer to an IResFile object.
   buf:      An in\out pointer to a buffer which will receive the children names.
   plen:     An in\out pointer to the total size of the output buffer.
   pnrElem:  An out pointer to the number of children found
===/pre>   
   
Return Value:
   SUCCESS, if the named resource was found and loaded
   EBADPARM, if an argument was invalid, or the buffer size was too small for the result
   EBADSTATE, if this resource file is not open 
   EUNABLETOLOAD, if the named parent resource could not be resolved or loaded
   
Comments:
   This function is a shortcut function for the interface function 
   IRESFILE_GetNamedChildrenV which takes an actual vararg parameter.

   If multiple resource files were opened in IRESFILE_Open(), all of the applicable children will be returned
   for a given parent.  In our example above, if "Main.Intro.Properties" existed in 2 resource files, the combined list
   of children (without duplicates) will be returned.

   Note: plen and pnrElem can be output as 0 if the parent has no children.  Callers should watch for this
   before allocating memory.

Side Effects: 
   None
   
See Also:
    IResFile Interface
    IRESFILE_GetNamed()

===========================================================================


IRESFILE_GetNamedSource()

Description:

   Gets a named source from a resource file. Names for a resources
   can be given as a variable set of, null terminated, string
   arguments. Each string specifies one or more levels in the naming
   tree. A simple string without '.' characters specifies a single
   level in the naming tree where as a dot separated set of names in a
   string specify a series of names in the naming tree.

   Loading the source, for example, of an image could look like:

===pre>
      IRESFILE_GetNamedSource(piResFile, HTYPE_VIEWER, &piSource, "Main", "Background", "Image", 0))
===/pre>

   or could look like:

===pre>
      IRESFILE_GetNamedSource(piResFile, HTYPE_VIEWER, &piSource, "Main.Background.Image", 0))
===/pre>

   or a mix of the above:

===pre>
      IRESFILE_GetNamedSource(piResFile, HTYPE_VIEWER, &piSource, "Main", "Background.Image", 0))
===/pre>

Prototype:
   int   IRESFILE_GetNamedSource(IResFile *p, uint16 t, ISource **ppo, ...);

Parameters:
===pre>   
   p:        A pointer to an IResFile object.
   t:        type of the resource to be loaded   
   ppo:      pointer to a destination for the resulting ISource pointer
===/pre>   
   
Return Value:
   SUCCESS, if the resource was found and loaded
   EFAILED, if the resource could not be found or failed to load
   ENOTSUPPORT, if sources are not supported for this resource file
   ENOMEMORY, if memory could not be allocated for the source result

Comments:
   This function is a shortcut function for the actual interface
   function IRESFILE_GetNamedSourceV which takes an actual vararg parameter.

Side Effects: 
   None
   
See Also:
    IResFile Interface
    IRESFILE_GetNamed()
    IRESFILE_GetSource()

===========================================================================

IRESFILE_GetNamedId()

Description:

   Gets the id for a named resource from a resource file. Names for a
   resource can be given as a variable set of, null terminated, string
   arguments. Each string specifies one or more levels in the naming
   tree. A simple string without '.' characters specifies a single
   level in the naming tree where as a dot separated set of names in a
   string specify a series of names in the naming tree.

   Note: The ID returned contains both the 16 bit search path index and 
   the 16 bit resource file ID.  The high order bits contain the search path index, 
   and the low order bits contain the resource ID. So for example, if the 
   resource ID found was 255, and the search path index was 1 (2nd file in the search path), 
   the ID returned would be: 0x000100FF

   This ID is compatible with all IRESFILE functions which accept a 32bit ID 
   parameter.

   Finding the id for a named image in the resource files could look like:

===pre>
      IRESFILE_GetNamedId(piResFile, RESTYPE_IMAGE, &resid, "Main", "Background", "Image", 0))
===/pre>

   or could look like:

===pre>
      IRESFILE_GetNamedId(piResFile, RESTYPE_IMAGE, &resid, "Main.Background.Image", 0))
===/pre>

   or a mix of the above:

===pre>
      IRESFILE_GetNamedId(piResFile, RESTYPE_IMAGE, &resid, "Main.Background", "Image", 0))
===/pre>

Prototype:
   int   IRESFILE_GetNamedId(IResFile *p, uint16 t, uint32 *pr, ...);

Parameters:
===pre>   
   p:        A pointer to an IResFile object.
   t:        type of the resource to be loaded
   pr:       pointer to a storage area for the resulting resource id
===/pre>   
   
Return Value:
   SUCCESS, if the named resource was found and the resource id located
   EBADPARM, if one of the given parameters is invalid
   EBADSTATE, if this resource file is not open 
   EUNSUPPORTED, if this resource file does not support named resources
   EUNABLETOLOAD, if the named resource could not be resolved 

Comments:
   This function is a shortcut function for the actual interface
   function IRESFILE_GetNamedIdV which takes an actual vararg parameter.

Side Effects: 
   None
   
See Also:
    IResFile Interface
    IRESFILE_Get()

===========================================================================

IRESFILE_GetNamedType()

Description:

   Gets the type of a named resource from a resource file. Names for a
   resource can be given as a variable set of, null terminated, string
   arguments. Each string specifies one or more levels in the naming
   tree. A simple string without '.' characters specifies a single
   level in the naming tree where as a dot separated set of names in a
   string specify a series of names in the naming tree.

   Finding the type for a named object in the resource file could look like:

===pre>
      IRESFILE_GetNamedType(piResFile, &restype, "Main", "Background", "Object", 0))
===/pre>

Prototype:
   int   IRESFILE_GetNamedType(IResFile *p, uint16* pt, ...);

Parameters:
===pre>   
   p:        A pointer to an IResFile object.
   pt:       A pointer to storage for type of resource found
===/pre>   
   
Return Value:
   SUCCESS, if the named resource was found and the resource id located
   EBADPARM, if one of the given parameters is invalid
   EBADSTATE, if this resource file is not open 
   EUNSUPPORTED, if this resource file does not support named resources
   EUNABLETOLOAD, if the named resource could not be resolved 

Comments:
   This function is a shortcut function for the actual interface
   function IRESFILE_GetNamedTypeV which takes an actual vararg parameter.

Side Effects: 
   None
   
See Also:
    IResFile Interface
    IRESFILE_Get()

===========================================================================

IRESFILE_GetNamedString()

Description:

   Retrieves the string for a named resource in a resource file. Names for a
   resource can be given as a variable set of null terminated string
   arguments. Each string specifies one or more levels in the naming
   tree. A simple string without '.' characters specifies a single
   level in the naming tree where as a dot separated set of names in a
   string specify a series of names in the naming tree.  The final argument must be 0 
   to indicate the end of the name list. 

   Finding a size of named string can be done by passing -1 as the buffer argument, as
   follows: 

===pre>
      IRESFILE_GetNamedString(piResFile, (AECHAR*)-1, &len, "Main", "Dialog", "Title", 0))
===/pre>

   After allocating the buffer, a future call would retrieve the actual string:

===pre>
      IRESFILE_GetNamedString(piResFile, pBuf, &len, "Main", "Dialog", "Title", 0))
===/pre>

   If successful, the number of bytes copied with be stored in len.

Prototype:
   int   IRESFILE_GetNamedString(IResFile *po, AECHAR *buf, uint32 *plen, ...)   

Parameters:
===pre>   
   po:       A pointer to an IResFile object.   
   buf:      Pointer to a storage area for the located string
   plen:     Pointer to size of resource or input buffer
===/pre>   
   
Return Value:
   SUCCESS, if the named resource was found and the resource id located
   EBADPARM, if one of the given parameters is invalid
   EBADSTATE, if this resource file is not open 
   EUNSUPPORTED, if this resource file does not support named resources
   EUNABLETOLOAD, if the named resource could not be resolved 

Comments:
   This function is a shortcut function for the actual interface
   function IRESFILE_GetNamedStringV which takes an actual vararg parameter.

Side Effects: 
   None
   
See Also:
    IResFile Interface
    IRESFILE_GetNamed()
    ISHELL_LoadResString()

*/
#endif /* __AEERESFILE_H__ */
