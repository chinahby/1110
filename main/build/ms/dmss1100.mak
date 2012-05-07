#============================================================================
#  Name:
#    DMSS6550.mak
#
#  Description:
#    Makefile to build the DMSS software for the SURF (Subscriber Unit
#    Reference Design) using the MSM6550 (ARM) ASIC.
#
# Copyright (c) 2004-2006 by QUALCOMM, Incorporated.  All Rights Reserved.
#============================================================================
#
#                        EDIT HISTORY FOR MODULE
#
# $Header: //depot/asic/qsc1100/build/ms/dmss1100.mak#33 $ $DateTime: 2009/09/08 01:19:42 $ $Author: pmuthe $
#
# when       who     what, where, why
# -------    ---     --------------------------------------------------------
# 06/15/07   gdp     Changes for WLAN Support.
# 05/21/07   gdp     Added QMDP Objects.
# 05/01/07   amw     Include files for NOR partition table.
# 01/15/07   fs      Added PARB
# 11/14/06   ven     Removed QTCAM
# 07/25/06   mpa     Added DATA_MAIN_MDM_OBJS
# 02/08/06   ljl     Added RVCT support
# 07/28/05   wd      L4 Merge
# 03/03/05   ap      Add VIDEOFMT.
# 02/25/05   jkl     Fixed VR featurization.
# 02/23/05   ap      Added QVP.
# 02/17/05   ap      Added PMAPP_OTG_OBJS
# 01/13/05   ap      Added profiling.
# 01/19/05   ts      Added support for non-secured NOR boot.
# 12/02/03   ap      Added PBM TASK support.
# 11/19/04   ap      Added support for MVS.
# 11/18/04   ap      Deleted spritegame.
# 11/08/04   jas     Split ext_misc.min from ext_oem.min
# 10/27/04   ap      Added support for QALBUM.
# 09/23/04   ap      Deleted BCMCS_OBJS, moved to lib.
# 09/22/04   ktr     Added Progressive Boot support.
# 09/18/04   jkl     Added TCXOMGR.
# 09/16/04   ap      Deleted XMLPARSE.
# 09/13/04   jkl     Added FOTAand Joystick.
# 09/07/04   drl     Added JPEG.
# 08/31/04   mvd     Replaced CAMIF with CAMSENSOR.
# 08/12/04   ap      Added DATA HDRBC OBJS.
# 08/09/04   jkl     Added NETDIAGNOSTICS_OBJS.
# 07/19/04   ny      Added qtv_dec.lib
# 06/18/04   ry      Added BTPF_OBJS.
# 06/03/04   jkl     Added build time.
# 05/28/04   jkl     Updated features.
# 05/10/04   AP      Initial release
#============================================================================

all : build_start dmss build_end

#-------------------------------------------------------------------------------
# Path to ARM 1.2 tools
#-------------------------------------------------------------------------------
ARMHOME := $(subst \,/,$(ARMHOME))


#-------------------------------------------------------------------------------
# Target configuration type
#-------------------------------------------------------------------------------
ifeq ($(USES_SURF),yes)
   TARGCONFIG = surf
else
   ifeq ($(USES_FFA),yes)
      TARGCONFIG = ffa
   else
      TARGCONFIG = none
   endif
endif

ifeq ($(USES_SIGN_AMSS),yes)
   SIGN_AMSS = true
endif

#-------------------------------------------------------------------------------
# Secondary Boot Loader Generation
#-------------------------------------------------------------------------------
ifeq ($(USES_GEN_QC_SBL),yes)
   SBL_TYPE  = qcsbl
else
   ifeq ($(USES_GEN_OEM_SBL),yes)
      SBL_TYPE = oemsbl
   endif
endif

#-------------------------------------------------------------------------------
# Boot security type
#-------------------------------------------------------------------------------
ifeq ($(USES_ROMBOOT),yes)
   SEC_MODE = rom
else
ifeq ($(USES_SECBOOT),yes)
   SEC_MODE = sec
else
   SEC_MODE = nonsec
endif
endif
export SEC_MODE
#-------------------------------------------------------------------------------
# Boot and component types
#-------------------------------------------------------------------------------
ifeq ($(USES_NAND_SDRAM),yes)
   FLASH_TYPE = nand
   BOOT_TYPE  = multiple
