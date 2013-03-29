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

#ifndef root_analysis
#define root_analysis
#include "fire.h"
#include "control.h"
typeRootMoveList RootMoveList[256];
#include "root_analysis.c"
#include "white.h"
#else
#include "black.h"
#endif

int MyAnalysis(typePos *Position, int Alpha, int Beta, int depth)
    {
    int Cnt, origAlpha, best_value, cnt, move_is_check, new_depth, v;
    typeRootMoveList *p, *q;
    typeDynamic *Pos0 = Position->Dyn;
    uint32 move;
    int cpu, rp;
    int extend, LMR;
    int to, value;
    uint64 Nodes, NodesStore, nodes;

    if (MultiPV != 1)
        return MyMultiPV(Position, depth);

    if (Beta > ValueMate)
        Beta = ValueMate;
    if (Alpha < -ValueMate)
        Alpha = -ValueMate;
    if (DoOutput && DepthInfo)
		{
        Send("info depth %d\n", depth >> 1);
#ifdef Log
		if (WriteLog)
			{
			log_file = fopen(log_filename, "a");
			fprintf(log_file, "info depth %d\n", depth >> 1);
			close_log();
			}
#endif
		}
    Cnt = 0;
    for (p = RootMoveList; p->move; p++)
        {
        Cnt++;
        p->move &= 0x7fff;
        }
    origAlpha = Alpha;
    p = RootMoveList;
    v = best_value = -ValueInfinity;
        Nodes = 0;
        for (cpu = 0; cpu < NumThreads; cpu++)
            for (rp = 0; rp < RPperCPU; rp++)
                Nodes += RootPosition[cpu][rp].nodes;
    NodesStore = Nodes;
    cnt = 0;
    while ((move = p->move))
        {
        Make(Position, move);
		EVAL (move, depth);
        move_is_check = (MoveIsCheck != 0);
        extend = 0;
        to = To(move);
        LMR = 0;
        if (!extend && cnt >= 3 && depth >= 10)
            LMR = 1;
        if (!extend && cnt >= 6 && depth >= 10)
            LMR = 2;
        new_depth = depth - 2 + extend - LMR;
        if (Pos1->cp || move_is_check)
            extend = 1;
		if (PassedPawnPush (to, FourthRank (to)))
			extend = 1;

        new_depth = depth - 2 + extend;
        if (DoOutput && CurrMoveInfo && Analysing && depth >= 24)
			{
            Send("info currmove %s currmovenumber %d\n", Notate(move, String1[Position->cpu]),(p - RootMoveList) + 1);
#ifdef Log
			if (WriteLog)
				{
				log_file = fopen(log_filename, "a");
				fprintf(log_file, "info currmove %s currmovenumber %d\n", Notate(move, String1[Position->cpu]),(p - RootMoveList) + 1);
				close_log();
				}
#endif
			}
		if (Is_Exact (Position->Dyn->exact))
			v = -Position->Dyn->Value;
		else if (best_value == -ValueInfinity || depth <= 2)
			v = -OppPV(Position, -Beta, -Alpha, new_depth, move_is_check);
        else
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
                if (new_depth >= MinTransMoveDepth && Analysing)
                    {
                    int an = new_depth - 12;
                    v = ValueInfinity;
                    while (an <= new_depth && v > Alpha)
                        {
                        v = -OppPV(Position, -Alpha - 1, -Alpha, an, move_is_check);
                        an += 4;
                        }
                    if (an > new_depth)
                        {
                        goto Dec;
                        new_depth += LMR;
                        }
                    }
                if (move_is_check)
                    v = -OppCutCheck(Position, -Alpha, new_depth);
                else
                    v = -OppCut(Position, -Alpha, new_depth);
                }
            new_depth += LMR;
            if (v > Alpha)
                v = -OppPV(Position, -Alpha - 1, -Alpha, new_depth, move_is_check);
            Dec:
            if (v > Alpha)
                v = -OppPV(Position, -Beta, -Alpha, new_depth, move_is_check);
            if (v <= Alpha)
                v = Alpha;
            }
        Undo(Position, move);
        CheckHalt();
        cnt++;
        if (v <= Alpha)
            p->value = origAlpha;
        else
            p->value = v;
            Nodes = 0;
            for (cpu = 0; cpu < NumThreads; cpu++)
                for (rp = 0; rp < RPperCPU; rp++)
                    Nodes += RootPosition[cpu][rp].nodes;
        p->nodes = Nodes - NodesStore;
        NodesStore = Nodes;
        if (v > best_value)
            {
            best_value = v;
            if (best_value == -ValueInfinity || v > Alpha)
                {
                HashLowerPV(Position->Dyn->Hash, move, depth, v);
                RootBestMove = move;
                RootScore = v;
                RootDepth = depth;

                if (LowDepthPVs)
                    {
					if (v > Alpha && v < Beta)
						Information(Position, GetClock() - StartClock, origAlpha, v, Beta);
					}
                else
					if (depth > MinPVDepth)
                    {
					if (v > Alpha && v < Beta)
						Information(Position, GetClock() - StartClock, origAlpha, v, Beta);
					}
                }
            }
        if (v > Alpha)
            Alpha = v;
        if (v < Beta)
            {
            p++;
            continue;
            }
        break;
        }
    for (p = RootMoveList + (Cnt - 1); p >= RootMoveList; p--)
        {
        move = p->move;
        value = p->value;
        nodes = p->nodes;
        for (q = p + 1; q < RootMoveList + Cnt; q++)
            {
            if (value < q->value)
                {
                (q - 1)->move = q->move;
                (q - 1)->value = q->value;
                (q - 1)->nodes = q->nodes;
                }
            else
                break;
            }
        q--;
        q->move = move;
        q->value = value;
        q->nodes = nodes;
        }
    for (p = RootMoveList + Cnt; p > RootMoveList; p--)
        {
        move = p->move;
        value = p->value;
        nodes = p->nodes;
        for (q = p + 1; q < RootMoveList + Cnt; q++)
            {
            if (nodes < q->nodes)
                {
                (q - 1)->move = q->move;
                (q - 1)->value = q->value;
                (q - 1)->nodes = q->nodes;
                }
            else
                break;
            }
        (q - 1)->move = move;
        (q - 1)->value = value;
        (q - 1)->nodes = nodes;
        }

    RootDepth = depth;
    if (!DoSearchMoves)
        {
        if (best_value <= origAlpha)
            HashUpperPV(Position->Dyn->Hash, depth, origAlpha);
        else if (best_value < Beta)
            HashExact(Position, RootBestMove, depth, best_value, FlagExact);
        }
	if (LowDepthPVs)
		Information(Position, GetClock() - StartClock, origAlpha, best_value, Beta);
    else
		{
		if (depth > MinPVDepth)
			Information(Position, GetClock() - StartClock, origAlpha, best_value, Beta);
		}
    return best_value;
    }
