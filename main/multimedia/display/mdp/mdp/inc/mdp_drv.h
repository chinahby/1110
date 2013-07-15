#ifndef MDP_DRV_H
#define MDP_DRV_H
/*============================================================================

                   MDP Device Driver Header File

  DESCRIPTION
  This file contains the definitions needed for the MDP device driver.

  Copyright (c) 2007-2008 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/multimedia/display/mdp1/mdp/main/latest/inc/mdp_drv.h#4 $ $DateTime: 2008/09/30 07:20:13 $ $Author: gauravs $

when      who    what, where, why
--------  -----  ----------------------------------------------------------
09/19/08  gs      compiler warning & Lint fixes
09/08/08  rk      Added macros for Cache handling on ULC and other targets.
03/13/08  dw      Add support for Epson QVGA panels
                  Add references to support MDP 1.4 hardware
11/07/07  dw      Add support for dynamic memory allocation
05/24/07  cl      MDP reset enhancement for 6k targets
04/06/07  dw      Add support for Epson Optrex QcifP EBI2 display panel.
03/30/07  trg     Add interface for qvga to wqvga scaling for 6k.  Use 
                  FEATURE_MDP_WSCALE to enable.
02/09/07  trg     Change prototypes to fix screen capture + scale issue.
08/03/06  trg     Add vsync info to mdp_start_script
07/17/06  dw      Remove FEATURE_DISP_TASK
05/16/06  trg     Add MDP QXDM messages
05/10/06  cjr     Added support for display API queue
02/22/06  trg     Add XRGB_8888 support for MDP 1
10/21/05  cjr     Added MDP1.1 support
06/20/05  gr      Added support for building with gcc
02/25/05  cjr     Added Gamma correction table 
02/10/05  cjr     Completed source file re-architect
08/17/04  cjr     added local function prototypes
05/28/04  cjr     Initial code
============================================================================*/

//============================================================================
//                        INCLUDE FILES
//============================================================================
#include "comdef.h"
#include "mdp_api.h"
#include "disp_task.h"
#include "disp.h"

#undef  MDP_DEBUG

/*  MDP Support for Secondary Display -> disable if no secondary display */
#ifndef FEATURE_SECONDARY_DISPLAY
  #ifdef FEATURE_MDP_SECONDARY_DISPLAY
#error code not present
  #endif
#endif

#define MDP_SCRIPT_ADDR     HWIO_MDP_SCRIPT_ADDR_ADDR
#define MDP_COMMAND         HWIO_MDP_COMMAND_ADDR
#define MDP_SCRIPT_STATUS   HWIO_MDP_SCRIPT_STATUS_ADDR
#define MDP_CGC_CONFIG      HWIO_MDP_CGC_CONFIG_ADDR
#define MDP_INTR_ENABLE     HWIO_MDP_INTR_ENABLE_ADDR
#define MDP_INTR_CLEAR      HWIO_MDP_INTR_CLEAR_ADDR
#define MDP_SYNC_CONFIG     HWIO_MDP_SYNC_CONFIG_ADDR
#define HCLK_DIV            HWIO_HCLK_DIV_ADDR
#define MDP_INTR_STATUS     HWIO_MDP_INTR_STATUS_ADDR

// MDP_TEST_MODE_REG is not defined in all targets.
// Therefore, we're using HWIO_MDP_COMMAND_ADDR and subtract 0x4
#define MDP_DBGBUS_REG_CFG           (HWIO_MDP_COMMAND_ADDR-0x04)
#define MDP_DBGBUS_TEST_MISR_RESET   (MDP_DBGBUS_REG_CFG+0x2c)
#define MDP_DBGBUS_TEST_EXPORT_MISR  (MDP_DBGBUS_REG_CFG+0x30)
#define MDP_DBGBUS_TEST_MISR_REG     (MDP_DBGBUS_REG_CFG+0x34)

#ifdef HWIO_MDP_DEBUG_BUS_VALUE_ADDR
#define MDP_DBGBUS_REG_VAL    (HWIO_MDP_DEBUG_BUS_VALUE_ADDR)
#else
#define MDP_DBGBUS_REG_VAL    (HWIO_MDP_ADDRESS_16_ADDR)
#endif