else
   ifeq ($(USES_NOR_PSRAM),yes)
      FLASH_TYPE = nor
      BOOT_TYPE  = single
   else
      FLASH_TYPE = shadow
      BOOT_TYPE  = single
   endif
endif


ifeq ($(USES_MMI), yes)
MODELNAME = $(BUILD)#  #Product Model english name.
INTERVERSION  = $(SVNVERSION)_$(MODELBUILDTIME)

CUSTOMER_NAME = common# Change "common" to the customer name you will sell to
MODEL_RF_NV = rf_$(MODELNAME)_nv.h#
CUST_SVC_NV = svc_$(CUSTOMER_NAME)_nv.h#
CUST_OEM_NV = oem_$(CUSTOMER_NAME)_nv.h#
endif

#应用的标签
ifeq ($(USES_MMI),yes)
ifneq ($(USES_ZI),yes)
USES_T9=yes
endif
USES_KEYGUARD=yes
USES_BREW_AEE_SRC=yes
USES_HANWANG=no
MMI_ADD_FLDDBG=yes

ifneq ($(USES_LOWMEM),yes)
ifeq ($(USES_MP4),yes)
MMI_ADD_VIDEOPLAYER=yes
endif
MMI_ADD_MUSICPLAYER=yes
MMI_ADD_MEDIAGALLERY=yes
MMI_ADD_RECORDER=yes
MMI_ADD_GAME=yes
endif
MMI_ADD_FM=yes
MMI_ADD_WORLDTIME=yes
MMI_ADD_SPORT=no
MMI_ADD_TOUCHPAD=no
endif

ifeq ($(USES_CONSTEFS),yes)
CONSTEFS_DIR = fs:/_rebuildefs_
endif
#-------------------------------------------------------------------------------
# Core includes
#-------------------------------------------------------------------------------
include boot_targets_cfgdata.min
include dmss_flags.min
include incpaths.min
include dmss_$(ASIC)_flags.min
include armtools.min
-include amss_lint_flags.min
-include amss_rvct_flags.min
include boot_targets_version_$(SEC_MODE).min
include boot_targets_$(SEC_MODE).min
export MIBIB
#-------------------------------------------------------------------------------
# Object includes
#-------------------------------------------------------------------------------
include dmss_objects.min

build_start:
	@$(BTIME) -start build_time.bt -b

build_end: dmss   # added dependency so timing for parallel builds is accurate
	@$(BTIME) -end build_time.bt -b

