/* =======================================================================
                               oscl_string_utils.cpp
DESCRIPTION
  Meaningful description of the definitions contained in this file.
  Description must specify if the module is portable specific, mobile
  specific, or common to both, and it should alert the reader if the
  module contains any conditional definitions which tailors the module to
  different targets.  Include any initialization and synchronizing
  requirements.

EXTERNALIZED FUNCTIONS
  List functions and a brief description that are exported from this file

INITIALIZATION AND SEQUENCING REQUIREMENTS
  Detail how to initialize and use this service.  The sequencing aspect
  is only needed if the order of operations is important.

Portions copyrighted by PacketVideo Corporation;
Copyright 1998, 2002, 2003 PacketVideo Corporation, All Rights Reserved; and
Portions copyrighted by QUALCOMM Incorporated;
Copyright 2003 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */

/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/utils/oscl/main/latest/src/oscl_string_utils.cpp#9 $
$DateTime: 2010/01/04 23:14:42 $
$Change: 1127650 $


========================================================================== */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */
#include <stdio.h>
#include <stdlib.h>       
#include <ctype.h>
#include <string.h>
#include <math.h>

/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"

#include "oscl_string_utils.h"

/* ==========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

========================================================================== */

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
const int base64tab[256]=
{
    64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,
    64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,62,64,64,64,63,
    52,53,54,55,56,57,58,59,60,61,64,64,64,64,64,64,64,0,1,2,3,4,5,6,7,8,9,
    10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,64,64,64,64,64,64,26,27,
    28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,
    64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,
    64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,
    64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,
    64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,
    64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,
    64,64,64,64,64,64,64,64,64,64,64,64,64
};

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Local Object Definitions
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Forward Declarations
** ----------------------------------------------------------------------- */

/* =======================================================================
**                            Function Definitions
** ======================================================================= */

/* ======================================================================
FUNCTION
  PV_atoi

DESCRIPTION
  Extracts an integer from the input string.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
bool PV_atoi(const char *buf,const char new_format, uint32& value)
{
  return PV_atoi(buf, new_format, strlen(buf), value);
}

/* ======================================================================
FUNCTION
  PV_atoi

DESCRIPTION
  Thorough, meaningful description of what this function does.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
bool PV_atoi(const char *buf,const char new_format, int length,
             uint32& value)
{
  uint32 old, temp;
  const char *ptr = buf;
  value = 0;
  switch ( new_format )
  {
  case 'x':{
      while ( ptr-buf < length )
      {
        if ( (*ptr == 'a')||(*ptr == 'A') )
          temp = 10;
        else if ( (*ptr == 'b')||(*ptr == 'B') )
          temp = 11;
        else if ( (*ptr == 'c')||(*ptr == 'C') )
          temp = 12;
        else if ( (*ptr == 'd')||(*ptr == 'D') )
          temp = 13;
        else if ( (*ptr == 'e')||(*ptr == 'E') )
          temp = 14;
        else if ( (*ptr == 'f')||(*ptr == 'F') )
          temp = 15;
        else if ( (*ptr >= 48) && (*ptr <= 57) )
          temp = (int)(*ptr-48);
        else return false;
        ++ptr;
        old = value;
        value = value*16 + temp;
        if ( old > value )
        {
          // overflow
          return false;
        }
      }
    }break;

    case 'd':{
      int numDigits = 0;
      while ( ptr-buf < length )
      {
        if ( (*ptr >= 48) && (*ptr <= 57) )
        {
          numDigits++;
          temp=(int)(*ptr-48);
          ++ptr;
          old = value;
          value = value*10 + temp;
          if ( old > value )
          {
            // overflow
            return false;
          }
        }
        else 
        {
          /* Ignore remaining input until next whitespace */
          while ( ptr-buf < length )
          {
            ++ptr;
          }
          if (numDigits)
          {
            return true;
          }
          else
          {
            return false;
          }
        }
      }
    }break;

  default :{
      while ( ptr-buf < length )
      {
        if ( (*ptr >= 48) && (*ptr <= 57) )
        {
          temp=(int)(*ptr-48);
          ++ptr;
          old = value;
          value = value*10 + temp;
          if ( old > value )
          {
            // overflow
            return false;
          }
        }
        else
        {
          return false;
        }
      }
    }break;
  }

  return true;
}

