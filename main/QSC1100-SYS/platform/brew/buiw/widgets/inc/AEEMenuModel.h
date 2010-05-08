/*
  ========================================================================

  FILE:  AEEMenuModel.h
  
  SERVICES: 

  GENERAL DESCRIPTION: I define a model that represents one level in a
  menu. A MenuModel understands IListModel (through
  QueryInterface). The resulting ListModel represents all enabled
  menuitems

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/


#ifndef __AEEMENUMODEL_H__
#define __AEEMENUMODEL_H__

#include "AEEWModel.h"

#include "bid/AEEIID_MENUMODEL.bid"

#include "bid/AEECLSID_MENUMODEL.bid"

#define MENUINDEX_LAST (uint16)-1
#define MENUITEM_ALL   (uint16)-2

// Supported flags for menu items
#define MMF_ENABLED            0x0001     // item is enabled
#define MMF_SUBMENU            0x0002     // item is submenu
#define MMF_SEPARATOR          0x0004     // item has a separator bar under it
#define MMF_ALL                0xFFFF     // all flags

/////////////////////////////////////////////////////////////////
// IMenuModel interface

typedef struct IMenuModel IMenuModel;
#define INHERIT_IMenuModel(iname) \
   INHERIT_IModel(iname); \
   int         (*InsertItem)     (iname *po, void *pvItem, uint16 idCmd, uint16 flags, uint16 index); \
   int         (*GetItemIndex)   (iname *po, uint16 idCmd); \
   int         (*SetItemFlags)   (iname *po, uint16 idCmd, uint16 flags, boolean bSet); \
   int         (*GetAt)          (iname *po, uint16 index, uint16 flagFilter, void **ppo, uint16 *pFlags, uint16 *idCmd); \
   int         (*DeleteAt)       (iname *po, uint16 index); \
   void        (*DeleteAll)      (iname *po); \
   int         (*Size)           (iname *po);\
   PFNNOTIFY   (*SetPfnFree)     (iname *po, PFNNOTIFY pfnFree)


AEEINTERFACE(IMenuModel) {
   INHERIT_IMenuModel(IMenuModel);
};

#define IMENUMODEL_AddRef(p)              AEEGETPVTBL((p),IMenuModel)->AddRef(p)
#define IMENUMODEL_Release(p)             AEEGETPVTBL((p),IMenuModel)->Release(p)
#define IMENUMODEL_QueryInterface(p,c,d)  AEEGETPVTBL((p),IMenuModel)->QueryInterface(p,c,d)
#define IMENUMODEL_AddListener(p,pl)      AEEGETPVTBL((p),IMenuModel)->AddListener(p,pl)
#define IMENUMODEL_Size(p)                AEEGETPVTBL((p),IMenuModel)->Size(p)
#define IMENUMODEL_InsertItem(p,pv,c,e,i) AEEGETPVTBL((p),IMenuModel)->InsertItem(p,pv,c,e,i)
#define IMENUMODEL_GetItemIndex(p,c)      AEEGETPVTBL((p),IMenuModel)->GetItemIndex(p,c)
#define IMENUMODEL_SetItemFlags(p,c,e,b)  AEEGETPVTBL((p),IMenuModel)->SetItemFlags(p,c,e,b)
#define IMENUMODEL_GetAt(p,i,f,ppo,e,c)   AEEGETPVTBL((p),IMenuModel)->GetAt(p,i,f,ppo,e,c)
#define IMENUMODEL_DeleteAt(p,i)          AEEGETPVTBL((p),IMenuModel)->DeleteAt(p,i)          
#define IMENUMODEL_DeleteAll(p)           AEEGETPVTBL((p),IMenuModel)->DeleteAll(p)         
#define IMENUMODEL_SetPfnFree(p,pfn)      AEEGETPVTBL((p),IMenuModel)->SetPfnFree(p,pfn)

#define IMENUMODEL_Add(p,pv,c,e)          AEEGETPVTBL((p),IMenuModel)->InsertItem(p,pv,c,e,MENUINDEX_LAST)

static __inline IModel *IMENUMODEL_TO_IMODEL(IMenuModel *me) {
   return (IModel *)(void*)me;
}


/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================
Menu Model Interface and Class IDs

Description:
   Each model is identified by a unique interface ID.  These IDs are used
   throughout the model framework to indicate the model that a given object
   will use to describe the data associated with that object.
   
   The menu model supports both an interface and the creation of class object
   instances.  The menu model interface or a new instance of the menu model
   class object may be created by calling ISHELL_CreateInstance() and passing
   in the desired class id of the object.  The object that is created is,
   itself, a model and may utilize those APIs defined for either the model or
   menu model interfaces.

   The menu model recognizes the following set of class and interface IDs.

Definition:

Identifier           Definition
----------           -----------------------------------
AEEIID_MENUMODEL:    Identifies the menu model interface
AEECLSID_MENUMODEL:  Identifies the class ID of the menu model

Comments:
   None

See Also:
   IMenuModel Interface
   ISHELL_CreateInstance()
   IWIDGET_GetModel()
   IWIDGET_SetModel()

===========================================================================
 INTERFACES DOCUMENTATION
===========================================================================

IMenuModel Interface

Description:
   IMenuModel is derived from the IListModel interface and is used to model a collection of
   objects, each of which is considered to be an item in a menu.  Typically, the menu
   model is used to manage the storage of data for a user interface object that takes the
   appearance of a menu -- such as a popup menu form.  As such, the menu model stores (as
   one might guess) menu item data from which a menu may be easily built, maintaining that data
   in the order that it would appear in a menu on the display.  Conceptually, the data stored
   in a menu model would look like this:

===pre>
                    Menu
         +--------------------------+   
         |                          |  Menu model index 0
         |        Menu Item 0       |
         |                          |
         |--------------------------|   
         |                          |  Menu model index 1
         |        Menu Item 1       |
         |                          |
         |--------------------------|
         |                          |  Menu model index 2
         |        Menu Item 2       |
         |                          |
         |--------------------------|
                      :
                      :
         |--------------------------|
         |                          |  Menu model index (n - 1)
         |     Menu Item n - 1      |
         |                          |
         +--------------------------+
===/pre>
   
   In general (though depending on the object that is attached to the menu model), the items 
   in the menu model will be displayed top-to-bottom from items 0 through item (n - 1), with
   item zero being at the top of the menu and item (n - 1) at the bottom.
      
   The data storage mechanism used by the menu model is (from the standpoint of clients using
   the menu model) opaque.  The data itself could be arbitrarily complex, depending on the
   capabilities and expectations of the object that has attached to the model.  For example,
   the menu model could store each item as text that an attached user interface object might
   display to the user as a standard text menu.  The data could just as easily be stored as
   a binary bitmap, which a dutifully equipped user interface object could expose as a menu
   containing colorful pictures, rather than text.  A third example could see each menu item
   stored as a complex data structure, which an attached object could utilize to display
   each item however it pleases.  For example, a widget could be created that displays menu
   items as a spinning wheel, rather than as a vertical list.  Each item on the wheel could
   correspond to an item stored in the menu model, perhaps as a structure containing letters,
   vowels, dollar figures and VALUABLE PRIZES!
   
===pre>
      Menu Model.
      Of.
      Fortune!
===/pre>
   
   The menu model interface provides APIs for accessing or modifying the data in a very
   straightforward manner that satisfies the requirements of BREW's menu capabilities, as
   well as routines for managing reference counting of the interface object, and functions
   that allows widgets and UI controls to register for a listener with the menu model.

See Also:
   IForm Interface
   IPopupMenu Interface

=======================================================================

IMENUMODEL_AddRef()

Description:

   This function increments the reference count of the IMenuModel interface object, allowing
   the interface to be shared by multiple callers.  The object is freed when the reference
   count reaches 0 (zero).

Prototype:

   uint32 IMENUMODEL_AddRef(IMenuModel *p);

Parameters:
===pre>
   p:  Pointer to the IMenuModel interface object
===/pre>

Return Value:
   Returns the incremented reference count for the IMenuModel interface object.

Comments:
   None

Side Effects:
   None

See Also:
   IMENUMODEL_Release()
   
=======================================================================

IMENUMODEL_Release()

Description:

   This function decrements the reference count of the IMenuModel interface object.  The
   object is freed from memory and is no longer valid once its reference count reaches 0 (zero).

Prototype:

   uint32 IMENUMODEL_Release(IMenuModel *p);

Parameters:
===pre>
   p:  Pointer to the IMenuModel interface object
===/pre>

Return Value:
   Returns the decremented reference count for the IMenuModel interface object.  If the
   returned reference count is 0 (zero), the object has been freed and will no longer be valid.

Comments:
   None

Side Effects:
   None

See Also:
   IMENUMODEL_AddRef()

=======================================================================

IMENUMODEL_QueryInterface()

Description: 
   This function queries an interface for an alterrnate interface that may provide
   extended functionality or a proprietary extension.  If successful, the function
   will return a pointer to the requested interface in the memory location pointed
   to by the 'd' parameter.  The requested interface is identified by a globally
   unique ID passed to the function in the 'c' parameter.

Prototype:

   int IMENUMODEL_QueryInterface(IMenuModel *p, AEECLSID c, void **d)

Parameters:
===pre>
   p:  Pointer to the IMenuModel interface object.
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
   
==========================================================================

IMENUMODEL_AddListener()

Description:
   This function allows objects such as widgets and UI controls to attach a listener to the given
   menu model.  The listener includes a callback function that BREW will call whenever the model has
   changed state, thereby notifying the owner of the listener that data has changed.  For example,
   consider a menu model that manages the items in a popup menu whose items change state depending
   on some external context.  A list widget could register as a listener of the menu model by calling
   IMENUMODEL_AddListener().  As the menu items change and are stored into the menu model, BREW will
   send out notifications to all registered objects, providing them the opportunity to update their
   display or react in some other manner.  The list widget implementing the popup menu, for example,
   as a listener of the menu model, might update the text of the displayed menu on the fly.

Prototype:
   int IMENUMODEL_AddListener(IListModel *p, ModelListener pl); 

Parameters:
===pre>
   p:   Pointer to the IListModel interface object.
   pl:  Pointer to the ModelListener that will be attached to the object.  The ModelListener will
        contain the PFNLISTENER callback function BREW will call to notify the registering object
        of changes to the value model.
===/pre>
   
Return Value:
===pre>
   SUCCESS          - Listener successfully added
   EBADPARM         - Invalid argument
   ENOMEMORY        - Insufficient memory
===/pre>

Comments:
   None

Side Effects: 
   None

See Also:
   IListModel Interface
   ModelListener
   ILISTMODEL_AddListener()
   PFNLISTENER
   
===========================================================================

IMENUMODEL_Size()

Description:
   This function retrieves the size of a given menu model, returned as the number of menu
   items stored in the model.  A menu model containing three menu items (red, green, blue)
   would return 3, while a menu containing four menu items (John, Paul, George, Ringo) would
   return 4.  Simple!

Prototype:
   int IMENUMODEL_Size(IMenuModel *p); 

Parameters:
===pre>
   po:  Pointer to the IMenuModel interface object.
===/pre>

Return Value:
   The current number of items stored in the menu model.

Comments:
   None

Side Effects: 
   None

See Also:
   IMenuModel Interface

===========================================================================


IMENUMODEL_InsertItem()

Description:
   This function inserts a new menu item into the menu model, inserting the new item
   into the existing menu at the location indicated by the 'i' parameter, which is a
   zero-based index value for specifying a position within the menu.  Note that this
   inserts the new item at this location, moving all other items down in the menu
   model's data store.  For example, with three menu items stored in the menu model,
   like so:
   
===pre>
         +--------------------------+   
         |                          |  
         |           Yes            |  Menu model index 0
         |                          |
         |--------------------------|   
         |                          |  
         |            No            |  Menu model index 1
         |                          |
         |--------------------------|
         |                          |  
         |          Maybe           |  Menu model index 2
         |                          |
         +--------------------------+
===/pre>
   
   Inserting a fourth item at index location 1 would move the items currently stored
   at index positions 1 and 2 "down" one spot, to locations 2 and 3, like so:
    
   
===pre>
         +--------------------------+   
         |                          |  
         |           Yes            |  Menu model index 0
         |                          |
         |--------------------------|   
         |                          |  
         |      Can't Decide        |  Menu model index 1  <--- Inserted item
         |                          |
         |--------------------------|   
         |                          |  
         |            No            |  Menu model index 2  <--- Moved from 1
         |                          |
         |--------------------------|
         |                          |  
         |          Maybe           |  Menu model index 3  <--- Moved from 2
         |                          |
         +--------------------------+
===/pre>
   
   To build a brand new menu model from top to bottom, an application would call
   IMENUMODEL_InsertItem() for each item, passing MENUINDEX_LAST in the 'i'
   parameter to indicate that the new item should be added at the end of the
   menu model.
   
   Though the example above assumes a user interface object that displays each item
   stored within the menu model as text, the menu model is capable of storing any
   arbitrarily complex data.  It is the responsibility of the object attached to the
   menu model to expose the stored data in a manner that makes sense for that object.

Prototype:
   int IMENUMODEL_InsertItem(IMenuModel *p, void *pv, uint16 c, uint16 e, uint16 i); 

Parameters:
===pre>
   p:   Pointer to the IMenuModel interface object.
   pv:  Pointer to the data that defines the content for this item.  How this
        data is exposed on the display is up to the object that has attached
        itself to the menu model.  The data could be as simple as a text string
        or as complex a lengthy data structure.
   c:   The command ID assigned to this item.
   e:   A set of flags that indicate various state, display or behavioral attributes
        for the menu item.  The defined menu item flags are:
             MMF_ENABLED    -  When this flag is ON, the menu item is currently enabled.
             MMF_SUBMENU    -  When this flag is ON, the menu item is a submenu.
             MMF_SEPARATOR  -  When this flag is ON, the menu item should be drawn
                               with a menu separator.  For traditional vertical menus,
                               this is usually drawn as a line under the menu text.
                               However, the separator could be rendered in whatever
                               manner is most appropriate for the object attached to
                               the menu model.
   i:   Indicates the zero-based index of where the new item should be inserted in the
        menu model.  The first item stored in the menu is indexed by 0, the second is
        indexed by 1, and so on...  When the index for an existing menu item is specified,
        the new item is inserted into the menu model before the item specified by 'i'.
        Pass MENUINDEX_LAST to indicate that the new menu item is to be added at the last
        position in the menu model.
        
===/pre>

Return Value:
===pre>
   SUCCESS    - The item was successfully added to the menu model.
   ENOMEMORY  - The menu model was unable to allocate enough memory to accommodate the new item,
                or the index passed in 'i' is outside the range of the data stored in the model.
===/pre>

Comments:
   None

Side Effects: 
   Successfully, inserting a new item in a menu model by calling IMENUMODEL_InsertItem() will
   cause BREW to send an EVT_MDL_LIST_CHANGE ListModelEvent to any object that has registered
   as a listener with the model.

   The fields of the ListModelEvent will contain the following information:
===pre>
       pos:      The index of the menu item that has been inserted into the model. 
       oldSize:  The number of items that were in the menu model before adding
                 the new item.
       newSize:  The number of items that are now stored in the menu model after
                 adding the new item.
===/pre>

See Also:
   Model Events
   IMenuModel Interface
   IMENUMODEL_Add()

===========================================================================

IMENUMODEL_GetItemIndex()

Description:
   This function returns the index of the menu item stored in the menu model that
   matches a given menu command ID.  Each item stored in the menu model is associated
   with a specific command ID that an attached object could use as a trigger in
   response to a user action -- for example, when the user selects a particular menu
   item, that command ID could be generated.  IMENUMODEL_GetItemIndex() provides a
   mechanism for translating a command ID back into an index, so the application could
   (for example) dip back into the menu model to retrieve the menu item represented
   by that command ID.

Prototype:
   int IMENUMODEL_GetItemIndex(IMenuModel *p, uint16 c); 

Parameters:
===pre>
   p:  Pointer to the IMenuModel interface object.
   c:  The command ID assigned to the item whose index is to be retrieved.
===/pre>

Return Value:
  This function returns the zero-based index of the menu item with command ID 'c', or -1
  if the menu model does not contain an item matching this ID.
 
Comments:
   Menu command IDs should be unique.  IMENUMODEL_GetItemIndex() will return the index of
   the last stored item in the menu model that matches the command ID passed in 'c'.

Side Effects: 
   None

See Also:
   IMenuModel Interface

===========================================================================

IMENUMODEL_GetAt()

Description:
   This function retrieves the data for a menu item stored within the menu model.
   The function may either be used to retrieve an item from a specific zero-based
   index location within the model, or to retrieve the item that matches a specific
   set of menu item attribute flags, such as the third enabled item, or the first
   submenu.  When the flag filter, 'f', is passed as MENUITEM_ALL, the data of the
   item stored at index location 'i' will be returned.  When a specific set of flags
   is identified, 'i' indicates the ordinal position of the item to be returned,
   matching at least one of the set flags.
   
   To illustrate, consider the following information about cool bands, lame bands,
   and various living and dead rock stars stored within a menu model:
   
===pre>
                              Menu Model
                     +--------------------------+   
                     |                          |  
          Index 0    | Rage Against The Machine |  Enabled
                     |                          |
                     |--------------------------|   
                     |                          |  
          Index 1    |     Bruce Springsteen    |  Enabled
                     |                          |
                     |--------------------------|   
                     |                          |  
          Index 2    |       Jimi Hendrix       |  Disabled (he's dead, Jim)
                     |                          |
                     |--------------------------|   
                     |                          |  
          Index 3    |       Janis Joplin       |  Disabled (dead again)
                     |                          |
                     |--------------------------|   
                     |                          |  
          Index 4    |        The Pixies        |  Enabled, with Separator
                     |    ------------------    |
                     |--------------------------|   
                     |                          |  
          Index 5    |           ABBA           |  Enabled
                     |                          |
                     |--------------------------|
                     |                          |  
          Index 6    |   Captain And Tennille   |  Enabled
                     |                          |
                     +--------------------------+
===/pre>

   From this menu model an application could request to directly retrieve the item
   stored at index location 3, as follows:
   
===pre>
       IMENUMODEL_GetAt (pMenuModel, 3, MENUITEM_ALL, &pMenuData, &flags, &idCmd);
===/pre>

   Called in this fashion, the function would retrieve the menu item data stored at
   index 3 within the model ("Janis Joplin").
   
   Using the same menu model, an application would retrieve the third enabled menu 
   item by calling the function with a flag filter, as follows:
   
===pre>
       IMENUMODEL_GetAt (pMenuModel, 2, MMF_ENABLED, &pMenuData, &flags, &idCmd);
===/pre>

   Here, with the index set to 2 (remember, indices are zero-based, so the third item
   is accessed using 2) and the flag filter set to MMF_ENABLED, the function would
   retrieve the third menu item that has the enabled flag set.  In this case, this would
   be the item for "The Pixies".

Prototype:
   int IMENUMODEL_GetAt(IMenuModel *p, uint16 i, uint16 f, void **ppo, uint16 *e, uint16 *c); 

Parameters:
===pre>
   p:    Pointer to the IMenuModel interface object.
   i:    When the flag filter 'f' is passed as MENUITEM_ALL, this parameter indicates the
         zero-based index of the item to be retrieved from the menu model.  When 'f' specifies
         an explicit set of menu flags, 'i' specifies which of the matching items is to be
         returned.  For example, if the function is called with the flag filter set to only
         look for enabled items, 'i' will indicate which of these enabled items the function
         will retrieve.  If there are 4 enabled items and 'i' is passed as 1, the second of
         these four enabled items will be returned.
   f:    A set of menu item attribute flags that provide a filter for the item to be returned.
         When 'f' is passed as MENUITEM_ALL, the function will return the item specified by
         the 'i' parameter, from the set of all stored menu items.  When 'f' specifies one or
         more menu item attributes, the function considers only those items that match at
         least one of the set filter flags, using 'i' as an index over only those matching
         items.  The defined menu item filter flags are:
               MMF_ENABLED    -  Retrieves only those items that are currently enabled.
               MMF_SUBMENU    -  Retrieves only those items that are submenus.
               MMF_SEPARATOR  -  Retrieves only those items that have a separator bar.
   ppo:  Pointer to a void pointer into which the function will return a pointer to the data
         that defines the content for this item.
   e:    Pointer into which the function will return the attribute flags for the menu item
         being retrieved.  The defined attribute flags are:
             MMF_ENABLED    -  When this flag is ON, the menu item is currently enabled.
             MMF_SUBMENU    -  When this flag is ON, the menu item is a submenu.
             MMF_SEPARATOR  -  When this flag is ON, the menu item will be drawn with
                               a menu separator.  For traditional vertical menus, this
                               is usually drawn as a line under the menu text.  However,
                               the separator could be rendered in whatever manner is
                               most appropriate for the object attached to the menu model.
   c:    Pointer into which the function will return the command ID assigned to the menu
         item being retrieved.
===/pre>

Return Value:
===pre>
   SUCCESS  - The function was able to successfully retrieve the requested item from
              the model interface.
   EBADPARM - The requested index is not within the range of items stored in the model.
===/pre>
   
Comments:
   The pointer passed in the 'ppo' parameter is a double-indirection -- i.e. it is a
   pointer to a pointer.  Applications should use care in ensuring that the proper level
   of indirection is used in calling the function.

Side Effects: 
   None

See Also:
   IMenuModel Interface
   IMENUMODEL_InsertItem()
   IMENUMODEL_GetItemIndex()
   IMENUMODEL_SetItemFlags()

===========================================================================

IMENUMODEL_SetItemFlags()

Description:
   This function changes the menu item attribute flags for a single menu item, which is
   identified by its command ID -- or all of the items stored in a menu model.  The flags
   to be changed are identified in the 'e' parameter as a bit mask specifying which
   individual flags are to be changed, or by passing MMF_ALL to indicate that all the flags
   should change.  The 'b' parameter indicates whether the specified flags should be set
   ('b' is TRUE) or cleared ('b' is FALSE).  Flags not specified in the 'e' parameter will
   not be affected by a call to IMENUMODEL_SetItemFlags().

Prototype:
   int IMENUMODEL_SetItemFlags(IMenuModel *p, uint16 c, uint16 e, boolean b); 

Parameters:
===pre>
   p:  Pointer to the IMenuModel interface object.
   c:  The command ID assigned to the item to be changed, or MENUITEM_ALL to indicate
       that all the items stored by the menu model should be changed.
   e:  A set of flags that indicate various state, display or behavioral attributes
       for the menu item.  The defined menu item flags are:
            MMF_ENABLED    -  When this flag is ON, the menu item is currently enabled.
            MMF_SUBMENU    -  When this flag is ON, the menu item is a submenu.
            MMF_SEPARATOR  -  When this flag is ON, the menu item should be drawn
                              with a menu separator.  For traditional vertical menus,
                              this is usually drawn as a line under the menu text.
                              However, the separator could be rendered in whatever
                              manner is most appropriate for the object attached to
                              the menu model.
   b:  This boolean value specifies how the attributes identified in the 'e' will
       effect the current flag settings for the menu item(s).  When 'b' is TRUE, the
       flags in 'e' will all be set.  When 'b' if FALSE, the flags in 'e' will be
       cleared.
===/pre>

Return Value:
===pre>
   SUCCESS   - The model was able to successfully set the specified flags
   EBADPARM  - The requested index is not within the range of items stored in the model.
===/pre>

Comments:
   To illustrate the effects of the MENUITEM_ALL constant, all of the items stored by a menu
   model could be disabled by calling IMENUMODEL_SetItemFlags() and passing MENUITEM_ALL for
   the 'c' parameter, MMF_ENABLED for the 'e' parameter, and FALSE for the 'b' parameter.

===pre>
       IMENUMODEL_SetItemFlags (pMenuModel, MENUITEM_ALL, MMF_ENABLED, FALSE);
===/pre>
   
   Likewise, to enable all of the items in a menu model, IMENUMODEL_SetItemFlags() could
   be called as follows:

===pre>
       IMENUMODEL_SetItemFlags (pMenuModel, MENUITEM_ALL, MMF_ENABLED, TRUE);
===/pre>
   

Side Effects: 
   None

See Also:
   IMenuModel Interface

===========================================================================

IMENUMODEL_DeleteAt()

Description:
   This function deletes an existing item from a menu model, releasing the memory occupied
   by that item and its data content pointer.  

Prototype:
   int IMENUMODEL_DeleteAt(IMenuModel *p, uint16 i);

Parameters:
===pre>
   p:  Pointer to the IMenuModel interface object.
   i:  Indicates the zero-based index of the item to be removed from the menu model.  The
       first item in the model is indexed by 0, the second is indexed by 1, and so on.  So,
       if IMENUMODEL_DeleteAt() is called with 'i' equal to 2, item 2 will be removed from
       the menu model, with items 3, 4, 5, etc all shifting one position lower.
===/pre>
   
Return Value:
===pre>
   SUCCESS   - The model was able to successfully delete the desired item.
   EBADPARM  - The requested index is not within the range of items stored in the model.
===/pre>

Comments:
   Following a successful call to IMENUMODEL_DeleteAt(), the number of items in the model is
   reduced by one.

   The menu model may define a custom free memory routine to release the memory consumed by
   complex items.  The custom free memory routine is registered with the menu model by calling
   IMENUMODEL_SetPfnFree().

Side Effects: 
   Deleting an item in a menu model by calling IMENUMODEL_DeleteAt() will cause BREW to send
   an EVT_MDL_LIST_CHANGE ListModelEvent to any object that has registered as a listener with
   the model.

   The fields of the ListModelEvent will contain the following information:
===pre>
       pos:      The index of the item that has been deleted from the menu model
       oldSize:  The number of items in the menu model before calling IMENUMODEL_DeleteAt().
       newSize:  The number of items in the menu model after calling IMENUMODEL_DeleteAt().
===/pre>

See Also:
   Model Events
   ListModelEvent
   IMenuModel Interface
   IMENUMODEL_DeleteAll()

===========================================================================

IMENUMODEL_DeleteAll()

Description:
   This function deletes all of the items stored in a menu model, releasing the memory referenced
   by each item and setting the size of the menu model to 0 (zero).  

Prototype:
   void IMENUMODEL_DeleteAll(IMenuModel *p);

Parameters:
===pre>
   p:  Pointer to the IMenuModel interface object.
===/pre>
   
Return Value:
   None

Comments:
   The menu model may define a custom free memory routine to release the memory consumed by
   complex items.  The custom free memory routine is registered with the menu model by calling
   calling IMENUMODEL_SetPfnFree() and will be called for each item stored in the menu model
   when issuing IMENUMODEL_DeleteAll().

Side Effects: 
   Deleting the items in a menu model by calling IMENUMODEL_DeleteAll() will cause BREW to send
   an EVT_MDL_LIST_CHANGE ListModelEvent to any object that has registered as a listener with
   the model.

   The fields of the ListModelEvent will contain the following information:
===pre>
       pos:      0
       oldSize:  The number of items in the menu model before calling IMENUMODEL_DeleteAll().
       newSize:  0 (since there are now no more items in the menu model)
===/pre>

See Also:
   Model Events
   ListModelEvent
   IMenuModel Interface
   IMENUMODEL_SetPfnFree()

===========================================================================

IMENUMODEL_SetPfnFree()

Description:
   This function specifies the callback function to be used by the menu model when releasing
   memory occupied by the items it stores.  The menu model manages information about each
   menu item, including its attribute flags, command ID and a content pointer to the data an
   attached object would use to render the menu item on the user interface.  The data referenced
   by the content pointer is opaque to the menu model and could point to an arbitrarily complex
   data structure with its own unique storage requirements.  The callback function allows a menu
   model to release "deep storage" -- custom memory allocated by an application and stored in
   the fields of an application defined data structure that is otherwise opaque to the menu
   model itself.
   
   The figure below illustrates complex items in a menu model that possesses "deep storage".  Each
   item stored in the menu model references content data allocated and owned by the application.
   For item x, this content storage includes a pointer to additional storage allocated and used by
   item x (the menu model doesn't even know this storage exists).  To release this storage, the
   menu model would provide a PFNNOTIFY callback that would free the contents of item x, as well
   as the custom memory pointed to from within item x.
   
===pre>
                                                      +--------------------------+
                                                      |                          |
                                :                     |        Contents of       |
                                :                     |           item x         |
                   |--------------------------|       |                          |       +---------+
       Item x      |                          |       |    Pointer to custom +---------> |         |
                   |   Content (void *)   +---------> |          storage         |       |         |
                   |                          |       +--------------------------+       +---------+
                   |--------------------------|
                   |                          |
     Item (x + 1)  |   Content (void *)   +---------> +--------------------------+
                   |                          |       |                          |
                   |--------------------------|       |        Contents of       |
                                :                     |        item (x + 1)      |
                                :                     |                          |
                                                      |                          |
                                                      +--------------------------+
===/pre>

Prototype:
   PFNNOTIFY IMENUMODEL_SetPfnFree(IMenuModel *p, PFNNOTIFY pfn);

Parameters:
===pre>
   po:   Pointer to the IMenuModel interface object.
   pfn:  Pointer to the callback function that BREW will call when freeing the memory
         associated with an item stored in the menu model.
===/pre>
   
Return Value:
   IMENUMODEL_SetPfnFree() returns a pointer to the menu model's old PFNNOTIFY callback function.
   A menu model may therefore manage multiple PFNNOTIFY callbacks, each capable of managing
   different item contents.

Comments:
   None

Side Effects: 
   None

See Also:
   IMenuModel Interface
   PFNNOTIFY


===========================================================================

IMENUMODEL_Add()

Description:
   This function adds a new menu item to a menu model, adding the item at the end of the current
   list of stored items.

Prototype:
   int IMENUMODEL_Add(IMenuModel *p, void *pv, uint16 c, uint16 e);

Parameters:
===pre>
   p:   Pointer to the IMenuModel interface object.
   pv:  Pointer to the data that defines the content for this item.  How this
        data is exposed on the display is up to the object that has attached
        itself to the menu model.  The data could be as simple as a text string
        or as complex a lengthy data structure.
   c:   The command ID assigned to this item.
   e:   A set of flags that indicate various state, display or behavioral attributes
        for the menu item.  The defined menu item flags are:
             MMF_ENABLED    -  When this flag is ON, the menu item is currently enabled.
             MMF_SUBMENU    -  When this flag is ON, the menu item is a submenu.
             MMF_SEPARATOR  -  When this flag is ON, the menu item should be drawn
                               with a menu separator.  For traditional vertical menus,
                               this is usually drawn as a line under the menu text.
                               However, the separator could be rendered in whatever
                               manner is most appropriate for the object attached to
                               the menu model.
===/pre>
   
Return Value:
===pre>
   SUCCESS    - The item was successfully added to the menu model.
   ENOMEMORY  - The menu model was unable to allocate enough memory to accommodate the new item.
===/pre>

Comments:
   None

Side Effects: 
   Successfully, adding a new item in a menu model by calling IMENUMODEL_Add() will cause
   BREW to send an EVT_MDL_LIST_CHANGE ListModelEvent to any object that has registered as
   a listener with the model.

   The fields of the ListModelEvent will contain the following information:
===pre>
       pos:      The index of the menu item that has been inserted into the model. 
       oldSize:  The number of items that were in the menu model before adding
                 the new item.
       newSize:  The number of items that are now stored in the menu model after
                 adding the new item.
===/pre>

See Also:
   Model Events
   IMenuModel Interface
   IMENUMODEL_InsertItem

==========================================================================

IMENUMODEL_TO_IMODEL()

Description:
   This function provides type safe casting of an IMenuModel pointer to an IModel pointer
   and should be used when passing an IMenuModel pointer to functions that require a
   pointer to an IModel.  This is safer than a simple cast, since the compiler will verify
   the pointer type prior to the actual cast.

Prototype:
   IModel *IMENUMODEL_TO_IMODEL(IMenuModel *me)

Parameters:
===pre>
   me:  Pointer to an IMenuModel interface object.
===/pre>

Return Value:
   Returns 'me' cast to an IModel pointer.

Comments:
   None

Side Effects: 
   None

See Also:
   IModel Interface
   IMenuModel Interface

*/
#endif /* __AEEMENUMODEL_H__ */
