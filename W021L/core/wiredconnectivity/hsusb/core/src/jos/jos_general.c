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

/* Jungo Confidential, Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com */
#define INCLUDE_GENERAL
#include "jos_internal.h"

#ifdef JDEBUG
typedef struct perf_t {
    jtime_t t;
    char *file;
    jint32_t line;
} perf_t;

#define PERF_TABLE_SIZE 100
static perf_t perf_times[PERF_TABLE_SIZE];
static jint32_t perf_i = 0;
jbool_t jstart_perf;

void jmark_time_v(char *file, jint32_t line)
{
    if (!jstart_perf || perf_i>=PERF_TABLE_SIZE)
        return;
    jget_time(&perf_times[perf_i].t);
    perf_times[perf_i].file = file;
    perf_times[perf_i].line = line;
    perf_i++;
}

long time_diff(jtime_t *end_time, jtime_t *start_time)
{
    return (end_time->tv_usec - start_time->tv_usec) / 1000 + 
        (end_time->tv_sec - start_time->tv_sec) * 1000;
}

void jdump_time()
{
    jint32_t i;
    jprintf("dumping times:\n");
    for (i=0; i<perf_i; i++)
    {
        jprintf("%2ld: %3ld ms. %s:%ld\n", i,
            time_diff(&perf_times[i].t, &perf_times[0].t),
            perf_times[i].file, perf_times[i].line);
    }
    perf_i = 0;
}

#ifdef DEBUG_WITH_TIMERS
long dbg_get_time_diff()
{
    static juint8_t      timer_initialized = 0;
    static os_time_t    timer;
    os_time_t           temp_timer;
    long rc;

    if (!timer_initialized)
    {
        timer_initialized = 1;
        os_get_time(&timer);
    }

    os_get_time(&temp_timer);
    rc = time_diff(&temp_timer, &timer);
    jos_memcpy(&timer, &temp_timer, sizeof(timer));
    return rc;
}
#endif
#endif

juint32_t j_arc4random(void)
{
    jtime_t x;
    jget_time(&x);
    return (x.tv_sec ^ x.tv_usec);
}

juint16_t __bswap16(juint16_t value)
{
    return (
        (value  >> 8) |
        (value) << 8);
}

juint32_t __bswap32(juint32_t value)
{
    return (
        (value  >> 24) |
        (value  >>  8 & 0x0000ff00) |
        (value  <<  8 & 0x00ff0000) |
        (value) << 24);
}

/*
 *   JOS String Manipulation functions
 */

char *j_strdup(const char *str)
{
    juint32_t len;
    char *dup;

    len = j_strlen(str)+1;
    dup = jmalloc(len, 0);
    if (!dup)
        return NULL;

    j_memcpy(dup, str, len);
    return dup;
}

/*
 *   Find the existence and location of a substring whithin an input string
 *   @substr - substring to find
 *   @instr - input string to test
 */
char *j_strstr(char *instr, char *substr)
{
    char *intmp, *subtmp;

    while (*instr)
    {
        if (*instr == *substr)
        {
            intmp = instr+1; subtmp = substr+1;
            while (*intmp && *subtmp && (*intmp == *subtmp))
            {
                intmp++;
                subtmp++;
            }

            if (*subtmp == 0)
                return instr;
            if (*intmp == 0)
                return NULL;
        }

        instr++;
    }

    return NULL;
}

/*
 *   Converts a numeric value to a "numeric string" given the appropriate
 *   representation @base.
 *   
 *   @min_digits pads the str with as many '0' required to provide a minimal
 *   amount of digits in an output string
 *
 *   @max_digits will return a string with at most max_digits digits. This can
 *   and should be used to limit the output to the given 'outstr' buffer size
 *
 *   @num - input: numeric value to convert
 *   
 *   Output:
 *      numeric value on success, or
 *      NUMERIC_VALUE_INVALID on failure (when input is not a number)
 */
char *j_numtostr(char *outstr, juint32_t base, juint32_t min_digits,
    juint32_t max_digits, juint32_t num)
{
    juint32_t i = 0, j = 0, val;
    char str[20];

    while (num)
    {
        val = (num % base);
        num /= base;

        if (val > 9)
            str[j] = val+'A'-10;
        else
            str[j] = val+'0';

        j++;
    }

    /* pad for minimal amount of digits */
    while (j < min_digits--)
        outstr[i++] = '0';

    /* reverse string up to max digits requested */
    while (j && max_digits--)
    {
        outstr[i] = str[j-1];
        i++; j--;
    }

    outstr[i] = 0;
    return outstr;
}

/* Internal function used by j_strtonum */
static juint32_t numeric_value(char c, juint32_t base)
{
    juint32_t val = NUMERIC_VALUE_INVALID;

    if (c >= '0' && c <= '9')
        val = c - '0';
    else if (c >= 'A' && c <= 'F')
        val = c - 'A' + 10;
    else if (c >= 'a' && c <= 'f')
        val = c - 'a' + 10;

    if (val >= base)
        return NUMERIC_VALUE_INVALID;

    return val;
}

/*
 *   Converts a "numeric string" to a numeric value given its @base.
 *   
 *   If @last_ptr is not NULL, it returns a pointer to the character in which 
 *   the conversion ended (i.e. one past the last recognized numeral)
 *
 *   Output:
 *      numeric value on success, or
 *      NUMERIC_VALUE_INVALID on failure (when input is not a number)
 */
juint32_t j_strtonum(char *instr, juint32_t base, char **last_ptr)
{
    char *ptr = instr;
    juint32_t value = 0;
    juint32_t cvalue;

    cvalue = numeric_value(*ptr, base);
    while (*ptr && cvalue!=NUMERIC_VALUE_INVALID)
    {
        value *= base;
        value += cvalue;
        ptr++;
        cvalue = numeric_value(*ptr, base);
    }

    if (last_ptr)
        *last_ptr = ptr;

    if (ptr == instr)
        return NUMERIC_VALUE_INVALID;

    return value;
}

/* Tests whether a given character is a white space */
jbool_t j_is_space(char c)
{
    if (c == ' ' ||
        c == '\t' ||
        c == '\r' ||
        c == '\n')
    {
        return 1;
    }

    return 0;
}

