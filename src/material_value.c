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
#define QueenEnding 1
#define RookEnding 2
#define OppositeBishopEnding 3
#define BishopEnding 4
#define KnightEnding 5
#define BishopKnightEnding 6
#define PawnEnding 7
#define WhiteMinorOnly 8
#define BlackMinorOnly 16
#define BishopKnightMate 32

#define Value4(w, x, y, z) \
	((((uint64) (z)) << 48) + (((uint64) (y)) << 32) + \
	(((uint64) (x)) << 16) + (((uint64) (w)) << 0))
#define Value4Scaled(w, x, y, z, av, sc) \
	Value4 ((w * sc) / av, (x * sc) / av, (y * sc) / av, (z * sc) / av)

#ifdef MatFactors
#define Factor1 80
#define Factor2 90
#define Factor3 110
#define Factor4 120
#define ValueP Value4 (PValue * Factor1 / 100, PValue * Factor2 / 100, PValue * Factor3 / 100, PValue * Factor4 / 100)
#define ValueN Value4 (NValue * Factor1 / 100, NValue * Factor2 / 100, NValue * Factor3 / 100, NValue * Factor4 / 100)
#define ValueB Value4 (BValue * Factor1 / 100, BValue * Factor2 / 100, BValue * Factor3 / 100, BValue * Factor4 / 100)
#define ValueR Value4 (RValue * Factor1 / 100, RValue * Factor2 / 100, RValue * Factor3 / 100, RValue * Factor4 / 100)
#define ValueQ Value4 (QValue * Factor1 / 100, QValue * Factor2 / 100, QValue * Factor3 / 100, QValue * Factor4 / 100)
#define ValueBP Value4 (BPValue * Factor1 / 100, BPValue * Factor2 / 100, BPValue * Factor3 / 100, BPValue * Factor4 / 100)
#else
#define ValueP Value4Scaled (80, 90, 110, 125, 100, PValue)
#define ValueN Value4Scaled (270, 295, 345, 370, 320, NValue)
#define ValueB Value4Scaled (280, 305, 355, 380, 330, BValue)
#define ValueR Value4Scaled (410, 460, 560, 610, 510, RValue)
#define ValueQ Value4Scaled (850, 925, 1075, 1150, 1000, QValue)
#define ValueBP Value4Scaled (30, 40, 60, 70, 50, BPValue)
#endif

#define KnightPawnAdjust Value4 (0, 2, 4, 5)
#define RookPawnAdjust Value4 (5, 4, 2, 0)

#define Phase_Minor (1)
#define PhaseRook (3)
#define PhaseQueen (6)

static int InitFlags(int wP, int wN, int wB, int wBL, int wBD, int wR, int wQ, int bP, int bN, int bB, int bBL,
	int bBD, int bR, int bQ)
    {
    uint8 Flags = ((wN || wB || wQ || wR) << 1) | ((bN || bB || bQ || bR) << 0);
    if (!wQ && !wR && wB + wN == 1 && wP <= 4 && bP <= 4)
        Flags &= 1;
    if (!bQ && !bR && bB + bN == 1 && wP <= 4 && bP <= 4)
        Flags &= 2;
    if (wQ == 1 && bQ == 1 && !wR && !bR && !wB && !bB && !wN && !bN)
        Flags |= QueenEnding << 2;
    if (wR == 1 && bR == 1 && !wQ && !bQ && !wB && !bB && !wN && !bN)
        Flags |= RookEnding << 2;
    if (wB == 1 && bB == 1 && !wQ && !bQ && !wR && !bR && !wN && !bN)
        {
        if ((wBL == 1 && bBD == 1) || (wBD == 1 && bBL == 1))
            Flags |= BishopEnding << 2;
        else
            Flags |= OppositeBishopEnding << 2;
        Flags |= (WhiteMinorOnly | BlackMinorOnly) << 2;
        }
    if (wN == 1 && bN == 1 && !wQ && !bQ && !wR && !bR && !wB && !bB)
        Flags |= KnightEnding << 2;
    if (wN == 1 && bB == 1 && !wQ && !bQ && !wR && !bR && !wB && !bN)
        Flags |= BishopKnightEnding << 2;
    if (wB == 1 && bN == 1 && !wQ && !bQ && !wR && !bR && !bB && !wN)
        Flags |= BishopKnightEnding << 2;
    if (wB == 1 && !wQ && !wR && !wN)
        Flags |= WhiteMinorOnly << 2;
    if (bB == 1 && !bQ && !bR && !bN)
        Flags |= BlackMinorOnly << 2;
    if (wN == 1 && !wQ && !wR && !wB)
        Flags |= WhiteMinorOnly << 2;
    if (bN == 1 && !bQ && !bR && !bB)
        Flags |= BlackMinorOnly << 2;
    if (!wN && !wB && !wR && !wQ && !bN && !bB && !bQ && !bR && wP + bP == 1)
        Flags |= PawnEnding << 2;
    if (wN == 1 && wB == 1 && !wR && !wQ && !wP && !bQ && !bR && !bB && !bN && !bP)
        Flags |= BishopKnightMate << 2;
    if (bN == 1 && bB == 1 && !bR && !bQ && !bP && !wQ && !wR && !wB && !wN && !wP)
        Flags |= BishopKnightMate << 2;
    return Flags;
    }
