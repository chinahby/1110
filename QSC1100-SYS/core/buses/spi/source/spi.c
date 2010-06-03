/*============================================================================
  FILE:         spi.c

  OVERVIEW:    This is the Serial Peripheral Interface (SPI) driver.

  The driver queues incoming transfer requests in the SPI transaction database
  and processes them on a first come first serve basis.

  New device tansfer setting combinations can be configured using configure 
  device and are added in the SPI device configuration table.

  DEPENDENCIES: - SpiInit needs to be called before any other SPI operation.
                - TLMM needs to be configured with the SPI pin functions.
                - For data mover support:
                  FEATURE_SPI_USES_DMOV must be enabled in custdrivers.h
                  DMOV SPI channel definitions are required in DM driver
 
                Copyright (c) 2007-2008 QUALCOMM Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/

/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

  $Header: //depot/asic/msmshared/users/wxie/spi/q1110/spi/source/spi.c#2 $$DateTime: 2009/01/08 12:53:36 $$Author: wxie $

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2008-06-12  jdt  Added Data Mover support.
                   Updated interface to clkregim.
                   Updated HAL interfaces after ISOD review.
                   Updated to unified coding standards.
  2008-05-14  jdt  Released driver for Raptor2
  2007-08-05  hj   Created

============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <spi.h>
#include <HALspi.h>
#include <clkrgm_msm.h>
#include <tramp.h>
#include <err.h>
#include <memory.h>
#include <sleep.h>
//#include <tlmm_gpio.h>
#include <gpio_1100.h>
//#include "quartz/KxMemory.h"

#ifdef FEATURE_SPI_USES_DMOV
   #include <dmov.h>
#endif

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

#define SPI_TRANSACTION_QUEUE_SIZE 12
#define SPI_MAX_DEVICE_CONFIGURATIONS 10
#define SPI_INVALID_VALUE (-1)

/* The SPI SW driver requires a mutex primitive to guard its state variables (since
 * the driver API will execute in client context). The Interrupt Requests generated 
 * by the hardware are handled in IST context and these ISRs also modify/access
 * the state variables. Currently the Critical Section macros can not be called in IST 
 * context and hence all the state variables are protected using INTLOCK macros.
 * However, to semantically distinguish the calls to INTLOCK for Mutual exclusion
 * and calls to INTLOCK to actually mask out all interrupts we define the following 
 * primitive. In future when Critical Sections are supported in ISTs, a cleaner mutual
 * exclusion implementation can be achieved */
#define SPI_MUTEX_LOCK INTLOCK()
#define SPI_MUTEX_FREE INTFREE()
/* The following macro defines  Interrupt locking mechanism for SPI. This may change 
 * depending on the Operating System */
#define SPI_INTERRUPT_LOCK INTLOCK()
#define SPI_INTERRUPT_UNLOCK INTFREE()

sleep_okts_handle gSleepHandle;

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/**
 * This is the internal transaction type used by the SPI
 * transaction queue.
 */
typedef struct
{
   uint32 nTransactionNumber; /**< the transacation number (starting with 0 from the very first transaction) */
   SpiTransactionResultType eTransactionResult; /**< the result of the transaction */
   SpiTransferType mTransaction; /**< the transfer parameters used for this transfer */
   uint32 *naReadptr; /**< for block mode, points to current block in panInputData */
   uint32 *naWriteptr; /**< for block mode, points to current block in panOutputData */
   uint32 nNumInputWords; /**< the number words to be copied from the input FIFO to the user buffer */
   uint32 nNumOutputWords; /**< the number of words to be copied from the user buffer to the output FIFO */
   boolean bOutputTransferDone; /**< flag set to TRUE when the output transfer is complete */
   boolean bInputTransferDone; /**< flag set to TRUE when the input transfer is complete */
   boolean bInfiniteMode; /**< flag indicates that the transfer requires SPI infinite mode */
   enum
   {
      SPI_IO_MODE_FIFO, SPI_IO_MODE_BLOCK, SPI_IO_MODE_DATA_MOVER
   } eIOmode; /**< the IO mode to be used for this transfer. In the current design, this is determined by the driver */
   void *pmClientUserTcb; /**< for synchronous mode, stores the TCB of the client which gets SPI DONE signal */
   rex_sigs_type mUserSignal; /**< the user signal to set when the transfer is complete (or set to 0 for no task signalling) */
} SpiTransactionType;


#ifdef FEATURE_SPI_USES_DMOV
typedef struct
{
   dmov_cmd_box_s_type       mSpiBoxCmd; /**< command used for transfers larger than the FIFO */
   dmov_cmd_single_s_type    mSpiSingleCmd; /**< command used for any remaining data that is less than the FIFO */
   dmov_cmd_single_s_type    mSpiLastCmd; /**< command used for remaining data that is not aligned to the FIFO block */
} SpiDmTxCmdListType;

typedef struct
{
   dmov_cmd_box_s_type       mSpiBoxCmd; /**< command used for transfers larger than the FIFO */
   dmov_cmd_single_s_type    mSpiSingleCmd; /**< command used for any remaining data that is less than the FIFO */
   dmov_cmd_box_s_type       mSpiDummyCmd; /**< command used to copy dummy data that the SPI core used to pad to the size of a block */
} SpiDmRxCmdListType;
#endif /* FEATURE_SPI_USES_DMOV */



/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/
/** This is the device configuration table. This is an array of pointers that
 * point to the device info struct's passed by the client to the driver.
 * The client must maintain the data in these struct's until the device is
 * deconfigured. */
static SpiDeviceInfoType *gapSpiDeviceInfoTable[SPI_MAX_DEVICE_CONFIGURATIONS];

/** This is the SPI transacation queue. Transactions are added
 *  to the queue when the client calls SpiSetupTransfer.
 */
static struct {
   SpiTransactionType aTransactions[SPI_TRANSACTION_QUEUE_SIZE];
   uint32 nCurrentIdx;
   uint32 nNumPending;
   uint32 nTotalDone;
   int32 nLastDevice;
   boolean bTransactionInProcess;
} gSpiTransactionQueue;

/** This is the global SPI core info obtained from the HAL */
static struct {
   uint32 nNumInstances; /**< the number of SPI cores on the system */
   uint32 nInputFifoSize; /**< the size of the SPI input FIFO (in 32-bit words) */
   uint32 nOutputFifoSize; /**< the size of the SPI output FIFO (in 32-bit words) */
   uint32 nInputBlockSize; /**< the size of a block in the SPI input FIFO (in 32-bit words) */
   uint32 nOutputBlockSize; /**< the size of a block in the SPI output FIFO (in 32-bit words) */
   uint32 nMaxNumInputTransfers; /**< the maximum number of transfers supported by SPI_MX_INPUT_COUNT */
   uint32 nMaxNumOutputTransfers; /**< the maximum number of transfers supportd by SPI_MX_OUTPUT_COUNT*/
} gSpiInfo;


#ifdef FEATURE_SPI_USES_DMOV
static dmov_xfer_desc_s_type gSpiDmTxXfer;
static dmov_xfer_desc_s_type gSpiDmRxXfer;

/**
 * This is the data mover command list used to copy data from
 * the user buffer to the SPI Output FIFO.
 * 
 * The data mover requires that this list be aligned on a
 * 64-byte boundary.
 */
static SpiDmTxCmdListType __align(8) gSpiDmTxCmdList;

/**
 * This is the data mover command list used to copy data from
 * the SPI Input FIFO to the user buffer.
 * 
 * The data mover requires that this list be aligned on a
 * 64-byte boundary.
 */
static SpiDmRxCmdListType __align(8) gSpiDmRxCmdList;

/**
 * This is a dummy variable used during the SPI Input FIFO data
 * mover transfer.
 * 
 * When the max input count counter has reached the end of the
 * transfer, the SPI core pads the SPI Input FIFO with zeros so
 * that there is always a full block's worth of data in the FIFO
 * when the transfer is complete.
 * 
 * Per the SPI core design, we must read out the entire block,
 * including this dummy data.
 * 
 * We copy this dummy data from from the SPI Input FIFO into
 * this dummy variable.
 */
static uint32 gnSpiDmDummy = 0;

/**
 * This is a pointer to a dummy buffer that is allocated to the
 * size of an output block. This block is used for output
 * transfers that are not aligned to the size of a block.
 * 
 * The SPI core always expects a full block of data to be
 * transferred into the output FIFO, even if the transfer does
 * not end on the size of a block.
 * 
 * When the data mover transfer is setup, any remaining data in
 * that is in the user buffer that is not aligned to the block
 * is copied into this dummy buffer. This way, we can guarantee
 * that the output data is always aligned to a block.
 */
static uint32 *gpanSpiDmLastTxBlock = NULL;
#endif


/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/
#ifdef FEATURE_SPI_USES_DMOV
void SpiDmRxCallback(const dmov_result_s_type *pResult, void *pUser);
static boolean SpiDmXferCmd(void);
#endif

/*======================================================================

  FUNCTION 		SpiEnableClock

  DESCRIPTION 	This function enables the SPI clock

  DEPENDENCIES    None
  PARAMETERS      None
  RETURN VALUE   	None
  SIDE EFFECTS   	None

======================================================================*/
static void SpiEnableClock(void)
{
   sleep_negate_okts(gSleepHandle);

   clk_regime_msm_enable(CLK_RGM_SPI0_M);
   // clk_regime_register_ahb_constant(CLK_AHB_SPI);
}



/*======================================================================

  FUNCTION 		SpiDisableClock

  DESCRIPTION 	This function disables the SPI clock

  DEPENDENCIES    None
  PARAMETERS      None
  RETURN VALUE   	None
  SIDE EFFECTS   	None

======================================================================*/
static void SpiDisableClock(void)
{
   // clk_regime_deregister_ahb_constant(CLK_AHB_SPI);
   clk_regime_msm_disable(CLK_RGM_SPI0_M);

   sleep_assert_okts(gSleepHandle);
}



