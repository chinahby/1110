
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*==

            T R A N S C E I V E R   R E S O U R C E   M A N A G E R

                Transceiver Resource Manager Configuration File

GENERAL DESCRIPTION

  This file supports configuration Transceiver Resource Manager


EXTERNALIZED FUNCTIONS

  None
  

REGIONAL FUNCTIONS

  None


INITIALIZATION AND SEQUENCING REQUIREMENTS

  None


Copyright (c) 2005 - 2008 by QUALCOMM, Inc.  All Rights Reserved.

=*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*==



===============================================================================

                           EDIT HISTORY FOR MODULE

$Header: //source/qcom/qct/modem/mcs/trm/main/latest/src/trmcfg.cpp#9 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when         who     what, where, why
----------   ---     ----------------------------------------------------------
10/21/2008   aps     Lower HDR Acquisition priority
09/18/2008   rkc     Added TRM_DEMOD_PAGE_CONTINUATION to trmcfg tables.
09/09/2008   sg      Fix lint/klocwork warnings
08/28/2008   adw     Added FEATURE_MCS_TRM to featurize TRM for ULC.
06/06/2008   sg      Added high priority BCMCS support.
05/22/2008   sg      Integrated changes from old depot 
09/21/2007   grl     Added full time SHDR support.
08/01/2007   ebb     Updated TRM_BYTES_PER_CLIENT
05/24/2007   ebb     Changed TRM_BYTES_PER_CLIENT to account for 64 bit additions
                     in client struct.
10/17/2006   cab     Added TRM_DEMOD_SMALL_SCI_PAGE reason.
                     1x acq and hi pri HDR traffic always compatible
09/25/2006   cab     Allowed 1x acq simultaneous with HDR high pri traffic
09/15/2006   cab     Integrated UMTS
07/25/2006   cab     Lowered HDR access priority to support access hybrid
06/07/2006   cab     Allowed GPS to run concurrently with HDR HPT
06/06/2006   pa      Featurize 1x acq on secondary chain for 6800B/7500B
06/06/2006   pa      Use lower priority than GPS for 1x acq on secondary chain.
06/06/2006   pa      Enable 1x acquisition on secondary chain
05/02/2006   cab     Added ifdef's for offline compilation and testing
04/20/2006   grl     Increased the priority of GPS above 1x paging for 6800.
12/02/2005   awj     Added priority for 1x secondary demod commands
11/21/2005   cab     Moved NV item id's to compile time assign outside library
11/08/2005   grl     Lowered priority of broadcast so that 1x acq can run.
10/21/2005   grl     Added SGPS-DO compatibility and HDR div compatibility.
09/10/2005   grl     Added broadcast access/traffic reasons.
08/29/2005   grl     Added support for dynamic compatibility masks.
07/19/2005   grl     Added support for SGPS+1x acq and removed S-idle-idle.
07/11/2005   grl     Added TRM config mask bit for SHDR enabling.
06/21/2005   ajn     Linting changes.
06/14/2005   ajn     Mode table compile-time initialized.
06/13/2005   grl     Added support for 1x to be active on both RX1 and TXRX1.
05/26/2005   ajn     Code review comment changes
03/28/2005   ajn     Added GPS_TDM reason, and client/reason compatibility
03/02/2005   ajn     Initial AMSS version

=============================================================================*/



/*=============================================================================

                           INCLUDE FILES FOR MODULE

=============================================================================*/

#include "customer.h"

#ifdef FEATURE_MCS_TRM

#include "trmcfg.h"

extern "C"
{
  #ifdef TRM_OFFLINE
    #include "trm_offline.h"
  #else
    #include "err.h"
  #endif
}



/*=============================================================================

                   DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, typesdefs,
and other items needed by this module.

=============================================================================*/

/*-----------------------------------------------------------------------------
  Locally defined TRM CFG related features
-----------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------------
  Number of clients in trm_client_enum_t
-----------------------------------------------------------------------------*/

#define TRM_NUM_CLIENTS       (TRM_LAST_CLIENT+1)

const uint32                  TRM::num_clients = TRM_NUM_CLIENTS;



/*-----------------------------------------------------------------------------
  NV item identifiers
-----------------------------------------------------------------------------*/

const nv_items_enum_type      TRM::nv_trm_config = NV_TRM_CONFIG_I;
const nv_items_enum_type      TRM::nv_gps_rf_config = NV_GPS_RF_CONFIG_I;



/*-----------------------------------------------------------------------------
  Client mode identifiers
-----------------------------------------------------------------------------*/

enum trm_mode_id_t
{
  /*-----------
    1X modes
  -----------*/

  /* General mode for no special restrictions */
  TRM_MODE_1X,

  /* Acquisition may be incompatible with GPS */
  TRM_MODE_1X_ACQ,

  /* 1X in diversity */
  TRM_MODE_1X_DIV,


  /*-----------
    HDR modes
  -----------*/

  /* General mode for no special restrictions */
  TRM_MODE_HDR,

  /* HDR in a traffic call */
  TRM_MODE_HDR_CONNECTED,

  /* HDR in broadcast */
  TRM_MODE_HDR_BROADCAST,

  /* Low latency traffic or high priority broadcast */
  TRM_MODE_HDR_LOW_LAT,

  /* HDR in idle  */
  TRM_MODE_HDR_IDLE,

  /* HDR in diversity */
  TRM_MODE_HDR_DIV,

  /* HDR in small SCI idle operation */
  TRM_MODE_HDR_SMALL_SCI_IDLE,

  /*-----------
    GPS modes
  -----------*/

  /* General mode for no special restrictions */
  TRM_MODE_GPS,

  /*-----------
    UMTS modes
  -----------*/

  /* General mode for no special restrictions */
  TRM_MODE_UMTS,

