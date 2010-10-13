#ifndef XMFI_H
#define XMFI_H
/*===========================================================================

                  X M F   S E R V I C E S   I N T E R N A L   
                           H E A D E R   F I L E

DESCRIPTION
  This header file contains all the internal definitions to XMF services.

Copyright (c) 2005 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
   
  $Header: //source/qcom/qct/multimedia/audio/parser/xmf/main/latest/src/xmfi.h#1 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/06/05    st     Created file.
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#ifdef FEATURE_XMF
#include "comdef.h"     /* Definitions for byte, word, etc.                */

/* <EJECT> */
/*---------------------------------------------------------------------------
** XMF General Defines
**---------------------------------------------------------------------------
*/

/* This status type is used for XMF parsing */
typedef enum {
  XMF_STATUS_SUCCESS,          /* Command was successful        */
  XMF_STATUS_BUFF_ERR,         /* Data underflow encountered    */
  XMF_STATUS_BUFF_PENDING,     /* Data buffer pending           */
  XMF_STATUS_DONE,             /* Command done                  */
  XMF_STATUS_FAILURE           /* Command failed                */
} xmf_status_type;

/* Mask to indicate status of XMF node */
typedef enum {
  XMF_NODE_MASK_FILE    = 0x0001,  /* File node               */
  XMF_NODE_MASK_PRELOAD = 0x0002,  /* Preload node            */
  XMF_NODE_MASK_DLS     = 0x0004,  /* DLS file node           */
  XMF_NODE_MASK_SMF     = 0x0008,  /* Standard MIDI file node */
  XMF_NODE_MASK_CUSTOM  = 0x0010   /* Custom format node      */
} xmf_node_mask_type;

/* XMF node structure */
typedef struct {
  uint32 status_mask;     /* Status mask for node                     */
  void   *prev;           /* Pointer to previous sibling node         */
  void   *next;           /* Pointer to next sibling node             */
  void   *parent;         /* Pointer to parent node                   */
  void   *child;          /* Pointer to child node                    */
  void   *next_content;   /* Pointer to next node of the same content */
  uint32 sibling;         /* Location of next sibling to this node    */
  uint32 ref_count;       /* Count of reference node indirections     */
  uint32 start;           /* Location of start of node                */
  uint32 end;             /* Location of end of node                  */
  uint32 num_items;       /* Number of items in this node             */
  uint32 resource;        /* Location of resource for this node       */
} xmf_node_type;

/* XMF states for state machine */
typedef enum {
  XMF_STATE_INIT,             /* Init state              */
  XMF_STATE_NODE,             /* Node processing state   */
  XMF_STATE_META,             /* Meta data state         */
  XMF_STATE_DONE,             /* Parsing done state      */
  XMF_STATE_ERROR,            /* Error state             */
  XMF_STATE_DONE_RECOGNIZE,   /* Finished recognition    */
  XMF_STATE_UNRECOGNIZED,     /* Unrecognized file state */
  XMF_STATE_INVALID           /* Invalid state           */
} xmf_state_enum_type;

/* XMF parsing header substate enum type */
typedef enum {
  XMF_STATE_HEAD_INIT,       /* Parsing header init substate       */
  XMF_STATE_HEAD_LENGTH,     /* Parsing header length substate     */
  XMF_STATE_HEAD_META_TABLE, /* Parsing header meta table substate */
  XMF_STATE_HEAD_TREE_START, /* Parsing header tree start substate */
  XMF_STATE_HEAD_TREE_END,   /* Parsing header tree end substate   */
  XMF_STATE_HEAD_INVALID     /* Parsing header invalid substate    */
} xmf_state_head_enum_type;

