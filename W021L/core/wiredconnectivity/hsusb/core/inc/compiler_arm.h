/************************************************************************** 
*************************   Restricted access   *************************** 
*************************************************************************** 












This file must only be used for the development of the HSUSB 
driver for the AMSS / BREW SW baselines using the Jungo USB Stack.
This file must not be used in any way for the development of any
functionally equivalent USB driver not using the Jungo USB stack.

For any questions please contact: Sergio Kolor, Liron Manor,
Yoram Rimoni, Dedy Lansky.











************************************************************************** 
*************************   Restricted access   ************************** 
**************************************************************************/ 

/* Jungo Confidential, Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com */
#ifndef _COMPILER_ARM_H_
#define _COMPILER_ARM_H_

/* Global mandatory definitions */
#define __RESTRICT__
#define __VOLATILE__ volatile

/* HSU Temp fix - compilation error for 6290 (redefenition on compiler_cl.h) */
#ifndef __INLINE__
#define __INLINE__ __inline
#endif /*__INLINE__*/
/* End of HSU Temp fix - compilation error for 6290 (redefenition on compiler_cl.h) */

#define __PACKED_PRE__ __packed
#define __PACKED_POST__ 

#define __FILE_NAME__ __MODULE__
/* Special definitions */
//#pragma anon_unions

#endif