#-------------------------------------------------------------------------------
# Object groups that are specific to this target
#-------------------------------------------------------------------------------
# INSERT OBJECTS MACROS HERE
# Build the full object list for this target
OBJECTS += \
                $(REX_OBJS) \
                $(ADC_OBJS)  \
                $(ADIE_OBJS) \
                $(ADSP_OBJS) \
                $(ANSIC_OBJS) \
                $(APPMGR_OBJS) \
                $(AUDFMT_OBJS) \
                $(AVS_OBJS) \
                $(AUDTP_OBJS)\
                $(AUDIOCAL_OBJS)\
		$(AUDIOSBC_OBJS) \
                $(BIO_OBJS) \
                $(BOOT_OBJS) \
                $(CHG_OBJS) \
                $(CLK_OBJS) \
                $(CLKREGIM_OBJS) \
                $(CM_OBJS) \
                $(CMX_OBJS) \
                $(COREAPP_OBJS) \
                $(MODEM_DATA_1X_707_OBJS) \
                $(MODEM_DATA_COMMON_1XATCOP_OBJS) \
                $(DATA_DS_OBJS) \
                $(DATA_ATCOP_OBJS) \
                $(DATA_CDPS_OBJS) \
                $(DATA_RLP_OBJS) \
                $(DATA_MIP_OBJS) \
                $(DATA_JCDMA_OBJS) \
                $(DCP_OBJS) \
                $(DH_OBJS) \
                $(DIAG_OBJS) \
                $(DLOAD_OBJS) \
                $(DMOV_OBJS) \
                $(DMOV_MSM_OBJS) \
                $(DRVLIB_OBJS)\
                $(DSM_OBJS) \
                $(FS_OBJS) \
                $(FTM_RF_OBJS) \
                $(GRAPH_OBJS) \
                $(HS_OBJS) \
                $(HS_SURF_OBJS) \
                $(HW_OBJS) \
                $(I2C_OBJS) \
                $(KPD_OBJS) \
                $(MDSP_OBJS) \
                $(MDSPSVC_OBJS) \
                $(MMOC_OBJS) \
                $(MUX_OBJS) \
                $(MVS_OBJS) \
                $(NVIM_OBJS) \
                $(PARB_OBJS) \
                $(PMAPP_GEN_OBJS) \
                $(PMAPP_OTG_OBJS) \
		$(PM_FTM_OBJS) \
                $(PNG_OBJS) \
                $(Q3D_DRV_OBJS) \
                $(QDSP_OBJS) \
		$(QDSPUTIL_OBJS) \
                $(RF_OBJS) \
                $(RTE_OBJS) \
                $(SADC_OBJS) \
                $(SBI_OBJS) \
                $(SD_OBJS) \
                $(SDAC_OBJS) \
                $(SEC_OBJS) \
                $(SIO_OBJS) \
		$(UART_OBJS) \
		$(RDEVMAP_OBJS) \
                $(SLEEP_OBJS) \
                $(SLEEPCTL_OBJS) \
                $(SRCH_OBJS) \
                $(SRCH4_OBJS) \
                $(1X_DRIVER_OBJS) \
                $(STM_OBJS) \
		$(SAMPLE_SERVER_OBJS) \
                $(TASK_OBJS) \
                $(TCXOMGR_OBJS) \
                $(THERM_OBJS) \
                $(TIME_OBJS) \
                $(TLMM_OBJS) \
                $(TMC_OBJS) \
                $(TOKENBUCKET_OBJS) \
                $(TRAMP_OBJS) \
                $(TRM_OBJS) \
                $(TS_CAMIF_DRV_OBJS) \
                $(USB_OBJS) \
                $(USB_CARKIT_OBJS) \
                $(UTILS_OBJS) \
                $(VBATT_OBJS) \
                $(VS_OBJS) \
                $(WMS_OBJS) \
                $(XMLLIB_OBJS) \
                $(EFUSE_OBJS) \
                $(FM_RDS_OBJS) \
                $(FM_OBJS) \
		$(ERR_OBJS) \
		$(ALARMCLOCK_OBJS)

ifeq ($(USES_LCD_TOUCH),yes)
OBJECTS += \
			$(TOUCHPAD_OBJS)
endif

