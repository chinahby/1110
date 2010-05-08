/*============================================================================
  FILE:         HALspi.c

  OVERVIEW:     This is the hardware abstraction layer (HAL) interfaces for
                the Serial Peripheral Interface (SPI) driver.

  DEPENDENCIES: None
 
                Copyright (c) 2007-2008 QUALCOMM Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/

/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

  $Header: //depot/asic/msmshared/users/wxie/spi/q1110/spi/source/common/HALspi.c#1 $$DateTime: 2009/01/08 12:32:31 $$Author: wxie $

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2008-06-12  jdt  Updates after releasing SPI HAL ISOD
  2008-05-14  jdt  Released SPI HAL
  2007-08-05  hj   Created

============================================================================*/


/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/


#if (QC_WINCE_VERSION >= 700)
#define INTFREE_SAV
#define INTLOCK_SAV
#endif /* QC_WINCE_VERSION == 700 */

#include <HALspi.h>
#include <msm.h>
#include <HALspiPlatform.h>


/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
/*
 * Identifier string for this HAL.
 */
#define HAL_SPI_VERSION_STRING "HAL_SPI_V1"

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/

/* ===========================================================================
**  HAL_spi_Init
**
** ======================================================================== */
void HAL_spi_Init(uint32 nInstanceid, char** ppszVersion)
{
   if(ppszVersion != NULL)
   {
      *ppszVersion = HAL_SPI_VERSION_STRING;
   }
}



/* ===========================================================================
**  HAL_spi_Reset
**
** ======================================================================== */
void HAL_spi_Reset(uint32 nInstanceId)
{
   SPI_HWIO_OUT(nInstanceId, SPI_SW_RESET, 1);
} /* END HAL_spi_Reset */



/* ===========================================================================
**  HAL_spi_Save
**
** ======================================================================== */
void HAL_spi_Save(uint32 nInstanceid)
{
} /* END HAL_spi_Save */



/* ===========================================================================
**  HAL_spi_Restore
**
** ======================================================================== */
void HAL_spi_Restore(uint32 nInstanceid)
{
} /* END HAL_spi_Restore */



/* ===========================================================================
**  HAL_spi_GetNumInstances
**
** ======================================================================== */
uint32 HAL_spi_GetNumInstances(void)
{
   return HAL_SPI_NUM_CORES;
} /* END HAL_spi_GetNumInstances */



/* ===========================================================================
**  HAL_spi_GetInputBlockSize
**
** ======================================================================== */
uint32 HAL_spi_GetInputBlockSize(uint32 nInstanceId)
{
   uint32 nInputBlockSize;

   nInputBlockSize = SPI_HWIO_INF(nInstanceId, SPI_IO_MODES, INPUT_BLOCK_SIZE);

   /* Return the size in terms of the number of 32-bit words */
   switch (nInputBlockSize)
   {
      case 0:
         nInputBlockSize = 1;
         break;

      case 1:
         nInputBlockSize = 4;
         break;

      case 2:
         nInputBlockSize = 8;
         break;

      case 3:
         /* fallthrough */
      default:
         nInputBlockSize = 0;
   }
   return nInputBlockSize;
} /* END HAL_spi_GetInputBlockSize */



/* ===========================================================================
**  HAL_spi_GetOutputBlockSize
**
** ======================================================================== */
uint32 HAL_spi_GetOutputBlockSize(uint32 nInstanceId)
{
   uint32 nOutputBlockSize;

   nOutputBlockSize = SPI_HWIO_INF(nInstanceId, SPI_IO_MODES, OUTPUT_BLOCK_SIZE);

   /* Return the size in terms of the number of 32-bit words */
   switch (nOutputBlockSize)
   {
      case 0: 
         nOutputBlockSize = 1;
         break;

      case 1:
         nOutputBlockSize = 4;
         break;

      case 2:
         nOutputBlockSize = 8;
         break;

      case 3:
         /* fallthrough */
      default:
         nOutputBlockSize = 0;
   }

   return nOutputBlockSize;
} /* END HAL_spi_GetOutputBlockSize */



/* ===========================================================================
**  HAL_spi_GetInputFifoSize
**
** ======================================================================== */
uint32 HAL_spi_GetInputFifoSize(uint32 nInstanceId)
{
   uint32 nInputFifoSize;
   uint32 nInputBlockSize;

   nInputFifoSize = SPI_HWIO_INF(nInstanceId, SPI_IO_MODES, INPUT_FIFO_SIZE);
   nInputBlockSize = HAL_spi_GetInputBlockSize(nInstanceId);

   /* Return the size in terms of the number of 32-bit words */
   switch (nInputFifoSize)
   {
      case 0:
         nInputFifoSize = 2 * nInputBlockSize;
         break;

      case 1:
         nInputFifoSize = 4 * nInputBlockSize;
         break;

      case 2:
         nInputFifoSize = 8 * nInputBlockSize;
         break;

      case 3:
         /* fallthrough */
      default:
         nInputFifoSize = 0;
   }

   return nInputFifoSize;
} /* END HAL_spi_GetInputFifoSize */