static int InitTokens(int wP, int wN, int wB, int wBL, int wBD, int wR, int wQ, int bP, int bN, int bB, int bBL,
   int bBD, int bR, int bQ)
    {
    int token = 0x80;
    if (wN == 0 && bN == 0 && wB == 0 && bB == 0 && wR == 0 && bR == 0 && wQ == 1 && bQ == 1)
        token = 0x70 + MAX(wP, bP);
    if (wN == 0 && bN == 0 && wB == 0 && bB == 0 && wQ == 0 && bQ == 0 && wR == 1 && bR == 1)
        token = 0x60 + (MAX(wP, bP) << 1);
    if (wN == 0 && bN == 0 && wR == 0 && bR == 0 && wQ == 0 && bQ == 0 && wB == 1 && bB == 1)
        {
        if ((wBL == 1 && wBD == 0 && bBL == 0 && bBD == 1) || (wBL == 0 && wBD == 1 && bBL == 1 && bBD == 0))
            token = 0x30 + (MAX(wP, bP) << 2);
        else
            token = 0x78 + (MAX(wP, bP) << 1);
        }
    if (wN == 1 && bN == 1 && wR == 0 && bR == 0 && wQ == 0 && bQ == 0 && wB == 0 && bB == 0)
        token = 0x80 + MAX(wP, bP);
    if (wN == 0 && bN == 0 && wR == 0 && bR == 0 && wQ == 0 && bQ == 0 && wB == 0 && bB == 0)
        token = 0xc0 - (MAX(wP, bP) << 3);
    if (wN == 0 && bN == 0 && wB == 1 && bB == 1 && wR == 1 && bR == 1 && wQ == 0 && bQ == 0)
        {
        if ((wBL == 1 && wBD == 0 && bBL == 0 && bBD == 1) || (wBL == 0 && wBD == 1 && bBL == 1 && bBD == 0))
            token = 0x70 + MAX(wP, bP);
        }
    return token;
    }
