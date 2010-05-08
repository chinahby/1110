/* linesize(132)
** pagesize(60)
** title("Dual Mode Subscriber Station")
** subtitle("XMF Services")
*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                           X M F    S E R V I C E S 

GENERAL DESCRIPTION
  This module contains XMF services software.

EXTERNALIZED FUNCTIONS
  cmx_jpeg_encode_abort
    This function issues a command to abort the JPEG encoder.

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright(c) 2007 by QUALCOMM, Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/multimedia/audio/parser/xmf/main/latest/src/xmf.c#3 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/24/08	kd     Fixed Link Error reported due to not inclusion of the msg.h
				   and err.h header files.
11/19/08    kd     Modified code in xmf_state_free_nodes to prevent possible 
				   dereferencing of NULL pointer.Fix for the CR-134608.
03/11/07   pl/st   Modified xmf_free_state() to free ALL memory allocated for 
                   the specified XMF state machine structure; 
                   added xmf_terminate() function to allow clients to terminate 
                   an active XMF parse; modified call to cmx_xmf_continue() to 
                   include client handle.
12/02/05    sk     Modified code for error handling.Modified code in
                   xmf_do_header(),xmf_load_buffer()and xmf_state_free_nodes() 
                   Fix for the CR 84036.  
11/11/05    st     Modified code for lint updates: modified code in
                   xmf_data_req_cb() to check for correct state and substate.
10/18/05    st     Modified code to initialize all parameters correctly,
                   CR 79673.
02/06/05    st     Created file.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                        INCLUDE FILES FOR MODULE

===========================================================================*/
#include "target.h"             /* Target specific definitions             */
#include "customer.h"           /* Customer specific definitions           */
#include "comdef.h"             /* Definitions for byte, word, etc.        */
#include "msg.h"                /* Message logging/reporting services      */
#include "err.h"                /* Error logging/reporting services        */

#ifdef FEATURE_XMF
#include "cmxi.h"               /* CMX internal typedefs and prototypes    */
#include "xmfi.h"               /* XMF internal typedefs and prototypes    */
#include <memory.h>             /* Memory manipulation routines            */
#include <stdlib.h>             /* Dynamic memory allocation routines      */

/* Dynamic buffer alocation definitions
*/
#define XMF_MALLOC(size)     malloc(size)
#define XMF_FREE(buffer)     free(buffer)

/* <EJECT> */
/*===========================================================================

                          DECLARATIONS FOR MODULE

===========================================================================*/

