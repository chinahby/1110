// $Header: //source/qcom/qct/multimedia/graphics/svg/services/interface/main/latest/inc/svg.h#1 $ $DateTime: 2008/06/24 02:53:37 $ $Author: ranjhith $

/* linesize(758)
** pagesize(60)
** title("Dual Mode Subscriber Station")
** subtitle("GRAPHICS")
*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                    S V G - H E A D E R F I L E

GENERAL DESCRIPTION
  This module contains svg software.

REFERENCES

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright(c) 2001 - 2006 by QUALCOMM, Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/27/06   lip     Add SVGT 1.2 and FEATURE_SVGDOM support.
04/20/05    lg     Added filename to svg_load_data_type. Allowed frame cmd to 
                   seek to a specific location.
03/28/05    lg     Removed unwanted comments.
03/17/05    lg     Modified code to remove DOM stuff that was added acceidentally.
12/16/04    lg     Created file.
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#ifndef SVG_H
#define SVG_H

#ifdef FEATURE_SVG

#include "cmx.h"
#include "svgver.h"

/* Size of chunk used for reading data from client */
#define  SVG_CHUNK_SIZE    3200

/* Max size of Title supported */
#define  SVG_TITLE_SIZE     128

/* This struct is used to indicate the status of CMX cb */
typedef enum{
  SVG_CMX_GET_FILE_SIZE,     /* Gets svg file size, need to be synchronous   */
  SVG_CMX_GET_DATA,          /* Request for reading data from file           */
  SVG_CMX_INFO,              /* SVG file info                                */
  SVG_CMX_EXT_REQ,           /* Request for reading an external file         */
  SVG_CMX_EXT_FILE_SIZE,     /* Get file size for external file              */
  SVG_CMX_EXT_DATA,          /* Request for reading an external file         */
  SVG_CMX_EXT_FREE,          /* Free handle for Request                      */
  SVG_CMX_PREPARE_DONE,      /* SVG decode done, ready for generating frames */
  SVG_CMX_GET_BUF,          /* Frame Buffer size for progressive rendering  */
  SVG_CMX_CONTINUE,          /* Gives up CPU to other tasks                  */
  SVG_CMX_ABORT,             /* Decode session aborted                       */
  SVG_CMX_FAIL,              /* Decode session failed                        */
  SVG_CMX_FRAME_DONE,        /* Frame generated                              */
  SVG_CMX_EVENT_DONE,        /* Event processed                              */
  SVG_CMX_PAUSE,             /* Paused Animation                             */
  SVG_CMX_RESUME,            /* Resumed Animation                            */
  SVG_CMX_REWIND,            /* Rewind Animation                             */
  SVG_CMX_FFORWARD,          /* FForward Animation                           */
  SVG_CMX_GET_TIME,          /* Total playback time calculated               */
  SVG_CMX_DOM_DATA           /* DOM API callback                             */
} svg_cmx_status_type;


/* This is DOM Document data type. The actual data type is stored in Decoder */
typedef void* svg_dom_document_type;
 
