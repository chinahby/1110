/*===========================================================================
                              OEMPBM.c

  OEM CONFIG CONTROL ROUTINES

  This file contains a reference implementation of a OEM PBM
  control interfaces.

        Copyright © 2007-2008 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
============================================================================*/

/*=============================================================================

$Header: //depot/asic/msmshared/apps/StaticExtensions/OEM/Src/OEMPBM.c#2 $
$DateTime: 2008/04/09 08:26:06 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/09/08   cvs     Fix compile error
11/29/07   clm     Create file.

=============================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "AEE_OEMHeap.h"
#include "AEEStdLib.h"
#include "OEMPBM.h"
#include "AEECallHistoryExt.h"
#include "pbmlib.h"

#include "msg.h"
#include "err.h"
#include "AEEDB.h"
/*===========================================================================

                    DEFINITIONS AND CONSTANTS

===========================================================================*/

//Set the maximum number of unicode characters for the name associated
//with a call
#define CH_MAX_NAME  64

/*==========================================================================

                       LOCAL/STATIC FUNCTION PROTOTYPES

===========================================================================*/
static int OEMPBM_BuildAddrFields(const AEECallHistoryEntry *pEntry,
                                  uint8 **pFieldBuf,
                                  uint32 *pFieldBufSize,
                                  int *pnFields,
                                  pbm_device_type *pb_id);
static AEEDBFieldType OEMPBM_CallHistoryIDtoType(uint16 wID);
static pbm_field_id_e_type OEMPBM_CallHistoryIDtoAddrFieldID(uint16 wID);
static pbm_tech_type_e_type OEMPBM_CallHistoryTechTypetoPBMTechType(uint16 wID);


