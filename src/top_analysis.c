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

#ifndef top_analysis
#define top_analysis
#include "fire.h"
#include "control.h"
#define IsCheck    \
	(Position->wtm ? \
		(wBitboardK & Position->Dyn->bAtt) : (bBitboardK & Position->Dyn->wAtt))
typeRootMoveList RootMoveList[256];
#include "top_analysis.c"
#include "white.h"
#else
#include "black.h"
#endif

void MyTopAnalysis(typePos *Position)
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
    int sm = 0;
	
    bool TriplePeek = false;
    int TriplePeekValue = -ValueInfinity;

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
        if (Analysing && DoSearchMoves)
            {
            sm = 0;
            while (SearchMoves[sm])
                {
                if (SearchMoves[sm] == move)
                    {
                    (q++)->move = move & 0x7fff;
                    break;
                    }
                sm++;
                }
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
            p->move |= 0xff000000 + (((PieceValue[to] << 4) - PieceValue[fr]) << 16);
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
            {
            RootScore = L;
            }
        else
            {
            RootScore = 0;
            }
        RootBestMove = 0;
        RootDepth = 0;
        return;
        }
    for (depth = 2; depth <= 250; depth += 2)
        {
        if (depth >= TopMinDepth && RootScore <= DeltaCutoff && -DeltaCutoff <= RootScore && MultiPV == 1)
            {
            A = 8;
            L = RootScore - A;
            U = RootScore + A;
            if (L < -DeltaCutoff)
                L = -ValueMate;
            if (U > DeltaCutoff)
                U = ValueMate;
            Again:
            v = MyAnalysis(Position, L, U, depth);
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
            v = MyAnalysis(Position, -ValueMate, ValueMate, depth);
        CheckIfDone:
        RootPrevious = RootScore;
        if (IvanAllHalt)
            return;
        CheckDone(Position, depth >> 1);
        if (IvanAllHalt)
            return;
        }
    }
