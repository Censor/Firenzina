/*
Firenzina is a UCI chess playing engine by Kranium (Norman Schmidt)
Firenzina is based on Ippolit source code: http://ippolit.wikispaces.com/
authors: Yakov Petrovich Golyadkin, Igor Igorovich Igoronov,
and Roberto Pescatore copyright: (C) 2009 Yakov Petrovich Golyadkin
date: 92th and 93rd year from Revolution
owners: PUBLICDOMAIN (workers)
dedication: To Vladimir Ilyich
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
*/

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


static INLINE int PopcntHard (UINT64 x)
	{
  	_asm
    	{
      	popcnt rax, x
    	}
	}
static INLINE int PopcntEmul (uint64 w)
	{
  	w = w - ((w >> 1) & 0x5555555555555555ULL);
  	w = (w & 0x3333333333333333ULL) + ((w >> 2) & 0x3333333333333333ULL);
  	w = (w + (w >> 4)) & 0x0f0f0f0f0f0f0f0fULL;
  	return (w * 0x0101010101010101ull) >> 56;
	}

#endif
