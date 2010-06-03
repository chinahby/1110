#ifndef _OEMSOCK_PRIV_H_
#define _OEMSOCK_PRIV_H_

#if defined AEE_SIMULATOR
# include "BREWSimFeatures.h"  // Simulator Feature Definitions
#endif

/* turns me on and off */
#include "OEMFeatures.h"


// Data dedicated transmission mode for hybrid HDR (MSM5500/6500)
#if defined(FEATURE_DDTM_CNTL)
# include "comdef.h"
# include "cm.h"
#endif

#include "dssocket.h"
#ifndef FEATURE_DATA_MM
# include "dssnet.h"
#else
# include "dserrno.h"
# include "dss_iface_ioctl.h"
#endif /* FEATURE_DATA_MM */

// Multiple PDP context support
#if defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS)
#define FEATURE_DATA_UMTS
#include "dsumtspdpreg.h"
#endif // defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS)

#ifdef FEATURE_BCMCS
#define FEATURE_DATA_BCMCS
#endif // FEATURE_BCMCS

// Don't simulate these
#if !defined(AEE_SIMULATOR)
# define IPCP_DNS_SUPPORTED
# if !defined(FEATURE_DATA_UMTS)
#  if defined(T_MSM6100)
#   include "ds707.h"             // for DS_MDR_MODE_*
#  else
#   include "dsi.h"
#  endif
#  define FAILOVER_3G_TO_2G
#if ((defined DSS_VERSION) && (DSS_VERSION >= 900))
#include "dsrlp.h"
#endif
#  define RLP3_CFG_SUPPORTED
#  define SDB_SUPPORTED
# endif // !defined(FEATURE_DATA_UMTS)
#endif // !defined(AEE_SIMULATOR)

// SDB should be simulated. Support is determined in runtime per
// simulator settings.
#if defined(AEE_SIMULATOR)
#define SDB_SUPPORTED
#endif //defined(AEE_SIMULATOR)

// Simulator and 5100 NT3503 provide server sockets
#if defined(AEE_SIMULATOR) || defined(T_MSM_5100) && !defined(T_MSM5100_ND2175) && !defined(T_MSM5100_AT1007) || defined(T_MSM6250)
# define OEM_PROVIDES_SERVER_SOCKETS // listen, accept, TCP bind, ephemeral ports
#endif // defined(AEE_SIMULATOR) || defined(T_MSM_5100) && !defined(T_MSM5100_ND2175) && !defined(T_MSM5100_AT1007) || defined(T_MSM6250)

// Simulator and 6100/6250 provide a loopback interface
#if defined(AEE_SIMULATOR) || defined(T_MSM6100) || defined(T_MSM6250)
# define OEM_PROVIDES_LOOPBACK
#endif // defined(AEE_SIMULATOR) || defined(T_MSM6100) || defined(T_MSM6250)

// Simulator and 6100/6250 always send FD_CLOSE event
#if ( !defined(AEE_SIMULATOR) && !defined(T_MSM6100) && !defined(T_MSM6250) ) || defined(T_MSM6100_5150)
# define DSS_FAKE_CLOSE
#endif // !defined(AEE_SIMULATOR) && !defined(T_MSM6100)

// include this last, as it depends on the above defines
#include "OEMDSS.h"         // backwards compatibility layer

// Determine if OEM provides QoS or not (based on AMSS DS API version)
#if ((defined DSS_VERSION) && (DSS_VERSION >= 800))
#define OEM_PROVIDES_QOS
#endif

// Determine if OEM provides Primary QoS or not (based on AMSS DS API version)
#if ((defined DSS_VERSION) && (DSS_VERSION >= 1400))
#define OEM_PROVIDES_PRIMARY_QOS
#endif

// Determine if OEM supports QOS Modify feature (based on AMSS DS API version)
// This backword compatibility is done via an OEM feature #define and not by adding 
// missing definitions in OEMDSS.h because the QOS Modify feature went through 
// some changes after it was added in DSS_VERSION 1100 and until the final version
// has been set in DSS_VERSION 1104.
// To avoid compensating for different definitions in the different versions 1100-1103
// we simply avoid using the QOS Modify definitions until DSS_VERSION 1104.
#if ((defined DSS_VERSION) && (DSS_VERSION >= 1104))
#define OEM_SUPPORTS_QOS_MODIFY
#endif

// Determine if OEM provides multicast or not (based on AMSS DS API version)
#if ((defined DSS_VERSION) && (DSS_VERSION >= 1100))
#define OEM_PROVIDES_MCAST
#endif

// Determine if OEM supports PPP termination with release reason
#if ( ( (defined DSS_VERSION) && (defined FEATURE_JCDMA_2) ) && \
      ( (DSS_VERSION >= 1303 && DSS_VERSION < 1400) ||(DSS_VERSION >= 1401) ) )
#define OEM_PROVIDES_TERMINATION_RELEASE_REASON
#endif

// Determine if the dss_iface_ioctl_bearer_tech_type struct contains the
// field so_mask
#if ((defined DSS_VERSION) && (DSS_VERSION >= 1600))
#define BEARER_TECH_CONTAINS_SO_MASK
#endif //((defined DSS_VERSION) && (DSS_VERSION >= 1600))

#endif // _OEMSOCK_PRIV_H_
