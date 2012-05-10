/*=============================================================================

                      D e v i c e  B o o t  L o a d e r

GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2007 by QUALCOMM, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/qsc1100/romboot/sbl/dbl/config/ebi1/dbl_ebi1.c#18 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
05/11/09   mp      Changes to support the 8MB NOR (SAMSUNG K5N6433ABM) on ULC
10/13/08   tnk     Added support for Intel_W18 flash .
09/01/07   ps      Initial revision.
=============================================================================*/


/*=============================================================================
                            INCLUDE FILES FOR MODULE
=============================================================================*/

#include "dbl_ebi1.h"
#include "boot_msm.h"
#include "dbl_error_handler.h"
#include "boot_dbl_if.h"
#include "dbl_hw.h"
#include "dbl_parser.h"
#include "customer.h"


#if defined FLASH_BURST_NOR
#define FLASH_ON_CS 0
#define FLASH_BASE_ADDRESS EBI1_CS0_BASE
#define EBI1_BUFC_CFG_VALUE 0xFF250001
#elif defined FLASH_SHADOW_PSRAM
#define FLASH_ON_CS 2
#define FLASH_BASE_ADDRESS EBI1_CS2_BASE
#define EBI1_BUFC_CFG_VALUE 0xFF25000F
#endif



/*=============================================================================
            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.
=============================================================================*/
typedef struct {
  const char                  *name;             /* Textual name of device. */
  word                        num_ids;           /* Number of codes to match */
  word                        codes[4];          /* Manufacture's codes. */
} dbl_nor_device;

typedef struct {
  volatile word *base_addr;
  dbl_nor_device * (*probe_fcn) (volatile word *probe_addr);
} DBLProbeTbl;

dbl_nor_device *dbl_intel_probe (volatile word *baseaddr);
dbl_nor_device *dbl_spansion_probe (volatile word *baseaddr);
dbl_nor_device *dbl_samsung_probe (volatile word *baseaddr);

/* For Probe table */
#define DBL_FLASH_PROBE_END 0xDEADBEEF

dbl_nor_device S29WS256N0SB = 
{
  "SPANSION S29WS256N0SB",
  4,                                              /* # of codes to match */
  {1,  0x227e, 0x2230, 0x2200 }                   /* Manufacture codes. */
};

dbl_nor_device S29WS512P =
{
  "SPANSION S29WS512P",
  4,                                              /* # of codes to match */
  {1,  0x227e, 0x223D, 0x2200 },                  /* Manufacture codes. */
};

dbl_nor_device Intel_128M18_ADMux =
{
  "INTEL 128 M18 ADMux",
  2,                             /* # of codes to match */    
  { 0x89, 0x8903 },              /* Manufacture codes. */
};

dbl_nor_device Intel_256M18_ADMux =
{
  "INTEL 256 M18 ADMux",
  2,                             /* # of codes to match */    
  { 0x89, 0x8904 },              /* Manufacture codes. */
};

dbl_nor_device Intel_512M18_ADMux =
{
  "INTEL 512 M18 ADMux",
  2,                             /* # of codes to match */    
  { 0x89, 0x8881 },              /* Manufacture codes. */
};

dbl_nor_device Intel_1024M18_ADMux =
{
  "INTEL 1024 M18 ADMux",
  2,                             /* # of codes to match */    
  { 0x89, 0x88B1 },              /* Manufacture codes. */
};

dbl_nor_device K5N5629ABM = 
{
  "SAMSUNG K5N5629ABM",
  2,                                 /* # of codes to match */
  {0x00EC, 0x2601}                   /* Manufacture codes. */
};
dbl_nor_device K5N5629ATC = 
{
  "SAMSUNG K5N5629ATC",
  2,                                 /* # of codes to match */
  {0x00EC, 0x2208}                   /* Manufacture codes. */
};

dbl_nor_device K5N5629ABC = 
{
  "SAMSUNG K5N5629ABC",
  2,                                 /* # of codes to match */
  {0x00EC, 0x2209}                   /* Manufacture codes. */
};

dbl_nor_device K5N5629AUC = 
{
  "SAMSUNG K5N5629AUC",
  2,                                 /* # of codes to match */
  {0x00EC, 0x3018}                   /* Manufacture codes. */
};

dbl_nor_device Intel_64W18_ADMux =
{
  "INTEL 64 W18 ADMux",
  2,                             /* # of codes to match */    
  { 0x20, 0x88C0 },              /* Manufacture codes. */
};

dbl_nor_device M36W0R5040U6ZS = 
{
  "NUMONYX M36W0R5040U6ZS",
  2,                                 /* # of codes to match */
  { 0x20, 0x8828 },                  /* Manufacture codes. */
};

dbl_nor_device M58LR128KC = 
{
  "NUMONYX M58LR128KC",
  2,                                 /* # of codes to match */
  { 0x20, 0x882e },                  /* Manufacture codes. */
};

dbl_nor_device K5N6433ABM = 
{
  "SAMSUNG K5N6433ABM",
  2,                                 /* # of codes to match */
  {0x00EC, 0x2255}                   /* Manufacture codes. */
};

dbl_nor_device K5N6433ATM = 
{
  "SAMSUNG K5N6433ATM",
  2,                                 /* # of codes to match */
  {0x00EC, 0x2254}                   /* Manufacture codes. */
};

dbl_nor_device K5N2833ATB = 
{
  "SAMSUNG K5N2833-66ATB",
  2,                                 /* # of codes to match */
  {0x00EC, 0x2404}                   /* Manufacture codes. */
};

dbl_nor_device K5N2833ABB = 
{
  "SAMSUNG K5N2833-66ABB",
  2,                                 /* # of codes to match */
  {0x00EC, 0x2405}                   /* Manufacture codes. */
};

/* List only flash parts tested by this target */
const dbl_nor_device *(spansion_parts[]) = {
  &S29WS256N0SB,
  &S29WS512P,
  NULL
};


/* List of all Intel parts that are probed similarly. */
const dbl_nor_device *(intel_parts[]) = {
#ifndef FEATURE_USES_LOWMEM
  &Intel_1024M18_ADMux,
  &Intel_512M18_ADMux,
  &Intel_256M18_ADMux,
  &Intel_128M18_ADMux,
  &Intel_64W18_ADMux,
#endif
  &M36W0R5040U6ZS,
  &M58LR128KC,
  NULL
};


/* List only flash parts tested by this target */
const dbl_nor_device *(samsung_parts[]) = {
  &K5N5629ABM,
  &K5N6433ABM,
  &K5N6433ATM,
  &K5N2833ATB,
  &K5N2833ABB,
  &K5N5629ATC,
  &K5N5629ABC,
  &K5N5629AUC,
  NULL
};


/* Probe table for flash */
DBLProbeTbl dbl_probe_info_flash[] = {
#ifndef FEATURE_USES_LOWMEM
  { (volatile word *)FLASH_BASE_ADDRESS, dbl_samsung_probe },
  { (volatile word *)FLASH_BASE_ADDRESS, dbl_intel_probe },
  { (volatile word *)FLASH_BASE_ADDRESS, dbl_spansion_probe },
#else
  { (volatile word *)FLASH_BASE_ADDRESS, dbl_intel_probe },
#endif
  { (volatile word *) DBL_FLASH_PROBE_END, 0 }
};


int probe_info_size = (sizeof(dbl_probe_info_flash)/sizeof(DBLProbeTbl))-1;


const static dbl_parser_cfg_data_item_type ebi1_cfg_data_S29WSN0SB_48MHZ[] =
{
  {WRITE_16_BIT,   (FLASH_BASE_ADDRESS | 0x00000000),  0xF0    },
  {WRITE_16_BIT,   (FLASH_BASE_ADDRESS | 0x00000AAA),  0xAA    },
  {WRITE_16_BIT,   (FLASH_BASE_ADDRESS | 0x00000554),  0x55    },
  {WRITE_16_BIT,   (FLASH_BASE_ADDRESS | 0x00000AAA),  0xD0    },
  {WRITE_16_BIT,   (FLASH_BASE_ADDRESS | 0x00000000),  0x66CB  },


  {HWIO_OPERATION,   HWIO_ADDRI(EBI1_CSn_CFG0, FLASH_ON_CS),  0x002400             },
  {HWIO_OPERATION,   HWIO_ADDRI(EBI1_CSn_CFG1, FLASH_ON_CS),  0x00010034           },
  {HWIO_OPERATION,   HWIO_ADDR(EBI1_BUFC_CFG),                EBI1_BUFC_CFG_VALUE  },

  /*-----------------------------------------------------------------------
                            End of Configuration
  -----------------------------------------------------------------------*/   
  {END_OF_CFG_DATA,  0x00000000,                    0x00000000    },
};

const static dbl_parser_cfg_data_item_type ebi1_cfg_data_S29WSN0SB_64MHZ[] =
{
  {WRITE_16_BIT,   (FLASH_BASE_ADDRESS | 0x00000000),  0xF0    },
  {WRITE_16_BIT,   (FLASH_BASE_ADDRESS | 0x00000AAA),  0xAA    },
  {WRITE_16_BIT,   (FLASH_BASE_ADDRESS | 0x00000554),  0x55    },
  {WRITE_16_BIT,   (FLASH_BASE_ADDRESS | 0x00000AAA),  0xD0    },
  {WRITE_16_BIT,   (FLASH_BASE_ADDRESS | 0x00000000),  0x66CB  },
  
  {HWIO_OPERATION,   HWIO_ADDRI(EBI1_CSn_CFG0, FLASH_ON_CS),  0x016400             },
  {HWIO_OPERATION,   HWIO_ADDRI(EBI1_CSn_CFG1, FLASH_ON_CS),  0x00010034           },
  {HWIO_OPERATION,   HWIO_ADDR(EBI1_BUFC_CFG),                EBI1_BUFC_CFG_VALUE  },
  /*-----------------------------------------------------------------------
                            End of Configuration
  -----------------------------------------------------------------------*/   
  {END_OF_CFG_DATA,  0x00000000,                    0x00000000            }
};

