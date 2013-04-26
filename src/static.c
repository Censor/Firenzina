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

#include "fire.h"
#define Combine(x,y)(((x) << 16) +(y))
static void BlackFromWhite()
    {
    int sq, pi;
    for (sq = A1; sq <= H8; sq++)
        for (pi = bEnumP; pi <= bEnumQ; pi++)
            PST(pi, sq) = -PST(pi - 8, H8 - sq);
    }
static int ValueOpenP(int sq)
    {
    int C[8] =
        {
        -20, -8, -2, 5, 5, -2, -8, -20
        };
    int T[8] =
        {
        0, -3, -2, -1, 1, 2, 3, 0
        };
    int file = File(sq), rank = Rank(sq);
    if (rank == R1 || rank == R8)
        return 0;
    return C[file] + T[rank];
    }
static int ValueEndP(int sq)
    {
    int C[8] =
        {
        -4, -6, -8, -10, -10, -8, -6, -4
        };
    int T[8] =
        {
        0, -3, -3, -2, -1, 0, 2, 0
        };
    int file = File(sq), rank = Rank(sq);
    if (rank == R1 || rank == R8)
        return 0;
    return C[file] + T[rank];
    }
static int ValueOpenN(int sq)
    {
    int T[8] =
        {
        -32, -10, 6, 15, 21, 19, 10, -11
        };
    int C[8] =
        {
        -26, -10, 1, 5, 5, 1, -10, -26
        };
    int rank = Rank(sq), file = File(sq);
    return(sq == A8 || sq == H8) ? (-120) : (T[rank] + C[file]);
    }
static int ValueEndN(int sq)
    {
    int V[8] =
        {
        2, 1, 0, -1, -2, -4, -7, -10
        };
    int T[8] =
        {
        -10, -5, -2, 1, 3, 5, 2, -3
        };
    int C[8] =
        {
        -4, -1, 2, 4, 4, 2, -1, -4
        };
    int rank = Rank(sq), file = File(sq);
    int d = ABS(file - rank), e = ABS(file + rank - 7);
    return V[d] + V[e] + T[rank] + C[file];
    }
static int ValueOpenB(int sq)
    {
    int V[8] =
        {
        10, 5, 1, -3, -5, -7, -8, -12
        };
    int W[8] =
        {
        -5, 0, 0, 0, 0, 0, 0, 0
        };
    int rank = Rank(sq), file = File(sq);
    int d = ABS(file - rank), e = ABS(file + rank - 7);
    return V[d] + V[e] + W[rank];
    }
static int ValueEndB(int sq)
    {
    int V[8] =
        {
        3, 2, 0, 0, -2, -2, -3, -3
        };
    int rank = Rank(sq), file = File(sq);
    int d = ABS(file - rank), e = ABS(file + rank - 7);
    return V[d] + V[e];
    }
static int ValueOpenR(int sq)
    {
    int C[8] =
        {
        -4, 0, 4, 8, 8, 4, 0, -4
        };
    return C[File(sq)];
    }
static int ValueEndR(int sq)
    {
    int T[8] =
        {
        0, 0, 0, 0, 1, 1, 1, -2
        };
    return T[Rank(sq)];
    }
static int ValueOpenQ(int sq)
    {
    int V[8] =
        {
        3, 2, 1, 0, -2, -4, -7, -10
        };
    int W[8] =
        {
        -2, 0, 1, 2, 2, 1, 0, -2
        };
    int rank = Rank(sq), file = File(sq);
    int d = ABS(file - rank), e = ABS(file + rank - 7);
    return V[d] + V[e] + W[rank] + W[file] - 5 * (rank == R1);
    }
static int ValueEndQ(int sq)
    {
    int V[8] =
        {
        1, 0, -1, -3, -4, -6, -8, -12
        };
    int W[8] =
        {
        -2, 0, 1, 2, 2, 1, 0, -2
        };
    int rank = Rank(sq), file = File(sq);
    int d = ABS(file - rank), e = ABS(file + rank - 7);
    return V[d] + V[e] + W[rank] + W[file];
    }
static int ValueOpenK(int sq)
    {
    int T[8] =
        {
        4, 1, -2, -5, -10, -15, -25, -35
        };
    int C[8] =
        {
        40, 45, 15, -5, -5, 15, 45, 40
        };
    int rank = Rank(sq), file = File(sq);
    return T[rank] + C[file];
    }
static int ValueEndK(int sq)
    {
    int V[8] =
        {
        2, 0, -2, -5, -8, -12, -20, -30
        };
    int T[8] =
        {
        -30, -5, 0, 5, 10, 5, 0, -10
        };
    int C[8] =
        {
        -15, 0, 10, 15, 15, 10, 0, -15
        };
    int rank = Rank(sq), file = File(sq);
    int d = ABS(file - rank), e = ABS(file + rank - 7);
    return V[d] + V[e] + T[rank] + C[file];
    }

void InitStatic()
    {
    int sq;
    for (sq = A1; sq <= H8; sq++)
        PST(wEnumP, sq) = Combine(ValueOpenP(sq), ValueEndP(sq));
    for (sq = A1; sq <= H8; sq++)
        PST(wEnumN, sq) = Combine(ValueOpenN(sq), ValueEndN(sq));
    for (sq = A1; sq <= H8; sq++)
        PST(wEnumBL, sq) = PST(wEnumBD, sq) = Combine(ValueOpenB(sq), ValueEndB(sq));
    for (sq = A1; sq <= H8; sq++)
        PST(wEnumR, sq) = Combine(ValueOpenR(sq), ValueEndR(sq));
    for (sq = A1; sq <= H8; sq++)
        PST(wEnumQ, sq) = Combine(ValueOpenQ(sq), ValueEndQ(sq));
    for (sq = A1; sq <= H8; sq++)
        PST(wEnumK, sq) = Combine(ValueOpenK(sq), ValueEndK(sq));
    BlackFromWhite();
    }