/*======================================================================

  FUNCTION 		SpiGetNumInputWords

  DESCRIPTION 	Gets the number of input words required for the
               specified number of transfers based on the packing mode
               selected for the specified device ID.

  DEPENDENCIES    None
  PARAMETERS
               nNumTransfers - The number of transfers
               nDeviceId - The device ID for which to calculate the words

  RETURN VALUE   	The number of words required for the transfer
  SIDE EFFECTS   	None

======================================================================*/
static uint32 SpiGetNumInputWords(uint32 nNumTransfers, uint32 nDeviceId)
{
   uint32 nWordsToTransfer;
   SpiTransferParametersType *pmTransferParameters;

   nWordsToTransfer = nNumTransfers;
   pmTransferParameters = gapSpiDeviceInfoTable[nDeviceId]->pmTransferParameters;

   if(SPI_INPUT_PACKING_ENABLED == pmTransferParameters->eInputPacking)
   {
      if(pmTransferParameters->nNumBits <= 8)
      {
         nWordsToTransfer /= 4;
         if(nNumTransfers % 4)
         {
            nWordsToTransfer++;
         }
      }
      else if(pmTransferParameters->nNumBits <= 16)
      {
         nWordsToTransfer /= 2;
         if(nNumTransfers % 2)
         {
            nWordsToTransfer++;
         }
      }
   }
   return nWordsToTransfer;
}



/*======================================================================

  FUNCTION 		SpiGetNumOutputWords

  DESCRIPTION 	Gets the number of output words required for the
               specified number of transfers based on the packing mode
               selected for the specified device ID.

  DEPENDENCIES    None
  PARAMETERS
               nNumTransfers - The number of transfers
               nDeviceId - The device ID for which to calculate the words

  RETURN VALUE   	The number of words required for the transfer
  SIDE EFFECTS   	None

======================================================================*/
static uint32 SpiGetNumOutputWords(uint32 nNumTransfers, uint32 nDeviceId)
{
   uint32 nWordsToTransfer;
   SpiTransferParametersType *pmTransferParameters;

   nWordsToTransfer = nNumTransfers;
   pmTransferParameters = gapSpiDeviceInfoTable[nDeviceId]->pmTransferParameters;

   if(SPI_OUTPUT_UNPACKING_ENABLED == pmTransferParameters->eOutputUnpacking)
   {
      if(pmTransferParameters->nNumBits <= 8)
      {
         nWordsToTransfer /= 4;
         if(nNumTransfers % 4)
         {
            nWordsToTransfer++;
         }
      }
      else if(pmTransferParameters->nNumBits <= 16)
      {
         nWordsToTransfer /= 2;
         if(nNumTransfers % 2)
         {
            nWordsToTransfer++;
         }
      }
   }
   return nWordsToTransfer;
}



/*======================================================================

  FUNCTION 		SpiAddTransaction

  DESCRIPTION 	Adds an SPI transaction to the transaction queue.

  DEPENDENCIES    Must be called within an SPI_MUTEX_LOCK
  PARAMETERS
               pmNewTransfer - The transfer information to be used
                  in the transaction.

  RETURN VALUE   	TRUE if the transaction was successfully added.
                  FALSE if there was an error adding the transaction.
  SIDE EFFECTS   	Initializes a new transaction in the global SPI transaction
                  queue, qSpiTransactionQueue, with the transfer parameters
                  passed as the parameter to this function.

======================================================================*/
static SpiTransactionResultType SpiAddTransaction(SpiTransferType *pmNewTransfer)
{
   uint32 nNumInputWords;
   uint32 nNumOutputWords;
   SpiTransactionType *pmNewTransaction;
   uint32 nNextTransactionIdx;

   if(pmNewTransfer->nDeviceId >= SPI_MAX_DEVICE_CONFIGURATIONS)
   {
      return SPI_FAILURE;
   }
   else if(SPI_TRANSACTION_QUEUE_SIZE == gSpiTransactionQueue.nNumPending)
   {
      return SPI_TRY_LATER;
   }

   nNextTransactionIdx = (gSpiTransactionQueue.nCurrentIdx + gSpiTransactionQueue.nNumPending) % SPI_TRANSACTION_QUEUE_SIZE;
   pmNewTransaction = &gSpiTransactionQueue.aTransactions[nNextTransactionIdx];

   memset(pmNewTransaction, 0x0, sizeof(SpiTransactionType));
   pmNewTransaction->eTransactionResult = SPI_FAILURE;

   if((0 == pmNewTransfer->nNumInputTransfers) && (0 == pmNewTransfer->nNumOutputTransfers))
   {
      return SPI_FAILURE;
   }

   if(pmNewTransfer->nDeviceId > SPI_MAX_DEVICE_CONFIGURATIONS)
   {
      return SPI_FAILURE;
   }

   nNumInputWords = SpiGetNumInputWords(pmNewTransfer->nNumInputTransfers, pmNewTransfer->nDeviceId);
   nNumOutputWords = SpiGetNumOutputWords(pmNewTransfer->nNumOutputTransfers, pmNewTransfer->nDeviceId);

   pmNewTransaction->eIOmode = SPI_IO_MODE_BLOCK;

#ifdef FEATURE_SPI_USES_DMOV
   if((nNumInputWords > gSpiInfo.nInputBlockSize) || (nNumOutputWords > gSpiInfo.nOutputBlockSize))
   {
      pmNewTransaction->eIOmode = SPI_IO_MODE_DATA_MOVER;
   }
#endif

   if(SPI_IO_MODE_DATA_MOVER == pmNewTransaction->eIOmode)
   {
      if((pmNewTransfer->nNumInputTransfers > gSpiInfo.nMaxNumInputTransfers) ||
          (pmNewTransfer->nNumOutputTransfers > gSpiInfo.nMaxNumOutputTransfers))
      {
         if(pmNewTransfer->nNumOutputTransfers > pmNewTransfer->nNumInputTransfers)
         {
            /* It is not possible to generate an event at the end of the transfer if
             * the number of output transfers is greater than the number of input transfers.
             */
            return SPI_FAILURE;
         }
         else if((pmNewTransfer->nNumInputTransfers % gSpiInfo.nInputBlockSize) > 0)
         {
            /* It is not possible to generate an event at the end of the transfer if
             * the number of transfers is not aligned to the block size.
             */
            return SPI_FAILURE;
         }
         pmNewTransaction->bInfiniteMode = TRUE;
      }
   }
   else
   {
      if((pmNewTransfer->nNumInputTransfers > gSpiInfo.nMaxNumInputTransfers) ||
          (pmNewTransfer->nNumOutputTransfers > gSpiInfo.nMaxNumOutputTransfers))
      {
         return SPI_FAILURE;
      }
   }

   pmNewTransaction->mTransaction.nNumInputTransfers 
      = pmNewTransfer->nNumInputTransfers;
   pmNewTransaction->mTransaction.nNumOutputTransfers 
      = pmNewTransfer->nNumOutputTransfers;
   pmNewTransaction->mTransaction.nDeviceId
      = pmNewTransfer->nDeviceId;
   pmNewTransaction->mTransaction.panInputData
      = pmNewTransfer->panInputData;
   pmNewTransaction->mTransaction.panOutputData
      = pmNewTransfer->panOutputData;

   /*Initially the current read and write pointers will point to the start of the streams */
   pmNewTransaction->naReadptr
      = (uint32*) pmNewTransfer->panInputData;
   pmNewTransaction->naWriteptr
      = (uint32*)pmNewTransfer->panOutputData;

   pmNewTransaction->nNumInputWords = nNumInputWords;
   pmNewTransaction->nNumOutputWords = nNumOutputWords;

   pmNewTransaction->mTransaction.pfCallback
      = pmNewTransfer->pfCallback;
   pmNewTransaction->nTransactionNumber 
      = ++gSpiTransactionQueue.nTotalDone;

   pmNewTransaction->bInputTransferDone = FALSE;
   pmNewTransaction->bOutputTransferDone = FALSE;

   pmNewTransaction->pmClientUserTcb = rex_self(); 
   pmNewTransaction->mUserSignal = pmNewTransfer->mUserSignal;
   gSpiTransactionQueue.nNumPending++;

   return SPI_SUCCESS;
} /* END SpiAddTransaction */



/*======================================================================

  FUNCTION 		SpiGetHalDeviceParameters

  DESCRIPTION 	This function gets the device parameters for the specified
               device and returns them in the format required for the HAL.

  DEPENDENCIES    None
  PARAMETERS      
                  pmHalDeviceParameters - Pointer to the HAL parameters
                     that will be returned.

                  nDeviceId - The device ID to use to get the parameters.

  RETURN VALUE   	None

  SIDE EFFECTS   	None

======================================================================*/
static void SpiGetHalDeviceParameters(HAL_spi_DeviceParametersType *pmHalDeviceParameters, uint32 nDeviceId)
{
   SpiDeviceParametersType *pmDeviceParameters          
      = gapSpiDeviceInfoTable[nDeviceId]->pmDeviceParameters;
   SpiDeviceBoardInfoType *pmDeviceBoardInfo
      = gapSpiDeviceInfoTable[nDeviceId]->pmDeviceBoardInfo;

   switch (pmDeviceParameters->eClockPolarity)
   {
      case SPI_CLK_IDLE_HIGH:
         pmHalDeviceParameters->eClockPolarity = HAL_spi_CLK_IDLE_HIGH;
         break;

      case SPI_CLK_IDLE_LOW:
         pmHalDeviceParameters->eClockPolarity = HAL_spi_CLK_IDLE_LOW;
         break;

      default:
         pmHalDeviceParameters->eClockPolarity = HAL_spi_CLK_IDLE_HIGH;
         break;
   }

   switch (pmDeviceParameters->eTransferMode)
   {
      case SPI_INPUT_FIRST_MODE:
         pmHalDeviceParameters->eTransferMode = HAL_spi_INPUT_FIRST_MODE;
         break;

      case SPI_OUTPUT_FIRST_MODE:
         pmHalDeviceParameters->eTransferMode = HAL_spi_OUTPUT_FIRST_MODE;
         break;
   }

   switch(pmDeviceParameters->eClockMode)
   {
      case SPI_CLK_ALWAYS_ON:
         pmHalDeviceParameters->bClockAlwaysOn = TRUE;
         break;
      case SPI_CLK_NORMAL:
         /* fallthrough */
      default:
         pmHalDeviceParameters->bClockAlwaysOn = FALSE;
   }

   switch (pmDeviceParameters->eCSMode)
   {
      case SPI_CS_KEEP_ASSERTED:
         pmHalDeviceParameters->bChipSelectAlwaysOn = TRUE;
         break;
      case SPI_CS_DEASSERT:
         /* fallthough */
      default:
         pmHalDeviceParameters->bChipSelectAlwaysOn = FALSE;
   }

   switch (pmDeviceParameters->eCSPolarity)
   {
      case SPI_CS_ACTIVE_LOW:
         pmHalDeviceParameters->eCSPolarity = HAL_spi_CS_ACTIVE_LOW;
         break;

      case SPI_CS_ACTIVE_HIGH:
         pmHalDeviceParameters->eCSPolarity = HAL_spi_CS_ACTIVE_HIGH;
         break;

      default:
         pmHalDeviceParameters->eCSPolarity = HAL_spi_CS_ACTIVE_LOW;
         break;
   }
   pmHalDeviceParameters->nChipSelectId = pmDeviceBoardInfo->nSlaveNumber;
} /* END spi_getDeviceConfiguration */



