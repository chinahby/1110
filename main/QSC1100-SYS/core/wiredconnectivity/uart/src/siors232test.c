/*===========================================================================

                       S I O R S 2 3 2 T E S T  . C
DESCRIPTION
  This file contains the functionality for echo-packet testing
  
   
  
Copyright (c) 2006-2008 by QUALCOMM Incorporated.  
All Rights Reserved.
Qualcomm Confidential and Proprietary

===========================================================================*/



/*===========================================================================

                      EDIT HISTORY FOR FILE

 
when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/06/08   rh      Correct the usage of diagdsm.h
08/07/07   rh      Added flow control base on watermark queue
02/15/07   rh      Move the UART logging feature here
05/01/06   rh      File creation - Adapted from USBTEST.C
09/27/06   rh      Add linear and AA/55 RX only test method
                               
===========================================================================*/

/*===========================================================================

                        INCLUDE FILES

===========================================================================*/



#include "task.h"
#include "sio.h"
#include "rdevmap.h"
#include "msm.h"
#include "comdef.h"
#include "siors232test.h"
#ifdef SIORS_USES_DIAGDSM
#include "diagdsm.h"
#endif


/*===========================================================================
  
	    PART 1 - SIO ECHO TEST FUNCTIONS
   
===========================================================================*/



static void siors232test_rdm_event_cb(rdm_assign_status_type status,
                                 rdm_service_enum_type service,
                                 rdm_device_enum_type device);

static void siors232test_process_switch(void);

static void siors232test_sio_process_rx_packet(dsm_item_type **dsm_item_ptr);

static void siors232test_lowater_cb(void);
static void siors232test_hiwater_cb(void);

static dsm_watermark_type siors232test_wmq;

static q_type siors232test_echo_q;

static sio_open_type siors232test_echo_open_struct =
{
    SIO_NO_STREAM_ID,                           /* Stream ID Type.  Filled in
                                                   by SIO for internal use.    */
    SIO_STREAM_RXTX,                            /* Type of stream being opened.
                                                   Filled in by SIO for 
                                                   internal use only.          */
    SIO_DS_PKT_MODE,                            /* Stream Mode Type            */
    NULL,                                       /* Received Data Queue         */
    &siors232test_wmq,                          /* Transmit Data Queue         */
    SIO_BITRATE_115200,                         /* Bit-rate for reception      */
    SIO_BITRATE_115200,                         /* Bit-rate for transmission   */
    SIO_PORT_UART_MAIN,                         /* Port which needs to be used */
    FALSE,                                      /* Is tail character used ?    */
    '\0',                                       /* If tail character is used, 
                                                   this is the tail character. */
    siors232test_sio_process_rx_packet,         /* If non-NULL, this function 
                                                   will be called for each 
                                                   packet which is received.   */
    SIO_FCTL_OFF,                               /* TX flow control method      */
    SIO_CTSRFR_AUTO_FCTL                        /* RX flow control method      */
};

typedef enum
{
    STATE_RESET,
    STATE_INIT,
    STATE_DONE,
    STATE_CLOSE_INIT,
    STATE_CLOSE_STAGE2,
    STATE_NUMBER_OF
}siors232test_process_state;

typedef enum
{
    SIORS232TEST_LOOPBACK,                      /* Standard loopback test     */
    SIORS232TEST_LINEAR,                        /* Counting from 0x00 to 0xff */
    SIORS232TEST_AA55                           /* Send 0xAA follow by 0x55   */
}siors232test_test_type;

typedef struct 
{
     siors232test_test_type     test_type;
     uint32                     next_byte;
     uint32                     error_count;
}siors232test_test_state_type;

static siors232test_process_state current_state = STATE_DONE;
static siors232test_test_state_type rstest_test_type;

