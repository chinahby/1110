/*****************************************************************************
* FILENAME
* tlg1100App.c
*
*
* ABSTRACT
* This file contains example procedures for initializing and managing the
* TLG1100. Note: This example code assumes that TLG_EMBEDDED_TIMERS is 
* enabled.
*
* $Revision: 1.2 $
*
* (c) 2006 Telegent Systems
*****************************************************************************/

#include "tlg1100Hal.h"
#include "tlg1100i2c.h"
#include "tlg1100App.h"
#include "analog_tv.h"
#include "TimerEvents.h"
#include "rda5888adp.h"

/* Default Channel Maps for USA, Canada, Korea, Taiwan, Mexico, 
Chile, Venezuela, the Philippines */
/** Default Channel map for NTSC */
tlg_channel_t(tlg_ntsc_chn_map) = { // vision_freq = center_freq - 1.725 - freq_offset(=0.025)
    {1,      57000000,  6,    TLG_VSTD_NTSC_M	},
    {2,      63000000,  6,    TLG_VSTD_NTSC_M	},
    {3,      69000000,  6,    TLG_VSTD_NTSC_M	},
    {4,      79000000,  6,    TLG_VSTD_NTSC_M	},
    {5,      85000000,  6,    TLG_VSTD_NTSC_M	},
    {6,     177000000,  6,    TLG_VSTD_NTSC_M	},
    {7,     183000000,  6,    TLG_VSTD_NTSC_M	},
    {8,     189000000,  6,    TLG_VSTD_NTSC_M	},
    {9,     195000000,  6,    TLG_VSTD_NTSC_M   },
    {10,    201000000,  6,    TLG_VSTD_NTSC_M	},
    {11,    207000000,  6,    TLG_VSTD_NTSC_M	},
    {12,    213000000,  6,    TLG_VSTD_NTSC_M	},
    {13,    473000000,  6,    TLG_VSTD_NTSC_M	},
    {14,    479000000,  6,    TLG_VSTD_NTSC_M	},
    {15,    485000000,  6,    TLG_VSTD_NTSC_M	},
    {16,    491000000,  6,    TLG_VSTD_NTSC_M	},
    {17,    497000000,  6,    TLG_VSTD_NTSC_M	},
    {18,    503000000,  6,    TLG_VSTD_NTSC_M	},
    {19,    509000000,  6,    TLG_VSTD_NTSC_M	},
    {20,    515000000,  6,    TLG_VSTD_NTSC_M	},
    {21,    521000000,  6,    TLG_VSTD_NTSC_M	},
    {22,    527000000,  6,    TLG_VSTD_NTSC_M	},
    {23,    533000000,  6,    TLG_VSTD_NTSC_M	},
    {24,    539000000,  6,    TLG_VSTD_NTSC_M	},
    {25,    545000000,  6,    TLG_VSTD_NTSC_M	},
    {26,    551000000,  6,    TLG_VSTD_NTSC_M	},
    {27,    557000000,  6,    TLG_VSTD_NTSC_M	},
    {28,    563000000,  6,    TLG_VSTD_NTSC_M	},
    {29,    569000000,  6,    TLG_VSTD_NTSC_M	},
    {30,    575000000,  6,    TLG_VSTD_NTSC_M	},
    {31,    581000000,  6,    TLG_VSTD_NTSC_M	},
    {32,    587000000,  6,    TLG_VSTD_NTSC_M	},
    {33,    593000000,  6,    TLG_VSTD_NTSC_M	},
    {34,    599000000,  6,    TLG_VSTD_NTSC_M	},
    {35,    605000000,  6,    TLG_VSTD_NTSC_M	},
    {36,    611000000,  6,    TLG_VSTD_NTSC_M	},
    {37,    617000000,  6,    TLG_VSTD_NTSC_M	},
    {38,    623000000,  6,    TLG_VSTD_NTSC_M	},
    {39,    629000000,  6,    TLG_VSTD_NTSC_M	},
    {40,    635000000,  6,    TLG_VSTD_NTSC_M	},
    {41,    641000000,  6,    TLG_VSTD_NTSC_M	},
    {42,    647000000,  6,    TLG_VSTD_NTSC_M	},
    {43,    653000000,  6,    TLG_VSTD_NTSC_M	},
    {44,    659000000,  6,    TLG_VSTD_NTSC_M	},
    {45,    665000000,  6,    TLG_VSTD_NTSC_M	},
    {46,    671000000,  6,    TLG_VSTD_NTSC_M	},
    {47,    677000000,	6,    TLG_VSTD_NTSC_M},
    {48,    683000000,  6,    TLG_VSTD_NTSC_M	},
    {49,    689000000,  6,    TLG_VSTD_NTSC_M	},
    {50,    695000000,  6,    TLG_VSTD_NTSC_M	},
    {51,    701000000,  6,    TLG_VSTD_NTSC_M	},
    {52,    707000000,  6,    TLG_VSTD_NTSC_M	},
    {53,    713000000,  6,    TLG_VSTD_NTSC_M	},
    {54,    719000000,  6,    TLG_VSTD_NTSC_M	},
    {55,    725000000,  6,    TLG_VSTD_NTSC_M	},
    {56,    731000000,  6,    TLG_VSTD_NTSC_M	},
    {57,    737000000,  6,    TLG_VSTD_NTSC_M	},
    {58,    743000000,  6,    TLG_VSTD_NTSC_M	},
    {59,    749000000,  6,    TLG_VSTD_NTSC_M	},
    {60,    755000000,  6,    TLG_VSTD_NTSC_M	},
    {61,    761000000,  6,    TLG_VSTD_NTSC_M	},
    {62,    767000000,  6,    TLG_VSTD_NTSC_M	},
    {63,    773000000,  6,    TLG_VSTD_NTSC_M	},
    {64,    779000000,  6,    TLG_VSTD_NTSC_M	},
    {65,    785000000,  6,    TLG_VSTD_NTSC_M	},
    {66,    791000000,  6,    TLG_VSTD_NTSC_M	},
    {67,    797000000,  6,    TLG_VSTD_NTSC_M	},
    {68,    803000000,  6,    TLG_VSTD_NTSC_M	},	
    {0, 0, 0,0}
};


/* Default Channel Map for China */
tlg_channel_t(tlg_pald_chn_map) = { // vision_freq = center_freq - 2.625 - freq_offset(=0.125) 
    {1,      52500000,  8,  TLG_VSTD_PAL_D},
    {2,      60500000,  8,  TLG_VSTD_PAL_D},
    {3,      68500000,  8,  TLG_VSTD_PAL_D},
    {4,      80000000,  8,  TLG_VSTD_PAL_D},
    {5,      88000000,  8,  TLG_VSTD_PAL_D},
    {6,     171000000,  8,  TLG_VSTD_PAL_D},
    {7,     179000000,  8,  TLG_VSTD_PAL_D},
    {8,     187000000,  8,  TLG_VSTD_PAL_D},
    {9,     195000000,  8,  TLG_VSTD_PAL_D},
    {10,    203000000,  8,  TLG_VSTD_PAL_D},
    {11,    211000000,  8,  TLG_VSTD_PAL_D},
    {12,    219000000,  8,  TLG_VSTD_PAL_D},
    {13,    474000000,  8,  TLG_VSTD_PAL_D},
    {14,    482000000,  8,  TLG_VSTD_PAL_D},
    {15,    490000000,  8,  TLG_VSTD_PAL_D},
    {16,    498000000,  8,  TLG_VSTD_PAL_D},
    {17,    506000000,  8,  TLG_VSTD_PAL_D},
    {18,    514000000,  8,  TLG_VSTD_PAL_D},
    {19,    522000000,  8,  TLG_VSTD_PAL_D},
    {20,    530000000,  8,  TLG_VSTD_PAL_D},
    {21,    538000000,  8,  TLG_VSTD_PAL_D},
    {22,    546000000,  8,  TLG_VSTD_PAL_D},
    {23,    554000000,  8,  TLG_VSTD_PAL_D},
    {24,    562000000,  8,  TLG_VSTD_PAL_D},
    {25,    610000000,  8,  TLG_VSTD_PAL_D},
    {26,    618000000,  8,  TLG_VSTD_PAL_D},
    {27,    626000000,  8,  TLG_VSTD_PAL_D},
    {28,    634000000,  8,  TLG_VSTD_PAL_D},
    {29,    642000000,  8,  TLG_VSTD_PAL_D},
    {30,    650000000,  8,  TLG_VSTD_PAL_D},
    {31,    658000000,  8,  TLG_VSTD_PAL_D},
    {32,    666000000,  8,  TLG_VSTD_PAL_D},
    {33,    674000000,  8,  TLG_VSTD_PAL_D},
    {34,    682000000,  8,  TLG_VSTD_PAL_D},
    {35,    690000000,  8,  TLG_VSTD_PAL_D},
    {36,    698000000,  8,  TLG_VSTD_PAL_D},
    {37,    706000000,  8,  TLG_VSTD_PAL_D},
    {38,    714000000,  8,  TLG_VSTD_PAL_D},
    {39,    722000000,  8,  TLG_VSTD_PAL_D},
    {40,    730000000,  8,  TLG_VSTD_PAL_D},
    {41,    738000000,  8,  TLG_VSTD_PAL_D},
    {42,    746000000,  8,  TLG_VSTD_PAL_D},
    {43,    754000000,  8,  TLG_VSTD_PAL_D},
    {44,    762000000,  8,  TLG_VSTD_PAL_D},
    {45,    770000000,  8,  TLG_VSTD_PAL_D},
    {46,    778000000,  8,  TLG_VSTD_PAL_D},
    {47,    786000000,  8,  TLG_VSTD_PAL_D},
    {48,    794000000,  8,  TLG_VSTD_PAL_D},
    {49,    802000000,  8,  TLG_VSTD_PAL_D},
    {50,    810000000,  8,  TLG_VSTD_PAL_D},
    {51,    818000000,  8,  TLG_VSTD_PAL_D},
    {52,    826000000,  8,  TLG_VSTD_PAL_D},
    {53,    834000000,  8,  TLG_VSTD_PAL_D},
    {54,    842000000,  8,  TLG_VSTD_PAL_D},
    {55,    850000000,  8,  TLG_VSTD_PAL_D},
    {56,    858000000,  8,  TLG_VSTD_PAL_D},
    {57,    866000000,  8,  TLG_VSTD_PAL_D},
    {58,    874000000,  8,  TLG_VSTD_PAL_D},
    {59,    882000000,  8,  TLG_VSTD_PAL_D},
    {60,    890000000,  8,    TLG_VSTD_PAL_D  },
    {61,    898000000,  8,    TLG_VSTD_PAL_D  },
    {62,    906000000,  8,    TLG_VSTD_PAL_D  },
    {63,    914000000,  8,    TLG_VSTD_PAL_D  },
    {64,    922000000,  8,    TLG_VSTD_PAL_D  },
    {65,    930000000,  8,    TLG_VSTD_PAL_D  },
    {66,    938000000,  8,    TLG_VSTD_PAL_D  },
    {67,    946000000,  8,    TLG_VSTD_PAL_D  },
    {68,    954000000,  8,    TLG_VSTD_PAL_D  },
    {69,    962000000,  8,    TLG_VSTD_PAL_D  },
    {0, 0, 0,0}
};