/*======================================================================

  FUNCTION 		SpiGetHalTransferParameters

  DESCRIPTION 	This function gets the transfer parameters for the specified
               device and returns them in the format required for the HAL.

  DEPENDENCIES    None
  PARAMETERS      
                  pmHalTransferParameters - Pointer to the HAL parameters
                     that will be returned.

                  nDeviceId - The device ID to use to get the parameters.

  RETURN VALUE   	None

  SIDE EFFECTS   	None

======================================================================*/
static void SpiGetHalTransferParameters(HAL_spi_TransferParametersType *pmHalTransferParameters, uint32 nDeviceId)
{
   SpiTransferParametersType* pmTransferParameters
      = gapSpiDeviceInfoTable[nDeviceId]->pmTransferParameters;

   pmHalTransferParameters->nNumBits = pmTransferParameters->nNumBits;

   if(SPI_LOOPBACK_ENABLED == pmTransferParameters->eLoopbackMode)
   {
      pmHalTransferParameters->eLoopbackMode = HAL_spi_LOOPBACK_ENABLED;
   }
   else
   {
      pmHalTransferParameters->eLoopbackMode = HAL_spi_LOOPBACK_DISABLED;
   }

   if(SPI_INPUT_PACKING_ENABLED == pmTransferParameters->eInputPacking)
   {
      pmHalTransferParameters->bReceivedDataPacked = TRUE;
   }
   else
   {
      pmHalTransferParameters->bReceivedDataPacked = FALSE;
   }

   if(SPI_OUTPUT_UNPACKING_ENABLED == pmTransferParameters->eOutputUnpacking)
   {
      pmHalTransferParameters->bTransmitDataPacked = TRUE;
   }
   else
   {
      pmHalTransferParameters->bTransmitDataPacked = FALSE;
   }

   /*
    * Always enable output bit shift enable so that there is
    * less confusion about how unpacked data is to be placed in the
    * user buffer.
    */
   pmHalTransferParameters->bOutputBitShift = TRUE;

   if(0 == pmHalTransferParameters->nNumInputTransfers)
   {
      pmHalTransferParameters->eTransferDirection = HAL_spi_OUTPUT_ONLY;
   }
   else if(0 == pmHalTransferParameters->nNumOutputTransfers)
   {
      pmHalTransferParameters->eTransferDirection = HAL_spi_INPUT_ONLY;
   }
   else
   {
      pmHalTransferParameters->eTransferDirection = HAL_spi_BI_DIRECTIONAL;
   }
} /* END SpiGetHalTransferParameters */



/*======================================================================

  FUNCTION 		SpiStartTransaction

  DESCRIPTION 	   1. Configures the SPI clock to the frequency required by the
                  transaction and calculates the number of ticks required to 
                  meet the chip-select deassertion time transfer parameter.

                  2. Configures the SPI core to be the master or slave as needed by
                  the transaction.

                  3. Configures the HAL with the device and transfer
                  parameters needed by the transaction. 

                  4. Sets the global SPI data mover data structures as needed for
                  the current SPI transfer.

                  5. Calls HAL_spi_StartTransfer to start the SPI transfer.


  DEPENDENCIES    None
  PARAMETERS      pmTransactionToSchedule - pointer to the transaction
  RETURN VALUE   	None
  SIDE EFFECTS   	None

======================================================================*/
static void SpiStartTransaction(SpiTransactionType *pmTransactionToSchedule)
{
   SpiDeviceParametersType *pmDeviceParameters;
   HAL_spi_DeviceParametersType mHalDeviceParameters;
   HAL_spi_TransferParametersType mHalTransferParameters;
   uint32 nMaxSlaveFrequency, nMinSlaveFrequency, nActualClkSpeed, nDeassertionTime, nDeassertionClockTicks;
   uint8 nSpiCoreNumber = gapSpiDeviceInfoTable[gSpiTransactionQueue.aTransactions[gSpiTransactionQueue.nCurrentIdx].mTransaction.nDeviceId]->pmDeviceBoardInfo->nCoreNumber;
   uint32 nThisDeviceId = pmTransactionToSchedule->mTransaction.nDeviceId;
   SpiDeviceBoardInfoType *pmDeviceBoardInfo
      = gapSpiDeviceInfoTable[nThisDeviceId]->pmDeviceBoardInfo;

   if(gSpiTransactionQueue.nLastDevice != nThisDeviceId)
   {
      /* 
       * Here, we set the SPI clock frequency to the value specified by the device parameters.
       *
       * If the SPI clock frequency could not be configured by clock regime to within the range specified,
       * then report a fatal error. Note that the clock frequency was already tested to see if it is
       * configurable when the client called SpiSetupTransfer, so if the clock cannot be configured
       * here, then the client must have changed the parameter since SpiSetupTransfer was called.
       *
       * Also, using the actual SPI clock frequency that was returned by the clock regime configuration,
       * we calculate the number of ticks needed to meet the minimum chip-select deassertion
       * wait time requred by the device parameters.
       */
      pmDeviceParameters = gapSpiDeviceInfoTable[gSpiTransactionQueue.aTransactions[gSpiTransactionQueue.nCurrentIdx].mTransaction.nDeviceId]->pmDeviceParameters;
      nMaxSlaveFrequency = pmDeviceParameters->nMaxSlaveFrequency;
      nMinSlaveFrequency = pmDeviceParameters->nMinSlaveFrequency;
      nDeassertionTime = pmDeviceParameters->nDeassertionTime;
      // nActualClkSpeed = clk_regime_sel_spi_clk(nMaxSlaveFrequency);
      nActualClkSpeed = nMaxSlaveFrequency;
      if((nMinSlaveFrequency > 0) && (nActualClkSpeed < nMinSlaveFrequency))
      {
         ERR_FATAL("Could not configure SPI clock to match slave device specifications", 0, 0, 0);
      }

      /* Calculate the number of clock ticks to meet the deassertion time requirements */
      if(nDeassertionTime > 0)
      {
         nDeassertionClockTicks = nDeassertionTime / (1000000000UL / nActualClkSpeed) + 1;
      }
      else
      {
         nDeassertionClockTicks = 0;
      }
      mHalDeviceParameters.nDeassertionClockTicks = nDeassertionClockTicks;

      /*
       * Set the core to be the master or slave. This is currently configured on a
       * per-device basis. However, switching between master and slave modes
       * would be highly unusual.
       */
      if(SPI_CORE_MODE_SLAVE == pmDeviceBoardInfo->eCoreMode)
      {
         HAL_spi_SetCoreMode(nSpiCoreNumber, HAL_spi_SLAVE);

         /* Currently, due to a bug in the hardware usage model, the Clock over run error
          * is reported in slave mode, although the data is not lost. Hence, we turn off
          * the Clock OverRun error from being reported at all in slave mode. 
          */
         HAL_spi_ConfigureErrors(nSpiCoreNumber,(uint32) (HAL_spi_CLOCK_UNDERRUN_ERROR | 
                                                          HAL_spi_INPUT_OVERRUN_ERROR | HAL_spi_OUTPUT_UNDERRUN_ERROR | HAL_spi_INPUT_UNDERRUN_ERROR | 
                                                          HAL_spi_OUTPUT_OVERRUN_ERROR | HAL_spi_TIMEOUT_ERROR ));
      }
      else
      {
         HAL_spi_SetCoreMode(nSpiCoreNumber, HAL_spi_MASTER);
      }

      /*
       * Here, we call SpiGetHalDeviceParameters to copy the driver's device
       * parameters into the data structure required by the HAL, and then
       * we call HAL_spi_SetDeviceParameters to configure the hardware registers
       * with the device parameters.
       */
      SpiGetHalDeviceParameters(&mHalDeviceParameters, nThisDeviceId);
      HAL_spi_SetDeviceParameters(nSpiCoreNumber, &mHalDeviceParameters);
      gSpiTransactionQueue.nLastDevice = nThisDeviceId;
   }

   /*
    * Here, now we copy the driver's transfer parameters into the data structure
    * required by the HAL and set the transfer parameters in the hardware registers.
    */
   mHalTransferParameters.nNumOutputTransfers = pmTransactionToSchedule->mTransaction.nNumOutputTransfers;
   mHalTransferParameters.nNumInputTransfers = pmTransactionToSchedule->mTransaction.nNumInputTransfers;

   SpiGetHalTransferParameters(&mHalTransferParameters, nThisDeviceId);

#ifdef FEATURE_SPI_USES_DMOV
   if(SPI_IO_MODE_DATA_MOVER == pmTransactionToSchedule->eIOmode)
   {
      /*
       * In data mover mode, we support so-called infinite mode,
       * which means that the max input done and max output done
       * counters are not used and are set to 0.
       *
       * This mode is only used when the number of transfers is
       * greater than the maximum value allowed by the max done
       * counter registers.
       *
       */
      if(pmTransactionToSchedule->bInfiniteMode)
      {
         mHalTransferParameters.nNumInputTransfers = 0;
         mHalTransferParameters.nNumOutputTransfers = 0;
      }
      mHalTransferParameters.eIOMode = HAL_spi_DATA_MOVER_MODE;

      /*
       * We are using data mover mode, so we need to set up the
       * data mover with the transfer descriptors used for this
       * transfer.
       */
      SpiDmXferCmd();
   }
   else
   {
      mHalTransferParameters.eIOMode = HAL_spi_BLOCK_MODE;
   }
#else
   mHalTransferParameters.eIOMode = HAL_spi_BLOCK_MODE;
#endif /* FEATURE_SPI_USES_DMOV */
   HAL_spi_SetTransferParameters(nSpiCoreNumber, &mHalTransferParameters);

   /* Finally, we start the transfer by placing the SPI core in the Run state */
   HAL_spi_StartTransfer(nSpiCoreNumber);
} /* END SpiStartTransaction */



