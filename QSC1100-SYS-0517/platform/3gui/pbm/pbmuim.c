/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                          PHONEBOOK MANAGER UIM ENTRIES

GENERAL DESCRIPTION
  This file contains the functions used to manage phonebook entries within the
  EFS.

  Copyright (c) 2002 - 2008 by QUALCOMM INCORPORATED.
  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //depot/asic/msmshared/services/pbm/pbmuim.c#74 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/04/09   krishna changes for CR 167785
12/26/08   kcp     adding changes for ASUS CR 165600
09/09/08   sg      Don't read IAP file(s) if there are no TYPE 2 files
09/03/08   sg      Don't SEEK/SEARCH on transparent files
07/07/08   sg      Use mmgsdi_seek() to skip over unused records to improve
                   SIM initialization time
06/12/08   sg      Fix deallocation of cache records in pbm_fileinfo_complete()
03/14/08   clm     Add PBM_EVENT_SIM_INIT_DONE event.
03/13/08   clm     Add restart of SIM read init for enable/disable FDN
01/17/08   clm     Add FEATURE_PBM_USIM_SUPPORT to featurize USIM features.
11/02/07   clm     Add refresh okay and init complete support to pbm for mmgsdi.
08/31/07   cvs     Fix issue with fcn during init
05/18/07   cvs     Fix abort when receiving SIM_INIT_COMPLETED while initializing.
03/12/07   cvs     Featurize 4 PB set support
02/23/07   cvs     Clear stale extension record indicator before writing USIM fields
02/22/07   cvs     Keep track of filetype for read + write ops
02/09/07   cvs     Support for 4 phonebook sets
01/16/07   cvs     Add event for phonebooks becoming ready
11/06/06   cvs     Fixes for cards with PB sets with different text sizes.
10/24/06   cvs     Register for refresh on all files, to fix FDN refresh issue
11/01/06   cvs     multi pb set fixes
10/10/06   cvs     Move messages to PBM's diag msg ID
10/03/06   cvs     Add support for MBN
08/15/06   cvs     fix crash w/enable+disable FDN with type 2 USIM fields
08/04/06   cvs     fix extension record overwrite/delete issues
07/27/06   slw     Fix menu items displayed when adding FDNs
07/25/06   slw     Added error reporting for pbm_create_sim_record call in pbm_sim_write
07/24/06   cvs     Fixes for unequal size multi pb set support
06/14/06   aps     support for ANRC and ANRC1 type records
06/13/06   aps     Fix for mem leak when FDN is enabled and disabled.
05/31/06   cvs     Fix support of multiple ANR files
05/03/06   cvs     convert to MMGSDI APIs.  Lint cleanup.
03/24/06   cvs     Fix GSDI error handling code in fileinfo_cb.
03/23/06   cvs     Fix MBDN UST entry.  Lint fixes.
03/22/06   cvs     better logging
03/13/06   cvs     add error handling in pbm_fileinfo_cb() if GSDI returns error
02/28/06   cvs     Add support for MBDN
02/23/06   cvs     rework memory allocation macros
02/22/06   cvs     Better error reporting on SIM write failures
02/09/06   cvs     When can't encode all characters on SIM, return errors
01/09/06   cvs     Lint Fixes
12/20/05   cvs     Fix reboot (infinite loop) when adding to a full USIM file
12/14/05   cvs     Fix data abort when IAP is programmed incorrectly.
12/13/05   cvs     Implement partial PB refresh
11/16/05   cvs     Lint Fixes
09/22/05   cvs     Free caches_in_use earlier.
08/22/05   AT      Moved contents inside of a FEATURE_PBM_TASK.
08/12/05   AT      Added UCS2 support for strings.
08/12/05   cvs     When extension records go unused, write out blank record
08/04/05   cvs     Recover from committed list for SIM errors
07/27/05   cvs     Cleanup more coverity errors
07/19/05   AT      Fix issue where PIN becomes blocked and unblocked
                   after SIM_INIT_COMPLETED.  Also, add range checking
                   on sim_id and pb_set before using them as an array index.
07/13/05   cvs     Init unused group file entries to 0 (no group), not ff
07/11/05   cvs     Better error messages
06/14/05   cvs     Fix for not being able to add ADN if ADN file is enumerated last.
05/26/05   cvs     support for API changes to make APIs more multiprocessor friendly
05/05/05   cvs     Clear out caches_in_use after deleting old record
04/29/05   cvs     caches_in_use is one based, so allocate at num_caches+1
04/12/05   AT      Only update a USIM sync value for ADN files.
03/22/05   PA      Do not update unique id for deletion/update.
                   Changed PBM_MAX_PBR_RECS to PBM_MAX_NUM_PB_SETS.
                   Fixed issue in IAP update for Type 2 ANR.
03/08/05   PA      Support for PBM notifications. Double check #records in ADN
                   cache before initializing USIM files.
02/23/05   PA      Added back pbmuim.c#8 changes and usage of special
                   service enums.
02/23/05   PA      Reverted to pbmuim.c#7 and using special GSDI service enums
                   for USIM.
02/08/05   PA      Support for MSISDN, check if IAP file is present.
01/18/05   PA      Conversion between 7-bit and 8-bit encoding
01/14/05   PA      Make PBM GSTK unaware
01/12/05   PA      Remove stub code for GSDI
01/12/05   PA      PBM USIM Support
11/19/04   PA      Support non-GSDI builds, code review comments.
11/02/04   PA      Support for SDN
09/29/04   PA      Initial Revision
===========================================================================*/
#include "customer.h"
#ifdef FEATURE_PBM_TASK
#include "pbm.h"

#ifdef FEATURE_MMGSDI
#include "pbmuim.h"
#include "pbmgsdi.h"
#include "pbmcache.h"
#include "pbmutils.h"
#include "pbm_conversion.h"

#include "mmgsdiutil.h"  //not intended to be included by clients, but
                         //needed for UIM filename <-> MMGSDI filename translation

/* <EJECT> */
/*===========================================================================
                              STATIC DATA
===========================================================================*/
/* The following variables are used when we are initializing
 * the files from the SIM after the PIN is verified.  We will
 * attempt to read all entries from each file in the sequence.
 * The initialize_index points to the index we are currently
 * working with.
 */
int pbm_initialize_index[MAX_UIMS];

pbm_file_device_e_type pbm_initialize_list[PBM_SIZE_INITIALIZE_LIST] =
{
  PBM_FILE_FDN,        /* Fixed Dialing Number list */
  PBM_FILE_LND,        /* SIM/RUIM Last Number Dialed list */
  PBM_FILE_SDN,        /* Service Dialling Numbers */
  PBM_FILE_MSISDN,     /* MSISDN */
  PBM_FILE_MBN,        /* CPHS MBN */

  /* 5 */
#ifdef FEATURE_PBM_USIM_SUPPORT
  PBM_FILE_MBDN,       /* MBDN */
  PBM_FILE_OCI,        /* USIM Outgoing Call Information file.  */
  PBM_FILE_ICI,        /* USIM Incoming Call Information file.  */
  PBM_FILE_PSC,        /* USIM Synchronization counter */
  PBM_FILE_PUID,       /* USIM Previous unique id */

  /* 10 */
  PBM_FILE_CC,         /* USIM Change counter */
  PBM_FILE_PBC,        /* USIM phonebook control */
  PBM_FILE_UID,        /* USIM Unique id */

  PBM_FILE_EMAIL,      /* USIM email in phonebook one */
  PBM_FILE_SNE,        /* USIM second number in phonebook one */

  /* 15 */
  PBM_FILE_GRP,        /* USIM group in phonebook one */
  PBM_FILE_ANR,        /* USIM additional number in phonebook one */
  PBM_FILE_ANRA,       /* USIM additional number in phonebook one */
  PBM_FILE_ANRB,       /* USIM additional number in phonebook one */
  PBM_FILE_ANRC,       /* USIM additional number in phonebook one */

  /* 20 */
  PBM_FILE_IAP,        /* USIM Index file */

  PBM_FILE_EMAIL1,     /* USIM email in phonebook two */
  PBM_FILE_SNE1,       /* USIM second number in phonebook two */
  PBM_FILE_GRP1,       /* USIM group in phonebook two */
  PBM_FILE_ANR1,       /* USIM additional number in phonebook two */

  /* 25 */
  PBM_FILE_ANRA1,      /* uSIM additional number in phonebook two */
  PBM_FILE_ANRB1,      /* uSIM additional number in phonebook two */
  PBM_FILE_ANRC1,      /* uSIM additional number in phonebook two */
  PBM_FILE_IAP1,       /* USIM Index file in phonebook two */

#ifdef FEATURE_USIM_1000_PBM
  PBM_FILE_EMAIL2,     /* USIM email in phonebook three */

  /* 30 */
  PBM_FILE_SNE2,       /* USIM second number in phonebook three */
  PBM_FILE_GRP2,       /* USIM group in phonebook three */
  PBM_FILE_ANR2,       /* USIM additional number in phonebook three */
  PBM_FILE_ANRA2,      /* uSIM additional number in phonebook three */
  PBM_FILE_ANRB2,      /* uSIM additional number in phonebook three */

  /* 35 */
  PBM_FILE_ANRC2,      /* uSIM additional number in phonebook three */
  PBM_FILE_IAP2,       /* USIM Index file in phonebook three */

  PBM_FILE_EMAIL3,     /* USIM email in phonebook four */
  PBM_FILE_SNE3,       /* USIM second number in phonebook four */
  PBM_FILE_GRP3,       /* USIM group in phonebook four */

  /* 40 */
  PBM_FILE_ANR3,       /* USIM additional number in phonebook four */
  PBM_FILE_ANRA3,      /* uSIM additional number in phonebook four */
  PBM_FILE_ANRB3,      /* uSIM additional number in phonebook four */
  PBM_FILE_ANRC3,      /* uSIM additional number in phonebook four */
  PBM_FILE_IAP3,       /* USIM Index file in phonebook four */
#endif
#endif /* FEATURE_PBM_USIM_SUPPORT */
  /* 45 */
#ifdef FEATURE_ECC_HANDLING_AT_PBM
  PBM_FILE_ECC,
#endif
  PBM_FILE_ADN,        /* Abbreviated Dialing Number list (SIM/USIM Phonebook) */
};

/* SIM phonebook files (that are accessible to the user directly) */
pbm_file_device_e_type pbm_sim_pb_files[PBM_NUM_SIM_PB] = {
                                           PBM_FILE_ADN, PBM_FILE_SDN,
                                           PBM_FILE_FDN, PBM_FILE_LND,
#ifdef FEATURE_PBM_USIM_SUPPORT
                                           PBM_FILE_OCI, PBM_FILE_ICI,
#endif /* FEATURE_PBM_USIM_SUPPORT */
                                           PBM_FILE_MSISDN,
#ifdef FEATURE_PBM_USIM_SUPPORT
                                           PBM_FILE_MBDN,
#endif /* FEATURE_PBM_USIM_SUPPORT */
                                           PBM_FILE_MBN};

/* Has a one-to-one mapping with pbm_file_device_e_type */
pbm_uim_file_entry pbm_file_to_uim_file[PBM_FILE_MAX_REQUESTS] = {
  {PBM_FILE_ADN,    {MMGSDI_TELECOM_ADN,    MMGSDI_USIM_ADN},
                    {PBM_FILE_ADN,          PBM_FILE_ADN}},
  {PBM_FILE_FDN,    {MMGSDI_TELECOM_FDN,    MMGSDI_USIM_FDN},
                    {PBM_FILE_FDN,          PBM_FILE_FDN}},
  {PBM_FILE_SDN,    {MMGSDI_TELECOM_SDN,    MMGSDI_USIM_SDN},
                    {PBM_FILE_SDN,          PBM_FILE_SDN}},
  {PBM_FILE_LND,    {MMGSDI_TELECOM_LND,    MMGSDI_NO_FILE_ENUM},
                    {PBM_FILE_ADN,          PBM_FILE_NONE}},  /* USES EXT1 */
#ifdef FEATURE_PBM_USIM_SUPPORT
  {PBM_FILE_MSISDN, {MMGSDI_TELECOM_MSISDN, MMGSDI_USIM_MSISDN},
                    {PBM_FILE_ADN,          PBM_FILE_OCI}}, /* USIM EXT5, SIM EXT1 */
#else
  {PBM_FILE_MSISDN, {MMGSDI_TELECOM_MSISDN, MMGSDI_USIM_MSISDN},
                    {PBM_FILE_ADN,          PBM_FILE_NONE}}, /* USIM EXT5, SIM EXT1 */
#endif /* FEATURE_PBM_USIM_SUPPORT */
  {PBM_FILE_MBN,    {MMGSDI_GSM_MN,         MMGSDI_USIM_MN},
                    {PBM_FILE_ADN,          PBM_FILE_ADN}},
  {PBM_FILE_EXT1,   {MMGSDI_TELECOM_EXT1,   MMGSDI_USIM_EXT1},
                    {PBM_FILE_ADN,          PBM_FILE_ADN}},
  {PBM_FILE_EXT2,   {MMGSDI_TELECOM_EXT2,   MMGSDI_USIM_EXT2},
                    {PBM_FILE_FDN,          PBM_FILE_FDN}},
  {PBM_FILE_EXT3,   {MMGSDI_TELECOM_EXT3,   MMGSDI_USIM_EXT3},
                    {PBM_FILE_SDN,          PBM_FILE_SDN}},
#ifdef FEATURE_ECC_HANDLING_AT_PBM
  {PBM_FILE_ECC,    {MMGSDI_GSM_ECC,        MMGSDI_USIM_ECC},
                    {PBM_FILE_ECC,          PBM_FILE_ECC}},
#endif
#ifdef FEATURE_PBM_USIM_SUPPORT
  {PBM_FILE_MBDN,   {MMGSDI_GSM_MBDN,       MMGSDI_USIM_MBDN},
                    {PBM_FILE_MBDN,         PBM_FILE_MBDN}},
  {PBM_FILE_OCI,    {MMGSDI_NO_FILE_ENUM,   MMGSDI_USIM_OCI},
                    {PBM_FILE_NONE,         PBM_FILE_OCI}},
  {PBM_FILE_ICI,    {MMGSDI_NO_FILE_ENUM,   MMGSDI_USIM_ICI},
                    {PBM_FILE_NONE,         PBM_FILE_OCI}}, /* uses EXT5 with OCI */
  {PBM_FILE_EXT5,   {MMGSDI_NO_FILE_ENUM,   MMGSDI_USIM_EXT5},
                    {PBM_FILE_NONE,         PBM_FILE_OCI}},
  {PBM_FILE_EXT6,   {MMGSDI_GSM_EXT6,       MMGSDI_USIM_EXT6},
                    {PBM_FILE_MBDN,         PBM_FILE_MBDN}},
  {PBM_FILE_PSC,    {MMGSDI_TELECOM_PSC,    MMGSDI_USIM_PSC},
                    {PBM_FILE_ALL,          PBM_FILE_ALL}},
  {PBM_FILE_PUID,   {MMGSDI_TELECOM_PUID,   MMGSDI_USIM_PUID},
                    {PBM_FILE_ALL,          PBM_FILE_ALL}},
  {PBM_FILE_CC,     {MMGSDI_TELECOM_CC,     MMGSDI_USIM_CC},
                    {PBM_FILE_ALL,          PBM_FILE_ALL}},
  {PBM_FILE_EMAIL,  {MMGSDI_TELECOM_EMAIL,  MMGSDI_USIM_EMAIL},
                    {PBM_FILE_ADN,          PBM_FILE_ADN}},
  {PBM_FILE_SNE,    {MMGSDI_TELECOM_SNE,    MMGSDI_USIM_SNE},
                    {PBM_FILE_ADN,          PBM_FILE_ADN}},
  {PBM_FILE_GRP,    {MMGSDI_TELECOM_GRP,    MMGSDI_USIM_GRP},
                    {PBM_FILE_ADN,          PBM_FILE_ADN}},
  {PBM_FILE_ANR,    {MMGSDI_TELECOM_ANR,    MMGSDI_USIM_ANR},
                    {PBM_FILE_ADN,          PBM_FILE_ADN}},
  {PBM_FILE_ANRA,   {MMGSDI_TELECOM_ANRA,   MMGSDI_USIM_ANRA},
                    {PBM_FILE_ADN,          PBM_FILE_ADN}},
  {PBM_FILE_ANRB,   {MMGSDI_TELECOM_ANRB,   MMGSDI_USIM_ANRB},
                    {PBM_FILE_ADN,          PBM_FILE_ADN}},
  {PBM_FILE_ANRC,   {MMGSDI_TELECOM_ANRC,   MMGSDI_USIM_ANRC},
                    {PBM_FILE_ADN,          PBM_FILE_ADN}},
  {PBM_FILE_IAP,    {MMGSDI_TELECOM_IAP,    MMGSDI_USIM_IAP},
                    {PBM_FILE_ALL,          PBM_FILE_ALL}},
  {PBM_FILE_UID,    {MMGSDI_TELECOM_UID,    MMGSDI_USIM_UID},
                    {PBM_FILE_ALL,          PBM_FILE_ALL}},
  {PBM_FILE_PBC,    {MMGSDI_TELECOM_PBC,    MMGSDI_USIM_PBC},
                    {PBM_FILE_ALL,          PBM_FILE_ALL}},
  {PBM_FILE_ADN1,   {MMGSDI_TELECOM_ADN1,   MMGSDI_USIM_ADN1},
                    {PBM_FILE_ADN,          PBM_FILE_ADN}},
  {PBM_FILE_EMAIL1, {MMGSDI_TELECOM_EMAIL1, MMGSDI_USIM_EMAIL1},
                    {PBM_FILE_ADN,          PBM_FILE_ADN}},
  {PBM_FILE_SNE1,   {MMGSDI_TELECOM_SNE1,   MMGSDI_USIM_SNE1},
                    {PBM_FILE_ADN,          PBM_FILE_ADN}},
  {PBM_FILE_GRP1,   {MMGSDI_TELECOM_GRP1,   MMGSDI_USIM_GRP1},
                    {PBM_FILE_ADN,          PBM_FILE_ADN}},
  {PBM_FILE_ANR1,   {MMGSDI_TELECOM_ANR1,   MMGSDI_USIM_ANR1},
                    {PBM_FILE_ADN,          PBM_FILE_ADN}},
  {PBM_FILE_ANRA1,  {MMGSDI_TELECOM_ANRA1,  MMGSDI_USIM_ANRA1},
                    {PBM_FILE_ADN,          PBM_FILE_ADN}},
  {PBM_FILE_ANRB1,  {MMGSDI_TELECOM_ANRB1,  MMGSDI_USIM_ANRB1},
                    {PBM_FILE_ADN,          PBM_FILE_ADN}},
  {PBM_FILE_ANRC1,  {MMGSDI_TELECOM_ANRC1,  MMGSDI_USIM_ANRC1},
                    {PBM_FILE_ADN,          PBM_FILE_ADN}},
  {PBM_FILE_IAP1,   {MMGSDI_TELECOM_IAP1,   MMGSDI_USIM_IAP1},
                    {PBM_FILE_ALL,          PBM_FILE_ALL}},
  {PBM_FILE_UID1,   {MMGSDI_TELECOM_UID1,   MMGSDI_USIM_UID1},
                    {PBM_FILE_ALL,          PBM_FILE_ALL}},
  {PBM_FILE_PBC1,   {MMGSDI_TELECOM_PBC1,   MMGSDI_USIM_PBC1},
                    {PBM_FILE_ALL,          PBM_FILE_ALL}},
#ifdef FEATURE_USIM_1000_PBM
  {PBM_FILE_ADN2,   {MMGSDI_TELECOM_ADN2,   MMGSDI_USIM_ADN2},
                    {PBM_FILE_ADN,          PBM_FILE_ADN}},
  {PBM_FILE_EMAIL2, {MMGSDI_TELECOM_EMAIL2, MMGSDI_USIM_EMAIL2},
                    {PBM_FILE_ADN,          PBM_FILE_ADN}},
  {PBM_FILE_SNE2,   {MMGSDI_TELECOM_SNE2,   MMGSDI_USIM_SNE2},
                    {PBM_FILE_ADN,          PBM_FILE_ADN}},
  {PBM_FILE_GRP2,   {MMGSDI_TELECOM_GRP2,   MMGSDI_USIM_GRP2},
                    {PBM_FILE_ADN,          PBM_FILE_ADN}},
  {PBM_FILE_ANR2,   {MMGSDI_TELECOM_ANR2,   MMGSDI_USIM_ANR2},
                    {PBM_FILE_ADN,          PBM_FILE_ADN}},
  {PBM_FILE_ANRA2,  {MMGSDI_TELECOM_ANRA2,  MMGSDI_USIM_ANRA2},
                    {PBM_FILE_ADN,          PBM_FILE_ADN}},
  {PBM_FILE_ANRB2,  {MMGSDI_TELECOM_ANRB2,  MMGSDI_USIM_ANRB2},
                    {PBM_FILE_ADN,          PBM_FILE_ADN}},
  {PBM_FILE_ANRC2,  {MMGSDI_TELECOM_ANRC2,  MMGSDI_USIM_ANRC2},
                    {PBM_FILE_ADN,          PBM_FILE_ADN}},
  {PBM_FILE_IAP2,   {MMGSDI_TELECOM_IAP2,   MMGSDI_USIM_IAP2},
                    {PBM_FILE_ALL,          PBM_FILE_ALL}},
  {PBM_FILE_UID2,   {MMGSDI_TELECOM_UID2,   MMGSDI_USIM_UID2},
                    {PBM_FILE_ALL,          PBM_FILE_ALL}},
  {PBM_FILE_PBC2,   {MMGSDI_TELECOM_PBC2,   MMGSDI_USIM_PBC2},
                    {PBM_FILE_ALL,          PBM_FILE_ALL}},
  {PBM_FILE_ADN3,   {MMGSDI_TELECOM_ADN3,   MMGSDI_USIM_ADN3},
                    {PBM_FILE_ADN,          PBM_FILE_ADN}},
  {PBM_FILE_EMAIL3, {MMGSDI_TELECOM_EMAIL3, MMGSDI_USIM_EMAIL3},
                    {PBM_FILE_ADN,          PBM_FILE_ADN}},
  {PBM_FILE_SNE3,   {MMGSDI_TELECOM_SNE3,   MMGSDI_USIM_SNE3},
                    {PBM_FILE_ADN,          PBM_FILE_ADN}},
  {PBM_FILE_GRP3,   {MMGSDI_TELECOM_GRP3,   MMGSDI_USIM_GRP3},
                    {PBM_FILE_ADN,          PBM_FILE_ADN}},
  {PBM_FILE_ANR3,   {MMGSDI_TELECOM_ANR3,   MMGSDI_USIM_ANR3},
                    {PBM_FILE_ADN,          PBM_FILE_ADN}},
  {PBM_FILE_ANRA3,  {MMGSDI_TELECOM_ANRA3,  MMGSDI_USIM_ANRA3},
                    {PBM_FILE_ADN,          PBM_FILE_ADN}},
  {PBM_FILE_ANRB3,  {MMGSDI_TELECOM_ANRB3,  MMGSDI_USIM_ANRB3},
                    {PBM_FILE_ADN,          PBM_FILE_ADN}},
  {PBM_FILE_ANRC3,  {MMGSDI_TELECOM_ANRC3,  MMGSDI_USIM_ANRC3},
                    {PBM_FILE_ADN,          PBM_FILE_ADN}},
  {PBM_FILE_IAP3,   {MMGSDI_TELECOM_IAP3,   MMGSDI_USIM_IAP3},
                    {PBM_FILE_ALL,          PBM_FILE_ALL}},
  {PBM_FILE_UID3,   {MMGSDI_TELECOM_UID3,   MMGSDI_USIM_UID3},
                    {PBM_FILE_ALL,          PBM_FILE_ALL}},
  {PBM_FILE_PBC3,   {MMGSDI_TELECOM_PBC3,   MMGSDI_USIM_PBC3},
                    {PBM_FILE_ALL,          PBM_FILE_ALL}},
#endif
#endif /* FEATURE_PBM_USIM_SUPPORT */
};

static gsdi_ef_ecc_data_T pbm_ecc_list[MAX_UIMS][MAX_APPS];    /* Buffer for GSDI
                                                            to write ECC */
extern uint32 ecc_has_entries;

/*
 * The initialization chain for each SIM is independent of each other and
 * two of them could be going on at the same time.
 *
 * An initialization chain starts from pbm_*_fileinfo_internal() and ends with
 * pbm_sim_init_completed().
 *
 * It includes getting file attribute for each SIM file
 * (pbm_make_fileinfo_request()/pbm_fileinfo_cb()), allocating memory
 * depending on the number of records read from file attribute
 * (pbm_fileinfo_complete()) and for each file (pbm_init_entries()),
 * reading all data in the file (pbm_sim_read()/pbm_sim_read_cb()).
 *
 * pbm_slot_initializing is set to TRUE when the above chain is in progress.
 *
 * pbm_init_fileinfo_internal() is used to initialize all SIM files.
 * pbm_start_fileinfo_internal() is used to start initialization based on
 * the files specified in pbm_fileinfo_requests.
 *
 * When either of them is called, if another initialization is going on,
 * the current initialization is deferred until the next callback where
 * pbm_restart_fileinfo_internal() picks it up.
 *
 * pbm_slot_restart is used to notify the callback to start again.
 *
 * The callbacks also check whether an initialization should be stopped
 * (say SIM removed) by checking pbm_slot_initializing.
 *
 * The initialization starts after PIN is verified in a SIM with PIN
 * enabled. pbm_slot_pin_waiting denotes whether a SIM is waiting for
 * PIN verification. pbm_slot_fdn_enabled denotes whether FDN is enabled
 * in the SIM.
 *
 */

/* Initialization variables */
uint32 pbm_slot_card_inserted = 0;   /* True if a card is inserted in the slot */
uint32 pbm_slot_initializing = 0;    /* Initialization is going on */
uint32 pbm_slot_restart = 0;         /* Somebody wants to restart */

/* SIM status */
uint32 pbm_slot_init_waiting = 0;    /* Waiting for sim_init_completed */
uint32 pbm_slot_pin_waiting = 0;     /* Waiting for pin */
uint32 pbm_slot_fdn_enabled = 0;     /* FDN enabled */

static byte pbm_ecc_card_app[] = {
       GSDI_GSM_SIM_MASK,
       GSDI_USIM_APP_MASK,
       GSDI_RUIM_APP_MASK,
};