/*===========================================================================

FUNCTION siors232test_process

DESCRIPTION
  Process the state machine changes which initialize the loop back operation

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void siors232test_process_switch ()
{ 
    static rdm_service_enum_type old_service;

    switch (current_state)
    {
    case STATE_RESET:
        /* Setup port setting variable                                             */
        /* Set the bit-rate to maximum to increase the chance of a buffer overflow */
        /* Transmit flow control is turned off as PC do not use this signal        */
        /* Receive flow control is done through hardware                           */
        siors232test_wmq.current_cnt = 0;
        siors232test_wmq.dont_exceed_cnt = 10000;
        siors232test_wmq.hi_watermark = 8000;
        siors232test_wmq.lo_watermark = 3000;
        siors232test_wmq.hiwater_func_ptr = siors232test_hiwater_cb;
        siors232test_wmq.lowater_func_ptr = siors232test_lowater_cb;

        siors232test_echo_open_struct.rx_bitrate = SIO_BITRATE_3200000;
        siors232test_echo_open_struct.tx_bitrate = SIO_BITRATE_3200000;
        siors232test_echo_open_struct.port_id = SIO_PORT_UART_MAIN;
        siors232test_echo_open_struct.tx_flow = SIO_FCTL_OFF;
        siors232test_echo_open_struct.rx_flow = SIO_CTSRFR_AUTO_FCTL;
        /* Setup queue */
        (void) q_init( &siors232test_echo_q );
        siors232test_wmq.q_ptr = &siors232test_echo_q;

        old_service = rdm_get_service(RDM_UART1_DEV);
        if (old_service != RDM_NULL_SRVC)
        {
            rdm_assign_port (old_service, RDM_NULL_DEV, siors232test_rdm_event_cb);
            current_state = STATE_INIT;
            break;
        }                       
        /* Note, there is no break statement here                   */
        /* If there is no need to change port, run to the next case */
    case STATE_INIT:
        /* Open the sio port */
        sio_open(&siors232test_echo_open_struct);
        /* We are done */
        current_state = STATE_DONE;
        break;

    case STATE_CLOSE_INIT:
        sio_close(siors232test_echo_open_struct.stream_id, siors232test_process_switch);
        current_state = STATE_CLOSE_STAGE2;
        break;

    case STATE_CLOSE_STAGE2:
        if(old_service != RDM_NULL_SRVC)
        {
            rdm_assign_port(old_service, RDM_UART1_DEV, siors232test_rdm_event_cb);
            current_state = STATE_DONE;
        }
        break;

    case STATE_DONE:
        break;
    default:
        break;
    }
}



/*===========================================================================

FUNCTION siors232test_hiwater_cb

DESCRIPTION
  This callback happens when the watermark queue is too full, 
  the action is to disable inbound flow 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void siors232test_hiwater_cb()
{
	sio_ioctl_param_type ioctl_p;
	sio_ioctl(siors232test_echo_open_struct.stream_id, 
			  SIO_IOCTL_INBOUND_FLOW_DISABLE,
			  &ioctl_p);
}


/*===========================================================================

FUNCTION siors232test_lowater_cb

DESCRIPTION
  This callback happens when the watermark queue has more room,
  thus the inbound flow can be re-enabled

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void siors232test_lowater_cb()
{

	sio_ioctl_param_type ioctl_p;
	sio_ioctl(siors232test_echo_open_struct.stream_id, 
			  SIO_IOCTL_INBOUND_FLOW_ENABLE,
			  &ioctl_p);
}


/*===========================================================================

FUNCTION siors232test_rdm_event_cb

DESCRIPTION
  Process peripheral emulation switch state callback from rdm.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void siors232test_rdm_event_cb(rdm_assign_status_type status,
                                      rdm_service_enum_type service, 
                                      rdm_device_enum_type device)
{
    siors232test_process_switch();
}

/*===========================================================================

FUNCTION siors232test_sio_process_rx_packet

DESCRIPTION
  process_rx packet function for UART

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

NOTE
  When a packet is received, the transmit function is called right away.  
  To simulate a busy system and to cause FIFO overflow, place a breakpoint 
  on this function to delay the execution.

===========================================================================*/

