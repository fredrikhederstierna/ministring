/**
 * This version is based on a public domain version of scanf.
 */

#include <stdio.h>
#include <stdarg.h>
#include <limits.h>
#include <ctype.h>
#include <string.h>

#include <scanf.h>

//----------------------------------------------------------------------

#define TRIM(s)     while((*(s)) &&    isspace((unsigned char)*(s)))  (s)++;
#define SKIP_ARG(s) while((*(s)) && (! isspace((unsigned char)*(s)))) (s)++;

//----------------------------------------------------------------------
/**
 * Unformat buffer into list of arguments
 *
 * @param buf   input buffer
 * @param fmt   format of buffer
 * @param args  arguments
 *
 * @return Number arguments read
 */
int vsscanf(const char * buf, const char * fmt, va_list args)
{
  const char *s = buf;
  const char *f = fmt;

  int colon;
  int width;
  int qual;
  int base;
  int sign;

  char * next = NULL;
  char digit;

  int num_args_read = 0;

  // while more in buffer to parse
  while ((*f) && (*s)) {
    
    // Check whitespace in format
    // Whitespace in format maps to space in input
    if (isspace((unsigned char) *f)) {
      // Skip leading whitespace
      TRIM(f);
      TRIM(s);
    }

    // Any char in format must match input
    if ((*f) && ((*f) != '%')) {
      if (*f++ == *s++) {
        // matched
        continue;
      }
      // no match
      break;
    }

    // check end of format string
    if (*f) {
      // more chars, skip '%'
      f++;
    }
    else {
      // end of format string, exit
      break;
    }

    // An optional starting asterisk indicates that the data
    // is to be retrieved from stdin but ignored,
    // i.e. it is not stored in the corresponding argument.
    if ((*f) == '*') {
      // skip argument until whitespace found, or end of string
      SKIP_ARG(f);
      SKIP_ARG(s);
      // continue parsing
      continue;
    }
    else if ((*f) == ':') {
      // Check for colon
      colon = 1;
      // Skip colon
      f++;
    }
    else {
      // No colon nor asterisk
      colon = 0;
    }

    // Check field width
    if (isdigit((unsigned char) *f)) {
      // Parse field width
      width = atoi(&f);
    }
    else {
      // no field width limiter
      width = -1;
    }
    // get qualifier
    qual = (*f);

    // get conversion qualifier
    if ((qual == 'h') ||
        (qual == 'l') ||
        (qual == 'L') ||
        (qual == 'Z') ||
        (qual == 'z')) {

      // step over qualifier
      f++;
      // if same qualifier again
      if (qual == (*f)) {
        if (qual == 'h') {
          qual = 'H';
          f++;
        }
        else if (qual == 'l') {
          qual = 'L';
          f++;
        }
      }
    }
    else {
      qual = -1;
    }

    // check for end of string
    if (!(*f) || !(*s))
      break;

    // Set initial base and sign
    base = 10;
    sign = 0;

    // Handle qualifier
    switch (*f++) {
    case 'c':
    {
      // Char qualifier
      char *sc = (char *) va_arg(args, char *);
      if (width == -1)
        width = 1;
      do {
        *sc++ = *s++;
        width--;
      } while ((*s) && (width > 0));
      
      num_args_read++;
      continue;
    }

    case 's':
    {
      // String qualifier
      char *ss = (char *) va_arg(args, char *);
      if (width == -1)
        width = INT_MAX;
      // skip leading white space in buffer
      TRIM(s);
      // now copy until next white space or :; if specified
      while ((*s) && (width > 0)) {
        if (colon) {
          if ((*s == ':') || (*s == ';'))
            break;
        }
        else {
          if (isspace((unsigned char) *s))
            break;
        }
        // read string
        if (ss) {
          *ss++ = *s++;
        }
        else {
          // null dest string, just skip
          s++;
        }
        width--;
      }
      if (ss) {
        // null-terminate
        *ss = '\0';
      }
      num_args_read++;
      continue;
    }
      
    case 'n':      
    {
      // return number of characters read so far
      int *i = (int *) va_arg(args, int *);
      *i = s - buf;
      continue;
    }

    case 'i':
      base = 0;
      // fall-through
    case 'd':
      sign = 1;
      // fall-through
    case 'u':
      break;

    case 'o':
      // octal
      base = 8;
      break;

    case 'x':
    case 'X':
      // hex
      base = 16;
      break;

    case '%':
      // looking for '%' in str
      if (*s++ == '%')
        continue;
    default:
      // invalid format; stop here
      return num_args_read;
    }

    // integer conversion
    // skip leading white space in buffer
    TRIM(s);

    // read out digit
    digit = *s;
    if (sign && ((digit == '-') || (digit == '+'))) {
      digit = *(s + 1);
    }

    // check for invalid non numeric arguments
    if ((digit == '\0') ||
        (base == 16 && !isxdigit((unsigned char) digit)) ||
        (base == 10 && ! isdigit((unsigned char) digit)) ||
        (base == 8  && (!isdigit((unsigned char) digit)  || (digit > '7'))) ||
        (base == 0  && ! isdigit((unsigned char) digit)))
      break;

    // check qualifier
    switch (qual) {

    case 'H':
      // char type, that is 'hh' in format
      if (sign) {
        signed char *sH = (signed char *) va_arg(args, signed char *);
        *sH = (signed char) strtol(s, &next, base);
      }
      else {
        unsigned char *sH = (unsigned char *) va_arg(args, unsigned char *);
        *sH = (unsigned char) strtoul(s, &next, base);
      }
      break;

    case 'h':
      // short type
      if (sign) {
        signed short *sh = (signed short *) va_arg(args, signed short *);
        *sh = (signed short) strtol(s, &next, base);
      }
      else {
        unsigned short *sh = (unsigned short *) va_arg(args, unsigned short *);
        *sh = (unsigned short) strtoul(s, &next, base);
      }
      break;

    case 'l':
      // long type
      if (sign) {
        signed long *l = (signed long *) va_arg(args, signed long *);
        *l = strtol(s, &next, base);
      }
      else {
        unsigned long *l = (unsigned long*) va_arg(args, unsigned long*);
        *l = strtoul(s, &next, base);
      }
      break;

    case 'L':
      // long long type
      if (sign) {
        signed long long *l = (signed long long*) va_arg(args, signed long long *);
        *l = strtoll(s, &next, base);
      }
      else {
        unsigned long long *l = (unsigned long long*) va_arg(args, unsigned long long*);
        *l = strtoull(s, &next, base);
      }
      break;

    case 'Z':
    case 'z':
    {
      // read size
      size_t *sz = (size_t*) va_arg(args, size_t *);
      *sz = (size_t) strtoul(s, &next, base);
      break;
    }

    default:
      // normal int
      if (sign) {
        signed int *i = (signed int *) va_arg(args, signed int *);
        *i = (signed int) strtol(s, &next, base);
      }
      else {
        unsigned int *i = (unsigned int*) va_arg(args, unsigned int *);
        *i = (unsigned int) strtoul(s, &next, base);
      }
      break;
    }

    num_args_read++;

    // check next, will this ever be true?? Shouldnt it be de-ref pointer?
    if (next == NULL) {
      // exit, no next arg
      break;
    }

    // Continue parse at next
    s = next;
  }

  // return numer of arguments read
  return num_args_read;
}

//----------------------------------------------------------------------
/**
 * Unformat a buffer into a list of arguments
 *
 * @param buf  input buffer
 * @param fmt  formatting of buffer
 * @param ...  resulting arguments
 *
 * @return Number arguments read
 */
int sscanf(const char * buf, const char * fmt, ...)
{
  va_list args;
  int args_read;
  va_start(args,fmt);
  args_read = vsscanf(buf, fmt, args);
  va_end(args);
  return args_read;
}