/*======================================================================

  FUNCTION 		SpiOutputServiceIsr

  DESCRIPTION 	Services the output service and/or the
               max output done interrupt request.

               In block mode, writes data from the user buffer into the
               output FIFO.

               Acknowledges the max output done flag and determines if
               a task needs to be signaled to indicate
               that the transfer was completed.
  
               Determines if next transfer is queued and calls the
               appropriate HAL function to setup the transfer.

  DEPENDENCIES    None
  PARAMETERS      None
  RETURN VALUE   	None
  SIDE EFFECTS   	Updates the status of the current transaction in the
                  transaction queue and starts the next transaction if
                  one is pending.

======================================================================*/
static void SpiOutputServiceIsr(void)
{
   boolean bSignalCurrentClient = FALSE;
   boolean bCallCallback = FALSE;
   uint8 nSpiCoreNumber;
   uint32 nDeviceId;
   uint32 nWordsToWrite;
   uint32 nTransactionInProgress;
   SpiTransactionType *mpCurrentTransaction;
   HAL_spi_RunStateType runState;
   rex_sigs_type mUserSignal;


   SPI_MUTEX_LOCK;

   nDeviceId = gSpiTransactionQueue.aTransactions[gSpiTransactionQueue.nCurrentIdx].mTransaction.nDeviceId;
   nSpiCoreNumber = gapSpiDeviceInfoTable[nDeviceId]->pmDeviceBoardInfo->nCoreNumber;
   mpCurrentTransaction = &gSpiTransactionQueue.aTransactions[gSpiTransactionQueue.nCurrentIdx];
   nTransactionInProgress = gSpiTransactionQueue.nCurrentIdx;

   runState = HAL_spi_GetRunState(nSpiCoreNumber);
   if(runState == HAL_spi_RESET_STATE)
   {
      /* Ignore spurious interrupts
       * Per VI test results, spurious interrupts may occur while in the RESET state */
      HAL_spi_AcknowledgeOutputServiceFlag(nSpiCoreNumber);
      HAL_spi_AcknowledgeMaxOutputDoneFlag(nSpiCoreNumber);
      MSG_HIGH("SPI: Spurious output interrupt occurred while core was not in Reset state", 0, 0, 0);
   }
   else
   {
      /* If the max output done flag is set, this means that this interrupt occurred because
       * the output transfer is complete. */
      if(HAL_spi_IsMaxOutputDoneFlagSet(nSpiCoreNumber))
      {
         HAL_spi_AcknowledgeMaxOutputDoneFlag(nSpiCoreNumber);
         /* Check to see if the input transfer is also complete (or if there was no input transfer) */
         if((0 == gSpiTransactionQueue.aTransactions[gSpiTransactionQueue.nCurrentIdx].mTransaction.nNumInputTransfers) ||
             gSpiTransactionQueue.aTransactions[gSpiTransactionQueue.nCurrentIdx].bInputTransferDone)
         {
            /* We are done with both the input and output transfer, so this transaction is complete.
             * Now, proceed to the the next pending transaction and signal the client that that current
             * transaction is complete. */

            /* Set the core state to the reset state to indicate. This is necessary to start a new
             * transfer. */
            HAL_spi_ResetTransfer(nSpiCoreNumber);

            /* Schedule next transfer if any */ 
            gSpiTransactionQueue.nNumPending--;
            if(gSpiTransactionQueue.nNumPending > 0)
            {
               /* Go to the next transaction */
               gSpiTransactionQueue.nCurrentIdx = (gSpiTransactionQueue.nCurrentIdx + 1) % SPI_TRANSACTION_QUEUE_SIZE;
               SpiStartTransaction(&gSpiTransactionQueue.aTransactions[gSpiTransactionQueue.nCurrentIdx]);
            }
            else
            {
               /* Clear the Transaction In Process variable */
               gSpiTransactionQueue.bTransactionInProcess = FALSE;
               SpiDisableClock();
            }
            bSignalCurrentClient = TRUE;
            /* Do housekeeping work here, like setting the debug variables, transfer results etc.*/
         }
         /* Else do nothing since the read operation for this transaction hasnt finished yet, we let the read_isr handle user signalling*/
         gSpiTransactionQueue.aTransactions[gSpiTransactionQueue.nCurrentIdx].bOutputTransferDone = TRUE;
      }
      else if(mpCurrentTransaction->eIOmode == SPI_IO_MODE_BLOCK)
      {
         /* If the interrupt did not occur due to the max output done flag, then
          * we know it must have occurred due to an output block service request.
          * This means that the output FIFO needs to be filled with a block's worth
          * of data to continue the output transfer. */
         /* Write to the output FiFo */ 
         nWordsToWrite = (mpCurrentTransaction->mTransaction.panOutputData + 
                          mpCurrentTransaction->nNumOutputWords) - mpCurrentTransaction->naWriteptr;

         if(nWordsToWrite >= gSpiInfo.nOutputBlockSize)
         {
            nWordsToWrite = gSpiInfo.nOutputBlockSize;
         }

         HAL_spi_AcknowledgeOutputServiceFlag(nSpiCoreNumber);
         HAL_spi_WriteData(nSpiCoreNumber, mpCurrentTransaction->naWriteptr, nWordsToWrite);
         gSpiTransactionQueue.aTransactions[gSpiTransactionQueue.nCurrentIdx].naWriteptr += nWordsToWrite;
      }

      if(bSignalCurrentClient)
      {
         /*
          * We are done with the transfer, so signal the user client.
          * If a callback function was configured, then we call the callback.
          * Otherwise, if a user task signal was configured, then we set the signal.
          */
         gSpiTransactionQueue.aTransactions[gSpiTransactionQueue.nCurrentIdx].eTransactionResult = SPI_SUCCESS; 
         if(NULL == gSpiTransactionQueue.aTransactions[nTransactionInProgress].mTransaction.pfCallback)
         {
            mUserSignal = gSpiTransactionQueue.aTransactions[nTransactionInProgress].mUserSignal;
            if(mUserSignal > 0)
            {
               rex_set_sigs((rex_tcb_type*) gSpiTransactionQueue.aTransactions[nTransactionInProgress].pmClientUserTcb, 
                            mUserSignal);
            }
         }
         else
         {
            bCallCallback = TRUE;
         }
      }
   }
   SPI_MUTEX_FREE;

   /*
    * Call the client-installed callback function. Since the duration of the callback function is unknown,
    * we call this function outside of the mutex lock.
    */
   if(bCallCallback)
   {
      (*gSpiTransactionQueue.aTransactions[nTransactionInProgress].mTransaction.pfCallback)(SPI_SUCCESS);
   }
} /* END SpiOutputServiceIsr */



