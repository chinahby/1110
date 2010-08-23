/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

           C A L L   M A N A G E R   U T I L I T Y   M O D U L E

GENERAL DESCRIPTION
  Call Manager utility functions.


Copyright (c) 2001 - 2009 by QUALCOMM INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/mmode/cm/rel/08H1/src/cmutil.c#8 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/19/09   rm      Adding new feature defintions to have consistent value in 
                   the RPC files. Added cmutil_get_emergency_num_count
05/27/09   aj      Adding support for updating clients with cdma local time
01/28/09   np      HPCD Rev C
01/26/09   fj      Moved featurization into functions, 
                   and added FEATURE_MM_SUPERSET for some data structures
02/10/09   rm      Adding FEATURE_MM_SUPERSET for FEATURE_IS95B_EXT_DISP
12/28/08   np      HPCD SMS outgoing with NPCD enabled. 
12/01/08   np      Fixed calling party number to have local IDD in ASCII 
                   instead of DMTF format
10/16/08   sv      Featurizing cm_util_country_info_for_dialing to resolve
                   compilation errors in Dual Processor Targets.
10/06/08   np      Added HPCD Rev B.
10/01/08   sv      Featurizing include aeestd.h to remove lint errors
06/13/08   sv      Memory Reduction for ULC
05/12/08   sg      Updated AMR WB vocoder support for GERAN
03/17/08   ka      Fixing plus to digit conversion when converted num
                   results in dtmf for SMS.
03/14/08   sg      AMR WB vocoder support for GERAN
01/04/08   ka      Adding handset based plus code dial feature
08/07/07   ic      Lint clean up
07/09/07   cl      Move cm_ext_disp_itr_* from cm.c to cmutil.c to resolve
                   link errors. Merge 482706.
03/09/07   sk      RAM usage reduction.
06/05/06   ka/ic   Correcting buffer size in cm_util_number_to_bcd
01/12/06   ic      For specific calls / walks Lint warnings, replaced global
                   -fsp comment with -e{error#} comments where needed
01/06/06   ic      Lint cleanup
11/03/05   ic      Added CM_ASSERT() for ptr function parameters and lint
                   comments for 613 Lint error
09/01/05   sk      Fixed cm_util_number_to_bcd() and cm_util_bcd_to_ascii()
                   to include characters a,b and c in the conversions.
07/25/05   sk      Added functions (for fixing CR 67893):
                   - cm_util_ussd_pack()
                   - cm_util_ussd_unpack()
                   - cm_util_ascii_to_gsm_alphabet()
                   - cm_util_gsm_alphabet_to_ascii()
                   Added comments to old API indicating that the new one
                   should be used instead.
05/28/05   ka      CTM bit added to bearer capability
11/15/04   ic      Lint cleanup with CM_DEBUG on
08/11/04   ka      Utility function for reading bearer capability structure
08/04/04   ic      CM Lint clean up - cmutil.c with Lint supression verified
                   and free of Lint issues in all CUTE configurations.
07/27/04   ws      Add routines to fill in default orig parameters.
07/26/04   ic      Changed include file order to customer.h, then cm.h then
                   cmi.h
07/07/04   ka      Fixed Lint errors.
04/30/04   ws      num type and num plan updates
02/25/04   ic      Changes to make the code pass unit test cases and at the
                   same time make calls in Saber build
02/06/04   ws      Initial jaguar/mainline merge.
01/19/04   ws      Added handling of '+' character in cm_utils_number_to_bcd
01/13/04   ka      Merged changes from MSM6200
11/04/03   RI      Modified cm_util_unpack() to handle line feed char.
04/28/03   AT      Lint cleanup.
01/21/03   ws      Updated copyright information for 2003
03/25/02   HD      Changed to use ISDIGIT macro.
03/12/02   RI      Fixed compiler warnings.
02/08/02   PRK     Changed the cm_util_sys_id_match function prototype.
01/23/02   PRK     Replaced cmutil prefix with cm_util prefix.
                   Added sys_id utility functions.
11/21/01   RI      Modified cmutil_phone_ascii_to_phone_bcd.
                    - modified paramter 1 from byte * to char *.
10/15/01   RI      Added support for Supplementary Services ( GSM/WCDMA ).

===========================================================================*/

#include "customer.h"
#include <string.h>
#include "comdef.h"
#include "cmutil.h"

#include "AEEstd.h"

#include "cmdbg.h"
#include "nv.h"
#include "cmnv.h"



#if (defined(FEATURE_GSM) || defined(FEATURE_WCDMA))
#error code not present
#endif




/*===========================================================================

FUNCTION: cm_util_pack

DESCRIPTION
  USE cm_util_ussd_pack INSTEAD. This function has bugs and exists for
  backward compatibility only.

  Pack the bits in a byte.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cm_util_pack(
  byte         *coded_data,

  byte         num_chars,

  const byte         *string_to_convert
)
{

   byte up;
   byte down;
   byte coded_length;
   byte index;
   byte coded_index = 0;
   boolean added = FALSE;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

   CM_ASSERT(coded_data        != NULL);
   CM_ASSERT(string_to_convert != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

   if ((num_chars % 8) == 0)
   {
      coded_length = (byte) (( num_chars * 7) / 8);
   }
   else
   {
      coded_length = (byte) (((num_chars * 7) / 8) + 1);
   }

   for (coded_index = 0; coded_index < coded_length; coded_index++)
   {
      coded_data[coded_index] = 0;
   }

   coded_index = 0;

   for (index = 0; index < num_chars; index++)
   {

      if ((coded_index % 7) == 0)
      {
         if (added == FALSE)
         {
            coded_data[coded_index] = string_to_convert[index];
            added = TRUE;
         }
         else
         {
            up = (string_to_convert[index] << (8 - ((coded_index + 1) % 7))); /*lint !e734 */
            down = (string_to_convert[index] >> ((coded_index + 1) % 7));

            coded_data[coded_index] |= up;
            coded_data[coded_index + 1] |= down;

            coded_index++;
            added = FALSE;
         }
      }

      else if ((coded_index % 7) == 6)
      {
         coded_data[coded_index] |= (byte) (string_to_convert[index] << 1);
         coded_index++;
      }

      else
      {

         up = (string_to_convert[index] << (8 - ((coded_index + 1) % 7))); /*lint !e734 */
         down = (string_to_convert[index] >> ((coded_index + 1) % 7));

         coded_data[coded_index] |= up;
         coded_data[coded_index + 1] |= down;

         coded_index++;
      }
   }

}


/*===========================================================================

FUNCTION: cm_util_unpack

DESCRIPTION
  USE cm_util_ussd_unpack INSTEAD. This function has bugs and exists for
  backward compatibility only.

  Unpack the bits into bytes.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cm_util_unpack(const byte *user_data, byte num_chars, byte *message_string)
{

  byte index = 0;
  int  loop_index;
  byte no_bit_mask;
  byte actual;
  byte next = 0;
  char character;
  boolean overwriting = FALSE;
  byte byte_number;
  byte char_counter = 0;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(user_data      != NULL);
  CM_ASSERT(message_string != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  no_bit_mask = 0x7F;

  if (((num_chars * 7) % 8) != 0)
  {
     byte_number = (byte) (((num_chars * 7) / 8) + 1);
  }
  else
  {
     byte_number = (byte) ((num_chars * 7) / 8);
  }

  /*
  ** in this loop, index =        byte counter,
  **               char_counter = char counter.
  */

  char_counter = 0;
  for (loop_index = 0; loop_index < byte_number; loop_index++)
  {
     if ((loop_index % 7) == 0)
     {
        no_bit_mask = 0x7F;
     }

     actual = user_data[loop_index] & no_bit_mask;

     character = actual;
     character <<= (loop_index % 7); /*lint !e734 */
     next >>= (8 - (loop_index % 7));
     character |= next;

     /*
     ** add char to buffer
     */

     message_string[char_counter] = character;
     if( char_counter < MAX_DISPLAY_TEXT_LEN - 2 )
     {
        char_counter++;
     }

     next = user_data[loop_index] & ~(no_bit_mask);

     if ((loop_index % 7) == 6)
     {
        next >>= 1;
        character = next;

        if (next == 0x0A)
        {

           /*********************************/
           /* This is a Line Feed character */
           /*********************************/

           index = LCD_COLUMNS - char_counter % LCD_COLUMNS;

           while (index != 0)
           {
              message_string[char_counter] = ' ';
              if( char_counter < MAX_DISPLAY_TEXT_LEN - 2 )
              {
                 char_counter++;
              }
              index --;
           }

           overwriting = FALSE;
        }
        else if (next == 0x0D)
        {

           /***************************************/
           /* This is a carriage return character */
           /***************************************/

           loop_index =  LCD_COLUMNS - char_counter % LCD_COLUMNS;

           overwriting = TRUE;

           loop_index = (char_counter - 1) - loop_index;

        }
        else
        {
           if (overwriting == TRUE)
           {
              message_string[index] = character;
              loop_index++;

              if ((loop_index % LCD_COLUMNS) == 0)
              {
                 overwriting = FALSE;
              }
           }
           else
           {
              message_string[char_counter] = character;
              if( char_counter < MAX_DISPLAY_TEXT_LEN - 2 )
              {
                 char_counter++;
              }
           }
        }
     }

     no_bit_mask >>= 1;
  }
}

#if (defined FEATURE_IS95B_EXT_DISP || defined FEATURE_MM_SUPERSET)
/*===========================================================================

FUNCTION CM_EXT_DISP_ITR_MORE

DESCRIPTION
  Tests the iterator to see if it points to the end of the records.

DEPENDENCIES
  Iterator is initialized using cm_ext_disp_itr_reset

RETURN VALUE
  FALSE if the iterator points past the end of the record, TRUE otherwise.

SIDE EFFECTS
  Nothing.

===========================================================================*/
boolean cm_ext_disp_itr_more
(
  const cm_ext_disp_itr_type *itr_ptr             /* pointer to iterator */
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( itr_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return(itr_ptr->curr_offset < itr_ptr->info_ptr->len);
}


/*===========================================================================

FUNCTION CM_EXT_DISP_ITR_NEXT

DESCRIPTION
  Increments the iterator that walks through the extended display subrecords
  which exist in a packed format in the alpha field of the call event info.

  If the end was already reached, the iterator is unchanged.

DEPENDENCIES
  Iterator is initialized using cm_ext_disp_itr_reset

RETURN VALUE
  none.

SIDE EFFECTS
  Nothing.

===========================================================================*/
void cm_ext_disp_itr_next
(
  cm_ext_disp_itr_type *itr_ptr             /* pointer to iterator */
)
{

  cm_ext_disp_subrecord* subrec_ptr;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(itr_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */


  if (itr_ptr->curr_offset < itr_ptr->info_ptr->len)
  { /* not at the end */
    /* now figure new index */
    /*lint -save -e826 -e413 */
    subrec_ptr = (cm_ext_disp_subrecord*)
                  &(itr_ptr->info_ptr->buf[itr_ptr->curr_offset]);

    itr_ptr->curr_offset += (byte)FPOS(cm_ext_disp_subrecord,chari) +
                            subrec_ptr->display_len;
    /*lint -restore */

  }
}


/*===========================================================================

FUNCTION CM_EXT_DISP_ITR_GET

DESCRIPTION
  The contents of the current record pointed to by the iterator are
  copied into the supplied cm_ext_disp_subrecord structure.

DEPENDENCIES
  Iterator is initialized using cm_ext_disp_itr_reset

RETURN VALUE
  none.

SIDE EFFECTS
  Nothing.

===========================================================================*/
void cm_ext_disp_itr_get
(
  const cm_ext_disp_itr_type *itr_ptr,             /* pointer to iterator */
  cm_ext_disp_subrecord *subrec_ptr          /* area to copy next record to */
)
{
  int i;
  int len;
  cm_ext_disp_subrecord* currrec_ptr;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(itr_ptr);
  CM_ASSERT(subrec_ptr);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*lint -save -e826 */
  currrec_ptr = (cm_ext_disp_subrecord*)
                 &(itr_ptr->info_ptr->buf[itr_ptr->curr_offset]);
  /*lint -restore */

  subrec_ptr->display_tag = currrec_ptr->display_tag;
  subrec_ptr->display_len = currrec_ptr->display_len;

  len = MIN(currrec_ptr->display_len,
            sizeof(currrec_ptr->chari));
  for (i=0;i<len;i++)
  {
    subrec_ptr->chari[i] = currrec_ptr->chari[i];
  }
}

/*===========================================================================

FUNCTION CM_EXT_DISP_ITR_RESET

DESCRIPTION
  Resets the iterator that walks through the extended display subrecords
  which exist in a packed format in the alpha field of the call event info

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Nothing.

===========================================================================*/
void cm_ext_disp_itr_reset
(
  cm_ext_disp_itr_type  *itr_ptr,            /* ptr to iterator */
  cm_alpha_s_type       *info_ptr            /* ptr to packed info struct */
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(itr_ptr  != NULL);
  CM_ASSERT(info_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* point to the packed record header */
  itr_ptr->info_ptr = info_ptr;

  /* point to the first subrecord */
  itr_ptr->curr_offset = 0;

}

#endif /* #ifdef FEATURE_IS95B_EXT_DISP */

/*===========================================================================

FUNCTION: cm_util_ascii_to_def_alphabet

DESCRIPTION
  USE cm_util_ascii_to_gsm_alphabet INSTEAD. This function has bugs and
  exists for backward compatibility only.

  Convert the ASCII to default alphabet.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cm_util_ascii_to_def_alphabet(

    byte          *default_alphabet_string,

    byte          num_chars,

    const byte    *ascii_string
)
{
  #if (defined(FEATURE_GSM) || defined(FEATURE_WCDMA))
#error code not present
  #else
  SYS_ARG_NOT_USED(default_alphabet_string);
  SYS_ARG_NOT_USED(num_chars);
  SYS_ARG_NOT_USED(ascii_string);
  SYS_ARG_NOT_CONST(default_alphabet_string);
  #endif

}


/*===========================================================================

FUNCTION: cm_util_def_alphabet_to_ascii

DESCRIPTION
  USE cm_util_gsm_alphabet_to_ascii INSTEAD. This function has bugs and
  exists for backward compatibility only.

  Convert the default alphabet to ASCII.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cm_util_def_alphabet_to_ascii(

    const byte    *default_alphabet_string,

    byte    num_chars,

    byte    *ascii_string
)
{

  #if (defined(FEATURE_GSM) || defined(FEATURE_WCDMA))
#error code not present
  #else
  SYS_ARG_NOT_USED(default_alphabet_string);
  SYS_ARG_NOT_USED(num_chars);
  SYS_ARG_NOT_USED(ascii_string);
  SYS_ARG_NOT_CONST(ascii_string);
  #endif
}

/*===========================================================================

FUNCTION: cm_util_ussd_pack

DESCRIPTION
  Pack 7-bit GSM characters into bytes (8-bits)

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
byte cm_util_ussd_pack(

    byte *packed_data,

    const byte *str,

    byte num_chars
)
{
  byte stridx=0;
  byte pckidx=0;
  byte shift;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(packed_data != NULL);
  CM_ASSERT(str         != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Loop through the 7-bit string till the last but one character.
  */
  while(stridx < (num_chars-1))
  {
   shift = stridx  & 0x07;

   /* A byte of packed data is always made up of only 2 7-bit characters. The
   ** shift of the two characters always depends on their index in the string.
   */
   packed_data[pckidx++] = (str[stridx] >> shift) |
                           (str[stridx+1] << (7-shift)); /*lint !e734 */

   /* If the second characters fits inside the current packed byte, then skip
   ** it for the next iteration.
   */
   if(shift==6) stridx++;
   stridx++;
  }

  /* Special case for the last 7-bit character.
  */
  if(stridx < num_chars)
  {
    shift = stridx & 0x07;
    /* The tertiary operator (?:) takes care of the special case of (8n-1)
    ** 7-bit characters which requires padding with CR (0x0D).
    */
    packed_data[pckidx++] = ((shift == 6) ? (CHAR_CR << 1) : 0) |
                          (str[stridx] >> shift);
  }

  /* Takes care of special case when there are 8n 7-bit characters and the last
  ** character is a CR (0x0D).
  */
  if((num_chars & 0x07) == 0 && str[num_chars - 1] == CHAR_CR)
  {
    packed_data[pckidx++] = CHAR_CR;
  }

  return pckidx;
} /* cm_util_ussd_pack() */


/*===========================================================================

FUNCTION: cm_util_ussd_unpack

DESCRIPTION
  Unpack the bytes (8-bit) into 7-bit GSM characters

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
byte cm_util_ussd_unpack(

    byte *str,

    const byte *packed_data,

    byte num_bytes
)
{

  byte stridx = 0;
  byte pckidx = 0;
  byte prev = 0;
  byte curr = 0;
  byte shift;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(str         != NULL);
  CM_ASSERT(packed_data != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  while(pckidx < num_bytes)
  {
    shift = stridx & 0x07;
    curr = packed_data[pckidx++];

    /* A 7-bit character can be split at the most between two bytes of packed
    ** data.
    */
    str[stridx++] = ( (curr << shift) | (prev >> (8-shift)) ) & 0x7F;

    /* Special case where the whole of the next 7-bit character fits inside
    ** the current byte of packed data.
    */
    if(shift == 6)
    {
      /* If the next 7-bit character is a CR (0x0D) and it is the last
      ** character, then it indicates a padding character. Drop it.
      */

      if((curr >> 1) == CHAR_CR && pckidx == num_bytes)
      {
        break;
      }
      str[stridx++] = curr >> 1;
    }

    prev = curr;
  }

  return stridx;

} /* cm_util_ussd_unpack() */


