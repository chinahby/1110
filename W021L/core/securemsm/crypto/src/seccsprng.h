/* Use stream cipher NLS as a Pseudo-Random Number Generator (PRNG)
 * Copyright C 2005, Qualcomm Inc. 
 * Created on 05/26/2005 by lxiao
 */
#ifndef _NLS_CSPRNG_DEFINED
#define _NLS_CSPRNG_DEFINED

#ifdef errno
#undef errno
#endif

#include "comdef.h"
#include "secnls.h"	/* type definition */
#include "secerrno.h"	/* error message definition */

/* seed/reseed function */
secerrno_enum_type csprng_seed(uint8 *seed, int seed_length);

/* random sequence output function */
secerrno_enum_type csprng_rand(uint8 *rand_out, int rand_length);

#endif