  /* WCDMA in diversity */
  TRM_MODE_UMTS_DIV,


  /* Number of client modes */
  TRM_NUM_MODES
};



/*-----------------------------------------------------------------------------
  Client mode id to bit-masks
-----------------------------------------------------------------------------*/

#define TRM_MASK_FROM_MODE(id)          ( 1 << id )

   
/*-----------------------------------------------------------------------------
  Returns the selected Simultaneous 1x HDR Mode
-----------------------------------------------------------------------------*/

#define TRMCFG_MSK_MODE_S1XHDR(mask)      \
  ( ( trmcfg_mode_s1xhdr_enum_type )( mask & 0x3 ) )


/*-----------------------------------------------------------------------------
  Define Simultaneous 1x HDR Modes from mask in NV_TRM_CONFIG_I
-----------------------------------------------------------------------------*/
typedef enum
{
  /* The default SHDR mode for the build is on */
  TRMCFG_MODE_S1XHDR_UNSET       = 0,
  
  /* SHDR is off. Normal hybrid mode is turned on */
  TRMCFG_MODE_S1XHDR_DISABLED    = 1,
  
  /* SHDR in Access, Traffic, and BCMCS is turned on */
  TRMCFG_MODE_S1XHDR_ENABLED     = 2,
  
  /* SHDR in BCMCS is turned on */
  TRMCFG_MODE_S1XHDR_BCMCS_ONLY  = 3,

  /* Number of possible modes */
  TRMCFG_MODE_S1XHDR_NUM
}
trmcfg_mode_s1xhdr_enum_type;

/* Feature Simultaneous 1x Paging and HDR BCMCS */
#ifdef FEATURE_HDR_SIMUL_1XPC_BCMCS
/* If feature defined, S1X/HDR mode is on for the BCMCS state */
#define TRMCFG_MODE_S1XHDR_DEFAULT TRMCFG_MODE_S1XHDR_BCMCS_ONLY
#else
/* The standard S1X/HDR mode is off, meaning legacy hybrid mode. */
#define TRMCFG_MODE_S1XHDR_DEFAULT TRMCFG_MODE_S1XHDR_DISABLED
#endif


/*-----------------------------------------------------------------------------
  Whether the given TRM config mask bit is on
-----------------------------------------------------------------------------*/

#define TRMCFG_MSK_BIT_ON(mask, bit)    ( mask & ( 1 << bit ) )


/*-----------------------------------------------------------------------------
  Define trm mask bits from NV_TRM_CONFIG_I
-----------------------------------------------------------------------------*/

/* Simultaneous 1x Paging and HDR Idle */
#define TRMCFG_MSK_1XIDLE_HDRIDLE_ENABLED_BIT    2

/* Simultaneous 1x Acq and HDR */
#define TRMCFG_MSK_1XACQ_HDR_ENABLED_BIT         3

/* Simultaneous GPS and HDR TC or Idle */
#define TRMCFG_MSK_HDR_GPS_ENABLED_BIT           4


/*-----------------------------------------------------------------------------
  TRM Library memory requirements
-----------------------------------------------------------------------------*/

/* The following value corresponds to the size of the TRMClient structure,
   declared in the trm.cpp library file, and must only be changed if the
   size of the structure in the library is changed. */
#ifdef TRM_OFFLINE
  #define TRM_BYTES_PER_CLIENT  150
#else
  #define TRM_BYTES_PER_CLIENT  76
#endif

/* Space required by all clients, in bytes */
#define TRM_BYTES             (TRM_BYTES_PER_CLIENT * TRM_NUM_CLIENTS)

/* Space required by all clients, in words. */
#define TRM_WORDS             ((TRM_BYTES+3)/4)



/*-----------------------------------------------------------------------------
  TRM Config static, non-constant data
-----------------------------------------------------------------------------*/

struct trmcfg_t
{
  /*-------------------------------------------------------------------------
    Mode to compatible modes table
  -------------------------------------------------------------------------*/
  
  trm_compatible_mask_t       compatible[ TRM_NUM_MODES ];


  /*-------------------------------------------------------------------------
    Memory buffer - Storage for TRM library, uint32 aligned
  -------------------------------------------------------------------------*/
  
  uint32                      buffer[ TRM_WORDS ];
};


/* TRMCFG's static, non-constant data storage */
static trmcfg_t               trmcfg;



/*-----------------------------------------------------------------------------
  TRM Memory Buffer
-----------------------------------------------------------------------------*/

/* Location and size of storage */
TRM::Memory                   TRM::memory =
{
  /* Location of storage */
  (uint8 *) trmcfg.buffer,

  /* Size of storage */
  sizeof(trmcfg.buffer)
};



/*-----------------------------------------------------------------------------
  Client log code

  These values are used by the TRM log parser to identify clients.
  The values must not change.
-----------------------------------------------------------------------------*/

enum trmlog_client_enum_t
{
  TRMLOG_1X                   = 0,
  TRMLOG_1X_SECONDARY         = 1,
  TRMLOG_HDR                  = 2,
  TRMLOG_HDR_SECONDARY        = 3,
  TRMLOG_GPS                  = 4,
  TRMLOG_UMTS                 = 5,
  TRMLOG_UMTS_SECONDARY       = 6,
  TRMLOG_MAX                  = TRMLOG_UMTS_SECONDARY
};



/*=============================================================================

                   CLIENT/REASON -> PRIORITY & MODE MAPPING

  Conditional compilation may be used to remove some of the following arrays,
  to conserve memory, if a specific target does not need them.

=============================================================================*/


/*-----------------------------------------------------------------------------
  Priority/Mode for a given client/reason pair
-----------------------------------------------------------------------------*/

typedef struct
{
  /* Priority for given client/reason */
  trm_pri_t                   pri;

  /* Mode for the the current client/reason */
  trm_mode_id_t               mode;
}
trmcfg_pri_mode_t;