/* ===========================================================================
**  HAL_spi_GetOutputFifoSize
**
** ======================================================================== */
uint32 HAL_spi_GetOutputFifoSize(uint32 nInstanceId)
{
   uint32 nOutputFifoSize;
   uint32 nOutputBlockSize;

   nOutputFifoSize = SPI_HWIO_INF(nInstanceId, SPI_IO_MODES, INPUT_FIFO_SIZE);
   nOutputBlockSize = HAL_spi_GetOutputBlockSize(nInstanceId);

   /* Return the size in terms of the number of 32-bit words */
   switch (nOutputFifoSize)
   {
      case 0:
         nOutputFifoSize = 2 * nOutputBlockSize;
         break;

      case 1:
         nOutputFifoSize = 4 * nOutputBlockSize;
         break;

      case 2:
         nOutputFifoSize = 8 * nOutputBlockSize;
         break;

      case 3:
         /* fallthrough */
      default:
         nOutputFifoSize = 0;
   }
   return nOutputFifoSize;        
} /* END HAL_spi_GetOutputFifoSize */



/* ===========================================================================
**  HAL_spi_GetMaxNumInputTransfers
**
** ======================================================================== */
uint32 HAL_spi_GetMaxNumInputTransfers(uint32 nInstanceId)
{
   return HAL_SPI_MAX_MX_INPUT_COUNT;
} /* END HAL_spi_GetMaxNumInputTransfers */



/* ===========================================================================
**  HAL_spi_GetMaxNumOutputTransfers
**
** ======================================================================== */
uint32 HAL_spi_GetMaxNumOutputTransfers(uint32 nInstanceId)
{
   return HAL_SPI_MAX_MX_OUTPUT_COUNT;
} /* END HAL_spi_GetMaxNumInputTransfers */



/* ===========================================================================
**  HAL_spi_ConfigureErrors
**
** ======================================================================== */
void HAL_spi_ConfigureErrors(uint32 nInstanceId, uint32 nErrorsToEnable)
{
   uint32 nErrorToSet;

   nErrorToSet = 0;

   if(nErrorsToEnable & (uint32)HAL_spi_CLOCK_UNDERRUN_ERROR)
   {
      nErrorToSet |= SPI_HWIO_FMSK(nInstanceId, SPI_ERROR_FLAGS_EN, CLK_UNDER_RUN_ERR_EN);
   }

   if(nErrorsToEnable & (uint32)HAL_spi_CLOCK_OVERRUN_ERROR)
   {
      nErrorToSet |= SPI_HWIO_FMSK(nInstanceId, SPI_ERROR_FLAGS_EN, CLK_OVER_RUN_ERR_EN);
   }

   if(nErrorsToEnable & (uint32)HAL_spi_INPUT_OVERRUN_ERROR)
   {
      nErrorToSet |= SPI_HWIO_FMSK(nInstanceId, SPI_ERROR_FLAGS_EN, INPUT_OVER_RUN_ERR_EN);
   }

   if(nErrorsToEnable & (uint32)HAL_spi_OUTPUT_UNDERRUN_ERROR)
   {
      nErrorToSet |= SPI_HWIO_FMSK(nInstanceId, SPI_ERROR_FLAGS_EN, OUTPUT_UNDER_RUN_ERR_EN);
   }

   if(nErrorsToEnable & (uint32)HAL_spi_INPUT_UNDERRUN_ERROR)
   {
      nErrorToSet |= SPI_HWIO_FMSK(nInstanceId, SPI_ERROR_FLAGS_EN, INPUT_UNDER_RUN_ERR_EN);
   }

   if(nErrorsToEnable & (uint32)HAL_spi_OUTPUT_OVERRUN_ERROR)
   {
      nErrorToSet |= SPI_HWIO_FMSK(nInstanceId, SPI_ERROR_FLAGS_EN, OUTPUT_OVER_RUN_ERR_EN);
   }

   if(nErrorsToEnable & (uint32)HAL_spi_TIMEOUT_ERROR)
   {
      nErrorToSet |= SPI_HWIO_FMSK(nInstanceId, SPI_ERROR_FLAGS_EN, TIME_OUT_ERR_EN);
   }

   SPI_HWIO_OUT(nInstanceId, SPI_ERROR_FLAGS_EN, nErrorToSet);
} /* END HAL_spi_ConfigureErrors */



