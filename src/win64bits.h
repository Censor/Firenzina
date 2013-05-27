/*******************************************************************************
Firenzina is a UCI chess playing engine by
Yuri Censor (Dmitri Gusev) and ZirconiumX (Matthew Brades).
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

#if defined(__GNUC__)

#include "bits.h"

#elif defined(_WIN64) && defined(_MSC_VER)

static INLINE int BSF(UINT64 x)
    {
    int r;
    _BitScanForward64(&r, x);
    return r;
    }
static INLINE int BSR(UINT64 x)
    {
    int r;
    _BitScanReverse64(&r, x);
    return r;
    }
#ifdef HasIntrinsics
#include "nmmintrin.h"
#endif
static INLINE long long PopcntHard (unsigned long long x)
	{
#ifdef HasIntrinsics
  	return _mm_popcnt_u64(x);
#else
	_asm
    {
      popcnt rax, x
    }
#endif
	}
static INLINE long long PopcntEmul (unsigned long long w)
	{
  	w = w - ((w >> 1) & 0x5555555555555555ULL);
  	w = (w & 0x3333333333333333ULL) + ((w >> 2) & 0x3333333333333333ULL);
  	w = (w + (w >> 4)) & 0x0f0f0f0f0f0f0f0fULL;
  	return (w * 0x0101010101010101ull) >> 56;
	}

#endif



