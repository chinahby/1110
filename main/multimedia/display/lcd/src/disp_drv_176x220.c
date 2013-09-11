/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   L C D    D I S P L A Y    D R I V E R

GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS
  disp_drv_init

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                         INCLUDE FILES FOR MODULE

===========================================================================*/
#include "disp_drv.h"

/*============================================================================

                 CONSTANT AND MACRO DEFINITIONS FOR MODULE

============================================================================*/

#define DISP_DRV_WIDTH             176
#define DISP_DRV_HEIGHT            220

/*============================================================================

                        STRUCTURE TYPES FOR MODULE

============================================================================*/

extern boolean disp_ili9225b_tft20(disp_drv_ic_type *pdispic);
extern boolean disp_nt35601_tft20(disp_drv_ic_type *pdispic);
extern boolean disp_tm7775r_tft20(disp_drv_ic_type *pdispic);
extern boolean disp_ili9225b_tft20_rot(disp_drv_ic_type *pdispic);
extern boolean disp_ili9225g_tft20_rot(disp_drv_ic_type *pdispic);
extern boolean disp_rm68130_tft20(disp_drv_ic_type *pdispic);
extern boolean disp_nv3038_tft20(disp_drv_ic_type *pdispic);

/*============================================================================

                           VARIABLE DECLARATION

============================================================================*/

static disp_ic_install_type     disp_ic_install[] = 
{
    disp_nv3038_tft20,	
    disp_nt35601_tft20,
    disp_tm7775r_tft20,
    disp_rm68130_tft20,
    disp_ili9225g_tft20_rot,

    //disp_ili9225b_tft20,//默认驱动，需放到最后，并且不检查IC的ID
    NULL
};

#include "disp_drv_generic.h"
