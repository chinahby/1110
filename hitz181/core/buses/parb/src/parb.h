#ifndef PARB_H_
#define PARB_H_
/*===========================================================================

        Pacing Arbiter   F O R   Q S C 6 0 5 5

DESCRIPTION
  All the declarations and definitions necessary to support interaction
  with the PARB functions for the QSC6055.

REFERENCES

Copyright (c) 2006, 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/qsc1100/drivers/parb/parb.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/29/07   fs      Initial creation
===========================================================================*/

/* HCLK to grant info indexing enum */

typedef enum {
    /* This enum is used as an index into slot_info_table, do not modify */
    HCLK_96mhz=0,
    HCLK_64mhz=1
    /* Add more... */
    
} gt_slot_info_table_index_type;

/* Function prototypes */

void parb_init(void);

int parb_grant_bandwidth(gt_slot_info_table_index_type hclk);

int ahb_bus_lock(void);

int ahb_bus_unlock(void);

#ifdef FEATURE_SIMULATE_LACK_OF_BW_TO_GPS
#error code not present
#endif

#endif /* PARB_H_ */