/* ======================================================================
FUNCTION
  skip_whitespace

DESCRIPTION
  Skips over any leading whitespace (i.e., a space or
  horizontal tab character) in the input string and
  returns the pointer to the first non-whitespace
  character.

DEPENDENCIES
  None

RETURN VALUE
  pointer to first non-whitespace character

SIDE EFFECTS
  None

========================================================================== */
const char* skip_whitespace(const char *ptr)
{
  while ( ptr && *ptr )
  {
    if ( *ptr != ' ' && *ptr != '\t' )
    {
      break;
    }

    ++ptr;
  }

  return ptr;
}

/* ======================================================================
FUNCTION
  skip_whitespace

DESCRIPTION
  Skips over any leading whitespace (i.e., a space or
  horizontal tab character) in the input string and
  returns the pointer to the first non-whitespace
  character.

DEPENDENCIES
  None

RETURN VALUE
  pointer to first non-whitespace character

SIDE EFFECTS
  None

========================================================================== */
char * skip_whitespace( char * ptr )
{
  while ( ptr && *ptr )
  {
    if ( *ptr != ' ' && *ptr != '\t' )
    {
      break;
    }

    ++ptr;
  }

  return ptr;
}

/* ======================================================================
FUNCTION
  skip_whitespace

DESCRIPTION
  Skips over any leading whitespace (i.e., a space or
  horizontal tab character) in the input string and
  returns the pointer to the first non-whitespace
  character.  The input string is represented by
  starting and ending pointers and does not need to be
  NULL terminated.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  pointer to first non-whitespace character

SIDE EFFECTS
  None.

========================================================================== */
const char* skip_whitespace(const char *start, const char *end)
{
  while ( start && (start < end) )
  {
    if ( *start != ' ' && *start != '\t' )
    {
      break;
    }

    ++start;
  }

  return start;
}

/* ======================================================================
FUNCTION
  skip_to_whitespace

DESCRIPTION
  Skips over any leading non-whitespace in the input string and
  returns the pointer to the first whitespace character.  The input string 
  is represented by starting and ending pointers and does not need to be
  NULL terminated.

DEPENDENCIES
  None

RETURN VALUE
  pointer to first whitespace character

SIDE EFFECTS
  None

========================================================================== */
const char* skip_to_whitespace(const char *start, const char *end)
{
  while ( start && (start < end) )
  {
    if ( oscl_iswhite(*start) )
    {
      break;
    }
    ++start;
  }

  return start;
}

/* ======================================================================
FUNCTION
  skip_to_char

DESCRIPTION
  Skips over all the characters in the input string until it finds the
  first occurrance of specified char 'c'. The input string is represented
  by starting and ending pointers and need not be NULL terminated.

DEPENDENCIES
  None

RETURN VALUE
  Returns pointer to 'c', if 'c' was found and the pointer after end,
  if 'c' was not found.

SIDE EFFECTS
  None

========================================================================== */
const char* skip_to_char(const char *start, const char *end, char c)
{
  while (start && (start <= end))
  {

    if (*start == c)
      break;

    ++start;
  }

  return start;
}

/* ======================================================================
FUNCTION
  skip_to_line_term

DESCRIPTION
  Skips over any characters to the next line terminator
  (i.e., \r and \n) and
  returns the pointer to the line term character.
  The input string is represented by
  starting and ending pointers and does not need to be
  NULL terminated.

DEPENDENCIES
  None

RETURN VALUE
  pointer to line terminator character

SIDE EFFECTS
  None

========================================================================== */
const char * skip_to_line_term(const char *start, const char *end)
{
  while ( start && (start < end) )
  {
    if ( *start == '\r' || *start == '\n' )
    {
      return start;
    }
    ++start;
  }

  return start;
}