#ifdef FEATURE_SVGDOM
/* DOM API type */
typedef enum{
  SVG_DOM_API_NONE = 0,

  SVG_DOM_API_NODE_NSURI,            /* Node::namespaceURI */
  SVG_DOM_API_NODE_LOCALNAME,        /* Node::localName */
  SVG_DOM_API_NODE_PARENT,           /* Node::parentNode */
  SVG_DOM_API_NODE_APPEND,           /* Node::appendChild() */
  SVG_DOM_API_NODE_INSERT,           /* Node::insertBefore() */
  SVG_DOM_API_NODE_REMOVE,           /* Node::removeChild() */
  SVG_DOM_API_NODE_GETDOCUMENT,      /* Node::GetOwnerDocument */
  SVG_DOM_API_NODE_GETCONTENT,      /* Node::GetTextContent */
  SVG_DOM_API_NODE_SETCONTENT,      /* Node::SetTextContent */

  SVG_DOM_API_DOC_CREATE,     /* Document::createElementNS()*/
  SVG_DOM_API_DOC_ELEMENT,        /* Document::documentElement */
  SVG_DOM_API_DOC_GETBYID,        /* Document::getElementById() */
  SVG_DOM_API_DOC_DELETE,        /* Deletes an unused Node */
  SVG_DOM_API_DOC_CREATEEVENT,     /* Document::createEvent()*/
  SVG_DOM_API_DOC_DESTROYEVENT,        /* Destroys an unused Event */

  SVG_DOM_API_ELEMENT_FIRSTCHILD,    /* ElementTraversal::firstElementChild */
  SVG_DOM_API_ELEMENT_LASTCHILD,     /* ElementTraversal::lastElementChild */
  SVG_DOM_API_ELEMENT_NEXTSIBLING,   /* ElementTraversal::nextElementSibling */
  SVG_DOM_API_ELEMENT_PREVSIBLING,   /* ElementTraversal::previousElementSibling */

  SVG_DOM_API_ELEMENT_GETID,         /* SVGElement::Id */
  SVG_DOM_API_ELEMENT_SETID,         /* SVGElement::Id */

  SVG_DOM_API_TRAIT_GETTRAIT,        /* TraitAccess::getTrait*/
  SVG_DOM_API_TRAIT_SETTRAIT,        /* TraitAccess::setTrait*/
  SVG_DOM_API_TRAIT_GETTRAITNS,      /* TraitAccess::getTraitNS*/
  SVG_DOM_API_TRAIT_SETTRAITNS,      /* TraitAccess::setTraitNS*/
  SVG_DOM_API_TRAIT_GETFLOAT,        /* TraitAccess::getFloatTrait*/
  SVG_DOM_API_TRAIT_SETFLOAT,        /* TraitAccess::setFloatTrait*/
  SVG_DOM_API_TRAIT_GETMATRIX,       /* TraitAccess::getMatrixTrait*/
  SVG_DOM_API_TRAIT_SETMATRIX,       /* TraitAccess::setMatrixTrait*/
  SVG_DOM_API_TRAIT_GETRECT,         /* TraitAccess::getRectTrait*/
  SVG_DOM_API_TRAIT_SETRECT,         /* TraitAccess::setRectTrait*/
  SVG_DOM_API_TRAIT_GETPATH,         /* TraitAccess::getPathTrait*/
  SVG_DOM_API_TRAIT_SETPATH,         /* TraitAccess::setPathTrait*/
  SVG_DOM_API_TRAIT_GETRGB,          /* TraitAccess::getRGBColorTrait*/
  SVG_DOM_API_TRAIT_SETRGB,          /* TraitAccess::setRGBColorTrait*/

  SVG_DOM_API_TRAIT_GETPRESTRAIT,    /* TraitAccess::getPresentationTrait */
  SVG_DOM_API_TRAIT_GETPRESTRAITNS,  /* TraitAccess::getPresentationTraitNS */
  SVG_DOM_API_TRAIT_GETPRESFLOAT,    /* TraitAccess::getFloatPresentationTrait */
  SVG_DOM_API_TRAIT_GETPRESMATRIX,   /* TraitAccess::getMatrixPresentationTrait */
  SVG_DOM_API_TRAIT_GETPRESPATH,     /* TraitAccess::getPathPresentationTrait */
  SVG_DOM_API_TRAIT_GETPRESRECT,     /* TraitAccess::getRectPresentationTrait */
  SVG_DOM_API_TRAIT_GETPRESCOLOR,    /* TraitAccess::getRGBColorPresentationTrait */

  SVG_DOM_API_EVTTARGET_ADD,         /* EventTarget::addEventListener */
  SVG_DOM_API_EVTTARGET_RMV,         /* EventTarget::removeEventListener */
  SVG_DOM_API_EVTTARGET_DISPATCH,    /* EventTarget::DispatchEvent */
  SVG_DOM_API_EVTLISTENER_CREATE,     /* Creates EventListner */
  SVG_DOM_API_EVTLISTENER_FREE,       /* Frees EventListner */

  SVG_DOM_API_EVENT_GETTARGET,       /* Event::currentTarget */
  SVG_DOM_API_EVENT_GETTYPE,         /* Event::type */
  SVG_DOM_API_EVENT_SCREENX,         /* MouseEvent::screenX */
  SVG_DOM_API_EVENT_SCREENY,         /* MouseEvent::screenY */
  SVG_DOM_API_EVENT_CLIENTX,         /* MouseEvent::clientX */
  SVG_DOM_API_EVENT_CLIENTY,         /* MouseEvent::clientY */
  SVG_DOM_API_EVENT_BUTTON,          /* MouseEvent::button */
  SVG_DOM_API_EVENT_DATA,            /* TextEvent::data */
  SVG_DOM_API_EVENT_KEY,             /* KeyEvent::keyIdentifier */
  SVG_DOM_API_EVENT_DETAIL,          /* TimeEvent::detail */
  SVG_DOM_API_EVENT_INIT,            /* Initializes an Event */

  SVG_DOM_API_MATRIX_GET,            /* SVGMatrix::getComponent*/
  SVG_DOM_API_MATRIX_MULTIPLY,       /* SVGMatrix::mMultiply*/
  SVG_DOM_API_MATRIX_INVERSE,        /* SVGMatrix::mInverse*/
  SVG_DOM_API_MATRIX_TRANSLATE,      /* SVGMatrix::mTranslate*/
  SVG_DOM_API_MATRIX_SCALE,          /* SVGMatrix::mScale*/
  SVG_DOM_API_MATRIX_ROTATE,         /* SVGMatrix::mRotate*/

  SVG_DOM_API_LOCATABLE_GETBBOX,     /* SVGLocatableElement::getBBox */
  SVG_DOM_API_LOCATABLE_GETSCTM,     /* SVGLocatableElement::getScreenCTM */
  SVG_DOM_API_LOCATABLE_GETSBOX,     /* SVGLocatableElement::getScreenBBox*/

  SVG_DOM_API_ELEMENT_BEGIN,       /* SVGAnimationElement::beginElement */
  SVG_DOM_API_ELEMENT_BEGINAT,     /* SVGAnimationElement::beginElementAt*/
  SVG_DOM_API_ELEMENT_END,         /* SVGAnimationElement::endElement*/
  SVG_DOM_API_ELEMENT_ENDAT,       /* SVGAnimationElement::endElementAt */
  SVG_DOM_API_ELEMENT_PAUSE,       /* SVGAnimationElement::unpauseElement*/
  SVG_DOM_API_ELEMENT_UNPAUSE,     /* SVGAnimationElement::pauseElement*/
  SVG_DOM_API_ELEMENT_PAUSED,      /* SVGAnimationElement::elementPaused*/

  SVG_DOM_API_SVGELEMENT_CREATEMATRIX, /* SVGSVGElement::createSVGMatrixComponents */
  SVG_DOM_API_SVGELEMENT_CREATERECT,   /* SVGSVGElement::createSVGRect */
  SVG_DOM_API_SVGELEMENT_CREATEPATH,   /* SVGSVGElement::createSVGPath */
  SVG_DOM_API_SVGELEMENT_CREATECOLOR,  /* SVGSVGElement::createSVGRGBColor */
  SVG_DOM_API_SVGELEMENT_DELETEMATRIX, /* Deletes a SVGMatrix */
  SVG_DOM_API_SVGELEMENT_DELETERECT,   /* Deletes a SVGRect */
  SVG_DOM_API_SVGELEMENT_DELETEPATH,   /* Deletes a SVGPath */
  SVG_DOM_API_SVGELEMENT_DELETECOLOR,  /* Deletes a SVGColor */
  SVG_DOM_API_SVGELEMENT_GETSCALE,     /* SVGSVGElement::currentScale */
  SVG_DOM_API_SVGELEMENT_GETROTATE,    /* SVGSVGElement::currentRotate */
  SVG_DOM_API_SVGELEMENT_GETTRANSLATE, /* SVGSVGElement::currentTranslate */
  SVG_DOM_API_SVGELEMENT_GETVIEWPORT,  /* SVGSVGElement::viewport */
  SVG_DOM_API_SVGELEMENT_GETTIME,      /* SVGSVGElement::currentTime */
  SVG_DOM_API_SVGELEMENT_SETSCALE,     /* SVGSVGElement::currentScale */
  SVG_DOM_API_SVGELEMENT_SETROTATE,    /* SVGSVGElement::currentRotate */
  SVG_DOM_API_SVGELEMENT_SETTIME,      /* SVGSVGElement::currentTime */
  SVG_DOM_API_SVGELEMENT_SETTRANSLATE, /* SVGSVGElement::currentTranslate */
  SVG_DOM_API_SVGELEMENT_PAUSE,        /* SVGSVGElement::pauseAnimations */
  SVG_DOM_API_SVGELEMENT_UNPAUSE,      /* SVGSVGElement::unpauseAnimations */
  SVG_DOM_API_SVGELEMENT_PAUSED,       /* SVGSVGElement::animationsPaused */
  SVG_DOM_API_SVGELEMENT_MOVEFOCUS,       /* SVGSVGElement::moveFocus */
  SVG_DOM_API_SVGELEMENT_SETFOCUS,        /* SVGSVGElement::setFocus */
  SVG_DOM_API_SVGELEMENT_GETFOCUSEDOBJECT,/* SVGSVGElement::getCurrentFocusedObject */

  SVG_DOM_API_SVGPATH_NOSEGMENTS,      /* SVGPath::numberOfSegments */
  SVG_DOM_API_SVGPATH_GETSEGMENT,      /* SVGPath::getSegment */
  SVG_DOM_API_SVGPATH_GETSEGMENTPARAM, /* SVGPath::getSegmentParam */
  SVG_DOM_API_SVGPATH_MOVETO,          /* SVGPath::moveTo */
  SVG_DOM_API_SVGPATH_LINETO,          /* SVGPath::lineTo */
  SVG_DOM_API_SVGPATH_QUADTO,          /* SVGPath::quadTo */
  SVG_DOM_API_SVGPATH_CURVETO,          /* SVGPath::curveTo */
  SVG_DOM_API_SVGPATH_CLOSE,            /* SVGPath::Close */

  SVG_DOM_API_MAX
}svg_dom_api_enum_type;