/*=============================================================================
FUNCTION: OEMPBM_AddEntry

DESCRIPTION:
   Translate and add AEE Call History Entry to pbm.

PARAMETERS:
   pEntry      [in]: Pointer to AEECallHistoryEntry

RETURN VALUE:
   BREW Error Code

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
int OEMPBM_AddEntry(const AEECallHistoryEntry *pEntry)
{
   uint8            *fields_buf;
   uint32            fields_buf_size;
   pbm_cat_e_type    wCat = PBM_CAT_NONE;
   uint16            wRecID = PBM_GET_FIRST_AVAILABLE;
   pbm_device_type   pb_id = PBM_DEFAULT;
   pbm_return_type   pbmRet;
   int               nFields;
   int               ret;

   if ((ret = OEMPBM_BuildAddrFields(pEntry, &fields_buf, &fields_buf_size, 
                                     &nFields, &pb_id)) != SUCCESS) {
      ERR("BuildAddrFields() failed.", 0, 0, 0);
      return ret;
   }

   pbmRet = pbm_record_write(pb_id, &wRecID, (uint16)wCat, nFields, fields_buf,
                             fields_buf_size, NULL, NULL);
   pbm_free_fields(&fields_buf,nFields);
   if (pbmRet != PBM_SUCCESS) {
      ERR("pbm_write() failed with error %d.", pbmRet, 0, 0);
      return EFAILED;
   }

   return SUCCESS;
}

/*=============================================================================
FUNCTION: OEMPBM_Clear

DESCRIPTION:
   Clears pbm call history.

PARAMETERS:
   None

RETURN VALUE:
   BREW Error Code.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
int OEMPBM_Clear(void)
{
   if (pbm_clear_call_history() != PBM_SUCCESS)
      return EFAILED;

   return SUCCESS;
}

/*=============================================================================
FUNCTION: OEMPBM_BuildAddrFields

DESCRIPTION:
   This function copies the AEECallHistoryEntry given as input to
   an AEEAddrField array allocated by itself and returns the field
   array.

PARAMETERS:
   pEntry:     [in]: Pointer to AEECallHistoryEntry
   pFields:    [in]: Pointer to PBM fields
   pnFields:   [in]: Pointer to number of fields

RETURN VALUE:
   BREW Error Code.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int OEMPBM_BuildAddrFields(const AEECallHistoryEntry *pEntry,
                                  uint8 **pFieldBuf,
                                  uint32 *pFieldBufSize,
                                  int *pnFields,
                                  pbm_device_type *pb_id)
{
   uint32 *field_lengths;
   uint8  *entry_index;
   uint8  *fields;
   pbm_field_s_type *fields_typed;
   uint16 nfields;  /* Number of fields copied from pEntry */
   AEEDBFieldType fType;
   uint16 wID;
   uint16 eidx;  /* entry index (call history fields) */
   uint16 fidx;  /* fields index (pbm fields) */
   uint16 k;
   boolean bNumber = FALSE;

   field_lengths = MALLOC(pEntry->wNumFields * sizeof(uint32));
   if (field_lengths == NULL)
      return ENOMEMORY;

   /* entry_index contains the mapping from pbm field index to call
    * history entry index for copying. */
   entry_index = MALLOC(pEntry->wNumFields * sizeof(uint8));
   if (entry_index == NULL) {
      FREE(field_lengths);
      return ENOMEMORY;
   }

   /* Check for existence of NUMBER field */
   for(eidx = 0; eidx < pEntry->wNumFields; ++eidx) {
      if (pEntry->pFields[eidx].wID == AEECALLHISTORY_FIELD_NUMBER) {
         bNumber = TRUE;
         break;
      }
   }

   /*******************************************************
    * Find field lengths
    ******************************************************/

   /* Decide which fields to add and find data lengths */
   for (eidx = fidx = 0; eidx < pEntry->wNumFields; ++eidx) {
      wID = pEntry->pFields[eidx].wID;

       /* Skip over fields that are not written.
        * CALL TYPE is not written to PBM since category contains the same
        * information. If there is a NUMBER field, NUMBER_EXT field
        * is ignored.
        */
      if (wID == AEECALLHISTORY_FIELD_NONE ||
          (bNumber && (wID == AEECALLHISTORY_FIELD_NUMBER_EX )))
         continue;

      if (wID == AEECALLHISTORY_FIELD_CALL_TYPE)
      {
        uint16 call_type;

        call_type = *((uint16*)pEntry->pFields[eidx].pData);

        switch (call_type)
        {
          case AEECALLHISTORY_CALL_TYPE_MISSED:
            if (pb_id)
              *pb_id = PBM_MISS;
            break;
          case AEECALLHISTORY_CALL_TYPE_TO:
            if (pb_id)
              *pb_id = PBM_DIALED;
            break;
          case AEECALLHISTORY_CALL_TYPE_FROM:
            if (pb_id)
              *pb_id = PBM_RCVD;
            break;
          default:
            MSG_ERROR("Unknown call history call type %d", call_type, 0, 0);
            if (pb_id)
              *pb_id = PBM_DIALED;
            break;
        }
        continue;
      }

      entry_index[fidx] = (uint8) eidx;

      if (wID == AEECALLHISTORY_FIELD_NUMBER_EX)
      {
         field_lengths[fidx++] = (pEntry->pFields[eidx].wDataLen /
                                 sizeof(AECHAR));
      } else {
         field_lengths[fidx++] = pEntry->pFields[eidx].wDataLen;
      }

   }

   nfields = fidx;

   /*******************************************************
    * Allocate PBM fields
    ******************************************************/

   if (!pbm_allocate_fields(&fields, pFieldBufSize, nfields, field_lengths)) {
      FREE(field_lengths);
      return ENOMEMORY;
   }
   fields_typed = (pbm_field_s_type*)fields;

   /*******************************************************
    * Copy fields from Call History Entry to PBM
    ******************************************************/
   for (fidx = 0; fidx < nfields; ++fidx) {

      eidx = entry_index[fidx];
      wID = pEntry->pFields[eidx].wID;

      fType = OEMPBM_CallHistoryIDtoType(pEntry->pFields[eidx].wID);

      switch (pEntry->pFields[eidx].wID) {

         case AEECALLHISTORY_FIELD_NUMBER:
         {
             char *str;

             pbm_fill_field(&fields_typed[fidx],
                       OEMPBM_CallHistoryIDtoAddrFieldID(pEntry->pFields[eidx].wID),
                       (pbm_field_type_e_type)fType,
                       pEntry->pFields[eidx].pData,
                       (uint16) field_lengths[fidx]);

             /* Translate NUMBER to pbm format (NUMBER_EXT) */
             str = (char *) pbm_get_field_data_ptr(fields,fidx);

             for (k = 0; k < field_lengths[fidx]; ++k) {
                if (*str == ',')
                   *str = PBM_TIMEPAUSE_CHAR;
                str++;
             }
             break;
         }

         default:
            if (fType == AEEDB_FT_PHONE) {
               char temp_str[CH_MAX_NAME];

               /* Translate wide string to normal */
               (void) WSTRTOSTR(pEntry->pFields[eidx].pData, temp_str,
                               sizeof(temp_str));

               pbm_fill_field(&fields_typed[fidx],
                          OEMPBM_CallHistoryIDtoAddrFieldID(pEntry->pFields[eidx].wID),
                          (pbm_field_type_e_type)fType,
                          (byte *)temp_str, (uint16)field_lengths[fidx]);
            } 
            else if ((pEntry->pFields[eidx].wID == AEECALLHISTORY_FIELD_TECH_TYPE) && 
                     (AEEDB_FT_WORD == fType))
            {
               pbm_tech_type_e_type pbm_tech_type = 
                 OEMPBM_CallHistoryTechTypetoPBMTechType(*((uint16*)pEntry->pFields[eidx].pData));
               byte *pData = (byte *)&pbm_tech_type;

               pbm_fill_field(&fields_typed[fidx],
                              OEMPBM_CallHistoryIDtoAddrFieldID(pEntry->pFields[eidx].wID),
                              (pbm_field_type_e_type)fType,
                              pData,
                              (uint16) field_lengths[fidx]);
            }
            else 
            {
               pbm_fill_field(&fields_typed[fidx],
                          OEMPBM_CallHistoryIDtoAddrFieldID(pEntry->pFields[eidx].wID),
                          fType==AEEDB_FT_STRING?PBM_FT_UCS2:(pbm_field_type_e_type)fType,
                          pEntry->pFields[eidx].pData,
                          (uint16) field_lengths[fidx]);
            }
            break;
      }
   }

   FREE(field_lengths);
   FREE(entry_index);

   *pFieldBuf = fields;
   *pnFields = nfields;

   return SUCCESS;

}