static int WhiteWeight(int wP, int wN, int wB, int wBL, int wBD, int wR, int wQ, int bP, int bN, int bB, int bBL,
   int bBD, int bR, int bQ)
    {
    int wMinor, bMinor, wPhase, bPhase, wWeight, wValue, bValue;
    wMinor = wB + wN;
    bMinor = bB + bN;
    wPhase = wMinor + (wR << 1) + (wQ << 2);
    bPhase = bMinor + (bR << 1) + (bQ << 2);
	// Yuri Censor eliminated unnecessary multiplication, 4/24/2013:
    // wValue = 3 * (wB + wN) + 5 * wR + 9 * wQ;
    // bValue = 3 * (bB + bN) + 5 * bR + 9 * bQ;
	// Capablanca weights are used here, irrespective of the UCI piece weight settings! Yuri Censor.
	wValue = wMinor + wR + wQ + (wPhase << 1); // Optimized by Yuri Censor, 4/24/2013
	bValue = bMinor + bR + bQ + (bPhase << 1); // Optimized by Yuri Censor, 4/24/2013
    wWeight = 10;
    if (!wP)
    {
        if (wPhase == 1)
            wWeight = 0;
        else if (wPhase == 2)
		{
			if (bPhase == 0)
			{
				if (wN == 2)
				{
					if (bP >= 1)
						wWeight = 3;
					else
						wWeight = 0;
				}
			}
			else if (bPhase == 1)
			{
				wWeight = 1;
				if (bN == 1)
				{
					if (wB == 2)
						wWeight = 8;
					else if (wR == 1)
						wWeight = 2;
				}
            }
			else if (bPhase == 2)
				wWeight = 1;
        }
        else if (wPhase == 3)
		{
			if (wR == 1)
			{
				if (bPhase == 2)
				{
					if (bR == 1)
					{
						if (wN == 1)
							wWeight = 1;
						if (wB == 1)
							wWeight = 1;
					}
					else if (bR == 0)
					{
						wWeight = 2;
						if (wB == 1 && bN == 2)
							wWeight = 6;
						else if (bN == 1)
						{
							if ((wBL == 1 && bBL == 1) || (wBD == 1 && bBD == 1))
								wWeight = 2;
							if ((wBD == 1 && bBL == 1) || (wBL == 1 && bBD == 1))
								wWeight = 7;
						}
					}
				}
				else if (bPhase == 3)
					wWeight = 2;
			}
			else if (wR == 0)
			{
				if (bPhase == 2)
				{
					if (bR == 1)
					{
						if (wN == 2)
							wWeight = 2;
						if (wB == 2)
							wWeight = 7;
					}
					else if (bR == 0)
					{
						wWeight = 2;
						if (wB == 2 && bN == 2)
							wWeight = 4;
					}
				}
				else if (bPhase == 3)
					wWeight = 2;
            }
		}
        else if (wPhase == 4)
		{
			if (wQ)
            {
				if (bPhase == 2)
				{
					if (bN == 2)
						wWeight = 2;
					else if (bN == 1)
						wWeight = 8;
					else if (bN == 0)
						wWeight = 7;
				}
				else if (bPhase == 3)
					wWeight = 1;
				else if (bPhase == 4)
					wWeight = 1;
            }
            if (wR == 2)
            {
				if (bPhase == 2 && bR == 0)
					wWeight = 7;
				else if (bPhase == 3)
					wWeight = 2;
				else if (bPhase == 4)
					wWeight = 1;
            }
			if (wR == 1)
            {
				if (bPhase == 3)
				{
					if (bR == 1)
						wWeight = 3;
					else if (bR == 0)
						wWeight = 2;
				}
				else if (bPhase == 4)
					wWeight = 2;
            }
			else if (wR == 0 && wQ == 0)
            {
				if (bPhase == 3)
				{
					if (bR == 1)
						wWeight = 4;
					else if (bR == 0)
						wWeight = 2;
				}
				else if (bPhase == 4)
				{
					if (bQ)
						wWeight = 8;
					else 
						wWeight = 1;
				}
            }
		}
        else if (wPhase == 5)
		{
			if (wQ)
            {
				if (bPhase == 4)
					wWeight = 2;
				else if (bPhase == 5)
					wWeight = 1;
				if (bPhase == 4 && bR == 2)
                {
					if (wN)
						wWeight = 3;
					if (wB)
						wWeight = 7;
                }
				else if (bPhase == 5)
					wWeight = 1;
            }
			if (wR == 1)
            {
				if (bPhase == 4)
				{
					if (bQ)
						wWeight = 9;
					if (bR == 2)
						wWeight = 7;
					else if (bR == 1)
						wWeight = 3;
					else if (bQ == 0 && bR == 0)
						wWeight = 1;
				}
				else if (bPhase == 5)
					wWeight = 2;
            }
			else if (wR == 2)
            {
				if (bPhase == 4)
				{
					if (bQ) 
					{
						if (wB == 1)
							wWeight = 8;
						if (wN == 1)
							wWeight = 7;
					}
					if (bR == 2)
						wWeight = 3;
					else if (bR == 1)
						wWeight = 2;
					else if (bQ == 0 && bR == 0)
						wWeight = 1;
				}
				else if (bPhase == 5)
					wWeight = 1;
            }
		}
        else if (wPhase == 6)
		{
			if (wQ) 
			{
				if (wR)
				{
					if (bPhase == 4 && bQ == 0 && bR == 0)
						wWeight = 2;
					else if (bPhase == 5 && bQ)
						wWeight = 1;
					if (bPhase == 4)
					{
						if (bR == 1)
							wWeight = 6;
						else if (bR == 2)
							wWeight = 3;
					}
					else if (bPhase == 5 && bR)
						wWeight = 1;
					else if (bPhase == 6)
						wWeight = 1;
				}
				else if (wR == 0)
				{
					if (bPhase == 4 && bQ == 0 && bR == 0)
						wWeight = 5;
					else if (bPhase == 5)
					{
						if (bQ)
							wWeight = 2;
						if (bR == 2)
							wWeight = 2;
						else if (bR == 1)
							wWeight = 1;
					}
					else if (bPhase == 6)
						wWeight = 1;
				}
            }
			else if (wQ == 0)
			{
				if (wR == 2)
				{
					if (bPhase == 5)
					{
						if (bQ)
							wWeight = 7;
						if (bR == 1)
							wWeight = 1;
						else if (bR == 2)
							wWeight = 2;
					}
					else if (bPhase == 6)
						wWeight = 1;
				}
				else if (wR == 1)
				{
					if (bPhase == 5)
					{
						if (bQ)
							wWeight = 9;
						if (bR == 2)
							wWeight = 3;
						else if (bR == 1)
							wWeight = 2;
					}
					else if (bPhase == 6)
					{
						wWeight = 1;
						if (bQ)
						{
							wWeight = 2;
							if (bR)
								wWeight = 4;
						}
					}
				}
            }
		}
        else if (wPhase >= 7)
		{
            if (wValue > bValue + 4)
                wWeight = 9;
            else if (wValue == bValue + 4)
                wWeight = 7;
            else if (wValue == bValue + 3)
                wWeight = 4;
            else if (wValue == bValue + 2)
                wWeight = 2;
            else if (wValue < bValue + 2)
                wWeight = 1;
        }
    }
    else if (wP == 1)
    {
        if (bPhase == 1)
		{
			if (wPhase == 1)
				wWeight = 3;
			else if (wPhase == 2)
			{
				if (wN == 2)
				{
					if (bP == 0)
						wWeight = 3;
					else
						wWeight = 5;
				}
				if (wR == 1)
					wWeight = 7;
			}
		}
        else if (bPhase == 2)
		{
			if (bR == 1 && wPhase == 2 && wR == 1)
				wWeight = 8;
			else if (bR == 0 && wPhase == 2)
				wWeight = 4;
		}
        else if (bPhase >= 3)
		{
			if (wPhase == bPhase)
			{
				if (bMinor > 0)
					wWeight = 3;
				else if (bMinor == 0)
					wWeight = 5;
				if (bPhase == 4 && bQ == 1)
					wWeight = 7;
			}
        }
	}
    if (wQ == 1 && wPhase == 4 && bPhase >= 2 && bP >= 1) // Added on 3/29/2013, recommended by Jose Maria Velasco
        wWeight = 5;
    return wWeight;
    }
