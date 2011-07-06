/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

  J O I N T   P H O T O G R A P H E R S  E X P E R T S   G R O U P   (JPEG)   
               D E C O D E R   H E A D E R   F I L E

GENERAL DESCRIPTION
  This header file contains the definitions necessary to interface to the
  JPEG decoder.
  

Copyright(c) 2005-2010 by QUALCOMM, Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/multimedia/stillimage/jpeg6k/jpegdecoder/main/latest/src/jpeg_huf.c#5 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/24/2010 gbc     Fixed a issue with component is null when decoding.
12/03/10   chai    fixed klock work errors
04/01/2009 sigu    Fixed the 'output picture is broken for YUV422 progressive 
                   decoding'
04/08/08   sigu    Fixed Klocwork errors for 6800 6.4 target.
11/27/06   vma     Add boundary checking on updating last AC coefficient in 
                   progressive decoding (1st phase fix, avoided buffer overrun
                   but not guaranteeing correctness of decoding)
04/24/06   arv     Fixed the issue where the jpeg decoding fails for progressive images  
                   having three or more DC tables
04/18/06   arv     Fixed the issue where the derived huffman tables were generated
                   incorrectly.
04/17/06   arv     Fixed the issue in jpeg decoding for progressive images where
                   the next block address was incremented incorrectly.
03/13/06   vma     Fixed all Lint errors
03/12/06   arv     Fixed the issue where the huffman tables were passed 
                   not based on the index specified by the SOS marker
01/30/06   arv     Fixed the issue in huffman decoding where few bytes are 
                   missing at the end of jpeg bitstream
01/09/06   arv     Fixed the issue for progressive decoding where the AC Scan 
                   index and component selector were passed incorrectly
                   Fixed the issue in decoding progressive jpeg picture that 
                   are non x16
11/28/05   arv     Fixed Lint errors - Phase 1
11/01/05   pdo     Software quality - removed lints.
09/16/05   pdo     Removed number of lint warnings - to be con'ted 
08/24/05   dl      Implement error recovery using restart marker.
03/18/05   pdo     Initial version

===========================================================================*/

/*===========================================================================

                       INCLUDE FILES FOR MODULE

===========================================================================*/

#include "msg.h"                /* Message logging/reporting services      */
#include "err.h"                /* Error logging/reporting services        */

#ifdef FEATURE_JPEG_DECODER
#include  "jpeg_huf.h"
#include  "jpeg_mrk.h"
#include  "jpegi.h"

#define   DCT_coeff int16
#define   DCTSIZE2  64

typedef struct 
{
  DCT_coeff  *coef_ptr;
  uint32      id, tbl;
} component_type;


uint8            huffsize[257];
uint32           huffcode[257];
jpeg_huff_tbl    dc_huff[4], ac_huff[4];
extern jpeg_derived_tbl dc_derived_table[4], ac_derived_table[4];
component_type   component;
extern MCU_info_type    mcu_info;
int32            start_bits;
/* internal definition for AC and DC component decoding */
typedef void (*jpeg_progdecode_func_ptr_type) (MCU_info_type mcu,uint32 num_comp, const uint32 compSel[]);
extern int32  startOfSpectralSel, endOfSpectralSel, succApproxBitLow, succApproxBitHigh, bitsRemaining ;
extern comp_data_type *comp_data;
extern uint16 restart_interval,restarts_left;
//JPEG_subsample_type subsampleType;
extern int32   last_dc_val[];
extern int32   EOBrun;
extern uint32  bitPack;
extern uint32  mcusperRow;
extern uint32  mcu_to_skip;
extern uint32  numberOfComponentsInFrame;
extern uint32  numberOfComponentsInScan;
extern boolean prog_flag; 
extern JPEG_subsample_type progSubSampleType;
extern uint32  scanDCTableSelector[JPGD_MAXCOMPONENTS];
extern uint32  scanACTableSelector[JPGD_MAXCOMPONENTS];
extern uint16  jpegd_imageWidth;
extern uint32  progCompSel;
extern uint32  progScanAC;

// Tables and macro used to fully decode the DPCM differences.
// (Note: In x86 asm this can be done without using tables.)
LOCAL const int32 extend_test[16] =   /* entry n is 2**(n-1) */
{ 0, 0x0001, 0x0002, 0x0004, 0x0008, 0x0010, 0x0020, 0x0040, 0x0080,
  0x0100, 0x0200, 0x0400, 0x0800, 0x1000, 0x2000, 0x4000};