/* DOM exception type */
typedef enum {
  SVG_DOM_NO_ERROR = 0,
  SVG_DOM_INDEX_SIZE_ERR = 1,
  SVG_DOM_HIERARCHY_REQUEST_ERR = 3,
  SVG_DOM_WRONG_DOCUMENT_ERR = 4,
  SVG_DOM_NO_MODIFICATION_ALLOWED_ERR = 7,
  SVG_DOM_NOT_FOUND_ERR = 8,
  SVG_DOM_NOT_SUPPORTED_ERR = 9,
  SVG_DOM_INVALID_STATE_ERR = 11,
  SVG_DOM_INVALID_MODIFICATION_ERR = 13,
  SVG_DOM_INVALID_ACCESS_ERR = 15,
  SVG_DOM_TYPE_MISMATCH_ERR = 17,

  SVG_DOM_EVENT_UNSPECIFIED_EVENT_TYPE_ERR = 100,
  SVG_DOM_EVENT_OUT_OF_MEMORY_ERR,

  SVG_DOM_SVG_INVALID_VALUE_ERR = 200,
  SVG_DOM_SVG_MATRIX_NOT_INVERTABLE

} svg_dom_exception_type;

/* DOM float type */
/* Floats are represented in 16.16 fixed format */
typedef uint32 svg_dom_fixed;