/* ===========================================================================
**  HAL_spi_GetErrors
**
** ======================================================================== */
uint32 HAL_spi_GetErrors(uint32 nInstanceId)
{
   uint32 nErrors;
   uint32 nTemp;

   nTemp = 0;

   nErrors = SPI_HWIO_IN(nInstanceId, SPI_ERROR_FLAGS);
   if(nErrors & SPI_HWIO_FMSK(nInstanceId, SPI_ERROR_FLAGS, CLK_UNDER_RUN_ERR))
   {
      nTemp |= HAL_spi_CLOCK_UNDERRUN_ERROR;
   }

   if(nErrors & SPI_HWIO_FMSK(nInstanceId, SPI_ERROR_FLAGS, CLK_OVER_RUN_ERR))
   {
      nTemp |= HAL_spi_CLOCK_OVERRUN_ERROR;
   }

   if(nErrors & SPI_HWIO_FMSK(nInstanceId, SPI_ERROR_FLAGS, INPUT_OVER_RUN_ERR))
   {
      nTemp |= HAL_spi_INPUT_OVERRUN_ERROR;
   }

   if(nErrors & SPI_HWIO_FMSK(nInstanceId, SPI_ERROR_FLAGS, OUTPUT_UNDER_RUN_ERR))
   {
      nTemp |= HAL_spi_OUTPUT_UNDERRUN_ERROR;
   }

   if(nErrors & SPI_HWIO_FMSK(nInstanceId, SPI_ERROR_FLAGS, INPUT_UNDER_RUN_ERR))
   {
      nTemp |= HAL_spi_INPUT_UNDERRUN_ERROR;
   }

   if(nErrors & SPI_HWIO_FMSK(nInstanceId, SPI_ERROR_FLAGS, OUTPUT_OVER_RUN_ERR))
   {
      nTemp |= HAL_spi_OUTPUT_OVERRUN_ERROR;
   }

   if(nErrors & SPI_HWIO_FMSK(nInstanceId, SPI_ERROR_FLAGS, TIME_OUT_ERR))
   {
      nTemp |= HAL_spi_TIMEOUT_ERROR; 
   }
   return nTemp;
} /* END HAL_spi_GetErrors */



/* ===========================================================================
**  HAL_spi_ClearErrors
**
** ======================================================================== */
void HAL_spi_ClearErrors(uint32 nInstanceId, uint32 nErrorsToClear)
{
   uint32 nTemp;

   nTemp = 0;

   if(nErrorsToClear & (uint32)HAL_spi_CLOCK_UNDERRUN_ERROR)
   {
      nTemp |= SPI_HWIO_FMSK(nInstanceId, SPI_ERROR_FLAGS_EN, CLK_UNDER_RUN_ERR_EN);
   }

   if(nErrorsToClear & (uint32)HAL_spi_CLOCK_OVERRUN_ERROR)
   {
      nTemp |= SPI_HWIO_FMSK(nInstanceId, SPI_ERROR_FLAGS_EN, CLK_OVER_RUN_ERR_EN);
   }

   if(nErrorsToClear & (uint32)HAL_spi_INPUT_OVERRUN_ERROR)
   {
      nTemp |= SPI_HWIO_FMSK(nInstanceId, SPI_ERROR_FLAGS_EN, INPUT_OVER_RUN_ERR_EN);
   }

   if(nErrorsToClear & (uint32)HAL_spi_OUTPUT_UNDERRUN_ERROR)
   {
      nTemp |= SPI_HWIO_FMSK(nInstanceId, SPI_ERROR_FLAGS_EN, OUTPUT_UNDER_RUN_ERR_EN);
   }

   if(nErrorsToClear & (uint32)HAL_spi_INPUT_UNDERRUN_ERROR)
   {
      nTemp |= SPI_HWIO_FMSK(nInstanceId, SPI_ERROR_FLAGS_EN, INPUT_UNDER_RUN_ERR_EN);
   }

   if(nErrorsToClear & (uint32)HAL_spi_OUTPUT_OVERRUN_ERROR)
   {
      nTemp |= SPI_HWIO_FMSK(nInstanceId, SPI_ERROR_FLAGS_EN, OUTPUT_OVER_RUN_ERR_EN);
   }

   if(nErrorsToClear & (uint32)HAL_spi_TIMEOUT_ERROR)
   {
      nTemp |= SPI_HWIO_FMSK(nInstanceId, SPI_ERROR_FLAGS_EN, TIME_OUT_ERR_EN);
   }

   SPI_HWIO_OUT(nInstanceId, SPI_ERROR_FLAGS_EN, nTemp);
} /* END HAL_spi_ClearErrors */



/* ===========================================================================
**  HAL_spi_SetTimeout
**
** ======================================================================== */
void HAL_spi_SetTimeout(uint32 nInstanceId, uint32 nTimeout)
{
   SPI_HWIO_OUT(nInstanceId, SPI_TIME_OUT, nTimeout);
} /* END HAL_spi_SetTimeout */



/* ===========================================================================
**  HAL_spi_GetCurrentTimeout
**
** ======================================================================== */
uint32 HAL_spi_GetCurrentTimeout(uint32 nInstanceId)
{
  return SPI_HWIO_IN(nInstanceId, SPI_TIME_OUT_CURRENT);
} /* END HAL_spi_GetCurrentTimeout */



