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

#include <ptp_types.h>
#include <ptp_utils.h>
#include <jos.h>
#include "dev_props.h"

/*
 *   PTP device properties
 */

static ptp_prop_value_t u8_percent_allowed_values[] = 
{
    { PTP_TYPE_UINT8, {0}, 0, 0, 0 },   /* Min */
    { PTP_TYPE_UINT8, {0}, 0, 0, 0 },   /* Max */
    { PTP_TYPE_UINT8, {0}, 0, 0, 0 }    /* Step */
};

static ptp_prop_value_t compression_allowed_values[] = 
{
    { PTP_TYPE_UINT8, {0}, 0, 0, 0 },   /* Min */
    { PTP_TYPE_UINT8, {0}, 0, 0, 0 },   /* Max */
    { PTP_TYPE_UINT8, {0}, 0, 0, 0 }    /* Step */
};

static ptp_prop_value_t u16_allowed_values[] = 
{
    { PTP_TYPE_UINT16, {0}, 0, 0, 0 },
    { PTP_TYPE_UINT16, {0}, 0, 0, 0 },
    { PTP_TYPE_UINT16, {0}, 0, 0, 0 },
    { PTP_TYPE_UINT16, {0}, 0, 0, 0 },
    { PTP_TYPE_UINT16, {0}, 0, 0, 0 },
    { PTP_TYPE_UINT16, {0}, 0, 0, 0 },
    { PTP_TYPE_UINT16, {0}, 0, 0, 0 },
    { PTP_TYPE_UINT16, {0}, 0, 0, 0 }
};

static ptp_prop_value_t aperture_allowed_values[] = 
{
    { PTP_TYPE_UINT16, {0}, 0, 0, 0 },  /* Min */
    { PTP_TYPE_UINT16, {0}, 0, 0, 0 },  /* Max */
    { PTP_TYPE_UINT16, {0}, 0, 0, 0 }   /* Step */
};

static ptp_prop_value_t focal_len_allowed_values[] = 
{
    { PTP_TYPE_UINT32, {0}, 0, 0, 0 },  /* Min */
    { PTP_TYPE_UINT32, {0}, 0, 0, 0 },  /* Max */
    { PTP_TYPE_UINT32, {0}, 0, 0, 0 }   /* Step */
};

static ptp_prop_value_t focus_dist_allowed_values[] = 
{
    { PTP_TYPE_UINT16, {0}, 0, 0, 0 },  /* Min */
    { PTP_TYPE_UINT16, {0}, 0, 0, 0 },  /* Max */
    { PTP_TYPE_UINT16, {0}, 0, 0, 0 }   /* Step */
};

static ptp_prop_value_t shutter_allowed_values[] = 
{
    { PTP_TYPE_UINT32, {0}, 0, 0, 0 },  /* Min */
    { PTP_TYPE_UINT32, {0}, 0, 0, 0 },  /* Max */
    { PTP_TYPE_UINT32, {0}, 0, 0, 0 }   /* Step */
};

static ptp_prop_value_t iso_allowed_values [] = 
{
    { PTP_TYPE_UINT16, {0}, 0, 0, 0 },
    { PTP_TYPE_UINT16, {0}, 0, 0, 0 },
    { PTP_TYPE_UINT16, {0}, 0, 0, 0 },
    { PTP_TYPE_UINT16, {0}, 0, 0, 0 },
    { PTP_TYPE_UINT16, {0}, 0, 0, 0 },
    { PTP_TYPE_UINT16, {0}, 0, 0, 0 },
    { PTP_TYPE_UINT16, {0}, 0, 0, 0 },
    { PTP_TYPE_UINT16, {0}, 0, 0, 0 },
    { PTP_TYPE_UINT16, {0}, 0, 0, 0 }
};

static ptp_prop_value_t auto_exp_allowed_values[] = 
{
    { PTP_TYPE_INT16, {0}, 0, 0, 0 },   /* Min */
    { PTP_TYPE_INT16, {0}, 0, 0, 0 },   /* Max */
    { PTP_TYPE_INT16, {0}, 0, 0, 0 }    /* Step */
};

static ptp_prop_value_t delay_allowed_values[] = 
{
    { PTP_TYPE_UINT32, {0}, 0, 0, 0 },  /* Min */
    { PTP_TYPE_UINT32, {0}, 0, 0, 0 },  /* Max */
    { PTP_TYPE_UINT32, {0}, 0, 0, 0 }   /* Step */
};