/*======================================================================

  FUNCTION 		SpiInputServiceIsr

  DESCRIPTION 	Services the input service and/or the max input done
               interrupt request.

               In block mode, reads the data from the input FIFO and
               copies it into the user buffer.

               Acknowledges the max input done flag and determines if
               a task needs to be signaled to indicate
               that the transfer was completed.
  
               Determines if next transfer is queued and calls the
               appropriate HAL function to setup the transfer.

  DEPENDENCIES    None
  PARAMETERS      None
  RETURN VALUE   	None
  SIDE EFFECTS   	Updates the status of the current transaction in the
                  transaction queue and starts the next transaction if
                  one is pending.

======================================================================*/
static void SpiInputServiceIsr(void)
{
   boolean bSignalCurrentClient = FALSE;
   boolean bLastReadDone = FALSE;
   boolean bCallCallback = FALSE;
   uint8 nSpiCoreNumber;
   uint32 nWordsToRead;
   uint32 nTransactionInProgress;
   uint32 nDeviceId;
   SpiTransactionType *mpCurrentTransaction;
   HAL_spi_RunStateType runState;
   rex_sigs_type mUserSignal;


   SPI_MUTEX_LOCK;

   nDeviceId = gSpiTransactionQueue.aTransactions[gSpiTransactionQueue.nCurrentIdx].mTransaction.nDeviceId;
   nSpiCoreNumber = gapSpiDeviceInfoTable[nDeviceId]->pmDeviceBoardInfo->nCoreNumber;
   mpCurrentTransaction = &gSpiTransactionQueue.aTransactions[gSpiTransactionQueue.nCurrentIdx];
   nTransactionInProgress = gSpiTransactionQueue.nCurrentIdx;

   runState = HAL_spi_GetRunState(nSpiCoreNumber);
   if(runState != HAL_spi_RUN_STATE)
   {
      /* Ignore spurious interrupts
       * Per VI test results, spurious interrupts may occur while in the RESET state */
      HAL_spi_AcknowledgeInputServiceFlag(nSpiCoreNumber);
      HAL_spi_AcknowledgeMaxInputDoneFlag(nSpiCoreNumber);
      MSG_HIGH("SPI: Spurious input interrupt occurred while core was not in Reset state", 0, 0, 0);
   }
   else
   {
      /*
       * If we are in data mover mode, then this interrupt occurred because of the
       * max input done flag and we know that the transfer is complete.
       * There is no need to check the input service flag.
       */
      if(SPI_IO_MODE_DATA_MOVER == mpCurrentTransaction->eIOmode)
      {
         bLastReadDone = TRUE;
      }
      else
      {
         /* We are in block mode, so we need to check if the interrupt was caused 
          * by a block service request. In this case, a block of data is available in
          * in the input FIFO and so we must read it out and place it in the user buffer.
          */
         if(HAL_spi_IsInputServiceFlagSet(nSpiCoreNumber))
         {
            nWordsToRead = (mpCurrentTransaction->mTransaction.panInputData
                            + mpCurrentTransaction->nNumInputWords) - mpCurrentTransaction->naReadptr;

            if(nWordsToRead >= gSpiInfo.nInputBlockSize)
            {
               nWordsToRead = gSpiInfo.nInputBlockSize;
            }

            if((mpCurrentTransaction->naReadptr + nWordsToRead) >= 
                (mpCurrentTransaction->mTransaction.panInputData + mpCurrentTransaction->nNumInputWords))
            {
               bLastReadDone = TRUE;
            }

            HAL_spi_ReadData(nSpiCoreNumber, mpCurrentTransaction->naReadptr, nWordsToRead);
            gSpiTransactionQueue.aTransactions[gSpiTransactionQueue.nCurrentIdx].naReadptr += nWordsToRead;
            HAL_spi_AcknowledgeInputServiceFlag(nSpiCoreNumber);
         }
      }

      /*
       * If the max input done flag is set and we have read all of the input data,
       * then we proceed to signal the user that 
       */
      if(HAL_spi_IsMaxInputDoneFlagSet(nSpiCoreNumber) && bLastReadDone)
      {
         HAL_spi_AcknowledgeMaxInputDoneFlag(nSpiCoreNumber);
         /* This means that the Total number of Words to transfer were transferred 
          *on the SPI bus. The Core is done doing the read part of the transfer. Schedule the next transfer*/
         if((0 == gSpiTransactionQueue.aTransactions[gSpiTransactionQueue.nCurrentIdx].mTransaction.nNumOutputTransfers) ||
             gSpiTransactionQueue.aTransactions[gSpiTransactionQueue.nCurrentIdx].bOutputTransferDone)
         {
            /* Reset the Core (soft reset to retain Config parameters */
            HAL_spi_ResetTransfer(nSpiCoreNumber);
            /* We are truly done, proceed to the next transaction if required */
            gSpiTransactionQueue.nNumPending--;      
            if(gSpiTransactionQueue.nNumPending > 0)
            {
               /* Go to the next transaction */
               gSpiTransactionQueue.nCurrentIdx = (gSpiTransactionQueue.nCurrentIdx + 1) % SPI_TRANSACTION_QUEUE_SIZE;
               SpiStartTransaction(&gSpiTransactionQueue.aTransactions[gSpiTransactionQueue.nCurrentIdx]);
            }
            else
            {
               /* Clear the Transaction In Process variable */
               gSpiTransactionQueue.bTransactionInProcess = FALSE;
               SpiDisableClock();
            }
            bSignalCurrentClient = TRUE;
            /* Do housekeeping work here, like setting the debug variables, transfer results etc.*/
         }
         gSpiTransactionQueue.aTransactions[gSpiTransactionQueue.nCurrentIdx].bInputTransferDone = TRUE;
      }

      if(bSignalCurrentClient)
      {
         /*
          * We are done with the transfer, so signal the user client.
          * If a callback function was configured, then we call the callback.
          * Otherwise, if a user task signal was configured, then we set the signal.
          */
         gSpiTransactionQueue.aTransactions[gSpiTransactionQueue.nCurrentIdx].eTransactionResult = SPI_SUCCESS; 
         if(NULL == gSpiTransactionQueue.aTransactions[nTransactionInProgress].mTransaction.pfCallback)
         {
            mUserSignal = gSpiTransactionQueue.aTransactions[nTransactionInProgress].mUserSignal;

            if(mUserSignal > 0)
            {
               rex_set_sigs((rex_tcb_type*) gSpiTransactionQueue.aTransactions[nTransactionInProgress].pmClientUserTcb, 
                         mUserSignal);
            }
         }
         else
         {
            bCallCallback = TRUE;
         }
      }
   }
   SPI_MUTEX_FREE;

   /*
    * Call the client-installed callback function. Since the duration of the callback function is unknown,
    * we call this function outside of the mutex lock.
    */
   if(bCallCallback)
   {
      (*gSpiTransactionQueue.aTransactions[nTransactionInProgress].mTransaction.pfCallback)(SPI_SUCCESS);
   }
} /* END SpiInputServiceIsr */



/*======================================================================

  FUNCTION 		SpiErrorHandlerIsr

  DESCRIPTION 	Services the SPI error interrupt

  DEPENDENCIES    None
  PARAMETERS      None
  RETURN VALUE   	None
  SIDE EFFECTS   	None

======================================================================*/
static void SpiErrorHandlerIsr(void)
{
   /* For now we cause a Fatal Error. In the future, could be improved to
      call a user callback function and report the error. */

   ERR_FATAL("SPI Core encountered error xid: %d Error flags : %x", 0, 0, 0);
} /* END SpiErrorHandlerIsr */



