#ifndef _BTRCMSGI_H
#define _BTRCMSGI_H

#ifdef FEATURE_BT

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                    B L U E T O O T H    R F C O M M
           M U L T I P L E X E R    C O N T R O L    C H A N N E L

GENERAL DESCRIPTION
  This file contains the Bluetooth RFCOMM Multiplexer Control Channel (MCC)
  message definitions.

Copyright (c) 2000 by QUALCOMM Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSHeader:   O:/src/asw/COMMON/vcs/btrcmsgi.h_v   1.10   03 Apr 2002 14:08:50   propach
$Header: //source/qcom/qct/wconnect/bthost/core/rel/00.00.26/src/btrcmsgi.h#1 $ $DateTime:

$Log:   O:/src/asw/COMMON/vcs/btrcmsgi.h_v  $
 *
 *    #5         16 Mar 2005            DP
 * Improve Credit Based Flow Control (CBFC).
 *
 *    #4         8  Mar 2005            DP
 * Improved CBFC so it now uses actual space available in receive watermark
 * to determine number of transmit credits to give remote device.
 * 
 *    Rev 1.10   03 Apr 2002 14:08:50   propach
 * Enhanced modem status changed events.  Adjust CBFC values
 * if BT_SWDEV_TEST_FASTER is defined.
 * 
 *    Rev 1.9   30 Nov 2001 10:11:32   propach
 * Tuned CBFC based on interop.
 * 
 *    Rev 1.8   23 Oct 2001 16:52:06   propach
 * Added support for RFCOMM Credit Based Flow Control (CBFC).
 * 
 *    Rev 1.7   07 Sep 2001 16:10:02   propach
 * Improved support for SPP.
 * 
 *    Rev 1.6   29 Aug 2001 11:56:02   propach
 * Merged certification tip onto main tip.
 * 
 *    Rev 1.5.1.1   15 Aug 2001 13:36:30   propach
 * Fixed data bit definitions in RPN.
 * 
 *    Rev 1.5.1.0   29 Jun 2001 15:25:04   waynelee
 * No change.
 * 
 *    Rev 1.5   08 Jun 2001 15:04:40   propach
 * Added RFCOMM link negotiations and modem status lines for SPP certification.
 * 
 *    Rev 1.4   26 Mar 2001 18:15:40   propach
 * Added RC commands and events to support serial port emulation settings
 * and modem status reading/writing.
 * 
 *    Rev 1.3   16 Mar 2001 17:28:30   propach
 * Added infrastructure for SPP including initial support of
 * RPN and RLS and reworking PN signaling messages.
 * 
 *    Rev 1.2   02 Mar 2001 12:00:08   propach
 * Updated PN message for 1.1 support.
 * 
 *    Rev 1.1   26 Feb 2001 12:39:18   propach
 * Updated RC flow control.  Updated MSC and test cmd/resp.  Improved
 * QXDM message outputs.
 * 
 *    Rev 1.0   29 Jan 2001 17:09:28   propach
 * Initial revision.
 * 
 *    Rev 1.5   22 Nov 2000 11:45:22   sramacha
 * 
 *    Rev 1.4   28 Aug 2000 16:13:24   propach
 * Cleanup RFCOMM.  Add use of msg/err macros.  Add driver fault
 * recording and statistics gathering.
 *
 *    Rev 1.3   17 Aug 2000 13:45:50   propach
 * Added Event Control (EC).  Fixed minor problems in RFCOMM.
 *
 *    Rev 1.2   02 Aug 2000 09:40:46   propach
 * Added QBTS support for BT.  Added to API.
 *
 *    Rev 1.1   19 Jul 2000 17:26:48   propach
 * Update BT API and featurization with cleanup.
 *
 *    Rev 1.0   04 Jul 2000 19:22:00   ryuen
 * Initial revision.

===========================================================================*/

/*-------------------------------------------------------------------------*/
/*  RFCOMM frame definitions.                                              */
/*-------------------------------------------------------------------------*/

