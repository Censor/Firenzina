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

#ifndef qsearch
#define qsearch
#include "fire.h"
#include "qsearch.c"
#include "white.h"
#else
#include "black.h"
#endif

int MyQsearch(typePos *Position, int value, int depth)
    {
    int Value, i, v, best_value;
    uint32 Temp, move, trans_move = 0, trans_depth, move_depth = 0;
    uint64 Target;
    typeMoveList List[256], *list, *p, *q;
    typeDynamic *Pos0 = Position->Dyn;
    TransDeclare();
	
	CheckForMate (value);

    CheckRepetition(false);
    Trans = HashPointer(Position->Dyn->Hash);
    for (i = 0; i < 4; i++, Trans++)
        {
        HyattHash(Trans, trans);
        if ((trans->hash ^ (Position->Dyn->Hash >> 32)) == 0)
            {
            if (trans->flags & FlagMoveLess)
                return 0;
            if (trans->DepthLower)
                {
                Value = HashLowerValue(trans);
                HashReBound(Value, value);
                if (Value >= value)
                    return(Value);
                }
            if (trans->DepthUpper)
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
    best_value = Pos0->Value + TempoValue2;
    if (best_value >= value)
        return(best_value);
    v = value - PrunePawn;
    Target = OppOccupied;
    if (v > best_value && HasPiece)
        {
        v = value - PruneMinor;
        Target ^= BitboardOppP;
        if (v > best_value)
            {
            Target ^= (BitboardOppN | BitboardOppB);
            v = value - PruneRook;
            if (v > best_value)
                Target ^= BitboardOppR;
            }
        if (BitboardOppP & MyAttacked)
            best_value += PrunePawn;
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
        if (!EasySEE(move) && (move & 0x7fff) != trans_move && SqSet[From(move)] & ~MyXray && !MySEE(Position, move))
            continue;
        move &= 0x7fff;
        Make(Position, move);
        EvalQsearch(value, move);
        if (IllegalMove)
            {
            Undo(Position, move);
            continue;
            }
        if (Is_Exact(Position->Dyn->exact))
            v = -Position->Dyn->Value;
        else if (MoveIsCheck)
            v = -OppQsearchCheck(Position, 1 - value, depth - 1);
        else
            v = -OppQsearch(Position, 1 - value, depth - 1);
        Undo(Position, move);
        CheckHalt();
        if (v <= best_value)
            continue;
        best_value = v;
        if (v >= value)
            {
            HashLower(Position->Dyn->Hash, move, 1, v);
            return(v);
            }
        }
    if (depth >= -1 && Pos0->Value >= value - (100 + (12 << (depth + 4))))
            {
            list = MyQuietChecks(Position, List, Target);
            for (i = 0; i < list - List; i++)
                {
                move = List[i].move;
                move &= 0x7fff;
                Make(Position, move);
                EvalQsearch(value, move);
                if (IllegalMove)
                    {
                    Undo(Position, move);
                    continue;
                    }
                if (Is_Exact(Position->Dyn->exact))
                    v = -Position->Dyn->Value;
                else
                    v = -OppQsearchCheck(Position, 1 - value, depth - 1);
                Undo(Position, move);
                CheckHalt();
                if (v <= best_value)
                    continue;
                best_value = v;
                if (v >= value)
                    {
                    HashLower(Position->Dyn->Hash, move, 1, v);
                    return(v);
                    }
                }
            }
    HashUpper(Position->Dyn->Hash, 1, best_value);
    return(best_value);
    }
int MyQsearchCheck(typePos *Position, int value, int depth)
    {
    int ignored, Value, i;
    int v, best_value, trans_depth, move_depth = 0;
    TransDeclare();
    uint64 Target;
    typeMoveList List[256], *list, *p, *q;
    typeDynamic *Pos0;
    uint32 move, Temp, trans_move = 0;
    Pos0 = Position->Dyn;
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
            if (trans->DepthLower)
                {
                Value = HashLowerValue(trans);
                HashReBound(Value, value);
                if (Value >= value)
                    return(Value);
                }
            if (trans->DepthUpper)
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
    best_value = HeightMultiplier * Height(Position) - ValueMate;
    Target = 0xffffffffffffffff;
    if (Pos0->Value + PruneCheck < value && HasPiece)
        {
        best_value = Pos0->Value + PruneCheck;
        v = value - 200;
        Target = OppOccupied;
        if (v > best_value)
            {
            Target ^= BitboardOppP;
            v = value - 500;
            best_value += 200;
            if (v > best_value)
                Target ^= (BitboardOppN | BitboardOppB);
            }
        }
    list = MyEvasion(Position, List, Target);
    if ((list - List) > 1)
        depth--;
    p = List;
    while (p->move)
        {
        if ((p->move & 0x7fff) == trans_move)
            p->move |= 0xfff00000;
        p++;
        }
    p = List;
    ignored = 0;
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
        if (IsInterpose(move) && value > -ValueCut && (move & 0x7fff) != trans_move && !MySEE(Position, move))
            {
            ignored++;
            continue;
            }
        if (Position->sq[To(move)] == 0 && (move & 0x6000) == 0 && (move & 0x7fff)
           != trans_move && HasPiece && MaxPositional(move) + Pos0->Value < value + 25 && value > -ValueCut)
            {
            ignored++;
            continue;
            }
        move &= 0x7fff;
        Make(Position, move);
        EvalQsearch(value, move);
        if (IllegalMove)
            {
            Undo(Position, move);
            continue;
            }
        if (Is_Exact(Position->Dyn->exact))
            v = -Position->Dyn->Value;
        else if (MoveIsCheck)
            v = -OppQsearchCheck(Position, 1 - value, depth);
        else
            v = -OppQsearch(Position, 1 - value, depth);
        Undo(Position, move);
        CheckHalt();
        if (v <= best_value)
            continue;
        best_value = v;
        if (v >= value)
            {
            HashLower(Position->Dyn->Hash, move, 1, v);
            return(v);
            }
        }
    if (ignored && best_value < -ValueCut)
        best_value = value - 1;
    HashUpper(Position->Dyn->Hash, 1, best_value);
    return(best_value);
    }