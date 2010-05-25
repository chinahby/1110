#ifndef HAL_SPI_H
#define HAL_SPI_H
/**============================================================================
  @file HALspi.h

   Serial Peripheral Interface (SPI) Hardware Abstraction Layer (HAL)
 
               Copyright (c) 2007-2008 QUALCOMM Incorporated.
               All Rights Reserved.
               Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //depot/asic/msmshared/users/wxie/spi/q1110/spi/include/HALspi.h#2 $ */

/*----------------------------------------------------------------------------
 * Include Files
 *--------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/* #include <HALcomdef.h> */
#include "comdef.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *--------------------------------------------------------------------------*/
/**
 * @def HAL_spi_NO_ERRORS
 * @brief Bit-mask of no SPI errors
 *
 * @def HAL_spi_CLOCK_UNDERRUN_ERROR
 * @brief Bit-mask for clock underrun error
 *
 * @def HAL_spi_CLOCK_OVERRUN_ERROR
 * @brief Bit-mask for clock overrun error
 *
 * @def HAL_spi_INPUT_OVERRUN_ERROR
 * @brief Bit-mask for input overrun error
 *
 * @def HAL_spi_OUTPUT_UNDERRUN_ERROR
 * @brief Bit-mask for output underrun error
 *
 * @def HAL_spi_INPUT_UNDERRUN_ERROR
 * @brief Bit-mask for input underrun error
 *
 * @def HAL_spi_OUTPUT_OVERRUN_ERROR
 * @brief Bit-mask for output overrun error
 *
 * @def HAL_spi_TIMEOUT_ERROR
 * @brief Bit-mask for timeout error
 *
 * @def HAL_spi_ALL_ERRORS
 * @brief Bit-mask of all SPI errors ORed together
 */
#define HAL_spi_NO_ERRORS 0x0
#define HAL_spi_CLOCK_UNDERRUN_ERROR 0x1
#define HAL_spi_CLOCK_OVERRUN_ERROR 0x2
#define HAL_spi_INPUT_OVERRUN_ERROR 0x4
#define HAL_spi_OUTPUT_UNDERRUN_ERROR 0x8
#define HAL_spi_INPUT_UNDERRUN_ERROR 0x10
#define HAL_spi_OUTPUT_OVERRUN_ERROR 0x20
#define HAL_spi_TIMEOUT_ERROR 0x40
#define HAL_spi_ALL_ERRORS 0x7F

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/**
 * This type specifies whether the core is the master or slave
 * on the SPI bus.
 */
typedef enum
{
   HAL_spi_MASTER, /**< the SPI core is the master */
   HAL_spi_SLAVE /**< the SPI core is the slave */
} HAL_spi_CoreModeType;

/**
 * This type is used by HAL_spi_GetRunState to return the state
 * of the SPI core.
 * 
 * The Reset state means that no SPI transfers will be
 * sent/received.
 * 
 * The Run state means that the SPI core is in the middle of an
 * SPI transfer. The SPI core will actively send/receive SPI
 * data in this state.
 * 
 * The Pause state means that the SPI transfer was paused. This
 * state is only available when the core is the Master. It is
 * intended for when the core needs to throttle the output in
 * order to fill the output FIFO and process the input FIFO
 * before continuing with the transfer.
 * 
 * SPI core, device, and transfer parameters must only be set
 * when the SPI core is in the Reset state.
 */
typedef enum {
   HAL_spi_RESET_STATE, /**< the SPI core is in the Reset state */
   HAL_spi_RUN_STATE, /**< the SPI core is in the Run state */
   HAL_spi_PAUSE_STATE, /**< the SPI core is in the Pause state */
   HAL_spi_INVALID_STATE /**< the SPI core state cannot be read at the present time or is in an invalid state */
} HAL_spi_RunStateType;

/**
 * This type specifies the clock polarity the core should use to
 * communicate with a device.
 */