/* DOM Node */
typedef void* svg_dom_node_type;

/* DOM SVGElement */
typedef svg_dom_node_type svg_dom_element_type;

/* DOM DOMString */
typedef uint16* svg_dom_string_type;

/* DOM Event type */
typedef void* svg_dom_event_type;

/* DOM Event Target type */
typedef void* svg_dom_evttarget_type;

/* DOM Event Listener type */
typedef void* svg_dom_evtlistener_type;

/* DOM HandleEvent macro */
typedef void (*svg_dom_handle_event_type)( void* client_data, void *p, svg_dom_event_type *evt );

/* DOM arguments to Event Target interface */
typedef struct{
  svg_dom_node_type target;      /* Pointer to Event Target */
  svg_dom_string_type  type;          /* Pointer to Event type */
  svg_dom_evtlistener_type listener;      /* Pointer to Event Listener */
  boolean useCapture;
  svg_dom_event_type event;
} svg_dom_evttarget_data_type;

typedef struct{
  svg_dom_evtlistener_type  listener;
  svg_dom_handle_event_type handler;
  void*                     client_data;
} svg_dom_evthandler_type;

typedef struct{
  svg_dom_event_type event;             /* Pointer to Event */
  svg_dom_string_type  evttarget_type;  /* Event Target type */
  boolean canbubble;
  boolean cancelable;
} svg_dom_event_data_type;

