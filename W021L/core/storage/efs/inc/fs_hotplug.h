/***********************************************************************
 * fs_hotplug.h
 *
 * Filesystem hotplug manager.
 * Copyright (C) 2005-2009 QUALCOMM, Inc.
 *
 * The EFS hotplug task responds to events indicating insertion and removal
 * of removable media.  It is also responsible for periodic polling of
 * devices that do not self detect.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/storage/efs/main/latest/inc/fs_hotplug.h#11 $ $DateTime: 2009/05/20 15:11:31 $ $Author: richardp $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2009-05-15   ebb   Updated #ifdefs to avoid mismatched signatures in stubs.
2009-05-08   ebb   Added inline stubs for unavailable hotplug functions.
2008-12-16   yg    Modifed device info structure.
2008-12-12   rp    Introducing i/o-vec interface.
2008-11-24   yog   Added hotplug_set_idle_device_event_duration().
2008-10-17   ebb   Updated mode flags for hotplug_checkfat.
2008-09-16   yog   Remove trailing commas from the enum declaration.
2008-09-15   yg    Updated comments for CheckFAT about task.
2008-05-30   ebb   Added callback to hotplug_checkfat.
2008-05-29   ebb   Added checkfat flags.
2008-05-23   rp    Introducing /inc folder as per new SCM.
2008-05-05   ebb   Added hotplug_checkfat interface.
2008-02-19   rp    Documentation: changed from @retval to @returns.
2008-02-04   sch   Corrected documentation mistake in dev read/write calls
2008-01-10   sch   Improved hotplug format and device API comments
2008-01-09   yg    Update documentation for hotplug_init, find_dev_by_path.
2007-12-18   rp    Improved documentation of the registration APIs.
2007-06-09   rp    Doxygen comments markup.
2007-04-08   s h   Clarify that dev_get_path needs a valid hdev.
2007-04-05   s h   Comment and cleanup.
2007-04-02   rp    Added hotplug event notification.
2007-03-14   s h   Improve hotplug device API comments.
2006-11-09   sch   Added CPRM specific definitions
2006-10-16   s h   hotplug_set_polling_interval returns prior value.
2006-10-16   s h   Change lock/unlock functions to return int status.
2006-10-10   s h   Export hotplug_find_dev_by_type() as public.
2006-09-20   s h   Export hotplug_find_dev_by_path() as public.
2006-05-16   s h   Added hotplug_dev_is_mounted()
2006-05-11   s h   Renamed efs_hotplug_... to hotplug_...
2006-05-10   s h   Removed mount/unmount access functions
2006-05-10   s h   Replaced DRV_GEOMETRY_DESC with block size & count
2006-05-10   s h   Added device access functions
2006-05-05   s h   Brought in DEFAULT_POLL_INTERVAL_MS
2006-05-03   s h   Made lock_card device-specific
2006-04-22   s h   Moved all private definitions into fs_hotplug_i.h
2006-04-17   s h   Improved comments
2006-04-17   s h   Added external prototypes for lock/unlock card.
2006-04-13   s h   Code review feedback implemented.
2006-04-13   s h   Allow compilation without USB features
2006-03-10   s h   Added USB CLOSED state
2006-03-10   sch   Added USB specific functionality
2005-09-20   dlb   Create

===========================================================================*/

#ifndef __FS_HOTPLUG_H__
#define __FS_HOTPLUG_H__

#include "comdef.h"
#include "customer.h"
#include "fs_config.h"


/**
 * @defgroup hotplug_module Hotplug Manager
 *
 * EFS2 supports removable media (MMC/SD, USB Mass storage) through the
 * notion of a hotplug manager.  This task keeps track of devices in the
 * system, and performs auto mounting and unmounting of the appropriate
 * mountpoints, so that these devices appear in the filesystem namespace.
 *
 * Some devices, such as USB mass storage, are able to notify us on
 * significant events, such as enumeration and device removal.
 * Automounting these devices is merely a matter of responding to the
 * appropriate events, and mounting or unmounting the device at the
 * appropriate time.
 *
 * Other devices, such as MMC/SD do not have a consistent way of detecting
 * card insertion.  A design may implement card detection with DAT3, power
 * drain detection, or a physical insertion switch.  The design may also
 * opt to do only software polling.  This module needs to be flexible to
 * these differing needs.
 *
 * Because power consumption is a significant concern in a mobile device,
 * the hotplug manager provides calls to enable and disable this periodic
 * polling.  Some suggestions are to enable and disable polling in a
 * similar manner to that used for the backlight.  When the user isn't
 * pressing keys, polling can be disabled.
 *
 * The entire hotplug manager is featurized on @b FEATURE_EFS_HOTPLUG.
 *
 *
 */


/**
 * @defgroup hotplug_config Hotplug Configuration Options
 *  @ingroup hotplug_module
 * \n \n
 * Targets can override these values in the cust files as needed.
 *
 * \link ::FS_HOTPLUG_DEFAULT_POLL_INTERVAL_MS Poll Interval \endlink \n
 * \link fs_config.h::FS_HOTPLUG_NOTIFY_MAX_CLIENTS Max Registration clients
 * \endlink \n
 * \link ::hotplug_set_polling_interval Adjust Poll Interval at runtime
 * \endlink \n
 *
 */