typedef enum {
   HAL_spi_CLK_IDLE_LOW, /**< the SPI clock is low when idle. first clock edge is rising. */
   HAL_spi_CLK_IDLE_HIGH /**< the SPI clock is high when idle. first clock edge is falling. */
} HAL_spi_ClockPolarityType;

/**
 * This type specifies the clock phase mode that the SPI core
 * will should use to communicate with a device.
 */
typedef enum {
   HAL_spi_INPUT_FIRST_MODE, /**< the SPI data input signal is sampled on the leading clock edge */ 
   HAL_spi_OUTPUT_FIRST_MODE /**< the SPI data input signal is be sampled on the trailing clock edge */
} HAL_spi_TransferModeType;

/**
 * When the SPI core is the master, this type is used to specify
 * whether the core should configure the chip select output to
 * be active high or active low.
 * 
 * When in slave mode, the core always expects an active low
 * chip select input.
 */
typedef enum {
   HAL_spi_CS_ACTIVE_LOW, /**< the chip select is active low */
   HAL_spi_CS_ACTIVE_HIGH  /**< the chip select is active high */
} HAL_spi_ChipSelectPolarityType;

/**
 * This type is used with HAL_spi_SetDeviceParameters to
 * configure the device parameters to be used by the SPI core
 * for the current transfer.
 */
typedef struct {
   HAL_spi_ClockPolarityType eClockPolarity; /**< the SPI clock polarity */
   HAL_spi_TransferModeType eTransferMode; /**< the SPI clock phase */
   boolean bClockAlwaysOn; /**< set to TRUE to run the SPI clock continuously from start to end of the transfer */
   boolean bChipSelectAlwaysOn; /**< set to TRUE*/
   HAL_spi_ChipSelectPolarityType eCSPolarity; /**< the chip-select polarity */
   uint32 nChipSelectId; /**< the chip select to use for this transfer */
   uint32 nDeassertionClockTicks; /**< the number of clock ticks to wait between transfer units */
} HAL_spi_DeviceParametersType;

/**
 * This type specifies whether the core should operate in
 * loopback mode.
 *
 * When loopback mode is enabled, the SPI core internally ties
 * SPI Data Out to SPI Data In. This mode is intended for
 * testing and debugging.
 */
typedef enum {
   HAL_spi_LOOPBACK_DISABLED, /*< Disable internal loopback mode */
   HAL_spi_LOOPBACK_ENABLED /*< Enable internal loopback mode */
} HAL_spi_LoopbackModeType;

/**
 * This type specifies whether the SPI core should send/receive
 * data from the output/input FIFO using FIFO mode, Block mode,
 * or Data Mover mode.
 * 
 * Although the hardware supports using different modes for
 * the output and input transfer, software only supports using
 * the same mode for both the output and input, since this is
 * the expected usage model.
 */
typedef enum {
   HAL_spi_FIFO_MODE, /*< Use FIFO mode to transfer data */
   HAL_spi_BLOCK_MODE, /*< Use Block mode to transfer data */
   HAL_spi_DATA_MOVER_MODE /*< Use Data Mover mode to transfer data */
} HAL_spi_IOModeType;

/**
 * This type specifies whether the SPI core should use so-called
 * Input Only, Output Only, or Bi-directional modes during the
 * transfer.
 * 
 * In Input Only mode, the SPI Data Out signal is tied low
 * during the transfer and no Output-related interrupts are
 * triggered during the transfer.
 * 
 * Similarly, in Output Only, the SPI Data In signal is ignored
 * by the SPI core and no Input-related interrupts are triggered
 * during the transfer.
 * 
 * In Bi-directional mode, data is both shifted in and out as
 * normal in a SPI full-duplex configuration.
 */
typedef enum {
   HAL_spi_INPUT_ONLY,
   HAL_spi_OUTPUT_ONLY,
   HAL_spi_BI_DIRECTIONAL
} HAL_spi_TransferDirectionType;

/**
 * This type is used with HAL_spi_TransferParameters to
 * configure the transfer parameters to be used by the SPI core
 * for the current transfer
 */
