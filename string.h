#ifndef _STRING_H_
#define _STRING_H_

#include <stdio.h>

void * memchr(const void *src, int c, size_t len);

int strcmp(const char *s1, const char *s2);
int strncmp(const char *s1, const char *s2, size_t n);
size_t strlen(const char *s);
size_t strnlen(const char *s, size_t max);
char * strstr(const char *in, const char *s);
char * strchr(char const *s, int c);
char * strnchr(const char * s, size_t len, int c);
char * strrchr(const char *s, int c);
char * strcat(char *dest, const char *src);
char * strncat(char *dest, const char *src, size_t len);
char * strcpy(char * __restrict to, const char * __restrict from);

unsigned long long strtoull(const char *cp, char **endp, unsigned int base);
long long strtoll(const char *cp, char **endp, unsigned int base);
unsigned long strtoul(const char *cp, char **endp, unsigned int base);
long strtol(const char *cp, char **endp, unsigned int base);
long double strtold(const char *s, char **endptr);
float strtof(const char *s, char **endptr);
double strtod(const char *s, char **endptr);
char * strtok(char *s1, const char *delimit);

int atoi(const char **s);
double atof(const char *s);

int strncasecmp(const char *s1, const char *s2, size_t n);
int strcasecmp(const char *s1, const char *s2);

size_t strspn(const char *s1, register const char *s2);
size_t strcspn(const char *s1, register const char *s2);
char *strpbrk(const char *s1, const char *s2);
char *strtok_r(char *s, const char *delim, char **last);

#endif /* _STRING_H_ */
