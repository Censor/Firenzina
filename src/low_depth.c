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

#ifndef low_depth
#define low_depth
#include "fire.h"
#include "history.h"
#include "null_move.h"
#include "low_depth.c"
#include "white.h"
#else
#include "black.h"
#endif

int MyLowDepth(typePos *Position, int value, int depth)
    {
    int cnt, Value, best_value, v, i, trans_move = 0;
    int move, move_depth = 0, trans_depth, to, fr;
    typeNext NextMove[1];
    typeDynamic *Pos0 = Position->Dyn;
    TransDeclare();

	CheckForMate (value);

    (Pos0 + 1)->move = MoveNone;
    CheckRepetition(false);
    Trans = HashPointer(Position->Dyn->Hash);
    for (i = 0; i < 4; i++, Trans++)
        {
        HyattHash(Trans, trans);
        if ((trans->hash ^ (Position->Dyn->Hash >> 32)) == 0)
            {
            if (trans->flags & FlagMoveLess)
                return 0;
            if (trans->DepthLower >= depth)
                {
                Value = HashLowerValue(trans);
                HashReBound(Value, value);
                if (Value >= value)
                    {
                    (Pos0 + 1)->move = trans->move;
                    return(Value);
                    }
                }
            if (trans->DepthUpper >= depth)
                {
                Value = HashUpperValue(trans);
                HashReBound(Value, value);
                if (Value < value)
                    return(Value);
                }
            trans_depth = trans->DepthLower;
            move = trans->move;
            if (move && trans_depth > move_depth)
                {
                move_depth = trans_depth;
                trans_move = move;
                }
            }
        }
	v = Pos0->Value + LowDepthMargin;
	if (v < value)
		return (value - 1);
    v = Pos0->Value - (70 + 10 * depth);
	if (v >= value && value >= -10000)
        return(Pos0->Value);
    best_value = MIN(Pos0->Value, value - 1);
    if (Pos0->Value >= value && MyNull)
        {
        v = value;
        if (v >= value)
            {
			int new_depth = depth - NullReduction;
			new_depth -= ScoreReduction (Pos0->Value - value);
            MakeNull(Position);
			if (QSearchCondition)
				v = -OppQsearch (Position, 1 - value, 0);
			else
				v = -OppLowDepth (Position, 1 - value, new_depth);
            UndoNull(Position);
            CheckHalt();
            }
        if (VerifyNull && v >= value)
            {

            v = MyQsearch(Position, value, 0);
            CheckHalt();
            }
        if (v >= value)
            {
            if (trans_move == MoveNone)
                HashLower(Position->Dyn->Hash, 0, depth, v);
            return(v);
            }
        }
    NextMove->phase = Trans1;
    NextMove->Target = OppOccupied;
    if (Pos0->Value + 50 + (depth << 3) < value)
        {
        NextMove->phase = Trans2;
        if (value >= Pos0->Value + 75 + (depth << 5))
            {
            NextMove->Target ^= BitboardOppP;
            if (BitboardOppP & MyAttacked)
                best_value += 125;
            if (depth <= 3 && value >= Pos0->Value + 400 + (depth << 5))
                {
                NextMove->Target ^= (BitboardOppN | BitboardOppB);
                best_value += 300;
                if (value >= Pos0->Value + 600 + (depth << 5))
                    {
                    NextMove->Target ^= BitboardOppR;
                    best_value += 200;
                    }
                }
            }
        }
    else if (depth <= 3 && Pos0->Value + (depth << 2) < value)
        {
        NextMove->phase = Trans3;
        NextMove->mask = (value - Pos0->Value) - (depth << 2) + 5;
        }
    NextMove->bc = 0;
    NextMove->move = 0;
    NextMove->trans_move = trans_move;
    cnt = 0;
    while ((move = MyNext(Position, NextMove)))
        {
        to = To(move);
        fr = From(move);
        if (IsRepetition(0))
            {
            best_value = MAX(0, best_value);
            cnt++;
            continue;
            }
        if (cnt >= depth && NextMove->phase == Ordinary_Moves && (move & 0xe000) == 0 && (SqSet[fr] & ~MyXray)
           && MyOccupied ^ (BitboardMyP | BitboardMyK))
            {
            if ((depth << 1) + MaxPositional(move) + Pos0->Value < value + 40 + (cnt << 1))
                {
                cnt++;
                continue;
                }
            }
        if (Position->sq[fr] != EnumMyK && (Position->sq[to] == 0 || (depth <= 5 && !EasySEE(move)))
           && (SqSet[fr] & ~MyXray) && !MoveIsEP(move) && move != trans_move && !MySEE(Position, move))
            {
            cnt++;
            continue;
            }
        move &= 0x7fff;
        Make(Position, move);
        EvalLowDepth(value, move);
        if (IllegalMove || (NextMove->phase == PositionalGainPhase && MoveIsCheck))
            {
            Undo(Position, move);
            continue;
            }
        if (Is_Exact(Position->Dyn->exact))
            v = -Position->Dyn->Value;
        else if (MoveIsCheck)
            v = -OppLowDepthCheck(Position, 1 - value, depth - 1);
        else
            {
            int new_depth = depth - 2;
            if (cnt >= depth && (depth << 1) - Pos1->Value < value + cnt)
                {
                Undo(Position, move);
                cnt++;
                continue;
                }
            if (QSearchCondition)
                v = -OppQsearch(Position, 1 - value, 0);
            else
                v = -OppLowDepth(Position, 1 - value, new_depth);
            }
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
        if (v >= best_value)
            best_value = v;
        if ((Pos0 + 1)->cp == 0 && MoveHistory(move))
            HistoryBad(move, depth);
        }
    if (!cnt && NextMove->phase <= Trans2)
        return(0);
    HashUpper(Position->Dyn->Hash, depth, best_value);
    return(best_value);
    }
