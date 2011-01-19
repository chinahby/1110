#ifndef HMAC_H
#define HMAC_H
/*
=======================================================================

FILE:         hmac.h

SERVICES:     @@@TODO: fill me in!

DESCRIPTION:  @@@TODO: fill me in!

=======================================================================
        Copyright © 2005 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=======================================================================
*/

typedef struct _HMacData {
   unsigned char bInit;
   unsigned char bIsHMac;
   unsigned char abOPad[65];
} HMacData;

#endif /* #ifndef HMAC_H */