static int BlackWeight(int wP, int wN, int wB, int wBL, int wBD, int wR, int wQ, int bP, int bN, int bB, int bBL,
   int bBD, int bR, int bQ)
    {
    int wMinor, bMinor, wPhase, bPhase, bWeight, wValue, bValue;
    wMinor = wB + wN;
    bMinor = bB + bN;
    wPhase = wMinor + (wR << 1) + (wQ << 2);
    bPhase = bMinor + (bR << 1) + (bQ << 2);
	// Yuri Censor eliminated unnecessary multiplication, 4/24/2013:
    // wValue = 3 * (wB + wN) + 5 * wR + 9 * wQ;
    // bValue = 3 * (bB + bN) + 5 * bR + 9 * bQ;
	// Capablanca weights are used here, irrespective of the UCI piece weight settings! Yuri Censor.
	wValue = wMinor + wR + wQ + (wPhase << 1); // Optimized by Yuri Censor, 4/24/2013
	bValue = bMinor + bR + bQ + (bPhase << 1); // Optimized by Yuri Censor, 4/24/2013
    bWeight = 10;
    if (!bP)
	{
        if (bPhase == 1)
            bWeight = 0;
        else if (bPhase == 2)
        {
            if (wPhase == 0)
            {
                if (bN == 2)
                {
                    if (bP >= 1)
                        bWeight = 3;
                    else
                        bWeight = 0;
                }
            }
            else if (wPhase == 1)
            {
                bWeight = 1;
                if (wN == 1)
				{
					if (bB == 2)
						bWeight = 8;
					else if (bR == 1)
						bWeight = 2;
				}
            }
            else if (wPhase == 2)
                bWeight = 1;
        }
        else if (bPhase == 3)
		{
			if (bR == 1)
            {
				if (wPhase == 2)
				{
					if (wR == 1)
					{
						if (bN == 1)
							bWeight = 1;
						if (bB == 1)
							bWeight = 1;
					}
					else if (wR == 0)
					{
						bWeight = 2;
						if (bB == 1 && wN == 2)
							bWeight = 6;
						else if (wN == 1)
						{
							if ((bBL == 1 && wBL == 1) || (bBD == 1 && wBD == 1))
								bWeight = 2;
							if ((bBD == 1 && wBL == 1) || (bBL == 1 && wBD == 1))
								bWeight = 7;
						}
					}
                }
				else if (wPhase == 3)
					bWeight = 2;
            }
			else if (bR == 0)
            {
				if (wPhase == 2)
				{
					if (wR == 1)
					{
						if (bN == 2)
							bWeight = 2;
						if (bB == 2)
							bWeight = 7;
					}
					else if (wR == 0)
					{
						bWeight = 2;
						if (bB == 2 && wN == 2)
							bWeight = 4;
					}
                }
				else if (wPhase == 3)
					bWeight = 2;
            }
		}
        else if (bPhase == 4)
		{
			if (bQ)
            {
				if (wPhase == 2)
				{
					if (wN == 2)
						bWeight = 2;
					else if (wN == 1)
						bWeight = 8;
					else if (wN == 0)
						bWeight = 7;
				}
				else if (wPhase == 3)
					bWeight = 1;
				else if (wPhase == 4)
					bWeight = 1;
            }
			if (bR == 2)
            {
				if (wPhase == 2 && wR == 0)
					bWeight = 7;
				else if (wPhase == 3)
					bWeight = 2;
				else if (wPhase == 4)
					bWeight = 1;
            }
			else if (bR == 1)
            {
				if (wPhase == 3)
				{
					if (wR == 1)
						bWeight = 3;
					else if (wR == 0)
						bWeight = 2;
				}
				else if (wPhase == 4)
					bWeight = 2;
            }
			else if (bR == 0 && bQ == 0)
            {
				if (wPhase == 3)
				{
					if (wR == 1)
						bWeight = 4;
					else if (wR == 0)
						bWeight = 2;
				}
				else if (wPhase == 4)
				{
					if (wQ)
						bWeight = 8;
					else 
						bWeight = 1;
				}
            }
		}
        if (bPhase == 5)
		{
			if (bQ)
            {
				if (wPhase == 4)
					bWeight = 2;
				else if (wPhase == 5)
					bWeight = 1;
				if (wPhase == 4 && wR == 2)
                {
					if (bN)
						bWeight = 3;
					if (bB)
						bWeight = 7;
                }
				else if (wPhase == 5)
					bWeight = 1;
            }
			if (bR == 1)
            {
				if (wPhase == 4)
				{
					if (wQ)
						bWeight = 9;
					if (wR == 2)
						bWeight = 7;
					else if (wR == 1)
						bWeight = 3;
					else if (wQ == 0 && wR == 0)
						bWeight = 1;
				}
				else if (wPhase == 5)
					bWeight = 2;
            }
			else if (bR == 2)
            {
				if (wPhase == 4)
				{
					if (wQ)
					{
						if (bB == 1)
							bWeight = 8;
						if (bN == 1)
							bWeight = 7;
					}
					if (wR == 2)
						bWeight = 3;
					else if (wR == 1)
						bWeight = 2;
					else if (wQ == 0 && wR == 0)
						bWeight = 1;
				}
				else if (wPhase == 5)
					bWeight = 1;
            }
		}
        else if (bPhase == 6)
		{
			if (bQ)
			{
				if (bR)
				{
					if (wPhase == 4 && wQ == 0 && wR == 0)
						bWeight = 2;
					else if (wPhase == 5 && wQ)
						bWeight = 1;
					if (wPhase == 4)
					{
						if (wR == 1)
							bWeight = 6;
						else if (wR == 2)
							bWeight = 3;
					}
					else if (wPhase == 5 && wR)
						bWeight = 1;
					else if (wPhase == 6)
						bWeight = 1;
				}
				else if (bR == 0)
				{
					if (wPhase == 4 && wQ == 0 && wR == 0)
						bWeight = 5;
					else if (wPhase == 5)
					{
						if (wQ)
							bWeight = 2;
						if (wR == 2)
							bWeight = 2;
						else if (wR == 1)
							bWeight = 1;
					}
					else if (wPhase == 6)
						bWeight = 1;
				}
            }
			else if (bQ == 0)
			{
				if (bR == 2)
				{
					if (wPhase == 5)
					{
						if (wQ)
							bWeight = 7;
						if (wR == 1)
							bWeight = 1;
						else if (wR == 2)
							bWeight = 2;
					}
					else if (wPhase == 6)
						bWeight = 1;
				}
				else if (bR == 1)
				{
					if (wPhase == 5)
					{
						if (wQ)
							bWeight = 9;
						if (wR == 2)
							bWeight = 3;
						else if (wR == 1)
							bWeight = 2;
					}
					else if (wPhase == 6)
					{
						bWeight = 1;
						if (wQ)
						{
							bWeight = 2;
							if (wR)
								bWeight = 4;
						}
					}
				}
            }
		}
        else if (bPhase >= 7)
        {
            if (bValue > wValue + 4)
                bWeight = 9;
            else if (bValue == wValue + 4)
                bWeight = 7;
            else if (bValue == wValue + 3)
                bWeight = 4;
            else if (bValue == wValue + 2)
                bWeight = 2;
            else if (bValue < wValue + 2)
                bWeight = 1;
        }
    }
    else if (bP == 1)
    {
        if (wPhase == 1)
        {
            if (bPhase == 1)
                bWeight = 3;
            else if (bPhase == 2)
			{
				if (bN == 2)
                {
					if (wP == 0)
						bWeight = 3;
					else
						bWeight = 5;
                }
				if (bR == 1)
					bWeight = 7;
            }
		}
        else if (wPhase == 2)
		{
			if (wR == 1 && bPhase == 2 && bR == 1)
				bWeight = 8;
			else if (wR == 0 && bPhase == 2)
				bWeight = 4;
		}
        else if (wPhase >= 3)
		{
			if (bPhase == wPhase)
			{
				if (wMinor > 0)
					bWeight = 3;
				else if (wMinor == 0)
					bWeight = 5;
				if (wPhase == 4 && wQ == 1)
					bWeight = 7;
			}
		}  
    }
	if (bQ == 1 && bPhase == 4 && wPhase >= 2 && wP >= 1) // Added on 3/29/2013, recommended by Jose Maria Velasco
		bWeight = 5;
    return bWeight;
    }