/* entry n is (-1 << n) + 1. */
/*lint -save -e701, for clarity, keep the format as the reference code */
LOCAL const int32 extend_offset[16] = { 
  0, ((-1)<<1) + 1, ((-1)<<2) + 1, ((-1)<<3) + 1, ((-1)<<4) + 1,
  ((-1)<<5) + 1, ((-1)<<6) + 1, ((-1)<<7) + 1, ((-1)<<8) + 1,
  ((-1)<<9) + 1, ((-1)<<10) + 1, ((-1)<<11) + 1, ((-1)<<12) + 1,
  ((-1)<<13) + 1, ((-1)<<14) + 1, ((-1)<<15) + 1};
/*lint -restore */

  #define HUFF_EXTEND(x,s) ((x) < extend_test[s] ? (x) + extend_offset[s] : (x))

/* Forward declaration */

boolean  jpegd_processRestart(void); 

/*===========================================================================

Function: JPEG_HUFF_INIT

Description:  JPEG Huffman initialization

Return Value:  None

Comments: 

Side Effects:
============================================================================*/
void jpeg_huff_init()
{
  memset(huffsize,0, sizeof(huffsize));
  memset(huffcode,0, sizeof(huffcode));
  memset(dc_huff,0, sizeof(dc_huff));
  memset(ac_huff,0, sizeof(ac_huff));
}

/*===========================================================================

Function: JPEG_GEN_HUFF_TBL

Description:  Creates intermediate tables for efficient Huffman decoding.

Return Value:  None

Comments: 

Side Effects:
============================================================================*/
JPEGD_STATUS_MSG jpeg_gen_huff_tbl(uint32 index, boolean isDC, jpeg_derived_tbl derived_table[], uint32 table_index)
{
  uint32 idx_l, idx_k;
  uint32 code, idx_j;
  int32  si;
  uint32 lookbits,lastk;
  jpeg_huff_tbl * huffTbl;
  JPEGD_STATUS_MSG jpegd_statusMessage = JPEGD_DECODING_OK;

  /* Based on the Figure C.1 flow diagram */
  if (!prog_flag)
    huffTbl = isDC ? &dc_huff[table_index] : &ac_huff[table_index];
  else
    huffTbl = isDC ? &dc_huff[index] : &ac_huff[index];

  for(idx_k = 0, idx_l = 1; idx_l <= 16; idx_l++)
  {
    lastk = huffTbl->bits[idx_l];
    if((lastk + idx_k) >256)
    {
      MSG_ERROR("BAD Huffman table", 0, 0, 0);
    }
    for(idx_j = 1; idx_j <= lastk; idx_j++)
      huffsize[idx_k++] = (uint8) idx_l;
  }
  huffsize[idx_k] = 0;
  lastk = idx_k;

  derived_table[index].pub = huffTbl;

  /* Based on the Figure C.2 flow diagram */
  code = 0;
  si = huffsize[0];
  idx_k = 0;

  /* This loop is more suitable with do ... repeat as in the flow */
  while(huffsize[idx_k])
  {
    while(huffsize[idx_k] == si)
    {
      huffcode[idx_k++] = code;
      code++;
    }

    code <<= 1;
    si++;
  }

  /* Based on figure F.15 to derive decoder tables */
  for(idx_j=0, idx_l=1; idx_l<17; idx_l++)
  {
    if(huffTbl->bits[idx_l])
    {
      derived_table[index].offset[idx_l] = idx_j - huffcode[idx_j];
      idx_j += huffTbl->bits[idx_l];
      if ((idx_j != NULL) && (idx_j <257))
         derived_table[index].maxcode[idx_l] = (int32) huffcode[idx_j -1];
    }
    else /* bits[idx_l] == 0 */
    {
      derived_table[index].maxcode[idx_l]=-1;   /* marked as invalid code */
    } 
  }
  derived_table[index].maxcode[idx_l]=-1;         /* marked as invalid code */

  /* Derive additonal tables for speedy decoding */
  /* first let's initialize the look_nbits array */

  memset(derived_table[index].look_nbits, 0, sizeof(derived_table[index].look_nbits));

  /* Need to initialize the look_nbits array before build it up */
  for(idx_j=0, idx_l=1; idx_l <= HUFF_LOOKAHEAD; idx_l++)
  {
    for(idx_k=1; idx_k <= huffTbl->bits[idx_l]; idx_k++,idx_j++)
    {
      lookbits = huffcode[idx_j] << (HUFF_LOOKAHEAD -idx_l);

      if (lookbits < (1<<HUFF_LOOKAHEAD))
      {
        for( si=1 << (HUFF_LOOKAHEAD -idx_l); si >0; si--)
        {
          derived_table[index].look_nbits[lookbits] = idx_l;
          derived_table[index].look_sym[lookbits] = huffTbl->huffval[idx_j];
          lookbits++;
        }
      }
      else
      {
        return JPEGD_INVALID_DHT_MARKER;
      }
    }
  }

  /* if isDC table then only certain symbols are legal */
  if(isDC)
  {
    for( idx_k=0; idx_k <lastk; idx_k++)
    {
      code = huffTbl->huffval[idx_k];
      if((code ==0) || (code > 15))
        return jpegd_statusMessage;
    }
  }

  return jpegd_statusMessage;
}