#define MDP_DBGBUS_SEL_REV    (0x2C0)
#define MDP_DBGBUS_MDDI_MISR  (0x300)
#define MDP_DBGBUS_EBI2_MISR  (0x182)
#define MDP_DBGBUS_SEL_MISR   (0x141)

#define MDP_BIT_REV_01_01  (0x31000000)
#define MDP_BIT_REV_01_02  (0x32000000)
#define MDP_BIT_REV_01_03  (0x33000000)
#define MDP_BIT_REV_01_04  (0x34000000) /* Actual chip rev ID: 0x34723032 */

#define MDP_BIT_REV_MASK   (0xff000000)
#define MDP_BIT_STATE_MASK (0x0000001f)

#ifdef FEATURE_MDDI
#error code not present
#endif

//#define WAIT_FOR_SCRIPT_DONE    while((inpdw(MDP_SCRIPT_STATUS) & 0x00000001))

#define MDP_GFX_LINK_POS 34
#define MDP_GOG_LINK_POS 40
#define MDP_VDO_LINK_POS 48
#define MDP_GOV_LINK_POS 55

#define MDP_GFX_SCR_SIZE 64
#define MDP_GOG_SCR_SIZE 64
#define MDP_VDO_SCR_SIZE 64
#define MDP_GOV_SCR_SIZE 64

#define MDP_EPSON_QCIF_SCR_SIZE \
  (MDP_GFX_SCR_SIZE+MDP_VDO_SCR_SIZE+MDP_GOG_SCR_SIZE+MDP_GOV_SCR_SIZE)

#define MDP_EPSON_QVGA_SCR_SIZE      MDP_EPSON_QCIF_SCR_SIZE
#define MDP_TMD_QVGA_SCR_SIZE        MDP_EPSON_QCIF_SCR_SIZE
#define MDP_MDDI_SCR_SIZE            MDP_EPSON_QCIF_SCR_SIZE
#define MDP_EP_OP_QCIFP_PRI_SCR_SIZE MDP_EPSON_QCIF_SCR_SIZE
#define MDP_EP_OP_QCIFP_SEC_SCR_SIZE MDP_EPSON_QCIF_SCR_SIZE
#ifdef CUST_EDITION
#define MDP_LCD_SCR_SIZE             MDP_EPSON_QCIF_SCR_SIZE
#endif

#define MDP_EPSON_QCIF_SCR_LEN 32
#define MDP_EPSON_QVGA_SCR_LEN 32
#define MDP_TMD_QVGA_SCR_LEN 32
#define MDP_MDDI_SCR_LEN 32
#define MDP_EP_OP_QCIFP_PRI_SCR_LEN 32
#define MDP_EP_OP_QCIFP_SEC_SCR_LEN 32
#ifdef CUST_EDITION
#define MDP_LCD_SCR_LEN 36
#endif

#define MDP_GFX_SCR_LEN 64
#define MDP_GOG_SCR_LEN 64
#define MDP_VDO_SCR_LEN 64
#define MDP_GOV_SCR_LEN 64

// Hard-coded values below due to HW being fixed for these values.
//   WARNING: DO NOT MODIFY COL/ROW SETTINGS FOR WSCALE
#ifdef FEATURE_MDP_WSCALE
#define MDP_WSCALE_IN_WIDTH_LINEAR_AND_EDGE   (240)  // DO NOT MODIFY!!!!!!
#define MDP_WSCALE_IN_HEIGHT_LINEAR_AND_EDGE  (320)  // DO NOT MODIFY!!!!!!
#define MDP_WSCALE_IN_WIDTH_NO_STRETCH        (192)  // DO NOT MODIFY!!!!!!
#define MDP_WSCALE_IN_HEIGHT_NO_STRETCH       (320)  // DO NOT MODIFY!!!!!!
#define MDP_WSCALE_OUT_WIDTH                  (240)  // DO NOT MODIFY!!!!!!
#define MDP_WSCALE_OUT_HEIGHT                 (400)  // DO NOT MODIFY!!!!!!
#endif // #ifdef FEATURE_MDP_WSCALE