#define RC_EA_LAST_B 0x01  /*  Extended Address  */
#define RC_CR_CMD_B  0x02  /*  Command/Response  */


typedef BT_PACKED struct
{
  uint8  address;  /*  DLCI        */
  uint8  control;  /*  Frame Type  */
  uint8  length;
} bt_rc_frame_hdr_type;

typedef BT_PACKED struct
{
  uint8  address;  /*  DLCI                   */
  uint8  control;  /*  Frame Type             */
  uint8  length1;  /*  Extended length field  */
  uint8  length2;
} bt_rc_frame_hdre_type;

#define BT_RCFHE_EXTRA_BYTES ( sizeof( bt_rc_frame_hdre_type ) -  \
                               sizeof( bt_rc_frame_hdr_type ) )


#define bt_rc_cbfc_credits_type uint8


/*  Address field definitions.  */

#define RCFH_ADDR_DLCI_DIR_B 0x04  /*  Direction       */
#define RCFH_ADDR_DLCI_SC_M  0xF8  /*  Server Channel  */
#define RCFH_ADDR_DLCI_M     (RCFH_ADDR_DLCI_DIR_B | RCFH_ADDR_DLCI_SC_M)

#define RCFH_ADDR_2_DLCI( addr ) (((addr) & RCFH_ADDR_DLCI_M) >> 2)
#define RCFH_DLCI_2_ADDR( dlci ) ((((dlci) & (RCFH_ADDR_DLCI_M >> 2)) \
                                       << 2) | RC_EA_LAST_B)

#define RC_DLCI_MCC_V 0  /*  Multiplexer Control Channel  */

#define RCFH_ADDR_2_SC( addr ) (((addr) & RCFH_ADDR_DLCI_SC_M) >> 3)
#define RCFH_SC_2_ADDR( sc )   (((sc) & (RCFH_ADDR_DLCI_SC_M >> 3)) << 3)
#define RC_SERV_CHAN_MIN_V 1
#define RC_SERV_CHAN_MAX_V 30

#define RCFH_ADDR_2_DIR( addr ) (((addr) & RCFH_ADDR_DLCI_DIR_B) ?  \
                                             TRUE : FALSE)
#define RCFH_DIR_2_ADDR( dir )  ((dir) ? RCFH_ADDR_DLCI_DIR_B : 0x0)


/*  Control field definitions.  */

#define RCFH_CTRL_PF_B 0x10  /*  Poll/Final  */
#define RCFH_CTRL_2_PF( ctrl ) (((ctrl) & RCFH_CTRL_PF_B) ? TRUE : FALSE)

#define RCFH_CTRL_FT_M      0xEF  /*  Frame Type                           */
#define RCFH_CTRL_FT_SABM_V 0x2F  /*  Set Asynchronous Balanced Mode Cmd   */
                                  /*    Command                            */
#define RCFH_CTRL_FT_DISC_V 0x43  /*  Disconnect Command                   */
#define RCFH_CTRL_FT_UIH_V  0xEF  /* Unnumbered Information with Header    */
                                  /*    check Command/Response             */
#define RCFH_CTRL_FT_UA_V   0x63  /*  Unnumbered Acknowledgement Response  */
#define RCFH_CTRL_FT_DM_V   0x0F  /*  Disconnect Mode Response             */
/*  The item below is defined in 07.10 but not in RFCOMM.                  */
#define RCFH_CTRL_FT_UI_V   0x03  /*  Unnumbered Information Cmd/Response  */


/*  Length field definitions.  */

#define RCFH_LEN_MAX_NON_EXT 0x7F

#define RCFH_LEN_LSB_M 0xFE
#define RCFH_LEN_MSB_M 0xFF
#define RCFH_VAL1_2_LEN1( val ) ((((val & (RCFH_LEN_LSB_M >> 1))) << 1) \
                                        | RC_EA_LAST_B)


/*-------------------------------------------------------------------------*/
/*  Frame Check Sequence (FCS) related definitions.                        */
/*-------------------------------------------------------------------------*/
#define bt_rc_fcs_type uint8

