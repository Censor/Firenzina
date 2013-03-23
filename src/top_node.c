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

#ifndef top_node
#define top_node
#include "fire.h"
#include "control.h"
#define IsCheck    \
	(Position->wtm ? \
		(wBitboardK & Position->Dyn->bAtt) : (bBitboardK & Position->Dyn->wAtt))
typeRootMoveList RootMoveList[256];
#include "top_node.c"
#include "white.h"
#else
#include "black.h"
#endif

void MyTop(typePos *Position)
    {
    int i, depth, A, L, U, v, Value = 0, trans_depth;
    int move_depth = 0, ExactDepth = 0;
    uint32 move, HashMove = 0, ExactMove = 0, to, fr;
    typeMoveList *mlx, *ml, ML[256];
    typeRootMoveList *p, *q, *list;
    TransDeclare();
    typeDynamic *Pos0 = Position->Dyn;
    int PieceValue[16] =
        {
        0, 1, 3, 0, 3, 3, 5, 9, 0, 1, 3, 0, 3, 3, 5, 9
        };

#ifdef RobboBases
    bool TriplePeek = false;
    int TriplePeekValue = -ValueInfinity;
#endif

    if (Analysing)
        {
        MyTopAnalysis(Position);
        return;
        }
	Mobility (Position);
    if (IsCheck)
        ml = MyEvasion(Position, ML, 0xffffffffffffffff);
    else
        {
        mlx = MyCapture(Position, ML, OppOccupied);
        ml = MyOrdinary(Position, mlx);
        SortOrdinary(ml, mlx, 0, 0, 0);
        }
    Trans = HashPointer(Position->Dyn->Hash);
    for (i = 0; i < 4; i++, Trans++)
        {
        HyattHash(Trans, trans);
        if ((trans->hash ^ (Position->Dyn->Hash >> 32)) == 0)
            {
            trans_depth = trans->DepthLower;
            move = trans->move;
            if (IsExact(trans))
                {
                ExactDepth = trans_depth;
                ExactMove = move;
                Value = HashUpperBound(trans);
                }
            if (move && trans_depth > move_depth)
                {
                move_depth = trans_depth;
                HashMove = move;
                }
            }
        }
    if (ExactDepth >= PreviousDepth - 6 && ExactMove == HashMove && !Ponder && ExactMove && PreviousFast
       && PreviousDepth >= 18 && MyOK(Position, ExactMove) && Value < DeltaCutoff && Value > -DeltaCutoff)
        {
        RootScore = Value;
        RootBestMove = ExactMove;
        RootDepth = ExactDepth;
        PreviousFast = false;
        if (!IsCheck)
            v = MyExclude(Position, Value - 50, PreviousDepth - 6, ExactMove);
        else
            v = MyExcludeCheck(Position, Value - 50, PreviousDepth - 6, ExactMove);
        if (v < Value - 50)
            return;
        }
    PreviousFast = true;
    for (i = 0; i < ml - ML; i++)
        RootMoveList[i].move = ML[i].move;
    RootMoveList[ml - ML].move = MoveNone;
    list = RootMoveList + (ml - ML);

#ifdef RobboBases
	if (UseRobboBases)
		{
		if (TripleBasesLoaded && TripleCondition(Position) && TripleValue(Position, &v, false, true))
			{
			TriplePeek = true;
			TriplePeekValue = v;
			}
		}
#endif

    q = RootMoveList;
    for (p = RootMoveList; p < list; p++)
        {
        move = p->move & 0x7fff;
        Make(Position, move);
		Mobility (Position);
        if (IllegalMove)
            {
            Undo(Position, move);
            continue;
            }
#ifdef RobboBases
        else if (UseRobboBases && TripleBasesLoaded && TriplePeek && TripleValue(Position, &v, false, true))
            {
            v = -v;
            if (TriplePeekValue > 0 && v > 0)
                (q++)->move = move & 0x7fff;
            if (TriplePeekValue == 0 && v >= 0)
                {
                if (v > 0)
                    TriplePeekValue = 1;
                (q++)->move = move & 0x7fff;
                }
            if (TriplePeekValue < 0)
                {
                (q++)->move = move & 0x7fff;
                if (v > 0)
                    TriplePeekValue = 1;
                if (v == 0)
                    TriplePeekValue = 0;
                }
            }
#endif

        else
            (q++)->move = move & 0x7fff;
        Undo(Position, move);
        }
    q->move = 0;
    list = q;

#ifdef RobboBases
	if (UseRobboBases)
		{
		q = RootMoveList;
		for (p = RootMoveList; p < list; p++)
			{
			move = p->move & 0x7fff;
			Make(Position, move);
			Mobility(Position);
			if (UseRobboBases && TripleBasesLoaded && TriplePeek && TripleValue(Position, &v, false, true))
				{
				v = -v;
				if (TriplePeekValue > 0 && v > 0)
					(q++)->move = move & 0x7fff;
				if (TriplePeekValue == 0 && v >= 0)
					(q++)->move = move & 0x7fff;
				if (TriplePeekValue < 0)
					(q++)->move = move & 0x7fff;
				}
			else
				(q++)->move = move & 0x7fff;
			Undo(Position, move);
			}
		q->move = 0;
		list = q;
		}
#endif

    for (p = RootMoveList; p < list; p++)
        {
        if (Position->sq[To(p->move)])
            {
            to = Position->sq[To(p->move)];
            fr = Position->sq[From(p->move)];
            p->move |= 0xff000000 +((16 * PieceValue[to] - PieceValue[fr]) << 16);
            }
        }
    for (p = RootMoveList; p < list; p++)
        if (p->move == HashMove)
            p->move |= 0xffff0000;
    for (p = list - 1; p >= RootMoveList; p--)
        {
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
    L = -ValueMate;
    U = ValueMate;
    if (!RootMoveList[0].move)
        {
        if (IsCheck)
            RootScore = L;
        else
            RootScore = 0;
        RootBestMove = 0;
        RootDepth = 0;
        return;
        }
    for (depth = 2; depth <= MaxDepth; depth += 2)
        {
        BadMove = false;
        BattleMove = false;
        if (depth >= TopMinDepth && RootScore <= DeltaCutoff && -DeltaCutoff <= RootScore)
            {
            A = AspirationWindow;
            L = RootScore - A;
            U = RootScore + A;
            if (L < -DeltaCutoff)
                L = -ValueMate;
            if (U > DeltaCutoff)
                U = ValueMate;
            Again:
            v = MyRootNode(Position, L, U, depth);
            if (IvanAllHalt)
                goto CheckIfDone;
            if (v > L && v < U)
                goto CheckIfDone;
            if (v <= L)
                {
                RootScore = L;
                L -= A;
                A += A >> 1;
                goto Again;
                }
            else
                {
                RootScore = U;
                U += A;
                A += A >> 1;
                goto Again;
                }
            }
        else
            v = MyRootNode(Position, -ValueMate, ValueMate, depth);
        CheckIfDone:
        if (depth == 2)
            EasyMove = true;
        RootPrevious = RootScore;
        if (IvanAllHalt)
            return;
        CheckDone(Position, depth >> 1);
        if (IvanAllHalt)
            return;
        }
    }