/*-----------------------------------------------------------------------------
  TRM_1X 
-----------------------------------------------------------------------------*/

static const trmcfg_pri_mode_t  trmcfg_pri_mode_1x[TRM_NUM_REASONS] = 
{
  /* TRM_ACCESS        */             { 200, TRM_MODE_1X },
  /* TRM_ACCESS_URGENT */             { 200, TRM_MODE_1X },
  /* TRM_BROADCAST_ACCESS */          { 200, TRM_MODE_1X },
  /* TRM_LOCATION_FIX */              { BAD, TRM_MODE_1X },
  /* TRM_TRAFFIC */                   { 210, TRM_MODE_1X },
  /* TRM_DEMOD_PAGE */                { 150, TRM_MODE_1X },
  /* TRM_DEMOD_BROADCAST */           { 150, TRM_MODE_1X },
  /* TRM_DEMOD_QPCH */                { 150, TRM_MODE_1X },
  /* TRM_DEMOD_GPS */                 { BAD, TRM_MODE_1X },
  /* TRM_ACQUISITION */               { 110, TRM_MODE_1X_ACQ },
  /* TRM_OFS */                       { BAD, TRM_MODE_1X },
  /* TRM_DIVERSITY */                 { BAD, TRM_MODE_1X_DIV },
  /* TRM_GPS_TDM */                   { BAD, TRM_MODE_1X },
  /* TRM_BROADCAST_TRAFFIC */         { 210, TRM_MODE_1X },
  /* TRM_LOW_LAT_TRAFFIC */           { 210, TRM_MODE_1X },
  /* TRM_TOTAL_CONTROL */             { BAD, TRM_MODE_1X },
  /* TRM_SMALL_SCI_PAGE */            { BAD, TRM_MODE_1X },
#ifdef FEATURE_FULL_TIME_SHDR_3
  /* TRM_DEMOD_MAX_SENS */            { 150, TRM_MODE_1X },
#else
  /* TRM_DEMOD_MAX_SENS */            { BAD, TRM_MODE_1X },
#endif
  /* TRM_HP_BROADCAST */              { BAD, TRM_MODE_1X },
  /* TRM_DEMOD_PAGE_CONTINUATION */   { BAD, TRM_MODE_1X }
};


/*-----------------------------------------------------------------------------
  TRM_1X_SECONDARY 
-----------------------------------------------------------------------------*/

static const trmcfg_pri_mode_t  trmcfg_pri_mode_1x_secondary[TRM_NUM_REASONS] = 
{
  /* TRM_ACCESS        */             { 200, TRM_MODE_1X },
  /* TRM_ACCESS_URGENT */             { 200, TRM_MODE_1X },
  /* TRM_BROADCAST_ACCESS */          { 200, TRM_MODE_1X },
  /* TRM_LOCATION_FIX */              { BAD, TRM_MODE_1X },
  /* TRM_TRAFFIC */                   { BAD, TRM_MODE_1X },
#ifdef FEATURE_FULL_TIME_SHDR_3
  /* TRM_DEMOD_PAGE */                { 100, TRM_MODE_1X },
  /* TRM_DEMOD_BROADCAST */           { 100, TRM_MODE_1X },
  /* TRM_DEMOD_QPCH */                { 100, TRM_MODE_1X },
#else
  /* TRM_DEMOD_PAGE */                { 150, TRM_MODE_1X },
  /* TRM_DEMOD_BROADCAST */           { 150, TRM_MODE_1X },
  /* TRM_DEMOD_QPCH */                { 150, TRM_MODE_1X },
#endif /* FEATURE_FULL_TIME_SHDR_3 */
  /* TRM_DEMOD_GPS */                 { BAD, TRM_MODE_1X },
#if ( defined(T_MSM6800B) || defined(T_MSM7500B) )
  /* TRM_ACQUISITION */               { 152, TRM_MODE_1X_ACQ },
#else
  /* TRM_ACQUISITION */               { BAD, TRM_MODE_1X_ACQ },
#endif
  /* TRM_OFS */                       {  25, TRM_MODE_1X },
  /* TRM_DIVERSITY */                 {  15, TRM_MODE_1X_DIV },
  /* TRM_GPS_TDM */                   { BAD, TRM_MODE_1X },
  /* TRM_BROADCAST_TRAFFIC */         { BAD, TRM_MODE_1X },
  /* TRM_LOW_LAT_TRAFFIC */           { BAD, TRM_MODE_1X },
  /* TRM_TOTAL_CONTROL */             { BAD, TRM_MODE_1X },
  /* TRM_SMALL_SCI_PAGE */            { BAD, TRM_MODE_1X },
#ifdef FEATURE_FULL_TIME_SHDR_3
  /* TRM_DEMOD_MAX_SENS */            { 151, TRM_MODE_1X },
#else
  /* TRM_DEMOD_MAX_SENS */            { BAD, TRM_MODE_1X },
#endif
  /* TRM_HP_BROADCAST */              { BAD, TRM_MODE_1X },
  /* TRM_DEMOD_PAGE_CONTINUATION */   { BAD, TRM_MODE_1X }
};


/*-----------------------------------------------------------------------------
  TRM_HDR 
-----------------------------------------------------------------------------*/

