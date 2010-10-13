/*======================================================

FILE:      rex_tls.h

SERVICES:  possible implementation of tls on rex

GENERAL DESCRIPTION:

  This file allocates tls keys and uses the rex 
    task stack to store indexed void *
   
PUBLIC CLASSES AND STATIC FUNCTIONS:

    rex_tls_create, rex_tls_set, rex_tls_get

INITIALIZATION & SEQUENCING REQUIREMENTS:

	This implementation is incompatible with FEATURE_REX_SWM

        Copyright © 2004 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/
#ifndef REX_TLS_H
#define REX_TLS_H /* #ifndef REX_TLS_H */

#include "customer.h"
#include "rex.h"

#ifndef FEATURE_SI_STACK_WM

int rex_tls_create(int *pkey);
int rex_tls_set(int key, void *val);
int rex_tls_get(int key, void **pval);

static __inline void **rex_tls_ptr(int key)
{
   return ((void **)(rex_self()->stack_limit))+key;
}


#endif /* #ifndef FEATURE_SI_STACK_WM */

#endif /* #ifndef REX_TLS_H */

