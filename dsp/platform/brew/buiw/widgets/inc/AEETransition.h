/*
  ========================================================================

  FILE:  AEETransition.h
  
  SERVICES:  

  GENERAL DESCRIPTION: Transitions take a widget through a series of 
  steps the create a certain effect on the presentation of the widget. 
  The base class encapsulates the transision aspects while Mover and Fader 
  represent two specific transitions

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2006 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/

#ifndef __AEETRANSITION_H__
#define __AEETRANSITION_H__

#include "AEE.h"
#include "AEEXYContainer.h"

#include "bid\AEEIID_TRANSITION.bid"
 
#include "bid\AEECLSID_FADER.bid"
#include "bid\AEECLSID_MOVER.bid"

typedef struct 
{
   IWidget *piWidget;
} TransitionDesc;

typedef struct ITransition ITransition;
#define INHERIT_ITransition(iname) \
   INHERIT_IQueryInterface(iname);\
   void     (*Start)       (iname *, TransitionDesc *desc, long msecs, PFNNOTIFY pfnDone, void *pCxt);\
   void     (*Stop)        (iname *);\
   boolean  (*IsRunning)   (iname *)

AEEINTERFACE(ITransition) {
   INHERIT_ITransition(ITransition);
};

#define ITRANSITION_AddRef(p)              AEEGETPVTBL(p,ITransition)->AddRef(p)
#define ITRANSITION_Release(p)             AEEGETPVTBL(p,ITransition)->Release(p)
#define ITRANSITION_QueryInterface(p,i,p2) AEEGETPVTBL(p,ITransition)->QueryInterface(p,i,p2)
#define ITRANSITION_Start(p,d,m,f,c)       AEEGETPVTBL(p,ITransition)->Start(p,d,m,f,c)
#define ITRANSITION_Stop(p)                AEEGETPVTBL(p,ITransition)->Stop(p)
#define ITRANSITION_IsRunning(p)           AEEGETPVTBL(p,ITransition)->IsRunning(p)


/////////////////////////////////////////////////////////////////
// Fader description
typedef struct 
{
   TransitionDesc base;

   uint8 start;
   uint8 end;
} FaderDesc;

#define FADERDESC_Init(f, w, s, e) \
   ((f)->base.piWidget = (w), (f)->start = (s), (f)->end = (e))

static __inline void ITRANSITION_StartFader(ITransition *po, FaderDesc *pd, uint32 msecs, PFNNOTIFY pfnDone, void *pCxt) {
   ITRANSITION_Start(po, (TransitionDesc *)pd, msecs, pfnDone, pCxt);
}

/////////////////////////////////////////////////////////////////
// Mover description
typedef struct 
{
   TransitionDesc base;

   IXYContainer *piContainer;
   struct {
      int16 x;
      int16 y;
   } start;
   struct {
      int16 x;
      int16 y;
   } end;
} MoverDesc;

#define MOVERDESC_Init(f, w, c, sx, sy, ex, ey) \
   ((f)->base.piWidget = (w), (f)->piContainer = (c), (f)->start.x = (sx), (f)->start.y = (sy), (f)->end.x = (ex), (f)->end.y = (ey))

static __inline void ITRANSITION_StartMover(ITransition *po, MoverDesc *pd, uint32 msecs, PFNNOTIFY pfnDone, void *pCxt) {
   ITRANSITION_Start(po, (TransitionDesc *)pd, msecs, pfnDone, pCxt);
}

/*
=======================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================
Transition Interface and Class IDs

Description:
   Each object in the widget framework is identified by a unique interface ID.
   These IDs are used throughout the framework to distinguish between the various
   object and interface types, such as containers and widgets.

   The transition interface and its implementation classes support a base interface
   and the creation of class object instances.  Instances of these class objects --
   the fader and the mover -- may be created  by calling ISHELL_CreateInstance()
   and passing in the desired class id of the object.
   
   The interfaces and class IDs used to identify the transition interface and its
   associated implementation classes are defined below.

Definition:

Identifier          Definition
----------          -----------------------------------
AEEIID_TRANSITION:  Identifies the popup menu form interface
AEECLSID_FADER:     Identifies the class ID of the popup form
AEECLSID_MOVER:     Identifies the class ID of the popup menu form
   
Comments:
   None

See Also:
   ISHELL_CreateInstance()

=============================================================================

TransitionDesc

Description:
   This data type is the base object descriptor for implementations of the transition
   interface.  As the base descriptor, TransitionDesc contains only a pointer to the
   widget that will be wrapped by the transition object.  Objects that derive from the
   base transition interface (such as the Fader and the Mover) and define their own
   object descriptors will include the TransitionDesc as the first field in the descriptor
   data structure they define.

Definition:
   typedef struct TransitionDesc {
      IWidget *piWidget;
   }

Members:
===pre>
   piWidget:  Pointer to the widget that is wrapped by the transition object.  This
              widget will be the target of the transitional visual effect.
===/pre>
Comments:
   None

See Also:
   FaderDesc
   MoverDesc

=============================================================================

FaderDesc

Description:
   This data type is the object descriptor for fader class object that implements the
   transition interface.  It extends the base transition descriptor, adding fields
   specific to fader transitions, defining the levels of transparency for the wrapped
   object at the start and of the visual fade effect.  For example, a object wrapped
   in a fader could begin its fade out transition as mostly opaque, though partially
   transparent, then gradually fade to total invisibility.

Definition:
   typedef struct FaderDesc {
      TransitionDesc  base;
      uint8           start;
      uint8           end;
   }

Members:
===pre>
   base:   The base transition descriptor, which identifies those fields that are
           common to all transition decorators.  The 'base' field will contain a
           pointer to the widget that is wrapped by the fader.
   start:  This field contains the transparency factor that will be applied to the
           wrapped object at the beginning of its fade transition.  When the fade
           transition begins, the object will be redrawn at this level of transparency,
           then gradually fade until the 'end' transparency level is reached.  A value
           of 0 for 'start' would produce a fully opaque object.  A value of 1 would
           add a small degree of transparency to the widget, 2 a little more, 3 a little
           more... up to 255 which would render the object completely transparent (i.e.
           invisible!).
   end:    This field contains the final transparency level for the object to be faded
           at the conclusion of the fadeout effect.  The fade effect begins with a
           transparency level of 'start', then gradually increases until the 'end' value
           is reached. 
===/pre>
Comments:
   None

See Also:
   TransitionDesc

=============================================================================

MoverDesc

Description:
   This data type is the object descriptor for mover class object that implements the
   transition interface.  It extends the base transition descriptor, adding fields
   specific to mover transitions -- specifically, fields that define the start and end
   coordinates for the movement of the object wrapped by the mover.  To illustrate,
   an object could be moved from its present location to the origin of the display by
   specifying the object's current location as the 'start' coordinate, then specifying
   the origin of the object's root container as the 'end' coordinate.  Upon triggering
   the transition effect, the object would then follow a path that would see it visually
   "move" across the screen from its current position to the upper left hand corner of
   the display.

Definition:
   typedef struct MoverDesc {
      TransitionDesc   base;
      IXYContainer    *piContainer;
      struct {
         int16 x;
         int16 y;
      } start;
      struct {
         int16 x;
         int16 y;
      } end;
   }

Members:
===pre>
   base:         The base transition descriptor, which identifies those fields that
                 are common to all transition decorators.  The 'base' field will
                 contain a pointer to the widget that is wrapped by the mover.
   piContainer:  Pointer to the XY container object that will manage the movement of
                 the object wrapped by the mover.  The coordinates defined by the
                 'start' and 'end' fields are defined relative to the coordinates of
                 'piContainer'.
   start:        This field contains the starting (x, y) coordinates for the object
                 to be moved, relative to the container specified by 'piContainer'.
   end:          This field contains the ending (x, y) coordinates for the object
                 to be moved, relative to the container specified by 'piContainer'.
===/pre>
Comments:
   None

See Also:
   TransitionDesc


===================================================================================
   MACRO DOCUMENTATION
===================================================================================

FADERDESC_Init()

Description:
   This macro initializes each of the fields of a FaderDesc data structure.  An application
   implementing a fader transition should call FADERDESC_Init() after creating an instance
   of the fader object.

Prototype:
   FADERDESC_Init(FaderDesc *f, IWidget *w, uint8 s, uint8 e);

Parameters:
===pre>
   f:  Pointer to a FaderDesc data structure, into which the macro will copy the
       remaining parameters.
   w:  Pointer to the widget that will be the target of fader transitions.
   s:  Specifies the initial transparency level to be applied to the widget wrapped
       by the fader object when the transition is started.  A value of 0 would start
       that transition by drawing the widget as fully opaque object.  A value of 1
       would add a small degree of transparency to the widget, 2 a little more, 3 a
       little more, and so on...  A value of 255 would indicate complete transparency.
   e:  Specifies the transparency level that will end the fadeout transition.
===/pre>

Return Value:
   None

Comments:
   None

Side Effects: 
   None

See Also:
   IWidget Interface
   FaderDesc

=============================================================================

MOVERDESC_Init()

Description:
   This macro initializes each of the fields of a MoverDesc data structure.  An application
   implementing a mover transition should call MOVERDESC_Init() after creating an instance
   of the mover object.

Prototype:
   MOVERDESC_Init(MoverDesc *f, IWidget *w, IXYContainer *c, int16 sx, int16 sy, int16 ex, int16 ey);

Parameters:
===pre>
   f:   Pointer to a MoverDesc data structure, into which the macro will copy the
        remaining parameters.
   w:   Pointer to the widget that will be the target of mover transitions.
   c:   Pointer to the container object that defined the coordinate plane for the
        transition object's movements
   sx:  Specifies the starting x coordinate for the target object.  The mover will
        place the target object at this location (relative to the container, 'c') at
        the start of the move transition.
   sy:  Specifies the starting y coordinate for the target object.  The mover will
        place the target object at this location (relative to the container, 'c') at
        the start of the move transition.
   ex:  Specifies the finishing x coordinate for the target object.  The mover will
        gradually move the target object in a linear direction towards this point
        until (ex, ey) is reached.
   ey:  Specifies the finishing y coordinate for the target object.  The mover will
        gradually move the target object in a linear direction towards this point
        until (ex, ey) is reached.
===/pre>

Return Value:
   None

Comments:
   None

Side Effects: 
   None

See Also:
   IWidget Interface
   MoverDesc

=============================================================================

===========================================================================
 INTERFACES DOCUMENTATION
===========================================================================

ITransition Interface

Description:
   The base transition interface is an abstract interface from which specific transition
   implementations should be based.  The transition interface is a wrapper around another
   object, providing a special visual effect to the object it wraps.  For example, a
   transition implementation could be written that causes a wrapped object to grow or shrink
   over some period of time.  Likewise, a transition could create the effect of an object
   disappearing as a cloud of smoke, or flying around the screen in accordance to some wild
   path of vectors.  Unfortunately, none of these "for example" transitions exist, so it is
   left to the imagination of the developer to write these or other cool transition effects.
   But fear not, two real transitions _do_ actually exist -- the fader transition and the
   mover transition -- each of which is described below.
   
   Each transition implementation is defined by its effect and duration.  For example, the
   mover transition has the effect of moving a wrapped object from one point to another over
   a specific period of time.  Likewise, the fader transition has the effect of applying a
   fade out effect to a wrapped object, gradually over some period of time.  The interface
   provides APIs for starting and stopping a transition effect, checking to see if the effect
   is currently running, and supplying notification at the completion of a transition effect.
   
   This document describes a pair of transition implementations -- the fader and mover -- each
   of which is used to provide a specific visual effect to a wrapped object.  Descriptions 
   of each transition implementation follow:

===h3>
Fader Transition
===/h3>
   The fader transition is a transition that applies a visual fade out effect to the widget it
   wraps.  For example, a popup menu could be wrapped by a fader.  Once displayed, the fader
   could be instructed to fade out the wrapped popup over some period of time.  As the fade
   out effect begins, the wrapped popup would be drawn with an initial level of transparency
   (which could, initially, be fully opaque), and then would gradually lose opacity until it
   reached a final level of transparency.
   
   To create a fader transition, an application would do the following:
===pre>
       1. Create the object that will be the target of the fader transition
       2. Call ISHELL_CreateInstance() with a class ID of AEECLSID_FADER to retrieve a
          reference counted instance of the fader transition.
       3. Call FADERDESC_Init() to initialize the fader and identify the widget it is to
          wrap.
       4. Call ITRANSITION_StartFader() to begin the fadeout transition on the widget it
          wraps.
===/pre>

===h3>
Mover Transition
===/h3>
   The mover transition is a transition that animates an object across the display.  For
   example, an application could wrap a mover transition around an image widget, then begin
   moving that images vertically, horizontally or diagonally across the display, specifying
   each movement as a series of point-to-point destinations.  The mover transition places
   the target object at a particular location (relative to a container that defines the
   movement's coordinate plane), then gradually moves the target object in a linear manner
   to a final destination location.
   
   To create a mover transition, an application would do the following:
===pre>
       1. Create the object that will be the target of the mover transition
       2. Call ISHELL_CreateInstance() with a class ID of AEECLSID_MOVER to retrieve a
          reference counted instance of the mover transition.
       3. Call MOVERDESC_Init() to initialize the fader mover identify the widget it is to
          wrap.
       4. Call ITRANSITION_StartFader() to begin the movement transition on the widget it
          wraps.
===/pre>

Supported Events:
   None.  The transition interface does not process any BREW events.  It is intended as an
   interface that applies a visual effect to another object.

Properties:
   None

Required Model:
   None
   
Model Data:
   None

Comments:
   None
   
See Also:
   ISHELL_CreateInstance()

===========================================================================

ITRANSITION_AddRef()

Description:

   This function increments the reference count of the ITransition interface object, allowing
   the interface to be shared by multiple callers.  The object is freed when the reference
   count reaches 0 (zero).

Prototype:

   uint32 ITRANSITION_AddRef(ITransition *p);

Parameters:
===pre>
   p:  Pointer to the ITransition interface object
===/pre>

Return Value:
   Returns the incremented reference count for the ITransition interface object.

Comments:
   None

Side Effects:
   None

See Also:
   ITRANSITION_Release()
   
=======================================================================

ITRANSITION_Release()

Description:

   This function decrements the reference count of the ITransition interface object.  The
   object is freed from memory and is no longer valid once its reference count reaches 0 (zero).

Prototype:

   uint32 ITRANSITION_Release(ITransition *p);

Parameters:
===pre>
   p:  Pointer to the ITransition interface object
===/pre>

Return Value:
   Returns the decremented reference count for the ITransition interface object.  If the returned
   reference count is 0 (zero), the object has been freed and will no longer be valid.

Comments:
   None

Side Effects:
   None

See Also:
   ITRANSITION_AddRef()

=======================================================================

ITRANSITION_QueryInterface()

Description: 
   This function queries an interface for an alterrnate interface that may provide
   extended functionality or a proprietary extension.  If successful, the function
   will return a pointer to the requested interface in the memory location pointed
   to by the 'd' parameter.  The requested interface is identified by a globally
   unique ID passed to the function in the 'c' parameter.

Prototype:

   int ITRANSITION_QueryInterface(ITransition *p, AEECLSID c, void **d)

Parameters:
===pre>
   p:  Pointer to the ITransition interface object.
   c:  A globally unique id that identifies the interface to be queried.
   d:  Pointer to a pointer returned by the function to reference the
       interface requested by the 'c' parameter.  If the queried item is not
       available, this pointer is returned as NULL.
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

=======================================================================

ITRANSITION_Start()

Description: 
   This function starts a particular transition effect.  Prior to starting the transition
   an application should first initialize the TransitionDesc data structure associated
   with the given transition (FaderDesc, MoverDesc, etc) to identify the target of the
   transition effect and any parameters specific to that effect.  Once started, the effect
   will continue until 'm' milliseconds have passed, or until the effect has been stopped
   by calling ITRANSITION_Stop().  At the conclusion of the transition effect (whether it
   has run to completion or has been terminated early), the transition completion
   notification callback passed in 'f' will be issued, providing the application with an
   opportunity to perform any special processing at the end of the effect.

Prototype:

   void ITRANSITION_Start(ITransition *p, TransitionDesc *d, long m, PFNNOTIFY f, void *c)

Parameters:
===pre>
   p:  Pointer to the ITransition interface object.
   d:  Pointer to the TransitionDesc data structure that identifies the widget
       to be the target of the transition, as well as any parameters specifically
       required by a particular implementation of the transition interface -- for
       example, those parameters required by the fader or mover transitions.
   m:  Defines the duration of the transition effect, expressed in milliseconds.
       For example, a 2 second fade out transition would pass 2000 in the 'm'
       parameter.
   f:  Pointer to the callback function that BREW will issue when the transition effect
       has ended -- whether it has run its entire course to completion, or the effect
       has been terminated by a call to ITRANSITION_Stop().  Applications that do not
       wish to receive notification following the completion of a transition effect may
       pass NULL for 'f', and no callback will be registered.
       
       The callback function prototype takes the following form:
       
           typedef void (* PFNNOTIFY)(void * pData);
       
       Where 'pData' is passed to the callback as the same pointer passed into the
       ITRANSITION_Start() function in the 'c' parameter.  This is a pointer to data
       owned and managed entirely by the caller.
   c:  Pointer to private data that is created and owned by the caller.  This parameter
       will be passed to the notification completion routine, 'f', when the transition
       effect has finished or has been terminated.
===/pre>

Return Value:
   None

Comments:
   None

Side Effects: 
   None

See Also:
   ITRANSITION_StartFader()
   ITRANSITION_StartMover()
   ITRANSITION_Stop()

=======================================================================

ITRANSITION_Stop()

Description: 
   This function prematurely terminates a transition effect before its timer has expired.
   Stopping a transition will immediately freeze whatever visual effect was currently in
   progress and (if such a callback is registered) trigger the transition completion
   notifier registered with the transition interface in the call to ITRANSITION_Start().

Prototype:

   void ITRANSITION_Stop(ITransition *p)

Parameters:
===pre>
   p:  Pointer to the ITransition interface object.
===/pre>

Return Value:
   None

Comments:
   Stopping a transition effect that is in progress does _not_ remove the transitioning
   object from the display.  For example, an application that wishes manages a fadeout
   effect for a popup form might call ITRANSITION_Stop() if the user presses the CLEAR
   button on their devices, intending to dismiss the popup from the display.  ITRANSITION_Stop()
   merely halts the visual effect of the transition -- in this case leaving the popup in
   some partially faded state.  It would then be up to the application to perform whatever
   operation necessary to remove the object from the display.

Side Effects: 
   None

See Also:
   ITRANSITION_Start()

=======================================================================

ITRANSITION_IsRunning()

Description: 
   This function tests to see whether or not a transition effect is currently in the
   process of performing its visualization effect, returning TRUE when the effect is
   active or FALSE when it is not.

Prototype:

   void ITRANSITION_IsRunning(ITransition *p)

Parameters:
===pre>
   p:  Pointer to the ITransition interface object.
===/pre>

Return Value:
===pre>
   TRUE  --  The transition effect is currently in progress.
   FALSE --  The transition effect is not in progress.
===/pre>

Comments:
   The transition effect is considered to be running after a call to ITRANSITION_Start()
   and before either the timer passed into ITRANSITION_Start() has expired, or before any
   call to ITRANSITION_Stop().

Side Effects: 
   None

See Also:
   ITRANSITION_Start()
   ITRANSITION_Stop()

=======================================================================

ITRANSITION_StartFader()

Description: 
   This function starts a fader transition effect.  Prior to starting the transition an
   application should first initialize the FaderDesc data structure to identify the target
   of the fadeout transition and any parameters specific to fader transitions.  Once started,
   the fade effect will continue until 'msecs' milliseconds have passed, or until the
   effect has been stopped by calling ITRANSITION_Stop().  At the conclusion of the fade out
   (whether it  has run to completion or has been terminated early), the transition completion
   notification callback passed in 'pfnDone' will be issued, providing the application with an
   opportunity to perform any special processing at the end of the effect.

Prototype:

   void ITRANSITION_StartFader(ITransition *po, FaderDesc *pd, uint32 msecs, PFNNOTIFY pfnDone, void *pCxt)

Parameters:
===pre>
   po:       Pointer to the ITransition interface object.
   pd:       Pointer to the FaderDesc data structure that identifies the widget to
             be the target of the transition, as well as the starting and ending
             transparency values for that widget as it undergoes its fadeout.
   msecs:    Defines the duration of the fadeout effect, expressed in milliseconds.
             For example, a 2 second fade out transition would pass 2000 in this
             parameter.
   pfnDone:  Pointer to the callback function that BREW will issue when the fadeout
             has ended -- whether it has run its entire course to completion, or
             the effect has been terminated by a call to ITRANSITION_Stop().
             Applications that do no wish to receive notification following the
             completion of a fader transition may pass NULL for 'pfnDone', and no
             callback will be registered.
       
             The callback function prototype takes the following form:
       
                 typedef void (* PFNNOTIFY)(void * pData);
       
             Where 'pData' is passed to the callback as the same pointer passed into
             the ITRANSITION_Start() function in the 'pCxt' parameter.  This is a pointer
             to data owned and managed entirely by the caller.
   pCxt:     Pointer to private data that is created and owned by the caller.  This
             parameter will be passed to the notification completion routine, 'pfDone',
             when the fadeout transition has finished or has been terminated.
===/pre>

Return Value:
   None

Comments:
   None

Side Effects: 
   None

See Also:
   ITRANSITION_Start()
   ITRANSITION_Stop()

=======================================================================

ITRANSITION_StartMover()

Description: 
   This function starts a mover transition effect.  Prior to starting the transition an
   application should first initialize the MoverDesc data structure to identify the target
   of the transition effect and any parameters specific to mover transitions.  Once started,
   the move transition will continue until 'msecs' milliseconds have passed, or until the
   effect has been stopped by calling ITRANSITION_Stop().  At the conclusion of the mover
   effect (whether it  has run to completion or has been terminated early), the transition
   completion notification callback passed in 'pfnDone' will be issued, providing the
   application with an opportunity to perform any special processing at the end of the effect.

Prototype:

   void ITRANSITION_StartMover(ITransition *po, MoverDesc *pd, uint32 msecs, PFNNOTIFY pfnDone, void *pCxt)

Parameters:
===pre>
   po:       Pointer to the ITransition interface object.
   pd:       Pointer to the MoverDesc data structure that identifies the widget to
             be the target of the transition, as well as the starting and ending
             coordinates for that widget as it undergoes it is moved within its
             managing container.
   msecs:    Defines the duration of the movement effect, expressed in milliseconds.
             For example, a 2 second movement transition would pass 2000 in this
             parameter.
   pfnDone:  Pointer to the callback function that BREW will issue when the move
             transition has ended -- whether it has run its entire course to, or
             the effect has been terminated by a call to ITRANSITION_Stop().
             Applications that do no wish to receive notification following the
             completion of a mover transition may pass NULL for 'pfnDone', and no
             callback will be registered.
       
             The callback function prototype takes the following form:
       
                 typedef void (* PFNNOTIFY)(void * pData);
       
             Where 'pData' is passed to the callback as the same pointer passed into
             the ITRANSITION_Start() function in the 'pCxt' parameter.  This is a pointer
             to data owned and managed entirely by the caller.
   pCxt:     Pointer to private data that is created and owned by the caller.  This
             parameter will be passed to the notification completion routine, 'pfDone',
             when the mover transition has finished or has been terminated.
===/pre>

Return Value:
   None

Comments:
   None

Side Effects: 
   None

See Also:
   ITRANSITION_Start()
   ITRANSITION_Stop()

*/
#endif /* __AEETRANSITION_H__ */
