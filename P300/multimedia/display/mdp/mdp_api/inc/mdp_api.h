#ifndef MDP_API_H
#define MDP_API_H
/*============================================================================

                   MDP Device Driver Header File

  DESCRIPTION
  This file contains the definitions needed for the MDP device driver.

  Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.


when      who    what, where, why
--------  -----  ----------------------------------------------------------
08/20/07  spw    Add outbuf_addr in MDPIMG struct for nondestructive blt
07/25/07  cjr    Add mdp_refresh_viewport_async() API
03/30/07  trg    Add interface for qvga to wqvga scaling for 6k.  Use 
                 FEATURE_MDP_WSCALE to enable.
03/20/07  cl     Added MDP profiling support
12/15/06  dw     Added support for adaptive backlight control
07/17/06  dw     Remove FEATURE_DISP_TASK
04/06/06  dw     Enable dithering operation
04/03/06  cjr    Added MDP reserved field
03/13/06  cjr    Added ascale support for MDP1
02/23/06  cjr    Added mdp_refresh_viewport()
02/22/06  trg    Add XRGB support for MDP 1
10/13/05  cjr    Added MDPOP_QUEUE_EXPAND
08/09/05  cjr    disp task & async API support
05/05/05  cjr    Added MDP_Y_CBCR_H2V2 image type
05/02/05  cjr    Initial code
============================================================================*/

//============================================================================
//                        INCLUDE FILES
//============================================================================

#include "comdef.h"
#include "disp.h"

/* -----------------------------------------------------------------------
** MDP Status
** ----------------------------------------------------------------------- */
typedef unsigned int MDP_STATUS;

#define MDP_FAIL   0
#define MDP_SUCCESSFUL 1

/* -----------------------------------------------------------------------
** MDP FFR/MBC Operation
** ----------------------------------------------------------------------- */
#ifdef BIT
#undef BIT
#endif

#define BIT(x)  (1<<(x))

#define MDPOP_NOP               0
#define MDPOP_LR                BIT(0)     /* left to right flip */
#define MDPOP_UD                BIT(1)     /* up and down flip */
#define MDPOP_ROT90             BIT(2)     /* rotate image to 90 degree */
#define MDPOP_ROT180            (MDPOP_UD|MDPOP_LR)
#define MDPOP_ROT270            (MDPOP_ROT90|MDPOP_UD|MDPOP_LR)

#define MDPOP_VDO_X2            BIT(3)     /* video up scale by 2 */
#define MDPOP_VDO_1BY4          BIT(4)     /* video down scale by 1/4 */
#define MDPOP_VDO_1BY2          BIT(5)     /* video down scale by 1/2 */
#define MDPOP_VDO_3BY4          BIT(6)     /* video down scale by 3/4 */

// arbitrary scale 
// for arbitrary scaling, you must set ascale field.
// ascale field defines the dimension after scaling operation
#define MDPOP_ASCALE            BIT(7)         

#define MDPOP_ALPHAB            BIT(8)     /* enable alpha blending */
#define MDPOP_TRANSP            BIT(9)     /* enable transparency */
#define MDPOP_VSYNC_OFF         BIT(10)    /* disable vsync */
#define MDPOP_SEL_CCS           BIT(11)    /* select CCS */

// This option is to copy user's buffer pointed by bmy_addr/cbcr_addr 
// into display task queue buffer. Video image will get color-converted 
// through ARM to offload MDP block
#define MDPOP_QUEUE_COPY        BIT(12)    

// This option is to allow MDP to optimize display queue.
// It will enable MDP to drop this update if the following update 
// occupies the same area of this update,
#define MDPOP_FAST_QUEUE        BIT(13)    

#ifdef FEATURE_MDP2
#error code not present
#endif

// This option is to allow MDP to optimize display queue.
// When the display free queue is empty, it will enable MDP to search the 
// previously owned display buffer from the pending queue and merge the
// current update region on the top of this buffer
// Note: The expansion is always in rectangular shape and it can include the 
//       region of "NOT" interest.  And it will act as "MDPOP_FAST_QUEUE" for
//       all overlapped region of interest

