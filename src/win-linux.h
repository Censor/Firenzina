/*******************************************************************************
Firenzina is a UCI chess playing engine by
Yuri Censor (Dmitri Gusev) and ZirconiumX (Matthew Brades).
Rededication: To the memories of Giovanna Tornabuoni and Domenico Ghirlandaio.
Special thanks to: Norman Schmidt, Jose Maria Velasco, Jim Ablett, Jon Dart.
Firenzina is a clone of Fire 2.2 xTreme by Kranium (Norman Schmidt). 
Firenzina is a derivative (via Fire) of FireBird by Kranium (Norman Schmidt) 
and Sentinel (Milos Stanisavljevic). Firenzina is based (via Fire and FireBird)
on Ippolit source code: http://ippolit.wikispaces.com/
Ippolit authors: Yakov Petrovich Golyadkin, Igor Igorovich Igoronov,
and Roberto Pescatore 
Ippolit copyright: (C) 2009 Yakov Petrovich Golyadkin
Ippolit date: 92th and 93rd year from Revolution
Ippolit owners: PUBLICDOMAIN (workers)
Ippolit dedication: To Vladimir Ilyich
"This Russian chess ship is a truly glorious achievement of the
 October Revolution and Decembrists movement!"

Firenzina is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Firenzina is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see http://www.gnu.org/licenses/.
*******************************************************************************/

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#define NomeWindows

#pragma warning(disable : 4244)
// conversion from 'int' to 'short', possible loss of data
#pragma warning(disable : 4018)
// signed/unsigned mismatch
#pragma warning(disable : 4996)
// 'sscanf': This function or variable may be unsafe
#pragma warning(disable : 4334)
// '<<' : result of 32-bit shift implicitly converted to 64 bits
#pragma warning(disable : 4761)
// integral size mismatch in argument; conversion supplied
#pragma warning(disable : 4311)
// pointer truncation from 'void *' to 'DWORD'
#pragma warning(disable : 4090)
// different 'const' qualifiers
#pragma warning(disable : 4113)
// 'int (__cdecl *)()' differs in parameter list
#pragma warning(disable : 4146)
// unary minus operator applied to unsigned type, result still unsigned

typedef __int8 sint8;
typedef __int16 sint16;
typedef __int32 sint32;
typedef __int64 sint64;
typedef unsigned __int8 boolean;
typedef unsigned __int8 uint8;
typedef unsigned __int16 uint16;
typedef unsigned __int32 uint32;
typedef unsigned __int64 uint64;

#define INLINE _inline
#define atoll _atoi64
#define Type64Bit "%I64d"
#define MemAlign(a, b, c) a = _aligned_malloc (c, b)
#define AlignedFree(x) _aligned_free (x)
#define __builtin_prefetch
#if defined(_WIN64)
#include "win64bits.h"
#else
#include "win32bits.h"
#endif

#define MutexType CRITICAL_SECTION
#define CondType HANDLE
#define Lock(x) EnterCriticalSection (x)
#define UnLock(x) LeaveCriticalSection (x)
#define WaitForLock(x, y) WaitForSingleObject (x, INFINITE)
#define SignalForLock(x, y) SetEvent (x)
#define LockInit(x) InitializeCriticalSection (x)
#define LockDestroy DeleteCriticalSection
#define CondInit(x, y) (x) = CreateEvent (0, false, false, 0)
#define TryLock(x) (!TryEnterCriticalSection (x))
#define PThreadCreate(N, b, thr, d) \
	(* (N)) = CreateThread (NULL, 0, thr, (LPVOID) (d), 0, NULL);
#define PThreadJoin(x)              \
	{ DWORD w; do { GetExitCodeThread (x, &w); } while (w == STILL_ACTIVE); }
HANDLE PThread[MaxCPUs], PThreadIO;
#define IvanThread(A) DWORD WINAPI SMPThread (LPVOID A)
#define IOThread(A) DWORD WINAPI io_thread (LPVOID A)
#define VoidStarType DWORD
#else
#define sint8 signed char
#define sint16 signed short int
#define sint32 int
#define sint64 long long int
#define uint8 unsigned char
#define uint16 unsigned short int
#define uint32 unsigned int
#define uint64 unsigned long long int
#define INLINE inline
#define Type64Bit "%lld"
#define MemAlign(a, b, c) posix_memalign ((void*) &(a), b, c)
#define AlignedFree(x) free (x)
#include "bits.h"
#include <pthread.h>
#define MutexType pthread_mutex_t
#define CondType pthread_cond_t
#define Lock(x) pthread_mutex_lock (x)
#define UnLock(x) pthread_mutex_unlock (x)
#define Wait(x, y) pthread_cond_wait (x, y)
#define Signal(x) pthread_cond_signal (x)
#define WaitForLock(x, y) { Lock (&(y)); Wait (&(x), &(y)); UnLock (&(y)); }
#define SignalForLock(x, y) { Lock (&(y)); Signal (&(x)); UnLock (&(y)); }
#define LockInit(x) pthread_mutex_init ((x), NULL)
#define LockDestroy pthread_mutex_destroy
#define TryLock(x) pthread_mutex_trylock (x)
#define CondInit(x, y) \
 { pthread_cond_init (&(x), NULL); pthread_mutex_init (&(y), NULL); }
#define PThreadCreate(N, b, thr, d) pthread_create (N, NULL, thr, (void*) (d))
#define PThreadJoin(x) pthread_join (x, NULL)
pthread_t PThread[MaxCPUs], PThreadIO;
#define IvanThread(A) void* SMPThread (void* A)
#define IOThread(A) void* io_thread (void* A)
#define VoidStarType void*
//#define LinuxLargePages true
#endif
