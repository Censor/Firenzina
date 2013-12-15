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
#if defined(_WIN32) && defined(_MSC_VER)

#define UINT64 unsigned long long int // Added by Yuri Censor for Firenzina, 3/4/2013
static _inline int BSF(UINT64 x) // Modified by Yuri Censor for Firenzina, 3/4/2013: _inline instead of INLINE
    {   // Reason: Intel 13.0 compiler seems happy this way
		{
		_asm
            {
            mov eax, dword ptr x[0]
			test eax, eax
			jz f_hi
			bsf eax, eax
			jmp f_ret
			f_hi: bsf eax, dword ptr x[4]
			add eax, 20h
			f_ret:
			}
		}
    }
static _inline int BSR(UINT64 x) // Modified by Yuri Censor for Firenzina, 3/4/2013: _inline instead of INLINE
    {   // Reason: Intel 13.0 compiler seems happy this way
		{
        _asm
            {
            mov eax, dword ptr x[4]
			test eax, eax
			jz l_lo
			bsr eax, eax
			add eax, 20h
			jmp l_ret
			l_lo: bsr eax, dword ptr x[0]
			l_ret:
            }
		}
    }
 static _inline int PopcntHard (UINT64 w) // Modified by Yuri Censor for Firenzina, 3/4/2013: _inline instead of INLINE
	{   // Reason: Intel 13.0 compiler seems happy this way   
		{
		_asm
			{
			popcnt edx,dword ptr w[0] 
			popcnt eax,dword ptr w[4] 
			add eax,edx  
			}
		}
	}

static _inline int PopcntEmul (UINT64 v) // Modified by Yuri Censor for Firenzina, 3/4/2013: _inline instead of INLINE
	{    // Reason: Intel 13.0 compiler seems happy this way
	unsigned int v1, v2;
	v1 = (unsigned int) (v & 0xFFFFFFFF);
	v1 -= (v1 >> 1) & 0x55555555;
	v1 = (v1 & 0x33333333) + ((v1 >> 2) & 0x33333333);
	v1 = (v1 + (v1 >> 4)) & 0x0F0F0F0F;
	v2 = (unsigned int) (v >> 32);
	v2 -= (v2 >> 1) & 0x55555555;
	v2 = (v2 & 0x33333333) + ((v2 >> 2) & 0x33333333);
	v2 = (v2 + (v2 >> 4)) & 0x0F0F0F0F;
	return ((v1 * 0x01010101) >> 24) + ((v2 * 0x01010101) >> 24);
	}

#else

#include "bits.h"

#endif
