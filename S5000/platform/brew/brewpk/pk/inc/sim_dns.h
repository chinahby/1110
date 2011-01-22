/*===========================================================================
                    IMPORTANT NOTICE

                  OEM DISTRIBUTION ONLY

FILE:  sim_dns.h

SERVICES:  

GENERAL DESCRIPTION:

        Copyright 1999-2002 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/
#ifndef _SIM_DNS_H_
#define _SIM_DNS_H_

#include <windows.h>
#include "AEE.h"
#include "AEENet.h"
#include "AEEComdef.h"

typedef struct SimDNS {
   boolean bSpecifyDNS;
   INAddr dnsIP1;
   INAddr dnsIP2;
} SimDNS;


#ifdef __cplusplus
extern "C" {
#endif

int  GetSysDNS( INAddr * aa, int naaSize );
void SimDNS_SetParams( SimDNS *dns );

#ifdef __cplusplus
}
#endif


#endif  //_SIM_DNS_H_