/* ======================================================================
FUNCTION
  skip_whitespace_and_line_term

DESCRIPTION
  Skips over any leading whitespace (i.e., a space or
  horizontal tab character) or line terminator (i.e., \r
  and \n) and
  returns the pointer to the first non-whitespace
  character.  The input string is represented by
  starting and ending pointers and does not need to be
  NULL terminated.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  pointer to first non-whitespace character

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
const char* skip_whitespace_and_line_term(const char *start, const char *end)
{
  while ( start && (start < end) )
  {
    if ( !oscl_iswhite(*start) )
    {
      break;
    }

    ++start;
  }

  return start;
}

/* ======================================================================
FUNCTION
  extract_string

DESCRIPTION
  Extracts string of a maximum size after skipping any
  leading whitespace.  The input string is represented by
  starting and ending pointers and does not need to be
  NULL terminated.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  length of the extracted string

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
int extract_string(const char * start, const char *end, char *outstring, int maxsize)
{
  int len = 0;

  if ( ! outstring )
  {
    return 0;
  }

  start = skip_whitespace(start, end);

  for ( ; start && (start < end) ; ++start )
  {

    if (oscl_iswhite(*start))
    {
      // whitespace so stop copying
      break;
    }

    if ( len < maxsize )
    {
      *outstring++ = *start;
    }
    else if ( len == maxsize )
    {
      // too long so just terminate the string
      *(outstring-1) = '\0';
    }
    ++len;

  }

  if ( len < maxsize )
  {
    // terminate the string
    *outstring = '\0';
  }

  return len;
}

/* ======================================================================
FUNCTION
  extract_string

DESCRIPTION
  Extracts string of a maximum size after skipping any
  leading whitespace.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  length of the extracted string

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
int extract_string(const char * in_ptr, char *outstring, int maxsize)
{
  int len = 0;

  if ( ! outstring )
  {
    return 0;
  }

  in_ptr = skip_whitespace(in_ptr, in_ptr+strlen(in_ptr));

  for ( ; in_ptr && *in_ptr ; ++in_ptr )
  {

    if ( *in_ptr == ' ' || *in_ptr == '\t' || *in_ptr == '\n' || *in_ptr == '\r' )
    {
      // whitespace so stop copying
      break;
    }

    if ( len < maxsize )
    {
      *outstring++ = *in_ptr;
    }
    else if ( len == maxsize )
    {
      // too long so just terminate the string
      *(outstring-1) = '\0';
    }
    ++len;

  }

  if ( len < maxsize )
  {
    // terminate the string
    *outstring = '\0';
  }

  return len;
}


/* ======================================================================
FUNCTION
  strip_control_chars

DESCRIPTION
  The function is passed a standard null-terminated string. It scans the
  string for any chars <0x20 (space), and removes them from the string.
  This operation uses nested loops and hence should not be used for
  time critical operations.
  
DEPENDENCIES
  None

RETURN VALUE
  The new length of the string

SIDE EFFECTS
  Passed string is possibly modified

========================================================================== */
int strip_control_chars(char *str, int length)
{
  // look for degenerate case
  if ((length <= 0) || (str == NULL) )
  {
    return 0;
  }

  int len = length;

   //perform the strip
   int i = 0;
   while ((i < len) && (len > 0))
   {
     if (str[i] <= ' ')
     {
        for (int j = i; j < (len - 1); j++)
        {
          str[j] = str[j + 1];
        }
        len--;
     }
     else
     {
       i++;
     }
   }
   str[len] = '\0';

   return len;
}

/* ======================================================================
FUNCTION:
  stripTrailingSlashFromURL

DESCRIPTION:
  If the URL has an extraneous slash at the end, strip it.

INPUT/OUTPUT PARAMETERS:
  url - The URL to modify

RETURN VALUE:
  The input URL.

SIDE EFFECTS:
  None.
======================================================================*/
char *stripTrailingSlashFromURL(char *url)
{
    if ((strincmp(url, "rtsp://", strlen("rtsp://")) != 0) ||
        (strincmp(url, "rtspt://", strlen("rtspt://")) != 0))
  {
    return url;
  }

  int i = -1;
  int slashes = 0;
  int prevSlashIndex = -1;
  int protoLength = 0;

  if(strincmp(url,"rtsp://", strlen("rtsp://") ) == 0)
    protoLength = strlen("rtsp://");
  else if (strincmp(url,"rtspt://", strlen("rtspt://") ) == 0)
    protoLength = strlen("rtspt://");
  
  for (i = protoLength; url[i] != '\0'; i++)
  {
    if (url[i] == '/')
    {
      prevSlashIndex = i;
      slashes++;
    }
  }

  // i is now the length of url
  if ((slashes > 1) && (prevSlashIndex == i - 1))
  {
    url[prevSlashIndex] = '\0';
  }

  return url;
}