/*===========================================================================

@FUNCTION cm_util_ascii_to_gsm_alphabet

@DESCRIPTION
  Convert the ASCII string to GSM default alphabets string and packs it
  into bytes.

@DEPENDENCIES
  None.

@RETURNS
  Number of characters written into the output buffer.

@SIDE_EFFECT
  None

===========================================================================*/
byte cm_util_ascii_to_gsm_alphabet(

    byte          *gsm_alphabet_string,

    const byte          *ascii_string,

    byte           num_chars
)
{
   #if (defined(FEATURE_GSM) || defined(FEATURE_WCDMA))
#error code not present
  #else
   SYS_ARG_NOT_USED(gsm_alphabet_string);
   SYS_ARG_NOT_USED(num_chars);
   SYS_ARG_NOT_USED(ascii_string);
   SYS_ARG_NOT_CONST(gsm_alphabet_string);
   return 0;
  #endif
} /* cm_util_ascii_to_gsm_alphabet */


/*===========================================================================

@FUNCTION: cm_util_gsm_alphabet_to_ascii

@DESCRIPTION
  Unpacks bytes of data into 7-bit GSM default alphabet string and then
  converts it to an ASCII string.

@DEPENDENCIES
  None.

@RETURNS
  Number of characters written into the output buffer.

@SIDE_EFFECT
  None

===========================================================================*/
byte cm_util_gsm_alphabet_to_ascii(

    byte    *ascii_string,

    const byte    *gsm_alphabet_string,

    byte     num_bytes
)
{
  #if (defined(FEATURE_GSM) || defined(FEATURE_WCDMA))
#error code not present
  #else
   SYS_ARG_NOT_USED(gsm_alphabet_string);
   SYS_ARG_NOT_USED(num_bytes);
   SYS_ARG_NOT_USED(ascii_string);
   SYS_ARG_NOT_CONST(ascii_string);
   return 0;
  #endif
} /* cm_util_gsm_alphabet_to_ascii */


/*===========================================================================

FUNCTION: cm_util_number_to_bcd
DESCRIPTION
  Convert the phone number from ASCII to BCD

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cm_util_number_to_bcd(const cm_num_s_type *number, byte *bcd_number)
{
  int i, j;                        /* Control variables for loops. */
  int bcd_index = BCD_NUM;         /* Index into output bcd_number */

  /* (digits + number_type) in bcd can be size CM_CALLED_PARTY_BCD_NO_LENGTH
  ** temp needs to be twice that of bcd. CM_CALLED_PARTY_BCD_NO_LENGTH is
  ** GW specific so using CM_MAX_NUMBER_CHARS instead.
  */
  uint8 temp[2 * CM_MAX_NUMBER_CHARS];
  uint8 number_type = 0;

   /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(number     != NULL);
  CM_ASSERT(bcd_number != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Store the number type of string coming in, it may be changed later */
  number_type = number->number_type;


  /* Copy the phone number into its temporary home */
  for (i = 0, j = 0; i < number->len; i++)
  {
    /*
    ** If international number, '+' in begining of dialed string, ignore '+'
    * and set the number_type to INTERNATIONAL.
    */
    if (i == 0)
    {
      if (number->buf[0] == '+')
      {
        number_type = (uint8)CM_NUM_TYPE_INTERNATIONAL;
        continue;
      }
    }

    /* If its a digit we care about.... */
    if (ISDIGIT(number->buf[i]) ||
        number->buf[i] == '*' ||
        number->buf[i] == '#' ||
        INRANGE(number->buf[i], 'a', 'c') ||
        INRANGE(number->buf[i], 'A', 'C') )
    {
      /* Store the bcd digits into temp. */
      temp[j++] = (number->buf[i] == '#') ? BCD_HASH :
                  (number->buf[i] == '*') ? BCD_STAR :
                  INRANGE(number->buf[i], 'a', 'c') ? (0x0C + number->buf[i] - 'a') :
                  INRANGE(number->buf[i], 'A', 'C') ? (0x0C + number->buf[i] - 'A') :
                  (number->buf[i] & 0x0f);
    }
  }
  /* Odd number of digits must have the 0x0f at the end. */
  if (j & 0x01)
  {
    temp[j++] = 0x0f;
  }

  /* Now that temp has the bcd codes in natural order... Squish them together
   * and reverse the order per bcd coding.
   */
  for (i = 0; i < j; i+=2)
  {
    /*lint -esym(644,temp)*/
    bcd_number[bcd_index++] = (byte) (temp[i+1] << 4) | temp[i];
    /*lint +esym(644,temp)*/
  }

  /* Put length value in to first element of bcd number array and the number
   * type and number plan into the second entry.
   */
  bcd_number[BCD_NT_NPI] = (byte) (0x80 | (number_type << 4) | number->number_plan);
  bcd_number[BCD_LEN] = (byte) (bcd_index - 1);

}


/*===========================================================================

FUNCTION: cm_util_bcd_to_ascii

DESCRIPTION
  Convert the phone number from BCD to ASCII

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cm_util_bcd_to_ascii(const byte *bcd_number, byte *ascii_number)
{
  int bcd_index = 0;
  int ascii_index = 0;
  byte bcd_length;
  uint8 asc_1 = 0;
  uint8 asc_2 = 0;
  boolean presentation_indicator_absent = TRUE;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(bcd_number   != NULL);
  CM_ASSERT(ascii_number != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  bcd_length = bcd_number[bcd_index++];

  /*****************************/
  /* International call or not */
  /*****************************/
  /*lint -save -e641*/
  if ((bcd_number[bcd_index] & CM_TON_MASK) >> 4 == CM_NUM_TYPE_INTERNATIONAL)
  {
     ascii_number[ascii_index++] = '+';
  }
  /*lint -restore */

  /* check if there is an extra byte or not (screening indicator,
  ** presentation_indicator)
  */

  presentation_indicator_absent =
     ((boolean)(bcd_number[bcd_index] & 0x80) >> 7);

  bcd_index++;

  /**************************/
  /* presentation_indicator */
  /**************************/

  if (presentation_indicator_absent == FALSE)
  {
     bcd_index++;
  }


  /*************************/
  /* Decode BCD into ascii */
  /*************************/

  for( ; bcd_index <= bcd_length; bcd_index++)
  {

     asc_1 = (bcd_number[bcd_index] & 0x0F);
     asc_2 = (bcd_number[bcd_index] & 0xF0) >> 4;

     ascii_number[ascii_index++] = (asc_1==BCD_STAR)? '*' :
                                    (asc_1==BCD_HASH)? '#' :
                                    INRANGE(asc_1, 0x0C, 0x0E)? (asc_1 - 0x0C) + 'a':
                                    asc_1 + '0';

     ascii_number[ascii_index++] = (asc_2==BCD_STAR)? '*' :
                                    (asc_2==BCD_HASH)? '#' :
                                    INRANGE(asc_2, 0x0C, 0x0E)? (asc_2 - 0x0C) + 'a':
                                    (asc_2==0x0F)? '\0' :
                                    asc_2 + '0';
  }

  /* Null terminate the ascii string */
  if (asc_2 != 0x0f)
  {
    ascii_number[ascii_index] = '\0';
  }
}


/*===========================================================================

FUNCTION: cm_util_subaddr_to_ascii

DESCRIPTION
  Convert the subaddress to ASCII.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cm_util_subaddr_to_ascii(
    byte   *subaddress,
    byte   *ascii_subaddress,
    byte   length)
{
  byte *data_pointer;
  byte index;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(subaddress != NULL);
  CM_ASSERT(ascii_subaddress != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  data_pointer = subaddress;

  data_pointer++;

  /*
  ** Look at the AFI (Authority and Format Identifier)
  */
  if (*data_pointer == 0x50)
  {
     /*
     ** The subaddress consists of ia5 characters
     */
     for (index = 0; index < (length - 2); index++)
     {
        ascii_subaddress[index] = subaddress[index + 2] + '0';
     }

     ascii_subaddress[index] = '\0';
  }
  else
  {
     ascii_subaddress[0] = '\0';
  }
}

/*===========================================================================

FUNCTION: cm_util_default_cdma_orig_params

DESCRIPTION
  Return a cm_cdma_orig_params_s_type  of default orig parameters

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cm_util_default_cdma_orig_params(

  cm_cdma_orig_params_s_type           *cdma_orig_param_ptr
)
{
#if defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)
  CM_ASSERT( cdma_orig_param_ptr != NULL );

  cdma_orig_param_ptr->srv_opt = 0;
  cdma_orig_param_ptr->activate_code = CM_OTASP_ACT_CODE_NONE;
  cdma_orig_param_ptr->drs_bit = TRUE;
  cdma_orig_param_ptr->sr_id_included = FALSE;
  cdma_orig_param_ptr->sr_id = 0;
  cdma_orig_param_ptr->qos_parms_incl = FALSE;
  cdma_orig_param_ptr->qos_parms_len = 0;
  cdma_orig_param_ptr->last_act_data_net = SYS_SYS_MODE_NO_SRV;

#else
  SYS_ARG_NOT_USED(cdma_orig_param_ptr);
  SYS_ARG_NOT_CONST(cdma_orig_param_ptr);

  CM_ERR("cm_util_default_cdma_orig_params shouldn't be called. ", 0,0,0);

#endif  /* FEATURE_CDMA_800 || FEATURE_CDMA_1900 */

}   /* cm_util_default_cdma_orig_params */

/*===========================================================================

FUNCTION: cm_util_default_gw_orig_params

DESCRIPTION
  Return a cm_cdma_orig_params_s_type  of default orig parameters

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cm_util_default_gw_orig_params(

  cm_gw_cs_orig_params_s_type                *gw_orig_param_ptr
)
{
#if (defined (FEATURE_WCDMA) || defined (FEATURE_GSM))
#error code not present
#else
  SYS_ARG_NOT_USED(gw_orig_param_ptr);
  SYS_ARG_NOT_CONST(gw_orig_param_ptr);

  CM_ERR("cm_util_default_gw_orig_params shouldn't be called. ", 0,0,0);

#endif  /* FEATURE_GSM || FEATURE_WCDMA */

}   /* cm_util_default_gw_orig_params */

/*===========================================================================

FUNCTION: cm_util_set_default_voice_bearer_cap

DESCRIPTION
  This function sets the bearer capability to defaul value indicating Voice call.

DEPENDENCIES
  None.

RETURN VALUE
  Default bearer capability.

SIDE EFFECTS
  None

===========================================================================*/
void cm_util_set_default_voice_bearer_cap(

  cm_bearer_capability_s_type    *bc
)
{
   #if defined(FEATURE_WCDMA) || defined (FEATURE_GSM)
#error code not present
   #else
   SYS_ARG_NOT_USED(bc);
   SYS_ARG_NOT_CONST(bc);
   CM_ERR("cm_util_set_default_voice_bearer_cap bc shouldn't be called. ",
          0,0,0);

   #endif /* defined(FEATURE_WCDMA) || defined (FEATURE_GSM) */
}/* cm_set_default_bearer_cap */



