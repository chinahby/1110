#ifndef AEEWIFIEXT_H
#define AEEWIFIEXT_H

/*===========================================================================
FILE:  AEEWIFIExt.h

SERVICES:  None

GENERAL DESCRIPTION:
        AEE WIFI Authentication method extensions

        Copyright © 2005 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR

===========================================================================

                    DEFINITIONS AND CONSTANTS

===========================================================================*/

#define AEEWIFI_AUTHENTICATION_8021X_TLS_RADIUS_WKEY 5
#define AEEWIFI_AUTHENTICATION_8021X_TLS_CERT        6
#define AEEWIFI_AUTHENTICATION_PAP                   7
#define AEEWIFI_AUTHENTICATION_CHAP                  8
#define AEEWIFI_AUTHENTICATION_MSCHAPV2              9
#define AEEWIFI_AUTHENTICATION_PEAP                  10
#define AEEWIFI_AUTHENTICATION_TTLS                  11
#define AEEWIFI_AUTHENTICATION_SIM                   12
#define AEEWIFI_AUTHENTICATION_AKA                   13

/* Valid combinations of authentication related options for different authentication types:
 *
 * AEEWIFI_AUTHENTICATION_WEP with AEEWIFI_ENCRYPTION_WEP_64bit
 *    AEEWIFI_OPT_WEP_64BIT_KEY_ID
 *    AEEWIFI_OPT_WEP_64BIT_KEY_0
 *    AEEWIFI_OPT_WEP_64BIT_KEY_1
 *    AEEWIFI_OPT_WEP_64BIT_KEY_2
 *    AEEWIFI_OPT_WEP_64BIT_KEY_3
 *
 * AEEWIFI_AUTHENTICATION_WEP with AEEWIFI_ENCRYPTION_WEP_128bit
 *    AEEWIFI_OPT_WEP_128BIT_KEY_ID
 *    AEEWIFI_OPT_WEP_128BIT_KEY_0
 *    AEEWIFI_OPT_WEP_128BIT_KEY_1
 *    AEEWIFI_OPT_WEP_128BIT_KEY_2
 *    AEEWIFI_OPT_WEP_128BIT_KEY_3
 *
 * AEEWIFI_AUTHENTICATION_8021X_PASSWORD:
 *    AEEWIFI_OPT_8021X_PASSWORD
 *
 * AEEWIFI_AUTHENTICATION_8021X_TLS:
 * AEEWIFI_AUTHENTICATION_8021X_TLS_RADIUS_WKEY:
 *    AEEWIFI_OPT_8021X_ID 
 *
 * AEEWIFI_AUTHENTICATION_8021X_MD5:
 *    AEEWIFI_OPT_8021X_ID
 *    AEEWIFI_OPT_8021X_PASSWORD
 *
 * AEEWIFI_AUTHENTICATION_8021X_TLS_CERT:
 *    AEEWIFI_OPT_8021X_ID
 *    AEEWIFI_OPT_CERTIFICATE_ID
 *
 * AEEWIFI_AUTHENTICATION_PAP:
 * AEEWIFI_AUTHENTICATION_CHAP:
 * AEEWIFI_AUTHENTICATION_MSCHAPV2:
 *    AEEWIFI_OPT_USER_ID
 *    AEEWIFI_OPT_8021X_ID
 *    AEEWIFI_OPT_8021X_PASSWORD
 *
 * AEEWIFI_AUTHENTICATION_PEAP:
 * AEEWIFI_AUTHENTICATION_TTLS:
 *    AEEWIFI_OPT_USER_ID
 *    AEEWIFI_OPT_8021X_ID
 *    AEEWIFI_OPT_CERTIFICATE_ID
 *    AEEWIFI_OPT_NESTED_AUTH_ID
 *
 * AEEWIFI_AUTHENTICATION_SIM:
 * AEEWIFI_AUTHENTICATION_AKA:
 * AEEWIFI_AUTHENTICATION_NONE:
 *    none
 */

#define AEEWIFI_OPT_NESTED_AUTH_ID  (WIFI_OPT_32BIT + 22)
 /* Integer Value: Identifies the start of parameters for an authentication scheme
  *                within an authentication scheme
  *
  * Note: When building a profile options for nested authentication scheme,
  * first add the nested authentication options, then add AEEWIFI_OPT_NESTED_AUTH_ID option,
  * and finally add the rest of options for the profile before calling IWIFI_CommitOptions()
  */

#define AEEWIFI_OPT_CERTIFICATE_ID    (WIFI_OPT_32BIT + 23)
 /* Integer Value: Specifies the certificate number to use with the authentication scheme */

#define AEEWIFI_OPT_USER_ID           (WIFI_OPT_SZ + 18)
  /* AECHAR Null Terminated String: Used for advanced authentication types */

#endif /* AEEWIFIEXT_H */