/* ======================================================================
FUNCTION:
  getFilenameFromURL

DESCRIPTION:
  Return the filename part (including the path) of the URL. The
  returned string points to within the argument url, starting from a
  / character. If the argument is not a URL, or does not contain a
  slash after ://, return NULL.

INPUT/OUTPUT PARAMETERS:
  url - The URL to search.

RETURN VALUE:
  See description. 

SIDE EFFECTS:
  None.
======================================================================*/
const char *getFilenameFromURL(const char *url)
{
  const char *temp = strstr(url, "://");

  if (temp == NULL)
  {
    return NULL;
  }

  return strstr(temp + 3, "/");
}

/* ======================================================================
FUNCTION
  retrieveValue

DESCRIPTION
 Skip white space from line_start_ptr and then convert following string to
 integer, then skip white space until line_end_ptr.

DEPENDENCIES
 None

RETURN VALUE
 bool - whether conversion succeedes.

SIDE EFFECTS
 temp_ptr returns the first non-whitespace character after conversion.

========================================================================== */
bool retrieveValue(const char* line_start_ptr, 
                   const char* line_end_ptr,
                   const char*& temp_ptr,
                   int &value, 
                   const char format)
{
   temp_ptr = skip_whitespace( line_start_ptr, line_end_ptr );
   if (temp_ptr >= line_end_ptr)
   {
      // empty line 
      return false;
   }
   // retrieve value 
   char *end_ptr = NULL;
   value = (int)strtol( temp_ptr, &end_ptr, format );
   temp_ptr = skip_whitespace( end_ptr, line_end_ptr );
   return true;
}

/* ======================================================================
FUNCTION
  retrieveDouble

DESCRIPTION
 Skip white space from line_start_ptr and then convert following string to
 double, then skip white space until line_end_ptr.

DEPENDENCIES
 None

RETURN VALUE
 bool - whether conversion succeeded.

SIDE EFFECTS
 temp_ptr returns the first non-whitespace character after conversion.

========================================================================== */
bool retrieveDouble(const char* line_start_ptr, 
                    const char* line_end_ptr,
                    const char*& temp_ptr,
                    double &value)
{
   temp_ptr = skip_whitespace( line_start_ptr, line_end_ptr );
   if (temp_ptr >= line_end_ptr)
   {
      // empty line 
      return false;
   }
   // retrieve value 
   char *end_ptr = NULL;
   value = strtod( temp_ptr, &end_ptr );

   if ((value == HUGE_VAL) || (value == -HUGE_VAL))
   {
     temp_ptr = line_start_ptr;
     return false;
   }

   temp_ptr = skip_whitespace( end_ptr, line_end_ptr );
   return true;
}


/* ======================================================================
FUNCTION
  strincmp

DESCRIPTION
  case insensitive compare

DEPENDENCIES
 None

RETURN VALUE
 int
========================================================================== */
int strincmp(const char* str1, const char* str2, int size)
{
   int rsl = 0, len = 0;
   while ((len < size) && (rsl == 0))
   {
     char c = str1[len];
     char d = str2[len];

     // Don't go past the null character in either string.
     if (c == '\0') 
     {
       if (d == '\0')
       {
         return 0;
       }
       return -1;
     }
     else if (d == '\0')
     {
       return 1;
     }

     rsl = tolower(c) - tolower(d);
     len++;
   }
   return rsl;
}

/* ======================================================================
FUNCTION
  strncpy_upper

DESCRIPTION
  Copy at most n characters from src to dest. No null characters are 
  implicitly copied. If n is greater than the length of src, null
  characters are appended until the length of src is reached. Copy
  the upper-case version of any characters in src.

DEPENDENCIES
  None

RETURN VALUE
  dest

SIDE EFFECTS
  None

========================================================================== */
char *strncpy_upper(char *dest, const char *src, int n)
{
   bool reached_end = false;

   for (int i = 0; i < n; i++)
   {
      if (!reached_end) 
      {
         reached_end = (src[i] == '\0');
      }

      if (reached_end)
      {
         dest[i] = '\0';
      }
      else
      {
         char c = src[i];     

         if ((c >= 'a') && (c <= 'z'))
         {
            c = c - 'a' + 'A';
         }

         dest[i] = c;
      }
   }
   return dest;
}