/* XMF parsing node substate enum type */
typedef enum {
  XMF_STATE_NODE_INIT,     /* Parsing node init substate            */
  XMF_STATE_NODE_LENGTH,   /* Parsing node length substate          */
  XMF_STATE_NODE_ITEMS,    /* Parsing node number of items substate */
  XMF_STATE_NODE_HEAD,     /* Parsing node header substate          */
  XMF_STATE_NODE_META,     /* Parsing node meta data substate       */
  XMF_STATE_NODE_UNPACK,   /* Parsing node unpack IDs substate      */
  XMF_STATE_NODE_CONTENTS, /* Parsing node content substate         */
  XMF_STATE_NODE_RESOURCE, /* Parsing node resource substate        */
  XMF_STATE_NODE_INVALID   /* Parsing node invalid substate         */
} xmf_state_node_enum_type;

/* XMF parsing meta data substate enum type */
typedef enum {
  XMF_STATE_META_INIT,            /* Parsing meta data init substate      */
  XMF_STATE_META_CONTENT,         /* Parsing meta data content substate   */
  XMF_STATE_META_FILE_TYPE,       /* Parsing meta data file type substate */
  XMF_STATE_META_NODE_NAME,       /* Parsing meta data node name substate */
  XMF_STATE_META_NODE_NAME_PEND,  /* Meta data name pending substate      */
  XMF_STATE_META_RESOURCE_FORMAT, /* Parsing meta data resource substate  */
  XMF_STATE_META_AUTOSTART,       /* Parsing meta data autostart substate */
  XMF_STATE_META_AUTOSTART_PEND,  /* Meta data autostart pending substate */
  XMF_STATE_META_INVALID          /* Parsing meta data invalid substate   */
} xmf_state_meta_enum_type;

/* Enum type that indicates the meta data field type */
typedef enum {
  XMF_META_FIELD_FILE_TYPE,        /* Meta data file type            */
  XMF_META_FIELD_NODE_NAME,        /* Meta data node name type       */
  XMF_META_FIELD_RESOURCE_FORMAT,  /* Meta data resource format type */
  XMF_META_FIELD_AUTOSTART,        /* Meta data autostart type       */
  XMF_META_FIELD_PRELOAD,          /* Meta data preload type         */
  XMF_META_FIELD_CUSTOM,           /* Meta data custom type          */
  XMF_META_FIELD_UNSUPPORTED       /* Meta data unsupported type     */
} xmf_meta_field_type;

/* XMF parse state type */
typedef struct {
  xmf_state_enum_type state;        /* Current state in XMF parsing      */
  uint8               sub_state;    /* Current substate in XMF parsing   */
  boolean             data_req;     /* Data required flag                */
  xmf_meta_field_type meta_field;   /* Current meta data field           */

  void    *handle;                  /* Handle of current XMF file        */
  uint8   *buf1;                    /* Data buffer 1                     */
  uint32  length_buf1;              /* Length of data buffer 1           */
  uint8   *buf2;                    /* Data buffer 2                     */
  uint32  length_buf2;              /* Length of data buffer 2           */
  uint32  index;                    /* Index into data buffers           */
  uint32  count;                    /* #bytes used in current operation  */
  uint32  current_position;         /* Current position in file          */
  uint32  file_position;            /* Data read position in file        */
  uint32  file_length;              /* Length of file                    */
  uint32  tree_start;               /* Location of start of XMF tree     */
  uint32  tree_end;                 /* Location of end of XMF tree       */

  uint32  contents;                 /* Content location of current node  */
  uint32  meta_end;                 /* Location of end of meta data      */
  uint32  meta_content_end;         /* Location of end of meta content   */

  uint32  auto_play_length;         /* Length of autoplay name           */
  uint8   *auto_play_buf;           /* Autoplay name buffer              */
  uint8   *name_buf;                /* Name buffer of current node       */

  xmf_node_type *root_node;         /* Pointer to root node              */
  xmf_node_type *current_node;      /* Pointer to current node           */
  xmf_node_type *auto_play_node;    /* Pointer to autoplay node          */
  xmf_node_type *initial_midi_node; /* Pointer to first SMF node         */
  xmf_node_type *current_midi_node; /* Pointer to current SMF node       */
  xmf_node_type *initial_dls_node;  /* Pointer to first DLS node         */
  xmf_node_type *current_dls_node;  /* Pointer to current DLS node       */
} xmf_state_type;
#endif /* FEATURE_XMF */
#endif /* XMFI_H */