/*===========================================================================

Function: JPEG_READ_DHT

Description:  Read a huffman table from a jpeg bitstream

Return Value: Boolean

Comments: Assumming the bitstream pointer is right after the DHT marker

Side Effects:
============================================================================*/
boolean  jpeg_read_dht(uint8 bits[],uint8 huff_val[])
{
  uint32 i, cnt, index, size;
  
  /* Base on Table B.5 of spec. to read in the Huffman table */
  /* Fetch the size of this chunk */
  size = fetchBits(16,FALSE);
  size -= 2;            /* not include the size */

  /* There are minimum of 2 portions in each table     */
  /* Portion 1 is the BITS number of the code          */
  /* Portion 2 is the VALUE of the code                */
  /* There could be more than one table in a DHT chunk */
  while(size > 16)
  {
    index   = fetchBits(8, FALSE);
    if(index & 0xEC)
    {
      MSG_ERROR ("Bad DHT table class and destination", 0, 0, 0);
      return(FALSE);
    }

    bits[0] = 0;
    cnt     = 0;

    for(i = 1; i < 17; i++)
    {
      bits[i] = (uint8) fetchBits(8, FALSE);
      cnt += bits[i];
    }

    if(cnt > 256)
    {
      MSG_ERROR ("Bad DHT cnt", 0, 0, 0);
      return(FALSE);
    }

    for(i = 0; i < cnt; i++)
      huff_val[i] = (uint8) fetchBits(8, FALSE);

    /* subtract the number of BITS and the BITS total count */
    size -= (17 + cnt);

    /* check for table class AC or DC  */
    if(index & 0x10)
    {
      /* This is AC table */
      memcpy(ac_huff[index&0x03].bits,bits, 17);
      memcpy(ac_huff[index&0x03].huffval,huff_val, cnt);
      ac_huff[index&0x03].read = TRUE;
    }
    else
    {
      memcpy(dc_huff[index&0x03].bits,bits, 17);
      memcpy(dc_huff[index&0x03].huffval,huff_val, cnt);
      dc_huff[index&0x03].read = TRUE;
    }
  }
  return TRUE;
}

/*===========================================================================

Function:  JPEG_CHECK_HUFF_TABLES

Description:  Verify that all required Huffman tables are available 
              for this scan.

Return Value:  JPEGD_STATUS_MSG

Comments: 

Side Effects:
============================================================================*/
JPEGD_STATUS_MSG jpeg_check_huff_tables(uint32 comps_in_scan)
{
  uint32 i, huff_tbl_set;

  JPEGD_STATUS_MSG jpegd_statusMessage = JPEGD_DECODING_OK;

  /* There areonly 2 set of tables for 3 components */
  huff_tbl_set = (comps_in_scan == 3) ? 2 : 1;

  for(i = 0; i < huff_tbl_set; i++)
  {
    if((startOfSpectralSel == 0) && (!dc_huff[scanDCTableSelector[i]].read))
    {
      jpegd_statusMessage = JPEGD_INVALID_DHT_TABLE;
      MSG_ERROR ("JPEG Decoder: Error in Huffman Table", 0, 0, 0);
      return jpegd_statusMessage;
    }

    if((endOfSpectralSel > 0) && (!ac_huff[scanACTableSelector[i]].read))
    {
      jpegd_statusMessage = JPEGD_INVALID_DHT_TABLE;
      MSG_ERROR ("JPEG Decoder: Error in Huffman Table", 0, 0, 0);
      return jpegd_statusMessage;
    }
  }

  for(i = 0; i < 4 ; i++)  //4 is max. number of huffman tables
  {
    if (JPEGD_DECODING_OK == jpegd_statusMessage)
    {
      if(dc_huff[scanDCTableSelector[i]].read)
      {
        jpegd_statusMessage = jpeg_gen_huff_tbl(i, TRUE, dc_derived_table, scanDCTableSelector[i]);
      }
    }

    if (JPEGD_DECODING_OK == jpegd_statusMessage)
    {
      if(ac_huff[scanACTableSelector[i]].read)
      {
        jpegd_statusMessage = jpeg_gen_huff_tbl(i, FALSE, ac_derived_table, scanACTableSelector[i]);
      }
    }
  }

  return jpegd_statusMessage;
}

