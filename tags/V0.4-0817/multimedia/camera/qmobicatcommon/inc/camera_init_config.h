/*****************************************************************************

Copyright (c) Qualcomm October 2006, all rights reserved.

$Header: //source/qcom/qct/multimedia/camera/qmobicatcommon/main/latest/inc/camera_init_config.h#1 $
$DateTime: 2008/05/09 16:40:03 $ 
$Author: karenp $

REVISION HISTORY:
  
  when        who       what, where, why
  ----------  --------  ------------------------------------------------------
  2007-01-24  jgarber   Added header with copyright.

DESCRIPTION:

  This file is a stub containing the definition of a structure
  for specifying a default initial value for a parameter, and an 
  empty array of initializers. The Chromatix tool generates a copy
  of this file which can be placed in the build with initializers 
  as tuned from that tool.

*****************************************************************************/
typedef struct
{
  char* name;
  int32 index;  //if in a table
  char* value;
} camera_config_init_values_type;

camera_config_init_values_type camera_config_init_values[] = 
{
  {NULL, 0, NULL}
};