const static dbl_parser_cfg_data_item_type ebi1_cfg_data_K5N5629ABM_48MHZ[] =
{
  {WRITE_16_BIT,   (FLASH_BASE_ADDRESS | 0x00000000),  0xF0    },
  {WRITE_16_BIT,   (FLASH_BASE_ADDRESS | 0x00000AAA),  0xAA    },
  {WRITE_16_BIT,   (FLASH_BASE_ADDRESS | 0x00000554),  0x55    },
  {WRITE_16_BIT,   (FLASH_BASE_ADDRESS | 0x004A6AAA),  0xC0    },
  {WRITE_16_BIT,   (FLASH_BASE_ADDRESS | 0x00000AAA),  0xAA    },
  {WRITE_16_BIT,   (FLASH_BASE_ADDRESS | 0x00000554),  0x55    },
  {WRITE_16_BIT,   (FLASH_BASE_ADDRESS | 0x00002AAA),  0xC5    },

  {HWIO_OPERATION,   HWIO_ADDRI(EBI1_CSn_CFG0, FLASH_ON_CS),  0x025400             },
  {HWIO_OPERATION,   HWIO_ADDRI(EBI1_CSn_CFG1, FLASH_ON_CS),  0x01010034           },
  {HWIO_OPERATION,   HWIO_ADDR(EBI1_BUFC_CFG),                EBI1_BUFC_CFG_VALUE  },

  /*-----------------------------------------------------------------------
                            End of Configuration
  -----------------------------------------------------------------------*/   
  {END_OF_CFG_DATA,  0x00000000,                    0x00000000    },
};

const static dbl_parser_cfg_data_item_type ebi1_cfg_data_K5N5629ABM_64MHZ[] =
{
  {WRITE_16_BIT,   (FLASH_BASE_ADDRESS | 0x00000000),  0xF0    },
  {WRITE_16_BIT,   (FLASH_BASE_ADDRESS | 0x00000AAA),  0xAA    },
  {WRITE_16_BIT,   (FLASH_BASE_ADDRESS | 0x00000554),  0x55    },
  {WRITE_16_BIT,   (FLASH_BASE_ADDRESS | 0x004A3AAA),  0xC0    },
  {WRITE_16_BIT,   (FLASH_BASE_ADDRESS | 0x00000AAA),  0xAA    },
  {WRITE_16_BIT,   (FLASH_BASE_ADDRESS | 0x00000554),  0x55    },
  {WRITE_16_BIT,   (FLASH_BASE_ADDRESS | 0x00002AAA),  0xC5    },

  {HWIO_OPERATION,   HWIO_ADDRI(EBI1_CSn_CFG0, FLASH_ON_CS),  0x013400             },
  {HWIO_OPERATION,   HWIO_ADDRI(EBI1_CSn_CFG1, FLASH_ON_CS),  0x01011034           },
  {HWIO_OPERATION,   HWIO_ADDR(EBI1_BUFC_CFG),                EBI1_BUFC_CFG_VALUE  },

  /*-----------------------------------------------------------------------
                            End of Configuration
  -----------------------------------------------------------------------*/   
  {END_OF_CFG_DATA,  0x00000000,                    0x00000000    },
};

const static dbl_parser_cfg_data_item_type ebi1_cfg_data_K5N6433ABM_48MHZ[] =
{
  {WRITE_16_BIT,   (FLASH_BASE_ADDRESS | 0x00000000),  0xF0    },
  {WRITE_16_BIT,   (FLASH_BASE_ADDRESS | 0x00000AAA),  0xAA    },
  {WRITE_16_BIT,   (FLASH_BASE_ADDRESS | 0x00000554),  0x55    },
  {WRITE_16_BIT,   (FLASH_BASE_ADDRESS | 0x002A6AAA),  0xC0    },
  {WRITE_16_BIT,   (FLASH_BASE_ADDRESS | 0x00000AAA),  0xAA    },
  {WRITE_16_BIT,   (FLASH_BASE_ADDRESS | 0x00000554),  0x55    },
  {WRITE_16_BIT,   (FLASH_BASE_ADDRESS | 0x00002AAA),  0xC5    },

  {HWIO_OPERATION,   HWIO_ADDRI(EBI1_CSn_CFG0, FLASH_ON_CS),  /*0x025400 */ 0x13400 },
  {HWIO_OPERATION,   HWIO_ADDRI(EBI1_CSn_CFG1, FLASH_ON_CS),  /*0x01010034 */ 0x1011034  },
  {HWIO_OPERATION,   HWIO_ADDR(EBI1_BUFC_CFG),                EBI1_BUFC_CFG_VALUE  },

  /*-----------------------------------------------------------------------
                            End of Configuration
  -----------------------------------------------------------------------*/   
  {END_OF_CFG_DATA,  0x00000000,                    0x00000000    },
};

const static dbl_parser_cfg_data_item_type ebi1_cfg_data_K5N5629ABM_96MHZ[] =
{
  {WRITE_16_BIT,   (FLASH_BASE_ADDRESS | 0x00000000),  0xF0    },
  {WRITE_16_BIT,   (FLASH_BASE_ADDRESS | 0x00000AAA),  0xAA    },
  {WRITE_16_BIT,   (FLASH_BASE_ADDRESS | 0x00000554),  0x55    },
  {WRITE_16_BIT,   (FLASH_BASE_ADDRESS | 0x004A6AAA),  0xC0    },
  {WRITE_16_BIT,   (FLASH_BASE_ADDRESS | 0x00000AAA),  0xAA    },
  {WRITE_16_BIT,   (FLASH_BASE_ADDRESS | 0x00000554),  0x55    },
  {WRITE_16_BIT,   (FLASH_BASE_ADDRESS | 0x00002AAA),  0xC5    },

  {HWIO_OPERATION,   HWIO_ADDRI(EBI1_CSn_CFG0, FLASH_ON_CS),  0x025400             },
  {HWIO_OPERATION,   HWIO_ADDRI(EBI1_CSn_CFG1, FLASH_ON_CS),  0x01010034           },
  {HWIO_OPERATION,   HWIO_ADDR(EBI1_BUFC_CFG),                EBI1_BUFC_CFG_VALUE  },

  /*-----------------------------------------------------------------------
                            End of Configuration
  -----------------------------------------------------------------------*/   
  {END_OF_CFG_DATA,  0x00000000,                    0x00000000    },
};

const static dbl_parser_cfg_data_item_type ebi1_cfg_data_K5N5629ATC_48MHZ[] =
{
  {WRITE_16_BIT,   (FLASH_BASE_ADDRESS | 0x00000000),  0xF0    },
  {WRITE_16_BIT,   (FLASH_BASE_ADDRESS | 0x00000AAA),  0xAA    },
  {WRITE_16_BIT,   (FLASH_BASE_ADDRESS | 0x00000554),  0x55    },
  {WRITE_16_BIT,   (FLASH_BASE_ADDRESS | 0x001A2AAA),  0xC0    },

  {HWIO_OPERATION,   HWIO_ADDRI(EBI1_CSn_CFG0, FLASH_ON_CS),  0x013400             },
  {HWIO_OPERATION,   HWIO_ADDRI(EBI1_CSn_CFG1, FLASH_ON_CS),  0x01010034           },
  {HWIO_OPERATION,   HWIO_ADDR(EBI1_BUFC_CFG),                EBI1_BUFC_CFG_VALUE  },

  /*-----------------------------------------------------------------------
                            End of Configuration
  -----------------------------------------------------------------------*/   
  {END_OF_CFG_DATA,  0x00000000,                    0x00000000    },
};

const static dbl_parser_cfg_data_item_type ebi1_cfg_data_K5N5629ATC_64MHZ[] =
{
  {WRITE_16_BIT,   (FLASH_BASE_ADDRESS | 0x00000000),  0xF0    },
  {WRITE_16_BIT,   (FLASH_BASE_ADDRESS | 0x00000AAA),  0xAA    },
  {WRITE_16_BIT,   (FLASH_BASE_ADDRESS | 0x00000554),  0x55    },
  {WRITE_16_BIT,   (FLASH_BASE_ADDRESS | 0x001A3AAA),  0xC0    },

  {HWIO_OPERATION,   HWIO_ADDRI(EBI1_CSn_CFG0, FLASH_ON_CS),  0x013400             },
  {HWIO_OPERATION,   HWIO_ADDRI(EBI1_CSn_CFG1, FLASH_ON_CS),  0x01010034           },
  {HWIO_OPERATION,   HWIO_ADDR(EBI1_BUFC_CFG),                EBI1_BUFC_CFG_VALUE  },

  /*-----------------------------------------------------------------------
                            End of Configuration
  -----------------------------------------------------------------------*/   
  {END_OF_CFG_DATA,  0x00000000,                    0x00000000    },
};

const static dbl_parser_cfg_data_item_type ebi1_cfg_data_K5N5629ATC_96MHZ[] =
{
  {WRITE_16_BIT,   (FLASH_BASE_ADDRESS | 0x00000000),  0xF0    },
  {WRITE_16_BIT,   (FLASH_BASE_ADDRESS | 0x00000AAA),  0xAA    },
  {WRITE_16_BIT,   (FLASH_BASE_ADDRESS | 0x00000554),  0x55    },
  {WRITE_16_BIT,   (FLASH_BASE_ADDRESS | 0x001A6AAA),  0xC0    },

  {HWIO_OPERATION,   HWIO_ADDRI(EBI1_CSn_CFG0, FLASH_ON_CS),  0x025400             },
  {HWIO_OPERATION,   HWIO_ADDRI(EBI1_CSn_CFG1, FLASH_ON_CS),  0x01010034           },
  {HWIO_OPERATION,   HWIO_ADDR(EBI1_BUFC_CFG),                EBI1_BUFC_CFG_VALUE  },

  /*-----------------------------------------------------------------------
                            End of Configuration
  -----------------------------------------------------------------------*/   
  {END_OF_CFG_DATA,  0x00000000,                    0x00000000    },
};

