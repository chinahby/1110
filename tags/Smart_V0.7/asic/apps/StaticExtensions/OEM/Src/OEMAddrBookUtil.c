/* =========================================================================

FILE: AEEAddrBookUtil.c

SERVICES: IAddrBook utility functions

DESCRIPTION
  This file contains IAddrBook related utility functions that apps can use
  to manage field types of AEEDB_FT_COMPLEX.

PUBLIC CLASSES:  
   N/A

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A


           Copyright ©2000-2004 QUALCOMM Incorporated.
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
  ========================================================================= */


/* =========================================================================
                     INCLUDES AND VARIABLE DEFINITIONS
   ========================================================================= */

#include "AEEStdLib.h"
#include "AEEAddrBook.h"
#include "AEEAddrBookUtil.h"
#include "AEE.h"
/* =========================================================================

                      PUBLIC DATA DECLARATIONS

   ========================================================================= */

#if !defined (offset)
#define offset(field, type)	(((char*)(&((type *)4)->field))-((char*)4))
#endif

// Macro to compute the number of bytes needed to align a pointer to a 4-byte
// boundary.
#define CALC_PAD_BYTES(p)  ((4 - ((uint32)(p) & 3)) & 3)

/* =========================================================================
                     FUNCTION DEFINITIONS
   ========================================================================= */

AEEAddrFieldComplex * AEEAddrFieldComplex_New( void )
{
   AEEAddrFieldComplex * pfc;

   pfc = MALLOC(sizeof(AEEAddrFieldComplex));

   if( NULL != pfc )
   {
      pfc->wNumSubfields = 0;
      pfc->byVersion = 0;
      pfc->byReserved = 0;
   }
   return pfc;
}

void AEEAddrFieldComplex_Dtor( AEEAddrFieldComplex *pfc )
{
   FREE((void *)pfc);
}


int AEEAddrFieldComplex_GetSubFieldCount( const AEEAddrFieldComplex *pfc )
{
   return (int32)pfc->wNumSubfields;
}

uint32 AEEAddrFieldComplex_GetSize( const AEEAddrFieldComplex *pfc )
{
   int nBytesTotal;
   int nBytesThisSubfield;
   int nSubfields;
   AEEAddrSubfield *pSubfield;
   int nPadBytes;

   // Initial size is the header information
   nBytesTotal = offset(buf, AEEAddrFieldComplex);

   nSubfields = pfc->wNumSubfields;
   pSubfield = (AEEAddrSubfield *)pfc->buf;  //lint !e826 area too small

   // Now iterate through the subfields and add up the size of each one
   while( nSubfields-- )
   {
      nPadBytes = CALC_PAD_BYTES(pSubfield->buf);
      nBytesThisSubfield = pSubfield->wDataLen + offset(buf, AEEAddrSubfield) + nPadBytes;
      pSubfield = (AEEAddrSubfield *)((byte *)pSubfield + nBytesThisSubfield); //lint !e826 area too small
      nBytesTotal += nBytesThisSubfield;
   }

   return (uint32)nBytesTotal;
}

int AEEAddrFieldComplex_AddSubField( AEEAddrFieldComplex **pfc, const AEEAddrField *paf )
{
   uint32 nBytesOrig;
   uint32 nBytesNew;
   uint32 nPadBytes;
   AEEAddrFieldComplex *pNew;
   AEEAddrSubfield *pSubfield;

   // Compute the new buffer size to hold the existing complex field plus the
   // new subfield.  Then, realloc the buffer
   nBytesOrig = AEEAddrFieldComplex_GetSize(*pfc);
   nPadBytes = CALC_PAD_BYTES(nBytesOrig + offset(buf, AEEAddrSubfield));
   nBytesNew = nBytesOrig + paf->wDataLen + offset(buf, AEEAddrSubfield) + nPadBytes;
   pNew = REALLOC((void *)(*pfc), nBytesNew);

   // If the realloc was successful, copy the new subfield data into the end
   // of the complex field.
   if( pNew )
   {
      *pfc = pNew;

      pSubfield = (AEEAddrSubfield *)((byte *)pNew + nBytesOrig);  //lint !e826 area too small
      pSubfield->fID = paf->fID;
      pSubfield->wDataLen = paf->wDataLen;
      pSubfield->fType = paf->fType;
      (void)MEMCPY( ((unsigned char *)pSubfield->buf) + nPadBytes, paf->pBuffer, paf->wDataLen );

      (*pfc)->wNumSubfields++;
   }
   else
   {
      return ENOMEMORY;
   }
   
   return AEE_SUCCESS;
}