typedef struct {
   uint8 nNumBits; /**< the number of bits per transfer unit */
   HAL_spi_LoopbackModeType eLoopbackMode; /**< configures internal loopback mode */
   boolean bReceivedDataPacked; /**< set TRUE if data is to be packed in the SPI input FIFO */
   boolean bTransmitDataPacked; /**< set to TRUE if data is packed in the SPI output FIFO */
   boolean bOutputBitShift; /**< set to TRUE to enable the "Output Bit Shift" function */
   uint32 nNumInputTransfers; /**< the number of transfer units that will be read into the input FIFO */
   uint32 nNumOutputTransfers; /**< the number of transfer units that will be sent from the output FIFO */
   HAL_spi_TransferDirectionType eTransferDirection; /**< configures the direction of the transfer */
   HAL_spi_IOModeType eIOMode; /**< specifies whether to use FIFO, block, or data mover to transfer data */
} HAL_spi_TransferParametersType; 

/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/

/* Mandatory API */
/**
 * This function is intended to perform any SPI device initialization prior
 * to the first use of the bus after power-up or a call to HAL_spi_Reset().
 *
 * @param nInstanceId - The identifier of the SPI core to initialize.
 * @param ppszVersion - Pointer to version string.
 * @see HAL_spi_Reset
 */
void HAL_spi_Init(uint32 nInstanceId, char **ppszVersion);

/**
 * This function is intended to return the SPI bus hardware to its power-on
 * status.
 * 
 * @param nInstanceId - The identifier of the SPI core to reset.
 * @see HAL_spi_Init
 */
void HAL_spi_Reset(uint32 nInstanceId);


/**
 * This mandatory function, combined with HAL_spi_Restore(), provides a way
 * to save and restore the state of HAL hardware.
 *
 * For SPI, this function does nothing.
 *
 * @param nInstanceId - The identifier of the SPI core to save.
 */
void HAL_spi_Save(uint32 nInstanceId);


/**
 * This mandatory function, combined with HAL_spi_Save(),
 * provides a way o save and restore the state of HAL hardware.
 * 
 * For SPI, this function does nothing.
 *
 * @param nInstanceId - The identifier of the SPI core to restore.
 */
void HAL_spi_Restore(uint32 nInstanceId);



/* Capabilities API */

/**
 * This function returns the number of SPI cores that are
 * implemented on the system. The SPI HAL interfaces support the
 * configuration of multiple cores by the instance ID.
 * 
 * Valid instance IDs are indexed starting at 0 up to N - 1,
 * where N is the number of instances returned by this function.
 * 
 * @return The number of SPI cores that exist on the system.
 */
uint32 HAL_spi_GetNumInstances(void);


/**
 * This function returns the block size of the hardware input
 * FIFO on the SPI core.
 * 
 * @param nInstanceId - The identifier of the SPI core to restore.
 * @return The block size of the SPI Input FIFO in terms of the
 *         number of 32-bit words.
 */
uint32 HAL_spi_GetInputBlockSize(uint32 nInstanceId);


/**
 * This function returns the block size of the hardware output
 * FIFO on the SPI core.
 * 
 * @param nInstanceId - The identifier of the SPI core to
 *       restore.
 * 
 * @return The size of the block in terms of 32-bit words.
 */
uint32 HAL_spi_GetOutputBlockSize(uint32 nInstanceId);


/**
 * This function returns the InputBlock Size which is set in the
 * RTL. Necessary for Block Mode Operation of the SPI Driver.
 * 
 * @param nInstanceId - The identifier of the SPI core to
 *                    restore.
 * 
 * @return The size of the FIFO in terms of 32-bit words.
 */
uint32 HAL_spi_GetInputFifoSize(uint32 nInstanceId);


/**
 * This function returns the InputBlock Size which is set in the
 * RTL. Necessary for Block Mode Operation of the SPI Driver.
 * 
 * @param nInstanceId - The identifier of the SPI core to
 *                    restore.
 * 
 * @return The size of the FIFO in terms of 32-bit words.
 */