#ifdef FEATURE_SPI_USES_DMOV
/*======================================================================

  FUNCTION 		SpiDmXferCmd

  DESCRIPTION 	Sends a transfer command to the data mover. Always does so
               from the top of SPI transaction database (pointed to by
               gSpiTransactionQueue.nCurrentIdx).

  DEPENDENCIES    None
  PARAMETERS      None
  RETURN VALUE   	None
  SIDE EFFECTS   	None

======================================================================*/
static boolean SpiDmXferCmd(void)
{
   SpiTransactionType *mpCurrentTransaction;
   const uint32 *panSPILastTXData;
   dmov_cmd_single_s_type *pCmdToPad;
   uint32 nDeviceId;
   uint32 nTransactionInProgress;
   uint32 nDMCommand;
   uint32 nBoxNumRows;
   uint32 nSingleLength;
   uint32 nDummyLength;
   uint32 nRemainderLength;
   uint32 i;
   uint8 nSpiCoreNumber;
   boolean returnValue = TRUE;

   nDeviceId = gSpiTransactionQueue.aTransactions[gSpiTransactionQueue.nCurrentIdx].mTransaction.nDeviceId;
   mpCurrentTransaction = &gSpiTransactionQueue.aTransactions[gSpiTransactionQueue.nCurrentIdx];
   nTransactionInProgress = gSpiTransactionQueue.nCurrentIdx;
   nSpiCoreNumber = gapSpiDeviceInfoTable[nDeviceId]->pmDeviceBoardInfo->nCoreNumber;

   /* Setup the data mover request for the output transfer.
    *
    *
    * If the transfer is greater than the FIFO size, then we use data mover
    * box mode commands to copy chunks of user data. The remainder of the user
    * data that is left over from the box commands is chained to a single item
    * command.
    *
    * If the transfer is larger than a block, but less than the FIFO size,
    * then we send a single item transfer up to the point that is aligned to
    * a block size.
    *
    * When the SPI core is in data mover mode, it expects data to be in
    * chunks that are aligned to a block size. So if the remainder of the data
    * to be transfered in the single item command is less than a block size,
    * we have to copy the data from the user buffer into a temporary buffer
    * (panSPILastTXData) that is the size of a block. We use the first single
    * item command to transfer all of the data up to the last full block, 
    * and then use the second single item command to transfer the final block.
    * 
    * If the size of the data for the first single item command happenes to be less
    * than or equal to a block, (that is, we don't need an additional command),
    * then instead of using a second single item command, we pad the first single
    * item command with data aligned to the size of a block and we don't use the
    * second single item command.
    *
    * Even if we have to pad the last remaining data to the size of a block, the
    * SPI_MX_OUTPUT_COUNT register still takes care of stopping the SPI core from
    * actually shifting the dummy data out on the bus, so the actual number of transfer
    * seen on the bus is still what is specified in nNumOutputWords.
    */
   if(mpCurrentTransaction->nNumOutputWords > 0)
   {
      /* calculate the number of rows needed for the box command */
      nBoxNumRows = (mpCurrentTransaction->nNumOutputWords) / gSpiInfo.nOutputFifoSize;  

      /* calculate the number words needed for the single command */
      nSingleLength = (mpCurrentTransaction->nNumOutputWords) % gSpiInfo.nOutputFifoSize;

      if(nBoxNumRows > 0)
      {
         /* Use CRCI to sync the data mover transfer to the SPI transmit.
          * Use a box command (multiple copy of source rows to destination rows, with offset).
          */
         nDMCommand = DMOV_FIELD(CMD_WORD,DST_CRCI_CTL,DMOV_CRCI_SPI_TX) |
                      DMOV_FIELD(CMD_WORD,ADDR_MODE,DMOV_ADDR_MODE_BOX);
         if(0 == nSingleLength)
         {
            nDMCommand |= DMOV_FIELD(CMD_WORD,LC,0x1);
         }
         /*
          * Here we setup the box command.
          * We want to copy multiple rows from the source (the user buffer)
          * to the destination (the Output FIFO). So each row is the size of the
          * output FIFO.
          */
         gSpiDmTxCmdList.mSpiBoxCmd.command = nDMCommand; 
         gSpiDmTxCmdList.mSpiBoxCmd.src_row_addr = (uint32)(mpCurrentTransaction->mTransaction.panOutputData);
         gSpiDmTxCmdList.mSpiBoxCmd.dst_row_addr = HWIO_PHYS(SPI_OUTPUT_FIFO); 
         gSpiDmTxCmdList.mSpiBoxCmd.dst_row_len = gSpiInfo.nOutputFifoSize * sizeof(uint32); 
         gSpiDmTxCmdList.mSpiBoxCmd.src_row_len = gSpiInfo.nOutputFifoSize * sizeof(uint32); 

         /* Set the number of rows to copy. The number of rows is the same for the source and destination. */
         gSpiDmTxCmdList.mSpiBoxCmd.dst_row_num = nBoxNumRows; 
         gSpiDmTxCmdList.mSpiBoxCmd.src_row_num = nBoxNumRows; 

         /* When we write to the output FIFO, we want to keep writing back to the same destination
          * address (the output FIFO), so we set the offset to 0. */
         gSpiDmTxCmdList.mSpiBoxCmd.dst_row_offset = 0; 

         /* As we copy rows from the user buffer, we want to keep
          * incrementing the source address, so we set the source row offset to the
          * size of the output FIFO.
          */
         gSpiDmTxCmdList.mSpiBoxCmd.src_row_offset = gSpiInfo.nOutputFifoSize * sizeof(uint32); 
         gSpiDmTxXfer.cmd_ptr = &gSpiDmTxCmdList.mSpiBoxCmd;
      }

      pCmdToPad = NULL;
      if(nSingleLength > 0)
      {
         /* Now we set up the single item transfer to any remaining data that is left over
          * from the box command. This is any data that is left over that is not aligned
          * to the size of the Output FIFO. */
         if(nSingleLength < gSpiInfo.nOutputBlockSize)
         {
            /* The number words needed for the single command is less than a block,
             * so we will use this single command as the final command.
             * We need to pad the data, we set pCmdToPad to point to this command.
             * Later, we will copy the user data into a dummy buffer that is aligned
             * to the size of a block.
             */
            pCmdToPad = &gSpiDmTxCmdList.mSpiSingleCmd;
            nRemainderLength = nSingleLength;
            nSingleLength = 0;
         }
         else
         {
            /* Use CRCI to sync the data mover transfer to the SPI transmit.
             * Use a single item command (simple copy of source to destination).
             */
            nDMCommand = DMOV_FIELD(CMD_WORD,DST_CRCI_CTL,DMOV_CRCI_SPI_TX) |
                         DMOV_FIELD(CMD_WORD,ADDR_MODE,DMOV_ADDR_MODE_SINGLE); 

            /* If the single item command is not aligned to the size of a block, 
             * then we use this single item command to send all of the data up to
             * the pointer of the remainder of the data that is not aligned, and then
             * we use an additional command to pad the remaining data to 
             * the size of a block.
             */
            nRemainderLength = nSingleLength % gSpiInfo.nOutputBlockSize;
            if(nRemainderLength != 0)
            {
               /* We need to use an additional command,
                * so set the last command pointer to the last command,
                * and subtract the remaining data that is left over from the single
                * item command from the single item length. This remaining data
                * will be sent in the additional "last" command */
               pCmdToPad = &gSpiDmTxCmdList.mSpiLastCmd;
               nSingleLength -= nRemainderLength;
            }
            else
            {
               /* The single item command is aligned to the size of a block,
                * so there is no need to use the additional command.
                * So we can set the single item command to be the last command (LC=1)
                */
               nDMCommand |= DMOV_FIELD(CMD_WORD,LC,0x1);
            }

            /* Set the command parameters for the single item command */
            gSpiDmTxCmdList.mSpiSingleCmd.command = nDMCommand; 

            /* The box command was used to send the first part of the user data, so
             * we set the single item command source address to point to the place in the
             * user buffer where the box command left off.
             */
            gSpiDmTxCmdList.mSpiSingleCmd.src_addr = (uint32)(mpCurrentTransaction->mTransaction.panOutputData 
                                                               + (nBoxNumRows * gSpiInfo.nOutputFifoSize));
            gSpiDmTxCmdList.mSpiSingleCmd.dst_addr = HWIO_PHYS(SPI_OUTPUT_FIFO);
            gSpiDmTxCmdList.mSpiSingleCmd.len = nSingleLength * sizeof(uint32);
         }

         /*
          * If the box command is not used, then we set the data mover command pointer to
          * start the command list with the single item command.
          */
         if(nBoxNumRows == 0)
         {
            gSpiDmTxXfer.cmd_ptr = &gSpiDmTxCmdList.mSpiSingleCmd;
         }
      }

      /*
       * Check to see if we needed to use the additional command used to pad the remaining data
       * from the single item command.
       */
      if(pCmdToPad != NULL)
      {
         /* We need to send the additional command in order to send a final block with the user
          * data padded with 0s to align the final data to the size of a block. */
         panSPILastTXData = mpCurrentTransaction->mTransaction.panOutputData + (nBoxNumRows * gSpiInfo.nOutputFifoSize) + nSingleLength;
         for(i = 0; i < nRemainderLength; i++)
         {
            gpanSpiDmLastTxBlock[i] = panSPILastTXData[i];
         }

         /* Set the data mover command to be the last command in the list.
          * Use CRCI to sync the data mover transfer to the SPI transmit.
          * Use a single item command (simple copy of source to destination).
          */
         nDMCommand = DMOV_FIELD(CMD_WORD,LC,0x1) |
            DMOV_FIELD(CMD_WORD,DST_CRCI_CTL,DMOV_CRCI_SPI_TX) |
            DMOV_FIELD(CMD_WORD,ADDR_MODE,DMOV_ADDR_MODE_SINGLE);

         pCmdToPad->command = nDMCommand;

         /* Set the source address to copy from the dummy block */
         pCmdToPad->src_addr = (uint32)gpanSpiDmLastTxBlock;

         /* Set the destination address to copy to the SPI output FIFO */
         pCmdToPad->dst_addr = HWIO_PHYS(SPI_OUTPUT_FIFO);

         /* Set the size to the size of the output block */
         pCmdToPad->len = gSpiInfo.nOutputBlockSize * sizeof(uint32);
      }

      /* Set the remaining parameters for the data mover transfer */
      gSpiDmTxXfer.device = DMOV_DEVICE_SPI;
      gSpiDmTxXfer.chan = DMOV_CHAN_SPI_TX;
      gSpiDmTxXfer.priority = DMOV_PRI_SPI;
      gSpiDmTxXfer.cmd_type = DMOV_CMD_LIST;
      gSpiDmTxXfer.callback_ptr = NULL;
      gSpiDmTxXfer.options = DMOV_XFER_DEFAULT;

      /* Setup the data mover transfer. The transfer shouldn't actually start until
       * we put the SPI core in the Run state. */
      if(!dmov_transfer(&(gSpiDmTxXfer)))
      {
         ERR_FATAL("SPI DMOV TX transfer failed!",0,0,0);
         returnValue = FALSE;
      }
   }

   /* Setup the data mover request for the input transfer.
    *
    * If the input transfer is greater than the FIFO size, then we use
    * data mover box mode command to copy chunks of user data.
    *
    * The remainder of the user data that is left over from the box commands
    * is chained to the single item command.
    *
    * If the input transfer is less than the FIFO size, then we skip
    * the box transfer and start directly with the single item command.
    *
    * When the SPI core is in data mover mode, if the received data is
    * less than a block, the remaining words in the block are filled
    * with 0. Since the SPI core expects the entire block to be read,
    * we use a box command to copy the remaining unused words into a
    * dummy word.
    *
    */
   if(mpCurrentTransaction->nNumInputWords > 0)
   {
      nBoxNumRows = (mpCurrentTransaction->nNumInputWords) / gSpiInfo.nInputFifoSize;  
      nSingleLength = (mpCurrentTransaction->nNumInputWords) % gSpiInfo.nInputFifoSize;
      nRemainderLength = nSingleLength % gSpiInfo.nInputBlockSize;
      if(nRemainderLength != 0)
      {
         nDummyLength = gSpiInfo.nInputBlockSize - nRemainderLength;
      }
      else
      {
         nDummyLength = 0;
      }

      /*
       * Setup the box command, if necessary
       */
      if(nBoxNumRows > 0)
      {
         /* Use CRCI to sync the data mover transfer to the SPI receive.
          * Use a box command (multiple copy of source rows to destination rows, with offset).
          */
         nDMCommand = DMOV_FIELD(CMD_WORD,SRC_CRCI_CTL,DMOV_CRCI_SPI_RX)
                      | DMOV_FIELD(CMD_WORD,ADDR_MODE,DMOV_ADDR_MODE_BOX); 

         /* If the single item command is not needed, then set this command to be 
          * the last command (LC = 1) */
         if(0 == nSingleLength)
         {
            nDMCommand |= DMOV_FIELD(CMD_WORD,LC,0x1); 
         }
         gSpiDmRxCmdList.mSpiBoxCmd.command = nDMCommand; 
         gSpiDmRxCmdList.mSpiBoxCmd.src_row_addr = HWIO_PHYS(SPI_INPUT_FIFO);
         gSpiDmRxCmdList.mSpiBoxCmd.dst_row_addr = (uint32)(mpCurrentTransaction->mTransaction.panInputData);
         gSpiDmRxCmdList.mSpiBoxCmd.dst_row_len = gSpiInfo.nInputFifoSize * sizeof(uint32); 
         gSpiDmRxCmdList.mSpiBoxCmd.src_row_len = gSpiInfo.nInputFifoSize * sizeof(uint32); 
         gSpiDmRxCmdList.mSpiBoxCmd.dst_row_num = nBoxNumRows; 
         gSpiDmRxCmdList.mSpiBoxCmd.src_row_num = nBoxNumRows; 
         gSpiDmRxCmdList.mSpiBoxCmd.dst_row_offset = gSpiInfo.nInputFifoSize * sizeof(uint32); 
         gSpiDmRxCmdList.mSpiBoxCmd.src_row_offset = 0; 

         gSpiDmRxXfer.cmd_ptr = &gSpiDmRxCmdList.mSpiBoxCmd;
      }

      /*
       * Setup the single item command, if necessary
       */
      if(nSingleLength > 0)
      {
         /* Use CRCI to sync the data mover transfer to the SPI receive.
          * Use a single item command (simple copy of source to destination).
          */
         nDMCommand = DMOV_FIELD(CMD_WORD,SRC_CRCI_CTL,DMOV_CRCI_SPI_RX)
                      | DMOV_FIELD(CMD_WORD,ADDR_MODE,DMOV_ADDR_MODE_SINGLE);

         /* If there isn't any dummy data expected in the input FIFO (because
          * the desired input transfer length is aligned to the size of an 
          * input FIFO block), then we set this command to be last command (LC=1).
          */
         if(0 == nDummyLength)
         {
            nDMCommand |= DMOV_FIELD(CMD_WORD,LC,0x1); 
         }
         gSpiDmRxCmdList.mSpiSingleCmd.command = nDMCommand;
         gSpiDmRxCmdList.mSpiSingleCmd.src_addr = HWIO_PHYS(SPI_INPUT_FIFO); 

         /* The box command was used to send the first part of the user data, so
          * we set the single item command destination address to point to the place in the
          * user buffer where the box command left off.
          */
         gSpiDmRxCmdList.mSpiSingleCmd.dst_addr = (uint32)(mpCurrentTransaction->mTransaction.panInputData + (gSpiInfo.nInputFifoSize * nBoxNumRows));
         gSpiDmRxCmdList.mSpiSingleCmd.len = nSingleLength * sizeof(uint32);

         /* If the box command was not used, then set the data mover transfer to begin
          * at the single item command.
          */
         if(nBoxNumRows == 0)
         {
            gSpiDmRxXfer.cmd_ptr = &gSpiDmRxCmdList.mSpiSingleCmd;
         }
      }

      /* Read out any extra data in the input FIFO, if necesssary */
      if(nDummyLength > 0)
      {
         /* 
          * - Set this command to be the last command (LC=1).
          * - Use CRCI to sync the data mover transfer to the SPI receive.
          * - Use a box command (multiple copy of source rows to destination
          *   rows, with offset).
          */
         nDMCommand = DMOV_FIELD(CMD_WORD,LC,0x1) |
                      DMOV_FIELD(CMD_WORD,SRC_CRCI_CTL,DMOV_CRCI_SPI_RX) |
                      DMOV_FIELD(CMD_WORD,ADDR_MODE,DMOV_ADDR_MODE_BOX);
         gSpiDmRxCmdList.mSpiDummyCmd.command = nDMCommand;

         /* 
          * Start reading from the input FIFO where we left off from the
          * single item transfer.
          */
         gSpiDmRxCmdList.mSpiDummyCmd.src_row_addr = HWIO_PHYS(SPI_INPUT_FIFO) + nSingleLength * sizeof(uint32);

         /* Copy the padded 0 data to the dummy buffer*/
         gSpiDmRxCmdList.mSpiDummyCmd.dst_row_addr = (uint32)&gnSpiDmDummy;

         /* 
          * We use a box command to repeatedly copy the padding words from the
          * input FIFO into our dummy word. So in this case, the size of each
          * row is the size of the dummy word.
          */
         gSpiDmRxCmdList.mSpiDummyCmd.dst_row_len = sizeof(uint32);

         /* The size of our source row is the size of the dummy padding data
          * we expect to be in the input FIFO */
         gSpiDmRxCmdList.mSpiDummyCmd.src_row_len = nDummyLength * sizeof(uint32);


         /* The number of rows to copy to is the number of dummy padding
          * words we expect to be in the input FIFO */
         gSpiDmRxCmdList.mSpiDummyCmd.dst_row_num = nDummyLength;

         /* The source length is just the one row of source data */
         gSpiDmRxCmdList.mSpiDummyCmd.src_row_num = 1;

         /* No need to use any offsets */
         gSpiDmRxCmdList.mSpiDummyCmd.dst_row_offset = 0;
         gSpiDmRxCmdList.mSpiDummyCmd.src_row_offset = 0;
      }

      /* Set the remaining parameters for the data mover transfer */
      gSpiDmRxXfer.device = DMOV_DEVICE_SPI;
      gSpiDmRxXfer.chan = DMOV_CHAN_SPI_RX;
      gSpiDmRxXfer.priority = DMOV_PRI_SPI;
      gSpiDmRxXfer.cmd_type = DMOV_CMD_LIST;

      if(mpCurrentTransaction->bInfiniteMode)
      {
         /* If we are in infinite mode, then the SPI ISR will not be called at the end of
          * the transfer, because the SPI_MX_INPUT_COUNT and SPI_MX_OUTPUT_COUNT are disabled
          * (set to 0). 
          *
          * Thus, we need to use the data mover callback to determine when the transfer is
          * complete. Note that we use the RX callback because the data mover transfer
          * from the SPI Input FIFO will only complete when the SPI transfer is finished.
          *
          * We can't use the TX callback, on the other hand, because it may be called before
          * the SPI transfer is actually finished (it is called as soon as the data mover copies
          * the last block into the SPI Output FIFO, which is not necessarily when the SPI transfer
          * is actually shifted out onto the bus). */
         gSpiDmRxXfer.callback_ptr = SpiDmRxCallback;
      }
      else
      {
         /* We are not in infinite mode, so we rely on the SPI ISRs to detect the end of
          the transfer */
         gSpiDmRxXfer.callback_ptr = NULL;
      }
      gSpiDmRxXfer.user_ptr = NULL;
      gSpiDmRxXfer.options = DMOV_XFER_DEFAULT;

      /* Setup the data mover transfer. The transfer shouldn't actually start until
       * we put the SPI core in the Run state. */
      if(!dmov_transfer(&(gSpiDmRxXfer)))
      {
         ERR_FATAL("SPI DMOV RX transfer failed!", 0, 0, 0);
         returnValue = FALSE;
      }
   }
   return returnValue;
} /* END SpiDmXferCmd */



