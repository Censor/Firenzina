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

#ifndef qsearch_pv
#define qsearch_pv
#include "fire.h"
#include "qsearch_pv.c"
#include "white.h"
#else
#include "black.h"
#endif

int MyPVQsearch(typePos *Position, int Alpha, int Beta, int depth)
    {
    int i;
    uint32 good_move = 0, trans_move = 0, move, BadCaps[64];
    uint32 trans_depth, move_depth = 0;
    int best_value, Value;
    uint64 Target;
    typeMoveList List[256], *list, *p, *q;
    int Temp, v;
    typeDynamic *Pos0 = Position->Dyn;
    int bc = 0;
    TransDeclare();
    CheckRepetition(false);
    if (Beta < -ValueMate)
        return(-ValueMate);
    if (Alpha > ValueMate)
        return(ValueMate);
    Trans = HashPointer(Position->Dyn->Hash);
    for (i = 0; i < 4; i++, Trans++)
        {
        HyattHash(Trans, trans);
        if ((trans->hash ^ (Position->Dyn->Hash >> 32)) == 0)
            {
            if (trans->flags & FlagMoveLess)
                return 0;
            if (IsExact(trans))
                {
                Value = HashUpperBound(trans);
                return(Value);
                }
            if (trans->DepthLower)
                {
                Value = HashLowerBound(trans);
                if (Value >= Beta)
                    return(Value);
                }
            if (trans->DepthUpper)
                {
                Value = HashUpperBound(trans);
                if (Value <= Alpha)
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
    best_value = Pos0->Value + TempoValue2;
    Target = OppOccupied;
    if (best_value >= Beta)
        {
        return(best_value);
        }
    else if (best_value > Alpha)
        Alpha = best_value;
    else
        {
        if (best_value < Alpha - PrunePawn && HasPiece)
            {
            Target ^= BitboardOppP;
            if (best_value < Alpha - PruneMinor)
                {
                Target ^= (BitboardOppN | BitboardOppB);
                if (best_value < Alpha - PruneRook)
                    Target ^= BitboardOppR;
                }
            best_value += PrunePawn;
            }
        }
    list = MyCapture(Position, List, Target);
    p = List;
    while (p->move)
        {
        if ((p->move & 0x7fff) == trans_move)
            p->move |= 0xffff0000;
        p++;
        }
    p = List;
    while (p->move)
        {
        move = p->move;
        q = ++p;
        while (q->move)
            {
            if (move < q->move)
                {
                Temp = q->move;
                q->move = move;
                move = Temp;
                }
            q++;
            }
        if (EasySEE(move) || (move & 0x7fff) == trans_move || MySEE(Position, move))
            {
            move &= 0x7fff;
            Make(Position, move);
			EVAL (move, depth);
            if (IllegalMove)
                {
                Undo(Position, move);
                continue;
                }
            if (Is_Exact(Position->Dyn->exact))
                v = -Position->Dyn->Value;
            else if (MoveIsCheck)
                v = -OppPVQsearchCheck(Position, -Beta, -Alpha, depth - 1);
            else
                v = -OppPVQsearch(Position, -Beta, -Alpha, depth - 1);
            Undo(Position, move);
            CheckHalt();
            if (v <= best_value)
                continue;
            best_value = v;
            if (v <= Alpha)
                continue;
            Alpha = v;
            good_move = move;
            if (v >= Beta)
                {
                HashLower(Position->Dyn->Hash, move, 1, v);
                return(v);
                }
            }
        else
            BadCaps[bc++] = move;
        }
    if (depth > 0)
        for (i = 0; i < bc; i++)
            {
            move = BadCaps[i] & 0x7fff;
            Make(Position, move);
			EVAL (move, depth);
            if (IllegalMove)
                {
                Undo(Position, move);
                continue;
                }
            if (Is_Exact(Position->Dyn->exact))
                v = -Position->Dyn->Value;
            else if (MoveIsCheck)
                v = -OppPVQsearchCheck(Position, -Beta, -Alpha, depth - 1);
            else
                v = -OppPVQsearch(Position, -Beta, -Alpha, depth - 1);
            Undo(Position, move);
            CheckHalt();
            if (v <= best_value)
                continue;
            best_value = v;
            if (v <= Alpha)
                continue;
            Alpha = v;
            good_move = move;
            if (v >= Beta)
                {
                HashLower(Position->Dyn->Hash, move, 1, v);
                return(v);
                }
            }
    if (depth >= -5 && Pos0->Value >= Alpha - (16 << (depth + 5)))
        {
        list = MyQuietChecks(Position, List, Target);
        for (i = 0; i < list - List; i++)
            {
            move = List[i].move & 0x7fff;
            Make(Position, move);
			EVAL (move, depth);
            if (IllegalMove)
                {
                Undo(Position, move);
                continue;
                }
            if (Is_Exact(Position->Dyn->exact))
                v = -Position->Dyn->Value;
            else
                v = -OppPVQsearchCheck(Position, -Beta, -Alpha, depth - 1);
            Undo(Position, move);
            CheckHalt();
            if (v <= best_value)
                continue;
            best_value = v;
            if (v <= Alpha)
                continue;
            Alpha = v;
            good_move = move;
            if (v >= Beta)
                {
                HashLower(Position->Dyn->Hash, move, 1, v);
                return(v);
                }
            }
        if (depth >= -4 && Pos0->Value >= Alpha - (2 << (4 + depth)))
            {
            list = MyPositionalGain(Position, List, Alpha - Pos0->Value + 5);
            for (i = 0; i < list - List; i++)
                {
                move = List[i].move & 0x7fff;
                Make(Position, move);
				EVAL (move, depth);
                if (-Pos1->Value < Alpha)
                    {
                    Undo(Position, move);
                    continue;
                    }
                if (IllegalMove || MoveIsCheck)
                    {
                    Undo(Position, move);
                    continue;
                    }
                if (Is_Exact(Position->Dyn->exact))
                    v = -Position->Dyn->Value;
                else
                    v = -OppPVQsearch(Position, -Beta, -Alpha,  depth - 1);
                Undo(Position, move);
                CheckHalt();
                if (v <= best_value)
                    continue;
                best_value = v;
                if (v <= Alpha)
                    continue;
                Alpha = v;
                good_move = move;
                HashLower(Position->Dyn->Hash, move, 1, v);
                if (v >= Beta)
                    return(v);
                }
            }
        }
    if (good_move)
        {
        HashExact(Position, good_move, 1, best_value, FlagExact);
        return(best_value);
        }
    HashUpper(Position->Dyn->Hash, 1, best_value);
    return(best_value);
    }
int MyPVQsearchCheck(typePos *Position, int Alpha, int Beta, int depth)
    {
    int i;
    uint32 trans_move = 0, good_move = 0, move, Temp;
    int best_value, Value;
    uint64 Target;
    typeMoveList List[256], *list, *p, *q;
    int v, trans_depth, move_depth = 0;
    typeDynamic *Pos0 = Position->Dyn;
    TransDeclare();
    CheckRepetition(true);
    if (Beta < -ValueMate)
        return(-ValueMate);
    if (Alpha > ValueMate)
        return(ValueMate);
    Trans = HashPointer(Position->Dyn->Hash);
    for (i = 0; i < 4; i++, Trans++)
        {
        HyattHash(Trans, trans);
        if ((trans->hash ^ (Position->Dyn->Hash >> 32)) == 0)
            {
            if (trans->flags & FlagMoveLess)
                return(HeightMultiplier * Height(Position) - ValueMate);
            if (IsExact(trans))
                {
                Value = HashUpperBound(trans);
                return(Value);
                }
            if (trans->DepthLower)
                {
                Value = HashLowerBound(trans);
                if (Value >= Beta)
                    return(Value);
                }
            if (trans->DepthUpper)
                {
                Value = HashUpperBound(trans);
                if (Value <= Alpha)
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
    best_value = HeightMultiplier * Height(Position) - ValueMate;
    Target = 0xffffffffffffffff;
    if (Pos0->Value + PruneCheck < Alpha && HasPiece)
        {
        best_value = Pos0->Value + PruneCheck;
        v = Alpha - QSAlphaThreshold;
        Target = OppOccupied;
        if (v > best_value)
            {
            Target ^= BitboardOppP;
            v = Alpha - QSAlphaThreshold * 250 / 100;
            best_value += QSAlphaThreshold;
            if (v > best_value)
                Target ^= (BitboardOppN | BitboardOppB);
            }
        }
    list = MyEvasion(Position, List, Target);
    if ((list - List) != 1)
        depth--;
    p = List;
    while (p->move)
        {
        if ((p->move & 0x7fff) == trans_move)
            p->move |= 0xfff00000;
        p++;
        }
    p = List;
    while (p->move)
        {
        move = p->move;
        q = ++p;
        while (q->move)
            {
            if (move < q->move)
                {
                Temp = q->move;
                q->move = move;
                move = Temp;
                }
            q++;
            }
        move &= 0x7fff;
        Make(Position, move);
        EVAL(move, depth);
        if (IllegalMove)
            {
            Undo(Position, move);
            continue;
            }
        if (Is_Exact(Position->Dyn->exact))
            v = -Position->Dyn->Value;
        else if (MoveIsCheck)
            v = -OppPVQsearchCheck(Position, -Beta, -Alpha, depth);
        else
            v = -OppPVQsearch(Position, -Beta, -Alpha, depth);
        Undo(Position, move);
        CheckHalt();
        if (v <= best_value)
            continue;
        best_value = v;
        if (v <= Alpha)
            continue;
        Alpha = v;
        good_move = move;
        HashLower(Position->Dyn->Hash, move, 1, v);
        if (v >= Beta)
            return(v);
        }
    if (good_move)
        {
        HashExact(Position, good_move, 1, best_value, FlagExact);
        return(best_value);
        }
    HashUpper(Position->Dyn->Hash, 1, best_value);
    return(best_value);
    }