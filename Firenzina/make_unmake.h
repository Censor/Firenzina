/*******************************************************************************
Firenzina is a UCI chess playing engine by Yuri Censor (Dmitri Gusev).
Rededication: To the memories of Giovanna Tornabuoni and Domenico Ghirlandaio.
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

#define Bitboard2(x, y) (1ULL << (x))|(1ULL << (y))
#define F1H1 Bitboard2 (F1, H1)
#define F1H1Left90 Bitboard2 (47, 63)
#define F1H1Left45 Bitboard2 (20, 35)
#define F1H1Right45 Bitboard2 (3, 0)
#define A1D1 Bitboard2 (A1, D1)
#define A1D1Left90 Bitboard2 (7, 31)
#define A1D1Left45 Bitboard2 (0, 9)
#define A1D1Right45 Bitboard2 (28, 10)
#define F8H8 Bitboard2 (F8, H8)
#define F8H8Left90 Bitboard2 (40, 56)
#define F8H8Left45 Bitboard2 (58, 63)
#define F8H8Right45 Bitboard2 (48, 35)
#define A8D8 Bitboard2 (A8, D8)
#define A8D8Left90 Bitboard2 (0, 24)
#define A8D8Left45 Bitboard2 (28, 49)
#define A8D8Right45 Bitboard2 (63, 57)

#define ClearOccupied(M, x) { Position->OccupiedBW &= M; }
#define SetOccupied(M, x) { Position->OccupiedBW |= M; }