/**
 * @defgroup hotplug_iterate Hotplug Device Iterate functions
 *  @ingroup hotplug_module
 * \n \n
 * These functions can be used to iterate through all the devices managed
 * by the hotplug manager.
 *
 * \link ::hotplug_hdev Access device by drive-number \endlink \n
 * \link ::hotplug_find_dev_by_type Access device by type \endlink \n
 * \link ::hotplug_find_dev_by_path Access device by path \endlink \n
 *
 */

/**
 * @defgroup hotplug_device Hotplug Device Access functions
 *  @ingroup hotplug_module
 * \n \n
 *
 * Once a hotplug 'hdev' handle is retrieved, it can be passed
 * to these functions to perform direct device access.
 *
 * In most cases, these calls (writes in particular) should be avoided
 * or used only on 'locked' devices so that direct device access does
 * not interfere with use by the FAT filesystem.  Additional
 * coordination may be required if there are multiple device users.
 * These "raw" device calls are to be used carefully.
 *
 * Each of these calls is passed to the appropriate Hotplug Device
 * layer (SD, USB, etc) for handling specific to that device.
 *
 * The common parameters are as follows:
 *
 * @param hdev
 *    This is a pointer to a Hotplug device, usually returned from one of the
 *    'find' functions above.  This represents the persistent device handle.
 *
 * @param lba
 *    the Linear Block Address, starting from zero.  This is the number of
 *    the first sector that will be read or written.
 *
 * @param buf
 *    The data buffer to read or write.  In most cases, this buffer should be
 *    located in "DMA-aligned" memory for maximum performance. (Some devices
 *    employ Data Mover hardware that benefits from specific address alignment
 *    in physical memory.)
 *
 * @param block_count
 *    The number of blocks (sectors) to read or write. (A nonsensical 0 value
 *    is not permitted, and may fail or succeed, depending on the device.)
 *
 * @return
 * These functions all return 0 for SUCCESS.
 * Any non-zero value indicates FAILURE, and the specific failure
 * values are device-specific.

 * These are the device access functions provided by hotplug module:
 * @par
 * \link ::hotplug_dev_open hotplug_dev_open \endlink \n
 * \link ::hotplug_dev_close hotplug_dev_close \endlink \n
 * \link ::hotplug_dev_read hotplug_dev_read \endlink \n
 * \link ::hotplug_dev_write hotplug_dev_write \endlink \n
 * \link ::hotplug_dev_write_udata hotplug_dev_write_udata \endlink \n
 * \link ::hotplug_dev_erase hotplug_dev_erase \endlink \n
 * \link ::hotplug_dev_get_size hotplug_dev_get_size \endlink \n
 * \link ::hotplug_dev_is_present hotplug_dev_is_present \endlink \n
 * \link ::hotplug_dev_is_mounted hotplug_dev_is_mounted \endlink \n
 * \link ::hotplug_dev_get_state hotplug_dev_get_state \endlink \n
 *
 */


/** @defgroup hotplug_notification Hotplug Notification
 *  @ingroup hotplug_module
 * \n \n
 * @b Detailed @b Description \n \n
 * The hotplug module can optionally provide system events like "Card-Inserted"
 * or "Card-Removed" etc. to another AMSS task. In order to receive the events,
 * the user will register with the hotplug module indicating what types of
 * events they would like to receive about a device managed by hotplug. The
 * APIs to register and unregister are declared in the public header file
 * fs_hotplug.h
 *
 * \link ::hotplug_dev_type Hotplug Device Types \endlink \n
 * Hotplug manages several different types of media, both removable and
 * permanent. Tasks can nominate interest in one or all of these devices by
 * registering to receive events from any of these device types.  Each type
 * has an enumerated name declared, and the special type "HOTPLUG_TYPE_ALL"
 * represents a wildcard type, including all of the devices managed by Hotplug.
 *
 *
 * \link ::HOTPLUG_NOTIFY_EVENT_CARD_INSERTED Hotplug Events \endlink \n
 * The users can register for any number or combination of the these events by
 * OR'ing them together and forming an event_mask to supply to the registration
 * API.

 * \link ::hotplug_notify_event_cb Registration Callback function \endlink \n
 * The client's callback function should conform to the above prototype. The
 * hotplug module will call this function whenever it has any event for a
 * device that the client has registered for.
 * @Note The callback functions must perform the minimum amount of processing,
 * and operate as if they are in interrupt context.  Each callback should do no
 * more than record simple state information and set signals, returning as soon
 * as possible. Specifically, calls to other libraries like clock timers and
 * queue functions are prohibited.  No blocking functions are allowed.
 * Extensive computation is to be avoided.
 * The callback functions could be invoked from any task with unknown stack
 * size or priority so callback functions should make every effort to be quick
 * and small.
 *
 *
 * \link ::hotplug_notify_register Registration \endlink \n
 * This registration API allows clients to register with the hotplug module to
 * receive notifications for any set of events for any type of device. After
 * successful registration the clients will receive event notifications from
 * the hotplug module through their callback function.
 *
 *
 * \link ::hotplug_notify_unregister Unregistration \endlink \n
 * The above API would allow the clients to unregister from the hotplug module.
 * After successful unregistration the clients will no longer receive event
 * notifications from the hotplug module.
*/

/** The hotplug device is an internal (private) data structure.
 * Here is an anonymous declaration so we can talk about pointers to
 * this structure.  */
struct hotplug_device;

/**
 * The device specific information of a hotplug device.
 * This structure is populated by hotplug_dev_get_device_info().
 *
 * @Note that the device specific information may not be available for
 * all device types.
 */
