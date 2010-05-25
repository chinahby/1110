/*===========================================================================

              D M S S   L O O P B A C K   I N T E R F A C E

          I N I T I A L I Z A T I O N   F U N C T I O N   A N D

        A C C E S S   C O N T R O L   L I S T   D E F I N I T I O N S
 
GENERAL DESCRIPTION
  The DMSS loopback interface allows applications on the phone to
  communicate with each other via DS sockets.
  The loopback interface is implemented as a ps_iface, i.e. the
  loopback traffic traverses the complete IP stack.

EXTERNALIZED FUNCTIONS
  lo_init()

INITIALIZATION AND SEQUENCING REQUIREMENTS
  lo_init() should be called as part of the PS initialization.

Copyright (c) 2002 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/27/03   om      Changed iface_create call to register ACL separately.
05/13/03   usb     Update to ACL rule names
05/02/03   usb     ps_iface_create() prototype change.
04/15/03   ss      Made the loop-back behavior asynchronous by putting the
                   data in a queue and signalling PS
02/12/03   om      Fixed lo ACL to include all local addresses.
11/01/02   om      Created Module.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "target.h"

#include "ps_acl.h"
#include "ps_aclrules.h"
#include "ps_meta_info.h"
#include "ps_iface.h"
#include "ps_route.h"
#include "ps_ip.h"
#include "ps_svc.h"

#include "ps_lo.h"


/*===========================================================================

                     DATA TYPES AND DEFINITIONS

===========================================================================*/
/*---------------------------------------------------------------------------
  The loopback interface
---------------------------------------------------------------------------*/
ps_iface_type lo_iface;

/*---------------------------------------------------------------------------
  The loopback interface's ACL
---------------------------------------------------------------------------*/
acl_type lo_acl;

/*---------------------------------------------------------------------------
  The loopback interface's packet counter
---------------------------------------------------------------------------*/
uint32 lo_pkt_cnt = 0;

/*---------------------------------------------------------------------------
  Queue to put the tx data to be processed by PS
---------------------------------------------------------------------------*/
q_type lo_tx_data_q;

/*---------------------------------------------------------------------------
  Forward declaration to use the fastest address lookup per interface
---------------------------------------------------------------------------*/
extern ps_iface_type *global_iface_ptr_array[MAX_SYSTEM_IFACES];


/*= = = = = = = =  = = = = = = = = = =  = = =  = = = = = = = = = = = = = = =
                    INTERNAL FUNCTION DEFINTIONS
= = = = = = = = = = = = = = =  = = = = =  = = = = = = = = = = = = = = = = =*/
/*===========================================================================
FUNCTION LO_PROCESS_TX_DATA_Q()

DESCRIPTION
  This function processes the protocol packets that are sent over the local
  loop-back interface. Each item of the lo_tx_data_q is an IP packet. This 
  function a packet from the queue and loops it back by calling ip_input.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE: When lo_tx_data_q is empty, clear signal from set signal mask
  FALSE: Don't clear signal yet - wait for lo_tx_data_q to be emptied out

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean lo_process_tx_data_q
(
  ps_sig_enum_type sig,
  void* user_data_ptr
)
{
  dsm_item_type* item_ptr;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /*-------------------------------------------------------------------------
    Get the item from the lo_tx_data_q. If the queue is empty then then ask 
    for the signal to be removed.
  -------------------------------------------------------------------------*/
  item_ptr = (dsm_item_type *) q_get(&lo_tx_data_q);
  if (item_ptr == NULL)
  {
    return (TRUE);
  }

  /*-------------------------------------------------------------------------
    Simply loop the data up the stack through ip_input()
  -------------------------------------------------------------------------*/
  ip_input( &lo_iface,
            item_ptr,
            NULL,
            FALSE );

  return FALSE;
}