static void siors232test_sio_process_rx_packet(dsm_item_type **dsm_item_ptr)
{
    uint32 i;

    switch (rstest_test_type.test_type) 
    {
    case SIORS232TEST_LOOPBACK:
        sio_transmit(siors232test_echo_open_struct.stream_id, *dsm_item_ptr);
        break;
    case SIORS232TEST_LINEAR:
        for(i=0;i<(*dsm_item_ptr)->used;i++)
        {
            if((*dsm_item_ptr)->data_ptr[i] != rstest_test_type.next_byte)
            {
                rstest_test_type.error_count++;
            }
            rstest_test_type.next_byte = (rstest_test_type.next_byte+1) & 0x0ff;
        }
        dsm_free_packet(dsm_item_ptr);
        break;
    case SIORS232TEST_AA55:
        for(i=0;i<(*dsm_item_ptr)->used;i++)
        {
            if((*dsm_item_ptr)->data_ptr[i] != rstest_test_type.next_byte)
            {
                rstest_test_type.error_count++;
            }
            if((*dsm_item_ptr)->data_ptr[i] == 0xAA)
            {
                rstest_test_type.next_byte = 0x55;
            }
            else
            {
                rstest_test_type.next_byte = 0xAA;
            }
        }
        dsm_free_packet(dsm_item_ptr);
        break;
    default:
        /* Free the packet to prevent memory leaks */
        dsm_free_packet(dsm_item_ptr);
        break;
    }
}  

/*===========================================================================

FUNCTION siors232test_echo_close                        EXTERNALIZED FUNCTION

DESCRIPTION
  Stop echo test process

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Should be called only once.

===========================================================================*/

void siors232test_echo_close(void)
{
    current_state = STATE_CLOSE_INIT;

    /* Call function to inititate closing of ports */
    siors232test_process_switch();
}                                     

/*===========================================================================
 
FUNCTION siors232test_echo_init                         EXTERNALIZED FUNCTION

DESCRIPTION
  Close the existing ports and open a specified port for echoing packets 
  from the host

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Should be called only once.

===========================================================================*/

void siors232test_echo_init(void)
{
    current_state = STATE_RESET;
    rstest_test_type.error_count = 0;
    rstest_test_type.next_byte = 0;
    rstest_test_type.test_type = SIORS232TEST_LOOPBACK;


    /* Call function to inititate closing of ports */
    siors232test_process_switch();
}                                     

/*===========================================================================
 
FUNCTION siors232test_rx_check_init                   EXTERNALIZED FUNCTION

DESCRIPTION
  Close the existing ports and open a specified port for echoing packets 
  from the host
  
PARAMETERS
  Two kind of test are defined, test_type = 1 for linear increment test,
  otherwise AA/55 test is used

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Should be called only once.

===========================================================================*/

void siors232test_rx_check_init(uint32 test_type) 
{
    current_state = STATE_RESET;
    rstest_test_type.error_count = 0;
    if(test_type == 1) {
        rstest_test_type.test_type = SIORS232TEST_LINEAR;
        rstest_test_type.next_byte = 0;
    }
    else {
        rstest_test_type.test_type = SIORS232TEST_AA55;
        rstest_test_type.next_byte = 0xAA;
    }

    /* Call function to inititate closing of ports */
    siors232test_process_switch();
}                                     

void siors232test_callback_func(void)
{
}