static const trmcfg_pri_mode_t  trmcfg_pri_mode_hdr[TRM_NUM_REASONS] = 
{
  /* TRM_ACCESS        */             { 145, TRM_MODE_HDR_CONNECTED },
  /* TRM_ACCESS_URGENT */             { 160, TRM_MODE_HDR_CONNECTED },
  /* TRM_BROADCAST_ACCESS */          { 160, TRM_MODE_HDR_BROADCAST },
  /* TRM_LOCATION_FIX */              { BAD, TRM_MODE_HDR },
  /* TRM_TRAFFIC */                   { 105, TRM_MODE_HDR_CONNECTED },
  /* TRM_DEMOD_PAGE */                { 120, TRM_MODE_HDR_IDLE },
  /* TRM_DEMOD_BROADCAST */           { 105, TRM_MODE_HDR_BROADCAST },
  /* TRM_DEMOD_QPCH */                { BAD, TRM_MODE_HDR },
  /* TRM_DEMOD_GPS */                 { BAD, TRM_MODE_HDR },
  /* TRM_ACQUISITION */               { 95,  TRM_MODE_HDR },
  /* TRM_OFS */                       { BAD, TRM_MODE_HDR },
  /* TRM_DIVERSITY */                 { BAD, TRM_MODE_HDR_DIV },
  /* TRM_GPS_TDM */                   { BAD, TRM_MODE_HDR },
  /* TRM_BROADCAST_TRAFFIC */         { 105, TRM_MODE_HDR_BROADCAST },
  /* TRM_LOW_LAT_TRAFFIC */           { 160, TRM_MODE_HDR_LOW_LAT },
  /* TRM_TOTAL_CONTROL */             { BAD, TRM_MODE_HDR },
  /* TRM_SMALL_SCI_PAGE */            { 105, TRM_MODE_HDR_SMALL_SCI_IDLE },
  /* TRM_DEMOD_MAX_SENS */            { BAD, TRM_MODE_HDR },
  /* TRM_HP_BROADCAST */              { 160, TRM_MODE_HDR_LOW_LAT },
  /* TRM_DEMOD_PAGE_CONTINUATION */   { 155, TRM_MODE_HDR_IDLE }
};


/*-----------------------------------------------------------------------------
  TRM_HDR_SECONDARY 
-----------------------------------------------------------------------------*/

static const trmcfg_pri_mode_t  trmcfg_pri_mode_hdr_secondary[TRM_NUM_REASONS] = 
{
  /* TRM_ACCESS        */             { 145, TRM_MODE_HDR_CONNECTED },
  /* TRM_ACCESS_URGENT */             { 160, TRM_MODE_HDR_CONNECTED },
  /* TRM_BROADCAST_ACCESS */          { 160, TRM_MODE_HDR_BROADCAST },
  /* TRM_LOCATION_FIX */              { BAD, TRM_MODE_HDR },
  /* TRM_TRAFFIC */                   { BAD, TRM_MODE_HDR_CONNECTED },
  /* TRM_DEMOD_PAGE */                { BAD, TRM_MODE_HDR_IDLE },
  /* TRM_DEMOD_BROADCAST */           { BAD, TRM_MODE_HDR_BROADCAST },
  /* TRM_DEMOD_QPCH */                { BAD, TRM_MODE_HDR },
  /* TRM_DEMOD_GPS */                 { BAD, TRM_MODE_HDR },
  /* TRM_ACQUISITION */               { BAD, TRM_MODE_HDR },
  /* TRM_OFS */                       {  20, TRM_MODE_HDR },
  /* TRM_DIVERSITY */                 {  10, TRM_MODE_HDR_DIV },
  /* TRM_GPS_TDM */                   { BAD, TRM_MODE_HDR },
  /* TRM_BROADCAST_TRAFFIC */         { BAD, TRM_MODE_HDR_BROADCAST },
  /* TRM_LOW_LAT_TRAFFIC */           { BAD, TRM_MODE_HDR_LOW_LAT },
  /* TRM_TOTAL_CONTROL */             { BAD, TRM_MODE_HDR },
  /* TRM_SMALL_SCI_PAGE */            { BAD, TRM_MODE_HDR_IDLE },
  /* TRM_DEMOD_MAX_SENS */            { BAD, TRM_MODE_HDR },
  /* TRM_HP_BROADCAST */              { BAD, TRM_MODE_HDR_LOW_LAT },
  /* TRM_DEMOD_PAGE_CONTINUATION */   { BAD, TRM_MODE_HDR_IDLE }
};


/*-----------------------------------------------------------------------------
  TRM_GPS 
-----------------------------------------------------------------------------*/

static const trmcfg_pri_mode_t  trmcfg_pri_mode_gps[TRM_NUM_REASONS] = 
{
  /* TRM_ACCESS        */             { BAD, TRM_MODE_GPS },
  /* TRM_ACCESS_URGENT */             { BAD, TRM_MODE_GPS },
  /* TRM_BROADCAST_ACCESS */          { BAD, TRM_MODE_GPS },
#ifdef FEATURE_FULL_TIME_SHDR_3
  /* TRM_LOCATION_FIX */              { 140, TRM_MODE_GPS },
#elif defined( T_MSM6800 )
  /* TRM_LOCATION_FIX */              { 155, TRM_MODE_GPS },
#else
  /* TRM_LOCATION_FIX */              { 140, TRM_MODE_GPS },
#endif /* FEATURE_FULL_TIME_SHDR_3 */
  /* TRM_TRAFFIC */                   { BAD, TRM_MODE_GPS },
  /* TRM_DEMOD_PAGE */                { BAD, TRM_MODE_GPS },
  /* TRM_DEMOD_BROADCAST */           { BAD, TRM_MODE_GPS },
  /* TRM_DEMOD_QPCH */                { BAD, TRM_MODE_GPS },
#if ( defined(FEATURE_FULL_TIME_SHDR_2) || defined(FEATURE_FULL_TIME_SHDR_3) )
  /* TRM_DEMOD_GPS */                 { 155, TRM_MODE_GPS },
#else
  /* TRM_DEMOD_GPS */                 {   5, TRM_MODE_GPS },
#endif /* FEATURE_FULL_TIME_SHDR_[2|3]*/
  /* TRM_ACQUISITION */               { BAD, TRM_MODE_GPS },
  /* TRM_OFS */                       { BAD, TRM_MODE_GPS },
  /* TRM_DIVERSITY */                 { BAD, TRM_MODE_GPS },
  /* TRM_GPS_TDM */                   { 220, TRM_MODE_GPS },
  /* TRM_BROADCAST_TRAFFIC */         { BAD, TRM_MODE_GPS },
  /* TRM_LOW_LAT_TRAFFIC */           { BAD, TRM_MODE_GPS },
  /* TRM_TOTAL_CONTROL */             { BAD, TRM_MODE_GPS },
  /* TRM_SMALL_SCI_PAGE */            { BAD, TRM_MODE_GPS },
  /* TRM_DEMOD_MAX_SENS */            { BAD, TRM_MODE_GPS },
  /* TRM_HP_BROADCAST */              { BAD, TRM_MODE_GPS },
  /* TRM_DEMOD_PAGE_CONTINUATION */   { BAD, TRM_MODE_GPS }
};


