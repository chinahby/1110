/*===========================================================================
                           OEMLoggerBTILMsgs.h
                              
  SERVICES: ILoggerBTIL Common FIFO Message Information
  
  GENERAL DESCRIPTION:
  typedefs, definitions for the ILoggerBTIL FIFO

  NOTES:
    This file should only be included from OEMLoggerBTIL.c
  
        Copyright © 1999-2004 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
============================================================================*/
#ifndef OEMLOGGERBTILMSGS_H
#define OEMLOGGERBTILMSGS_H

// Warning: Do not change the FIFO name below.  BTIL is expecting a fifo
// with this name.
#define BTIL_LOGGER_FIFO_NAME "fifo:/~0x0101730f/infifos/btillogger"

#define BTIL_LOGGER_FIFO_SIZE 2048

#endif /* OEMLOGGERBTILMSGS_H */