int MyLowDepthCheck(typePos *Position, int value, int depth)
    {
    int ignored, trans_move = 0, trans_depth, move_depth = 0;
    int Value, i, move, best_value, v, new_depth;
    bool Gen;
    TransDeclare();
    typeMoveList List[256], *list, *p, *q;
    typeDynamic *Pos0 = Position->Dyn;
	CheckForMate (value);

    CheckRepetition(true);
    Trans = HashPointer(Position->Dyn->Hash);
    for (i = 0; i < 4; i++, Trans++)
        {
        HyattHash(Trans, trans);
        if ((trans->hash ^ (Position->Dyn->Hash >> 32)) == 0)
            {
            if (trans->flags & FlagMoveLess)
                return(HeightMultiplier * Height(Position) - ValueMate);
            if (trans->DepthLower && trans->DepthLower >= depth)
                {
                Value = HashLowerValue(trans);
                HashReBound(Value, value);
                if (Value >= value)
                    return(Value);
                }
            if (trans->DepthUpper && trans->DepthUpper >= depth)
                {
                Value = HashUpperValue(trans);
                HashReBound(Value, value);
                if (Value < value)
                    return(Value);
                }
            trans_depth = trans->DepthLower;
            move = trans->move;
            if (move && trans_depth > move_depth)
                {
                move_depth = trans_depth;
                trans_move = move;
                }
            }
        }
    if (trans_move && !MyOK(Position, trans_move))
        trans_move = 0;
    best_value =  HeightMultiplier * Height(Position) - ValueMate;
    p = List;
    List[0].move = trans_move;
    Gen = false;
    List[1].move = 0;
    ignored = 0;
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
                        p->move |= (p->move & 0xffff) | ((HistoryValue(Position, p->move) >> 1) << 16);
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
        move = p->move;
        p++;
        if (IsRepetition(0))
            {
            best_value = MAX(0, best_value);
            continue;
            }
        if (IsInterpose(move) && value > -ValueCut && (move & 0x7fff) != trans_move && !MySEE(Position, move))
            {
            ignored++;
            continue;
            }
        move &= 0x7fff;
        Make(Position, move);
        EvalLowDepth(value, move);
        if (IllegalMove)
            {
            Undo(Position, move);
            continue;
            }
        if (Is_Exact(Position->Dyn->exact))
            v = -Position->Dyn->Value;
        else if (MoveIsCheck)
            v = -OppLowDepthCheck(Position, 1 - value, depth - 1 + ExtendInCheck);
        else
            {
            new_depth = depth - 2 + ExtendInCheck;
            if (QSearchCondition)
                v = -OppQsearch(Position, 1 - value, 0);
            else
                v = -OppLowDepth(Position, 1 - value, new_depth);
            }
        Undo(Position, move);
        CheckHalt();
        if (v <= best_value)
            continue;
        best_value = v;
        if (v >= value)
            {
            HashLower(Position->Dyn->Hash, move, MAX(1, depth), v);
            return(v);
            }
        }
    if (ignored && best_value < -ValueCut)
        best_value = value - 1;
    HashUpper(Position->Dyn->Hash, MAX(1, depth), best_value);
    return(best_value);
    }