/*-----------------------------------------------------------------------------
  TRM_UMTS 
-----------------------------------------------------------------------------*/

static const trmcfg_pri_mode_t  trmcfg_pri_mode_umts[TRM_NUM_REASONS] = 
{
  /* TRM_ACCESS        */             { BAD, TRM_MODE_UMTS },
  /* TRM_ACCESS_URGENT */             { BAD, TRM_MODE_UMTS },
  /* TRM_BROADCAST_ACCESS */          { BAD, TRM_MODE_UMTS },
  /* TRM_LOCATION_FIX */              { BAD, TRM_MODE_UMTS },
  /* TRM_TRAFFIC */                   { BAD, TRM_MODE_UMTS },
  /* TRM_DEMOD_PAGE */                { BAD, TRM_MODE_UMTS },
  /* TRM_DEMOD_BROADCAST */           { BAD, TRM_MODE_UMTS },
  /* TRM_DEMOD_QPCH */                { BAD, TRM_MODE_UMTS },
  /* TRM_DEMOD_GPS */                 { BAD, TRM_MODE_UMTS },
  /* TRM_ACQUISITION */               { BAD, TRM_MODE_UMTS },
  /* TRM_OFS */                       { BAD, TRM_MODE_UMTS },
  /* TRM_DIVERSITY */                 { BAD, TRM_MODE_UMTS_DIV },
  /* TRM_GPS_TDM */                   { BAD, TRM_MODE_UMTS },
  /* TRM_BROADCAST_TRAFFIC */         { BAD, TRM_MODE_UMTS },
  /* TRM_LOW_LAT_TRAFFIC */           { BAD, TRM_MODE_UMTS },
  /* TRM_TOTAL_CONTROL */             { 195, TRM_MODE_UMTS },
  /* TRM_SMALL_SCI_PAGE */            { BAD, TRM_MODE_UMTS  },
  /* TRM_DEMOD_MAX_SENS */            { BAD, TRM_MODE_UMTS },
  /* TRM_HP_BROADCAST */              { BAD, TRM_MODE_UMTS },
  /* TRM_DEMOD_PAGE_CONTINUATION */   { BAD, TRM_MODE_UMTS }
};


/*-----------------------------------------------------------------------------
  TRM_UMTS_SECONDARY 
-----------------------------------------------------------------------------*/

static const trmcfg_pri_mode_t  trmcfg_pri_mode_umts_secondary[TRM_NUM_REASONS] = 
{
  /* TRM_ACCESS        */             { BAD, TRM_MODE_UMTS },
  /* TRM_ACCESS_URGENT */             { BAD, TRM_MODE_UMTS },
  /* TRM_BROADCAST_ACCESS */          { BAD, TRM_MODE_UMTS },
  /* TRM_LOCATION_FIX */              { BAD, TRM_MODE_UMTS },
  /* TRM_TRAFFIC */                   { BAD, TRM_MODE_UMTS },
  /* TRM_DEMOD_PAGE */                { BAD, TRM_MODE_UMTS },
  /* TRM_DEMOD_BROADCAST */           { BAD, TRM_MODE_UMTS },
  /* TRM_DEMOD_QPCH */                { BAD, TRM_MODE_UMTS },
  /* TRM_DEMOD_GPS */                 { BAD, TRM_MODE_UMTS },
  /* TRM_ACQUISITION */               { BAD, TRM_MODE_UMTS },
  /* TRM_OFS */                       { BAD, TRM_MODE_UMTS },
  /* TRM_DIVERSITY */                 {  10, TRM_MODE_UMTS_DIV },
  /* TRM_GPS_TDM */                   { BAD, TRM_MODE_UMTS },
  /* TRM_BROADCAST_TRAFFIC */         { BAD, TRM_MODE_UMTS },
  /* TRM_LOW_LAT_TRAFFIC */           { BAD, TRM_MODE_UMTS },
  /* TRM_TOTAL_CONTROL */             { BAD, TRM_MODE_UMTS },
  /* TRM_SMALL_SCI_PAGE */            { BAD, TRM_MODE_UMTS },
  /* TRM_DEMOD_MAX_SENS */            { BAD, TRM_MODE_UMTS },
  /* TRM_HP_BROADCAST */              { BAD, TRM_MODE_UMTS },
  /* TRM_DEMOD_PAGE_CONTINUATION */   { BAD, TRM_MODE_UMTS }
};



/*=============================================================================

                             CLIENT CONSTANT DATA

=============================================================================*/

/*-----------------------------------------------------------------------------
  Client const data structure type
-----------------------------------------------------------------------------*/

typedef struct
{
  /* Log code for the client */
  trmlog_client_enum_t           log_code;

  /* Number of reasons */
  uint8                          reasons;

  /* Mode for the the current client/reason */
  const trmcfg_pri_mode_t *      pri_mode;
}
trmcfg_client_t;



