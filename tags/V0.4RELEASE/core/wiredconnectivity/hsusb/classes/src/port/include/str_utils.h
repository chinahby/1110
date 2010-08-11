#ifndef _STR_UTILS_H_
#define _STR_UTILS_H_

#include <jtypes.h>

#define NUMERIC_VALUE_INVALID   0xFFFFFFFF
juint32_t j_snscanf(char *buf, char **end_ptr, char *format, ...);

#endif/*_STR_UTILS_H_*/

