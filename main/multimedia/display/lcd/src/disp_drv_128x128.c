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

#define DISP_DRV_WIDTH             128
#define DISP_DRV_HEIGHT            128

/*============================================================================

                        STRUCTURE TYPES FOR MODULE

============================================================================*/

extern boolean disp_st7735r_tft144(disp_drv_ic_type *pdispic);

extern boolean disp_ili9163c_tft144(disp_drv_ic_type *pdispic);
extern boolean disp_st7735s_tft144(disp_drv_ic_type *pdispic);


/*============================================================================

                           VARIABLE DECLARATION

============================================================================*/

static disp_ic_install_type     disp_ic_install[] = 
{
#if 1
    disp_ili9163c_tft144,
#endif
	disp_st7735s_tft144,
    disp_st7735r_tft144,//默认驱动，需放到最后，并且不检查IC的ID
    NULL
};

#include "disp_drv_generic.h"
