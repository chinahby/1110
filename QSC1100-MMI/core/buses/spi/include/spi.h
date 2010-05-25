#ifndef SPI_H
#define SPI_H
/**===========================================================================
  @file spi.h
 
  Serial Peripheral Interface (SPI) Driver Header File

               Copyright (c) 2007-2008 QUALCOMM Incorporated.
               All Rights Reserved.
               Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //depot/asic/msmshared/users/wxie/spi/q1110/spi/include/spi.h#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <comdef.h>
#include <rex.h>


/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/**
 * The return type used by SpiInit
 */
typedef enum
{
   SPI_INIT_SUCCESS = 0, /**< the SPI driver was successfully initialized */
   SPI_INIT_FAILURE = 1 /**< the SPI driver initialized failed (out of memory) */
} SpiInitResultType;

/**
 * The clock mode parameter type
 */
typedef enum
{
   SPI_CLK_NORMAL, /**< the SPI clock runs only during a transfer unit */
   SPI_CLK_ALWAYS_ON /**< the SPI clock runs continously from the start of the transfer */
} SpiClockModeType;

/**
 * The clock polarity parameter type
 */
typedef enum
{
   SPI_CLK_IDLE_LOW, /**< the SPI clock signal is low when the clock is idle */
   SPI_CLK_IDLE_HIGH /**< the SPI clock signal is high when the clock is idle */
} SpiClockPolarityType;

/**
 * The clock phase parameter type
 */
typedef enum
{
   SPI_INPUT_FIRST_MODE, /**< the SPI data input signal is sampled on the leading clock edge */ 
   SPI_OUTPUT_FIRST_MODE /**< the SPI data input signal is sampled on the trailing clock edge */
} SpiTransferModeType;

/**
 * The chip select polarity parameter type
 */
typedef enum
{
   SPI_CS_ACTIVE_LOW, /**< the SPI chip select is active low */
   SPI_CS_ACTIVE_HIGH /**< the SPI chip select is active high */
} SpiCSPolarityType;

/**
 * The chip select mode parameter type
 */
typedef enum
{
   SPI_CS_DEASSERT, /**< the SPI chip select is de-asserted between transfer units */
   SPI_CS_KEEP_ASSERTED /**< the SPI chip select is kept asserted between transfer units */
} SpiCSModeType;

/**
 * The transaction result type. This is used to indicate to the
 * client application whether the transaction was successfully
 * enqueued in the SPI transaction database when
 * SpiSetupTransfer was called.
 */
typedef enum
{
   SPI_FAILURE = -1, /**< there was an error with the SPI transaction */
   SPI_SUCCESS = 1, /**< the transaction was successfully enqueued */
   SPI_TRY_LATER = 2 /**< the trasaction could not be added because the queue was full. */
} SpiTransactionResultType;

/**
 * This is the type define for callback functions from SPI
 * client. Callbacks are provided for asynchronous
 * transactions/errors.
 */
typedef void (*SpiCallbackType) (SpiTransactionResultType eResult);  

/**
 * The transfer struct used to setup an SPI transfer.
 */
typedef struct
{
   uint32 nDeviceId; /**< the device ID which was returned by SpiConfigureDevice */
   uint32 nNumInputTransfers; /**< the number of input transfers in this transfer */
   uint32 nNumOutputTransfers; /**< the number of output transfers in this transfer */
   SpiCallbackType pfCallback; /**< a callback function to call when the transfer is complete.
                                    Called if not NULL and mUserSignal is 0. */
   rex_sigs_type mUserSignal; /**< a user task signal to set when the transfer is complete.
                                    SpiSetupTransfer will wait until the signal is set before returning.
                                    Set to 0 for no task signalling. */

   /*These are the input and the output streams, the client is repsonsible for
    * maintaining their validity throughout the transaction period. The Protocol Driver
    * will not allocate (for read) and deallocate (for write) memory for these. */

   const uint32 *panInputData; /**< The pointer to the user input buffer where the data will be copied.
                                    The calling application is responsible for allocating this buffer and
                                    maintaining its validity until the transfer is complete. */
   const uint32 *panOutputData; /**< The pointer to the user output buffer which contains the data to be sent.
                                    The calling application is responsible for allocating this buffer and
                                    maintaining its validity until the transfer is complete. */
} SpiTransferType;