pbm_fileinfo_request_t pbm_fileinfo_requests[ (int)PBM_FILE_MAX_REQUESTS *
                                              MAX_UIMS];

static void pbm_make_fileinfo_request(pbm_slot_type sim_id,
                                      uint32 start);
static void pbm_init_fileinfo_requests(pbm_slot_type sim_id,
                                       pbm_file_device_e_type file_id);
static pbm_return_type pbm_seek_unused_records(pbm_slot_type sim_id,
                                               pbm_file_device_e_type file_id);

#ifdef FEATURE_PBM_USIM_SUPPORT
/*===========================================================================
FUNCTION pbm_usim_read_pbr_cb

DESCRIPTION
  Initiliaze linkage type and add the available USIM files to init list.

PARAMETERS
  None.

SIDE EFFECTS
===========================================================================*/
void pbm_usim_read_pbr_cb(const gsdi_cnf_T *sim_data)
{
  pbm_slot_type sim_id;
  pbm_file_device_e_type file_id;
  int i;
  int j;
  gsdi_pbr_table_row_type *gsdi_pbr_table;
  int num_rows;
  int local;
  PB_CACHE *pb_cache;
  PB_USIM_CACHE *usim_cache;
  PB_MASTER_USIM_CACHE *m_usim_cache;
  pbm_pb_set_type pb_set;

  PBM_CHECK_PTR_RET(sim_data, VOID);

  sim_id = (sim_data->message_header.slot == GSDI_SLOT_1 ? PBM_SLOT_1 : PBM_SLOT_2);

  PBM_VALIDATE_SLOT_RET(sim_id, VOID);

  local = USE_USIM_LOCAL(sim_id);

  if ((gsdi_returns_T)sim_data->message_header.gsdi_status == GSDI_SUCCESS)
  {
    int num_type2_files = 0;

    gsdi_pbr_table = sim_data->get_pbr_table_cnf.pbr_tbl_p;
    num_rows = sim_data->get_pbr_table_cnf.num_rows;

    for (i = 0; i < num_rows; i++)
    {
      file_id = PBM_FILE_MAX_REQUESTS;
      for (j = 0; j < (int)PBM_FILE_MAX_REQUESTS; j++)
      {
        mmgsdi_return_enum_type mmgsdi_ret;
        mmgsdi_file_enum_type mmgsdi_file;

        mmgsdi_ret = mmgsdi_util_convert_from_uim_items_enum (
          (uim_items_enum_type)gsdi_pbr_table[i].file_name,
          &mmgsdi_file);

        if (mmgsdi_ret != MMGSDI_SUCCESS)
        {
          PBM_MSG_ERR("convert UIM to MMGSDI enum fialed %d", mmgsdi_ret, 0, 0);
        }

        if (pbm_file_to_uim_file[j].uim_filename[local] == mmgsdi_file)
        {
          file_id = (pbm_file_device_e_type) j;
          break;
        }
      }

      if (file_id != PBM_FILE_MAX_REQUESTS)
      {
        switch(pbm_file_id_to_cache_type(file_id))
        {
          case PBM_CACHE_TYPE_PB:
            pb_cache = (PB_CACHE *)pbm_file_id_to_cache(sim_id, file_id); //lint !e826 area too small
            if (pb_cache)
            {
              pb_set = pbm_file_id_to_pb_set(file_id);
              PBM_CHECK_PBSET_RET(pb_set, VOID);
              m_usim_cache = &adn_m_usim_cache[sim_id][pb_set];
              m_usim_cache->adn_sfi = (uint8)gsdi_pbr_table[i].file_sfid;
            }
            break;

          case PBM_CACHE_TYPE_USIM:
            usim_cache = (PB_USIM_CACHE *)pbm_file_id_to_cache(sim_id, file_id); //lint !e826 area too small
            if (usim_cache)
            {
              usim_cache->mapping_type = gsdi_pbr_table[i].file_mapping;
              usim_cache->order = (uint8) gsdi_pbr_table[i].iap_order;
              if (usim_cache->mapping_type == GSDI_MAPPING_TYPE_2)
              {
                ++num_type2_files;
              }
              pbm_init_fileinfo_requests(sim_id, file_id);
            }
            break;

          case PBM_CACHE_TYPE_INDEX:
          case PBM_CACHE_TYPE_SYNC1:
          case PBM_CACHE_TYPE_SYNC2:
            pbm_init_fileinfo_requests(sim_id, file_id);
            break;

          case PBM_CACHE_TYPE_EXT:
#ifdef FEATURE_ECC_HANDLING_AT_PBM
          case PBM_CACHE_TYPE_ECC:
#endif
            /*nothing to do for extension files here */
            break;

          default:
            PBM_MSG_ERR("Not handled file %d in read pbr", file_id, 0, 0);
            break;
        }
      }
    }

    pbm_init_fileinfo_requests(sim_id, PBM_FILE_PSC);
    pbm_init_fileinfo_requests(sim_id, PBM_FILE_CC);
    pbm_init_fileinfo_requests(sim_id, PBM_FILE_PUID);

    /* If there are no TYPE 2 files then don't read the IAP files */
    if (num_type2_files == 0)
    {
#ifdef FEATURE_PBM_USIM_SUPPORT
      pbm_fileinfo_requests[PBM_SIM_FILENAME_TO_INDEX(sim_id, PBM_FILE_IAP)].do_request = FALSE;
      pbm_fileinfo_requests[PBM_SIM_FILENAME_TO_INDEX(sim_id, PBM_FILE_IAP1)].do_request = FALSE;
#ifdef FEATURE_USIM_1000_PBM
      pbm_fileinfo_requests[PBM_SIM_FILENAME_TO_INDEX(sim_id, PBM_FILE_IAP2)].do_request = FALSE;
      pbm_fileinfo_requests[PBM_SIM_FILENAME_TO_INDEX(sim_id, PBM_FILE_IAP3)].do_request = FALSE;
#endif /* FEATURE_USIM_1000_PBM */
#endif /* FEATURE_PBM_USIM_SUPPORT */
    }
  }

  pbm_make_fileinfo_request(sim_id, 0);

}

/*===========================================================================
FUNCTION pbm_usim_read_pbr

DESCRIPTION
  Read the PBR structure from GSDI.

PARAMETERS
   None.

SIDE EFFECTS
===========================================================================*/
static pbm_return_type pbm_usim_read_pbr(pbm_slot_type sim_id)
{
  gsdi_slot_id_type slot;

  PBM_VALIDATE_SLOT_RET(sim_id, PBM_ERROR);

  slot = (sim_id == PBM_SLOT_1) ? GSDI_SLOT_1 : GSDI_SLOT_2;

  if ((gsdi_returns_T)
       gsdi_get_phonebook_reference_table(slot, 0, pbm_gsdi_read_pbr_cb) !=
                                                              GSDI_SUCCESS)
  {
    PBM_MSG_ERR("Failed to obtain PBR record from GSDI for sim %d",
                sim_id, 0, 0);
    return PBM_ERROR;
  }

  return PBM_SUCCESS;
}
#endif /* FEATURE_PBM_USIM_SUPPORT */

/*===========================================================================
FUNCTION pbm_fileinfo_complete

DESCRIPTION
  After we have all the file attributes we need, this function will
  allocate the appropriate arrays based upon those answers.

PARAMETERS
   None.

SIDE EFFECTS
  Globals describing the world according to this SIM are set.
===========================================================================*/
static void pbm_fileinfo_complete(pbm_slot_type sim_id)
{
  int i;
  uint16 k;
#ifdef FEATURE_PBM_USIM_SUPPORT
  int j;
#ifdef FEATURE_USIM_1000_PBM
  int m_index[PBM_MAX_NUM_PB_SETS] = { 1, 1, 1, 1 };
#else
  int m_index[PBM_MAX_NUM_PB_SETS] = { 1, 1 };
#endif

  pbm_file_device_e_type adn_file_id[PBM_MAX_NUM_PB_SETS] = {
    PBM_FILE_ADN,
    PBM_FILE_ADN1,
#ifdef FEATURE_USIM_1000_PBM
    PBM_FILE_ADN2,
    PBM_FILE_ADN3,
#endif
  };
  boolean bAllocMasterUSIMCache = FALSE;
#endif /* FEATURE_PBM_USIM_SUPPORT */

#define ALLOC_MEM_W_ERRCODE(dest, num, size, code)  \
  PBM_MEM_FREEIF((dest));                           \
  if ((num)) {                                      \
    PBM_MEM_ALLOC((dest), (size));                  \
    if ((dest) == NULL) {                           \
      PBM_MSG_ERR("Could not allocate %d blocks of memory for 0x%x",  (num), &(dest), 0); \
      {code};                                       \
      (num) = 0;                                    \
    }                                               \
  }

#define ALLOC_MEM(dest, num, size)                  \
  ALLOC_MEM_W_ERRCODE((dest), (num), (size), NO_ACTION)

  PBM_VALIDATE_SLOT_RET(sim_id, VOID);

#ifdef FEATURE_PBM_USIM_SUPPORT
  if (!pbm_refresh_init)
  {
    /* we're not refreshing, so this is powerup.  We want to alloc USIM caches */
    bAllocMasterUSIMCache = TRUE;
  }
  else
  {
    for (i=0; i < pbm_refresh_num_file_devices; i++)
    {
      if (pbm_refresh_pbm_file_devices[i] == PBM_FILE_ADN)
      {
        /* we're refreshing ADN.  We want to alloc USIM caches */
        bAllocMasterUSIMCache = TRUE;
        break;
      }
    }
  }

  /* Allocate master USIM cache for ADN. */
  if (bAllocMasterUSIMCache)
  {
    for ( j = 0; j < (int)PBM_MAX_NUM_PB_SETS; j++)
    {
      i = PBM_SIM_FILENAME_TO_INDEX(sim_id, adn_file_id[j]);
      if (j < adn_cache[sim_id].num_files)
      {
        if (pbm_fileinfo_requests[i].do_request)
        {
          PB_MASTER_USIM_CACHE *m_usim_cache = &adn_m_usim_cache[sim_id][j];

          ALLOC_MEM(m_usim_cache->usim_cache_array, m_usim_cache->num_caches,
            (uint32) (m_usim_cache->num_caches+1) * sizeof(PB_USIM_CACHE *));

          ALLOC_MEM_W_ERRCODE(m_usim_cache->index_array, m_usim_cache->num_caches,
              (uint32) (m_usim_cache->num_caches+1) * sizeof(byte *),
                       PBM_MEM_FREEIF(m_usim_cache->usim_cache_array));

          if (m_usim_cache->num_caches)
          {
            memset(m_usim_cache->usim_cache_array, 0,
                 (uint32) (m_usim_cache->num_caches+1) * sizeof(PB_USIM_CACHE *));
            memset(m_usim_cache->index_array, 0,
                 (uint32) (m_usim_cache->num_caches+1) * sizeof(byte *));
          }
        }
      }
      else
      {
        pbm_fileinfo_requests[i].do_request = FALSE;
      }
    }
  }
#endif /* FEATURE_PBM_USIM_SUPPORT */

  /* For every file within this UIM, allocate the corresponding cache
   * entries */
  for (i = (int)sim_id * (int) PBM_FILE_MAX_REQUESTS;
       i < ((int)sim_id + 1) * (int) PBM_FILE_MAX_REQUESTS;
       i++)
  {
    pbm_file_device_e_type file_id = (pbm_file_device_e_type)
                                     (i % (int) PBM_FILE_MAX_REQUESTS);
    void *cache = pbm_file_id_to_cache(sim_id, file_id);

    if (!pbm_fileinfo_requests[i].do_request)
      continue;

    if (!cache)
    {
      PBM_MSG_ERR("Invalid cache for file id %d sim %d", file_id, sim_id, 0);
      pbm_fileinfo_requests[i].do_request = FALSE;
      continue;
    }

    switch (pbm_file_id_to_cache_type(file_id))
    {
      /*---------------------------------------------------------------------*/
      /* EXT_CACHE */
      case PBM_CACHE_TYPE_EXT:
      {
        pbm_ext_cache_s_type *ext_cache = (pbm_ext_cache_s_type *)cache;

        ALLOC_MEM (ext_cache->in_use_array, ext_cache->num_of_rec,
          ((uint32)ext_cache->num_of_rec + 1) * sizeof(pbm_ext_rec_in_use_s_type));

        if (ext_cache->num_of_rec)
        {
          memset(ext_cache->in_use_array, 0, ((uint32)ext_cache->num_of_rec + 1) *
                 sizeof(pbm_ext_rec_in_use_s_type));
        }
        break;
      } /* EXT_CACHE */

      /*---------------------------------------------------------------------*/
      /* PB_CACHE */
      case PBM_CACHE_TYPE_PB:
#ifdef FEATURE_ECC_HANDLING_AT_PBM
      /* ECC_CACHE */
      case PBM_CACHE_TYPE_ECC:
#endif
      {
        PB_CACHE *pb_cache = (PB_CACHE *) cache; //lint !e826 area too small

        /* if pb_cache_array is already filled with pointers to
           records, we need to free those records first */
        if (pb_cache->records_in_use > 0) {
          for ( k = 1; k <= pb_cache->num_of_records; k++ ) {
            if (pb_cache->pb_cache_array[k]) {
              (void)pbm_cache_record_free(pb_cache->pb_cache_array[k]);
            }
          }
        }

        ALLOC_MEM(pb_cache->pb_cache_array, pb_cache->num_of_records,
                  ((uint32) pb_cache->num_of_records + 1) *
                  sizeof(pbm_addr_cache_s_type *));

        pb_cache->start = 1;
        if (pb_cache->num_of_records)
        {
          memset(pb_cache->pb_cache_array, 0,
                 ((uint32) pb_cache->num_of_records + 1) *
                           sizeof(pbm_addr_cache_s_type *));
          pb_cache->circular = pbm_file_circular(file_id);
          pb_cache->start = pb_cache->num_of_records;

        }
        pb_cache->records_in_use = 0;
        break;
      } /* PB_CACHE */

      /*---------------------------------------------------------------------*/
#ifdef FEATURE_PBM_USIM_SUPPORT
      /* USIM_CACHE */
      case PBM_CACHE_TYPE_USIM:
      {
        PB_USIM_CACHE *usim_cache = (PB_USIM_CACHE *) cache; //lint !e826 area too small
        PB_MASTER_USIM_CACHE *m_usim_cache;
        pbm_pb_set_type pb_set = pbm_file_id_to_pb_set(file_id);
        int adn_records_this_file;
        int index;

        if (!usim_cache->num_of_records || !adn_cache[sim_id].num_of_records)
          break;

        PBM_CHECK_PBSET_RET(pb_set, VOID);
        m_usim_cache = &adn_m_usim_cache[sim_id][pb_set];

        if (usim_cache->mapping_type == GSDI_MAPPING_TYPE_2)
          index = usim_cache->order;
        else
          index = m_usim_cache->iap_rec_len + m_index[pb_set]++;

        if (index > m_usim_cache->num_caches) /* 1-based array */
        {
          PBM_MSG_ERR("ADN cache has wrong number of usim caches %d, %d",
                      m_usim_cache->num_caches, index, 0);
          break;
        }

        /* if pb_cache_array is already filled with pointers to
           records, we need to free those records first */
        if (usim_cache->records_in_use > 0) {
          for ( k = 1; k <= usim_cache->num_of_records; k++ ) {
            if (usim_cache->pb_cache_array[k]) {
              (void)pbm_usim_cache_delete(usim_cache, k, FALSE);
            }
          }
        }

        /* Allocate memory for USIM data */
        ALLOC_MEM(usim_cache->pb_cache_array, usim_cache->num_of_records,
                  ((uint32) usim_cache->num_of_records + 1) *
                  sizeof(pbm_usim_cache_s_type));

        /* Allocate memory for master USIM data */
        adn_records_this_file = adn_cache[sim_id].records_in_pb_set[pb_set];

        PBM_CHECK_PTR_RET(m_usim_cache->index_array, VOID);
        ALLOC_MEM_W_ERRCODE(m_usim_cache->index_array[index],
                            usim_cache->num_of_records,
                   (uint32) (adn_records_this_file+1) * sizeof(byte),
                            PBM_MEM_FREEIF(usim_cache->pb_cache_array));

        if (usim_cache->num_of_records)
        {
          memset(usim_cache->pb_cache_array, 0,
                 ((uint32) usim_cache->num_of_records + 1) *
                           sizeof(pbm_usim_cache_s_type));

          memset(m_usim_cache->index_array[index], 0,
                 (uint32) (adn_records_this_file+1) * sizeof(byte));

          /* Add this cache to the master usim cache array */
          m_usim_cache->usim_cache_array[index] = usim_cache;
          if (usim_cache->mapping_type != GSDI_MAPPING_TYPE_2)
            usim_cache->order = (uint8)index;
        }
        break;
      } /* USIM_CACHE */

      /*---------------------------------------------------------------------*/
      case PBM_CACHE_TYPE_INDEX:
      case PBM_CACHE_TYPE_SYNC1:
        break;

      /*---------------------------------------------------------------------*/
      case PBM_CACHE_TYPE_SYNC2:
      {
        PB_SYNC2_CACHE *sync2_cache = (PB_SYNC2_CACHE *) cache; //lint !e826 area too small

        if (!sync2_cache->num_of_records || !adn_cache[sim_id].num_of_records)
          break;

        /* Allocate memory for USIM data */
        ALLOC_MEM(sync2_cache->data_array, sync2_cache->num_of_records,
                  ((uint32) sync2_cache->num_of_records + 1) *
                            sizeof(uint16));
        if (sync2_cache->num_of_records)
        {
          memset(sync2_cache->data_array, 0,
                 ((uint32) sync2_cache->num_of_records + 1) *
                           sizeof(uint16));
        }

        break;
      }
#endif /* FEATURE_PBM_USIM_SUPPORT */
      /*---------------------------------------------------------------------*/

      default:
        PBM_MSG_ERR("Unrecognized file id %d", file_id, 0, 0);
        break;
    }

    pbm_fileinfo_requests[i].do_request = FALSE;
  }

  /* Initialize the PBM SEEK data before starting to read */
  pbm_init_seek_data(sim_id);

  pbm_initialize_index[sim_id] = 0;

  pbm_init_entries(sim_id);
}

/*===========================================================================
FUNCTION PBM_MAKE_FILEINFO_REQUEST

DESCRIPTION
  This function looks for the pbm_fileinfo_requests array looking for
  the next item to request.  If it finds one, it launches the request,
  otherwise it says we have completed getting the entries.

PARAMETERS
   start [in] The file_id with which to start. The interval of file_id is
              [PBM_FILE_ADN, PBM_FILE_MAX_REQUESTS).
              start is NOT the index into pbm_fileinfo_requests array but the
              index can be calculated from it.

SIDE EFFECTS
  Globals describing the world according to this SIM are set.
===========================================================================*/
static void pbm_make_fileinfo_request(pbm_slot_type sim_id, uint32 start)
{
  pbm_return_type pbm_status;
  uint32 i;                /* Index into pbm_fileinfo_requests array */
  uint32 file_id;
  PB_CACHE *cache;

  PBM_VALIDATE_SLOT_RET(sim_id, VOID);

  /* Starting at the current location, find the next item for which we
   * want to make a request. */
  for (file_id = start; file_id < (uint32)PBM_FILE_MAX_REQUESTS; file_id++)
  {
    i = file_id + ((uint32)sim_id*(uint32)PBM_FILE_MAX_REQUESTS);

    /* If this offset needs the request, send it off. */
    if (pbm_fileinfo_requests[i].do_request)
    {
      if ((pbm_fileinfo_requests[i].service_available == GSDI_NONE) ||
          (pbm_gsdi_is_service_allowed (sim_id, pbm_fileinfo_requests[i].service_available)))
      {
        pbm_status = pbm_gsdi_get_file_attributes(
            sim_id,                             /* Slot index. */
            pbm_fileinfo_requests[i].filename,  /* Filename */
            i,                                  /* Client Ref */
            pbm_gsdi_fileinfo_cb);              /* Async CB. */
        if (pbm_status == PBM_SUCCESS)
          return;
        else
          PBM_MSG_ERR("Unexpected return value from pbm_gsdi_get_file_attributes %d for index %d",
              pbm_status, i, 0);
      }
      /* Could not get file attributes, so this file will not be read */
      cache = pbm_pb_id_to_cache(pbm_file_id_to_pb_id(sim_id,
                                          (pbm_file_device_e_type)file_id));
      if (cache != NULL)
        cache->status = PBM_STATUS_NOT_AVAILABLE;
    }
  }

  /* If we reach this line, that means we have made all the requests that
   * will be pending for this sim_id.  Call pbm_fileinfo_complete to process
   * this information and continue the initialization process.
   */
  pbm_fileinfo_complete(sim_id);
}

/*===========================================================================
FUNCTION PBM_FILL_FILEINFO_REQUEST

DESCRIPTION
  This function is used to fill a fileinfo request for a given PBM file.

DEPENDENCIES
  card_apps must be previously set.
  Not reentrant. This function should always be called from PBM task.

SIDE EFFECTS
  Globals describing the world according to this SIM are set.
===========================================================================*/
static void pbm_fill_fileinfo_request(pbm_slot_type sim_id,
                                      pbm_file_device_e_type file_id,
                                      gsdi_services_type service,
                                      mmgsdi_file_enum_type sim_file)
{
  pbm_fileinfo_requests[PBM_SIM_FILENAME_TO_INDEX(sim_id, file_id)].service_available   = service;
  pbm_fileinfo_requests[PBM_SIM_FILENAME_TO_INDEX(sim_id, file_id)].filename            = sim_file;
  pbm_fileinfo_requests[PBM_SIM_FILENAME_TO_INDEX(sim_id, file_id)].do_request = TRUE;
}