static ptp_prop_value_t zoom_allowed_values[] = 
{
    { PTP_TYPE_UINT8, {0}, 0, 0, 0 },   /* Min */
    { PTP_TYPE_UINT8, {0}, 0, 0, 0 },   /* Max */
    { PTP_TYPE_UINT8, {0}, 0, 0, 0 }    /* Step */
};

static ptp_prop_value_t u16_range_allowed_values[] = 
{
    { PTP_TYPE_UINT16, {0}, 0, 0, 0 },  /* Min */
    { PTP_TYPE_UINT16, {0}, 0, 0, 0 },  /* Max */
    { PTP_TYPE_UINT16, {0}, 0, 0, 0 }   /* Step */
};

static ptp_prop_value_t burst_interval_allowed_values[] = 
{
    { PTP_TYPE_UINT16, {0}, 0, 0, 0 },  /* Min */
    { PTP_TYPE_UINT16, {0}, 0, 0, 0 },  /* Max */
    { PTP_TYPE_UINT16, {0}, 0, 0, 0 }   /* Step */
};

/* Map of all default and current property values */
devprop_map_elem_t devprop_value_map[NUM_DEV_PROPS];

/* Map of all property descriptors (points to values in devprop_value_map) */
ptp_dev_prop_desc_t devprop_desc_map[NUM_DEV_PROPS] =
{
    { /* 1 */
        PTP_DEV_PROP_BATTERY_LEVEL,
        PTP_TYPE_UINT8,
        0,
        NULL,
        NULL,
        PTP_RANGE_FORM,
        3,
        u8_percent_allowed_values
    },
    { /* 2 */
        PTP_DEV_PROP_FUNCTIONAL_MODE,
        PTP_TYPE_UINT16, 
        1, 
        NULL, 
        NULL, 
        PTP_ENUM_FORM, 
        2, 
        u16_allowed_values
    },
    { /* 3 */
        PTP_DEV_PROP_IMAGE_SIZE, 
        PTP_TYPE_STR, 
        1, 
        NULL, 
        NULL, 
        PTP_ENUM_FORM, 
        0, 
        NULL 
    },
    { /* 4 */
        PTP_DEV_PROP_COMPRESSION_SETTING, 
        PTP_TYPE_UINT8, 
        1, 
        NULL, 
        NULL, 
        PTP_RANGE_FORM, 
        3, 
        compression_allowed_values
    },
    { /* 5 */
        PTP_DEV_PROP_WHITE_BALANCE, 
        PTP_TYPE_UINT16, 
        1, 
        NULL, 
        NULL, 
        PTP_ENUM_FORM, 
        8, 
        u16_allowed_values 
    },
    { /* 6 */
        PTP_DEV_PROP_RGB_GAIN, 
        PTP_TYPE_STR, 
        1, 
        NULL, 
        NULL, 
        PTP_ENUM_FORM, 
        0, 
        NULL 
    },
    { /* 7 */
        PTP_DEV_PROP_F_NUMBER, 
        PTP_TYPE_UINT16, 
        1, 
        NULL, 
        NULL, 
        PTP_RANGE_FORM, 
        3, 
        aperture_allowed_values 
    },
    { /* 8 */
        PTP_DEV_PROP_FOCAL_LENGTH, 
        PTP_TYPE_UINT32, 
        1, 
        NULL, 
        NULL, 
        PTP_RANGE_FORM, 
        3, 
        focal_len_allowed_values 
    },
    { /* 9 */
        PTP_DEV_PROP_FOCUS_DISTANCE, 
        PTP_TYPE_UINT16, 
        1, 
        NULL, 
        NULL, 
        PTP_RANGE_FORM, 
        3, 
        focus_dist_allowed_values 
    },
    { /* 10 */
        PTP_DEV_PROP_FOCUS_MODE, 
        PTP_TYPE_UINT16, 
        1, 
        NULL, 
        NULL, 
        PTP_ENUM_FORM, 
        4, 
        u16_allowed_values 
    },
    { /* 11 */
        PTP_DEV_PROP_EXPOSURE_METERING_MODE, 
        PTP_TYPE_UINT16, 
        1, 
        NULL, 
        NULL, 
        PTP_ENUM_FORM, 
        5, 
        u16_allowed_values 
    },    
    { /* 12 */
        PTP_DEV_PROP_FLASH_MODE, 
        PTP_TYPE_UINT16, 
        1, 
        NULL, 
        NULL, 
        PTP_ENUM_FORM, 
        7, 
        u16_allowed_values 
    },
    { /* 13 */
        PTP_DEV_PROP_EXPOSURE_TIME, 
        PTP_TYPE_UINT32, 
        1, 
        NULL, 
        NULL, 
        PTP_RANGE_FORM, 
        3, 
        shutter_allowed_values 
    },
    { /* 14 */
        PTP_DEV_PROP_EXPOSURE_PROGRAM_MODE,
        PTP_TYPE_UINT16, 
        1, 
        NULL, 
        NULL, 
        PTP_ENUM_FORM, 
        8, 
        u16_allowed_values 
    },
    { /* 15 */
        PTP_DEV_PROP_EXPOSURE_INDEX, 
        PTP_TYPE_UINT16, 
        1, 
        NULL, 
        NULL, 
        PTP_ENUM_FORM, 
        9, 
        iso_allowed_values 
    },
    { /* 16 */
        PTP_DEV_PROP_EXPOSURE_BIAS_COMPENSATION, 
        PTP_TYPE_INT16, 
        1, 
        NULL, 
        NULL, 
        PTP_RANGE_FORM, 
        3, 
        auto_exp_allowed_values 
    },
    { /* 17 */
        PTP_DEV_PROP_DATE_TIME, 
        PTP_TYPE_STR, 
        1, 
        NULL, 
        NULL, 
        PTP_NONE_FORM, 
        0, 
        NULL 
    },
    { /* 18 */
        PTP_DEV_PROP_CAPTURE_DELAY, 
        PTP_TYPE_UINT32, 
        1, 
        NULL, 
        NULL, 
        PTP_RANGE_FORM, 
        3, 
        delay_allowed_values 
    },
    { /* 19 */
        PTP_DEV_PROP_STILL_CAPTURE_MODE, 
        PTP_TYPE_UINT16, 
        1, 
        NULL, 
        NULL, 
        PTP_ENUM_FORM, 
        4, 
        u16_allowed_values 
    },
    { /* 20 */
        PTP_DEV_PROP_CONTRAST, 
        PTP_TYPE_UINT8, 
        1, 
        NULL, 
        NULL, 
        PTP_RANGE_FORM, 
        3, 
        u8_percent_allowed_values
    },
    { /* 21 */
        PTP_DEV_PROP_SHARPNESS, 
        PTP_TYPE_UINT8, 
        1, 
        NULL, 
        NULL, 
        PTP_RANGE_FORM, 
        3, 
        u8_percent_allowed_values 
    },
    { /* 22 */
        PTP_DEV_PROP_DIGITAL_ZOOM, 
        PTP_TYPE_UINT8, 
        1, 
        NULL, 
        NULL, 
        PTP_RANGE_FORM, 
        3, 
        zoom_allowed_values 
    },
    { /* 23 */
        PTP_DEV_PROP_EFFECT_MODE, 
        PTP_TYPE_UINT16, 
        1, 
        NULL, 
        NULL, 
        PTP_ENUM_FORM, 
        4, 
        u16_allowed_values 
    },
    
    { /* 24 */
        PTP_DEV_PROP_BURST_NUMBER, 
        PTP_TYPE_UINT16, 
        1, 
        NULL, 
        NULL, 
        PTP_RANGE_FORM, 
        3, 
        u16_range_allowed_values 
    },
    { /* 25 */
        PTP_DEV_PROP_BURST_INTERVAL, 
        PTP_TYPE_UINT16, 
        1, 
        NULL, 
        NULL, 
        PTP_RANGE_FORM, 
        3, 
        burst_interval_allowed_values 
    },
    { /* 26 */
        PTP_DEV_PROP_TIMELAPSE_NUMBER, 
        PTP_TYPE_UINT16, 
        1, 
        NULL, 
        NULL, 
        PTP_RANGE_FORM, 
        3, 
        u16_range_allowed_values 
    },
    { /* 27 */
        PTP_DEV_PROP_TIMELAPSE_INTERVAL, 
        PTP_TYPE_UINT32, 
        1, 
        NULL, 
        NULL, 
        PTP_RANGE_FORM, 
        3, 
        delay_allowed_values 
    },
    { /* 28 */
        PTP_DEV_PROP_FOCUS_METERING_MODE, 
        PTP_TYPE_UINT16, 
        1, 
        NULL, 
        NULL, 
        PTP_ENUM_FORM, 
        3, 
        u16_allowed_values 
    },
    { /* 29 */
        PTP_DEV_PROP_UPLOAD_URL, 
        PTP_TYPE_STR, 
        1, 
        NULL, 
        NULL, 
        PTP_NONE_FORM, 
        0, 
        NULL 
    },
    { /* 30 */
        PTP_DEV_PROP_ARTIST, 
        PTP_TYPE_STR, 
        1, 
        NULL, 
        NULL, 
        PTP_NONE_FORM, 
        0, 
        NULL 
    },
    { /* 31 */
        PTP_DEV_PROP_COPYRIGHT_INFO, 
        PTP_TYPE_STR, 
        1, 
        NULL, 
        NULL, 
        PTP_NONE_FORM, 
        0,
        NULL 
    }    
};