OBJECTS2 = \
                $(AUXSETTINGS_OBJS) \
                $(FMRADIOAPP_OBJS) \
                $(CAMERACOMMONINTERFACE_OBJS) \
                $(CAMERACONTROL_OBJS) \
                $(CAMERAPARMS_OBJS) \
                $(CAMERARAW_OBJS) \
                $(CAMERASERVICES_OBJS) \
                $(CAMERA_DIAG_OBJS) \
                $(CAMERA_ADSP_OBJS) \
                $(CAMERA_VFE_OBJS) \
                $(CAMERA_FLASH_OBJS) \
                $(VIDEO_RESOURCES_OBJS)\
                $(CAMERA_HJR_OBJS)\
                $(CAMERA_YCBCRDOWNSIZER_OBJS)\
                $(CAMERA_LPM_OBJS)\
                $(CAMERA_3A_AEC_OBJS) \
                $(CAMERA_3A_AWB_OBJS) \
                $(CAMERA_3A_AF_OBJS) \
                $(CAMERA_3A_COMMON_OBJS) \
                $(CAMERA_CAMERADRIVETASK_OBJS) \
                $(CAMERA_QMOBICAT_OBJS) \
                $(CAMERA_MOBICAT_OBJS) \
                $(CAMSENSORCOMMON_OBJS) \
                $(CAMSENSOR_MT9D112_MU2M0YU_OBJS)\
                $(IPL_COLORCONVERSION_OBJS)\
                $(IPL_COMPOSE_OBJS)\
                $(IPL_HJR_OBJS)\
                $(IPL_REDEYE_OBJS)\
                $(IPL_SPECIALEFFECTS_OBJS)\
                $(IPL_STITCH_OBJS)\
                $(IPL_TRANSFORMATION_OBJS)\
                $(IPL_UTILITY_OBJS)\
                $(EXIF_OBJS) \
                $(JPEGADSPDRIVER_OBJS)\
                $(JPEGDECODER_OBJS)\
                $(JPEGENCODER_OBJS)\
                $(JPEGMALLOC_OBJS)\
                $(EXT_IIPL_OBJS)\
                $(EXT_IYCBCR_OBJS)\
                $(OEM_ICAMERA_OBJS) \
                $(OEM_IMAGEPRO_OBJS) \
                $(OEM_IBITMAPFX_OBJS)\
                $(EXT_QAUDIOFX_OBJS) \
                $(QCAMCOMMON_OBJS) \
                $(QCAMERA_OBJS) \
                $(QALBUM_OBJS) \
                $(MALLOC_MGR_OBJS) \
                $(AUDFMT_CMX_OBJS) \
                $(DATASTATSAPP_OBJS)\
                $(DISP_OBJS) \
                $(FLDDBG_OBJS) \
                $(JPEG_OBJS) \
                $(LCD_SURF_OBJS) \
                $(MDP_OBJS)\
                $(MDP_API_OBJS)\
                $(MEDIAPLAYER_OBJS) \
                $(NETSETTINGS_OBJS) \
                $(SHOW3D_OBJS) \
                $(SVCPRG_OBJS) \
                $(WMSAPP_OBJS) \
                $(OEM_OBJS) \
                $(OEM_IMEDIASVG_NONPK_OBJS) \
		$(OEM_USBDRIVER_LIB_OBJS) \
                $(STATIC_EXT_OBJS)\
                $(OEM_NONPK_OBJS) \
                $(OEM_ICAMERA_NONPK_OBJS) \
                $(OEM_IMAGEPRO_NONPK_OBJS) \
                $(OEM_AUDIO_NONPK_OBJS)\
                $(EXT_ADDRBOOKEXT_OBJS) \
                $(EXT_IALERT_OBJS) \
                $(EXT_ICM_OBJS) \
                $(EXT_IPDP_OBJS) \
                $(EXT_OEMCFGI_OBJS) \
                $(EXT_SYSMODELBASE_OBJS) \
                $(EXT_IBATT_OBJS)\
                $(EXT_ISUPPSVC_OBJS) \
                $(EXT_IRATS_OBJS) \
                $(EXT_IVECTOR_OBJS) \
                $(EXT_OEM_OBJS) \
                $(BTHOSTSTATEXT_OBJS) \
                $(EXT_QTV_OBJS) \
                $(EXT_IJOY_OBJS) \
                $(EXT_OVERLAY_OBJS) \
                $(EXT_I3D_OBJS) \
                $(EXT_IDIAG_OBJS) \
                $(EXT_IQOSSESSIONEX_OBJS) \
                $(EXT_QVP_OBJS) \
                $(ICARD_OBJS) \
                $(ICARDSESSION_OBJS) \
                $(ICAT_OBJS) \
                $(ISTK_OBJS) \
                $(EXT_WMS_LIB_OBJS) \
                $(AUDFMT_AACREC_OBJS) \
                $(EXT_GIFVIEWER_OBJS) \
                $(Q3D_LIB_OBJS)

#                $(ACP_OBJS) \        deleted for 6500

ifneq ($(USES_UIONE),yes)
ifneq ($(USES_MANGO_UI),yes)
OBJECTS2 += \
                $(CONTACT_OBJS) \
                $(DIALERAPP_OBJS) \
                $(RECENTCALLS_OBJS) \
                $(EXT_IANNUNCIATOR_OBJS) \
                $(EXT_IALARM_OBJS) 
endif
endif
ifeq ($(USES_MANGO_UI),yes)
        OBJECTS2 += $(BREWPORT_OBJS)
endif

ifeq ($(USES_PWRDB),yes)
        OBJECTS2 += $(PWRDB_OBJS)
endif

