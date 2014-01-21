/*******************************************************************************
Firenzina is a UCI chess playing engine by
Kranium (Norman Schmidt), Yuri Censor (Dmitri Gusev) and ZirconiumX (Matthew Brades).
Rededication: To the memories of Giovanna Tornabuoni and Domenico Ghirlandaio.
Special thanks to: Norman Schmidt, Jose Maria Velasco, Jim Ablett, Jon Dart, Andrey Chilantiev, Quoc Vuong.
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


#include "fire.h" // Added by YC on 1/9/2014
#if defined(__GNUC__)

#include <limits.h>
#include <stdint.h>

#define INLINE inline

static INLINE int BSR (uint64 w)
	{
	int64_t ret = sizeof(uint64) * CHAR_BIT - 1;

	return w ? ret - __builtin_clzll(w) : ret;
	}

static INLINE int BSF (uint64 w)
	{
	return __builtin_ffsll(w) - 1;
	}


#if defined(HasPopCNT)
static INLINE int POPCNT (uint64 w)
	{
	return __builtin_popcountll(w);
	}

#else

static INLINE int POPCNT (uint64 w)
{
  w = w - ((w >> 1) & 0x5555555555555555ull);
  w = (w & 0x3333333333333333ull) + ((w >> 2) & 0x3333333333333333ull);
  w = (w + (w >> 4)) & 0x0f0f0f0f0f0f0f0full;
  return(w * 0x0101010101010101ull) >> 56;
}

#endif

#elif defined(_WIN64)
static INLINE int BSR (uint64 w)
	{
	uint64 x;
	asm ("bsrq %1,%0\n": "=&r" (x):"r" (w));
	return x;
	}
static INLINE int BSF (uint64 w)
	{
	uint64 x;
	asm ("bsfq %1,%0\n": "=&r" (x):"r" (w));
	return x;
	}

#if defined(HasPopCNT)
static INLINE int POPCNT (uint64 w)
	{
	uint64 x;
	asm ("popcntq %1,%0\n": "=&r" (x):"r" (w));
	return x;
	}
#else
static INLINE int POPCNT (uint64 w)
{
  w = w - ((w >> 1) & 0x5555555555555555ull);
  w = (w & 0x3333333333333333ull) + ((w >> 2) & 0x3333333333333333ull);
  w = (w + (w >> 4)) & 0x0f0f0f0f0f0f0f0full;
  return(w * 0x0101010101010101ull) >> 56;
}
#endif

#elif defined(_WIN32)

static INLINE int BSR (uint64 w)
	{
	int x1, x2;
	asm ("bsr %1,%0\n" "jnz 1f\n" "bsr %0,%0\n"
		"subl $32,%0\n" "1: addl $32,%0\n":
		"=&q" (x1), "=&q" (x2):
		"1"((int) (w >> 32)), "0"((int) w));
	return x1;
	}

static INLINE int BSF (uint64 w)
	{
	int x1, x2;
	asm ("bsf %0,%0\n" "jnz 1f\n" "bsf %1,%0\n" "jz 1f\n" "addl $32,%0\n" "1:":
		"=&q" (x1), "=&q" (x2):
		"1"((int) (w >> 32)), "0"((int) w));
	return x1;
	}

static INLINE int POPCNT (uint64 w)
	{
	uint32 u, v;
	u = (w & 0xffffffff);
	v = w >> 32;
	u -=((u >> 1) & 0x55555555);
	v -=((v >> 1) & 0x55555555);
	u =(((u >> 2) & 0x33333333) + (u & 0x33333333));
	v =(((v >> 2) & 0x33333333) + (v & 0x33333333));
	u =(((u >> 4) + u) & 0x0f0f0f0f);
	v =(((v >> 4) + v) & 0x0f0f0f0f);
	return((u * 0x01010101) >> 24) +((v * 0x01010101) >> 24);
	}


#endif