struct hotplug_device_info
{
  uint32   manufacturer_id;
  uint32   oem_id;
  uint32   prod_rev;
  uint32   prod_serial_num;
  byte     product_name[16];
  byte     manufactured_date[16];
};

/***************************************************
 * CONFIGURATION OPTIONS
 *
 * Targets can override these values as needed
 ****************************************************/

/**
 * This is the starting interval (in milliseconds) to use for polling
 * any external media.  (It is actually the time between polls, so if
 * the poll takes a while (MMC takes 600ms+), the overall interval
 * will be this plus the poll time.)
 *
 * The target can set this value in cust, which allows a different
 * default to be used without changing this file.  This default value
 * will be used if none is previously defined.
 *
 * This interval is used at startup, until a call to
 * efs_hotplug_set_polling_interval() changes it.
 */
#ifndef FS_HOTPLUG_DEFAULT_POLL_INTERVAL_MS
#define FS_HOTPLUG_DEFAULT_POLL_INTERVAL_MS   10000 /* 10 seconds */
#endif


/**
 * This is the starting interval (in minutes) to use for setting the
 * idle device event generation interval. We keep track of the reads/writes
 * on per slot basis and if there is no activity going on that slot for the
 * two consecutive polls then we would generate the event for timeout.
 *
 * The target can set this value in cust, which allows a different
 * default to be used without changing this file.  This default value
 * will be used if none is previously defined.
 *
 * Default value of the timeout event interval has been set as 30 mins.
 */

#ifndef FS_HOTPLUG_TIMEOUT_EVT_INTERVAL_MINUTES
#define FS_HOTPLUG_TIMEOUT_EVT_INTERVAL_MINUTES  30  /* 30 mins */
#endif


/***************************************************
 * EXTERNAL INTERFACE
 *
 * These functions can be called from AMSS
 ****************************************************/
#if defined (FEATURE_EFS_HOTPLUG) || defined (FEATURE_EFS_COLDPLUG)

/**
 * Set the frequency at which Hotplug task will poll ALL devices for changes.
 *
 * @param milliseconds
 *    Amount of time (excluding the detection time) between consecutive hotplug
 *    polls\n
 *
 *    If the value is too small then the task will run frequently, polling the
 *    interface for the presence of media too often resulting in more CPU
 *    usage.\n
 *    If the value is too big then the phone will be sluggish in detecting
 *    new media.\n
 *    This value should be set to 0 to inhibit polling entirely, perhaps
 *    for sleep or low-current scenarios.\n
 *    A poll interval of two to five seconds is reasonable.
 *
 * @return
 *    Returns the previous value of the polling interval.
 */
unsigned int hotplug_set_polling_interval (unsigned int milliseconds);

/**
 * @deprecated
 * Since we did release one VU that called it by this unfortunate
 * name, add a define to make migration easy.
 * This name is DEPRECATED. */
#define efs_hotplug_set_polling_interval(x) hotplug_set_polling_interval(x)

/**
 * The current version of the hotplug_device_info structure.
 */
#define HOTPLUG_DEVICE_INFO_STRUCT_VERSION        (1)

/**
 * @par Name:
 *   hotplug_init - Initialize the hotplug manager.
 *
 * @par Synopsis:
 *  @code
 *    #include "fs_hotplug.h"
 *    void hotplug_init (void);
 *  @endcode
 *
 * @par Description:
 *    Initialize the hotplug manager, and start its task. This must be called
 *    once during startup. EFS2 must be initialized first. EFS currently calls
 *    this after its own startup.
 */
void hotplug_init (void);

/**
 * Formats the media referred by mount point with a fresh filesystem,
 * completely DESTROYING the previous contents.
 *
 * This function also determines which file system is appropriate for
 * the size of the device (FAT16 or FAT32) and formats accordingly.
 * The device is locked for file system operations during formatting. It
 * can still be accessed using direct device calls. It is advisable to
 * lock the device before calling this function to avoid file system
 * inconsistencies.
 *
 * @param dirname
 * The provided pathname is the directory under which the media would
 * normally be mounted by Hotplug (eg: /mmc1 or /usb1)
 *
 * @return
 *    Return 0 on success, -1 on failure.
 *
 * see also: ::hotplug_format_dev
 */
int hotplug_format (const char *dirname);

/**
 * Formats the media referred by hotplug device instance with a fresh
 * filesystem, completely DESTROYING the previous contents.
 *
 * This function also determines which file system is appropriate for
 * the size of the device (FAT16 or FAT32) and formats accordingly.
 * The device is locked for file system operations during formatting. It
 * can still be accessed using direct device calls. It is advisable to
 * lock the device before calling this function to avoid file system
 * inconsistencies.
 *
 * @param hdev
 *    This is a pointer to a Hotplug device, usually returned from
 *    one of the 'find' functions.  This represents the persistent
 *    device handle.
 *
 * @return
 *    Return 0 on success, -1 on failure.
 *
 * see also: ::hotplug_format
 */
int hotplug_format_dev (struct hotplug_device *hdev);


/**
 * Lock all media access on one device.  While media access is
 * "locked", hotplug will stay off the device and ignore any media.
 * Hotplug will forcibly unmount the device and disable polling, so
 * the device will not be auto-mounted.
 *
 * The device remains useable through direct read/write calls, but
 * Hotplug itself will not attempt to (FAT) mount it.
 *
 * Usage:
 * This is used during Cardreader mode, CPRM, and other
 * exclusive-access applications.
 *
 * @param hdev
 *    This is a pointer to a Hotplug device, usually returned from
 *    one of the 'find' functions.  This represents the persistent
 *    device handle.
 *
 * @return
 *    Return 0 on success, -1 on failure.
 *
 * This function can fail if the device is already locked or if hotplug
 * is unable to unmount or close the media
 */
