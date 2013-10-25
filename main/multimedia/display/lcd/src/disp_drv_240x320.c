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

#define DISP_DRV_WIDTH             240
#define DISP_DRV_HEIGHT            320

/*============================================================================

                        STRUCTURE TYPES FOR MODULE

============================================================================*/

extern boolean disp_ili9325d_tft26_rot(disp_drv_ic_type *pdispic);
extern boolean disp_ili9341_tft23_rot(disp_drv_ic_type *pdispic);
extern boolean disp_rm68090_tft24_rot(disp_drv_ic_type *pdispic);
extern boolean disp_st7789s_tft24_rot(disp_drv_ic_type *pdispic);

/*============================================================================

                           VARIABLE DECLARATION

============================================================================*/

static disp_ic_install_type     disp_ic_install[] = 
{
    //disp_ili9163c_tft177hsd_rot,//默认驱动，需放到最后，并且不检查IC的ID
    //disp_ili9341_tft23_rot,
   // 
    disp_rm68090_tft24_rot,
    disp_ili9341_tft23_rot,
    disp_st7789s_tft24_rot,
    disp_ili9325d_tft26_rot,
    NULL
};

#include "disp_drv_generic.h"