static uint64 ComputeValue(int wP, int wN, int wB, int wBL, int wBD, int wR, int wQ, int bP, int bN, int bB, int bBL,
   int bBD, int bR, int bQ)
    {
    uint64 va = 0;
    int wMinor = wN + wB;
    int bMinor = bN + bB;

    va += ((wB >> 1) - (bB >> 1)) * ValueBP;
    va += (wP - bP) * ValueP;
    va += (wN - bN) * ValueN;
    va += (wB - bB) * ValueB;
    va += (wR - bR) * ValueR;
    va += (wQ - bQ) * ValueQ;

    if (wR == 2)
        va -= Value4(16, 20, 28, 32);
    if (bR == 2)
        va += Value4(16, 20, 28, 32);
    if (wQ + wR >= 2)
        va -= Value4(8, 10, 14, 16);
    if (bQ + bR >= 2)
        va += Value4(8, 10, 14, 16);
    if (wMinor > bMinor)
        va += Value4(20, 15, 10, 5);
    if (bMinor > wMinor)
        va -= Value4(20, 15, 10, 5);
    va -= (wP - 5) * wR * RookPawnAdjust;
    va += (wP - 5) * wN * KnightPawnAdjust;
    va += (bP - 5) * bR * RookPawnAdjust;
    va -= (bP - 5) * bN * KnightPawnAdjust;
    va -= (wP - 5) * (wB >> 1) * Value4(0, 1, 2, 3);
    va += (bP - 5) * (bB >> 1) * Value4(0, 1, 2, 3);
    if (wB == 2 && bMinor == 0)
        va += Value4(5, 5, 5, 5);
    if (bB == 2 && wMinor == 0)
        va -= Value4(5, 5, 5, 5);
    return va;
    }

