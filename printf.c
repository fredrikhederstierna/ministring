/**
 * This version is based on a public domain version of printf.
 */

#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <printf.h>

#define __printchar(out,c) printf("%c",(c));

//---------------------------------------

#define PRINT_PAD_RIGHT (1)
#define PRINT_PAD_ZERO  (2)

//---------------------------------------
static int prints(char **out, const char *string, int width, int pad)
{
  register int pc = 0;
  register int padchar = ' ';

  if (width > 0) {
    register int len = 0;
    register const char *ptr;
    for (ptr = string; *ptr; ++ptr)
      ++len;
    if (len >= width)
      width = 0;
    else
      width -= len;
    if (pad & PRINT_PAD_ZERO)
      padchar = '0';
  }
  if (!(pad & PRINT_PAD_RIGHT)) {
    for (; width > 0; --width) {
      __printchar(out, padchar);
      ++pc;
    }
  }
  for (; *string ; ++string) {
    __printchar(out, *string);
    ++pc;
  }
  for (; width > 0; --width) {
    __printchar(out, padchar);
    ++pc;
  }

  return pc;
}

/* following length should be enough for 32 bit int */
#define PRINTI_BUF_LEN  (12)

static int printi(char **out, int i, int b, int sg, int width, int pad, int letbase)
{
  char printi_buf[PRINTI_BUF_LEN];

  register char *s;
  register int t, neg = 0, pc = 0;
  register unsigned int u = i;

  if (i == 0) {
    printi_buf[0] = '0';
    printi_buf[1] = '\0';
    return prints(out, printi_buf, width, pad);
  }

  if (sg && (b == 10) && (i < 0)) {
    neg = 1;
    u = -i;
  }

  s = printi_buf + PRINTI_BUF_LEN - 1;
  *s = '\0';

  while (u) {
    t = u % b;
    if (t >= 10)
      t += letbase - '0'- 10;
    *--s = t + '0';
    u /= b;
  }

  if (neg) {
    if (width && (pad & PRINT_PAD_ZERO)) {
      __printchar(out, '-');
      ++pc;
      --width;
    } else {
      *--s = '-';
    }
  }

  return pc + prints(out, s, width, pad);
}

//------------------------------------------------------
int pprint(char **out, const char *format, va_list args)
{
  register int width, pad, longlong;
  register int pc = 0;
  char scr[2];

  for (; *format != 0; ++format) {
    if (*format == '%') {
      ++format;
      width = pad = longlong = 0;
      if (*format == '\0')
        break;
      if (*format == '%')
        goto print_out;
      if (*format == '-') {
        ++format;
        pad = PRINT_PAD_RIGHT;
      }
      while (*format == '0') {
        ++format;
        pad |= PRINT_PAD_ZERO;
      }
      for (; (*format >= '0') && (*format <= '9'); ++format) {
        width *= 10;
        width += *format - '0';
      }
      if (*format == 'l') {
        ++format;
        if (*format == 'l') {
          ++format;
          longlong = 1;
        }
      }
      if (*format == 'h') {
        ++format;
        if (*format == 'h') {
          ++format;
        }
      }

      char fmt = *format;
      switch (fmt) {
      case 'd':
      case 'i':
      case 'x':
      case 'X':
      case 'u': {
        int  intbase = 10;
        char ascbase = 'a';
        int  addsign = 1;
        int  width_longlong = 0;
        int  ival_hi = 0;
        int  ival;

        if (longlong) {
          long long llval = va_arg( args, long long );
          // fix width if longlong (only works for X)
          width_longlong = width;
          if (width > 8) {
            width = 8;
          }
          ival    = (int)(llval << 32);
          ival_hi = (int)(llval & 0xFFFFFFFF);
        }
        else {
          // short int and char is converted to long int by compiler
          ival = va_arg( args, int );
        }

        if (fmt == 'x') {
          intbase = 16;
          addsign = 0;
        }
        else if (fmt == 'X') {
          intbase = 16;
          addsign = 0;
          ascbase = 'A';
        }
        else if (fmt == 'u') {
          addsign = 0;
        }

        // skip if zero and long long
        if (!(longlong && (ival == 0) && (width_longlong <= 8))) {
          int printed_chars = printi(out, ival, intbase, addsign, width, pad, ascbase);
          pc += printed_chars;
          width_longlong -= printed_chars;
        }

        if (longlong) {
          pc += printi(out, ival_hi, intbase, addsign, width_longlong, pad, ascbase);
        }
        continue;
      }
      case 'p': {
        register int p = va_arg( args, int );
        pc += printi(out, p, 16, 0, width, pad, 'A');
        continue;
      }
      case 's': {
        register char *s = (char *)va_arg( args, char * );
        pc += prints(out, s ? s : "(null)", width, pad);
        continue;
      }
      case 'c': {
        /* char are converted to int then pushed on the stack */
        scr[0] = (char)va_arg( args, int );
        scr[1] = '\0';
        pc += prints(out, scr, width, pad);
        continue;
      }
      default:
        break;
      } // switch

    } else {
      print_out: __printchar(out, *format);
      ++pc;
    }
  }
  if (out)
    **out = '\0';
  va_end(args );
  return pc;
}

//----------------------------------------------------
// if out is NULL, send to stdout (0)
int sprintf(char *out, const char *format, ...)
{
  int ret;
  va_list args;
  va_start(args, format);
  if (out)
    ret = pprint(&out, format, args);
  else
    ret = pprint(0, format, args);
  return ret;
}

//----------------------------------------------------
// TODO: check size n
int snprintf(char *out, size_t size, const char *format, ...)
{
  int ret;
  va_list args;
  va_start(args, format);
  if (out)
    ret = pprint(&out, format, args);
  else
    ret = pprint(0, format, args);
  return ret;
}

//----------------------------------------------------
// if out is NULL, send to stdout (0)
int vsprintf(char *out, const char *format, va_list args)
{
  int ret;
  if (out)
    ret = pprint(&out, format, args);
  else
    ret = pprint(0, format, args);
  return ret;
}

//----------------------------------------------------
// TODO: check size n
int vsnprintf(char *out, size_t size, const char *format, va_list args)
{
  int ret;
  if (out)
    ret = pprint(&out, format, args);
  else
    ret = pprint(0, format, args);
  return ret;
}
