/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

  MDP Device Driver Layer Processor

  GENERAL DESCRIPTION
  This module contains functions and data blocks pertaining to the
  Mobile Display Processor (MDP) Driver Layer Processor

  EXTERNALIZED FUNCTIONS

  INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

  Copyright (c) 2005 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*============================================================================

  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/multimedia/display/mdp1/mdp_api/main/latest/src/mdp_layer_proc.c#2 $ $DateTime: 2008/06/20 10:19:05 $ $Author: davidwon $

when      who     what, where, why
--------  -----   ----------------------------------------------------------
06/20/08  dw      Fixed bug to prevent erroneous disabling of vsync 
05/16/05  cjr     7500 MDP integration
03/08/05  cjr     Adjusted MDP system bitmap with BREW offset
02/10/05  cjr     Completed source file re-architect
02/09/05  cjr     Initial code
============================================================================ */

/*============================================================================
INCLUDE FILES FOR MODULE
============================================================================*/
#include <math.h>
#include "customer.h"

#if defined (FEATURE_MDP) || defined (FEATURE_MDP2)
  #include "mdp_drv.h"
  #include "mdp_api_proc.h"    
  #include "mdp_layer_proc.h"

/* rex task associated data */
extern rex_crit_sect_type   mdp_crit_sect;
extern MDPIMG_LIST * mdpImgTailPtr_1[];
extern MDPIMG_LIST * mdpImgTailPtr_2[];

#ifdef FEATURE_MDP2
#error code not present
#endif