int hotplug_lock_dev (struct hotplug_device *hdev);

/**
 * Unlock a previously locked media. After unlock, hotplug will immediately
 * force a poll, and in this poll the hotplug will try to mount the device.
 *
 * @param hdev
 *    This is a pointer to a Hotplug device, usually returned from
 *    one of the 'find' functions.  This represents the persistent
 *    device handle.
 *
 * @return
 *    Return 0 on success, -1 on failure.
 *
 * This function can fail if the device is NOT locked
 */
int hotplug_unlock_dev (struct hotplug_device *hdev);


/**
 * @deprecated
 *    Old names for the above hotplug_lock_dev (DEPRECATED, Do not use!) */
void fs_sfat_lock_card (void);

/**
 * @deprecated
 * Old names for the above hotplug_unlock_dev (DEPRECATED, Do not use!) */
void fs_sfat_unlock_card (void);

/**
 * This function returns a device entry by index/"drive number".
 *
 * @param driveno
 *    The drive number is an arbitrary number associated with a
 *    particular hotplug device.  It's impossible for an application to
 *    predict which device is associated with a particular drive number,
 *    so it's best to use this only when a drive number is derived from
 *    an hdev handle during runtime.
 *
 * @return
 *    Returns NULL if no device was found with that drive number
 *            or hotplug device entry referred by driveno
 */
struct hotplug_device *hotplug_hdev (int16 driveno);

/**
 * Each Hotplug device has a "type" that identifies the specific
 * device handler for this particular device.
 *
 * The ALL type is a wildcard that matches any type of media.
 */
typedef enum
{
  HOTPLUG_TYPE_INVALID = 0, /**< Invalid device */
  HOTPLUG_TYPE_MMC     = 1, /**< SD or MMC */
  HOTPLUG_TYPE_REMOTE  = 2, /**< Device hosted on other processor */
  HOTPLUG_TYPE_USB_MS  = 3, /**< USB Host Mass Storage (thumdrive) */
  HOTPLUG_TYPE_DEVNULL = 4, /**< NULL (nop) device */
  HOTPLUG_TYPE_FTL     = 5, /**< Flash Translation Layer (FAT-on-NAND) */
  HOTPLUG_TYPE_SD_CPRM = 6, /**< Protected area of SD card (for CPRM) */
  HOTPLUG_TYPE_ALL     = 7  /**< All the above devices */
} hotplug_dev_type;

/**
 * This function returns a handle to the next device of this "type", or the
 * first one of this type if the value of last == NULL.
 * It returns 'NULL' when there are no more devices of that type.
 *
 * Usage scenario:
 * This could be used to iterate through all devices of TYPE_MMC, for
 * example.  It's generally called with 'NULL' the first time, and
 * then each returned hdev pointer is passed back in to find the next
 * one.
 *
 * @b Example:
 * @code
 *    first = hotplug_find_dev_by_type (HOTPLUG_TYPE_MMC, NULL);
 *    hdev = hotplug_find_dev_by_type (HOTPLUG_TYPE_MMC, first);
 * @endcode
 *
 * @return
 *    Returns the next device of type "type" or NULL if no such device
 *    is found
 */
struct hotplug_device *
hotplug_find_dev_by_type (hotplug_dev_type type,
                          struct hotplug_device *last);

/**
 * @par Name:
 *   hotplug_find_dev_by_path - Get the device handle by path name.
 *
 * @par Synopsis:
 *  @code
 *    #include "fs_hotplug.h"
 *    struct hotplug_device * hotplug_find_dev_by_path (const char *path);
 *  @endcode
 *
 * @par Description:
 *   Find a device and get its handle, that would be mounted
 *   under this pathname.
 * @b Example:
 * @code
 *    hotplug_find_dev_by_path ("/mmc1");
 * @endcode
 *
 * @param path
 *   The path is required, NULL terminated string and the leading slash is
 *   optional.
 *
 * @return
 *   Returns the hdev handle, or NULL if no device exists with this name.
 *
 * @sa
 *   hotplug_find_dev_by_type, hotplug_dev_open, hotplug_lock_dev
 */
struct hotplug_device *
hotplug_find_dev_by_path (const char *path);

/** --------------- Device Access Functions --------------- */

/**
 * Open the device for use.
 *
 * This must be called before read or write access, and gives the
 * device layer an opportunity to power up the interface or otherwise
 * prepare for media use.
 *
 * @param hdev
 *    This is a pointer to a Hotplug device, usually returned from
 *    one of the 'find' functions.  This represents the persistent
 *    device handle.
 *
 * @return
 *    0 for SUCCESS, Any non-zero value indicates FAILURE, and the specific
 *    failure values are device-specific.
 */
int hotplug_dev_open (struct hotplug_device *hdev);

/**
 * Close the device.
 *
 * This indicates that no further device access is expected, and
 * permits the device layer to shut down as appropriate.
 *
 * @param hdev
 *    This is a pointer to a Hotplug device, usually returned from
 *    one of the 'find' functions.  This represents the persistent
 *    device handle.
 *
 * @return
 *    0 for SUCCESS, Any non-zero value indicates FAILURE, and the specific
 *    failure values are device-specific.
 */
int hotplug_dev_close (struct hotplug_device *hdev);