/* ===========================================================================
**  HAL_spi_SetCoreMode
**
** ======================================================================== */
void HAL_spi_SetCoreMode(uint32 nInstanceId, HAL_spi_CoreModeType eCoreMode)
{
   switch (eCoreMode)
   {
      case HAL_spi_SLAVE:
         SPI_HWIO_OUTF(nInstanceId, SPI_CONFIG, SLAVE_OPERATION, 1);
         break;

      case HAL_spi_MASTER:
         SPI_HWIO_OUTF(nInstanceId, SPI_CONFIG, SLAVE_OPERATION, 0);
         break;

      default:
         break;
   }
} /* END HAL_spi_SetCoreMode */



/* ===========================================================================
**  HAL_spi_SetChipSelectTristate
**
** ======================================================================== */
void HAL_spi_SetChipSelectTristate(uint32 nInstanceId, boolean bTristateCS)
{
   if(bTristateCS)
   {
      SPI_HWIO_OUTF(nInstanceId, SPI_IO_CONTROL, TRISTATE_CS, 1);
   }
   else
   {
      SPI_HWIO_OUTF(nInstanceId, SPI_IO_CONTROL, TRISTATE_CS, 0);
   }
} /* END HAL_spi_SetChipSelectTristate */



/* ===========================================================================
**  HAL_spi_SetSPIDataOutTristate
**
** ======================================================================== */
void HAL_spi_SetSPIDataOutTristate(uint32 nInstanceId, boolean bTristateSPIDataOut)
{
   if(!bTristateSPIDataOut)
   {
      SPI_HWIO_OUTF(nInstanceId, SPI_IO_CONTROL, NO_TRI_STATE, 1);
   }
   else
   {
      SPI_HWIO_OUTF(nInstanceId, SPI_IO_CONTROL, NO_TRI_STATE, 0);
   }
} /* END HAL_spi_SetSPIDataOutTristate */



/* ===========================================================================
**  HAL_spi_SetDeviceParameters
**
** ======================================================================== */
void HAL_spi_SetDeviceParameters(uint32 nInstanceId, HAL_spi_DeviceParametersType *pmDeviceParameters)
{
   uint32 nTemp;
   uint32 nTempMask;

   /* Configure the clock polarity to use for this device */
   if(pmDeviceParameters->eClockPolarity == HAL_spi_CLK_IDLE_HIGH)
   {
      SPI_HWIO_OUTF(nInstanceId, SPI_IO_CONTROL, CLK_IDLE_HIGH, 1);
   }
   else
   {
      SPI_HWIO_OUTF(nInstanceId, SPI_IO_CONTROL, CLK_IDLE_HIGH, 0);
   }

   /* Configure the clock phase to use for this device */
   if(pmDeviceParameters->eTransferMode == HAL_spi_INPUT_FIRST_MODE)
   {
      SPI_HWIO_OUTF(nInstanceId, SPI_CONFIG, INPUT_FIRST, 1);
   }
   else
   {
      SPI_HWIO_OUTF(nInstanceId, SPI_CONFIG, INPUT_FIRST, 0);
   }

   /* Configure whether the clock should always be on while talking
    * to this device */
   if(pmDeviceParameters->bClockAlwaysOn)
   {
      SPI_HWIO_OUTF(nInstanceId, SPI_IO_CONTROL, CLK_ALWAYS_ON, 1);
   }
   else
   {
      SPI_HWIO_OUTF(nInstanceId, SPI_IO_CONTROL, CLK_ALWAYS_ON, 0);
   }

   /* Configure whether the chip select should stay asserted during
    * the entire transfer to this device
    */
   if(pmDeviceParameters->bChipSelectAlwaysOn)
   {
      SPI_HWIO_OUTF(nInstanceId, SPI_IO_CONTROL, MX_CS_MODE, 1);
   }
   else
   {
      SPI_HWIO_OUTF(nInstanceId, SPI_IO_CONTROL, MX_CS_MODE, 0);
   }

   /*
    * Configure the chip select polarity for this device.
    *
    * Active high chip select is configured by setting bit
    * corresponding to the chip select number in the CS_N_POLARITY
    * field of SPI_IO_CONTROL.
    *
    * We read the field first and mask out the other bits in order
    * to preserve the configurations for the other chip selects.
    */
   nTemp = SPI_HWIO_INF(nInstanceId, SPI_IO_CONTROL, CS_N_POLARITY);
   nTempMask = 1 << pmDeviceParameters->nChipSelectId;
   if(pmDeviceParameters->eCSPolarity == HAL_spi_CS_ACTIVE_HIGH)
   {
      nTemp |= nTempMask;
   }
   else
   {
      nTemp &= ~nTempMask;
   }
   SPI_HWIO_OUTF(nInstanceId, SPI_IO_CONTROL, CS_N_POLARITY, nTemp);

   /* Configure which chip select to use to talk to the device */
   SPI_HWIO_OUTF(nInstanceId, SPI_IO_CONTROL, CS_SELECT, pmDeviceParameters->nChipSelectId);

   /* Configure the deassertion time (inter-byte delay) between transfers
    * to this device */
   if(pmDeviceParameters->nDeassertionClockTicks > 0)
   {
      /* SPI_DEASSERT_WAIT waits the specified number of clock ticks plus one, so we need
       * to subtract one from the desired number of clock ticks when writing to the register */ 
      SPI_HWIO_OUT(nInstanceId, SPI_DEASSERT_WAIT, pmDeviceParameters->nDeassertionClockTicks-1);
   }
   else
   {
      /* If value of zero was specified, then we write 0, which means the
       * deassertion time will be one clock cycle */
      SPI_HWIO_OUT(nInstanceId, SPI_DEASSERT_WAIT, 0);
   }
} /* END HAL_spi_SetDeviceParameters */