/*======================================================================

  FUNCTION 		SpiDmRxCallback

  DESCRIPTION 	Checks the results of the DM SPI RX transfer.
               If result is failure then the client is notified of the 
               failure and we move to next transaction.

  DEPENDENCIES    None
  PARAMETERS      None
  RETURN VALUE   	None
  SIDE EFFECTS   	Updates the status of the current transaction.
                  Starts the next transaction in the transaction queue
                  if a transactino is pending. 

======================================================================*/
void SpiDmRxCallback(const dmov_result_s_type *pResult, void *pUser)
{
   boolean bStartNextSPITransfer = FALSE;
   boolean bCallCallback = FALSE;
   SpiTransactionResultType result = SPI_SUCCESS;
   SpiTransactionType *mpCurrentTransaction;
   uint32 nTransactionInProgress;
   uint32 nDeviceId;
   uint8 nSpiCoreNumber = 0;
   rex_sigs_type mUserSignal;

   SPI_MUTEX_LOCK;

   gSpiTransactionQueue.nLastDevice = SPI_INVALID_VALUE;
   nSpiCoreNumber = gapSpiDeviceInfoTable[gSpiTransactionQueue.aTransactions[gSpiTransactionQueue.nCurrentIdx].mTransaction.nDeviceId]->pmDeviceBoardInfo->nCoreNumber;

   /*
    * If the data mover operation failed, then reset the transfer and
    * disable the SPI clock, but do not proceed to the next transfer.
    */
   if(!DMOV_RESULT_OK(pResult))
   {
      result = SPI_FAILURE;
      HAL_spi_Reset(nSpiCoreNumber);
      SpiDisableClock();
      return;
   }

   /*
    *  Due to a bug in the hardware, we need to reset the core and 
    *  then reconfigure it to start the next transfer.
    */
   HAL_spi_Reset(nSpiCoreNumber);  

   nDeviceId = gSpiTransactionQueue.aTransactions[gSpiTransactionQueue.nCurrentIdx].mTransaction.nDeviceId;
   mpCurrentTransaction = &gSpiTransactionQueue.aTransactions[gSpiTransactionQueue.nCurrentIdx];
   nTransactionInProgress = gSpiTransactionQueue.nCurrentIdx;  

   gSpiTransactionQueue.nNumPending--;
   if(gSpiTransactionQueue.nNumPending > 0)
   {
      gSpiTransactionQueue.nCurrentIdx = (gSpiTransactionQueue.nCurrentIdx + 1) % SPI_TRANSACTION_QUEUE_SIZE;
      SpiStartTransaction(&gSpiTransactionQueue.aTransactions[gSpiTransactionQueue.nCurrentIdx]);
   }
   else
   {
      gSpiTransactionQueue.bTransactionInProcess = FALSE;
      SpiDisableClock();
   }

   gSpiTransactionQueue.aTransactions[gSpiTransactionQueue.nCurrentIdx].eTransactionResult = result;
   if(NULL == gSpiTransactionQueue.aTransactions[nTransactionInProgress].mTransaction.pfCallback)
   {
      mUserSignal = gSpiTransactionQueue.aTransactions[nTransactionInProgress].mUserSignal;
      rex_set_sigs((rex_tcb_type*) gSpiTransactionQueue.aTransactions[nTransactionInProgress].pmClientUserTcb, 
                   mUserSignal);
   }
   else
   {
      bCallCallback = TRUE;
   }  
   SPI_MUTEX_FREE;

   if(bCallCallback)
   {
      (*gSpiTransactionQueue.aTransactions[nTransactionInProgress].mTransaction.pfCallback)(result);
   }
}
#endif /* FEATURE_SPI_USES_DMOV */



/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/

