/**
 * This version is based on a public domain version of string library functions.
 */

#include <stdio.h>
#include <ctype.h>
#include <math.h>
#include <float.h>
#include <limits.h>
#include <assert.h>

#include <string.h>

//#define ASSERT(cond)
#define ASSERT(cond) assert(cond)

//----------------------------------------------------------------------
int strcmp(const char *s1, const char *s2)
{
  ASSERT(s1);
  ASSERT(s2);

  while ((*s1 == *s2) && *s1) {
    s1++;
    s2++;
  }

  return (*s1 - *s2);
}

//----------------------------------------------------------------------
int strncmp(const char *s1, const char *s2, size_t n)
{
  ASSERT(s1);
  ASSERT(s2);

  if (n == 0) {
    return 0;
  }

  while (--n && (*s1 == *s2) && *s1) {
    s1++;
    s2++;
  }

  return (*s1 - *s2);
}

//----------------------------------------------------------------------
size_t strlen(const char *s)
{
  size_t len = 0;

  ASSERT(s);

  while (*s++ && ++len);

  return len;
}

//----------------------------------------------------------------------
void * memchr(const void *src, int c, size_t len)
{
  char *s = (char*)src;

  ASSERT(s);

  while (len--) {
    if (*s == c) {
      return (void *) s;
    }
    src++;
  }
  return NULL;
}

//----------------------------------------------------------------------
size_t strnlen(const char *s, size_t max)
{
  const char *end = (const char *) memchr(s, 0, max);
  return end ? (size_t)(end - s) : max;
}

//----------------------------------------------------------------------
char * strstr(const char *in, const char *s)
{
  char c;
  size_t len;

  ASSERT(s);

  c = *s++;
  if (!c) {
    return (char *) in; // Trivial empty string case
  }

  len = strlen(s);
  do {
    char sc;

    do {
      sc = *in++;
      if (!sc)
        return (char *) 0;
    } while (sc != c);
  } while (strncmp(in, s, len) != 0);

  return (char *) (in - 1);
}

//----------------------------------------------------------------------
char * strchr(char const *s, int c)
{
  ASSERT(s);

  do {
    if ((unsigned) *s == (unsigned) c)
      return (char *)s;

  } while ((*(++s)) != 0);

  return NULL;
}

//----------------------------------------------------------------------
char * strnchr(const char * s, size_t len, int c)
{
  size_t pos;

  ASSERT(s);

  for (pos = 0; pos < len; pos++) {
    if (s[pos] == c)
      return (char *)(s + pos);
  }
  return NULL;
}

//----------------------------------------------------------------------
char * strrchr(const char *s, int c)
{
  char * save;

  ASSERT(s);

  for (save = NULL;; ++s) {
    if (*s == c) {
      save = (char*)s;
    }
    if (!*s)
      return(save);
  }
  // NOTREACHED
  return NULL;
}

//----------------------------------------------------------------------
char * strcat(char *dest, const char *src)
{
  char *ret = dest;

  ASSERT(dest);
  ASSERT(src);

  while (*dest)
    dest++;

  while ((*dest++ = *src++));

  return ret;
}

//----------------------------------------------------------------------
char * strncat(char *dest, const char *src, size_t len)
{
  size_t i;
  char *ret = dest;

  ASSERT(dest);
  ASSERT(src);

  while (*dest) {
    dest++;
  }

  for (i = 0; (i < len) && *src; i++) {
      *dest++ = *src++;
  }

  *dest = '\0';

  return ret;
}

//----------------------------------------------------------------------
char * strcpy(char * __restrict to, const char * __restrict from)
{
  char *save = to;

  ASSERT(from);
  ASSERT(to);
  
  while ((*to = *from) != 0) {
    to++;
    from++;
  }
  return save;
}

//----------------------------------------------------------------------
/**
 * Parse prefix for base
 *
 * @param cp   String to parse
 * @param base Numeric base, also out parameter
 *
 * @return Parsed string without prefix
 */
static const char * __parse_base_prefix(const char *cp, unsigned int *base)
{
  ASSERT(base);
  ASSERT(cp);
  
  // if no base set
  if (*base == 0) {
    // check octal
    if (*cp == '0') {
      // skip leading zero prefix for octal/hex
      cp++;
      // check prefix and first digit hex
      if ((tolower((unsigned char) *cp) == 'x') && isxdigit((unsigned char) cp[1])) {
        // skip hex prefix
        cp++;
        // hex
        *base = 16;
      }
      else {
        // octal
        *base = 8;
      }
    }
    else {
      // decimal
      *base = 10;
    }
  }
  else if (*base == 16) {
    // check for leading hex prefix
    if (cp[0] == '0' && tolower((unsigned char) cp[1]) == 'x') {
      // skip hex prefix
      cp += 2;
    }
  }
  return cp;
}