/*===========================================================================
Function: jpeg_huffDecode

Description:  Huffman decode next symbol in the bit-stream.

Return Value: JPEGD_STATUS_MSG

Comments: 

Side Effects:
============================================================================*/
uint32 jpeg_huffDecode(const jpeg_derived_tbl *huff_tbl)
{
  int32  code;
  uint32 nbits;

  /* Use only for huffman debuging
  if (bits_left < 8)
     MSG_ERROR("Not enough bits in this decode symbol", 0, 0, 0);
  */
  
  code = (bitPack >> 24) & 0xFF;
  if( (nbits = huff_tbl->look_nbits[code])!= 0)
  {
    /* Found a symbol with nbits */
    (void) fetchBits((int32)nbits, TRUE);  
    return(huff_tbl->look_sym[code]);
  }
  else
  {
    start_bits = 9;
    /* At this point, assume we know the minimm code size, start with that size */

    code = (int32)fetchBits((int32)start_bits, TRUE);

    /* Based on Figure F.16 to decode a code */
    /* Check for valid code. If not, check the Huffman code one bit at a time. */
    while(code > huff_tbl->maxcode[start_bits])
    {
      code = (code*2) | (int32) fetchBits(1, TRUE); 
      start_bits++;
    }

    /* Make sure the code is not longer than 16 bits  */
    if(start_bits > 16)
    {
      MSG_ERROR("JPEG: code is longer than 16 bits !", 0, 0, 0);
      return 0;     /* return a known zero as the result */
    }

    return huff_tbl->pub->huffval[(uint32)code + huff_tbl->offset[start_bits]];
  } 
}