/*===========================================================================
FUNCTION PBM_INIT_FILEINFO_REQUESTS

DESCRIPTION
  This function is used to initialize pbm_fileinfo_requests for
  the specified SIM filenames.

  Note that extension filenames are initialized if required and need not
  be specified separately.

DEPENDENCIES
  card_apps must be previously set.
  Not reentrant. This function should always be called from PBM task.

SIDE EFFECTS
  Globals describing the world according to this SIM are set.
===========================================================================*/
static void pbm_init_fileinfo_requests(pbm_slot_type sim_id,
                                       pbm_file_device_e_type file_id)
{
  PBM_VALIDATE_SLOT_RET(sim_id, VOID);

  switch (file_id) {
    /* PBM_FILE_ADN */
    case PBM_FILE_ADN:
#ifdef FEATURE_PBM_USIM_SUPPORT
      if (USE_USIM(sim_id))
      {
        if (USE_USIM_LOCAL(sim_id))
        {
          /* Local USIM Phonebook */
          pbm_fill_fileinfo_request(sim_id, PBM_FILE_ADN,  GSDI_LOCAL_PHONEBOOK, MMGSDI_USIM_ADN);
          pbm_fill_fileinfo_request(sim_id, PBM_FILE_EXT1, GSDI_EXT1,            MMGSDI_USIM_EXT1);

          /* Local USIM ADN1 (Phonebook set 2) */
          pbm_fill_fileinfo_request(sim_id, PBM_FILE_ADN1, GSDI_NONE,            MMGSDI_USIM_ADN1);
#ifdef FEATURE_USIM_1000_PBM
          /* Local USIM ADN2 (Phonebook set 3) */
          pbm_fill_fileinfo_request(sim_id, PBM_FILE_ADN2, GSDI_NONE,            MMGSDI_USIM_ADN2);
          /* Local USIM ADN3 (Phonebook set 4) */
          pbm_fill_fileinfo_request(sim_id, PBM_FILE_ADN3, GSDI_NONE,            MMGSDI_USIM_ADN3);
#endif
        }
        else
        {
          /* Global USIM Phonebook */
          pbm_fill_fileinfo_request(sim_id, PBM_FILE_ADN,  GSDI_GLOBAL_PHONEBOOK, MMGSDI_TELECOM_ADN);
          pbm_fill_fileinfo_request(sim_id, PBM_FILE_EXT1, GSDI_EXT1,             MMGSDI_TELECOM_EXT1);

          /* Global USIM ADN1 (Phonebook set 2) */
          pbm_fill_fileinfo_request(sim_id, PBM_FILE_ADN1, GSDI_GLOBAL_PHONEBOOK, MMGSDI_TELECOM_ADN1);
#ifdef FEATURE_USIM_1000_PBM
          /* Global USIM ADN2 (Phonebook set 3) */
          pbm_fill_fileinfo_request(sim_id, PBM_FILE_ADN2, GSDI_GLOBAL_PHONEBOOK, MMGSDI_TELECOM_ADN2);
          /* Global USIM ADN3 (Phonebook set 4) */
          pbm_fill_fileinfo_request(sim_id, PBM_FILE_ADN3, GSDI_GLOBAL_PHONEBOOK, MMGSDI_TELECOM_ADN3);
#endif
        }
      }
      else
#endif /* FEATURE_PBM_USIM_SUPPORT */
      {
        /* Not USIM ie GSM SIM or RUIM */
        pbm_fill_fileinfo_request(sim_id, PBM_FILE_ADN,  GSDI_ADN,              MMGSDI_TELECOM_ADN);
        pbm_fill_fileinfo_request(sim_id, PBM_FILE_EXT1, GSDI_EXT1,             MMGSDI_TELECOM_EXT1);
      }
      break;

     /* PBM_FILE_SDN */
    case PBM_FILE_SDN:
#ifdef FEATURE_PBM_USIM_SUPPORT
      if (USE_USIM(sim_id))
      {
        pbm_fill_fileinfo_request(sim_id, PBM_FILE_SDN, GSDI_SDN,  MMGSDI_USIM_SDN);
        pbm_fill_fileinfo_request(sim_id, PBM_FILE_EXT3, GSDI_EXT3, MMGSDI_USIM_EXT3);
      }
      else
#endif /* FEATURE_PBM_USIM_SUPPORT */
      {
        pbm_fill_fileinfo_request(sim_id, PBM_FILE_SDN, GSDI_SDN,  MMGSDI_TELECOM_SDN);
        pbm_fill_fileinfo_request(sim_id, PBM_FILE_EXT3, GSDI_EXT3, MMGSDI_TELECOM_EXT3);
      }
      break;

     /* PBM_FILE_MSISDN */
    case PBM_FILE_MSISDN:
#ifdef FEATURE_PBM_USIM_SUPPORT
      if (USE_USIM(sim_id))
      {
        /* USIM uses EXT5 */
        pbm_fill_fileinfo_request(sim_id, PBM_FILE_MSISDN, GSDI_MSISDN, MMGSDI_USIM_MSISDN);
        pbm_fill_fileinfo_request(sim_id, PBM_FILE_EXT3, GSDI_EXT5, MMGSDI_USIM_EXT5);
      }
      else
#endif /* FEATURE_PBM_USIM_SUPPORT */
      {
        /* SIM uses EXT3 */
        pbm_fill_fileinfo_request(sim_id, PBM_FILE_MSISDN, GSDI_MSISDN, MMGSDI_TELECOM_MSISDN);
        pbm_fill_fileinfo_request(sim_id, PBM_FILE_EXT3, GSDI_EXT3, MMGSDI_TELECOM_EXT3);
      }

      break;

#ifdef FEATURE_PBM_USIM_SUPPORT
    /* PBM_FILE_MBDN */
    case PBM_FILE_MBDN:
      if (USE_USIM(sim_id))
      {
        /* uses EXT6 */
        pbm_fill_fileinfo_request(sim_id, PBM_FILE_MBDN, GSDI_MAILBOX_DIAL, MMGSDI_USIM_MBDN);
        pbm_fill_fileinfo_request(sim_id, PBM_FILE_EXT6, GSDI_MAILBOX_DIAL, MMGSDI_USIM_EXT6);
      }
      else
      {
        /* uses EXT6 */
        pbm_fill_fileinfo_request(sim_id, PBM_FILE_MBDN, GSDI_MAILBOX_DIAL, MMGSDI_GSM_MBDN);
        pbm_fill_fileinfo_request(sim_id, PBM_FILE_EXT6, GSDI_MAILBOX_DIAL, MMGSDI_GSM_EXT6);
      }

      break;
#endif /* FEATURE_PBM_USIM_SUPPORT */

    /* PBM_FILE_MBN */
    case PBM_FILE_MBN:
#ifdef FEATURE_PBM_USIM_SUPPORT
      if (USE_USIM(sim_id))
      {
        pbm_fill_fileinfo_request(sim_id, PBM_FILE_MBN, GSDI_NONE, MMGSDI_USIM_MN);
      }
      else
#endif /* FEATURE_PBM_USIM_SUPPORT */
      {
        pbm_fill_fileinfo_request(sim_id, PBM_FILE_MBN, GSDI_NONE, MMGSDI_GSM_MN);
      }

      break;

    /* PBM_FILE_FDN */
    case PBM_FILE_FDN:
#ifdef FEATURE_PBM_USIM_SUPPORT
      if (USE_USIM(sim_id))
      {
        pbm_fill_fileinfo_request(sim_id, PBM_FILE_FDN, GSDI_FDN, MMGSDI_USIM_FDN);
        pbm_fill_fileinfo_request(sim_id, PBM_FILE_EXT2, GSDI_EXT2, MMGSDI_USIM_EXT2);
      }
      else
#endif /* FEATURE_PBM_USIM_SUPPORT */
      {
        pbm_fill_fileinfo_request(sim_id, PBM_FILE_FDN, GSDI_FDN, MMGSDI_TELECOM_FDN);
        pbm_fill_fileinfo_request(sim_id, PBM_FILE_EXT2, GSDI_EXT2, MMGSDI_TELECOM_EXT2);
      }
      break;

    /* PBM_FILE_LND */
    case PBM_FILE_LND:
#ifdef FEATURE_PBM_USIM_SUPPORT
      if (USE_USIM(sim_id) == 0)
#endif /* FEATURE_PBM_USIM_SUPPORT */
      {
        pbm_fill_fileinfo_request(sim_id, PBM_FILE_EXT1, GSDI_EXT1, MMGSDI_TELECOM_EXT1);
        pbm_fill_fileinfo_request(sim_id, PBM_FILE_LND, GSDI_LND, MMGSDI_TELECOM_LND);
      }
      break;
#ifdef FEATURE_PBM_USIM_SUPPORT
    /* PBM_FILE_OCI */
    case PBM_FILE_OCI:
      if (USE_USIM(sim_id))
      {
        pbm_fill_fileinfo_request(sim_id, PBM_FILE_OCI, GSDI_OCI_OCT, MMGSDI_USIM_OCI);
        pbm_fill_fileinfo_request(sim_id, PBM_FILE_EXT5, GSDI_EXT5, MMGSDI_USIM_EXT5);
      }
      break;

    /* PBM_FILE_ICI */
    case PBM_FILE_ICI:
      if (USE_USIM(sim_id))
      {
        pbm_fill_fileinfo_request(sim_id, PBM_FILE_ICI, GSDI_ICI_ICT, MMGSDI_USIM_ICI);
        pbm_fill_fileinfo_request(sim_id, PBM_FILE_EXT5, GSDI_EXT5, MMGSDI_USIM_EXT5);
      }
      break;
#endif /* FEATURE_PBM_USIM_SUPPORT */
#ifdef FEATURE_ECC_HANDLING_AT_PBM
    case PBM_FILE_ECC:
      if (USE_USIM(sim_id))
        pbm_fill_fileinfo_request(sim_id, PBM_FILE_ECC,  GSDI_NONE, MMGSDI_USIM_ECC);
      else if(pbm_get_card_apps(sim_id) & GSDI_RUIM_APP_MASK) //for RUIM mode
        pbm_fill_fileinfo_request(sim_id, PBM_FILE_ECC,  GSDI_NONE, MMGSDI_CDMA_ECC);
      else //for GSM mode
        pbm_fill_fileinfo_request(sim_id, PBM_FILE_ECC,  GSDI_NONE, MMGSDI_GSM_ECC);
      break;
#endif
    /* PBM_FILE_EXT1 */
    case PBM_FILE_EXT1:
#ifdef FEATURE_PBM_USIM_SUPPORT
      if (USE_USIM_LOCAL(sim_id))
      {
        pbm_fill_fileinfo_request(sim_id, PBM_FILE_EXT1, GSDI_EXT1, MMGSDI_USIM_EXT1);
      }
      else
#endif /* FEATURE_PBM_USIM_SUPPORT */
      {
        pbm_fill_fileinfo_request(sim_id, PBM_FILE_EXT1, GSDI_EXT1, MMGSDI_TELECOM_EXT1);
      }
      break;

    /* PBM_FILE_EXT2 */
    case PBM_FILE_EXT2:
#ifdef FEATURE_PBM_USIM_SUPPORT
      if (USE_USIM(sim_id))
      {
        pbm_fill_fileinfo_request(sim_id, PBM_FILE_EXT2, GSDI_EXT2, MMGSDI_USIM_EXT2);
      }
      else
#endif /* FEATURE_PBM_USIM_SUPPORT */
      {
        pbm_fill_fileinfo_request(sim_id, PBM_FILE_EXT2, GSDI_EXT2, MMGSDI_TELECOM_EXT2);
      }
      break;
#ifdef FEATURE_PBM_USIM_SUPPORT
    /* PBM_FILE_EXT5 */
    case PBM_FILE_EXT5:
      pbm_fill_fileinfo_request(sim_id, PBM_FILE_EXT6, GSDI_MAILBOX_DIAL, MMGSDI_USIM_EXT5);
      break;

    /* PBM_FILE_EXT6 */
    case PBM_FILE_EXT6:
      if (USE_USIM(sim_id))
      {
        pbm_fill_fileinfo_request(sim_id, PBM_FILE_EXT6, GSDI_MAILBOX_DIAL, MMGSDI_USIM_EXT6);
      }
      else
      {
        pbm_fill_fileinfo_request(sim_id, PBM_FILE_EXT6, GSDI_MAILBOX_DIAL, MMGSDI_GSM_EXT6);
      }

      break;

      /* USIM files */
    case PBM_FILE_PSC:
    case PBM_FILE_PUID:
    case PBM_FILE_CC:
    case PBM_FILE_EMAIL:
    case PBM_FILE_SNE:
    case PBM_FILE_GRP:
    case PBM_FILE_ANR:
    case PBM_FILE_ANRA:
    case PBM_FILE_ANRB:
    case PBM_FILE_ANRC:
    case PBM_FILE_IAP:
    case PBM_FILE_UID:
    case PBM_FILE_PBC:
    case PBM_FILE_ADN1:
    case PBM_FILE_EMAIL1:
    case PBM_FILE_SNE1:
    case PBM_FILE_GRP1:
    case PBM_FILE_ANR1:
    case PBM_FILE_ANRA1:
    case PBM_FILE_ANRB1:
    case PBM_FILE_ANRC1:
    case PBM_FILE_IAP1:
    case PBM_FILE_UID1:
    case PBM_FILE_PBC1:
#ifdef FEATURE_USIM_1000_PBM
    case PBM_FILE_ADN2:
    case PBM_FILE_EMAIL2:
    case PBM_FILE_SNE2:
    case PBM_FILE_GRP2:
    case PBM_FILE_ANR2:
    case PBM_FILE_ANRA2:
    case PBM_FILE_ANRB2:
    case PBM_FILE_ANRC2:
    case PBM_FILE_IAP2:
    case PBM_FILE_UID2:
    case PBM_FILE_PBC2:
    case PBM_FILE_ADN3:
    case PBM_FILE_EMAIL3:
    case PBM_FILE_SNE3:
    case PBM_FILE_GRP3:
    case PBM_FILE_ANR3:
    case PBM_FILE_ANRA3:
    case PBM_FILE_ANRB3:
    case PBM_FILE_ANRC3:
    case PBM_FILE_IAP3:
    case PBM_FILE_UID3:
    case PBM_FILE_PBC3:
#endif
      if (USE_USIM(sim_id))
      {
        pbm_fill_fileinfo_request(sim_id, file_id, GSDI_NONE,
                                  pbm_file_to_uim_file[file_id].uim_filename[USE_USIM_LOCAL(sim_id)]);
      }
      else
      {
        PBM_MSG_ERR("File %d available only for USIM (sim %d)", file_id, sim_id, 0);
      }
      break;
#endif /* FEATURE_PBM_USIM_SUPPORT */
    default:
      PBM_MSG_ERR("Unsupported init file_name %d", file_id, 0, 0);
      break;
  }
}

/*===========================================================================
FUNCTION PBM_START_FILEINFO_REQUEST

DESCRIPTION
   This function starts the initialization chain.
   It sends the first make file info request or reads USIM PBR if we are using
   a USIM.

DEPENDENCIES
  card_apps must be previously set.
  Assumes that pbm_fileinfo_requests have already been filled in.
  Not reentrant. This function should always be called from PBM task.

SIDE EFFECTS
  Globals describing the world according to this SIM are set.
===========================================================================*/
static void pbm_start_fileinfo_request(pbm_slot_type sim_id)
{
#ifdef FEATURE_PBM_USIM_SUPPORT
  boolean bReadPBR;
  int i;
#endif /* FEATURE_PBM_USIM_SUPPORT */
  PBM_VALIDATE_SLOT_RET(sim_id, VOID);

#ifdef FEATURE_PBM_USIM_SUPPORT
  if (USE_USIM(sim_id))
  {
    /* Read USIM phonebook structure and then start initialization */
    bReadPBR = FALSE;
    if (!pbm_refresh_init)
    {
      bReadPBR = TRUE;
    }
    else
    {
      for (i=0; i < pbm_refresh_num_file_devices; i++)
      {
        if (pbm_refresh_pbm_file_devices[i] == PBM_FILE_ADN)
        {
          bReadPBR = TRUE;
          break;
        }
      }
    }

    if (bReadPBR)
    {
      if (pbm_usim_read_pbr(sim_id) != PBM_SUCCESS)
      {
        PBM_MSG_ERR("Unable to read PBR info from USIM", 0, 0, 0);
        /* Go ahead with the remaining files */
        pbm_make_fileinfo_request(sim_id, 0);
      }
    }
    else
    {
      /* Start initialization chain right away */
      pbm_make_fileinfo_request(sim_id, 0);
    }
  }
  else
#endif /* FEATURE_PBM_USIM_SUPPORT */
  {
    /* Start initialization chain right away */
    pbm_make_fileinfo_request(sim_id, 0);
  }
}

/*===========================================================================
FUNCTION PBM_INIT_FILEINFO_INTERNAL

DESCRIPTION
  This function will be called when GSDI gives us the SIM init completed
  event. If there is an initialization in progress for this SIM,
  that initialization is stopped and restarted from the beginning.

  Only one sequence of initializations can happen for each SIM.

DEPENDENCIES
  card_apps must be previously set.
  Not reentrant. This function should always be called from PBM task.

SIDE EFFECTS
  Globals describing the world according to this SIM are set.
===========================================================================*/
void pbm_init_fileinfo_internal(pbm_slot_type sim_id)
{
  int i;

  PBM_VALIDATE_SLOT_RET(sim_id, VOID);


  pbm_init_info.pbm_sim_init_done[sim_id] = FALSE;


  /* find the files that need refreshing */
  if (pbm_refresh_init)
  {
    for (i=0; i<pbm_refresh_num_file_devices; i++)
    {
      pbm_init_fileinfo_requests(sim_id, pbm_refresh_pbm_file_devices[i]);
    }
  }
  else
  {
    pbm_init_fileinfo_requests(sim_id, PBM_FILE_ADN);
    pbm_init_fileinfo_requests(sim_id, PBM_FILE_SDN);
    pbm_init_fileinfo_requests(sim_id, PBM_FILE_MSISDN);
    pbm_init_fileinfo_requests(sim_id, PBM_FILE_MBN);
    pbm_init_fileinfo_requests(sim_id, PBM_FILE_FDN);
    pbm_init_fileinfo_requests(sim_id, PBM_FILE_LND);
#ifdef FEATURE_PBM_USIM_SUPPORT
    pbm_init_fileinfo_requests(sim_id, PBM_FILE_ICI);
    pbm_init_fileinfo_requests(sim_id, PBM_FILE_OCI);
    pbm_init_fileinfo_requests(sim_id, PBM_FILE_MBDN);
#endif /* FEATURE_PBM_USIM_SUPPORT */
#ifdef FEATURE_ECC_HANDLING_AT_PBM
    pbm_init_fileinfo_requests(sim_id, PBM_FILE_ECC);
#endif
  }

  /* If I'm still waiting, continue initialization later */
  if (pbm_slot_initializing & PBM_SIMID_TO_SLOT_MASK(sim_id))
  {
    PBM_SIMSLOT_SET(pbm_slot_restart, sim_id);
    return;
  }

#ifndef FEATURE_ECC_HANDLING_AT_PBM
  //if this feature is defined then we will use different procedure for ECC numbers
  pbm_init_ecc_internal(sim_id);
#endif

  PBM_SIMSLOT_SET(pbm_slot_initializing, sim_id);

  pbm_start_fileinfo_request(sim_id);
}

/*===========================================================================
FUNCTION PBM_START_FILEINFO_INTERNAL

DESCRIPTION
  This function starts the initialization chain without re-initializing the
  pbm_fileinfo_requests array. The caller should set the appropriate
  element in pbm_fileinfo_requests before calling this function using
  pbm_init_fileinfo_requests().

  This is useful for reading only some of the files in the SIM.

DEPENDENCIES
  card_apps must be previously set.
  Assumes that pbm_fileinfo_requests have already been filled in.
  Not reentrant. This function should always be called from PBM task.

SIDE EFFECTS
  Globals describing the world according to this SIM are set.
===========================================================================*/
static void pbm_start_fileinfo_internal(pbm_slot_type sim_id)
{
  PBM_VALIDATE_SLOT_RET(sim_id, VOID);

  /* If there is an initialization going on, continue this later */
  if (pbm_slot_initializing & PBM_SIMID_TO_SLOT_MASK(sim_id)) {
    PBM_SIMSLOT_SET(pbm_slot_restart, sim_id);
    return;
  }

  PBM_SIMSLOT_SET(pbm_slot_initializing, sim_id);

  /* Start initialization chain */
  pbm_start_fileinfo_request(sim_id);
}

/*===========================================================================
FUNCTION PBM_RESTART_FILEINFO_INTERNAL

DESCRIPTION
  This function is called from a GSDI callback to restart an initialization
  chain that is in limbo. An initialization chain is suspended when
  pbm_init_fileinfo_internal or pbm_start_fileinfo_internal is called
  while another initialization is in progress.

DEPENDENCIES
  Not reentrant. This function should always be called from PBM task.

SIDE EFFECTS
  Globals describing the world according to this SIM are set.
===========================================================================*/
void pbm_restart_fileinfo_internal(pbm_slot_type sim_id)
{
  PBM_VALIDATE_SLOT_RET(sim_id, VOID);

  PBM_SIMSLOT_RESET(pbm_slot_initializing, sim_id);
  PBM_SIMSLOT_RESET(pbm_slot_restart, sim_id);

  pbm_handle_sim_init_completed(sim_id);
}

/*===========================================================================
FUNCTION PBM_STOP_FILEINFO_INTERNAL

DESCRIPTION
  This function is called to stop an initialization chain when SIM is
  removed.

DEPENDENCIES
  Not reentrant. This function should always be called from PBM task.

SIDE EFFECTS
  Globals describing the world according to this SIM are set.
===========================================================================*/
void pbm_stop_fileinfo_internal(pbm_slot_type sim_id)
{
  PBM_VALIDATE_SLOT_RET(sim_id, VOID);

  PBM_SIMSLOT_RESET(ecc_initialized, sim_id);

  PBM_SIMSLOT_RESET(pbm_slot_initializing, sim_id);
  PBM_SIMSLOT_RESET(pbm_slot_restart, sim_id);

  PBM_SIMSLOT_RESET(pbm_slot_pin_waiting, sim_id);
  PBM_SIMSLOT_RESET(pbm_slot_fdn_enabled, sim_id);
}

/*===========================================================================
FUNCTION PBM_FDN_ENABLE

DESCRIPTION
  This function is called is response to a GSDI_FDN_ENABLED event.
  It re-initializes FDN. If ADN is restricted when FDN is enabled for this
  SIM, then ADN cache is also updated.

DEPENDENCIES
  Not reentrant. This function should always be called from PBM task.

SIDE EFFECTS
  Globals describing the world according to this SIM are set.
===========================================================================*/
void pbm_fdn_enable(pbm_slot_type sim_id)
{
  PBM_VALIDATE_SLOT_RET(sim_id, VOID);

  PBM_SIMSLOT_SET(pbm_slot_fdn_enabled, sim_id);

  if (!(pbm_slot_pin_waiting & PBM_SIMID_TO_SLOT_MASK(sim_id))) {
    pbm_init_fileinfo_requests(sim_id, PBM_FILE_ADN);
    pbm_init_fileinfo_requests(sim_id, PBM_FILE_FDN);

    pbm_start_fileinfo_internal(sim_id);
  }
}

/*===========================================================================
FUNCTION PBM_FDN_DISABLE

DESCRIPTION
  This function is called is response to a GSDI_FDN_DISABLED event.
  It re-initializes FDN and if ADN is readable now, it reinitalizes
  ADN too.

DEPENDENCIES
  Not reentrant. This function should always be called from PBM task.

SIDE EFFECTS
  Globals describing the world according to this SIM are set.
===========================================================================*/
void pbm_fdn_disable(pbm_slot_type sim_id)
{
  PBM_VALIDATE_SLOT_RET(sim_id, VOID);

  PBM_SIMSLOT_RESET(pbm_slot_fdn_enabled, sim_id);

  if (!(pbm_slot_pin_waiting & PBM_SIMID_TO_SLOT_MASK(sim_id))) {
    pbm_init_fileinfo_requests(sim_id, PBM_FILE_ADN);
    pbm_init_fileinfo_requests(sim_id, PBM_FILE_FDN);

    pbm_start_fileinfo_internal(sim_id);
  }
}

/*===========================================================================
FUNCTION PBM_FILL_PB_CACHE_INFO

DESCRIPTION
  This function is called to fill in the cache info for a PB cache
  when info comes back from GSDI

PARAMETERS

SIDE EFFECTS
  Globals describing the world according to this SIM are set.
===========================================================================*/
static void pbm_fill_pb_cache_info(PB_CACHE *cache,
                                   pbm_slot_type sim_id,
                                   pbm_pb_set_type pb_set,
                                   int record_length,
                                   int min_record_length,
                                   pbm_file_device_e_type file_id,
                                   mmgsdi_file_structure_enum_type file_type,
                                   mmgsdi_file_enum_type  filename,
                                   boolean increment_num_files,
                                   int num_records)
{
  if (record_length >= min_record_length)
  {
    cache[sim_id].file_ids[pb_set] = file_id;
    cache[sim_id].file_type = file_type;
    cache[sim_id].filenames[pb_set] = filename;
    if (increment_num_files){
      cache[sim_id].num_files++;
      cache[sim_id].num_of_records += num_records;
    } else {
      cache[sim_id].num_files = 1;
      cache[sim_id].num_of_records = num_records;
    }
    cache[sim_id].record_length = record_length;
    cache[sim_id].records_in_pb_set[pb_set] = num_records;
    cache[sim_id].text_length = record_length - min_record_length;
    if (cache[sim_id].num_of_records > 0)
      cache[sim_id].status = PBM_STATUS_NOT_READY;
    else
      PBM_SET_PB_CACHE_READY(&cache[sim_id]);
  }
  else
  {
    PBM_MSG_ERR("Bad data received from SIM 0x%x 0x%x 0x%x",
                sim_id,
                filename,
                record_length);
    cache[sim_id].status = PBM_STATUS_NOT_AVAILABLE;
  }
}

/*===========================================================================
FUNCTION PBM_FILL_PB_EXTCACHE_INFO

DESCRIPTION
  This function is called to fill in the cache info for a PB ext cache
  when info comes back from GSDI

PARAMETERS

SIDE EFFECTS
  Globals describing the world according to this SIM are set.
===========================================================================*/
static boolean pbm_fill_pb_extcache_info(pbm_ext_cache_s_type *pExtCache,
                                         pbm_file_device_e_type file_id,
                                         mmgsdi_file_structure_enum_type file_type,
                                         mmgsdi_file_enum_type  filename,
                                         int num_records,
                                         int record_length,
                                         pbm_slot_type sim_id)
{
   if (record_length == EXT_KNOWN_REC_LEN)
   {
      pExtCache->file_id        = file_id;
      pExtCache->file_type      = file_type;
      pExtCache->filename       = filename;
      pExtCache->num_of_rec     = num_records;
      pExtCache->ext_rec_length = record_length;
      return TRUE;
   }
   else
   {
      PBM_MSG_ERR("Bad EXT data received from SIM 0x%x 0x%x 0x%x",
                  sim_id,
                  filename,
                  record_length);
      return FALSE;
   }
}

#ifdef FEATURE_PBM_USIM_SUPPORT
/*===========================================================================
FUNCTION PBM_FILL_PB_SYNC1CACHE_INFO

DESCRIPTION
  This function is called to fill in the cache info for a PB sync1 cache
  when info comes back from GSDI

PARAMETERS

SIDE EFFECTS
  Globals describing the world according to this SIM are set.
===========================================================================*/
static void pbm_fill_pb_sync1cache_info(PB_SYNC1_CACHE *pSync1Cache,
                                        pbm_file_device_e_type file_id,
                                        mmgsdi_file_structure_enum_type file_type,
                                        mmgsdi_file_enum_type  filename,
                                        int num_records,
                                        int record_length,
                                        pbm_slot_type sim_id)
{
   if (file_type == MMGSDI_TRANSPARENT_FILE)
   {
      pSync1Cache->file_id   = file_id;
      pSync1Cache->file_type = file_type;
      pSync1Cache->filename  = filename;
      /* GSDI does not provide number of records and record length for
       * transparent files */
      pSync1Cache->num_of_records = num_records;
      pSync1Cache->record_length  = record_length;
   }
   else
   {
      PBM_MSG_ERR("Bad sync1 cache data received from SIM 0x%x 0x%x 0x%x", sim_id,
                  filename, file_type);
   }
}

/*===========================================================================
FUNCTION PBM_FILL_PB_SYNC2CACHE_INFO

DESCRIPTION
  This function is called to fill in the cache info for a PB sync2 cache
  when info comes back from GSDI

PARAMETERS

SIDE EFFECTS
  Globals describing the world according to this SIM are set.
===========================================================================*/
static void pbm_fill_pb_sync2cache_info(PB_SYNC2_CACHE *pSync2Cache,
                                        pbm_file_device_e_type file_id,
                                        mmgsdi_file_structure_enum_type file_type,
                                        mmgsdi_file_enum_type  filename,
                                        int num_records,
                                        int record_length,
                                        pbm_slot_type sim_id)
{
   pbm_pb_set_type pb_set;

   pb_set = pbm_file_id_to_pb_set(file_id);
   PBM_CHECK_PBSET_RET(pb_set, VOID);

   if (record_length)
   {
      if (pSync2Cache->record_length !=
          record_length)
      {
         PBM_MSG_ERR("Record length of Sync2 cache and PB %d does not match %d %d",
                     pb_set,
                     pSync2Cache->record_length,
                     record_length);
         return;
      }
      pSync2Cache->file_ids[pb_set] = file_id;
      pSync2Cache->file_type = file_type;
      pSync2Cache->filenames[pb_set] = filename;
      pSync2Cache->num_of_records += num_records;
      pSync2Cache->records_in_pb_set[pb_set] = num_records;
      pSync2Cache->num_files++;
   }
   else
   {
      PBM_MSG_ERR("Bad Sync2 data received from SIM 0x%x 0x%x 0x%x", sim_id,
                  filename, record_length);
   }
}
#endif /* FEATURE_PBM_USIM_SUPPORT */