#if defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)

/*===========================================================================
Handset based plus code dialing
===========================================================================*/
/* Structure to hold sid conflict and resolution with LTM offset table
*/
struct sid_conflict_resolution_record_s
{
   uint16 sid;
   uint16 mcc1; /* Country code for country1 */
   /* offset of local time from UTC in units of 30 minutes
   ** when daylight savings is OFF. For ex- USA falls within
   ** range of -10 to -5 hrs from UTC when daylt savings is OFF,
   ** so min value is -20 and max value is -10
   */
   int8  mcc1_min_ltm_no_dts;
   int8  mcc1_max_ltm_no_dts;

   /* offset of local time from UTC in units of 30 minutes
   ** when daylight savings is ON. For ex- USA falls within
   ** range of -10 to -4 hrs from UTC when daylt savings is ON,
   ** so min value is -20 and max value is -8
   */
   int8  mcc1_min_ltm_with_dts;
   int8  mcc1_max_ltm_with_dts;
   uint16 mcc2; /* Country code for country 2 */
   /* offset of local time from UTC in units of 30 minutes
   ** when daylight savings is OFF. For ex- USA falls within
   ** range of -10 to -5 hrs from UTC when daylt savings is OFF,
   ** so min value is -20 and max value is -10
   */
   int8  mcc2_min_ltm_no_dts;
   int8  mcc2_max_ltm_no_dts;

   /* offset of local time from UTC in units of 30 minutes
   ** when daylight savings is ON. For ex- USA falls within
   ** range of -10 to -4 hrs from UTC when daylt savings is ON,
   ** so min value is -20 and max value is -8
   */
   int8  mcc2_min_ltm_with_dts;
   int8  mcc2_max_ltm_with_dts;
};