ifeq ($(USES_CAMERA),yes)
		OBJECTS2 += $(CAMSENSOR_GC0309_OBJS)
		OBJECTS2 += $(CAMSENSOR_SIV121A_OBJS)
		OBJECTS2 += $(CAMSENSOR_OV7675_OBJS)
		OBJECTS2 += $(SOFTDSP_OBJS)
endif

ifeq ($(USES_DSP),yes)
		OBJECTS2 += $(AIT_SENSOR_OBJS)
		OBJECTS2 += $(AIT_TV_TLG1120_OBJS)
		OBJECTS2 += $(A8_CAMERA_OBJS)
		OBJECTS2 += $(AIT_API_OBJS)
endif
#-------------------------------------------------------------------------------
# Object List Modifiers
#    Alter the object list based on the requested features
#-------------------------------------------------------------------------------

ifeq ($(USES_FDI),yes)
	OBJECTS += $(FDI_OBJS)
endif

ifneq ($(USES_THINUI),yes)
        OBJECTS += $(OBJECTS2)
endif

ifeq ($(USES_IALARM), yes)
        OBJECTS += $(EXT_IALARM_OBJS)
endif

ifeq ($(USES_HWTC),yes)
    # *** Need to be first to be able to change default DMSS settings to default HWTC settings
    include $(HWTC)/hwtc_flags.min
endif

ifeq ($(USES_HFAT),yes)
        OBJECTS += $(HFAT_OBJS)
endif


ifeq ($(USES_UIM),yes)
        OBJECTS += $(RUIM_OBJS) $(NVIM_RUIM_OBJS) $(MMGSDI_OBJS)
endif

ifeq ($(USES_AUTH),yes)
        OBJECTS += $(AUTH_OBJS)
endif

ifeq ($(USES_MMC),yes)
        OBJECTS += $(MMC_OBJS)
endif

ifeq ($(USES_SDCC),yes)
        OBJECTS += $(SDCC_OBJS)
endif

ifeq ($(USES_PLT),yes)
        OBJECTS += $(PLT_OBJS) $(RPC_OBJS)
endif

ifeq ($(USES_SVGDOM), yes)
        OBJECTS += $(EXT_SVGDOM_OBJS)
endif

ifeq ($(USES_FTM),yes)
        OBJECTS += $(FTM_OBJS)
  ifeq ($(USES_FTM_HWTC),yes)
          OBJECTS += $(FTM_HWTC_OBJS)
  endif
endif

OBJECTS += $(AUDFMT_MPEG_OBJS)  # sbin's fix for FEATURE_AAC inclusion.

ifeq ($(USES_CAMCORDER),yes)
        OBJECTS += $(CAMCORDER_OBJS)
		OBJECTS += $(CAMCORDER_LTIER_OBJS)
	OBJECTS += $(QCAMCORDER_OBJS)
endif