typedef void* svg_dom_matrix_type;


typedef void* svg_dom_path_type;


typedef struct{
  svg_dom_fixed x, y;
} svg_dom_point_type;


typedef struct{
  svg_dom_fixed x, y, width, height;
} svg_dom_rect_type;



typedef struct{
  uint32 red, green, blue;
} svg_dom_color_type;


/* DOM type of data */
typedef enum{
  SVG_DOM_DATA_NONE = 0,
  SVG_DOM_DATA_DOCUMENT,        /* doc is valid */
  SVG_DOM_DATA_ELEMENT,     /* element is valid */
  SVG_DOM_DATA_NODE,            /* node is valid */
  SVG_DOM_DATA_NS,          /* ns_value is valid */

  SVG_DOM_DATA_STRING,      /* string is valid */
  SVG_DOM_DATA_FLOAT,       /* f is valid */
  SVG_DOM_DATA_BOOL,            /* b is valid */
  SVG_DOM_DATA_LONG,            /* l is valid */
  SVG_DOM_DATA_SHORT,       /* s is valid */

  SVG_DOM_DATA_EVENT,       /* event is valid */
  SVG_DOM_DATA_EVTTARGET,    /* evttarget is valid */
  SVG_DOM_DATA_EVTLISTENER,  /* evtlistener is valid */

  SVG_DOM_DATA_MATRIX,      /* matrix is valid */
  SVG_DOM_DATA_MATRIX_DATA,  /* matrix_data is valid */
  SVG_DOM_DATA_POINT,       /* point is valid */
  SVG_DOM_DATA_PATH,            /* path is valid */
  SVG_DOM_DATA_RECT,            /* rect is valid */
  SVG_DOM_DATA_RECT_PTR,        /* rect_ptr is valid */
  SVG_DOM_DATA_COLOR,       /* color is valid */
  SVG_DOM_DATA_COLOR_PTR,       /* color_ptr is valid */

  SVG_DOM_DATA_SVGSVGELEMENT   /* ns_value is valid */

} svg_dom_data_enum_type;

/* DOM arguments to Node interface */
typedef struct{
  svg_dom_node_type node;           /* Parent Node */
  svg_dom_node_type child;          /* Child Node  */
  svg_dom_node_type refchild;       /*Reference Child*/
  svg_dom_string_type content;       /* string for TextContent */
} svg_dom_node_data_type;

/* DOM arguments to Document interface */
typedef struct{
  svg_dom_document_type document;   /* Pointer to Document */
  svg_dom_string_type   name;   /* Pointer to Qualified name or ID */
  union{
    svg_dom_string_type   URI;        /* Pointer to NS URI */
    svg_dom_element_type  element;       /* Pointer to Element */
    svg_dom_event_type  event;       /* Pointer to Event */
  }data;
}svg_dom_doc_data_type;

/* DOM arguments to SVGElement interface */
typedef struct{
  
  svg_dom_element_type element;
  svg_dom_string_type name;
  svg_dom_data_enum_type type;    /* Type of data in union */
  union{
    svg_dom_string_type string;
    svg_dom_matrix_type matrix;
    svg_dom_path_type   path;
    svg_dom_rect_type   rect;
    svg_dom_color_type   color;
    svg_dom_fixed                  fl;
     struct{
      svg_dom_string_type ns;
      svg_dom_string_type value;
    }ns_value;
  }data;
} svg_dom_element_data_type;

/* DOM arguments to SVGMatrix interface */
typedef struct{
  svg_dom_matrix_type matrix;
  svg_dom_exception_type error;
  svg_dom_data_enum_type type;
  union{
    svg_dom_matrix_type secondMatrix;   /* Used with mMultiply */
    svg_dom_fixed f;            /* Used with mScale and mRotate */
    svg_dom_point_type p;         /* Used with mTranslate */
    long  index;        /* Used with getComponent */
  }data;
}svg_dom_matrix_data_type;

