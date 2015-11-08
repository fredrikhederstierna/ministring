#ifndef _PRINTF_H_
#define _PRINTF_H_

#include <stdarg.h>

int pprint(char **out, const char *format, va_list args);
int sprintf(char *out, const char *format, ...);
int snprintf(char *out, size_t size, const char *format, ...);
int vsprintf(char *out, const char *format, va_list args);
int vsnprintf(char *out, size_t size, const char *format, va_list args);

#endif /*_PRINTF_H_*/
