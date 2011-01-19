/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/obexauth.c#1 $ 

  when        who  what, where, why
  ----------  ---  ------------------------------------------------------------
  2008-11-19   NM  Fixed issue of sending Auth response in response to a Bad 
                   Obex Auth Challenge Packet with no nonce (mandatory tag)

*    #7       21 Mar 2007            MH
* Added realm string type check while storing challenge
*
*    #6       18 Oct 2006             PR
* Merged OI Code v3.0.14rev1revQ
*
*    #5       19 Sept 2006            MH
* Compose challenge function changed to support Realm Information 
===========================================================================*/
/**
@file  
OBEX authentication support
*/

/**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2003 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/
#define __OI_MODULE__ OI_MODULE_OBEX_SRV

#include "oi_debug.h"
#include "oi_argcheck.h"
#include "oi_assert.h"
#include "oi_time.h"
#include "oi_memmgr.h"
#include "oi_obexspec.h"
#include "oi_utils.h"
#include "oi_obexauth.h"
#include "md5_hash.h"

#ifndef OI_CODE
/* This is included to get BT Messages */
#include "btmsg.h"
#endif

void OI_OBEXAUTH_Reset(OBEX_COMMON *common)
{
    OI_DBGTRACE(("OI_OBEXAUTH_Reset\n"));

    OI_MemZero(common->authentication, sizeof(OBEX_AUTHENTICATION));
}



OI_STATUS OI_OBEXAUTH_SaveAuthInfo(OBEX_COMMON *common,
                                   const OI_BYTE *userId,
                                   OI_UINT8 userIdLen,
                                   const OI_CHAR *password)
{
    OI_UINT len = OI_StrLen(password);

#ifndef OI_CODE
    BT_MSG_DEBUG("BT OBEX : Save Auth Info : PasswordLen=%d : UIDLen=%d ",len, userIdLen,0);
#endif
OI_DBGTRACE(("OI_OBEXAUTH_SaveAuthInfo(userId=%?s,password=%s)\n", userId, userIdLen, password));

    /*
     * Truncate to maximum password length. This means that authentication
     * with really long passwords will not work.
     */
    if (len > OI_OBEX_MAX_PASSWORD_LEN) {
        return OI_OBEX_PASSWORD_TOO_LONG;
    }
    OI_MemCopy(common->authentication->password, password, len);
    common->authentication->passwordLen = len;

    ARGCHECK(userIdLen <= OI_OBEX_MAX_USERID_LEN);

    if ((userId != NULL) && (userIdLen > 0)) {
        OI_MemCopy(common->authentication->userId, userId, userIdLen);
        common->authentication->userIdLen = userIdLen;
    } else {
        common->authentication->userIdLen = 0;
    }

    return OI_OK;
}