extern int32 mdpLcdSize[MAX_LCD_NUM][2];
extern uint32 * mdp_sys_bmp;
/*===========================================================================

FUNCTION mdp_adjust_XY()

DESCRIPTION
  This function is to calculate and adjust the original sub-region before
  any MDP operation such as MDPOP_ROT90, MDPOP_LR, MDPOP_UD

DEPENDENCIES
  mdp_drv_init() is called to setup MDP HW block and initialize the
  associated driver structures and interrupt controllers.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mdp_adjust_XY(MDPIMG *mdpImg,
                   int32 *sx1, 
                   int32 *sx2, 
                   int32 *sy1, 
                   int32 *sy2, 
                   MDPSCALE_INFO *scale)
{
  MDP_CWIN cwin;
  int32 lcd_x, lcd_y;
  int32 temp;
  boolean rotFlag, lrFlag, udFlag;
  
  rotFlag = (mdpImg->mdpOp&MDPOP_ROT90) ? TRUE : FALSE;
  lrFlag  = (mdpImg->mdpOp&MDPOP_LR)    ? TRUE : FALSE;
  udFlag  = (mdpImg->mdpOp&MDPOP_UD)    ? TRUE : FALSE;  
  
  lcd_x = mdpImg->lcd_x + MDP_SYS_OFFSET_X;
  lcd_y = mdpImg->lcd_y + MDP_SYS_OFFSET_Y;

  if (scale->scale_flag)
  {
    mdp_vdo_scale_cal(sx1,sx2,sy1,sy2,
#ifdef FEATURE_MDP2
#error code not present
#else                                            
                      (uint16)scale->num,scale->den,
                      (uint16)scale->num,scale->den,                      
#endif                                            
                      lcd_x, lcd_y);
  }
    
  cwin.x1 = *sx1;
  cwin.x2 = *sx2;
  cwin.y1 = *sy1;
  cwin.y2 = *sy2;
    
  if(rotFlag)
  {
    cwin.x1 = mdpImg->cwin.x1 + *sy1 - lcd_y;
    cwin.x2 = mdpImg->cwin.x1 + *sy2 - lcd_y;
    cwin.y1 = mdpImg->cwin.y2 + lcd_x - *sx2;  
    cwin.y2 = mdpImg->cwin.y2 + lcd_x - *sx1;           
  }
  else
  {
    cwin.x1 = mdpImg->cwin.x1 + *sx1 - lcd_x;
    cwin.x2 = mdpImg->cwin.x1 + *sx2 - lcd_x;
    cwin.y1 = mdpImg->cwin.y1 + *sy1 - lcd_y;
    cwin.y2 = mdpImg->cwin.y1 + *sy2 - lcd_y;
  }
  
  if ((lrFlag&&(!rotFlag)) || (udFlag&&(rotFlag)))
  {
    temp = cwin.x1;
    cwin.x1 = mdpImg->cwin.x2 - (cwin.x2 - mdpImg->cwin.x1);
    cwin.x2 = mdpImg->cwin.x2 - (temp - mdpImg->cwin.x1);
  }

  if ((udFlag&&(!rotFlag)) || (lrFlag&&(rotFlag)))
  {    
    temp = cwin.y1;    
    cwin.y1 = mdpImg->cwin.y2 - (cwin.y2 - mdpImg->cwin.y1);
    cwin.y2 = mdpImg->cwin.y2 - (temp - mdpImg->cwin.y1);
  }

  *sx1 = cwin.x1;
  *sx2 = cwin.x2;
  *sy1 = cwin.y1;
  *sy2 = cwin.y2;          
}

/*===========================================================================

FUNCTION mdpCalSubWin()

DESCRIPTION
  This function is to divide the sub-region between overlay rectangular
  and the background image

DEPENDENCIES
  mdp_drv_init() is called to setup MDP HW block and initialize the
  associated driver structures and interrupt controllers.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mdpCalSubWin(MDP_CWIN *thisXY, MDP_CWIN *ovXY, MDP_SUBWIN *sub)
{
  int sel, i;

/*===========================================================================
  ___________________                              ___________________
 |                   |                            |                   | 
 |                   |                            | SubRegion(SR) 1   | 
 |     _________     |                            |___________________|
 |    |         |    |                            |    |         |    |
 |    |   FG    |    |                            |    |         |    |
 |    |         |    |                            | SR |   SR    | SR |
 |    |   Img   |    |      =======>>>            |  2 |    3    |  4 |
 |    |         |    |                            |    |         |    |
 |    |         |    |    After the division,     |    |         |    |
 |    |         |    |                            |    |         |    |
 |     ---------     |                            |-------------------|
 |        BG         |                            |        SR         | 
 |                   |                            |         5         | 
 |        Img        |                            |                   | 
  -------------------                              -------------------

===========================================================================*/

  sel = BIT(0)|BIT(1)|BIT(2)|BIT(3);

  if(thisXY->x1 == ovXY->x1)
    sel &= ~BIT(0);
  if(thisXY->y1 == ovXY->y1)
    sel &= ~BIT(1);
  if(thisXY->x2 == ovXY->x2)
    sel &= ~BIT(2);
  if(thisXY->y2 == ovXY->y2)
    sel &= ~BIT(3);

  i = 0;
  if(sel != 0)
  {
    if(sel & BIT(0))
    {
      sub->cwin[i].x1 = thisXY->x1;
      sub->cwin[i].x2 = ovXY->x1-1;
      sub->cwin[i].y1 = ovXY->y1;
      sub->cwin[i].y2 = ovXY->y2;

      sub->lcdX[i] = (uint16)sub->cwin[i].x1;
      sub->lcdY[i] = (uint16)sub->cwin[i].y1;

      i++;
    }

    if(sel & BIT(1))
    {
      sub->cwin[i].x1 = thisXY->x1;
      sub->cwin[i].x2 = thisXY->x2;
      sub->cwin[i].y1 = thisXY->y1;
      sub->cwin[i].y2 = ovXY->y1-1;

      sub->lcdX[i] = (uint16)sub->cwin[i].x1;
      sub->lcdY[i] = (uint16)sub->cwin[i].y1;

      i++;
    }

    if(sel & BIT(2))
    {
      sub->cwin[i].x1 = ovXY->x2+1;
      sub->cwin[i].x2 = thisXY->x2;
      sub->cwin[i].y1 = ovXY->y1;
      sub->cwin[i].y2 = ovXY->y2;

      sub->lcdX[i] = (uint16)sub->cwin[i].x1;
      sub->lcdY[i] = (uint16)sub->cwin[i].y1;

      i++;
    }

    if(sel & BIT(3))
    {
      sub->cwin[i].x1 = thisXY->x1;
      sub->cwin[i].x2 = thisXY->x2;
      sub->cwin[i].y1 = ovXY->y2+1;
      sub->cwin[i].y2 = thisXY->y2;

      sub->lcdX[i] = (uint16)sub->cwin[i].x1;
      sub->lcdY[i] = (uint16)sub->cwin[i].y1;

      i++;
    }
  }
  sub->win = i;

}

