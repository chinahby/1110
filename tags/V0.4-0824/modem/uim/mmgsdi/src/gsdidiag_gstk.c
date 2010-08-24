/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   SAT/USAT  Diagnostic Interface


Supports automated testing of SAT/USAT/CCAT cards via DIAG commands.
Implements packet processing.

Copyright (c) 2008-2009 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                              Edit History

 $Header: //source/qcom/qct/modem/uim/mmgsdi/rel/07H1_2/src/gsdidiag_gstk.c#4 $ $DateTime: 2009/05/05 11:19:21 $ $Author: jsharma $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/05/09   js      Featurized gstkdiag_end_session_event()
04/29/09   js      Added prototype for static functions
02/13/09   yb      Removed dependency of feature FEATURE_GSTK_TEST_AUTOMATION
                   on multiclient support and streaming APDU support
11/13/08   xz      1) Fix issue of unable to dereg via DIAG
                   2) Fix issue of unable to do TP DL via DIAG
10/24/08   xz      Lint Fix
10/07/08   xz      Added more debug messages
09/29/08   xz      Added support of passing GSTK state via DIAG port
09/02/08   xz      Added support of gstk_client_reg_ext()
                   Set GSTK to GSTK_START_S when restart
08/11/08   tml     Added GSTK diag support
===========================================================================*/

#include "intconv.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_GSTK_TEST_AUTOMATION
#error code not present
#endif /* FEATURE_GSTK_TEST_AUTOMATION */
