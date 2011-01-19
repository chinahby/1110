/************************************************************************** 
*************************   Restricted access   *************************** 
*************************************************************************** 












This file must only be used for the development of the HSUSB 
driver for the AMSS / BREW SW baselines using the Jungo USB Stack.
This file must not be used in any way for the development of any
functionally equivalent USB driver not using the Jungo USB stack.

For any questions please contact: Sergio Kolor, Liron Manor,
Yoram Rimoni, Dedy Lansky.











************************************************************************** 
*************************   Restricted access   ************************** 
**************************************************************************/ 

/* Jungo Confidential, Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com */
/* Generic HID Class Driver API */

#ifndef _UW_HOST_HID_H_
#define _UW_HOST_HID_H_

#include "jtypes.h"
#include <uw_host_hid_common.h> /* Common HID Class Driver API */

/* Device operation flags:
 * NON_BLOCKING_READ:
 *     If set the application's read completion callback (notify_read_complete)
 *     is called whenever data is available from the device */
#define NON_BLOCKING_READ  0x1

/* Flags bit-mask */
typedef juint32_t flag_t;
/* Generic HID class driver API device context */
typedef void* hid_gen_dev_ctx_t;
/* Private HID class driver application data */
typedef void* hid_gen_app_ctx_t;

/************************************************************
 * HID Class Driver Application -> Generic HID Class Driver *
 ************************************************************/
/* HID class driver application notification callbacks structure */
/* Callback parameters:
 * app_ctx              - Private application context
 * hid_gen_dev_ctx      - Generic HID API device context
 * report_id            - Report ID
 * report_size          - Report size (in bytes) */
typedef struct
{
    /* Device attach callback
     * Requirement: Mandatory */
    jbool_t (*notify_attach)(hid_gen_app_ctx_t app_ctx,
        hid_gen_dev_ctx_t hid_gen_dev_ctx);
    /* Device detach callback
     * Requirement: Mandatory */
    void (*notify_detach)(hid_gen_app_ctx_t app_ctx,
        hid_gen_dev_ctx_t hid_gen_dev_ctx);
    /* Read completion callback
     * Requirement: Mandatory */
    void (*notify_read_complete)(hid_gen_app_ctx_t app_ctx, 
        hid_gen_dev_ctx_t hid_gen_dev_ctx, juint8_t report_id,
        juint32_t report_size);
} notify_t;

#define REPORT_POOL_SIZE 5

/****************************************************************
 * Generic HID Class Driver API -> HID Class Driver Application *
 ****************************************************************/
/* //////////////////////////////
     Device Operation Functions 
   ////////////////////////////// */

/*****************************************************************************\
 * hid_general_register: Registers an application with the generic HID class
 *                       driver API.
 *
 *                       If there are already attached devices when the
 *                       function is called, the function will call the
 *                       application's attach notification callback for each
 *                       of the attached devices.
 *                       
 *                       NOTE: This function must be called before any other
 *                             generic HID class driver function.
 * 
 * PARAMETERS: (IN) notify_callback - Pointer to an application notification
 *                                    callbacks structure.
 *             (IN) app_ctx         - Private application data to be passed
 *                                    as a parameter to the application's
 *                                    callback functions (see notify_t).
 *
 * RETURN VALUE: Zero on success;
 *               Error code on failure.
\*****************************************************************************/
jresult_t hid_general_register(notify_t *notify_callback,
    hid_gen_app_ctx_t app_ctx);

/*****************************************************************************\
 * hid_general_open: Enables and starts operating an attached HID device.
 * 
 * PARAMETERS: (IN) hid_gen_dev_ctx - Generic HID class driver API device
 *                                    context, as received in the
 *                                    notify_attach() callback.
 *             (IN) flags           - A bit-mask of device operation flags.
 *                                    Currently supported flags:
 *                                    - NON_BLOCKING_READ: Notifications on
 *                                      data availability will be sent to the
 *                                      application using the
 *                                      notify_read_complete() callback.
 *
 * RETURN VALUE: Generic HID class driver API device context, to be used in
 *               calls to the other generic HID class driver API, on success;
 *               NULL on failure.
\*****************************************************************************/
hid_gen_dev_ctx_t hid_general_open(hid_gen_dev_ctx_t hid_gen_dev_ctx,
    flag_t flags);

