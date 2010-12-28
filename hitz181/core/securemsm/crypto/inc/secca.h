#ifndef SECCA_H
#define SECCA_H

#ifdef FEATURE_SEC_EFS_CA
/*==========================================================================

               A S W   S E C U R I T Y   S E R V I C E S

        C E R T I F I C A T E    A U T H O R I T I E S    H E A D E R

GENERAL DESCRIPTION

  This module contains the functions to interface with Embedded File 
  System (EFS) to handle the Certificate Authority (CA). The mobile 
  needs to be provisioned with a number of trusted CAs in order to 
  verify the server's certificate. The CAs are stored in EFS with
  compliance with the Distinguished Encoding Rules (DER) format. 

  During the intialization, CAs are read out, parsed and verified, 
  one by one. The file names are checked to conform with certain
  naming conventions. The file names are stored in a file name array
  so that it can speed up the searching for an issuing CA. 

EXTERNALIZED FUNCTIONS

  secca_init()

  secca_read_certificate()

  secca_write_certificate()

  secca_update_certificate()

  secca_delete_certificate()

  secca_delete_all_certificate()

  secca_rename_certificate()

INITIALIZATION AND SEQUENCING REQUIREMENTS

  secossl_init() must be called before using the other functions.

Copyright (c) 2001-2004 by QUALCOMM, Incorporated.  All Rights Reserved.
==========================================================================*/

/*==========================================================================

                      EDIT HISTORY FOR FILE


when       who     what, where, why
--------   ---     ---------------------------------------------------------
11/12/04   rv      changes to correct LINT errors
07/29/03   rwh     Added state indicator for CA initialization.
02/18/03   jay     Changed a feature name
09/30/02   om      Added symlink param to secca_update_certificate().
05/18/01   yll     Several fixes.
04/24/01   yll     Initial file.

==========================================================================*/
#include "comdef.h"
#include "fs.h"


/*--------------------------------------------------------------------------
                Modular Constants and definitions
--------------------------------------------------------------------------*/

/* The CA certificates are stored in EFS under cert/ directory */
#define SECCA_DIR_NAME            "cert"
#define SECCA_DIR_NAME_LEN        4
#define SECCA_DIR_CHAR_NAME_LEN   5

#define SECCA_EXT_NAME            ".der"
#define SECCA_EXT_NAME_LEN        4

#define SECCA_MAX_NUM_CA          10

#define SECCA_FILENAME_MAX_LEN    ( FS_FILENAME_MAX_LENGTH - \
                                    SECCA_DIR_CHAR_NAME_LEN - \
                                    SECCA_EXT_NAME_LEN )

#define SECCA_SYMLINK_PREFIX      "__"
#define SECCA_SYMLINK_PREFIX_LEN  2
#define SECCA_SYMLINK_MAX_LEN     ( FS_FILENAME_MAX_LENGTH - \
                                    SECCA_DIR_CHAR_NAME_LEN - \
                                    SECCA_SYMLINK_PREFIX_LEN )
#define SECCA_SYMLINK_MAX_TARGET_LEN  (SECCA_FILENAME_MAX_LEN + 1)

/* An array of CA certificate names */
extern char secca_file_name[SECCA_MAX_NUM_CA][FS_FILENAME_MAX_LENGTH];
extern uint16   secca_fs_num_ca;

/* State indicator of CA initialization. */
extern boolean secca_init_invoked;


/*==========================================================================

                      PUBLIC FUNCTION DECLARATIONS

==========================================================================*/

/*==========================================================================

FUNCTION SECCA_INIT

DESCRIPTION
  The function initialize the Authority Certificate. It reads out the
  CAs from the EFS under the specified cert directory, parse it, and 
  verify it. It then checks on the CA name to conform to a certain
  name convention, to speed up the searching for an issuing CA.

RETURN VALUE
  E_SUCCESS, if processing is successful.
  E_EFS_ERROR, if EFS returns an error.
  E_NO_DATA, if there is no CA provisioned in the EFS.
  E_FAILURE, other error reasons.
  
DEPENDENCIES
  None
==========================================================================*/
extern secerrno_enum_type secca_init( void );