/*===========================================================================
Function: JPEG_NEXT_BLOCK_ADDR

Description:  Determine the address of next 8x8 block based on MCU index and
              the block number in the current MCU.

Return Value: Pointer to DCT coefficients, component ID

Comments: Access to global coefficient buffers, comp_data[]

Side Effects:
============================================================================*/
component_type jpeg_next_block_addr(MCU_info_type MCU, JPEG_subsample_type subsampleType,
                                    uint32 blkNum_inMCU, boolean isDC)
{
  /* Default component ID is 0 and it is not a common equation */
  boolean     common_eq = FALSE;
  uint32 MCUperRow = MCU.per_row;
  memset(&component, 0, sizeof(component));

  switch(subsampleType)
  {
  case JPEGD_GRAY:
    common_eq = TRUE;
    break;
  case JPEG_H2V2:
    switch(blkNum_inMCU)
    {
    case 0:
    case 1: /* Same as H2V1 regarding 1st & 2nd blocks in MCU  */ 
      component.coef_ptr = comp_data[0].seg_ptr + 
                           (MCU.in_col* MCU.per_row*comp_data[0].comp_v_samp*comp_data[0].comp_h_samp +
                            (MCU.in_row*comp_data[0].comp_h_samp + blkNum_inMCU))*64;
      break;
    case 2:
    case 3:  
      component.coef_ptr = comp_data[0].seg_ptr + 
                           (MCU.in_col* MCU.per_row*comp_data[0].comp_v_samp*comp_data[0].comp_h_samp + 
                            MCU.per_row*comp_data[0].comp_h_samp)*64 + (MCU.in_row*comp_data[0].comp_h_samp + blkNum_inMCU-2)*64;
      break;
    case 4:
      common_eq     = TRUE;
      component.id  = 1;
      component.tbl = 1;
      break;
    case 5:
      common_eq = TRUE;
      component.id = 2;
      component.tbl = 1;
      break;
    default:
      MSG_ERROR("invalid block ID in an MCU", 0, 0, 0);
    }
    break;
  case JPEG_H2V1:
    switch(blkNum_inMCU)
    {
    case 0:
    case 1:
      component.coef_ptr = comp_data[0].seg_ptr + (MCU.in_col* MCU.per_row*comp_data[0].comp_h_samp +
                                                   (MCU.in_row*comp_data[0].comp_h_samp + blkNum_inMCU))*64;
      break;
    case 2:
      common_eq     = TRUE;
      component.id  = 1;
      component.tbl = 1;
      break;
    case 3:
      common_eq     = TRUE;
      component.id  = 2;
      component.tbl = 1;
      break;
    default:
      MSG_ERROR("invalid block ID in an MCU", 0, 0, 0);
    }
    break;
  case JPEG_H1V2:
    switch(blkNum_inMCU)
    {
    case 0:
    case 1:
      component.coef_ptr = comp_data[0].seg_ptr + (MCU.in_col* MCU.per_row*comp_data[0].comp_v_samp + MCU.per_row*blkNum_inMCU)*64 +
                           (MCU.in_row*comp_data[0].comp_h_samp )*64;
      break;
    case 2:
      common_eq     = TRUE;
      component.id  = 1;
      component.tbl = 1;
      break;
    case 3:
      common_eq     = TRUE;
      component.id  = 2; 
      component.tbl = 1;
      break;
    default:
      MSG_ERROR("invalid block ID in an MCU", 0, 0, 0);

    }
    break;

  case JPEG_H1V1:
    common_eq = TRUE;
      
    if( numberOfComponentsInScan == 3)
    {
      component.id = blkNum_inMCU;
      component.tbl = blkNum_inMCU ? 1 : 0; 
    }
    else
    {
      component.id = progCompSel;
      component.tbl = progScanAC;
    } 
   
    if ((prog_flag) && (progSubSampleType == JPEG_H2V2) ||(progSubSampleType == JPEG_H2V1))
    {
      if (common_eq == FALSE)
        MCUperRow = MCU.per_row + 1;
      else if ((component.id == 0) && (MCU.per_row % 2) != 0)
        MCUperRow = MCU.per_row + 1;
    }

    break;

  default:
    component.coef_ptr = NULL;
    MSG_ERROR("invalid JEG subsample factor", 0, 0, 0);
  }

   if(prog_flag)
   {
     if ((subsampleType == JPEG_H2V2) && (blkNum_inMCU > 3))
       component.id = blkNum_inMCU - 3;
     else if ((subsampleType == JPEG_H2V1) && (blkNum_inMCU > 1))
       component.id = blkNum_inMCU - 1;

     /*To fix the Kloc work Critical errors*/
     /*Ensure that component.id never exceeds JPGD_MAXCOMPONENTS*/
     /*which is the maximum scanDCTableSelector or scanACTableSelector can hold*/
     if (component.id < JPGD_MAXCOMPONENTS)
     {
        if(isDC)
            component.tbl = scanDCTableSelector[component.id];
        else
            component.tbl = scanACTableSelector[component.id];
     }
     else 
     {
       MSG_ERROR("JPEG_HUF_C Invalid component.id %d", component.id, 0, 0);
     }

   }
 
  if(common_eq)
    component.coef_ptr = comp_data[component.id].seg_ptr + (MCU.in_col * MCUperRow + MCU.in_row)*64;

  return(component);
}
/*===========================================================================
Function: JPEG_DEC_MCU

Description:  Huffman decode one MCU worth of data in the bit-stream.

Return Value: None

Comments: It is necessary to know the component ID for every block

Side Effects:
============================================================================*/

boolean jpeg_dec_mcu (const MCU_info_type *mcu /*, uint32 blocks_per_MCU*/)
{
  uint32 cnt;
  uint32  s, k, r;

  /* Expect that it is at the beginning of a new MCU's bit-stream   */
  /* If it's Restart marker bit-stream then it is right after the marker */

  /* Perform 1 MCU worth of Huffman decoding */
  for(cnt = 0; cnt < mcu->blk_count ; cnt++)
  {
    /* Get the corresponding coefficient pointer for this block */
    /* How about component ID here?????  */
    component = jpeg_next_block_addr(*mcu, mcu->subsample, cnt, 0);

    if (component.coef_ptr == 0x0)
    {
        MSG_ERROR("jpeg_dec_mcu : component.coef_ptr = NULL",0,0,0);
        return FALSE;        
    }

    /*  To avoid memory corruption  */ 
    if(component.coef_ptr >= (comp_data[component.id].seg_ptr + 2*comp_data[component.id].coeff_size))
    {
      MSG_ERROR("Potential memory corruption detected",0,0,0);
      return TRUE;
    }

    /* Section F.2.2.1: decode the DC coefficient difference */
    s = jpeg_huffDecode(&dc_derived_table[component.tbl]);
    if(s)
    {
      r = fetchBits((int32)s, TRUE);
      if(s<16)
        s = (uint32)HUFF_EXTEND((int32)r, s);
    }

    /* This has to be for the DC predictor coefficient */
    /* Update last_dc_val on this component */
    last_dc_val[component.id] += (int32)s;
    /* Output the DC coefficient (assumes jpeg_natural_order[0] = 0) */
    component.coef_ptr[0] = (int16) last_dc_val[component.id];


    /* Section F.2.2.2: decode the AC coefficients */
    /* Assume coef_ptr area are preset to 0; zeroes will be skipped */
    for(k = 1; k < DCTSIZE2; k++)
    {
      s= jpeg_huffDecode(&ac_derived_table[component.tbl]);
      r = s >> 4;  /* This is RRRR in spec. */
      s &= 15;     /* This is SSSS in spec. */

      if(s)
      {
        k += r;
        if (k <= 63)
        {
          r = fetchBits((int32)s, TRUE);  
          if(s<16)
            s = (uint32)HUFF_EXTEND((int32)r, s);
          /* Output coefficient in natural order. */
          component.coef_ptr[k]= (int16) s;
        }
      }
      else /* that is SSSS = 0  */
      {
        if(r != 15)
          break;
        k += 15;
      }
    }

    /* Completed one block */
  }
  /* It is successfully done with one MCU */
  return TRUE;
}

