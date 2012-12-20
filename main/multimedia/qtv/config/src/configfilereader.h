#ifndef _QTV_CONFIG_FILE_READER_H_
#define _QTV_CONFIG_FILE_READER_H_
/* =======================================================================
                               ConfigFileReader.h
DESCRIPTION
  Class declaration for configuration file reader.

  Copyright 2003 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */

/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/config/main/latest/src/configfilereader.h#7 $
$DateTime: 2009/04/29 04:39:32 $
$Change: 899616 $

========================================================================== */

/* =======================================================================
**               Includes and Public Data Declarations
** ======================================================================= */
/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "oscl_file_io.h"
#include "deeplist.h"
#include "QtvConfig.h"

/* ==========================================================================

                        DATA DECLARATIONS

========================================================================== */

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */
  
/* -----------------------------------------------------------------------
** Global Constant Data Declarations
** ----------------------------------------------------------------------- */
/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */
  
/* =======================================================================
**                          Macro Definitions
** ======================================================================= */

/* =======================================================================
**                        Function Declarations
** ======================================================================= */

class QtvConfigFileReader 
{
public:
   // Constructor
   QtvConfigFileReader();

   virtual ~QtvConfigFileReader();

   // Open the file with the argument filename for preparation to read. 
   // If the file cannot be opened, return false. Otherwise return true.
   bool open(const char *filename);

   // Read the configuration file and parse it, setting configuration items in
   // config. Then close the file. Return the number of items successfully 
   // parsed.
   int readItems(QtvConfig *config);

   // An array of upper-cased version of the string representing each setting name,
   // indexed by StreamingConfigItem.
   static const char *upperCaseNameForConfigItem[QtvConfig::QTVCONFIG_ITEM_COUNT];

private:

   // Read the config file and allocate a string that contains its entire contents.
   // This string must be freed by the caller! If pLength is not NULL, set its 
   // dereferenced value to the length of the string.
   char *getText(int *pLength);

   // Given the config file contents in string format, parse the string and set
   // configuration items in config. Return the number of items parsed.   
   int parseItemsFromConfigString(const char *configFileString, int length,
      QtvConfig *config);

   // Return the config item id for the argument name. The name must be in all 
   // upper case letters. If no id is found, return QTVCONFIG_ITEM_COUNT.
   QtvConfig::QtvConfigItemId getItemIDForName(const char *name);

   // Assuming setting_value_str represents a value for the argument item ID, 
   // convert the string and set *pValue to the value. Return true iff 
   // conversion was successful.
   bool convertValue(QtvConfig::QtvConfigItemId item_id, const char *setting_value_str, 
                     int value_str_length, void *pValue);

   // Assuming setting_value_str represents an integer value for the argument item ID, 
   // convert the string and set *pValue to the value. Return true iff 
   // conversion was successful.
   bool convertInt32Value(const char *setting_value_str,void *pValue);

   bool convertProbeURL(const char *setting_value_str, 
                        int value_str_length,
                        ProbeURLType* pValue);


#ifdef FEATURE_QTV_ENCRYPTED_STREAMS
   bool convertSessionID(const char *setting_value_str, int value_str_length,
                         SessionIDType* pValue);
#endif

   bool stripBrackets(const char *& str, const char *&end_str);



   OSCL_FILE *fp;
};

#endif /* _QTV_CONFIG_FILE_READER_H_ */