/**
 * Read attempts to read upto 'block_count' number of blocks starting
 * from 'lba' from the open device referred by 'hdev' into the buffer
 * starting at 'buf'
 *
 * @param hdev
 *    This is a pointer to a Hotplug device, usually returned from
 *    one of the 'find' functions.  This represents the persistent
 *    device handle.
 * @param lba
 *    Start reading from this block
 * @param buf
 *    Buffer that holds the read data
 * @param block_count
 *    Number of blocks to read
 *
 * @return
 *    0 for SUCCESS, Any non-zero value indicates FAILURE, and the specific
 *    failure values are device-specific.
 *
 * Note: This call fails if the device is not open
 *
 * Please click @ref hotplug_device "here" for information about the
 * parameters and return values.
 */
int hotplug_dev_read  (struct hotplug_device *hdev,
                       uint32 lba,
                       unsigned char *buf,
                       uint16 block_count);

/**
 * Write attempts to write upto 'block_count' number of blocks starting
 * from 'lba' to the open device referred by 'hdev' from the buffer starting
 * at 'buf'
 *
 * @param hdev
 *    This is a pointer to a Hotplug device, usually returned from
 *    one of the 'find' functions.  This represents the persistent
 *    device handle.
 * @param lba
 *    Start writing from this block
 * @param buf
 *    Buffer that holds the data to be written
 * @param block_count
 *    Number of blocks to write
 *
 * @return
 *    0 for SUCCESS, Any non-zero value indicates FAILURE, and the specific
 *    failure values are device-specific.
 *
 * Note: This call fails if the device is not open
 *
 * Please click @ref hotplug_device "here" for information about the
 * parameters and return values.
 */
int hotplug_dev_write (struct hotplug_device *hdev,
                       uint32 lba,
                       unsigned char *buf,
                       uint16 block_count);

/**
 * Write_udata attempts to write upto 'block_count' number of blocks, starting
 * at 'lba', of 'User Data' to the open device referred by 'hdev' from the
 * buffer starting at 'buf'
 *
 * Some devices (eg: CPRM) must distinguish between filesystem metadata
 * (directory entries, accounting structures, etc) and "User Data" or
 * stored payload that is subject to encryption or special processing.
 * This function is used to write "user data" that is determined to
 * have originated from the application, and not the filesystem
 * itself.  For most devices, this is handled identical to
 * hotplug_dev_write().
 *
 * @param hdev
 *    This is a pointer to a Hotplug device, usually returned from
 *    one of the 'find' functions.  This represents the persistent
 *    device handle.
 * @param lba
 *    Start writing from this block
 * @param buf
 *    Buffer that holds the user data to be written
 * @param block_count
 *    Number of blocks to write
 *
 * @return
 *    0 for SUCCESS, Any non-zero value indicates FAILURE, and the specific
 *    failure values are device-specific.
 *
 * Please click @ref hotplug_device "here" for information about the
 * parameters and return values.
 *
 * see also: ::hotplug_dev_write
 */
int hotplug_dev_write_udata (struct hotplug_device *hdev,
                             uint32 lba,
                             unsigned char *buf,
                             uint16 block_count);

/**
 * Erase attempts to erase upto 'block_count' number of blocks, starting at
 * 'lba' from an open device referred by 'hdev'
 *
 * Some devices may support a "block erase" function.  This erases a
 * span of blocks more quickly than writing 0s to them.  For most
 * devices, this is unsupported and will always return a failure.

 * @param hdev
 *    This is a pointer to a Hotplug device, usually returned from
 *    one of the 'find' functions.  This represents the persistent
 *    device handle.
 * @param lba
 *    Start erasing from this block
 * @param block_count
 *    Number of blocks to erase
 *
 * @return
 *    0 for SUCCESS, Any non-zero value indicates FAILURE, and the specific
 *    failure values are device-specific.
 *
 * Please click @ref hotplug_device "here" for information about the
 * parameters and return values.
 */
int hotplug_dev_erase (struct hotplug_device *hdev,
                       uint32 lba,
                       uint16 block_count);

/**
 * Gets the size (number of blocks and size of each block) for a given device.
 *
 * @param hdev
 *    This is a pointer to a Hotplug device, usually returned from
 *    one of the 'find' functions above.  This represents the persistent
 *    device handle.
 *
 * @param block_count
 *    How many blocks (sectors) are available on this media. This function will
 *    write to this variable.
 *
 * @param bytes_per_block
 *    All blocks are a uniform size of this many bytes. This function will
 *    write to this variable.
 *
 * @return
 *    Returns 0 on success and -1 on failure
 *    'block_count' and 'bytes_per_block' are populated with device specific
 *    numbers on success
 */
int hotplug_dev_get_size (struct hotplug_device *hdev,
                          uint32 *block_count,
                          uint16 *bytes_per_block);

/**
 * This function checks for media presence.
 *
 * If a card is present in the slot, it means that sector read
 * or write calls are available.
 *
 * @param hdev
 *    This is a pointer to a Hotplug device, usually returned from
 *    one of the hotplug 'find' functions.  This represents the persistent
 *    device handle.
 *
 * @return
 *    Returns 1 if media in this device is present
 *    0 otherwise
 *
 * @Note
 *    This information is dynamic, so applications should allow for the
 *    possibility that media was removed (or inserted) immediately after
 *    this call returned.
 */
int hotplug_dev_is_present (struct hotplug_device *hdev);