/* ======================================================================
FUNCTION
  strnstr

DESCRIPTION
  Search for the first occurrance of str_to_find in src, which must occur
  within the first n - strlen(str_to_find) in src. 

DEPENDENCIES
  None

RETURN VALUE
  If found, return a pointer to the location in src. Otherwise, return
  NULL.

SIDE EFFECTS
  None

========================================================================== */
const char *oscl_strnstr(const char *src, const char *str_to_find, int n)
{
   if ((src == NULL) || (str_to_find == NULL) || (n < 1))
   {
      return NULL;
   }

   int find_length = strlen(str_to_find);

   // If we're looking for the null terminator ...
   if (find_length == 0)
   {
      return src + strlen(src);
   }

   for (int i = 0; i <= n - find_length; i++)
   {
      if (src[i] == '\0')
      {
         return NULL;
      }

      for (int j = 0; j < find_length; j++)
      {         
         char c = src[i + j];

         if (c == '\0')
         {
             // rest of src is shorter than str_to_find
             return NULL;
         }

         if (c != str_to_find[j])
         {
            goto SearchNextIndex;
         }
      }

      // Hooray, we have a match!
      return src + i;

SearchNextIndex:
      ;
   }
   return NULL;
}

/* ======================================================================
FUNCTION
  strncpy_word

DESCRIPTION
  Copy up to n characters of src to dest, stopping at whitespace or null
  characters in src. Terminate dest with a null character. If nchars_copied
  is not NULL, set it to the number of characters copied, including the null
  character. This may write up to n + 1 characters in dest.

DEPENDENCIES
  None

RETURN VALUE
  dest

SIDE EFFECTS
  None

========================================================================== */
char *strncpy_word(char *dest, const char *src, int n, int *nchars_copied)
{
   bool done_copy = false;
   int i = 0;
         
   if (n < 0)
   {
      if (NULL != nchars_copied)
      {
         *nchars_copied = 0;
      }
      return dest;
   }
         
   for (i = 0; !done_copy; i++)
   {
      if (i >= n)
      {         
         done_copy = true;
      }
      else
      {
         char d = src[i];
         switch (d)
         {
            case '\0':
            case '\n': case '\r':
            case ' ': case '\t': case '\f':             
            done_copy = true;
            break;

            default:
            dest[i] = src[i];
            break;
         }
      }
   }

   dest[i - 1] = '\0';

   if (NULL != nchars_copied)
   {
      *nchars_copied = i;
   }

   return dest;
}

#ifndef PLATFORM_LTK
int strcmpi(const char *s1, const char *s2)
{
  int i = 0, len1 = 0, len2 = 0, retval = 0;
  char c, d;

  len1 = strlen(s1); 
  len2 = strlen(s2);

  if((s1[len1] != '\0') || (s2[len2] != '\0'))
  {
    retval = -2;
    return retval;
  }

  do {
    c = tolower(s1[i]);
    d = tolower(s2[i]);

    if (c == '\0')
    {
      if (d == '\0')
      {
        retval = 0;
        break;
      }
      retval = 1;
      break;
    }
    else if (d == '\0')
    {
      retval = -1;
      break;
    }
    else if (c < d)
    {
      retval = -1;
      break;
    } 
    else if (c > d)
    {
      retval = 1;
      break;
    }

    i++;
  } while ((c != '\0') && (d != '\0'));

  return retval;
}
#endif /* PLATFORM_LTK */
    
bool oscl_iswhite(char c)
{
  switch (c) 
  {
    case ' ':
    case '\t':
    case '\n':
    case '\r':
    return true;

    default:
    return false;
  }
}