/*==========================================================================

FUNCTION SECCA_READ_CERTIFICATE

DESCRIPTION
  This functions reads out a CA from the EFS. The passed in name should be
  the fullname (contain the directory). The CA is stored in the DER format.
  Upon return, if successful, a valid CA in the DER format is returned, as 
  well as the length of the CA. Otherwise, an error code will be returned.

RETURN VALUE
  E_SUCCESS,    if successful.
  E_EFS_ERROR,  if EFS returns error.
  E_FAILURE,    if failed.

DEPENDENCIES

==========================================================================*/
extern secerrno_enum_type secca_read_certificate
(
  char          *name,
  uint8         *cert_data,
  uint16        *cert_len
);

/*==========================================================================

FUNCTION SECCA_WRITE_CERTIFICATE

DESCRIPTION
  This function writes a CA to the EFS. If a valid name is passed in, this
  function will write the CA to EFS under the passed in name. Note, in 
  this case, it does not check if the name is valid. The caller should 
  make sure that name contains the full directory name, and is unique.
  If the name is NULL, this function will determine a valid name for the
  CA conforming to a certain name convention, and write the CA to EFS
  under this name.

RETURN VALUE
  E_SUCCESS,    if successful.
  E_EFS_ERROR,  if EFS operation returns error.
  E_FAILURE,    any other failured reasons.

DEPENDENCIES

==========================================================================*/
extern secerrno_enum_type secca_write_certificate
(
  char          *name,
  uint8         *cert_data,
  uint16        *cert_len
);

/*==========================================================================

FUNCTION SECCA_UPDATE_CERTIFICATE

DESCRIPTION
  This function updates a CA in the EFS. It first searches for a matching
  name, and if found, then updates the CA. In the case when the CA changes
  its name, this function will rename the CA according to a certain naming
  conventions.

RETURN VALUE

  E_SUCCESS,    if successful.
  E_EFS_ERROR,  if EFS operation failed.
  E_FAILURE,    All other failure reasons.

DEPENDENCIES

==========================================================================*/
extern secerrno_enum_type secca_update_certificate
(
  char          *name,
  char          *symlink,
  uint8         *cert_data,
  uint16        *cert_len
);

/*==========================================================================

FUNCTION SECCA_DELETE_CERTIFICATE

DESCRIPTION
  This function deletes a CA from EFS by the name of the certificate.

RETURN VALUE
  E_SUCCESS,    if successful
  E_EFS_ERROR,  if EFS operation failed.
  E_FAILURE,    any other failure reasons.

DEPENDENCIES

==========================================================================*/
extern secerrno_enum_type secca_delete_certificate
(
  char          *name
);

/*==========================================================================

FUNCTION SECCA_DELETE_ALL_CERTIFICATES

DESCRIPTION
  This function deletes all the certificates from EFS.

RETURN VALUE
  E_SUCCESS,    if successful
  E_EFS_ERROR,  if EFS operation fails.
  E_FAILURE,    if any other failure reasons.

DEPENDENCIES

==========================================================================*/
extern secerrno_enum_type secca_delete_all_certificates ( void );

/*==========================================================================

FUNCTION SECCA_RENAME_CERTIFICATE

DESCRIPTION
  This function renames a certificate from old_name to new_name.

  It does check whether the new_name conforms to the naming conventions.
  Caller should make sure that CA name is valid.

RETURN VALUE
  E_SUCCESS,    if successful,
  E_EFS_ERROR,  if EFS operation returns error.
  E_FAILURE,    any other failure reasons.

DEPENDENCIES

==========================================================================*/
extern secerrno_enum_type secca_rename_certificate
(
  char          *old_name,
  char          *new_name
);

#else /* FEATURE_SEC_EFS_CA */

#define SECCA_MAX_NUM_CA         0
#define SECCA_DIR_CHAR_NAME_LEN  20
#define SECCA_FILENAME_MAX_LEN   20
extern char    secca_file_name[SECCA_MAX_NUM_CA+1][SECCA_FILENAME_MAX_LEN];
extern uint16  secca_fs_num_ca;

#endif /* FEATURE_SEC_EFS_CA */

#endif /* SECCA_H */