tlg_channel_t(tlg_s_shenzhen_chn_map) = { // vision_freq = center_freq - 2.125 - freq_offset(=0.625)
    {1,      52500000,  8,  TLG_VSTD_PAL_I},
    {2,      60500000,  8,  TLG_VSTD_PAL_I},
    {3,      68500000,  8,  TLG_VSTD_PAL_I},
    {4,      80000000,  8,  TLG_VSTD_PAL_I},
    {5,      88000000,  8,  TLG_VSTD_PAL_I},
    {6,     171000000,  8,  TLG_VSTD_PAL_I},
    {7,     179000000,  8,  TLG_VSTD_PAL_I},
    {8,     187000000,  8,  TLG_VSTD_PAL_I},
    {9,     195000000,  8,  TLG_VSTD_PAL_I},
    {10,    203000000,  8,  TLG_VSTD_PAL_I},
    {11,    211000000,  8,  TLG_VSTD_PAL_I},
    {12,    219000000,  8,  TLG_VSTD_PAL_I},
    {13,    474000000,  8,  TLG_VSTD_PAL_I},
    {14,    482000000,  8,  TLG_VSTD_PAL_I},
    {15,    490000000,  8,  TLG_VSTD_PAL_I},
    {16,    498000000,  8,  TLG_VSTD_PAL_I},
    {17,    506000000,  8,  TLG_VSTD_PAL_I},
    {18,    514000000,  8,  TLG_VSTD_PAL_I},
    {19,    522000000,  8,  TLG_VSTD_PAL_I},
    {20,    530000000,  8,  TLG_VSTD_PAL_I},
    {21,    538000000,  8,  TLG_VSTD_PAL_I},
    {22,    546000000,  8,  TLG_VSTD_PAL_I},
    {23,    554000000,  8,  TLG_VSTD_PAL_I},
    {24,    562000000,  8,  TLG_VSTD_PAL_I},
    {25,    610000000,  8,  TLG_VSTD_PAL_I},
    {26,    618000000,  8,  TLG_VSTD_PAL_I},
    {27,    626000000,  8,  TLG_VSTD_PAL_I},
    {28,    634000000,  8,  TLG_VSTD_PAL_I},
    {29,    642000000,  8,  TLG_VSTD_PAL_I},
    {30,    650000000,  8,  TLG_VSTD_PAL_I},
    {31,    658000000,  8,  TLG_VSTD_PAL_I},
    {32,    666000000,  8,  TLG_VSTD_PAL_I},
    {33,    674000000,  8,  TLG_VSTD_PAL_I},
    {34,    682000000,  8,  TLG_VSTD_PAL_I},
    {35,    690000000,  8,  TLG_VSTD_PAL_I},
    {36,    698000000,  8,  TLG_VSTD_PAL_I},
    {37,    706000000,  8,  TLG_VSTD_PAL_I},
    {38,    714000000,  8,  TLG_VSTD_PAL_I},
    {39,    722000000,  8,  TLG_VSTD_PAL_I},
    {40,    730000000,  8,  TLG_VSTD_PAL_I},
    {41,    738000000,  8,  TLG_VSTD_PAL_I},
    {42,    746000000,  8,  TLG_VSTD_PAL_I},
    {43,    754000000,  8,  TLG_VSTD_PAL_I},
    {44,    762000000,  8,  TLG_VSTD_PAL_I},
    {45,    770000000,  8,  TLG_VSTD_PAL_I},
    {46,    778000000,  8,  TLG_VSTD_PAL_I},
    {47,    786000000,  8,  TLG_VSTD_PAL_I},
    {48,    794000000,  8,  TLG_VSTD_PAL_I},
    {49,    802000000,  8,  TLG_VSTD_PAL_I},
    {50,    810000000,  8,  TLG_VSTD_PAL_I},
    {51,    818000000,  8,  TLG_VSTD_PAL_I},
    {52,    826000000,  8,  TLG_VSTD_PAL_I},
    {53,    834000000,  8,  TLG_VSTD_PAL_I},
    {54,    842000000,  8,  TLG_VSTD_PAL_I},
    {55,    850000000,  8,  TLG_VSTD_PAL_I},
    {56,    858000000,  8,  TLG_VSTD_PAL_I},
    {57,    866000000,  8,  TLG_VSTD_PAL_I},
    {58,    874000000,  8,  TLG_VSTD_PAL_I},
    {59,    882000000,  8,  TLG_VSTD_PAL_I},
    {60,    890000000,  8,    TLG_VSTD_PAL_I  },
    {61,    898000000,  8,    TLG_VSTD_PAL_I  },
    {62,    906000000,  8,    TLG_VSTD_PAL_I  },
    {63,    914000000,  8,    TLG_VSTD_PAL_I  },
    {64,    922000000,  8,    TLG_VSTD_PAL_I  },
    {65,    930000000,  8,    TLG_VSTD_PAL_I  },
    {66,    938000000,  8,    TLG_VSTD_PAL_I  },
    {67,    946000000,  8,    TLG_VSTD_PAL_I  },
    {68,    954000000,  8,    TLG_VSTD_PAL_I  },
    {69,    962000000,  8,    TLG_VSTD_PAL_I  },
    
    {0, 0, 0,0}

};


/* Default Channel Map for Vietnam */
tlg_channel_t(tlg_vietnam_chn_map) = { // vision_freq = center_freq - 2.625 - freq_offset(=0.125)
    {1,      52500000,  8,    TLG_VSTD_PAL_D  },
    {2,      62000000,  8,    TLG_VSTD_PAL_D  },
    {3,      80000000,  8,    TLG_VSTD_PAL_D  },
    {4,      96000000,  8,    TLG_VSTD_PAL_D  },
    {5,     178000000,  8,    TLG_VSTD_PAL_D  },
    {6,     186000000,  8,    TLG_VSTD_PAL_D  },
    {7,     194000000,  8,    TLG_VSTD_PAL_D  },
    {8,     202000000,  8,    TLG_VSTD_PAL_D  },
    {9,    210000000,  8,    TLG_VSTD_PAL_D  },
    {10,    218000000,  8,    TLG_VSTD_PAL_D  },
    {11,    226000000,  8,    TLG_VSTD_PAL_D  },
    {12,    474000000,  8,    TLG_VSTD_PAL_K  },
    {13,    482000000,  8,    TLG_VSTD_PAL_K  },
    {14,    490000000,  8,    TLG_VSTD_PAL_K  },
    {15,    498000000,  8,    TLG_VSTD_PAL_K  },
    {16,    506000000,  8,    TLG_VSTD_PAL_K  },
    {17,    514000000,  8,    TLG_VSTD_PAL_K  },
    {18,    522000000,  8,    TLG_VSTD_PAL_K  },
    {19,    530000000,  8,    TLG_VSTD_PAL_K  },
    {20,    538000000,  8,    TLG_VSTD_PAL_K  },
    {21,    546000000,  8,    TLG_VSTD_PAL_K  },
    {22,    554000000,  8,    TLG_VSTD_PAL_K  },
    {23,    562000000,  8,    TLG_VSTD_PAL_K  },
    {24,    570000000,  8,    TLG_VSTD_PAL_K  },
    {25,    578000000,  8,    TLG_VSTD_PAL_K  },
    {26,    586000000,  8,    TLG_VSTD_PAL_K  },
    {27,    594000000,  8,    TLG_VSTD_PAL_K  },
    {28,    602000000,  8,    TLG_VSTD_PAL_K  },
    {29,    610000000,  8,    TLG_VSTD_PAL_K  },
    {30,    618000000,  8,    TLG_VSTD_PAL_K  },
    {31,    626000000,  8,    TLG_VSTD_PAL_K  },
    {32,    634000000,  8,    TLG_VSTD_PAL_K  },
    {33,    642000000,  8,    TLG_VSTD_PAL_K  },
    {34,    650000000,  8,    TLG_VSTD_PAL_K  },
    {35,    658000000,  8,    TLG_VSTD_PAL_K  },
    {36,    666000000,  8,    TLG_VSTD_PAL_K  },
    {37,    674000000,  8,    TLG_VSTD_PAL_K  },
    {38,    682000000,  8,    TLG_VSTD_PAL_K  },
    {39,    690000000,  8,    TLG_VSTD_PAL_K  },
    {40,    698000000,  8,    TLG_VSTD_PAL_K  },
    {41,    706000000,  8,    TLG_VSTD_PAL_K  },
    {42,    714000000,  8,    TLG_VSTD_PAL_K  },
    {43,    722000000,  8,    TLG_VSTD_PAL_K  },
    {44,    730000000,  8,    TLG_VSTD_PAL_K  },
    {45,    738000000,  8,    TLG_VSTD_PAL_K  },
    {46,    746000000,  8,    TLG_VSTD_PAL_K  },
    {47,    754000000,  8,    TLG_VSTD_PAL_K  },
    {48,    762000000,  8,    TLG_VSTD_PAL_K  },
    {49,    770000000,  8,    TLG_VSTD_PAL_K  },
    {50,    778000000,  8,    TLG_VSTD_PAL_K  },
    {51,    786000000,  8,    TLG_VSTD_PAL_K  },
    {52,    794000000,  8,    TLG_VSTD_PAL_K  },
    {53,    802000000,  8,    TLG_VSTD_PAL_K  },
    {54,    810000000,  8,    TLG_VSTD_PAL_K  },
    {55,    818000000,  8,    TLG_VSTD_PAL_K  },
    {56,    826000000,  8,    TLG_VSTD_PAL_K  },
    {57,    834000000,  8,    TLG_VSTD_PAL_K  },
    {58,    842000000,  8,    TLG_VSTD_PAL_K  },
    {59,    850000000,  8,    TLG_VSTD_PAL_K  },
    {60,    858000000,  8,    TLG_VSTD_PAL_K  },
    {0, 0, 0, 0}
};