uint32 HAL_spi_GetOutputFifoSize(uint32 nInstanceId);


/**
 * This function returns the maximum number of input transfers
 * that can be configured in block or data mover mode. If more
 * transfers are required than are supported by the core, the
 * core can be placed in infinite mode by setting the number of
 * transfers to 0.
 * 
 * @param nInstanceId - The identifier of the SPI core to query
 *                    for the capability.
 * 
 * @return The maximum number of input transfers will be
 *         returned.
 */
uint32 HAL_spi_GetMaxNumInputTransfers(uint32 nInstanceId);


/**
 * This function returns the maximum number of output transfers
 * that can be configured in block or data mover mode. If more
 * transfers are required than are supported by the core, the
 * core can be placed in infinite mode by setting the number of
 * transfers to 0.
 * 
 * @param nInstanceId - The identifier of the SPI core to query
 *                    for the capability.
 * 
 * @return The maximum number of output transfers will be
 *         returned.
 */
uint32 HAL_spi_GetMaxNumOutputTransfers(uint32 nInstanceId);



/* Error API */


/**
 * This function returns the status of SPI errors. Error flags
 * are ORed together to report multiple errors.
 *
 * @param nInstanceId - The identifier from which the SPI core gets the errors.
 * @return The status of the SPI core error flags.
 */
uint32 HAL_spi_GetErrors(uint32 nInstanceId);


/**
 *    This function clears the errors as specified in a bit mask
 *    nErrorsToClear.
 * 
 * @param nInstanceId - The identifier of the SPI core which clears the errors.
 * @param nErrorsToClear - A variable set with bit-masks to select which errors
 *                  to clear. Bit-masks are ORed together to clear multiple
 *                  errors. If the corresponding bit is set in
 *                  the bit-mask, then the error is cleared,
 *                  otherwise it is unchanged.
 */
void HAL_spi_ClearErrors(uint32 nInstanceId, uint32 nErrorsToClear);


/**
 * This function masks out error Types the driver does not
 * want enabled. The HAL converts the semantic error types to
 * bit positions in the core registers.
 * 
 * @param nInstanceId - The identifier of the SPI core to configure.
 * @param nErrorsToEnable - A variable set with bit-masks to select which errors
 *                  to enable. Bit-masks are ORed together to enable
 *                  multiple errors. If the corresponding bit is
 *                  set in the bit-mask, then the error is
 *                  enabled, otherwise it is disabled. See the
 *                  Description for more details.
 */
void HAL_spi_ConfigureErrors(uint32 nInstanceId, uint32 nErrorsToEnable);


/**
 * This function sets the period of time that an SPI transfer is
 * allowed before a timeout error occurs.
 * 
 * @param nInstanceId - The identifier of the SPI core which is used set
 *                      the timeout period.
 * @param nTimeout - The timeout period in SPI clock ticks.
 */
void HAL_spi_SetTimeout(uint32 nInstanceId, uint32 nTimeout);



/**
 * This function gets the current value of the SPI timeout
 * timer.
 * 
 * @param nInstanceId - The identifier of the SPI core which is
 *               used to get the timeout period.
 * @return Returns the current value of the SPI timeout timer in
 *         terms of SPI clock ticks.
 */
uint32 HAL_spi_GetCurrentTimeout(uint32 nInstanceId);



/* Configuration API */

/**
 * This function configures the specified SPI core to run in
 * master or slave mode.
 * 
 * @param nInstanceId - The identifier of the SPI core to
 *                    configure.
 * @param eCoreMode - The core mode.
 */
void HAL_spi_SetCoreMode(uint32 nInstanceId, HAL_spi_CoreModeType eCoreMode);


/**
 * This function configures the tri-state behavior of the SPI
 * chip selects.
 * 
 * @param nInstanceId - The identifier of the SPI core which is
 *                    used to set the state.
 * @param bTristateCS - Set to TRUE to set the chip select outputs to be always
 *                      tri-state. Set to FALSE to enable normal functionality.
 */
