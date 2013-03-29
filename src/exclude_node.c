<<<<<<< HEAD:src/exclude_node.c
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
>>>>>>> Linux/Housekeeping/Bug fixes/Extend xTreme/Defs:Firenzina/exclude_node.c
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

#ifndef exclude_node
#define exclude_node
#include "fire.h"
#include "history.h"
#include "null_move.h"
#include "exclude_node.c"
#include "white.h"
#else
#include "black.h"
#endif

int MyExclude(typePos *Position, int value, int depth, uint32 Move)
    {
    int move, i;
    TransDeclare();
    int trans_depth, move_depth = 0, trans_move = 0, Value, cnt;
    int v, extend, new_depth, move_is_check;
    typeNext NextMove[1];
    typeDynamic *Pos0 = Position->Dyn;
    uint64 zob = Position->Dyn->Hash;
    int to, fr;
    int Reduction;

	CheckForMate (value);

    (Pos0 + 1)->move = 0;
    CheckRepetition(false);
    zob ^= Hash(EnumMyK, From(Move)) ^ Hash(EnumOppK, To(Move));
    Trans = HashPointer(zob);
    for (i = 0; i < 4; i++, Trans++)
        {
        HyattHash(Trans, trans);
        if ((trans->hash ^ (zob >> 32)) == 0)
            {
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
                v = -OppCut(Position, 1 - value, new_depth);
            UndoNull(Position);
            CheckHalt();
            }
        new_depth -= VerifyReduction;
        if (VerifyNull && v >= value && new_depth > 7)
            {
            int Flags = Position->Dyn->flags;
            Position->Dyn->flags &= ~3;
            v = MyExclude(Position, value, new_depth, Move);
            Position->Dyn->flags = Flags;
            CheckHalt();
            }
        if (v >= value)
            {
            if (trans_move == 0)
                HashLower(zob, 0, depth, v);
            return(v);
            }
        }
    cnt = 0;
    NextMove->trans_move = trans_move;
    NextMove->phase = Trans1;
    NextMove->Target = OppOccupied;
    if (depth < SearchDepthMin && value - Pos0->Value >= MinDepthMultiplier *(depth - 4))
        {
        NextMove->phase = Trans2;
        cnt = 1;
        if (value - Pos0->Value >= MinDepthMultiplier * (depth - 2))
            NextMove->Target ^= BitboardOppP;
        }
    NextMove->move = 0;
    NextMove->bc = 0;
    NextMove->exclude = Move & 0x7fff;
    v = value;
    while (true)
        {
        move = MyNext(Position, NextMove);
        if (!move)
            break;
        if ((move & 0x7fff) == (Move & 0x7fff))
            continue;
        to = To(move);
        fr = From(move);
        if (IsRepetition(0))
            {
            cnt++;
            continue;
            }
        if (cnt > CountLimit && NextMove->phase == Ordinary_Moves && (move & 0xe000) == 0 && SqSet[fr] & ~MyXray && depth < SearchDepthMin)
            {
            if ((6 << (depth - SearchDepthReduction)) + MaxPositional(move) + (Pos0->Value) < value + 30 + 2 *cnt)
                {
                cnt++;
                continue;
                }
            }
        Reduction = 0;
        if (depth < SearchDepthMin && (2 << (depth - SearchDepthReduction)) + (Pos0->Value) < value + 125
			&& NextMove->phase == Ordinary_Moves && MyKingSq != fr && SqSet[fr] & ~MyXray && (move & 0x8000) == 0
			&& !MySEE(Position, move))
            {
            cnt++;
            continue;
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
        extend = 0;
        if (PassedPawnPush(to, SixthRank(to)))
            extend = 1;
        if (Is_Exact(Position->Dyn->exact))
            v = -Position->Dyn->Value;
        else if (move_is_check)
            v = -OppCutCheck(Position, 1 - value, depth - 1);
        else
            {
            if (cnt > CountLimit && depth < SearchDepthMin && Pos1->cp == 0 && (2 << (depth - SearchDepthReduction)) - Pos1->Value < value + cnt - UndoCountThreshold)
                {
                Undo(Position, move);
                cnt++;
                continue;
                }
            if (NextMove->phase == Ordinary_Moves && (cnt >= 3 || Reduction))
                {
                new_depth = depth - 2 + extend - BSR(1 + cnt) - Reduction;
                if (QSearchCondition)
                    v = -OppQsearch(Position, 1 - value, 0);
                else if (LowDepthCondition)
                    v = -OppLowDepth(Position, 1 - value, new_depth);
                else
                    v = -OppCut(Position, 1 - value, new_depth);
                if (v < value)
                    goto Done;
                }
            new_depth = depth - 2 + extend;
            if (LowDepthCondition)
                v = -OppLowDepth(Position, 1 - value, new_depth);
            else
                v = -OppCut(Position, 1 - value, new_depth);
            }
        Done:
        Undo(Position, move);
        CheckHalt();
        cnt++;
        if (v >= value)
            {
            if ((Pos0 + 1)->cp == 0 && MoveHistory(move))
                HistoryGood(move, depth);
            HashLower(zob, move, depth, v);
            return(v);
            }
        if ((Pos0 + 1)->cp == 0 && MoveHistory(move))
            HistoryBad(move, depth);
        }
    v = value - 1;
    HashUpper(zob, depth, v);
    return(v);
    }
int MyExcludeCheck(typePos *Position, int value, int depth, uint32 Move)
    {
    int move, cnt;
    int trans_depth, move_depth = 0, trans_move = 0, Value, new_depth, v, i;
    TransDeclare();
    typeMoveList List[256], *list, *p, *q;
    uint64 zob = Position->Dyn->Hash;
    int best_value;
    typeDynamic *Pos0 = Position->Dyn;
    bool Gen;
	CheckForMate (value);

    zob ^= Hash(EnumMyK, From(Move)) ^ Hash(EnumOppK, To(Move));
    (Pos0 + 1)->move = MoveNone;
    CheckRepetition(true);
    Trans = HashPointer(zob);
    for (i = 0; i < 4; i++, Trans++)
        {
        HyattHash(Trans, trans);
        if ((trans->hash ^ (zob >> 32)) == 0)
            {
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
					UpdateAge();
					return(Value);
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
        if (move == Move)
            continue;
        if (IsRepetition(0))
            {
            best_value = MAX(0, best_value);
            cnt++;
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
            new_depth = depth - 1;
            if (LowDepthCondition)
                v = -OppLowDepthCheck(Position, 1 - value, new_depth);
            else
                v = -OppCutCheck(Position, 1 - value, new_depth);
            }
        else
            {
            if (cnt >= 1)
                {
                new_depth = depth - 2 - MIN(2, cnt) + ExtendInCheck;
                if (QSearchCondition)
                    v = -OppQsearch(Position, 1 - value, 0);
                else if (LowDepthCondition)
                    v = -OppLowDepth(Position, 1 - value, new_depth);
                else
                    v = -OppCut(Position, 1 - value, new_depth);
                if (v < value)
                    goto Loop;
                }
            new_depth = depth - 2 + ExtendInCheck;
            if (LowDepthCondition)
                v = -OppLowDepth(Position, 1 - value, new_depth);
            else
                v = -OppCut(Position, 1 - value, new_depth);
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
        HashLower(zob, move, MAX(1, depth), v);
        return(v);
        }
    HashUpper(zob, MAX(1, depth), best_value);
    return(best_value);
    }