/*===========================================================================
FUNCTION PBM_FILEINFO_CB

DESCRIPTION
  This function is called by GSDI when it has the results of our last
  get_file_attributes request.   It takes the request, looks at the
  client_ref to determine for which type of file the request was made,
  and stores the results.  It then calls the make_request function to
  get the next set of file attributes.  This way, this are never more
  than one PBM request to GSDI outstanding.

PARAMETERS
   sim_data [in] The GSDI results.

SIDE EFFECTS
  Globals describing the world according to this SIM are set.
===========================================================================*/
void pbm_fileinfo_cb(mmgsdi_return_enum_type status,
                     mmgsdi_cnf_enum_type    data_type,
                     const mmgsdi_cnf_type  *data_ptr)
{
  pbm_slot_type sim_id;
  pbm_file_device_e_type file_id;
  uint32 client_ref;
#ifdef FEATURE_PBM_USIM_SUPPORT
  PB_USIM_CACHE *usim_cache;
  PB_MASTER_USIM_CACHE *m_usim_cache;
#endif /* FEATURE_PBM_USIM_SUPPORT */
  pbm_pb_set_type pb_set;
  mmgsdi_file_structure_enum_type file_type;
  int number_of_records = 0;
  int record_length = 0;

  PBM_CHECK_PTR_RET(data_ptr, VOID);

  if (data_type != MMGSDI_GET_FILE_ATTR_CNF)
  {
    PBM_MSG_ERR("invalid confirmation type %d in pbm_fileinfo_cb", data_type, 0, 0);
    return;
  }

  client_ref = data_ptr->response_header.client_data;
  if (client_ref >= ARR_SIZE(pbm_fileinfo_requests))
  {
    PBM_MSG_ERR("Invalid client_ref found: %d\n", client_ref, 0, 0);
    return;
  }

  sim_id = (pbm_slot_type)((int) client_ref / (int) PBM_FILE_MAX_REQUESTS);
  PBM_VALIDATE_SLOT_RET(sim_id, VOID);

  file_id = (pbm_file_device_e_type) (client_ref % (int)PBM_FILE_MAX_REQUESTS);

  /* Restart initialization if required */
  if (pbm_slot_restart & PBM_SIMID_TO_SLOT_MASK(sim_id))
  {
    PBM_MSG_HIGH("PBM restarting init of SIM %d", sim_id, 0, 0);
    pbm_restart_fileinfo_internal(sim_id);
    return;
  }

  /* Stop initialization if required */
  if (!(pbm_slot_initializing & PBM_SIMID_TO_SLOT_MASK(sim_id))) {
    PBM_MSG_HIGH("PBM stopping init of SIM %d", sim_id, 0, 0);
    return;
  }

  if (status == MMGSDI_SUCCESS)
  {
    file_type = data_ptr->get_file_attr_cnf.file_attrib.file_type;
    switch (file_type)
    {
      case MMGSDI_LINEAR_FIXED_FILE:
        number_of_records = data_ptr->get_file_attr_cnf.file_attrib.file_info.linear_fixed_file.num_of_rec;
        record_length     = data_ptr->get_file_attr_cnf.file_attrib.file_info.linear_fixed_file.rec_len;
        break;

      case MMGSDI_CYCLIC_FILE:
        number_of_records = data_ptr->get_file_attr_cnf.file_attrib.file_info.cyclic_file.num_of_rec;
        record_length     = data_ptr->get_file_attr_cnf.file_attrib.file_info.cyclic_file.rec_len;
        break;

      default:
        number_of_records = 0;
        record_length = 0;
        break;
    }

    //If we're a transparent file, or if we have records
    if ((file_type == MMGSDI_TRANSPARENT_FILE) || (number_of_records > 0))
    {
      switch (file_id)
      {
        case PBM_FILE_ADN:
#ifdef FEATURE_PBM_USIM_SUPPORT
        case PBM_FILE_ADN1:
#ifdef FEATURE_USIM_1000_PBM
        case PBM_FILE_ADN2:
        case PBM_FILE_ADN3:
#endif
#endif /* FEATURE_PBM_USIM_SUPPORT */
          pb_set = pbm_file_id_to_pb_set(file_id);
          PBM_CHECK_PBSET_RETCODE_BREAK(pb_set, adn_cache[sim_id].status,
                                        PBM_STATUS_NOT_AVAILABLE);

          if ((file_id != PBM_FILE_ADN) && (adn_cache[sim_id].record_length != record_length))
            {
              PBM_MSG_ERR("ADN and ADN-n files differ in record length %d %d, pb set %d",
                          adn_cache[sim_id].record_length,
                          record_length,
                          pb_set);
              adn_cache[sim_id].status = PBM_STATUS_NOT_AVAILABLE;
              break;
            }

          pbm_fill_pb_cache_info(adn_cache,
                                 sim_id,
                                 pb_set,
                                 record_length,
                                 ADN_KNOWN_REC_LEN,
                                 file_id,
                                 file_type,
                                 pbm_fileinfo_requests[client_ref].filename,
                                 (file_id == PBM_FILE_ADN) ? FALSE : TRUE,
                                 number_of_records);

            PBM_MSG_HIGH("PBM found %d records in PB set %d",
                         adn_cache[sim_id].records_in_pb_set[pb_set],
                         pb_set,0);
#ifdef FEATURE_PBM_USIM_SUPPORT
            adn_cache[sim_id].m_usim_cache[pb_set] =
              &adn_m_usim_cache[sim_id][pb_set];
#endif /* FEATURE_PBM_USIM_SUPPORT */
          if ((pbm_slot_fdn_enabled & PBM_SIMID_TO_SLOT_MASK(sim_id)) &&
              !data_ptr->get_file_attr_cnf.file_attrib.file_info.linear_fixed_file.file_status.read_write_when_invalidated)
              adn_cache[sim_id].status = PBM_STATUS_RESTRICTED;
          break;

        case PBM_FILE_SDN:
          pbm_fill_pb_cache_info(sdn_cache,
                                 sim_id,
                                 PBM_USIM_PB_ONE,
                                 record_length,
                                 ADN_KNOWN_REC_LEN,
                                 file_id,
                                 file_type,
                                 pbm_fileinfo_requests[client_ref].filename,
                                 FALSE,
                                 number_of_records);
          break;

        case PBM_FILE_MSISDN:
          pbm_fill_pb_cache_info(msisdn_cache,
                                 sim_id,
                                 PBM_USIM_PB_ONE,
                                 record_length,
                                 ADN_KNOWN_REC_LEN,
                                 file_id,
                                 file_type,
                                 pbm_fileinfo_requests[client_ref].filename,
                                 FALSE,
                                 number_of_records);
          break;

        case PBM_FILE_MBN:
          pbm_fill_pb_cache_info(mbn_cache,
                                 sim_id,
                                 PBM_USIM_PB_ONE,
                                 record_length,
                                 ADN_KNOWN_REC_LEN,
                                 file_id,
                                 file_type,
                                 pbm_fileinfo_requests[client_ref].filename,
                                 FALSE,
                                 number_of_records);
          break;

        case PBM_FILE_FDN:
          pbm_fill_pb_cache_info(fdn_cache,
                                 sim_id,
                                 PBM_USIM_PB_ONE,
                                 record_length,
                                 FDN_KNOWN_REC_LEN,
                                 file_id,
                                 file_type,
                                 pbm_fileinfo_requests[client_ref].filename,
                                 FALSE,
                                 number_of_records);
          break;

        case PBM_FILE_LND:
          pbm_fill_pb_cache_info(lnd_cache,
                                 sim_id,
                                 PBM_USIM_PB_ONE,
                                 record_length,
                                 LND_KNOWN_REC_LEN,
                                 file_id,
                                 file_type,
                                 pbm_fileinfo_requests[client_ref].filename,
                                 FALSE,
                                 number_of_records);
          break;
#ifdef FEATURE_ECC_HANDLING_AT_PBM
       case PBM_FILE_ECC:
          //for any GSM sim no of ecc recs will be always 1
          //and max len would be GSDI_MAX_ECC_LEN
          if (!USE_USIM(sim_id))
          {
            record_length = GSDI_MAX_ECC_LEN;
            number_of_records = 5;
          }
          pbm_fill_pb_cache_info(ecc_sim_cache,
                                 sim_id,
                                 PBM_USIM_PB_ONE,
                                 record_length,
                                 PBM_ECC_MIN_LEN,  // not sure whether we should use PBM_ECC_MAX_LEN or PBM_ECC_MIN_LEN as this is of variable len
                                 file_id,
                                 file_type,
                                 pbm_fileinfo_requests[client_ref].filename,
                                 FALSE,
                                 number_of_records);
          break;
#endif

#ifdef FEATURE_PBM_USIM_SUPPORT
        case PBM_FILE_OCI:
          pbm_fill_pb_cache_info(oci_cache,
                                 sim_id,
                                 PBM_USIM_PB_ONE,
                                 record_length,
                                 OCI_KNOWN_REC_LEN,
                                 file_id,
                                 file_type,
                                 pbm_fileinfo_requests[client_ref].filename,
                                 FALSE,
                                 number_of_records);
          break;

        case PBM_FILE_ICI:
          pbm_fill_pb_cache_info(ici_cache,
                                 sim_id,
                                 PBM_USIM_PB_ONE,
                                 record_length,
                                 ICI_KNOWN_REC_LEN,
                                 file_id,
                                 file_type,
                                 pbm_fileinfo_requests[client_ref].filename,
                                 FALSE,
                                 number_of_records);
          break;

        case PBM_FILE_MBDN:
          pbm_fill_pb_cache_info(mbdn_cache,
                                 sim_id,
                                 PBM_USIM_PB_ONE,
                                 record_length,
                                 ADN_KNOWN_REC_LEN,
                                 file_id,
                                 file_type,
                                 pbm_fileinfo_requests[client_ref].filename,
                                 FALSE,
                                 number_of_records);
          break;
#endif /* FEATURE_PBM_USIM_SUPPORT */

        case PBM_FILE_EXT1:
          if (pbm_fill_pb_extcache_info(&ext1_cache[sim_id], file_id, file_type,
                                      pbm_fileinfo_requests[client_ref].filename,
                                      number_of_records, record_length, sim_id))
          {
            adn_cache[sim_id].extension_cache = &ext1_cache[sim_id];
            lnd_cache[sim_id].extension_cache = &ext1_cache[sim_id];
            mbn_cache[sim_id].extension_cache = &ext1_cache[sim_id];
            if (!USE_USIM(sim_id)) /* For SIM */
              msisdn_cache[sim_id].extension_cache = &ext1_cache[sim_id];
          }
          break;

        case PBM_FILE_EXT2:
          if (pbm_fill_pb_extcache_info(&ext2_cache[sim_id], file_id, file_type,
                                      pbm_fileinfo_requests[client_ref].filename,
                                      number_of_records, record_length, sim_id))
          {
            fdn_cache[sim_id].extension_cache = &ext2_cache[sim_id];
          }
          break;

        case PBM_FILE_EXT3:
          if (pbm_fill_pb_extcache_info(&ext3_cache[sim_id], file_id, file_type,
                                      pbm_fileinfo_requests[client_ref].filename,
                                      number_of_records, record_length, sim_id))
          {
            sdn_cache[sim_id].extension_cache = &ext3_cache[sim_id];
          }
          break;

#ifdef FEATURE_PBM_USIM_SUPPORT
        case PBM_FILE_EXT5:
          if (pbm_fill_pb_extcache_info(&ext5_cache[sim_id], file_id, file_type,
                                      pbm_fileinfo_requests[client_ref].filename,
                                      number_of_records, record_length, sim_id))
          {
            ici_cache[sim_id].extension_cache = &ext5_cache[sim_id];
            oci_cache[sim_id].extension_cache = &ext5_cache[sim_id];
            if (USE_USIM(sim_id)) /* For USIM */
              msisdn_cache[sim_id].extension_cache = &ext5_cache[sim_id];
          }
          break;

        case PBM_FILE_EXT6:
          if (pbm_fill_pb_extcache_info(&ext6_cache[sim_id], file_id, file_type,
                                      pbm_fileinfo_requests[client_ref].filename,
                                      number_of_records, record_length, sim_id))
          {
            if (USE_USIM(sim_id)) /* For USIM */
              mbdn_cache[sim_id].extension_cache = &ext6_cache[sim_id];
          }
          break;

        case PBM_FILE_EMAIL:
        case PBM_FILE_EMAIL1:
#ifdef FEATURE_USIM_1000_PBM
        case PBM_FILE_EMAIL2:
        case PBM_FILE_EMAIL3:
#endif
        case PBM_FILE_SNE:
        case PBM_FILE_SNE1:
#ifdef FEATURE_USIM_1000_PBM
        case PBM_FILE_SNE2:
        case PBM_FILE_SNE3:
#endif
        case PBM_FILE_GRP:
        case PBM_FILE_GRP1:
#ifdef FEATURE_USIM_1000_PBM
        case PBM_FILE_GRP2:
        case PBM_FILE_GRP3:
#endif
        case PBM_FILE_ANR:
        case PBM_FILE_ANRA:
        case PBM_FILE_ANRB:
        case PBM_FILE_ANRC:
        case PBM_FILE_ANR1:
        case PBM_FILE_ANRA1:
        case PBM_FILE_ANRB1:
        case PBM_FILE_ANRC1:
#ifdef FEATURE_USIM_1000_PBM
        case PBM_FILE_ANR2:
        case PBM_FILE_ANRA2:
        case PBM_FILE_ANRB2:
        case PBM_FILE_ANRC2:
        case PBM_FILE_ANR3:
        case PBM_FILE_ANRA3:
        case PBM_FILE_ANRB3:
        case PBM_FILE_ANRC3:
#endif
          /* TODO: record length check */
          if (record_length)
          {
            pb_set = pbm_file_id_to_pb_set(file_id);
            PBM_CHECK_PBSET_BREAK(pb_set);
            usim_cache = pbm_file_id_to_cache(sim_id, file_id); //lint !e826 area too small

            if (usim_cache == NULL)
            {
              PBM_MSG_ERR("NULL usim_cache",0,0,0);
              break;
            }

            usim_cache->file_id = file_id;
            usim_cache->file_type = file_type;
            usim_cache->filename = pbm_fileinfo_requests[client_ref].filename;
            usim_cache->record_length = record_length;
            usim_cache->num_of_records = number_of_records;
            if (usim_cache->mapping_type != GSDI_MAPPING_TYPE_2)
              adn_m_usim_cache[sim_id][pb_set].num_caches++;
          }
          else
          {
            PBM_MSG_ERR("Bad data received from SIM 0x%x 0x%x 0x%x", sim_id,
                pbm_fileinfo_requests[client_ref].filename,
                record_length);
          }
          break;

        case PBM_FILE_IAP:
          pb_set = pbm_file_id_to_pb_set(file_id);
          PBM_CHECK_PBSET_BREAK(pb_set);

          if (record_length)
          {
            m_usim_cache = &adn_m_usim_cache[sim_id][pb_set];
            m_usim_cache->file_id = file_id;
            m_usim_cache->file_type = file_type;
            m_usim_cache->filename = pbm_fileinfo_requests[client_ref].filename;
            m_usim_cache->iap_rec_len = record_length;
            m_usim_cache->iap_num_records = number_of_records;
            m_usim_cache->num_caches += record_length;
          }
          else
          {
            PBM_MSG_ERR("Bad IAP data received from SIM 0x%x 0x%x 0x%x", sim_id,
                pbm_fileinfo_requests[client_ref].filename,
                record_length);
          }
          break;

        case PBM_FILE_IAP1:
#ifdef FEATURE_USIM_1000_PBM
        case PBM_FILE_IAP2:
        case PBM_FILE_IAP3:
#endif
          pb_set = pbm_file_id_to_pb_set(file_id);
          PBM_CHECK_PBSET_BREAK(pb_set);

          if (record_length)
          {
            m_usim_cache = &adn_m_usim_cache[sim_id][pb_set];
            if (adn_m_usim_cache[sim_id][PBM_USIM_PB_ONE].iap_rec_len !=
                  record_length)
            {
              PBM_MSG_ERR("Record length of IAP and IAP %d does not match %d %d",
                          pb_set,
                          adn_m_usim_cache[sim_id][PBM_USIM_PB_ONE].iap_rec_len,
                          record_length);
              break;
            }
            m_usim_cache->file_id = file_id;
            m_usim_cache->file_type = file_type;
            m_usim_cache->filename = pbm_fileinfo_requests[client_ref].filename;
            m_usim_cache->iap_rec_len = record_length;
            m_usim_cache->iap_num_records = number_of_records;
            m_usim_cache->num_caches += record_length;
          }
          else
          {
            PBM_MSG_ERR("Bad IAP-n data received from SIM 0x%x 0x%x 0x%x", sim_id,
                pbm_fileinfo_requests[client_ref].filename,
                record_length);
          }
          break;

        case PBM_FILE_PSC:
          pbm_fill_pb_sync1cache_info(&psc_cache[sim_id], file_id, file_type,
                pbm_fileinfo_requests[client_ref].filename,
                                      1, 4, sim_id);
          break;

        case PBM_FILE_PUID:
          pbm_fill_pb_sync1cache_info(&puid_cache[sim_id], file_id, file_type,
                pbm_fileinfo_requests[client_ref].filename,
                                      1, 2, sim_id);
          break;

        case PBM_FILE_CC:
          pbm_fill_pb_sync1cache_info(&cc_cache[sim_id], file_id, file_type,
                pbm_fileinfo_requests[client_ref].filename,
                                      1, 2, sim_id);
          break;

        case PBM_FILE_PBC:
          if (record_length)
          {
            pbc_cache[sim_id].file_ids[PBM_USIM_PB_ONE] = file_id;
            pbc_cache[sim_id].file_type = file_type;
            pbc_cache[sim_id].filenames[PBM_USIM_PB_ONE] = pbm_fileinfo_requests[client_ref].filename;
            pbc_cache[sim_id].num_of_records = number_of_records;
            pbc_cache[sim_id].records_in_pb_set[PBM_USIM_PB_ONE] = number_of_records;
            pbc_cache[sim_id].num_files = 1;
            pbc_cache[sim_id].record_length = record_length;
          }
          else
          {
            PBM_MSG_ERR("Bad PBC data received from SIM 0x%x 0x%x 0x%x", sim_id,
                pbm_fileinfo_requests[client_ref].filename,
                record_length);
          }
          break;

        case PBM_FILE_PBC1:
#ifdef FEATURE_USIM_1000_PBM
        case PBM_FILE_PBC2:
        case PBM_FILE_PBC3:
#endif
           pbm_fill_pb_sync2cache_info(&pbc_cache[sim_id], file_id, file_type,
                                       pbm_fileinfo_requests[client_ref].filename,
                                       number_of_records, record_length, sim_id);
          break;

        case PBM_FILE_UID:
          pb_set = pbm_file_id_to_pb_set(file_id);
          PBM_CHECK_PBSET_BREAK(pb_set);

          if (record_length)
          {
            uid_cache[sim_id].file_ids[pb_set] = file_id;
            uid_cache[sim_id].file_type = file_type;
            uid_cache[sim_id].filenames[pb_set] =
              pbm_fileinfo_requests[client_ref].filename;
            uid_cache[sim_id].num_of_records = number_of_records;
            uid_cache[sim_id].record_length = record_length;
            uid_cache[sim_id].records_in_pb_set[pb_set] = number_of_records;
            uid_cache[sim_id].num_files = 1;
          }
          else
          {
            PBM_MSG_ERR("Bad UID data received from SIM 0x%x 0x%x 0x%x", sim_id,
                pbm_fileinfo_requests[client_ref].filename,
                record_length);
          }
          break;

        case PBM_FILE_UID1:
#ifdef FEATURE_USIM_1000_PBM
        case PBM_FILE_UID2:
        case PBM_FILE_UID3:
#endif
           pbm_fill_pb_sync2cache_info(&uid_cache[sim_id], file_id, file_type,
                                       pbm_fileinfo_requests[client_ref].filename,
                                       number_of_records, record_length, sim_id);
          break;
#endif /* FEATURE_PBM_USIM_SUPPORT */

        default:
          PBM_MSG_ERR("Unexpected file attributes received SIM %d REQUEST %d!", sim_id, file_id, 0);
          break;
      }
    }
  }
  else
  { /* (status != MMGSDI_SUCCESS) */
    void                  *cache;
    pbm_cache_type_e_type cache_type;

    switch (status)
    {
      case MMGSDI_NOT_FOUND:
        PBM_MSG_HIGH("GSDI error MMGSDI_NOT_FOUND sim 0x%x pbm file %d", sim_id, file_id, 0);
        break;

      case MMGSDI_ACCESS_DENIED:
        PBM_MSG_HIGH("GSDI error MMGSDI_ACCESS_DENIED sim 0x%x pbm file %d", sim_id, file_id, 0);
        break;

      default:
        PBM_MSG_HIGH("GSDI error code 0x%x sim 0x%x pbm file %d", status, sim_id, file_id);
        break;
    }

    cache = pbm_file_id_to_cache(sim_id, file_id);
    cache_type = pbm_file_id_to_cache_type(file_id);

    /* PB caches are the only ones with a status field */
    if (cache_type == PBM_CACHE_TYPE_PB)
    {
      if (cache != NULL)
      {
        /* beware that multiple GSDI files map to the same cache in some cases like ADN */
        if (((PB_CACHE*)cache)[sim_id].num_of_records == 0)
        {
          ((PB_CACHE*)cache)[sim_id].status = PBM_STATUS_NOT_AVAILABLE;
        }
      }
      else
      {
        PBM_MSG_ERR("NULL cache ptr, sim id 0x%x file_id 0x%x", sim_id, file_id, 0);
      }
    }
  }

  /* After we process the results, put the next request in. */
  pbm_make_fileinfo_request(sim_id, (1 + (uint32) file_id));

} /*void pbm_fileinfo_cb(gsdi_cnf... */

/*===========================================================================
FUNCTION PBM_SIM_INIT_COMPLETED

DESCRIPTION
  This function is called during boot after all the records have been read.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
static void pbm_sim_init_completed(pbm_slot_type sim_id)
{
  PBM_VALIDATE_SLOT_RET(sim_id, VOID);

#ifdef FEATURE_PBM_USIM_SUPPORT
  PBM_MEM_FREEIF(uid_cache[sim_id].data_array);
  PBM_MEM_FREEIF(pbc_cache[sim_id].data_array);
#endif /* FEATURE_PBM_USIM_SUPPORT */

  PBM_MSG_HIGH("PBM Finished init for sim %d", sim_id, 0, 0);
  if ((ds_ready_fn != NULL) && (sim_id == PBM_SLOT_1))  //lint !e774 always true
    (*ds_ready_fn)(check_pb_ready(PBM_ADN) == PBM_SUCCESS ? TRUE : FALSE);

  PBM_SIMSLOT_RESET(pbm_slot_initializing, sim_id);

  pbm_gsdi_refresh_complete(sim_id);
  pbm_gsdi_client_init_complete(sim_id);

  pbm_init_info.pbm_sim_init_done[sim_id] = TRUE;

  PBM_NOTIFY(PBM_EVENT_SIM_INIT_DONE, sim_id, sim_id);
}

/*===========================================================================
FUNCTION PBM_SIM_READ_CB

DESCRIPTION
  This function is called during init by GSDI when a record is read in.
  It places the record into the linked list and if there is more to read
  it requests the next record to be read. Once we read the last record, it calls pbm_sim_init_completed to say init is done.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
static void pbm_sim_read_cb(pbm_return_type pbm_returned, pbm_sim_record_s_type *record)
{
  PB_CACHE *cache;
  pbm_device_type pb_id;
  pbm_slot_type sim_id;
  int record_num = 0;

  PBM_CHECK_PTR_RET(record, VOID);

  sim_id = record->sim_id;
  PBM_VALIDATE_SLOT_RET(sim_id, VOID);

  pb_id = pbm_file_id_to_pb_id(sim_id, record->file_id);

  cache = pbm_pb_id_to_cache(pb_id);
  if (cache != NULL)
  {
    if (pbm_returned == PBM_SUCCESS) /* If we can trust the data. */
    {
      if (pbm_cache_record_add(pb_id, &record->data.num_text, record->index))
        {
          record_num = pbm_get_next_used_record(sim_id, record->index);

        if ((record_num <= cache->num_of_records) &&
            ((pbm_returned = pbm_sim_read(record->sim_id,
                                        record->file_id,
                                        record_num,
                                        pbm_sim_read_cb)) == PBM_SUCCESS))
          return;
      }

      if (record_num > cache->num_of_records)
      {
#ifdef FEATURE_PBM_USIM_SUPPORT
        if (cache->m_sync_cache &&
            cache->m_sync_cache->pbc_cache)
        {
          cache->status = PBM_STATUS_SYNC;
          pbm_reset_pbc(sim_id, 1);
          return;
        }
        else
#endif /* FEATURE_PBM_USIM_SUPPORT */
        {
          PBM_SET_PB_CACHE_READY(cache);
        }
      }
      else
      {
        PBM_MSG_ERR("Unable to read next record from SIM card, Error %d after %d of max %d",
            pbm_returned, record->index, cache->num_of_records);

        /* This cache will be unusable, make sure its clear. */
        rex_enter_crit_sect(&pbm_crit_sect);
        pbm_clear_cache(cache);
        rex_leave_crit_sect(&pbm_crit_sect);
        cache->status = PBM_STATUS_NOT_AVAILABLE;
      }
    }
    else
    {
      PBM_MSG_ERR("Unable to read from SIM card, Error %d after %d of max %d",
          pbm_returned, cache->records_in_use, cache->num_of_records);

      /* This cache will be unusable, make sure its clear. */
      rex_enter_crit_sect(&pbm_crit_sect);
      pbm_clear_cache(cache);
      rex_leave_crit_sect(&pbm_crit_sect);
      cache->status = PBM_STATUS_NOT_AVAILABLE;
    }
  }
  else
  {
    PBM_MSG_ERR("Could not find cache for phonebook %d", pb_id, 0, 0);
  }

  /* Move to next file regardless of why we got here. */
  pbm_initialize_index[sim_id]++;

  pbm_init_entries(sim_id);
}

