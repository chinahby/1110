#ifndef IXDECRYPTORCLIENT_H
#define IXDECRYPTORCLIENT_H

/*===========================================================================

                       I X D E C R Y P T O R C L I E N T 

                            H E A D E R    F I L E




DESCRIPTION
  This is the header file defining an IxDecryptor client

EXTERNALIZED FUNCTIONS


  Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

  $Header: //source/qcom/qct/core/securemsm/smecom/main/latest/com/src/IxDecryptorClient.h#1 $
  $DateTime: 2007/11/04 10:21:15 $
  $Author: jmiera $


when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/03/07    ws     Initial Revision

=============================================================================*/
/*===========================================================================
                           
			 INCLUDE FILES

============================================================================*/
#include "IxErrno.h"
#include "IxDecryptor.h"
#include "AEEContentHandlerTypes.h"

/*===========================================================================

                           CLASS DEFINITION

===========================================================================*/

class IxDecryptorClient : public IxDecryptor
{   
  public:

     /*------------------------------------------------------------------------
                               PUBLIC METHODS
     ------------------------------------------------------------------------*/
     /* Constructor */
     IxDecryptorClient();
     IxDecryptorClient( IxDecryptor                 *decryptorServer,
                        AEEContentDRMSystemEType     drm_system );  

     virtual ~IxDecryptorClient();

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
                                       byte*                                 pAttr,
                                       uint32                                nAttrLen
                                   );

     /** Read
     * This function is used to read a decrypted packet
     *
     * @param pEncryptedBuf      - Pointer to buffer containing the encrypted packet
     *                             and returned plaintext buffer
     * @param nEncryptedBufSize  - size of the above buffer
     *  
     * @return IxErrnoType  - Error number or Success.
     */

      virtual IxErrnoType  Read (   byte*    pEncryptedBuf,
                                    int   nEncryptedBufSize
                                );

  private:

      IxDecryptor              *associated_server;
      AEEContentDRMSystemEType  system_id;

};
       
#endif /* IxDecryptorClient_H */
