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

#ifndef cut_node
#define cut_node
#include "fire.h"
#include "history.h"
#include "null_move.h"
#include "cut_node.c"
#include "white.h"
#else
#include "black.h"
#endif

#define ValueRed1 (depth)
#define ValueRed2 (depth << 1)

int MyCut(typePos *Position, int value, int depth)
    {
    int height, move, i, singular;
    TransDeclare();
    int trans_depth, move_depth = 0, trans_move = 0, Value, cnt;
    int v, extend, new_depth, move_is_check;
    typeNext NextMove[1];
    typeDynamic *Pos0 = Position->Dyn;
    uint64 zob = Position->Dyn->Hash;
    int to, fr;
    bool Split;

	CheckForMate (value);

    (Pos0 + 1)->move = 0;
    CheckRepetition(false);
    Trans = HashPointer(zob);
    for (i = 0; i < 4; i++, Trans++)
        {
        HyattHash(Trans, trans);
        if ((trans->hash ^ (zob >> 32)) == 0)
            {
            if (trans->flags & FlagMoveLess)
                return 0;
            trans_depth = trans->DepthLower;
            move = trans->move;
            if (move && trans_depth > move_depth)
                {
                move_depth = trans_depth;
                (Pos0 + 1)->move = trans_move = move;
                }
            trans_depth = MAX(trans->DepthLower, trans->DepthUpper);
            if (trans->DepthLower >= depth)
                {
                Value = HashLowerValue(trans);
                HashReBound(Value, value);
                if (Value >= value)
                    {
                    if (!((trans->flags & FlagAll) == FlagAll))
                        if (MyNull || move)
                            {
                            UpdateAge();
                            return(Value);
                            }
                    }
                }
            if (trans->DepthUpper >= depth)
                {
                Value = HashUpperValue(trans);
                HashReBound(Value, value);
                if (Value < value)
                    {
                    UpdateAge();
                    return(Value);
                    }
                }
            }
        }
    NextMove->trans_move = trans_move;
    if (Pos0->Value >= value && MyNull)
        {
		new_depth = depth - NullReduction;
		new_depth -= ScoreReduction(Pos0->Value - value);
        new_depth += KingDangerAdjust(Pos0->wKdanger, Pos0->bKdanger);
        v = value;
        if (v >= value)
            {
            MakeNull(Position);
            if (QSearchCondition)
                v = -OppQsearch(Position, 1 - value, 0);
            else if (LowDepthCondition)
                v = -OppLowDepth(Position, 1 - value, new_depth);
            else
                v = -OppAll(Position, 1 - value, new_depth);
            UndoNull(Position);
            CheckHalt();
            }
        if (VerifyNull && v >= value)
            {
            int Flags = Position->Dyn->flags;
            Position->Dyn->flags &= ~3;
            new_depth -= VerifyReduction;
            if (QSearchCondition)
                v = MyQsearch(Position, value, 0);
            else if (LowDepthCondition)
                v = MyLowDepth(Position, value, new_depth);
            else
                v = MyCut(Position, value, new_depth);
            Position->Dyn->flags = Flags;
            CheckHalt();
            }
        if (v >= value)
            {
            if (trans_move == MoveNone)
                HashLower(Position->Dyn->Hash, 0, depth, v);
            return(v);
            }
        }
    if (trans_move == MoveNone && depth >= 6)
        {
        new_depth = depth - 4;
        if (LowDepthCondition)
            v = MyLowDepth(Position, value, new_depth);
        else
            v = MyCut(Position, value, new_depth);
        CheckHalt();
        if (v >= value)
            trans_move = (Pos0 + 1)->move;
        }

    singular = 0;
    if (depth >= MinTransMoveDepth && trans_move && MyOK(Position, trans_move))
        {
        v = MyExclude(Position, value - ValueRed1, depth - DepthRed, trans_move & 0x7fff);
        CheckHalt();
        if (v < value - ValueRed1)
            {
            singular++;
            height = Height(Position);
            if (height << 2 <= depth)
                singular++;
            v = MyExclude(Position, value - ValueRed2, depth - DepthRed, trans_move & 0x7fff);
            CheckHalt();
            if (v < value - ValueRed2)
                {
                singular++;
                if (height << 3 <= depth)
                    singular++;
                }
            }
        }
    cnt = 0;
    NextMove->trans_move = trans_move;
    NextMove->phase = Trans1;
    NextMove->Target = OppOccupied;
    if (depth < SearchDepthMin && value - Pos0->Value >= MinDepthMultiplier * (depth - 5))
        {
        NextMove->phase = Trans2;
        cnt = 1;
        if (value - Pos0->Value >= MinDepthMultiplier * (depth - 2))
            NextMove->Target ^= BitboardOppP;
        }
    NextMove->move = MoveNone;
    NextMove->bc = 0;
    v = value;
    Split = false;
    while (true)
        {
        if (SMPFree != 0 && SplitAtCN && depth >= CNSplitDepth && !Split
           && NextMove->phase != Trans1 && cnt >= 1 && NextMove->phase <= Ordinary_Moves)
            {
            int r;
            bool b;
            Split = true;
            b = IvanSplit(Position, NextMove, depth, value, value, NodeTypeCut, &r);
            if (b)
                return r;
            }
        move = MyNext(Position, NextMove);
        if (!move)
            break;
        to = To(move);
        fr = From(move);
        if (IsRepetition(0))
            {
            cnt++;
            continue;
            }
        if (cnt > CountLimit && NextMove->phase == Ordinary_Moves && (move & 0xe000) == 0 && SqSet[fr] & ~MyXray && depth < SearchDepthMin)
            {
            if ((1 << (depth - SearchDepthReduction)) + MaxPositional(move) + (Pos0->Value) < value + 35 + 2 *cnt)
                {
                cnt++;
                continue;
                }
            }
        move &= 0x7fff;
        Make(Position, move);
        EvalCutAll(value, move);
        if (IllegalMove)
            {
            Undo(Position, move);
            continue;
            }
        if (MoveIsCheck)
            move_is_check = 1;
        else
            move_is_check = 0;
        if (move != NextMove->trans_move)
            singular = 0;
        extend = 0;
        if (move == NextMove->trans_move)
            {
            if (PassedPawnPush(to, FourthRank(to)))
                extend = 1;
            }
        else
            {
            if (PassedPawnPush(to, SixthRank(to)))
                extend = 1;
            }
        if (NextMove->trans_move == move && To((Pos1 - 1)->move) == To(Pos1->move) && (Pos1 - 1)->cp != 0)
            extend++;
        extend = MAX(extend, singular);
        if (Is_Exact(Position->Dyn->exact))
            v = -Position->Dyn->Value;
        else if (move_is_check)
            {
            new_depth = depth - 2 + MAX(1, extend);
            v = -OppAllCheck(Position, 1 - value, new_depth);
            }
        else
            {
            if (cnt > 2 && depth < SearchDepthMin && NextMove->phase == Ordinary_Moves &&
				!extend && (2 << (depth - SearchDepthReduction)) - Pos1->Value < value + cnt - UndoCountThreshold)
                {
                Undo(Position, move);
                cnt++;
                continue;
                }
            if (NextMove->phase == Ordinary_Moves && !extend)
                {
                new_depth = depth - 2 - (4 + BSR(4 + cnt));
                if (QSearchCondition)
                    v = -OppQsearch(Position, 1 - value, 0);
                else if (LowDepthCondition)
                    v = -OppLowDepth(Position, 1 - value, new_depth);
                else
                    v = -OppAll(Position, 1 - value, new_depth);
                if (v < value)
                    goto Done;
                }
            new_depth = depth - 2 + extend;
            if (LowDepthCondition)
                v = -OppLowDepth(Position, 1 - value, new_depth);
            else
                v = -OppAll(Position, 1 - value, new_depth);
            }
        Done:
        Undo(Position, move);
        CheckHalt();
        cnt++;
        if (v >= value)
            {
            if ((Pos0 + 1)->cp == 0 && MoveHistory(move))
                HistoryGood(move, depth);
            HashLower(Position->Dyn->Hash, move, depth, v);
            return(v);
            }
        if ((Pos0 + 1)->cp == 0 && MoveHistory(move))
            HistoryBad(move, depth);
        }
    if (!cnt && NextMove->phase <= Trans2)
        return(0);
    v = value - 1;
    HashUpperCut(Position, depth, v);
    return(v);
    }