/**
 * This struct is used to configure the device-specific
 * parameters for an SPI transfer.
 */
typedef struct
{
   SpiClockModeType eClockMode; /**< specify whether the SPI clock should run
                                     continuously from the start to the end
                                     of the transfer */
   SpiClockPolarityType eClockPolarity; /**< configures the clock polarity */
   SpiTransferModeType eTransferMode; /**< configures the clock phase */
   uint32 nDeassertionTime; /**< in master mode, configures the minimum time to wait
                                 between transfer units in nanoseconds */
   uint32 nMinSlaveFrequency; /**< in master mode, specify the minimum
                                   SPI clock frequency supported by the slave device */
   uint32 nMaxSlaveFrequency; /**< in mster mode, specify the
                                   maximum SPI clock frequency supported by the slave device */
   SpiCSPolarityType eCSPolarity; /**< configures the chip select polarity */
   SpiCSModeType eCSMode; /**< configures whether the chip select should stay asserted in
                               between transfer units */
} SpiDeviceParametersType;

/**
 * The SPI core mode parameter type. This is used to configure
 * whether the SPI core should act as the master or the slave
 * when communicating with a device.
 */
typedef enum
{
   SPI_CORE_MODE_SLAVE, /**< the SPI core is the slave */
   SPI_CORE_MODE_MASTER /**< the SPI core is the master */
} SpiCoreModeType;

/**
 * This struct contains the board-specific information for an
 * SPI device.
 */
typedef struct
{
   uint8 nCoreNumber; /**< Specifies which SPI core to use for this transfer.
                  If only one core exists on the system, this needs to be 0. */
   uint8 nSlaveNumber; /**< For a slave device, specifies which chip
                            select the slave is on. */
   SpiCoreModeType eCoreMode; /**< Specifies whether the core is the master or
                                    slave with respect to the device. */
} SpiDeviceBoardInfoType;

/**
 * The loopback parameter type. This is used to specify whether
 * the transfer should occur with the core in internal loopback
 * mode.
 * 
 * For normal operation, loopback mode should be disabled.
 * 
 * When internal loopback mode is enabled, the SPI core ties the
 * SPI data out signal to its SPI data in signal.
 */
typedef enum
{
   SPI_LOOPBACK_DISABLED, /**< internal loopback is disabled. */
   SPI_LOOPBACK_ENABLED /** internal loopback is enabled. */
} SpiLoopbackModeType;

/**
 * This parameter type specifies whether the input data should
 * be packed into the user input buffer as it is received.
 * 
 * When packing is disabled, each n-bit transfer unit is placed
 * into its own 32-bit word. n is the number of bits per
 * transfer unit as specified in the transfer parameter
 * nNumBits.
 * 
 * When packing is enabled, each n-bit transfer unit is packed
 * into the 32-bit input buffer in 8-bit or 16-bit units.
 * 
 *  n <= 8: 8-bit packing.
 *  8 < n <= 16: 16-bit packing.
 *  n > 16: no packing
 */
typedef enum
{
   SPI_INPUT_PACKING_DISABLED, /* disable input packing */
   SPI_INPUT_PACKING_ENABLED /* enable input packing */
} SpiInputPackingType;

/**
 *  This parameter type specifies whether the output data should
 *  be unpacked from the user output buffer as it is
 *  transmitted.
 * 
 *  When unpacking is disabled, each 32-bit word in the user
 *  input buffer contains the data for a single n-bit transfer
 *  unit. n is the number of bits per transfer unit as specified
 *  in the transfer parameter nNumBits.
 * 
 *  When unpacking is enable, the user places the n-bit transfer
 *  unit data in packed quantities of 8-bit or 16-bit units.
 * 
 *  n <= 8: 8-bit packing.
 *  8 < n <= 16: 16-bit packing.
 *  n > 16: no packing
 */