/*****************************************************************************\
 * hid_general_close: Disables and stops operating an attached HID device.
 * 
 * PARAMETERS: (IN) hid_gen_dev_ctx - Generic HID class driver API device
 *                                    context, as received from
 *                                    hid_general_open().
 * 
 * RETURN VALUE: Zero on success;
 *               Error code on failure.
\*****************************************************************************/
jresult_t hid_general_close(hid_gen_dev_ctx_t hid_gen_dev_ctx);

/*****************************************************************************\
 * hid_general_read: Returns the report data received from an open device.
 *              
 *                   If no data is available when the function is called:
 *                   - If the NON_BLOCKING_READ flag is turned on, the
 *                     function will return with the status JEAGAIN and the
 *                     application's notify_read_complete() callback function
 *                     will be called when data arrives.
 *                   - If the NON_BLOCKING_READ flag is turned off, the
 *                     read function will block until data arrives.
 *
 *                   NOTE: The function does NOT remove the report ID from the
 *                         report data.
 * 
 * PARAMETERS: (IN) hid_gen_dev_ctx - Generic HID class driver API device
 *                                    context, as received from
 *                                    hid_general_open().
 *             (OUT) read_buf       - Pointer to a data buffer to be filled with
 *                                    the read data.
 *             (IN)  report_id      - Report ID.
 * 
 * RETURN VALUE: Zero on success;
 *               Error code on failure.
\*****************************************************************************/
jresult_t hid_general_read(hid_gen_dev_ctx_t hid_gen_dev_ctx, void *read_buf, 
    juint8_t report_id);

/*****************************************************************************\
 * hid_general_write: Sends report data to an open device. 
 *                    The function will block until the data is sent.
 *
 *                    NOTE: If the report ID is different than 0 the
 *                          application MUST add a report ID in the
 *                          first byte of the report data.
 * 
 * PARAMETERS: (IN) hid_gen_dev_ctx - Generic HID class driver API device
 *                                    context, as received from
 *                                    hid_general_open().
 *             (OUT) write_buf      - Pointer to a data buffer containing the
 *                                    data to be written to the device.
 *             (IN)  report_id      - Report ID (used for size validation).
 *             (IN)  type           - Report type. Can be any of the following:
 *                                    - UHID_INPUT_REPORT
 *                                    - UHID_OUTPUT_REPORT
 *                                    - UHID_FEATURE_REPORT
 * 
 * RETURN VALUE: Zero on success;
 *               Error code on failure.
\*****************************************************************************/
jresult_t hid_general_write(hid_gen_dev_ctx_t hid_gen_dev_ctx, void *write_buf,
    juint8_t report_id, juint8_t type);

/*****************************************************************************\
 * hid_general_set_idle: Sends a Set_Idle Request to an open device.
 *
 * PARAMETERS: (IN) hid_gen_dev_ctx - Generic HID class driver API device
 *                                    context, as received from
 *                                    hid_general_open().
 *             (IN) report_id       - Report ID (0 = all report IDs). 
 *             (IN) idle_rate       - Idle rate (0 = infinite).
 *  
 * RETURN VALUE: Zero on success;
 *               Error code on failure.
\*****************************************************************************/
jresult_t hid_general_set_idle(hid_gen_dev_ctx_t hid_gen_dev_ctx,
    juint8_t report_id, juint8_t idle_rate);

/*****************************************************************************\
 * hid_general_get_idle: Sends a Get_Idle Request to an open device 
 *                       and returns the idle rate value.
 *
 * PARAMETERS: (IN) hid_gen_dev_ctx - Generic HID class driver API device
 *                                    context, as received from
 *                                    hid_general_open().
 *             (IN) report_id       - Report ID (0 = all report IDs). 
 *             (OUT) idle_rate      - Idle rate (0 = infinite).
 *  
 * RETURN VALUE: Zero on success;
 *               Error code on failure.
\*****************************************************************************/
jresult_t hid_general_get_idle(hid_gen_dev_ctx_t hid_gen_dev_ctx, 
    juint8_t report_id, juint8_t *idle_rate);

/*****************************************************************************\
 * hid_general_set_protocol: Sends a Set_Protocol Request to an open device.
 *
 * PARAMETERS: (IN) hid_gen_dev_ctx - Generic HID class driver API device
 *                                    context, as received from
 *                                    hid_general_open().
 *             (IN) protocol_value  - Protocol value.
 *                                    0 = Boot Protocol
 *                                    1 = Report Protocol
 *  
 * RETURN VALUE: Zero on success;
 *               Error code on failure.
\*****************************************************************************/
jresult_t hid_general_set_protocol(hid_gen_dev_ctx_t hid_gen_dev_ctx,
    juint8_t protocol_value);

