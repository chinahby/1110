/*****************************************************************************

  C A M E R A   T U N E    V A R I A B L E S

  (runtime tuning, that is)

DESCRIPTION:
  The QMobiCaT engine is designed to not need to know anything about
  any specific tuning parameters.  Unfortunately there are exceptions 
  to this.  Any references to specific parameters or other camera 
  services implementation details go in this plaform-specific file.

Copyright (c) Qualcomm 2006, all rights reserved.

$Header: //source/qcom/qct/multimedia/camera/6k/qmobicat/main/latest/src/camera_tune_msm.c#1 $
$DateTime: 2008/05/09 16:40:03 $
$Author: karenp $

REVISION HISTORY:

  when        who       what, where, why
  ----------  --------  ------------------------------------------------------
  2007-06-14  jgarber   Fixed compiler errors if QMobiCaT features not defined.
  12/19/06   klee     Added fix on CR 108540 - compiling error under FEATURE_CAMERA_YCBCR_ONLY
  2006-10-06  jgarber   Initial rewrite of QMobiCaT engine to use
                        camera services tunable parameters API.

*****************************************************************************/

#ifdef  FEATURE_CAMERA_MOBICAT_CAMERA_CONFIG_H

//////////////////////////////////////////////////////////////////////
//  The 3A live summary screen parameters are defined here.
////////////////////////////////////////////////////////////////
static const camera_tune_registry_type summary_parms[] = 
{
#ifndef FEATURE_CAMERA_YCBCR_ONLY
  {CAMERA_API_AEC_CURRENT_SENSOR_LUMA,     "Luma", 1, 0},
#endif /* nFEATURE_CAMERA_YCBCR_ONLY */
  {CAMERA_API_AF_POSITION,                 "AF",   1, 0}
#ifndef FEATURE_CAMERA_YCBCR_ONLY
  ,{CAMERA_API_AEC_CURRENT_EXP_INDEX,       "AE",   1, 0}
  ,{CAMERA_API_AWB_CURRENT_SAMPLE_DECISION, "AWB",  1, 0}
#endif /* nFEATURE_CAMERA_YCBCR_ONLY */
};

#endif //FEATURE_CAMERA_MOBICAT_CAMERA_CONFIG_H

#ifdef  FEATURE_CAMERA_MOBICAT

//////////////////////////////////////////////////////////////////////
//  Sensor file name suffix
////////////////////////////////////////////////////////////////
extern int8                          camera_asi                ;
extern camsensor_static_params_type  camsensor_static_params [];

typedef struct
{
   const char * description;
   const char * file_name;
}  Sensor_Name_Map;


Sensor_Name_Map  sensor_map_function[] =
{
   { "SONY imx006fq 1.3 MP"                      , "_imx006fq"           }
  ,{ "SONY imx006fq 1.3 MP"                      , "_imx006fq_sn1m3"     }
  ,{ "Sony IU011F 2.0 MP Piezo AF"               , "_iu011f_sn2m0pz"     }
  ,{ "Sony IU011FS 2.0 MP"                       , "_iu011fs"            }
  ,{ "Sony IU011FS 2.0 MP"                       , "_iu011fs_sn2m0sm"    }
  ,{ "Sony IU018F 3.0 MP"                        , "_iu018f_sn3m0pz"     }
  ,{ "Sharp LZOP3731 2.0 MP"                     , "_lzop3731"           }
  ,{ "Sharp LZOP3733 YCbCr 2.0 MP"               , "_lzop3733"           }
  ,{ "Sharp LZOP3733 YCbCr 2.0 MP"               , "_lzop3733_sh2m0vcyu" }
  ,{ "Panasonic Mn101E19a 1.3 MP"                , "_mn101e19a"          }
  ,{ "Panasonic Mn101E19a 1.3 MP"                , "_mn101e19a_mc1m3yu"  }
  ,{ "Micron MT9D011 2.0 MP"                     , "_mt9d011"            }
  ,{ "Micron MT9D011 2.0 MP"                     , "_mt9d011_mu2m0"      }
  ,{ "Micron MT9D011 2.0 MP VCM AF"              , "_mt9d011_mu2m0vc"    }
  ,{ "Micron MT9D111 2.0 MP"                     , "_mt9d111_mu2m0"      }
  ,{ "Micron MT9M011 1.3 MP"                     , "_mt9m011"            }
  ,{ "Micron MT9V111 VGA CMOS"                   , "_mt9v111"            }
  ,{ "Micron MT9V111 VGA CMOS"                   , "_mt9v111_ycbcr"      }
  ,{ "National Instruments GPIO Sensor Emulator" , "_ni_gpio"            }
  ,{ "Casio NX5300 2.0 MP"                       , "_nx5300"             }
  ,{ "Casio NX5300 2.0 MP"                       , "_nx5300_cs2m0sm_ccd" }
  ,{ "Omnivision 2620 2.0 MP"                    , "_ov2620"             }
  ,{ "Omnivision 2620 2.0 MP"                    , "_ov2620_ov2m0"       }
  ,{ "Omnivision 2630 2.0 MP Rev4"               , "_ov2630"             }
  ,{ "Omnivision 2630 2.0 MP Rev2"               , "_ov2630"             }
  ,{ "Omnivision 2630 2.0 MP Rev4"               , "_ov2630_ov2m0"       }
  ,{ "Omnivision 2630 2.0 MP Rev2"               , "_ov2630_ov2m0"       }
  ,{ "Omnivision OV9650 1.3 MP"                  , "_ov9650"             }
  ,{ "Samsung S5K3A1EB 1.3 MP"                   , "_s5k3a1eb"           }
  ,{ NULL , NULL }
};


//////////////////////////////////////////////////////////////////////////////


const char *  camera_tune_sensor_file_name( const char * description )
{
  int i;
  for (i = 0 ; sensor_map_function[i].description != NULL ; ++i)
  {
    if ( strstr(description , sensor_map_function[i].description) != NULL )
    {
      return sensor_map_function[i].file_name ;
    };
  };
  return "_unknown_sensor" ;  //  nothing found.
}


//////////////////////////////////////////////////////////////////////////////


const char *  camera_tune_current_sensor_file_name( void )
{
  return camera_tune_sensor_file_name
         (
           camsensor_static_params[ camera_asi ].sensor_name
         );
}

#endif //FEATURE_CAMERA_MOBICAT

#ifdef  FEATURE_CAMERA_MOBICAT_CAMERA_CONFIG_H

//
//  If Diag tries to read or write a .ini file 
//  and camera_svcs is not running, then the dsp
//  image loaded may be unable to service some
//  read function requests for variables to be
//  read or written from or to a .ini file.
//


static boolean  camera_svcs_is_running( APP_STATE camera_tune_application_state )
{
  camera_parm_info_type  camera_svcs_info;

  (void) camera_get_parm( CAMERA_PARM_STATE , &camera_svcs_info );

  MSG_HIGH( "JOHN : camera_svcs_Info.current_value = %d" , 
            camera_svcs_info.current_value , 0 , 0 
          );

  return  (
            ( camera_tune_application_state != APP_STATE_INIT )
            &&
            ( (camera_state_type)( camera_svcs_info.current_value )
              != CAMERA_STATE_INIT )
          );
}


#endif //FEATURE_CAMERA_MOBICAT_CONFIG_H