enum 
{
  MDP_GFX_SCR,
  MDP_GOG_SCR,
  MDP_VDO_SCR,
  MDP_GOV_SCR,
#ifdef CUST_EDITION
  MDP_LCD_SCR,
#else
  MDP_EPSON_QCIF_SCR,
  MDP_EPSON_QVGA_SCR,
  MDP_TMD_QVGA_SCR,
  MDP_EP_OP_QCIFP_PRI_SCR,
  MDP_EP_OP_QCIFP_SEC_SCR,
#endif
#ifdef FEATURE_MDDI
#error code not present
#endif /* FEATURE_MDDI */
MDP_MAX_SCRNUM

};

typedef enum
{
  MDP_REV_01_00,
  MDP_REV_01_01,
  MDP_REV_01_02,      
  MDP_REV_01_03,
  MDP_REV_01_04
}MDP_REV;

/******************/
#define MDPOP_FFRMASK   (MDPOP_LR|MDPOP_UD|MDPOP_ROT90)
#define MDPOP_FFREND    (MDPOP_FFRMASK+1)
#define MDPOP_VDO_NOP   0
#define MDPOP_VDOMASK   (MDPOP_VDO_X2|MDPOP_VDO_1BY4|MDPOP_VDO_1BY2|MDPOP_VDO_3BY4)

#ifdef FEATURE_MDP_WSCALE
#define MDPOP_WSCALE_NOP   (0)
#define MDPOP_WSCALE_MASK  (MDPOP_WSCALE_LINEAR_STRETCH |\
                            MDPOP_WSCALE_EDGE_STRETCH |\
                            MDPOP_WSCALE_NO_STRETCH)
#endif // #ifdef FEATURE_MDP_WSCALE

#define MDP_MBC_NOP     0
#define MDPOP_MBCMASK   (MDPOP_TRANSP|MDPOP_ALPHAB)
#define MDPOP_MBC_SHIFT 7   /* 0x XX 0000 0000 -> 0x XX0 */
/******************/

typedef struct mdpFFRdata_
{
  uint32 imgAddr;
  uint32 line;
  uint32 block;
  uint32 stripe;
} MDP_FFR_DATA;

typedef struct mdpCropXY_
{
  uint16 cx1; 
  uint16 cx2; 
  uint16 cy1; 
  uint16 cy2;
} MDP_CROP_XY;


typedef struct mdpSubMddiWin_
{
  int32 x1; 
  int32 x2; 
  int32 y1; 
  int32 y2;

  uint16 lcdX;
  uint16 lcdY;
} MDP_SUBWIN_MDDI;

typedef struct mdpScrCnt_
{
  uint32 gfx_scr_cnt;
  uint32 gog_scr_cnt;
  uint32 vdo_scr_cnt;
  uint32 gov_scr_cnt;
#ifdef CUST_EDITION
  uint32 lcd_scr_cnt;
#else
  uint32 epson_qcif_scr_cnt;
  uint32 epson_qvga_scr_cnt;
  uint32 tmd_qvga_scr_cnt;
  uint32 ep_op_qcifp_pri_scr_cnt;
  uint32 ep_op_qcifp_sec_scr_cnt;
#endif
#ifdef FEATURE_MDDI
#error code not present
#endif /* FEATURE_MDDI */
} MDP_SCRCNT;

/* overlay interface API defines */
typedef struct mdpScale_Info_
{ 
  uint16 num;
  uint16 den;
  uint32 scale;
  uint32 scale_flag;
  uint32 odd_scale;
  uint32 lineNum;
} MDPSCALE_INFO;

typedef struct mdpImgList_
{
  uint32 key_id;
  struct mdpImgList_ *prev;
  struct mdpImgList_ *next;
  MDPIMG img;
  MDPIMG orig_img;
  uint32 mbcOp;     /* alpha-blend, tp operation */
  struct mdpImgList_ *bgPtr;
  int32 enabled;
  int32 locked;
  MDPSCALE_INFO scaleInfo;
  void * appCtx;
#ifdef DISP_TASK_BUFFER_SHARING  
  disp_packet_type * disp_packet;
  MDPIMG orig_img_before_queue_copy;  
#endif
}MDPIMG_LIST;