/* Default Channel Map for Western Europe, Turkey, UAE, Afghanistan,
Singapore, Thailand, Cambodia, Indonesia, Malaysia, India,Pakistan,Portugal,Spain, and Laos */
tlg_channel_t(tlg_w_europe_chn_map) = { // PAL B: vision_freq = center_freq - 2.125 - freq_offset(=0.125)
    {1,      50500000,      7,    TLG_VSTD_PAL_B},
    {2,      57500000,      7,    TLG_VSTD_PAL_B},
    {3,      64500000,      7,    TLG_VSTD_PAL_B},
    {4,      177500000,     7,    TLG_VSTD_PAL_B},
    {5,      184500000,     7,    TLG_VSTD_PAL_B},
    {6,      191500000,     7,    TLG_VSTD_PAL_B},
    {7,      198500000,     7,    TLG_VSTD_PAL_B},
    {8,      205500000,     7,    TLG_VSTD_PAL_B},
    {9 ,     212500000,     7,    TLG_VSTD_PAL_B},
    {10,     219500000,     7,    TLG_VSTD_PAL_B},
    {11,     226500000,     7,    TLG_VSTD_PAL_B},
    {12,     474000000,     8,    TLG_VSTD_PAL_G},// PAL G: vision_freq = center_freq - 2.125 - freq_offset(=0.625)
    {13,     482000000,     8,    TLG_VSTD_PAL_G},
    {14,     490000000,     8,    TLG_VSTD_PAL_G},
    {15,     498000000,     8,    TLG_VSTD_PAL_G},
    {16,     506000000,     8,    TLG_VSTD_PAL_G},
    {17,     514000000,     8,    TLG_VSTD_PAL_G},
    {18,     522000000,     8,    TLG_VSTD_PAL_G},
    {19,     530000000,     8,    TLG_VSTD_PAL_G},
    {20,     538000000,     8,    TLG_VSTD_PAL_G},
    {21,     546000000,     8,    TLG_VSTD_PAL_G},
    {22,     554000000,     8,    TLG_VSTD_PAL_G},
    {23,     562000000,     8,    TLG_VSTD_PAL_G},
    {24,     570000000,     8,    TLG_VSTD_PAL_G},
    {25,     578000000,     8,    TLG_VSTD_PAL_G},
    {26,     586000000,     8,    TLG_VSTD_PAL_G},
    {27,     594000000,     8,    TLG_VSTD_PAL_G},
    {28,     602000000,     8,    TLG_VSTD_PAL_G},
    {29,     610000000,     8,    TLG_VSTD_PAL_G},
    {30,     618000000,     8,    TLG_VSTD_PAL_G},
    {31,     626000000,     8,    TLG_VSTD_PAL_G},
    {32,     634000000,     8,    TLG_VSTD_PAL_G},
    {33,     642000000,     8,    TLG_VSTD_PAL_G},
    {34,     650000000,     8,    TLG_VSTD_PAL_G},
    {35,     658000000,     8,    TLG_VSTD_PAL_G},
    {36,     666000000,     8,    TLG_VSTD_PAL_G},
    {37,     674000000,     8,    TLG_VSTD_PAL_G},
    {38,     682000000,     8,    TLG_VSTD_PAL_G},
    {39,     690000000,     8,    TLG_VSTD_PAL_G},
    {40,     698000000,     8,    TLG_VSTD_PAL_G},
    {41,     706000000,     8,    TLG_VSTD_PAL_G},
    {42,     714000000,     8,    TLG_VSTD_PAL_G},
    {43,     722000000,     8,    TLG_VSTD_PAL_G},
    {44,     730000000,     8,    TLG_VSTD_PAL_G},
    {45,     738000000,     8,    TLG_VSTD_PAL_G},
    {46,     746000000,     8,    TLG_VSTD_PAL_G},
    {47,     754000000,     8,    TLG_VSTD_PAL_G},
    {48,     762000000,     8,    TLG_VSTD_PAL_G},
    {49,     770000000,     8,    TLG_VSTD_PAL_G},
    {50,     778000000,     8,    TLG_VSTD_PAL_G},
    {51,     786000000,     8,    TLG_VSTD_PAL_G},
    {52,     794000000,     8,    TLG_VSTD_PAL_G},
    {53,     802000000,     8,    TLG_VSTD_PAL_G},
    {54,     810000000,     8,    TLG_VSTD_PAL_G},
    {55,     818000000,     8,    TLG_VSTD_PAL_G},
    {56,     826000000,     8,    TLG_VSTD_PAL_G},
    {57,     834000000,     8,    TLG_VSTD_PAL_G},
    {58,     842000000,     8,    TLG_VSTD_PAL_G},
    {59,    850000000,  8,    TLG_VSTD_PAL_G  },
    {60,    858000000,  8,    TLG_VSTD_PAL_G  },
    {0, 0, 0,0}
};

/* Default Channel Map for Burma */
tlg_channel_t(tlg_burma_chn_map) = { // vision_freq = center_freq - 1.725 - freq_offset(=0.025)
    {1,      51000000,  6,    TLG_VSTD_NTSC_M  },
    {2,      53000000,  6,    TLG_VSTD_NTSC_M  },
    {3,      55000000,  6,    TLG_VSTD_NTSC_M  },
    {4,      57000000,  6,    TLG_VSTD_NTSC_M  },
    {5,      59000000,  6,    TLG_VSTD_NTSC_M  },
    {6,      61000000,  6,    TLG_VSTD_NTSC_M  },
    {7,      63000000,  6,    TLG_VSTD_NTSC_M  },
    {8,      65000000,  6,    TLG_VSTD_NTSC_M  },
    {9,      67000000,  6,    TLG_VSTD_NTSC_M  },
    {10,     69000000,  6,    TLG_VSTD_NTSC_M  },
    {11,     71000000,  6,    TLG_VSTD_NTSC_M  },
    {12,     73000000,  6,    TLG_VSTD_NTSC_M  },
    {13,     75000000,  6,    TLG_VSTD_NTSC_M  },
    {14,     77000000,  6,    TLG_VSTD_NTSC_M  },
    {15,     79000000,  6,    TLG_VSTD_NTSC_M  },
    {16,     81000000,  6,    TLG_VSTD_NTSC_M  },
    {17,     83000000,  6,    TLG_VSTD_NTSC_M  },
    {18,     85000000,  6,    TLG_VSTD_NTSC_M  },
    {19,     87000000,  6,    TLG_VSTD_NTSC_M  },
    {20,     89000000,  6,    TLG_VSTD_NTSC_M  },
    {21,     91000000,  6,    TLG_VSTD_NTSC_M  },
    {22,     93000000,  6,    TLG_VSTD_NTSC_M  },
    {23,     95000000,  6,    TLG_VSTD_NTSC_M  },
    {24,     97000000,  6,    TLG_VSTD_NTSC_M  },
    {25,     99000000,  6,    TLG_VSTD_NTSC_M  },
    {26,    101000000,  6,    TLG_VSTD_NTSC_M  },
    {27,    103000000,  6,    TLG_VSTD_NTSC_M  },
    {28,    105000000,  6,    TLG_VSTD_NTSC_M  },
    {29,    107000000,  6,    TLG_VSTD_NTSC_M  },
    {30,    109000000,  6,    TLG_VSTD_NTSC_M  },
    {31,    111000000,  6,    TLG_VSTD_NTSC_M  },
    {32,    169000000,  6,    TLG_VSTD_NTSC_M  },
    {33,    171000000,  6,    TLG_VSTD_NTSC_M  },
    {34,    173000000,  6,    TLG_VSTD_NTSC_M  },
    {35,    175000000,  6,    TLG_VSTD_NTSC_M  },
    {36,    177000000,  6,    TLG_VSTD_NTSC_M  },
    {37,    179000000,  6,    TLG_VSTD_NTSC_M  },
    {38,    181000000,  6,    TLG_VSTD_NTSC_M  },
    {39,    183000000,  6,    TLG_VSTD_NTSC_M  },
    {40,    185000000,  6,    TLG_VSTD_NTSC_M  },
    {41,    187000000,  6,    TLG_VSTD_NTSC_M  },
    {42,    189000000,  6,    TLG_VSTD_NTSC_M  },
    {43,    191000000,  6,    TLG_VSTD_NTSC_M  },
    {44,    193000000,  6,    TLG_VSTD_NTSC_M  },
    {45,    195000000,  6,    TLG_VSTD_NTSC_M  },
    {46,    197000000,  6,    TLG_VSTD_NTSC_M  },
    {47,    199000000,  6,    TLG_VSTD_NTSC_M  },
    {48,    201000000,  6,    TLG_VSTD_NTSC_M  },
    {49,    203000000,  6,    TLG_VSTD_NTSC_M  },
    {50,    205000000,  6,    TLG_VSTD_NTSC_M  },
    {51,    207000000,  6,    TLG_VSTD_NTSC_M  },
    {52,    209000000,  6,    TLG_VSTD_NTSC_M  },
    {53,    211000000,  6,    TLG_VSTD_NTSC_M  },
    {54,    213000000,  6,    TLG_VSTD_NTSC_M  },
    {55,    215000000,  6,    TLG_VSTD_NTSC_M  },
    {56,    217000000,  6,    TLG_VSTD_NTSC_M  },
    {57,    219000000,  6,    TLG_VSTD_NTSC_M  },
    {58,    221000000,  6,    TLG_VSTD_NTSC_M  },
    {59,    223000000,  6,    TLG_VSTD_NTSC_M  },
    {60,    225000000,  6,    TLG_VSTD_NTSC_M  },
    {61,    227000000,  6,    TLG_VSTD_NTSC_M  },
    {62,    229000000,  6,    TLG_VSTD_NTSC_M  },
    {63,    231000000,  6,    TLG_VSTD_NTSC_M  },
    {64,    473000000,  6,    TLG_VSTD_NTSC_M  },
    {65,    479000000,  6,    TLG_VSTD_NTSC_M  },
    {66,    485000000,  6,    TLG_VSTD_NTSC_M  },
    {67,    491000000,  6,    TLG_VSTD_NTSC_M  },
    {68,    497000000,  6,    TLG_VSTD_NTSC_M  },
    {69,    503000000,  6,    TLG_VSTD_NTSC_M  },
    {70,    509000000,  6,    TLG_VSTD_NTSC_M  },
    {71,    515000000,  6,    TLG_VSTD_NTSC_M  },
    {72,    521000000,  6,    TLG_VSTD_NTSC_M  },
    {73,    527000000,  6,    TLG_VSTD_NTSC_M  },
    {74,    533000000,  6,    TLG_VSTD_NTSC_M  },
    {75,    539000000,  6,    TLG_VSTD_NTSC_M  },
    {76,    545000000,  6,    TLG_VSTD_NTSC_M  },
    {77,    551000000,  6,    TLG_VSTD_NTSC_M  },
    {78,    557000000,  6,    TLG_VSTD_NTSC_M  },
    {79,    563000000,  6,    TLG_VSTD_NTSC_M  },
    {80,    569000000,  6,    TLG_VSTD_NTSC_M  },
    {81,    575000000,  6,    TLG_VSTD_NTSC_M  },
    {82,    581000000,  6,    TLG_VSTD_NTSC_M  },
    {83,    587000000,  6,    TLG_VSTD_NTSC_M  },
    {84,    593000000,  6,    TLG_VSTD_NTSC_M  },
    {85,    599000000,  6,    TLG_VSTD_NTSC_M  },
    {86,    605000000,  6,    TLG_VSTD_NTSC_M  },
    {87,    611000000,  6,    TLG_VSTD_NTSC_M  },
    {88,    617000000,  6,    TLG_VSTD_NTSC_M  },
    {89,    623000000,  6,    TLG_VSTD_NTSC_M  },
    {90,    629000000,  6,    TLG_VSTD_NTSC_M  },
    {91,    635000000,  6,    TLG_VSTD_NTSC_M  },
    {92,    641000000,  6,    TLG_VSTD_NTSC_M  },
    {93,    647000000,  6,    TLG_VSTD_NTSC_M  },
    {94,    653000000,  6,    TLG_VSTD_NTSC_M  },
    {95,    659000000,  6,    TLG_VSTD_NTSC_M  },
    {96,    665000000,  6,    TLG_VSTD_NTSC_M  },
    {97,    671000000,  6,    TLG_VSTD_NTSC_M  },
    {98,    677000000,  6,    TLG_VSTD_NTSC_M  },
    {99,    683000000,  6,    TLG_VSTD_NTSC_M  },
    {100,   689000000,  6,    TLG_VSTD_NTSC_M  },
    {101,   695000000,  6,    TLG_VSTD_NTSC_M  },
    {102,   701000000,  6,    TLG_VSTD_NTSC_M  },
    {103,   707000000,  6,    TLG_VSTD_NTSC_M  },
    {104,   713000000,  6,    TLG_VSTD_NTSC_M  },
    {105,   719000000,  6,    TLG_VSTD_NTSC_M  },
    {106,   725000000,  6,    TLG_VSTD_NTSC_M  },
    {107,   731000000,  6,    TLG_VSTD_NTSC_M  },
    {108,   737000000,  6,    TLG_VSTD_NTSC_M  },
    {109,   743000000,  6,    TLG_VSTD_NTSC_M  },
    {110,   749000000,  6,    TLG_VSTD_NTSC_M  },
    {111,   755000000,  6,    TLG_VSTD_NTSC_M  },
    {112,   761000000,  6,    TLG_VSTD_NTSC_M  },
    {113,   767000000,  6,    TLG_VSTD_NTSC_M  },
    {114,   773000000,  6,    TLG_VSTD_NTSC_M  },
    {115,   779000000,  6,    TLG_VSTD_NTSC_M  },
    {116,   785000000,  6,    TLG_VSTD_NTSC_M  },
    {117,   791000000,  6,    TLG_VSTD_NTSC_M  },
    {118,   797000000,  6,    TLG_VSTD_NTSC_M  },
    {119,   803000000,  6,    TLG_VSTD_NTSC_M  },
    {0, 0, 0, 0}
};