//----------------------------------------------------------------------
/**
 * Convert string to unsigned long long type
 *
 * @param cp    Start of string
 * @param endp  Pointer set to the end of parsed string
 * @param base  Number base to use
 *
 * @return Converted unsigned long long
 */
unsigned long long strtoull(const char *cp, char **endp, unsigned int base)
{
  unsigned long long ret = 0;
  char c;

  cp = __parse_base_prefix(cp, &base);

  do {
    unsigned int val;

    c = *cp;

    // if hex/dec digit
    if (isdigit((unsigned char) c)) {
      val = c - '0';
    }
    else if (isxdigit((unsigned char) c)) {
      val = tolower((unsigned char) c) - 'a' + 10;
    }
    else {
      val = -1;
    }

    // check if valid
    if (val < base) {
      ret = (ret * base) + val;
      cp++;
    }
    else {
      // invalid, exit
      c = 0;
    }         
  } while ( c );
  
  // set end pointer
  if (endp) {
    *endp = (char *) cp;
  }

  return ret;
}

//----------------------------------------------------------------------
/**
 * Convert string to signed long long type
 *
 * @param cp    Start of string
 * @param endp  Pointer set to the end of parsed string
 * @param base  Number base to use
 *
 * @return Converted signed long long
 */
long long strtoll(const char *cp, char **endp, unsigned int base)
{
  ASSERT(cp);
  
  if (*cp == '+') {
    return strtoull(cp + 1, endp, base);
  }
  else if (*cp == '-') {
    return -strtoull(cp + 1, endp, base);
  }
  else {
    return strtoull(cp, endp, base);
  }
}

//----------------------------------------------------------------------
/**
 * Convert string to unsigned long type
 *
 * @param cp    Start of string
 * @param endp  Pointer set to the end of parsed string
 * @param base  Number base to use
 *
 * @return Converted unsigned long
 */
unsigned long strtoul(const char *cp, char **endp, unsigned int base)
{
  return (unsigned long) strtoull(cp, endp, base);
}

//----------------------------------------------------------------------
/**
 * Convert string to signed long type
 *
 * @param cp    Start of string
 * @param endp  Pointer set to the end of parsed string
 * @param base  Number base to use
 *
 * @return Converted signed long
 */
long strtol(const char *cp, char **endp, unsigned int base)
{
  ASSERT(cp);
  
  if (*cp == '+') {
    return strtoul(cp + 1, endp, base);
  }
  else if (*cp == '-') {
    return -strtoul(cp + 1, endp, base);
  }
  else {
    return strtoul(cp, endp, base);
  }
}

//----------------------------------------------------------------------
/**
 * Convert string to integer and move pointer forward
 *
 * @param s String to convert to integer, also outparameter of moved pointer
 *
 * @return Converted integer
 */
int atoi(const char **s)
{
  int i = 0;
  char c;

  ASSERT(s);
  
  do {
    c = **s;
    if ( isdigit((unsigned char) c) ) {
      i = (i * 10) + (c - '0');
      (*s)++;
    }
    else {
      c = 0;
    }
  } while ( c );

  return i;
}

//----------------------------------------------------------------------
int strncasecmp(const char *s1, const char *s2, size_t n)
{
  if (n == 0) {
    return 0;
  }

  ASSERT(s1);
  ASSERT(s2);
  
  while ((n-- != 0) &&
         (tolower(*s1) == tolower(*s2))) {
    if ((n == 0)      ||
        (*s1 == '\0') ||
        (*s2 == '\0')) {
      break;
    }
    s1++;
    s2++;
  }

  return tolower(*(unsigned char *) s1) - tolower(*(unsigned char *) s2);
}

//----------------------------------------------------------------------
int strcasecmp(const char *s1, const char *s2)
{
  return strncasecmp(s1, s2, INT_MAX);
}

//----------------------------------------------------------------------
char * strtok(char *s1, const char *delimit)
{
  static char *last_token = NULL; /* UNSAFE SHARED STATE! */
  char *tmp;

  /* Skip leading delimiters if new string. */
  if ( s1 == NULL ) {
    s1 = last_token;
    /* End of story? */
    if (s1 == NULL) {
      return NULL;
    }
  }
  else {
    s1 += strspn(s1, delimit);
  }

  /* Find end of segment */
  tmp = strpbrk(s1, delimit);
  if (tmp) {
    /* Found another delimiter, split string and save state. */
    *tmp = '\0';
    last_token = tmp + 1;
  }
  else {
    /* Last segment, remember that. */
    last_token = NULL;
  }

  return s1;
}

