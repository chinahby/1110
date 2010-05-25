#ifndef _TCACHE_H_
#define _TCACHE_H_

#ifdef __cplusplus
extern "C" {
#endif

#if F_SEEKCACHE
void _f_setcache(SLOT *insert_slot, SLOT slots[], unsigned int length);
int _f_getcache(SLOT *query_slot, SLOT slots[], unsigned int length);
void _f_initcache(SLOT slots[], int length);
#endif

/****************************************************************************
 *
 * end of tcache.h
 *
 ***************************************************************************/
#ifdef __cplusplus
}
#endif

#endif /*_TCACHE_H_*/