const static dbl_parser_cfg_data_item_type ebi1_cfg_data_S29WSP_48MHZ[] =
{
  {WRITE_16_BIT,   (FLASH_BASE_ADDRESS | 0x00000000),  0xF0    },
  {WRITE_16_BIT,   (FLASH_BASE_ADDRESS | 0x00000AAA),  0xAA    },
  {WRITE_16_BIT,   (FLASH_BASE_ADDRESS | 0x00000554),  0x55    },
  {WRITE_16_BIT,   (FLASH_BASE_ADDRESS | 0x00000AAA),  0xD0    },
  {WRITE_16_BIT,   (FLASH_BASE_ADDRESS | 0x00000000),  0x66CB  },

  {WRITE_16_BIT,   (FLASH_BASE_ADDRESS | 0x00000002),  0xFFEE  },
  
  {HWIO_OPERATION,   HWIO_ADDRI(EBI1_CSn_CFG0, FLASH_ON_CS),  0x002300             },
  {HWIO_OPERATION,   HWIO_ADDRI(EBI1_CSn_CFG1, FLASH_ON_CS),  0x00000034           },
  {HWIO_OPERATION,   HWIO_ADDR(EBI1_BUFC_CFG),                EBI1_BUFC_CFG_VALUE  },
  /*-----------------------------------------------------------------------
                            End of Configuration
  -----------------------------------------------------------------------*/   
  {END_OF_CFG_DATA,  0x00000000,                    0x00000000  }
};

const static dbl_parser_cfg_data_item_type ebi1_cfg_data_S29WSP_64MHZ[] =
{
  {WRITE_16_BIT,   (FLASH_BASE_ADDRESS | 0x00000000),  0xF0    },
  {WRITE_16_BIT,   (FLASH_BASE_ADDRESS | 0x00000AAA),  0xAA    },
  {WRITE_16_BIT,   (FLASH_BASE_ADDRESS | 0x00000554),  0x55    },
  {WRITE_16_BIT,   (FLASH_BASE_ADDRESS | 0x00000AAA),  0xD0    },
  {WRITE_16_BIT,   (FLASH_BASE_ADDRESS | 0x00000000),  0x66CB  },

  {WRITE_16_BIT,   (FLASH_BASE_ADDRESS | 0x00000002),  0xFFEE  },
  
  {HWIO_OPERATION,   HWIO_ADDRI(EBI1_CSn_CFG0, FLASH_ON_CS),  0x013400             },
  {HWIO_OPERATION,   HWIO_ADDRI(EBI1_CSn_CFG1, FLASH_ON_CS),  0x00010034           },
  {HWIO_OPERATION,   HWIO_ADDR(EBI1_BUFC_CFG),                EBI1_BUFC_CFG_VALUE  },
  /*-----------------------------------------------------------------------
                            End of Configuration
  -----------------------------------------------------------------------*/   
  {END_OF_CFG_DATA,  0x00000000,                    0x00000000  }
};

const static dbl_parser_cfg_data_item_type ebi1_cfg_data_S29WSP_96MHZ[] =
{
  {WRITE_16_BIT,   (FLASH_BASE_ADDRESS | 0x00000000),  0xF0    },
  {WRITE_16_BIT,   (FLASH_BASE_ADDRESS | 0x00000AAA),  0xAA    },
  {WRITE_16_BIT,   (FLASH_BASE_ADDRESS | 0x00000554),  0x55    },
  {WRITE_16_BIT,   (FLASH_BASE_ADDRESS | 0x00000AAA),  0xD0    },
  {WRITE_16_BIT,   (FLASH_BASE_ADDRESS | 0x00000000),  0x4ECB  },

  {WRITE_16_BIT,   (FLASH_BASE_ADDRESS | 0x00000002),  0xFFEF  },
  
  {HWIO_OPERATION,   HWIO_ADDRI(EBI1_CSn_CFG0, FLASH_ON_CS),  0x24400              },
  {HWIO_OPERATION,   HWIO_ADDRI(EBI1_CSn_CFG1, FLASH_ON_CS),  0x00010034           },
  {HWIO_OPERATION,   HWIO_ADDR(EBI1_BUFC_CFG),                EBI1_BUFC_CFG_VALUE  },
  /*-----------------------------------------------------------------------
                            End of Configuration
  -----------------------------------------------------------------------*/   
  {END_OF_CFG_DATA,  0x00000000,                    0x00000000            }
};

const static dbl_parser_cfg_data_item_type ebi1_cfg_data_common[] =
{
  {HWIO_OPERATION,   HWIO_ADDR(EBI1_CFG),               0x04080  },
  {HWIO_OPERATION,   HWIO_ADDR(EBI1_AHB_RD_PRIORITY ),  0x024    },
  {HWIO_OPERATION,   HWIO_ADDR(EBI1_AHB_WR_PRIORITY),   0x8024   },

  {HWIO_OPERATION,   HWIO_ADDR(GPIO_PAGE),   62   },
  {HWIO_OPERATION,   HWIO_ADDR(GPIO_CFG),    0x4  },
  {HWIO_OPERATION,   HWIO_ADDR(GPIO_PAGE),   61   },
  {HWIO_OPERATION,   HWIO_ADDR(GPIO_CFG),    0x4  },
  {HWIO_OPERATION,   HWIO_ADDR(GPIO_PAGE),   60   },
  {HWIO_OPERATION,   HWIO_ADDR(GPIO_CFG),    0x4  },
  {HWIO_OPERATION,   HWIO_ADDR(GPIO_PAGE),   59   },
  {HWIO_OPERATION,   HWIO_ADDR(GPIO_CFG),    0x4  },
  {HWIO_OPERATION,   HWIO_ADDR(GPIO_PAGE),   58   },
  {HWIO_OPERATION,   HWIO_ADDR(GPIO_CFG),    0x4  },
  {HWIO_OPERATION,   HWIO_ADDR(GPIO_PAGE),   57   },
  {HWIO_OPERATION,   HWIO_ADDR(GPIO_CFG),    0x4  },
  {HWIO_OPERATION,   HWIO_ADDR(GPIO_PAGE),   56   },
  {HWIO_OPERATION,   HWIO_ADDR(GPIO_CFG),    0x4  },
  {HWIO_OPERATION,   HWIO_ADDR(GPIO_PAGE),   55   },
  {HWIO_OPERATION,   HWIO_ADDR(GPIO_CFG),    0x4  },
#ifndef FLASH_SHADOW_PSRAM /* AD24 is shared with CRE pin. Do not configure it in Shadow Mode */
#if 0 /* Temporary comment this for all the cases */
  {HWIO_OPERATION,   HWIO_ADDR(GPIO2_PAGE),  33   },
  {HWIO_OPERATION,   HWIO_ADDR(GPIO2_CFG),   0x4  },
#endif
#endif
  {HWIO_OPERATION,   HWIO_ADDR(EBI2_PAD_HDRIVE_MSEL_0),  0x04444444  },
  {HWIO_OPERATION,   HWIO_ADDR(EBI1_PAD_HDRIVE_MSEL_0),  0x04444444  },
  {HWIO_OPERATION,   HWIO_ADDR(EBI1_PAD_HDRIVE_MSEL_1),  0x0444      },
  {HWIO_OPERATION,   HWIO_ADDR(GPIO_PAD_HDRIVE_MSEL_7),  0x04444444  },

  {HWIO_OPERATION,   HWIO_ADDR(GPIO_PAD_HDRIVE_MSEL_6),  0x4    },
  {HWIO_OPERATION,   HWIO_ADDR(GPIO_PAD_HDRIVE_MSEL_5),  0x4    },
  {HWIO_OPERATION,   HWIO_ADDR(GPIO_PAD_HDRIVE_MSEL_4),  0x70407	},
  {HWIO_OPERATION,   HWIO_ADDR(GPIO_PAD_HDRIVE_MSEL_3),  0x77000000  },

  {HWIO_OPERATION,   HWIO_ADDR(EBI1_PAD_PULL_1 ),  0x00B  },
  {HWIO_OPERATION,   HWIO_ADDR(EBI1_PAD_PULL_0),   0x008  },
  {HWIO_OPERATION,   HWIO_ADDR(EBI2_PAD_PULL_1),   0x01   },
  {HWIO_OPERATION,   HWIO_ADDR(EBI2_PAD_PULL_0),   0x00   },

  /* Extra configuration for enabling EBI1 CS2 in shadow mode */
  {HWIO_OPERATION,   HWIO_ADDR(GPIO_PAGE),  34   },
  {HWIO_OPERATION,   HWIO_ADDR(GPIO_CFG),   0x4  },

  /*-----------------------------------------------------------------------
                            End of Configuration
  -----------------------------------------------------------------------*/   
  {END_OF_CFG_DATA,  0x00000000,                    0x00000000            }
};

const static dbl_parser_cfg_data_item_type ebi1_cfg_data_Intel_M18_48MHZ[] =
{
  /* Changed this values as per VI team */
  {WRITE_16_BIT,   (FLASH_BASE_ADDRESS | ( 0x00002903 << 1 )),  0x60    },
  {WRITE_16_BIT,   (FLASH_BASE_ADDRESS | ( 0x00002903 << 1 )),  0x03    },
  {WRITE_16_BIT,   (FLASH_BASE_ADDRESS | ( 0x00002903 << 1 )),  0xFF    },

  
  {HWIO_OPERATION,   HWIO_ADDRI(EBI1_CSn_CFG0, FLASH_ON_CS),  0x13400              },
  {HWIO_OPERATION,   HWIO_ADDRI(EBI1_CSn_CFG1, FLASH_ON_CS),  0x1011034            },
  {HWIO_OPERATION,   HWIO_ADDR(EBI1_BUFC_CFG),                EBI1_BUFC_CFG_VALUE  },
  /*-----------------------------------------------------------------------
                            End of Configuration
  -----------------------------------------------------------------------*/   
  {END_OF_CFG_DATA,  0x00000000,                    0x00000000    }
};