typedef enum
{
   SPI_OUTPUT_UNPACKING_DISABLED, /*< disable output unpacking */
   SPI_OUTPUT_UNPACKING_ENABLED /**< enable output unpacking */
} SpiOutputUnpackingType;

/**
 * This struct is used to specify the transfer-specific
 * parameters for an SPI transfer.
 */
typedef struct
{
   uint8 nNumBits; /**< configures the number of bits to use per transfer unit */
   SpiLoopbackModeType eLoopbackMode; /**< enables/disables internal loopback mode */
   SpiInputPackingType eInputPacking; /**< specifies whether data should be 
                                           packed into the user input buffer */
   SpiOutputUnpackingType eOutputUnpacking; /**< specifies whether data should
                                    be unpacked from the user output buffer */
} SpiTransferParametersType; 

/**
 * This struct is the aggregate of the board-specific,
 * device-specific, and transfer-specific parameters used
 * to configure an SPI device.
 * 
 * This is the struct that is used to configure a device using
 * SpiConfigureDevice.
 */
typedef struct
{
   SpiDeviceParametersType *pmDeviceParameters; /**< the device-specific parameters */
   SpiDeviceBoardInfoType *pmDeviceBoardInfo; /**< the board-specific parameters */
   SpiTransferParametersType *pmTransferParameters; /**< the transfer-specific parameters */
} SpiDeviceInfoType;


/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
 * Initializes the SPI driver.
 * 
 * This function must be called once at initialization before
 * calling any other SPI driver functions.
 * 
 * @return SPI_INIT_FAILURE if an error occurred. Otherwise,
 *         SPI_INIT_SUCCESS if successful
 */
extern SpiInitResultType SpiInit(void);

/**
 * Initializes the SPI driver.
 * 
 * Adds a device and corresponding device specific data to
 * the device configuration table. The caller must maintain the
 * SpiDeviceInfoType and its members until the device is
 * deconfigured.
 *
 * In the current design, since the caller maintains the
 * configuration data, these parameters can be over-ridden on a
 * per transaction basis.
 * 
 * @param pmDeviceTransferInfo - Pointer to the parameters
 * @return If successful, returns the device ID which
 *                should be passed by the client to the
 *                SpiSetupTransfer function to initiate the
 *                transfer on this device.
 */
extern int32 SpiConfigureDevice(SpiDeviceInfoType* pmDeviceTransferInfo);

/**
 * Removes the device from the SPI device table.
 *
 * @param nDeviceId - the device ID of the device to remove
 *
 */
extern void SpiDeconfigureDevice(uint32 nDeviceId);

/**
 * SpiSetupTransfer
 * Requests a bidirectional data transfer to be added to the transfer queue.
 *
 * This function immediately adds the transfer to the request queue, unless
 * the transfer queue is full, in which case it returns SPI_TRY_LATER.
 *
 * If pfCallback is not NULL, then this function will return immediately.
 * Otherwise, the function will wait until the signal passed in the mUserSignal
 * element of pmNewTransfer is received.
 * 
 * @param pmNewTransfer - Pointer to a struct which contains the
 *                      set of parameters to be used for the
 *                      transaction.
 *
 * @return SPI_SUCESS - The transfer was successfully setup (in
 *                         Async mode) or the transfer was
 *                         succefully carried out (in Sync
 *                         mode).
 * 
 *        SPI_FAILURE - The transfer could not be setup or there
 *                         was an error in carrying out the
 *                         transfer.
 * 
 *        SPI_TRY_LATER - The SPI transfer queue is full at the
 *                         moment.
 */
extern SpiTransactionResultType SpiSetupTransfer(SpiTransferType *pmNewTransfer);

#endif /* SPI_H */