/*===========================================================================

FUNCTION mdp_vdo_scale_cal()

DESCRIPTION
  This function is to resize the video window according to the scale factor

DEPENDENCIES
  mdp_drv_init() is called to setup MDP HW block and initialize the
  associated driver structures and interrupt controllers.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mdp_vdo_scale_cal(int32 *x1, int32 *x2, int32 *y1, int32 *y2, 
                       uint16 x_num, uint16 x_den,                       
                       uint16 y_num, uint16 y_den,
                       int32 lcd_x, int32 lcd_y)
{
  int32 temp;

  *x1 -= lcd_x;
  *x2 -= lcd_x;
  *y1 -= lcd_y;
  *y2 -= lcd_y;  

  temp = *x1;
  *x1 = (temp)*x_den/x_num;
  // x1 + length*rev_scale + round_off
  *x2 = *x1 + ((*x2-temp+1)*x_den)/x_num-1 + (((*x2-temp+1)*x_den)%x_num?1:0);
  
  temp = *y1;
  *y1 = (temp)*y_den/y_num;
  // y1 + length*rev_scale + round_off  
  *y2 = *y1 + ((*y2-temp+1)*y_den)/y_num-1 + (((*y2-temp+1)*y_den)%y_num?1:0);
  
  *x1 += lcd_x;
  *x2 += lcd_x;
  *y1 += lcd_y;
  *y2 += lcd_y;  
}

/*===========================================================================

FUNCTION mdpCalOverXYInt()

DESCRIPTION
  This function is to calculate overlay window between two images

DEPENDENCIES
  mdp_drv_init() is called to setup MDP HW block and initialize the
  associated driver structures and interrupt controllers.

RETURN VALUE
  MDP_SUCCESSFUL if there is an overlap otherwise MDP_FAIL

SIDE EFFECTS
  None

===========================================================================*/
int mdpCalOverXYInt(int32 fg_x1,
                    int32 fg_x2,
                    int32 fg_y1,
                    int32 fg_y2,
                    int32 bg_x1,
                    int32 bg_x2,
                    int32 bg_y1,
                    int32 bg_y2,
                    MDP_CWIN *overXY)
{
  if((bg_x1 > fg_x2) || (fg_x1 > bg_x2))
  {
    return MDP_FAIL;
  }

  if((bg_y1 > fg_y2) || (fg_y1 > bg_y2))
  {
    return MDP_FAIL;
  }



  if(bg_x1 > fg_x1)
  {
    overXY->x1 = bg_x1;
  }
  else
  {
    overXY->x1 = fg_x1;
  }

  if(bg_x2 > fg_x2)
  {
    overXY->x2 = fg_x2;
  }
  else
  {
    overXY->x2 = bg_x2;
  }


  if(bg_y1 > fg_y1)
  {
    overXY->y1 = bg_y1;
  }
  else
  {
    overXY->y1 = fg_y1;
  }

  if(bg_y2 > fg_y2)
  {
    overXY->y2 = fg_y2;
  }
  else
  {
    overXY->y2 = bg_y2;
  }

  // now overXY has the overlapped region of two images...
  
  return MDP_SUCCESSFUL;
}
  