/* ===========================================================================
**  HAL_spi_SetTransferParameters
**
** ======================================================================== */
void HAL_spi_SetTransferParameters(uint32 nInstanceId, HAL_spi_TransferParametersType *pmTransferParameters)
{
   /* Configure the number of bits per transfer unit.
    *
    * The SPI_CONFIG register uses the register value plus one as
    * the number of bits, so we need to subtract one before passing to the
    * register.
    */
   SPI_HWIO_OUTF(nInstanceId, SPI_CONFIG, N, (pmTransferParameters->nNumBits - 1));

   /* Configure whether internal loopback mode is used for the transfer. */
   if(pmTransferParameters->eLoopbackMode == HAL_spi_LOOPBACK_ENABLED)
   {
      SPI_HWIO_OUTF(nInstanceId, SPI_CONFIG, LOOP_BACK, 1);
   }
   else
   {
      SPI_HWIO_OUTF(nInstanceId, SPI_CONFIG, LOOP_BACK, 0);
   }

   /* Configure whether data received into the SPI input FIFO should be
    * packed into the FIFO word when it is shifted in from the SPI bus */
   if(pmTransferParameters->bReceivedDataPacked)
   {
      SPI_HWIO_OUTF(nInstanceId, SPI_IO_MODES, PACK_EN, 1);
   }
   else
   {
      SPI_HWIO_OUTF(nInstanceId, SPI_IO_MODES, PACK_EN, 0);
   }

   /* Configure whether output data should be unpacked from the SPI 
    * output FIFO as it is sent out on the bus.
    */
   if(pmTransferParameters->bTransmitDataPacked)
   {
      SPI_HWIO_OUTF(nInstanceId, SPI_IO_MODES, UNPACK_EN, 1);
   }
   else
   {
      SPI_HWIO_OUTF(nInstanceId, SPI_IO_MODES, UNPACK_EN, 0);
   }

   /*
    * Configure whether the output bit shift function should
    * be enabled or disabled.
    */
   if(pmTransferParameters->bOutputBitShift)
   {
      SPI_HWIO_OUTF(nInstanceId, SPI_IO_MODES, OUTPUT_BIT_SHIFT_EN, 1);
   }
   else
   {
      SPI_HWIO_OUTF(nInstanceId, SPI_IO_MODES, OUTPUT_BIT_SHIFT_EN, 0);
   }

   SPI_HWIO_OUT(nInstanceId, SPI_MX_READ_COUNT, pmTransferParameters->nNumInputTransfers);
   SPI_HWIO_OUT(nInstanceId, SPI_MX_INPUT_COUNT, pmTransferParameters->nNumInputTransfers);
   SPI_HWIO_OUT(nInstanceId, SPI_MX_OUTPUT_COUNT, pmTransferParameters->nNumOutputTransfers);
   SPI_HWIO_OUTF(nInstanceId, SPI_IO_MODES, INPUT_MODE, pmTransferParameters->eIOMode);
   SPI_HWIO_OUTF(nInstanceId, SPI_IO_MODES, OUTPUT_MODE, pmTransferParameters->eIOMode);

   switch (pmTransferParameters->eTransferDirection)
   {
      case HAL_spi_INPUT_ONLY:
         SPI_HWIO_OUTF(nInstanceId, SPI_CONFIG, NO_OUTPUT, 1);
         SPI_HWIO_OUTF(nInstanceId, SPI_CONFIG, NO_INPUT, 0);
         break;

      case HAL_spi_OUTPUT_ONLY:
         SPI_HWIO_OUTF(nInstanceId, SPI_CONFIG, NO_OUTPUT, 0);
         SPI_HWIO_OUTF(nInstanceId, SPI_CONFIG, NO_INPUT, 1);
         break;

      case HAL_spi_BI_DIRECTIONAL:
         SPI_HWIO_OUTF(nInstanceId, SPI_CONFIG, NO_OUTPUT, 0);
         SPI_HWIO_OUTF(nInstanceId, SPI_CONFIG, NO_INPUT, 0);
         break;

      default:
         break;
   }
} /* END HAL_spi_SetTransferParameters */