//#define FEATURE_MDP_QUEUE_EXPAND

#ifdef FEATURE_MDP_QUEUE_EXPAND
#define MDPOP_QUEUE_EXPAND      BIT(15)    
#else
#define MDPOP_QUEUE_EXPAND  0   
#endif

#define MDPOP_DITHER_OFF        BIT(16)

// MDP profiling
// For MDP profiling, user must set the MDPOP_PROFILING MDP option to indicate
// that current MDPIMG should be included in the profiling.  
// Note: mdp_profiling_start() should be called for MDP profiling to occur
#ifdef FEATURE_DISPLAY_PROFILING
#define MDPOP_PROFILING         BIT(17)    
#endif /* FEATURE_DISPLAY_PROFILING */


//  This feature is supported only on MDP cores 1.3 and beyond (65nm).
//    WSCALE feature scales the OUTPUT of the MDP core from QVGA (width = 240
//    height = 320) to WQVGA (width=240, height=400).  This is a very specific
//    HW feature since it will only work on full screen (QVGA) data.  Please
//    refer to the application notes before attempting to use this feature!
//    This HW feature is not scaleable to other LCD sizes.
//    
//    LINEAR_STRETCH:
//       INPUT SIZE  = 240 x 320
//       OUTPUT SIZE = 240 x 400
//       STRETCH     = x1 in width, x1.25 in height
// 
//    EDGE_STRETCH:
//       INPUT SIZE  = 240 x 320
//       OUTPUT SIZE = 240 x 400
//       STRETCH     = x1 in width, x1 to x2 in height variable (center 
//                       aspect ratio maintained, increase scaling on edges)
// 
//    NO_STRETCH:
//       INPUT SIZE  = 192 x 320 (cropped QVGA content)
//       OUTPUT SIZE = 240 x 400
//       STRETCH     = x1.25 in width and height

#ifdef FEATURE_MDP_WSCALE
#define MDPOP_WSCALE_LINEAR_STRETCH  BIT(20)
#define MDPOP_WSCALE_EDGE_STRETCH    BIT(21)
#define MDPOP_WSCALE_NO_STRETCH      BIT(22)
#endif // #ifdef FEATURE_MDP_WSCALE

/* -----------------------------------------------------------------------
** MDP Foreground/Background Image
** ----------------------------------------------------------------------- */

#ifdef FEATURE_MDP2
#error code not present
#else
#define MDP_MAX_LAYER       3

typedef enum
{
  MDP_LAYER_0,
  MDP_LAYER_1,         
  MDP_LAYER_2   
} MDP_LAYER;   
#endif

#define MDP_TOP_MOST_LAYER       ((MDP_LAYER)(MDP_MAX_LAYER-1))
#define MDP_BOTTOM_MOST_LAYER    MDP_LAYER_0

#define MDP_FGL                  MDP_LAYER_2
#define MDP_BGL                  MDP_LAYER_0

/* -----------------------------------------------------------------------
** MDP Image Type defines
** ----------------------------------------------------------------------- */
typedef enum
{  
   MDP_RGB_565,      // RGB 565 planer 
   MDP_XRGB_8888,    // RGB 888 padded
   MDP_Y_CBCR_H2V2,  // Y and CbCr, pseudo planer w/ Cb is in MSB
#ifdef FEATURE_MDP2   
#error code not present
#endif
   MDP_IMGTYPE_LIMIT // Non valid image type after this enum
}MDPIMG_TYPE;

#define MDP_RGB565   MDP_RGB_565
#define MDP_YCBCR420 MDP_Y_CBCR_H2V2

/* -----------------------------------------------------------------------
** Image Rectangular Region
** ----------------------------------------------------------------------- */
typedef struct mdp_cwin_
{
  int32 x1;         /* starting column */
  int32 x2;         /* ending column */
  int32 y1;         /* starting row */
  int32 y2;         /* ending row */
} MDP_CWIN;

typedef MDP_CWIN MDP_OVER_XYInt;

