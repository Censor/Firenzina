<<<<<<< HEAD:src/robbo_init.c
/*
Firenzina is a UCI chess playing engine by Kranium (Norman Schmidt)
Firenzina is based on Ippolit source code: http://ippolit.wikispaces.com/
authors: Yakov Petrovich Golyadkin, Igor Igorovich Igoronov,
and Roberto Pescatore copyright: (C) 2009 Yakov Petrovich Golyadkin
date: 92th and 93rd year from Revolution
owners: PUBLICDOMAIN (workers)
dedication: To Vladimir Ilyich
=======
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
>>>>>>> Linux/Housekeeping/Bug fixes/Extend xTreme/Defs:Firenzina/robbo_init.c
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

#include "fire.h"
#include "robbo_totalbase.h"
static void InitKingNormal()
    {
    int a, b, s = 0;
    for (a = A1; a <= H8; a++)
        for (b = A1; b <= H8; b++)
            {
            if (a == b || AttK[a] & SqSet[b])
                {
                KingNormal[(b << 6) + a] = -1;
                continue;
                }
            if (!(Triangle & SqSet[a]))
                {
                KingNormal[(b << 6) + a] = -1;
                continue;
                }
            if (Margin & SqSet[a] && !(TriMar & SqSet[b]))
                {
                KingNormal[(b << 6) + a] = -1;
                continue;
                }
            KingNormal[(b << 6) + a] = s++;
            }
    }
static void InitKingPawn()
    {
    int a, b, s = 0;
    for (a = A1; a <= H8; a++)
        for (b = A1; b <= H8; b++)
            {
            if (a == b || AttK[a] & SqSet[b])
                {
                KingPawn[(b << 6) + a] = -1;
                continue;
                }
            if (File(a) >= FE)
                {
                KingPawn[(b << 6) + a] = -1;
                continue;
                }
            KingPawn[(b << 6) + a] = s++;
            }
    }
static void InitInverse()
    {
    int wK, bK;
    for (wK = A1; wK <= H8; wK++)
        for (bK = A1; bK <= H8; bK++)
            {
            if (KingNormal[(wK << 6) + bK] != -1)
                InverseNormalKing[KingNormal[(wK << 6) + bK]] = (wK << 6) | bK;
            if (KingPawn[(wK << 6) + bK] != -1)
                InversePawnKing[KingPawn[(wK << 6) + bK]] = (wK << 6) | bK;
            }
    }
static void InitDoubleSquare()
    {
    int a, b, s = 0;
    for (a = A1; a <= H8; a++)
        for (b = A1; b < a; b++)
            Sq2Normal[s++] = a | (b << 6);
    s = 0;
    for (a = A2; a <= H7; a++)
        for (b = A2; b < a; b++)
            Sq2Pawn[s++] = a | (b << 6);
    s = 0;
    for (a = A2; a <= H6; a++)
        for (b = A2; b < a; b++)
            Sq2Blocked[s++] = a | (b << 6);
    }
static void InitTriple3()
    {
    int a, b, c, s = 0;
    for (a = A1; a <= H8; a++)
        for (b = A1; b < a; b++)
            for (c = A1; c < b; c++)
                Sq3normal[s++] = a | (b << 6) | (c << 12);
    s = 0;
    for (a = A2; a <= H7; a++)
        for (b = A2; b < a; b++)
            for (c = A2; c < b; c++)
                Sq3Pawn[s++] = a | (b << 6) | (c << 12);
    s = 0;
    for (a = A2; a <= H6; a++)
        for (b = A2; b < a; b++)
            for (c = A2; c < b; c++)
                Sq3blocked[s++] = a | (b << 6) | (c << 12);
    }
static void InitQuadruple4()
    {
    int a, b, c, d, s = 0;
    for (a = A1; a <= H8; a++)
        for (b = A1; b < a; b++)
            for (c = A1; c < b; c++)
                for (d = A1; d < c; d++)
                    Sq4normal[s++] = a | (b << 6) | (c << 12) | (d << 18);
    s = 0;
    for (a = A2; a <= H7; a++)
        for (b = A2; b < a; b++)
            for (c = A2; c < b; c++)
                for (d = A2; d < c; d++)
                    Sq4Pawn[s++] = a | (b << 6) | (c << 12) | (d << 18);
    s = 0;
    for (a = A2; a <= H6; a++)
        for (b = A2; b < a; b++)
            for (c = A2; c < b; c++)
                for (d = A2; d < c; d++)
                    Sq4blocked[s++] = a | (b << 6) | (c << 12) | (d << 18);
    }
static void InitSpear()
    {
    int wK, bK, K1, K2, u, sq;
    for (wK = A1; wK <= H8; wK++)
        for (bK = A1; bK <= H8; bK++)
            {
            K1 = bK;
            K2 = wK;
            u = 0;
            if (File(K1) >= FE)
                {
                u = 4;
                SpearNorm(4, K1);
                SpearNorm(4, K2);
                }
            if (Rank(K1) >= R5)
                {
                u |= 2;
                SpearNorm(2, K1);
                SpearNorm(2, K2);
                }
            if (Rank(K1) == File(K1) && Rank(K2) > File(K2))
                u |= 1;
            else if (Rank(K1) > File(K1))
                u |= 1;
            oSpear[wK][bK] = u;
            }
    for (sq = A1; sq <= H8; sq++)
        {
        SpearPawn[0][sq] = sq;
        SpearPawn[1][sq] = SpearVert[sq];
        SpearPawn[2][sq] = SpearHoriz[sq];
        SpearPawn[3][sq] = (FH - File(sq)) + ((R8 - Rank(sq)) << 3);
        }
    }
static void InitSpearNormal()
    {
    int i, sq, w;
    for (i = 0; i < 8; i++)
        for (sq = A1; sq <= H8; sq++)
            {
            w = sq;
            if (i & 4)
                w = SpearHoriz[w];
            if (i & 2)
                w = SpearVert[w];
            if (i & 1)
                w = SpearDiag[w];
            SpearNorm[i][sq] = w;
            }
    }
static void InitDoubleSame()
    {
    int i, j;
    for (i = A1; i <= H8; i++)
        for (j = A1; j <= H8; j++)
            {
            SameO2[i][j] = (i > j) ? ((i * (i + 1)) >> 1) + j - i : ((j * (j + 1)) >> 1) + i - j;
            }
    }
static void InitTriple()
    {
    int i, j, k, mas, mez, min;
    for (i = A1; i <= H8; i++)
        for (j = A1; j <= H8; j++)
            for (k = A1; k <= H8; k++)
                {
                mas = MAX(MAX(i, j), k);
                min = MIN(MIN(i, j), k);
                if (i == mas)
                    mez = MAX(j, k);
                if (j == mas)
                    mez = MAX(i, k);
                if (k == mas)
                    mez = MAX(i, j);
                SameO3[i][j][k] = (mas * (mas - 1) * (mas - 2)) / 6 + ((mez * (mez + 1)) >> 1) - mez + min;
                }
    }
static void InitQuadruple()
    {
    int i, j, k, l, mas, mez2, mez3, min;
    for (i = A1; i <= H8; i++)
        for (j = A1; j <= H8; j++)
            for (k = A1; k <= H8; k++)
                for (l = A1; l <= H8; l++)
                    {
                    mas = MAX(MAX(i, j), MAX(k, l));
                    min = MIN(MIN(i, j), MIN(k, l));
                    if (i == mas)
                        mez2 = MAX(MAX(j, k), l);
                    if (j == mas)
                        mez2 = MAX(MAX(i, k), l);
                    if (k == mas)
                        mez2 = MAX(MAX(i, j), l);
                    if (l == mas)
                        mez2 = MAX(MAX(i, j), k);
                    if (i == min)
                        mez3 = MIN(MIN(j, k), l);
                    if (j == min)
                        mez3 = MIN(MIN(i, k), l);
                    if (k == min)
                        mez3 = MIN(MIN(i, j), l);
                    if (l == min)
                        mez3 = MIN(MIN(i, j), k);
                    SameO4[i][j][k][l] =
                       (mas * (mas - 1) * (mas - 2) * (mas - 3)) / 24 + (mez2 * (mez2 - 1) * (mez2 - 2)) / 6
                          + (mez3 * ((mez3 - 1)) >> 1) + min;
                    }
    }
static void StackSpear()
    {
    int pi, sq;
    for (pi = 0; pi < 16; pi++)
        ColScamb[pi] = pi ^ 8;
    ColScamb[0] = 0;
    ColScamb[8] = 8;
    for (sq = A1; sq <= H8; sq++)
        {
        SpearHoriz[sq] = (FH - File(sq)) + (Rank(sq) << 3);
        SpearVert[sq] = File(sq) + ((R8 - Rank(sq)) << 3);
        SpearDiag[sq] = (File(sq) << 3) + Rank(sq);
        }
    }
void InitTotalBaseUtility(bool Build)
    {
    StackSpear();
    strcpy(TotalBaseDir, "RobboTotalBase");
    strcpy(CompressPrefix, "iCOMP.");
    InitKingNormal();
    InitKingPawn();
    InitInverse();
    InitDoubleSquare();
    InitTriple3();
    if (Build)
        InitQuadruple4();
    InitSpearNormal();
    InitSpear();
    InitDoubleSame();
    InitTriple();
    if (Build)
        InitQuadruple();
    }