#define BT_RC_FCS_BYTES_UIH     2  /*  Number of bytes over which FCS  */
#define BT_RC_FCS_BYTES_NON_UIH 3  /*    is calculated by frame type.  */


/*-------------------------------------------------------------------------*/
/*  Non-UIH frames include SABM, UA, DM, and DISC.                         */
/*-------------------------------------------------------------------------*/

typedef BT_PACKED struct
{
  bt_rc_frame_hdr_type  rcf_hdr;
  bt_rc_fcs_type        rcf_fcs;
} bt_rc_non_uih_frame_type;


/*-------------------------------------------------------------------------*/
/*  Multiplexter Control Channel message definitions                       */
/*-------------------------------------------------------------------------*/

typedef BT_PACKED struct
{
  uint8  type;
  uint8  length;
} bt_rc_mcc_hdr_type;

typedef BT_PACKED struct
{
  uint8  type;
  uint8  length1;  /*  Extended length field  */
  uint8  length2;
} bt_rc_mcc_hdre_type;

#define BT_RCMCCHE_EXTRA_BYTES ( sizeof( bt_rc_mcc_hdre_type ) -  \
                                 sizeof( bt_rc_mcc_hdr_type ) )

/*  Type field definitions.  */

#define RCMCCH_TYPE_MT_M       0xFC  /*  Message Type                      */
#define RCMCCH_TYPE_MT_TEST_V  0x20  /*  Test                              */
#define RCMCCH_TYPE_MT_FCON_V  0xA0  /*  Flow Control On                   */
#define RCMCCH_TYPE_MT_FCOFF_V 0x60  /*  Flow Control Off                  */
#define RCMCCH_TYPE_MT_NSC_V   0x10  /*  Non Supported Command (response)  */
#define RCMCCH_TYPE_MT_MSC_V   0xE0  /*  Modem Status Command              */
#define RCMCCH_TYPE_MT_PN_V    0x80  /*  Parameter Negotiation             */
#define RCMCCH_TYPE_MT_RPN_V   0x90  /*  Remote Port Negotiation           */
#define RCMCCH_TYPE_MT_RLS_V   0x50  /*  Remote Line Status                */
/*  The ones below are defined in 07.10 but not used in RFCOMM             */
#define RCMCCH_TYPE_MT_PSC_V   0x40  /*  Power Saving Control              */
#define RCMCCH_TYPE_MT_CLD_V   0xC0  /*  Multiplexer Close Down            */
#define RCMCCH_TYPE_MT_SNC_V   0xD0  /*  Service Negotiation               */

#define RCMCCH_MT_TO_TYPE( mt ) (mt | RC_EA_LAST_B)


/*  Length field definitions.  */

#define RCMCCH_LEN_MAX_NON_EXT 0x7F
#define RCMCCH_LEN_M           0xFE
#define RCMCCH_VALLSB_2_LENLSB( val ) ((((val & (RCMCCH_LEN_M >> 1)))  \
                                            << 1) | RC_EA_LAST_B)


/*  DLCI field within MCC messages definitions (MSC, RPN, RLS).  */

#define RCMCCM_MDLCI_2_DLCI( mdlci ) RCFH_ADDR_2_DLCI(mdlci)
#define RCMCCM_DLCI_2_MDLCI( dlci ) (RCFH_DLCI_2_ADDR(dlci) | 0x02)

#define RCMCCM_MDLCI_IS_OK( mdlci ) (                     \
            ( ((mdlci) & (RC_EA_LAST_B | 0x02)) ==        \
                 (RC_EA_LAST_B | 0x02)) ? TRUE : FALSE )


/*-------------------------------------------------------------------------*/
/*  Flow Control (FC) messages include FCon and FCoff.                     */
/*-------------------------------------------------------------------------*/