/* -----------------------------------------------------------------------
** {3x3} + {3} ccs matrix
** ----------------------------------------------------------------------- */
typedef struct mdp_ccs_type_
{
  uint16 ccs1;
  uint16 ccs2;
  uint16 ccs3;
  uint16 ccs4;
  uint16 ccs5;
  uint16 ccs6;
  uint16 ccs7;
  uint16 ccs8;
  uint16 ccs9;
  uint16 ccs10;
  uint16 ccs11;
  uint16 ccs12;  
}MDP_CCS_TYPE;

/* -----------------------------------------------------------------------
** MDP AScale and Capture Structure
** ----------------------------------------------------------------------- */
 
typedef struct mdp_ascale_
{
  uint32 width;
  uint32 height;
}MDP_ASCALE;

#ifdef FEATURE_MDP2   
#error code not present
#endif /* FEATURE_MDP2 */

#ifdef FEATURE_DISPLAY_PROFILING
/* -----------------------------------------------------------------------
** MDP profiling result structure
** ----------------------------------------------------------------------- */
typedef struct mdp_profile_result
{
  uint32 num_sample;              /* total number of samples profiled */ 
  boolean vsync_enabled;          /* whether vsync is enabled or not */
  uint32 num_blending[MDP_MAX_LAYER];  /* number of blending times             */ 
  uint32 avg_blending[MDP_MAX_LAYER];  /* average blending time for each layer */ 
  uint32 min_blending[MDP_MAX_LAYER];  /* minimum blending time for each layer */ 
  uint32 max_blending[MDP_MAX_LAYER];  /* maximum blending time for each layer */ 
  uint32 num_mapping[MAX_LCD_NUM];   /* number of mapping times            */ 
  uint32 avg_mapping[MAX_LCD_NUM];   /* average mapping time for each lcd  */ 
  uint32 min_mapping[MAX_LCD_NUM];   /* minimum mapping time for each lcd  */ 
  uint32 max_mapping[MAX_LCD_NUM];   /* maximum mapping time for each lcd  */ 
  uint32 num_update[MAX_LCD_NUM];   /* number of update times            */ 
  uint32 avg_update[MAX_LCD_NUM];   /* average update time for each lcd  */ 
  uint32 min_update[MAX_LCD_NUM];   /* minimum update time for each lcd  */ 
  uint32 max_update[MAX_LCD_NUM];   /* maximum update time for each lcd  */ 
  uint32 avg_interframe;          /* average interframe time         */ 
  uint32 min_interframe;          /* minimum interframe time         */ 
  uint32 max_interframe;          /* maximum interframe time         */ 
} MDP_PROFILE_RESULT;
#endif /* FEATURE_DISPLAY_PROFILING */


 /* -----------------------------------------------------------------------
** MDP Image Structure
** ----------------------------------------------------------------------- */
typedef struct mdpImg_
{ 
  LCD_TYPE dest;       /* PRIMARY_LCD_TYPE or SECONDARY_LCD_TYPE */
  MDPIMG_TYPE imgType; /* MDP Image type */  
  uint32 *bmy_addr;    /* bitmap or y addr */
  uint32 *cbcr_addr;   /* cbcr addr */
  uint32 width;        /* image width */
  MDP_CWIN cwin;       /* sub window */
  int32 lcd_x;         /* LCD x location */
  int32 lcd_y;         /* LCD y location */
  uint32 mdpOp;        /* image opertion (rotation,flip up/down, alpha/tp) */
  uint32 tpVal;        /* transparency color */
  uint32 alpha;        /* alpha percentage 0%(0x0) ~ 100%(0x100) */
  MDP_LAYER layer;     /* foreground(MDP_FGL) or background(MDP_BGL) */
  MDP_CCS_TYPE * ccsPtr; /* color conversion coeff pointer if MDPOP_SEL_CCS defined */
  MDP_ASCALE ascale;   /* arbitrary scale info */
#ifdef FEATURE_MDP2   
#error code not present
#endif /* FEATURE_MDP2 */
  uint32 tag;          /* do not use - internal field */
#ifdef FEATURE_WINCE
#error code not present
#endif /* FEATURE_WINCE */
} MDPIMG;