const static dbl_parser_cfg_data_item_type ebi1_cfg_data_Intel_M18_64MHZ[] =
{
  /* Changed this values as per VI team */
  {WRITE_16_BIT,   (FLASH_BASE_ADDRESS | ( 0x00002903 << 1 )),  0x60    },
  {WRITE_16_BIT,   (FLASH_BASE_ADDRESS | ( 0x00002903 << 1 )),  0x03    },
  {WRITE_16_BIT,   (FLASH_BASE_ADDRESS | ( 0x00002903 << 1 )),  0xFF    },

  
  {HWIO_OPERATION,   HWIO_ADDRI(EBI1_CSn_CFG0, FLASH_ON_CS),  0x13400              },
  {HWIO_OPERATION,   HWIO_ADDRI(EBI1_CSn_CFG1, FLASH_ON_CS),  0x1011034            },
  {HWIO_OPERATION,   HWIO_ADDR(EBI1_BUFC_CFG),                EBI1_BUFC_CFG_VALUE  },
  /*-----------------------------------------------------------------------
                            End of Configuration
  -----------------------------------------------------------------------*/   
  {END_OF_CFG_DATA,  0x00000000,                    0x00000000    }
};

const static dbl_parser_cfg_data_item_type ebi1_cfg_data_Intel_W18_48MHZ[] =
{
  /* Changed this values as per VI team */

  {WRITE_16_BIT,   (FLASH_BASE_ADDRESS | ( 0x000021c3 << 1 )),  0x60    },
  {WRITE_16_BIT,   (FLASH_BASE_ADDRESS | ( 0x000021c3 << 1 )),  0x03    },
  {HWIO_OPERATION,   HWIO_ADDRI(EBI1_CSn_CFG0, FLASH_ON_CS),  0x13400              },
  {HWIO_OPERATION,   HWIO_ADDRI(EBI1_CSn_CFG1, FLASH_ON_CS),  0x1011034            },
  {HWIO_OPERATION,   HWIO_ADDR(EBI1_BUFC_CFG),                EBI1_BUFC_CFG_VALUE  },
  /*-----------------------------------------------------------------------
                            End of Configuration
  -----------------------------------------------------------------------*/   

  {END_OF_CFG_DATA,  0x00000000,                    0x00000000    }
};

const static dbl_parser_cfg_data_item_type ebi1_cfg_data_Intel_M58_48MHZ[] =
{
  /* Changed this values as per VI team */

  {WRITE_16_BIT,   (FLASH_BASE_ADDRESS | ( 0x000021c3 << 1 )),  0x60    },
  {WRITE_16_BIT,   (FLASH_BASE_ADDRESS | ( 0x000021c3 << 1 )),  0x03    },
  {WRITE_16_BIT,   (FLASH_BASE_ADDRESS | ( 0x000021C3 << 1 )),  0xFF    },
  
  {HWIO_OPERATION,   HWIO_ADDRI(EBI1_CSn_CFG0, FLASH_ON_CS),  0x13400              },
  {HWIO_OPERATION,   HWIO_ADDRI(EBI1_CSn_CFG1, FLASH_ON_CS),  0x1011034            },
  {HWIO_OPERATION,   HWIO_ADDR(EBI1_BUFC_CFG),                EBI1_BUFC_CFG_VALUE  },
  /*-----------------------------------------------------------------------
                            End of Configuration
  -----------------------------------------------------------------------*/   

  {END_OF_CFG_DATA,  0x00000000,                    0x00000000    }
};

const static dbl_parser_cfg_data_item_type ebi1_cfg_data_Intel_M58LR_48MHZ[] =
{
  /* Changed this values as per VI team */

  {WRITE_16_BIT,   (FLASH_BASE_ADDRESS | ( 0x000021c3 << 1 )),  0x60    },
  {WRITE_16_BIT,   (FLASH_BASE_ADDRESS | ( 0x000021c3 << 1 )),  0x03    },
  {WRITE_16_BIT,   (FLASH_BASE_ADDRESS | ( 0x000021C3 << 1 )),  0xFF    },
  
  {HWIO_OPERATION,   HWIO_ADDRI(EBI1_CSn_CFG0, FLASH_ON_CS),  0x13400              },
  {HWIO_OPERATION,   HWIO_ADDRI(EBI1_CSn_CFG1, FLASH_ON_CS),  0x1011034            },
  {HWIO_OPERATION,   HWIO_ADDR(EBI1_BUFC_CFG),                EBI1_BUFC_CFG_VALUE  },
  /*-----------------------------------------------------------------------
                            End of Configuration
  -----------------------------------------------------------------------*/   

  {END_OF_CFG_DATA,  0x00000000,                    0x00000000    }
};


const static dbl_parser_cfg_data_item_type ebi1_cfg_data_Intel_M18_96MHZ[] =
{
  {WRITE_16_BIT,   (FLASH_BASE_ADDRESS | ( 0x00004903 << 1 )),  0x60    },
  {WRITE_16_BIT,   (FLASH_BASE_ADDRESS | ( 0x00004903 << 1 )),  0x03    },
  {WRITE_16_BIT,   (FLASH_BASE_ADDRESS | ( 0x00004903 << 1 )),  0xFF    },

  {HWIO_OPERATION,   HWIO_ADDRI(EBI1_CSn_CFG0, FLASH_ON_CS),  0x025400             },
  {HWIO_OPERATION,   HWIO_ADDRI(EBI1_CSn_CFG1, FLASH_ON_CS),  0x00010034           },
  {HWIO_OPERATION,   HWIO_ADDR(EBI1_BUFC_CFG),                EBI1_BUFC_CFG_VALUE  },
  /*-----------------------------------------------------------------------
                            End of Configuration
  -----------------------------------------------------------------------*/   
  {END_OF_CFG_DATA,  0x00000000,                    0x00000000  }
};




const static dbl_parser_cfg_data_item_type ebi1_cfg_data_ebi1_default[] = 
{

 {HWIO_OPERATION,   HWIO_ADDRI(EBI1_CSn_CFG0, FLASH_ON_CS),  0x033FF00  },
 {HWIO_OPERATION,   HWIO_ADDRI(EBI1_CSn_CFG1, FLASH_ON_CS),  0x03030020  },
 {HWIO_OPERATION,   HWIO_ADDRI(EBI1_CSn_CFG2, FLASH_ON_CS),  0x3},

  {HWIO_OPERATION,   HWIO_ADDRI(EBI1_CSn_CFG0, 1),    0x030FF00    },
  {HWIO_OPERATION,   HWIO_ADDRI(EBI1_CSn_CFG1, 1),    0x03030020   },
  {HWIO_OPERATION,   HWIO_ADDRI(EBI1_CSn_CFG2, 1),    0x3},

  /*-----------------------------------------------------------------------
                            End of Configuration
  -----------------------------------------------------------------------*/   
  {END_OF_CFG_DATA,  0x00000000,                    0x00000000  }
};

const static dbl_parser_cfg_data_item_type ebi1_samsung_cfg_data_PSRAM_CS1_ASYC[] = 
{

  {HWIO_OPERATION,   HWIO_ADDRI(EBI1_CSn_CFG0, 1),    0x003300    },
  {HWIO_OPERATION,   HWIO_ADDRI(EBI1_CSn_CFG1, 1),    0x00010020  },
  /*-----------------------------------------------------------------------
                            End of Configuration
  -----------------------------------------------------------------------*/   
  {END_OF_CFG_DATA,  0x00000000,                    0x00000000  }
};

const static dbl_parser_cfg_data_item_type ebi1_samsung_cfg_data_PSRAM_CS1_48MHZ[] = 
{

  {READ_16_BIT,      0x9FFFFFE,               0x0       },
  {HWIO_OPERATION,   HWIO_ADDR(PAUSE_TIMER),  48 * 1000  }, 

  {READ_16_BIT,      0x9FFFFFE,               0x0        },
  {HWIO_OPERATION,   HWIO_ADDR(PAUSE_TIMER),  48 * 1000  }, 

  {WRITE_16_BIT,     0x9FFFFFE,               0x1        },
  {HWIO_OPERATION,   HWIO_ADDR(PAUSE_TIMER),  48 * 1000  }, 

  {WRITE_16_BIT,     0x9FFFFFE,               0x81103    },
  {HWIO_OPERATION,   HWIO_ADDR(PAUSE_TIMER),  48 * 1000  }, 
  
  {READ_16_BIT,      0x9FFFFFE,               0x0        },
  {HWIO_OPERATION,   HWIO_ADDR(PAUSE_TIMER),  48 * 1000  }, 

  {READ_16_BIT,      0x9FFFFFE,               0x0        },
  {HWIO_OPERATION,   HWIO_ADDR(PAUSE_TIMER),  48 * 1000  }, 

  {WRITE_16_BIT,     0x9FFFFFE,               0x1        },
  {HWIO_OPERATION,   HWIO_ADDR(PAUSE_TIMER),  48 * 1000  }, 

  {READ_16_BIT,      0x9FFFFFE,               0x0        },
  {HWIO_OPERATION,   HWIO_ADDR(PAUSE_TIMER),  48 * 1000  }, 

  {HWIO_OPERATION,   HWIO_ADDRI(EBI1_CSn_CFG0, 1),    0x003300    },
  {HWIO_OPERATION,   HWIO_ADDRI(EBI1_CSn_CFG1, 1),    0x0031003C  },

  /*-----------------------------------------------------------------------
                            End of Configuration
  -----------------------------------------------------------------------*/   
  {END_OF_CFG_DATA,  0x00000000,                    0x00000000  }
};