void HAL_spi_SetChipSelectTristate(uint32 nInstanceId, boolean bTristateCS);



/**
 * This function configures the tri-state behavior of the SPI
 * data out signal.
 * 
 * @param nInstanceId - The identifier of the SPI core which is
 *               used to set the data out tristate.
 * @param bTristateSPIDataOut - Set to TRUE to configure the SPI data out signal
 *                          to go to tri-state when the SPI chip select is
 *                          de-asserted. Otherwise, set to
 *                          FALSE.
 */
void HAL_spi_SetSPIDataOutTristate(uint32 nInstanceId, boolean bTristateSPIDataOut);



/* Data Transfer API */

/**
 * This function configures the SPI device-specific parameters
 * before starting an SPI transfer.
 * 
 * @param nInstanceId - The identifier of the SPI core on which
 *               to setup the current device parameters.
 * @param pmDeviceParameters - Pointer to the HAL_spi_DeviceConfiguration
 *                          struct which contains the device configuration
 *                          parameters.
 */
void HAL_spi_SetDeviceParameters(uint32 nInstanceId, HAL_spi_DeviceParametersType *pmDeviceParameters);



/**
 * This function configures the transfer-specific parameters
 * before starting an SPI transfer.
 * 
 * @param nInstanceId - The identifier of the SPI core on which to setup
 *                  the current transfer parameters.
 * @param pmTransferParameters - Pointer to the
 *                  HAL_spi_TransferParametersType struct which
 *                  contains the transfer parameters.
 */
void HAL_spi_SetTransferParameters(uint32 nInstanceId, HAL_spi_TransferParametersType *pmTransferParameters);



/**
 * This function starts the transfer previously configured, by
 * putting the core in RUN state.
 * 
 * @param nInstanceId - The identifier of the SPI core that is
 *               used to start the transfer.
 */
void HAL_spi_StartTransfer(uint32 nInstanceId);


/**
 * This function pauses an ongoing SPI transfer.
 * 
 * @param nInstanceId - The identifier of the SPI core that is
 *               used to pause the transfer.
 */
void HAL_spi_PauseTransfer(uint32 nInstanceId);



/**
 * This function resets an ongoing transfer, by placing the Core
 * in RESET state. Note that this is not the same as
 * HAL_spi_Reset where the Core is issued a soft reset and all
 * state information is lost. This function only resets the
 * transfer portion of the Core and maintains the configuration
 * information.
 * 
 * @param nInstanceId - The identifier of the SPI core that is
 *               used to reset the transfer.
 */
void HAL_spi_ResetTransfer(uint32 nInstanceId);



/**
 * This function reads the incoming data from the FIFO into the
 * pReadData pointer. The second parameter indicates how many
 * words to read from the FIFO. There is no error checking in
 * HAL, the driver has to ensure that an arbitrarily large value
 * is not passed. Reading from the FIFO when no data is present
 * will only return garbage. However, reading more than the
 * maximum number of units, set in the transfer configuration
 * register will result in incorrect usage model and an UNDER
 * RUN error may result.
 * 
 * @param nInstanceId - The identifier of the SPI core that is used to read data.
 * @param panReadData - Pointer to the buffer to place the data.
 * @param nWordsToRead - The number of 32-bit words to read from the buffer. 
 */
void HAL_spi_ReadData(uint32 nInstanceId, uint32 *panReadData, uint32 nWordsToRead);



/**
 * This function writes the outgoing data from the Driver space
 * to the FIFO. The second parameted indicates how many words to
 * write to the FIFO. The driver is assumed to have done error
 * checking.
 * 
 * @param nInstanceId - The identifier of the SPI core that is
 *                    used to write data.
 * @param panWriteData - Pointer to the buffer to written to the FIFO.
 * @param nWordsToWrite - The number of 32-bit words to write from the buffer to the FIFO.
 */
void HAL_spi_WriteData(uint32 nInstanceId, uint32 *panWriteData, uint32 nWordsToWrite);