/* ===========================================================================
**  HAL_spi_StartTransfer
**
** ======================================================================== */
void HAL_spi_StartTransfer(uint32 nInstanceId)
{
   SPI_HWIO_OUTF(nInstanceId, SPI_OPERATIONAL, STATE, HAL_spi_RUN_STATE);
} /* END HAL_spi_StartTransfer */ 



/* ===========================================================================
**  HAL_spi_PauseTransfer
**
** ======================================================================== */
void HAL_spi_PauseTransfer(uint32 nInstanceId)
{
   SPI_HWIO_OUTF(nInstanceId, SPI_OPERATIONAL, STATE, HAL_spi_PAUSE_STATE);
} /* END HAL_spi_PauseTransfer */ 



/* ===========================================================================
**  HAL_spi_ResetTransfer
**
** ======================================================================== */
void HAL_spi_ResetTransfer(uint32 nInstanceId)
{
   SPI_HWIO_OUTF(nInstanceId, SPI_OPERATIONAL, STATE, HAL_spi_RESET_STATE);
} /* END HAL_spi_ResetTransfer */ 



/* ===========================================================================
**  HAL_spi_ReadData
**
** ======================================================================== */
void HAL_spi_ReadData(uint32 nInstanceId, uint32* pReadData, uint32 nWordsToRead)
{
   for (; nWordsToRead; nWordsToRead--)
   {
      *pReadData++ = SPI_HWIO_IN(nInstanceId, SPI_INPUT_FIFO);
   }
} /* END HAL_spi_ReadData */



/* ===========================================================================
**  HAL_spi_WriteData
**
** ======================================================================== */
void HAL_spi_WriteData(uint32 nInstanceId, uint32* pWriteData,uint32 nWordsToWrite)
{
   for (; nWordsToWrite; nWordsToWrite--)
   {
      SPI_HWIO_OUT(nInstanceId, SPI_OUTPUT_FIFO, *pWriteData++);
   }
} /* END HAL_spi_WriteData */



/* ===========================================================================
**  HAL_spi_AcknowledgeMaxInputDoneFlag
**
** ======================================================================== */
void HAL_spi_AcknowledgeMaxInputDoneFlag(uint32 nInstanceId)
{
   uint32 nSpiOperationalRegValue;

   nSpiOperationalRegValue = SPI_HWIO_IN(nInstanceId, SPI_OPERATIONAL);
   nSpiOperationalRegValue |= SPI_HWIO_FMSK(nInstanceId, SPI_OPERATIONAL, MAX_INPUT_DONE_FLAG); 

   /* Need to leave the other flags as they are.. so explicitly 
    * set the other bits to 0, as writing 1 to those positions
    * would clear the flags */
   nSpiOperationalRegValue &= ~SPI_HWIO_FMSK(nInstanceId, SPI_OPERATIONAL, OUTPUT_SERVICE_FLAG);
   nSpiOperationalRegValue &= ~SPI_HWIO_FMSK(nInstanceId, SPI_OPERATIONAL, INPUT_SERVICE_FLAG);
   nSpiOperationalRegValue &= ~SPI_HWIO_FMSK(nInstanceId, SPI_OPERATIONAL, MAX_OUTPUT_DONE_FLAG);

   SPI_HWIO_OUT(nInstanceId, SPI_OPERATIONAL, nSpiOperationalRegValue);
} /* END HAL_spi_AcknowledgeMaxInputDoneFlag */



/* ===========================================================================
**  HAL_spi_AcknowledgeMaxOutputDoneFlag
**
** ======================================================================== */
void HAL_spi_AcknowledgeMaxOutputDoneFlag(uint32 nInstanceId)
{
   uint32 nSpiOperationalRegValue;

   nSpiOperationalRegValue = SPI_HWIO_IN(nInstanceId, SPI_OPERATIONAL);
   nSpiOperationalRegValue |= SPI_HWIO_FMSK(nInstanceId, SPI_OPERATIONAL, MAX_OUTPUT_DONE_FLAG); 

   /* Need to leave the other flags as they are.. so explicitly 
    * set the other bits to 0, as writing 1 to those positions
    * would clear the flags */
   nSpiOperationalRegValue &= ~SPI_HWIO_FMSK(nInstanceId, SPI_OPERATIONAL, OUTPUT_SERVICE_FLAG);
   nSpiOperationalRegValue &= ~SPI_HWIO_FMSK(nInstanceId, SPI_OPERATIONAL, MAX_INPUT_DONE_FLAG);
   nSpiOperationalRegValue &= ~SPI_HWIO_FMSK(nInstanceId, SPI_OPERATIONAL, INPUT_SERVICE_FLAG);

   SPI_HWIO_OUT(nInstanceId, SPI_OPERATIONAL, nSpiOperationalRegValue);
} /* END HAL_spi_AcknowledgeMaxOutputDoneFlag */