/* ======================================================================
FUNCTION:
  expect

DESCRIPTION:
  Returns true iff a C-string begins with another string (the expecatation). 
  Optionally skips whitespace before and after the expectation.
  If the expectation is found, the string pointer is updated to just after the 
  occurrance of the expecation, and any trailing whitespace if specified.

INPUT/OUTPUT PARAMETERS:
  expectation The string that must match, must be null-terminated.
  sptr A pointer to a C-string. The value of the pointer is updated to after the
  expectation if found.
  eptr - The end boundary for the C-string. If the match doesn't fit before the
  end boundary, this function returns false.
  skipBeginningWhitespace - If true, initial whitespace will be skipped before
  expecting the match.
  skipEndingWhitespace - If true, initial whitespace will be skipped after
  expecting the match.

RETURN VALUE:
  Returns true iff the expectation is found immediately (after skipping any
  beginning whitespace if specified), and before the end boundary.
  
SIDE EFFECTS:
  The value of sptr is updated if the match succeeds (see above).
======================================================================*/
bool expect(const char *expectation, const char **sptr, const char *eptr,
            bool skipBeginningWhitespace, bool skipEndingWhitespace) 
{
   const char *ptr = *sptr;

   if (skipBeginningWhitespace) 
   {
      while ((ptr < eptr) && isspace(*ptr)) 
      {
         ptr++;
      }
   }

   int charsLeft = eptr - ptr;
   int charsToExpect = strlen(expectation);
    
   // If we don't have enough chars to match, fail.
   if (charsLeft < charsToExpect) 
   {
      return false;
   }

   // Match the expecation now, but don't match the empty string.
   if ((charsLeft > 0) && (strncmp(ptr, expectation, charsToExpect) != 0)) 
   {
      return false;         
   }

   // We matched, so skip the expectation.
   ptr += charsToExpect;

   if (skipEndingWhitespace) 
   {
      while ((ptr < eptr) && isspace(*ptr)) 
      {
         ptr++;
      }      
   }

   // Update start pointer
   *sptr = ptr;
   return true;
}

/* ======================================================================
FUNCTION:
  parseDelimitedList

DESCRIPTION:
  Given a string which is a bunch of strings separated by a separator character
  and optional whitespace, call the callback function for each element string.

INPUT/OUTPUT PARAMETERS:  
  sptr - A string pointing to the beginning of the list.
  eptr - A string pointing to the end of the list. If NULL, it will the 
  location of the first null character after sptr.
  separator - The character that separates the elements
  handleElementCallback - The callback function that is called for each element.
  cbData - Data given to the callback function.

RETURN VALUE:
  None
  
SIDE EFFECTS:
  None
======================================================================*/
void parseDelimitedList(const char *sptr, const char *eptr, char separator, 
  bool (*handleElementCallback)(const char *start, const char *end, void *cbData),
  void *cbData)
{
  // Figure out eptr if NULL.
  if (eptr == NULL)
  {
    eptr = sptr + strlen(sptr);
  }

  while (sptr < eptr)
  {
    // Location of next comma or end of string
    const char *delim = sptr;
    
    while (delim < eptr)
    {
        if (*delim == separator)
        {
          break;
        }
        delim++;
    }

    // Stop parsing if the callback returns true.
    if ((*handleElementCallback)(sptr, delim, cbData))
    {
      return;
    }

    sptr = skip_whitespace(delim + 1, eptr);
  }
}

int base64_decode( const char * CodedData, int TotalData, unsigned char * DecodedData )
{
    int BytesDecoded, BytesEncoded;
    const char *InData = CodedData;
    unsigned char *OutData;
    
    if(DecodedData == 0L)
    {
        return ((TotalData+3)/4) * 3;
    }

    InData = CodedData;
    while(base64tab[*(InData++)] <= 63);
    BytesEncoded = (int)(InData - CodedData - 1);
    if (BytesEncoded < TotalData)
    {
      TotalData = BytesEncoded;
    }
    BytesDecoded = ((TotalData+3)/4) * 3;

    InData = CodedData;
    OutData = DecodedData;

    while (TotalData > 0)
    {
        *(OutData++) =
            (unsigned char) (base64tab[*InData] << 2 | base64tab[InData[1]] >> 4);
        if (base64tab[InData[2]] <= 63)
        {
          *(OutData++) =
              (unsigned char) (base64tab[InData[1]] << 4 | base64tab[InData[2]] >> 2);
        }
        if (base64tab[InData[3]] <= 63)
        {
          *(OutData++) =
              (unsigned char) (base64tab[InData[2]] << 6 | base64tab[InData[3]]);
        }
        InData += 4;
        TotalData -= 4;
    }

    if(TotalData & 03)
    {
        if(base64tab[InData[-2]] > 63)
            BytesDecoded -= 2;
        else
            BytesDecoded -= 1;
    }

    return BytesDecoded;
} 

