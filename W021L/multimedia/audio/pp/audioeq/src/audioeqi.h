#ifndef AUDIOEQI_H
#define AUDIOEQI_H
/*===========================================================================

                       A U D I O   E Q U A L I Z E R   
                           H E A D E R   F I L E

DESCRIPTION
  This header file contains all the definitions of the equalizer coefficient
  mathimatic function.

REFERENCES

Copyright (c) 2004 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/multimedia/audio/pp/audioeq/main/latest/src/audioeqi.h#1 $ $DateTime: 2008/05/20 07:02:55 $ $Author: vparasar $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/13/04    aw     Initial revision.

===========================================================================*/
#include "comdef.h"
#include "target.h"

long mul32x32Shift2(int x, int y, unsigned short shift);
int qdsp_norm(int input,int *output, int *shift_factor);
int sqrtfp(long a);
int dsplib_approx_divide(int numer,int denom,int *result,int *shift_factor);
int dsplib_approx_invert(int input,int *result,int *shift_factor);
int dsplib_taylor_invert(int ,int *,int *,int );

int calculateBassBoost(int V0, int fc, int fs, int *bassNum, int *bassDen);
int calculateBoostDEN(int cutoffQ0, int SamplingFreqQ0, int* KQ28);
int calculateNumeratorBassBoost(int V0, int K, int *numerator, int DEN);
int calculateDenominatorBassBoost(int K, int *denomenator, int DEN);


int calculateBandBoost(int , int , int , int , int *, int *);
int calculateBandBoostDEN(int cutoffQ0, int SamplingFreqQ0,int queQ0, int* KQ28,int *KbyQ);
int calculateNumBandBoost(int V0, int K, int *numerator, int DEN,int KbyQ);
int calculateDenBandBoost(int K, int *denomenator, int DEN, int KbyQ);

void calculateTrebleBoost(int , int , int , int *, int *,int *);
int calculateTrebleBoostDEN(int cutoffQ0, int SamplingFreqQ0,int queQ0, int* KQ28);
int calculateNumTrebleBassBoost(int V0, int K, int *numerator, int DEN,int KbyQ);
int calculateDenTrebleBassBoost(int K, int *denomenator, int DEN, int KbyQ);

void calculateTrebleCut(int V0, int fc, int fs, int *bassNum, int *bassDen);
int calculateTrebleCutDEN(int cutoffQ0, int SamplingFreqQ0,int V0, int* KQ28,int *Ksqrt2V0);
int calculateNumTrebleCut(int V0, int K, int *numerator, int DEN);
int calculateDenTrebleCut(int K, int *denomenator, int DEN,int Ksqrt2V0,int V0);


void calculatebandCut(int V0, int fc, int fs, int que, int *bandNum, int *bandDen);
int calculatebandCutDEN(int cutoffQ0, int SamplingFreqQ0,int V0, int queQ0,int* KQ28,int *KbyQ28,int *denSR);
int calculateNumbandCut(int V0, int K, int *numerator, int DEN, int KbyQ28,int denSR);
int calculateDenbandCut(int K, int *denomenator, int DEN,int V0,int KbyQ28,int denSR);

void calculateBassCut(int , int , int , int *, int *);
int calculateBassCutDEN(int cutoffQ0, int SamplingFreqQ0,int V0, int* KQ28,int *Ksqrt2V0,int *denSR);
int calculateNumBassCut(int V0, int K, int *numerator, int DEN, int denSR);
int calculateDenBassCut(int K, int *denomenator, int DEN,int Ksqrt2V0,int V0, int denSR);

void calculateTrebleboost(int V0, int fc, int fs, int *bassNum, int *bassDen, int *shift);
int calculateNumTrebleboost(int V0, int K, int *numerator, int DEN);
int calculateDenTrebleboost(int K, int *denomenator, int DEN);


/******* defnitions of the utility functions used ********************/

int sqrtfp(long a);
long mul32x32Shift2(int x, int y, unsigned short shift);
int dsplib_approx_divide(int numer,int denom,int *result,int *shift_factor);
int dsplib_approx_invert(int input,int *result,int *shift_factor);
int qdsp_norm(int input,int *output, int *shift_factor) ;
void qdsp_norm64(int64 inp,int *norm_inp,int *shift);
#endif /* AUDIOEQI_H */
