/*===========================================================================

                           IMAGE HEADER

DESCRIPTION
	This file is used to generate image headers in binary format.

Copyright(c) 2004,2005 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================
                      EDIT HISTORY FOR FILE

    This section contains comments describing changes made to this file.
    Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/qsc1100/tools/headergen/image_header.c#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/20/06   rjain   Support for cfg_data header in NOR builds
28/06/06   rjain   Support for relocatable QC-SBL + General formatting; user
                   can now specify a destination address for the QC-SBL image.
05/08/06   ly      Update to use KB based user partition instead of block based
                   partition
12/12/04   tkuo    Update for secboot mode.
10/10/04   tkuo    Fix bug with mix destination and source. Add load_partition
                   _entry() to get the source address of NAND.
09/28/04   erwinv  Added o|a command line argument
09/24/04   erwinv  Created

===========================================================================*/

#include <stdio.h>         /* Note: Library files must be included first. */
#include <string.h>
#include <sys/stat.h>
#include "miheader.h"
#include "mibib.h"
#include "miparti.h"

#if (USES_SECBOOT_2_0 == YES)
#include "boot_headers.h"
#endif
/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
/*---------------------------------------------------------------------------
  NOR bus width identifier definitions and magic number definition for
  identifying structure that points to boot configuration data in NOR.
---------------------------------------------------------------------------*/
#define NOR_16_BIT_ID   0x735A
#define NOR_32_BIT_ID   0x8CA5
#define NOR_MAGIC_NUM   0xebb5

/*---------------------------------------------------------------------------
  Magic number definition for identifying start of boot configuration data.
---------------------------------------------------------------------------*/
/*#define NOR_CODEWORD    0x00f4ebb5     /* Used for NOR only. */

/*---------------------------------------------------------------------------
  Magic number definitions for identifying good NAND blocks containing the
  SBL image.
---------------------------------------------------------------------------*/
#define SBL_MAGIC_NUM_1  0x5FE85DDF
#define SBL_MAGIC_NUM_2  0x5264CEBC

/*---------------------------------------------------------------------------
  Magic Number for AutoDetecting Page Size Algorithm
---------------------------------------------------------------------------*/
#define AUTODETECT_PAGE_SIZE_MAGIC_NUM  0x7D0B435A

/*---------------------------------------------------------------------------
  Page Size AutoDetect defines
---------------------------------------------------------------------------*/
#define AUTODETECT_1K_PAGE_SIZE             1024                 /* 1K Byte Page   */
#define AUTODETECT_MAX_PAGE_SIZE_SUPPORTED  (4*AUTODETECT_1K_PAGE_SIZE) /* 4K Bytes Page  */
#define AUTODETECT_MIN_PAGE_SIZE_SUPPORTED  (2*AUTODETECT_1K_PAGE_SIZE) /* 512 Bytes Page */

/*---------------------------------------------------------------------------
  Maximum size of configuration data.
---------------------------------------------------------------------------*/
#define MAX_SIZE_OF_VERIFY_BUFFER 0x2000        /* 8K bytes */

