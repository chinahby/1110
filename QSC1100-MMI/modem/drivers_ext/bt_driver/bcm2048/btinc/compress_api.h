/*****************************************************************************/
/*                                                                           */
/*  Name:          compress_api.h                                            */
/*                                                                           */
/*  Description:   this file defines compression wrapper functions           */
/*                                                                           */
/*                                                                           */
/*  Date        Modification                                                 */
/*  ------------------------                                                 */
/*  10/07/01    Ash     Create                                               */
/*                                                                           */
/*                                                                           */
/*  Copyright (c) 1999-2001, Widcomm Inc., All Rights Reserved.              */
/*  Widcomm Bluetooth Core. Proprietary and confidential.                    */
/*****************************************************************************/

#ifndef COMPRESS_API_H
#define COMPRESS_API_H
/************************************************************************/
/* Function:    void *    Compress_Close(...)                           */
/*                                                                      */
/* Purpose:    Release resources allocated by the compression process.  */
/*                                                                      */
/* Parameters:    void    p    - Pointer to compression control structure.*/
/************************************************************************/
extern    void    Compress_Close (void *p);

/************************************************************************/
/* Function:    void *    Compress_Data(...)                            */
/*                                                                      */
/* Purpose:    Compress a block of data.                                */
/*                                                                      */
/* Params:    BT_HDR *    p_in    - Buffered data to compress.          */
/*                                  - p_in->event must contain the      */
/*                                    original value of p_in->offset    */
/*                                    (that points to the HCI header).  */
/*            UINT16        outmax    - Maximum output capacity.        */
/*            void *        p_compr    - Compression control structure. */
/*          int *       status  - Status of call to Compress_Data       */
/*                          = Z_SUCCESS (0)                             */
/*                          = Z_STREAM_END (1)                          */
/*                          = Z_MORE_DATA (50)                          */
/*                          = Z_STREAM_ERROR (-2)                       */
/*                          = Z_BUF_ERROR (-5)                          */
/*                                                                      */
/* Returns:    Pointer to output buffer if process resulted in output of*/
/*            compressed data.  Returns status of call in the parameter */
/*          specified as "status".                                      */
/************************************************************************/
extern    void *    Compress_Data(BT_HDR * p_in, UINT16 outmax, void * p_compr, int * status);

/************************************************************************/
/* Parameters associated with Compress_Data():                          */
/*                                                                      */
/*  COMPRESSION_MARKER  - byte which marks end of compressed data block.*/
/*  Z_MORE_DATA         - exit code indicating more processing req'd.   */
/*  Z_SUCCESS           - the compression operation has completed.      */
/************************************************************************/
#define Z_MORE_DATA         50
#define Z_SUCCESS           0

/************************************************************************/
/* Function:    void *    Compress_Init(...)                            */
/*                                                                      */
/* Purpose:    Initialize the compression / decompression operation     */
/*            control structures.                                       */
/*                                                                      */
/* Parameters:    UINT8    mem_level    - Memory alloc parameter (2 - 8).*/
/*                UINT8    wbits        - LZ77 window size (10 - 15).   */
/*              UINT8   *p_mem_pool - memory pool                       */
/*              UINT16  mem_pool_size - memory pool size                */
/*                                                                      */
/* Returns:    Pointer to compression control structure (NULL if unable */
/*            to complete initialization).                              */
/************************************************************************/
extern    void *    Compress_Init(UINT8 mem_level, UINT8 wbits, 
                              UINT8 *p_mem_pool, UINT32 mem_pool_size);

/************************************************************************/
/* Function:    void *    Decompress_Data(...)                          */
/*                                                                      */
/* Purpose:    Decompress a block of data.                              */
/*                                                                      */
/* Params:    BT_HDR *    p_in    - Buffered data to decompress.        */
/*          BT_HDR *    p_out   - Output buffer (to receive data).      */
/*            void *        p_compr    - Compression control structure. */
/*                                                                      */
/* Assumptions: If p_base is NULL it is assumed that p_in is a START    */
/*              packet.  Otherwise p_in is a CONTINUATION packet.       */
/*                                                                      */
/* Returns:    int status  - Status of call to Decompress_Data          */
/*                          = Z_SUCCESS (0)                             */
/*                          = Z_STREAM_END (1)                          */
/*                          = Z_NEED_DICT (2)                           */
/*                          = Z_OUTPUT_COMPLETE (50)                    */
/*                          = Z_LEFTOVER_DATA (60)                      */
/*                          = Z_STREAM_ERROR (-2)                       */
/*                          = Z_DATA_ERROR (-3)                         */
/*                          = Z_MEM_ERROR (-4)                          */
/*                          = Z_BUF_ERROR (-5)                          */
/************************************************************************/
extern    int Decompress_Data(BT_HDR * p_in, BT_HDR * p_out, void * p_compr);

/************************************************************************/
/* Function:    Compress_Get_FixMemorySize                              */
/*                                                                      */
/* Purpose:    Return the estimate the fix memory size base on window bits */
/*          and memory level                                            */
/*                                                                      */
/* Parameters:    wbits: window bits                                    */
/*                mem_level: memory level                               */
/************************************************************************/
extern UINT32 Compress_Get_FixMemorySize(UINT8 mem_level, UINT8 wbits);

/************************************************************************/
/* Parameters associated with Decompress_Data():                        */
/*                                                                      */
/* Z_OUTPUT_COMPLETE    - exit code indicating no more data will be     */
/*                          received for the current packet under       */
/*                          construction.                               */
/************************************************************************/
#define Z_OUTPUT_COMPLETE   50
#define Z_LEFTOVER_DATA     60
#define Z_SUCCESS           0
#endif