/*===========================================================================

FUNCTION xmf_initialize_node

DESCRIPTION
  This function initializes the indicated node.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void xmf_initialize_node (
  xmf_node_type *node
) {
  if(node != NULL) {
    node->status_mask  = 0;
    node->prev         = NULL;
    node->next         = NULL;
    node->parent       = NULL;
    node->child        = NULL;
    node->next_content = NULL;
    node->sibling      = 0;
    node->ref_count    = 0;
    node->start        = 0;
    node->end          = 0;
    node->num_items    = 0;
    node->resource     = 0;
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION xmf_get_node

DESCRIPTION
  This function allocates a node.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
xmf_node_type *xmf_get_node ()
{
  xmf_node_type *ret_val;

  ret_val = (xmf_node_type *) XMF_MALLOC(sizeof(xmf_node_type));

  if(ret_val != NULL) {
    xmf_initialize_node(ret_val);
  }
  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION xmf_free_node

DESCRIPTION
  This function frees the indicated node.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void xmf_free_node (
  xmf_node_type *node
) {
  if(node != NULL) {
    XMF_FREE(node);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION xmf_state_free_nodes

DESCRIPTION
  This function frees the allocated nodes in the state structure.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void xmf_state_free_nodes (
  xmf_state_type *state
) {
  xmf_node_type *node, *next_node;

  if((state != NULL) && (state->root_node != NULL)) {
    node = state->root_node;
    if(node->child != NULL) {
      do {
        while(node->child != NULL) {
          node = (xmf_node_type *) node->child;
        }

        if(node->next != NULL) {
          next_node = (xmf_node_type *) node->next;
        } else {
          next_node = (xmf_node_type *) node->parent;
          if(next_node != NULL) {
            next_node->child = NULL;
          } else {
            MSG_ERROR("unexpected NULL parent node",0,0,0); 
            break;  
		  }
        }

        xmf_free_node(node);

        node = next_node;
      } while(node != state->root_node);
    }

    xmf_free_node(node);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION xmf_state_reset

DESCRIPTION
  This function resets the indiated XMF state structure.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void xmf_state_reset (
  xmf_state_type *state
) {
  if(state != NULL) {
    state->state             = XMF_STATE_INIT;
    state->sub_state         = 0;
    state->data_req          = FALSE;
    state->meta_field        = XMF_META_FIELD_UNSUPPORTED;
    state->handle            = NULL;
    state->buf1              = NULL;
    state->length_buf1       = 0;
    state->buf2              = NULL;
    state->length_buf2       = 0;
    state->index             = 0;
    state->count             = 0;
    state->current_position  = 0;
    state->file_position     = 0;
    state->file_length       = 0;
    state->tree_start        = 0;
    state->tree_end          = 0;
    state->contents          = 0;
    state->meta_end          = 0;
    state->meta_content_end  = 0;
    state->auto_play_length  = 0;
    state->auto_play_buf     = NULL;
    state->name_buf          = NULL;
    state->root_node         = NULL;
    state->current_node      = NULL;
    state->auto_play_node    = NULL;
    state->initial_midi_node = NULL;
    state->current_midi_node = NULL;
    state->initial_dls_node  = NULL;
    state->current_dls_node  = NULL;
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION xmf_get_state

DESCRIPTION
  This function allocates an XMF state structure.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
xmf_state_type *xmf_get_state ()
{
  xmf_state_type *ret_val;

  ret_val = (xmf_state_type *) XMF_MALLOC(sizeof(xmf_state_type));

  if(ret_val != NULL) {
    xmf_state_reset(ret_val);
  }
  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION xmf_next_byte

DESCRIPTION
  This function returns the next byte in the XMF data buffers.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
xmf_status_type xmf_next_byte (
  xmf_state_type *state,
  uint8              *value
) {
  xmf_status_type ret_val = XMF_STATUS_BUFF_ERR;
  uint32              index;

  if(state->index < state->length_buf1) {
    *value  = state->buf1[state->index];
    ret_val = XMF_STATUS_SUCCESS;
  } else {
    index = state->index - state->length_buf1;
    if(index < state->length_buf2) {
      *value  = state->buf2[index];
      ret_val = XMF_STATUS_SUCCESS;
    }
  }
  if(ret_val == XMF_STATUS_SUCCESS) {
    state->index++;
    state->current_position++;
    state->count++;
  }
  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION xmf_read_var_length

DESCRIPTION
  This function reads a variable length field in an XMF file.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
xmf_status_type xmf_read_var_length (
  xmf_state_type *state,
  uint32             *length
) {
  xmf_status_type ret_val;
  uint32              tlong = 0;
  uint8               tbyte;

  do {
    ret_val = xmf_next_byte(state, &tbyte);
    tlong   = (tlong << 7) | (tbyte & 0x7f);
  } while((ret_val == XMF_STATUS_SUCCESS) && (tbyte & 0x80) &&
          (tlong < 0x2000000));

  if((tbyte & 0x80) && (tlong >= 0x2000000)) {
    ret_val = XMF_STATUS_FAILURE;
  } else if(ret_val == XMF_STATUS_SUCCESS) {
    *length = tlong;
  }

  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION xmf_reposition

DESCRIPTION
  This function repositions the parse to the indicated position.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void xmf_reposition (
  xmf_state_type *state,
  uint32         position
) {
  uint32 tlong;

  if(state->current_position <= position) {
    tlong         = position - state->current_position;
    state->index += tlong;
  } else {
    tlong = state->current_position - position;
    if(state->index >= tlong) {
      state->index -= tlong;
    } else {
      state->index  = state->length_buf1 + state->length_buf2;
    }
  }
  state->current_position = position;
}

/* <EJECT> */
/*===========================================================================

FUNCTION xmf_load_buffer

DESCRIPTION
  This function loads data buffers if necessary.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
xmf_status_type xmf_load_buffer (
  xmf_state_type *state
) {
  cmx_xmf_status_type status;
  xmf_status_type     ret_val = XMF_STATUS_FAILURE;
  uint32              num_bytes;

  if(state->index < state->length_buf1) {
    if(state->buf2 == NULL) {
      state->buf2 = cmx_xmf_get_buff(state->handle, &num_bytes);
      if(state->buf2 != NULL) {
        status = cmx_xmf_get_data(state->handle, state->buf2,
                                  state->file_position, &num_bytes, state);
        switch(status) {
          case CMX_XMF_SUCCESS:
            if(num_bytes != 0) {
              state->length_buf2    = num_bytes;
              state->file_position += state->length_buf2;
              ret_val               = XMF_STATUS_SUCCESS;
            }
            break;

          case CMX_XMF_BUFF_PENDING:
            ret_val = XMF_STATUS_BUFF_PENDING;
            break;
        }
      }
    }
  } else if(state->index < (state->length_buf1 + state->length_buf2)) {
    cmx_xmf_free_buff(state->handle, state->buf1);
    state->index       -= state->length_buf1;
    state->buf1         = state->buf2;
    state->length_buf1  = state->length_buf2;
    state->buf2         = cmx_xmf_get_buff(state->handle, &num_bytes);
    if(state->buf2 != NULL) {
      status = cmx_xmf_get_data(state->handle, state->buf2,
                                state->file_position, &num_bytes, state);
      switch(status) {
        case CMX_XMF_SUCCESS:
          if(num_bytes != 0) {
            state->length_buf2    = num_bytes;
            state->file_position += state->length_buf2;
            ret_val               = XMF_STATUS_SUCCESS;
          }
          break;

        case CMX_XMF_BUFF_PENDING:
          ret_val = XMF_STATUS_BUFF_PENDING;
          break;
      }
    }
  } else {
    if(state->buf1 != NULL) {
      cmx_xmf_free_buff(state->handle, state->buf1);
      state->buf1 = NULL;
    }

    if(state->buf2 != NULL) {
      cmx_xmf_free_buff(state->handle, state->buf2);
      state->buf2 = NULL;
    }

    state->buf1 = cmx_xmf_get_buff(state->handle, &num_bytes);
    if(state->buf1 != NULL) {
      status = cmx_xmf_get_data(state->handle, state->buf1,
                                state->current_position, &num_bytes, state);

      state->file_position = state->current_position;
      state->index         = 0;

      switch(status) {
        case CMX_XMF_SUCCESS:
          if(num_bytes != 0) {
            state->length_buf1    = num_bytes;
            state->file_position += state->length_buf1;
            ret_val               = XMF_STATUS_BUFF_PENDING;
            cmx_xmf_continue(state->handle, state);
          }
          break;

        case CMX_XMF_BUFF_PENDING:
          ret_val = XMF_STATUS_BUFF_PENDING;
          break;
      }
    }
  }
  state->data_req = FALSE;

  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION xmf_check_autostart

DESCRIPTION
  This function checks if the current node is the autoplay node.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void xmf_check_autostart (
  xmf_state_type *state
) {
  if(state->name_buf != NULL) {
    if(state->auto_play_buf != NULL) {
      if(memcmp(state->auto_play_buf, state->name_buf,
                state->auto_play_length) == 0) {
        state->auto_play_node = state->current_node;

        XMF_FREE(state->auto_play_buf);
        state->auto_play_buf = NULL;
      }
    }
    XMF_FREE(state->name_buf);
    state->name_buf = NULL;
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION xmf_do_header

DESCRIPTION
  This function processes the XMF header.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void xmf_do_header (
  xmf_state_type *state
) {
  xmf_status_type status = XMF_STATUS_SUCCESS;
  uint32          tlong;

  do {
    state->count = 0;
    switch(state->sub_state) {
      case XMF_STATE_HEAD_INIT:
// temp work around for mxmf
        if((state->length_buf1 > 12) &&
           ((memcmp(state->buf1, "XMF_1.00", 8) == 0) ||
            (memcmp(state->buf1, "XMF_2.00", 8) == 0))) {
          if(state->buf1[8] == 0) {
            state->index            = 16;
            state->current_position = 16;
          } else {
            state->index            = 8;
            state->current_position = 8;
          }
          state->sub_state = XMF_STATE_HEAD_LENGTH;
          if(cmx_xmf_recognized(state->handle) == CMX_XMF_DONE) {
            state->state = XMF_STATE_DONE_RECOGNIZE;
          }
        } else {
          state->state = XMF_STATE_UNRECOGNIZED;
        }
        break;

      case XMF_STATE_HEAD_LENGTH:
        status = xmf_read_var_length(state, &tlong);
        if(status == XMF_STATUS_SUCCESS) {
          state->file_length = tlong;
          state->sub_state   = XMF_STATE_HEAD_META_TABLE;
        }
        break;

      case XMF_STATE_HEAD_META_TABLE:
        status = xmf_read_var_length(state, &tlong);
        if(status == XMF_STATUS_SUCCESS) {
          state->index            += tlong;
          state->current_position += tlong;
          state->sub_state         = XMF_STATE_HEAD_TREE_START;
        }
        break;

      case XMF_STATE_HEAD_TREE_START:
        status = xmf_read_var_length(state, &tlong);
        if(status == XMF_STATUS_SUCCESS) {
          state->tree_start = tlong;
          state->sub_state  = XMF_STATE_HEAD_TREE_END;
        }
        break;

      case XMF_STATE_HEAD_TREE_END:
        status = xmf_read_var_length(state, &tlong);
        if(status == XMF_STATUS_SUCCESS) {
          state->tree_end  = tlong;

          if((state->current_position <= state->tree_start) &&
             (state->tree_end > state->tree_start)) {
            tlong = state->tree_start - state->current_position;

            state->index            += tlong;
            state->current_position  = state->tree_start;
            state->state             = XMF_STATE_NODE;
            state->sub_state         = 0;
          } else {
            state->state = XMF_STATE_ERROR;
          }
        }
        break;
    }
  } while((status == XMF_STATUS_SUCCESS) &&
          (state->state == XMF_STATE_INIT));

  switch(status) {
    case XMF_STATUS_BUFF_ERR:
      state->data_req = TRUE;
      break;

    case XMF_STATUS_FAILURE:
      state->state = XMF_STATE_ERROR;
      break;
  }

  if(state->data_req) {
    if((state->count <= state->index) &&
       (state->count <= state->current_position)) {
      state->index            -= state->count;
      state->current_position -= state->count;
    } else {
      state->state = XMF_STATE_ERROR;
    }
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION xmf_do_node

DESCRIPTION
  This function processes a node.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void xmf_do_node (
  xmf_state_type *state
) {
  xmf_node_type   *node, *parent;
  xmf_status_type status = XMF_STATUS_SUCCESS;
  uint32          tlong;

  do {
    state->count = 0;
    switch(state->sub_state) {
      case XMF_STATE_NODE_INIT:
        parent = state->current_node;
        while((parent != NULL) && (parent->num_items == 0)) {
          parent = parent->parent;
        }
        if((parent == NULL) && (state->root_node != NULL)) {
          state->state = XMF_STATE_DONE;
        } else {
          node = xmf_get_node();
          if(node != NULL) {
            if(parent == NULL) {
              state->root_node = node;
            } else {
              node->parent = parent;
              if(parent->status_mask & XMF_NODE_MASK_PRELOAD) {
                node->status_mask |= XMF_NODE_MASK_PRELOAD;
              }
              parent->num_items--;
              if(parent->child == NULL) {
                parent->child = node;
              } else {
                parent = parent->child;
                while(parent->next != NULL) {
                  parent = parent->next;
                }
                parent->next = node;
                node->prev   = parent;

                xmf_reposition(state, parent->sibling);
              }
            }
            node->start         = state->current_position;
            state->current_node = node;
            state->sub_state    = XMF_STATE_NODE_LENGTH;
          } else {
            if(parent == NULL) {
              state->state = XMF_STATE_ERROR;
            } else {
              state->state = XMF_STATE_DONE;
            }
          }
        }
        break;

      case XMF_STATE_NODE_LENGTH:
        status = xmf_read_var_length(state, &tlong);
        if(status == XMF_STATUS_SUCCESS) {
          node             = state->current_node;
          node->end        = node->start + tlong;
          if(node->ref_count == 0) {
            node->sibling  = node->end;
          }
          state->sub_state = XMF_STATE_NODE_ITEMS;
        }
        break;

      case XMF_STATE_NODE_ITEMS:
        status = xmf_read_var_length(state, &tlong);
        if(status == XMF_STATUS_SUCCESS) {
          node = state->current_node;
          if(node->ref_count == 0) {
            if(tlong == 0) {
              node->status_mask |= XMF_NODE_MASK_FILE;
            }
            node->num_items = tlong;
          } else if(node->num_items != tlong) {
            state->state = XMF_STATE_ERROR;
          }
          state->sub_state = XMF_STATE_NODE_HEAD;
        }
        break;

      case XMF_STATE_NODE_HEAD:
        status = xmf_read_var_length(state, &tlong);
        if(status == XMF_STATUS_SUCCESS) {
          node             = state->current_node;
          state->contents  = node->start + tlong;
          state->sub_state = XMF_STATE_NODE_META;
          if(node->end <= state->contents) {
            state->state = XMF_STATE_ERROR;
          }
        }
        break;

      case XMF_STATE_NODE_META:
        status = xmf_read_var_length(state, &tlong);
        if(status == XMF_STATUS_SUCCESS) {
          if(tlong != 0) {
            state->meta_end  = state->current_position + tlong;
            state->state     = XMF_STATE_META;
            state->sub_state = XMF_STATE_META_INIT;
          } else {
            if(state->current_node->status_mask & XMF_NODE_MASK_FILE) {
// resource meta data not encountered in file node
              state->state     = XMF_STATE_ERROR;
            } else {
              state->sub_state = XMF_STATE_NODE_UNPACK;
            }
          }
        }
        break;

      case XMF_STATE_NODE_UNPACK:
        status = xmf_read_var_length(state, &tlong);
        if(status == XMF_STATUS_SUCCESS) {
          if((tlong == 0) && (state->current_position <= state->contents)) {
            tlong = state->contents - state->current_position;

            state->index            += tlong;
            state->current_position  = state->contents;
            state->sub_state         = XMF_STATE_NODE_CONTENTS;
          } else {
            state->state = XMF_STATE_ERROR;
          }
        }
        break;

      case XMF_STATE_NODE_CONTENTS:
        status = xmf_read_var_length(state, &tlong);
        if(status == XMF_STATUS_SUCCESS) {
          node = state->current_node;
          switch(tlong) {
            case 1:
              if(node->num_items == 0) {
                node->resource = state->current_position;
              }
              state->sub_state = XMF_STATE_NODE_INIT;
              break;

            case 2:
              status = xmf_read_var_length(state, &tlong);
              if(status == XMF_STATUS_SUCCESS) {
                if(node->num_items == 0) {
                  node->resource = tlong;
                } else {
                  xmf_reposition(state, tlong);
                }
                state->sub_state = XMF_STATE_NODE_INIT;
              }
              break;

            case 3:
              status = xmf_read_var_length(state, &tlong);
              if(status == XMF_STATUS_SUCCESS) {
                node->ref_count++;
                if(node->ref_count < 5) {
                  node->start      = tlong;
                  state->sub_state = XMF_STATE_NODE_LENGTH;
                  xmf_reposition(state, tlong);
                } else {
                  state->state = XMF_STATE_ERROR;
                }
              }
              break;
          }
        }
        break;
    }
  } while((status == XMF_STATUS_SUCCESS) &&
          (state->state == XMF_STATE_NODE));

  switch(status) {
    case XMF_STATUS_BUFF_ERR:
      state->data_req = TRUE;
      break;

    case XMF_STATUS_FAILURE:
      state->state = XMF_STATE_ERROR;
      break;
  }

  if(state->data_req) {
    if((state->count <= state->index) &&
       (state->count <= state->current_position)) {
      state->index            -= state->count;
      state->current_position -= state->count;
    } else {
      state->state = XMF_STATE_ERROR;
    }
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION xmf_do_meta

DESCRIPTION
  This function processes meta data.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void xmf_do_meta (
  xmf_state_type *state
) {
  xmf_node_type       *node  = state->current_node;
  xmf_status_type     status = XMF_STATUS_SUCCESS;
  uint32              tlong, length;
  boolean             skip_content, content_done = FALSE;
  cmx_xmf_status_type data_status;

  do {
    state->count = 0;
    switch(state->sub_state) {
      case XMF_STATE_META_INIT:
        status = xmf_read_var_length(state, &tlong);
        if(status == XMF_STATUS_SUCCESS) {
          if(tlong == 0) {
            status = xmf_read_var_length(state, &tlong);
            if(status == XMF_STATUS_SUCCESS) {
              state->sub_state = XMF_STATE_META_CONTENT;
              switch(tlong) {
                case 0:
                  if(node == state->root_node) {
                    state->meta_field = XMF_META_FIELD_FILE_TYPE;
                  } else {
                    state->meta_field = XMF_META_FIELD_UNSUPPORTED;
                  }
                  break;

                case 1:
                  if((state->auto_play_buf != NULL) &&
                     (state->auto_play_node == NULL) &&
                     (node->status_mask & XMF_NODE_MASK_FILE)) {
                    state->meta_field = XMF_META_FIELD_NODE_NAME;
                  } else {
                    state->meta_field = XMF_META_FIELD_UNSUPPORTED;
                  }
                  break;

                case 3:
                  if(node->status_mask & XMF_NODE_MASK_FILE) {
                    state->meta_field = XMF_META_FIELD_RESOURCE_FORMAT;
                  } else {
                    state->meta_field = XMF_META_FIELD_UNSUPPORTED;
                  }
                  break;

                case 11:
                  if((node == state->root_node) &&
                                (state->auto_play_buf == NULL)) {
                    state->meta_field = XMF_META_FIELD_AUTOSTART;
                  } else {
                    state->meta_field = XMF_META_FIELD_UNSUPPORTED;
                  }
                  break;

                case 12:
                  state->meta_field = XMF_META_FIELD_PRELOAD;
                  break;

                default:
                  state->meta_field = XMF_META_FIELD_UNSUPPORTED;
                  break;
              }
            }
          } else {
            state->sub_state         = XMF_STATE_META_CONTENT;
            state->meta_field        = XMF_META_FIELD_CUSTOM;
            state->current_position += tlong;
            state->index            += tlong;
          }
        }
        break;

      case XMF_STATE_META_CONTENT:
        status = xmf_read_var_length(state, &tlong);
        if(status == XMF_STATUS_SUCCESS) {
          status = xmf_read_var_length(state, &length);
          if(status == XMF_STATUS_SUCCESS) {
            skip_content = TRUE;
            if(tlong == 0) {
              skip_content            = FALSE;
              state->meta_content_end = state->current_position + length;
              switch(state->meta_field) {
                case XMF_META_FIELD_FILE_TYPE:
                  state->sub_state = XMF_STATE_META_FILE_TYPE;
                  break;

                case XMF_META_FIELD_NODE_NAME:
                  state->sub_state = XMF_STATE_META_NODE_NAME;
                  break;

                case XMF_META_FIELD_RESOURCE_FORMAT:
                  state->sub_state = XMF_STATE_META_RESOURCE_FORMAT;
                  break;

                case XMF_META_FIELD_AUTOSTART:
                  state->sub_state = XMF_STATE_META_AUTOSTART;
                  break;

                case XMF_META_FIELD_PRELOAD:
                  node->status_mask |= XMF_NODE_MASK_PRELOAD;
                  skip_content       = TRUE;
                  break;

                default:
                  skip_content = TRUE;
                  break;
              }
            }

            if(skip_content) {
              state->current_position += length;
              state->index            += length;
              content_done             = TRUE;
            }
          }
        }
        break;

      case XMF_STATE_META_FILE_TYPE:
        status = xmf_read_var_length(state, &tlong);
        if(status == XMF_STATUS_SUCCESS) {
          if((tlong & ~0x1) == 0x6) {
            status = xmf_read_var_length(state, &tlong);
            if(status == XMF_STATUS_SUCCESS) {
              status = xmf_read_var_length(state, &length);
              if(status == XMF_STATUS_SUCCESS) {
                if(((tlong != 0) && (tlong != 1) && (tlong != 2)) ||
                   (length != 0) ||
                   (state->current_position != state->meta_content_end)) {
// change to unsupported file type
                  state->state = XMF_STATE_ERROR;
                } else {
                  content_done = TRUE;
                }
              }
            }
          } else {
// change to unsupported file type
            state->state = XMF_STATE_ERROR;
          }
        }
        break;

      case XMF_STATE_META_NODE_NAME:
        status = xmf_read_var_length(state, &tlong);
        if(status == XMF_STATUS_SUCCESS) {
          if((tlong & ~0x1) == 0x0) {
            length = state->meta_content_end - state->current_position;

            if(length == state->auto_play_length) {
              state->name_buf = XMF_MALLOC(length);
              if(state->name_buf != NULL) {
                if(length <= ((state->length_buf1 + state->length_buf2) -
                              state->index)) {
                  if(state->index < state->length_buf1) {
                    tlong = MIN(state->length_buf1 - state->index, length);
                    memcpy(state->name_buf, &state->buf1[state->index], tlong);
                    memcpy(&state->name_buf[tlong], state->buf2,
                           length - tlong);
                  } else {
                    tlong = state->index - state->length_buf1;
                    memcpy(state->name_buf, &state->buf2[tlong], length);
                  }
                } else {
                  data_status = cmx_xmf_get_data(state->handle, state->name_buf,
                                                 state->current_position,
                                                 &length, state);
                  if(data_status == CMX_XMF_BUFF_PENDING) {
                    state->sub_state = XMF_STATE_META_NODE_NAME_PEND;
                  } else if(data_status == CMX_XMF_FAILURE) {
                    status = XMF_STATUS_FAILURE;
                  }
                }
                if(status == XMF_STATUS_SUCCESS) {
                  xmf_check_autostart(state);
                  xmf_reposition(state, state->meta_content_end);
                  content_done = TRUE;
                }
              } else {
                state->state = XMF_STATE_ERROR;
              }
            } else {
              xmf_reposition(state, state->meta_content_end);
              content_done = TRUE;
            }
          } else {
            xmf_reposition(state, state->meta_content_end);
            content_done = TRUE;
          }
        }
        break;

      case XMF_STATE_META_NODE_NAME_PEND:
        xmf_check_autostart(state);
        xmf_reposition(state, state->meta_content_end);
        content_done = TRUE;
        break;

      case XMF_STATE_META_RESOURCE_FORMAT:
        status = xmf_read_var_length(state, &tlong);
        if(status == XMF_STATUS_SUCCESS) {
          if((tlong & ~0x1) == 0x6) {
            status = xmf_read_var_length(state, &tlong);
            if(status == XMF_STATUS_SUCCESS) {
              if(tlong == 0) {
                status = xmf_read_var_length(state, &tlong);
                if(status == XMF_STATUS_SUCCESS) {
                  switch(tlong) {
                    case 0:
                    case 1:
                      node->status_mask |= XMF_NODE_MASK_SMF;
                      content_done       = TRUE;
                      if(state->initial_midi_node == NULL) {
                        state->initial_midi_node = node;
                        state->current_midi_node = node;
                      } else {
                        state->current_midi_node->next_content = node;
                        state->current_midi_node               = node;
                      }
                      break;

                    case 2:
                    case 3:
                    case 4:
                    case 5:
                      node->status_mask |= XMF_NODE_MASK_DLS;
                      content_done       = TRUE;
                      if(state->initial_dls_node == NULL) {
                        state->initial_dls_node = node;
                        state->current_dls_node = node;
                      } else {
                        state->current_dls_node->next_content = node;
                        state->current_dls_node               = node;
                      }
                      break;

                    default:
// change to unexpected resource
                      state->state = XMF_STATE_ERROR;
                      break;
                  }
                }
              } else {
                node->status_mask |= XMF_NODE_MASK_CUSTOM;
                xmf_reposition(state, state->meta_content_end);
                content_done = TRUE;
              }
            }
          } else {
// change to unsupported format
            state->state = XMF_STATE_ERROR;
          }
        }
        break;

      case XMF_STATE_META_AUTOSTART:
        status = xmf_read_var_length(state, &tlong);
        if(status == XMF_STATUS_SUCCESS) {
          if((tlong & ~0x1) == 0x0) {
            length = state->meta_content_end - state->current_position;

            state->auto_play_buf = XMF_MALLOC(length);
            if(state->auto_play_buf != NULL) {
              if(length <= ((state->length_buf1 + state->length_buf2) -
                            state->index)) {
                if(state->index < state->length_buf1) {
                  tlong = MIN(state->length_buf1 - state->index, length);
                  memcpy(state->auto_play_buf, &state->buf1[state->index],
                         tlong);
                  memcpy(&state->auto_play_buf[tlong], state->buf2,
                         length - tlong);
                } else {
                  tlong = state->index - state->length_buf1;
                  memcpy(state->auto_play_buf, &state->buf2[tlong], length);
                }
              } else {
                data_status = cmx_xmf_get_data(state->handle,
                                               state->auto_play_buf,
                                               state->current_position,
                                               &length, state);
                if(data_status == CMX_XMF_BUFF_PENDING) {
                  state->sub_state = XMF_STATE_META_AUTOSTART_PEND;
                } else if(data_status == CMX_XMF_FAILURE) {
                  status = XMF_STATUS_FAILURE;
                }
              }
              state->auto_play_length = length;
              if(status == XMF_STATUS_SUCCESS) {
                xmf_reposition(state, state->meta_content_end);
                content_done = TRUE;
              }
            } else {
              state->state = XMF_STATE_ERROR;
            }
          } else {
            xmf_reposition(state, state->meta_content_end);
            content_done = TRUE;
          }
        }
        break;

      case XMF_STATE_META_AUTOSTART_PEND:
        xmf_reposition(state, state->meta_content_end);
        content_done = TRUE;
        break;
    }

    if(content_done) {
      if(state->current_position < state->meta_end) {
        state->sub_state = XMF_STATE_META_INIT;
      } else if(state->current_position > state->meta_end) {
        state->state     = XMF_STATE_ERROR;
      } else {
        state->state     = XMF_STATE_NODE;
        state->sub_state = XMF_STATE_NODE_UNPACK;

        if(node->status_mask & XMF_NODE_MASK_FILE) {
          if(!(node->status_mask & (XMF_NODE_MASK_DLS |
                                    XMF_NODE_MASK_SMF |
                                    XMF_NODE_MASK_CUSTOM))) {
// resource meta data not encountered in file node
            state->state = XMF_STATE_ERROR;
          }
        }
      }
      content_done = FALSE;
    }
  } while((status == XMF_STATUS_SUCCESS) &&
          (state->state == XMF_STATE_META));

  switch(status) {
    case XMF_STATUS_BUFF_ERR:
      state->data_req = TRUE;
      break;

    case XMF_STATUS_FAILURE:
      state->state = XMF_STATE_ERROR;
      break;
  }

  if(state->data_req) {
    if((state->count <= state->index) &&
       (state->count <= state->current_position)) {
      state->index            -= state->count;
      state->current_position -= state->count;
    } else {
      state->state = XMF_STATE_ERROR;
    }
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION xmf_free_state

DESCRIPTION
  This function frees all memory allocated for XMF state machine.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void xmf_free_state (
  xmf_state_type *state
) {
  if(state != NULL) {
    if(state->name_buf != NULL) {
      XMF_FREE(state->name_buf);
      state->name_buf = NULL;
    }
    if(state->auto_play_buf != NULL) {
      XMF_FREE(state->auto_play_buf);
      state->auto_play_buf = NULL;
    }
    xmf_state_free_nodes(state);
    XMF_FREE(state);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION xmf_failure

DESCRIPTION
  This function fails the indicated XMF parse.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void xmf_failure (
  xmf_state_type *state
) {
  void *handle_ptr;

  if(state != NULL) {
    handle_ptr = state->handle;
    xmf_free_state(state);
    cmx_xmf_failure(handle_ptr);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION xmf_terminate

DESCRIPTION
  This function terminates XMF parsing.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void xmf_terminate (
  void *xmf_state
) {
  xmf_state_type *state = (xmf_state_type *) xmf_state;

  if(state != NULL) {
    xmf_free_state(state);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION xmf_load_and_play

DESCRIPTION
  This function loads DLS files and plays a MIDI file from XMF file.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void xmf_load_and_play (
  xmf_state_type *state,
  xmf_node_type  *dls_node
) {
  uint32 dls_location = 0;

  state->current_dls_node = dls_node;
  if(dls_node != NULL) {
    dls_location = dls_node->resource;
  }
  if(cmx_xmf_load_and_play(state->handle, dls_location,
                           state->auto_play_node->resource, state) ==
     CMX_XMF_SUCCESS) {
    xmf_free_state(state);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION xmf_load_and_play_cont

DESCRIPTION
  This function continues loading and playing the XMF file.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void xmf_load_and_play_cont (
  void *xmf_state
) {
  xmf_state_type *state = (xmf_state_type *) xmf_state;

  if(state != NULL) {
    xmf_load_and_play(state, state->current_dls_node->next_content);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION xmf_process_continue

DESCRIPTION
  This function continues processing the indicated XMF file.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void xmf_process_continue (
  void *xmf_state
) {
  xmf_state_type  *state = (xmf_state_type *) xmf_state;
  xmf_status_type status;
  void            *handle_ptr;

  do {
    do {
      switch(state->state) {
        case XMF_STATE_INIT:
          xmf_do_header(state);
          break;

        case XMF_STATE_NODE:
          xmf_do_node(state);
          break;

        case XMF_STATE_META:
          xmf_do_meta(state);
          break;
      }
    } while((!state->data_req) &&
            ((state->state == XMF_STATE_INIT) ||
             (state->state == XMF_STATE_NODE) ||
             ((state->state == XMF_STATE_META) &&
              (state->sub_state != XMF_STATE_META_NODE_NAME_PEND) &&
              (state->sub_state != XMF_STATE_META_AUTOSTART_PEND))));

    if(state->data_req) {
      status = xmf_load_buffer(state);
      if(status == XMF_STATUS_FAILURE) {
        state->state = XMF_STATE_ERROR;
      }
    } else {
      status = XMF_STATUS_DONE;
    }
  } while (status == XMF_STATUS_SUCCESS);

  if(state->state == XMF_STATE_DONE_RECOGNIZE) {
    xmf_free_state(state);
  } else if(state->state == XMF_STATE_UNRECOGNIZED) {
    handle_ptr = state->handle;
    xmf_free_state(state);
    cmx_xmf_unrecognized(handle_ptr);
  } else if(state->state == XMF_STATE_ERROR) {
    xmf_failure(state);
  } else if(status == XMF_STATUS_DONE) {
    if(state->auto_play_node == NULL) {
      state->auto_play_node = state->initial_midi_node;
    }
    if(state->auto_play_node != NULL) {
      xmf_load_and_play(state, state->initial_dls_node);
    } else {
      xmf_failure(state);
    }
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION xmf_data_req_cb

DESCRIPTION
  This function handles data request callbacks.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void xmf_data_req_cb (
  cmx_xmf_status_type status,
  void                *xmf_state,
  uint32              num_bytes
) {
  xmf_state_type *state = (xmf_state_type *) xmf_state;

  if(state != NULL) {
    if(status == CMX_XMF_SUCCESS) {
      if((state->state == XMF_STATE_META) &&
         (state->sub_state == XMF_STATE_META_AUTOSTART_PEND)) {
        state->auto_play_length = num_bytes;
      } else if((state->state != XMF_STATE_META) ||
                (state->sub_state != XMF_STATE_META_NODE_NAME_PEND)) {
        if(num_bytes != 0) {
          if(state->buf2 != NULL) {
            state->length_buf2 = num_bytes;
          } else {
            state->length_buf1 = num_bytes;
          }
          state->file_position += num_bytes;
        } else {
          state->state = XMF_STATE_ERROR;
        }
      }
    } else {
      state->state = XMF_STATE_ERROR;
    }

    xmf_process_continue(state);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION xmf_verify

DESCRIPTION
  This function checks if the indicated file is an XMF file and, if so,
  parses and plays the file.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void xmf_verify (
  void *handle_ptr
) {
  cmx_xmf_status_type status;
  xmf_state_type      *state;

  state = xmf_get_state();
  if(state != NULL) {
    state->handle = handle_ptr;

    state->buf1 = cmx_xmf_get_buff(handle_ptr, &state->length_buf1);
    if(state->buf1 != NULL) {
      status = cmx_xmf_get_data(state->handle, state->buf1,
                                state->file_position, &state->length_buf1,
                                state);

      if(status == CMX_XMF_SUCCESS) {
        state->file_position += state->length_buf1;
        xmf_process_continue(state);
      } else if(status == CMX_XMF_FAILURE) {
        xmf_failure(state);
      }
    } else {
      xmf_failure(state);
    }
  } else {
    cmx_xmf_unrecognized(handle_ptr);
  }
}
#endif /* FEATURE_XMF */