/**
 * This function will acknowledge that all the data to be read
 * from the SPI bus has been sent out. The hardware usage model
 * requires this function to be called before proceeding to the
 * next transfer.
 * 
 * @param nInstanceId - The identifier of the SPI core on which
 *                    to acknowledge the flag.
 */
void HAL_spi_AcknowledgeMaxInputDoneFlag(uint32 nInstanceId);



/**This function will acknowledge that all the data to be sent on the SPI bus 
 * has been sent out. The hardware usage model requires this
 * function to be called before proceeding to the next transfer.
 * 
 * @param nInstanceId - The identifier of the SPI core on which to acknowledge the flag.
 */
void HAL_spi_AcknowledgeMaxOutputDoneFlag(uint32 nInstanceId);



/**
 * This function will acknowledge that a blocks worth of data to
 * be read from the SPI bus has been sent out. The hardware
 * usage model requires this function to be called before
 * proceeding to the next transfer. Applies only in block mode
 * (when the MX input count is non zero).
 * 
 * @param nInstanceId - The identifier of the SPI core on which
 *                    to acknowledge the flag.
 */
void HAL_spi_AcknowledgeInputServiceFlag(uint32 nInstanceId);



/**
 * This function will acknowledge that a blocks worth of data to
 * be sent on the SPI bus has been sent out. The hardware usage
 * model requires this function to be called before proceeding
 * to transfer the next block. Applies only in block mode (when
 * the MX output count is non zero).
 * 
 * @param nInstanceId - The identifier of the SPI core on which
 *                    to acknowledge the flag.
 */
void HAL_spi_AcknowledgeOutputServiceFlag(uint32 nInstanceId);

/* Status API */


/**
 * This function returns the current operational state of the
 * SPI core.
 *
 * @param nInstanceId - The identifier of the SPI core from
 *                    which to read the state.
 *
 * @return An enumeration in which the current run state is returned.
 */
HAL_spi_RunStateType HAL_spi_GetRunState(uint32 nInstanceId);



/**
 * This function will return the current value of the maximum
 * input unit count (the maximum number of n-bit units to be
 * transferred over the SPI bus). As the core transfers each
 * unit, it counts down this value and the function returns the
 * current value. This function applies only in Block and DM
 * mode
 * 
 * @param nInstanceId - The identifier of the SPI core on which to get the
 *                  current value of the maximum input count counter.
 * 
 * @return The maximum input count counter is returned.
 */
uint32 HAL_spi_GetCurrentMaxInputCount(uint32 nInstanceId);



/**
 * This function will return the current value of the maximum
 * output unit count (the maximum number of n-bit units to be
 * transferred over the SPI bus). As the core transfers each
 * unit, it counts down this value and the function returns the
 * current value. This function applies only in Block and DM
 * mode
 * 
 * @param nInstanceId - The identifier of the SPI core on which
 *               to get the current value of the maximum output
 *               count counter.
 * 
 * @return The maximum output count counter is returned.
 */
uint32 HAL_spi_GetCurrentMaxOutputCount(uint32 nInstanceId);



/**
 * This function returns the current value of the maximum read
 * count counter. This counter is set to the number of input
 * transfer units as specified in the transfer parameter
 * nNumInputTransfers.

 * @param nInstanceId - The identifier of the SPI core on which to get 
                 the current value of the maximum read count counter.

 * @return The maximum read count counter is returned.
 */
uint32 HAL_spi_GetCurrentMaxReadCount(uint32 nInstanceId);


/**
 * This function will return the current status of the Input
 * Service Flag. When there is Data in the Input FIFO, an
 * interrupt is generated and this flag is set. The Software has
 * to come and clear the flag to acknowledge reading the data.
 * 
 * @param nInstanceId - The identifier of the SPI core from
 *                    which to report.
 * 
 * @return boolean - TRUE if the input service flag is set.
 *         Otherwise, returns FALSE.
 */
boolean HAL_spi_IsInputServiceFlagSet(uint32 nInstanceId);