tlg_channel_t(tlg_ntsc_japan_chn_map) = { // vision_freq = center_freq - 1.725 - freq_offset(=0.025)
    {1,     93000000,   6,    TLG_VSTD_NTSC_M},
    {2,     99000000,   6,    TLG_VSTD_NTSC_M},
    {3,     105000000,  6,    TLG_VSTD_NTSC_M},
    {4,     173000000,  6,    TLG_VSTD_NTSC_M},
    {5,     179000000,  6,    TLG_VSTD_NTSC_M},
    {6,     185000000,  6,    TLG_VSTD_NTSC_M},
    {7,     191000000,  6,    TLG_VSTD_NTSC_M},
    {8,     195000000,  6,    TLG_VSTD_NTSC_M},
    {9,     201000000,  6,    TLG_VSTD_NTSC_M},
    {10,    207000000,  6,    TLG_VSTD_NTSC_M},
    {11,    213000000,  6,    TLG_VSTD_NTSC_M},
    {12,    219000000,  6,    TLG_VSTD_NTSC_M},
    {13,    473000000,  6,    TLG_VSTD_NTSC_M},
    {14,    479000000,  6,    TLG_VSTD_NTSC_M},
    {15,    485000000,  6,    TLG_VSTD_NTSC_M},
    {16,    491000000,  6,    TLG_VSTD_NTSC_M},
    {17,    497000000,  6,    TLG_VSTD_NTSC_M},
    {18,    503000000,  6,    TLG_VSTD_NTSC_M},
    {19,    509000000,  6,    TLG_VSTD_NTSC_M},
    {20,    515000000,  6,    TLG_VSTD_NTSC_M},
    {21,    521000000,  6,    TLG_VSTD_NTSC_M},
    {22,    527000000,  6,    TLG_VSTD_NTSC_M},
    {23,    533000000,  6,    TLG_VSTD_NTSC_M},
    {24,    539000000,  6,    TLG_VSTD_NTSC_M},
    {25,    545000000,  6,    TLG_VSTD_NTSC_M},
    {26,    551000000,  6,    TLG_VSTD_NTSC_M},
    {27,    557000000,  6,    TLG_VSTD_NTSC_M},
    {28,    563000000,  6,    TLG_VSTD_NTSC_M},
    {29,    569000000,  6,    TLG_VSTD_NTSC_M},
    {30,    575000000,  6,    TLG_VSTD_NTSC_M},
    {31,    581000000,  6,    TLG_VSTD_NTSC_M},
    {32,    587000000,  6,    TLG_VSTD_NTSC_M},
    {33,    593000000,  6,    TLG_VSTD_NTSC_M},
    {34,    599000000,  6,    TLG_VSTD_NTSC_M},
    {35,    605000000,  6,    TLG_VSTD_NTSC_M},
    {36,    611000000,  6,    TLG_VSTD_NTSC_M},
    {37,    617000000,  6,    TLG_VSTD_NTSC_M},
    {38,    623000000,  6,    TLG_VSTD_NTSC_M},
    {39,    629000000,  6,    TLG_VSTD_NTSC_M},
    {40,    635000000,  6,    TLG_VSTD_NTSC_M},
    {41,    641000000,  6,    TLG_VSTD_NTSC_M},
    {42,    647000000,  6,    TLG_VSTD_NTSC_M},
    {43,    653000000,  6,    TLG_VSTD_NTSC_M},
    {44,    659000000,  6,    TLG_VSTD_NTSC_M},
    {45,    665000000,  6,    TLG_VSTD_NTSC_M},
    {46,    671000000,  6,    TLG_VSTD_NTSC_M},
    {47,    677000000,  6,    TLG_VSTD_NTSC_M},
    {48,    683000000,  6,    TLG_VSTD_NTSC_M},
    {49,    689000000,  6,    TLG_VSTD_NTSC_M},
    {50,    695000000,  6,    TLG_VSTD_NTSC_M},
    {51,    701000000,  6,    TLG_VSTD_NTSC_M},
    {52,    707000000,  6,    TLG_VSTD_NTSC_M},
    {53,    713000000,  6,    TLG_VSTD_NTSC_M},
    {54,    719000000,  6,    TLG_VSTD_NTSC_M},
    {55,    725000000,  6,    TLG_VSTD_NTSC_M},
    {56,    731000000,  6,    TLG_VSTD_NTSC_M},
    {57,    737000000,  6,    TLG_VSTD_NTSC_M},
    {58,    743000000,  6,    TLG_VSTD_NTSC_M},
    {59,    749000000,  6,    TLG_VSTD_NTSC_M},
    {60,    755000000,  6,    TLG_VSTD_NTSC_M},
    {61,    761000000,  6,    TLG_VSTD_NTSC_M},
    {62,    767000000,  6,    TLG_VSTD_NTSC_M},
    {0, 0, 0,0}

};

tlg_channel_t(tlg_palb_newz_chn_map) = {
    {1,    47500000,  	7,  TLG_VSTD_PAL_B},
    {2,    57500000,  	7,  TLG_VSTD_PAL_B},
    {3,    64500000,  	7,  TLG_VSTD_PAL_B},
    {4,    177500000,  	7,  TLG_VSTD_PAL_B},
    {5,    184500000,  	7,  TLG_VSTD_PAL_B},
    {6,    191500000,  	7,  TLG_VSTD_PAL_B},
    {7,    198500000,  	7,  TLG_VSTD_PAL_B},
    {8,    205500000,  	7,  TLG_VSTD_PAL_B},
    {9,    212500000,  	7,  TLG_VSTD_PAL_B},
    {0, 0, 0,0}

};