/*---------------------------------------------------------------------------
  Defines the codewords used to specify NAND data width, page size greater
  than 512 byte (future devices), SUPERAND sector size, and NAND device type
  of NAND or SUPERAND.  These codewords are located in the configuration data
  header.
---------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION  USAGE

DESCRIPTION
  This displays usage information for image_header.exe

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void usage(char *progname)
{
    printf("Usage:  %s flash_type header_type input_code_file output_header_file ", progname);
    printf("[signature_file cert_chain_file]\n");
    printf("\tflash_type -         nand or nor\n");
    printf("\theader_type -        oemsbl, amss, qcsbl, fsbl, dbl, osbl, appsbl, apps, hash, or hostdl\n");
    printf("\tinput_code_file -    file to produce header from\n");
    printf("\tfsbl_code_file -     fsbl image file to produce header from, requried only if input_code_file is dbl\n");
    printf("\toutput_header_file - file to write header to\n");
    printf("\n\t\tOptional args for secure boot:\n");
    printf("\t\tsignature_file -   input signature file\n");
    printf("\t\tcert_chain_file -  input certification file\n");
    printf("\n");
    return;
}

/*===========================================================================

FUNCTION  MAIN

DESCRIPTION
  This is the main entry point for image_auth.

DEPENDENCIES

RETURN VALUE
  None.
SIDE EFFECTS
  None.

===========================================================================*/
int main(int argc, char *argv[])
{
  /* Local Vars */
  int i = 0;
  char *code_file_name = NULL;      /* file to create header from */
  char *output_file_name = NULL;    /* file to create header into */
  char *signature_file_name = NULL; /* signature file filen name*/
  char *cert_chain_file_name = NULL;/* certificate chain file name*/
  char *image_file_name = NULL;     /* image file name*/
  char *header_type;                /* "amss" header or "oemsbl" header */
  char *flash_type;                 /* "nand" flash or "nor" flash */
  FILE *out_fp, *in_fp;
  unsigned int image_dest = 0, image_source = 0;
  unsigned int signature_size = 0;
  unsigned int cert_chain_size = 0;
  unsigned int code_size;
  unsigned int image_size;
  boolean is_nand;
  boolean change_qcsbl_dest;
  struct stat in_stats;
  image_type id = NONE_IMG;

  unsigned long nprg_dest_addr = 0;
  unsigned long qcsbl_dest_addr = 0;

#if (USES_SECBOOT_2_0 == YES)
  char *sbl_file_name = NULL;
  boot_sbl_header_type sbl_header;
  sbl_header.codeword = 0;
#endif
  uint32 header[10] =
  {
    NONE_IMG,                 /* image_type image_id */
    FLASH_PARTITION_VERSION,  /* header_vsn_num */
    0, /* image_src. DBL can read image source address from system partition, src address is not set here*/
    0, /* image_dest_ptr */
    0, /* image_size */
    0, /* code_size */
    0, /* signature_ptr */
    0, /* signature_size */
    0, /* cert_chain_ptr */
    0  /* cert_chain_size */
  };

  /* process command line args */
  flash_type = argv[1];
  header_type = argv[2];
  code_file_name = argv[i+3];
#if (USES_SECBOOT_2_0 == YES)
  if ((strcmp(header_type, "dbl") == 0) || (strcmp(header_type, "dbl_preamble") == 0))
  {
    sbl_file_name = argv[4];
    if (strstr(sbl_file_name, "fsbl.mbn")==NULL)
    {
      printf("Error! Expect FSBL image for DBL header generation\n");
      usage(argv[0]);
      exit(-1);
    }
    i = 1;
  }
#endif
  output_file_name = argv[i+4];

  if (strcmp(header_type, "nandprg")==0)
  {
    id = NANDPRG_IMG;
    if (argc<i+6)
    {
      printf("Error! Expect hostdl addr for NPRG header generation\n");
    }
    else
    {
      sscanf(argv[i+5], "%x", &nprg_dest_addr);
      printf("NPRG dest addr: 0x%x\n", nprg_dest_addr);
    }

    /* For secure boot only */
    if(argc == i+9)
    {
      signature_file_name = argv[i+6];
      cert_chain_file_name = argv[i+7];
      image_file_name = argv[i+8];
    }
    else
    {
      signature_file_name = NULL;
      cert_chain_file_name = NULL;
      image_file_name = NULL;
    }
  }
  else if (strcmp(header_type, "norprg")==0)
  {
    id = NORPRG_IMG;
    if (argc<i+6)
    {
      printf("Error! Expect hostdl addr for ARMPRG header generation\n");
    }
    else
    {
      sscanf(argv[i+5], "%x", &nprg_dest_addr);
      printf("ARMPRG dest addr: 0x%x\n", nprg_dest_addr);
    }

    /* For secure boot only */
    if(argc == i+9)
    {
      signature_file_name = argv[i+6];
      cert_chain_file_name = argv[i+7];
      image_file_name = argv[i+8];
    }
    else
    {
      signature_file_name = NULL;
      cert_chain_file_name = NULL;
      image_file_name = NULL;
    }
  }
  else if (strcmp(header_type, "qcsbl")==0)
  {
    id = QCSBL_IMG;
    /* User may or may not supply the QC-SBL destination address */
    if ((argc==i+6)||(argc==i+9))
    {
      change_qcsbl_dest = TRUE;
      sscanf(argv[i+5], "%x", &qcsbl_dest_addr);
      printf("QC-SBL dest addr: 0x%x\n", qcsbl_dest_addr);
    }
    else
    {
      change_qcsbl_dest = FALSE;
    }
    /* For secured boot only */
    if(argc == i+9)
    {
      signature_file_name = argv[i+6];
      cert_chain_file_name = argv[i+7];
      image_file_name = argv[i+8];
    }
    /* For secured boot only */
    else if(argc ==i+8)
    {
      signature_file_name = argv[i+5];
      cert_chain_file_name = argv[i+6];
      image_file_name = argv[i+7];
    }
    else
    {
      signature_file_name = NULL;
      cert_chain_file_name = NULL;
      image_file_name = NULL;
    }
  }
  else
  {
    /* For secured boot only */
    if(argc ==i+8)
    {
      signature_file_name = argv[i+5];
      cert_chain_file_name = argv[i+6];
      image_file_name = argv[i+7];
    }
    else
    {
      signature_file_name = NULL;
      cert_chain_file_name = NULL;
      image_file_name = NULL;
    }
  }

  if (strcmp(flash_type, "nand")==0)
  {
    is_nand = TRUE;
  }
  else
  {
    is_nand = FALSE;
  }

  if (strcmp(header_type, "oemsbl")==0)
  {
    id = OEM_SBL_IMG;
  }
  else if (strcmp(header_type, "amss") == 0)
  {
    id = AMSS_IMG;
  }
  else if (strcmp(header_type, "qcsbl") == 0)
  {
    id = QCSBL_IMG;
  }
  else if (strcmp(header_type, "hash") == 0)
  {
    id = HASH_IMG;
  }
  else if (strcmp(header_type, "nandprg") == 0)
  {
    id = NANDPRG_IMG;
  }
  else if ((strcmp(header_type, "cfgdata") == 0) && (!is_nand))
  {
    id = CFG_DATA;
  }
  else if (strcmp(header_type, "norprg") == 0)
  {
    id = NORPRG_IMG;
  }
  else if (strcmp(header_type, "fsbl")==0)
  {
    id = FSBL_IMG;
  }
  else if ((strcmp(header_type, "dbl") == 0) || (strcmp(header_type, "dbl_preamble") == 0))
  {
    id = DBL_IMG;
  }
  else if (strcmp(header_type, "osbl") == 0)
  {
    id = OSBL_IMG;
  }
  else
  {
    printf("Invalid image type: %s\n", header_type);
    return;
  }

  /* determine which header, flash type */
  if (id == AMSS_IMG)
  { /* AMSS */
    if (is_nand)
    {
      /* Note: for OEMSBL and AMSS, image_source is set to 0 for header generation.
      **       The correct src addr for OEMSBL or AMSS will be read later from
      **       partition table during boot-up (qcsbl_load_header function)
      */
      image_dest = MI_NAND_AMSS_IMG_DEST_ADDR;
    }
    else
    {
      image_dest = MI_NOR_AMSS_IMG_DEST_ADDR;
      image_source = MI_NOR_AMSS_IMG_SRC_ADDR;
    }
  }
  else if (id == HASH_IMG)
  { /* HASH */
    /* Since we dont have a fixed address for HASH TABLE We will fill this in pboot_add_hash.exe */
    image_dest = 0;
    /*
    if (is_nand)
    {
      image_dest = (unsigned int) MI_NAND_AMSS_HASH_TBL_DEST_ADDR;
    }
    else
    {
      image_dest = MI_NOR_AMSS_HASH_TBL_SRC_ADDR;
    }*/

  }
  else if (id == NANDPRG_IMG)
  { /* nandprg */
    if (is_nand)
    {
      image_dest = (unsigned int) nprg_dest_addr;
    }
    else
    {
      image_dest = nprg_dest_addr;
    }
    image_source= nprg_dest_addr;
  }
  else if (id == NORPRG_IMG)
  {
    image_dest = nprg_dest_addr;
    image_source= nprg_dest_addr;
  }
#if (USES_SECBOOT_2_0 != YES)
  else if (id == OEM_SBL_IMG)
  { /* OEMSBL */
    if (is_nand)
    { /* nand flash */
      /* Note: for OEMSBL and AMSS, image_source is set to 0 for header generation.
      **       The correct src addr for OEMSBL or AMSS will be read later from
      **       partition table during boot-up (qcsbl_load_header function)
      */
      image_dest = MI_NAND_OEM_SBL_IMG_DEST_ADDR;
    }
    else
    { /* nor flash */
      image_dest= MI_NOR_OEM_SBL_IMG_DEST_ADDR;
      image_source= MI_NOR_OEM_SBL_IMG_SRC_ADDR;
    }
  }
  else if (id == QCSBL_IMG)
  { /* QCSBL */
    if (is_nand)
    {
      if (change_qcsbl_dest)
      {
        image_dest = qcsbl_dest_addr;
      }
      else
      {
      image_dest = MI_NAND_QCSBL_IMG_DEST_ADDR;
      }
      /* For QCSBL, we need to know qcsbl starting block to generate qcsblhd_cfgdata */
      image_source= find_qcsbl_start_block();
    }
    else
    {
      if (change_qcsbl_dest)
      {
        image_dest = qcsbl_dest_addr;
      }
      else
      {
        image_dest = MI_NOR_QCSBL_IMG_DEST_ADDR;
      }
      image_source = MI_NOR_QCSBL_IMG_SRC_ADDR;
    }
  }
  else if (id == CFG_DATA)
  { /* cfg_data */
    /* Figure out image source and destination address within Boot ROM */
    image_dest = 0x0;
    image_source= 0x0;
  }
#else
  else if (id == FSBL_IMG) /* fsbl */
  {
    if (is_nand)
    {
      image_dest = MI_NAND_FSBL_IMG_DEST_ADDR;
    }
    else
    {
      image_dest = MI_NOR_FSBL_IMG_DEST_ADDR;
      image_source = MI_NOR_FSBL_IMG_SRC_ADDR;
  }
  }
  else if (id == DBL_IMG) /* dbl */
  {
    memset((void*)&sbl_header, 0xFF, sizeof(sbl_header));
    if (is_nand)
    {
      /* DBL source address in in offset of bytes */
      sbl_header.dbl_dest_ptr = (uint8*)MI_NAND_DBL_IMG_DEST_ADDR;
      sbl_header.fsbl_dest_ptr = (uint8*)MI_NAND_FSBL_IMG_DEST_ADDR;
    }
    else
    {
      sbl_header.dbl_dest_ptr = (uint8*)MI_NOR_DBL_IMG_DEST_ADDR;
      sbl_header.dbl_src = MI_NOR_DBL_IMG_SRC_ADDR;
      sbl_header.fsbl_dest_ptr = (uint8*)MI_NOR_FSBL_IMG_DEST_ADDR;
      sbl_header.fsbl_src = MI_NOR_FSBL_IMG_SRC_ADDR;
    }

    /* determine the size of the input file */
    /* size of  code image */
    stat(sbl_file_name, &in_stats);
    if ((&in_stats != NULL) && (in_stats.st_size !=0 ))
    {
      sbl_header.fsbl_image_size = in_stats.st_size;
  }
    else
    {
      printf("Error, %s file doesn't exist or size is 0", sbl_file_name);
  }
  }
  else if (id == OSBL_IMG) /* osbl */
  {
    if (is_nand)
    {
      image_dest = MI_NAND_OSBL_IMG_DEST_ADDR;
  }
    else
    {
      image_dest = MI_NOR_OSBL_IMG_DEST_ADDR;
      image_source = MI_NOR_OSBL_IMG_SRC_ADDR;
    }
  }
#endif
  /* determine the size of the input file */
  /* size of  code image */
  stat(code_file_name, &in_stats);
  code_size = in_stats.st_size;
  if(signature_file_name != NULL)
  {
    /* size of  signature image */
    stat(signature_file_name, &in_stats);
    signature_size = in_stats.st_size;
  }
  else
  {
    signature_size = 0;
  }
  /* size of  cert chain image */
  if(cert_chain_file_name != NULL)
  {
    stat(cert_chain_file_name, &in_stats);
    cert_chain_size = in_stats.st_size;
    printf("cert_chain_file_name = %s\n", cert_chain_file_name);
  }
  else
  {
    cert_chain_size = 0;
  }

  /* size of  image */
  if(image_file_name != NULL)
  {
    stat(image_file_name, &in_stats);
    image_size = in_stats.st_size;
  }
  else
  {
    image_size = code_size;
  }

  /* fill in values in header */
  header[0] = id;
  header[2] = image_source;
  header[3] = image_dest;
  header[4] = image_size;                               /* image_size */
  header[5] = code_size;                                /* code_size */
  header[6] = image_dest+code_size;                     /* signature_ptr */
  header[7] = signature_size;                           /* signature_size */
  header[8] = image_dest+code_size+signature_size;      /* cert_chain_ptr */
  header[9] = cert_chain_size;                          /* cert_chain_size*/


  /* For HASH_IMG - Zero out these values since we dont know the Destination Address of the Hash yet
  ** These values will be filled in by the tool pboot_add_hash.exe.
  ** Virtual Address of Hash Segment = Address of the First ZI segement + Size of the ZI segment + PAD to Make it 4k Aligned + Size of Hash Header (40 bytes)
  **
  ** For CFG_DATA - Zero out these values as they can vary between Boot ROM revisions. The ROM code
  ** will perform a runtime calculation of these values
  */

  if ((id == HASH_IMG)||(id == CFG_DATA))
  { /* hash */
    header[3] = 0;
    header[6] = 0; /* signature_ptr */
    header[8] = 0; /* cert_chain_ptr */
  }


  /* open the binary ouput file */
  out_fp = fopen(output_file_name, "wb");
  if (out_fp==NULL)
  { /* make sure file open succeeded */
    printf("File creation failed: %s", output_file_name);
	return 0;
  }


#if (USES_SECBOOT_2_0 == YES)
  if (((strcmp(header_type, "dbl") == 0)) || (strcmp(header_type, "dbl_preamble") == 0))
  {
    static struct{
      char * flash_name;
      uint32 codeword;
    }const tFlash_type[] = 
    {
      {"nand", NAND_8_BIT},
      {"onenand_16_bit", ONENAND_16_BIT},
      {"mdoc_16_bit", MDOC_16_BIT},
      {"nor", NOR_CODEWORD},
      {NULL, 0x0}
    };

    for (i = 0; tFlash_type[i].flash_name != NULL; i++)
    {
      if (strcmp(tFlash_type[i].flash_name, flash_type) == 0)
      {
        sbl_header.codeword = tFlash_type[i].codeword;
        break;
      }
    }
    if (sbl_header.codeword == 0)
    {
      printf("flash_type not recognized\n");
    }
    sbl_header.magic = MAGIC_NUM;
    sbl_header.dbl_image_size = image_size;
    sbl_header.dbl_code_size = code_size;
    sbl_header.dbl_sig_ptr = (uint8*)(sbl_header.dbl_dest_ptr+code_size);
    sbl_header.dbl_sig_size = signature_size;
    sbl_header.dbl_cert_chain_ptr  = (uint8*)(sbl_header.dbl_dest_ptr+code_size+signature_size);
    sbl_header.dbl_cert_chain_size = cert_chain_size;

    /* Add preamnle for DBL*/
    if ( (is_nand) && (strcmp(header_type, "dbl_preamble") == 0) )
    {
      uint32    autodetectpage[MAX_SIZE_OF_VERIFY_BUFFER];  /* file buffer. */
      uint32  *data_ptr;
      /* set all the enteries in the buffer to the constant value. */
      memset (autodetectpage, 0XFF, sizeof (autodetectpage));
      data_ptr  = autodetectpage;
      *data_ptr = sbl_header.codeword;
      data_ptr++;
      *data_ptr = sbl_header.magic;
      data_ptr++;
      *data_ptr = AUTODETECT_PAGE_SIZE_MAGIC_NUM;

      for ( i=0; i<(AUTODETECT_MAX_PAGE_SIZE_SUPPORTED/AUTODETECT_MIN_PAGE_SIZE_SUPPORTED); i++ )
        fwrite (autodetectpage, 1, AUTODETECT_MIN_PAGE_SIZE_SUPPORTED, out_fp);

      /* set all the enteries in the buffer to the constant value. */
      memset (autodetectpage, 0XFF, sizeof (autodetectpage));    
      for ( i=0; i<(AUTODETECT_MAX_PAGE_SIZE_SUPPORTED/AUTODETECT_MIN_PAGE_SIZE_SUPPORTED); i++ )
        fwrite (autodetectpage, 1, AUTODETECT_MIN_PAGE_SIZE_SUPPORTED, out_fp);
      sbl_header.dbl_src += AUTODETECT_MAX_PAGE_SIZE_SUPPORTED * 2;
    }
  /* write in the values */
    fwrite(&sbl_header, sizeof(boot_sbl_header_type), 1, out_fp);
  }
  else
  {
  fwrite(header, 4, 10, out_fp);
  }
#else
  /* write in the values */
  fwrite(header, 4, 10, out_fp);
#endif
  fclose(out_fp);
  return 0;
}
