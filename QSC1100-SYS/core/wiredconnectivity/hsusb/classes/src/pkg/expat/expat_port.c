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

#include "expat_port.h"

void *expat_malloc(jsize_t size)
{
    void *p;
    
    p = jmalloc(size + sizeof(juint32_t), 0);
    if (!p)
        return NULL;

    j_memcpy(p, &size, sizeof(juint32_t));    

    return (juint8_t *)p + sizeof(juint32_t);
}

void expat_free(void *ptr)
{
    if (!ptr)
        return;

    jfree((juint8_t *)ptr - sizeof(juint32_t));
}

void *expat_realloc(void *ptr, jsize_t size)
{
    void *new;
    juint32_t old_size;

    if (!ptr)
        return expat_malloc(size);

    if (!size)
    {
        expat_free(ptr);
        return NULL;
    }

    new = expat_malloc(size);
    if (!new)
        return NULL;

    j_memcpy(&old_size, (juint8_t *)ptr - sizeof(juint32_t), sizeof(juint32_t));
    
    j_memcpy(new, ptr, MIN(size, old_size));

    expat_free(ptr);

    return new;
}

void *memmove(void *out, void *in, juint32_t n)
{
    juint32_t i;
    
    if (out < in)
    {
        for (i = 0; i < n; i++)
            ((juint8_t *) out)[i] = ((juint8_t *) in)[i];
    }
    else
    {
        for (i = n; i; i--)
            ((juint8_t *) out)[i-1] = ((juint8_t *) in)[i-1];
    }
    return out;
}

