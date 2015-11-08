#ifndef _SCANF_H_
#define _SCANF_H_

#include <stdarg.h>

int vsscanf(const char * buf, const char * fmt, va_list args);
int sscanf(const char * buf, const char * fmt, ...);

#endif /*_SCANF_H_*/