/**
 * If this function returns success, then it means that media is present, and
 * that a valid FAT filesystem was found on it, and the filesystem on this
 * device is mounted and ready for file access.
 *
 * @param hdev
 *    This is a pointer to a Hotplug device, usually returned from
 *    one of the 'find' functions above.  This represents the persistent
 *    device handle.
 *
 * @return
 *    Returns 1 if Hotplug currently has this device mounted.
 *    (eg: the card is present and mounted as /mmc1).
 *    Returns 0 if no media is present, or if media is present, but not
 *    useable as a FAT filesystem (ie, unformatted or corrupt).
 */
int hotplug_dev_is_mounted (struct hotplug_device *hdev);

/**
 * Return the mountpoint pathname of a specific device referred by 'hdev'.
 *
 * This path is where the media is (or will be) mounted when present
 * and formatted.
 *
 * @param hdev
 *    This is a pointer to a Hotplug device, usually returned from
 *    one of the hotplug 'find' functions.  This represents the persistent
 *    device handle.
 *
 * @return
 *    The return value is a pointer to a fixed (unwritable)
 *    null-terminated string, or NULL if hdev is not valid.
 */
const char * hotplug_dev_get_path (struct hotplug_device *hdev);

/**
 * The last-known state of a device is represented by these flags.
 * This structure is populated by hotplug_dev_get_state().
 */
struct hdev_state
{
  boolean is_present;   /**< Media is present in the slot or device */
  boolean is_mounted;    /**< A FAT filesystem is mounted and ready */
  boolean is_formatting;     /**< A format operation is in progress */
  boolean is_locked; /**< The device is locked for exclusive access */
};

/**
 * Fetch the last known state of this hotplug device.
 *
 * An application can query the state at any time.
 * This is an inexpensive operation and performs NO media access.
 *
 * @param hdev
 *    This is a pointer to a Hotplug device, usually returned from
 *    one of the 'find' functions above.  This represents the persistent
 *    device handle.
 *
 * @param state
 *    A pointer to the ::hdev_state structure. Upon success this structure
 *    will be populated with the last known information about this device.
 *
 * @return
 *    Returns 1 on success, 0 on failure (invalid device).
 *
 * @Note that the 'is_present' flag represents the last-known media
 * presence (at the time of the last hotplug poll), and may be stale
 * information.
 */
int hotplug_dev_get_state (struct hotplug_device *hdev,
                           struct hdev_state *state);


/**
 * Fetch device specific information of a hotplug device.
 *
 * An application can query the device info at any time.
 *
 * @param hdev
 *    This is a pointer to a Hotplug device, usually returned from
 *    one of the 'find' functions above.  This represents the persistent
 *    device handle.
 *
 * @param dev_info
 *    A pointer to the ::hotplug_device_info structure. Upon success this
 *    structure will be populated with the device specific information about
 *    this device.
 *
 * @Note that the device specific information may not be available for
 * all device types. -1 is returned if the info is not available.
 *
 * @return
 *    Returns 0 on success, -1 on failure.
 */
int hotplug_dev_get_device_info (struct hotplug_device *hdev,
                                 struct hotplug_device_info *dev_info);


/* This is the set of events that hotplug can provide notification. */
/**
 * This event signals that new media was detected in this device.  The media
 * is known to be present at the time of this event, but may be removed at
 * any time after.  (Applications should be prepared for rapid transitions.)
 * While this event signifies that media is present, that media may not contain
 * a formatted or useable filesystem.
 * (See also: ::HOTPLUG_NOTIFY_EVENT_MOUNTED)
*/
#define HOTPLUG_NOTIFY_EVENT_CARD_INSERTED      0x001

/**
 * Media has been removed from the device, and the device is no longer ready
 * for read/write access.
 * When media is rapidly changed, even with the same physical media being
 * removed and inserted, these events will always be generated for each
 * detection cycle.
 */
#define HOTPLUG_NOTIFY_EVENT_CARD_REMOVED       0x002

/**
 * When new media is detected as present, an attempt will be made to mount
 * the filesystem as a Windows-compatible FAT filesystem.  If this mount
 * succeeds, then the device is now available for filesystem access (efs_open,
 * efs_read, etc) and this event will be generated.
 */
#define HOTPLUG_NOTIFY_EVENT_MOUNTED            0x004

/**
 * When media is locked, removed, or otherwise rendered unavailable to the
 * filesystem, this event signifies that the filesystem has been unmounted.
 * Files on the device are no longer accessible through the EFS API.
 * Further, any files that were still open are immediately forced into an
 * error state.  No further read/write access will be permitted on those
 * existing file handles.
 */
#define HOTPLUG_NOTIFY_EVENT_UNMOUNTED          0x008

/**
 * An attempt was made to mount the media as a FAT filesystem, and this mount
 * failed.  This may indicate that the media does not contain a formatted FAT
 * filesystem, or that the filesystem is corrupt.  The media may be
 * temporarily unavailable (spinning up), unreadable or incompatible.  In most
 * cases, Hotplug will periodically attempt to remount the filesystem.
 */
#define HOTPLUG_NOTIFY_EVENT_MOUNT_FAILED       0x010

/**
 * This event is sent at the beginning of a Format operation.  Formatting large
 * devices can take significant time, and this even indicates the operation has
 * started.
 */
#define HOTPLUG_NOTIFY_EVENT_FORMAT_START       0x020

/**
 * This event is sent when the format operation terminates (either
 * successfully, or with error).  After this event, the format is no longer
 * in progress.
 */
#define HOTPLUG_NOTIFY_EVENT_FORMAT_COMPLETE    0x040