ifeq ($(USES_MP4),yes)
        OBJECTS += $(MPEG4FILEFORMAT_OBJS)
        OBJECTS += $(QTV_AUDIOPLAYER_OBJS)
        OBJECTS += $(QTV_PLAYERTASK_OBJS)
        OBJECTS += $(QTV_AVSYNC_OBJS)
        OBJECTS += $(QTV_PLAYERAPI_OBJS)
        OBJECTS += $(QTV_RENDERER_OBJS)
        OBJECTS += $(QTV_BASE_LEGACY_OBJS)
        OBJECTS += $(QTV_AUDIOFILEPARSER_OBJS)
        OBJECTS += $(QTV_ASFPARSER_OBJS)
        OBJECTS += $(QTV_AVIPARSER_OBJS)
        OBJECTS += $(QTV_FILEMEDIABASE_OBJS)
        OBJECTS += $(QTV_MP4PARSER_OBJS)
        OBJECTS += $(QTV_TIMETEXT_OBJS)
        OBJECTS += $(QTV_TRACKSELECTION_OBJS)
        OBJECTS += $(QTV_BASE_OBJS)
        OBJECTS += $(QTV_DESCRAMBLERPROXY_OBJS)
        OBJECTS += $(QTV_FASTTRACK_OBJS)
        OBJECTS += $(QTV_PROGRESSIVEDOWNLOAD_OBJS)
        OBJECTS += $(QTV_NETIO_OBJS)
        OBJECTS += $(QTV_PVPLAYLISTINFO_OBJS)
        OBJECTS += $(QTV_RTP_OBJS)
        OBJECTS += $(QTV_RTSP_OBJS)
        OBJECTS += $(QTV_SDP_BASE_OBJS)
        OBJECTS += $(QTV_MEDIAINFO_OBJS)
        OBJECTS += $(QTV_MEDIAINFOPARSER_OBJS)
        OBJECTS += $(QTV_DOWNLOAD_MEDIA_BASE_OBJS)
        OBJECTS += $(QTV_SESSIONINFO_OBJS)
        OBJECTS += $(QTV_SESSIONINFOPARSER_OBJS)
        OBJECTS += $(QTV_STREAMCONTROL_OBJS)
        OBJECTS += $(QTV_STREAMMAPI_OBJS)
        OBJECTS += $(QTV_STREAMPOLICY_OBJS)
        OBJECTS += $(QTV_STREAMSESSIONDB_OBJS)
        OBJECTS += $(QTV_STREAMUTILS_OBJS)
        OBJECTS += $(QTV_GENERIC_OBJS)
        OBJECTS += $(QTV_CONCURRENCYMGR_OBJS)
        OBJECTS += $(QTV_CONFIG_OBJS)
        OBJECTS += $(QTV_DIAG_OBJS)
        OBJECTS += $(QTV_MAPICORE_OBJS)
        OBJECTS += $(QTV_ADTS_OBJS)
        OBJECTS += $(QTV_FILTER_COMMON_OBJS)
        OBJECTS += $(QTV_FADER_OBJS)
        OBJECTS += $(QTV_LOGGER_OBJS)
        OBJECTS += $(QTV_REALTIME_OBJS)
        OBJECTS += $(QTV_TAM_OBJS)
        OBJECTS += $(QTV_TSB_OBJS)
        OBJECTS += $(QTV_VIDEOTS_OBJS)
        OBJECTS += $(QTV_FLO_OBJS)
        OBJECTS += $(QTV_MEDIAMASK_OBJS)
        OBJECTS += $(QTV_MAPIUTILS_OBJS)
        OBJECTS += $(QTV_BCAST_GENERIC_OBJS)
        OBJECTS += $(QTV_BSD_OBJS)
        OBJECTS += $(QTV_IPL_OBJS)
        OBJECTS += $(QTV_OSCL_OBJS)
        OBJECTS += $(QTV_QCPLAYER_OBJS)
        OBJECTS += $(QTV_TASK_OBJS)
        OBJECTS += $(QTV_VIDEODELAYANALYZER_OBJS)
        OBJECTS += $(QTV_ZREX_OBJS)
        OBJECTS += $(QTV_UTILS_ALLOCATOR_OBJS)
        OBJECTS += $(QTV_STREAMRECORDER_OBJS)
        OBJECTS += $(QTV_UTILS_URL_OBJS)
        OBJECTS += $(QTV_UTILS_SYSTEM_OBJS)
        OBJECTS += $(QTV_UTILS_BLOB_OBJS)
        OBJECTS += $(QTV_UTILS_HSM_OBJS)
        OBJECTS += $(QTV_UTILS_UNICODE_OBJS)
        OBJECTS += $(QTV_UTILS_F3_OBJS)
        OBJECTS += $(QTV_UTILS_COMMON_OBJS)
        OBJECTS += $(QTV_UTILS_DISPATCH_OBJS)
        OBJECTS += $(QTV_OEM_OBJS)
	OBJECTS += $(QTV_VIDEOPLAYER_OBJS)
	OBJECTS += $(QTV_STREAMDISABLE_OBJS)
endif

ifeq ($(USES_BT),yes)
        OBJECTS += $(BTHOSTCORE_OBJS)
        OBJECTS += $(BTHOSTFTM_OBJS)
  ifeq ($(USES_IBT),yes)
          OBJECTS += $(BTHOSTSTATAPP_OBJS)
          OBJECTS += $(BTHOSTIBT_OBJS)
  endif
  ifeq ($(USES_QBT),yes)
          OBJECTS += $(BTHOSTPF_OBJS)
  endif