/* DOM arguments to SVGSVGElement interface */
typedef struct{
//  svg_dom_document_type document;   /* Pointer to Document */
  svg_dom_element_type element;
  svg_dom_data_enum_type type;
  union{
    svg_dom_fixed f ;               /* Used with currentScale, currentRotate & currentTime */
    struct{
      long red, green, blue;
    }color;                 /* Used with createSVGRGBColor */
    struct{
      svg_dom_fixed a, b, c, d, e, f;
    }matrix_data;                /* Used with createSVGMAtrixComponents */
    svg_dom_point_type point; /* Used with setCurrentTranslate */
    svg_dom_path_type path; /* Used with dleeteSVGPath */
    svg_dom_matrix_type m; /* Used with dleeteSVGMatrix */
    svg_dom_rect_type *rect_ptr; /* Used with dleeteSVGRect */
    svg_dom_color_type *color_ptr; /* Used with dleeteSVGRGBColor */
    uint8               s;         /* Used with MoveFocus */
    svg_dom_node_type   node;      /* Used with setFocus */
  }data;
} svg_dom_svgelement_data_type;

typedef struct{
  svg_dom_path_type  path;
  union{
    uint32  index;
    struct{
      uint32 cmd, param;
    }param;
    struct{
      svg_dom_fixed x1, y1;
    }move;
    struct{
      svg_dom_fixed x1, y1;
    }line;
    struct{
      svg_dom_fixed x1, y1, x2, y2;
    }quad;
    struct{
      svg_dom_fixed x1, y1, x2, y2, x3, y3;
    }curve;
  }data;
} svg_dom_svgpath_data_type;

/* DOM callback function data */
typedef struct{
  svg_dom_api_enum_type  api;       /* API called */
  svg_dom_exception_type error; /* Exception if any */
  svg_dom_data_enum_type type;    /* Type of data in union */
  uint32                 num_bytes; /* No of bytes in data */
  union{
    svg_dom_string_type      string;    /* DOMString data */
    struct{
      svg_dom_string_type ns;
      svg_dom_string_type value;
    }ns_value;                      /* Namespace data */
    svg_dom_document_type    doc;    /* document data */
    svg_dom_element_type    element;    /* element data */
    svg_dom_node_type       node;   /* node data */
    svg_dom_event_type  event;  /* Event data */
    svg_dom_evttarget_type evttarget; /* EventTarget data */
    svg_dom_evtlistener_type evtlistener; /* EventListener data */
    svg_dom_point_type     *point;  /* SVGPoint data */
    svg_dom_matrix_type     matrix; /* SVGMatrix data */
    svg_dom_path_type       path;   /* SVGPath data */
    svg_dom_rect_type       *rect;   /* SVGRect data */
    svg_dom_color_type      *color;  /* SVGRGBColor data */
    svg_dom_fixed           f;      /* float data */
    boolean                 b;      /* bool data */
    int                     i;      /* int data */
    long                    l;      /* long data */
    uint16                  s;      /* unsigned short data */
  }data;
} svg_dom_cmx_data_type;

/* DOM API data type */
typedef union{
  svg_dom_node_type             node;
  svg_dom_node_data_type        node_data;
  svg_dom_document_type         doc;
  svg_dom_doc_data_type         doc_data;
  svg_dom_event_type            event;
  svg_dom_event_data_type       event_data;
  svg_dom_evttarget_data_type   evttarget_data;
  svg_dom_evtlistener_type      evtlistener;
  svg_dom_evthandler_type       evthandler;
  svg_dom_element_type          element;
  svg_dom_element_data_type     element_data;
  svg_dom_matrix_data_type      matrix_data;
  svg_dom_svgelement_data_type  svgelement_data;
  svg_dom_svgpath_data_type     svgpath_data;
} svg_dom_data_type;
#endif /* FEATURE_SVGDOM */


/* CMX callback function */
typedef void (*svg_cmx_cb_type)(svg_cmx_status_type svg_status, void *handle, 
                                void* server_data ); 

/* SVG decode state callback */
typedef void (*svg_decoder_cb_type)( void* handle ); 

/* Callback function used for processing requests */
typedef void (*svg_cmx_request_cb_type) ( void* client_data, void* cmx_handle, char* filename);

