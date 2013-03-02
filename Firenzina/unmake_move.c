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

#include "fire.h"
#include "make_unmake.h"

static INLINE void UnMakeWhiteOO(typePos *Position, int to)
    {
    if (to == G1)
        {
        wBitboardOcc ^= F1H1;
        wBitboardR ^= F1H1;
        Position->sq[F1] = 0;
        Position->sq[H1] = wEnumR;
        Position->OccupiedBW ^= F1H1;
        }
    else if (to == C1)
        {
        wBitboardOcc ^= A1D1;
        wBitboardR ^= A1D1;
        Position->sq[D1] = 0;
        Position->sq[A1] = wEnumR;
        Position->OccupiedBW ^= A1D1;
        }
    }
static INLINE void UnMakeBlackOO(typePos *Position, int to)
    {
    if (to == G8)
        {
        bBitboardOcc ^= F8H8;
        bBitboardR ^= F8H8;
        Position->sq[F8] = 0;
        Position->sq[H8] = bEnumR;
        Position->OccupiedBW ^= F8H8;
        }
    else if (to == C8)
        {
        bBitboardOcc ^= A8D8;
        bBitboardR ^= A8D8;
        Position->sq[D8] = 0;
        Position->sq[A8] = bEnumR;
        Position->OccupiedBW ^= A8D8;
        }
    }
void UndoWhite(typePos *Position, uint32 move)
    {
    int fr, to, pi, cp, z;
    uint64 mask;
    fr = From(move);
    to = To(move);
    pi = Position->sq[to];
    Position->wtm ^= 1;
    Position->height--;
    if (MoveIsProm(move))
        {
        Position->bitboard[pi] &= SqClear[to];
        pi = wEnumP;
        }
    Position->sq[fr] = pi;
    Position->sq[to] = Position->Dyn->cp;
    if (pi == wEnumK)
        Position->wKsq = fr;
    mask = SqSet[fr];
    wBitboardOcc |= mask;
    Position->bitboard[pi] |= mask;
    SetOccupied(mask, fr);
    mask = SqClear[to];
    wBitboardOcc &= mask;
    Position->bitboard[pi] &= mask;
    cp = Position->Dyn->cp;
    if (cp)
        {
        mask = ~mask;
        bBitboardOcc |= mask;
        Position->bitboard[cp] |= mask;
        }
    else
        {
        ClearOccupied(mask, to);
        if (MoveIsOO(move))
            UnMakeWhiteOO(Position, to);
        else if (MoveIsEP(move))
            {
            z = to ^ 8;
            Position->sq[z] = bEnumP;
            mask = SqSet[z];
            bBitboardOcc |= mask;
            bBitboardP |= mask;
            SetOccupied(mask, z);
            }
        }
    Position->Dyn--;
    Position->StackHeight--;
    }
void UndoBlack(typePos *Position, uint32 move)
    {
    int fr, to, pi, cp, z;
    uint64 mask;
    fr = From(move);
    to = To(move);
    pi = Position->sq[to];
    Position->wtm ^= 1;
    Position->height--;
    if (MoveIsProm(move))
        {
        Position->bitboard[pi] &= SqClear[to];
        pi = bEnumP;
        }
    Position->sq[fr] = pi;
    Position->sq[to] = Position->Dyn->cp;
    if (pi == bEnumK)
        Position->bKsq = fr;
    mask = SqSet[fr];
    bBitboardOcc |= mask;
    Position->bitboard[pi] |= mask;
    SetOccupied(mask, fr);
    mask = SqClear[to];
    bBitboardOcc &= mask;
    Position->bitboard[pi] &= mask;
    cp = Position->Dyn->cp;
    if (cp)
        {
        mask = ~mask;
        wBitboardOcc |= mask;
        Position->bitboard[cp] |= mask;
        }
    else
        {
        ClearOccupied(mask, to);
        if (MoveIsOO(move))
            UnMakeBlackOO(Position, to);
        else if (MoveIsEP(move))
            {
            z = to ^ 8;
            Position->sq[z] = wEnumP;
            mask = SqSet[z];
            wBitboardOcc |= mask;
            wBitboardP |= mask;
            SetOccupied(mask, z);
            }
        }
    Position->Dyn--;
    Position->StackHeight--;
    }
void Undo(typePos *Position, uint32 move)
    {
    if (!Position->wtm)
        UndoWhite(Position, move);
    else
        UndoBlack(Position, move);
    }