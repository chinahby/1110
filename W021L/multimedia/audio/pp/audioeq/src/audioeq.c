/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        A U D I O   E Q U A L I Z E R   

GENERAL DESCRIPTION
  These file contains calculation for audio equalizer

EXTERNALIZED FUNCTIONS
  audioeq_calccoefs

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright(c) 2004 - 2006 by QUALCOMM, Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/multimedia/audio/pp/audioeq/main/latest/src/audioeq.c#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/03/09   bk      Added Macros __NON_DEMAND_PAGED_FUNCTION__ and 
                   __NON_DEMAND_PAGED_FUNCTION_END__ for non demand
                   pageable functions.
04/02/06    aw     Fixed compiling warnings.
12/13/04    aw     Fixed compiling warnings.
06/12/04    aw     Initial version.
===========================================================================*/

/*===========================================================================

                       INCLUDE FILES FOR MODULE


===========================================================================*/
#include "target.h"             /* Target specific definitions             */
#include "customer.h"           /* Customer specific definitions           */
#include "audioeqi.h"           /* Equalizer internal function definitions */

/* <EJECT> */
/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/
#define ONEQ28            (1<<28)            /* The value of 1 in Q28      */
#define SQRT2Q30          (0x5A827998)       /* Square root of 2 in Q30    */
#define PIQ29             (0x3243F6A8*2)     /* The value of PI in Q29     */
#define GAIN_INDEX_OFFSET (0)                /* The Gain table offset      */

/* Linear gains. Q28. To convert logarithmic gains to linear gains
*/
int V0LinearTable[]={
  268435456, /* 0 */
  301189535,
  337940217,
  379175160, /* 3 */
  425441527,
  477353244,
  535599149, /* 6 */
  600952130,
  674279380,
  756553907, /* 9 */
  848867446,
  952444939,
  1068660799, /* 12 */
  1199057137,
  1345364236,
  1509523501 /* 15 */
};

/*===========================================================================

FUNCTION voceq_calccoefs

DESCRIPTION
  This function calculate the coefs and the shift compensation for the 
  numerator coefs
  type: 1 -> bass
        2 -> treble
        3 -> band

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
extern void audioeq_calccoefs(
  int32  V0,
  int32  fc,
  int32  fs,
  uint16 type,
  int32  Q,
  int32  *bassNum,
  int32  *bassDen,
  uint16 *shiftQ)
{
  int shift;
	short i;
	int Den[3];
        int *Num = (int *)bassNum;
	*shiftQ=2;	// default shift compensation = 2
	/* select the correct equalizer required*/

  if (V0 < 0) {
    type += 3;
    V0 = -V0;
  }
	switch(type)
	{
	case 1: /* bassboost*/
		*shiftQ=(short)calculateBassBoost(V0, fc, fs, Num, Den);
		
		break;
	case 2:  /* trebleboost*/
		calculateTrebleboost(V0, fc, fs, Num, Den,&shift);
		*shiftQ = (short)(32- shift); //Q factor compensation shift
		break;
	case 3:	 /* bandboost*/
		*shiftQ=(short)calculateBandBoost(V0, Q, fc, fs, Num, Den);
		break;
	case 4:	 /* bass cut*/
		calculateBassCut(V0, fc, fs, Num, Den);
		break;
	case 5:
		calculateTrebleCut(V0, fc, fs, Num, Den);
		break;
	case 6:
		calculatebandCut(V0, fc, fs, Q, Num, Den);
		break;
	default:
		return;
	} /* end of switch*/

	for(i=0;i<2;i++)
	{
		bassDen[i]   = Den[i+1];
	}


} // end of wrapper function to calculate coefs.
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/


/*===========================================================================

FUNCTION calculateBassBoost

DESCRIPTION
  This function is used to calculate the coefs for bassboost case.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
int calculateBassBoost(int V0, int fc, int fs, int *bassNum, int *bassDen)
{
    int mySR=1, DEN, K;
    V0=V0LinearTable[(int)V0+GAIN_INDEX_OFFSET];
    /* return DEN = q28, K=q28 */
    DEN=calculateBoostDEN(fc, fs, &K);
    mySR = calculateNumeratorBassBoost(V0, K, bassNum, DEN);
    calculateDenominatorBassBoost(K, bassDen, DEN);
	mySR = 32-mySR;	 // Q factors for the numerator.
    return mySR;
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/