/* Callback function used for processing requests */
typedef void (*svg_cmx_get_request_cb_type) ( void* client_data);

/* This struct is used for loading SVG animations */
typedef struct {
    svg_cmx_cb_type  cb_func;               /* general CMX callback           */
    void*            cb_data;
    uint8*           mem_buffer;            /* Memory allocated upfront       */
    uint32           mem_buffer_size;       /* Size of memory allocated upfront */

    uint16           svg_frame_width;       /* Width of display area          */
    uint16           svg_frame_height;      /* Height of display area         */

    uint16           dpi;                  /* Dots per inch of display        */
    uint8            transparency;         /* true= transpaprency tirned on   */
    uint8            max_requests;         /* Max no of requests supported    */
    boolean          progressive;          /* true = Progressive rendering turned on */
    uint32           bk_color;             /* Background color                */
    char*            filename;             /* Filename                        */

}svg_load_data_type;

/* This struct is used with Continue cb */
typedef struct{
  svg_decoder_cb_type  callback_ptr;      /* callback function                */
  void                *client_data;       /* callback data                    */
} svg_cmx_continue_type;
                                          
/* This struct is used with Get data cb */
typedef struct{
    uint8              buffer[SVG_CHUNK_SIZE];  /* contents of svg file       */
    uint32             position;                /* position in the svg file   */
    uint32             num_bytes;               /* no of bytes in buffer      */
    uint32             rem_bytes;               /* no of bytes remaining      */
    svg_decoder_cb_type     callback_ptr;       /* callback function          */
    void              *client_data;             /* callback data              */
} svg_cmx_get_data_type;

/* This struct is used with Info cb */
typedef struct{
  boolean              animated;               /* true= Animations            */
  boolean              zoom_pan_events;        /* true= Zoom and Pan supported */
  boolean              text_only;              /* true= supports Text-only mode */
  boolean              bk_seek;                /* true= backward seek allowed */
  boolean              user_event;             /* ture= user event trigger in content */
  char                 title[128];             /* Title defined in SVG file   */
  cmx_color_scheme_enum_type   color_scheme;   /* Color scheme used           */
  void*                handle;                 /* SVG handle                  */
  uint32               width, height;          /* Size of viewport            */
  uint32               num_requests;           /* No of URL Requests          */
  svg_dom_document_type doc;                   /* DOM document handle         */
}svg_cmx_info_type;

/* This struct is used with render framec function */
typedef struct{
  uint32            time_ms;  
  uint8*            frame_buf;                /* Frame buffer                */
  uint32            frame_size;               /* size of frame buffer        */
  boolean           seek;
}svg_frame_data_type;

/* This struct is used with Frame Done cb */
typedef struct{
  uint8       *frame_buf;                     /* pointer to frame buffer     */
  uint32      frame_size;                     /* size of frame buffer        */
  uint32      time_ms;                        /* Time in ms                  */
  boolean     flag_last_frame;                /* true = last frame           */
} svg_cmx_frame_done_type;

/* This struct is used for Event done cb */
typedef struct{
  boolean     handled;                       /* true = event handled         */
  cmx_svg_event_enum_type   event;           /* Event that was passed        */
}svg_cmx_event_done_type;

/* This struct is sued for EXT REQ cb */
typedef struct{
  char        filename[FS_FILENAME_MAX_LENGTH_P];   /* Name of external file  */
  uint8       type;       /* 0-Required, 1-Not-required, 2-Replace, 3-Don't replace*/
  svg_cmx_get_request_cb_type next_cb;      /* callback for next request      */
  svg_cmx_request_cb_type   callback_ptr;   /* callback function              */
  void       *client_data;                  /* callback data                  */
  uint8     **buf_ptr;                      /* Pointer to buffer storing file */
  uint32     *num_bytes;                    /* No of bytes in buffer          */
} svg_cmx_request_type;

/* This struct is used for EXT REQ DATA cb */
typedef struct{
  uint8                   *buffer;          /* stores the contents of svg file */
  uint32                  position;         /* position in the svg file       */
  uint32                  num_bytes;        /* no of bytes in buffer          */
  svg_cmx_request_cb_type callback_ptr;     /* callback function              */
  void                    *client_data;     /* callback data                  */
} svg_cmx_req_data_type;

