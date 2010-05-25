#ifndef _OI_L2CAP_QOS
#define _OI_L2CAP_QOS
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/oi_l2cap_qos.h#1 $ 

$Log: $

===========================================================================*/
/**
 * @file   
 * This file contains values from the flow specification for guaranteed Quality of Service.
 * 
 * Management of guaranteed quality of service at the L2CAP level is not supported
 * in the present BLUEmagic 3.0 software implementation of L2CAP. 
 * This file contains some of the infrastructure for management of L2CAP guaranteed
 * quality of service. The flow specification structures defined in this file do not 
 * presently have any functional effect at the L2CAP level. L2CAP management of 
 * guaranteed quality of service will be supported in a future version of BLUEmagic software.
 */
 
/**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2003 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/

/** structure indicating flow specification for an L2CAP connection */
typedef struct _OI_L2CAP_FLOWSPEC {

    /** level of service required */
    OI_UINT8   serviceType;

    /** rate at which traffic credits are granted in bytes/sec */
    OI_UINT32   tokenRate;

    /** size of the token bucket in bytes */
    OI_UINT32   tokenBucketSize;

    /** limit in bytes/second on how fast packets may be sent back-to-back from applications */
    OI_UINT32   peakBandwidth;

    /** maximum delay between tranmission by sender and tranmission over air, in microseconds */
    OI_UINT32   latency;

    /** difference between maximum and minimum delay that a packet will experience */
    OI_UINT32   delayVariation;

} OI_L2CAP_FLOWSPEC;

#define OI_L2CAP_SERVICE_TYPE_NO_TRAFFIC   0x00                             /**< service type: no traffic  */
#define OI_L2CAP_SERVICE_TYPE_BEST_EFFORT  0x01                             /**< service type: best effort  */
#define OI_L2CAP_SERVICE_TYPE_GUARANTEED   0x02                             /**< service type: guaranteed  */
#define OI_L2CAP_SERVICE_TYPE_DEFAULT      OI_L2CAP_SERVICE_TYPE_BEST_EFFORT   /**< The default service type is best effort.  */

#define OI_L2CAP_TOKEN_RATE_NONE           0                                /**< No token rate is specified.  */
#define OI_L2CAP_TOKEN_RATE_MAX            0xffffffff                       /**< Token rate is maximum available.  */
#define OI_L2CAP_TOKEN_RATE_DEFAULT        OI_L2CAP_TOKEN_RATE_NONE            /**< The default is for no token rate to be specified.  */

#define OI_L2CAP_TOKEN_BUCKET_NONE         0                                /**< No token bucket is needed.  */
#define OI_L2CAP_TOKEN_BUCKET_MAX          0xffffffff                       /**< Token bucket is maximum available.  */
#define OI_L2CAP_TOKEN_BUCKET_DEFAULT      OI_L2CAP_TOKEN_BUCKET_NONE          /**< The default is for no token rate to be specified.  */

#define OI_L2CAP_PEAK_BANDWIDTH_UNKNOWN    0                                /**< Maximum bandwidth is unknown. */
#define OI_L2CAP_PEAK_BANDWIDTH_DEFAULT    OI_L2CAP_PEAK_BANDWIDTH_UNKNOWN     /**< The default is for the maximum bandwidth setting to indicate that maximum bandwidth is unknown. */

#define OI_L2CAP_LATENCY_DONT_CARE         0xffffffff                       /**< The application does not care about latency. */
#define OI_L2CAP_LATENCY_DEFAULT           OI_L2CAP_LATENCY_DONT_CARE          /**< The default is for the latency setting to indicate that the application does not care about latency. */

#define OI_L2CAP_DELAY_VARIATION_DONT_CARE 0xffffffff                       /**< The application does not care about delay variation. */
#define OI_L2CAP_DELAY_VARIATION_DEFAULT   OI_L2CAP_DELAY_VARIATION_DONT_CARE  /**< The default is for the delay variation setting to indicate that the application does not care about delay variation. */

#endif /* _OI_L2CAP_QOS */