/* Default Channel Maps for UK, Hong Kong, and South Africa */
tlg_channel_t(tlg_a_hongkong_chn_map) = {
    {1,      48000000,  7,    TLG_VSTD_PAL_B  },
    {2,      56000000,  7,    TLG_VSTD_PAL_B  },
    {3,      64000000,  7,    TLG_VSTD_PAL_B  },
    {4,     177500000,  7,    TLG_VSTD_PAL_B  },
    {5,     185500000,  7,    TLG_VSTD_PAL_B  },
    {6,     193500000,  7,    TLG_VSTD_PAL_B  },
    {7,     201500000,  7,    TLG_VSTD_PAL_B  },
    {8,     209500000,  7,    TLG_VSTD_PAL_B  },
    {9,    217500000,  7,    TLG_VSTD_PAL_B  },
    {10,    474000000,  8,    TLG_VSTD_PAL_I  },
    {11,    482000000,  8,    TLG_VSTD_PAL_I  },
    {12,    490000000,  8,    TLG_VSTD_PAL_I  },
    {13,    498000000,  8,    TLG_VSTD_PAL_I  },
    {14,    506000000,  8,    TLG_VSTD_PAL_I  },
    {15,    514000000,  8,    TLG_VSTD_PAL_I  },
    {16,    522000000,  8,    TLG_VSTD_PAL_I  },
    {17,    530000000,  8,    TLG_VSTD_PAL_I  },
    {18,    538000000,  8,    TLG_VSTD_PAL_I  },
    {19,    546000000,  8,    TLG_VSTD_PAL_I  },
    {20,    554000000,  8,    TLG_VSTD_PAL_I  },
    {21,    562000000,  8,    TLG_VSTD_PAL_I  },
    {22,    570000000,  8,    TLG_VSTD_PAL_I  },
    {23,    578000000,  8,    TLG_VSTD_PAL_I  },
    {24,    586000000,  8,    TLG_VSTD_PAL_I  },
    {25,    594000000,  8,    TLG_VSTD_PAL_I  },
    {26,    602000000,  8,    TLG_VSTD_PAL_I  },
    {27,    610000000,  8,    TLG_VSTD_PAL_I  },
    {28,    618000000,  8,    TLG_VSTD_PAL_I  },
    {29,    626000000,  8,    TLG_VSTD_PAL_I  },
    {30,    634000000,  8,    TLG_VSTD_PAL_I  },
    {31,    642000000,  8,    TLG_VSTD_PAL_I  },
    {32,    650000000,  8,    TLG_VSTD_PAL_I  },
    {33,    658000000,  8,    TLG_VSTD_PAL_I  },
    {34,    666000000,  8,    TLG_VSTD_PAL_I  },
    {35,    674000000,  8,    TLG_VSTD_PAL_I  },
    {36,    682000000,  8,    TLG_VSTD_PAL_I  },
    {37,    690000000,  8,    TLG_VSTD_PAL_I  },
    {38,    698000000,  8,    TLG_VSTD_PAL_I  },
    {39,    706000000,  8,    TLG_VSTD_PAL_I  },
    {40,    714000000,  8,    TLG_VSTD_PAL_I  },
    {41,    722000000,  8,    TLG_VSTD_PAL_I  },
    {42,    730000000,  8,    TLG_VSTD_PAL_I  },
    {43,    738000000,  8,    TLG_VSTD_PAL_I  },
    {44,    746000000,  8,    TLG_VSTD_PAL_I  },
    {45,    754000000,  8,    TLG_VSTD_PAL_I  },
    {46,    762000000,  8,    TLG_VSTD_PAL_I  },
    {47,    770000000,  8,    TLG_VSTD_PAL_I  },
    {48,    778000000,  8,    TLG_VSTD_PAL_I  },
    {49,    786000000,  8,    TLG_VSTD_PAL_I  },
    {50,    794000000,  8,    TLG_VSTD_PAL_I  },
    {51,    802000000,  8,    TLG_VSTD_PAL_I  },
    {52,    810000000,  8,    TLG_VSTD_PAL_I  },
    {53,    818000000,  8,    TLG_VSTD_PAL_I  },
    {54,    826000000,  8,    TLG_VSTD_PAL_I  },
    {55,    834000000,  8,    TLG_VSTD_PAL_I  },
    {56,    842000000,  8,    TLG_VSTD_PAL_I  },
    {57,    850000000,  8,    TLG_VSTD_PAL_I  },
    {58,    858000000,  8,    TLG_VSTD_PAL_I  },
    {0, 0, 0, 0}
};


/* Default Channel Map for Brazil */
tlg_channel_t(tlg_brazil_chn_map) = { // vision_freq = center_freq - 1.725 - freq_offset(=0.025)
    {1,      57000000,  6,    TLG_VSTD_PAL_M  },
    {2,      63000000,  6,    TLG_VSTD_PAL_M  },
    {3,      69000000,  6,    TLG_VSTD_PAL_M  },
    {4,      79000000,  6,    TLG_VSTD_PAL_M  },
    {5,      85000000,  6,    TLG_VSTD_PAL_M  },
    {6,     177000000,  6,    TLG_VSTD_PAL_M  },
    {7,     183000000,  6,    TLG_VSTD_PAL_M  },
    {8,     189000000,  6,    TLG_VSTD_PAL_M  },
    {9,    195000000,  6,    TLG_VSTD_PAL_M  },
    {10,    201000000,  6,    TLG_VSTD_PAL_M  },
    {11,    207000000,  6,    TLG_VSTD_PAL_M  },
    {12,    213000000,  6,    TLG_VSTD_PAL_M  },
    {13,    473000000,  6,    TLG_VSTD_PAL_M  },
    {14,    479000000,  6,    TLG_VSTD_PAL_M  },
    {15,    485000000,  6,    TLG_VSTD_PAL_M  },
    {16,    491000000,  6,    TLG_VSTD_PAL_M  },
    {17,    497000000,  6,    TLG_VSTD_PAL_M  },
    {18,    503000000,  6,    TLG_VSTD_PAL_M  },
    {19,    509000000,  6,    TLG_VSTD_PAL_M  },
    {20,    515000000,  6,    TLG_VSTD_PAL_M  },
    {21,    521000000,  6,    TLG_VSTD_PAL_M  },
    {22,    527000000,  6,    TLG_VSTD_PAL_M  },
    {23,    533000000,  6,    TLG_VSTD_PAL_M  },
    {24,    539000000,  6,    TLG_VSTD_PAL_M  },
    {25,    545000000,  6,    TLG_VSTD_PAL_M  },
    {26,    551000000,  6,    TLG_VSTD_PAL_M  },
    {27,    557000000,  6,    TLG_VSTD_PAL_M  },
    {28,    563000000,  6,    TLG_VSTD_PAL_M  },
    {29,    569000000,  6,    TLG_VSTD_PAL_M  },
    {30,    575000000,  6,    TLG_VSTD_PAL_M  },
    {31,    581000000,  6,    TLG_VSTD_PAL_M  },
    {32,    587000000,  6,    TLG_VSTD_PAL_M  },
    {33,    593000000,  6,    TLG_VSTD_PAL_M  },
    {34,    599000000,  6,    TLG_VSTD_PAL_M  },
    {35,    605000000,  6,    TLG_VSTD_PAL_M  },
    {36,    611000000,  6,    TLG_VSTD_PAL_M  },
    {37,    617000000,  6,    TLG_VSTD_PAL_M  },
    {38,    623000000,  6,    TLG_VSTD_PAL_M  },
    {39,    629000000,  6,    TLG_VSTD_PAL_M  },
    {40,    635000000,  6,    TLG_VSTD_PAL_M  },
    {41,    641000000,  6,    TLG_VSTD_PAL_M  },
    {42,    647000000,  6,    TLG_VSTD_PAL_M  },
    {43,    653000000,  6,    TLG_VSTD_PAL_M  },
    {44,    659000000,  6,    TLG_VSTD_PAL_M  },
    {45,    665000000,  6,    TLG_VSTD_PAL_M  },
    {46,    671000000,  6,    TLG_VSTD_PAL_M  },
    {47,    677000000,  6,    TLG_VSTD_PAL_M  },
    {48,    683000000,  6,    TLG_VSTD_PAL_M  },
    {49,    689000000,  6,    TLG_VSTD_PAL_M  },
    {50,    695000000,  6,    TLG_VSTD_PAL_M  },
    {51,    701000000,  6,    TLG_VSTD_PAL_M  },
    {52,    707000000,  6,    TLG_VSTD_PAL_M  },
    {53,    713000000,  6,    TLG_VSTD_PAL_M  },
    {54,    719000000,  6,    TLG_VSTD_PAL_M  },
    {55,    725000000,  6,    TLG_VSTD_PAL_M  },
    {56,    731000000,  6,    TLG_VSTD_PAL_M  },
    {57,    737000000,  6,    TLG_VSTD_PAL_M  },
    {58,    743000000,  6,    TLG_VSTD_PAL_M  },
    {59,    749000000,  6,    TLG_VSTD_PAL_M  },
    {60,    755000000,  6,    TLG_VSTD_PAL_M  },
    {61,    761000000,  6,    TLG_VSTD_PAL_M  },
    {62,    767000000,  6,    TLG_VSTD_PAL_M  },
    {63,    773000000,  6,    TLG_VSTD_PAL_M  },
    {64,    779000000,  6,    TLG_VSTD_PAL_M  },
    {65,    785000000,  6,    TLG_VSTD_PAL_M  },
    {66,    791000000,  6,    TLG_VSTD_PAL_M  },
    {67,    797000000,  6,    TLG_VSTD_PAL_M  },
    {68,    803000000,  6,    TLG_VSTD_PAL_M  },
    {0, 0, 0, 0}
};