typedef BT_PACKED struct
{
  bt_rc_frame_hdr_type  rcf_hdr;
  bt_rc_mcc_hdr_type    mcc_hdr;
  bt_rc_fcs_type        rcf_fcs;
} bt_rc_mcc_fc_frame_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*  Modem Status Command (MSC) definitions.                                */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

typedef BT_PACKED struct
{
  uint8  dlci;
  uint8  v24_signals;
} bt_rc_mcc_msc_type;

typedef BT_PACKED struct
{
  bt_rc_mcc_hdr_type  mcc_hdr;
  bt_rc_mcc_msc_type  msc_body;
} bt_rc_mcc_msc_msg_type;

typedef BT_PACKED struct
{
  bt_rc_frame_hdr_type    rcf_hdr;
  bt_rc_mcc_msc_msg_type  msc_msg;
  bt_rc_fcs_type          rcf_fcs;
} bt_rc_mcc_msc_frame_type;

#define RCMCCM_MSC_V24_FC_FLOW_OFF_B  0x02

#define RCMCCM_MSC_V24_LOC_DEFAULT_V (            \
                 RC_EA_LAST_B |                   \
                 RCMCCM_MSC_V24_RTC_ASSERTED_B |  \
                 RCMCCM_MSC_V24_RTR_ASSERTED_B       )

#define RCMCCM_MSC_V24_FAR_DEFAULT_V (            \
                 RC_EA_LAST_B |                   \
                 RCMCCM_MSC_V24_RTR_ASSERTED_B       )

#define RCMCCM_MSC_V24_MS_SIGS_M (                \
                 RCMCCM_MSC_V24_RTC_ASSERTED_B |  \
                 RCMCCM_MSC_V24_RTR_ASSERTED_B |  \
                 RCMCCM_MSC_V24_IC_ASSERTED_B  |  \
                 RCMCCM_MSC_V24_DV_ASSERTED_B        )


/*  MSCE is an MSC Extended with the optional break signal field.  */

typedef BT_PACKED struct
{
  bt_rc_mcc_msc_type  msc_body;
  uint8               break_signals;
} bt_rc_mcc_msce_type;

typedef BT_PACKED struct
{
  bt_rc_mcc_hdr_type   mcc_hdr;
  bt_rc_mcc_msce_type  msce_body;
} bt_rc_mcc_msce_msg_type;

typedef BT_PACKED struct
{
  bt_rc_frame_hdr_type     rcf_hdr;
  bt_rc_mcc_msce_msg_type  msce_msg;
  bt_rc_fcs_type           rcf_fcs;
} bt_rc_mcc_msce_frame_type;

#define BT_RC_MSCE_EXTRA_BYTES ( sizeof( bt_rc_mcc_msce_type ) -  \
                                 sizeof( bt_rc_mcc_msc_type ) )


#define RCMCCM_MSCE_BRK_SIG_B 0x02  /*  Break Signal                   */
#define RCMCCM_MSCE_BRK_LEN_M 0xF0  /*  Break Length (units of 200ms)  */
#define RCMCCM_MSCE_BKS_2_LEN( bks ) ((bks & RCMCCM_MSCE_BRK_LEN_M) >> 4)
#define RCMCCM_MSCE_LEN_2_BKS( len ) \
                              ((len & (RCMCCM_MSCE_BRK_LEN_M >> 4)) << 4)


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*  DLC Parameter Negotiation (PN) definitions.                            */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

typedef BT_PACKED struct
{
  uint8   dlci;
  uint8   cl_field;
  uint8   priority;
  uint8   unused1;
  uint16  max_frame_size;
  uint8   unused2;
  uint8   k_field;
} bt_rc_mcc_pn_type;

typedef BT_PACKED struct
{
  bt_rc_mcc_hdr_type  mcc_hdr;
  bt_rc_mcc_pn_type   pn_body;
} bt_rc_mcc_pn_msg_type;

typedef BT_PACKED struct
{
  bt_rc_frame_hdr_type   rcf_hdr;
  bt_rc_mcc_pn_msg_type  pn_msg;
  bt_rc_fcs_type         rcf_fcs;
} bt_rc_mcc_pn_frame_type;