/*===========================================================================

FUNCTION SMLZ_InitDecoder_static

DESCRIPTION
  This function calculates K and DEN for given filter uses newton's devide 
  method.
  Wc    = 2*pi*(0.1/32); % cut off freq =  2*pi*fc/fs
  K     = Wc/2
  V0    = 10^(6/20);
  DEN   = (1+(2^0.5)*K+K^2);
  B1(1) = (1+(2*V0)^0.5*K+V0*(K^2))/DEN;
  B1(2) = (2*(V0*(K^2)-1))/DEN;
  B1(3) = (1-((2*V0)^0.5)*K+V0*(K^2))/DEN;
  A1(1) = 1;
  A1(2) = (2*(K^2-1))/DEN;
  A1(3) = ( 1- (2^0.5)*K + (K^2) )/DEN;

DEPENDENCIES
  None

RETURN VALUE
  DEN = (1+(2^0.5)*K+K^2); q28

SIDE EFFECTS
  None

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
int calculateBoostDEN(
  int cutoffQ0,                   /* cutoff or center freq.                */
  int SamplingFreqQ0,             /* sampling freq.                        */
  int* KQ28                       /* pi*(fc/fs), q28                       */
)
{
  int K, SR, x, y;
    //sfQ15 = SamplingFreqQ0 << 15;
    /* output = K with qfactor = SR */
    dsplib_approx_divide(cutoffQ0, SamplingFreqQ0, &K, &SR);
    /* Convert K to Q31 */
    if (SR > 31) {
        K = K >> (SR-31);
    } else {
        K = K << (31-SR);
    }

    /* Multiply K by pi */
    /* K = q31+q29-q32+q0= Q28 */
    K = mul32x32Shift2(K,PIQ29,0);

    /* Calculate DEN = DEN = (1+(2^0.5)*K+K^2); */
    /* SQRT2Q30 = 2^0.5, q30 */
    /* x = (2^0.5)*K, q28 */
    x = mul32x32Shift2(K,SQRT2Q30,2);
    /* y = K*K, q28 */
    y = mul32x32Shift2(K,K,4);
    x = ONEQ28 + x + y;
    *KQ28 = K;
    return x;
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/*===========================================================================

FUNCTION calculateNumeratorBassBoost

DESCRIPTION
  This function calculates Numerator coeff. for BassBoost

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
int calculateNumeratorBassBoost(
  int V0,                                /* Gain value                     */
  int K,                                 /* Shift value                    */
  int *numerator,                        /* output: numerator[], q30       */
  int DEN                                /* Den value                      */
)
{
    long V0double, V0sqrt,  KSqrQ28, temp1, temp2;
    int B1Q28, B2Q28, B3Q28, SR;
    int B1Q30, B2Q30, B3Q30;
	int normb;
	int SR1,SR2,SR3,Smax;

	int64 b0,b1,b2;
    /* v0DOUBLE = Q28 */
    V0double = V0;
    /* Take Sqrt of V0; V0sqrt = q14 */
    V0sqrt=sqrtfp(V0double); 
    /* V0sqrt = q28 */
    V0sqrt = V0sqrt << 14;

	/* sqrt(2*V0) = sqrt(2)*sqrt(v0), q28 */
    V0sqrt=mul32x32Shift2(V0sqrt,SQRT2Q30,2);
    /* calculate KsqrQ28 = q28  */
    KSqrQ28 = mul32x32Shift2(K, K, 4);  // no overflow till now


    /* temp1 (2*V0)^0.5*K = q16, mulshift2 = (q28+q28-q32)+4 = q27 */
    temp1 = mul32x32Shift2(V0sqrt,K,3); // no overflow, temp1 = sqrt(2*V0)*K 
	b2 = (int64)temp1<<1;				// make Q28 in a longer word.
										// b0 = sqrt(2*V0)*K
	temp2 = mul32x32Shift2(V0,KSqrQ28,2); // temp2 in Q26
	
	b1 = (int64)(temp2)<<2;			 // b1 = V0*K^2
	b0=(int64)ONEQ28+(int64)b2+ (int64)b1; // still in Q28

	// extra function now.
	qdsp_norm64(b0,&normb,&SR1);
	B1Q28 = normb; //Q fctr = srb+28;

	// calculat b1 now
	b0 = (int64)(b1-ONEQ28)<<1;
	qdsp_norm64(b0,&normb,&SR2);
	B2Q28 = normb;
	
	//calculate b2 = 1-sqrt(2*V0)*k +v0*k^2
	b2 = (int64)ONEQ28 - (int64)b2+ (int64)b1;
	qdsp_norm64(b2,&normb,&SR3);
    B3Q28 = (int)normb;		// no overflows.

	// now divide by DEN
	dsplib_approx_divide(B1Q28, DEN ,&B1Q30, &SR);
	SR1 = SR+SR1;
	dsplib_approx_divide(B2Q28, DEN ,&B2Q30, &SR);
	SR2 = SR+SR2;
	dsplib_approx_divide(B3Q28, DEN ,&B3Q30, &SR);
	SR3 = SR+SR3;

	// find min of the SRs.
	Smax = (SR1>SR2)?SR2:SR1;
	Smax= (Smax>SR3)?SR3:Smax;
	//Smax=30;

	numerator[0] = B1Q30>>(SR1-Smax);
    numerator[1] = B2Q30>>(SR2-Smax);
    numerator[2] = B3Q30>>(SR3-Smax);
    return Smax;
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/*===========================================================================

FUNCTION calculateDenominatorBassBoost

DESCRIPTION
  This function calculates denominator coeff. for BassBoost

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
int calculateDenominatorBassBoost(
  int K,                          /* inputs:  K q28,                       */
  int *denomenator,               /* Output: denominator[], q30            */
  int DEN                         /* inputs:  DEN q28                      */
)
{
    long  KSqrQ28, temp1;
    int A2Q28, A3Q28, SR;
    int A1Q30, A2Q30, A3Q30;
    // assume KsqrQ28 = q28 
    KSqrQ28 = mul32x32Shift2(K, K, 4);
    /* Calculate A(2) = (2*(K^2-1)), q28 */
    A2Q28 = 2*(KSqrQ28-ONEQ28);
    /* Calculate A(3) = temp1 = ( 1- (2^0.5)*K + (K^2) ) */
    /* temp1 = (2^0.5)*K, q28 */
    temp1 = mul32x32Shift2(K, SQRT2Q30, 2);
    A3Q28 = ONEQ28 - temp1 + KSqrQ28;
   /*  TEST */

    /* divide by DEN */
    A1Q30 = 1 << 30;
    dsplib_approx_divide(A2Q28, DEN ,&A2Q30, &SR);
    if (SR>30) 
        A2Q30 = A2Q30 >> (SR-30);
    else 
        A2Q30 = A2Q30 << (30-SR);
    dsplib_approx_divide(A3Q28, DEN ,&A3Q30, &SR);
   if (SR>30) 
        A3Q30 = A3Q30 >> (SR-30);
    else 
        A3Q30 = A3Q30 << (30-SR);
    denomenator[0]=A1Q30;
    denomenator[1]=A2Q30;
    denomenator[2]=A3Q30;
    return 1;
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/


/*===========================================================================

FUNCTION calculateBandBoost

DESCRIPTION
  This function calculate the coefficients of Bandboost
  K    = Wc/2. Wc=28pi*fc/fs
  DEN  = 1+k/Q+k^2
  V0   = 10^(gain/20)
  B(0) = (1+V0*K/Q+K^2)/DEN
  B(1) = 2*(K^2-1)/DEN
  B(2) = (1-V0K/Q +K^2)/DEN
  A(1) = 2*(K^2-1)/DEN
  A(2) = (1-K/Q+k^2)/DEN

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
int calculateBandBoost(int V0, int que, int fc, int fs, int *bassNum, int *bassDen)
{
	int mySR, DEN, K,KbyQ;
	/* Calcualte K*/
	/* Themax value of K is pi/2 = 1.5708*/
	/* hence the Q factor can be Q2.30 (unsigned)*/

	/* calculate V0, V0 in Q28*/
	V0=V0LinearTable[(int)V0+GAIN_INDEX_OFFSET];
	
	/* calculate DEN, returns K,KbyQ & DEN in Q28.*/
	DEN=calculateBandBoostDEN(fc, fs, que,&K, &KbyQ);

	/* calculate the numerator coeffs*/
    mySR = calculateNumBandBoost(V0, K, bassNum, DEN,KbyQ);
	/* calculate the denominator coeffs*/
    calculateDenBandBoost(K, bassDen, DEN,KbyQ);
	return (32-mySR);
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/*===========================================================================

FUNCTION calculateBandBoostDEN

DESCRIPTION
  This function calculates K and DEN for given filter uses dsp_approx_divide

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
int calculateBandBoostDEN
(
  int cutoffQ0,                    /* Input: fc: cutoff or center freq.    */
  int SamplingFreqQ0,              /* Input: fs: sampling freq.            */
  int queQ0,                       
  int *KQ28,                       /* Output: Kq28=pi*(fc/fs), q28.        */
  int *KbyQ28                      /* Output: DEN = 1+k/Q+k^2; q28         */
)                 
{
	    int SR, K, x, y;
		int invque,queSR;
	    //sfQ15 = SamplingFreqQ0 << 15;
	    /* output = K with qfactor = SR */
	    dsplib_approx_divide(cutoffQ0, SamplingFreqQ0, &K, &SR);

	    /* Convert K to Q31 */
	    if (SR > 31) {
		K = K >> (SR-31);
	    } else {
		K = K << (31-SR);
	    }


		/*calculate 1/Q. output: invque with qfactor=SR*/
		dsplib_approx_divide(1, queQ0, &invque, &queSR);
		queSR = queSR -8;	// Q is in Q8
	    /* Convert Q to Q31 */
	    if (queSR > 31) {
		invque = invque >> (queSR-31);
	    } else {
		invque = invque << (31-queSR);
	    }


	    /* Multiply K by pi */
	    /* K = q30+q29-q32+q0= Q28 */
	    K = mul32x32Shift2(K,PIQ29,0);
	    /* Calculate DEN = DEN = 1+k/Q+k^2; */
	    /* x= K *invQ, Q28+Q31-Q32+Q1=Q28*/    
	    x = mul32x32Shift2(K,invque,1);
		//x=K>>1;
		*KbyQ28=x;
	    /* y = K*K, q28 */
	    y = mul32x32Shift2(K,K,4);
	    x=ONEQ28+x+y;
    *KQ28 = K;
    return x;
} /* End of function to calculate DEN for bandboost*/
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/*===========================================================================

FUNCTION calculateNumBandBoost

DESCRIPTION
  This function calculates Numerator coeff. for BandBoost
  B(0) = (1+V0*K/Q+K^2)/DEN
  B(1) = 2*(K^2-1)/DEN
  B(2) = (1-V0K/Q +K^2)/DEN

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
int calculateNumBandBoost(
  int V0,                  /* input: V0 q28,                               */
  int K,                   /* input: K q28,                                */
  int *numerator,          /* output: numerator[], q30                     */
  int DEN,                 /* input: DEN, q28                              */
  int KbyQ                 /* input: KbyQ, q28                             */
)
{
    long  V0KbyQ,  KSqrQ28;
    int B0Q28,B1Q28, B2Q28, SR,SR1,SR3,SR2;
    int B0Q30, B1Q30, B2Q30;
	int64 b0,b2;
    int Smax,normb;
	/* calculate V0*K/Q */
    /* calculate KsqrQ28 = q28  */
    KSqrQ28 = mul32x32Shift2(K, K, 4);
	
    /* sqrt(2*V0) = sqrt(2)*sqrt(v0), q26 */
    V0KbyQ=mul32x32Shift2(V0,KbyQ,2);
    /* calculate KsqrQ28 = q28  */
    KSqrQ28 = mul32x32Shift2(K, K, 4);

    /* Calculate B(0) = (1+V0*K/Q+(K^2))*/
	/* B0 in Q 28						*/
	b0=(int64)ONEQ28 +((int64)V0KbyQ<<2)+(int64)KSqrQ28;
	//b0 = (int64)(b0>>1);
	qdsp_norm64(b0,&normb,&SR1);

	B0Q28 = normb; 
    
	/* Calculate B(1) = 2*(K^2-1) */
	B1Q28=(KSqrQ28-ONEQ28)<<1;

	/* Calculate B(2)= (1-V0K/Q +K^2)*/
	b2=(int64)ONEQ28-((int64)V0KbyQ<<2)+(int64)KSqrQ28;
    qdsp_norm64(b2,&normb,&SR3);
	B2Q28 = normb;
    /* divide by DEN */
    /* B1Q30 = qSR */
    dsplib_approx_divide(B0Q28, DEN ,&B0Q30, &SR);
	SR1=SR+SR1;
   
	dsplib_approx_divide(B1Q28, DEN ,&B1Q30, &SR2);
	//SR2=SR2+S

	dsplib_approx_divide(B2Q28, DEN ,&B2Q30, &SR);
	SR3=SR3+SR;
    Smax=(SR1>SR2)?SR2:SR1;
	Smax=(Smax>SR3)?SR3:Smax;

    numerator[0] = B0Q30>>(SR1-Smax);
    numerator[1] = B1Q30>>(SR2-Smax);
    numerator[2] = B2Q30>>(SR3-Smax);
    return Smax;
} /* end of band boost num calculations.*/
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/*===========================================================================

FUNCTION calculateDenBandBoost

DESCRIPTION
  This function calculates denominator coeff. for BassBoost
  A(1)= 2*(K^2-1)/DEN
  A(2)= (1-K/Q+k^2)/DEN

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
int calculateDenBandBoost(int K, int *denomenator, int DEN,int KbyQ28)
{
    long  KSqrQ28;
    int A1Q28, A2Q28, SR;
    int A0Q30, A1Q30, A2Q30;

    // assume KsqrQ28 = q28 
    KSqrQ28 = mul32x32Shift2(K, K, 4);
    /* Calculate A(2) = (2*(K^2-1)), q28 */
    A1Q28 = 2*(KSqrQ28-ONEQ28);
    /* Calculate A(2) = temp1 = ( 1- K/Q + (K^2) ) */
    A2Q28 = ONEQ28 - KbyQ28 + KSqrQ28;
   /*  TEST */

    /* divide by DEN */
    A0Q30 = 1 << 30;
    dsplib_approx_divide(A1Q28, DEN ,&A1Q30, &SR);
    if (SR>30) 
        A1Q30 = A1Q30 >> (SR-30);
    else 
        A1Q30 = A1Q30 << (30-SR);
    dsplib_approx_divide(A2Q28, DEN ,&A2Q30, &SR);
   if (SR>30) 
        A2Q30 = A2Q30 >> (SR-30);
    else 
        A2Q30 = A2Q30 << (30-SR);
    denomenator[0]=A0Q30;
    denomenator[1]=A1Q30;
    denomenator[2]=A2Q30;
    return 1;
}/*end of band boost den coefs calc*/
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/*===========================================================================

FUNCTION calculateTrebleCut

DESCRIPTION
  This function calculate the coefficients of Treblecut
  K=Wc/2. Wc=28pi*fc/fs
  V0 = 10^(gain/20)
  DEN = V0+sqrt(2V0)*k+k^2
  B(0)= (1-sqrt(2)K+K^2)/DEN
  B(1)= 2*(K^2-1)/DEN
  B(2)= (1-sqrt(2)*K +K^2)/DEN
  A(1)= 2*(K^2-V0)/DEN
  A(2)= (V0-sqrt(2*V0)K+k^2)/DEN

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
void calculateTrebleCut(int V0, int fc, int fs, int *bassNum, int *bassDen)
{
	int DEN, K,Ksqrt2V0;
	/* Calcualte K*/
	/* Themax value of K is pi/2 = 1.5708*/
	/* hence the Q factor can be Q2.30 (unsigned)*/

	/* calculate V0, V0 in Q28*/
	V0=V0LinearTable[(int)V0+GAIN_INDEX_OFFSET];
	
	/* calculate DEN, returns K,KbyQ & DEN in Q27.*/
	DEN=calculateTrebleCutDEN(fc, fs, V0,&K, &Ksqrt2V0);
	
	/* calculate the numerator coeffs*/
    calculateNumTrebleCut(V0, K, bassNum, DEN);
	/* calculate the denominator coeffs*/
    calculateDenTrebleCut(K, bassDen, DEN,Ksqrt2V0,V0);
 }
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/*===========================================================================

FUNCTION calculateTrebleCutDEN

DESCRIPTION
  This function calculates K and DEN for given filter uses dsp_approx_divide

DEPENDENCIES
  None

RETURN VALUE
  DEN = 1+k/Q+k^2; q27

SIDE EFFECTS
  None

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
int calculateTrebleCutDEN
(
  int cutoffQ0, 
  int SamplingFreqQ0,
  int V0, 
  int *KQ28,
  int *Ksqrt2V0
)
{
    int SR, K, x, y;
	
	long V0double,V0sqrt;
	int64 den;
    //sfQ15 = SamplingFreqQ0 << 15;
    /* output = K with qfactor = SR */
    dsplib_approx_divide(cutoffQ0, SamplingFreqQ0, &K, &SR);

    /* Convert K to Q31 */
    if (SR > 31) {
        K = K >> (SR-31);
    } else {
        K = K << (31-SR);
    }

    /* Multiply K by pi */
    /* K = q31+q29-q32+q0= Q28 */
    K = mul32x32Shift2(K,PIQ29,0);

    /* v0DOUBLE = Q28 */
    V0double = V0;
    /* Take Sqrt of V0; V0sqrt = q14 */
    V0sqrt=sqrtfp(V0double); 
    /* V0sqrt = q28 */
    V0sqrt = V0sqrt << 14;
    /* sqrt(2*V0) = sqrt(2)*sqrt(v0), q28 */
    V0sqrt=mul32x32Shift2(V0sqrt,SQRT2Q30,2);


    /* Calculate DEN = DEN = V0+sqrt(2*V0)k+k^2; */
    /* x= K *sqrt(2*V0), Q28+Q28-Q32+Q3=Q27*/    
    x = mul32x32Shift2(K,V0sqrt,4);
	*Ksqrt2V0=x;
	/* y = K*K, q28 */
    y = mul32x32Shift2(K,K,4);
    den=(int64)V0+(int64)x+(int64)y;
	den=(int64)den>>1;
    *KQ28 = K;
    return ((int)den);
} /* End of function to calculate DEN for bandboost*/
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/*===========================================================================

FUNCTION calculateNumTrebleCut

DESCRIPTION
  This function calculates Numerator coeff. for TrebleCut
  B(0)= (1-sqrt(2)K+K^2)/DEN
  B(1)= 2*(K^2-1)/DEN
  B(2)= (1+sqrt(2)*K +K^2)/DEN


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
int calculateNumTrebleCut(int V0, int K, int *numerator, int DEN)
{
    long  KSqrQ28;
    int B0Q28,B1Q28, B2Q28, SR;
    int B0Q30, B1Q30, B2Q30;
    long KSqr2Q28;
	/* calculate KsqrQ28 = q28  */
    KSqrQ28 = mul32x32Shift2(K, K, 4);
	
    /* sqrt(2)*K = q28 */
    KSqr2Q28=mul32x32Shift2(SQRT2Q30,K,2);
    
    /* Calculate B(0) = (1-sqrt(2)K+K^2)*/
	/* B0 in Q 28						*/
    B0Q28 = ONEQ28 -KSqr2Q28+KSqrQ28;
    
	/* Calculate B(1) = 2*(K^2-1) */
	B1Q28=(KSqrQ28-ONEQ28)<<1;

	/* Calculate B(2)= (1+sqrt(2)*K +K^2))*/
	B2Q28 = ONEQ28+KSqr2Q28+KSqrQ28;
    
    /* divide by DEN */
    /* B1Q30 = qSR */
    dsplib_approx_divide(B0Q28, DEN ,&B0Q30, &SR);
    /* Convert B3Q30 to Q30 */
    if (SR > 30)
        B0Q30 = B0Q30 >> (SR-30);
    else
        B0Q30 = B0Q30 << (30-SR);
    dsplib_approx_divide(B1Q28, DEN ,&B1Q30, &SR);
    /* Convert B1Q30 to Q30 */
    if (SR > 30)
        B1Q30 = B1Q30 >> (SR-30);
    else
        B1Q30 = B1Q30 << (30-SR);
    dsplib_approx_divide(B2Q28, DEN ,&B2Q30, &SR);
    /* Convert B2Q30 to Q30 */
    if (SR > 30) 
        B2Q30 = B2Q30 >> (SR-30);
    else 
        B2Q30 = B2Q30 << (30-SR);
    numerator[0] = B0Q30>>1; // to compensate for Q27 of den
    numerator[1] = B1Q30>>1;
    numerator[2] = B2Q30>>1;
    return 1;
} /* end of band boost num calculations.*/
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/*===========================================================================

FUNCTION calculateDenTrebleCut

DESCRIPTION
  This function calculates denominator coeff. for TrebleCut
  A(1)= 2*(K^2-V0)/DEN
  A(2)= (V0-sqrt(2*V0)K+k^2)/DEN

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
int calculateDenTrebleCut(int K, int *denomenator, int DEN,int Ksqrt2V0,int V0)
{
    long  KSqrQ28;
    int A1Q28, A2Q28, SR;
    int A0Q30, A1Q30, A2Q30;

    // assume KsqrQ28 = q28 
    KSqrQ28 = mul32x32Shift2(K, K, 4);
    /* Calculate A(2) = (2*(K^2-V0)), q27 */
    A1Q28 = (KSqrQ28-V0);
    /* Calculate A(2) = temp1 = ( V0- sqrt(2*V0)K + K^2 ) */
    A2Q28 = V0 - Ksqrt2V0 + KSqrQ28;
   /*  TEST */

    /* divide by DEN */
    A0Q30 = 1 << 30;
    dsplib_approx_divide(A1Q28, DEN ,&A1Q30, &SR);
    if (SR>30) 
        A1Q30 = A1Q30 >> (SR-30);
    else 
        A1Q30 = A1Q30 << (30-SR);
    dsplib_approx_divide(A2Q28, DEN ,&A2Q30, &SR);
   if (SR>30) 
        A2Q30 = A2Q30 >> (SR-30);
    else 
        A2Q30 = A2Q30 << (30-SR);
    denomenator[0]=A0Q30;
    denomenator[1]=A1Q30;
    denomenator[2]=A2Q30>>1;
    return 1;
}/*end of band boost den coefs calc*/
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/*===========================================================================

FUNCTION calculateBassCut

DESCRIPTION
  This function  calculate the coefficients of BassCut
  K=Wc/2. Wc=28pi*fc/fs
  V0 = 10^(gain/20)
  DEN = 1+sqrt(2V0)*k+V0*k^2
  B(0)= (1+sqrt(2)K+K^2)/DEN
  B(1)= 2*(K^2-1)/DEN
  B(2)= (1-sqrt(2)*K +K^2)/DEN
  A(1)= 2*(V0*K^2-1)/DEN
  A(2)= (1-sqrt(2*V0)K+V0*k^2)/DEN

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
void calculateBassCut(int V0, int fc, int fs, int *bassNum, int *bassDen)
{
	int DEN, K,Ksqrt2V0,denSR;
	/* Calcualte K*/
	/* Themax value of K is pi/2 = 1.5708*/
	/* hence the Q factor can be Q2.30 (unsigned)*/

	/* calculate V0, V0 in Q28*/
	V0=V0LinearTable[(int)V0+GAIN_INDEX_OFFSET];
	
	/* calculate DEN, returns K,Ksqrt2V0 & DEN in Q28.*/
	DEN=calculateBassCutDEN(fc, fs, V0,&K, &Ksqrt2V0, &denSR);
	
	/* calculate the numerator coeffs*/
    calculateNumBassCut(V0, K, bassNum, DEN,denSR);
	/* calculate the denominator coeffs*/
    calculateDenBassCut(K, bassDen, DEN,Ksqrt2V0,V0,denSR);
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/*===========================================================================

FUNCTION calculateBassCutDEN

DESCRIPTION
  This function calculates K and DEN for given filter uses dsp_approx_divide

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
int calculateBassCutDEN(int cutoffQ0, int SamplingFreqQ0,int V0,int *KQ28,int *Ksqrt2V0, int *denSR)
{
    int SR, K ;
	long V0double,V0sqrt,KSqrQ28;
	int normden,temp2;
	int64 v0ksqr;
	int64 den;
    //sfQ15 = SamplingFreqQ0 << 15;
    /* output = K with qfactor = SR */
    dsplib_approx_divide(cutoffQ0, SamplingFreqQ0, &K, &SR);

    /* Convert K to Q31 */
    if (SR > 31) {
        K = K >> (SR-31);
    } else {
        K = K << (31-SR);
    }

    /* Multiply K by pi */
    /* K = q31+q29-q32+q0= Q28 */
    K = mul32x32Shift2(K,PIQ29,0);
    /* v0DOUBLE = Q28 */
    V0double = V0;
    /* Take Sqrt of V0; V0sqrt = q14 */
    V0sqrt=sqrtfp(V0double); 
    /* V0sqrt = q28 */
    V0sqrt = V0sqrt << 14;
    /* sqrt(2*V0) = sqrt(2)*sqrt(v0), q28 */
    V0sqrt=mul32x32Shift2(V0sqrt,SQRT2Q30,2);

	*Ksqrt2V0 = mul32x32Shift2(V0sqrt,K,4);	// sqrt(2*V0)*K
	

	    /* calculate KsqrQ28 = q28  */
    KSqrQ28 = mul32x32Shift2(K, K, 4);  // no overflow till now

	temp2 = mul32x32Shift2(V0,KSqrQ28,2); // temp2 in Q26

	v0ksqr =(int64)temp2<<2;			 //V0*K^2 in Q28

	den = (int64)ONEQ28  + (int64)(*Ksqrt2V0) +(int64)(v0ksqr);

	*KQ28 =K;
	
	qdsp_norm64(den,&normden,denSR);
	return normden;
} /* End of function to calculate DEN for basscut*/
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/*===========================================================================

FUNCTION calculateNumBassCut

DESCRIPTION
  This function calculates Numerator coeff. for BassCut
  B(0)= (1-sqrt(2)K+K^2)/DEN
  B(1)= 2*(K^2-1)/DEN
  B(2)= (1-sqrt(2)*K +K^2)/DEN

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
int calculateNumBassCut(int V0, int K, int *numerator, int DEN, int denSR)
{
    long  KSqrQ28;
    int B0Q28,B1Q28, B2Q28, SR;
    int B0Q30, B1Q30, B2Q30;
    long KSqr2Q28;
	/* calculate KsqrQ28 = q28  */
    KSqrQ28 = mul32x32Shift2(K, K, 4);
	
    /* sqrt(2)*K = q28 */
    KSqr2Q28=mul32x32Shift2(SQRT2Q30,K,2);
    
    /* Calculate B(0) = (1+sqrt(2)*K+K^2)*/
	/* B0 in Q 28						*/
    B0Q28 = ONEQ28 +KSqr2Q28+KSqrQ28;
    
	/* Calculate B(1) = 2*(K^2-1) */
	B1Q28=(KSqrQ28- ONEQ28 )<<1;

	/* Calculate B(2)= (1-sqrt(2)*K +K^2)*/
	B2Q28 = ONEQ28 -KSqr2Q28+KSqrQ28;
    
    /* divide by DEN */
    /* B1Q30 = qSR */
    dsplib_approx_divide(B0Q28, DEN ,&B0Q30, &SR);
	SR=SR-denSR;
    /* Convert B3Q30 to Q30 */
    if (SR > 30)
        B0Q30 = B0Q30 >> (SR-30);
    else
        B0Q30 = B0Q30 << (30-SR);
    dsplib_approx_divide(B1Q28, DEN ,&B1Q30, &SR);
	SR=SR-denSR;
    /* Convert B1Q30 to Q30 */
    if (SR > 30)
        B1Q30 = B1Q30 >> (SR-30);
    else
        B1Q30 = B1Q30 << (30-SR);
    dsplib_approx_divide(B2Q28, DEN ,&B2Q30, &SR);
	SR=SR-denSR;
    /* Convert B2Q30 to Q30 */
    if (SR > 30) 
        B2Q30 = B2Q30 >> (SR-30);
    else 
        B2Q30 = B2Q30 << (30-SR);
    numerator[0] = B0Q30;
    numerator[1] = B1Q30;
    numerator[2] = B2Q30;
    return 1;
} /* end of bass cut num calculations.*/
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/*===========================================================================

FUNCTION calculateDenBassCut

DESCRIPTION
  This function calculates denominator coeff. for BassCut
  A(1)= 2*(V0*K^2-1)/DEN
  A(2)= (1-sqrt(2*V0)K+V0*k^2)/DEN

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
int calculateDenBassCut(int K, int *denomenator, int DEN,int Ksqrt2V0,int V0,int denSR)
{
    long  KSqrQ28;
    int A1Q28, A2Q28, SR1,SR2,SR;
    int A0Q30, A1Q30, A2Q30;
	int temp;
	int64 temp2,temp3;

    // assume KsqrQ28 = q28 
    KSqrQ28 = mul32x32Shift2(K, K, 4);
    /* Calculate A(1) = (2*(V0*K^2-1)), q28 */
	/* temp = V0*K*K, Q28 */
    temp = mul32x32Shift2(V0,KSqrQ28,2); // temp in Q26
	temp2 = (int64)temp<<2;			 // temp2 = V0*K^2 in Q28
	temp3 = (int64)(temp2-(int64)ONEQ28 )*2; // temp3 = 2*(V0K^2-1)


	qdsp_norm64(temp3 ,&A1Q28,&SR1);


    /* Calculate A(2) = ( 1- sqrt(2*V0)K + V0*K^2 ) */
	temp3 = (int64)ONEQ28 - (int64)Ksqrt2V0 + (int64)temp2;
    qdsp_norm64(temp3 ,&A2Q28,&SR2);
   /*  TEST */

    /* divide by DEN */
    A0Q30 = 1 << 30;
    dsplib_approx_divide(A1Q28, DEN ,&A1Q30, &SR);
	SR = SR+SR1-denSR;
    if (SR>30) 
        A1Q30 = A1Q30 >> (SR-30);
    else 
        A1Q30 = A1Q30 << (30-SR);
    dsplib_approx_divide(A2Q28, DEN ,&A2Q30, &SR);
	SR = SR+SR2-denSR;
   if (SR>30) 
        A2Q30 = A2Q30 >> (SR-30);
    else 
        A2Q30 = A2Q30 << (30-SR);
    denomenator[0]=A0Q30;
    denomenator[1]=A1Q30;
    denomenator[2]=A2Q30;
    return 1;
}/*end of basscut den coefs calc*/
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/*===========================================================================

FUNCTION calculatebandCut

DESCRIPTION
  This function calculate the coefficients of bandCut
  K=Wc/2. Wc=28pi*fc/fs
  V0 = 10^(gain/20)
  DEN = 1+V0*k/Q+k^2
  B(0)= (1+K/Q+K^2)/DEN
  B(1)= 2*(K^2-1)/DEN
  B(2)= (1-K/Q +K^2)/DEN
  A(1)= 2*(K^2-1)/DEN
  A(2)= (1-V0*K/Q+V0*k^2)/DEN

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
void calculatebandCut(int V0, int fc, int fs, int que, int *bandNum, int *bandDen)
{
	int DEN, K,KbyQ,denSR;
	/* Calcualte K*/
	/* Themax value of K is pi/2 = 1.5708*/
	/* hence the Q factor can be Q2.30 (unsigned)*/

	/* calculate V0, V0 in Q28*/
	V0=V0LinearTable[(int)V0+GAIN_INDEX_OFFSET];
	
	/* calculate DEN, returns K,KbyQ & DEN in Q28.*/
	DEN=calculatebandCutDEN(fc, fs, V0,que,&K, &KbyQ,&denSR);
	
	/* calculate the numerator coeffs*/
    calculateNumbandCut(V0, K, bandNum, DEN,KbyQ,denSR);
	/* calculate the denominator coeffs*/
	//int calculateDenbandCut(int K, int *denomenator, int DEN,int Ksqrt2V0,int V0,int KbyQ28)
    calculateDenbandCut(K, bandDen, DEN,V0,KbyQ,denSR);
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/*===========================================================================

FUNCTION calculatebandCutDEN

DESCRIPTION
  This function calculates K and DEN for given filter uses dsp_approx_divide

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
int calculatebandCutDEN(int cutoffQ0, int SamplingFreqQ0,int V0, int queQ0,int* KQ28,int *KbyQ28,int *denSR)
{
    int SR, K, x, y;
	int invque,queSR;
	int64 res;
	int norm_den;
	//long V0double,V0sqrt;
    //sfQ15 = SamplingFreqQ0 << 15;
    /* output = K with qfactor = SR */
    dsplib_approx_divide(cutoffQ0, SamplingFreqQ0, &K, &SR);

    /* Convert K to Q31 */
    if (SR > 31) {
        K = K >> (SR-31);
    } else {
        K = K << (31-SR);
    }

    /* Multiply K by pi */
    /* K = q31+q29-q32+q0= Q28 */
    K = mul32x32Shift2(K,PIQ29,0);

	/*calculate 1/Q. output: invque with qfactor=SR*/
	dsplib_approx_divide(1, queQ0, &invque, &queSR);

	queSR = queSR -8;	// compensating for Q factor of the Q 
    /* Convert Q to Q31 */
    if (queSR > 31) {
        invque = invque >> (queSR-31);
    } else {
        invque = invque << (31-queSR);
    }
    /* Calculate DEN = DEN = 1+V0*k/Q+k^2; */
    /* x= K/Q, Q28+Q31-Q32+Q1=Q28*/    
    x = mul32x32Shift2(K,invque,1);
	*KbyQ28=x;
	/* x= V0*K/Q Q28       */
	x = mul32x32Shift2(V0,x,0);
	res = (int64)x<<4;
		/* y = K*K, q28 */
    y = mul32x32Shift2(K,K,4);
    res =(int64)ONEQ28 +(int64)res+(int64)y;
	qdsp_norm64(res,&norm_den,denSR);
	
    *KQ28 = K;
    return norm_den;
} /* End of function to calculate DEN for bandcut*/
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/*===========================================================================

FUNCTION calculateNumbandCut

DESCRIPTION
  This functionn calculates Numerator coeff. for bandCut
  B(0)= (1+K/Q+K^2)/DEN
  B(1)= 2*(K^2-1)/DEN
  B(2)= (1-K/Q +K^2)/DEN

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
int calculateNumbandCut(int V0, int K, int *numerator, int DEN, int KbyQ28,int denSR)
{
    long   KSqrQ28;
    int B0Q28,B1Q28, B2Q28, SR;
    int B0Q30, B1Q30, B2Q30;
    
	/* calculate KsqrQ28 = q28  */
    KSqrQ28 = mul32x32Shift2(K, K, 4);
	
    
    /* Calculate B(0) = (1+K/Q+K^2)*/
	/* B0 in Q 28						*/
    B0Q28 = ONEQ28 +KbyQ28+KSqrQ28;
    
	/* Calculate B(1) = 2*(K^2-1) */
	B1Q28=(KSqrQ28- ONEQ28 )<<1;

	/* Calculate B(2)= (1-K/Q +K^2)*/
	B2Q28 =  ONEQ28 -KbyQ28+KSqrQ28;
    
    /* divide by DEN */
    /* B1Q30 = qSR */
    dsplib_approx_divide(B0Q28, DEN ,&B0Q30, &SR);
	SR=SR-denSR;
    /* Convert B3Q30 to Q30 */
    if (SR > 30)
        B0Q30 = B0Q30 >> (SR-30);
    else
        B0Q30 = B0Q30 << (30-SR);
    dsplib_approx_divide(B1Q28, DEN ,&B1Q30, &SR);
	SR=SR-denSR;
    /* Convert B1Q30 to Q30 */
    if (SR > 30)
        B1Q30 = B1Q30 >> (SR-30);
    else
        B1Q30 = B1Q30 << (30-SR);
    dsplib_approx_divide(B2Q28, DEN ,&B2Q30, &SR);
	SR=SR-denSR;
    /* Convert B2Q30 to Q30 */
    if (SR > 30) 
        B2Q30 = B2Q30 >> (SR-30);
    else 
        B2Q30 = B2Q30 << (30-SR);
    numerator[0] = B0Q30;
    numerator[1] = B1Q30;
    numerator[2] = B2Q30;
    return 1;
} /* end of band cut num calculations.*/
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
/*===========================================================================

FUNCTION calculateDenbandCut

DESCRIPTION
  This function calculates denominator coeff. for bandCut
  A(1)= 2*(V0*K^2-1)/DEN
  A(2)= (1-sqrt(2*V0)K+V0*k^2)/DEN

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
int calculateDenbandCut(int K, int *denomenator, int DEN,int V0,int KbyQ28,int denSR)
{
    long  KSqrQ28,temp;
    int A1Q28, A2Q28, SR;
    int A0Q30, A1Q30, A2Q30;

    // assume KsqrQ28 = q28 
    KSqrQ28 = mul32x32Shift2(K, K, 4);
    /* Calculate A(1) = (2*(K^2-1)), q28 */
	
	A1Q28 = (KSqrQ28- ONEQ28 )*2;
    /* Calculate A(2) = ( 1- V0*K/Q + V0*K^2 ) */
	/* temp = V0*K/Q, Q28*/
	temp = mul32x32Shift2(V0,KbyQ28,4);
    A2Q28 =  ONEQ28  - temp + KSqrQ28;
   /*  TEST */

    /* divide by DEN */
    A0Q30 = 1 << 30;
    dsplib_approx_divide(A1Q28, DEN ,&A1Q30, &SR);
	SR=SR-denSR;
    if (SR>30) 
        A1Q30 = A1Q30 >> (SR-30);
    else 
        A1Q30 = A1Q30 << (30-SR);
    dsplib_approx_divide(A2Q28, DEN ,&A2Q30, &SR);
	SR=SR-denSR;
   if (SR>30) 
        A2Q30 = A2Q30 >> (SR-30);
    else 
        A2Q30 = A2Q30 << (30-SR);
    denomenator[0]=A0Q30;
    denomenator[1]=A1Q30;
    denomenator[2]=A2Q30;
    return 1;
}/*end of bandcut den coefs calc*/
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
/*===========================================================================

FUNCTION calculateTrebleboost

DESCRIPTION
  This function calculate the coefficients of Trebleboost
  K=Wc/2. Wc=28pi*fc/fs
  V0 = 10^(gain/20)
  DEN = (1+sqrt(2)*K +K^2)
  B(0)= (V0+sqrt(2*V0)K+K^2)/DEN
  B(1)= 2*(K^2-V0)/DEN
  B(2)= (V0-sqrt(2V0)*k+k^2)/DEN
  A(1)= 2*(K^2-1)/DEN
  A(2)= (1-sqrt(2)K+k^2)/DEN

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
void calculateTrebleboost(int V0, int fc, int fs, int *bassNum, int *bassDen, int *shift)
{
	int DEN, K;
	/* Calcualte K*/
	/* Themax value of K is pi/2 = 1.5708*/
	/* hence the Q factor can be Q2.30 (unsigned)*/

	/* calculate V0, V0 in Q28*/
	V0=V0LinearTable[(int)V0+GAIN_INDEX_OFFSET];
	
	/* calculate DEN, returns K,KbyQ & DEN in Q28.*/
	//DEN=calculateTrebleboostDEN(fc, fs, V0,&K);
	// trebleboost and baseboost use the same DEN
	DEN=calculateBoostDEN(fc, fs,&K);
	
	/* calculate the numerator coeffs*/
    *shift= calculateNumTrebleboost(V0, K, bassNum, DEN);
	/* calculate the denominator coeffs*/
    calculateDenTrebleboost(K, bassDen, DEN);
	
 }
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
/*===========================================================================

FUNCTION calculateNumTrebleboost

DESCRIPTION
  This function calculates Numerator coeff. for Trebleboost
  B(0)= (1-sqrt(2)K+K^2)/DEN
  B(1)= 2*(K^2-1)/DEN
  B(2)= (1+sqrt(2)*K +K^2)/DEN

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
int calculateNumTrebleboost(int V0, int K, int *numerator, int DEN)
{
    long  KSqrQ28;
    int B0Q28,B1Q28, B2Q28,SR1,SR2,SR3;
    int B0Q30, B1Q30, B2Q30;
	int Smax;
	int x;
	int64 den;
	long V0double,V0sqrt;
	/* calculate KsqrQ28 = q28  */
    KSqrQ28 = mul32x32Shift2(K, K, 4);
	
    /* sqrt(2)*K = q28 */
    mul32x32Shift2(SQRT2Q30,K,2);


    /* v0DOUBLE = Q28 */
    V0double = V0;
    /* Take Sqrt of V0; V0sqrt = q14 */
    V0sqrt=sqrtfp(V0double); 
    /* V0sqrt = q28 */
    V0sqrt = V0sqrt << 14;
    /* sqrt(2*V0) = sqrt(2)*sqrt(v0), q28 */
    V0sqrt=mul32x32Shift2(V0sqrt,SQRT2Q30,2);


    /* Calculate DEN = DEN = V0+sqrt(2*V0)k+k^2; */
    /* x= K *sqrt(2*V0), Q28+Q28-Q32+Q4=Q28*/    
    x = mul32x32Shift2(K,V0sqrt,4);
    den=(int64)V0+(int64)x+(int64)KSqrQ28;
	den=(int64)den>>1;
    /* Calculate B(0) = (1-sqrt(2)K+K^2)*/
	/* B0 in Q 27						*/
    B0Q28 = (int)den;
    
	/* Calculate B(1) = 2*(K^2-1),Q27 */
	B1Q28=(KSqrQ28-V0);

	/* Calculate B(2)= (1+sqrt(2)*K +K^2))*/
	B2Q28 = V0-x+KSqrQ28;
    
    /* divide by DEN */
    /* B1Q30 = qSR */
    dsplib_approx_divide(B0Q28, DEN ,&B0Q30, &SR1);
    /* Convert B3Q30 to Q30 */
    dsplib_approx_divide(B1Q28, DEN ,&B1Q30, &SR2);

	dsplib_approx_divide(B2Q28, DEN ,&B2Q30, &SR3);

	// find the maximum SR of the three
	//Smax =(SR1>SR2) ? SR1 : SR2;
	//Smax =(Smax>SR3) ? Smax : SR3;
	SR1=SR1-1;
	SR2=SR2-1;
	Smax=SR1;
	if(Smax>SR2) Smax=SR2;
	if(Smax>SR2) Smax=SR3;
    numerator[0] = B0Q30>>(SR1-Smax); // to compensate for Q27 of B0,B1
    numerator[1] = B1Q30>>(SR2-Smax);
    numerator[2] = B2Q30>>(SR3-Smax);

    return Smax;
} /* end of band boost num calculations.*/
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
/*===========================================================================

FUNCTION calculateDenTrebleboost

DESCRIPTION
  This function calculates denominator coeff. for Trebleboost
  A(1)= 2*(K^2-V0)/DEN
  A(2)= (V0-sqrt(2*V0)K+k^2)/DEN

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
int calculateDenTrebleboost(int K, int *denomenator, int DEN)
{
    long  KSqrQ28,Ksqrt2Q28;
    int A1Q28, A2Q28, SR;
    int A0Q30, A1Q30, A2Q30;

    // assume KsqrQ28 = q28 
    KSqrQ28 = mul32x32Shift2(K, K, 4);
    /* Calculate A(2) = (2*(K^2-1)), q27 */
    A1Q28 = (KSqrQ28- ONEQ28 )*2;
    /* Calculate A(2) = temp1 = ( 1- sqrt(2)K + K^2 ) */
	Ksqrt2Q28 = mul32x32Shift2(K,SQRT2Q30,2);
    A2Q28 =  ONEQ28  - Ksqrt2Q28 + KSqrQ28;
   /*  TEST */

    /* divide by DEN */
    A0Q30 = 1 << 30;
    dsplib_approx_divide(A1Q28, DEN ,&A1Q30, &SR);
    if (SR>30) 
        A1Q30 = A1Q30 >> (SR-30);
    else 
        A1Q30 = A1Q30 << (30-SR);
    dsplib_approx_divide(A2Q28, DEN ,&A2Q30, &SR);
   if (SR>30) 
        A2Q30 = A2Q30 >> (SR-30);
    else 
        A2Q30 = A2Q30 << (30-SR);
    denomenator[0]=A0Q30;
    denomenator[1]=A1Q30;
    denomenator[2]=A2Q30;
    return 1;
}/*end of band boost den coefs calc*/
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/*===========================================================================

FUNCTION sqrfp

DESCRIPTION
  This function 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
int sqrtfp(long a)
{
    short i;
    long y=0;
    for(i=15;i>=0;i--) {
        if(a-(y+(2<<(i-1)))*(y+(2<<(i-1))) >= 0) {
            y+=(2 << (i-1));
		}
            
    }
    return y;
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
/*===========================================================================

FUNCTION mul32x32Shift2

DESCRIPTION
  This function multiply two 32-bit numbers, shiftLeft 
  if x = q20, y = q20, shift = 0 => result = q20+20-32+0 = q8 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
long mul32x32Shift2(int x, int y, unsigned short shift)
{
    short isNeg = 0;
    unsigned short xh, xl, yh, yl;
  unsigned int result;
  uint64 res64, resm;

    if (x < 0) {
        x = -x;
        isNeg = ~isNeg;
    }
    if (y < 0) {
        y = -y;
        isNeg = ~isNeg;
    }
	xh = (unsigned short)( x >> 16 );
    xl = (unsigned short)( x & 0x0000FFFF );
    yh = (unsigned short)( y >> 16 );
    yl = (unsigned short)( y & 0x0000FFFF );

	res64 = (uint64)((xl*yl)+0x8000);
	res64 = res64 &(int64)(0x0ffffffff);
	res64 = (uint64)(res64>>(16-shift));
	
	
	resm  = (uint64)((uint64)xh*yl+(uint64)xl*yh)<<shift;
	res64 = (uint64)(res64+resm);
	res64 = res64>>16;
	
	resm  = (uint64)(xh*yh)<<shift;
	res64 = res64+resm;
	result = (unsigned int)res64;
	return ( isNeg ? - ( (long)result ) : (long)result );
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
/*===========================================================================

FUNCTION dsplib_approx_divide

DESCRIPTION
  This function divicde

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
int dsplib_approx_divide(int numer,int denom,int *result,int *shift_factor)
{
  int norm_num,r,s_d,s_n;
    
/*using the other one*/  
  //dsplib_approx_invert(denom,&r,&s_d);
  dsplib_taylor_invert(denom,&r,&s_d,21);
  s_d=s_d+1;
  qdsp_norm(numer,&norm_num,&s_n);
  *shift_factor = s_n - s_d;

  *result = mul32x32Shift2(norm_num,r,1);

  return 1;
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
/*===========================================================================

FUNCTION dsplib_taylor_invert

DESCRIPTION
  This function inverse of a number using Taylor's series expansion
  1/x = 1+y+y^2+...+y^iters, where y=1-x.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
int dsplib_taylor_invert(int input,int *res,int *shift_factor,int iters)
{
 int norm_divisor;
 int r;
 int y;
 short i;
 unsigned long result=0;
 int oneQ30=1<<30;

 /* bit-align-normalize input */
  /* output wd always be in 0.5 to 1 */
  qdsp_norm(input, &norm_divisor, &r);

   /* calculate y in Q30*/
   y=(int)0x40000000 - (norm_divisor>>1);
   result = oneQ30+y; /*result = 1+y */
   for(i=0;i<iters;i++)
   {
	   /*result = 1+y*result  */
	   result=oneQ30+mul32x32Shift2(y,result,2);
   }

 *res=result;
 *shift_factor=r-31;