/* Default Channel Map for Argentina */
tlg_channel_t(tlg_argentina_chn_map) = {  // vision_freq = center_freq - 1.725 - freq_offset(=0.025)
    {1,      57000000,  6,    TLG_VSTD_PAL_NC  },
    {2,      63000000,  6,    TLG_VSTD_PAL_NC  },
    {3,      69000000,  6,    TLG_VSTD_PAL_NC  },
    {4,      79000000,  6,    TLG_VSTD_PAL_NC  },
    {5,      85000000,  6,    TLG_VSTD_PAL_NC  },
    {6,     177000000,  6,    TLG_VSTD_PAL_NC  },
    {7,     183000000,  6,    TLG_VSTD_PAL_NC  },
    {8,     189000000,  6,    TLG_VSTD_PAL_NC  },
    {9,    195000000,  6,    TLG_VSTD_PAL_NC  },
    {10,    201000000,  6,    TLG_VSTD_PAL_NC  },
    {11,    207000000,  6,    TLG_VSTD_PAL_NC  },
    {12,    213000000,  6,    TLG_VSTD_PAL_NC  },
    {13,    473000000,  6,    TLG_VSTD_PAL_NC  },
    {14,    479000000,  6,    TLG_VSTD_PAL_NC  },
    {15,    485000000,  6,    TLG_VSTD_PAL_NC  },
    {16,    491000000,  6,    TLG_VSTD_PAL_NC  },
    {17,    497000000,  6,    TLG_VSTD_PAL_NC  },
    {18,    503000000,  6,    TLG_VSTD_PAL_NC  },
    {19,    509000000,  6,    TLG_VSTD_PAL_NC  },
    {20,    515000000,  6,    TLG_VSTD_PAL_NC  },
    {21,    521000000,  6,    TLG_VSTD_PAL_NC  },
    {22,    527000000,  6,    TLG_VSTD_PAL_NC  },
    {23,    533000000,  6,    TLG_VSTD_PAL_NC  },
    {24,    539000000,  6,    TLG_VSTD_PAL_NC  },
    {25,    545000000,  6,    TLG_VSTD_PAL_NC  },
    {26,    551000000,  6,    TLG_VSTD_PAL_NC  },
    {27,    557000000,  6,    TLG_VSTD_PAL_NC  },
    {28,    563000000,  6,    TLG_VSTD_PAL_NC  },
    {29,    569000000,  6,    TLG_VSTD_PAL_NC  },
    {30,    575000000,  6,    TLG_VSTD_PAL_NC  },
    {31,    581000000,  6,    TLG_VSTD_PAL_NC  },
    {32,    587000000,  6,    TLG_VSTD_PAL_NC  },
    {33,    593000000,  6,    TLG_VSTD_PAL_NC  },
    {34,    599000000,  6,    TLG_VSTD_PAL_NC  },
    {35,    605000000,  6,    TLG_VSTD_PAL_NC  },
    {36,    611000000,  6,    TLG_VSTD_PAL_NC  },
    {37,    617000000,  6,    TLG_VSTD_PAL_NC  },
    {38,    623000000,  6,    TLG_VSTD_PAL_NC  },
    {39,    629000000,  6,    TLG_VSTD_PAL_NC  },
    {40,    635000000,  6,    TLG_VSTD_PAL_NC  },
    {41,    641000000,  6,    TLG_VSTD_PAL_NC  },
    {42,    647000000,  6,    TLG_VSTD_PAL_NC  },
    {43,    653000000,  6,    TLG_VSTD_PAL_NC  },
    {44,    659000000,  6,    TLG_VSTD_PAL_NC  },
    {45,    665000000,  6,    TLG_VSTD_PAL_NC  },
    {46,    671000000,  6,    TLG_VSTD_PAL_NC  },
    {47,    677000000,  6,    TLG_VSTD_PAL_NC  },
    {48,    683000000,  6,    TLG_VSTD_PAL_NC  },
    {49,    689000000,  6,    TLG_VSTD_PAL_NC  },
    {50,    695000000,  6,    TLG_VSTD_PAL_NC  },
    {51,    701000000,  6,    TLG_VSTD_PAL_NC  },
    {52,    707000000,  6,    TLG_VSTD_PAL_NC  },
    {53,    713000000,  6,    TLG_VSTD_PAL_NC  },
    {54,    719000000,  6,    TLG_VSTD_PAL_NC  },
    {55,    725000000,  6,    TLG_VSTD_PAL_NC  },
    {56,    731000000,  6,    TLG_VSTD_PAL_NC  },
    {57,    737000000,  6,    TLG_VSTD_PAL_NC  },
    {58,    743000000,  6,    TLG_VSTD_PAL_NC  },
    {59,    749000000,  6,    TLG_VSTD_PAL_NC  },
    {60,    755000000,  6,    TLG_VSTD_PAL_NC  },
    {61,    761000000,  6,    TLG_VSTD_PAL_NC  },
    {62,    767000000,  6,    TLG_VSTD_PAL_NC  },
    {63,    773000000,  6,    TLG_VSTD_PAL_NC  },
    {64,    779000000,  6,    TLG_VSTD_PAL_NC  },
    {65,    785000000,  6,    TLG_VSTD_PAL_NC  },
    {66,    791000000,  6,    TLG_VSTD_PAL_NC  },
    {67,    797000000,  6,    TLG_VSTD_PAL_NC  },
    {68,    803000000,  6,    TLG_VSTD_PAL_NC  },
    {0, 0, 0, 0}
};

//#define SUPPORT_CHANNEL_MAP_SECAM_BG
#ifdef SUPPORT_CHANNEL_MAP_SECAM_BG
tlg_channel_t(tlg_secam_b_g_chn_map) = 
{
    {2,      50500000,  7,    TLG_VSTD_SECAM_B  },
    {3,      57500000,  7,    TLG_VSTD_SECAM_B  },
    {4,      64500000,  7,    TLG_VSTD_SECAM_B  },
    {5,     177500000,  7,    TLG_VSTD_SECAM_B  },
    {6,     184500000,  7,    TLG_VSTD_SECAM_B  },
    {7,     191500000,  7,    TLG_VSTD_SECAM_B  },
    {8,     198500000,  7,    TLG_VSTD_SECAM_B  },
    {9,     205500000,  7,    TLG_VSTD_SECAM_B  },
    {10,    212500000,  7,    TLG_VSTD_SECAM_B  },
    {11,    219500000,  7,    TLG_VSTD_SECAM_B  },
    {12,    226500000,  7,    TLG_VSTD_SECAM_B  },
    {21,    474000000,  8,    TLG_VSTD_SECAM_G  },
    {22,    482000000,  8,    TLG_VSTD_SECAM_G  },
    {23,    490000000,  8,    TLG_VSTD_SECAM_G  },
    {24,    498000000,  8,    TLG_VSTD_SECAM_G  },
    {25,    506000000,  8,    TLG_VSTD_SECAM_G  },
    {26,    514000000,  8,    TLG_VSTD_SECAM_G  },
    {27,    522000000,  8,    TLG_VSTD_SECAM_G  },
    {28,    530000000,  8,    TLG_VSTD_SECAM_G  },
    {29,    538000000,  8,    TLG_VSTD_SECAM_G  },
    {30,    546000000,  8,    TLG_VSTD_SECAM_G  },
    {31,    554000000,  8,    TLG_VSTD_SECAM_G  },
    {32,    562000000,  8,    TLG_VSTD_SECAM_G  },
    {33,    570000000,  8,    TLG_VSTD_SECAM_G  },
    {34,    578000000,  8,    TLG_VSTD_SECAM_G  },
    {35,    586000000,  8,    TLG_VSTD_SECAM_G  },
    {36,    594000000,  8,    TLG_VSTD_SECAM_G  },
    {37,    602000000,  8,    TLG_VSTD_SECAM_G  },
    {38,    610000000,  8,    TLG_VSTD_SECAM_G  },
    {39,    618000000,  8,    TLG_VSTD_SECAM_G  },
    {40,    626000000,  8,    TLG_VSTD_SECAM_G  },
    {41,    634000000,  8,    TLG_VSTD_SECAM_G  },
    {42,    642000000,  8,    TLG_VSTD_SECAM_G  },
    {43,    650000000,  8,    TLG_VSTD_SECAM_G  },
    {44,    658000000,  8,    TLG_VSTD_SECAM_G  },
    {45,    666000000,  8,    TLG_VSTD_SECAM_G  },
    {46,    674000000,  8,    TLG_VSTD_SECAM_G  },
    {47,    682000000,  8,    TLG_VSTD_SECAM_G  },
    {48,    690000000,  8,    TLG_VSTD_SECAM_G  },
    {49,    698000000,  8,    TLG_VSTD_SECAM_G  },
    {50,    706000000,  8,    TLG_VSTD_SECAM_G  },
    {51,    714000000,  8,    TLG_VSTD_SECAM_G  },
    {52,    722000000,  8,    TLG_VSTD_SECAM_G  },
    {53,    730000000,  8,    TLG_VSTD_SECAM_G  },
    {54,    738000000,  8,    TLG_VSTD_SECAM_G  },
    {55,    746000000,  8,    TLG_VSTD_SECAM_G  },
    {56,    754000000,  8,    TLG_VSTD_SECAM_G  },
    {57,    762000000,  8,    TLG_VSTD_SECAM_G  },
    {58,    770000000,  8,    TLG_VSTD_SECAM_G  },
    {59,    778000000,  8,    TLG_VSTD_SECAM_G  },
    {60,    786000000,  8,    TLG_VSTD_SECAM_G  },
    {61,    794000000,  8,    TLG_VSTD_SECAM_G  },
    {62,    802000000,  8,    TLG_VSTD_SECAM_G  },
    {63,    810000000,  8,    TLG_VSTD_SECAM_G  },
    {64,    818000000,  8,    TLG_VSTD_SECAM_G  },
    {65,    826000000,  8,    TLG_VSTD_SECAM_G  },
    {66,    834000000,  8,    TLG_VSTD_SECAM_G  },
    {67,    842000000,  8,    TLG_VSTD_SECAM_G  },
    {68,    850000000,  8,    TLG_VSTD_SECAM_G  },
    {69,    858000000,  8,    TLG_VSTD_SECAM_G  },
    {0, 0, 0, 0}
};
#endif 