/**
 * This function will return the current status of the output
 * Service Flag. When there is no Data in the Output FIFO, and
 * the MX OUTPUT count current is non zero an interrupt is
 * generated and this flag is set. The Software has to come and
 * clear the flag to acknowledge writing the data to FIFO.
 * 
 * @param nInstanceId - The identifier of the SPI core from which to report.
 * 
 * @return boolean - TRUE if the output service flag is set. Otherwise, returns FALSE.
 */
boolean HAL_spi_IsOutputServiceFlagSet(uint32 nInstanceId);


/**
 * This function will return the status of the Max Input Done
 * Flag. This flag is set when the maximum number of units as
 * set in the MX Input Count are transferred over the SPI Bus.
 * 
 * @param nInstanceId - The identifier of the SPI core from
 *                    which to report.
 * 
 * @return boolean - TRUE if the maximum input done flag is set. Otherwise, returns FALSE.
 */
boolean HAL_spi_IsMaxInputDoneFlagSet(uint32 nInstanceId);


/**
 * This function will return the status of the Max Output Done
 * Flag. This flag is set when the maximum number of units as
 * set in the MX oputput Count are transferred over the SPI Bus.
 * 
 * @param nInstanceId - The identifier of the SPI core from which to report.
 * 
 * @return boolean - TRUE if the maximum output done flag is set. Otherwise, returns FALSE.   
 */
boolean HAL_spi_IsMaxOutputDoneFlagSet(uint32 nInstanceId);


/**
 * This function returns the value of the status flag in the
 * core which indicates that the SPI input FIFO is full.
 * 
 * @param nInstanceId - The identifier of the SPI core on which to get the
 *                  status of the input FIFO.
 * 
 * @return boolean - TRUE if the Input FIFO is full. Otherwise, returns FALSE.
 */
boolean HAL_spi_IsInputFifoFull(uint32 nInstanceId);


/**
 * This function will return the status of the Max Output Done
 * Flag. This flag is set when the maximum number of units as
 * set in the MX oputput Count are transferred over the SPI Bus.

 * @param nInstanceId - The identifier of the SPI core on which to get the 
                 status of the output FIFO.

 * @return boolean - TRUE if the output FIFO is full. Otherwise, returns FALSE.   
 */
boolean HAL_spi_IsOutputFifoFull(uint32 nInstanceId);


/**
 * This function returns the value of the status flag in the
 * core which indicates that the SPI input FIFO is not empty.
 * 
 * @param nInstanceId - The identifier of the SPI core on which to
 *                  get the status of the input FIFO.
 * @return boolean - TRUE if the input FIFO is not empty.
 *         Otherwise, returns FALSE.
 */
boolean HAL_spi_IsInputFifoNotEmpty(uint32 nInstanceId);


/**
 * This function returns the value of the status flag in the
 * core which indicates that the SPI output FIFO is not empty.
 * 
 * @param nInstanceId - The identifier of the SPI core on which to get 
 *               the status of the output FIFO.
 * @return boolean - TRUE if the output FIFO is not empty.
 *         Otherwise, returns FALSE
 */
boolean HAL_spi_IsOutputFifoNotEmpty(uint32 nInstanceId);


/**
 * This function returns the current number of words available
 * in the input FIFO.
 * 
 * @param nInstanceId - The identifier of the SPI core on which
 *              to get the current value of the maximum input
 *               count counter.
 * 
 * @return The current number of words available in the input FIFO.
 */
uint32 HAL_spi_GetInputFifoCount(uint32 nInstanceId);


/**
 * This function returns the current number of words in the
 * output FIFO.
 *
 *
 * @return nInstanceId - The identifier of the SPI core on
 *               which to get the current value of the maximum
 *               output count counter.
 * 
 * @return The current number of words that in the output FIFO.
 */
uint32 HAL_spi_GetOutputFifoCount(uint32 nInstanceId);



#ifdef __cplusplus
}
#endif

#endif /* HAL_SPI_H */