#define MDP_MAX_IMGLIST	128

#define MDP_CONVTP(tpVal) (((tpVal&0xF800)<<8)|((tpVal&0x7E0)<<5)|((tpVal&0x1F)<<3))

#define MDP_INVALID_LAYER(layer) ((layer) >= MDP_MAX_LAYER)

#ifdef FEATURE_MDP_SECONDARY_DISPLAY
#error code not present
#else
#define MDP_INVALID_DEST(mdpImg) ((mdpImg->dest != PRIMARY_LCD_TYPE))
#endif

// MDP minimum pixel size define and
// Simultaneous fill and dump (1=Enable, 0=Disable)

#ifdef FEATURE_MDDI
#error code not present
#else
  #define MDP_MIN_PIXEL_SIZE 2
#endif /* FEATURE_MDDI */

//LCD column/row
#define COL_IDX 0
#define ROW_IDX 1

#define GAMMA_TABLE_SIZE 64

/*===========================================================================
    LOCAL FUNCTIONS PROTOTYPES
============================================================================*/
void mdp_set_gamma(float gammaR, float gammaG, float gammaB);
void mdp_clear_ovimg_sub(MDPIMG_LIST *node);
uint32 mdp_verify_dispArea(MDPIMG *mdpImg);
void mdp_touch_img(MDPIMG_LIST *node);
void mdp_start_script(uint32 *scrPtr,
                      LCD_TYPE destDev,
                      boolean addVsyncScript,
                      MDP_CWIN vsyncCwin);
void mdp_wait_script_done(void);
void mdp_scrlist_init(void);
uint32 * mdp_scr_get(int q);
void mdp_reset_scrcnt(MDP_SCRCNT *msc);
void mdp_scr_free(MDP_SCRCNT *msc);
void mdp_sub_layer_update(MDPIMG *mdpImg, MDPIMG_LIST *node);
void disp_cls_mdp(uint8 dst);
void mdp_cls(LCD_TYPE dest, uint16 x, uint16 y, uint16 width, uint16 height);
int mdp_mddi_sub_cal(MDP_OVER_XYInt *fgXY, 
                     MDP_SUBWIN_MDDI *fgSubMddi, int index, 
                     uint32 lineNum, int bypass, uint32 alignOffset);
void mdp_copyLayer565(int32 lcdX,
                      int32 lcdY,
                      uint32 width,
                      uint32 height,
                      uint32 mdpOp,
                      MDPSCALE_INFO scale,
                      uint32 wScale,
                      uint16 *buf565Ptr,
                      uint32 bufSzBytes);
void mdp_clearLayer565(uint16 *buff565Ptr,
		               uint32 buffSzBytes,
		               uint16 clearValue);
void mdpCalCropXY(  uint16 x1, 
                    uint16 x2, 
                    uint16 y1, 
                    uint16 y2, 
                    uint16 vdbX1, 
                    uint16 vdbX2, 
                    uint16 vdbY1, 
                    uint16 vdbY2, 
                    uint32 mdpOp,
                    MDP_CROP_XY *crobXY);
int mdpCalFFRdataFg(uint32 * image_addr,
                    uint32 mdpOp,
                    uint16 width,
                    uint16 vdbX1, 
                    uint16 vdbX2, 
                    uint16 vdbY1, 
                    uint16 vdbY2, 
                    MDP_FFR_DATA *ffrPtr,
                    MDPIMG_TYPE imgType
                    );
int mdpCalFFRdataBg(uint32 * image_addr,
                    uint32 mdpOp,
                    uint16 width,
                    uint16 vdbX1, 
                    uint16 vdbX2, 
                    uint16 vdbY1, 
                    uint16 vdbY2, 
                    MDP_FFR_DATA *ffrPtr,
                    MDPIMG_TYPE imgType
                    );
