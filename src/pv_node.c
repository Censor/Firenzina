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

#ifndef pv_node
#define pv_node
#include "fire.h"
#include "history.h"
#include "control.h"
#include "pv_node.c"
#include "white.h"
#else
#include "black.h"
#endif

#define ValueRed1 (depth >> 1)
#define ValueRed2 (depth)

int MyPV(typePos *Position, int Alpha, int Beta, int depth, int check)
    {
    typeNext NextMove[1];
    TransDeclare();
    int good_move, v, Value, i, trans_depth, move, move_depth = 0, trans_move = 0, hash_depth;
    typeMoveList *list, *p, *q;
    int extend, best_value, new_depth, move_is_check, to, fr;
    typeDynamic *Pos0 = Position->Dyn;
    int singular, LMR, cnt, Median, Margin;
    bool Split, see;
    if (Beta < -ValueMate)
        return(-ValueMate);
    if (Alpha > ValueMate)
        return(ValueMate);
    Median = (Beta + Alpha) >> 1;
    Margin = (Beta - Alpha);
    if (depth <= 1)
        {
        if (check)
            return MyPVQsearchCheck(Position, Alpha, Beta, 1);
        else
            return MyPVQsearch(Position, Alpha, Beta, 1);
        }
    CheckRepetition(check);
    NextMove->trans_move = 0;
    hash_depth = 0;
    NextMove->move = 0;
    NextMove->bc = 0;
    Trans = HashPointer(Position->Dyn->Hash);
    (Pos0 + 1)->move = 0;
    for (i = 0; i < 4; i++, Trans++)
        {
        HyattHash(Trans, trans);
        if ((trans->hash ^ (Position->Dyn->Hash >> 32)) == 0)
            {
            if (trans->flags & FlagMoveLess)
                return(check) ? (HeightMultiplier * Height(Position) - ValueMate) : 0;
            trans_depth = trans->DepthLower;
            move = trans->move;
            if (move && trans_depth > move_depth)
                {
                move_depth = trans_depth;
                (Pos0 + 1)->move = trans_move = move;
                }
            trans_depth = MAX(trans->DepthLower, trans->DepthUpper);
            if (IsExact(trans) && trans_depth >= depth)
                {
                Value = HashLowerBound(trans);
                UpdateAge();

                if (!Analysing)
                    return(Value);
                }
            if (trans_depth > hash_depth)
                hash_depth = trans_depth;
            if (trans->DepthLower > trans->DepthUpper)
                {
                Value = HashLowerBound(trans);
                if (trans->flags & LowerFromPV && trans_depth >= depth && Value >= Beta)
                    return(Beta);
                }
            else
                {
                Value = HashUpperBound(trans);
                if (trans->flags & UpperFromPV && trans_depth >= depth && Value <= Alpha)
                    return(Alpha);
                }
            }
        }
    if (!trans_move && depth >= 6)
        {
        v = Alpha;
        if (depth >= 10)
            {
            v = MyPV(Position, Alpha - depth, Beta + depth, depth - 8, check);
            CheckHalt();
            if (v > Alpha - depth)
                trans_move = (Pos0 + 1)->move;
            }
        if (v > Alpha - depth)
            v = MyPV(Position, Alpha - depth, Beta + depth, depth - 4, check);
        CheckHalt();
        if (v > Alpha - depth)
            trans_move = (Pos0 + 1)->move;
        }
    else if (depth >= 10 && depth > hash_depth + 8)
        {
        v = MyPV(Position, Alpha - depth, Beta + depth, depth - 8, check);
        CheckHalt();
        if (v > Alpha - depth)
            trans_move = (Pos0 + 1)->move;
        if (v > Alpha - depth)
            {
            v = MyPV(Position, Alpha - depth, Beta + depth, depth - 4, check);
            CheckHalt();
            if (v > Alpha - depth)
                trans_move = (Pos0 + 1)->move;
            }
        }
    NextMove->trans_move = trans_move;
    NextMove->phase = Trans1;
    extend = 0;
    NextMove->Target = OppOccupied;
    singular = 0;
    if (check)
        {
        list = MyEvasion(Position, NextMove->List, 0xffffffffffffffff);
        NextMove->phase = Evade_Phase;
        for (p = list - 1; p >= NextMove->List; p--)
            {
            if ((p->move & 0x7fff) == trans_move)
                p->move |= 0xffff0000;
            else if (p->move <= (0x80 << 24))
                {
                if ((p->move & 0x7fff) == Pos0->killer1)
                    p->move |= 0x7fff8000;
                else if ((p->move & 0x7fff) == Pos0->killer2)
                    p->move |= 0x7fff0000;
                else
                    p->move |= (p->move & 0x7fff) | (HistoryValue(Position, p->move) << 15);
                }
            move = p->move;
            for (q = p + 1; q < list; q++)
                {
                if (move < q->move)
                    (q - 1)->move = q->move;
                else
                    break;
                }
            q--;
            q->move = move;
            }
        if ((list - NextMove->List) <= 1)
            singular = 2;
        if ((list - NextMove->List) == 2)
            singular = 1;
        if ((list - NextMove->List) > 2)
            singular = 0;
        }

    if (depth >= MinTransMoveDepth && NextMove->trans_move && singular < 2 && MyOK(Position, NextMove->trans_move))
        {
        move = NextMove->trans_move;
        if (check)
            v = MyExcludeCheck(Position, Alpha - ValueRed1, depth - DepthRed, move & 0x7fff);
        else
            v = MyExclude(Position, Alpha - ValueRed1, depth - DepthRed, move & 0x7fff);
        CheckHalt();
        if (v < Alpha - ValueRed1)
            {
            singular = 1;
            if (check)
                v = MyExcludeCheck(Position, Alpha - ValueRed2, depth - DepthRed, move & 0x7fff);
            else
                v = MyExclude(Position, Alpha - ValueRed2, depth - DepthRed, move & 0x7fff);
            CheckHalt();
            if (v < Alpha - ValueRed2)
                singular = 2;
            }
        }
    best_value = -ValueInfinity;
    NextMove->move = 0;
    NextMove->bc = 0;
    good_move = 0;
    Split = false;
    cnt = 0;
    while (true)
        {
        if (SMPFree != 0 && !check && depth >= PVSplitDepth && !Split
			&& best_value != -ValueInfinity)
            {
            int r;
            bool b;
            Split = true;
            b = IvanSplit(Position, NextMove, depth, Beta, Alpha, NodeTypePV, &r);
            CheckHalt();
            if (b)
                {
                if (r > Alpha || !good_move)
                    return r;
                move = good_move;
                (Pos0 + 1)->move = good_move & 0x7fff;
                best_value = r;
                goto Ivan;
                }
            }
        move = MyNext(Position, NextMove);
        if (!move)
            break;
        to = To(move);
        fr = From(move);
        if (Alpha > 0 && Pos0->reversible >= 2 && ((To(move) << 6) | From(move)) == (Pos0 - 1)->move
           && Position->sq[To(move)] == 0)
            {
            best_value = MAX(0, best_value);
            cnt++;
            continue;
            }
        if (MoveIsEP(move) || Position->sq[fr] == EnumMyK)
            see = true;
        else if (Position->sq[to] != 0)
            see = EasySEE(move) || MySEE(Position, move);
        else
            see = MySEE(Position, move);
        move &= 0x7fff;
        Make(Position, move);
		EVAL (move, depth);
        if (IllegalMove)
            {
            Undo(Position, move);
            continue;
            }
        move_is_check = (MoveIsCheck != 0);
        extend = 0;
        if (check)
            extend = 1;
        if (see && (Pos1->cp || move_is_check))
            extend = 1;
		if (PassedPawnPush (to, FourthRank (to)))
			extend = 1;	
		if (see && Pos1->cp && Pos1->cp != EnumOppP)
			extend = 2;
        if (Pos1->cp && (Pos1->material & 0xff) == 0 && Pos1->cp != EnumOppP)
            extend = 6;
        if (NextMove->trans_move != move)
            singular = 0;
        if (!extend && cnt >= 3 && NextMove->phase == Ordinary_Moves && Margin < 100 && depth >= 10)
            LMR = BSR(1 + cnt) - 1;
        else
            LMR = 0;
        new_depth = depth - 2 + MAX(extend, singular) - LMR;
        if (Is_Exact(Position->Dyn->exact))
            v = -Position->Dyn->Value;
        else if (NextMove->trans_move != move && new_depth > 1)
            {
            if (LowDepthConditionPV)
                {
                if (move_is_check)
                    v = -OppLowDepthCheck(Position, -Alpha, new_depth);
                else
                    v = -OppLowDepth(Position, -Alpha, new_depth);
                }
            else
                {
                if (move_is_check)
                    v = -OppCutCheck(Position, -Alpha, new_depth);
                else
                    v = -OppCut(Position, -Alpha, new_depth);
                }
            new_depth += LMR;
            if (v > Alpha)
                v = -OppPV(Position, -Beta, -Alpha, new_depth, move_is_check);
            }
        else
            v = -OppPV(Position, -Beta, -Alpha, new_depth, move_is_check);
        Undo(Position, move);
        cnt++;
        CheckHalt();
        if (v <= Alpha && Position->sq[To(move)] == 0 && MoveHistory(move))
            HistoryBad1(move, depth);
        if (v <= best_value)
            continue;
        best_value = v;
        if (v <= Alpha)
            continue;
        Alpha = v;
        good_move = move;
        HashLowerPV(Position->Dyn->Hash, move, depth, v);
        if (v >= Beta)
            {
            if (Position->sq[To(move)] == 0 && MoveHistory(move))
                HistoryGood(move, depth);
            return(v);
            }
        }
    move = good_move;
    (Pos0 + 1)->move = good_move & 0x7fff;
    if (best_value == -ValueInfinity)
        {
        best_value = (check) ? (HeightMultiplier * Height(Position) - ValueMate) : 0;
        HashExact(Position, MoveNone, depth, best_value, FlagExact | FlagMoveLess);
        return(best_value);
        }
    Ivan:
    if (move)
        {
        if (Position->sq[To(move)] == 0 && MoveHistory(move))
            HistoryGood(move, depth);
        HashExact(Position, move, depth, best_value, FlagExact);
        return(best_value);
        }
    HashUpperPV(Position->Dyn->Hash, depth, best_value);
    return(best_value);
    }