static void CalculateMaterialValue(int c)
    {
    int wQ, bQ, wR, bR, wBL, bBL, wBD, bBD, wN, bN, wP, bP, n, Value, wB, bB;
    int wt, wWeight, bWeight, phase, va1, va2, va3, va4;
    uint64 va;
    n = c;
    wQ = n & 1;
    n /= 2;
    bQ = n & 1;
    n /= 2;
    wR = n % 3;
    n /= 3;
    bR = n % 3;
    n /= 3;
    wBL = n & 1;
    n /= 2;
    wBD = n & 1;
    n /= 2;
    bBL = n & 1;
    n /= 2;
    bBD = n & 1;
    n /= 2;
    wN = n % 3;
    n /= 3;
    bN = n % 3;
    n /= 3;
    wP = n % 9;
    n /= 9;
    bP = n % 9;
    wB = wBL + wBD;
    bB = bBL + bBD;
    va = ComputeValue(wP, wN, wB, wBL, wBD, wR, wQ, bP, bN, bB, bBL, bBD, bR, bQ);
    phase = Phase_Minor * (wN + wB + bN + bB) + PhaseRook * (wR + bR) + PhaseQueen * (wQ + bQ);
    va1 = va & 0xffff;
    va2 = ((va >> 16) & 0xffff) + (va1 > 0x8000);
    va1 = (sint16)va1;
    va3 = ((va >> 32) & 0xffff) + (va2 > 0x8000);
    va2 = (sint16)va2;
    va4 = ((va >> 48) & 0xffff) + (va3 > 0x8000);
    va3 = (sint16)va3;
    va4 = (sint16)va4;
    if (phase < 8)
        {
        va4 *= 8 - phase;
        va3 *= phase;
        va = va3 + va4;
        Value =(((int)va) >> 3);
        }
    else if (phase < 24)
        {
        va3 *= 24 - phase;
        va2 *= phase - 8;
        va = va2 + va3;
        Value =((int)va) >> 4;
        }
    else
        {
        va2 *= 32 - phase;
        va1 *= phase - 24;
        va = va1 + va2;
        Value =(((int)va) >> 3);
        }
    wWeight = WhiteWeight(wP, wN, wB, wBL, wBD, wR, wQ, bP, bN, bB, bBL, bBD, bR, bQ);
    bWeight = BlackWeight(wP, wN, wB, wBL, wBD, wR, wQ, bP, bN, bB, bBL, bBD, bR, bQ);
    if (Value > 0)
        wt = wWeight;
    else
        wt = bWeight;
    Value *= wt;
    Value /= 10;
    Value *= (MaterialWeight << 10) / 100;
    Value >>= 10;
    Material[c].Value = Value;
    Material[c].token = InitTokens(wP, wN, wB, wBL, wBD, wR, wQ, bP, bN, bB, bBL, bBD, bR, bQ);
    Material[c].flags = InitFlags(wP, wN, wB, wBL, wBD, wR, wQ, bP, bN, bB, bBL, bBD, bR, bQ);
    }
int InitMaterialValue()
    {
    int c;
    SetValueArray();
    for (c = 0; c < 419904; c++)
        CalculateMaterialValue(c);
    EvalHashClear();
    return false;
    }
