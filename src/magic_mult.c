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

#include "fire.h"
#include "magic_mult.h"

static uint64 BishopAtt(uint64 O, int sq)
    {
    uint64 T = 0;
    int f, r;
    f = File(sq);
    r = Rank(sq);
	for (f--, r--; f >= FA && r >= R1 && !(SqSet[(r << 3) + f] & O); f--, r--)
		T |= SqSet[(r << 3) + f];
	if (f > FA && r > R1)
		T |= SqSet[(r << 3) + f];
	f = File(sq);
	r = Rank(sq);
	for (f++, r--; f <= FH && r >= R1 && !(SqSet[(r << 3) + f] & O); f++, r--)
		T |= SqSet[(r << 3) + f];
	if (f < FH && r > R1)
		T |= SqSet[(r << 3) + f];
	f = File(sq);
	r = Rank(sq);
	for (f++, r++; f <= FH && r <= R8 && !(SqSet[(r << 3) + f] & O); f++, r++)
		T |= SqSet[(r << 3) + f];
	if (f < FH && r < R8)
		T |= SqSet[(r << 3) + f];
	f = File(sq);
	r = Rank(sq);
	for (f--, r++; f >= FA && r <= R8 && !(SqSet[(r << 3) + f] & O); f--, r++)
		T |= SqSet[(r << 3) + f];
	if (f > FA && r < R8)
		T |= SqSet[(r << 3) + f];
	return T;
	}
static uint64 RookAtt(uint64 O, int sq)
	{
	uint64 T = 0;
	int f, r;
	f = File(sq);
	r = Rank(sq);
	for (f--; f >= FA && !(SqSet[(r << 3) + f] & O); f--)
		T |= SqSet[(r << 3) + f];
	if (f > FA)
		T |= SqSet[(r << 3) + f];
	f = File(sq);
	r = Rank(sq);
	for (f++; f <= FH && !(SqSet[(r << 3) + f] & O); f++)
		T |= SqSet[(r << 3) + f];
	if (f < FH)
		T |= SqSet[(r << 3) + f];
	f = File(sq);
	r = Rank(sq);
	for (r++; r <= R8 && !(SqSet[(r << 3) + f] & O); r++)
		T |= SqSet[(r << 3) + f];
	if (r < R8)
		T |= SqSet[(r << 3) + f];
	f = File(sq);
	r = Rank(sq);
	for (r--; r >= R1 && !(SqSet[(r << 3) + f] & O); r--)
		T |= SqSet[(r << 3) + f];
	if (r > R1)
		T |= SqSet[(r << 3) + f];
	return T;
    }
void MagicMultInit()
    {
    int sq, ATT[64], i, j, cnt, b;
    uint64 T;
    BI[A1] = RI[A1] = 0;
    for (sq = A1; sq < H8; sq++)
        {
        BI[sq + 1] = BI[sq] + (1 << BS[sq]);
        RI[sq + 1] = RI[sq] + (1 << RS[sq]);
        }
    for (sq = A1; sq <= H8; sq++)
        {
        BMask[sq] = Diag[sq] & 0x007e7e7e7e7e7e00;
        RMask[sq] = Ortho[sq];
        if (Rank(sq) != R1)
            RMask[sq] &= ~Rank1;
        if (Rank(sq) != R8)
            RMask[sq] &= ~Rank8;
        if (File(sq) != FA)
            RMask[sq] &= ~FileA;
        if (File(sq) != FH)
            RMask[sq] &= ~FileH;
        }
    for (sq = A1; sq <= H8; sq++)
        {
        BishopMM[sq].index = MMDiag + BI[sq];
        RookMM[sq].index = MMOrtho + RI[sq];
        BishopMM[sq].shift = 64 - BS[sq];
        RookMM[sq].shift = 64 - RS[sq];
        RookMM[sq].mult = RMult[sq];
        BishopMM[sq].mult = BMult[sq];
        RookMM[sq].mask = RMask[sq];
        BishopMM[sq].mask = BMask[sq];
        }
    for (sq = A1; sq <= H8; sq++)
        {
        T = BMask[sq];
        cnt = 0;
        while (T)
            {
            b = BSF(T);
            BitClear(b, T);
            ATT[cnt++] = b;
            }
        for (i = 0; i < (1 << cnt); i++)
            {
            T = 0;
            for (j = 0; j < cnt; j++)
                if (i &(1 << j))
                    T |= 1ULL << ATT[j];
            BishopMM[sq].index[(T *BishopMM[sq].mult) >> BishopMM[sq].shift] = BishopAtt(T, sq);
            }
        T = RMask[sq];
        cnt = 0;
        while (T)
            {
            b = BSF(T);
            BitClear(b, T);
            ATT[cnt++] = b;
            }
        for (i = 0; i < (1 << cnt); i++)
            {
            T = 0;
            for (j = 0; j < cnt; j++)
                if (i &(1 << j))
                    T |= 1ULL << ATT[j];
            RookMM[sq].index[(T *RookMM[sq].mult) >> RookMM[sq].shift] = RookAtt(T, sq);
            }
        }
    }