/* -----------------------------------------------------------------------
** MDP Callback Structure
** ----------------------------------------------------------------------- */

typedef struct
{  
  void (*cb_func) (void *, MDP_STATUS); /* user callback function pointer  */
  void * arg;                           /* user data pointer */
} MDP_CB;

/*===========================================================================
    MDP DRIVER API FUNCTIONS PROTOTYPES
============================================================================*/

uint32 mdp_register_ovimg(MDPIMG *mdpImg);
uint32 mdp_replace_ovimg(MDPIMG *mdpImg, uint32 id);
void mdp_deregister_ovimg(uint32 id);
void mdp_deregister_all_ovimg(LCD_TYPE dest);
void mdp_deregister_all_ovimg_in_layer(LCD_TYPE dest, MDP_LAYER layer);
MDP_STATUS mdp_enable_ovimg(uint32 id, int32 flag);
void mdp_enable_all_ovimg(LCD_TYPE dest, int32 flag);
void mdp_enable_all_ovimg_in_layer(LCD_TYPE dest, int32 flag, MDP_LAYER layer);
int32 mdp_is_ovimg_valid(uint32 id);
void mdp_clear_ovimg(uint32 id);
void mdp_set_sys_bmp(uint32 *bmp);
int mdp_memcpy_img(MDPIMG *img, uint16 *dst_addr);

#ifndef FEATURE_MDP2
int mdp_memcpy_dblimg(MDPIMG *fgImg, MDPIMG *bgImg, uint16 *dst_addr);
#endif

// Sync API
MDP_STATUS mdp_disp_update(MDPIMG *fg_img);
MDP_STATUS mdp_update_ovimg(uint32 id);

// Async API
MDP_STATUS mdp_disp_update_async(MDPIMG * mdpImg, MDP_CB *cb);
MDP_STATUS mdp_update_ovimg_async(uint32 id, MDP_CB *cb);

#ifdef FEATURE_MDP2   
#error code not present
#endif

#ifdef FEATURE_DISPLAY_PROFILING
void mdp_profiling_start(void);
void mdp_profiling_stop(MDP_PROFILE_RESULT *result);
#endif /* FEATURE_DISPLAY_PROFILING */