/*===========================================================================
FUNCTION PBM_INIT_ENTRIES

DESCRIPTION
  This function is called after PIN is verified and GSDI is ready for a flood
  of UI requests.

DEPENDENCIES
   pbm_init must be called first.

SIDE EFFECTS
===========================================================================*/
void pbm_init_entries(pbm_slot_type sim_id)
{
  pbm_cache_type_e_type cache_type;
  pbm_file_device_e_type file_id;
  int i;

  /*
  ** OK, Start reading the entries... asynchronously.
   */
  PBM_VALIDATE_SLOT_RET(sim_id, VOID);

  while (pbm_initialize_index[sim_id] < (int) ARR_SIZE(pbm_initialize_list))
  {
    PBM_MSG_HIGH("PBM starts init of PBM file index %d, File id %d", pbm_initialize_index[sim_id], pbm_initialize_list[pbm_initialize_index[sim_id]], 0);

    file_id = pbm_initialize_list[pbm_initialize_index[sim_id]];
    cache_type = pbm_file_id_to_cache_type(file_id);

    /* don't do anything if this is a refresh, and this is not an affected file (group) */
    if (pbm_refresh_init)
    {
      for (i=0; i < pbm_refresh_num_file_devices; i++)
      {
        if (pbm_refresh_pbm_file_devices[i] == file_id)
        {
          /* this is one we want to refresh.  Carry on. */
          break;
        }
      }
      if (i == pbm_refresh_num_file_devices)
      {
        /* this is not one we want to refresh. Move to next file. */
        PBM_MSG_HIGH("Skipping init of PBM file %d", pbm_initialize_index[sim_id],0,0);
        pbm_initialize_index[sim_id]++;
        continue;
      }
    }

    switch (cache_type)
    {
      case PBM_CACHE_TYPE_PB:
#ifdef FEATURE_ECC_HANDLING_AT_PBM
      case PBM_CACHE_TYPE_ECC:
#endif
      {
        /* Read file stored in PB_CACHE */
        PB_CACHE *cache = (PB_CACHE *)pbm_file_id_to_cache(sim_id, file_id); //lint !e826 area too small

        if (cache != NULL)
        {
          if ((cache->status == PBM_STATUS_NOT_READY) &&
              (cache->num_of_records > 0))
          {
            if (PBM_SUCCESS == pbm_seek_unused_records(sim_id, file_id))
              return;
          }
        }
        break;
      }
#ifdef FEATURE_PBM_USIM_SUPPORT
      case PBM_CACHE_TYPE_USIM:
      {
        /* Read file stored in USIM_CACHE */
        PB_USIM_CACHE *usim_cache =
          (PB_USIM_CACHE *)pbm_file_id_to_cache(sim_id, file_id); //lint !e826 area too small

        if (usim_cache != NULL)
        {
          if (usim_cache->num_of_records && adn_cache[sim_id].num_of_records)
          {
            if (PBM_SUCCESS == pbm_seek_unused_records(sim_id, file_id))
              return;
          }
        }
        break;
      }

      case PBM_CACHE_TYPE_INDEX:
      {
        PB_MASTER_USIM_CACHE *m_usim_cache =
          (PB_MASTER_USIM_CACHE *)pbm_file_id_to_cache(sim_id, file_id); //lint !e826 area too small

        if (m_usim_cache != NULL)
        {
          if (m_usim_cache->iap_num_records && adn_cache[sim_id].num_of_records)
          {
            if (PBM_SUCCESS == pbm_seek_unused_records(sim_id, file_id))
              return;
          }
        }
        break;
      }

      case PBM_CACHE_TYPE_SYNC1:
      {
        PB_SYNC1_CACHE *sync1_cache =
          (PB_SYNC1_CACHE *) pbm_file_id_to_cache(sim_id, file_id);

        if (sync1_cache != NULL)
        {
          if (sync1_cache->num_of_records && adn_cache[sim_id].num_of_records)
          {
            /* Don't SEEK for unused records on transparent files */
            if (PBM_SUCCESS == pbm_sync_read(sim_id, file_id, 1, pbm_sync1_read_cb))
              return;
          }
        }
        break;
      }

      case PBM_CACHE_TYPE_SYNC2:
      {
        PB_SYNC2_CACHE *sync2_cache =
          (PB_SYNC2_CACHE *) pbm_file_id_to_cache(sim_id, file_id); //lint !e826 area too small

        if (sync2_cache != NULL)
        {
          if (sync2_cache->num_of_records && adn_cache[sim_id].num_of_records)
          {
            if (PBM_SUCCESS == pbm_seek_unused_records(sim_id, file_id))
              return;
          }
        }
        break;
      }
#endif /* FEATURE_PBM_USIM_SUPPORT */
      default:
        PBM_MSG_ERR("Unknown file id %d in sim %d", file_id, sim_id, 0);
        break;
    }

    /* Didn't initialize this file, move onto next. */
    PBM_MSG_HIGH("PBM skipping init of PBM file index %d, File id %d", pbm_initialize_index[sim_id], pbm_initialize_list[pbm_initialize_index[sim_id]], 0);
    pbm_initialize_index[sim_id]++;
  }

  /* Memory allocated for the last file's SEEK data hasn't been freed */
  pbm_free_seek_data(sim_id);

  pbm_sim_init_completed(sim_id);
}

/*===========================================================================
FUNCTION PBM_RECORD_WRITE_TO_SIM

DESCRIPTION
  Adds a record to UIM. This is an internal function and should be called only
  from within PBM task.

DEPENDENCIES
  The caller should free write cmd structure in case of an error.

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_record_write_to_sim(pbm_write_record_s_type *write)
{
  int buffer_index = -1;
  PB_CACHE *pb_cache;
  pbm_return_type ret;
  pbm_pb_set_type pb_set;

  PBM_CHECK_PTR_RET(write, PBM_ERROR);

  pb_cache = pbm_pb_id_to_cache(pbm_record_id_to_phonebook(write->rec_id));
  PBM_CHECK_PTR_RET(pb_cache, PBM_ERROR);

  pb_set = pbm_record_id_to_pb_set(write->rec_id);
  PBM_CHECK_PBSET_RET(pb_set, PBM_ERROR);

  buffer_index = pbm_alloc_record();
  if (buffer_index < 0)
  {
    PBM_MSG_ERR("No pbm buffers %d", buffer_index, 0, 0);
    pbm_list_recover(write, FALSE);
    return PBM_ERROR;
  }

  pbm_buffers[buffer_index].write = write;
  pbm_buffers[buffer_index].state = PBM_WRITE_NOT_INIT;

#ifdef FEATURE_PBM_USIM_DEBUG
  pbm_buffers[buffer_index].debug_num_writes = 0;
#endif

  if ((ret = pbm_uim_write((uint32)buffer_index)) != PBM_SUCCESS)
  {
    PBM_MSG_ERR("Unable to write record %d to UIM %d %d", write->rec_id, ret, buffer_index);
    pbm_list_recover(write, FALSE);
    pbm_free_record(buffer_index);
    return ret;
  }

  return PBM_SUCCESS;
}

#ifdef FEATURE_STA_PBM
/*===========================================================================
FUNCTION PBM_INIT_FILEINFO

DESCRIPTION
  Through GSDI will return the total number of records, and the number of record
  in use for the specified file.

DEPENDENCIES
  card_apps must be previously set.

SIDE EFFECTS
  Globals describing the world according to this SIM are set.
===========================================================================*/
void pbm_init_file_info(void) {
  return;
}
#endif /* FEATURE_STA_PBM */

/*===========================================================================
FUNCTION PBM_UIM_WRITE

DESCRIPTION

PARAMETERS:

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_uim_write (uint32 buffer_index)
{
  pbm_write_record_s_type *write;
  pbm_async_gsdi_buffer   *buffer;
  pbm_return_type          ret = PBM_SUCCESS;
  pbm_slot_type            sim_id;
  pbm_file_device_e_type   file_id;
  pbm_pb_set_type          pb_set;

  if (buffer_index >= PBM_ASYNC_BUFFERS || !pbm_buffers[buffer_index].in_use)
  {
    PBM_MSG_ERR("Invalid buffer index %d", buffer_index, 0, 0);
    return PBM_ERROR;
  }

#ifdef FEATURE_PBM_USIM_DEBUG
  if (pbm_buffers[buffer_index].debug_num_writes++ > 1000)
  {
    PBM_MSG_ERR("Loop in execution of code !!", 0, 0, 0);
    return PBM_ERROR;
  }
#endif

  buffer = &pbm_buffers[buffer_index];
  write = buffer->write;
  sim_id =  pbm_pb_id_to_sim_id(pbm_record_id_to_phonebook(write->rec_id));
  PBM_VALIDATE_SLOT_RET(sim_id, PBM_ERROR);

  file_id = pbm_record_id_to_file_id(write->rec_id);
  pb_set = pbm_record_id_to_pb_set(write->rec_id);
  PBM_CHECK_PBSET_RET(pb_set, PBM_ERROR);

  buffer->m_file_id = file_id;

  while(buffer->state != PBM_WRITE_DONE)
  {
    switch (buffer->state)
    {
      case PBM_WRITE_NOT_INIT:
        buffer->record.sim_id = sim_id;
        buffer->record.file_id = file_id;
        buffer->record.index = pbm_record_id_to_location(write->rec_id);
        buffer->findex = 0;

#ifdef FEATURE_PBM_USIM_SUPPORT
        /* Allocate memory for caches in use */
        PBM_MEM_FREEIF(buffer->caches_in_use);
        PBM_MEM_ALLOC(buffer->caches_in_use,
                   ((uint32) sizeof(boolean)) *
                   ((uint32) adn_m_usim_cache[sim_id][pb_set].num_caches + 1));
        PBM_CHECK_PTR_RET(buffer->caches_in_use, PBM_ERROR_MEM_FULL);

        /* Initialize to FALSE */
        memset(buffer->caches_in_use, 0,
                   ((uint32) sizeof(boolean)) *
                   ((uint32) adn_m_usim_cache[sim_id][pb_set].num_caches + 1));

        pbm_group_fields(write->prev_rec);
        pbm_group_fields(write->curr_rec);

        if (!USE_USIM(sim_id) ||
            ((file_id != PBM_FILE_ADN) && (file_id != PBM_FILE_ADN1)
#ifdef FEATURE_USIM_1000_PBM
             && (file_id != PBM_FILE_ADN2) && (file_id != PBM_FILE_ADN3)
#endif
             ))
#endif /* FEATURE_PBM_USIM_SUPPORT */
        {
          /* non-USIM record / non-ADN record */
          if (pbm_uim_write_master(buffer_index, &ret) || ret != PBM_SUCCESS)
          {
            PBM_MEM_FREEIF(buffer->caches_in_use);
            buffer->state = PBM_WRITE_DONE;
            return ret;
          }
        }
#ifdef FEATURE_PBM_USIM_SUPPORT
        buffer->state = PBM_WRITE_SYNC;
        break;

      case PBM_WRITE_SYNC:
        if (pbm_uim_write_sync(buffer_index, &ret) || ret != PBM_SUCCESS)
          return ret;

        buffer->findex = 0;
        buffer->state = PBM_WRITE_PREV_USIM;
        break;

      case PBM_WRITE_PREV_USIM:
        if (pbm_uim_write_usim(buffer_index, &ret) || ret != PBM_SUCCESS)
          return ret;

        buffer->findex = 0;
        buffer->state = PBM_WRITE_MASTER;

        /* we're done getting rid of the old record.  Mark all of the caches
         * in use to false.  The new record can use any of them. */
        memset(buffer->caches_in_use, 0,
                   ((uint32) sizeof(boolean)) *
                   ((uint32) adn_m_usim_cache[sim_id][pb_set].num_caches + 1));
        break;

      case PBM_WRITE_MASTER:
        if (pbm_uim_write_master(buffer_index, &ret) || ret != PBM_SUCCESS)
          return ret;

        buffer->findex = 0;
        buffer->state = PBM_WRITE_CURR_USIM;
        break;

      case PBM_WRITE_CURR_USIM:
        if (pbm_uim_write_usim(buffer_index, &ret) || ret != PBM_SUCCESS)
          return ret;

        PBM_MEM_FREEIF(buffer->caches_in_use);

        buffer->findex = 0;
        if (adn_m_sync_cache[sim_id].reset_pbc)
          buffer->state = PBM_WRITE_RESETPBC;
        else
          buffer->state = PBM_WRITE_DONE;
        break;

      case PBM_WRITE_RESETPBC:
        if (pbm_uim_write_pbc(buffer_index, &ret) || ret != PBM_SUCCESS)
          return ret;

        buffer->findex = 0;
#endif /* FEATURE_PBM_USIM_SUPPORT */
        buffer->state = PBM_WRITE_DONE;
        break;

      default:
        break;
    }
  }

  pbm_uim_write_cb(buffer_index, PBM_SUCCESS);

  return PBM_SUCCESS;
}

/*===========================================================================
FUNCTION PBM_WRITE_MASTER

DESCRIPTION

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
boolean pbm_uim_write_master(uint32 buffer_index, pbm_return_type *ret)
{
  pbm_async_gsdi_buffer *buffer;
  boolean bret = TRUE;

  PBM_CHECK_PTR_RET(ret, FALSE);
  *ret = PBM_ERROR;

  buffer = &pbm_buffers[buffer_index];
  PBM_CHECK_PTR_RET(buffer->write, FALSE);

  PBM_VALIDATE_SLOT_RET(buffer->record.sim_id, FALSE);

#ifdef FEATURE_PBM_USIM_SUPPORT
  if (adn_m_sync_cache[buffer->record.sim_id].reset_pbc || buffer->findex)
  {
    bret = FALSE;
    *ret = PBM_SUCCESS;
  }
  else
#endif /* FEATURE_PBM_USIM_SUPPORT */
  {
    buffer->record.file_id = buffer->m_file_id;
    buffer->record.index = pbm_record_id_to_location(buffer->write->rec_id);
    buffer->findex_delta = 1;

    if ((*ret = pbm_sim_write(buffer_index)) != PBM_SUCCESS)
    {
      PBM_MSG_ERR("Unable to add record %d to SIM %d %d", buffer->write->rec_id, *ret, buffer_index);
      bret = FALSE;
    }
    else
    {
      bret = TRUE;
    }
  }

  return bret;
}

#ifdef FEATURE_PBM_USIM_SUPPORT
/*===========================================================================
FUNCTION PBM_UIM_WRITE_SYNC

DESCRIPTION

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
boolean pbm_uim_write_sync(uint32 buffer_index, pbm_return_type *ret)
{
  pbm_async_gsdi_buffer *buffer = &pbm_buffers[buffer_index];
  pbm_write_record_s_type *write;
  pbm_slot_type sim_id;
  pbm_file_device_e_type file_id[] = {PBM_FILE_PSC, PBM_FILE_CC, PBM_FILE_PUID,
                                      PBM_FILE_UID};

  PBM_CHECK_PTR_RET(ret, FALSE);
  *ret = PBM_ERROR;

  write = buffer->write;
  sim_id = buffer->record.sim_id;
  PBM_VALIDATE_SLOT_RET(sim_id, FALSE);

  if (!adn_m_sync_cache[sim_id].sync_present)
  {
    *ret = PBM_SUCCESS;
    return FALSE;
  }

  while (buffer->findex < ARR_SIZE(file_id))
  {
    switch (file_id[buffer->findex])
    {
      case PBM_FILE_PUID:
      case PBM_FILE_CC:
        buffer->record.file_id = file_id[buffer->findex];
        buffer->record.index = 1;
        buffer->findex_delta = 1;
        *ret = pbm_sync_write_sync1(buffer_index);
        return (*ret == PBM_SUCCESS ? TRUE : FALSE);

      case PBM_FILE_PSC:
        if (!write->sync.update_psc)
        {
          buffer->findex++;
        }
        else
        {
          buffer->record.file_id = PBM_FILE_PSC;
          buffer->record.index = 1;
          buffer->findex_delta = 1;
          *ret = pbm_sync_write_sync1(buffer_index);
          return (*ret == PBM_SUCCESS ? TRUE : FALSE);
        }
        break;

      case PBM_FILE_UID:
        if (!write->sync.update_psc)
        {
          buffer->record.file_id = PBM_FILE_UID;
          buffer->record.index = pbm_record_id_to_location(write->rec_id);
          buffer->findex_delta = 1;
          *ret = pbm_sync_write_sync2(buffer->write->curr_rec->unique_id,
                                      buffer_index);
          return (*ret == PBM_SUCCESS ? TRUE : FALSE);
        }
        else
        {
          uint16 data;
          uint16 index;
          PB_CACHE *pb_cache = &adn_cache[sim_id];

          if (buffer->record.file_id != PBM_FILE_UID)
            index = 1;
          else
            index = (uint16) buffer->record.index+1;

          /* Find first non-NULL record */
          while (index <= pb_cache->num_of_records)
          {
            if (pb_cache->pb_cache_array[index])
              break;
            else
              index++;
          }

          /* Check if we are done */
          if (index > pb_cache->num_of_records)
          {
            buffer->findex++;
            break;
          }

          buffer->findex_delta = 0;
          buffer->record.index = index;
          buffer->record.file_id = PBM_FILE_UID;

          data = pb_cache->pb_cache_array[buffer->record.index]->unique_id;
          *ret =  pbm_sync_write_sync2(data, buffer_index);
          return (*ret == PBM_SUCCESS ? TRUE : FALSE);
        }

      default:
        PBM_MSG_ERR("Unexpected file id %d", file_id[buffer->findex], 0, 0);
        break;
    }
  }

  if (adn_cache[sim_id].status == PBM_STATUS_SYNC)
    PBM_SET_PB_CACHE_READY(&adn_cache[sim_id]);

  *ret = PBM_SUCCESS;

  return FALSE;
}

/*===========================================================================
FUNCTION PBM_UIM_WRITE_PBC

DESCRIPTION

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
boolean pbm_uim_write_pbc(uint32 buffer_index, pbm_return_type *ret)
{
  pbm_async_gsdi_buffer *buffer = &pbm_buffers[buffer_index];
  pbm_write_record_s_type *write;

  PBM_CHECK_PTR_RET(ret, FALSE);
  *ret = PBM_ERROR;

  if (buffer->findex)
  {
    *ret = PBM_SUCCESS;
    return FALSE;
  }

  write = buffer->write;

  buffer->record.file_id = PBM_FILE_PBC;
  buffer->record.index = pbm_record_id_to_location(write->rec_id);
  buffer->findex_delta = 1;
  *ret = pbm_sync_write_sync2(0, buffer_index);
  return (*ret == PBM_SUCCESS ? TRUE : FALSE);
}

/*===========================================================================
FUNCTION pbm_uim_write_usim

DESCRIPTION

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
boolean pbm_uim_write_usim(uint32 buffer_index, pbm_return_type *ret)
{
  int i;
  pbm_addr_cache_s_type *record = NULL;
  pbm_async_gsdi_buffer *buffer;
  uint8 *index_array = NULL;

  PBM_CHECK_PTR_RET(ret, FALSE);
  *ret = PBM_ERROR;

  buffer = &pbm_buffers[buffer_index];
  PBM_CHECK_PTR_RET(buffer->write, FALSE);

  if (buffer->state == PBM_WRITE_PREV_USIM)
  {
    record = buffer->write->prev_rec;
    index_array = buffer->write->prev_usim_index;
  }
  else if (buffer->state == PBM_WRITE_CURR_USIM)
  {
    record = buffer->write->curr_rec;
    index_array = buffer->write->curr_usim_index;
  }

  if (record)
  {
    for (i = buffer->findex; i < record->num_fields; i++)
    {
      if (pbm_is_usim_field(record->fields[i].field_id))
      {
        buffer->findex = (uint16)i;
        return pbm_usim_write(buffer_index, ret);
      }
    }

    buffer->findex = record->num_fields;

    /* Add IAP entry */
    if (index_array)
    {
      pbm_pb_set_type pb_set = pbm_record_id_to_pb_set(buffer->write->rec_id);

      PBM_CHECK_PBSET_RET(pb_set, FALSE);

      PBM_VALIDATE_SLOT_RET(buffer->record.sim_id, FALSE);

      /* Add iap entry if IAP file is present and we have not already done it */
      if (adn_m_usim_cache[buffer->record.sim_id][pb_set].iap_num_records &&
          (pbm_file_id_to_cache_type(buffer->record.file_id) !=
                                                        PBM_CACHE_TYPE_INDEX))
      {
        if (pbm_iap_write(buffer_index) != PBM_SUCCESS)
        {
          PBM_MSG_ERR("Unable to update iap file %d", *ret, 0, 0);
          return FALSE;
        }
        else
        {
          *ret = PBM_SUCCESS;
          return TRUE;
        }
      }
    }
  }

  *ret = PBM_SUCCESS;

  return FALSE;
}
#endif /* FEATURE_PBM_USIM_SUPPORT */

/*===========================================================================
FUNCTION PBM_UIM_WRITE_CB

DESCRIPTION
  This function is called in response to a pbm_uim_write function after
  all processing is done. The input parameter 'ret' specifes whether the
  operation succeeded or not.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
void pbm_uim_write_cb(uint32 buffer_index, pbm_return_type ret)
{
  pbm_write_record_s_type *write;
  boolean usim_fields_present = FALSE;

  if (buffer_index >= PBM_ASYNC_BUFFERS || !pbm_buffers[buffer_index].in_use)
  {
    PBM_MSG_ERR("Invalid buffer_index %d", buffer_index, 0, 0);
    (void) rex_set_sigs(&pbm_tcb, PBM_CMD_COMPLETE_SIG);
    return;
  }

  write = pbm_buffers[buffer_index].write;
  PBM_CHECK_PTR_RET(write, VOID);

  if (ret == PBM_SUCCESS)
  {
    pbm_list_update(write);
  }
  else
  {
#ifdef FEATURE_PBM_USIM_SUPPORT
    if ((write->curr_rec && pbm_is_usim_required(write->rec_id,
                                  write->curr_rec->fields,
                                  write->curr_rec->num_fields)) ||
        (write->prev_rec && pbm_is_usim_required(write->rec_id,
                                  write->prev_rec->fields,
                                  write->prev_rec->num_fields)))
    {
      usim_fields_present = TRUE;
    }
#endif /* FEATURE_PBM_USIM_SUPPORT */
    /* For now, if there is an error in USIM write, the cache entry is deleted.
     * TODO: A record that reflects the contents of USIM at the point
     * of error can be built from the values in USIM cache. */
    pbm_list_recover(write, usim_fields_present);
  }

  pbm_write_notify(write, ret);
  pbm_free_record((int)buffer_index);

  pbm_write_cmd_free(write);
}

/*===========================================================================
FUNCTION PBM_SIM_WRITE

DESCRIPTION

PARAMETERS:
  record.index
  record.sim_id
  record.file_id
  write

DEPENDENCIES
  Cache recovery should be done outside this function if write fails.

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_sim_write (uint32 buffer_index)
{
  PB_CACHE                *cache;
  pbm_device_type          pb_id;
  pbm_file_device_e_type   file_id;
  pbm_slot_type            sim_id;
  pbm_write_record_s_type *write;
  pbm_num_text_record_s_type *record;
  boolean                  delete_flag;
  byte                     buffer[PBM_MAX_NUM_LENGTH];
  mmgsdi_file_enum_type    filename;
  pbm_return_type          ret_val;
  int                      rec_num_in_file;

  write = pbm_buffers[buffer_index].write;
  PBM_CHECK_PTR_RET(write, PBM_ERROR);

  record = &pbm_buffers[buffer_index].record.data.num_text;

  /* Fill SIM data */
  delete_flag = (write->curr_rec && write->curr_rec->num_fields) ? FALSE : TRUE;
  if (delete_flag)
  {
    memset(record, 0, sizeof(pbm_record_s_type));
    record->index = pbm_buffers[buffer_index].record.index;
  }
  else
  {
    pbm_build_num_text_record_from_fields(write->curr_rec->fields,
                                 write->curr_rec->num_fields,
                                 pbm_buffers[buffer_index].record.index,
                                 record);
  }

  sim_id = pbm_buffers[buffer_index].record.sim_id;
  PBM_VALIDATE_SLOT_RET(sim_id, PBM_ERROR);

  file_id = pbm_buffers[buffer_index].record.file_id;

  pb_id = pbm_record_id_to_phonebook(write->rec_id);
  cache = pbm_pb_id_to_cache(pb_id);
  if (!cache)
  {
    PBM_MSG_ERR("No cache pb_id = %d", pb_id, 0, 0);
    return PBM_ERROR;
  }

  /* Convert the incoming string to bcd format */
  if (!pbm_ascii_to_bcd((byte *)record->number, record->type, buffer))
  {
    PBM_MSG_ERR("Could not convert ascii->bcd %30s %d %30s", record->number, record->type, buffer);
    return PBM_ERROR;
  }

  /* buffer contains the bcd in the format    */
  /* byte  0      1      2 thru buffer[0]     */
  /*     <len><ton/npi><bcd packed number>    */
  /* Note that length is the length including */
  /* the TON/NPI field.                       */

  /* Do we need an extension record?  */
  filename = pbm_pb_record_index_to_filename(cache, record->index);
  rec_num_in_file = pbm_pb_record_index_to_fileloc(cache, record->index);
  if ((buffer[0] - 1) > PBM_SIM_DIALING_NUMBER_LEN)
  {
    return pbm_create_ext_record(buffer_index, cache->extension_cache,
                                 buffer, (uint16)rec_num_in_file, filename);
  }
  /* see if we used to need one, but no longer */
  else if (pbm_check_extension_record(cache->extension_cache, rec_num_in_file, filename))
  {
    return pbm_clear_ext_record(buffer_index, cache->extension_cache,
                                (uint16)rec_num_in_file, filename);
  }
  else
  {
    pbm_buffers[buffer_index].extension = 0xff;
    ret_val = pbm_create_sim_record(sim_id, file_id, record, (int)buffer_index);
    if (ret_val != PBM_SUCCESS)
    {
      PBM_MSG_ERR("Error %d from pbm_create_sim_record", ret_val, 0, 0);
      return ret_val;
    }
  }

  return PBM_SUCCESS;
}

/*===========================================================================
FUNCTION PBM_SIM_READ

DESCRIPTION
  Reads a specified record to phone book. This function is used for any file
  that is directly available to PBM clients for read/write. This means the
  record will be read to a PB_CACHE. Ex: ADN, FDN, SDN etc.

  To read a USIM record, pbm_usim_read() should be used.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_sim_read (pbm_slot_type sim_id,
                              pbm_file_device_e_type file_id,
                              int record_num,
                              void (*proc_func) ( pbm_return_type,
                                                  pbm_sim_record_s_type *))

{
  int buffer_index;
  PB_CACHE *cache;
  mmgsdi_file_enum_type filename;
  int index;

#ifdef FEATURE_ECC_HANDLING_AT_PBM
  //proc fun check is not required hence we do all the processing
  //at pbm_gsdi_ecc_read_cb itself
  if (file_id != PBM_FILE_ECC)
#endif
  {
    if (proc_func == NULL)
      return PBM_ERROR;
  }

  if (pbm_file_id_to_cache_type(file_id) != PBM_CACHE_TYPE_PB
#ifdef FEATURE_ECC_HANDLING_AT_PBM
       && pbm_file_id_to_cache_type(file_id) != PBM_CACHE_TYPE_ECC
#endif
     )
  {
    PBM_MSG_ERR("Wrong function (pbm_sim_read) used to read file %d", file_id,
                0, 0);
    return PBM_ERROR;
  }

  PBM_VALIDATE_SLOT_RET(sim_id, PBM_ERROR);

  cache = (PB_CACHE *) pbm_file_id_to_cache(sim_id, file_id); /* What cache. */ //lint !e826 area too small
  PBM_CHECK_PTR_RET(cache, PBM_ERROR);

  filename = pbm_pb_record_index_to_filename(cache, record_num);
  index = pbm_pb_record_index_to_fileloc(cache, record_num);

  if (filename == MMGSDI_NO_FILE_ENUM || index <= 0)
  {
    PBM_MSG_ERR("Invalid cache/index %d %d %d",
                file_id, record_num, cache->num_files);
    return PBM_ERROR;
  }

  buffer_index = pbm_alloc_record();

  if (buffer_index < 0)
    return PBM_ERROR;

  pbm_buffers[buffer_index].proc_func = proc_func;
  pbm_buffers[buffer_index].record.index = record_num;
  pbm_buffers[buffer_index].record.file_id = file_id;
  pbm_buffers[buffer_index].record.sim_id = sim_id;
  pbm_buffers[buffer_index].gsdi_resp_type = PBM_MMGSDI_READ_CB;
  if (pbm_send_read_request(sim_id, file_id, filename,
                            index, buffer_index, pbm_gsdi_async_cb))
  {
    return PBM_SUCCESS;
  }

  PBM_MSG_ERR("Could not issue read request", 0, 0, 0);
  pbm_free_record(buffer_index);
  return PBM_ERROR;
}