/* Declarations for Static table and sid conflict table
**
** -- NEED TO AVOID ADDING duplicate country entry for sid conflicts
**
** -- NEED TO AVOID ADDING duplicate country entry for multiple MCCs
*/
static const struct country_specific_record_s   country_list [ /* COUNTRY_LIST_LEN */] =
{

/*Sid range   cc  idd spc ndd         Network solution          MCC*/     
14976, 15103, 93, "00", "", "0", NETWORK_SOLUTION_ABSENT, 412,  /* Afghanistan 14976*/

8288, 8319, 213, "00", "", "7", NETWORK_SOLUTION_ABSENT, 603,  /* Algeria 8288*/

4100, 4100, 1, "011", "", "1", NETWORK_SOLUTION_ABSENT, 544,  /* American Samoa 4100*/

9504, 9535, 244, "00", "", "0", NETWORK_SOLUTION_ABSENT, 631, /* Angola 9504*/

32128, 32255, 54, "00", "", "0", NETWORK_SOLUTION_ABSENT, 722, /* Argentina 32128*/

15938, 15945, 374, "00", "", "0", NETWORK_SOLUTION_ABSENT, 283, /* Armenia 15938*/

9648, 9663, 297, "00", "", "", NETWORK_SOLUTION_ABSENT, 363,  /* Aruba 9648*/

7680, 7807, 61, "0011", "", "0", NETWORK_SOLUTION_ABSENT, 505,  /* Australia 7680*/

8320, 8447, 61, "0011", "", "0", NETWORK_SOLUTION_ABSENT, 505,  /* Australia 8320*/

22912, 23039, 43, "00", "", "0", NETWORK_SOLUTION_ABSENT, 232,  /* Austria 22912*/

15946, 15953, 994, "00", "", "0", NETWORK_SOLUTION_ABSENT, 400,  /* Azerbaijan 15946*/

8080, 8095, 1, "011", "", "1", NETWORK_SOLUTION_ABSENT, 364,  /* Bahamas 8080 */

13472, 13503, 880, "00", "", "0", NETWORK_SOLUTION_ABSENT, 470,  /* Bangladesh 13472*/

8160, 8175, 1, "011", "", "1", NETWORK_SOLUTION_ABSENT, 342,  /* Barbados 8160 */

15954, 15961, 375, "810", "", "8", NETWORK_SOLUTION_ABSENT, 257,  /* Belarus 15954 */

32640, 32649, 501, "00", "", "0", NETWORK_SOLUTION_ABSENT, 702,  /* Belize 32640*/
   
9088, 9119, 229, "00", "", "", NETWORK_SOLUTION_ABSENT, 616,  /* Benin 9088 */

8096, 8111, 1, "011", "", "1", NETWORK_SOLUTION_ABSENT, 350,  /* Bermuda 8096 */

10176, 10207, 267, "00", "", "", NETWORK_SOLUTION_ABSENT, 652,  /* Botswana 10176 */

31872, 32127, 55, "0015", "", "0", NETWORK_SOLUTION_ABSENT, 724,  /* Brazil 31872*/

8112, 8127, 1, "011", "", "1", NETWORK_SOLUTION_ABSENT, 348,  /* British Virgin Islands 8112 */

15584, 15615, 359, "00", "", "0", NETWORK_SOLUTION_ABSENT, 284,  /* Bulgaria 15584 */

9952, 9983, 257, "00", "", "", NETWORK_SOLUTION_ABSENT, 642,  /* Burundi 9952*/

11104, 11135, 855, "001", "", "0", NETWORK_SOLUTION_ABSENT, 456,  /* Cambodia 11104*/

9344, 9375, 237, "00", "", "", NETWORK_SOLUTION_ABSENT, 624,  /* Cameroon 9344*/

16384, 18431,1, "011", "", "1", NETWORK_SOLUTION_ABSENT, 302,  /* Canada 16384*/

8128, 8143, 1, "011", "", "1", NETWORK_SOLUTION_ABSENT, 346,  /* Cayman Islands 8128*/

31744, 31754, 56, "00", "", "0", NETWORK_SOLUTION_ABSENT, 730,  /* Chile 31744*/

31809, 31820, 56, "00", "", "0", NETWORK_SOLUTION_ABSENT, 730,  /* Chile 31809*/

31841, 31854, 56, "00", "", "0", NETWORK_SOLUTION_ABSENT, 730,  /* Chile 31841*/

13568, 14335, 86, "00", "13", "0", NETWORK_SOLUTION_ABSENT, 460, /* China 13568*/

25600, 26111, 86, "00", "13", "0", NETWORK_SOLUTION_ABSENT, 460, /* China 25600*/

13568, 14335, 86, "00", "15", "0", NETWORK_SOLUTION_ABSENT, 460, /* China 13568*/

25600, 26111, 86, "00", "15", "0", NETWORK_SOLUTION_ABSENT, 460, /* China 25600*/

13568, 14335, 86, "00", "18", "0", NETWORK_SOLUTION_ABSENT, 460, /* China 13568*/

25600, 26111, 86, "00", "18", "0", NETWORK_SOLUTION_ABSENT, 460, /* China 25600*/

31616, 31743, 57, "009", "", "03", NETWORK_SOLUTION_ABSENT, 732, /* Colombia 31616*/

8960, 8991, 225, "00", "", "0", NETWORK_SOLUTION_ABSENT, 612,  /* Cote D'Ivore/Ivory coast 8960*/

22300, 22300,875, "00", "", "", NETWORK_SOLUTION_ABSENT, 901,  /* Cruise Ship-MCP*/

/* MCC is N/A in the following : We don't include */
/*1, 2175,1, "011", "", "1", NETWORK_SOLUTION_ABSENT, N/A, */ /* Cruise Ship-Sea Mobile*/
/* 2304, 7679,1, "011", "", "1", NETWORK_SOLUTION_ABSENT, N/A, */ /* Cruise Ship-Sea Mobile*/

/*4177, 4177,1, "011", "", "1", NETWORK_SOLUTION_ABSENT, N/A,*/  /* Cruise Ship-WMS*/

24416, 24447,875, "00", "", "", NETWORK_SOLUTION_ABSENT, 274,  /* Cruise Ship-On Waves : 24416*/

16128, 16255,420, "00", "", "", NETWORK_SOLUTION_ABSENT, 230,  /* Czech republic : 16128*/

22528, 22543,45, "00", "", "", NETWORK_SOLUTION_ABSENT, 238,  /* Demark : 22528*/

8832, 8847,1, "011", "", "1", NETWORK_SOLUTION_ABSENT, 370,  /* Dominica Republic :8832*/

31296, 31327, 593, "00", "", "0", NETWORK_SOLUTION_ABSENT, 740,/* Ecuador 31296*/

8224, 8255,20, "00", "", "0", NETWORK_SOLUTION_ABSENT, 602,  /* Egypt : 8224*/


32704, 32735, 503, "00", "", "", NETWORK_SOLUTION_ABSENT, 706, /* El Salvador*/

10872, 10879,372, "00", "", "", NETWORK_SOLUTION_ABSENT, 248, /* Estonia : 10872*/

9760, 9791,251, "00", "", "0", NETWORK_SOLUTION_ABSENT, 636,  /* Ethiopia : 9760*/

10960, 10975,679, "00", "", "", NETWORK_SOLUTION_ABSENT,542 ,  /* Fiji : 10960*/

24448, 24451,358, "00", "", "0", NETWORK_SOLUTION_ABSENT,244 ,  /* Finalnd : 24448*/

8544, 8575,220, "00", "", "", NETWORK_SOLUTION_ABSENT,607 ,  /* Gambia : 8544*/

15962, 15969,995, "810", "", "8", NETWORK_SOLUTION_ABSENT,282 ,  /* Georgia : 15962*/

9216, 9247,233, "00", "", "", NETWORK_SOLUTION_ABSENT,620 ,  /* Ghana : 9216*/

9696, 9711,1, "011", "", "1", NETWORK_SOLUTION_ABSENT,535 ,  /* Guam : 9696*/

32672, 32703, 502, "00", "", "", NETWORK_SOLUTION_ABSENT, 704,  /* Guatemala 32672*/

32608, 32639, 509, "00", "", "0", NETWORK_SOLUTION_ABSENT, 372,  /* Haiti 32608*/

32736, 32767, 504, "00", "", "0", NETWORK_SOLUTION_ABSENT, 708,  /* Honduras 32736*/

10640, 10655, 852, "001", "", "", NETWORK_SOLUTION_ABSENT, 454,/* Hong Kong*/

24416, 24447,354, "00", "", "0", NETWORK_SOLUTION_ABSENT,274 ,  /* Iceland : 24416*/

14464, 14847, 91, "00", "", "0", NETWORK_SOLUTION_ABSENT, 404,  /* India 14464*/

10496, 10623, 62, "001", "", "0", NETWORK_SOLUTION_ABSENT, 510, /* Indonesia 10496*/

15456, 15487,964, "00", "", "0", NETWORK_SOLUTION_ABSENT,418 ,  /* Iraq : 15456*/

24384, 24387,353, "00", "", "0", NETWORK_SOLUTION_ABSENT,272 ,  /* Ireland : 24384*/

8448, 8479, 972, "00", "", "0", NETWORK_SOLUTION_ABSENT, 425, /* Israel 8448*/

8176, 8191,1, "011", "", "1", NETWORK_SOLUTION_ABSENT,338 ,  /* Jamaica : 8176*/

12288, 13311, 81, "001010", "", "0", NETWORK_SOLUTION_ABSENT, 440, /* Japan 12288*/

8928, 8943, 7, "810", "", "8", NETWORK_SOLUTION_ABSENT, 401, /* Kazakhstan 8928*/

9856, 9887,254, "000", "", "0", NETWORK_SOLUTION_ABSENT,639 ,  /* Kenya : 9856*/

2176, 2303, 82, "001", "", "0", NETWORK_SOLUTION_ABSENT, 450, /* Korea (South)*/

11312, 11327,965, "00", "", "0", NETWORK_SOLUTION_ABSENT,419 ,  /* Kuwait : 11312*/

21550, 21566, 996, "00", "", "0", NETWORK_SOLUTION_ABSENT, 437, /* Kyrgyz Republic*/

13440, 13471,856, "00", "", "0", NETWORK_SOLUTION_ABSENT,457 ,  /* Laos : 13440*/

10784, 10785, 371, "00", "", "", NETWORK_SOLUTION_ABSENT, 247, /* Latvia 10784*/

11296, 11311,853, "00", "", "0", NETWORK_SOLUTION_ABSENT,455 , /* Macao/Macau : 11296*/

10048, 10079,261, "00", "", "0", NETWORK_SOLUTION_ABSENT,646 , /* Madagascar : 10048*/

10368, 10495, 60, "00", "", "0", NETWORK_SOLUTION_ABSENT, 502,  /* Malaysia 10368*/

8768, 8799,223, "00", "", "0", NETWORK_SOLUTION_ABSENT,610 ,  /*Mail Republic : 8768*/

8736, 8767,222, "00", "", "0", NETWORK_SOLUTION_ABSENT,609 ,  /*Maruitania : 8736*/

9120, 9151,230, "020", "", "0", NETWORK_SOLUTION_ABSENT,617 ,  /*Mauritius : 9120*/

24576, 25075, 52, "00", "", "01", NETWORK_SOLUTION_ABSENT, 334, /* Mexico 24576*/

25100, 25124, 52, "00", "", "01", NETWORK_SOLUTION_ABSENT, 334, /* Mexico 25100*/

15922, 15937,373, "00", "", "0", NETWORK_SOLUTION_ABSENT,259 ,  /*Moldova : 15922*/

15520, 15551, 976, "001", "", "0", NETWORK_SOLUTION_ABSENT, 428, /* Mongolia 15520*/

8256, 8287, 212, "00", "", "", NETWORK_SOLUTION_ABSENT, 604,  /* Morocco 8256*/

9984, 10015, 258, "00", "", "0", NETWORK_SOLUTION_ABSENT, 634, /* Mozambique 9984*/

15232, 15359, 95, "00", "", "", NETWORK_SOLUTION_ABSENT, 414, /* Myanmar (Burma)*/

10112, 10143, 264, "00", "", "0", NETWORK_SOLUTION_ABSENT, 649, /* Namibia */

15552, 15583, 977, "00", "", "0", NETWORK_SOLUTION_ABSENT, 429, /* Nepal 15552*/

31392, 31407, 599, "00", "", "0", NETWORK_SOLUTION_ABSENT, 362, /* Netherlands Antilles*/

8576, 8703, 64, "00", "", "0", NETWORK_SOLUTION_PRESENT, 530,  /* New Zealand*/

32512, 32543, 505, "00", "", "0", NETWORK_SOLUTION_ABSENT, 710, /* Nicaragua 32512*/

9024, 9055, 227, "00", "", "0", NETWORK_SOLUTION_ABSENT, 614, /* Niger 9024*/

9248, 9279, 234, "009", "", "0", NETWORK_SOLUTION_ABSENT, 621, /* Nigeria 9248*/

9680, 9695, 1, "011", "", "1", NETWORK_SOLUTION_ABSENT, 534, /* - Northern Marianas*/

22272, 22399, 47, "00", "", "", NETWORK_SOLUTION_ABSENT, 242, /* Norway 22272*/

11344, 11359, 968, "00", "", "0", NETWORK_SOLUTION_ABSENT, 422, /* Oman 11344*/

14848, 14975, 92, "00", "", "0", NETWORK_SOLUTION_ABSENT, 410, /* Pakistan 14848*/

32576, 32607, 507, "00", "", "0", NETWORK_SOLUTION_ABSENT, 714, /* Panama 32576*/

32384, 32511, 51, "00", "", "0", NETWORK_SOLUTION_ABSENT, 716, /* Peru 32384*/

10624, 10639, 63, "00", "", "0", NETWORK_SOLUTION_ABSENT, 515, /* Philippines 10624*/

10656, 10751, 63, "00", "", "0", NETWORK_SOLUTION_ABSENT, 515, /* Philippines 10656*/

16256, 16383, 48, "00", "", "0", NETWORK_SOLUTION_ABSENT, 260,  /* Poland 16256 */

24320, 24351, 351, "00", "", "", NETWORK_SOLUTION_ABSENT, 268, /* Portugal 24320*/

1, 2175, 1, "011", "", "1", NETWORK_SOLUTION_ABSENT,330,  /* Puerto Rico: 1 */

2304, 7679, 1, "011", "", "1", NETWORK_SOLUTION_ABSENT,330,  /* Puerto Rico: 1 */

16000, 16127, 40, "00", "", "0", NETWORK_SOLUTION_ABSENT, 226,  /* Romania 16000*/

11392, 11554, 7, "810", "", "8", NETWORK_SOLUTION_ABSENT, 250,/* Russia 11392*/

11556, 11599, 7, "810", "", "8", NETWORK_SOLUTION_ABSENT, 250, /* Russia 11556*/

11601, 12287, 7, "810", "", "8", NETWORK_SOLUTION_ABSENT, 250, /* Russia 11601*/

9728, 9759,250, "00", "", "0", NETWORK_SOLUTION_ABSENT,635 ,  /*Rwanda : 9728*/

15488, 15519, 966, "00", "", "0", NETWORK_SOLUTION_ABSENT, 420, /* Saudi Arabia*/

8704, 8735, 221, "00", "", "0", NETWORK_SOLUTION_ABSENT, 608,   /* Senegal 8704*/

9184, 9215, 232, "00", "", "0", NETWORK_SOLUTION_ABSENT, 619, /* Sierra Leone*/

10240, 10367, 27, "00", "", "0", NETWORK_SOLUTION_ABSENT, 655, /* South Africa*/

15104, 15231, 94, "00", "", "0", NETWORK_SOLUTION_ABSENT, 413, /* Sri Lanka*/

8032, 8047, 1, "011", "", "1", NETWORK_SOLUTION_ABSENT, 356,  /* - St. Kitts/Nevis*/

8010, 8011, 1, "011", "", "1", NETWORK_SOLUTION_ABSENT, 358,  /* - St. Lucia 8010*/

8016, 8031, 1, "011", "", "1", NETWORK_SOLUTION_ABSENT, 358,  /* - St. Lucia 8016*/

9568, 9599, 249, "00", "", "0", NETWORK_SOLUTION_ABSENT, 634,  /* - Sudan*/

31136, 31167, 597, "00", "", "", NETWORK_SOLUTION_ABSENT, 746,  /* - Sriname*/

22400, 22403,46, "00", "", "0", NETWORK_SOLUTION_ABSENT,240 , /* Sweden : 22400*/

13504, 13535, 886, "002", "", "", NETWORK_SOLUTION_ABSENT, 466, /* Taiwan 13504*/

10856, 10871, 992, "810", "", "8", NETWORK_SOLUTION_ABSENT, 436, /* Tajikistan 10856*/

9888, 9919, 255, "000", "", "0", NETWORK_SOLUTION_ABSENT, 640, /* Tanzania 9888*/

8192, 8223, 66, "001", "", "0", NETWORK_SOLUTION_ABSENT, 520, /* Thailand 8192*/

9056, 9087, 228, "00", "", "", NETWORK_SOLUTION_ABSENT, 615,  /* Togolese Republic /Togo*/

9824, 9855, 1, "011", "", "1", NETWORK_SOLUTION_ABSENT, 374,  /* - Trinidad*/

10800, 10815, 1, "011", "", "1", NETWORK_SOLUTION_ABSENT, 376,  /* - Turk and Caicos*/

15970,15977,993, "810", "", "8", NETWORK_SOLUTION_ABSENT,438 ,  /*Turkmenistan: 15970*/

9920, 9951, 256, "000", "", "0", NETWORK_SOLUTION_ABSENT, 641,  /* Uganda 9920*/

15906, 15921,380, "810", "", "8", NETWORK_SOLUTION_ABSENT,255 ,  /*Ukranie : 15906*/

31168, 31199, 598, "00", "", "0", NETWORK_SOLUTION_ABSENT, 748,  /* Uruguay 31168*/

1, 2175, 1, "011", "", "1", NETWORK_SOLUTION_ABSENT, 332,  /* - USA Virgin Islands :1*/

2304, 7679, 1, "011", "", "1", NETWORK_SOLUTION_ABSENT, 332, /* - USA Virgin Islands : 2304*/

1, 2175, 1, "011", "", "1", NETWORK_SOLUTION_ABSENT, 310,  /* - USA*/

1, 2175, 1, "011", "", "1", NETWORK_SOLUTION_ABSENT, 311,   /* - USA*/

1, 2175, 1, "011", "", "1", NETWORK_SOLUTION_ABSENT, 312,  /* - USA*/

1, 2175, 1, "011", "", "1", NETWORK_SOLUTION_ABSENT, 313, /* - USA*/

1, 2175, 1, "011", "", "1", NETWORK_SOLUTION_ABSENT, 314, /* - USA*/

1, 2175, 1, "011", "", "1", NETWORK_SOLUTION_ABSENT, 315, /* - USA*/

1, 2175, 1, "011", "", "1", NETWORK_SOLUTION_ABSENT, 316, /* - USA*/

2304, 7679, 1, "011", "", "1", NETWORK_SOLUTION_ABSENT, 310, /* - USA*/

2304, 7679, 1, "011", "", "1", NETWORK_SOLUTION_ABSENT, 311, /* - USA*/

2304, 7679, 1, "011", "", "1", NETWORK_SOLUTION_ABSENT, 312, /* - USA*/

2304, 7679, 1, "011", "", "1", NETWORK_SOLUTION_ABSENT, 313, /* - USA*/

2304, 7679, 1, "011", "", "1", NETWORK_SOLUTION_ABSENT, 314, /* - USA*/

2304, 7679, 1, "011", "", "1", NETWORK_SOLUTION_ABSENT, 315, /* - USA*/

2304, 7679, 1, "011", "", "1", NETWORK_SOLUTION_ABSENT, 316, /* - USA*/

10832, 10855, 998, "810", "", "8", NETWORK_SOLUTION_ABSENT, 434,  /* Uzbekistan 10832*/

11555, 11555, 998, "810", "", "8", NETWORK_SOLUTION_ABSENT, 434,  /* Uzbekistan 11555*/

11600, 11600, 998, "810", "", "8", NETWORK_SOLUTION_ABSENT, 434,    /* Uzbekistan 11600*/

31488, 31615, 58, "00", "", "0", NETWORK_SOLUTION_ABSENT, 734, /* Venezuela 31488*/

13312, 13439, 84, "00", "", "0", NETWORK_SOLUTION_ABSENT, 452,/* Vietnam 13312*/

11328, 11343, 967, "00", "", "0", NETWORK_SOLUTION_ABSENT, 421,  /* Yemen 11328*/

11360, 11375, 967, "00", "", "0", NETWORK_SOLUTION_ABSENT, 421,  /* Yemen 11360*/

9472, 9503,243, "00", "", "", NETWORK_SOLUTION_ABSENT,630 ,  /*Zaire/Democratic republic of Congo*/

10016, 10047, 260, "00", "", "0", NETWORK_SOLUTION_ABSENT, 645,  /* Zambia 10016*/

10080, 10111, 263, "00", "", "0", NETWORK_SOLUTION_ABSENT, 648  /* Zambabwe 10016*/

};

    
static const struct sid_conflict_resolution_record_s       sid_cnflt_res_list [ /* SID_CONFLICT_LIST_LEN*/ ] =
{
1, 310, -20, -10, -16, -8, 404, 11, 11, LTM_INVALID, LTM_INVALID,    /* 1 USA India */
7, 310, -20, -10, -16, -8, 404, 11, 11, LTM_INVALID, LTM_INVALID,    /* 7 USA India */
13, 310, -20, -10, -16, -8, 454, 16, 16, LTM_INVALID, LTM_INVALID,   /* 13 USA Hong Kong */
111, 310,  -20, -10, -16, -8, 621, 2, 2, LTM_INVALID, LTM_INVALID,   /* 111 USA Nigeria */
211, 310, -20, -10, -16, -8, 621, 2, 2, LTM_INVALID, LTM_INVALID,    /* 211 USA Nigeria */
311, 310,  -20, -10, -16, -8, 621, 2, 2, LTM_INVALID, LTM_INVALID,   /* 311 USA Nigeria */
1235, 310, -20, -10, -16, -8, 515, 16, 16, LTM_INVALID, LTM_INVALID, /* 1235 USA Philppines */
1901, 310,  -20, -10, -16, -8, 250, 4, 6, 6, 8,                       /* 1901 USA Russia */
1111, 310,  -20, -10, -16, -8, 450, 18, 18, LTM_INVALID, LTM_INVALID, /* 1111 USA Korea(South) */
1112, 310,  -20, -10, -16, -8, 450, 18, 18, LTM_INVALID, LTM_INVALID, /* 1112 USA Korea(South) */
1113, 310,  -20, -10, -16, -8, 450, 18, 18, LTM_INVALID, LTM_INVALID, /* 1113 USA Korea(South) */
1700, 310,  -20, -10, -16, -8, 450, 18, 18, LTM_INVALID, LTM_INVALID, /* 1700 USA Korea(South) */
2177, 310,  -20, -10, -16, -8, 450, 18, 18, LTM_INVALID, LTM_INVALID, /* 2177 USA Korea(South) */
2179, 310,  -20, -10, -16, -8, 450, 18, 18, LTM_INVALID, LTM_INVALID, /* 2179 USA Korea(South) */
2181, 310,  -20, -10, -16, -8, 450, 18, 18, LTM_INVALID, LTM_INVALID, /* 2181 USA Korea(South) */
2183, 310,  -20, -10, -16, -8, 450, 18, 18, LTM_INVALID, LTM_INVALID, /* 2183 USA Korea(South) */
2185, 310,  -20, -10, -16, -8, 450, 18, 18, LTM_INVALID, LTM_INVALID, /* 2185 USA Korea(South) */
2187, 310,  -20, -10, -16, -8, 450, 18, 18, LTM_INVALID, LTM_INVALID, /* 2187 USA Korea(South) */
2189, 310,  -20, -10, -16, -8, 450, 18, 18, LTM_INVALID, LTM_INVALID, /* 2189 USA Korea(South) */
2191, 310,  -20, -10, -16, -8, 450, 18, 18, LTM_INVALID, LTM_INVALID, /* 2191 USA Korea(South) */
2193, 310,  -20, -10, -16, -8, 450, 18, 18, LTM_INVALID, LTM_INVALID, /* 2193 USA Korea(South) */
2195, 310,  -20, -10, -16, -8, 450, 18, 18, LTM_INVALID, LTM_INVALID, /* 2195 USA Korea(South) */
2197, 310,  -20, -10, -16, -8, 450, 18, 18, LTM_INVALID, LTM_INVALID, /* 2197 USA Korea(South) */
2199, 310,  -20, -10, -16, -8, 450, 18, 18, LTM_INVALID, LTM_INVALID, /* 2199 USA Korea(South) */
2201, 310,  -20, -10, -16, -8, 450, 18, 18, LTM_INVALID, LTM_INVALID, /* 2201 USA Korea(South) */
2203, 310,  -20, -10, -16, -8, 450, 18, 18, LTM_INVALID, LTM_INVALID, /* 2203 USA Korea(South) */
2205, 310,  -20, -10, -16, -8, 450, 18, 18, LTM_INVALID, LTM_INVALID, /* 2205 USA Korea(South) */
2207, 310,  -20, -10, -16, -8, 450, 18, 18, LTM_INVALID, LTM_INVALID, /* 2207 USA Korea(South) */
2209, 310,  -20, -10, -16, -8, 450, 18, 18, LTM_INVALID, LTM_INVALID, /* 2209 USA Korea(South) */
2211, 310,  -20, -10, -16, -8, 450, 18, 18, LTM_INVALID, LTM_INVALID, /* 2211 USA Korea(South) */
2213, 310,  -20, -10, -16, -8, 450, 18, 18, LTM_INVALID, LTM_INVALID, /* 2213 USA Korea(South) */
2215, 310,  -20, -10, -16, -8, 450, 18, 18, LTM_INVALID, LTM_INVALID, /* 2215 USA Korea(South) */
2217, 310,  -20, -10, -16, -8, 450, 18, 18, LTM_INVALID, LTM_INVALID, /* 2217 USA Korea(South) */
2219, 310,  -20, -10, -16, -8, 450, 18, 18, LTM_INVALID, LTM_INVALID, /* 2219 USA Korea(South) */
2221, 310,  -20, -10, -16, -8, 450, 18, 18, LTM_INVALID, LTM_INVALID, /* 2221 USA Korea(South) */
2223, 310,  -20, -10, -16, -8, 450, 18, 18, LTM_INVALID, LTM_INVALID, /* 2223 USA Korea(South) */
2225, 310,  -20, -10, -16, -8, 450, 18, 18, LTM_INVALID, LTM_INVALID, /* 2225 USA Korea(South) */
2227, 310,  -20, -10, -16, -8, 450, 18, 18, LTM_INVALID, LTM_INVALID, /* 2227 USA Korea(South) */
2229, 310,  -20, -10, -16, -8, 450, 18, 18, LTM_INVALID, LTM_INVALID, /* 2229 USA Korea(South) */
2231, 310,  -20, -10, -16, -8, 450, 18, 18, LTM_INVALID, LTM_INVALID, /* 2231 USA Korea(South) */
2233, 310,  -20, -10, -16, -8, 450, 18, 18, LTM_INVALID, LTM_INVALID, /* 2233 USA Korea(South) */
2235, 310,  -20, -10, -16, -8, 450, 18, 18, LTM_INVALID, LTM_INVALID, /* 2235 USA Korea(South) */
2237, 310,  -20, -10, -16, -8, 450, 18, 18, LTM_INVALID, LTM_INVALID, /* 2237 USA Korea(South) */
2239, 310,  -20, -10, -16, -8, 450, 18, 18, LTM_INVALID, LTM_INVALID, /* 2239 USA Korea(South) */
2241, 310,  -20, -10, -16, -8, 450, 18, 18, LTM_INVALID, LTM_INVALID, /* 2241 USA Korea(South) */
2243, 310,  -20, -10, -16, -8, 450, 18, 18, LTM_INVALID, LTM_INVALID, /* 2243 USA Korea(South) */
2301, 310,  -20, -10, -16, -8, 450, 18, 18, LTM_INVALID, LTM_INVALID, /* 2301 USA Korea(South) */
2303, 310,  -20, -10, -16, -8, 450, 18, 18, LTM_INVALID, LTM_INVALID, /* 2301 USA Korea(South) */
4369, 310,  -20, -10, -16, -8, 450, 18, 18, LTM_INVALID, LTM_INVALID, /* 2303 USA Korea(South) */
4370, 310,  -20, -10, -16, -8, 450, 18, 18, LTM_INVALID, LTM_INVALID, /* 2303 USA Korea(South) */
4371, 310,  -20, -10, -16, -8, 450, 18, 18, LTM_INVALID, LTM_INVALID, /* 2303 USA Korea(South) */

2222, 450, 18, 18, LTM_INVALID, LTM_INVALID, 404, 11, 11, LTM_INVALID, LTM_INVALID,  /* 2222 Korea(South) India */
4100, 544, -22, -22, LTM_INVALID, LTM_INVALID, 734, -9, -9, LTM_INVALID, LTM_INVALID, /* 4100 America Samoa Venezuela */
4120, 310, -12, -12, -10, -10, 734, -9, -9, LTM_INVALID, LTM_INVALID, /* 4120 USA Venezuela */
4130, 310, 20, 20, LTM_INVALID, LTM_INVALID, 734, -9, -9, LTM_INVALID, LTM_INVALID, /* 4130 USA Venezuela */
4140, 310, -12, -12, -10, -10, 734, -9, -9, LTM_INVALID, LTM_INVALID, /* 4140 USA Venezuela */

8198, 520, 14, 14, LTM_INVALID, LTM_INVALID, 350, -8, -6, LTM_INVALID, LTM_INVALID, /* 8198 Thailand Bermuda */
8294, 603, 2, 2, -2, -2, 310,  -20, -10, -16, -8, /* 8294 Algeria USA */
8358, 505, 16, 20, 20, 22, 310, -20, -10, -16, -8, /* 8358 Australia USA */
8360, 505, 16, 20, 20, 22, 310, -20, -10, -16, -8, /* 8360 Australia USA */
8616, 530, 24, 24, 26, 26, 310,  -20, -10, -16, -8, /* 8616 New Zealand USA */
8860, 625, -2, -2, LTM_INVALID, LTM_INVALID, 310,  -20, -10, -16, -8,  /* 8860 Cape Verde USA */
8861, 625, -2, -2, LTM_INVALID, LTM_INVALID, 310,  -20, -10, -16, -8, /* 8861 Cape Verde USA */
8863, 625, -2, -2, LTM_INVALID, LTM_INVALID, 310,  -20, -10, -16, -8, /* 8863 Cape Verde USA */
8950, 647, 8, 8, LTM_INVALID, LTM_INVALID, 310,  -20, -10, -16, -8, /* 8950 Reunion Island USA */
8952, 647, 8, 8, LTM_INVALID, LTM_INVALID, 310,  -20, -10, -16, -8, /* 8952 Reunion Island USA */
8960, 612, 0, 0, LTM_INVALID, LTM_INVALID, 310,  -20, -10, -16, -8, /* 8960 Cote d'Ivoire USA */
8962, 612, 0, 0, LTM_INVALID, LTM_INVALID, 310,  -20, -10, -16, -8, /* 8962 Cote d'Ivoire USA */
9080, 615, 0, 0, LTM_INVALID, LTM_INVALID, 310,  -20, -10, -16, -8, /* 9080 Togolese Republic USA */
9212, 619, 0, 0, LTM_INVALID, LTM_INVALID, 310,  -20, -10, -16, -8, /* 9212 Sierra Leone USA */
9244, 620, 0, 0, LTM_INVALID, LTM_INVALID, 310,  -20, -10, -16, -8, /* 9244 Ghana USA */
9426, 620, 0, 0, LTM_INVALID, LTM_INVALID, 310,  -20, -10, -16, -8, /* 9426 Ghana USA */
9322, 623, 2, 2, LTM_INVALID, LTM_INVALID, 310,  -20, -10, -16, -8, /* 9322 Central Africa Republic USA */
9394, 627, 2, 2, LTM_INVALID, LTM_INVALID, 310,  -20, -10, -16, -8, /* 9394 Equatorial Guinea USA */
9488, 629, 2, 2, LTM_INVALID, LTM_INVALID, 310,  -20, -10, -16, -8, /* 9488 Democratic Republic of the Congo USA */
9562, 632, 0, 0, LTM_INVALID, LTM_INVALID, 310,  -20, -10, -16, -8, /* 9562 Guinea-Bissau USA */
9640, 653, 4, 4, LTM_INVALID, LTM_INVALID, 310,  -20, -10, -16, -8, /* 9640 Swaziland USA */
9642, 653, 4, 4, LTM_INVALID, LTM_INVALID, 310,  -20, -10, -16, -8, /* 9642 Swaziland USA */
9644, 653, 4, 4, LTM_INVALID, LTM_INVALID, 310,  -20, -10, -16, -8, /* 9644 Swaziland USA */
9788, 636, 6, 6, LTM_INVALID, LTM_INVALID, 310,  -20, -10, -16, -8, /* 9788 Ethiopia USA */
9790, 636, 6, 6, LTM_INVALID, LTM_INVALID, 310,  -20, -10, -16, -8, /* 9790 Ethiopia USA */
12461, 440, 18, 18, LTM_INVALID, LTM_INVALID, 470, 12, 12, LTM_INVALID, LTM_INVALID, /* 12461 Japan Bangladesh */
12463, 440, 18, 18, LTM_INVALID, LTM_INVALID, 470, 12, 12, LTM_INVALID, LTM_INVALID, /* 12463 Japan Bangladesh */
12464, 440, 18, 18, LTM_INVALID, LTM_INVALID, 470, 12, 12, LTM_INVALID, LTM_INVALID, /* 12464 Japan Bangladesh */
12561, 440, 18, 18, LTM_INVALID, LTM_INVALID, 525, 16, 16, LTM_INVALID, LTM_INVALID, /* 12561 Japan Singapore */
12978, 440, 18, 18, LTM_INVALID, LTM_INVALID, 363, -8, -8,  LTM_INVALID, LTM_INVALID, /* 12978 Japan Aruba */
14850, 410, 10, 10, LTM_INVALID, LTM_INVALID, 404, 11, 11, LTM_INVALID, LTM_INVALID, /* 14850 Pakistan India */
32752, 708, -12, -12, LTM_INVALID, LTM_INVALID, 364, -10, -10, -8, -8  /* 14850 Honduras Bahamas */

};