const static dbl_parser_cfg_data_item_type ebi1_cfg_data_PSRAM_CS1_48MHZ[] =
{

  {READ_16_BIT,      0x9FFFFFE,               0x0       },
  {HWIO_OPERATION,   HWIO_ADDR(PAUSE_TIMER),  48 * 1000  }, 

  {READ_16_BIT,      0x9FFFFFE,               0x0        },
  {HWIO_OPERATION,   HWIO_ADDR(PAUSE_TIMER),  48 * 1000  }, 

  {WRITE_16_BIT,     0x9FFFFFE,               0x1        },
  {HWIO_OPERATION,   HWIO_ADDR(PAUSE_TIMER),  48 * 1000  }, 

  {WRITE_16_BIT,     0x9FFFFFE,               0x81103    },
  {HWIO_OPERATION,   HWIO_ADDR(PAUSE_TIMER),  48 * 1000  }, 
  
  {READ_16_BIT,      0x9FFFFFE,               0x0        },
  {HWIO_OPERATION,   HWIO_ADDR(PAUSE_TIMER),  48 * 1000  }, 

  {READ_16_BIT,      0x9FFFFFE,               0x0        },
  {HWIO_OPERATION,   HWIO_ADDR(PAUSE_TIMER),  48 * 1000  }, 

  {WRITE_16_BIT,     0x9FFFFFE,               0x1        },
  {HWIO_OPERATION,   HWIO_ADDR(PAUSE_TIMER),  48 * 1000  }, 

  {READ_16_BIT,      0x9FFFFFE,               0x0        },
  {HWIO_OPERATION,   HWIO_ADDR(PAUSE_TIMER),  48 * 1000  }, 

  {HWIO_OPERATION,   HWIO_ADDRI(EBI1_CSn_CFG0, 1),    0x002200    },
  {HWIO_OPERATION,   HWIO_ADDRI(EBI1_CSn_CFG1, 1),    0x0031003C  },

  /*-----------------------------------------------------------------------
                            End of Configuration
  -----------------------------------------------------------------------*/   
  {END_OF_CFG_DATA,  0x00000000,                    0x00000000  }
};

#if 0
const static dbl_parser_cfg_data_item_type ebi1_samsung_cfg_data_PSRAM_CS1_48MHZ[] =
{
/* Start CRE seq to config BCR.
    In ULC CRE pin tied to GPIO_33 thus we need to drive this pin high 
    to program the BCR.
  */

  {HWIO_OPERATION,   HWIO_ADDR(GPIO2_OE_1),   2 << HWIO_SHFT(GPIO2_OE_1, GPIO_OE_33_32)  }, /* OE GPIO 33 = 1      */
  {HWIO_OPERATION,   HWIO_ADDR(GPIO2_OUT_1),  2 << HWIO_SHFT(GPIO2_OUT_1, GPIO_OUT_33_32) }, /* Write "1" to GPIO33 */

  {WRITE_16_BIT,   (EBI1_CS1_BASE | 0x102206),  0x0  }, /* Write BCR */

  {HWIO_OPERATION,   HWIO_ADDR(GPIO2_OUT_1),  0 << HWIO_SHFT(GPIO2_OUT_1, GPIO_OUT_33_32) }, /* Write "0" to GPIO33 */
  
  {HWIO_OPERATION,   HWIO_ADDR(GPIO2_PAGE),  33   },
  {HWIO_OPERATION,   HWIO_ADDR(GPIO2_CFG),   0x1  }, /* pull down*/

  {HWIO_OPERATION,   HWIO_ADDR(GPIO2_OE_1),  0 << HWIO_SHFT(GPIO2_OE_1, GPIO_OE_33_32) }, /* OE GPIO 33 = 0 */

  {HWIO_OPERATION,   HWIO_ADDRI(EBI1_CSn_CFG0, 1),    0x002200    },
  {HWIO_OPERATION,   HWIO_ADDRI(EBI1_CSn_CFG1, 1),    0x0031003C  },

  /*-----------------------------------------------------------------------
                            End of Configuration
  -----------------------------------------------------------------------*/   
  {END_OF_CFG_DATA,  0x00000000,  0x00000000  }

};

#endif
const static dbl_parser_cfg_data_item_type ebi1_samsung_cfg_data_PSRAM_CS1_64MHZ[] =
{

  {READ_16_BIT,      0x9FFFFFE,               0x0       },
  {HWIO_OPERATION,   HWIO_ADDR(PAUSE_TIMER),  48 * 1000  }, 

  {READ_16_BIT,      0x9FFFFFE,               0x0        },
  {HWIO_OPERATION,   HWIO_ADDR(PAUSE_TIMER),  48 * 1000  }, 

  {WRITE_16_BIT,     0x9FFFFFE,               0x1        },
  {HWIO_OPERATION,   HWIO_ADDR(PAUSE_TIMER),  48 * 1000  }, 

  {WRITE_16_BIT,     0x9FFFFFE,               0x81903    },
  {HWIO_OPERATION,   HWIO_ADDR(PAUSE_TIMER),  48 * 1000  }, 
  
  {READ_16_BIT,      0x9FFFFFE,               0x0        },
  {HWIO_OPERATION,   HWIO_ADDR(PAUSE_TIMER),  48 * 1000  }, 

  {READ_16_BIT,      0x9FFFFFE,               0x0        },
  {HWIO_OPERATION,   HWIO_ADDR(PAUSE_TIMER),  48 * 1000  }, 

  {WRITE_16_BIT,     0x9FFFFFE,               0x1        },
  {HWIO_OPERATION,   HWIO_ADDR(PAUSE_TIMER),  48 * 1000  }, 

  {READ_16_BIT,      0x9FFFFFE,               0x0        },
  {HWIO_OPERATION,   HWIO_ADDR(PAUSE_TIMER),  48 * 1000  }, 

  {HWIO_OPERATION,   HWIO_ADDRI(EBI1_CSn_CFG0, 1),    0x002200    },
  {HWIO_OPERATION,   HWIO_ADDRI(EBI1_CSn_CFG1, 1),    0x0031003C  },
/*
  {HWIO_OPERATION,   HWIO_ADDRI(EBI1_CSn_CFG0, 1),    0x003300    },
  {HWIO_OPERATION,   HWIO_ADDRI(EBI1_CSn_CFG1, 1),    0x0031003C  },
*/
  /*-----------------------------------------------------------------------
                            End of Configuration
  -----------------------------------------------------------------------*/   
  {END_OF_CFG_DATA,  0x00000000,                    0x00000000  }
};



const static dbl_parser_cfg_data_item_type ebi1_cfg_data_PSRAM_CS1_64MHZ[] =
{

  {READ_16_BIT,      0x9FFFFFE,               0x0       },
  {HWIO_OPERATION,   HWIO_ADDR(PAUSE_TIMER),  48 * 1000  }, 

  {READ_16_BIT,      0x9FFFFFE,               0x0        },
  {HWIO_OPERATION,   HWIO_ADDR(PAUSE_TIMER),  48 * 1000  }, 

  {WRITE_16_BIT,     0x9FFFFFE,               0x1        },
  {HWIO_OPERATION,   HWIO_ADDR(PAUSE_TIMER),  48 * 1000  }, 

  {WRITE_16_BIT,     0x9FFFFFE,               0x81903    },
  {HWIO_OPERATION,   HWIO_ADDR(PAUSE_TIMER),  48 * 1000  }, 
  
  {READ_16_BIT,      0x9FFFFFE,               0x0        },
  {HWIO_OPERATION,   HWIO_ADDR(PAUSE_TIMER),  48 * 1000  }, 

  {READ_16_BIT,      0x9FFFFFE,               0x0        },
  {HWIO_OPERATION,   HWIO_ADDR(PAUSE_TIMER),  48 * 1000  }, 

  {WRITE_16_BIT,     0x9FFFFFE,               0x1        },
  {HWIO_OPERATION,   HWIO_ADDR(PAUSE_TIMER),  48 * 1000  }, 

  {READ_16_BIT,      0x9FFFFFE,               0x0        },
  {HWIO_OPERATION,   HWIO_ADDR(PAUSE_TIMER),  48 * 1000  }, 

  {HWIO_OPERATION,   HWIO_ADDRI(EBI1_CSn_CFG0, 1),    0x002200    },
  {HWIO_OPERATION,   HWIO_ADDRI(EBI1_CSn_CFG1, 1),    0x0031003C  },

  /*-----------------------------------------------------------------------
                            End of Configuration
  -----------------------------------------------------------------------*/   
  {END_OF_CFG_DATA,  0x00000000,                    0x00000000  }
};

#if 0
const static dbl_parser_cfg_data_item_type ebi1_samsung_cfg_data_PSRAM_CS1_64MHZ[] =
{
/* Start CRE seq to config BCR.
    In ULC CRE pin tied to GPIO_33 thus we need to drive this pin high 
    to program the BCR.
  */

  {HWIO_OPERATION,   HWIO_ADDR(GPIO2_OE_1),   2 << HWIO_SHFT(GPIO2_OE_1, GPIO_OE_33_32)  }, /* OE GPIO 33 = 1      */
  {HWIO_OPERATION,   HWIO_ADDR(GPIO2_OUT_1),  2 << HWIO_SHFT(GPIO2_OUT_1, GPIO_OUT_33_32) }, /* Write "1" to GPIO33 */

  {WRITE_16_BIT,   (EBI1_CS1_BASE | 0x102206),  0x0  }, /* Write BCR */

  {HWIO_OPERATION,   HWIO_ADDR(GPIO2_OUT_1),  0 << HWIO_SHFT(GPIO2_OUT_1, GPIO_OUT_33_32) }, /* Write "0" to GPIO33 */
  
  {HWIO_OPERATION,   HWIO_ADDR(GPIO2_PAGE),  33   },
  {HWIO_OPERATION,   HWIO_ADDR(GPIO2_CFG),   0x1  }, /* pull down*/

  {HWIO_OPERATION,   HWIO_ADDR(GPIO2_OE_1),  0 << HWIO_SHFT(GPIO2_OE_1, GPIO_OE_33_32) }, /* OE GPIO 33 = 0 */

  {HWIO_OPERATION,   HWIO_ADDRI(EBI1_CSn_CFG0, 1),    0x002200    },
  {HWIO_OPERATION,   HWIO_ADDRI(EBI1_CSn_CFG1, 1),    0x0031003C  },

  /*-----------------------------------------------------------------------
                            End of Configuration
  -----------------------------------------------------------------------*/   
  {END_OF_CFG_DATA,  0x00000000,  0x00000000  }

};
#endif


