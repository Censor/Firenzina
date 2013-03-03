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

#ifndef ok_move
#define ok_move
#include "fire.h"
#include "ok_move.c"
#include "white.h"
#else
#include "black.h"
#endif

bool MyOK(typePos *Position, uint32 move)
    {
    int fr, to, pi, cp;
    uint64 toSet;
    to = To(move);
    toSet = SqSet[to];
    fr = From(move);
    pi = Position->sq[fr];
    if (pi == 0)
        return false;
    if (PieceIsOpp(pi))
        return false;
    cp = Position->sq[to];
    if (cp && PieceIsMine(cp))
        return false;
    if (cp == EnumOppK)
        return false;
    if (pi == EnumMyP)
        {
        if (EighthRank(to) && !MoveIsProm(move))
            return false;
        if (MoveIsEP(move) && to == Position->Dyn->ep && (fr == BackLeft(to) || fr == BackRight(to)))
            return true;
        if (fr == BackLeft(to) || fr == BackRight(to))
            {
            if (toSet & OppOccupied)
                return true;
            return false;
            }
        if (fr == Backward(to))
            {
            if ((toSet & Position->OccupiedBW) == 0)
                return true;
            return false;
            }
        if (fr != Backward2(to) || Rank(fr) != NumberRank2)
            return false;
        if (Position->OccupiedBW & SqSet[Forward(fr)])
            return false;
        return true;
        }
    if (MoveIsProm(move))
        return false;
    if (pi == EnumMyN)
        {
        if (AttN[fr] & toSet)
            return true;
        return false;
        }
    if (pi == EnumMyBL || pi == EnumMyBD)
        {
        if (AttB(fr) & toSet)
            return true;
        return false;
        }
    if (MoveIsOO(move))
        {
        if (to == WhiteG1)
            {
            if (!CastleOO || Position->OccupiedBW & WhiteF1G1 || OppAttacked & WhiteF1G1)
                return false;
            return true;
            }
        if (to == WhiteC1)
            {
            if (!CastleOOO || Position->OccupiedBW & WhiteB1C1D1 || OppAttacked & WhiteC1D1)
                return false;
            return true;
            }
        }
    if (pi == EnumMyR)
        {
        if (AttR(fr) & toSet)
            return true;
        return false;
        }
    if (pi == EnumMyQ)
        {
        if (AttQ(fr) & toSet)
            return true;
        return false;
        }
    if (pi == EnumMyK)
        {
        if (AttK[fr] & toSet && (toSet & OppAttacked) == 0)
            return true;
        return false;
        }
    return false;
    }