/**
 * The device has been locked for exclusive access by a Hotplug client.  This
 * is used to prohibit the use of media by the FAT filesystem.  Locking a card
 * will forcefully unmount the device and prevent periodic polling so that an
 * application (WLAN, Card Reader Mode, etc) can have exclusive use of the
 * device.
 */
#define HOTPLUG_NOTIFY_EVENT_CARD_LOCKED        0x080

/**
 * The Hotplug device has been released from exclusive access and is now
 * available for Hotplug to poll and mount.
 */
#define HOTPLUG_NOTIFY_EVENT_CARD_UNLOCKED      0x100

/**
 * The Hotplug device has been gone to idle state as there is no activity from
 * some predetermined time.
 */
#define HOTPLUG_NOTIFY_EVENT_DEVICE_IDLE        0x200

/** For convenience this macro is available to indicate all the above events.
 *  It is used only for registration, and never passed into the callback at
 *  event time.
 */
#define HOTPLUG_NOTIFY_ALL_EVENTS \
  ( \
    HOTPLUG_NOTIFY_EVENT_CARD_INSERTED      | \
    HOTPLUG_NOTIFY_EVENT_CARD_REMOVED       | \
    HOTPLUG_NOTIFY_EVENT_MOUNTED            | \
    HOTPLUG_NOTIFY_EVENT_UNMOUNTED          | \
    HOTPLUG_NOTIFY_EVENT_MOUNT_FAILED       | \
    HOTPLUG_NOTIFY_EVENT_FORMAT_START       | \
    HOTPLUG_NOTIFY_EVENT_FORMAT_COMPLETE    | \
    HOTPLUG_NOTIFY_EVENT_CARD_LOCKED        | \
    HOTPLUG_NOTIFY_EVENT_CARD_UNLOCKED      | \
    HOTPLUG_NOTIFY_EVENT_DEVICE_IDLE          \
  )

/** This bitmask represents one or more events */
typedef unsigned int hotplug_notify_event;

/** Each hotplug subscription provides a handle upon registration.
 * This handle must be retained by the application to deregister
 * at a later time. */
typedef unsigned int hotplug_notify_handle;

/** This handle is not valid, and is returned in the event of error. */
#define HOTPLUG_NOTIFY_INVALID_HANDLE       0


/**
 * Callback functions for notification must be of this type.
 *
 * On event, these arguments will be provided during the callback:
 *
 * hdev: The specific device that triggered the event.
 * event_mask: The event that has taken place.
 * reg_hdl: Same registration handle, returned while registering this callback.
 * user_data: Convenience context data, unused by hotplug.
 *
 * Callback functions have no return value.
 */
typedef void (*hotplug_notify_event_cb)
(
  struct hotplug_device     *hdev,
  hotplug_notify_event      event_mask,
  hotplug_notify_handle     reg_hdl,
  void                      *user_data
);

/**
 * @par Name:
 *    hotplug_notify_register - Register a callback function with hotplug to
 *    receive hotplug event notifications.
 *
 * @par Synopsis:
 *  @code
 *    #include "fs_hotplug.h"
 *    hotplug_notify_handle hotplug_notify_register (
 *                hotplug_dev_type device_type,
 *                hotplug_notify_event event_mask,
 *                hotplug_notify_event_cb user_cb,
 *                void *user_data);
 *    @endcode
 *
 * @par Description:
 *  To initiate hotplug notification events, the application registers for the
 *  devices and events of interest.
 *
 * @param device_type
 *  One specific Hotplug device type,or HOTPLUG_TYPE_ALL as a wildcard
 *  for any device.
 *
 * @param event_mask
 *  An 'or'ed combination of one or more events of interest.
 *  Click \link ::HOTPLUG_NOTIFY_EVENT_CARD_INSERTED here \endlink to see the
 *  various types of events.
 *
 * @param user_cb
 *  The callback function to be used on event.
 *
 * @param user_data
 *  Arbitrary data pointer for context.  Hotplug will store this pointer and
 *  provide it to your callback, but does not use it.
 *
 * @return
 *  A valid hotplug_notify_handle on success, or HOTPLUG_NOTIFY_INVALID_HANDLE
 *  on error.
 *
 * @sa
 *  hotplug_notify_unregister
 *
 **********************************************************************/
hotplug_notify_handle hotplug_notify_register (hotplug_dev_type device_type,
                                               hotplug_notify_event event_mask,
                                               hotplug_notify_event_cb user_cb,
                                               void *user_data);

/**
 * @par Name:
 *  hotplug_notify_unregister - Terminate notification events for a previous
 *  registration.
 *
 * @par Synopsis:
 *  @code
 *    #include "fs_hotplug.h"
 *    int hotplug_notify_unregister (hotplug_notify_handle reg_hdl);
 *  @endcode
 *
 * @par Description:
 *  This API can be used to terminate notification events for a previous
 *  registration.
 *
 * @param reg_hdl
 *  The registration handle is the same one returned while registration.
 *
 * @return
 *  0 on success, -1 on failure
 *
 * @sa
 *  hotplug_notify_register
 *
 **********************************************************************/
int hotplug_notify_unregister (hotplug_notify_handle reg_hdl);