const static dbl_parser_cfg_data_item_type ebi1_samsung_cfg_data_PSRAM_CS1_96MHZ[] =
{

  {READ_16_BIT,      0x9FFFFFE,               0x0       },
  {HWIO_OPERATION,   HWIO_ADDR(PAUSE_TIMER),  48 * 1000  }, 

  {READ_16_BIT,      0x9FFFFFE,               0x0        },
  {HWIO_OPERATION,   HWIO_ADDR(PAUSE_TIMER),  48 * 1000  }, 

  {WRITE_16_BIT,     0x9FFFFFE,               0x1        },
  {HWIO_OPERATION,   HWIO_ADDR(PAUSE_TIMER),  48 * 1000  }, 

  {WRITE_16_BIT,     0x9FFFFFE,               0x81903    },
  {HWIO_OPERATION,   HWIO_ADDR(PAUSE_TIMER),  48 * 1000  }, 
  
  {READ_16_BIT,      0x9FFFFFE,               0x0        },
  {HWIO_OPERATION,   HWIO_ADDR(PAUSE_TIMER),  48 * 1000  }, 

  {READ_16_BIT,      0x9FFFFFE,               0x0        },
  {HWIO_OPERATION,   HWIO_ADDR(PAUSE_TIMER),  48 * 1000  }, 

  {WRITE_16_BIT,     0x9FFFFFE,               0x1        },
  {HWIO_OPERATION,   HWIO_ADDR(PAUSE_TIMER),  48 * 1000  }, 

  {READ_16_BIT,      0x9FFFFFE,               0x0        },
  {HWIO_OPERATION,   HWIO_ADDR(PAUSE_TIMER),  48 * 1000  }, 

  {HWIO_OPERATION,   HWIO_ADDRI(EBI1_CSn_CFG0, 1),    0x003300    },
  {HWIO_OPERATION,   HWIO_ADDRI(EBI1_CSn_CFG1, 1),    0x0031003C  },

  /*-----------------------------------------------------------------------
                            End of Configuration
  -----------------------------------------------------------------------*/   
  {END_OF_CFG_DATA,  0x00000000,                    0x00000000  }
};


const static dbl_parser_cfg_data_item_type ebi1_cfg_data_PSRAM_CS1_96MHZ[] =
{

  {READ_16_BIT,      0x9FFFFFE,               0x0       },
  {HWIO_OPERATION,   HWIO_ADDR(PAUSE_TIMER),  48 * 1000  }, 

  {READ_16_BIT,      0x9FFFFFE,               0x0        },
  {HWIO_OPERATION,   HWIO_ADDR(PAUSE_TIMER),  48 * 1000  }, 

  {WRITE_16_BIT,     0x9FFFFFE,               0x1        },
  {HWIO_OPERATION,   HWIO_ADDR(PAUSE_TIMER),  48 * 1000  }, 

  {WRITE_16_BIT,     0x9FFFFFE,               0x81903    },
  {HWIO_OPERATION,   HWIO_ADDR(PAUSE_TIMER),  48 * 1000  }, 
  
  {READ_16_BIT,      0x9FFFFFE,               0x0        },
  {HWIO_OPERATION,   HWIO_ADDR(PAUSE_TIMER),  48 * 1000  }, 

  {READ_16_BIT,      0x9FFFFFE,               0x0        },
  {HWIO_OPERATION,   HWIO_ADDR(PAUSE_TIMER),  48 * 1000  }, 

  {WRITE_16_BIT,     0x9FFFFFE,               0x1        },
  {HWIO_OPERATION,   HWIO_ADDR(PAUSE_TIMER),  48 * 1000  }, 

  {READ_16_BIT,      0x9FFFFFE,               0x0        },
  {HWIO_OPERATION,   HWIO_ADDR(PAUSE_TIMER),  48 * 1000  }, 

  {HWIO_OPERATION,   HWIO_ADDRI(EBI1_CSn_CFG0, 1),    0x002200    },
  {HWIO_OPERATION,   HWIO_ADDRI(EBI1_CSn_CFG1, 1),    0x0031003C  },

  /*-----------------------------------------------------------------------
                            End of Configuration
  -----------------------------------------------------------------------*/   
  {END_OF_CFG_DATA,  0x00000000,                    0x00000000  }
};





#if 0
const static dbl_parser_cfg_data_item_type ebi1_samsung_cfg_data_PSRAM_CS1_96MHZ[] =
{
/* Start CRE seq to config BCR.
    In ULC CRE pin tied to GPIO_33 thus we need to drive this pin high 
    to program the BCR.
  */

  {HWIO_OPERATION,   HWIO_ADDR(GPIO2_OE_1),   2 << HWIO_SHFT(GPIO2_OE_1, GPIO_OE_33_32)  }, /* OE GPIO 33 = 1      */
  {HWIO_OPERATION,   HWIO_ADDR(GPIO2_OUT_1),  2 << HWIO_SHFT(GPIO2_OUT_1, GPIO_OUT_33_32) }, /* Write "1" to GPIO33 */

  {WRITE_16_BIT,   (EBI1_CS1_BASE | 0x103206),  0x0  }, /* Write BCR */

  {HWIO_OPERATION,   HWIO_ADDR(GPIO2_OUT_1),  0 << HWIO_SHFT(GPIO2_OUT_1, GPIO_OUT_33_32) }, /* Write "0" to GPIO33 */
  
  {HWIO_OPERATION,   HWIO_ADDR(GPIO2_PAGE),  33   },
  {HWIO_OPERATION,   HWIO_ADDR(GPIO2_CFG),   0x1  }, /* pull down*/

  {HWIO_OPERATION,   HWIO_ADDR(GPIO2_OE_1),  0 << HWIO_SHFT(GPIO2_OE_1, GPIO_OE_33_32) }, /* OE GPIO 33 = 0 */

  {HWIO_OPERATION,   HWIO_ADDRI(EBI1_CSn_CFG0, 1),    0x002200    },
  {HWIO_OPERATION,   HWIO_ADDRI(EBI1_CSn_CFG1, 1),    0x0031003C  },

  /*-----------------------------------------------------------------------
                            End of Configuration
  -----------------------------------------------------------------------*/   
  {END_OF_CFG_DATA,  0x00000000,  0x00000000  }

};
#endif


#ifdef FLASH_SHADOW_PSRAM
const static dbl_parser_cfg_data_item_type ebi1_cfg_data_PSRAM_CS0_48MHZ[] =
{ 
 /* Start CRE seq to config BCR.
    In ULC CRE pin tied to GPIO_33 thus we need to drive this pin high 
    to program the BCR.
  */

  {HWIO_OPERATION,   HWIO_ADDR(GPIO2_OE_1),   2 << HWIO_SHFT(GPIO2_OE_1, GPIO_OE_33_32)  }, /* OE GPIO 33 = 1      */
  {HWIO_OPERATION,   HWIO_ADDR(GPIO2_OUT_1),  2 << HWIO_SHFT(GPIO2_OUT_1, GPIO_OUT_33_32) }, /* Write "1" to GPIO33 */

  {WRITE_16_BIT,   (EBI1_CS0_BASE | 0x102206),  0x0  }, /* Write BCR */

  {HWIO_OPERATION,   HWIO_ADDR(GPIO2_OUT_1),  0 << HWIO_SHFT(GPIO2_OUT_1, GPIO_OUT_33_32) }, /* Write "0" to GPIO33 */
  
  {HWIO_OPERATION,   HWIO_ADDR(GPIO2_PAGE),  33   },
  {HWIO_OPERATION,   HWIO_ADDR(GPIO2_CFG),   0x1  }, /* pull down*/

  {HWIO_OPERATION,   HWIO_ADDR(GPIO2_OE_1),  0 << HWIO_SHFT(GPIO2_OE_1, GPIO_OE_33_32) }, /* OE GPIO 33 = 0 */

  {HWIO_OPERATION,   HWIO_ADDRI(EBI1_CSn_CFG0, 0),    0x002200    },
  {HWIO_OPERATION,   HWIO_ADDRI(EBI1_CSn_CFG1, 0),    0x0031003C  },

  /*-----------------------------------------------------------------------
                            End of Configuration
  -----------------------------------------------------------------------*/   
  {END_OF_CFG_DATA,  0x00000000,  0x00000000  }
};


const static dbl_parser_cfg_data_item_type ebi1_cfg_data_PSRAM_CS0_64MHZ[] =
{
 /* Start CRE seq to config BCR.
    In ULC CRE pin tied to GPIO_33 thus we need to drive this pin high 
    to program the BCR.
  */

  {HWIO_OPERATION,   HWIO_ADDR(GPIO2_OE_1),   2 << HWIO_SHFT(GPIO2_OE_1, GPIO_OE_33_32)  }, /* OE GPIO 33 = 1      */
  {HWIO_OPERATION,   HWIO_ADDR(GPIO2_OUT_1),  2 << HWIO_SHFT(GPIO2_OUT_1, GPIO_OUT_33_32) }, /* Write "1" to GPIO33 */

  {WRITE_16_BIT,   (EBI1_CS0_BASE | 0x102206),  0x0  }, /* Write BCR */

  {HWIO_OPERATION,   HWIO_ADDR(GPIO2_OUT_1),  0 << HWIO_SHFT(GPIO2_OUT_1, GPIO_OUT_33_32) }, /* Write "0" to GPIO33 */
  
  {HWIO_OPERATION,   HWIO_ADDR(GPIO2_PAGE),  33   },
  {HWIO_OPERATION,   HWIO_ADDR(GPIO2_CFG),   0x1  }, /* pull down*/

  {HWIO_OPERATION,   HWIO_ADDR(GPIO2_OE_1),  0 << HWIO_SHFT(GPIO2_OE_1, GPIO_OE_33_32) }, /* OE GPIO 33 = 0 */

  {HWIO_OPERATION,   HWIO_ADDRI(EBI1_CSn_CFG0, 0),    0x002200    },
  {HWIO_OPERATION,   HWIO_ADDRI(EBI1_CSn_CFG1, 0),    0x0031003C  },

  /* Extra configuration for enabling EBI1 CS2 in shadow mode */
  {HWIO_OPERATION,   HWIO_ADDR(GPIO_PAGE),  34   },
  {HWIO_OPERATION,   HWIO_ADDR(GPIO_CFG),   0x4  },

  /*-----------------------------------------------------------------------
                            End of Configuration
  -----------------------------------------------------------------------*/   
  {END_OF_CFG_DATA,  0x00000000,                    0x00000000  }
};