return(1);
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/*===========================================================================

FUNCTION qdsp_norm

DESCRIPTION
  This function calculates the norm in 32bits

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
int qdsp_norm(int input,int *output, int *shift_factor) 
{
  int sf = 0;
  if (input)
  { 
    /* non-zero input, shift left until "normed" */
    while (((input << 1) ^ ~input) & 0x80000000)
    {
      input <<= 1;
      sf++;
    }
    *output       = input;
    *shift_factor = sf;
  } 
  else
  {
    /* zero input, leave as zero */
    *output       = 0;
    *shift_factor = 0;
  }
  return 1;
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/*===========================================================================

FUNCTION qdsp_norm64

DESCRIPTION
  This function calculates the norm in 64bits.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
void qdsp_norm64(int64 inp,int *norm_inp,int *shift)
{
  int sf = 0;
  int64 inp1;
  inp1 = (int64)(inp>>16);
  if (inp)
  { 
    /* non-zero input, shift left until "normed" */
    /* while (((inp << 1) ^ ~inp) & (int64)0x8000000000000000)
    ** Fixed compiling warnings 
    */
    while ((((inp << 1) ^ (~inp)) >> 63 ) & 0x1)
    {
      inp <<= 1;
      sf++;
    }
	inp1 = (int64)(inp>>32)&0xffffffff;
    *norm_inp= (int)(inp1);
    *shift = sf-32;
  } 
  else
  {
    /* zero input, leave as zero */
    *norm_inp      = 0;
    *shift = 0;
  }

}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
