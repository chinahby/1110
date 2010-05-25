#============================================================================
#  Name:
#    ssutil.mak
#
#        Copyright © 2000-2001 QUALCOMM Incorporated.
#               All Rights Reserved.
#            QUALCOMM Proprietary/GTDR
#
#----------------------------------------------------------------------------
#============================================================================


TARGET     = gsm1xssutil         # Target name for output files and object dir
TARGETROOT = ..
TARGET_INC = -I$(TARGETROOT)\gsm1xsso

TARGET_OBJS =  \
            SSUtil_AEEModGen.o \
            SSUtil.o \
            SSFacilityExtToInt.o \


!INCLUDE $(TARGETROOT)\gsm1xcommon\gsm1xcommon.mak

# ------------------------------
# DO NOT EDIT BELOW THIS LINE

