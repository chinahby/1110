#============================================================================
#  Name:
#    ssoperations.mak
#
#        Copyright © 2000-2001 QUALCOMM Incorporated.
#               All Rights Reserved.
#            QUALCOMM Proprietary/GTDR
#
#----------------------------------------------------------------------------
#============================================================================


TARGET     = gsm1xsso                # Target name for output files and object dir
TARGETROOT = ..

TARGET_OBJS =  \
            SSAgentApp.o \
            SSCallDatabase.o \
            SSOperations_AEEAppGen.o \
            SSOperations_AEEModGen.o \
            SSOperations_App.o \
            SSOperations_CC.o \
            SSOperations_CommonEventDefinitions.o \
            SSOperations_CommonMessageDefinitions.o \
            SSOperations_DialogAbout.o \
            SSOperations_DialogCommon.o \
            SSOperations_DialogCallDetails.o \
            SSOperations_DialogDialer.o \
            SSOperations_DialogHandler.o \
            SSOperations_DialogIncoming.o \
            SSOperations_DialogPopup.o \
            SSOperations_DialogPopupHandler.o \
            SSOperations_DialogConnectingIncomingCall.o \
            SSOperations_DialogEndingCall.o \
            SSOperations_DialogOriginatingSecondCall.o \
            SSOperations_DialogSwappingCalls.o \
            SSOperations_DialogBuildingMPTY.o \
            SSOperations_DialogMPTYCallDetails.o \
            SSOperations_DialogEntryMenu.o \
            SSOperations_DialogInvalidPhoneState.o \
            SSOperations_DialogVolume.o \
            SSOperations_DialogWaitForIncomingCall.o \
            SSOperations_FSM.o \
            SSOperations_Hold.o \
            SSOperations_MN.o \
            SSOperations_MPTY.o \
            SSOperations_Mod.o \
            SSOperations_Toggle.o \
            SSPromptApp.o \
            SSqueue.o \

!INCLUDE $(TARGETROOT)\gsm1xcommon\gsm1xcommon.mak

# ------------------------------
# DO NOT EDIT BELOW THIS LINE