/*================================================================================
   MDP API DOCUMENTATION
==================================================================================

FUNCITON NAME: mdp_register_ovimg()

DESCRIPTION:
	This function is to register an overlay image into the primary or secondary 
	display overlay list.
	
PARAMETERS:	
	MDPIMG *mdpImg
		New image parameters
	
	[-- MDPIMG structure description --]
			
  	LCD_TYPE dest
		Specifies the destination of overlay image.
		PRIMARY_LCD_TYPE or SECONDARY_LCD_TYPE. 
				
	MDPIMG_TYPE imgType
	   	MDP_RGB565 		- RGB565 format
	  	MDP_Y_CBCR_H2V2	- Y+CbCr pseudo planer format where Cb is in MSB
	  	MDP_Y_CRCB_H2V2	- Y+CbCr pseudo planer format where Cr is in MSB	  	
		
	uint32 *bmy_addr
		RGB bitmap pointer for MDP_RGB image type or video data Y pointer for 
		MDP_YCBCR image type.  
		
	uint32 *cbcr_addr
		Video data CbCr pointer for MDP_YCBCR image type or "don't care" for 
		MDP_RGB image type
		
	uint16 width
		Specifies the actual image source width
		
	MDP_CWIN cwin
		Specifies the rectangular region to be displayed
		
	int32 lcd_x
		Destination LCD column location
		
	int32 lcd_y
		Destination LCD row location
		
	uint32 mdpOp
		Specifies MDP image opertion 
			Common operation
				MDPOP_LR		
				MDPOP_UD			
				MDPOP_ROT90			
				MDPOP_ROT180		
				MDPOP_ROT270		
				MDPOP_ALPHAB		
				MDPOP_TRANSP		
				
			YCbCr operation		
				MDPOP_VDO_X2		
				MDPOP_VDO_1BY4		
				MDPOP_VDO_1BY2		
				MDPOP_VDO_3BY4					
			
	uint32 tpVal
		Specifies the background "transparency" color
		Only valid if MDPOP_TRANSP is set in mdp operation field
		
	uint32 alpha
		Specifies alpha percentage of this image
		0x0 is equal to 0% and 0x100 is equal to 100%
		Only valid if MDPOP_ALPHAB is set in mdp operation field
		
	MDP_LAYER layer		
		Specifies which layer this image belongs to
		foreground(MDP_FGL) or background(MDP_BGL)

EXAMPLE:

	MDPIMG mdpImg;
	
	mdpImg.dest = PRIMARY_LCD_TYPE;		
	mdpImg.imgType = MDP_RGB565;
	mdpImg.bmy_addr = (uint32 *)my_bitmap_image;
	mdpImg.width = 60;
	mdpImg.cwin.x1 = 0;
	mdpImg.cwin.x2 = 59;
	mdpImg.cwin.y1 = 0;
	mdpImg.cwin.y2 = 65;
	mdpImg.lcd_x = 100;
	mdpImg.lcd_y = 100;
	mdpImg.mdpOp = MDPOP_ALPHAB | MDPOP_TRANSP | MDPOP_ROT90;
	mdpImg.tpVal = 0xffff;
	mdpImg.alpha = 0x7f;
	mdpImg.layer = MDP_FGL;

	//registering 60x66 my_bimtmap_image.
	mdp_register_ovimg(&mdpImg);

RETURN VALUE:
	Returns the registration ID if successful, otherwise returns NULL
==================================================================================
==================================================================================
FUNCITON NAME: mdp_replace_ovimg()

DESCRIPTION
	This function is to replace the specified overlay image with the new image.

PARAMETERS:	
	MDPIMG *mdpImg
		New image parameters
	
	uint32 id
		Previously returned registration ID or NULL
	
EXAMPLE:
	MDPIMG mdpImg;
	uint32 rId;
	
	mdpImg.dest = PRIMARY_LCD_TYPE;		
	mdpImg.imgType = MDP_RGB565;
	mdpImg.bmy_addr = (uint32 *)my_bitmap_image;
	mdpImg.width = 60;
	mdpImg.cwin.x1 = 0;
	mdpImg.cwin.x2 = 59;
	mdpImg.cwin.y1 = 0;
	mdpImg.cwin.y2 = 65;
	mdpImg.lcd_x = 100;
	mdpImg.lcd_y = 100;
	mdpImg.mdpOp = MDPOP_ALPHAB | MDPOP_TRANSP | MDPOP_ROT90;
	mdpImg.tpVal = 0xffff;
	mdpImg.alpha = 0x7f;
	mdpImg.layer = MDP_FGL;

	//registering 60x66 my_bimtmap_image.
	rId = mdp_register_ovimg(&mdpImg);
	
	mdpImg.lcd_x = 100;
	mdpImg.lcd_y = 200;

	//move 60x66 my_bimtmap_image into new location (100,200)
	rId = mdp_replace_ovimg(&mdpImg, rId);

RETURN VALUE:
	Returns the given registration ID if successful, otherwise returns NULL
==================================================================================
==================================================================================
FUNCITON NAME: mdp_deregister_ovimg()

DESCRIPTION
	This function is to remove the specified overlay image

PARAMETERS:	
	uint32 id
		Previously returned registration ID or NULL
	
EXAMPLE:
	MDPIMG mdpImg;
	uint32 rId;
	
	mdpImg.dest = PRIMARY_LCD_TYPE;		
	mdpImg.imgType = MDP_RGB565;
	mdpImg.bmy_addr = (uint32 *)my_bitmap_image;
	mdpImg.width = 60;
	mdpImg.cwin.x1 = 0;
	mdpImg.cwin.x2 = 59;
	mdpImg.cwin.y1 = 0;
	mdpImg.cwin.y2 = 65;
	mdpImg.lcd_x = 100;
	mdpImg.lcd_y = 100;
	mdpImg.mdpOp = MDPOP_ALPHAB | MDPOP_TRANSP | MDPOP_ROT90;
	mdpImg.tpVal = 0xffff;
	mdpImg.alpha = 0x7f;
	mdpImg.layer = MDP_FGL;

	//registering 60x66 my_bimtmap_image.
	rId = mdp_register_ovimg(&mdpImg);

	//remove my_bimtmap_image
	rId = mdp_deregister_ovimg(rId);

RETURN VALUE:
	None
==================================================================================
==================================================================================
FUNCITON NAME: mdp_deregister_all_ovimg()

DESCRIPTION
	This function is to remove all overlay images for all layers

PARAMETERS:	
	LCD_TYPE dest
		Specifies the destination of the overlay image list to purge
		PRIMARY_LCD_TYPE or SECONDARY_LCD_TYPE. 

	
EXAMPLE:

	mdp_deregister_all_ovimg(PRIMARY_LCD_TYPE);

RETURN VALUE:
	None
==================================================================================
==================================================================================
FUNCITON NAME: mdp_deregister_all_ovimg_in_layer()

DESCRIPTION
	This function is to remove all overlay images in the specified layer

PARAMETERS:	
	LCD_TYPE dest
		Specifies the destination of the overlay image list to purge
		PRIMARY_LCD_TYPE or SECONDARY_LCD_TYPE.
		
	MDP_LAYER layer
		Foreground Layer(MDP_FGL) or Background Layer(MDP_BGL)
	
EXAMPLE:

	mdp_deregister_all_ovimg(PRIMARY_LCD_TYPE);

RETURN VALUE:
	None
	
==================================================================================
==================================================================================
FUNCITON NAME: mdp_disp_update()

DESCRIPTION
	This function is to display the specified image on LCD screen.
	Any overlapped image in the overlay list also gets displayed.		

PARAMETERS:	
	Same as mdp_register_ovimg()
				
EXAMPLE:
	MDPIMG mdpImg;
	uint32 rId;
	
	mdpImg.dest = PRIMARY_LCD_TYPE;		
	mdpImg.imgType = MDP_Y_CBCR_H2V2;
	mdpImg.bmy_addr = (uint32 *)Y_data;
	mdpImg.cbcr_addr = (uint32 *)CbCr_data;	
	mdpImg.width = 60;
	mdpImg.cwin.x1 = 0;
	mdpImg.cwin.x2 = 59;
	mdpImg.cwin.y1 = 0;
	mdpImg.cwin.y2 = 65;
	mdpImg.lcd_x = 100;
	mdpImg.lcd_y = 100;
	mdpImg.mdpOp = MDPOP_ROT180;
	mdpImg.layer = MDP_BGL;

	//displaying 60x66 YCbCr image at location (100,100) with 180 rotation
	mdp_disp_update(&mdpImg);

RETURN VALUE:
	Returns MDP_SUCCESSFUL if successful, otherwise returns MDP_FAIL
==================================================================================
==================================================================================
FUNCITON NAME: mdp_enable_ovimg()

DESCRIPTION
	This function is to enable/disable the specified overlay image.
	At the default, the overlay image is enabled in mdp_register_ovimg().

PARAMETERS:	
	uint32 id
		Previously returned registration ID or NULL
		
	int32 flag
		TRUE or FALSE
		
	
EXAMPLE:
	MDPIMG mdpImg;
	uint32 rId;
	
	mdpImg.dest = PRIMARY_LCD_TYPE;		
	mdpImg.imgType = MDP_RGB565;
	mdpImg.bmy_addr = (uint32 *)my_bitmap_image;
	mdpImg.width = 60;
	mdpImg.cwin.x1 = 0;
	mdpImg.cwin.x2 = 59;
	mdpImg.cwin.y1 = 0;
	mdpImg.cwin.y2 = 65;
	mdpImg.lcd_x = 100;
	mdpImg.lcd_y = 100;
	mdpImg.mdpOp = MDPOP_ALPHAB | MDPOP_TRANSP | MDPOP_ROT90;
	mdpImg.tpVal = 0xffff;
	mdpImg.alpha = 0x7f;

	//registering 60x66 my_bimtmap_image (default = enabled)
	rId = mdp_register_ovimg(&mdpImg);

	//disabling my_bimtmap_image overlay image
	mdp_ctrl_ovimg(rId, FALSE);

RETURN VALUE:
	Returns MDP_SUCCESSFUL if successful, otherwise returns MDP_FAIL
==================================================================================
==================================================================================
FUNCITON NAME: mdp_enable_all_ovimg()

DESCRIPTION
	This function is to enable/disable all overlay images in the list

PARAMETERS:	
	LCD_TYPE dest
		Specifies the destination of overlay image list
		PRIMARY_LCD_TYPE or SECONDARY_LCD_TYPE. 
		
	int32 flag
		TRUE or FALSE
	
EXAMPLE:

RETURN VALUE:
	None
==================================================================================
==================================================================================
FUNCITON NAME: mdp_enable_all_ovimg_in_layer()

DESCRIPTION
	This function is to enable/disable all overlay images in the specified layer

PARAMETERS:	
	LCD_TYPE dest
		Specifies the destination of the overlay image list to purge
		PRIMARY_LCD_TYPE or SECONDARY_LCD_TYPE.
		
 	int32 flag
 		TRUE or FALSE
 		
	MDP_LAYER layer
		Foreground Layer(MDP_FGL) or Background Layer(MDP_BGL)
	
EXAMPLE:

	mdp_deregister_all_ovimg(PRIMARY_LCD_TYPE);

RETURN VALUE:
	None
		
==================================================================================
==================================================================================
FUNCITON NAME: mdp_is_ovimg_valid()

DESCRIPTION
	This function is to verify the given ID points to valid registration.

PARAMETERS:	
	uint32 id
		Previously returned registration ID or NULL
			
EXAMPLE:

RETURN VALUE:
	Returns TRUE if valid, otherwise returns FALSE
==================================================================================
==================================================================================
FUNCITON NAME: mdp_clear_ovimg()

DESCRIPTION
	This function is to clear the registered foreground or background 
	layer image. 

PARAMETERS:	
	uint32 id
		Previously returned registration ID or NULL

EXAMPLE:	

RETURN VALUE:
	None
==================================================================================
==================================================================================
FUNCITON NAME: mdp_update_ovimg()

DESCRIPTION
	This function is to redraw the overlay image

PARAMETERS:	
	uint32 id
		Previously returned registration ID or NULL
			
EXAMPLE:
		
RETURN VALUE:
	None
==================================================================================
==================================================================================
FUNCITON NAME: mdp_set_sys_bmp()

DESCRIPTION
	This function is to set the system bitmap pointer.
	Appliction can pass IDisplay Bitmap Pointer and activate the MDP system rule.
	MDP system rule is basically to allow displaying only registered regions of 
	IDisplay_Update() bitmap buffer.  Any non-registered region of this bitmap
	will be discarded.

PARAMETERS:	
	uint32 *bmp
		Bitmap Pointer
			
EXAMPLE:
		
RETURN VALUE:
	None	
==================================================================================
==================================================================================
FUNCITON NAME: mdp_memcpy_img()

DESCRIPTION
	This function is to copy MDP image from source memory to destination memory.
    Source image can be RGB or YCbCr and the destination image is always RGB.
    It only supports a single image(RGB) or frame(YCbCr) copy.  Therefore, 
    transparent or AlphaBlend operation is not supported through this interface.

PARAMETERS:	
	MDPIMG *mdpImg
		MDP image parameter
    
    uint32 *dst_addr
        Destination memory address
					
EXAMPLE:
		
RETURN VALUE:
    Returns MDP_SUCCESSFUL if successful, otherwise returns MDP_FAIL
==================================================================================
==================================================================================
FUNCITON NAME: mdp_memcpy_dblimg()

DESCRIPTION
	This function is to copy two overlapped MDP images from source memory to 
	destination memory.  Source image can be RGB or YCbCr and the destination image 
	is always RGB.  It supports two images copy, RGB over YCbCr or RGB over RGB.

PARAMETERS:	
	MDPIMG *fgImg
		Foreground MDP image
    
   	MDPIMG *bgImg
		Background MDP image

    uint32 *dst_addr
        Destination memory address
					
EXAMPLE:
		
RETURN VALUE:
    Returns MDP_SUCCESSFUL if successful, otherwise returns MDP_FAIL
    
==================================================================================
==================================================================================
FUNCITON NAME: mdp_refresh_viewport()

DESCRIPTION
  This function is to refresh the region of diplay for registered/enabled overlays 
  into the destined display area

PARAMETERS:	
    LCD_TYPE dest
        Logical display destination
					
    MDP_CWIN *cwinPtr
        Display region defined by MDP CropWindow parameter
        Or the entire display region if cwinPtr is NULL
					
EXAMPLE:
		
RETURN VALUE:
    Returns MDP_SUCCESSFUL if successful, otherwise returns MDP_FAIL        

==================================================================================
==================================================================================
FUNCITON NAME: mdp_refresh_viewport_async()

DESCRIPTION
  This function is to refresh the region of diplay for registered/enabled overlays 
  into the destined display area

PARAMETERS:	
    LCD_TYPE dest
        Logical display destination
					
    MDP_CWIN *cwinPtr
        Display region defined by MDP CropWindow parameter
        Or the entire display region if cwinPtr is NULL

    MDP_CB *cb					
        callback info
        
EXAMPLE:
		
RETURN VALUE:
    Returns MDP_SUCCESSFUL if successful, otherwise returns MDP_FAIL        

==================================================================================
==================================================================================
FUNCTION mdp_profiling_start()

DESCRIPTION
  This function starts MDP profiling.

PARAMETERS:
	None

EXAMPLE:
    MDP_PROFILE_RESULT result;

    mdp_profiling_start();
    mdpImg.mdpOp |= MDPOP_PROFILING;
  	mdp_disp_update(&mdpImg);
    mdp_profiling_stop(&result);

RETURN VALUE:
	None
==================================================================================
==================================================================================
FUNCTION mdp_profiling_stop()

DESCRIPTION
  This function stops MDP profiling & return profiling result to caller.

PARAMETERS:
	MDP_PROFILE_RESULT *result
		[out] Pointer to profiling result
	
	[-- MDP_PROFILE_RESULT structure description --]
			
  uint32 num_sample   
    total number of samples profiled 

  boolean vsync_enabled          
    whether vsync is enabled or not 

  uint32 num_blending[MDP_MAX_LAYER]  
    number of blending times              

  uint32 avg_blending[MDP_MAX_LAYER]  
    average blending time for each layer  

  uint32 min_blending[MDP_MAX_LAYER]
    minimum blending time for each layer  

  uint32 max_blending[MDP_MAX_LAYER]  
    maximum blending time for each layer  

  uint32 num_mapping[MAX_LCD_NUM]  
     number of mapping times             

  uint32 avg_mapping[MAX_LCD_NUM]   
    average mapping time for each lcd   

  uint32 min_mapping[MAX_LCD_NUM] 
    minimum mapping time for each lcd   

  uint32 max_mapping[MAX_LCD_NUM]  
    maximum mapping time for each lcd   

  uint32 num_update[MAX_LCD_NUM]  
     number of update times             

  uint32 avg_update[MAX_LCD_NUM]   
    average update time for each lcd   

  uint32 min_update[MAX_LCD_NUM] 
    minimum update time for each lcd   

  uint32 max_update[MAX_LCD_NUM]  
    maximum update time for each lcd   

  uint32 avg_interframe  
    average interframe time          

  uint32 min_interframe  
    minimum interframe time          

  uint32 max_interframe    
    maximum interframe time         

EXAMPLE:

RETURN VALUE
  None
==================================================================================
================================================================================*/
#endif /* MDP_API_H */