/*=============================================================================
FUNCTION: OEMPBM_CallHistoryIDtoType

DESCRIPTION:
   Returns the field type most closely associated with field type of a
   call history entry. Used to store in PBM.

PARAMETERS:
   wID         [in]: Call History ID Type

RETURN VALUE:
   AEEDBFieldType

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static AEEDBFieldType OEMPBM_CallHistoryIDtoType(uint16 wID)
{
   AEEDBFieldType fType = AEEDB_FT_NONE;

   switch (wID) {
      case AEECALLHISTORY_FIELD_NONE:
         fType = AEEDB_FT_NONE;
         break;

      case AEECALLHISTORY_FIELD_CALL_TYPE:
      case AEECALLHISTORY_FIELD_NUMBER_TYPE:
      case AEECALLHISTORY_FIELD_NUMBER_PLAN:
      case AEECALLHISTORY_FIELD_TECH_TYPE:
         fType = AEEDB_FT_WORD;
         break;

      case AEECALLHISTORY_FIELD_DATE_TIME:
      case AEECALLHISTORY_FIELD_CALL_DURATION:
         fType = AEEDB_FT_DWORD;
         break;

      case AEECALLHISTORY_FIELD_NUMBER:
      case AEECALLHISTORY_FIELD_NUMBER_EX:
         fType = AEEDB_FT_PHONE;
         break;

      case AEECALLHISTORY_FIELD_NAME:
         fType = AEEDB_FT_STRING;
         break;

      default:
         break;
   }

   return fType;
}

/*=============================================================================
FUNCTION: OEMPBM_CallHistoryIDtoAddrFieldID

DESCRIPTION:
   Maps a CallHistory field id to a pbm field ID to be
   stored in PBM.

PARAMETERS:
   wID         [in]: Call History field id

RETURN VALUE:
   pbm_field_id_e_type

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static pbm_field_id_e_type OEMPBM_CallHistoryIDtoAddrFieldID(uint16 wID)
{
   pbm_field_id_e_type fID = PBM_FIELD_NONE;

   switch (wID) {
      case AEECALLHISTORY_FIELD_NONE:
         fID = PBM_FIELD_NONE;
         break;

      case AEECALLHISTORY_FIELD_CALL_TYPE:
         fID = PBM_FIELD_CALL_TYPE;
         break;

      case AEECALLHISTORY_FIELD_NUMBER_TYPE:
         fID = PBM_FIELD_NUMBER_TYPE;
         break;

      case AEECALLHISTORY_FIELD_NUMBER_PLAN:
         fID = PBM_FIELD_NUMBER_PLAN;
         break;

      case AEECALLHISTORY_FIELD_DATE_TIME:
         fID = PBM_FIELD_DATE;
         break;

      case AEECALLHISTORY_FIELD_CALL_DURATION:
         fID = PBM_FIELD_DURATION;
         break;

      case AEECALLHISTORY_FIELD_NUMBER:
      case AEECALLHISTORY_FIELD_NUMBER_EX:
         fID = PBM_FIELD_PHONE_GENERIC;
         break;

      case AEECALLHISTORY_FIELD_NAME:
         fID = PBM_FIELD_NAME;
         break;

      case AEECALLHISTORY_FIELD_TECH_TYPE:
         fID = PBM_FIELD_TECH_TYPE;
         break;
      default:
       break;
   }

   return fID;
}

/*=============================================================================
FUNCTION: OEMPBM_CallHistoryTechTypetoPBMTechType

DESCRIPTION:
   Maps a CallHistory tech type to pbm tech type to be
   stored in PBM.

PARAMETERS:
   wID         [in]: Call History TechType

RETURN VALUE:
   pbm_tech_type_e_type

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static pbm_tech_type_e_type OEMPBM_CallHistoryTechTypetoPBMTechType(uint16 wID)
{
   pbm_tech_type_e_type techType = PBM_TECH_TYPE_VOICE;

   switch (wID) {
      case AEECALLHISTORY_TECH_TYPE_VOICE:
         techType = PBM_TECH_TYPE_VOICE;
         break;

      case AEECALLHISTORY_TECH_TYPE_VIDEO:
         techType = PBM_TECH_TYPE_VIDEO;
         break;

      case AEECALLHISTORY_TECH_TYPE_DATA:
         techType = PBM_TECH_TYPE_DATA;
         break;

      default:
         break;
   }
   return techType;
}

/*=============================================================================
FUNCTION: OEMPBM_VerifyNumber

DESCRIPTION:
   Verifies if a number can be in a NUMBER item.

PARAMETERS:
   None

RETURN VALUE:
   BREW Error Code.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
int OEMPBM_VerifyNumber(byte ucChar)
{
   if (ucChar < '0' || ucChar > '9')
   {
      switch (ucChar)
      {
         case '#':
         case '*':
         case '+':
         case ',':
         case PBM_TIMEPAUSE_CHAR:
         case PBM_HARDPAUSE_CHAR:
         case PBM_EXPANSION_CHAR:
            break;

         default:
            return EFAILED;
      }
   }
   return SUCCESS;
}

/*=============================================================================
FUNCTION: OEMPBM_VerifyNumberEx

DESCRIPTION:
   Verifies if a character can be in a NUMBER_EX item.

PARAMETERS:
   None

RETURN VALUE:
   BREW Error Code.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
int OEMPBM_VerifyNumberEx(uint16 usChar)
{
   if (usChar < '0' || usChar > '9')
   {
      switch (usChar)
      {
         case '#':
         case '*':
         case '+':
         case ',':
         case PBM_TIMEPAUSE_CHAR:
         case PBM_HARDPAUSE_CHAR:
         case PBM_EXPANSION_CHAR:
         case '\0':
            break;

         default:
            return EFAILED;
      }
   }
   return SUCCESS;
}