/* ===========================================================================
**  HAL_spi_AcknowledgeInputServiceFlag
**
** ======================================================================== */
void HAL_spi_AcknowledgeInputServiceFlag (uint32 nInstanceId)
{
   uint32 nSpiOperationalRegValue;

   nSpiOperationalRegValue = SPI_HWIO_IN(nInstanceId, SPI_OPERATIONAL);
   nSpiOperationalRegValue |= SPI_HWIO_FMSK(nInstanceId, SPI_OPERATIONAL, INPUT_SERVICE_FLAG); 

   /* Need to leave the other flags as they are.. so explicitly 
    * set the other bits to 0, as writing 1 to those positions
    * would clear the flags */
   nSpiOperationalRegValue &= ~SPI_HWIO_FMSK(nInstanceId, SPI_OPERATIONAL, OUTPUT_SERVICE_FLAG);
   nSpiOperationalRegValue &= ~SPI_HWIO_FMSK(nInstanceId, SPI_OPERATIONAL, MAX_INPUT_DONE_FLAG);
   nSpiOperationalRegValue &= ~SPI_HWIO_FMSK(nInstanceId, SPI_OPERATIONAL, MAX_OUTPUT_DONE_FLAG);

   SPI_HWIO_OUT(nInstanceId, SPI_OPERATIONAL, nSpiOperationalRegValue);
} /* END HAL_spi_AcknowledgeInputServiceFlag */



/* ===========================================================================
**  HAL_spi_AcknowledgeOutputServiceFlag
**
** ======================================================================== */
void HAL_spi_AcknowledgeOutputServiceFlag(uint32 nInstanceId)
{
   uint32 nSpiOperationalRegValue;

   nSpiOperationalRegValue = SPI_HWIO_IN(nInstanceId, SPI_OPERATIONAL);
   nSpiOperationalRegValue |= SPI_HWIO_FMSK(nInstanceId, SPI_OPERATIONAL, OUTPUT_SERVICE_FLAG); 

   /* Need to leave the other flags as they are.. so explicitly 
    * set the other bits to 0, as writing 1 to those positions
    * would clear the flags */
   nSpiOperationalRegValue &= ~SPI_HWIO_FMSK(nInstanceId, SPI_OPERATIONAL, INPUT_SERVICE_FLAG);
   nSpiOperationalRegValue &= ~SPI_HWIO_FMSK(nInstanceId, SPI_OPERATIONAL, MAX_INPUT_DONE_FLAG);
   nSpiOperationalRegValue &= ~SPI_HWIO_FMSK(nInstanceId, SPI_OPERATIONAL, MAX_OUTPUT_DONE_FLAG);

   SPI_HWIO_OUT(nInstanceId, SPI_OPERATIONAL, nSpiOperationalRegValue);
} /* END HAL_spi_AcknowledgeOutputServiceFlag */



/* ===========================================================================
**  HAL_spi_GetRunState
**
** ======================================================================== */
HAL_spi_RunStateType HAL_spi_GetRunState(uint32 nInstanceId)
{
   uint32 nStateValid;
   uint32 nState;
   HAL_spi_RunStateType eRunState;

   nStateValid = SPI_HWIO_INF(nInstanceId, SPI_OPERATIONAL, STATE_VALID);
   if(0 == nStateValid)
   {
      eRunState = HAL_spi_INVALID_STATE;
   }
   else
   {
      nState = SPI_HWIO_INF(nInstanceId, SPI_OPERATIONAL, STATE);
   
      switch(nState)
      {
         case 0:
            eRunState = HAL_spi_RESET_STATE;
            break;
   
         case 1:
            eRunState = HAL_spi_RUN_STATE;
            break;
   
         case 3:
            eRunState = HAL_spi_PAUSE_STATE;
            break;
   
         default:
            eRunState = HAL_spi_INVALID_STATE;
      }
   }

   return eRunState;
} /* END HAL_spi_GetRunState */ 



/* ===========================================================================
**  HAL_spi_GetCurrentMaxInputCount
**
** ======================================================================== */
uint32 HAL_spi_GetCurrentMaxInputCount(uint32 nInstanceId)
{
   return SPI_HWIO_IN(nInstanceId, SPI_MX_INPUT_CNT_CURRENT);
} /* END HAL_spi_GetCurrentMaxInputCount */



/* ===========================================================================
**  HAL_spi_GetCurrentMaxOutputCount
**
** ======================================================================== */
uint32 HAL_spi_GetCurrentMaxOutputCount(uint32 nInstanceId)
{
   return SPI_HWIO_IN(nInstanceId, SPI_MX_OUTPUT_CNT_CURRENT);
} /* END HAL_spi_GetCurrentMaxOutputCount */



/* ===========================================================================
**  HAL_spi_GetCurrentMaxReadCount
**
** ======================================================================== */
uint32 HAL_spi_GetCurrentMaxReadCount(uint32 nInstanceId)
{
   return SPI_HWIO_IN(nInstanceId, SPI_MX_READ_CNT_CURRENT);
} /* END HAL_spi_GetCurrentMaxReadCount */