int MyCutCheck(typePos *Position, int value, int depth)
    {
    int height, move, cnt, Reduction, extend;
    int trans_depth, move_depth = 0, trans_move = 0, Value, new_depth, v, i;
    TransDeclare();
    typeMoveList List[256], *list, *p, *q;
    uint64 zob = Position->Dyn->Hash;
    int best_value, singular;
    typeDynamic *Pos0 = Position->Dyn;
    bool Gen;

	CheckForMate (value);

    (Pos0 + 1)->move = MoveNone;
    CheckRepetition(true);
    Trans = HashPointer(zob);
    for (i = 0; i < 4; i++, Trans++)
        {
        HyattHash(Trans, trans);
        if ((trans->hash ^ (zob >> 32)) == 0)
            {
            if (trans->flags & FlagMoveLess)
                return(HeightMultiplier * Height(Position) - ValueMate);
            trans_depth = trans->DepthLower;
            move = trans->move;
            if (move && trans_depth > move_depth)
                {
                move_depth = trans_depth;
                (Pos0 + 1)->move = trans_move = move;
                }
            trans_depth = MAX(trans->DepthLower, trans->DepthUpper);
            if (trans->DepthLower >= depth)
                {
                Value = HashLowerValue(trans);
                HashReBound(Value, value);
                if (Value >= value)
                    {
                    if (!((trans->flags & FlagAll) == FlagAll))
                        {
                        UpdateAge();
                        return(Value);
                        }
                    }
                }
            if (trans->DepthUpper >= depth)
                {
                Value = HashUpperValue(trans);
                HashReBound(Value, value);
                if (Value < value)
                    {
                    UpdateAge();
                    return(Value);
                    }
                }
            }
        }
    if (trans_move && !MyOK(Position, trans_move))
        trans_move = MoveNone;
    best_value = HeightMultiplier * Height(Position) - ValueMate;
    singular = 0;
    if (depth >= MinTransMoveDepth && trans_move)
        {
        v = MyExcludeCheck(Position, value - depth, depth - DepthRed, trans_move & 0x7fff);
        CheckHalt();
        if (v < value - depth)
            {
            singular++;
            height = Height(Position);
            if (height << 2 <= depth)
                singular++;
            v = MyExcludeCheck(Position, value - 2 * depth, depth - DepthRed, trans_move & 0x7fff);
            CheckHalt();
            if (v < value - 2 *depth)
                {
                singular++;
                if (height << 3 <= depth)
                    singular++;
                }
            }
        }
    p = List;
    List[0].move = trans_move;
    cnt = 0;
    Gen = false;
    List[1].move = 0;
    while (p->move || !Gen)
        {
        if (!p->move)
            {
            list = MyEvasion(Position, List + 1, 0xffffffffffffffff);
            Gen = true;
            for (p = list - 1; p >= List + 1; p--)
                {
                if ((p->move & 0x7fff) == trans_move)
                    p->move = 0;
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
            p = List + 1;
            continue;
            }
        move = p->move & 0x7fff;
        p++;
        if (move != trans_move)
            singular = 0;
        if (IsRepetition(0))
            {
            cnt++;
            best_value = MAX(0, best_value);
            continue;
            }
        Make(Position, move);
        EvalCutAll(value, move);
        if (IllegalMove)
            {
            Undo(Position, move);
            continue;
            }
        if (Is_Exact(Position->Dyn->exact))
            v = -Position->Dyn->Value;
        else if (MoveIsCheck)
            {
            new_depth = depth - 2;
            if (singular)
                new_depth += singular;
            else
                new_depth++;
            if (LowDepthCondition)
                v = -OppLowDepthCheck(Position, 1 - value, new_depth);
            else
                v = -OppAllCheck(Position, 1 - value, new_depth);
            }
        else
            {
            if (cnt >= 1)
                {
                if (depth > 8)
                    Reduction = BSR(depth - 7);
                else
                    Reduction = 0;
                Reduction += 1 + MIN(cnt, 2);
                new_depth = depth + ExtendInCheck - Reduction - 2;
                if (QSearchCondition)
                    v = -OppQsearch(Position, 1 - value, 0);
                else if (LowDepthCondition)
                    v = -OppLowDepth(Position, 1 - value, new_depth);
                else
                    v = -OppAll(Position, 1 - value, new_depth);
                if (v < value)
                    goto Loop;
                }
            extend = !singular && ExtendInCheck;
            new_depth = depth - 2 + extend + singular;
            if (LowDepthCondition)
                v = -OppLowDepth(Position, 1 - value, new_depth);
            else
                v = -OppAll(Position, 1 - value, new_depth);
            }
        Loop:
        Undo(Position, move);
        CheckHalt();
        if (v > best_value)
            best_value = v;
        if (v < value)
            {
            cnt++;
            continue;
            }
        HashLower(Position->Dyn->Hash, move, MAX(1, depth), v);
        return(v);
        }
    HashUpperCut(Position, MAX(1, depth), best_value);
    return(best_value);
    }