#define SUPPORT_CHANNEL_MAP_SECAM_DK
#ifdef SUPPORT_CHANNEL_MAP_SECAM_DK
tlg_channel_t(tlg_secam_d_k_chn_map) = {
    {1,      52500000,  8,    TLG_VSTD_SECAM_D  },
    {2,      62000000,  8,    TLG_VSTD_SECAM_D  },
    {3,      80000000,  8,    TLG_VSTD_SECAM_D  },
    {4,      88000000,  8,    TLG_VSTD_SECAM_D  },
    {5,      96000000,  8,    TLG_VSTD_SECAM_D  },
    {6,     178000000,  8,    TLG_VSTD_SECAM_D  },
    {7,     186000000,  8,    TLG_VSTD_SECAM_D  },
    {8,     194000000,  8,    TLG_VSTD_SECAM_D  },
    {9,     202000000,  8,    TLG_VSTD_SECAM_D  },
    {10,    210000000,  8,    TLG_VSTD_SECAM_D  },
    {11,    218000000,  8,    TLG_VSTD_SECAM_D  },
    {12,    226000000,  8,    TLG_VSTD_SECAM_D  },
    {21,    474000000,  8,    TLG_VSTD_SECAM_K  },
    {22,    482000000,  8,    TLG_VSTD_SECAM_K  },
    {23,    490000000,  8,    TLG_VSTD_SECAM_K  },
    {24,    498000000,  8,    TLG_VSTD_SECAM_K  },
    {25,    506000000,  8,    TLG_VSTD_SECAM_K  },
    {26,    514000000,  8,    TLG_VSTD_SECAM_K  },
    {27,    522000000,  8,    TLG_VSTD_SECAM_K  },
    {28,    530000000,  8,    TLG_VSTD_SECAM_K  },
    {29,    538000000,  8,    TLG_VSTD_SECAM_K  },
    {30,    546000000,  8,    TLG_VSTD_SECAM_K  },
    {31,    554000000,  8,    TLG_VSTD_SECAM_K  },
    {32,    562000000,  8,    TLG_VSTD_SECAM_K  },
    {33,    570000000,  8,    TLG_VSTD_SECAM_K  },
    {34,    578000000,  8,    TLG_VSTD_SECAM_K  },
    {35,    586000000,  8,    TLG_VSTD_SECAM_K  },
    {36,    594000000,  8,    TLG_VSTD_SECAM_K  },
    {37,    602000000,  8,    TLG_VSTD_SECAM_K  },
    {38,    610000000,  8,    TLG_VSTD_SECAM_K  },
    {39,    618000000,  8,    TLG_VSTD_SECAM_K  },
    {40,    626000000,  8,    TLG_VSTD_SECAM_K  },
    {41,    634000000,  8,    TLG_VSTD_SECAM_K  },
    {42,    642000000,  8,    TLG_VSTD_SECAM_K  },
    {43,    650000000,  8,    TLG_VSTD_SECAM_K  },
    {44,    658000000,  8,    TLG_VSTD_SECAM_K  },
    {45,    666000000,  8,    TLG_VSTD_SECAM_K  },
    {46,    674000000,  8,    TLG_VSTD_SECAM_K  },
    {47,    682000000,  8,    TLG_VSTD_SECAM_K  },
    {48,    690000000,  8,    TLG_VSTD_SECAM_K  },
    {49,    698000000,  8,    TLG_VSTD_SECAM_K  },
    {50,    706000000,  8,    TLG_VSTD_SECAM_K  },
    {51,    714000000,  8,    TLG_VSTD_SECAM_K  },
    {52,    722000000,  8,    TLG_VSTD_SECAM_K  },
    {53,    730000000,  8,    TLG_VSTD_SECAM_K  },
    {54,    738000000,  8,    TLG_VSTD_SECAM_K  },
    {55,    746000000,  8,    TLG_VSTD_SECAM_K  },
    {56,    754000000,  8,    TLG_VSTD_SECAM_K  },
    {57,    762000000,  8,    TLG_VSTD_SECAM_K  },
    {58,    770000000,  8,    TLG_VSTD_SECAM_K  },
    {59,    778000000,  8,    TLG_VSTD_SECAM_K  },
    {60,    786000000,  8,    TLG_VSTD_SECAM_K  },
    {61,    794000000,  8,    TLG_VSTD_SECAM_K  },
    {62,    802000000,  8,    TLG_VSTD_SECAM_K  },
    {63,    810000000,  8,    TLG_VSTD_SECAM_K  },
    {64,    818000000,  8,    TLG_VSTD_SECAM_K  },
    {65,    826000000,  8,    TLG_VSTD_SECAM_K  },
    {66,    834000000,  8,    TLG_VSTD_SECAM_K  },
    {67,    842000000,  8,    TLG_VSTD_SECAM_K  },
    {68,    850000000,  8,    TLG_VSTD_SECAM_K  },
    {69,    858000000,  8,    TLG_VSTD_SECAM_K  },
    {0, 0, 0, 0}
};

tlg_channel_t(tlg_secam_d_k_hybrid_chn_map) = {
    {1,      52500000,  8,    TLG_VSTD_PAL_SECAM_D  },
    {2,      62000000,  8,    TLG_VSTD_PAL_SECAM_D  },
    {3,      80000000,  8,    TLG_VSTD_PAL_SECAM_D  },
    {4,      88000000,  8,    TLG_VSTD_PAL_SECAM_D  },
    {5,      96000000,  8,    TLG_VSTD_PAL_SECAM_D  },
    {6,     178000000,  8,    TLG_VSTD_PAL_SECAM_D  },
    {7,     186000000,  8,    TLG_VSTD_PAL_SECAM_D  },
    {8,     194000000,  8,    TLG_VSTD_PAL_SECAM_D  },
    {9,     202000000,  8,    TLG_VSTD_PAL_SECAM_D  },
    {10,    210000000,  8,    TLG_VSTD_PAL_SECAM_D  },
    {11,    218000000,  8,    TLG_VSTD_PAL_SECAM_D  },
    {12,    226000000,  8,    TLG_VSTD_PAL_SECAM_D  },
    {21,    474000000,  8,    TLG_VSTD_PAL_SECAM_K  },
    {22,    482000000,  8,    TLG_VSTD_PAL_SECAM_K  },
    {23,    490000000,  8,    TLG_VSTD_PAL_SECAM_K  },
    {24,    498000000,  8,    TLG_VSTD_PAL_SECAM_K  },
    {25,    506000000,  8,    TLG_VSTD_PAL_SECAM_K  },
    {26,    514000000,  8,    TLG_VSTD_PAL_SECAM_K  },
    {27,    522000000,  8,    TLG_VSTD_PAL_SECAM_K  },
    {28,    530000000,  8,    TLG_VSTD_PAL_SECAM_K  },
    {29,    538000000,  8,    TLG_VSTD_PAL_SECAM_K  },
    {30,    546000000,  8,    TLG_VSTD_PAL_SECAM_K  },
    {31,    554000000,  8,    TLG_VSTD_PAL_SECAM_K  },
    {32,    562000000,  8,    TLG_VSTD_PAL_SECAM_K  },
    {33,    570000000,  8,    TLG_VSTD_PAL_SECAM_K  },
    {34,    578000000,  8,    TLG_VSTD_PAL_SECAM_K  },
    {35,    586000000,  8,    TLG_VSTD_PAL_SECAM_K  },
    {36,    594000000,  8,    TLG_VSTD_PAL_SECAM_K  },
    {37,    602000000,  8,    TLG_VSTD_PAL_SECAM_K  },
    {38,    610000000,  8,    TLG_VSTD_PAL_SECAM_K  },
    {39,    618000000,  8,    TLG_VSTD_PAL_SECAM_K  },
    {40,    626000000,  8,    TLG_VSTD_PAL_SECAM_K  },
    {41,    634000000,  8,    TLG_VSTD_PAL_SECAM_K  },
    {42,    642000000,  8,    TLG_VSTD_PAL_SECAM_K  },
    {43,    650000000,  8,    TLG_VSTD_PAL_SECAM_K  },
    {44,    658000000,  8,    TLG_VSTD_PAL_SECAM_K  },
    {45,    666000000,  8,    TLG_VSTD_PAL_SECAM_K  },
    {46,    674000000,  8,    TLG_VSTD_PAL_SECAM_K  },
    {47,    682000000,  8,    TLG_VSTD_PAL_SECAM_K  },
    {48,    690000000,  8,    TLG_VSTD_PAL_SECAM_K  },
    {49,    698000000,  8,    TLG_VSTD_PAL_SECAM_K  },
    {50,    706000000,  8,    TLG_VSTD_PAL_SECAM_K  },
    {51,    714000000,  8,    TLG_VSTD_PAL_SECAM_K  },
    {52,    722000000,  8,    TLG_VSTD_PAL_SECAM_K  },
    {53,    730000000,  8,    TLG_VSTD_PAL_SECAM_K  },
    {54,    738000000,  8,    TLG_VSTD_PAL_SECAM_K  },
    {55,    746000000,  8,    TLG_VSTD_PAL_SECAM_K  },
    {56,    754000000,  8,    TLG_VSTD_PAL_SECAM_K  },
    {57,    762000000,  8,    TLG_VSTD_PAL_SECAM_K  },
    {58,    770000000,  8,    TLG_VSTD_PAL_SECAM_K  },
    {59,    778000000,  8,    TLG_VSTD_PAL_SECAM_K  },
    {60,    786000000,  8,    TLG_VSTD_PAL_SECAM_K  },
    {61,    794000000,  8,    TLG_VSTD_PAL_SECAM_K  },
    {62,    802000000,  8,    TLG_VSTD_PAL_SECAM_K  },
    {63,    810000000,  8,    TLG_VSTD_PAL_SECAM_K  },
    {64,    818000000,  8,    TLG_VSTD_PAL_SECAM_K  },
    {65,    826000000,  8,    TLG_VSTD_PAL_SECAM_K  },
    {66,    834000000,  8,    TLG_VSTD_PAL_SECAM_K  },
    {67,    842000000,  8,    TLG_VSTD_PAL_SECAM_K  },
    {68,    850000000,  8,    TLG_VSTD_PAL_SECAM_K  },
    {69,    858000000,  8,    TLG_VSTD_PAL_SECAM_K  },
    {0, 0, 0, 0}
};
#endif


int factory_ch[3]={1,2,3};
tlg_channel_t(tlg_factory_chn_map) = {
    {1,      80000000,   8,    TLG_VSTD_PAL_D},
    {2,      219000000,  8,    TLG_VSTD_PAL_D},
    {3,      802000000,  8,    TLG_VSTD_PAL_D},
    {0, 0, 0, 0}
};


p_tlg_channel_t(p_tlg_cur_map) = NULL; /* Current Default Channel Map*/
int              tlg_cur_mode  = TLG_MODE_NONE; /* Current Mode      */
int              tlg_cur_std   = TLG_VSTD_NONE; /* Current Video Std */
int              tlg_cur_chn_cnt    = 0;             /* Current Channel Count */
int              tlg_init_done = 0;             /* Flag Init called  */
uint32           tlg_i2c_addr  = 0x62;
uint16 current_tv_area =0xFFFF;
uint16 current_chn = 0;


/*****************************************************************************
* TLGAPP_Init_Aux
******************************************************************************/
TLG1100DLL_API int TLGAPP_Init_Aux(uint32 base_addr, int start)
{
    if (start == TLG_NEW_INIT)
        RDA_DCDCInit(base_addr);

    tlg_init_done = 1;

    return(TLG_ERR_SUCCESS);
}

TLG1100DLL_API int TLGAPP_Init(uint32 base_addr)
{
	RDA_GetVersion();

    return TLGAPP_Init_Aux(base_addr, TLG_NEW_INIT);
}

/*****************************************************************************
* TLGAPP_ChannelSetup
*****************************************************************************/
TLG1100DLL_API int TLGAPP_ChannelSetup(int mode, int standard, int bw)
{
    if (!tlg_init_done) TLGAPP_Init(tlg_i2c_addr);
    tlg_cur_std = standard;	

    return (TLG_ERR_SUCCESS);
}
    