/*-----------------------------------------------------------------------------
  Client const data structures

  Conditional compilation may be used to remove some of the following array
  structures, to conserve memory, if a specific target does not need them.
-----------------------------------------------------------------------------*/

static const trmcfg_client_t trmcfg_client[ TRM_NUM_CLIENTS ] =
{
  /* TRM_1X */
  {
    TRMLOG_1X,
    TRM_NUM_REASONS,
    trmcfg_pri_mode_1x
  },

  /* TRM_1X_SECONDARY */
  {
    TRMLOG_1X_SECONDARY,
    TRM_NUM_REASONS,
    trmcfg_pri_mode_1x_secondary
  },

  /* TRM_HDR */
  {
    TRMLOG_HDR,
    TRM_NUM_REASONS,
    trmcfg_pri_mode_hdr
  },

  /* TRM_HDR_SECONDARY */
  {
    TRMLOG_HDR_SECONDARY,
    TRM_NUM_REASONS,
    trmcfg_pri_mode_hdr_secondary
  },

  /* TRM_GPS */
  {
    TRMLOG_GPS,
    TRM_NUM_REASONS,
    trmcfg_pri_mode_gps
  },

  /* TRM_UMTS */
  {
    TRMLOG_UMTS,
    TRM_NUM_REASONS,
    trmcfg_pri_mode_umts
  },

  /* TRM_UMTS_SECONDARY */
  {
    TRMLOG_UMTS_SECONDARY,
    TRM_NUM_REASONS,
    trmcfg_pri_mode_umts_secondary
  }

};




/*=============================================================================

                            FUNCTION DECLARATIONS

=============================================================================*/




/*=============================================================================

FUNCTION TRM::pri

DESCRIPTION
  Return the priority for a given client/reason pair.
  
DEPENDENCIES
  None

RETURN VALUE
  Priority

SIDE EFFECTS
  None

=============================================================================*/

trm_pri_t TRM::pri
(
  /* Client requesting the priority */
  trm_client_enum_t           client,

  /* Reason for the client's request for an RF chain */
  trm_reason_enum_t           reason
)
{

/*---------------------------------------------------------------------------*/

  if ( reason < trmcfg_client[ client ].reasons )
  {
    return trmcfg_client[ client ].pri_mode[ reason ].pri;
  }
  else
  {
    ERR_FATAL("Client %d: Invalid reason %d", client, reason, 0);

    return BAD;
  }

} /* TRM::pri( trm_client_enum_t, trm_reason_enum_t) */



/*=============================================================================

FUNCTION TRM::get_compatibility

DESCRIPTION
  Retrieve the compatibility mask for a given client/reason pair.
  
DEPENDENCIES
  None

RETURN VALUE
  A bitmask representing all the compatibilities of a given client/reason pair.

SIDE EFFECTS
  None

=============================================================================*/

trm_compatible_mask_t TRM::get_compatibility
(
  /* Client requesting the compatibility masks */
  trm_client_enum_t           client,

  /* Reason for the client's request */
  trm_reason_enum_t           reason
)
{
  /* Operating "Mode" for the client/reason */
  trm_mode_id_t               mode_id;

  /* A bitmask representing the compatibility of the client/reason pair */
  trm_compatible_mask_t       compatibility = 0;

/*---------------------------------------------------------------------------*/

  /* Sanity check the reason */
  if ( reason < trmcfg_client[ client ].reasons )
  {
    /* Look up mode from client/reason pair */
    mode_id = trmcfg_client[ client ].pri_mode[ reason ].mode;

    /* Sanity check the mode_id */
    if ( mode_id >= TRM_NUM_MODES )
    {
      ERR_FATAL("Invalid mode_id %d for client %d", mode_id, client, 0 );
    }

    /* Look up compatible modes for the mode id */
    compatibility = trmcfg.compatible[ mode_id ];
  }
  else
  {
    ERR_FATAL("Client %d: Invalid reason %d", client, reason, 0); 
  }

  return compatibility;

} /* TRM::get_compatibility( client, reason ) */



/*=============================================================================

FUNCTION TRM::get_compatibility_mode

DESCRIPTION
  Retrieve the compatibility mode for a given client/reason pair.
  
DEPENDENCIES
  None

RETURN VALUE
  The compatibility mode of a given client/reason pair.

SIDE EFFECTS
  None

=============================================================================*/

trm_compatible_mask_t TRM::get_compatibility_mode
(
  /* Client requesting the compatibility masks */
  trm_client_enum_t           client,

  /* Reason for the client's request */
  trm_reason_enum_t           reason
)
{
  /* Operation "Mode" for the client/reason */
  trm_mode_id_t               mode_id;

  /* A bitmask representing the client mode */
  trm_compatible_mask_t       mode = 0;

/*---------------------------------------------------------------------------*/

  if ( reason < trmcfg_client[ client ].reasons )
  {
    /* Look up mode from client/reason pair */
    mode_id = trmcfg_client[ client ].pri_mode[ reason ].mode;

    /* Convert mode id into a mode bitmask */
    mode = TRM_MASK_FROM_MODE( mode_id );
  }
  else
  {
    ERR_FATAL("Client %d: Invalid reason %d", client, reason, 0); 
  }

  return mode;

} /* TRM::get_compatibility_mode( client, reason ) */



/*=============================================================================

FUNCTION TRM::client_log_id

DESCRIPTION
  Return the log code for a client.
  
DEPENDENCIES
  None

RETURN VALUE
  Priority

SIDE EFFECTS
  None

=============================================================================*/

uint8 TRM::client_log_id
(
  /* Client log code is needed for */
  trm_client_enum_t client
)
{

/*---------------------------------------------------------------------------*/

  return trmcfg_client[ client ].log_code;

} /* TRM::client_log_id( trm_client_enum_t ) */