OI_STATUS OI_OBEXAUTH_SaveChallenge(OBEX_COMMON *common,
                                    OI_OBEX_HEADER *challenge)
{
    OBEX_AUTHENTICATION *auth = common->authentication;
    OI_STATUS status = OI_OK;
    OI_UINT8 options = 0;
    OI_UINT i = 0;
#ifndef OI_CODE
    OI_UINT8 realmStrType ;
    OI_BOOL  isNonceAvail = 0;
#endif


    OI_DBGTRACE(("OI_OBEXAUTH_SaveChallenge\n"));

    OI_ASSERT(auth);
    OI_ASSERT(challenge->id == OI_OBEX_HDR_AUTHENTICATION_CHALLENGE);

#ifndef OI_CODE
    BT_MSG_DEBUG( "BT OBEX: Save Challenge", 0, 0, 0 );
#endif

    /*
     * Locate the digest challenge nonce and save it.
     */
    while (i < challenge->val.byteseq.len) {
        OI_UINT8 tag = challenge->val.byteseq.data[i++];
        OI_UINT8 len = challenge->val.byteseq.data[i++];

#ifndef OI_CODE
        BT_MSG_DEBUG("BT OBEX: Save Challenge : Tag=%d : Len=%d", tag, len, 0 );
#endif

        switch(tag) {
            case OI_OBEX_CHALLENGE_DIGEST_REALM_TAG:
#ifdef OI_CODE
#error code not present
#else           
                if (len > 0) {
                    if (len > OI_OBEX_MAX_REALM_LEN) {
                        status = OI_OBEX_VALUE_NOT_ACCEPTABLE;
                    } else {
                        realmStrType = challenge->val.byteseq.data[i++];
                        BT_MSG_DEBUG( "BT OBEX: Digest Realm  Tag found RealmLen=%d:RealmStrType=%d", 
                                                                                len, realmStrType,0);
                        switch(realmStrType){
                            case REALM_STR_ASCII :
                            case REALM_STR_ISO_8859_1 :
                            case REALM_STR_ISO_8859_2 :
                            case REALM_STR_ISO_8859_3 :
                            case REALM_STR_ISO_8859_4 :
                            case REALM_STR_ISO_8859_5 :
                            case REALM_STR_ISO_8859_6 :
                            case REALM_STR_ISO_8859_7 :
                            case REALM_STR_ISO_8859_8 :
                            case REALM_STR_ISO_8859_9 :
                            case REALM_STR_UNICODE    :
                                auth->realmLen = len ;
                                auth->realmStrType = realmStrType ;
                                OI_MemCopy(auth->realmStr, &challenge->val.byteseq.data[i], len);
                                break;
                            default :
                                BT_MSG_DEBUG( "BT OBEX: Invalid Realm String Type",0,0,0); 
                                status = OI_OBEX_VALUE_NOT_ACCEPTABLE;
                                break;
                        } //End switch
                    }
                } // if len
#endif
                break;
            case OI_OBEX_CHALLENGE_DIGEST_OPTIONS_TAG:
                if (len != 1) {
                    status = OI_OBEX_INVALID_AUTH_DIGEST;
                } else {
                    options = challenge->val.byteseq.data[i];
                    OI_DBGPRINT2(("OBEX receive challenge options = %1x", options));
                }
                break;
            case OI_OBEX_CHALLENGE_DIGEST_NONCE_TAG:
                if (len != OI_OBEX_AUTH_DIGEST_LEN) {
                    status = OI_OBEX_INVALID_AUTH_DIGEST;
                } else {
#ifndef OI_CODE
                    isNonceAvail = 1; 
#endif
                    OI_MemCopy(auth->challengeDigest, &challenge->val.byteseq.data[i], OI_OBEX_AUTH_DIGEST_LEN);
                }
                break;
            default:
                OI_DBGPRINT2(("Unexpected tag in challenge %d", tag));
                break;
        }
        i += len;
    }

#ifndef OI_CODE
    if (!isNonceAvail) {
        BT_MSG_DEBUG("OBEX client : Nonce not present in auth challenge header", 0, 0, 0);
        status = OI_OBEX_BAD_PACKET;

        return status;
    }
#endif
    /*
     * Set options.
     */
    auth->userIdRequired = (options & OI_OBEX_AUTH_USERID_OPTION) != 0;
    common->readOnly = (options & OI_OBEX_AUTH_READONLY_OPTION) != 0;

#ifndef OI_CODE
    BT_MSG_DEBUG("BT OBEX: Save Challenge...UIDReqd = %d : Readonly Flag=%d", auth->userIdRequired, common->readOnly , 0 );    
#endif

    return status;
}


OI_STATUS OI_OBEXAUTH_SaveResponse(OBEX_COMMON *common,
                                   OI_OBEX_HEADER *response)
{
    OBEX_AUTHENTICATION *auth = common->authentication;
    OI_STATUS status = OI_OK;
    OI_UINT i = 0;

    OI_DBGTRACE(("OI_OBEXAUTH_SaveResponse\n"));

    OI_ASSERT(auth);
    OI_ASSERT(response->id == OI_OBEX_HDR_AUTHENTICATION_RESPONSE);

#ifndef OI_CODE
    BT_MSG_DEBUG( "BT OBEX: Save Response", 0, 0, 0 );
#endif

    OI_MemZero(auth->userId, OI_OBEX_MAX_USERID_LEN);
    auth->userIdLen = 0;

    while (i < response->val.byteseq.len) {
        OI_UINT8 tag = response->val.byteseq.data[i++];
        OI_UINT8 len = response->val.byteseq.data[i++];
#ifndef OI_CODE
        BT_MSG_DEBUG( "BT OBEX: Save Response : Tag=%d : Len=%d", tag, len, 0 );
#endif

        switch (tag) {
            case OI_OBEX_RESPONSE_DIGEST_REQUEST_TAG:
                if (len != OI_OBEX_AUTH_DIGEST_LEN) {
                    status = OI_OBEX_INVALID_AUTH_DIGEST;
                } else {
                    OI_MemCopy(auth->responseDigest, &response->val.byteseq.data[i], OI_OBEX_AUTH_DIGEST_LEN);
                }
                break;
            case OI_OBEX_RESPONSE_DIGEST_USERID_TAG:
                if (len > 0) {
                    if (len > OI_OBEX_MAX_USERID_LEN) {
                        status = OI_OBEX_INVALID_AUTH_DIGEST;
                    } else {
                        OI_MemCopy(auth->userId, &response->val.byteseq.data[i], len);
                        OI_DBGPRINT2(("Obex authentication response user id = %s", auth->userId));
                        auth->userIdLen = len;
                    }
                }
                break;
        }
        i += len;
    }
    return status;
}