/*===========================================================================

FUNCTION mdp_get_subwin_gfx()

DESCRIPTION
  This function is to calculate overlay and disjoined window

DEPENDENCIES
  mdp_drv_init() is called to setup MDP HW block and initialize the
  associated driver structures and interrupt controllers.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mdp_get_subwin_gfx(LCD_TYPE destDev,
                        MDPIMG_LIST *fgImg,
                        MDPIMG_LIST *bgImg,
                        MDP_SUBWIN *fgSub,
                        MDP_SUBWIN *ovSub)
{
  MDP_CWIN overXY;

  fgSub->win = 0;
  ovSub->win = 0;

  /* lcd overlap coordinate calculation*/
  if(mdpCalOverXYInt(fgImg->img.cwin.x1,
                     fgImg->img.cwin.x2,
                     fgImg->img.cwin.y1,
                     fgImg->img.cwin.y2,
                     bgImg->img.cwin.x1,
                     bgImg->img.cwin.x2,
                     bgImg->img.cwin.y1,
                     bgImg->img.cwin.y2,
                     &overXY) == MDP_SUCCESSFUL)
  {
    mdpCalSubWin(&fgImg->img.cwin, &overXY, fgSub);
    ovSub->win = 1;
    ovSub->cwin[0] = overXY;
  }
  else
  {
    fgSub->lcdX[0] = (uint16)fgImg->img.cwin.x1;
    fgSub->lcdY[0] = (uint16)fgImg->img.cwin.y1;
    fgSub->win = 1;
    fgSub->cwin[0] = fgImg->img.cwin;

    ovSub->win = 0;
  }
}

/*===========================================================================

FUNCTION mdpPeekOverXYInt()

DESCRIPTION
  This function is to determine if there is an overlap between two images.

DEPENDENCIES
  mdp_drv_init() is called to setup MDP HW block and initialize the
  associated driver structures and interrupt controllers.

RETURN VALUE
  MDP_SUCCESSFUL if there is an overlap otherwise MDP_FAIL

SIDE EFFECTS
  None

===========================================================================*/
uint32 mdpPeekOverXYInt(int32 fg_x1,
                        int32 fg_x2,
                        int32 fg_y1,
                        int32 fg_y2,
                        int32 bg_x1,
                        int32 bg_x2,
                        int32 bg_y1,
                        int32 bg_y2)
{
  if((bg_x1 > fg_x2) || (fg_x1 > bg_x2))
  {
    return MDP_FAIL;
  }

  if((bg_y1 > fg_y2) || (fg_y1 > bg_y2))
  {
    return MDP_FAIL;
  }

  return MDP_SUCCESSFUL;
}

/*===========================================================================

FUNCTION mdp_sort_imglist()

DESCRIPTION
  This function is to sort node list from the top image to the bottom image

DEPENDENCIES
  mdp_drv_init() is called to setup MDP HW block and initialize the
  associated driver structures and interrupt controllers.

RETURN VALUE
  MDP_SUCCESSFUL if there is an overlap otherwise MDP_FAIL

SIDE EFFECTS
  None

===========================================================================*/
MDPIMG_LIST * mdp_sort_imglist(MDPIMG_LIST * node)
{
  MDPIMG_LIST * head, * curNode, *this_node;

  if(node == NULL)
    return NULL;

  head = node;
  node = node->next;
  head->next = NULL;

  while(node != NULL)
  {
    this_node = node;
    node = node->next;

    curNode = head;
    while(curNode != NULL)
    {
      /* cwin.y1 points to the actual LCD location*/
      if(this_node->img.cwin.y1 <= curNode->img.cwin.y1)
      {
        mdp_imglist_insert_front(curNode, this_node);

        if(curNode == head)
        {
          head = this_node;
        }
        curNode = NULL;
      }
      else
      {
        if(curNode->next == NULL)
        {
          mdp_imglist_insert_back(curNode, this_node);
          curNode = NULL;
        }
        else
        {
          curNode = curNode->next;
        }
      }
    }
  }

  return head;
}