/*=============================================================================

FUNCTION TRMCFG_SET_COMPATIBLE

DESCRIPTION
  Indicate modes are compatible (can run simultaneously) 
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

static void trmcfg_set_compatible
(
  /* Primary client identifier */
  trm_mode_id_t               mode_1_id,

  /* Secondary client identifier */
  trm_mode_id_t               mode_2_id
)
{

/*---------------------------------------------------------------------------*/

  /* Set mode_1 as compatible with mode_2, and vis-versa */
  trmcfg.compatible[ mode_1_id ] |= TRM_MASK_FROM_MODE( mode_2_id );
  trmcfg.compatible[ mode_2_id ] |= TRM_MASK_FROM_MODE( mode_1_id );

} /* trmcfg_set_compatible( mode_1_id, mode_2_id ) */

   

/*=============================================================================

FUNCTION TRMCFG_SET_INCOMPATIBLE

DESCRIPTION
  Indicate modes are uncompatible (can't run simultaneously) 
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

static void trmcfg_set_incompatible
(
  /* Primary client identifier */
  trm_mode_id_t               mode_1_id,

  /* Secondary client identifier */
  trm_mode_id_t               mode_2_id
)
{

/*---------------------------------------------------------------------------*/

  /* Set mode_1 as compatible with mode_2, and vis-versa */
  trmcfg.compatible[ mode_1_id ] &= ~TRM_MASK_FROM_MODE( mode_2_id );
  trmcfg.compatible[ mode_2_id ] &= ~TRM_MASK_FROM_MODE( mode_1_id );

} /* trmcfg_set_incompatible( mode_1_id, mode_2_id ) */



/*============================================================================

FUNCTION TRM::ENABLE_MODE

DESCRIPTION
  Enable the given TRM mode if possible.
    
DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/

void TRM::enable_mode
( 
  trm_mode_enum_t mode
)
{

/*--------------------------------------------------------------------------*/

  switch ( mode )
  {
    case TRM_MODE_SIMUL_1XIDLE_HDRTC:

      /* 1X (non-diversity) & HDR broadcast */
      trmcfg_set_compatible( TRM_MODE_1X,      TRM_MODE_HDR_BROADCAST );

      /* 1X (non-diversity) & HDR connected */
      trmcfg_set_compatible( TRM_MODE_1X,      TRM_MODE_HDR_CONNECTED );

      break;
  }

} /* TRM::enable_mode( mode ) */



/*============================================================================

FUNCTION TRM::DISABLE_MODE

DESCRIPTION
  Disable the given TRM mode.
  
DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/

void TRM::disable_mode
( 
  trm_mode_enum_t mode
)
{

/*--------------------------------------------------------------------------*/

  switch ( mode )
  {
    case TRM_MODE_SIMUL_1XIDLE_HDRTC:

      /* 1X (non-diversity) & HDR broadcast */
      trmcfg_set_incompatible( TRM_MODE_1X,      TRM_MODE_HDR_BROADCAST );

      /* 1X (non-diversity) & HDR connected */
      trmcfg_set_incompatible( TRM_MODE_1X,      TRM_MODE_HDR_CONNECTED );

      break;
  }

} /* TRM::disable_mode( mode ) */