/**
 * Compose the response to an OBEX authentication challenge.
 */

void OI_OBEXAUTH_ComposeResponse(OBEX_COMMON *common,
                                 OI_OBEX_HEADER *response)
{
    OI_UINT16 len = 0;
    MD5_CONTEXT md5Context;
    OBEX_AUTHENTICATION *auth = common->authentication;

    OI_DBGTRACE(("OI_OBEXAUTH_ComposeResponse\n"));

    OI_ASSERT(auth);
#ifndef OI_CODE                        
    BT_MSG_DEBUG("BT OBEX: Compose Response", 0, 0, 0 );
#endif

    /*
     * Compose a digest response to the authentication challenge by appending
     * the password to the nonce received in the challenge and computing the MD5
     * hash of the resultant string.
     */
    OI_MD5_Init(&md5Context);
    /*
     * Hash in the challenge digest (nonce), a separator (colon per OBEX spec),
     * and the password
     */
    OI_MD5_Update(&md5Context, common->authentication->challengeDigest, OI_OBEX_AUTH_DIGEST_LEN);
    OI_MD5_Update(&md5Context, (OI_BYTE*)":", 1);
    OI_MD5_Update(&md5Context, common->authentication->password, common->authentication->passwordLen);
    /*
     * Generate the MD5 result and compose the response digest.
     */
    auth->responseHeader[len++] = OI_OBEX_RESPONSE_DIGEST_REQUEST_TAG;
    auth->responseHeader[len++] = OI_OBEX_AUTH_DIGEST_LEN;
    OI_MD5_Final(&auth->responseHeader[len], &md5Context);
    len += OI_OBEX_AUTH_DIGEST_LEN;
    /*
     * Append the user id if we have one.
     */
    if (auth->userIdLen > 0) {
        auth->responseHeader[len++] = OI_OBEX_RESPONSE_DIGEST_USERID_TAG;
        auth->responseHeader[len++] = auth->userIdLen;
        OI_MemCopy(&auth->responseHeader[len], auth->userId, auth->userIdLen);
        len += auth->userIdLen;
    }
    /*
     * Initialize the authentication response header.
     */
    response->id = OI_OBEX_HDR_AUTHENTICATION_RESPONSE;
    response->val.authenticationResponse.data = auth->responseHeader;
    response->val.authenticationResponse.len = len;
}



/**
 * Generate an OBEX authentication challenge.
 */