static AEEAddrSubfield * GetSubField( const AEEAddrFieldComplex *pfc, int nFieldIdx)
{
   AEEAddrSubfield *pSubfield;
   uint32 nPadBytes;

   // Verify the field index is valid
   if( nFieldIdx >= 0 && nFieldIdx < pfc->wNumSubfields )
   {
      // Start with the first subfield
      pSubfield = (AEEAddrSubfield *)pfc->buf; //lint !e826 area too small
      
      // Iterate through the subfields until we hit the desired index
      while( nFieldIdx-- )
      {
         nPadBytes = CALC_PAD_BYTES(pSubfield->buf);
         pSubfield = (AEEAddrSubfield *)((byte *)pSubfield + pSubfield->wDataLen 
                                         + offset(buf, AEEAddrSubfield) + nPadBytes); //lint !e826 area too small
      }

      return pSubfield;
   }

   return NULL;
}

int AEEAddrFieldComplex_GetSubField( const AEEAddrFieldComplex *pfc, int nFieldIdx, AEEAddrField *paf )
{
   AEEAddrSubfield *pSubfield;
   uint32 nPadBytes;

   pSubfield = GetSubField(pfc, nFieldIdx);

   if( pSubfield )
   {
      // Copy the subfield data back to the passed AEEAddrField struct
      nPadBytes = CALC_PAD_BYTES(pSubfield->buf);
      paf->fID = pSubfield->fID;
      paf->fType = pSubfield->fType;
      paf->wDataLen = pSubfield->wDataLen;
      paf->pBuffer = ((unsigned char *)pSubfield->buf) + nPadBytes;

      return AEE_SUCCESS;
   }

   return EBADPARM;
}


int AEEAddrFieldComplex_RemoveSubField( AEEAddrFieldComplex **pfc, int nFieldIdx, uint32 *pnBytes )
{
   AEEAddrFieldComplex *pfcNew;
   AEEAddrField af;
   int i;
   int ret;

   // Verify the field index is valid
   if( nFieldIdx >= 0 && nFieldIdx < (*pfc)->wNumSubfields )
   {
      // Create a new complex field
      pfcNew = AEEAddrFieldComplex_New();
      if( pfcNew )
      {
         // Now iterate through the subfields and add each one to the new complex
         // field we just created, skipping the one we want to delete.
         for( i = 0; i < (*pfc)->wNumSubfields; i++ )
         {
            if( i != nFieldIdx )
            {
               ret = AEEAddrFieldComplex_GetSubField(*pfc, i, &af);
               if( ret != AEE_SUCCESS )
               {
                  AEEAddrFieldComplex_Dtor(pfcNew);
                  return ret;
               }
               ret = AEEAddrFieldComplex_AddSubField(&pfcNew, &af);
               if( ret != AEE_SUCCESS )
               {
                  AEEAddrFieldComplex_Dtor(pfcNew);
                  return ret;
               }
            }
         }

         // Successfully copied the subfields.  Free the original complex field.
         AEEAddrFieldComplex_Dtor(*pfc);

         // Write the address of the new complex field back into the in/out pfc
         // parameter.
         *pfc = pfcNew;

         // If the caller provided a non-zero pointer to a number of bytes, 
         // return the new total size of the complex field.
         if( pnBytes )
         {
            *pnBytes = AEEAddrFieldComplex_GetSize(*pfc);
         }
         return AEE_SUCCESS;
      }
      return ENOMEMORY;
   }
   return EBADPARM;
}