/*===========================================================================
Function: JPEG_DEC_MCU_ROW

Description:  Huffman decode MCU rows from data in the bit-stream.

Return Value: None

Comments: Plan to make it decoding multiple MCU rows

Side Effects:
============================================================================*/

boolean jpeg_dec_mcu_row ( int32 num_row, uint32 MCU_per_row)
{
  uint32 k;
  int32 cnt;
  uint32 mcu_row, mcu_left_in_row;
  boolean  status = TRUE;
  for(cnt = 0; cnt < num_row; cnt++)
  {
    for(mcu_row = 0; mcu_row < MCU_per_row; mcu_row++)
    {
      if((restart_interval != 0) && (restarts_left == 0))
      {
        if(jpegd_processRestart() == FALSE)
          break;
      } 
      mcu_left_in_row = mcusperRow - mcu_row;
      if(mcu_to_skip >= mcu_left_in_row)
      {
        mcu_to_skip -= mcu_left_in_row;
        break;
      }
      else if(mcu_to_skip > 0)
      {
        for(k = 0; k < numberOfComponentsInFrame; k++)
        {
          comp_data[k].dataUnit_x_mcu += mcu_to_skip * comp_data[k].comp_h_samp;
        }
        mcu_row += mcu_to_skip;
        mcu_to_skip = 0;
      }

      status = jpeg_dec_mcu(&mcu_info);
      mcu_info.in_row++;
      restarts_left--;
    }
  }
  return status;
}
/*===========================================================================
Function: JPEG_DECODE_1st_DC

Description:  MCU decoding for DC initial scan (either spectral selection,
              or first pass of successive approximation).

Return Value:  boolean

Comments: 

Side Effects:
============================================================================*/
boolean jpeg_dec_1st_DC(MCU_info_type mcu, uint32 num_comp, const uint32 compSel[])
{
  uint32 cnt;
  int32 s, r;

  /* Perform 1 MCU worth of Huffman decoding */
  for(cnt = 0; cnt < mcu.blk_count; cnt++)
  {
    if(num_comp == 1) cnt = compSel[0];
    /* Get the corresponding coefficient pointer for this block */
    /* How about component ID here?????  */
    component = jpeg_next_block_addr( mcu ,  mcu.subsample, cnt, 1);

    /*  To avoid memory corruption  */ 
    if(component.coef_ptr >= (comp_data[component.id].seg_ptr + 2*comp_data[component.id].coeff_size))
    {
      MSG_ERROR("Potential memory corruption detected",0,0,0);
      return TRUE;
    }
    /* Section F.2.2.1: decode the DC coefficient difference */
    s = (int32)jpeg_huffDecode(&dc_derived_table[component.tbl]);

    if(s)
    {
      r = (int32)fetchBits(s, TRUE);  
      if(s<16)
      s = HUFF_EXTEND(r, s);
    }

    /* Update last_dc_val on this component */
    last_dc_val[component.id] += s;
    /* Output the DC coefficient (assumes jpeg_natural_order[0] = 0) */
    component.coef_ptr[0] = (int16)((uint32)last_dc_val[component.id]<< succApproxBitLow);
  }
  return TRUE;
}

