#============================================================================
#  Name:
#    ssmanagement.mak
#
#        Copyright © 2000-2001 QUALCOMM Incorporated.
#               All Rights Reserved.
#            QUALCOMM Proprietary/GTDR
#
#----------------------------------------------------------------------------
#============================================================================


TARGET     = gsm1xssm                # Target name for output files and object dir
TARGETROOT = ..

TARGET_OBJS = \
	    AEEAppGen.o \
            AEEModGen.o \
            SSManagement_App.o \
            SSManagement_DialogHandler.o \
            SSManagement_DialogMsgDecoding.o \
            SSManagement_Mod.o \
            SSManagement_StateMachine.o


!INCLUDE $(TARGETROOT)\gsm1xcommon\gsm1xcommon.mak

# ------------------------------
# DO NOT EDIT BELOW THIS LINE
