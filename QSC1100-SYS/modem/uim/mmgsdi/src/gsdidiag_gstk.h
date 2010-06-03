#ifndef GSDIDIAG_GSTK_H
#define GSDIDIAG_GSTK_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   GSDI DIAG GSTK Diagnostic Interface

Supports automated test of the GSTK (Generic Sim Toolkit) via DIAG commands

Copyright (c) 2008 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                              Edit History

 $Header: //source/qcom/qct/modem/uim/mmgsdi/rel/07H1_2/src/gsdidiag_gstk.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------
11/13/08   xz      Add support of de-reg
09/29/08   xz      Added support of passing GSTK state via DIAG port
09/02/08   xz      Added support of gstk_client_reg_ext()
08/11/08   tml     Initial version
===========================================================================*/

#include "customer.h"

#ifdef FEATURE_GSTK_TEST_AUTOMATION
#error code not present
#endif /* FEATURE_GSTK_TEST_AUTOMATION */

#endif /*  GSDIDIAG_GSTK_H */