/*=============================================================================

FUNCTION TRM::set_compatible_clients

DESCRIPTION
  Identify clients which can run in parallel using seperate RF chains 
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void TRM::set_compatible_clients
(
  /* Whether or not RF chains may be tuned independently */
  boolean                     independent_chains,

  /* TRM configuration bit mask */
  uint32                      mask
)
{
  /* Mode of s1x/hdr operation */
  trmcfg_mode_s1xhdr_enum_type  s1xhdr_mode;

/*---------------------------------------------------------------------------*/

  /*------------------------------------------------------------------------
    Set up 1X modes
  ------------------------------------------------------------------------*/

  /* Set up diversity */
  trmcfg_set_compatible( TRM_MODE_1X,      TRM_MODE_1X_DIV );
  trmcfg_set_compatible( TRM_MODE_1X_ACQ,  TRM_MODE_1X_DIV );


  /*------------------------------------------------------------------------
    Set up HDR modes
  ------------------------------------------------------------------------*/

  /* Set up diversity */
  trmcfg_set_compatible( TRM_MODE_HDR,                 TRM_MODE_HDR_DIV );
  trmcfg_set_compatible( TRM_MODE_HDR_CONNECTED,       TRM_MODE_HDR_DIV );
  trmcfg_set_compatible( TRM_MODE_HDR_BROADCAST,       TRM_MODE_HDR_DIV );
  trmcfg_set_compatible( TRM_MODE_HDR_LOW_LAT,         TRM_MODE_HDR_DIV );
  trmcfg_set_compatible( TRM_MODE_HDR_IDLE,            TRM_MODE_HDR_DIV );
  trmcfg_set_compatible( TRM_MODE_HDR_SMALL_SCI_IDLE,  TRM_MODE_HDR_DIV );


  /*------------------------------------------------------------------------
    Set up UMTS modes
  ------------------------------------------------------------------------*/

  /* Set up diversity */
  trmcfg_set_compatible( TRM_MODE_UMTS,      TRM_MODE_UMTS_DIV );


  /*------------------------------------------------------------------------
    Set up GPS modes
  ------------------------------------------------------------------------*/

  /* No-op */


  /*------------------------------------------------------------------------
    Supported simultaneous modes
  ------------------------------------------------------------------------*/

  if ( independent_chains )
  {
    /* All 1X modes are compatible with any other 1x mode. */
    trmcfg_set_compatible( TRM_MODE_1X,      TRM_MODE_1X );

    /* 1X (except Acq) & GPS - GPS has its own antenna */
    trmcfg_set_compatible( TRM_MODE_1X,      TRM_MODE_GPS );

    /* Grab the mode S1XHDR field from the TRM_CONFIG mask */
    s1xhdr_mode = TRMCFG_MSK_MODE_S1XHDR( mask );

    if ( s1xhdr_mode == TRMCFG_MODE_S1XHDR_UNSET )
    {
      /* If the mode has not been explicitly set, set the mode to the 
         compile time default mode. */
      s1xhdr_mode = TRMCFG_MODE_S1XHDR_DEFAULT;
    }

    /* Now set the compatibilities appropriately based on the SHDR mode
       selected. */

    /* Simultaneous 1x and BCMCS mode (Traffic uses hybrid mode) */
    if ( s1xhdr_mode == TRMCFG_MODE_S1XHDR_BCMCS_ONLY )
    {
      /* 1X (non-diversity) & HDR broadcast */
      trmcfg_set_compatible( TRM_MODE_1X,      TRM_MODE_HDR_BROADCAST );
    }
    /* Standard simultaneous 1x and HDR mode */
    else if ( s1xhdr_mode == TRMCFG_MODE_S1XHDR_ENABLED  )
    {
      /* 1X (non-diversity) & HDR broadcast */
      trmcfg_set_compatible( TRM_MODE_1X,      TRM_MODE_HDR_BROADCAST );

      /* 1X (non-diversity) & HDR connected */
      trmcfg_set_compatible( TRM_MODE_1X,      TRM_MODE_HDR_CONNECTED );
    }

    /* S1X and HDR idle is enabled. */
    if ( TRMCFG_MSK_BIT_ON( mask, TRMCFG_MSK_1XIDLE_HDRIDLE_ENABLED_BIT ) )
    {
      /* 1X (non-diversity) & HDR idle */
      trmcfg_set_compatible( TRM_MODE_1X,      TRM_MODE_HDR_IDLE );
    }

    /* S1X acq and HDR is enabled. */
    if ( TRMCFG_MSK_BIT_ON( mask, TRMCFG_MSK_1XACQ_HDR_ENABLED_BIT ) )
    {
      /* 1X acq & HDR idle */
      trmcfg_set_compatible( TRM_MODE_1X_ACQ,  TRM_MODE_HDR_IDLE );

      /* 1X acq & HDR connected */
      trmcfg_set_compatible( TRM_MODE_1X_ACQ,  TRM_MODE_HDR_CONNECTED);

      /* 1X acq & HDR broadcast */
      trmcfg_set_compatible( TRM_MODE_1X_ACQ,  TRM_MODE_HDR_BROADCAST);
    }

#if ( defined(FEATURE_FULL_TIME_SHDR) || defined(FEATURE_FULL_TIME_SHDR_2) || defined(FEATURE_FULL_TIME_SHDR_3) )

    /* 1X (non-diversity) & HDR broadcast */
    trmcfg_set_compatible( TRM_MODE_1X,      TRM_MODE_HDR_BROADCAST );

    /* 1X (non-diversity) & HDR connected */
    trmcfg_set_compatible( TRM_MODE_1X,      TRM_MODE_HDR_CONNECTED );

    /* 1x idle and HDR idle. */
    trmcfg_set_compatible( TRM_MODE_1X,      TRM_MODE_HDR_IDLE );

#endif /* FEATURE_FULL_TIME_SHDR[_2|3] */

    /* 1X acq & HDR high priority traffic or broadcast are always compatible. */
    trmcfg_set_compatible( TRM_MODE_1X_ACQ,  TRM_MODE_HDR_LOW_LAT);


    /* 1X & Low Latency traffic or broadcast are always compatible */
    trmcfg_set_compatible( TRM_MODE_1X,        TRM_MODE_HDR_LOW_LAT);

    /* All 1X modes are compatible with any other 1x mode. */
    trmcfg_set_compatible( TRM_MODE_1X,      TRM_MODE_1X );

    /* 1X (except Acq) & GPS */
    trmcfg_set_compatible( TRM_MODE_1X,      TRM_MODE_GPS );

    /* UMTS & GPS */
    trmcfg_set_compatible( TRM_MODE_UMTS,    TRM_MODE_GPS );

#ifdef T_MSM6800
    /* For MSM6800 GPS is not using SRCH4 engine, so it can operate in parallel
       with 1x Acq. */
    trmcfg_set_compatible( TRM_MODE_1X_ACQ,        TRM_MODE_GPS );

    /* SGPS + HDR is enabled. */
    trmcfg_set_compatible( TRM_MODE_HDR,                TRM_MODE_GPS );
    trmcfg_set_compatible( TRM_MODE_HDR_CONNECTED,      TRM_MODE_GPS );
    trmcfg_set_compatible( TRM_MODE_HDR_IDLE,           TRM_MODE_GPS );
    trmcfg_set_compatible( TRM_MODE_HDR_SMALL_SCI_IDLE, TRM_MODE_GPS );
    trmcfg_set_compatible( TRM_MODE_HDR_BROADCAST,      TRM_MODE_GPS );

    /* 1X (non-diversity) & HDR idle */
    trmcfg_set_compatible( TRM_MODE_1X,  TRM_MODE_HDR_SMALL_SCI_IDLE );
#endif

#ifdef FEATURE_GPS_HIGHEST_TRM_CHAIN1_PRIORITY
    trmcfg_set_compatible( TRM_MODE_HDR_LOW_LAT,        TRM_MODE_GPS );
#endif
  }

} /* TRM::set_compatible_clients( independent_chains ) */

#else /* FEATURE_MCS_TRM not defined, add external declaration to avoid 
         compiler warnings */

int mcs_no_external_declarations_in_trmcfg;

#endif /* FEATURE_MCS_TRM */
