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
#include <jos.h>

#define IS_NUM(x)       IS_VALID((x), '0', '9')
#define IS_HEX_UPPER(x) IS_VALID((x), 'A', 'F')
#define IS_HEX_LOWER(x) IS_VALID((x), 'a', 'f')
#define IS_WHITE(x)     ((x) == ' ' || (x) == '\t' || (x) == '\n')
#define IS_VALID(x, start, end) ((x) >= (start) && (x) <= (end))

#define IS_STR_OPEN(x)  ((x) == '[')
#define IS_STR_CLOSE(x) ((x) == ']')
#define IS_PARAM_SEP(x) ((x) == '=')
#define IS_END(x)       ((x) == '\0')

#define IS_SPECIAL(x)   (IS_STR_CLOSE(x) || IS_STR_OPEN(x) || \
    IS_PARAM_SEP(x) || IS_END(x))

#define IS_WORD_STR(x)  (!IS_SPECIAL(x) && !IS_WHITE(x))
#define IS_SENTENCE_STR(x)      \
    (!IS_END(x) && !IS_STR_OPEN(x) && !IS_STR_CLOSE(x))

/* Skip all white space */
jint32_t skip_white(char *str, jint32_t i)
{
    while (IS_WHITE(str[i]))
        i++;

    return i;
}

jresult_t match_composite_string(char *str, jint32_t *i, jint32_t *start, jint32_t *end);

jresult_t match_sentence(char *str, jint32_t *i, jint32_t *start, jint32_t *end)
{
    jresult_t rc;
    jint32_t temp;

    DBG_X(DTEST_PARAM, ("Sentance %ld [%s]\n", *i, str));
    *start = *i;
   
    /* Find all chars that are not "special" */
    while (str[*i])
    {
        /* Normal char ? Just skip on */
        if (IS_SENTENCE_STR(str[*i]))
            (*i)++;

        /* Composite string ? Parse it */
        else if (IS_STR_OPEN(str[*i]))
        {
            rc = match_composite_string(str, i, &temp, &temp); 
            if (rc)
                return rc;
        }
        /* Something else ? Finish out */
        else
            break;
    }

    DBG_X(DTEST_PARAM, ("Sentance done [%ld:%ld]\n", *start, *i));

    *end = *i;
    return 0;
}

jresult_t match_composite_string(char *str, jint32_t *i, jint32_t *start, jint32_t *end)
{
    jresult_t rc;
    jint32_t temp;
    
    DBG_X(DTEST_PARAM, ("Composite %ld [%s]\n", *i, str));

    /* Match opening string braket */
    if (!IS_STR_OPEN(str[*i]))
    {
        DBG_W(DTEST_PARAM, ("Match error, expected opening string braket at %ld\n",
            *i));
        return JEINVAL;
    }

    /* Advance pointer and set start */
    (*i)++;
    *start = *i;

    /* Find embedded sentance */
    rc = match_sentence(str, i, &temp, &temp);
    if (rc)
        return rc;

    /* Match closing braket */
    if (!IS_STR_CLOSE(str[*i]))
    {
        DBG_W(DTEST_PARAM, ("Match error, expected closing string braket at %ld\n",
            *i));
        return JEINVAL;
    }

    /* Set end and advance pointer beyond the braket */
    *end = *i;
    (*i)++;

    DBG_X(DTEST_PARAM, ("Composite done [%ld:%ld]\n", *start, *end));
    return 0;
}

jresult_t match_word(char *str, jint32_t *i, jint32_t *start, jint32_t *end)
{
    *start = *i;

    while (IS_WORD_STR(str[*i]))
        (*i)++;

    if (*start == *i)
    {
        DBG_W(DTEST_PARAM, ("Match error, expected to find string at %ld\n", *i));
        return JEINVAL;
    }

    *end = *i;
    return 0;
}

/* Find a single string */
jresult_t match_string(char *str, jint32_t *i, jint32_t *start, jint32_t *end)
{
    jresult_t rc;
    
    *i = skip_white(str, *i);
   
    if (IS_STR_OPEN(str[*i]))
        rc = match_composite_string(str, i, start, end);
    else
        rc = match_word(str, i, start, end);

    if (rc)
        return rc;
 
    DBG_X(DTEST_PARAM, ("Found word [%s] size %ld [%ld:%ld]\n",
        str + *start, *end - *start, *start, *end));

    return 0;
}

jresult_t match_equal(char *str, jint32_t *i)
{
    /* Skip till the '=' */
    *i = skip_white(str, *i);

    /* We expect a '=' mark now */
    if (str[*i] != '=')
    {
        DBG_W(DTEST_PARAM, ("Match error, expected to find separator at %ld\n", *i));
        return JEINVAL;
    }
    
    /* We got '=', all is good */
    (*i)++;
    return 0;
}

jresult_t param_get_val(char *params, char *name, char *ret, jint32_t max_chars)
{
    jresult_t rc;
    jint32_t start, end;
    jint32_t i = 0;
    jint32_t found = 0;
    
    DBG_V(DTEST_PARAM, ("Get val [%s] from [%s]\n", name, params));

    if (!params || !name || !ret)
        return JEINVAL;

    /* In case we get empty param list */
    i = skip_white(params, i);

    for (i = 0; params[i];)
    {
        /* Find the KEY */
        rc = match_string(params, &i, &start, &end);
        if (rc)
            return rc;

        /* Is it what we want ? */
        if (j_strncmp(params + start, name, end - start) == 0)
            found = 1;

        /* Find the '=' mark */
        rc = match_equal(params, &i);
        if (rc)
            return rc;

        /* Find the Value */
        rc = match_string(params, &i, &start, &end);
        if (rc)
            return rc;

        /* If the value is for a key we need, return it */
        if (found)
        {
            os_strlcpy(ret, params + start, MIN(end - start, max_chars) + 1);

            DBG_V(DTEST_PARAM, ("[%s] = [%s]\n", name, ret));
            return 0;
        }

        /* Advance the pointer so we catch the terminating NULL */
        i = skip_white(params, i);
    }
    return JENOENT;
}

jresult_t param_get_uint(char *str, jint32_t *val)
{
    jint32_t i;
    *val = 0;
    
    for (i = 0; str[i]; i++)
    {
        if (IS_NUM(str[i]))
            *val = *val * 10 + str[i] - '0';
        else
            return JEINVAL;
    }
    return 0;
}

jresult_t param_get_hex(char *str, jint32_t *val)
{
    jint32_t i;
    *val = 0;
    
    for (i = 0; str[i]; i++)
    {
        if (IS_NUM(str[i]))
            *val = *val * 16 + str[i] - '0';
        else if (IS_HEX_UPPER(str[i]))
            *val = *val * 16 + (str[i] - 'A' + 10);
        else if (IS_HEX_LOWER(str[i]))
            *val = *val * 16 + (str[i] - 'a' + 10);
        else
            return JEINVAL;
    }
    return 0;
}

jresult_t param_get_int(char *str, jint32_t *val)
{
    jint32_t sign = 1;
    jresult_t rc;

    DBG_X(DTEST_PARAM, ("Get int [%s]\n", str));

    if (!str || !val) 
        return JEINVAL;

    /* Check if negative */
    if (str[0] == '-')
    {
        sign = -1;
        str++;
    }

    if (!str[0])
        return JEINVAL;

    if (str[0] == '0' && str[1] == 'x')
        rc = param_get_hex(str + 2, val);
    else
        rc = param_get_uint(str, val);

    *val *= sign;

    return rc;
}
            