#ifdef FEATURE_PBM_USIM_SUPPORT
/*===========================================================================
FUNCTION PBM_USIM_READ_CB

DESCRIPTION
  This function is called during init by GSDI when a USIM record is read in.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
void pbm_usim_read_cb(pbm_return_type pbm_returned, pbm_sim_record_s_type *record)
{
  PB_USIM_CACHE *usim_cache;
  pbm_slot_type sim_id;
  pbm_file_device_e_type file_id;
  int record_num = 0;

  PBM_CHECK_PTR_RET(record, VOID);

  sim_id = record->sim_id;
  PBM_VALIDATE_SLOT_RET(sim_id, VOID);

  file_id = record->file_id;

  usim_cache = pbm_file_id_to_cache(sim_id, file_id); //lint !e826 area too small
  if (usim_cache != NULL)
  {
    if (pbm_returned == PBM_SUCCESS) /* If we can trust the data. */
    {
      if  (pbm_usim_cache_record_add(usim_cache, record))
      {
        record_num = pbm_get_next_used_record(sim_id, record->index);

        if ((record_num <= usim_cache->num_of_records) &&
            ((pbm_returned = pbm_usim_read(record->sim_id,
                                     record->file_id,
                                     record_num,
                                     pbm_usim_read_cb)) == PBM_SUCCESS))
          return;
      }

      if (record_num <= usim_cache->num_of_records)
      {
        PBM_MSG_ERR("Unable to read next record from USIM card, Error %d after %d of max %d",
            pbm_returned, record->index, usim_cache->num_of_records);

        /* This cache will be unusable, make sure its clear. */
        rex_enter_crit_sect(&pbm_crit_sect);
        pbm_clear_usim_cache(usim_cache);
        rex_leave_crit_sect(&pbm_crit_sect);
      }
    }
    else
    {
      PBM_MSG_ERR("Unable to read from SIM card, Error %d after %d of max %d",
          pbm_returned, usim_cache->records_in_use, usim_cache->num_of_records);

      /* This cache will be unusable, make sure its clear. */
      rex_enter_crit_sect(&pbm_crit_sect);
      pbm_clear_usim_cache(usim_cache);
      rex_leave_crit_sect(&pbm_crit_sect);
    }

    /* Update the field info structure dynamically */
    if (usim_cache->num_of_records)
    {
      int n, i;
      pbm_field_id_e_type field_id =
                            pbm_file_id_to_field_id(usim_cache->file_id);
      pbm_g_field_info_s_type *field_info = &g_field_info[ADN_GROUP];

      n = ((field_id == PBM_FIELD_GRPNAME_ID) ? usim_cache->record_length : 1);
      for ( i = 0; i < field_info->num_fields; i++)
        if (field_info->info[i].field_id == field_id)
          field_info->info[i].num_items += n;
    }
  }
  else
  {
    PBM_MSG_ERR("Could not find cache for file %d", file_id, 0, 0);
  }

  /* Move to next file regardless of why we got here. */
  pbm_initialize_index[sim_id]++;

  pbm_init_entries(sim_id);
}

/*===========================================================================
FUNCTION PBM_SYNC1_READ_CB

DESCRIPTION
  This function is called during init by GSDI when a USIM record is read in.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
void pbm_sync1_read_cb(pbm_return_type pbm_returned, pbm_sim_record_s_type *record)
{
  PB_SYNC1_CACHE *sync1_cache;
  pbm_slot_type sim_id;
  pbm_file_device_e_type file_id;

  PBM_CHECK_PTR_RET(record, VOID);

  sim_id = record->sim_id;
  PBM_VALIDATE_SLOT_RET(sim_id, VOID);

  file_id = record->file_id;

  sync1_cache = pbm_file_id_to_cache(sim_id, file_id);
  if (sync1_cache != NULL)
  {
    if (pbm_returned == PBM_SUCCESS) /* If we can trust the data. */
    {
      switch (file_id)
      {
        case PBM_FILE_PSC:
          psc_cache[sim_id].data.psc = record->data.dword;
          adn_m_sync_cache[sim_id].psc_cache = &psc_cache[sim_id];
          break;

        case PBM_FILE_PUID:
          puid_cache[sim_id].data.puid = record->data.word;
          adn_m_sync_cache[sim_id].puid_cache = &puid_cache[sim_id];
          break;

        case PBM_FILE_CC:
          cc_cache[sim_id].data.cc = record->data.word;
          adn_m_sync_cache[sim_id].cc_cache = &cc_cache[sim_id];
          break;

        default:
          PBM_MSG_ERR("Unexpected file id %d", file_id, 0, 0);
          break;
      }
    }
    else
    {
      PBM_MSG_ERR("Unable to read from SIM card, Error %d ",
                  pbm_returned, 0, 0);
    }
  }
  else
  {
    PBM_MSG_ERR("Could not find cache for file %d", file_id, 0, 0);
  }

  /* Move to next file regardless of why we got here. */
  pbm_initialize_index[sim_id]++;

  pbm_init_entries(sim_id);
}

/*===========================================================================
FUNCTION PBM_SYNC2_READ_CB

DESCRIPTION
  This function is called during init by GSDI when a USIM record is read in.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
void pbm_sync2_read_cb(pbm_return_type pbm_returned, pbm_sim_record_s_type *record)
{
  PB_SYNC2_CACHE *sync2_cache;
  pbm_slot_type sim_id;
  int index;
  pbm_file_device_e_type file_id;
  int record_num = 0;

  PBM_CHECK_PTR_RET(record, VOID);

  sim_id = record->sim_id;
  PBM_VALIDATE_SLOT_RET(sim_id, VOID);

  file_id = record->file_id;
  index = record->index;

  sync2_cache = pbm_file_id_to_cache(sim_id, file_id); //lint !e826 area too small
  if (sync2_cache != NULL)
  {
    if (pbm_returned == PBM_SUCCESS) /* If we can trust the data. */
    {
      switch (file_id)
      {
        case PBM_FILE_UID:
          uid_cache[sim_id].data_array[index] = record->data.word;
          break;

        case PBM_FILE_PBC:
          pbc_cache[sim_id].data_array[index] = record->data.word;
          break;

        default:
          PBM_MSG_ERR("Unexpected file id %d", file_id, 0, 0);
          break;
      }

      record_num = pbm_get_next_used_record(sim_id, record->index);

      if  ((record_num <= sync2_cache->num_of_records) &&
           ((pbm_returned = pbm_sync_read(record->sim_id,
                                       record->file_id,
                                       record_num,
                                       pbm_sync2_read_cb)) == PBM_SUCCESS))
          return;

      if (record_num > sync2_cache->num_of_records)
      {
        switch (file_id)
        {
          case PBM_FILE_PBC:
            adn_m_sync_cache[sim_id].pbc_cache = sync2_cache;
            adn_cache[sim_id].m_sync_cache = &adn_m_sync_cache[sim_id];
            break;

          case PBM_FILE_UID:
            adn_m_sync_cache[sim_id].uid_cache = sync2_cache;
            if (adn_m_sync_cache[sim_id].psc_cache &&
                adn_m_sync_cache[sim_id].cc_cache &&
                adn_m_sync_cache[sim_id].puid_cache &&
                adn_m_sync_cache[sim_id].uid_cache)
            {
              adn_cache[sim_id].m_sync_cache = &adn_m_sync_cache[sim_id];
              adn_cache[sim_id].m_sync_cache->sync_present = TRUE;
            }
            break;

          default:
            break;
        }
      }
      else
      {
        PBM_MSG_ERR("Unable to read next record from USIM card, Error %d after %d of max %d",
            pbm_returned, record->index, sync2_cache->num_of_records);

        /* This cache will be unusable, make sure its clear. */
        rex_enter_crit_sect(&pbm_crit_sect);
        pbm_clear_sync2_cache(sync2_cache);
        rex_leave_crit_sect(&pbm_crit_sect);
      }
    }
    else
    {
      PBM_MSG_ERR("Unable to read from SIM card, Error %d after %d of max %d",
          pbm_returned, record->index, sync2_cache->num_of_records);

      /* This cache will be unusable, make sure its clear. */
      rex_enter_crit_sect(&pbm_crit_sect);
      pbm_clear_sync2_cache(sync2_cache);
      rex_leave_crit_sect(&pbm_crit_sect);
    }
  }
  else
  {
    PBM_MSG_ERR("Could not find cache for file %d", file_id, 0, 0);
  }

  /* Move to next file regardless of why we got here. */
  pbm_initialize_index[sim_id]++;

  pbm_init_entries(sim_id);
}

/*===========================================================================
FUNCTION PBM_INDEX_READ_CB

DESCRIPTION
  This function is called during init by GSDI when a USIM record is read in.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
void pbm_index_read_cb(pbm_return_type pbm_returned, pbm_sim_record_s_type *record)
{
  PB_MASTER_USIM_CACHE *m_usim_cache;
  PB_USIM_CACHE *usim_cache;
  pbm_slot_type sim_id;
  pbm_file_device_e_type file_id;
  int i;
  int record_num = 0;

  PBM_CHECK_PTR_RET(record, VOID);

  sim_id = record->sim_id;
  PBM_VALIDATE_SLOT_RET(sim_id, VOID);

  file_id = record->file_id;

  m_usim_cache = pbm_file_id_to_cache(sim_id, file_id); //lint !e826 area too small
  if (m_usim_cache != NULL)
  {
    if (pbm_returned == PBM_SUCCESS) /* If we can trust the data. */
    {

      /* Check if record matches master index,
       * otherwise do not use that USIM field */
      for (i = 1; i <= m_usim_cache->num_caches; i++)
      {
        usim_cache = m_usim_cache->usim_cache_array[i];
        if (usim_cache && usim_cache->mapping_type == GSDI_MAPPING_TYPE_2 &&
            (m_usim_cache->index_array[i][record->index] !=
             record->data.iap.index[i-1]))
        {
          PBM_MSG_ERR("Information in file %d and IAP record for index %d does not match", usim_cache->file_id, record->index, 0);

          /* USIM spec says to update pointer (ie IAP) before data, so we don't have the data that goes here.  Delete it. */
          if (m_usim_cache->index_array[i][record->index] != 0)
          {
            (void) pbm_usim_cache_delete(usim_cache,  m_usim_cache->index_array[i][record->index],TRUE);
            m_usim_cache->index_array[i][record->index] = 0;
          }
        }
      }

      record_num = pbm_get_next_used_record(sim_id, record->index);

      if  ((record_num <= m_usim_cache->iap_num_records) &&
           ((pbm_returned = pbm_index_read(record->sim_id,
                                       record->file_id,
                                       record_num,
                                       pbm_index_read_cb)) == PBM_SUCCESS))
          return;

      if (record_num <= m_usim_cache->iap_num_records)
      {
        PBM_MSG_ERR("Unable to read next record from USIM card, Error %d after %d of max %d",
            pbm_returned, record->index, m_usim_cache->iap_num_records);

        /* This cache will be unusable, make sure its clear. */
        rex_enter_crit_sect(&pbm_crit_sect);
        pbm_clear_index_cache(m_usim_cache);
        rex_leave_crit_sect(&pbm_crit_sect);
      }
    }
    else
    {
      PBM_MSG_ERR("Unable to read next record from USIM card, Error %d after %d of max %d",
            pbm_returned, record->index, m_usim_cache->iap_num_records);

      /* This cache will be unusable, make sure its clear. */
      rex_enter_crit_sect(&pbm_crit_sect);
      pbm_clear_index_cache(m_usim_cache);
      rex_leave_crit_sect(&pbm_crit_sect);
    }
  }
  else
  {
    PBM_MSG_ERR("Could not find cache for file %d", file_id, 0, 0);
  }

  /* Move to next file regardless of why we got here. */
  pbm_initialize_index[sim_id]++;

  pbm_init_entries(sim_id);
}
/*===========================================================================
FUNCTION PBM_USIM_READ

DESCRIPTION
  Reads a specified record to USIM phone book. This function should be used
  to read only USIM files.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_usim_read( pbm_slot_type sim_id,
                               pbm_file_device_e_type file_id,
                               int record_num,
                               void (*proc_func) ( pbm_return_type,
                                           pbm_sim_record_s_type *))

{
  int buffer_index;
  PB_USIM_CACHE *usim_cache;
  mmgsdi_file_enum_type filename;
  pbm_cache_type_e_type cache_type;
  int index;

  if (proc_func == NULL)
    return PBM_ERROR;

  cache_type = pbm_file_id_to_cache_type(file_id);
  if (cache_type != PBM_CACHE_TYPE_USIM)
  {
    PBM_MSG_ERR("Wrong function (pbm_usim_read) used to read file %d", file_id,
                0, 0);
    return PBM_ERROR;
  }

  PBM_VALIDATE_SLOT_RET(sim_id, PBM_ERROR);

  usim_cache = pbm_file_id_to_cache(sim_id, file_id);
  PBM_CHECK_PTR_RET(usim_cache, PBM_ERROR);

  filename = usim_cache->filename;
  index = record_num;

  if (filename == MMGSDI_NO_FILE_ENUM || index <= 0)
  {
    PBM_MSG_ERR("Invalid cache/index %d %d %d",
                file_id, record_num, 0);
    return PBM_ERROR;
  }

  buffer_index = pbm_alloc_record();

  if (buffer_index < 0)
    return PBM_ERROR;

  pbm_buffers[buffer_index].proc_func = proc_func;
  pbm_buffers[buffer_index].record.index = record_num;
  pbm_buffers[buffer_index].record.file_id = file_id;
  pbm_buffers[buffer_index].record.sim_id = sim_id;
  pbm_buffers[buffer_index].gsdi_resp_type = PBM_MMGSDI_READ_CB;
  if (pbm_send_read_request(sim_id, file_id, filename,
                            index, buffer_index, pbm_gsdi_async_cb))
  {
    return PBM_SUCCESS;
  }

  PBM_MSG_ERR("Could not issue read request", 0, 0, 0);
  pbm_free_record(buffer_index);
  return PBM_ERROR;
}

/*===========================================================================
FUNCTION PBM_INDEX_READ

DESCRIPTION

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_index_read(pbm_slot_type sim_id,
                               pbm_file_device_e_type file_id,
                               int record_num,
                               void (*proc_func) ( pbm_return_type,
                                           pbm_sim_record_s_type *))

{
  int buffer_index;
  PB_MASTER_USIM_CACHE *m_usim_cache;
  mmgsdi_file_enum_type filename;
  pbm_cache_type_e_type cache_type;
  int index;

  if (proc_func == NULL)
    return PBM_ERROR;

  cache_type = pbm_file_id_to_cache_type(file_id);
  if (cache_type != PBM_CACHE_TYPE_INDEX)
  {
    PBM_MSG_ERR("Wrong function (pbm_index_read) used to read file %d", file_id,
                0, 0);
    return PBM_ERROR;
  }

  PBM_VALIDATE_SLOT_RET(sim_id, PBM_ERROR);

  m_usim_cache = pbm_file_id_to_cache(sim_id, file_id);

  PBM_CHECK_PTR_RET(m_usim_cache, PBM_ERROR);

  if (!m_usim_cache->iap_num_records)
  {
    PBM_MSG_ERR("IAP file not present", 0, 0, 0);
    return PBM_ERROR;
  }

  filename = m_usim_cache->filename;
  index = record_num;

  if (filename == MMGSDI_NO_FILE_ENUM || index <= 0)
  {
    PBM_MSG_ERR("Invalid cache/index %d %d %d",
                file_id, record_num, 0);
    return PBM_ERROR;
  }

  buffer_index = pbm_alloc_record();

  if (buffer_index < 0)
    return PBM_ERROR;

  pbm_buffers[buffer_index].proc_func = proc_func;
  pbm_buffers[buffer_index].record.index = record_num;
  pbm_buffers[buffer_index].record.file_id = file_id;
  pbm_buffers[buffer_index].record.sim_id = sim_id;
  pbm_buffers[buffer_index].gsdi_resp_type = PBM_MMGSDI_READ_CB;
  if (pbm_send_read_request(sim_id, file_id, filename,
                            index, buffer_index, pbm_gsdi_async_cb))
  {
    return PBM_SUCCESS;
  }

  PBM_MSG_ERR("Could not issue read request", 0, 0, 0);
  pbm_free_record(buffer_index);
  return PBM_ERROR;
}

/*===========================================================================
FUNCTION pbm_sync_read

DESCRIPTION
  Reads a specified record to USIM phone book. This function should be used
  to read only synchronization files.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_sync_read(pbm_slot_type sim_id,
                              pbm_file_device_e_type file_id,
                              int record_num,
                              void (*proc_func) ( pbm_return_type,
                                           pbm_sim_record_s_type *))

{
  int buffer_index;
  mmgsdi_file_enum_type filename;
  int index;
  pbm_cache_type_e_type cache_type;

  if (proc_func == NULL)
    return PBM_ERROR;

  cache_type = pbm_file_id_to_cache_type(file_id);
  if (cache_type != PBM_CACHE_TYPE_SYNC1 &&
      cache_type != PBM_CACHE_TYPE_SYNC2)
  {
    PBM_MSG_ERR("Invalid function pbm_sync_read called for file_id %d", file_id,
                 0, 0);
    return PBM_ERROR;
  }

  PBM_VALIDATE_SLOT_RET(sim_id, PBM_ERROR);

  if (file_id >= PBM_FILE_MAX_REQUESTS)
  {
    PBM_MSG_ERR("Invalid file id %d", file_id, 0, 0);
    return PBM_ERROR;
  }

  if (cache_type == PBM_CACHE_TYPE_SYNC1)
  {
    PB_SYNC1_CACHE *sync1_cache;

    sync1_cache = pbm_file_id_to_cache(sim_id, file_id);
    PBM_CHECK_PTR_RET(sync1_cache, PBM_ERROR);

    filename = sync1_cache->filename;
    index = 0; /* Record number should be 0 for transparent file */
  }
  else
  {
    PB_SYNC2_CACHE *sync2_cache;

    sync2_cache = pbm_file_id_to_cache(sim_id, file_id); //lint !e826 area too small
    PBM_CHECK_PTR_RET(sync2_cache, PBM_ERROR);

    filename = pbm_sync2_record_index_to_filename(sync2_cache, record_num);
    index = pbm_sync2_record_index_to_fileloc(sync2_cache, record_num);
  }

  if (filename == MMGSDI_NO_FILE_ENUM || index < 0)
  {
    PBM_MSG_ERR("Invalid file or index %d %d", file_id, record_num, 0);
    return PBM_ERROR;
  }

  buffer_index = pbm_alloc_record();

  if (buffer_index < 0)
    return PBM_ERROR;

  pbm_buffers[buffer_index].proc_func = proc_func;
  pbm_buffers[buffer_index].record.index = record_num;
  pbm_buffers[buffer_index].record.file_id = file_id;
  pbm_buffers[buffer_index].record.sim_id = sim_id;
  pbm_buffers[buffer_index].gsdi_resp_type = PBM_MMGSDI_READ_CB;
  if (pbm_send_read_request(sim_id, file_id, filename,
                            index, buffer_index, pbm_gsdi_async_cb))
  {
    return PBM_SUCCESS;
  }

  PBM_MSG_ERR("Could not issue read request", 0, 0, 0);
  pbm_free_record(buffer_index);
  return PBM_ERROR;
}
#endif /* FEATURE_PBM_USIM_SUPPORT */

/*===========================================================================
FUNCTION PBM_SIM_WRITE_CB

DESCRIPTION
  After GSDI has written the extension record to EXT1, PBM_WRITE_ADN_CB
  is called to copy the data in to a pbm_buffer for later processing.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
void pbm_sim_write_cb( mmgsdi_return_enum_type status,
                       const mmgsdi_cnf_type  *data_ptr)
{
  uint32 client_ref;
  PB_CACHE *cache;      /* The cache has info on extension records. */
  pbm_file_device_e_type file_id;

  if (!data_ptr )
  {
    PBM_MSG_ERR("PBM Bad pointer passed to function 0x%x 0x%x 0x%x",
        data_ptr, 0, 0);
    (void) rex_set_sigs(&pbm_tcb, PBM_CMD_COMPLETE_SIG);
    return;
  }

  client_ref = data_ptr->response_header.client_data;
  file_id = pbm_buffers[client_ref].record.file_id;

  cache = pbm_file_id_to_cache(pbm_buffers[client_ref].record.sim_id,
                               pbm_buffers[client_ref].m_file_id);

  if (cache == NULL)
  {
    PBM_MSG_ERR("cache was NULL for phonebook file 0x%x", file_id, 0, 0);
    pbm_uim_write_cb(client_ref, PBM_ERROR);
    return;
  }

  if (status != MMGSDI_SUCCESS)
  {
    pbm_return_type error_code = PBM_ERROR;

    /* give better error codes for access denied errors */
    if (status == MMGSDI_ACCESS_DENIED)
    {
      switch(file_id)
      {
        /* these require ADM access, which we don't have a special error for. */
        case PBM_FILE_SDN:
        case PBM_FILE_EXT3:
        case PBM_FILE_MSISDN:
        case PBM_FILE_MBN:
#ifdef FEATURE_PBM_USIM_SUPPORT
        case PBM_FILE_MBDN:
#endif /* FEATURE_PBM_USIM_SUPPORT */
          error_code = PBM_ERROR;
          break;

        /* these require PIN2 */
        case PBM_FILE_FDN:
        case PBM_FILE_EXT2:
          error_code = PBM_ERROR_PIN2_REQ;
          break;

        /* the rest of the PB requires PIN1 */
        default:
          error_code = PBM_ERROR_SIM_PIN_REQ;
          break;
      }
    }

    pbm_uim_write_cb(client_ref, error_code);
    return;
  }

  if (pbm_buffers[client_ref].extension != 0xFF)
  {
    if (cache->extension_cache)
    {
      cache->extension_cache->in_use_array[pbm_buffers[client_ref].extension].uim_device =
        pbm_pb_record_index_to_filename(cache, pbm_buffers[client_ref].record.index);
      cache->extension_cache->in_use_array[pbm_buffers[client_ref].extension].index =
        (byte)pbm_pb_record_index_to_fileloc(cache, pbm_buffers[client_ref].record.index);
    }
    else
    {
      PBM_MSG_ERR("We need an extension record but no cache.", 0, 0, 0);
    }
  }

  if (pbm_buffers[client_ref].state != PBM_WRITE_DONE)
  {
    pbm_buffers[client_ref].findex += pbm_buffers[client_ref].findex_delta;
    if (pbm_uim_write(client_ref) != PBM_SUCCESS)
      pbm_uim_write_cb(client_ref, PBM_ERROR);
  }
  else
  {
    pbm_uim_write_cb(client_ref, PBM_SUCCESS);
  }

  return;
}

#ifdef FEATURE_PBM_USIM_SUPPORT
/*===========================================================================
FUNCTION PBM_USIM_WRITE_CB

DESCRIPTION TODO: Template, needs actual code
  After GSDI has written the extension record to EXT1, PBM_WRITE_ADN_CB
  is called to copy the data in to a pbm_buffer for later processing.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
void pbm_usim_write_cb( mmgsdi_return_enum_type status,
                        const mmgsdi_cnf_type  *data_ptr)
{
  uint32 client_ref;
  PB_USIM_CACHE *usim_cache;
  pbm_return_type ret = PBM_SUCCESS;
  pbm_addr_cache_s_type *curr_rec;

  if (!data_ptr )
  {
    PBM_MSG_ERR("PBM Bad pointer passed to function 0x%x 0x%x 0x%x",
        data_ptr, 0, 0);
    (void) rex_set_sigs(&pbm_tcb, PBM_CMD_COMPLETE_SIG);
    return;
  }

  client_ref = data_ptr->response_header.client_data;

  if (status != MMGSDI_SUCCESS)
  {
    pbm_uim_write_cb(client_ref, PBM_ERROR);
    return;
  }

  usim_cache = pbm_file_id_to_cache(pbm_buffers[client_ref].record.sim_id,
                                    pbm_buffers[client_ref].record.file_id);
  if (!usim_cache)
  {
    pbm_uim_write_cb(client_ref, PBM_ERROR);
    return;
  }

  /* Add to USIM cache */
  if (pbm_buffers[client_ref].state == PBM_WRITE_PREV_USIM)
  {
    ret = pbm_usim_cache_delete(usim_cache,
                                (uint16)pbm_buffers[client_ref].record.index,
                                FALSE);
  }
  else if (pbm_buffers[client_ref].state == PBM_WRITE_CURR_USIM)
  {
    curr_rec = pbm_buffers[client_ref].write->curr_rec;
    ret = pbm_usim_cache_fields_add(usim_cache,
                                    (uint16) pbm_buffers[client_ref].record.index,
                                    curr_rec->record_id,
                                    &curr_rec->fields[pbm_buffers[client_ref].findex],
                                    pbm_buffers[client_ref].findex_delta);
  }

  if (ret != PBM_SUCCESS)
  {
    PBM_MSG_ERR("Unable to write to location %d in cache of USIM file %d",
      pbm_buffers[client_ref].record.index, usim_cache->file_id, 0);
  }

  if (pbm_buffers[client_ref].extension != 0xFF)
  {
    if (adn_cache[pbm_buffers[client_ref].record.sim_id].extension_cache)
    {
      adn_cache[pbm_buffers[client_ref].record.sim_id].extension_cache->in_use_array[pbm_buffers[client_ref].extension].uim_device =
        usim_cache->filename;
      adn_cache[pbm_buffers[client_ref].record.sim_id].extension_cache->in_use_array[pbm_buffers[client_ref].extension].index =
        (byte) pbm_buffers[client_ref].record.index;
    }
    else
    {
      PBM_MSG_ERR("We need an extension record but no cache.", 0, 0, 0);
    }
  }
  if (pbm_buffers[client_ref].state != PBM_WRITE_DONE)
  {
    pbm_buffers[client_ref].findex += pbm_buffers[client_ref].findex_delta;
    if (pbm_uim_write(client_ref) != PBM_SUCCESS)
      pbm_uim_write_cb(client_ref, PBM_ERROR);
  }
  else
  {
    pbm_uim_write_cb(client_ref, PBM_SUCCESS);
  }

  return;
}