/*= = = = = = = =  = = = = = = = = = =  = = =  = = = = = = = = = = = = = = =
                    EXTERNAL FUNCTION DEFINTIONS
= = = = = = = = = = = = = = =  = = = = =  = = = = = = = = = = = = = = = = =*/
/*===========================================================================
FUNCTION LO_INIT()

DESCRIPTION
  This function is called at powerup (PS initialization) to create
  the loopback interface, bring it up, and add it's route.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void lo_init( void )
{
  ip_addr_type  my_addr;
  ip_addr_type  my_netmask;
  ip_addr_type  my_gateway;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Initialize the loopback tx data queue. The tx data is put in this queue
    and processed by the PS task.
  -------------------------------------------------------------------------*/
  q_init( &lo_tx_data_q );

  /*-------------------------------------------------------------------------
    Set the signal handler and enable the signal to process the tx data
  -------------------------------------------------------------------------*/
  ps_set_sig_handler( PS_LO_TX_DATA_Q_SIGNAL, lo_process_tx_data_q, NULL );
  ps_enable_sig( PS_LO_TX_DATA_Q_SIGNAL );

  /*-------------------------------------------------------------------------
    Initialize the loopback ACL 
  -------------------------------------------------------------------------*/
  lo_acl.acl_fptr = lo_rt_acl;
  lo_acl.if_ptr   = &lo_iface;

  /*-------------------------------------------------------------------------
    Initialize the loopback address info
  -------------------------------------------------------------------------*/
  my_addr.type       = IPV4_ADDR;
  my_addr.addr.v4    = 0x7f000001;
  my_netmask.type    = IPV4_ADDR;
  my_netmask.addr.v4 = htonl( 0xfffffff0 );
  my_gateway.type    = IPV4_ADDR;
  my_gateway.addr.v4 = htonl( 0x00000000 );

  /*-------------------------------------------------------------------------
    Create the loopback interface - Don't register the ACL here as
    the loopback and IPSec interface have to be in a specific order.
    They are explicitly registered by PS at powerup.
  -------------------------------------------------------------------------*/
  ps_iface_create( &lo_iface, LO_IFACE, NULL );

  /*-------------------------------------------------------------------------
    Set the lo iface's transmit function to loop back to IP
  -------------------------------------------------------------------------*/
  ps_iface_set_tx_function( &lo_iface,
                            lo_tx_cmd,
                            NULL );

  /*-------------------------------------------------------------------------
    Set the lo iface's IP address to 127.0.0.1 and bring it up
  -------------------------------------------------------------------------*/
  if ( ps_iface_set_addr( &lo_iface, &my_addr ) != 0 )
  {
    MSG_ERROR( "LO: Set address to 127.0.0.1 failed", 0, 0, 0 );
    return;
  }
  ps_iface_phys_link_state( &lo_iface ) = PHYS_LINK_UP;
  ps_iface_up_ind( &lo_iface );

  /*-------------------------------------------------------------------------
    Add the route for this iface
  -------------------------------------------------------------------------*/
  if ( route_add( ROUTE_HOST,
                  my_addr,
                  my_netmask,
                  my_gateway,
                  &lo_iface,
                  1,
                  NULL ) != 0 )
  {
    MSG_ERROR( "LO: Route add failed", 0, 0, 0 );
  }

  MSG_HIGH( "LO: iface created (0x%x)", &lo_iface, 0, 0 );

} /* lo_init() */

/*===========================================================================
FUNCTION LO_TX_CMD()

DESCRIPTION
  This function is used as transmit command for the loopback interface.
  It discards any meta info and send the IP datagramm back to the stack.

DEPENDENCIES
  None

PARAMETERS
  this_iface_ptr     - Pointer to transmitting interface
  pkt_chain_ptr      - Reference pointer to IP datagramm
  meta_info_ref_ptr  - Reference pointer to packet's meta info
  tx_cmd_info        - User data pointer (not used)

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
int lo_tx_cmd
(
  ps_iface_type*       this_iface_ptr,
  dsm_item_type**      pkt_chain_ptr,
  ps_meta_info_type**  meta_info_ref_ptr,
  void*                tx_cmd_info
)
{
  /*-------------------------------------------------------------------------
    No meta info is defined for the loopback interface, discard it.
  -------------------------------------------------------------------------*/
  PS_META_INFO_FREE( meta_info_ref_ptr );

  /*-------------------------------------------------------------------------
    Put the data in the queue and signal PS to processed it.
  -------------------------------------------------------------------------*/
  lo_pkt_cnt++;
  MSG_LOW( "LO: pkt in queue 0x%x (%d)", *pkt_chain_ptr, lo_pkt_cnt, 0 );
  q_put( &lo_tx_data_q, &((*pkt_chain_ptr)->link) );
  PS_SET_SIGNAL( PS_LO_TX_DATA_Q_SIGNAL );

  /*-------------------------------------------------------------------------
    Make sure the sender thinks the data is gone ...
  -------------------------------------------------------------------------*/
  *pkt_chain_ptr = NULL;
  
  return 0;

} /* lo_tx_cmd() */

/*===========================================================================

                    ACCESS CONTROL LIST DEFINITION

===========================================================================*/

ACL_DEF( lo_rt_acl )
ACL_START
{
  int i;

  PASS( ACL_IFNAME_CLASS,  REQUIRED_IFACE_IS( LO_IFACE ) );
  DENY( DST_ADDR_IS_IN( 0x00000000, 0xffffffff ) );
  PASS( ACL_IPADDR_CLASS,  DST_ADDR_IS_IN( IPV4( 127,0,0,0 ), 0xffffff00 ) );
  for ( i=0; i<MAX_SYSTEM_IFACES; i++ )
  {
      PASS( ACL_IPADDR_CLASS, 
            DST_ADDR_IS_IN( 
              PS_IFACE_IP_V4_ADDR( global_iface_ptr_array[i] ), 0xffffffff ) );
  }
  PASS( ACL_DEFAULT_CLASS, ALL );
}
ACL_END
