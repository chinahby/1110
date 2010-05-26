//Last Modify Date : 2003-5-6 17:02:35
//:EB_funcs.h : ErBiDigital export API functions.

#ifndef __EB_FUNCS_H_
#define __EB_FUNCS_H_
#include "EB.h"

#ifdef __cplusplus
extern "C" {
#endif
    int EBStartup(const void *pdata);
    int EBInputMethodInit(pEBInputStruct pEBIS);
    int EBSwitchInputMethod(pEBInputStruct pEBIS,
			    unsigned char inputMethod);
    int EBFindFirstPageC(pEBInputStruct pEBIS);
    int EBInputReset(pEBInputStruct pEBIS);
    int EBAssocFirstPageC(pEBInputStruct pEBIS);
    void EBShutdown(void);
    int EBStringToStroke(pEBInputStruct pEBIS, char *pBuffer,
			 int bufferLen);

    int EBCanPageDown(pEBInputStruct pEBIS);
    int EBCanPageUp(pEBInputStruct pEBIS);
    int EBIsAssociateMode(pEBInputStruct pEBIS);
    int EBIsInterpunctionMode(pEBInputStruct pEBIS);
    int EBGetStatus(pEBInputStruct pEBIS, unsigned long *pStatus);
    int EBAssocAddHistoryISN(pEBInputStruct pEBIS,
			     const unsigned short *isns, int length);
    int EBAssocSetHistoryISNs(pEBInputStruct pEBIS,
			      const unsigned short *ISNs, int length);
    int EBInterpunctionFirstPageC(pEBInputStruct pEBIS);
    int EBNextPageC(pEBInputStruct pEBIS);
    int EBPrevPageC(pEBInputStruct pEBIS);
//research
    int EBWord2Codes(pEBInputStruct pEBIS, unsigned short isn,
		     char *pCodeBuffer);

    typedef void (*fpEBShDataUpdate) (const void *pData,
				      unsigned long len);

    int EBShDataInit(void *pData, unsigned long len,
		     unsigned short minCountToAdjustPhraseFreq,
		     unsigned short minCountToAdjustZiFreq,
		     fpEBShDataUpdate updateFunction);

    //自造词数据重置
    int EBShDataReset(void);

    int EBShAddPhrase(const unsigned short *isns,
		      const char *syllable_together);

    int EBHelperDeleteInputChar(pEBInputStruct pEBIS);

    int EBHelperAddInputChar(pEBInputStruct pEBIS, int chr);

    int EBHelperEBISRestart(pEBInputStruct pEBIS);

    int EBHelperSelect(pEBInputStruct pEBIS, unsigned short index);

    int EBHelperCancelSelect(pEBInputStruct pEBIS);

#ifdef __cplusplus
}
#endif
#endif				//__EB_FUNCS_H_
