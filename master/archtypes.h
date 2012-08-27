//
// Word size dependent definitions
// DAL 1/03
//
#ifndef ARCHTYPES_H
#define ARCHTYPES_H

#ifdef __x86_64__
#define X64BITS
#endif

// for when we care about how many bits we use
typedef signed char      int8;
typedef signed short     int16;
typedef signed long      int32;

#ifdef _WIN32
#ifdef _MSC_VER
typedef signed __int64   int64;
#endif
#elif defined _LINUX
typedef long long	int64;
#endif

typedef unsigned char      uint8;
typedef unsigned short     uint16;
typedef unsigned long      uint32;
#ifdef _WIN32
#ifdef _MSC_VER
typedef unsigned __int64   uint64;
#endif
#elif defined _LINUX
typedef unsigned long long uint64;
#endif

#endif /* ARCHTYPES_H */