endif

ifeq ($(USES_STA),yes)
        OBJECTS += $(STA_OBJS)
endif

ifeq ($(USES_ONCRPC),yes)
        OBJECTS += $(ONCRPC_OBJS)
endif


ifeq ($(USES_VRSD),yes)
        OBJECTS := $(filter-out $(PUREVOICE_OBJS), $(OBJECTS))
endif

ifeq ($(USES_EFS2),yes)
        OBJECTS += $(EFS_OBJS)
        OBJECTS += $(FLASH_OBJS)
endif

ifeq ($(USES_AUDFMT_AMR),yes)
        OBJECTS += $(AMRSUP_OBJS)
endif

ifeq ($(USES_WM),yes)
        OBJECTS += $(AUDFMT_WMA_OBJS)
endif

ifeq ($(USES_EGL),yes)
        OBJECTS += $(OGLES_OEM_OBJS)
endif

ifeq ($(USES_GRP),yes)
        OBJECTS += $(GRP_OBJS)
endif

ifeq ($(USES_GSTK),yes)
        OBJECTS += $(GSTK_OBJS)
        OBJECTS += $(CATAPP_OBJS)
endif

ifeq ($(USES_QVP),yes)
        OBJECTS += $(VIDEOPHONE_OBJS)
endif

ifeq ($(USES_PBMTASK),yes)
        OBJECTS += $(PBM_OBJS)
endif

ifeq ($(USES_HWTC), yes)
    # *** HWTC must be last. Need to remove unwanted objects/libraries.

    # HWTC requires the UIM drivers only
    # the hwtc.min will remove unwanted
    # files.
    ifeq ($(USES_UIM),no)
          OBJECTS += $(RUIM_OBJS)
    endif

    include $(HWTC)/hwtc.min
endif

#部分OBJS不在对应的min文件里添加,而是跑这里了
ifeq ($(USES_MMI),yes)

ifeq ($(USES_T9),yes)
        OBJECTS += $(T9_OBJS)
endif

ifeq ($(USES_REND),yes)
        OBJECTS += $(REND_OBJS)
endif

ifeq ($(USES_KEYGUARD),yes)
	OBJECTS += $(KEYGUARD_OBJS)
endif

ifeq ($(USES_BREW_AEE_SRC),yes)
        OBJECTS += $(AEE_OBJS)
endif

#包含自定义的菜单
ifeq ($(USES_MMI), yes)
include $(MMIDIR)/mmi.min
endif

endif

ifeq ($(USES_MDDI),yes)
     OBJECTS += $(MDDI_OBJS)
endif

ifeq ($(USES_PROGRESSIVE_BOOT), yes)
       OBJECTS += $(BOOT_PROGRESSIVE_OBJS)

endif

ifeq ($(USES_PROFDIAG), yes)
       OBJECTS += $(PROFDIAG_OBJS)
endif

ifeq ($(USES_DTV), yes)
       OBJECTS += $(TARGETDIR)/dtv_common.lib
endif

ifeq ($(USES_DTV_ISDB), yes)
       OBJECTS += $(TARGETDIR)/dtv_isdb.lib
endif

ifeq ($(USES_QIPCALL), yes)
       OBJECTS += $(QIPCALL_OBJS)
endif

ifeq ($(USES_VR), yes)
        OBJECTS += $(VDB_OBJS)
        OBJECTS += $(VM_OBJS)
        OBJECTS += $(VR_OBJS)
        OBJECTS += $(VRE_OBJS)
        OBJECTS += $(PUREVOICE_OBJS)
endif

ifeq ($(USES_QDJ), yes)
   OBJECTS += $(QDJ_OBJS)
endif

ifeq ($(USES_REAL),yes)
    OBJECTS += $(AUDFMT_REAL_AUDIO_OBJS)
    OBJECTS += $(QTV_REAL_OBJS)
    ifeq ($(USES_MP4),no)
        OBJECTS += $(QTV_BSD_OBJS)
    endif
endif

