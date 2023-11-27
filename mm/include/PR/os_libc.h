#ifndef PR_OS_LIBC_H
#define PR_OS_LIBC_H

#include "libc/stdarg.h"
#include <string.h>

// 2S2H This file might just outright get removed soon. Ifdeffing for now
#if 0
//void bcopy(void* __src, void* __dest, int __n);
//int bcmp(void* __s1, void* __s2, int __n);
//void bzero(void* begin, int length);

void osSyncPrintf(const char* fmt, ...);
#endif

#endif