/*======================================================================

  FUNCTION 		SpiInit

  DESCRIPTION 	Initializes the SPI driver

  DEPENDENCIES    None
  PARAMETERS      None
  RETURN VALUE   	SPI_INIT_SUCCESS if successful. 
                  SPI_INIT_FAILURE if an error occurred.
  SIDE EFFECTS   	None

======================================================================*/
SpiInitResultType SpiInit(void)
{
   uint8 nInstanceId;
   uint8 nDeviceId;
   SpiInitResultType result = SPI_INIT_SUCCESS;

  gSleepHandle = sleep_register("SPI", FALSE);

  gpio_tlmm_config(SPI_CLK);
  // TBD this should be configured according to the value passed in by client
  // nSlaveNumber of SpiDeviceBoardInfoType
  gpio_tlmm_config(SPI_CS_N);
  gpio_tlmm_config(SPI_DATA_O);
  gpio_tlmm_config(SPI_DATA_I);

   /* Install the SPI ISRs */ 
   tramp_set_isr(TRAMP_SPI1_INPUT_SERVICES_ISR, SpiInputServiceIsr);
   tramp_set_isr(TRAMP_SPI1_OUTPUT_SERVICES_ISR, SpiOutputServiceIsr);
   tramp_set_isr(TRAMP_SPI1_ERROR_ISR, SpiErrorHandlerIsr);

   SPI_MUTEX_LOCK;

   /* Points to the current ongoing transaction in the Transaction DB */
   gSpiTransactionQueue.nCurrentIdx = 0;
   /* This is the total number of transactions saved in the Transaction DB.
    * Each of these transactions will be processed in order */
   gSpiTransactionQueue.nNumPending = 0;
   gSpiTransactionQueue.nTotalDone = 0;
   gSpiTransactionQueue.nLastDevice = SPI_INVALID_VALUE;
   gSpiTransactionQueue.bTransactionInProcess = FALSE; 

   SPI_INTERRUPT_LOCK;

   /* It is assumed that even for multiple cores, the RTL settings for the cores will be the same, that is the 
    * FIFO and Block sizes will have the same values and hence we only read from the first core always */
   SpiEnableClock();

   gSpiInfo.nNumInstances = HAL_spi_GetNumInstances();
   gSpiInfo.nInputFifoSize = HAL_spi_GetInputFifoSize(0);
   gSpiInfo.nOutputFifoSize = HAL_spi_GetOutputFifoSize(0);
   gSpiInfo.nInputBlockSize = HAL_spi_GetInputBlockSize(0);
   gSpiInfo.nOutputBlockSize = HAL_spi_GetOutputBlockSize(0);
   gSpiInfo.nMaxNumInputTransfers = HAL_spi_GetMaxNumInputTransfers(0);
   gSpiInfo.nMaxNumOutputTransfers = HAL_spi_GetMaxNumOutputTransfers(0);

   SpiDisableClock();

   for (nInstanceId = 0; nInstanceId < gSpiInfo.nNumInstances; nInstanceId++)
   {
      HAL_spi_Init(nInstanceId, NULL);
   }

#ifdef FEATURE_SPI_USES_DMOV
   if(NULL == gpanSpiDmLastTxBlock)
   {
      gpanSpiDmLastTxBlock = malloc(gSpiInfo.nOutputBlockSize * sizeof(uint32));
   }

   if(NULL == gpanSpiDmLastTxBlock)
   {
      result = SPI_INIT_FAILURE;
   }
#endif

   for(nDeviceId = 0; nDeviceId < SPI_MAX_DEVICE_CONFIGURATIONS; nDeviceId++)
   {
      gapSpiDeviceInfoTable[nDeviceId] = NULL;
   }

   SPI_INTERRUPT_UNLOCK;
   SPI_MUTEX_FREE;

   return result;
} /* END SpiInit */



/*======================================================================

  FUNCTION 		SpiConfigureDevice

  DESCRIPTION 	Adds a device and corresponding device specific data to
               the device table.

               In the current design, the parameters passed in
               pmDeviceTransferInfo must be maintained by the client
               during the SPI transfers. The struct members must be
               maintained until SpiDeconfigureDevice is
               called to deinitialize the device.

               Since the user app maintains the parameters, once the 
               device is "configured" the parameters may be changed
               between transfers without having to call
               SpiConfigureDevice again.

  DEPENDENCIES    None
  PARAMETERS      pmDeviceTransferInfo - Pointer to the parameters
  RETURN VALUE   	If successful, returns the device ID which should be
                  passed by the client to the SpiSetupTransfer function
                  to initiate the transfer on this device.
                 
                  Returns SPI_FAILURE If a failure occurred while 
                  configuring the device parameters.
  SIDE EFFECTS   	Configures the SPI clock frequency with the frequency
                  of the device. Thus, should not be called while SPI
                  transfers are pending or in progress.

======================================================================*/
int32 SpiConfigureDevice(SpiDeviceInfoType* pmDeviceTransferInfo)
{
   uint32 nActualClkSpeed, nMinSlaveFrequency, nMaxSlaveFrequency;   
   int32 mResult = SPI_FAILURE;
   int32 i;

   SPI_MUTEX_LOCK;

   if((pmDeviceTransferInfo->pmDeviceBoardInfo != NULL) &&
       (pmDeviceTransferInfo->pmDeviceParameters != NULL) &&
       (pmDeviceTransferInfo->pmTransferParameters != NULL)
      )
   {
      nMaxSlaveFrequency = pmDeviceTransferInfo->pmDeviceParameters->nMaxSlaveFrequency;
      nMinSlaveFrequency = pmDeviceTransferInfo->pmDeviceParameters->nMinSlaveFrequency;
      // nActualClkSpeed = clk_regime_sel_spi_clk(nMaxSlaveFrequency);
      nActualClkSpeed = nMaxSlaveFrequency;

      if((nMinSlaveFrequency > 0) && (nActualClkSpeed < nMinSlaveFrequency))
      {
         mResult = SPI_FAILURE;
      }
      else
      {
         for(i = 0; i < SPI_TRANSACTION_QUEUE_SIZE; i++)
         {
            if(gapSpiDeviceInfoTable[i] == NULL)
            {
               mResult = i;
               gapSpiDeviceInfoTable[i] = pmDeviceTransferInfo;
               break;
            }
         }
      }
   }
   SPI_MUTEX_FREE

   return mResult;
} /* END SpiConfigureDevice */ 



/*======================================================================

  FUNCTION 		SpiDeonfigureDevice

  DESCRIPTION 	Removes the device from the device table.

               In the current design, the device info is stored in a
               table of pointers. This function sets the pointer for
               the specified device ID to NULL.

               Primarily intended for testing.

  DEPENDENCIES    None
  PARAMETERS      nDeviceId - the device ID of the device to remove
  RETURN VALUE   	None
  SIDE EFFECTS   	None

======================================================================*/
void SpiDeconfigureDevice(uint32 nDeviceId)
{
   SPI_MUTEX_LOCK;

   if(nDeviceId < SPI_TRANSACTION_QUEUE_SIZE)
   {
      gapSpiDeviceInfoTable[nDeviceId] = NULL;
   }

   SPI_MUTEX_FREE;
}



/*======================================================================

  FUNCTION 		SpiSetupTransfer

  DESCRIPTION
   Requests a bidirectional data transfer to be added to the transfer queue.
 
   This function immediately adds the transfer to the request queue, unless
   the transfer queue is full, in which case it returns SPI_TRY_LATER.
 
   If pfCallback is not NULL, then this function will return immediately.

   Otherwise, the function will wait until the signal passed in the mUserSignal
   element of pmNewTransfer is received.


  DEPENDENCIES    None

  PARAMETERS      
      piTransferType    The set of transmission parameters to be used.

  RETURN VALUE
      SPI_SUCCESS        The transfer was successfully setup (in Async mode) 
                           or the transfer was succefully carried out (in Sync mode)

      SPI_FAILURE       The transfer could not be setup or there was an error
                           in carrying out the transfer.

      SPI_TRY_LATER     The SPI transfer queue is full at the moment and the client
                           should back out for a random time and retry.
 

  SIDE EFFECTS    Sets the SPI clock frequency to the frequency required
                  by the transfer.

                  Adds a new transaction to the SPI transaction queue.

                  Starts the transaction if no transaction is in progress.

======================================================================*/
SpiTransactionResultType SpiSetupTransfer(SpiTransferType *pmNewTransfer)
{
   SpiTransactionResultType result = SPI_SUCCESS;
   boolean bScheduleTransfer = FALSE;
   boolean bWaitSignal = FALSE;

   uint32 nTempCurrentIdx;

   SPI_MUTEX_LOCK;
   result = SpiAddTransaction(pmNewTransfer);
   if((result == SPI_SUCCESS) && !gSpiTransactionQueue.bTransactionInProcess)
   {
      gSpiTransactionQueue.bTransactionInProcess = TRUE;
      bScheduleTransfer = TRUE;
   }

   if(bScheduleTransfer)
   {
      SpiEnableClock();
      nTempCurrentIdx = gSpiTransactionQueue.nCurrentIdx;
      SpiStartTransaction(&gSpiTransactionQueue.aTransactions[gSpiTransactionQueue.nCurrentIdx]);

      if(pmNewTransfer->mUserSignal > 0)
      {
         bWaitSignal = TRUE;
      }
   }
   SPI_MUTEX_FREE;

   /*
    * If the client passed a user signal parameter, then we wait here until
    * the transfer is complete. When the transfer is complete, the user signal
    * is set by the SPI ISR. In this way, the user task is in sync with the SPI
    * transfer. Alternatively, the client may pass a callback function instead
    * of a signal.
    */
   if(bWaitSignal)
   {
      rex_wait(pmNewTransfer->mUserSignal);

      rex_clr_sigs ((rex_tcb_type*) gSpiTransactionQueue.aTransactions[nTempCurrentIdx].pmClientUserTcb, 
                            pmNewTransfer->mUserSignal);
   }
      
   return result;
} /* END SpiSetupTransfer */