/* ======================================================================
FUNCTION:
  indexOf

DESCRIPTION:
  Finds the first occurance of a character (among a string of choices).

INPUT/OUTPUT PARAMETERS:
  choices A C-string containing all the characters to search for.
  sptr The C-string to search.
  eptr The end boundary for sptr. If none of the choice characters are found before
  reaching eptr, this function fails.
  choiceIndexPtr A pointer to a int which is set with the index of choice character
  that is actually found. May be NULL, in which case nothing is set.

RETURN VALUE:
  Returns the index of the first occurance of a choice character. If no choice
  character is found before eptr, returns -1.
  
SIDE EFFECTS:
  The value pointed to by choiceIndexPtr may be updated (see above).
======================================================================*/
int indexOf(const char *choices, const char *sptr, const char *eptr, 
            int *choiceIndexPtr) 
{
   int numChoices = strlen(choices);   

   for (const char *ptr = sptr; ptr < eptr; ptr++) 
   {
      char c = *ptr;

      for (int i = 0; i < numChoices; i++) 
      {
         if (c == choices[i]) 
         {
            if (choiceIndexPtr != NULL) {
               *choiceIndexPtr  = i;
            }
            // Calculate offset from beginning.
            return (ptr - sptr);
         }
      }
   }

   // Not found
   return -1;
}

/* ======================================================================
FUNCTION:
  parseSignedInt32

DESCRIPTION:
  Parse a signed integer

INPUT/OUTPUT PARAMETERS:
  sptr A pointer to a C-string that may begin with an signed base-10 number 
  string. If so, the value of the pointer will be updated to just after the 
  integer string.
  eptr The end boundary for the C-string to parse. Parsing will stop if the boundary
  is hit, even if more number characters follow.
  valueptr A pointer to a 32-bit integer which is updated with the parsed value
  value if successful.

RETURN VALUE:
  Return true iff successful.
  
SIDE EFFECTS:
  The value pointed to by valueptr is updated (see above).
======================================================================*/
bool parseSignedInt32(const char **sptr, const char *eptr, int32 *valueptr) 
{
  const char *ptr = *sptr;

  if (ptr >= eptr)
  {
    return false;
  }

  bool negate = false;
  if (*ptr == '+')
  {
    ptr++;

    if (ptr >= eptr)
    {
      return false;
    }   
  } 
  else if (*ptr == '-')
  {
    ptr++;

    if (ptr >= eptr)
    {
      return false;
    }   

    negate = true;
  }

  if (!parseUnsignedInt32(&ptr, eptr, valueptr))
  {
    return false;
  }

  *sptr = ptr;
  
  if (negate)
  {
    *valueptr = -(*valueptr);
  }
  
  return true;
}

/* ======================================================================
FUNCTION:
  parseUnsignedInt32

DESCRIPTION:
  Parse a (positive) integer

INPUT/OUTPUT PARAMETERS:
  sptr A pointer to a C-string that may begin with an unsigned base-10 number 
  string. If so, the value of the pointer will be updated to just after the 
  integer string.
  eptr The end boundary for the C-string to parse. Parsing will stop if the boundary
  is hit, even if more number characters follow.
  valueptr A pointer to a 32-bit integer which is updated with the parsed value
  value if successful.

RETURN VALUE:
  Return true iff successful.
  
SIDE EFFECTS:
  The value pointed to by valueptr is updated (see above).
======================================================================*/
bool parseUnsignedInt32(const char **sptr, const char *eptr, int32 *valueptr) 
{
   bool anyDigitsFound = false;
   int32 value = 0;
   const char *ptr = *sptr;
   bool done = false;

   while ((ptr < eptr) && !done) 
   {
      char c = *ptr;

      if (isdigit(c)) 
      {
         anyDigitsFound = true;
         value *= 10;
         value += (c - '0');
         ptr++;
      } else {
         done = true;
         // terminate loop, don't increment ptr
      }
   }

   if (!anyDigitsFound)
   {
      return false;
   }

   *valueptr = value;
   *sptr = ptr;
   return true;
}