/*===========================================================================

FUNCTION siors232test_command                        EXTERNALIZED FUNCTION

DESCRIPTION
  Test different IOCTL commands

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/

void siors232test_command(char cmd)
{
	sio_ioctl_param_type ioctl_p;
	dsm_item_type *item_ptr;
	int i;
	switch(cmd)
	{
		case 'A':
			sio_ioctl(siors232test_echo_open_struct.stream_id, 
					  SIO_IOCTL_INBOUND_FLOW_DISABLE,
					  &ioctl_p);
			break;
		case 'B':
			sio_ioctl(siors232test_echo_open_struct.stream_id, 
					  SIO_IOCTL_INBOUND_FLOW_ENABLE,
					  &ioctl_p);
			break;
		case 'C':
	        item_ptr = dsm_new_buffer( DSM_DIAG_ITEM_POOL);
    	    for(i=0;i<40;i++)
        	{
				item_ptr->data_ptr[i] = 'A';
			}
			(item_ptr)->used = i;
        	sio_transmit(siors232test_echo_open_struct.stream_id, item_ptr);

	        item_ptr = dsm_new_buffer( DSM_DIAG_ITEM_POOL);
    	    for(i=0;i<40;i++)
        	{
				item_ptr->data_ptr[i] = 'B';
			}
			(item_ptr)->used = i;
        	sio_transmit(siors232test_echo_open_struct.stream_id, item_ptr);

			ioctl_p.record_flush_func_ptr = siors232test_callback_func;
			sio_ioctl(siors232test_echo_open_struct.stream_id, 
					  SIO_IOCTL_FLUSH_TX,
					  &ioctl_p);

	        item_ptr = dsm_new_buffer( DSM_DIAG_ITEM_POOL);
    	    for(i=0;i<40;i++)
        	{
				item_ptr->data_ptr[i] = 'C';
			}
			(item_ptr)->used = i;
        	sio_transmit(siors232test_echo_open_struct.stream_id, item_ptr);

	        item_ptr = dsm_new_buffer( DSM_DIAG_ITEM_POOL);
    	    for(i=0;i<40;i++)
        	{
				item_ptr->data_ptr[i] = 'D';
			}
			(item_ptr)->used = i;
        	sio_transmit(siors232test_echo_open_struct.stream_id, item_ptr);

	        item_ptr = dsm_new_buffer( DSM_DIAG_ITEM_POOL);
    	    for(i=0;i<40;i++)
        	{
				item_ptr->data_ptr[i] = 'E';
			}
			(item_ptr)->used = i;
        	sio_transmit(siors232test_echo_open_struct.stream_id, item_ptr);

			ioctl_p.record_flush_func_ptr = siors232test_callback_func;
			sio_ioctl(siors232test_echo_open_struct.stream_id, 
					  SIO_IOCTL_FLUSH_TX,
					  &ioctl_p);
			break;
		case 'D':
			sio_ioctl(siors232test_echo_open_struct.stream_id, 
					  SIO_IOCTL_POWERDOWN,
					  &ioctl_p);
			break;
		case 'E':
			sio_ioctl(siors232test_echo_open_struct.stream_id, 
					  SIO_IOCTL_WAKEUP,
					  &ioctl_p);
			break;
		case 'F':
	        item_ptr = dsm_new_buffer( DSM_DIAG_ITEM_POOL);
    	    for(i=0;i<10;i++)
        	{
				item_ptr->data_ptr[i] = 'A';
			}
			(item_ptr)->used = i;
        	sio_transmit(siors232test_echo_open_struct.stream_id, item_ptr);

	        item_ptr = dsm_new_buffer( DSM_DIAG_ITEM_POOL);
    	    for(i=0;i<10;i++)
        	{
				item_ptr->data_ptr[i] = 'B';
			}
			(item_ptr)->used = i;
        	sio_transmit(siors232test_echo_open_struct.stream_id, item_ptr);

	        item_ptr = dsm_new_buffer( DSM_DIAG_ITEM_POOL);
    	    for(i=0;i<10;i++)
        	{
				item_ptr->data_ptr[i] = 'C';
			}
			(item_ptr)->used = i;
        	sio_transmit(siors232test_echo_open_struct.stream_id, item_ptr);

	        item_ptr = dsm_new_buffer( DSM_DIAG_ITEM_POOL);
    	    for(i=0;i<10;i++)
        	{
				item_ptr->data_ptr[i] = 'D';
			}
			(item_ptr)->used = i;
        	sio_transmit(siors232test_echo_open_struct.stream_id, item_ptr);

	        item_ptr = dsm_new_buffer( DSM_DIAG_ITEM_POOL);
    	    for(i=0;i<10;i++)
        	{
				item_ptr->data_ptr[i] = 'E';
			}
			(item_ptr)->used = i;
        	sio_transmit(siors232test_echo_open_struct.stream_id, item_ptr);
			break;
		default:
			break;
	}
}