/*****************************************************************************\
 * hid_general_get_protocol: Sends a Get_Protocol Request to an open device 
 *                           and returns the protocol value.
 *
 * PARAMETERS: (IN) hid_gen_dev_ctx - Generic HID class driver API device
 *                                    context, as received from
 *                                    hid_general_open().
 *             (OUT) protocol_value - Protocol value.
 *                                    0 = Boot Protocol
 *                                    1 = Report Protocol
 *  
 * RETURN VALUE: Zero on success;
 *               Error code on failure.
\*****************************************************************************/
jresult_t hid_general_get_protocol(hid_gen_dev_ctx_t hid_gen_dev_ctx,
    juint8_t *protocol_value);

/* /////////////////////////////////
     Report Manipulation Functions 
   ///////////////////////////////// */

/*****************************************************************************\
 * hid_general_is_collection: Checks if an open HID device's report descriptor
 *                            has the specified usage item.
 *                              
 *                            NOTE: This function can be used to verify the
 *                                  HID device's type (e.g. mouse, keyboard,
 *                                  gamepad, etc.).
 *                                    
 * PARAMETERS: (IN) hid_gen_dev_ctx - Generic HID class driver API device
 *                                    context, as received from
 *                                    hid_general_open().
 *             (IN) usage           - Usage code created using the HID_USAGE2
 *                                    macro.
 * 
 * RETURN VALUE: 1 if the report descriptor has the specified usage item;
 *               0 otherwise.
\*****************************************************************************/
jbool_t hid_general_is_collection(hid_gen_dev_ctx_t hid_gen_dev_ctx,
    juint32_t usage);

/*****************************************************************************\
 * hid_general_get_field_location: Returns the field location for a given
 *                                 usage code in the specified report.
 *                                 The field location should be used in calls
 *                                 to functions that require a field location
 *                                 parameter, such as
 *                                 hid_general_get_field_value() or
 *                                 hid_general_set_field_value().
 *
 *                                 NOTE: The returned field location must
 *                                       later be freed by the application
 *                                       using hid_general_free_location().
 *
 * PARAMETERS: (IN) hid_gen_dev_ctx - Generic HID class driver API device
 *                                    context, as received from
 *                                    hid_general_open().
 *             (IN)  usage          - Usage code created using the HID_USAGE2
 *                                    macro.
 *             (IN)  type           - Report type. Can be any of the following:
 *                                    - UHID_INPUT_REPORT
 *                                    - UHID_OUTPUT_REPORT
 *                                    - UHID_FEATURE_REPORT
 *             (IN)  report_id      - Report ID.
 *             (OUT) location       - Pointer to a pointer to the field 
 *                                    location, to be returned by the function.
 * 
 * RETURN VALUE: 0 if the specified usage code is found in the report;
 *               JENOENT otherwise.
\*****************************************************************************/
jresult_t hid_general_get_field_location(hid_gen_dev_ctx_t hid_gen_dev_ctx,
    juint32_t usage, juint8_t type, juint8_t report_id, void **location);

/*****************************************************************************\
 * hid_general_free_location: Frees field location resources previously
 *                            allocated using hid_general_get_field_location().
 *
 *                            NOTE: This function should be called from the
 *                                  application's device detach function for
 *                                  each allocated field location.
 *
 * PARAMETERS: (IN/OUT) location - Pointer to a pointer to field location
 *                                 resources received from a previous call
 *                                 to hid_general_get_field_location().
 *                                 The function resets the resources pointer
 *                                 to NULL.
 * 
 * RETURN VALUE:  None.
\*****************************************************************************/
void hid_general_free_location(void **location);

/*****************************************************************************\
 * hid_general_get_field_value: Returns the value of a given report field,
 *                              specified by its location
 *
 * PARAMETERS: (IN) location    - Pointer to the field location.
 *             (IN) report_data - Raw data received from a previous call to
 *                                hid_general_read().
 * 
 * RETURN VALUE: The value of the specified field.
\*****************************************************************************/
juint32_t hid_general_get_field_value(void *location, void* report_data);