/**
 * @par Name:
 *   hotplug_checkfat - Check for corruption within FAT filesystem.
 *
 * @par Synopsis:
 *  @code
 *    #include "fs_hotplug.h"
 *    int hotplug_checkfat (const char *path, int mode, int *errors,
 *                          void (*callback) (void *), void *param);
 *
 *  @endcode
 *
 * @par Description:
 *   hotplug_checkfat is a utility that validates the integrity of inner
 *   FAT filesystem data structures. NOTE: This feature is only available
 *   when FEATURE_HFAT_CHKFAT is defined. There are 3 classes of errors
 *   within the FAT filesystem that it can detect and fix.
 *
 *   1. Errors within the file allocation table.
 *   2. Errors within the directory entries.
 *   3. Recovery of lost file chains.
 *
 *   Depending on the parameters passed efs_checkfat can either check
 *   for errors and return a bit mask of the error type in the
 *   errors parameter, fix cluster and directory entry errors
 *   and/or recover lost file chains.
 *
 *
 * @param path
 *   pathname of the drive to be checked. ie: /mmc1, /ftl1, etc...
 *
 * @param mode
 *   The mode in which to run hotplug_checkfat. The options are:
 *   1. CHKFAT_READONLY - Do NOT make changes, return error mask.
 *   2. CHKFAT_FIX - Fix errors and delete lost file chains.
 *   3. CHKFAT_FIX_RECOVER - Fix errors and recover lost file chains.
 *   4. CHKFAT_ERASE_BAD_CHAIN - Erase bad chains rather than truncate.
 *      * Can be OR'd with previous 2 flags.
 *
 * @param callback
 *   This function will get called periodically from checkfat to prevent
 *   from the watch dog timing out. There should not be additional work
 *   performed other than state updates and kicking the watch dog.
 *
 * @return
 *   Returns 0 on success and -1 on failure.
 *
 * NOTE: Please adjust stack size of the task based on the desired directory
 * depth CHKFAT_MAX_DIR_DEPTH. If this is improperly set, a stack overrun is
 * likely. Also, this function inherently takes time to complete. The time
 * taken corresponds to the size of disk to be checked and the number of
 * directories and files on the disk.
 *
 * WARNING: Because of the above reason this function should not be called
 * from UI task, since it might lose the events like incoming call and SMS
 * which need to be processed immediately. Instead create a lower priority
 * task that executes this function and UI task can monitor and update the
 * UI as the checkFAT progresses.
 *
 * WARNING: This revision has a known issue with multiple active HFAT
 * devices. If any other HFAT devices are present but not mounted a
 * potential deadlock condition may arise. The workaround is to ensure all
 * HFAT devices are mounted before hotplug_checkfat is run. To accomplish
 * this all mountable devices must be formatted, after which hotplug_checkfat
 * can be run after all devices have been polled after boot (~30 seconds).
 *
 * Flags for checkfat
 * These can be OR'd together.
 */
#define CHKFAT_READONLY            0x1     /* Do not change on disk */
#define CHKFAT_FIX_ERRORS          0x2     /* Fix errors */
#define CHKFAT_RECOVER_FILES       0x4     /* Recover files */

/* Old flags (Deprecated) */
#define CHKFAT_FIX                 CHKFAT_FIX_ERRORS
#define CHKFAT_FIX_RECOVER         (CHKFAT_FIX_ERRORS | CHKFAT_RECOVER_FILES)
#define CHKFAT_ERASE_BAD_CHAIN     0       /* Default is erasing bad chain */

/* Bit mask of checkfat failure conditions */
#define CHKFAT_BAD_ELEMENT         0x1     /* Cluster chain errors */
#define CHKFAT_CROSS_CHAINED       0x2     /* 2 chains pointing to 1 cluster */
#define CHKFAT_LOST_CHAIN_FOUND    0x4     /* Lost chain present */
#define CHKFAT_FATS_INCONSISTENT   0x8     /* FAT2 doesn't match FAT1 */
#define CHKFAT_SHORT_ENTRY_INVALID 0x10    /* Bad shortname entry */
#define CHKFAT_BAD_DIR_REFERENCE   0x20    /* Bad . or .. reference */
#define CHKFAT_LONG_ENTRY_INVALID  0x40    /* Bad LFN entry       */

/* Error conditions */
#define CHKFAT_DIR_DEPTH_EXCEEDED  0x1000  /* Exceeded dir depth limit */

int hotplug_checkfat (const char *path, int mode, int *errors,
                      void (*callback) (void *), void *param);

/**
 * Set the frequency at which Hotplug task will check for any idle device.
 *
 * @param minutes
 *    This would take amount of time (excluding the detection time) between
 *    consecutive idle_device_event_generation polls in minutes.
 *    The maximum value this can take is 254 minutes.
 *    This value should be set to 0 to inhibit polling entirely.
 *
 * @return
 *    Returns nothing.
 */
void hotplug_set_idle_device_event_duration (uint8 minutes);

/* If hotplug is not enabled add create stubs for dependencies on these
 * functions.
 */
#else /* !defined (FEATURE_EFS_HOTPLUG) && !defined (FEATURE_EFS_COLDPLUG) */

static inline int
hotplug_format (const char *path)
{
  (void) path;
  return -1;
}

static inline void
hotplug_set_idle_device_event_duration (uint8 minutes)
{
  (void) minutes;
}

static inline struct hotplug_device *
hotplug_find_dev_by_path (const char *path)
{
  (void) path;
  return NULL;
}

static inline int
hotplug_dev_get_device_info (struct hotplug_device *hdev,
                             struct hotplug_device_info *dev_info)
{
  (void) hdev;
  (void) dev_info;

  return -1;
}
#endif

#endif /* not __FS_HOTPLUG_H__ */