int mdpCalFFRdataLuma(  uint32 * image_addr,
                        uint32 mdpOp,
                        uint32 scale,
                        uint16 width,
                        uint16 vdbX1, 
                        uint16 vdbX2, 
                        uint16 vdbY1, 
                        uint16 vdbY2, 
                        MDP_FFR_DATA *ffrPtr,
                        int16 bypp    /* bytes per pixel */
                        );

int mdpCalFFRdataChroma(uint32 * image_addr,
                        uint32 mdpOp,
                        uint32 scale,
                        uint16 width,
                        uint16 vdbX1, 
                        uint16 vdbX2, 
                        uint16 vdbY1, 
                        uint16 vdbY2, 
                        MDP_FFR_DATA *ffrPtr,
                        int16 bypp);    /* bytes per pixel */
void mdp_get_real_xy(MDPIMG_LIST *fgImg);
int mdp_mem_block_copy(uint16 *src_addr, 
                       uint32 src_width, 
                       uint32 src_height, 
                       uint16 *dst_addr);

MDP_STATUS mdp_disp_update_proc(MDPIMG *mdpImg, disp_cb_type * user_cb);
uint32 mdp_disp_update_sub(MDPIMG *mdpImg, boolean mdpMemMode, uint16 *outBuf,
                           uint32 bufSzBytes, disp_cb_type * user_cb);

int mdp_disp_update_gfx(LCD_TYPE destDev,
                        uint32 *fg_image_addr, 
                        MDPIMG_TYPE fg_imgType,
                        uint16 fg_width, 
                        uint16 fg_x1_off,
                        uint16 fg_x2_off,
                        uint16 fg_y1_off,    
                        uint16 fg_y2_off,
                        int32  fg_x, 
                        int32  fg_y,
                        uint32 fg_mdpOp
                        );
int mdp_disp_gfx_sub(   LCD_TYPE destDev,
                        uint32 *image_addr, 
                        MDPIMG_TYPE imgType,
                        uint16 width, 
                        uint16 height, 
                        uint16 x1, 
                        uint16 x2,
                        uint16 y1,
                        uint16 y2, 	
                        uint16 lcd_col_start,
                        uint16 lcd_row_start,
                        uint32 *output_addr,
                        uint32 mdpOp,
                        uint32 *wScale,  // VDB output QVGA->WQVGA scaling
                        uint32 *linkAddr,
                        MDP_SCRCNT *mdpScrCnt);
int mdp_disp_vdo_sub(   LCD_TYPE destDev,
                        uint32 *Y_addr, 
                        uint32 *CbCr_addr,
                        uint16 width, 
                        uint16 height, 
                        uint16 x1, 
                        uint16 x2,
                        uint16 y1,
                        uint16 y2, 	
                        uint16 lcd_col_start,
                        uint16 lcd_row_start,
                        uint32 *output_addr,
                        uint32 mdpOp,
                        uint32 scale,   // input VDB video(YUV) scaling
                        uint32 *wScale,  // VDB output QVGA->WQVGA scaling
                        uint32 *linkAddr,
                        MDP_SCRCNT *mdpScrCnt);
int mdp_disp_gov_sub(   LCD_TYPE destDev,
                        uint32 *Y_addr,
                        uint32 *CbCr_addr,
                        uint16 width, 
                        uint16 height, 
                        uint16 x1, 
                        uint16 x2,
                        uint16 y1,
                        uint16 y2, 	
                        uint16 lcd_col_start,
                        uint16 lcd_row_start,
                        uint32 *output_addr,
                        uint32 mdpOp,
                        uint32 *bg_image_addr,
                        MDPIMG_TYPE bg_imgType,
                        uint32 bg_width,
                        uint32 bg_height,
                        uint16 bg_x1,
                        uint16 bg_x2,
                        uint16 bg_y1,
                        uint16 bg_y2,
                        uint32 bg_mdpOp,
                        uint32 scale,   // input VDB video(YUV) scaling
                        uint32 *wScale,  // VDB output QVGA->WQVGA scaling
                        uint32 mbcOp,
                        uint32 tpVal,
                        uint32 alphaFrg,
                        uint32 alphaBkg,
                        uint32 *linkAddr,
                        MDP_SCRCNT *mdpScrCnt,
                        uint32 *linkPos);