#define ARRAY_ELEMENTS(a) (sizeof(a)/sizeof (a[0]))

void devprops_init(void)
{
    jint_t i;

    /* Clear all values */
    j_memset(devprop_value_map, 0, sizeof(devprop_value_map));

    /* Initialize value_map data types (this must be done first) */
    for (i=0; i<NUM_DEV_PROPS; i++)
    {
        devprop_value_map[i].def.data_type = devprop_desc_map[i].data_type;
        devprop_value_map[i].curr.data_type = devprop_desc_map[i].data_type;
    }

    /* Initialize allowed values */
    u8_percent_allowed_values[1].data.uint8 = 100;      /* Max */
    u8_percent_allowed_values[2].data.uint8 = 1;        /* Step */

    compression_allowed_values[1].data.uint8 = 5;       /* Max */
    compression_allowed_values[2].data.uint8 = 1;       /* Step */

    for (i=0; i<ARRAY_ELEMENTS(u16_allowed_values); i++)
        u16_allowed_values[i].data.uint16 = i;

    aperture_allowed_values[0].data.uint16 = 280;
    aperture_allowed_values[1].data.uint16 = 3200;
    aperture_allowed_values[2].data.uint16 = 100;

    focal_len_allowed_values[0].data.uint32 = 2800;
    focal_len_allowed_values[1].data.uint32 = 30000;
    focal_len_allowed_values[2].data.uint32 = 100;

    focus_dist_allowed_values[0].data.uint16 = 280;
    focus_dist_allowed_values[1].data.uint16 = 0xFFFF;
    focus_dist_allowed_values[2].data.uint16 = 100;

    shutter_allowed_values[0].data.uint32 = 3;
    shutter_allowed_values[1].data.uint32 = 300000;
    shutter_allowed_values[2].data.uint32 = 100;

    iso_allowed_values[0].data.uint16 = 100;
    iso_allowed_values[1].data.uint16 = 200;
    iso_allowed_values[2].data.uint16 = 400;
    iso_allowed_values[3].data.uint16 = 800;
    iso_allowed_values[4].data.uint16 = 1200;
    iso_allowed_values[5].data.uint16 = 1600;
    iso_allowed_values[6].data.uint16 = 3200;
    iso_allowed_values[7].data.uint16 = 6400;
    iso_allowed_values[8].data.uint16 = 0xFFFF;

    auto_exp_allowed_values[0].data.uint16 = (juint16_t)-5000;
    auto_exp_allowed_values[1].data.uint16 = 5000;
    auto_exp_allowed_values[2].data.uint16 = 1000;

    delay_allowed_values[1].data.uint32 = 10000;
    delay_allowed_values[2].data.uint32 = 50;

    zoom_allowed_values[0].data.uint8 = 10;
    zoom_allowed_values[1].data.uint8 = 200;
    zoom_allowed_values[2].data.uint8 = 10;

    u16_range_allowed_values[0].data.uint16 = 1;
    u16_range_allowed_values[1].data.uint16 = 0xFFFF;
    u16_range_allowed_values[2].data.uint16 = 1;

    burst_interval_allowed_values[1].data.uint16 = 0xFFFF;
    burst_interval_allowed_values[2].data.uint16 = 1;

    /* Initialize IMAGE_SIZE allowed values */
    devprop_desc_map[2].num_allowed_values = 3;
    devprop_desc_map[2].allowed_values = jmalloc(3*sizeof(ptp_prop_value_t),
        M_ZERO);
    devprop_desc_map[2].allowed_values[0].data_type = PTP_TYPE_STR;
    devprop_desc_map[2].allowed_values[0].data.string =
        ptp_string_atow("800 x 600");
    devprop_desc_map[2].allowed_values[1].data_type = PTP_TYPE_STR;
    devprop_desc_map[2].allowed_values[1].data.string =
        ptp_string_atow("1024 x 768");
    devprop_desc_map[2].allowed_values[2].data_type = PTP_TYPE_STR;
    devprop_desc_map[2].allowed_values[2].data.string =
        ptp_string_atow("2048 x 1024");

    /* Initialize RGB_GAIN allowed values */
    devprop_desc_map[5].num_allowed_values = 5;
    devprop_desc_map[5].allowed_values = jmalloc(5*sizeof(ptp_prop_value_t),
        M_ZERO);
    devprop_desc_map[5].allowed_values[0].data_type = PTP_TYPE_STR;
    devprop_desc_map[5].allowed_values[0].data.string =
        ptp_string_atow("1:1:1");
    devprop_desc_map[5].allowed_values[1].data_type = PTP_TYPE_STR;
    devprop_desc_map[5].allowed_values[1].data.string =
        ptp_string_atow("10:1:10");
    devprop_desc_map[5].allowed_values[2].data_type = PTP_TYPE_STR;
    devprop_desc_map[5].allowed_values[2].data.string =
        ptp_string_atow("1:10:1");
    devprop_desc_map[5].allowed_values[3].data_type = PTP_TYPE_STR;
    devprop_desc_map[5].allowed_values[3].data.string =
        ptp_string_atow("15:15:1");
    devprop_desc_map[5].allowed_values[4].data_type = PTP_TYPE_STR;
    devprop_desc_map[5].allowed_values[4].data.string =
        ptp_string_atow("0:15:5");

    /* Assign default values */
    devprop_value_map[0].def.data.uint8 = 100;
    for (i=1; i<NUM_DEV_PROPS; i++)
    {
        /* Use first allowed value as default value, where applicable */
        if (devprop_desc_map[i].num_allowed_values &&
            devprop_desc_map[i].allowed_values)
        {
            ptp_prop_value_copy(&devprop_value_map[i].def,
                &devprop_desc_map[i].allowed_values[0]);
        }
    }

    /* Initialize value_map and assign values to desc_map */
    for (i=0; i<NUM_DEV_PROPS; i++)
    {
        ptp_prop_value_copy(&devprop_value_map[i].curr,
            &devprop_value_map[i].def);
        devprop_desc_map[i].current_value = &devprop_value_map[i].curr;
        devprop_desc_map[i].default_value = &devprop_value_map[i].def;
    }
}

#undef ARRAY_ELEMENTS

void devprops_free(void)
{
    int i;

    /* Clear all current and default values */
    for (i=0; i<NUM_DEV_PROPS; i++)
    {
        ptp_prop_value_clear(&devprop_value_map[i].def);
        ptp_prop_value_clear(&devprop_value_map[i].curr);
    }

    /* Free IMAGE_SIZE allowed values */
    for (i=0; i<devprop_desc_map[2].num_allowed_values; i++)
        ptp_prop_value_clear(&devprop_desc_map[2].allowed_values[i]);
    jfree(devprop_desc_map[2].allowed_values);
    devprop_desc_map[2].allowed_values = NULL;
    devprop_desc_map[2].num_allowed_values = 0;

    /* Free RGB_GAIN allowed values */
    for (i=0; i<devprop_desc_map[5].num_allowed_values; i++)
        ptp_prop_value_clear(&devprop_desc_map[5].allowed_values[i]);
    jfree(devprop_desc_map[5].allowed_values);
    devprop_desc_map[5].allowed_values = NULL;
    devprop_desc_map[5].num_allowed_values = 0;
}

