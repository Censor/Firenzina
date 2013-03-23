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

#include "fire.h"

#define BishopKnightMate (Position->Dyn->flags & 128)
#define MaxDist(i,j) (MAX (FileDistance (i, j), RankDistance (i, j)))
#define MinDist(i,j) (MIN (FileDistance (i, j), RankDistance (i, j)))
#define WhiteMinorOnly (Position->Dyn->flags & 32)
#define WhiteHasPiece (Position->Dyn->flags & 2)
#define BlackMinorOnly (Position->Dyn->flags & 64)
#define BlackHasPiece (Position->Dyn->flags & 1)

int EvalEnding(typePos *Position, int Value, uint64 wPatt, uint64 bPatt)
    {

    if (BishopKnightMate)
        {
        if (Value > 0)
            {
            if (wBitboardBL)
                Value -= 20 * MIN(MaxDist(A8, Position->bKsq), MaxDist(H1, Position->bKsq))
                   + 10 * MIN(MinDist(A8, Position->bKsq), MinDist(H1, Position->bKsq));
            else
                Value -= 20 * MIN(MaxDist(A1, Position->bKsq), MaxDist(H8, Position->bKsq))
                   + 10 * MIN(MinDist(A1, Position->bKsq), MinDist(H8, Position->bKsq));
            }
        else
            {
            if (bBitboardBL)
                Value += 20 * MIN(MaxDist(A8, Position->wKsq), MaxDist(H1, Position->wKsq))
                   + 10 * MIN(MinDist(A8, Position->wKsq), MinDist(H1, Position->wKsq));
            else
                Value += 20 * MIN(MaxDist(A1, Position->wKsq), MaxDist(H8, Position->wKsq))
                   + 10 * MIN(MinDist(A1, Position->wKsq), MinDist(H8, Position->wKsq));
            }
        }
    if (Position->Dyn->reversible > 50)
        {
        Value *= (114 - Position->Dyn->reversible);
        Value /= 64;
        }
    if (Value > 0)
        {
        if (!Position->wtm && !BlackHasPiece && (bBitboardK ^ bBitboardP) == bBitboardOcc
           && !((bBitboardP >> 8) & ~Position->OccupiedBW) && !(bPatt & wBitboardOcc) && !Position->Dyn->ep
           && !(AttK[Position->bKsq]& ~Position->Dyn->wAtt) && !Position->Dyn->bKcheck)
            Value = 0;
        if (WhiteMinorOnly)
            {
            if (wBitboardN)
                {
                if (wBitboardP == SqSet[A7] && (bBitboardK | AttK[Position->bKsq]) & SqSet[A8])
                    Value = 0;
                if (wBitboardP == SqSet[H7] && (bBitboardK | AttK[Position->bKsq]) & SqSet[H8])
                    Value = 0;
                }
            else if (wBitboardBL && !(wBitboardP & NotH) && (bBitboardK | AttK[Position->bKsq]) & SqSet[H8])
                {
                if (wBitboardP & SqSet[H5] && bBitboardP == (SqSet[G7] | SqSet[H6]))
                    ;
                else
                    Value = 0;
                }
            else if (wBitboardBD && !(wBitboardP & NotA) && (bBitboardK | AttK[Position->bKsq]) & SqSet[A8])
                {
                if (wBitboardP & SqSet[A5] && bBitboardP == (SqSet[B7] | SqSet[A6]))
                    ;
                else
                    Value = 0;
                }
            if (!wBitboardP)
                Value = 0;
            }
        }
    else
        {
        if (Position->wtm && !WhiteHasPiece && (wBitboardK ^ wBitboardP) == wBitboardOcc
           && !((wBitboardP << 8) & ~Position->OccupiedBW) && !(wPatt & bBitboardOcc) && !Position->Dyn->ep
           && !(AttK[Position->wKsq]& ~Position->Dyn->bAtt) && !Position->Dyn->wKcheck)
            Value = 0;
        if (BlackMinorOnly)
            {
            if (bBitboardN)
                {
                if (bBitboardP == SqSet[A2] && (wBitboardK | AttK[Position->wKsq]) & SqSet[A1])
                    Value = 0;
                if (bBitboardP == SqSet[H2] && (wBitboardK | AttK[Position->wKsq]) & SqSet[H1])
                    Value = 0;
                }
            else if (bBitboardBD && !(bBitboardP & NotH) && (wBitboardK | AttK[Position->wKsq]) & SqSet[H1])
                {
                if (bBitboardP & SqSet[H4] && wBitboardP == (SqSet[G2] | SqSet[H3]))
                    ;
                else
                    Value = 0;
                }
            else if (bBitboardBL && !(bBitboardP & NotA) && (wBitboardK | AttK[Position->wKsq]) & SqSet[A1])
                {
                if (bBitboardP & SqSet[A4] && wBitboardP == (SqSet[B2] | SqSet[A3]))
                    ;
                else
                    Value = 0;
                }
            if (!bBitboardP)
                Value = 0;
            }
        }
    return Value;
    }