int mdp_disp_gog_sub(   LCD_TYPE destDev,
                        uint32 *image_addr, 
                        MDPIMG_TYPE imgType,
                        uint16 width, 
                        uint16 height, 
                        uint16 x1, 
                        uint16 x2,
                        uint16 y1,
                        uint16 y2,
                        uint16 lcd_col_start,
                        uint16 lcd_row_start,
                        uint32 *output_addr,
                        uint32 mdpOp,
                        uint32 *bg_image_addr,
                        MDPIMG_TYPE bg_imgType,
                        uint32 bg_width,
                        uint32 bg_height,
                        uint16 bg_x1,
                        uint16 bg_y1,
                        uint32 bg_mdpOp,
                        uint32 *wScale,  // VDB output QVGA->WQVGA scaling
                        uint32 mbcOp,
                        uint32 tpVal,
                        uint32 alphaFrg,
                        uint32 alphaBkg,
                        uint32 *linkAddr,
                        MDP_SCRCNT *mdpScrCnt,
                        uint32 *linkPos);
void mdp_vsync_handler(void);
void mdp_check_ccs(MDPIMG *mdpImg);
void mdp_set_ccs(MDP_CCS_TYPE *ccs, boolean in_reset);
void mdp_set_vsync(void);
void mdp_hw_init(void);
void mdp_reset(void);
void mdp_isr(void);
int mdp_drv_init(void);
int mdp_ioctl(int cmd, void *arg);
int mdp_install(char * str);
boolean mdp_verify_dest(LCD_TYPE dest);
int mdp_is_dest_mddi(LCD_TYPE dest);

#ifdef FEATURE_HS_DISPLAY_CAPTURE
//screen capture functions
uint32 mdp_capture_display(MDPIMG *Image);
void mdp_enable_capture_mode(boolean flag);
#endif /*FEATURE_HS_DISPLAY_CAPTURE*/

/*===========================================================================
============================================================================*/

#ifdef FEATURE_MDP_WSCALE
boolean mdp_valid_img_for_wscale(uint16 lcd_col_start,
                                 uint16 lcd_col_end,
                                 uint16 lcd_row_start,
                                 uint16 lcd_row_end,
                                 uint32 wScale);
#endif // #ifdef FEATURE_MDP_WSCALE

#if (defined (T_QSC1110))
   #include "armmmu.h"  /*only needed on ULC for new Cache handling APIs.It has Cache write back policy*/
   #define MDP_INVALIDATE_DCACHE_REGION(ptr,size)   mmu_dcache_inval_region((void *)(ptr),(uint32)(size));
   /*On ULC,The INVALIDATE operation causes the stale information in the data cache lines to be marked 
    *available for reuse: there is no write of cached information to volatile memory. */
   #define MDP_FLUSH_DCACHE_REGION(ptr,size)        mmu_dcache_flush_region((void *)(ptr),(uint32)(size));
   /*On ULC, The FLUSH operation is the combination of the CLEAN and INVALIDATE operations.
    *The CLEAN operation causes write of cached information in the data cache out to the 
	*volatile memory and then the cache lines are not specifically marked available for reuse. 
    *FLUSHING is the safe mechanism to write cached information and mark the cache lines as stale, to prepare for reuse. */
								 
 #else /*if (defined (T_QSC1110))*/
   extern void mmu_invalidate_data_cache_lines( uint32* start_ptr, uint32 num_bytes );
   #define MDP_INVALIDATE_DCACHE_REGION(ptr,size)   mmu_invalidate_data_cache_lines((uint32 *)(ptr),(uint32)(size));  /*Actually does flushing*/
   #define MDP_FLUSH_DCACHE_REGION(ptr,size)        mmu_invalidate_data_cache_lines((uint32 *)(ptr),(uint32)(size));  /*Depricated on Newer targets*/
#endif /*if (defined (T_QSC1110))*/

#endif /* MDP_DRV_H */