/* This struct is used for Request Free cb */
typedef struct{
  boolean     handled;                       /* true = request handled successfully */
  char        filename[FS_FILENAME_MAX_LENGTH_P];   /* Name of external file  */
}svg_cmx_request_done_type;

/* <EJECT> */
/*===========================================================================

FUNCTION svg_load_animation

DESCRIPTION
  This function starts the SVG decoder

DEPENDENCIES
  None

RETURN VALUE
  svg_handle_type - pointer to svg_handle

SIDE EFFECTS
  None
===========================================================================*/
void* svg_load_animation( 
  svg_load_data_type* svg_load_data 
);

/* <EJECT> */
/*===========================================================================

FUNCTION svg_render_frame

DESCRIPTION
  This function renders an Animation frame. The frame buffer should not be
  reset/changed by the caller as the SVG decoder is optimized to only redraw 
  the rectangle that has changed since the previous frame. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean svg_render_frame( 
 void* handle, 
 svg_frame_data_type* frame_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION svg_process_event

DESCRIPTION
  This function Processes user events. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  SVG_CMX_EVENT_DONE callback
  SVG_CMX_FRAME callback
  SVG_CMX_EXT_REQ callback
===========================================================================*/
void svg_process_event( 
  void* handle,         
  cmx_svg_event_enum_type   event,     
  void*           event_data,
  uint32          delay
);

/* <EJECT> */
/*===========================================================================

FUNCTION svg_get_time

DESCRIPTION
  This function gets the time for SVG file playback. The function plays the file
  for this getting the total time.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void svg_get_time( 
  void* handle, 
  uint8*  buffer, 
  uint32 buffer_size
);

/* <EJECT> */
/*===========================================================================

FUNCTION svg_decode_abort

DESCRIPTION
  This function Aborts a SVG session 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void svg_decode_abort( 
  void* handle      
);

/* <EJECT> */
/*===========================================================================

FUNCTION svg_decode_stop

DESCRIPTION
  This function Cleansup all memory allocated for SVG decoder. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void svg_decode_stop( 
  void* handle        
);

/* <EJECT> */
/*===========================================================================

FUNCTION svg_decode_pause

DESCRIPTION
  This function Pauses a SVG session 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void svg_decode_pause( 
  void* handle          
);

/* <EJECT> */
/*===========================================================================

FUNCTION svg_decode_resume

DESCRIPTION
  This function resumes a SVG session 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void svg_decode_resume( 
  void* handle          
);

/* <EJECT> */
/*===========================================================================

FUNCTION svg_frame_rewind

DESCRIPTION
  This function rewinds SVG Animations to the given time

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  svg_handle->frame_time
===========================================================================*/
void svg_frame_rewind( 
  void* handle, 
  uint32 delay
);

/* <EJECT> */
/*===========================================================================

FUNCTION svg_frame_fforward

DESCRIPTION
  This function fast-forwards SVG Animations to the given time

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  svg_handle->frame_time
===========================================================================*/
void svg_frame_fforward( 
  void* handle, 
  uint32 delay
);

/* <EJECT> */
/*===========================================================================

FUNCTION svg_reset_frame

DESCRIPTION
  This function resets the Zoom and Pan factors of SVG session.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void svg_reset_frame( 
  void*  handle
);

/* <EJECT> */
/*===========================================================================

FUNCTION svg_set_viewport

DESCRIPTION
  This function sets the size of Viewport for SVG decoder.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void svg_set_viewport( 
  void* handle, 
  uint32 width, 
  uint32 height
);

/* <EJECT> */
/*===========================================================================

FUNCTION svg_process_request

DESCRIPTION
  This function processes callbacks for external file requests

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void svg_process_request( 
  void* handle, 
  void* cmx_handle,
  char* filename
);

#ifdef FEATURE_SVGDOM
/* <EJECT> */
/*===========================================================================

FUNCTION svg_dom_apis

DESCRIPTION
  This function is calls the SVG DOM APIs

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void svg_dom_apis(
  void* handle,
  svg_dom_api_enum_type type,
  svg_dom_data_type* data
);
#endif /* FEATURE_SVGDOM */

#endif /* FEATURE_SVG */
#endif /*SVG_H */