/*****************************************************************************
* TLGAPP_SetChannelMap
******************************************************************************/
TLG1100DLL_API int TLGAPP_SetChannelMap(p_tlg_channel_t(map), int mode, int vidstd, int bw)
{
    int i;
    TLGAPP_ChannelSetup(mode, vidstd, bw);
    p_tlg_cur_map = map;
    tlg_cur_chn_cnt = 0;
    for(i=0; p_tlg_cur_map[i] CHN_S != 0; i++)
	{
        tlg_cur_chn_cnt++;
    }
    return (TLG_ERR_SUCCESS);
}

TLG1100DLL_API int TLGAPP_SetChannelMapExt(int area)
{
	int result = TLG_ERR_SUCCESS;
	
	current_tv_area = area;

	kal_prompt_trace(MOD_MED,"TLGAPP_SetChannelMapExt, area = %d\n", area);

	switch(area)
	{
		case TV_AREA_RUSSIA:
			result = TLGAPP_SetChannelMap(tlg_secam_d_k_chn_map, TLG_MODE_ANALOG_TV,
                       TLG_VSTD_SECAM_D, TLG_BW_8);
			break;

		case TV_AREA_CHINA:
			result = TLGAPP_SetChannelMap(tlg_pald_chn_map, TLG_MODE_ANALOG_TV, 
			           TLG_VSTD_PAL_D, TLG_BW_8);	
			break;

		case TV_AREA_CHINA_SHENZHEN:
			result = TLGAPP_SetChannelMap(tlg_s_shenzhen_chn_map, TLG_MODE_ANALOG_TV, 
                     TLG_VSTD_PAL_I, TLG_BW_8);	
			break;

		case TV_AREA_CHINA_HONGKONG:
		case TV_AREA_UK:
		case TV_AREA_SOUTH_AFRICA:
		case TV_AREA_CHINA_MACAO:
			result = TLGAPP_SetChannelMap(tlg_a_hongkong_chn_map, TLG_MODE_ANALOG_TV, 
			 TLG_VSTD_PAL_I, TLG_BW_8);	
			break;

		case TV_AREA_CHINA_TAIWAN:
		case TV_AREA_AMERICA:
		case TV_AREA_MEXICO:
		case TV_AREA_PHILIPINES:
		case TV_AREA_KOREA:
		case TV_AREA_CHILE:
		case TV_AREA_VENEZUELA: 
		case TV_AREA_CANADA:
			result = TLGAPP_SetChannelMap(tlg_ntsc_chn_map, TLG_MODE_ANALOG_TV, 
			TLG_VSTD_NTSC_M, TLG_BW_7);
			break;
			
		case TV_AREA_ARGENTINA:
			result = TLGAPP_SetChannelMap(tlg_argentina_chn_map, TLG_MODE_ANALOG_TV, 
			TLG_VSTD_PAL_NC, TLG_BW_8);
			break;

            case TV_AREA_BRAZIL:
			result = TLGAPP_SetChannelMap(tlg_brazil_chn_map, TLG_MODE_ANALOG_TV, 
			TLG_VSTD_PAL_M, TLG_BW_6);
			break;
			 
		case TV_AREA_JAPAN:
			result = TLGAPP_SetChannelMap(tlg_ntsc_japan_chn_map, TLG_MODE_ANALOG_TV, 
			TLG_VSTD_NTSC_M, TLG_BW_6);
			break;
			
		case TV_AREA_WESTERNEUROP:
		case TV_AREA_TURKEY:
		//case TV_AREA_UAE:/*阿联酋*/
		case TV_AREA_AFGHANISTA:
		case TV_AREA_SINGAPORE:
		case TV_AREA_THAI:
		case TV_AREA_CAMBODIA:
		case TV_AREA_INDONESIA:
		case TV_AREA_MALAYSIA:
		case TV_AREA_LAOS:
		case TV_AREA_INDIA:
		case TV_AREA_PAKISTAN:
		case TV_AREA_SPAIN:
		case TV_AREA_PORTUGAL:
		case TV_AREA_ABU_DHABI:
		case TV_AREA_YEMEN:
		case TV_AREA_BAHRAIN:
		case TV_AREA_KUWAIT:
			result = TLGAPP_SetChannelMap(tlg_w_europe_chn_map, TLG_MODE_ANALOG_TV, 
			TLG_VSTD_PAL_B, TLG_BW_7);	
			break;

		case TV_AREA_VIETNAM:
			result = TLGAPP_SetChannelMap(tlg_vietnam_chn_map, TLG_MODE_ANALOG_TV, 
			TLG_VSTD_PAL_D, TLG_BW_8);
			break;

		case TV_AREA_BURMA:
			result = TLGAPP_SetChannelMap(tlg_burma_chn_map, TLG_MODE_ANALOG_TV,
			TLG_VSTD_NTSC_M, TLG_BW_6);
			break;

		case TV_AREA_FACTORYMODE:
			result = TLGAPP_SetChannelMap(tlg_factory_chn_map, TLG_MODE_ANALOG_TV, 
			TLG_VSTD_PAL_D, TLG_BW_8);
			break;
			
		default:
			current_tv_area=TV_AREA_CHINA;
			result = TLGAPP_SetChannelMap(tlg_pald_chn_map, TLG_MODE_ANALOG_TV, 
			TLG_VSTD_PAL_I, TLG_BW_8);	
			break;
	}

	return result;
}

/*****************************************************************************
* TLGAPP_SetChannelAux
******************************************************************************/
TLG1100DLL_API int TLGAPP_SetChannelAux(int chn, int useFast)
{
    int i = 0;
	int result = 0;
	
    if (chn == 0)
    {
		TLGAPP_TurnOnTestPattern();
#ifdef RDA5888_OUTPUT_SP1_ENABLE
		TLG_WriteReg(0x62, 0x12f, 0x0231); // 0231 -> 0x007e
#endif

		return 0;
    }

    kal_prompt_trace(0,"TLGAPP_SetChannelAux, p_tlg_cur_map = %d",p_tlg_cur_map);
	current_chn = chn;
	
    if (p_tlg_cur_map == NULL)
		return 0;

    kal_prompt_trace(0,"TLGAPP_SetChannelAux_2");

    if (!tlg_init_done)
		TLGAPP_Init(tlg_i2c_addr);

    kal_prompt_trace(0,"TLGAPP_SetChannelAux_3, tlg_cur_mode=%d,chn=%d", tlg_cur_mode, chn);

    while (p_tlg_cur_map[i] CHN_S != 0)
    {
        /* found channel to change to */
        if (p_tlg_cur_map[i] CHN_S == chn)
        {
            /* update bw if needed */
            TLGAPP_ChannelSetup(tlg_cur_mode, p_tlg_cur_map[i] STD_S, p_tlg_cur_map[i] BW_S);
            result = TLG_ScanTVChn(tlg_i2c_addr, p_tlg_cur_map[i] HZ_S);
			break;
        }
        i++;
    }

	if (p_tlg_cur_map[i] CHN_S == 0)
	{
		kal_prompt_trace(0,"TLGAPP_SetChannelAux_3, channel not exist");
		TLGAPP_TurnOnTestPattern();
#ifdef RDA5888_OUTPUT_SP1_ENABLE
		TLG_WriteReg(0x62, 0x12f, 0x0231); // 0231 -> 0x007e
#endif	
	}
	
    return(result);
}

/*****************************************************************************
* TLGAPP_SetChannel
******************************************************************************/
TLG1100DLL_API int TLGAPP_SetChannel(int chn)
{
	int ret=0;
	
	kal_prompt_trace(0,"TLGAPP_SetChanne, chn = %d\n",chn);
	GPTI_StopItem(g_nRdamtvTimer);

	//analogtv_preview_stop();
	kal_prompt_trace(0,"TLGAPP_SetChanne_2\n");
	ret = TLGAPP_SetChannelAux(chn, false /* useFast */);
	kal_prompt_trace(0,"TLGAPP_SetChanne_3\n");

#if 1
	if (ret == 0)
	{
	    kal_prompt_trace(0,"TLGAPP_SetChanne_4\n");
		g_nIsTPMode = 1;
		TLG_ReadReg(0x62, 0x130, &g_nTPRegBak);
		TLG_WriteReg(0x62, 0x130, 0x803);
#ifdef RDA5888_OUTPUT_SP1_ENABLE
		TLG_WriteReg(0x62, 0x12f, 0x0231); // 0231 -> 0x007e
#endif		
	}
	else
	{
	    kal_prompt_trace(0,"TLGAPP_SetChanne_5\n");
		g_nIsTPMode = 0;
	}
	
	kal_prompt_trace(0,"TLGAPP_SetChanne_6\n");
#endif

	GPTI_StopItem(g_nRdamtvTimer);
	GPTI_StartItem(g_nRdamtvTimer,kal_milli_secs_to_ticks(100),RDA_ATVCheckStatus,0);	
	
	kal_prompt_trace(0,"TLGAPP_SetChanne end\n");
    return ret;
}

/*****************************************************************************
* TLGAPP_TurnOnTestPattern
******************************************************************************/
TLG1100DLL_API void TLGAPP_TurnOnTestPattern(void)
{
    kal_prompt_trace(0,"TLGAPP_TurnOnTestPattern");
    TLG_ScanTVChn(0x62, 100000);
	TLG_WriteReg(0x62, 0x130,0x0803);	
}

uint32 channelTable[]= {530000000, 538000000, 482000000,618000000,674000000,714000000,187000000};
unsigned char Channel = 0;
TLG1100DLL_API void TLGAPP_TurnOnTestChannel(void)
{
    kal_prompt_trace(0,"TLGAPP_TurnOnTestChannel");
    tlg_cur_std = TLG_VSTD_PAL_D;
 //   TLG_ScanTVChn(0x62, 530000000); // 东方娱乐
  TLG_ScanTVChn(0x62, channelTable[Channel++]); // 东方娱乐

	if(Channel==7)
		Channel = 0;
  
  kal_sleep_task(300);
  RDA_SysReset();

	//GPTI_StopItem(g_nRdamtvTimer);
	//GPTI_StartItem(g_nRdamtvTimer,kal_milli_secs_to_ticks(100),RDA_ATVCheckStatus,0);	
	//kal_prompt_trace(0,"GPTI_StartItem\n");

}

/*****************************************************************************
* TLGAPP_TurnOffTestPattern
******************************************************************************/
TLG1100DLL_API void TLGAPP_TurnOffTestPattern(void)
{
    TLG_WriteReg(0x62, 0x130,0x0800);
}