//----------------------------------------------------------------------
char *strtok_r(char *s, const char *delim, char **last)
{
  char *spanp;
  int c, sc;
  char *tok;
  
  if ((s == NULL) && ((s = *last) == NULL)) {
    return NULL;
  }
  
  // Skip (span) leading delimiters (s += strspn(s, delim), sort of).
cont:
  c = *s++;
  for (spanp = (char *)delim; (sc = *spanp++) != 0; ) {
    if (c == sc) {
      goto cont;
    }
  }

  // no non-delimiter characters
  if (c == 0) {
    *last = NULL;
    return NULL;
  }
  tok = (s - 1);

  // Scan token (scan for delimiters: s += strcspn(s, delim), sort of).
  // Note that delim must have one NULL.
  for (;;) {
    c = *s++;
    spanp = (char *)delim;
    do {
      if ((sc = *spanp++) == c) {
        if (c == 0) {
          s = NULL;
        }
        else {
          char *w = s - 1;
          *w = '\0';
        }
        *last = s;
        return tok;
      }
    }
    while (sc != 0);
  }
  // NOT REACHABLE.
}

//----------------------------------------------------------------------
double strtod(const char *s, char **endptr)
{
  double number;
  int exponent;
  int negative;
  char *p = (char *) s;
  double p10;
  int n;
  int num_digits;
  int num_decimals;

  ASSERT(p);
  
  // Skip leading whitespace
  while (isspace(*p)) {
    p++;
  }

  // Handle optional sign
  negative = 0;
  switch (*p) {
    case '-': negative = 1; // Fall through to increment position
    case '+': p++;
  }

  number       = 0.;
  exponent     = 0;
  num_digits   = 0;
  num_decimals = 0;

  // Process string of digits
  while (isdigit(*p)) {
    number = number * 10. + (*p - '0');
    p++;
    num_digits++;
  }

  // Process decimal part
  if (*p == '.') {
    p++;

    while (isdigit(*p)) {
      number = number * 10. + (*p - '0');
      p++;
      num_digits++;
      num_decimals++;
    }

    exponent -= num_decimals;
  }

  if (num_digits == 0) {
    return 0.0;
  }

  // Correct for sign
  if (negative) {
    number = -number;
  }

  // Process an exponent string
  if ((*p == 'e') || (*p == 'E')) {
    // Handle optional sign
    negative = 0;
    switch (*++p) {
      case '-': negative = 1;   // Fall through to increment pos
      case '+': p++;
    }

    // Process string of digits
    n = 0;
    while (isdigit(*p)) {
      n = n * 10 + (*p - '0');
      p++;
    }

    if (negative) {
      exponent -= n;
    }
    else {
      exponent += n;
    }
  }

  if ((exponent < DBL_MIN_EXP)  || (exponent > DBL_MAX_EXP)) {
    return HUGE_VAL;
  }

  // Scale the result
  p10 = 10.;
  n = exponent;
  if (n < 0) {
    n = -n;
  }
  while (n) {
    if (n & 1) {
      if (exponent < 0) {
        number /= p10;
      }
      else {
        number *= p10;
      }
    }
    n >>= 1;
    p10 *= p10;
  }

  if (endptr) {
    *endptr = p;
  }

  return number;
}

//----------------------------------------------------------------------
float strtof(const char *s, char **endptr)
{
  return (float) strtod(s, endptr);
}

//----------------------------------------------------------------------
long double strtold(const char *s, char **endptr)
{
  return strtod(s, endptr);
}

//----------------------------------------------------------------------
double atof(const char *s)
{
  return strtod(s, NULL);
}

//----------------------------------------------------------------------
/*
 * Span the string s2 (skip characters that are in s2).
 */
size_t strspn(const char *s1, register const char *s2)
{
  register const char *p = s1, *spanp;
  register char c, sc;

  ASSERT(s1);
  ASSERT(s2);  
  
  // Skip any characters in s2, excluding the terminating \0.
cont:
  c = *p++;
  for (spanp = s2; (sc = *spanp++) != 0;) {
    if (sc == c) {
      goto cont;
    }
  }
  return (p - 1 - s1);
}

//----------------------------------------------------------------------
/*
 * Span the complement of string s2.
 */
size_t strcspn(const char *s1, register const char *s2)
{
  register const char *p, *spanp;
  register char c, sc;

  ASSERT(s1);
  ASSERT(s2);  

  // Stop when any character found from s2.
  // Note that s2 must be z-terminated.
  for (p = s1;;) {
    c = *p++;
    spanp = s2;
    do {
      if ((sc = *spanp++) == c) {
        return (p - 1 - s1);
      }
    } while (sc != 0);
  }
  // NOT REACHABLE.
}

//----------------------------------------------------------------------
char *strpbrk(const char *s1, const char *s2)
{
  const  char *c = s2;

  ASSERT(s1);
  ASSERT(s2);  
  
  if (!*s1) {
    return (char *) NULL;
  }
  
  while (*s1) {
    for (c = s2; *c; c++) {
      if (*s1 == *c) {
        break;
      }
    }
    if (*c) {
      break;
    }
    s1++;
  }
  
  if (*c == '\0') {
    s1 = NULL;
  }
  
  return (char *) s1;
}