/*===========================================================================

Function:  JPEG_DEC_1ST_AC

Description:  MCU decoding initial AC scan (either spectral selection,
              or first pass of successive approximation).

Return Value:  boolean

Comments: 

Side Effects:
============================================================================*/
boolean jpeg_dec_1st_AC(MCU_info_type mcu, uint32 num_comp, const uint32 compSel[])
{
  uint32 cnt;
  int32 r, k, s;

  for(cnt=0; cnt < mcu.blk_count; cnt++)
  {
    if(num_comp == 1) cnt = compSel[0];
    /* There is always only one block per MCU for AC */
    if(EOBrun > 0)           /* if this band is all 0  */
      EOBrun--;              /* skip it for this block */
    else
    {
      /* Get the corresponding coefficient pointer for this block */
      component = jpeg_next_block_addr( mcu , mcu.subsample, cnt, 0);

      /*  To avoid memory corruption  */ 
      if(component.coef_ptr >= (comp_data[component.id].seg_ptr + 2*comp_data[component.id].coeff_size))
      {
        MSG_ERROR("Potential memory corruption detected",0,0,0);
        return TRUE;
      }
      
      for(k = startOfSpectralSel; k <= endOfSpectralSel; k++) 
      {
        s = (int32)jpeg_huffDecode(&ac_derived_table[component.tbl]);
        r = s >> 4; /*lint !e704 actual data is less than 16 bits */
        s &= 15;
        if(s)
        {
          k += r;
          r = (int32)fetchBits(s, TRUE);  
          s = HUFF_EXTEND(r, s);
          /* Do successive approximation */
          component.coef_ptr[k] = (int16) ((uint32)s << succApproxBitLow);
        }
        else
        {
          if(r == 15) /* special case for ZRL */
          {
            k += 15;        /* skip 15 zeroes in band */
          }
          else
          {  /* EOBrun, run length is 2^r + appended bits */
            EOBrun = 1 << r;
            if(r)
            {   /* EOBrun, r > 0 */
              r = (int32)fetchBits(r, TRUE);   
              EOBrun += r;
            }
            EOBrun--;       /* process end of band at this moment */
            break;          /* force end-of-band */
          }
        }
      }  /* end of specral loop */
    } /* end of if EOBrun else */
  } /* end of for(cnt   */
  return TRUE;  /* Completed the band in this MCU */
}

/*===========================================================================

Function: JPEG_DEC_REFINE_DC

Description:  MCU decoding for DC successive approximation refinement scan.

Return Value:  boolean

Comments: 

Side Effects:
============================================================================*/
boolean jpeg_dec_refine_DC(MCU_info_type mcu, uint32 num_comp, const uint32 compSel[])
{
  uint32 cnt;
  DATAUNIT_DESC p1 = (DATAUNIT_DESC)(1 << succApproxBitLow);

  for(cnt = 0; cnt < mcu.blk_count; cnt++)
  {
    if(num_comp == 1) cnt = compSel[0];
    /* Get the corresponding coefficient pointer for this block */
    component = jpeg_next_block_addr( mcu , mcu.subsample, cnt, 1);

    /*  To avoid memory corruption  */ 
    if(component.coef_ptr >= (comp_data[component.id].seg_ptr + 2*comp_data[component.id].coeff_size) )
    {
      MSG_ERROR("Potential memory corruption detected",0,0,0);
      return TRUE;
    }

    /* Process DC for this MCU */
    /* Each refining data is 1 bit */
    if(fetchBits(1, TRUE) )
      component.coef_ptr[0] |= p1;
  }
  return TRUE;
}

