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

#ifndef SMPSearch
#define SMPSearch
#include "fire.h"

#if defined(__GNUC__)
#define INLINE inline
#endif


static INLINE void SMPBadHistory(typePos *Pos, uint32 m, SplitPoint *sp)
    {
    if ((Pos->Dyn + 1)->cp == 0 && MoveHistory(m))
        {
        int sv = HistoryValue(Pos, m);
        if (Pos->Dyn->Value > sp->alpha - 50)
            HistoryValue(Pos, m) = sv - ((sv * sp->depth) >> 8);
        }
    }
#include "SMP_search.c"
#include "white.h"
#else
#include "black.h"
#endif

void MyPVNodeSMP(typePos *Position)
    {
    int v;
    int alpha;
    int beta;
    int m;
    typeNext *NextMove;
    SplitPoint *sp;
    int extend;
    int to;
    int new_depth;
    sp = Position->SplitPoint;
    while (true)
        {
        Lock(sp->splock);
        beta = sp->beta;
        alpha = sp->alpha;
        if (sp->tot)
            {
            UnLock(sp->splock);
            return;
            }
        NextMove = sp->MovePick;
        m = MyNext(Position, NextMove);
        if (!m)
            {
            NextMove->phase = Fase0;
            UnLock(sp->splock);
            return;
            }
        UnLock(sp->splock);
        Make(Position, m);
        Eval(Position, -0x7fff0000, 0x7fff0000, m, sp->depth);
        if (MyKingCheck)
            {
            Undo(Position, m);
            continue;
            }
        extend = 0;
        to = To(m);
        if (PassedPawnPush(to, SixthRank(to)))
            extend = 2;
        else
            {
            if (Position->Dyn->cp != 0 || OppKingCheck != 0)
                extend = 1;
            else if (PassedPawnPush(to, FourthRank(to)))
                extend = 1;
            }

        new_depth = sp->depth - 2 + extend;

        if (OppKingCheck)
            v = -OppCutCheck(Position, -alpha, new_depth);
        else
            v = -OppCut(Position, -alpha, new_depth);
        if (v <= alpha)
            {
            Undo(Position, m);
            if (Position->stop)
                return;
            SMPBadHistory(Position, m, sp);
            continue;
            }
        if (!sp->tot && !Position->stop)
            {
            bool b = (OppKingCheck != 0);
            v = -OppPV(Position, -beta, -alpha, new_depth, b);
            Undo(Position, m);
            if (Position->stop)
                return;
            if (v > alpha)
                {
                Lock(sp->splock);
                if (v > sp->alpha)
                    {
                    sp->alpha = v;
                    sp->value = v;
                    sp->good_move = m;
                    }
                UnLock(sp->splock);
                HashLower(Position->Dyn->Hash, m, sp->depth, v);
                }
            }
        else
            Undo(Position, m);
        if (Position->stop)
            return;
        if (v >= beta)
            {
            FailHigh(sp, Position, m);
            return;
            }
        }
    }
void MyAllSMP(typePos *Position)
    {
    int v;
    int m;
    typeNext *NextMove;
    SplitPoint *sp;
    int scout, depth, ph, c;
    sp = Position->SplitPoint;
    scout = sp->beta;
    depth = sp->depth;
    while (true)
        {
        Lock(sp->splock);
        if (sp->tot)
            {
            UnLock(sp->splock);
            return;
            }
        NextMove = sp->MovePick;
        m = MyNext(Position, NextMove) & 0x7fff;
        ph = NextMove->phase;
        c = NextMove->move;
        if (!m)
            {
            NextMove->phase = Fase0;
            UnLock(sp->splock);
            return;
            }
        UnLock(sp->splock);
        if (m == NextMove->exclude)
            continue;
        Make(Position, m);
        Eval(Position, -0x7fff0000, 0x7fff0000, m, depth);
        if (MyKingCheck)
            {
            Undo(Position, m);
            continue;
            }
        m &= 0x7fff;
        if (OppKingCheck)
            {
            v = -OppCutCheck(Position, 1 - scout, depth - 1);
            }
        else
            {
            int to = To(m);
            int extend = 0;
            if (PassedPawnPush(to, SixthRank(to)))
                extend = 1;
            if (ph == Ordinary_Moves && !extend)
                {
                int Reduction = 2 + BSR(2 + c);
                int NewDepth = MAX(8, depth - Reduction);
                v = -OppCut(Position, 1 - scout, NewDepth);
                if (v < scout)
                    goto I;
                }
            v = -OppCut(Position, 1 - scout, depth - 2 + extend);
            }
        I:
        Undo(Position, m);
        if (Position->stop)
            return;
        if (v >= scout)
            {
            FailHigh(sp, Position, m);
            return;
            }
        SMPBadHistory(Position, m, sp);
        }
    }
void MyCutSMP(typePos *Position)
    {
    int v;
    int m;
    typeNext *NextMove;
    SplitPoint *sp;
    int scout, depth, ph, c;
    sp = Position->SplitPoint;
    scout = sp->beta;
    depth = sp->depth;
    while (true)
        {
        Lock(sp->splock);
        if (sp->tot)
            {
            UnLock(sp->splock);
            return;
            }
        NextMove = sp->MovePick;
        m = MyNext(Position, NextMove);
        ph = NextMove->phase;
        c = NextMove->move;
        if (!m)
            {
            NextMove->phase = Fase0;
            UnLock(sp->splock);
            return;
            }
        UnLock(sp->splock);
        Make(Position, m);
        Eval(Position, -0x7fff0000, 0x7fff0000, m, depth);
        if (MyKingCheck)
            {
            Undo(Position, m);
            continue;
            }
        m &= 0x7fff;
        if (OppKingCheck)
            {
            v = -OppAllCheck(Position, 1 - scout, depth - 1);
            }
        else
            {
            int to = To(m);
            int extend = 0;
            if (PassedPawnPush(to, SixthRank(to)))
				extend = 1;
            if (ph == Ordinary_Moves && !extend)
                {
                int Reduction = 4 + BSR(5 + c);
                int NewDepth = MAX(8, depth - Reduction);
                v = -OppAll(Position, 1 - scout, NewDepth);
                if (v < scout)
                    goto I;
                }
            v = -OppAll(Position, 1 - scout, depth - 2 + extend);
            }
        I:
        Undo(Position, m);
        if (Position->stop)
            return;
        if (v >= scout)
            {
            FailHigh(sp, Position, m);
            return;
            }
        SMPBadHistory(Position, m, sp);
        }
    }
