#ifndef IXDECRYPTOR_H
#define IXDECRYPTOR_H

/*===========================================================================

                        S E C U R I T Y    S E R V I C E S
   
                             I X D E C R Y P T O R
						 
	                        H E A D E R  F I L E

FILE:  IxDecryptor.h


DESCRIPTION

	This header file defines the interface to IxDecryptor.
 

EXTERNALIZED FUNCTIONS : 


  Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.  
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE
                            
  $Header: //source/qcom/qct/core/securemsm/smecom/main/latest/com/inc/IxDecryptor.h#3 $
  


when       who     what, where, why
--------   ---     ----------------------------------------------------------

===========================================================================*/


/*===========================================================================
                           
							 INCLUDE FILES

============================================================================*/
#include "IxErrno.h"
#include "comdef.h"

/*===========================================================================

                           CLASS DEFINITION

===========================================================================*/

class IxDecryptor
{   
  public:

      /*------------------------------------------------------------------------
                               PUBLIC TYPES
      ------------------------------------------------------------------------*/
  
     /* Attribute Types */
     typedef enum DecryptorEnums 
     {
	   CIPHER_PARAMS                = 1,
           CIPHER_ENCRYPTION_METHOD     = 2,
           CIPHER_ENCRYPTION_KEY_VALUE  = 3,
           CIPHER_ENCRYPTION_IV_LENGTH  = 4,
           CIPHER_ENCRYPTION_PADDING    = 5,
           CIPHER_SELECTIVE_ENCRYPTION  = 6,
           CIPHER_UNPROTECTED           = 7,
	   ATTRIB_MAX
     } DecryptorAttribEType;

     /*------------------------------------------------------------------------
                               PUBLIC METHODS
     ------------------------------------------------------------------------*/
     
     virtual ~IxDecryptor(){};

     /** SetAttr
     *  This function is used for setting attributes needed to
     *   decrypt  a packet 
     *
     *
     * @param eAttrId      - One of DecryptorAttribEType
     * @param pAttrPtr     - Pointer to the attribute to be set
     * @param nAttrLen     - Length of attribute
     *  
     * @return IxErrnoType  - Error number or Success.
     */
     virtual IxErrnoType  SetAttr (    DecryptorAttribEType                  eAttrId,
                                       byte*                                   pAttr,
                                       uint32                                nAttrLen
                                   ) = 0;

     /** Read
     * This function is used to read a decrypted packet
     *
     * @param pEncryptedBuf      - Pointer to buffer containing the encrypted packet
     * @param nEncryptedBufSize  - size of the above buffer
     * @param pPlaintxtBuf       - Pointer to the buffer to place the
     *                             decrypted packet ( Buffer
     *                             allocated by the caller )
     * @param pPlaintxtBufSize   - This is a in-out parameter, where
     *                             on input the caller sets the size
     *                             of the allocated buffer and on
     *                             return the method updates with the
     *                             actual number of bytes that needs
     *                             to be read from that buffer
     *  
     * @return IxErrnoType  - Error number or Success.
     */

      virtual IxErrnoType  Read (   byte*    pEncryptedBuf,
                                    int     nEncryptedBufSize
                                ) = 0;
};
       
#endif /* IXDECRYPTOR_H */