const static dbl_parser_cfg_data_item_type ebi1_cfg_data_PSRAM_CS0_96MHZ[] =
{
 /* Start CRE seq to config BCR.
    In ULC CRE pin tied to GPIO_33 thus we need to drive this pin high 
    to program the BCR.
  */
  
  {HWIO_OPERATION,   HWIO_ADDR(GPIO2_OE_1),   2 << HWIO_SHFT(GPIO2_OE_1, GPIO_OE_33_32)  }, /* OE GPIO 33 = 1      */
  {HWIO_OPERATION,   HWIO_ADDR(GPIO2_OUT_1),  2 << HWIO_SHFT(GPIO2_OUT_1, GPIO_OUT_33_32) }, /* Write "1" to GPIO33 */

  {WRITE_16_BIT,   (EBI1_CS0_BASE | 0x103206),  0x0  }, /* Write BCR */

  {HWIO_OPERATION,   HWIO_ADDR(GPIO2_OUT_1),  0 << HWIO_SHFT(GPIO2_OUT_1, GPIO_OUT_33_32) }, /* Write "0" to GPIO33 */
  
  {HWIO_OPERATION,   HWIO_ADDR(GPIO2_PAGE),  33   },
  {HWIO_OPERATION,   HWIO_ADDR(GPIO2_CFG),   0x1  }, /* pull down*/

  {HWIO_OPERATION,   HWIO_ADDR(GPIO2_OE_1),  0 << HWIO_SHFT(GPIO2_OE_1, GPIO_OE_33_32) }, /* OE GPIO 33 = 0 */

  {HWIO_OPERATION,   HWIO_ADDRI(EBI1_CSn_CFG0, 0),    0x002200    },
  {HWIO_OPERATION,   HWIO_ADDRI(EBI1_CSn_CFG1, 0),    0x0031003C  },

  /* Extra configuration for enabling EBI1 CS2 in shadow mode */
  {HWIO_OPERATION,   HWIO_ADDR(GPIO_PAGE),  34   },
  {HWIO_OPERATION,   HWIO_ADDR(GPIO_CFG),   0x4  },

  /*-----------------------------------------------------------------------
                            End of Configuration
  -----------------------------------------------------------------------*/   
  {END_OF_CFG_DATA,  0x00000000,                    0x00000000  }
};
#endif

/***********************************************************************
FUNCTION      dbl_nor_device_probe

DESCRIPTION   This functions helps probe and identify the memory device
              to see if it is one of the known nor devices. For every
              nor device defined in the array nor_ops_array the
              probe_device function is called until the device is
              successfully identified.

DEPENDENCIES  None

RETURN VALUE  fs_device_data structure pointer fs_device_t if the
              nor device was identified successfully else FS_NO_DEVICE
**********************************************************************/
dbl_nor_device
*dbl_nor_device_probe ( void )
{
  int i;
  DBLProbeTbl *prPtr;
  dbl_nor_device *nor_device = NULL;

  prPtr = dbl_probe_info_flash;
  for (i = 0; (i < probe_info_size) && (prPtr->base_addr !=
      (volatile word *)DBL_FLASH_PROBE_END); i++)
  {
    nor_device = prPtr->probe_fcn((word *)prPtr->base_addr);

    if (nor_device != NULL)
    {
      /* We detected a device */
      break;
    }
    else
    {
      prPtr++;
    }
  }

  return nor_device;
} /* End of fs_nor_device_probe */
/*=============================================================================
                              FUNCTION DEFINITIONS
=============================================================================*/

/***********************************************************************
FUNCTION      FLASH_SPANSION_GET_ID_CODES

DESCRIPTION   This function puts the SPANSION flash into Autoselect mode and
              reads the MFG ID code from the flash.

RETURN VALUE   NONE
**********************************************************************/
void
dbl_spansion_get_id_codes(volatile word *baseaddr, word *dest)
{
   *(baseaddr) = 0x00F0 ;

  /* Write the command sequence to place the device into Autoselect mode */
   *(baseaddr + 0x555L) = 0x00AA;
   *(baseaddr + 0x2AAL) = 0x0055;
   *(baseaddr + 0x555L) = 0x0090;

   /* Check both words of the IID */
   dest[0] = *(baseaddr + 0x0000L);

   /* Write the command to reset the device to normal ROM mode */
   *(baseaddr) = 0x00F0 ;

    /* Write command to get device ID */
   *(baseaddr + 0x555L) = 0x00AA;
   *(baseaddr + 0x2AAL) = 0x0055;
   *(baseaddr + 0x555L) = 0x0090;

   dest[1] = *(baseaddr + 0x01L );
   dest[2] = *(baseaddr + 0x0EL );
   dest[3] = *(baseaddr + 0x0FL );

    /* Write the command to reset the device to normal ROM mode */
   *(baseaddr) = 0x00F0 ;
}

/***********************************************************************
FUNCTION      FLASH_SAMSUNG_GET_ID_CODES

DESCRIPTION   This function puts the SAMSUNG flash into Autoselect mode and
              reads the MFG ID code from the flash.

RETURN VALUE   NONE
**********************************************************************/
void
dbl_samsung_get_id_codes(volatile word *baseaddr, word *dest)
{
  *(baseaddr) = 0x00F0 ;

  /* Write the command sequence to place the device into Autoselect mode */
  *(baseaddr + 0x555L) = 0x00AA;
  *(baseaddr + 0x2AAL) = 0x0055;
  *(baseaddr + 0x555L) = 0x0090;

  /* Check both words of the IID */
  dest[0] = *(baseaddr + 0x0000L);

  /* Write the command to reset the device to normal ROM mode */
  *(baseaddr) = 0x00F0 ;

  /* Write command to get device ID */
  *(baseaddr + 0x555L) = 0x00AA;
  *(baseaddr + 0x2AAL) = 0x0055;
  *(baseaddr + 0x555L) = 0x0090;

  dest[1] = *(baseaddr + 0x01L );
  dest[2] = *(baseaddr + 0x0EL );
  dest[3] = *(baseaddr + 0x0FL );

  /* Write the command to reset the device to normal ROM mode */
  *(baseaddr) = 0x00F0 ;
}

/***********************************************************************
FUNCTION      FLASH_INTEL_COPY_CFI_DATA

DESCRIPTION   This function puts the INTEL flash into CFI read mode and
              copies count bytes from the CFI data into the dest

RETURN VALUE   NONE
**********************************************************************/
void
flash_intel_copy_cfi_data(volatile word *baseaddr, word *dest, word count)
{
  word tmp = 0;

  /* Set the Read CFI Query command */
  *(baseaddr) = 0x98;

  for (tmp = 0; tmp < count; tmp ++)
    *(dest++) = *(baseaddr+tmp);

  /* Set the Flash back to Read mode */
  *(baseaddr) = 0xFF;
}
/*===========================================================================

FUNCTION dbl_intel_probe

DESCRIPTION
  Detect the presence of an Intel flash device.

DEPENDENCIES
  None

RETURN VALUE
  NULL           - No Intel device detected.
  otherwise      - Pointer to fsi_device

SIDE EFFECTS
  None

===========================================================================*/
dbl_nor_device *dbl_intel_probe (volatile word *baseaddr)
{
  word codes[4];
  dbl_nor_device const **dev;

  /* Call function to go get the codes from the device */
  flash_intel_copy_cfi_data(baseaddr, codes, 2);

  /*  See if we can find a match for the MFG and part ID */
  for (dev = intel_parts; *dev != NULL; dev++)
  {
    if ((codes[0] == (*dev)->codes[0]) && 
    	(codes[1] == (*dev)->codes[1]))
    {
      break;
    }
  }

  /* 
   * If we did not find a match, the dev pointer is holding NULL
   * which indicates to the calling function that we did not
   * find a match, otherwise, it points to the device table for
   * the type of flash part we identified.
   */
  return (dbl_nor_device *) *dev;
}

/*===========================================================================

FUNCTION DBL_SPANSION_PROBE

DESCRIPTION
  Detect the presence of an SPANSION flash device.

DEPENDENCIES
  

RETURN VALUE
  NULL           - No SPANSION device detected.
  otherwise      - Pointer to fsi_nor_device

SIDE EFFECTS
  None

===========================================================================*/

dbl_nor_device * 
dbl_spansion_probe (volatile word *baseaddr)
{
  dbl_nor_device const **dev;

  word  codes[4];
  int ids, ids_found;

  /* Call function to go get the codes from the device */
  dbl_spansion_get_id_codes(baseaddr, codes);
  
  /* Scan for the codes.  Check indicated number of ids for
     each component in the list.  Stop when we find a match.
     our indicator of no match. */
  for (dev = spansion_parts; *dev != NULL; dev++) {
    ids_found = 0;
    for (ids = 0; ids < (*dev)->num_ids; ids++)
    {
      if (codes[ids] == (*dev)->codes[ids])
      {
        /* Every time we find a match, increment ids_found.
         * When we exit the loop, if ids_found equals the number
         * of IDs we had to match we found the part
         */
        ids_found++;
      }
    }
    if (ids_found == (*dev)->num_ids)
    {
      break;
    }
  }

  return (dbl_nor_device *) *dev;

}/* dbl_spansion_probe */

/*===========================================================================

FUNCTION DBL_SAMSUNG_PROBE

DESCRIPTION
  Detect the presence of a SAMSUNG flash device.

DEPENDENCIES
  

RETURN VALUE
  NULL           - No SAMSUNG device detected.
  otherwise      - Pointer to fsi_nor_device

SIDE EFFECTS
  None

===========================================================================*/