#define RCMCCM_PN_DLCI_M 0x3F

#define RCMCCM_PN_CL_M 0xF0
#define RCMCCM_PN_CLF_2_CLVAL( clf ) ((clf & RCMCCM_PN_CL_M) >> 4)
#define RCMCCM_PN_CLVAL_2_CLF( clval ) \
                              ((clval & (RCMCCM_PN_CL_M >> 4)) << 4)

#define RCMCCM_PN_CLVAL_CBFC_REQ_V 0xF  /*  Credit Based     */
#define RCMCCM_PN_CLVAL_CBFC_YES_V 0xE  /*    flow control.  */
#define RCMCCM_PN_CLVAL_CBFC_NO_V  0x0

#define RCMCCM_PN_PRIORITY_M 0x3F

#define RCMCCM_PN_K_M 0x07
#define RCMCCM_PN_KF_2_KVAL( kf )   (kf & RCMCCM_PN_K_M)
#define RCMCCM_PN_KVAL_2_KF( kval ) (kval & RCMCCM_PN_K_M)

/*  Credit Based Flow Control (CBFC).  */
#define BT_RC_CBFC_CREDITS_MIN_PN  3
#define BT_RC_CBFC_CREDITS_MAX_PN  7
#define BT_RC_CBFC_CREDITS_DEFAULT BT_RC_CBFC_CREDITS_MIN_PN
#define BT_RC_CBFC_CREDITS_MAX_LOC 100


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*  Non Supported Command (NSC) Response definitions.                      */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

typedef BT_PACKED struct
{
  uint8  cmd_type;
} bt_rc_mcc_nsc_type;

typedef BT_PACKED struct
{
  bt_rc_mcc_hdr_type  mcc_hdr;
  bt_rc_mcc_nsc_type  nsc_body;
} bt_rc_mcc_nsc_msg_type;

typedef BT_PACKED struct
{
  bt_rc_frame_hdr_type    rcf_hdr;
  bt_rc_mcc_nsc_msg_type  nsc_msg;
  bt_rc_fcs_type          rcf_fcs;
} bt_rc_mcc_nsc_frame_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*  Remote Port Negotiation (RPN) definitions.                             */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*  RPN is an RPN request with just the DLCI field.  */

typedef BT_PACKED struct
{
  uint8  dlci;
} bt_rc_mcc_rpn_type;
  
typedef BT_PACKED struct
{
  bt_rc_mcc_hdr_type  mcc_hdr;
  bt_rc_mcc_rpn_type  rpn_body;
} bt_rc_mcc_rpn_msg_type;

typedef BT_PACKED struct
{
  bt_rc_frame_hdr_type    rcf_hdr;
  bt_rc_mcc_rpn_msg_type  rpn_msg;
  bt_rc_fcs_type          rcf_fcs;
} bt_rc_mcc_rpn_frame_type;


/*  RPNE is an RPN request extended with the other fields.  */

typedef BT_PACKED struct
{
  bt_rc_mcc_rpn_type  rpn_body;
  uint8               baudrate;
  uint8               format;
  uint8               flow_ctrl;
  uint8               xon_char;
  uint8               xoff_char;
  uint16              param_mask;
} bt_rc_mcc_rpne_type;
  
typedef BT_PACKED struct
{
  bt_rc_mcc_hdr_type   mcc_hdr;
  bt_rc_mcc_rpne_type  rpne_body;
} bt_rc_mcc_rpne_msg_type;

typedef BT_PACKED struct
{
  bt_rc_frame_hdr_type     rcf_hdr;
  bt_rc_mcc_rpne_msg_type  rpne_msg;
  bt_rc_fcs_type           rcf_fcs;
} bt_rc_mcc_rpne_frame_type;

#define BT_RC_RPNE_EXTRA_BYTES ( sizeof( bt_rc_mcc_rpne_type ) -  \
                                 sizeof( bt_rc_mcc_rpn_type ) )