#endif /* FEATURE_CDMA_800 || FEATURE_CDMA_1900 */

/*===========================================================================

FUNCTION CM_UTIL_ENCODE_DTMF

DESCRIPTION
  Populates the dtmf equivalent of given ascii value.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  -  dtmf_val contains the converted value.
  FALSE -  dtmf_val contains 0 (represented by CMUTIL_DTMF_FOR_DIAL_0).

SIDE EFFECTS
  None

===========================================================================*/
boolean cm_util_encode_dtmf(

  uint8 ascii_val,
    /* Ascii value */

  uint8 *dtmf_val_ptr
    /* dtmf value */
)
{
#if defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)
  boolean status = FALSE;
    /* Status of conversion */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_HIGH ("in call %d %c ", ascii_val, ascii_val, 0);

  /* Sanity check on data_val_ptr */
  if (!dtmf_val_ptr)
  {
    CM_ERR ("data_val_ptr is NULL", 0, 0, 0);

    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* set status to TRUE */
  status = TRUE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  *dtmf_val_ptr = CMUTIL_DTMF_FOR_DIAL_0;

  switch (ascii_val)
  {
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      *dtmf_val_ptr = ascii_val - '0';
      break;

    case '0':
      *dtmf_val_ptr = CMUTIL_DTMF_FOR_DIAL_0;
      break;

    case '#':
      *dtmf_val_ptr = CMUTIL_DTMF_FOR_DIAL_PND;
      break;

    case '*':
      *dtmf_val_ptr = CMUTIL_DTMF_FOR_DIAL_STAR;
      break;

    default:
      status = FALSE;
      CM_ERR( "Illegal digit code %d", ascii_val, 0, 0 );
  } /* end switch */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Status of encode operation */
  return (status);

#else
  SYS_ARG_NOT_USED(ascii_val);
  SYS_ARG_NOT_USED(dtmf_val_ptr);
  SYS_ARG_NOT_CONST(dtmf_val_ptr);

  CM_ERR("cm_util_encode_dtmf shouldn't be called. ", 0,0,0);

  return (FALSE);
#endif  /* FEATURE_CDMA_800 || FEATURE_CDMA_1900 */

} /* cm_util_encode_dtmf() */

/*===========================================================================

FUNCTION CM_UTIL_DTMF_TO_ASCII

DESCRIPTION
  Populates the ascii equivalent of given DTMF value.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  -  ascii_val contains the converted value.
  FALSE -  sscii_val contains 0 (represented by CMUTIL_DTMF_FOR_DIAL_0).

SIDE EFFECTS
  None

===========================================================================*/
boolean cm_util_dtmf_to_ascii(

  uint8 dtmf_val,
    /* Ascii value */

  uint8 *ascii_val_ptr
    /* dtmf value */
)
{
  boolean status = FALSE;
    /* Status of conversion */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_HIGH ("in call %d %c ", dtmf_val, dtmf_val, 0);

  /* Sanity check on data_val_ptr */
  if (!ascii_val_ptr)
  {
    CM_ERR ("ascii_val_ptr is NULL", 0, 0, 0);

    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* set status to TRUE */
  status = TRUE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  *ascii_val_ptr = '0';

  switch (dtmf_val)
  {
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
      *ascii_val_ptr = dtmf_val + '0';
      break;

    case CMUTIL_DTMF_FOR_DIAL_0:
      *ascii_val_ptr = '0';
      break;

    case CMUTIL_DTMF_FOR_DIAL_PND:
      *ascii_val_ptr = '#';
      break;

    case CMUTIL_DTMF_FOR_DIAL_STAR:
      *ascii_val_ptr = '*';
      break;

    default:
      status = FALSE;
      CM_ERR( "Illegal dtmf code %d", dtmf_val, 0, 0 );
  } /* end switch */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Status of encode operation */
  return (status);

} /* cm_util_dtmf_to_ascii() */


/*===========================================================================

FUNCTION: cmutil_populate_country_list

DESCRIPTION
 Given sid, ltm offset and daylt savings, it gives the list of countries
 that satisfy the given parameters.

DEPENDENCIES
  None.

RETURN VALUE
  List of countries that contain given sid

SIDE EFFECTS
  None

===========================================================================*/
void cmutil_populate_country_list (

  uint16 curr_sid,
    /* sid for which country list is generated */

  int8 curr_ltm,
    /* ltm offset of current system */

  sys_daylt_savings_e_type curr_daylt,
    /* daylt savings flag of current system */

  cm_ss_country_list_s_type *countries_with_sid_ltm_cnft
    /* Pointer that will carry the result of countries */
)
{
#if defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)
  int list_index = 0;

  boolean is_sid_cnflct =  FALSE;
    /* flag to tell if there is a sid cnfclt */

  int sid_cnflt_res_entry = 0;
    /* Index in sid conflict record list */

  boolean is_timezone_match = FALSE;
    /* Does current timezone match for given country */

  uint16 found_mcc=CM_INVALID_MOBILE_COUNTRY_CODE;  
     /* Found mcc through sid confliction resoultion table based LTM offset*/    
    
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Sanity check */
  if(  !BETWEEN(curr_ltm, -26, 26)
     ||
     !BETWEEN(curr_daylt, SYS_DAYLT_SAVINGS_NONE, SYS_DAYLT_SAVINGS_MAX)
     ||
     !countries_with_sid_ltm_cnft
   )
  {
    CM_ERR (" Input Error sid=%d,ltm= %d, daytl=%d", curr_sid, 
        curr_ltm, curr_daylt);
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  countries_with_sid_ltm_cnft->country_code_len = 0;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_HIGH ("cmutil_populate_country_list, sid = %d", curr_sid, 0, 0);

  /* Is the given sid used by more than one country
  */
  for (list_index = 0; list_index < (int)ARR_SIZE(sid_cnflt_res_list);
                                               ++list_index)
  {
    if (sid_cnflt_res_list[list_index].sid == curr_sid)
    {
      is_sid_cnflct = TRUE;

      sid_cnflt_res_entry = list_index;

      break;
    }

  } /* For */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Sid conflict is found, try to resolve the conflict */
  if (is_sid_cnflct)
  {
       CM_MSG_HIGH ("sid %d does conflict", curr_sid, 0, 0);        
       /* Check if current entry matches daylt and ltm_off
       */
       /* Daylt is ON */
       if (curr_daylt == SYS_DAYLT_SAVINGS_ON) 
       {
         CM_MSG_HIGH ("curr daylt is ON curr ltm= %d",curr_ltm , 0, 0);

         if ((curr_ltm >= 
              sid_cnflt_res_list[sid_cnflt_res_entry].mcc1_min_ltm_with_dts )
              && 
             (curr_ltm <= 
              sid_cnflt_res_list[sid_cnflt_res_entry].mcc1_max_ltm_with_dts))
         {
           found_mcc = sid_cnflt_res_list[sid_cnflt_res_entry].mcc1;
           is_timezone_match = TRUE;
         }
         else if ((curr_ltm >= 
                   sid_cnflt_res_list[sid_cnflt_res_entry].mcc2_min_ltm_with_dts )
                   && 
                  (curr_ltm <= 
                   sid_cnflt_res_list[sid_cnflt_res_entry].mcc2_min_ltm_with_dts))
         {
           found_mcc = sid_cnflt_res_list[sid_cnflt_res_entry].mcc2;
           is_timezone_match = TRUE;
         }
         else 
         {
           CM_ERR (" Conflict resoulution table is wrong %d, %d, %d", 
           curr_sid, curr_ltm, curr_daylt);
         }        
       }
       /* Daylt is OFF */   
       else if (curr_daylt == SYS_DAYLT_SAVINGS_OFF) 
       {
         CM_MSG_HIGH ("curr daylt is OFF curr ltm= %d",curr_ltm , 0, 0);

         if((curr_ltm >= 
             sid_cnflt_res_list[sid_cnflt_res_entry].mcc1_min_ltm_no_dts)
             && 
             (curr_ltm <= 
              sid_cnflt_res_list[sid_cnflt_res_entry].mcc1_max_ltm_no_dts)
            )
         {
           found_mcc = sid_cnflt_res_list[sid_cnflt_res_entry].mcc1;
           is_timezone_match = TRUE;
         }      
         else if ((curr_ltm >= 
                   sid_cnflt_res_list[sid_cnflt_res_entry].mcc2_min_ltm_no_dts )
                   && 
                   (curr_ltm <= 
                    sid_cnflt_res_list[sid_cnflt_res_entry].mcc2_max_ltm_no_dts))
         {
           found_mcc = sid_cnflt_res_list[sid_cnflt_res_entry].mcc2;
           is_timezone_match = TRUE;
         }
         else 
         {
           CM_ERR (" Conflict resoultin table is wrong %d, %d, %d", curr_sid, 
                   curr_ltm, curr_daylt);
         }     
       }    
       /* Daylt is not ON or OFF  */
       else 
       {
         CM_MSG_HIGH ("curr daylt is not in ON/OFF curr itm=%d", curr_daylt ,
                       0, 0);
       }    

       if (is_timezone_match == TRUE) 
       {

          countries_with_sid_ltm_cnft->country_code_len = 1;

          countries_with_sid_ltm_cnft->country_code_list[0] =
                        found_mcc;

          countries_with_sid_ltm_cnft->present = TRUE;

          countries_with_sid_ltm_cnft->sid     = curr_sid;       

          CM_MSG_HIGH ("cft and res table used, mcc = %d", 
            found_mcc, 0, 0);
       }    
       else 
       {
          CM_MSG_HIGH ("not found through cft and res table,    mcc = %d",
            found_mcc, 0, 0);
       }    
   }
   /* Populate the country and return if sid does not conflict */
  else 
  {
     CM_MSG_HIGH ("sid %d does not conflict", curr_sid, 0, 0);

     countries_with_sid_ltm_cnft->country_code_len = 0;

    for (list_index = 0; list_index < (int)ARR_SIZE(country_list);
         ++list_index)
    {
        if  ((curr_sid >= country_list[list_index].sid1) &&
           (curr_sid <= country_list[list_index].sid2))
        {
            countries_with_sid_ltm_cnft->country_code_len = 1;

            countries_with_sid_ltm_cnft->country_code_list[0] =
                                country_list[list_index].mobile_country_code;

            countries_with_sid_ltm_cnft->present = TRUE;

            countries_with_sid_ltm_cnft->sid     = curr_sid;        

            CM_MSG_HIGH ("Country FOUND, mcc = %d",
                     country_list[list_index].country_code, 0, 0);

            return;
          }
      }

    CM_MSG_HIGH("NO SID MATCH FOUND", 0, 0, 0);
    return;
  }
#else

  SYS_ARG_NOT_USED(curr_sid);
  SYS_ARG_NOT_USED(curr_ltm);
  SYS_ARG_NOT_USED(curr_daylt);
  SYS_ARG_NOT_USED(countries_with_sid_ltm_cnft);
  SYS_ARG_NOT_CONST(countries_with_sid_ltm_cnft);

#endif /* #if defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900) */
} /* cmutil_populate_country_list () */



/*===========================================================================
FUNCTION: cmutil_identify_current_country

DESCRIPTION

   Identifies current country given details like sid, ltm and daylt savings.


  Returns
  CMUTIL_CURRENT_COUNTRY_RESOLVED - curr_mcc is populated
  CMUTIL_CURRENT_COUNTRY_CONFLICT - curr_mcc is INVALID
  CMUTIL_CURRENT_COUNTRY_FAILED   - curr_mcc is INVALID

DEPENDENCIES
  None.

RETURN VALUE
  curr_mcc

SIDE EFFECTS
  None

===========================================================================*/
cmutil_current_country_e_type cmutil_identify_current_country (

  uint16 curr_sid,
    /* Sid of current system */

  int8   curr_ltm,
    /* Ltm offset of current system */

  sys_daylt_savings_e_type curr_daylt,
    /* daylt savings for current system */

  uint16 *curr_mcc
    /* current mcc */
)
{

  cm_ss_country_list_s_type countries_with_sid_ltm_cnft;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Sanity check */
  if(  !BETWEEN(curr_ltm, -26, 26)
     ||
     !BETWEEN(curr_daylt, SYS_DAYLT_SAVINGS_NONE, SYS_DAYLT_SAVINGS_MAX)
     ||
     !curr_mcc
   )
  {
    CM_ERR (" Error with input sid %d, ltm %d, daylt %d",
                              curr_sid, curr_ltm, curr_daylt);

    return CMUTIL_CURRENT_COUNTRY_FAILED;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_HIGH("curr_sid = %d, curr_ltm = %d, daylt = %d ",
              curr_sid, curr_ltm, curr_daylt);

  countries_with_sid_ltm_cnft.country_code_len = 0;

  *curr_mcc = CM_INVALID_MOBILE_COUNTRY_CODE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Populates list of countries that match
  ** curr_sid, curr_daylt and curr_ltm. By doing
  ** this we may resolve sid conflict
  */
  cmutil_populate_country_list (curr_sid, curr_ltm, curr_daylt,
                                 &countries_with_sid_ltm_cnft);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Return country code if there is just
  ** 1 LTM match
  */
  if (countries_with_sid_ltm_cnft.country_code_len > 1)
  {
    CM_MSG_HIGH ("Countries are in conflict", 0, 0, 0);
    return CMUTIL_CURRENT_COUNTRY_CONFLICT;
  }
  else if (countries_with_sid_ltm_cnft.country_code_len == 1)
  {
    *curr_mcc = countries_with_sid_ltm_cnft.country_code_list[0];
    CM_MSG_HIGH ("Country FOUND, mcc = %d", *curr_mcc, 0, 0);
    return CMUTIL_CURRENT_COUNTRY_RESOLVED;
  }
  else
  {
    CM_MSG_HIGH ("Country NOT FOUND ", 0, 0, 0);
    return CMUTIL_CURRENT_COUNTRY_FAILED;
  }
  
} /* cmutil_identify_current_country () */


/*===========================================================================

FUNCTION: cmutil_replace_plus_with_digits

DESCRIPTION
  Takes a number with + in first position and replaces it with digits.

DEPENDENCIES
  None.

RETURN VALUE
  CMUTIL_CONVER_RESULT_NUM_MODIFIED - converted_num_ptr has converted number
  CMUTIL_CONVER_RESULT_NUM_UNCHANGED - converted_num_ptr same as input
  CMUTIL_CONVER_RESULT_NUM_INPUT_ERRa - converted_num_ptr same as input

SIDE EFFECTS
  converted_num carries the converted digits

===========================================================================*/
cmutil_conver_result_e_type cmutil_replace_plus_with_digits (

 const cm_num_details_for_conversion_s_type  *num_details_for_conv_ptr,
  /* Number that needs to be converted */

 cm_num_s_type                               *converted_num_ptr
    /* Output carrying the converted number */
)
/*lint -esym(818,converted_num_ptr)*/
{
#if defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)
  uint8 conversion_digits[CMUTIL_PLUS_REPL_DIGIT_LEN];
    /* Carries digits that replace plus */

  int conversion_digits_len = 0;
    /* Length of conversion_digits */

  char serving_cc_str [CMUTIL_COUNTRY_CODE_LEN];
    /* Carries country code */

  int  loop = 0;

  /* offset within the plus number from
  ** where numbers can be taken
  */
  int  pos_to_copy_num_from = 1;

  int  spc_position = 0;

  uint16 serving_mcc = 0;

  const cm_num_s_type *num_with_plus_ptr = NULL;
    /* Points to input with plus */

  cm_country_code_type home_mcc    = CM_INVALID_MOBILE_COUNTRY_CODE;
    /* Home MCC */

  cm_country_code_type current_mcc = CM_INVALID_MOBILE_COUNTRY_CODE;
    /* Current MCC */

  cm_call_type_e_type  curr_call_type = CM_CALL_TYPE_NONE;
    /* Call type for which conversion is being made */

  int pos_of_first_digit = 0;
    /* Gives the starting position of digits with/without plus */

  boolean   is_destn_international = FALSE;
  /* Is the destn for the call national/intern */

  boolean   is_serving_mcc_found = FALSE;
    /* serving mcc in list of countries */

  int       digit_pos_for_converted_num = 0;
    /* Carries position where num has to be copied in converted num */

  int       serving_mcc_loop_indexes [CMUTIL_MCC_LOOP_LEN];
    /* Carries loop indexes that match a serving_mcc */

  int       serving_mcc_loop_index_len = 0;
    /* Length of serving_mcc_loop_indexes[] */

  boolean   is_spc_match_found = FALSE;
    /* Is there a spc match for the country and the dialed number */

  int       i = 0;
    /* Loop counter */

  int       list_entry = 0;
    /* Entry into country_list */

  uint8     dtmf_val  = 0;
    /* carries dtmf value */

    
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Verify input */
  if (
       !num_details_for_conv_ptr
       ||
       !converted_num_ptr
     )
  {
    CM_ERR ("Invalid input to cmutil_replace_plus_with_digits", 0, 0, 0);

    return CMUTIL_CONVER_RESULT_NUM_INPUT_ERR;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Copy details from input */

  num_with_plus_ptr = &num_details_for_conv_ptr->num;

  /* Current country */
  current_mcc       = num_details_for_conv_ptr->current_mobile_country_code;

  /* Mobile's Home country */
  home_mcc          = num_details_for_conv_ptr->home_mobile_country_code;

  /* Voice, SMS  */
  curr_call_type    = num_details_for_conv_ptr->call_type;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Verify input */
  if ( current_mcc == CM_INVALID_MOBILE_COUNTRY_CODE
       ||
       home_mcc == CM_INVALID_MOBILE_COUNTRY_CODE
       ||
       num_with_plus_ptr->len > (uint8)ARR_SIZE(num_with_plus_ptr->buf) /* !!! cm_num_validate() instead !!!! */
       ||
       num_with_plus_ptr->len > (uint8)ARR_SIZE(converted_num_ptr->buf)
     )
  {
    CM_ERR ("Invalid input to cmutil_replace_plus_with_digits", 0, 0, 0);

    return CMUTIL_CONVER_RESULT_NUM_INPUT_ERR;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


 CM_FUNC_START( "cmutil_replace_plus_with_digits()",0,0,0 );

  /* Clear conver digits */
  memset (conversion_digits, 0, sizeof(conversion_digits));

  /* Clear CC string */
  memset (serving_cc_str, 0, sizeof(serving_cc_str));

  /* Clear array carrying mcc loop indexes */
  memset (serving_mcc_loop_indexes, 0, ARR_SIZE(serving_mcc_loop_indexes));

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  memcpy ((char *)converted_num_ptr, (char *)num_with_plus_ptr,
           sizeof(*converted_num_ptr));

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Either number should start with "+" or num_type
  ** should be CM_NUM_TYPE_INTERNATIONAL for plus
  ** code functionality to kick in
  */
  /*lint -save -e641 */
  if (! ((num_with_plus_ptr->buf[0] == '+') ||
         (num_with_plus_ptr->number_type == CM_NUM_TYPE_INTERNATIONAL)
        )
     )
  {
    CM_ERR("Num does not have + or NUM_TYPE_INTER", 0, 0, 0);

    return CMUTIL_CONVER_RESULT_NUM_UNCHANGED;
  }
  /*lint -restore */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If there is a plus in buffer need to skip it */
  if (num_with_plus_ptr->buf[0] == '+')
  {
    pos_of_first_digit = 1;
  }
  else
  {
    pos_of_first_digit = 0;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (curr_call_type == CM_CALL_TYPE_VOICE)
  {
    serving_mcc = current_mcc;
  }
  else if (curr_call_type == CM_CALL_TYPE_SMS)
  {
    serving_mcc = home_mcc;
  }
  else
  {
    CM_MSG_HIGH("Unrecognized call type %d, return", 
        curr_call_type, 0, 0);

    return CMUTIL_CONVER_RESULT_NUM_UNCHANGED;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* MCC */
  CM_MSG_HIGH("serving_mcc = %d", serving_mcc, 0, 0);

 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  is_serving_mcc_found = FALSE;

  /* Find entry in country list for serving MCC */
  for (i = 0; i < (int)ARR_SIZE(country_list); ++i)
  {
    if  (country_list[i].mobile_country_code == serving_mcc)
    {
      is_serving_mcc_found = TRUE;

      /* Record the loop indices that match serving_mcc */
      serving_mcc_loop_indexes [++ serving_mcc_loop_index_len] = i;
    }
  }

  if ( !is_serving_mcc_found || !serving_mcc_loop_index_len )
  {
    CM_MSG_HIGH("Country matching with MCC not found, return",
        0, 0, 0);

    return CMUTIL_CONVER_RESULT_NUM_UNCHANGED;
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  
  /* index of serving_mcc in country_list, array will
  ** have atleast one entry at this point.
  */
  CM_ASSERT (serving_mcc_loop_index_len > 0);

  loop = serving_mcc_loop_indexes[1];


  /* Convert country code into string */
   
  if ( !(std_strlprintf (serving_cc_str, ARR_SIZE(serving_cc_str),
                           "%d", country_list[loop].country_code)) )
  {
    CM_ERR (" Country code %d for loop %d Invalid",
                               country_list[loop].country_code, loop, 0 );
  }
  
   
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If outgoing number starts with serving country code, use NDD or SPC */
  if (memcmp ( (void *)&num_with_plus_ptr->buf[pos_of_first_digit],
               (void *) serving_cc_str,
               strlen(serving_cc_str)) == 0)
  {

    /* SPC when used is after Countrycode +.CC.SPC.Number */
    spc_position = pos_of_first_digit + (int)strlen(serving_cc_str);

    if ( !(spc_position < (int)ARR_SIZE(num_with_plus_ptr->buf)))
    {
      CM_ERR ("spc_position exceeds buffer size", 0, 0, 0 );

      return CMUTIL_CONVER_RESULT_NUM_UNCHANGED;
    }

    /* Need to strip + and country code.
    */
    pos_to_copy_num_from = pos_of_first_digit + (int)strlen(serving_cc_str);

   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Check for all SPCs that can correspond to matching mcc */
    for (i=1; i<= serving_mcc_loop_index_len; ++i)
    {

      /* Copy the entry index in country_list */
      list_entry = serving_mcc_loop_indexes[i];

      /* Checking for spc match with dialed number */
      if (  ((int)strlen(country_list[list_entry].special_prefix_code) != 0)
          &&
          (memcmp ( (void *) &num_with_plus_ptr->buf[spc_position],
                    (void *)  country_list[list_entry].special_prefix_code,
                     strlen (country_list[list_entry].special_prefix_code)) == 0)
       )
      {
        is_spc_match_found = TRUE;

        CM_MSG_HIGH ("Number contains SPC for cc=%d and spc=%d",
           country_list[list_entry].country_code,
           country_list[list_entry].special_prefix_code, 0);
      }

    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* If there is no SPC match, prepend NDD*/
    if (!is_spc_match_found)
    {
      CM_MSG_HIGH ("Number is being dialed with NDD for cc=%d",
                                   country_list[loop].country_code, 0, 0);

      conversion_digits_len =
                      (int) strlen (country_list[loop].national_direct_dial);

      /* KW complaints : Fix ?*/
      if ((conversion_digits_len > (int)CMUTIL_PLUS_REPL_DIGIT_LEN))
      {
        CM_ERR ("conversion_digits_len exceeds buffer size", 0, 0, 0 );
        /* Limit size the buffer size*/
       conversion_digits_len = (int)CMUTIL_PLUS_REPL_DIGIT_LEN;
      
      }
      
      memcpy (conversion_digits, country_list[loop].national_direct_dial,
               (size_t)conversion_digits_len);

    }

    /* Check for NATIONAL/INTERNATIONAL Destn */
    is_destn_international = FALSE;

  }
  /* If outgoing number does not start with serving CC, start with IDD */
  else
  {
    CM_MSG_HIGH ("Number is being dialed with IDD for cc=%d",
                    country_list[loop].country_code, 0, 0);

    conversion_digits_len =
                       (int) strlen (country_list[loop].inter_dialed_digits);
    /* KW complaints : Fix ?*/  
    if ((conversion_digits_len > (int)CMUTIL_PLUS_REPL_DIGIT_LEN))
    {
       CM_ERR ("conversion_digits_len exceeds buffer size", 0, 0, 0 );
       /* Limit size the buffer size*/
       conversion_digits_len = (int)CMUTIL_PLUS_REPL_DIGIT_LEN;
      
    }
    memcpy (conversion_digits, country_list[loop].inter_dialed_digits,
             (size_t)conversion_digits_len);

    /* Need to strip + only
    */
    pos_to_copy_num_from = pos_of_first_digit;

    /* Check for NATIONAL/INTERNATIONAL Destn */
    is_destn_international = TRUE;
  }

 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If Network support is available in the country
  ** do not convert plus into digits. SMS shouldn't be taken for this consideration. 
  */
  if ( (country_list[loop].is_network_soln) &&
       (is_destn_international) && (curr_call_type == CM_CALL_TYPE_VOICE)
     )
  {
    CM_MSG_MED ("Netw support avail for this country ", 0, 0, 0);

    /* Network soultion is present, so conversion should be placed*/    
     return CMUTIL_CONVER_RESULT_NUM_UNCHANGED;
  }
  else
  {
    /* Digits will be used in place of plus
    ** so all calls become national
    */
    /*lint -save -e641 */
    converted_num_ptr->number_type = CM_NUM_TYPE_NATIONAL;
    /*lint -restore */
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  CM_MSG_MED ("Digits being added to front ", 0, 0, 0);

  for (loop=0; loop<conversion_digits_len; ++loop)
  {
    CM_MSG_HIGH ("%c", conversion_digits[loop], 0, 0);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if additional digits can be accomodated in result */
  if ( (conversion_digits_len + num_with_plus_ptr->len) >
        (int)ARR_SIZE(converted_num_ptr->buf)
     )
  {
    CM_ERR ("Cannot accomodate result in output %d, %d",
             conversion_digits_len, num_with_plus_ptr->len, 0);

    return CMUTIL_CONVER_RESULT_NUM_UNCHANGED;
  }

 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy the conversion digits, NDD or IDD */
  for (loop=0; loop<conversion_digits_len; ++loop)
  {
    converted_num_ptr->buf[loop] = conversion_digits[loop];
  }

  /* Position after IDD or NDD has been added */
  digit_pos_for_converted_num = conversion_digits_len;

  /* Start copying Number part +.CC.Number or +.CC.SPCNumber */
  for (loop=pos_to_copy_num_from; loop<num_with_plus_ptr->len;
                                     ++loop, ++digit_pos_for_converted_num)
  {
    converted_num_ptr->buf[digit_pos_for_converted_num] =
                  num_with_plus_ptr->buf[loop];

  }

  /* Actual length of converted number */
  converted_num_ptr->len = (uint8)digit_pos_for_converted_num;

 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if digit_mode can be set to 4BIT_DTMF.
  ** 4BIT_DTMF is preferred over ASCII type for
  ** call origination, since it reduces the length
  ** of origination message.
  */
  converted_num_ptr->digit_mode = CM_DIGIT_MODE_4BIT_DTMF;

  /* Cannot be converted to DTMF if characters other
  ** than 0-9, *, # are used.
  ** One example is when input number is "+1858123+++".
  ** The trailing + should not be changed.
  */
  for (loop=0; loop < converted_num_ptr->len; ++loop)
  {
    if ( !(  cm_util_encode_dtmf ( converted_num_ptr->buf[loop],
                                   &dtmf_val
                                 )
          )
        )
    {
      converted_num_ptr->digit_mode = CM_DIGIT_MODE_8BIT_ASCII;
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Dialed digits for Voice calls are converted into DTMF
  ** by CP. Since SMS is sent as a encoded message directly by WMS,
  ** CM shld do it here
  **
  ** digit_mode == 4BIT_DTMF means that all the digits contained in number
  ** buffer can be converted into DTMF digits.
  */
  if ( (curr_call_type                == CM_CALL_TYPE_SMS)         &&
       /* Got converted from ASCII -> 4BIT */
       (num_with_plus_ptr->digit_mode == CM_DIGIT_MODE_8BIT_ASCII) &&
       (converted_num_ptr->digit_mode == CM_DIGIT_MODE_4BIT_DTMF)
     )
  {

    /* Loop through the digits to convert each of them
        ** to dtmf
    */
    for (loop=0; loop < converted_num_ptr->len; ++loop)
    {

      if ( !(cm_util_encode_dtmf ( converted_num_ptr->buf[loop],
                                   &dtmf_val ))
         )
      {
        CM_ERR ("encode_dtmf error, shld have been caught before", 
            0, 0, 0);
      }

      converted_num_ptr->buf[loop] = dtmf_val;
    }

  }

 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_HIGH ("Final dialed string has digit_mode %d",
                             converted_num_ptr->digit_mode, 0, 0);

  for (loop=0; loop<converted_num_ptr->len; ++loop)
  {
    CM_MSG_HIGH ("%c", converted_num_ptr->buf[loop], 0, 0);
  }

 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return CMUTIL_CONVER_RESULT_NUM_MODIFIED;

#else

  SYS_ARG_NOT_USED(num_details_for_conv_ptr);
  SYS_ARG_NOT_USED(converted_num_ptr);
  SYS_ARG_NOT_CONST(converted_num_ptr);

  return CMUTIL_CONVER_RESULT_NUM_UNCHANGED;

#endif /* #if defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900) */
} /* cmutil_replace_plus_with_digits () */
/*lint +esym(818,converted_num_ptr)*/

/*===========================================================================

FUNCTION: cmutil_replace_digits_with_plus

DESCRIPTION
  Takes a number with + in first position and replaces it with digits.

DEPENDENCIES
  None.

RETURN VALUE
  CMUTIL_CONVER_RESULT_NUM_MODIFIED - converted_num_ptr has converted number
  CMUTIL_CONVER_RESULT_NUM_UNCHANGED - converted_num_ptr same as input
  CMUTIL_CONVER_RESULT_NUM_INPUT_ERRa - converted_num_ptr same as input

SIDE EFFECTS
  converted_num carries the converted digits

===========================================================================*/
cmutil_conver_result_e_type cmutil_replace_digits_with_plus (

 const cm_num_details_for_conversion_s_type  *num_details_for_conv_ptr,
  /* Number that needs to be converted */

 cm_num_s_type                               *converted_num_ptr
    /* Output carrying the converted number */
)
/*lint -esym(818,converted_num_ptr)*/
{
#if defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)
  int idd_digits_len = 0;
    /* Length of idd_digits */

  char inter_dialed_digits[CMUTIL_IDD_DIGIT_LEN];
   /* International Dialed Digits*/ 
   
  int  loop = 0;

  /* offset within the plus number from
  ** where numbers can be taken
  */
  int  pos_to_copy_num_from = 1;

  uint16 serving_mcc = 0;

  const cm_num_s_type *num_ptr = NULL;
    /* Points to input number */

  cm_country_code_type home_mcc    = CM_INVALID_MOBILE_COUNTRY_CODE;
    /* Home MCC */

  cm_country_code_type current_mcc = CM_INVALID_MOBILE_COUNTRY_CODE;
    /* Current MCC */

  cm_call_type_e_type  curr_call_type = CM_CALL_TYPE_NONE;
    /* Call type for which conversion is being made */

  boolean   is_serving_mcc_found = FALSE;
    /* serving mcc in list of countries */

  int       digit_pos_for_converted_num = 0;
    /* Carries position where num has to be copied in converted num */

  int       serving_mcc_loop_indexes [30];
    /* Carries loop indexes that match a serving_mcc */

  int       serving_mcc_loop_index_len = 0;
    /* Length of serving_mcc_loop_indexes[] */

  int       i = 0;
    /* Loop counter */

  uint8     dtmf_val  = 0;
    /* carries dtmf value */

 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Verify input */
  if (
       !num_details_for_conv_ptr
       ||
       !converted_num_ptr
     )
  {
    CM_ERR ("Invalid input to cmutil_replace_plus_with_digits", 0, 0, 0);

    return CMUTIL_CONVER_RESULT_NUM_INPUT_ERR;
  }

  /* Copy details from input */
  num_ptr = &num_details_for_conv_ptr->num;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If number type is international call (CM_NUM_TYPE_INTERNATIONAL), then
  ** HPCD should bypass. so returns CMUTIL_CONVER_RESULT_NUM_UNCHANGED
  */
  /*lint -save -e641 */
   if (  num_ptr->number_type == CM_NUM_TYPE_INTERNATIONAL) 
   {
      CM_MSG_HIGH("HPCD number type = %d", 
        num_ptr->number_type , 0, 0);

      return CMUTIL_CONVER_RESULT_NUM_UNCHANGED;
   }
   /*lint -restore */
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
  /* Current country */
  current_mcc       = num_details_for_conv_ptr->current_mobile_country_code;

  /* Mobile's Home country */
  home_mcc          = num_details_for_conv_ptr->home_mobile_country_code;

  /* Voice, SMS  */
  curr_call_type    = num_details_for_conv_ptr->call_type;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Verify input */
  if ( current_mcc == CM_INVALID_MOBILE_COUNTRY_CODE
       ||
       home_mcc == CM_INVALID_MOBILE_COUNTRY_CODE
       ||
       num_ptr->len > (uint8)ARR_SIZE(num_ptr->buf) /* !!! cm_num_validate() instead !!!! */
       ||
       num_ptr->len > (uint8)ARR_SIZE(converted_num_ptr->buf)
     )
  {
    CM_ERR ("Invalid input to cmutil_replace_digits_with_plus", 0, 0, 0);

    return CMUTIL_CONVER_RESULT_NUM_INPUT_ERR;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_MED("cmutil_replace_digits_with_plus ", 0, 0, 0);

  
  /* Clear array carrying mcc loop indexes */
  memset (serving_mcc_loop_indexes, 0, ARR_SIZE(serving_mcc_loop_indexes));

  /* Clear IDD string */
  memset (inter_dialed_digits, 0, sizeof(inter_dialed_digits));


 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy given number directly to converted_num */
  // strncpy ((char *)&converted_num_ptr->buf, (char *)&num_with_plus_ptr->buf,
  //          num_with_plus_ptr->len);
  memcpy ((char *)converted_num_ptr, (char *)num_ptr,
           sizeof(*converted_num_ptr));

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (curr_call_type == CM_CALL_TYPE_VOICE)
  {
    serving_mcc = current_mcc;
  }
  else if (curr_call_type == CM_CALL_TYPE_SMS)
  {
    serving_mcc = home_mcc;
  }
  else
  {
    CM_MSG_HIGH("Unrecognized call type %d, return ", 
        curr_call_type, 0, 0);

    return CMUTIL_CONVER_RESULT_NUM_UNCHANGED;
  }

 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* MCC */
  CM_MSG_HIGH("serving_mcc = %d", serving_mcc, 0, 0);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  is_serving_mcc_found = FALSE;

  /* Find entry in country list for serving MCC */
  for (i = 0; i < (int)ARR_SIZE(country_list); ++i)
  {
    if  (country_list[i].mobile_country_code == serving_mcc)
    {
      is_serving_mcc_found = TRUE;

      /* Record the loop indices that match serving_mcc */
      serving_mcc_loop_indexes [++ serving_mcc_loop_index_len] = i;

      /* Found MCC : Can extract IDD*/
      break;   
    }
  }

  if ( !is_serving_mcc_found || !serving_mcc_loop_index_len )
  {
    CM_MSG_MED("Country matching with MCC not found , return", 
        0, 0, 0);

    return CMUTIL_CONVER_RESULT_NUM_UNCHANGED;
  }

  
  CM_ASSERT (serving_mcc_loop_index_len > 0);

  loop = serving_mcc_loop_indexes[1];

 
  /* Copy IDD from coutry list record to local variable*/
  idd_digits_len =
                       (int) strlen (country_list[loop].inter_dialed_digits);

  /* KW complaints : Fix ?*/
   if ((idd_digits_len > (int)CMUTIL_IDD_DIGIT_LEN))
   {
       CM_ERR ("idd_digits_len exceeds buffer size", 0, 0, 0 );
       /* Limit size the buffer size*/
       idd_digits_len = (int)CMUTIL_IDD_DIGIT_LEN;
      
   }
   memcpy (inter_dialed_digits, country_list[loop].inter_dialed_digits,
             (size_t)idd_digits_len);


  /* If Incomming number starts with  IDD */
  if (memcmp ( (void *)&num_ptr->buf[0],
               (void *) inter_dialed_digits,
               strlen(inter_dialed_digits)) == 0)
  {

    /* IDD is included in calling party number (caller ID) */
    pos_to_copy_num_from = (int)strlen(inter_dialed_digits);
    digit_pos_for_converted_num=0;
    
    /* Remove IDD in buf */ 
    for (loop=pos_to_copy_num_from; loop<num_ptr->len;
                                     ++loop, ++digit_pos_for_converted_num)
    {
      converted_num_ptr->buf[digit_pos_for_converted_num] =
                  num_ptr->buf[loop];
    }

    /* IDD is removed so  Num tyep should be INTERNATIONAL*/
    /*lint -save -e641 */   
    converted_num_ptr->number_type = CM_NUM_TYPE_INTERNATIONAL;
    /*lint -restore */
  
    /* Actual length of converted number */
    converted_num_ptr->len = (uint8)digit_pos_for_converted_num;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Check if digit_mode can be set to 4BIT_DTMF.
    ** 4BIT_DTMF is preferred over ASCII type for
    ** call origination, since it reduces the length
    ** of origination message.
    */
    converted_num_ptr->digit_mode = CM_DIGIT_MODE_4BIT_DTMF;

    /* Cannot be converted to DTMF if characters other
    ** than 0-9, *, # are used.
    ** One example is when input number is "+1858123+++".
    ** The trailing + should not be changed.
    */
    for (loop=0; loop < converted_num_ptr->len; ++loop)
    {
      if ((  cm_util_encode_dtmf ( converted_num_ptr->buf[loop],
                                     &dtmf_val
                                   )
            )
          )
      {
        converted_num_ptr->digit_mode = CM_DIGIT_MODE_8BIT_ASCII;
      }
    }

   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    if (  /* Got converted from ASCII -> 4BIT */
         (num_ptr->digit_mode == CM_DIGIT_MODE_8BIT_ASCII) &&
         (converted_num_ptr->digit_mode == CM_DIGIT_MODE_4BIT_DTMF)
       )
    {

      /* Loop through the digits to convert each of them
          ** to dtmf
      */
      for (loop=0; loop < converted_num_ptr->len; ++loop)
      {

        if ( !(cm_util_encode_dtmf ( converted_num_ptr->buf[loop],
                                     &dtmf_val ))
           )
        {
          CM_ERR ("encode_dtmf error, shld have been caught before", 
            0, 0, 0);
        }

        converted_num_ptr->buf[loop] = dtmf_val;
      }

    }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   CM_MSG_MED ("Final dialed string has digit_mode %d",
                             converted_num_ptr->digit_mode, 0, 0);

   for (loop=0; loop<converted_num_ptr->len; ++loop)
   {
      CM_MSG_HIGH ("%c", converted_num_ptr->buf[loop], 0, 0);
   }

   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   return CMUTIL_CONVER_RESULT_NUM_MODIFIED;


  }
  else
  {
    return CMUTIL_CONVER_RESULT_NUM_UNCHANGED;
  }
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#else

  SYS_ARG_NOT_USED(num_details_for_conv_ptr);
  SYS_ARG_NOT_USED(converted_num_ptr);
  SYS_ARG_NOT_CONST(converted_num_ptr);

  return CMUTIL_CONVER_RESULT_NUM_UNCHANGED;

#endif /* #if defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900) */
} /* cmutil_replace_plus_with_digits () */
/*lint +esym(818,converted_num_ptr)*/

/*===========================================================================

FUNCTION: cm_util_perform_num_conversion

DESCRIPTION
  This function performs the following conversions

   - For CONV_REQ_PLUS_TO_DIGITS replaces starting "+" of number with
     digits.

   - For CONV_REQ_DIGITS_TO_PLUS replaces specific length of starting
     digits with "+".


DEPENDENCIES
  None.

RETURN VALUE
 If it is CMUTIL_NUM_CONVERSION_STATUS_SUCCESS
 converted_num_ptr carries changed number.

SIDE EFFECTS
  converted_num carries the converted digits

===========================================================================*/
cmutil_conver_result_e_type cm_util_perform_num_conversion(

  cmutil_num_conv_req_e_type               num_conv_req,
    /* Convert from plus to digits or digits to plus */

  const cm_num_details_for_conversion_s_type      *num_details_for_conv_ptr,
    /* Holds actual number on which conversion has
    ** to be performed
    */

  cm_num_s_type                             *converted_num_ptr /* !!! Check CM return convention !!!! */
    /* Converted number as a result of operation
    */

)
/*lint -esym(818,converted_num_ptr)*/
{

  cmutil_conver_result_e_type conver_result = CMUTIL_CONVER_RESULT_NONE;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!num_details_for_conv_ptr || !converted_num_ptr)
  {
    CM_ERR (" One of the input param is NULL ", 0, 0, 0);

    return CMUTIL_CONVER_RESULT_NUM_INPUT_ERR;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_HIGH (" num conv req %d, number length %d",
                  num_conv_req, num_details_for_conv_ptr->num.len, 0);

 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Copy given number for conversion to converted num */
  memcpy (converted_num_ptr, &(num_details_for_conv_ptr->num),
          sizeof(*converted_num_ptr));

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* !!!! Copy it into return_converted_num_ptr !!!!!*/
  if (num_details_for_conv_ptr->hs_based_plus_dial_setting ==
             CM_HS_BASED_PLUS_DIAL_DISABLED)
  {
    CM_ERR ("Plus dial is disabled ", 0, 0, 0);

    return  CMUTIL_CONVER_RESULT_NUM_UNCHANGED;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process request to convert plus to digits*/
  if (num_conv_req == CMUTIL_NUM_CONV_REQ_PLUS_TO_DIGITS)
  {

    conver_result = cmutil_replace_plus_with_digits (
                            num_details_for_conv_ptr,
                            converted_num_ptr );

  }
  /* Process request to convert digits to plus*/
  else if (num_conv_req == CMUTIL_NUM_CONV_REQ_DIGITS_TO_PLUS)
  {
    conver_result = cmutil_replace_digits_with_plus (
                            num_details_for_conv_ptr,
                            converted_num_ptr );  
  }
  else
  {
    CM_ERR ("Invalid conversion request %d", num_conv_req, 0, 0);
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If conversion failed clear converted_num */

  return conver_result;
}

/*===========================================================================

FUNCTION: cm_util_init_num_conversion_params

DESCRIPTION
  Initializes parameters that will be used in num conversion function

DEPENDENCIES
  None.

RETURN VALUE
  Initialized parameters

SIDE EFFECTS
  None
===========================================================================*/
void cm_util_init_num_conversion_params(

  cmutil_num_conv_req_e_type               *num_conv_req_ptr,
    /* Convert from plus to digits or digits to plus */

  cm_num_details_for_conversion_s_type      *num_details_for_conv
    /* Holds actual number on which conversion has
    ** to be performed
    */

)
/*lint -esym(818,num_conv_req_ptr)*/
/*lint -esym(818,num_details_for_conv)*/
{

  *num_conv_req_ptr = CMUTIL_NUM_CONV_REQ_NO_REQ;

  num_details_for_conv->call_type                   = CM_CALL_TYPE_VOICE;

  num_details_for_conv->current_mobile_country_code = CM_INVALID_MOBILE_COUNTRY_CODE;

  num_details_for_conv->home_mobile_country_code    = CM_INVALID_MOBILE_COUNTRY_CODE;

  num_details_for_conv->hs_based_plus_dial_setting  = CM_HS_BASED_PLUS_DIAL_DISABLED;

  cm_num_init(&num_details_for_conv->num);


} /* cm_util_init_num_conversion_params () */

/*===========================================================================

FUNCTION: cm_util_country_info_for_dialing

DESCRIPTION
  provide country network information for plus coding dialing but actully 
  a assisted dialing.  

DEPENDENCIES
  None.

RETURN VALUE
 
  country_info_ptr carries country information corresponding mcc 


SIDE EFFECTS
  None
===========================================================================*/
cmutil_country_info_result_e_type cm_util_country_info_for_dialing(

  cm_country_code_type req_mcc,
     /* Requested MCC, input is invalid mcc,
     ** we try to provide serving mcc  
     */ 

  cmutil_multi_country_specific_record_s *country_info_ptr
    /* Country network information corresponding to MCC*/ 

)  
/*lint -esym(818,country_info_ptr)*/
{
    #if defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)

    cmutil_country_info_result_e_type is_found = 
               CMUTIL_COUNTRY_INFO_RESULT_NOT_FOUND;
    /* flag for MCC found*/

    int          list_entry = 0;
    /* inex for entire table*/
   
    cmss_s_type  *ss_ptr       = cmss_ptr();
      /* point at serving_system object */

    uint8 found_entry_list=0;
    /* index for multiple MCC (SPC)*/  

  
    /* We consider that client does not know current serving mcc, 
    ** so we provide current mcc 
    */
     
    if( req_mcc ==   CM_INVALID_MOBILE_COUNTRY_CODE)
    {
       if(( req_mcc = ss_ptr->info.current_mcc) == 
             CM_INVALID_MOBILE_COUNTRY_CODE) 
       {
          CM_ERR (" Country info dialing : Invalid MCC ", 0, 0, 0);
          return CMUTIL_COUNTRY_INFO_RESULT_MCC_ERR;
       }        
    }

    /* Find entry in country list for serving MCC */
    for(list_entry = 0; list_entry < (int)ARR_SIZE(country_list); 
        ++list_entry)
    {
        if(country_list[list_entry].mobile_country_code == req_mcc)
        {
           CM_MSG_HIGH("Country info Reqeust mcc found : mcc  = %d index=%d", 
            req_mcc, list_entry, 0);

           memcpy(&country_info_ptr->country_entry[found_entry_list],
                  &country_list[list_entry],
                  sizeof(cmutil_country_specific_record_s));
   
           if (++found_entry_list >= CMUTIL_MAX_ALLOWED_DUP_ENTRY)
           {
               CM_ERR (" Country info dialing : MAX entries  ", 0, 0, 0);
               break;   
           }
        }
    }

    if(found_entry_list>0)
    {
        is_found= CMUTIL_COUNTRY_INFO_RESULT_FOUND;
        country_info_ptr->entry_number = found_entry_list; 
    }
    
    return is_found;
    
    #else
      SYS_ARG_NOT_USED(req_mcc);
      SYS_ARG_NOT_USED(country_info_ptr);
      return CMUTIL_COUNTRY_INFO_RESULT_NOT_FOUND; 
    #endif /* #if defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900) */
          
}
/*lint +esym(818,country_info_ptr)*/

/*===========================================================================

FUNCTION: cmutil_get_emergency_num_count

DESCRIPTION
  Returns the maximum number of emergency calls
  Based on the feature GSM or WCDMA, this value will differ

DEPENDENCIES
  None.

RETURN VALUE
  Initialized parameters

SIDE EFFECTS
  None
===========================================================================*/

int cmutil_get_emergency_num_count()
{
  #if (defined(FEATURE_GSM) || defined(FEATURE_WCDMA))
#error code not present
  #else
  return CM_MIN_EMERGENCY_NUM_COUNT;
  #endif
}