/*===========================================================================
FUNCTION PBM_SYNC_WRITE_CB

DESCRIPTION

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
void pbm_sync_write_cb( mmgsdi_return_enum_type status,
                        const mmgsdi_cnf_type  *data_ptr)
{
  uint32 client_ref;

  if (!data_ptr )
  {
    PBM_MSG_ERR("PBM Bad pointer passed to function 0x%x 0x%x 0x%x",
        data_ptr, 0, 0);
    (void) rex_set_sigs(&pbm_tcb, PBM_CMD_COMPLETE_SIG);
    return;
  }

  client_ref = data_ptr->response_header.client_data;

  if (status != MMGSDI_SUCCESS)
  {
    pbm_uim_write_cb(client_ref, PBM_ERROR);
    return;
  }

  if (pbm_buffers[client_ref].state != PBM_WRITE_DONE)
  {
    pbm_buffers[client_ref].findex += pbm_buffers[client_ref].findex_delta;
    if (pbm_uim_write(client_ref) != PBM_SUCCESS)
      pbm_uim_write_cb(client_ref, PBM_ERROR);
  }
  else
  {
    pbm_uim_write_cb(client_ref, PBM_SUCCESS);
  }

  return;
}
#endif /* FEATURE_PBM_USIM_SUPPORT */

/*===========================================================================
FUNCTION PBM_EXT_WRITE_CB

DESCRIPTION
  Called after GSDI has written the extension record.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
void pbm_ext_write_cb( mmgsdi_return_enum_type status,
                       const mmgsdi_cnf_type  *data_ptr)
{
  uint32 client_ref;
  pbm_return_type   ret_val;

  if (!data_ptr )
  {
    PBM_MSG_ERR("PBM Bad pointer passed to function 0x%x 0x%x 0x%x",
        data_ptr, 0, 0);
    (void) rex_set_sigs(&pbm_tcb, PBM_CMD_COMPLETE_SIG);
    return;
  }

  client_ref = data_ptr->response_header.client_data;
  if (status == MMGSDI_SUCCESS)
  {
    pbm_buffers[client_ref].record.file_id = pbm_buffers[client_ref].m_file_id;
    if (pbm_file_id_to_field_id(pbm_buffers[client_ref].m_file_id) ==
                                    PBM_FIELD_ADDITIONAL_NUMBER)
    {
#ifdef FEATURE_PBM_USIM_SUPPORT
      if (pbm_create_anr_record(pbm_buffers[client_ref].record.sim_id,
                                   pbm_buffers[client_ref].m_file_id,
                                   &pbm_buffers[client_ref].record.data.anr,
                                   client_ref) != PBM_SUCCESS)
#endif /* FEATURE_PBM_USIM_SUPPORT */
      {
        pbm_uim_write_cb(client_ref, PBM_ERROR);
      }
    }
    else
    {
      ret_val = pbm_create_sim_record(pbm_buffers[client_ref].record.sim_id,
                             pbm_buffers[client_ref].record.file_id,
                             &pbm_buffers[client_ref].record.data.num_text,
                             (int) client_ref);
      if ( ret_val != PBM_SUCCESS )
      {
        pbm_uim_write_cb(client_ref, ret_val);
      }
    }
  }
  else
  {
    pbm_uim_write_cb(client_ref, PBM_ERROR);
  }

  return;
}

#ifdef FEATURE_PBM_USIM_SUPPORT
/*===========================================================================
FUNCTION PBM_INDEX_WRITE_CB

DESCRIPTION
  Called after GSDI has written the extension record.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
void pbm_index_write_cb( mmgsdi_return_enum_type status,
                         const mmgsdi_cnf_type  *data_ptr)
{
  uint32 client_ref;

  if (!data_ptr )
  {
    PBM_MSG_ERR("PBM Bad pointer passed to function 0x%x 0x%x 0x%x",
        data_ptr, 0, 0);
    (void) rex_set_sigs(&pbm_tcb, PBM_CMD_COMPLETE_SIG);
    return;
  }

  client_ref = data_ptr->response_header.client_data;

  if (status != MMGSDI_SUCCESS)
  {
    pbm_uim_write_cb(client_ref, PBM_ERROR);
    return;
  }

  if (pbm_buffers[client_ref].state != PBM_WRITE_DONE)
  {
    if (pbm_uim_write(client_ref) != PBM_SUCCESS)
      pbm_uim_write_cb(client_ref, PBM_ERROR);
  }
  else
  {
    pbm_uim_write_cb(client_ref, PBM_SUCCESS);
  }

  return;
}

/*===========================================================================
FUNCTION PBM_USIM_WRITE

DESCRIPTION

PARAMETERS
  field_index
  record.sim_id
  write

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
boolean pbm_usim_write(uint32 buffer_index, pbm_return_type *ret)
{
  pbm_async_gsdi_buffer *buffer;
  pbm_addr_cache_s_type *record = NULL;
  pbm_write_record_s_type *write;
  PB_USIM_CACHE *usim_cache;
  uint16 field_index;
  pbm_pb_set_type pb_set;
  pbm_slot_type sim_id;
  uint8 *index_array = NULL;
  uint16 index = 0;
  int i;

  PBM_CHECK_PTR_RET(ret, FALSE);

  buffer = &pbm_buffers[buffer_index];

  do
  {
    write = buffer->write;
    PBM_CHECK_PTR_BREAK(write, *ret, PBM_ERROR);

    if (buffer->state == PBM_WRITE_PREV_USIM)
    {
      record = write->prev_rec;
      index_array = write->prev_usim_index;
    }
    else if (buffer->state == PBM_WRITE_CURR_USIM)
    {
      record = write->curr_rec;
      index_array = write->curr_usim_index;
    }
    PBM_CHECK_PTR_BREAK(record, *ret, PBM_ERROR);
    PBM_CHECK_PTR_BREAK(index_array, *ret, PBM_ERROR);

    pb_set = pbm_record_id_to_pb_set(write->rec_id);
    PBM_CHECK_PBSET_BREAK(pb_set);

    memset(&buffer->record.data, 0, sizeof(pbm_generic_sim_data_s_type));
    buffer->extension = 0xff;

    sim_id = buffer->record.sim_id;
    field_index = buffer->findex;
    switch(record->fields[field_index].field_id)
    {
      case PBM_FIELD_EMAIL:
        usim_cache = &email_cache[sim_id][pb_set];
        buffer->record.file_id = usim_cache->file_id;
        buffer->record.index = index_array[usim_cache->order];
        buffer->findex_delta = 1;

        if (buffer->state == PBM_WRITE_PREV_USIM)
          buffer->record.data.text.adn_record_id = buffer->write->rec_id;
        else
          pbm_build_record_from_usim_fields(buffer->write->rec_id,
                                        &record->fields[field_index],
                                        buffer->findex_delta, &buffer->record);

        *ret = pbm_usim_write_text(buffer_index, TRUE);
        break;

      case PBM_FIELD_ADDITIONAL_NUMBER:
        i = 0;
        index = 0;
        //go through the caches, and find the first one that is not
        //in use, and is valid for this type of data
        while ((usim_cache = pbm_get_next_usim_cache(write->rec_id,
                                 PBM_FIELD_ADDITIONAL_NUMBER, &i)) != NULL &&
                buffer->caches_in_use[usim_cache->order])
          NO_ACTION

        if (usim_cache != NULL)
        {
          index = index_array[usim_cache->order];
          if (index)
          {
            buffer->record.file_id = usim_cache->file_id;
            buffer->record.index = index;
            buffer->caches_in_use[usim_cache->order] = TRUE;
          }
        }
        if (!usim_cache || !index)
        {
          *ret = PBM_ERROR;
          break;
        }
        buffer->findex_delta = 1;

        if (buffer->state == PBM_WRITE_PREV_USIM)
          buffer->record.data.text.adn_record_id = buffer->write->rec_id;
        else
          pbm_build_record_from_usim_fields(buffer->write->rec_id,
                                        &record->fields[field_index],
                                        buffer->findex_delta, &buffer->record);
        *ret = pbm_usim_write_anr(buffer_index);
        break;

      case PBM_FIELD_OTHERNAME:
        usim_cache = &sne_cache[sim_id][pb_set];
        buffer->record.file_id = usim_cache->file_id;
        buffer->record.index = index_array[usim_cache->order];
        buffer->findex_delta = 1;

        if (buffer->state == PBM_WRITE_PREV_USIM)
          buffer->record.data.text.adn_record_id = buffer->write->rec_id;
        else
          pbm_build_record_from_usim_fields(buffer->write->rec_id,
                                        &record->fields[field_index],
                                        buffer->findex_delta, &buffer->record);

        *ret = pbm_usim_write_text(buffer_index, FALSE);
        break;

      case PBM_FIELD_GRPNAME_ID:
        usim_cache = &grp_cache[sim_id][pb_set];
        buffer->record.file_id = usim_cache->file_id;
        buffer->record.index = index_array[usim_cache->order];
        i = 0;
        while (record->fields[i].field_id == PBM_FIELD_GRPNAME_ID &&
               i < record->num_fields) i++;
        buffer->findex_delta = (uint16) MIN(i, usim_cache->record_length);

        if (buffer->state != PBM_WRITE_PREV_USIM)
          pbm_build_record_from_usim_fields(buffer->write->rec_id,
                                        &record->fields[field_index],
                                        buffer->findex_delta, &buffer->record);

        *ret = pbm_usim_write_grp_id(buffer_index);
        break;

      default:
        *ret = PBM_ERROR;
        break;
    }

    if (*ret == PBM_SUCCESS)
      return TRUE;

  } while (FALSE); /*lint !e717 */

  return FALSE;
}

/*===========================================================================
FUNCTION PBM_SYNC_WRITE_SYNC1

DESCRIPTION

PARAMETERS
  record.index
  record.file_id
  record.sim_id
  write

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_sync_write_sync1(uint32 buffer_index)
{
  byte                         *gsdi_buffer;
  pbm_async_gsdi_buffer        *buffer;
  pbm_slot_type                 sim_id;
  pbm_file_device_e_type        file_id;
  pbm_return_type               pbm_retval;
  PB_SYNC1_CACHE               *sync_cache;

  buffer = &pbm_buffers[buffer_index];

  sim_id = buffer->record.sim_id;
  file_id = buffer->record.file_id;

  sync_cache = pbm_file_id_to_cache(sim_id, file_id);
  PBM_CHECK_PTR_RET(sync_cache, PBM_ERROR);

  PBM_MEM_ALLOC(gsdi_buffer, (uint32) sync_cache->record_length);
  PBM_CHECK_PTR_RET(gsdi_buffer, PBM_ERROR_MEM_FULL);
  memset(gsdi_buffer, 0, (uint32)sync_cache->record_length);


  switch (file_id)
  {
    case PBM_FILE_PUID:
    {
      uint16 data = buffer->write->curr_rec->unique_id;
      gsdi_buffer[0] = (data & 0xFF00) >> 8;
      gsdi_buffer[1] = (data & 0x00FF);
      break;
    }

    case PBM_FILE_CC:
    {
      uint16 data = buffer->write->sync.cc;
      gsdi_buffer[0] = (data & 0xFF00) >> 8;
      gsdi_buffer[1] = (data & 0x00FF);
      break;
    }

    case PBM_FILE_PSC:
    {
      uint32 data = buffer->write->sync.psc;
      gsdi_buffer[0] = (data & 0xFF000000) >> 24;
      gsdi_buffer[1] = (data & 0x00FF0000) >> 16;
      gsdi_buffer[2] = (data & 0x0000FF00) >> 8;
      gsdi_buffer[3] = (data & 0x000000FF);
      break;
    }

    default:
      PBM_MSG_ERR("Invalid file id %d", file_id, 0, 0);
      break;
  }

  pbm_buffers[buffer_index].gsdi_resp_type = PBM_MMGSDI_WRITE_CB;
  pbm_retval = pbm_gsdi_uim_write(sim_id,
                                  sync_cache->filename,
                                  sync_cache->file_type,
                                  0, /* transparent file */
                                  gsdi_buffer,
                                  sync_cache->record_length,
                                  buffer_index,
                                  pbm_gsdi_async_cb);
  if (pbm_retval != PBM_SUCCESS)
  {
    PBM_MSG_ERR("pbm_gsdi_uim_write returned %d", pbm_retval, 0, 0);
    PBM_MEM_FREEIF(gsdi_buffer);
    return PBM_ERROR;
  }

  PBM_MEM_FREEIF(gsdi_buffer);
  return PBM_SUCCESS;
}

/*===========================================================================
FUNCTION PBM_SYNC_WRITE_SYNC2

DESCRIPTION

PARAMETERS
  record.index
  record.file_id
  record.sim_id
  write

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_sync_write_sync2(uint16 data, uint32 buffer_index)
{
  byte                         *gsdi_buffer;
  pbm_async_gsdi_buffer        *buffer;
  pbm_slot_type                 sim_id;
  pbm_file_device_e_type        file_id;
  pbm_return_type               pbm_retval;
  PB_SYNC2_CACHE               *sync_cache;
  pbm_pb_set_type               pb_set;
  int                           index;
  mmgsdi_file_enum_type         filename;
  mmgsdi_file_structure_enum_type filetype;

  buffer = &pbm_buffers[buffer_index];

  sim_id = buffer->record.sim_id;
  file_id = buffer->record.file_id;

  sync_cache = pbm_file_id_to_cache(sim_id, file_id);
  PBM_CHECK_PTR_RET(sync_cache, PBM_ERROR);

  PBM_MEM_ALLOC(gsdi_buffer, (uint32) sync_cache->record_length);
  PBM_CHECK_PTR_RET(gsdi_buffer, PBM_ERROR_MEM_FULL);
  memset(gsdi_buffer, 0, (uint32)sync_cache->record_length);

  pb_set = pbm_record_id_to_pb_set(buffer->write->rec_id);
  PBM_CHECK_PBSET_RET(pb_set, PBM_ERROR);

  filename = sync_cache->filenames[pb_set];
  filetype = sync_cache->file_type;
  index = pbm_sync2_record_index_to_fileloc(sync_cache, buffer->record.index);

  switch(file_id)
  {
    case PBM_FILE_UID:
    {
      gsdi_buffer[0] = (data & 0xFF00) >> 8;
      gsdi_buffer[1] = (data & 0x00FF);
      break;
    }

    case PBM_FILE_PBC:
      /* Writes are always resets */
      break;

    default:
      PBM_MSG_ERR("Invalid file id %d", file_id, 0, 0);
      break;
  }

  pbm_buffers[buffer_index].gsdi_resp_type = PBM_MMGSDI_WRITE_CB;
  pbm_retval = pbm_gsdi_uim_write(sim_id,
                                  filename,
                                  filetype,
                                  index,
                                  gsdi_buffer,
                                  sync_cache->record_length,
                                  buffer_index,
                                  pbm_gsdi_async_cb);
  if (pbm_retval != PBM_SUCCESS)
  {
    PBM_MSG_ERR("pbm_gsdi_uim_write returned %d", pbm_retval, 0, 0);
    PBM_MEM_FREEIF(gsdi_buffer);
    return PBM_ERROR;
  }

  PBM_MEM_FREEIF(gsdi_buffer);
  return PBM_SUCCESS;
}

/*===========================================================================
FUNCTION PBM_IAP_WRITE

DESCRIPTION

PARAMETERS
  record.sim_id
  write

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_iap_write(uint32 buffer_index)
{
  pbm_async_gsdi_buffer        *buffer;
  pbm_addr_cache_s_type        *record = NULL;
  pbm_write_record_s_type      *write;
  PB_USIM_CACHE                *usim_cache;
  pbm_pb_set_type               pb_set;
  uint16                        index = 0;
  PB_MASTER_USIM_CACHE         *m_usim_cache;
  int                           i;
  pbm_iap_record_s_type        *iap_record;
  uint8                        *index_array = NULL;

  buffer = &pbm_buffers[buffer_index];

  write = buffer->write;
  PBM_CHECK_PTR_RET(write, PBM_ERROR);

  if (buffer->state == PBM_WRITE_PREV_USIM)
  {
    record = write->prev_rec;
    index_array = write->prev_usim_index;
  }
  else if (buffer->state == PBM_WRITE_CURR_USIM)
  {
    record = write->curr_rec;
    index_array = write->curr_usim_index;
  }

  PBM_CHECK_PTR_RET(record, PBM_ERROR);

  pb_set = pbm_record_id_to_pb_set(write->rec_id);
  PBM_CHECK_PBSET_RET(pb_set, PBM_ERROR);

  m_usim_cache = &adn_m_usim_cache[buffer->record.sim_id][pb_set];
  if (!m_usim_cache->iap_num_records)
  {
    PBM_MSG_ERR("IAP file not present", 0, 0, 0);
    return PBM_ERROR;
  }

  iap_record = &buffer->record.data.iap;
  memset(iap_record, 0xFF, sizeof(pbm_iap_record_s_type));

  index = (uint16) pbm_record_id_to_fileloc(write->rec_id);
  if (buffer->state != PBM_WRITE_PREV_USIM && index_array)
  {
    for (i = 1; i <= m_usim_cache->iap_rec_len; i++)
    {
      usim_cache = m_usim_cache->usim_cache_array[i];
      if (usim_cache && usim_cache->mapping_type == GSDI_MAPPING_TYPE_2)
      {
        iap_record->index[i-1] = index_array[i] ? index_array[i] : 0xFF;
      }
    }
  }

  buffer->record.file_id = m_usim_cache->file_id;
  buffer->record.index = index;

  return pbm_usim_write_iap(buffer_index);
}

/*===========================================================================
FUNCTION pbm_usim_write_text

DESCRIPTION
  pbm_return_type pbm_usim_write_text(uint32 buffer_index, boolean gsm7_only)

  This function will write a text record to a USIM file.  The encoding
  of the text is either GSM7 or, if gsm7_only is false, one of the UCS2
  encoding schemes allowed.

PARAMETERS
  buffer_index: [in] Contains the index to where we are writing the info.
  gsm7_only: [in] TRUE if we must encode in GSM7 alphabet only.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_usim_write_text(uint32 buffer_index, boolean gsm7_only)
{
  byte                   *gsdi_buffer;
  PB_USIM_CACHE          *usim_cache;
  pbm_async_gsdi_buffer  *buffer;
  mmgsdi_file_enum_type   filename;
  mmgsdi_file_structure_enum_type filetype;
  pbm_slot_type           sim_id;
  pbm_return_type         pbm_retval;
  uint16                  text_size;
  pbm_text_record_s_type *text_record;
  uint16                  text_len;
  uint16                  chars_converted;

  buffer = &pbm_buffers[buffer_index];
  usim_cache = pbm_file_id_to_cache(buffer->record.sim_id,
                                    buffer->record.file_id);
  PBM_CHECK_PTR_RET(usim_cache, PBM_ERROR);

  PBM_MEM_ALLOC(gsdi_buffer, (uint32) usim_cache->record_length);
  PBM_CHECK_PTR_RET(gsdi_buffer, PBM_ERROR_MEM_FULL);
  memset(gsdi_buffer, 0xFF, (uint32) usim_cache->record_length);

  text_record = &buffer->record.data.text;
  filename = usim_cache->filename;
  filetype = usim_cache->file_type;
  sim_id = buffer->record.sim_id;

  text_size = (uint16) usim_cache->record_length;
  if ((text_size >= 2) && (usim_cache->mapping_type == GSDI_MAPPING_TYPE_2))
    text_size -= 2;

  text_len = (uint16) pbmutils_wstrlen(text_record->text);
  (void) pbmconvert_string_to_sim(text_record->text, text_len, PBM_FT_UCS2,
                                  gsdi_buffer, text_size,
                                  &chars_converted, gsm7_only);

  if (chars_converted != pbmutils_wstrlen(text_record->text))
  {
    PBM_MSG_ERR("Did not convert all characters expected %d converted %d",
                pbmutils_wstrlen(text_record->text), chars_converted, 0);
    PBM_MEM_FREEIF(gsdi_buffer);
    return PBM_ERROR;
  }

  if (usim_cache->mapping_type == GSDI_MAPPING_TYPE_2 &&
      buffer->state == PBM_WRITE_CURR_USIM)
  {
    if (pbm_fill_adn_sfi(text_record->adn_record_id, &gsdi_buffer[text_size]) !=
          PBM_SUCCESS)
    {
      PBM_MSG_ERR("Unable to fill ADN SFI", 0, 0, 0);
      PBM_MEM_FREEIF(gsdi_buffer);
      return PBM_ERROR;
    }
  }

  pbm_buffers[buffer_index].gsdi_resp_type = PBM_MMGSDI_WRITE_CB;
  pbm_retval = pbm_gsdi_uim_write(sim_id,
                                  filename,
                                  filetype,
                                  buffer->record.index,
                                  gsdi_buffer,
                                  usim_cache->record_length,
                                  buffer_index,
                                  pbm_gsdi_async_cb);
  if (pbm_retval != PBM_SUCCESS)
  {
    PBM_MSG_ERR("pbm_gsdi_uim_write returned %d", pbm_retval, 0, 0);
    PBM_MEM_FREEIF(gsdi_buffer);
    return PBM_ERROR;
  }

  PBM_MEM_FREEIF(gsdi_buffer);

  return PBM_SUCCESS;
}

/*===========================================================================
FUNCTION PBM_USIM_WRITE_ANR

DESCRIPTION

PARAMETERS
  field_index
  record.sim_id
  write

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_usim_write_anr(uint32 buffer_index)
{
  PB_USIM_CACHE         *usim_cache;
  pbm_async_gsdi_buffer *buffer;
  pbm_slot_type          sim_id;
  byte                   num_buffer[PBM_MAX_NUM_LENGTH];
  pbm_anr_record_s_type *anr_record;
  pbm_ext_cache_s_type  *ext_cache;

  buffer = &pbm_buffers[buffer_index];
  usim_cache = pbm_file_id_to_cache(buffer->record.sim_id,
                                    buffer->record.file_id);
  PBM_CHECK_PTR_RET(usim_cache, PBM_ERROR);

  sim_id = buffer->record.sim_id;

  anr_record = &buffer->record.data.anr;

  if (!pbm_ascii_to_bcd((byte *)anr_record->number, anr_record->type, num_buffer))
  {
    return PBM_ERROR;
  }

  /* Do we need an extension record?  */
  ext_cache = &ext1_cache[sim_id];

  if ((num_buffer[0] - 1) > PBM_SIM_DIALING_NUMBER_LEN)
  {
    pbm_buffers[buffer_index].m_file_id = buffer->record.file_id;
    return pbm_create_ext_record(buffer_index, ext_cache, num_buffer,
                                 (uint16)buffer->record.index,
                                 usim_cache->filename);
  }
  /* see if we used to need one, but no longer */
  else if (pbm_check_extension_record(ext_cache, (uint16)buffer->record.index, usim_cache->filename))
  {
    pbm_buffers[buffer_index].m_file_id = buffer->record.file_id;
    return pbm_clear_ext_record(buffer_index,
                                ext_cache,
                                (uint16)buffer->record.index,
                                usim_cache->filename);
  }
  else
  {
    pbm_buffers[buffer_index].extension = 0xff;
    return pbm_create_anr_record(sim_id, buffer->record.file_id,
                                 anr_record, buffer_index);
  }
}

/*===========================================================================
FUNCTION PBM_USIM_WRITE_GRP_ID

DESCRIPTION

PARAMETERS
  field_index
  record.sim_id
  write

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_usim_write_grp_id(uint32 buffer_index)
{
  byte                  *gsdi_buffer;
  PB_USIM_CACHE         *usim_cache;
  pbm_async_gsdi_buffer *buffer;
  mmgsdi_file_enum_type  filename;
  mmgsdi_file_structure_enum_type filetype;
  pbm_slot_type          sim_id;
  pbm_return_type        pbm_retval;
  int                    i;

  buffer = &pbm_buffers[buffer_index];
  usim_cache = pbm_file_id_to_cache(buffer->record.sim_id,
                                    buffer->record.file_id);
  PBM_CHECK_PTR_RET(usim_cache, PBM_ERROR);

  PBM_MEM_ALLOC(gsdi_buffer, (uint32) usim_cache->record_length);
  PBM_CHECK_PTR_RET(gsdi_buffer, PBM_ERROR_MEM_FULL);
  //unused group entries should be 0
  memset(gsdi_buffer, 0, (uint32) usim_cache->record_length);

  filename = usim_cache->filename;
  filetype = usim_cache->file_type;
  sim_id = buffer->record.sim_id;

  for (i = 0; i < usim_cache->record_length; i++)
    if (buffer->record.data.grp.group_id[i])
      gsdi_buffer[i] = (uint8) buffer->record.data.grp.group_id[i];

  pbm_buffers[buffer_index].gsdi_resp_type = PBM_MMGSDI_WRITE_CB;
  pbm_retval = pbm_gsdi_uim_write(sim_id,
                                  filename,
                                  filetype,
                                  buffer->record.index,
                                  gsdi_buffer,
                                  usim_cache->record_length,
                                  buffer_index,
                                  pbm_gsdi_async_cb);
  if (pbm_retval != PBM_SUCCESS)
  {
    PBM_MSG_ERR("pbm_gsdi_uim_write returned %d", pbm_retval, 0, 0);
    PBM_MEM_FREEIF(gsdi_buffer);
    return PBM_ERROR;
  }

  PBM_MEM_FREEIF(gsdi_buffer);
  return PBM_SUCCESS;
}

/*===========================================================================
FUNCTION PBM_USIM_WRITE_IAP

DESCRIPTION

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_usim_write_iap(uint32 buffer_index)
{
  byte                         *gsdi_buffer;
  PB_MASTER_USIM_CACHE         *index_cache;
  pbm_async_gsdi_buffer        *buffer;
  mmgsdi_file_enum_type         filename;
  mmgsdi_file_structure_enum_type filetype;
  pbm_slot_type                 sim_id;
  pbm_return_type               pbm_retval;

  buffer = &pbm_buffers[buffer_index];
  index_cache = pbm_file_id_to_cache(buffer->record.sim_id,
                                    buffer->record.file_id);
  PBM_CHECK_PTR_RET(index_cache, PBM_ERROR);

  PBM_MEM_ALLOC(gsdi_buffer, (uint32) index_cache->iap_rec_len);
  PBM_CHECK_PTR_RET(gsdi_buffer, PBM_ERROR_MEM_FULL);
  memset(gsdi_buffer, 0, (uint32)index_cache->iap_rec_len);

  filename = index_cache->filename;
  filetype = index_cache->file_type;
  sim_id = buffer->record.sim_id;

  memcpy(gsdi_buffer, buffer->record.data.iap.index,
         (uint32) index_cache->iap_rec_len);

  pbm_buffers[buffer_index].gsdi_resp_type = PBM_MMGSDI_WRITE_CB;
  pbm_retval = pbm_gsdi_uim_write(sim_id,
                                  filename,
                                  filetype,
                                  buffer->record.index,
                                  gsdi_buffer,
                                  index_cache->iap_rec_len,
                                  buffer_index,
                                  pbm_gsdi_async_cb);
  if (pbm_retval != PBM_SUCCESS)
  {
    PBM_MSG_ERR("pbm_gsdi_uim_write returned %d", pbm_retval, 0, 0);
    PBM_MEM_FREEIF(gsdi_buffer);
    return PBM_ERROR;
  }

  PBM_MEM_FREEIF(gsdi_buffer);
  return PBM_SUCCESS;
}


/*===========================================================================
FUNCTION PBM_RESET_PBC

DESCRIPTION

DEPENDENCIES
  Should be called only from PBM task and only during initialization
  PBC file is present (pb_cache->m_sync_cache->pbc_cache is not NULL).

SIDE EFFECTS
===========================================================================*/
void pbm_reset_pbc(pbm_slot_type sim_id, int index)
{
  int i;
  pbm_device_type pb_id;
  uint16 rec_id;
  PB_CACHE *pb_cache = &adn_cache[sim_id];


  pb_cache->m_sync_cache->reset_pbc = TRUE;

  for (i = index; i <= pb_cache->num_of_records; i++)
  {
    if (pbc_cache[sim_id].data_array[i] & PBM_PBC_MODIFIED_MASK)
    {
      pb_id = (sim_id == PBM_SLOT_1 ? PBM_ADN : PBM_ADN2);
      rec_id = pbm_location_to_record_id(pb_id, i);
      if (pbm_delete_usim_fields(rec_id, pbm_reset_pbc_cb) == PBM_SUCCESS)
        return;
      else
        PBM_MSG_ERR("Unable to reset pbc record %d", rec_id, 0, 0);
    }
  }

  pb_cache->m_sync_cache->reset_pbc = FALSE;

  PBM_SET_PB_CACHE_READY(pb_cache);

  pbm_initialize_index[sim_id]++;

  pbm_init_entries(sim_id);
}