/*===========================================================================

Function: JPEG_DEC_REFINE_AC

Description:  MCU decoding for AC successive approximation refinement scan.

Return Value:  boolean

Comments: 

Side Effects:
============================================================================*/
boolean jpeg_dec_refine_AC (MCU_info_type mcu, uint32 num_comp, const uint32 compSel[])
{
  int32 k, r, s;
  uint32 cnt;
  /* 1 in the bit position being coded */
  DATAUNIT_DESC p1 = (DATAUNIT_DESC)(1 << succApproxBitLow);      
  /* -1 in the bit position being coded */
  DATAUNIT_DESC m1 = (DATAUNIT_DESC)((-1) << succApproxBitLow); /*lint !e701 setup the mask*/   

  DATAUNIT_DESC cur_coef;

  for(cnt=0; cnt < mcu.blk_count; cnt++)
  {
    if(num_comp == 1) cnt = compSel[0];
    /* Get the corresponding coefficient pointer for this block */
    /* How about component ID here?????  */
    component = jpeg_next_block_addr( mcu , mcu.subsample, cnt, 0);

    /*  To avoid memory corruption  */ 
    if(component.coef_ptr >= (comp_data[component.id].seg_ptr + 2*comp_data[component.id].coeff_size) )
    {
      MSG_ERROR("Potential memory corruption detected",0,0,0);
      return TRUE;
    } 
    k = startOfSpectralSel;

    /* There is always only one block per MCU for AC */
    /* if this band is not all 0, process it */
    if(EOBrun == 0)
    {
      
      do
      {
        s = (int32)jpeg_huffDecode(&ac_derived_table[component.tbl]);
        r = s >> 4; /*lint !e704 actual data is less than 16 bits */
        s &= 15;

        if(s)
        {
          if(s != 1)
            MSG_ERROR("Huff bad code",0,0,0);
          if(fetchBits(1, TRUE) /*fetch_bits(1)*/)
            s = p1;
          else
            s = m1;
        }
        else /* note s = 0 for processing zero run length */
        {
          if(r != 15)
          {
            EOBrun = 1 << r; /* EOB, run length is 2^r + appended bits */
            if(r)
            {
              r = (int32)fetchBits(r, TRUE);
              EOBrun += r;
            }
            break;      /* rest of block is handled by EOB logic */
          }
        }
        /* Advance over already-nonzero coefs and r still-zero coefs,
        * appending correction bits to the nonzeroes.  A correction bit is 1
        * if the absolute value of the coefficient must be increased.
        */
        do
        {
          cur_coef = component.coef_ptr[k];
          if(cur_coef != 0)
          {
            if(fetchBits(1, TRUE))
            {
              if((cur_coef & p1) == 0)
              { /* do nothing if already set it */
                if(cur_coef >= 0)
                  component.coef_ptr[k] += p1;
                else
                  component.coef_ptr[k] += m1;
              }
            }
          }
          else
          {
            if(--r < 0)
              break;       /* reached target zero coefficient */
          }
          k++;
        } while(k <= endOfSpectralSel);

        if(s && k <= endOfSpectralSel)
          component.coef_ptr[k] = (int16) s;
      } while(k++ < endOfSpectralSel); // end of For loop
    }// end of if EOBrun

    if(EOBrun > 0)
    {
      /* Scan any remaining coefficient positions after the end-of-band
      * (the last newly nonzero coefficient, if any).  Append a correction
      * bit to each already-nonzero coefficient.  A correction bit is 1
      * if the absolute value of the coefficient must be increased.
        */
      do
      {
        cur_coef = component.coef_ptr[k];
        if(cur_coef != 0)
        {
          if(fetchBits(1, TRUE))
          {
            if((cur_coef & p1) == 0)
            { /* do nothing if already changed it */
              if(cur_coef >= 0)
                component.coef_ptr[k] += p1;
              else
                component.coef_ptr[k] += m1;
            }
          }
        }
      }  while(k++ < endOfSpectralSel);
      /* Count one block completed in EOB run */
      EOBrun--;
    }
  } /* end of for(cnt   */
  return TRUE;
}

/*===========================================================================

Function: JPEG_DECODE_PROG_SCAN

Description:  This function handles the progressive scans

Return Value:  Boolean

Comments: 

Side Effects:
============================================================================*/
boolean jpeg_decode_prog_scan(MCU_info_type mcu, uint32 num_comp, const uint32 compSel[])
{
  uint32  mcu_row_cnt, mcu_col_cnt;
  jpeg_progdecode_func_ptr_type decode_block;

  if( !succApproxBitHigh )
  {
    if(!startOfSpectralSel)
    {
      // This scan is first_DC
      decode_block = (jpeg_progdecode_func_ptr_type) jpeg_dec_1st_DC;
    }
    else
    {
      // This scan is first_AC
      decode_block = (jpeg_progdecode_func_ptr_type) jpeg_dec_1st_AC;
    }
  }
  else
  {
    if(!startOfSpectralSel)
    {
      // This scan is refine_DC
      decode_block = (jpeg_progdecode_func_ptr_type) jpeg_dec_refine_DC;
    }
    else
    {
      // This scan is refine_AC
      decode_block = (jpeg_progdecode_func_ptr_type) jpeg_dec_refine_AC;
    }
  }/* end of if (!successive */

  for(mcu_col_cnt=0; mcu_col_cnt < mcu.per_col; mcu_col_cnt++)
  {
    for(mcu_row_cnt=0; mcu_row_cnt < mcu.per_row; mcu_row_cnt++)
    {
      /* check for restart marker if requireded */
      if((restart_interval) && (restarts_left == 0))
        (void) jpegd_processRestart();

      decode_block(mcu, num_comp, compSel);
      mcu.in_row++;             
      restarts_left--;
    }/* done with one MCU row */
    mcu.in_row = 0;
    mcu.in_col++;

  }/* done with one scan */
  mcu.in_col = 0;

  /* align to next scan */
  (void) fetchBits(bitsRemaining & 7, TRUE);
  if(jpegd_initializeScan() != JPEGD_DECODING_OK)
    return FALSE; /* end of image detected */

  return TRUE;
}

#endif

