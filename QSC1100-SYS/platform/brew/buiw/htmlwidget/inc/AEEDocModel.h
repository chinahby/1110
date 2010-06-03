/*
  ========================================================================

  FILE:  AEEDocModel.h
  
  SERVICES: 

  GENERAL DESCRIPTION: 

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/
#ifndef __AEEDOCMODEL_H__
#define __AEEDOCMODEL_H__

#include "AEESource.h"
#include "AEEWModel.h"

#include "AEEIID_DOCMODEL.BID"

#define EVT_HDM_BASE    0x00001200


/////////////////////////////////////////////////////////////////////////////
// IDocModel events

#define EVT_HDM_RESET         (EVT_HDM_BASE + 0)  // doc model has been reset
#define EVT_HDM_STOP          (EVT_HDM_BASE + 1)  // doc model has been stopped


/////////////////////////////////////////////////////////////////////////////
// IDocModel interface

typedef struct IDocModel IDocModel;

#define INHERIT_IDocModel(iname) \
   INHERIT_IModel(iname);\
   int   (*LoadSource)  (iname *po, ISource *piSource);\
   int   (*Write)       (iname *po, const byte *pbWrite, int cbLen, int *pcbWrote);\
   void  (*Writeable)   (iname *po, AEECallback *pcb);\
   void  (*Reset)       (iname *po);\
   void  (*Stop)        (iname *po);\
   int   (*SetBufferSize) (iname *po, int cbSize)
      

AEEINTERFACE(IDocModel) {
   INHERIT_IDocModel(IDocModel);
};


#define IDOCMODEL_AddRef(p)               AEEGETPVTBL((p),IDocModel)->AddRef(p)
#define IDOCMODEL_Release(p)              AEEGETPVTBL((p),IDocModel)->Release(p)
#define IDOCMODEL_QueryInterface(p,c,d)   AEEGETPVTBL((p),IDocModel)->QueryInterface(p,c,d)
#define IDOCMODEL_AddListener(p,pl)       AEEGETPVTBL((p),IDocModel)->AddListener(p,pl)
#define IDOCMODEL_Notify(p,pe)            AEEGETPVTBL((p),IDocModel)->Notify(p,pe)
#define IDOCMODEL_LoadSource(p,pi)        AEEGETPVTBL((p),IDocModel)->LoadSource(p,pi)
#define IDOCMODEL_Write(p,pb,cb,pcbw)     AEEGETPVTBL((p),IDocModel)->Write(p,pb,cb,pcbw)
#define IDOCMODEL_Writeable(p,pcb)        AEEGETPVTBL((p),IDocModel)->Writeable(p,pcb)
#define IDOCMODEL_Reset(p)                AEEGETPVTBL((p),IDocModel)->Reset(p)
#define IDOCMODEL_Stop(p)                 AEEGETPVTBL((p),IDocModel)->Stop(p)
#define IDOCMODEL_WriteTerminate(p)       AEEGETPVTBL((p),IDocModel)->Write(p,NULL,0,NULL)
#define IDOCMODEL_SetBufferSize(p,s)      AEEGETPVTBL((p),IDocModel)->SetBufferSize(p,s) 


// safe upcast
static __inline IModel *IDOCMODEL_TO_IMODEL(IDocModel *p) { 
   return (IModel *)p; 
}


/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================
IDocModel Interface ID

Description:
   Each model is identified by a unique interface ID.  These IDs are used
   throughout the widget framework to indicate the model that a given object
   will use to describe the data associated with that object.  

   The model interface is an abstract interface that has no concrete
   implementation (and thus no class ID to create one).  Instead it acts
   as the base interface for all model interfaces.

   The DocModel is identified by the following interface ID.

Definition:

Identifier        Definition
----------        -----------------------------------
AEEIID_DOCMODEL:  Identifies the document model interface
   
Comments:
   None

See Also:
   IDocModel Interface
   IWIDGET_GetModel()


IDocModel Events:
---------------------

EVT_HDM_RESET        - This event indicates that the document as been reset with IDOCMODEL_Reset()
                       There is no 'dwParam' data associated with this event

EVT_HDM_STOP         - This event indicates that the document as been stopped with IDOCMODEL_Stop()
                       There is no 'dwParam' data associated with this event 


===========================================================================
 INTERFACES DOCUMENTATION
===========================================================================

IDocModel Interface

Description:
   IDocModel is inherited from the IModel interface and is used to input data into
   a document viewer such as the HTML widget. This model only contains applet
   side methods (for inputting data), the widget to model interface is undefined 
   and uses private methods for data access.
   
=======================================================================

IDOCMODEL_AddRef()

Description:

   This function increments the reference count of the IDocModel interface object, allowing
   the interface to be shared by multiple callers.  The object is freed when the reference
   count reaches 0 (zero).

Prototype:

   uint32 IDOCMODEL_AddRef(IDocModel *p);

Parameters:
===pre>
   p:  Pointer to the IDocModel interface object
===/pre>

Return Value:
   Returns the incremented reference count for the IDocModel interface object.

Comments:
   None

Side Effects:
   None

See Also:
   IDOCMODEL_Release()
   
=======================================================================
IDOCMODEL_Release()

Description:

   This function decrements the reference count of the IDocModel interface object.  The object
   is freed from memory and is no longer valid once its reference count reaches 0 (zero).

Prototype:

   uint32 IDOCMODEL_Release(IDocModel *p);

Parameters:
===pre>
   p:  Pointer to the IDocModel interface object
===/pre>

Return Value:
   Returns the decremented reference count for the IDocModel interface object.  If the returned
   reference count is 0 (zero), the object has been freed and will no longer be valid.

Comments:
   None

Side Effects:
   None

See Also:
   IDOCMODEL_AddRef()

=======================================================================

IDOCMODEL_QueryInterface()

Description: 
   This function queries an interface for an alterrnate interface that may provide
   extended functionality or a proprietary extension.  If successful, the function
   will return a pointer to the requested interface in the memory location pointed
   to by the 'd' parameter.  The requested interface is identified by a globally
   unique ID passed to the function in the 'c' parameter.

Prototype:

   int IDOCMODEL_QueryInterface(IDocModel *p, AEECLSID c, void **d);

Parameters:
===pre>
   p:  [in]  Pointer to the IDocModel interface object.
   c:  [in]  A globally unique id that identifies the interface to be queried.
   d:  [out] Address of a memory location that will receive the requrested 
             interface pointer. If the queried item is not available, this 
             pointer is returned as NULL.
===/pre>

Return Value:
===pre>
   SUCCESS          - The requested interface was found and has been returned
                      in the 'd' parameter.
   ECLASSNOTSUPPORT - The requested interface is not supported.
===/pre>

Comments:  
   The pointer passed in the 'd' parameter is a double-indirection -- i.e. it is a
   pointer to a pointer.  Applications should use care in ensuring that the proper
   level of indirection is used in calling the function.

Side Effects: 
   Upon successful retrieval of an interface pointer, that interface is returned with
   its reference count incremented.  The caller is responsible for releasing this object
   at some point in the future.

See Also:
   IQI_QueryInterface()
   
==========================================================================

IDOCMODEL_AddListener()

Description:
   This function allows client objects to attach a listener to the given model.  The 
   listener includes a callback function that will be called whenever the model has 
   changed state.  

Prototype:
   int IDOCMODEL_AddListener(IDocModel *p, ModelListener *pl); 

Parameters:
===pre>
   p:   Pointer to the IDocModel interface object.
   pl:  Pointer to the ModelListener that will be attached to the object.  The ModelListener will
        contain the PFNLISTENER callback function that will be called for IDocModel events.
===/pre>
   
Return Value:
===pre>
   SUCCESS          - Listener successfully added
   EBADPARM         - Invalid argument
   ENOMEMORY        - Insufficient memory
===/pre>

Comments:  
   There is no guarantee that the listeners will be called in order.

Side Effects: 
   None

See Also:
   ModelListener
   PFNLISTENER

==========================================================================

IDOCMODEL_LoadSource()

Description:
   This function is used to pass an ISource interface to the IDocModel object
   to use as a data source. The IDocModel will use the Read() and Readable() 
   methods on the ISource to load document data into the model. The ISource
   interface is released on error or end-of-stream.  

Prototype:
   int IDOCMODEL_LoadSource(IDocModel *po, ISource *piSource);

Parameters:
===pre>
   p:          Pointer to the IDocModel interface object.
   piSource:   Pointer to an ISource interface to be used as a read source
===/pre>
   
Return Value:
===pre>
   SUCCESS     The model accepted the ISource and initiated a read operation.
   ENOMEMORY   Out of memory error occurred while trying to allocate a read buffer.
   EBADPARM    Invalid parameter (piSource)
===/pre>

Comments:  
   None

Side Effects: 
   None

See Also:
   


==========================================================================

IDOCMODEL_Write()

Description:
   This function is used to pass document data to the model.  This is an append operation 
   -- the data is added to whatever was previously in the document.  A Write operation 
   must be terminated with IDOCMODEL_WriteTermiante() to inform the model that no more 
   data is to be expected and the document is completely loaded.  
   
   This function will return the number of bytes that were successfully written. If 
   the return value does not equal 'cbLen', another write operation may be scheduled with 
   IDOCMODEL_Writeable().  

Prototype:
   int IDOCMODEL_Write(IDocModel *po, const byte *pbWrite, int cbLen, int *pcbWrote);
   

Parameters:
===pre>
   p:          Pointer to the IDocModel interface object.
   pbWrite:    Pointer to buffer containing document data
   cbLen:      Count of data bytes in buffer 'pbWrite'
   pcbWrote:   Address of variable to receive number of bytes accepted/written
===/pre>
   
Return Value:
===pre>
   SUCCESS     Successfully wrote bytes or in WOULDBLOCK state, call writeable 
               if (*pcbWrote != cbLen)
   EBADSTATE   LoadSource operation is pending, cannot accept Write
===/pre>

Comments:  
   None

Side Effects: 
   None

See Also:
   IDOCMODEL_Writeable
   IDOCMODEL_WriteTerminate


==========================================================================

IDOCMODEL_Writeable()

Description:
   This function is used to schedule a write operation. The function 
   IDOCMODEL_Write() is a non-blocking function that will return immediately 
   if the model cannot accept all passed data. If any of the data could not
   be written, this function is used to pass a callback function to be called
   when Write may succeed.  The callback function is contained in an AEECallback
   data structure, which may be initialized with the CALLBACK_Init() macro.

Prototype:
   void IDOCMODEL_Writeable(IDocModel *po, AEECallback *pcb);

Parameters:
===pre>
   p:   Pointer to the IDocModel interface object.
   pcb: Pointer to an AEECallback data structure containing a callback function
        that will be called when Write() may succeed.
===/pre>
   
Return Value:
   None

Comments:  
   None

Side Effects: 
   None

See Also:
   AEECallback
   CALLBACK_Init

==========================================================================

IDOCMODEL_Reset()

Description:
   This function resets the state of the document to the empty state.

Prototype:
   void IDOCMODEL_Reset(IDocModel *po);

Parameters:
===pre>
   p:   Pointer to the IDocModel interface object.
===/pre>
   
Return Value:
   None

Comments:  
   None

Side Effects: 
   None

See Also:


==========================================================================

IDOCMODEL_Stop()

Description:
   This function stops the current loading of the document. If a LoadSource()
   operation is pending, it will be canceled and the ISource will be released.
   if a Writeable() is pending, it will be canceled and the document will
   be terminated. 
   
Prototype:
   void IDOCMODEL_Stop(IDocModel *po);

Parameters:
===pre>
   p:   Pointer to the IDocModel interface object.
===/pre>
   
Return Value:
   None

Comments:  
   None

Side Effects: 
   None

See Also:
   IDOCMODEL_Write
   IDOCMODEL_Writeable
   IDOCMODEL_LoadSource
   

==========================================================================

IDOCMODEL_WriteTerminate()

Description:
   This macro function will terminate the document that is being loaded with
   Write/Writeable. This is the equivalant of receiving end-of-stream for an 
   ISource operation.

Prototype:
   int32   IDOCMODEL_WriteTerminate(IDocModel *po);

Parameters:
===pre>
   p:   Pointer to the IDocModel interface object.
===/pre>
   
Return Value:
   (int32)     0 always

Comments:  
   None

Side Effects: 
   None

See Also:
   IDOCMODEL_Write
   IDOCMODEL_Writeable



==========================================================================

IDOCMODEL_SetBufferSize()

Description:
   This function will set the buffer size of the IDocModel to minimize 
   blocking of the Write/Writeable methods. An application may use this
   function to increase the size of the buffer so that LoadSource() and
   Write() will accept and read larger data inputs at one time. 

Prototype:
   int   IDOCMODEL_SetBufferSize(IDocModel *p, int cbSize);

Parameters:
===pre>
   p:   Pointer to the IDocModel interface object.
   cbSize:  Request size of buffer in bytes.
===/pre>
   
Return Value:
   SUCCESS     Buffer size was successfully set
   ENOMEMORY   Out of memory error occurred while trying to allocate buffer.

Comments:  
   None

Side Effects: 
   None

See Also:
   IDOCMODEL_Write
   IDOCMODEL_Writeable


==========================================================================

IDOCMODEL_TO_IMODEL()

Description:
   This function provides type safe casting of an IDocModel pointer to an IModel pointer
   and should be used when passing an IDocModel pointer to functions that require a
   pointer to an IModel.  This is safer than a simple cast, since the compiler will verify
   the pointer type prior to the actual cast.

Prototype:
   IModel *IDOCMODEL_TO_IMODEL(IDocModel *p);

Parameters:
===pre>
   p:  Pointer to an IDocModel interface object.
===/pre>

Return Value:
   Returns 'p' cast to an IModel pointer.

Comments:  
   None

Side Effects: 
   None

See Also:
   IModel Interface
   IDocModel Interface
 

*/

#endif // __AEEDOCMODEL_H__