/*  Parameter Mask definitions.  */

#define RCMCCM_RPN_PM_BR_B      0x0001  /*  Bit Rate.            */
#define RCMCCM_RPN_PM_D_B       0x0002  /*  Data bits.           */
#define RCMCCM_RPN_PM_S_B       0x0004  /*  Stop bits.           */
#define RCMCCM_RPN_PM_P_B       0x0008  /*  Parity.              */
#define RCMCCM_RPN_PM_PT_B      0x0010  /*  Parity Type.         */
#define RCMCCM_RPN_PM_XON_B     0x0020  /*  XON character.       */
#define RCMCCM_RPN_PM_XOFF_B    0x0040  /*  XOFF character.      */
#define RCMCCM_RPN_PM_XO_IN_B   0x0100  /*  XON/XOFF on input.   */
#define RCMCCM_RPN_PM_XO_OUT_B  0x0200  /*  XON/XOFF on output.  */
#define RCMCCM_RPN_PM_RTR_IN_B  0x0400  /*  RTR on input.        */
#define RCMCCM_RPN_PM_RTR_OUT_B 0x0800  /*  RTR on output.       */
#define RCMCCM_RPN_PM_RTC_IN_B  0x1000  /*  RTC on input.        */
#define RCMCCM_RPN_PM_RTC_OUT_B 0x2000  /*  RTC on output.       */

#define RCMCCM_RPN_PM_DEFAULT_V ( RCMCCM_RPN_PM_BR_B      |  \
                                  RCMCCM_RPN_PM_D_B       |  \
                                  RCMCCM_RPN_PM_S_B       |  \
                                  RCMCCM_RPN_PM_P_B       |  \
                                  RCMCCM_RPN_PM_PT_B      |  \
                                  RCMCCM_RPN_PM_XON_B     |  \
                                  RCMCCM_RPN_PM_XOFF_B    |  \
                                  RCMCCM_RPN_PM_XO_IN_B   |  \
                                  RCMCCM_RPN_PM_XO_OUT_B  |  \
                                  RCMCCM_RPN_PM_RTR_IN_B  |  \
                                  RCMCCM_RPN_PM_RTR_OUT_B |  \
                                  RCMCCM_RPN_PM_RTC_IN_B  |  \
                                  RCMCCM_RPN_PM_RTC_OUT_B      )


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*  Remote Line Status (RLS) definitions.                                  */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

typedef BT_PACKED struct
{
  uint8  dlci;
  uint8  value1;
} bt_rc_mcc_rls_type;

typedef BT_PACKED struct
{
  bt_rc_mcc_hdr_type  mcc_hdr;
  bt_rc_mcc_rls_type  rls_body;
} bt_rc_mcc_rls_msg_type;

typedef BT_PACKED struct
{
  bt_rc_frame_hdr_type    rcf_hdr;
  bt_rc_mcc_rls_msg_type  rls_msg;
  bt_rc_fcs_type          rcf_fcs;
} bt_rc_mcc_rls_frame_type;


/*  Value Octet 1 definitions.  */

#define RCMCCM_RLS_V1_LS_ERR_B       0x01  /*  Line Status error present.  */
#define RCMCCM_RLS_V1_LSET_M         0x0E  /*  Line Status Error Type.     */
#define RCMCCM_RLS_V1_LSET_OVERRUN_V 0x02  /*  Overrun error.              */
#define RCMCCM_RLS_V1_LSET_PARITY_V  0x04  /*  Parity error.               */
#define RCMCCM_RLS_V1_LSET_FRAMING_V 0x08  /*  Framing error.              */

#define RCMCCM_RLS_V1_2_LEV( v1 ) (v1 & RCMCCM_RLS_V1_LSET_M)
#define RCMCCM_RLS_LEV_2_V1( lev ) ((lev & RCMCCM_RLS_V1_LSET_M) |  \
                                       RCMCCM_RLS_V1_LS_ERR_B)

#endif /* FEATURE_BT */

#endif /* _BTRCMSGI_H */