/*****************************************************************************\
 * hid_general_set_field_value: Sets the value of a given report field,
 *                              specified by its location.
 *
 * PARAMETERS: (IN)  location    - Pointer to the field location.
 *             (OUT) report_data - A pre-allocated report data buffer.
 *                                 After the function successfully sets the
 *                                 specified field value in this buffer, the
 *                                 calling application should pass the buffer
 *                                 to hid_general_write() in order to send the
 *                                 report data to the relevant device.
 *             (IN)  value       - The field value to set.
 * 
 * RETURN VALUE: Zero on success;
 *               Error code on failure.
\*****************************************************************************/
jresult_t hid_general_set_field_value(void *location, void* report_data,
    juint32_t value);

/*****************************************************************************\
 * hid_general_is_location_of_usage: Verifies usage of a given report field,
 *                                   specified by its location.
 *
 * PARAMETERS: (IN) usage    - Expected usage code, created using the
 *                             HID_USAGE2 macro.
 *             (IN) location - Pointer to the field location.
 * 
 * RETURN VALUE: TRUE if the actual usage matches the input usage value;
 *               FALSE otherwise.
\*****************************************************************************/
jbool_t hid_general_is_location_of_usage(juint32_t usage, void *location);

/*****************************************************************************\
 * hid_general_get_att_from_location: Returns the usage attributes for a given
 *                                    report field, specified by its location.
 *
 * PARAMETERS: (IN)  location - Pointer to the field location.
 *             (OUT) att      - Pointer to usage attributes information.
 * 
 * RETURN VALUE: None
\*****************************************************************************/
void hid_general_get_att_from_location(void *location, usage_att_t *att);

/*****************************************************************************\
 * hid_general_get_report_size: Returns the size of the specified report.
 *
 * PARAMETERS: (IN) hid_gen_dev_ctx - Generic HID class driver API device
 *                                    context, as received from
 *                                    hid_general_open().
 *             (IN) report_id       - Report ID.
 *             (IN) type            - Report type. Can be any of the following:
 *                                    - UHID_INPUT_REPORT
 *                                    - UHID_OUTPUT_REPORT
 *                                    - UHID_FEATURE_REPORT
 * 
 * RETURN VALUE: The size of the report, in bytes.
\*****************************************************************************/
juint8_t hid_general_get_report_size(hid_gen_dev_ctx_t hid_gen_dev_ctx, 
    juint8_t report_id, juint8_t type);

/*****************************************************************************\
 * hid_general_get_usbd_handle: Get USB core device handle
 *
 * PARAMETERS: (IN) hid_gen_dev_ctx - Generic HID class driver API device
 *                                    context, as received from
 *                                    hid_general_open().
 * 
 * RETURN VALUE: Pointer to the USB core device handle
\*****************************************************************************/
void *hid_general_get_usbd_handle(hid_gen_dev_ctx_t hid_gen_dev_ctx);

/*****************************************************************************\
 * hid_general_suspend_device: Suspend the specific device
 *
 * PARAMETERS: (IN) hid_gen_dev_ctx - Generic HID class driver API device
 *                                    context, as received from
 *                                    hid_general_open().
 * 
 * RETURN VALUE: Zero on success;
 *               Error code on failure.
\*****************************************************************************/
jresult_t hid_general_suspend_device(hid_gen_dev_ctx_t hid_gen_dev_ctx);

/*****************************************************************************\
 * hid_general_resume_device: Resume the specific device
 *
 * PARAMETERS: (IN) hid_gen_dev_ctx - Generic HID class driver API device
 *                                    context, as received from
 *                                    hid_general_open().
 * 
 * RETURN VALUE: Zero on success;
 *               Error code on failure.
\*****************************************************************************/
jresult_t hid_general_resume_device(hid_gen_dev_ctx_t hid_gen_dev_ctx);

#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif /* FEATURE_BT_EXTPF_HID_HOST */

/* Begin – Qualcomm HSUSB addition */   
/*****************************************************************************\
 * hid_general_get_bootprotocol_location:  get the location of usage on boot protocol report descriptor
 *
 * PARAMETERS: (IN) bp_desc - boot protocol report descriptor
 *                  desc_size - the number of byte of boot protocol report descriptor, bp_desc
 *                  usage - usage to look for
 *                  type - type of usage
 *
 *             (OUT)location - location for the usage
 * 
 * RETURN VALUE: Zero on success;
 *               Error code on failure.
\*****************************************************************************/
jresult_t hid_general_get_bootprotocol_location(
  void * bp_desc,
  juint16_t desc_size,
  juint32_t usage, 
  juint8_t type, 
  void **location );
/* End – Qualcomm HSUSB addition  */ 

#endif
