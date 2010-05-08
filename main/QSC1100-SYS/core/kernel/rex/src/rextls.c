/*======================================================

FILE:      rex_tls.c

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
#include "customer.h"

#ifndef FEATURE_SI_STACK_WM

#include "rextls.h"

static int g_next_tls_key = 0;

int rex_tls_create(int *pkey)
{
   *pkey = g_next_tls_key++;

   return 0;
}

int rex_tls_set(int key, void *val)
{
   if (key >= g_next_tls_key) {
      return -1;
   }

   *rex_tls_ptr(key) = val;

   return 0;
}

int rex_tls_get(int key, void **pval)
{
   if (key >= g_next_tls_key) {
      return -1;
   }

   *pval = *rex_tls_ptr(key);

   return 0;
}

#endif