/* ===========================================================================
**  HAL_spi_IsInputServiceFlagSet
**
** ======================================================================== */
boolean HAL_spi_IsInputServiceFlagSet(uint32 nInstanceId)
{
   uint32 nField;

   nField = SPI_HWIO_INF(nInstanceId, SPI_OPERATIONAL, INPUT_SERVICE_FLAG);

   if(nField == 1)
   {
      return TRUE;
   }
   else
   {
      return FALSE;
   }
} /* END HAL_spi_IsInputServiceFlagSet */



/* ===========================================================================
**  HAL_spi_IsOutputServiceFlagSet
**
** ======================================================================== */
boolean HAL_spi_IsOutputServiceFlagSet(uint32 nInstanceId)
{
   uint32 nField;

   nField = SPI_HWIO_INF(nInstanceId, SPI_OPERATIONAL, OUTPUT_SERVICE_FLAG);

   if(nField == 1)
   {
      return TRUE;
   }
   else
   {
      return FALSE;
   }
} /* END HAL_spi_IsOutputServiceFlagSet */



/* ===========================================================================
**  HAL_spi_IsMaxInputDoneFlagSet
**
** ======================================================================== */
boolean HAL_spi_IsMaxInputDoneFlagSet(uint32 nInstanceId)
{
   uint32 nField;

   nField = SPI_HWIO_INF(nInstanceId, SPI_OPERATIONAL, MAX_INPUT_DONE_FLAG);

   if(nField == 1)
   {
      return TRUE;
   }
   else
   {
      return FALSE;
   }
} /* END HAL_spi_IsMaxInputDoneFlagSet */



/* ===========================================================================
**  HAL_spi_IsMaxOutputDoneFlagSet
**
** ======================================================================== */
boolean HAL_spi_IsMaxOutputDoneFlagSet(uint32 nInstanceId)
{
   uint32 nField;

   nField = SPI_HWIO_INF(nInstanceId, SPI_OPERATIONAL, MAX_OUTPUT_DONE_FLAG);

   if(nField == 1)
   {
      return TRUE;
   }
   else
   {
      return FALSE;
   }
} /* END HAL_spi_IsMaxOutputDoneFlagSet */



/* ===========================================================================
**  HAL_spi_IsIsInputFifoFull
**
** ======================================================================== */
boolean HAL_spi_IsInputFifoFull(uint32 nInstanceId)
{
   uint32 nField;

   nField = SPI_HWIO_INF(nInstanceId, SPI_OPERATIONAL, INPUT_FIFO_FULL);

   if(nField == 1)
   {
      return TRUE;
   }
   else
   {
      return FALSE;
   }
} /* END HAL_spi_IsIsInputFifoFull */



/* ===========================================================================
**  HAL_spi_IsOutputFifoFull
**
** ======================================================================== */
boolean HAL_spi_IsOutputFifoFull(uint32 nInstanceId)
{
   uint32 nField;

   nField = SPI_HWIO_INF(nInstanceId, SPI_OPERATIONAL, OUTPUT_FIFO_FULL);

   if(nField == 1)
   {
      return TRUE;
   }
   else
   {
      return FALSE;
   }
} /* END HAL_spi_IsOutputFifoFull */



/* ===========================================================================
**  HAL_spi_IsInputFifoNotEmpty
**
** ======================================================================== */
boolean HAL_spi_IsInputFifoNotEmpty(uint32 nInstanceId)
{
   uint32 nField;

   nField = SPI_HWIO_INF(nInstanceId, SPI_OPERATIONAL, INPUT_FIFO_NOT_EMPTY);

   if(nField == 1)
   {
      return TRUE;
   }
   else
   {
      return FALSE;
   }
} /* END HAL_spi_IsInputFifoNotEmpty */



/* ===========================================================================
**  HAL_spi_IsOutputFifoNotEmpty
**
** ======================================================================== */
boolean HAL_spi_IsOutputFifoNotEmpty(uint32 nInstanceId)
{
   uint32 nField;

   nField = SPI_HWIO_INF(nInstanceId, SPI_OPERATIONAL, OUTPUT_FIFO_NOT_EMPTY);

   if(nField == 1)
   {
      return TRUE;
   }
   else
   {
      return FALSE;
   }
} /* END HAL_spi_IsOutputFifoNotEmpty */



/* ===========================================================================
**  HAL_spi_GetInputFifoCount
**
** ======================================================================== */
uint32 HAL_spi_GetInputFifoCount(uint32 nInstanceId)
{
   return SPI_HWIO_INF(nInstanceId, SPI_FIFO_WORD_CNT, INPUT_FIFO_WORD_CNT);
} /* END HAL_spi_GetInputFifoCount */



/* ===========================================================================
**  HAL_spi_GetOutputFifoCount
**
** ======================================================================== */
uint32 HAL_spi_GetOutputFifoCount(uint32 nInstanceId)
{
   return SPI_HWIO_INF(nInstanceId, SPI_FIFO_WORD_CNT, OUTPUT_FIFO_WORD_CNT);
} /* END HAL_spi_GetOutputFifoCount */