void OI_OBEXAUTH_ComposeChallenge(const OI_CHAR* privateKey,
                                  OBEX_COMMON *common,
                                  OI_OBEX_HEADER *challenge)
{
    OBEX_AUTHENTICATION *auth = common->authentication;
    OI_UINT8 options = 0;
    OI_UINT16 len = 0;
    OI_UINT16 keyLen = OI_StrLen(privateKey);
    MD5_CONTEXT md5Context;
    OI_TIME timestamp;

    OI_DBGTRACE(("OI_OBEXAUTH_ComposeChallenge(privateKey=%s)\n", privateKey));

#ifndef OI_CODE
    BT_MSG_DEBUG( "BT OBEX: Compose Challenge : ReadOnly=%d : UIDReqd=%d : RealmLen=%d", 
                                common->readOnly, auth->userIdRequired, auth->realmLen );
#endif

    OI_ASSERT(auth);
    OI_ASSERT(privateKey != NULL);

    /*
     * Compose a digest challenge by appending a private key t(provided as a
     * configuration parameter)o a time stamp and computing the MD5 hash of the
     * resultant string.
     */
    OI_MD5_Init(&md5Context);
    OI_Time_Now(&timestamp);
    /*
     * Hash in the timestamp and the private key. Private key is a NULL
     * terminated string.
     */
    OI_MD5_Update(&md5Context, (OI_BYTE*) &timestamp, sizeof(timestamp));
    OI_MD5_Update(&md5Context, (OI_BYTE*) privateKey, keyLen);
    /*
     * Generate the MD5 result and compose the challenge digest.
     */
    auth->challengeHeader[len++] = OI_OBEX_CHALLENGE_DIGEST_NONCE_TAG;
    auth->challengeHeader[len++] = OI_OBEX_AUTH_DIGEST_LEN;
    OI_MD5_Final(&auth->challengeHeader[len], &md5Context);
    len += OI_OBEX_AUTH_DIGEST_LEN;
    /*
     * Send options if needed.
     */
    if (common->readOnly) {
        options |= OI_OBEX_AUTH_READONLY_OPTION;
    }
    if (auth->userIdRequired) {
        options |= OI_OBEX_AUTH_USERID_OPTION;
    }
    if (options) {
        OI_DBGPRINT2(("OBEX compose challenge options = %1x", options));
        auth->challengeHeader[len++] = OI_OBEX_CHALLENGE_DIGEST_OPTIONS_TAG;
        auth->challengeHeader[len++] = 1;
        auth->challengeHeader[len++] = options;
    }
#ifndef OI_CODE
    /* Add realm info */    
    if(auth->realmLen){
        BT_MSG_HIGH("OI_OBEXAUTH_ComposeChallenge RealmLen=%d:RealmType=%d",
                                     auth->realmLen, auth->realmStrType,0);

        auth->challengeHeader[len++] = OI_OBEX_CHALLENGE_DIGEST_REALM_TAG;
        auth->challengeHeader[len++] = auth->realmLen;
        auth->challengeHeader[len++] = auth->realmStrType;
        OI_MemCopy( &auth->challengeHeader[len],&auth->realmStr[0],auth->realmLen);
        len += auth->realmLen;
    }
#endif
    /*
     * Initialize the authentication challenge header. 
     */
    challenge->id = OI_OBEX_HDR_AUTHENTICATION_CHALLENGE;
    challenge->val.authenticationChallenge.data = auth->challengeHeader;
    challenge->val.authenticationChallenge.len = len;
}


OI_STATUS OI_OBEXAUTH_Authenticate(OBEX_COMMON *common)
{
    OBEX_AUTHENTICATION *auth = common->authentication;
    MD5_CONTEXT md5Context;
    OI_BYTE digest[OI_OBEX_AUTH_DIGEST_LEN];

    OI_DBGTRACE(("OI_OBEXAUTH_Authenticate\n"));

    OI_ASSERT(auth);

    OI_DBGPRINT(("Authentication user id: %?s password: %?s\n", auth->userId,
                 auth->userIdLen, auth->password, auth->passwordLen));
    
#ifndef OI_CODE
    BT_MSG_DEBUG("BT OBEX: Authenticate", 0, 0, 0 );
#endif

    /*
     * Compose the expected response to the challenge.
     */
    OI_MD5_Init(&md5Context);
    /*
     * Hash in the digest from the challenge we sent earlier followed by the
     * separator (colon) and the passowrd.
     */
    OI_MD5_Update(&md5Context, &auth->challengeHeader[2], OI_OBEX_AUTH_DIGEST_LEN);
    OI_MD5_Update(&md5Context, (OI_BYTE*)":", 1);
    OI_MD5_Update(&md5Context, auth->password, auth->passwordLen);
    /*
     * Generate the MD5 result.
     */
    OI_MD5_Final(digest, &md5Context);
    /*
     * Compare the computed digest with the response digest
     */
    if (OI_MemCmp(digest, auth->responseDigest, OI_OBEX_AUTH_DIGEST_LEN) == 0) {
#ifndef OI_CODE
        BT_MSG_DEBUG("BT OBEX: Authentication succeeded", 0, 0, 0 );
#endif
        OI_DBGPRINT(("Authentication succeeded\n"));
        return OI_OK;
    } else {
#ifndef OI_CODE
        BT_MSG_HIGH("BT OBEX: Authentication Failed", 0, 0, 0 );
#endif
        OI_DBGPRINT(("Authentication failed\n"));
        return OI_OBEX_UNAUTHORIZED;
    }
}

