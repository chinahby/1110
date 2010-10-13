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

#include <jos.h>

#define HAVE_EXPAT_CONFIG_H

#define jsize_t    juint32_t
#define memset    j_memset
#define memcpy    j_memcpy
#define memcmp    j_memcmp
#define malloc    expat_malloc
#define free      expat_free
#define realloc   expat_realloc
#define memmove   expat_memmove

#define assert(a) KASSERT(a, ("Assertion in %s %d\n", __FILE__, __LINE__))

#define j_offsetof(var, member) \
    ((juint8_t *)&((var).member) - (juint8_t *)&(var)) 

void *expat_malloc(jsize_t size);
void  expat_free(void *ptr);
void *expat_realloc(void *ptr, jsize_t size);
void *expat_memmove(void *out, void *in, juint32_t n);
