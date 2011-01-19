#ifndef AEEADDRBOOKUTIL_H
#define AEEADDRBOOKUTIL_H

/*============================================================================
FILE:	AEEAddrBookUtil.h

SERVICES: IAddrBook utility functions

DESCRIPTION:
   This file defines utility functions which may be used to manage
   address fields of type AEEDB_FT_COMPLEX.
   
PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  Not Applicable

        Copyright © 1999-2004 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
============================================================================*/

#include "AEEAddrBook.h"
AEEAddrFieldComplex * AEEAddrFieldComplex_New( void );
void AEEAddrFieldComplex_Dtor( AEEAddrFieldComplex *pfc );
int AEEAddrFieldComplex_GetSubFieldCount( const AEEAddrFieldComplex *pfc );
uint32 AEEAddrFieldComplex_GetSize( const AEEAddrFieldComplex *pfc );
int AEEAddrFieldComplex_AddSubField( AEEAddrFieldComplex **pfc, const AEEAddrField *paf );
int AEEAddrFieldComplex_GetSubField( const AEEAddrFieldComplex *pfc, int nFieldIdx, AEEAddrField *paf );
int AEEAddrFieldComplex_RemoveSubField( AEEAddrFieldComplex **pfc, int nFieldIdx, uint32 *nBytes );

#endif // AEEADDRBOOKUTIL_H