dbl_nor_device * 
dbl_samsung_probe (volatile word *baseaddr)
{
  dbl_nor_device const **dev;

  word  codes[4];
  int ids, ids_found;

  /* Call function to go get the codes from the device */
  dbl_samsung_get_id_codes(baseaddr, codes);
  
  /* Scan for the codes.  Check indicated number of ids for
     each component in the list.  Stop when we find a match.
     our indicator of no match. */
  for (dev = samsung_parts; *dev != NULL; dev++)
  {
    ids_found = 0;
    for (ids = 0; ids < (*dev)->num_ids; ids++)
    {
      if (codes[ids] == (*dev)->codes[ids])
      {
        /* Every time we find a match, increment ids_found.
         * When we exit the loop, if ids_found equals the number
         * of IDs we had to match we found the part
         */
        ids_found++;
      }
    }
    if (ids_found == (*dev)->num_ids)
    {
      break;
    }
  }

  return (dbl_nor_device *) *dev;

}/* dbl_samsung_probe */

#ifdef FLASH_SHADOW_PSRAM
/*===========================================================================

FUNCTION  DBL_EBI1_SHADOW_PSRAM_CONFIGURE

DESCRIPTION
 This function will configure the shadow PSRAM on the EBI1 interface.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dbl_ebi1_shadow_psram_configure
(
  const boot_clk_speed_type *configured_clk_speed
)
{
  if( configured_clk_speed->ebi1 == 48 )
  {
    dbl_parse_cfg_data(ebi1_cfg_data_PSRAM_CS0_48MHZ);
  }
  else if( configured_clk_speed->ebi1 == 64 )
  {
    dbl_parse_cfg_data(ebi1_cfg_data_PSRAM_CS0_64MHZ);
  }
  else if( configured_clk_speed->ebi1 == 96 )
  {
    dbl_parse_cfg_data(ebi1_cfg_data_PSRAM_CS0_96MHZ);
  }
  else
  {
    DBL_ERR_FATAL(DBL_ERR_EBI1_CFG_FAILED);
  }
}
#endif
/*===========================================================================

FUNCTION  DBL_EBI1_DATA_PSRAM_CONFIGURE

DESCRIPTION
 This function will configure the Data PSRAM on EBI1 interface.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dbl_ebi1_data_psram_configure
(
  const boot_clk_speed_type *configured_clk_speed
)
{
    dbl_nor_device *dev = dbl_nor_device_probe();
	
	DBL_VERIFY(dev != NULL, DBL_ERR_NOR_DETECTION_FAILED );
#ifndef FEATURE_USES_LOWMEM
    if((dev == &K5N6433ABM) || (dev == &K5N6433ATM) || (dev == &K5N2833ATB) || (dev == &K5N2833ABB) || (dev == &K5N5629ABM))
    {
        dbl_parse_cfg_data(ebi1_cfg_data_ebi1_default);
        dbl_parse_cfg_data(ebi1_samsung_cfg_data_PSRAM_CS1_ASYC);
    }
	else if((dev == &K5N5629ATC) || (dev == &K5N5629ABC) || (dev == &K5N5629AUC))
	{
        if( configured_clk_speed->ebi1 == 48 )
        {
            dbl_parse_cfg_data(ebi1_samsung_cfg_data_PSRAM_CS1_48MHZ);
        }
        else if( configured_clk_speed->ebi1 == 64 )
        {
            dbl_parse_cfg_data(ebi1_samsung_cfg_data_PSRAM_CS1_64MHZ);
        }
        else if( configured_clk_speed->ebi1 == 96 )
        {
            dbl_parse_cfg_data(ebi1_samsung_cfg_data_PSRAM_CS1_96MHZ);
        }
        else
        {
            DBL_ERR_FATAL(DBL_ERR_EBI1_CFG_FAILED);
        }
	}
  	else
#endif
	{
        if( configured_clk_speed->ebi1 == 48 )
        {
            dbl_parse_cfg_data(ebi1_cfg_data_PSRAM_CS1_48MHZ);
        }
#ifndef FEATURE_USES_LOWMEM
        else if( configured_clk_speed->ebi1 == 64 )
        {
            dbl_parse_cfg_data(ebi1_cfg_data_PSRAM_CS1_64MHZ);
        }
        else if( configured_clk_speed->ebi1 == 96 )
        {
            dbl_parse_cfg_data(ebi1_cfg_data_PSRAM_CS1_96MHZ);
        }	
        else
        {
            DBL_ERR_FATAL(DBL_ERR_EBI1_CFG_FAILED);
        }
#endif
    }
}

/*===========================================================================

FUNCTION  DBL_EBI1_NOR_CONFIGURE

DESCRIPTION
 This function will configure the EBI1 NOR

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dbl_ebi1_nor_configure
(
  const boot_clk_speed_type *configured_clk_speed
)
{
  dbl_nor_device *dev = dbl_nor_device_probe();

  DBL_VERIFY(dev != NULL, DBL_ERR_NOR_DETECTION_FAILED );
#ifndef FEATURE_USES_LOWMEM
  if(dev == &S29WS256N0SB)
  {
    if( configured_clk_speed->ebi1 == 48 )
    {
      dbl_parse_cfg_data(ebi1_cfg_data_S29WSN0SB_48MHZ);
    }
    else if( configured_clk_speed->ebi1 == 64 )
    {
      dbl_parse_cfg_data(ebi1_cfg_data_S29WSN0SB_64MHZ);
    }
    else
    {
      DBL_ERR_FATAL(DBL_ERR_EBI1_CFG_FAILED);
    }
  }
  else if (dev == &S29WS512P)
  {
    if( configured_clk_speed->ebi1 == 48 )
    {
      dbl_parse_cfg_data(ebi1_cfg_data_S29WSP_48MHZ);
    }
    else if( configured_clk_speed->ebi1 == 64 )
    {
      dbl_parse_cfg_data(ebi1_cfg_data_S29WSP_64MHZ);
    }
    else if( configured_clk_speed->ebi1 == 96 )
    {
      dbl_parse_cfg_data(ebi1_cfg_data_S29WSP_96MHZ);
    }
    else
    {
      DBL_ERR_FATAL(DBL_ERR_EBI1_CFG_FAILED);
    }
  }
  else if ( (dev == &Intel_1024M18_ADMux) || (dev == &Intel_512M18_ADMux) || (dev == &Intel_256M18_ADMux) || (dev == &Intel_128M18_ADMux))
  {
    if( configured_clk_speed->ebi1 == 48 )
    {
      dbl_parse_cfg_data(ebi1_cfg_data_Intel_M18_48MHZ);
    }
    else if( configured_clk_speed->ebi1 == 64 )
    {
      dbl_parse_cfg_data(ebi1_cfg_data_Intel_M18_64MHZ);
    }
  	else if( configured_clk_speed->ebi1 == 96 )
    {
      dbl_parse_cfg_data(ebi1_cfg_data_Intel_M18_96MHZ);
    }
  
    else
    {
      DBL_ERR_FATAL(DBL_ERR_EBI1_CFG_FAILED);
    }
  } 
  else if ((dev == &K5N5629ABM))
  {
    if( configured_clk_speed->ebi1 == 48 )
    {
      dbl_parse_cfg_data(ebi1_cfg_data_K5N5629ABM_48MHZ);
    }
	else if(configured_clk_speed->ebi1 == 64 )
	{
	  dbl_parse_cfg_data(ebi1_cfg_data_K5N5629ABM_64MHZ);
	}
    else if( configured_clk_speed->ebi1 == 96 )
    {
      dbl_parse_cfg_data(ebi1_cfg_data_K5N5629ABM_96MHZ);
    }
    else
    {
      DBL_ERR_FATAL(DBL_ERR_EBI1_CFG_FAILED);
    }
  }
  else if ((dev == &K5N5629ATC) || (dev == &K5N5629ABC) || (dev == &K5N5629AUC))
  {
    if( configured_clk_speed->ebi1 == 48 )
    {
      dbl_parse_cfg_data(ebi1_cfg_data_K5N5629ATC_48MHZ);
    }
	else if(configured_clk_speed->ebi1 == 64 )
	{
	  dbl_parse_cfg_data(ebi1_cfg_data_K5N5629ATC_64MHZ);
	}
    else if( configured_clk_speed->ebi1 == 96 )
    {
      dbl_parse_cfg_data(ebi1_cfg_data_K5N5629ATC_96MHZ);
    }
    else
    {
      DBL_ERR_FATAL(DBL_ERR_EBI1_CFG_FAILED);
    }
  }
  else if (dev == &Intel_64W18_ADMux)
  {
     dbl_parse_cfg_data(ebi1_cfg_data_Intel_W18_48MHZ);
  }
  else if ( dev == &M58LR128KC)
  {
		dbl_parse_cfg_data(ebi1_cfg_data_Intel_M58LR_48MHZ);
  }
  else if(dev == &M36W0R5040U6ZS)
  {
     dbl_parse_cfg_data(ebi1_cfg_data_Intel_M58_48MHZ);
  }
  else if (dev == &K5N6433ABM || dev == &K5N6433ATM)
  {
    dbl_parse_cfg_data(ebi1_cfg_data_K5N6433ABM_48MHZ);
  }
  else if(dev == &K5N2833ATB || dev == &K5N2833ABB)
  {
    dbl_parse_cfg_data(ebi1_cfg_data_K5N6433ABM_48MHZ);
  }
  else
  {
    DBL_ERR_FATAL( DBL_ERR_NOR_DETECTION_FAILED );
  }
#else
  dbl_parse_cfg_data(ebi1_cfg_data_Intel_M58_48MHZ);
#endif
}

/*===========================================================================

FUNCTION  DBL_EBI1_CONFIGURE

DESCRIPTION
 This function will configure the controller on the EBI1 interface.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dbl_ebi1_configure
(
  const boot_clk_speed_type *configured_clk_speed
)
{

  dbl_parse_cfg_data(ebi1_cfg_data_common);

#ifdef FLASH_SHADOW_PSRAM
  dbl_ebi1_shadow_psram_configure( configured_clk_speed );
#endif

  dbl_ebi1_data_psram_configure( configured_clk_speed );

  dbl_ebi1_nor_configure( configured_clk_speed );



} /* dbl_ebi1_configure */

/*===========================================================================

FUNCTION  DBL_EBI1_SHARE_DATA

DESCRIPTION
 This function will save the DDR information.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dbl_ebi1_share_data
(
  dbl_if_shared_ddr_device_info_type *ddr_dev
)
{

} /* dbl_ebi1_share_data */