ifeq ($(USES_DITE_LINK), yes)
     # For L4, Rex and Tramp server libraries are built by the L4 build, which
     # invokes the AMSS/DMSS make file. Don't add to objects since we don't build
     DITE_OBJECTS += $(OBJECTS)\
        $(LIBDIR)/libl4.a $(LIBDIR)/libiguana.a $(LIBDIR)/libhwio.a $(LIBDIR)/libcircular_buffer.a \
        $(LIBDIR)/librex.a $(LIBDIR)/librange_fl.a $(LIBDIR)/libll.a $(LIBDIR)/libnaming.a \
        $(LIBDIR)/libl4e.a $(LIBDIR)/libqdms.a $(LIBDIR)/libqmalloc.a\
        $(LIBDIR)/libefs2.a $(LIBDIR)/libdata_buffer.a \
        $(LIBDIR)/libqids_smms.a \
        $(LIBDIR)/libqids_rpc_infra.a $(LIBDIR)/libqids_rpc_infra_rex.a \
        $(LIBDIR)/libqids_rpc_stubs_server.a $(LIBDIR)/libqids_rpc_stubs_common.a \
        $(LIBDIR)/libmutex.a
endif

ifeq ($(USES_SNDCTL),yes)
     OBJECTS += $(EXT_SNDCTL_OBJS)
endif

ifeq ($(USES_SEC_CLNT), yes)
     OBJECTS += $(SEC_OEM_OBJS)
endif

ifeq ($(USES_HSU), yes)
  OBJECTS += $(HSU_OBJS)
endif

ifeq ($(USES_SPI), yes)
  OBJECTS += $(SPI_OBJS)
endif

ifeq ($(USES_BREW_BUIW), yes)
  OBJECTS += $(BUIW_OBJS)
endif
#-------------------------------------------------------------------------------
# Add PBL_OBJS and CFG_DATA_OBJS to the object list if its a NOR boot.
#-------------------------------------------------------------------------------
ifeq ($(FLASH_TYPE), nor)
    OBJECTS += \
        ${PBL_OBJS}
endif

#-------------------------------------------------------------------------------
# Define KEEP_OBJECTS and add all the objects to be kept when we use -remove 
# linker option
#-------------------------------------------------------------------------------
KEEP_OBJECTS = pboot_pad_scl.o\(*\) bootapp.o\(*\) boot_reset_handler.o\(*\) boot_vectors.o\(*\) *\(SHARED_DATA\)

#-------------------------------------------------------------------------------
# Message Levels
#    These operate on object groups - so these instructions must appear after
#    all the object groups are named
#-------------------------------------------------------------------------------
include dmss_msg_config.min

#-------------------------------------------------------------------------------
# Dependency Generation
#    Names the location of the dependency file and options to the dependency
#    generator.
#-------------------------------------------------------------------------------
DEPFILE := $(TARGETDIR)/$(TARGETDIR).dep

ifeq ($(USES_36_DIGIT_DIAL_STRING),yes)
EXCLUDEDIR := -X ../../services/nv $(EXCLUDEDIR)
else
EXCLUDEDIR := -X ../../services/nv/jcdma_nv $(EXCLUDEDIR)
endif

DEPOPTS := @dmss_$(ASIC)_depopts.min $(EXCLUDEDIR) -d ../.. -bid $(BUILD) -f $(DEPFILE)

ifneq ($(MEM_USAGE),yes)
-include $(DEPFILE)
-include dmss_$(ASIC)_dep.min
endif

#-------------------------------------------------------------------------------
# Memory Usage Generation
#    Generate memory usage for this target
#-------------------------------------------------------------------------------
MEMOPTS := -b $(ASIC)$(BUILD)$(VERSION) -mf $(BUILD).map -lf $(MEMLIST) -sf memsort.lf > $(ASIC)$(BUILD)$(VERSION).mem.html

#-------------------------------------------------------------------------------
# Media File Copy Script
#   Script to copy various media, bar files, mif files, mod files, etc. to the
#   loadefs directory for packaging into HXB files
#-------------------------------------------------------------------------------
MEDIACOPY = dmss6550_mediacopy.cmd

OBJDIR = $(TARGETDIR)

#-------------------------------------------------------------------------------
# Build Rules
#-------------------------------------------------------------------------------
include dmss_rules.min
-include amss_lint_rules.min
-include amss_rvct_rules.min