/*===========================================================================
FUNCTION PBM_RESET_PBC_CB

DESCRIPTION

DEPENDENCIES
  Should be called only from PBM task and only during initialization

SIDE EFFECTS
===========================================================================*/
/*lint -e{818} suppress could be const */
void pbm_reset_pbc_cb(pbm_writecb_data_s_type *cb_data)
{
  int index;
  pbm_slot_type sim_id;

  PBM_CHECK_PTR_RET(cb_data, VOID);

  index = pbm_record_id_to_location(cb_data->rec_id);
  sim_id = pbm_pb_id_to_sim_id(pbm_record_id_to_phonebook(cb_data->rec_id));

  if (cb_data->ret != PBM_SUCCESS)
  {
    PBM_MSG_ERR("Failed to reset PBC entry for record %d sim %d",
                 index, sim_id, 0);
  }

  pbm_reset_pbc(sim_id, index+1);
}

/*===========================================================================
FUNCTION PBM_DELETE_USIM_FIELDS

DESCRIPTION

DEPENDENCIES
  Should be called only from PBM task and only during initialization.
  Cache status should not be ready at this point ie clients should not be
  trying to write.

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_delete_usim_fields(uint16 rec_id,
                                       PBM_WRITE_CB_FUNC proc_func)
{
  pbm_device_type          pb_id = pbm_record_id_to_phonebook(rec_id);
  PB_CACHE                *pb_cache = pbm_pb_id_to_cache(pb_id);
  pbm_pb_set_type          pb_set = pbm_record_id_to_pb_set(rec_id);
  int                      index = pbm_record_id_to_fileloc(rec_id);
  PB_MASTER_USIM_CACHE    *m_usim_cache;
  PB_USIM_CACHE           *usim_cache;
  int                      usim_index;
  uint8                   *prev_index = NULL;
  pbm_write_record_s_type *write_cmd = NULL;
  pbm_addr_cache_s_type   *prev_record = NULL;
  pbm_addr_cache_s_type   *curr_record = NULL;
  int                      num_usim_fields = 0;
  pbm_field_s_type       **usim_fields = NULL;
  pbm_usim_cache_s_type   *usim_rec = NULL;
  uint32                  *field_lengths = NULL;
  int i, j;

  PBM_CHECK_PBSET_RET(pb_set, PBM_ERROR);

  PBM_CHECK_PTR_RET(pb_cache, PBM_ERROR);

  m_usim_cache = pb_cache->m_usim_cache[pb_set];
  if (!m_usim_cache)
    return PBM_SUCCESS;

  PBM_MEM_ALLOC(usim_fields, PBM_MAX_USIM_FIELDS_PER_REC *
                             sizeof(pbm_field_s_type *));
  PBM_CHECK_PTR_RET(usim_fields, PBM_ERROR);

  /* Find number of usim fields and usim field pointers */
  for ( i = 1; i <= m_usim_cache->num_caches; i++)
  {
    usim_cache = m_usim_cache->usim_cache_array[i];
    if (!usim_cache)
      continue;

    usim_index = m_usim_cache->index_array[i][index];
    if (usim_index)
    {
      usim_rec = usim_cache->pb_cache_array[usim_index];
      if (usim_rec)
      {
        for (j = 0; j < usim_rec->num_fields; j++)
          usim_fields[num_usim_fields++] = &usim_rec->fields[j];
      }
      else
      {
        PBM_MSG_ERR("Invalid record found in USIM %d index %d",
                     usim_cache->file_id, usim_index, 0);
      }
    }
  }

  /* Allocate all required memory */
  PBM_MEM_ALLOC(prev_index, (uint32) (m_usim_cache->num_caches+1) *
                                     sizeof(uint8));
  PBM_MEM_ALLOC(field_lengths, (uint32)num_usim_fields*sizeof(uint32));

  write_cmd = (pbm_write_record_s_type *)
                  pbm_cmd_alloc(sizeof(pbm_write_record_s_type));
  prev_record = pbm_cache_record_new();
  curr_record = pbm_cache_record_new();

  if (!prev_index || !field_lengths || !write_cmd ||
      !prev_record || !curr_record)
  {
    PBM_MSG_ERR("Unable to allocate required memory", 0, 0, 0);
    PBM_MEM_FREEIF(prev_index);
    PBM_MEM_FREEIF(field_lengths);
    PBM_MEM_FREEIF(write_cmd);
    PBM_MEM_FREEIF(prev_record);
    PBM_MEM_FREEIF(curr_record);
    return PBM_ERROR_MEM_FULL;
  }

  for (i = 0; i < num_usim_fields; i++)
    field_lengths[i] = usim_fields[i]->data_len;

  if (!pbm_allocate_fields_internal(&prev_record->fields,
                           (uint32)num_usim_fields, field_lengths))
  {
    PBM_MSG_ERR("Unable to allocate field data", 0, 0, 0);
    PBM_MEM_FREEIF(prev_index);
    PBM_MEM_FREEIF(field_lengths);
    PBM_MEM_FREEIF(write_cmd);
    PBM_MEM_FREEIF(prev_record);
    PBM_MEM_FREEIF(curr_record);
    return PBM_ERROR_MEM_FULL;
  }

  for (i = 0; i < num_usim_fields; i++)
    pbm_fill_field(&prev_record->fields[i], usim_fields[i]->field_id,
                   usim_fields[i]->field_type, pbm_get_field_data_ptr(usim_fields[i],0),
                   (uint16) field_lengths[i]);

  prev_record->num_fields = (uint16) num_usim_fields;
  prev_record->record_id = rec_id;
  prev_record->unique_id = 0;

  curr_record->record_id = rec_id;
  curr_record->unique_id =
            pbm_uim_generate_sync_values(pb_cache, &write_cmd->sync, TRUE);

  PBM_MEM_FREEIF(field_lengths);
  PBM_MEM_FREEIF(usim_fields);

  if (!pbm_copy_usim_index(rec_id, prev_index))
  {
    PBM_MSG_ERR("Unable to copy previous index array", 0, 0, 0);
    PBM_MEM_FREEIF(prev_index);
    PBM_MEM_FREEIF(write_cmd);
    (void) pbm_cache_record_free(prev_record);
    return PBM_ERROR;
  }

  if (pbm_master_usim_cache_update(rec_id, prev_index, NULL)
                                                          != PBM_SUCCESS)
  {
    PBM_MSG_ERR("Unable to update master index", 0, 0, 0);
  }

  write_cmd->cmd_type   = PBM_CMD_WRITE;
  write_cmd->rec_id     = rec_id;
  write_cmd->prev_rec   = prev_record;
  write_cmd->proc_func  = proc_func;

  write_cmd->curr_rec        = curr_record;
  write_cmd->curr_usim_index = NULL;
  write_cmd->prev_usim_index = prev_index;

  pbm_client_cmd_q_put(PBM_CMD_PTR(write_cmd));

  return PBM_SUCCESS;
}
#endif /* FEATURE_PBM_USIM_SUPPORT */

/*===========================================================================
FUNCTION PBM_UIM_FILENAME_TO_REFRESH_GROUP

DESCRIPTION
  This function will return the refresh file group corresponding to the
  UIM filename

SIDE EFFECTS
===========================================================================*/
pbm_file_device_e_type pbm_uim_filename_to_refresh_group(mmgsdi_file_enum_type uim_file,
                                                         pbm_slot_type sim_id)
{
  int i;
  uint32 j;

  /* find this file */
  for (i=0; i<(int)ARR_SIZE(pbm_file_to_uim_file); i++)
  {
    for (j=0; j<ARR_SIZE(pbm_file_to_uim_file[0].uim_filename); j++)
    {
      if (uim_file == pbm_file_to_uim_file[i].uim_filename[j])
      {
        return pbm_file_to_uim_file[i].pbm_file_group[USE_USIM(sim_id)];
      }
    }
  }

  PBM_MSG_ERR("Could not find group for file %d, sim %d, USIM_LOCAL %d",
                uim_file, sim_id, (int)USE_USIM_LOCAL(sim_id));
  return PBM_FILE_NONE;
}

/*===========================================================================
FUNCTION PBM_ECC_BCD_TO_ASCII

DESCRIPTION
  This function converts an ECC number into an ASCII string.  Note that
  the input string must be at least 7 characters in length.  This function
  will always set the first 7 characters.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int pbm_ecc_bcd_to_ascii(const byte *ecc_number, char *ascii_out )
{
   int bcd_index;
   byte asc_1 = 0;
   byte asc_2 = 0;
   int returned_length = -1;
   int ascii_index = 0;

   PBM_CHECK_PTR_RET(ecc_number, 0);
   PBM_CHECK_PTR_RET(ascii_out, 0);

   /*******************************/
   /* Decode ECC's BCD into ascii */
   /*******************************/
   for( bcd_index = 0; bcd_index < 3; bcd_index++)
   {
      asc_1 = (ecc_number[bcd_index] & 0x0F);
      asc_2 = (ecc_number[bcd_index] & 0xF0) >> 4;

      if ((returned_length == -1) && ((ascii_out[ascii_index++] = pbm_bcd_to_char_mapping[asc_1]) == '\0'))
        returned_length = ascii_index - 1;
      if ((returned_length == -1) && ((ascii_out[ascii_index++] = pbm_bcd_to_char_mapping[asc_2]) == '\0'))
        returned_length = ascii_index - 1;
   }

   /* Null terminate the ascii string */
   ascii_out[ascii_index] = '\0';
   if (returned_length == -1) returned_length = ascii_index;

   return returned_length;
}


/*===========================================================================
FUNCTION PBM_INIT_UIM_ECC_LIST

DESCRIPTION
  This function is called at init to set up UIM ecc list

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
void pbm_init_uim_ecc_list (void)
{
  memset (pbm_ecc_list, 0, sizeof(pbm_ecc_list));
}

/*===========================================================================
FUNCTION PBM_INIT_ECC_INTERNAL

DESCRIPTION
  This function is called after a SIM_INSERTED event is received from GSDI.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
void pbm_init_ecc_internal ( pbm_slot_type sim_id )
{
  pbm_return_type pbm_status;         /* Status from GSDI calls. */
  char ascii_emergency_number[7];     /* Emergency number in ASCII */
  uint16 ascii_emergency_alpha[250];    /* Emergency number Alpha Identifier. */
  int app_index;                      /* Index thru UIM Applications */
#ifdef FEATURE_MMGSDI_UMTS
#ifdef FEATURE_PBM_USIM_SUPPORT
  byte emergency_category;            /* Emergency service category octet. */
  uint32 name_length;                 /* Length of NAME for USIM ECC */
#endif /* FEATURE_PBM_USIM_SUPPORT */
#endif
  int  data_length;                 /* Length of ASCII version of ECC # */
  boolean ecc_contains_data = FALSE;        /* Whether or not this UIM had an ECC */
  uint8 gsdi_card_apps;

  PBM_VALIDATE_SLOT_RET(sim_id, VOID);

  if (ecc_initialized & PBM_SIMID_TO_SLOT_MASK(sim_id))
    return;

  memset(ascii_emergency_alpha, 0, sizeof(ascii_emergency_alpha));

  /* Get the list of valid Applications for the current UIM */
  pbm_gsdi_get_apps_available(sim_id, &gsdi_card_apps);
  pbm_set_card_apps(sim_id, gsdi_card_apps);
  for (app_index = 0; app_index < MAX_APPS; app_index++)
  {
    /* This block of code checks to see if we have a card installed */
    int ecc_offset = 0;          /* Where in the ecc_list are we */

    /* Read ecc data from SIM/USIM */
    if (pbm_get_card_apps(sim_id) & pbm_ecc_card_app[app_index])
    {
      /* Based on the APPLICATION, get the correct ECC */
      switch (pbm_ecc_card_app[app_index])
      {
#ifdef FEATURE_MMGSDI_GSM
        case GSDI_GSM_SIM_MASK:
          pbm_status = pbm_gsdi_sim_get_ecc (sim_id,
                                              &pbm_ecc_list[sim_id][app_index]);
          break;
#endif /* FEATURE_MMGSDI_GSM */

#ifdef FEATURE_MMGSDI_UMTS
#ifdef FEATURE_PBM_USIM_SUPPORT
        case GSDI_USIM_APP_MASK:
          pbm_status = pbm_gsdi_usim_get_ecc (sim_id,
                                               &pbm_ecc_list[sim_id][app_index]);
          break;
#endif /* FEATURE_PBM_USIM_SUPPORT */
#endif /* FEATURE_MMGSDI_UMTS */

#ifdef FEATURE_MMGSDI_CDMA
        case GSDI_RUIM_APP_MASK:
          pbm_status = pbm_gsdi_ruim_get_ecc (sim_id,
                                               &pbm_ecc_list[sim_id][app_index]);
          break;
#endif /* FEATURE_MMGSDI_CDMA */
        default:
          pbm_status = PBM_ERROR;
      }

      if (pbm_status == PBM_SUCCESS)
      {
        if (pbm_ecc_list[sim_id][app_index].ef_exists)
        {
          /* This block of code checks to see the card type of card
           * currently installed and copies the correct number of bytes
           * into that structure.
           */
          data_length = 0;
          switch (pbm_ecc_card_app[app_index])
          {
#if defined (FEATURE_MMGSDI_GSM) || defined(FEATURE_MMGSDI_CDMA)
            case GSDI_GSM_SIM_MASK:
            case GSDI_RUIM_APP_MASK:
              for (ecc_offset = 0; ecc_offset < pbm_ecc_list[sim_id][app_index].len;
                   ecc_offset += ECC_LEN)
              {
                // The number is in each
                // pbm_ecc_list[sim_id][app_index].data[0][ecc_offset],
                // Build the number into an ASCII string.
                // Note that its almost a BCD, but is missing the length
                // which is fixed at 3 octets.
                data_length = pbm_ecc_bcd_to_ascii(
                    &pbm_ecc_list[sim_id][app_index].data[0][ecc_offset],
                    ascii_emergency_number);

                if (data_length > 0)
                {
                  ecc_contains_data = TRUE;
                }
                else
                {
                  PBM_MSG_HIGH("SIM ECC record %d had 0 length", ecc_offset, 0, 0);
                }

                // Put the information into a PBM record and save it to the cache.
                if (pbm_build_ecc_record(
                      (sim_id==PBM_SLOT_1?PBM_FIELD_SIM1_ECC:PBM_FIELD_SIM2_ECC),
                      data_length>0?(uint32)data_length + 1:0,
                      ascii_emergency_number, 0, NULL, FALSE, 0)!=PBM_SUCCESS)
                {
                  PBM_MSG_ERR("Could not build PBM record.", 0, 0, 0);
                }
              }
              break;
#endif /* (FEATURE_MMGSDI_GSM) || defined(FEATURE_MMGSDI_CDMA) */

#ifdef FEATURE_MMGSDI_UMTS
#ifdef FEATURE_PBM_USIM_SUPPORT
            case GSDI_USIM_APP_MASK:
              for (ecc_offset = 0;
                   ecc_offset < pbm_ecc_list[sim_id][app_index].num_of_recs;
                   ecc_offset++)
              {
                // The number is in each pbm_ecc_list[sim_id][app_index].data[ecc_offset][0],
                // Build the number into an ASCII string.
                // Per Spec, the first 3 octets are the emergency number.
                data_length = pbm_ecc_bcd_to_ascii(
                    &pbm_ecc_list[sim_id][app_index].data[ecc_offset][0],
                    ascii_emergency_number);

                if (data_length > 0)
                {
                  ecc_contains_data = TRUE;
                }
                else
                {
                  PBM_MSG_HIGH("SIM ECC record %d had 0 length", ecc_offset, 0, 0);
                }

                // Bytes 4 - pbm_ecc_list[sim_id][app_index].len - 1 are the Alpha Identifier
                if (pbm_ecc_list[sim_id][app_index].len > 4)
                {
                  uint8 *data_buffer =
                          &pbm_ecc_list[sim_id][app_index].data[ecc_offset][3];

                  // Convert the Alpha Identifier to a UCS2 string.
                  name_length = pbmconvert_sim_to_string(
                                        data_buffer,
                                        (pbm_ecc_list[sim_id][app_index].len - 4),
                                        ascii_emergency_alpha,
                                        ARR_SIZE(ascii_emergency_alpha));
                }
                else
                {
                  name_length = 0;
                }

                // The last octet is the Service Category
                emergency_category = pbm_ecc_list[sim_id][app_index].data[ecc_offset][pbm_ecc_list[sim_id][app_index].len - 1];

                /* Now build the PBM cache record and store in the cache. */
                if (pbm_build_ecc_record(((sim_id==PBM_SLOT_1)?PBM_FIELD_SIM1_ECC:PBM_FIELD_SIM2_ECC),
                                          data_length>0?(uint32)data_length + 1:0,
                                          ascii_emergency_number,
                                          sizeof(uint16) * (name_length?name_length + 1:0),
                                          ascii_emergency_alpha,
                                          TRUE,
                                          emergency_category) != PBM_SUCCESS)
                {
                  PBM_MSG_ERR("Could not build ECC record", 0, 0, 0);
                }
              }

              break;
#endif /* FEATURE_PBM_USIM_SUPPORT */
#endif /* FEATURE_MMGSDI_UMTS */

            default:
              break;
          }

        } /* pbm_ecc_list.ef_exists */
      }
    }
  }

  PBM_SIMSLOT_SET(ecc_initialized, sim_id);
  if (ecc_contains_data)
    PBM_SIMSLOT_SET(ecc_has_entries, sim_id);
}

/*===========================================================================
FUNCTION PBM_SEEK_UNUSED_RECORDS

DESCRIPTION
   Get the list of unused records via mmgsdi_seek(). These are returned via
   the pbm_mmgsdi_seek_cb().

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None
===========================================================================*/
static pbm_return_type pbm_seek_unused_records(pbm_slot_type sim_id,
                                               pbm_file_device_e_type file_id)
{
   pbm_cache_type_e_type cache_type;
   uint32 client_ref;

   PBM_MSG_HIGH("PBM starts SEEK of PBM file: sim %d, File id %d", sim_id, file_id, 0);

   /* Free data allocated for the previous file_id */
   pbm_free_seek_data(sim_id);
   pbm_init_seek_data(sim_id);

   cache_type = pbm_file_id_to_cache_type(file_id);

   switch (cache_type)
   {
      case PBM_CACHE_TYPE_PB:
#ifdef FEATURE_ECC_HANDLING_AT_PBM
      case PBM_CACHE_TYPE_ECC:
#endif
      {
         PB_CACHE *pb_cache = (PB_CACHE *)pbm_file_id_to_cache(sim_id, file_id);

         pbm_seek_data[sim_id].num_files      = pb_cache->num_files;
         pbm_seek_data[sim_id].num_of_records = pb_cache->num_of_records;
         pbm_seek_data[sim_id].record_length  = pb_cache->record_length;
         memcpy(pbm_seek_data[sim_id].filenames, pb_cache->filenames,
                sizeof(pbm_seek_data[sim_id].filenames));
         memcpy(pbm_seek_data[sim_id].records_in_pb_set, pb_cache->records_in_pb_set,
                sizeof(pbm_seek_data[sim_id].records_in_pb_set));
         break;
      }

#ifdef FEATURE_PBM_USIM_SUPPORT
      case PBM_CACHE_TYPE_USIM:
      {
         PB_USIM_CACHE *usim_cache =
            (PB_USIM_CACHE *)pbm_file_id_to_cache(sim_id, file_id);

         pbm_seek_data[sim_id].num_files      = 1;
         pbm_seek_data[sim_id].num_of_records = usim_cache->num_of_records;
         pbm_seek_data[sim_id].record_length  = usim_cache->record_length;
         pbm_seek_data[sim_id].filenames[PBM_USIM_PB_ONE] = usim_cache->filename;
         pbm_seek_data[sim_id].records_in_pb_set[PBM_USIM_PB_ONE] = usim_cache->num_of_records;
         break;
      }

      case PBM_CACHE_TYPE_INDEX:
      {
         PB_MASTER_USIM_CACHE *m_usim_cache =
            (PB_MASTER_USIM_CACHE *)pbm_file_id_to_cache(sim_id, file_id);

         pbm_seek_data[sim_id].num_files      = 1;
         pbm_seek_data[sim_id].num_of_records = m_usim_cache->iap_num_records;
         pbm_seek_data[sim_id].record_length  = m_usim_cache->iap_rec_len;
         pbm_seek_data[sim_id].filenames[PBM_USIM_PB_ONE] = m_usim_cache->filename;
         pbm_seek_data[sim_id].records_in_pb_set[PBM_USIM_PB_ONE] = m_usim_cache->iap_num_records;
         break;
      }

      case PBM_CACHE_TYPE_SYNC2:
      {
         PB_SYNC2_CACHE *sync2_cache =
            (PB_SYNC2_CACHE *) pbm_file_id_to_cache(sim_id, file_id);

         pbm_seek_data[sim_id].num_files      = sync2_cache->num_files;
         pbm_seek_data[sim_id].num_of_records = sync2_cache->num_of_records;
         pbm_seek_data[sim_id].record_length  = sync2_cache->record_length;
         memcpy(pbm_seek_data[sim_id].filenames, sync2_cache->filenames,
                sizeof(pbm_seek_data[sim_id].filenames));
         memcpy(pbm_seek_data[sim_id].records_in_pb_set, sync2_cache->records_in_pb_set,
                sizeof(pbm_seek_data[sim_id].records_in_pb_set));
         break;
      }
#endif /* FEATURE_PBM_USIM_SUPPORT */

      default:
         return PBM_ERROR;
   }

   pbm_alloc_seek_data(sim_id, pbm_seek_data[sim_id].num_of_records);

   client_ref = sim_id * PBM_FILE_MAX_REQUESTS + file_id;
   if (PBM_SUCCESS == pbm_send_seek_request(sim_id, client_ref))
   {
      return PBM_SUCCESS;
   }
   else
   {
      PBM_MSG_ERR("PBM SEEK failed of PBM file: sim %d, File id %d", sim_id, file_id, 0);

      /* SEEK failed, so try reading records sequentially */
      if (PBM_SUCCESS == pbm_start_record_read(sim_id, file_id, 1))
      {
         return PBM_SUCCESS;
      }
   }

   /* Something went wrong, start reading next file */
   return PBM_ERROR;
}

/*===========================================================================
FUNCTION PBM_START_RECORD_READ

DESCRIPTION
   This function is called after the SEEK operation is complete to start
   reading records

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_start_record_read(pbm_slot_type sim_id,
                                      pbm_file_device_e_type file_id,
                                      int record_num)
{
   pbm_cache_type_e_type cache_type = pbm_file_id_to_cache_type(file_id);

   switch (cache_type)
   {
     case PBM_CACHE_TYPE_PB:
        return pbm_sim_read(sim_id, file_id, record_num, pbm_sim_read_cb);
#ifdef FEATURE_ECC_HANDLING_AT_PBM
     case PBM_CACHE_TYPE_ECC:
        return pbm_sim_read(sim_id, file_id, record_num, NULL);
#endif

#ifdef FEATURE_PBM_USIM_SUPPORT
     case PBM_CACHE_TYPE_USIM:
        return pbm_usim_read(sim_id, file_id, record_num, pbm_usim_read_cb);

     case PBM_CACHE_TYPE_INDEX:
        return pbm_index_read(sim_id, file_id, record_num, pbm_index_read_cb);

     case PBM_CACHE_TYPE_SYNC2:
        return pbm_sync_read(sim_id, file_id, record_num, pbm_sync2_read_cb);
#endif /* FEATURE_PBM_USIM_SUPPORT */

     default:
       PBM_MSG_ERR("Unknown file id %d in sim %d", file_id, sim_id, 0);
       break;
   }

   return PBM_ERROR;
}

#endif /* FEATURE_MMGSDI */
#endif /* FEATURE_PBM_TASK */