/*===========================================================================

FUNCTION mdp_cal_ovimg_rec()

DESCRIPTION
  This function is to calculate the overlapped rectangular regions from the
  same or opposite layer

DEPENDENCIES
  mdp_drv_init() is called to setup MDP HW block and initialize the
  associated driver structures and interrupt controllers.

RETURN VALUE
  MDPIMG_LIST head pointer if successful otherwise MDP_FAIL

SIDE EFFECTS
  None

===========================================================================*/
uint32 mdp_cal_ovimg_rec( MDPIMG *mdpImg,
                          uint32 sameLayer,
                          uint32 * fgSubFlag,
                          MDPIMG * orig_fgImg,
                          MDPSCALE_INFO * orig_scale,
                          uint32 * orig_fgMbcOp,
                          uint32 * alphaMask,
                          uint32 * vsync_flag,
                          MDP_CWIN * this_cwin,
                          uint32 sysBmpMode)
{
  LCD_TYPE destDev;
  MDPIMG_LIST *node, *bgListTail, *fgListNode,
  *this_fgList, *fgListHead, *next_fgList;

  MDP_SUBWIN fgSub, ovSub;
  int32 i;
  MDP_LAYER layer;

  if(mdpImg == NULL)
    return MDP_FAIL;

  if(mdp_verify_img(mdpImg) != MDP_SUCCESSFUL)
    return MDP_FAIL;

  if((node = mdp_imglist_get()) == NULL)
    return MDP_FAIL;

  destDev = mdpImg->dest;
  node->img = *mdpImg;
  node->bgPtr = NULL;
  node->img.lcd_x += MDP_SYS_OFFSET_X;
  node->img.lcd_y += MDP_SYS_OFFSET_Y;

  fgListHead = node;

  layer = fgListHead->img.layer;    

  switch (fgListHead->img.dest)
  {
    case DISPLAY_TYPE_1:
    if(sameLayer == TRUE)
      bgListTail = mdpImgTailPtr_1[layer];
    else
      bgListTail = mdpImgTailPtr_1[2-layer]; //opposite layer
      break;

    case DISPLAY_TYPE_2:
    if(sameLayer == TRUE)
      bgListTail = mdpImgTailPtr_2[layer];
    else
      bgListTail = mdpImgTailPtr_2[2-layer]; //opposite layer
      break;

#ifdef FEATURE_MDP2
#error code not present
#endif
      
    default:  
      return MDP_FAIL;
  }

  mdp_touch_img(node);

  if(mdp_verify_dispArea(&node->img) != MDP_SUCCESSFUL)
  {
    mdp_imglist_remove(node);
    return MDP_FAIL;
  }

  *orig_fgImg = fgListHead->img;
  *orig_scale = fgListHead->scaleInfo;
  *orig_fgMbcOp = fgListHead->mbcOp;

  if (mdpImg->mdpOp & MDPOP_VSYNC_OFF)
  {
    *vsync_flag = FALSE;
  }
  else
  {
    *vsync_flag = TRUE;
  }
  
  if(mdpImg->bmy_addr != NULL)
  {
    *fgSubFlag = TRUE;
  }
  else
  {
    orig_fgImg->imgType = MDP_RGB565;
    *fgSubFlag = FALSE;
  }

  if(mdpImg->mdpOp & MDPOP_DISABLE_ALPHAB)
  {
    *alphaMask = ~MDPOP_ALPHAB;
  }
  else
  {
    *alphaMask = 0xffffffff;
  }

  /* cropping LCD display region */
  if (mdpCalOverXYInt(fgListHead->img.cwin.x1,
                  fgListHead->img.cwin.x2,
                  fgListHead->img.cwin.y1,
                  fgListHead->img.cwin.y2,
                  0,
                  mdpLcdSize[destDev][COL_IDX],
                  0,
                  mdpLcdSize[destDev][ROW_IDX],
                  &fgListHead->img.cwin) != MDP_SUCCESSFUL)
  {                  
    mdp_imglist_purge(fgListHead);
    return MDP_FAIL;
  }              

  *this_cwin = fgListHead->img.cwin;

#ifndef FEATURE_MDP2  
  if((orig_fgImg->imgType == MDP_YCBCR420) && (orig_fgImg->layer == MDP_LAYER_2))
  {
    return(uint32 )fgListHead;
  }
#endif

  rex_enter_crit_sect(&mdp_crit_sect);

   
  while(bgListTail != NULL)
  {


    if (bgListTail->enabled == TRUE)
    {
      if ((sameLayer == TRUE) && (sysBmpMode))
      {
        // if we're looking at IDISPLAY_Update foreground image,
        // ovimg registration must come from BREW device bitmap pointer.
        // And we only apply this rule for the system bitmap belongs to UI
        if((uint32)orig_fgImg->bmy_addr != 
           ((uint32)bgListTail->img.bmy_addr-
            ((mdp_sys_bmp!=NULL)?MDP_SYS_BMP_OFFSET:0)))
        {
          bgListTail = bgListTail->prev;
          continue;
        }
      }

      next_fgList = fgListHead;
      while(next_fgList != NULL)
      {
        fgListNode = next_fgList;
        next_fgList = fgListNode->next;

        // if there is an overlap and not previously processed window
        if((fgListNode->bgPtr == NULL) &&
           (mdpPeekOverXYInt(fgListNode->img.cwin.x1,
                             fgListNode->img.cwin.x2,
                             fgListNode->img.cwin.y1,
                             fgListNode->img.cwin.y2,
                             bgListTail->img.cwin.x1,
                             bgListTail->img.cwin.x2,
                             bgListTail->img.cwin.y1,
                             bgListTail->img.cwin.y2
                            ) == MDP_SUCCESSFUL))
        {
        
          this_fgList = fgListNode;

          mdp_get_subwin_gfx(destDev, fgListNode, bgListTail,
                             &fgSub, &ovSub);

/*
          if(sameLayer == TRUE)
          {
            // if it's IDisplay_Update, we only can display the overlapped
            // registered area
            fgSub.win = 0;
          }
*/
          // non-overlapped windows
          for(i=0;i<fgSub.win; i++)
          {
            if((node = mdp_imglist_get()) == NULL)
            {
              mdp_imglist_purge(fgListHead);
              rex_leave_crit_sect(&mdp_crit_sect);
              return MDP_FAIL;
            }
            node->img.cwin = fgSub.cwin[i];
            node->bgPtr = NULL;
            mdp_imglist_insert_back(this_fgList, node);
          }

          // overlapped windows
          if(ovSub.win > 0)
          {
            if((node = mdp_imglist_get()) == NULL)
            {
              mdp_imglist_purge(fgListHead);
              rex_leave_crit_sect(&mdp_crit_sect);
              return MDP_FAIL;
            }
            node->img.cwin = ovSub.cwin[0];
            node->bgPtr = bgListTail;
            mdp_imglist_insert_back(this_fgList, node);
          }

          if(this_fgList == fgListHead)
            fgListHead = this_fgList->next;

          mdp_imglist_remove(this_fgList);
        }
      }
    }
    bgListTail = bgListTail->prev;
  }

  if(fgListHead != NULL)
  {
    // sorting from top to bottom
    fgListHead = mdp_sort_imglist(fgListHead);
  }
  rex_leave_crit_sect(&mdp_crit_sect);
  return(uint32 )fgListHead;
}

#endif /* (FEATURE_MDP) || (FEATURE_MDP2) */
