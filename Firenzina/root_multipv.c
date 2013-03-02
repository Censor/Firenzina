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

#ifndef root_multipv
#define root_multipv
#include "fire.h"
#include "control.h"
typeRootMoveList RootMoveList[256];
void ApplySort(int n, typeMPV *mpv)
    {
    typeMPV *p;
    int s, x;
    uint64 y;
    for (s = 0; s < n && mpv[s].move; s++)
        {
        if (s == 0)
            continue;
        p = &mpv[s];
        while (p != mpv && p->Value > (p - 1)->Value)
            {
            x = p->Value;
            p->Value = (p - 1)->Value;
            (p - 1)->Value = x;
            x = p->move;
            p->move = (p - 1)->move;
            (p - 1)->move = x;
            x = p->depth;
            p->depth = (p - 1)->depth;
            (p - 1)->depth = x;
            x = p->alpha;
            p->alpha = (p - 1)->alpha;
            (p - 1)->alpha = x;
            x = p->beta;
            p->beta = (p - 1)->beta;
            (p - 1)->beta = x;
            y = p->nodes;
            p->nodes = (p - 1)->nodes;
            (p - 1)->nodes = y;
            p--;
            }
        }
    }
#include "root_multipv.c"
#include "white.h"
#else
#include "black.h"
#endif
#define CheckHaltMulti() { if (IvanAllHalt || Position->stop) goto UndoLabel; }

int MyMultiPV(typePos *Position, int depth)
    {
    int Cnt, cnt, best_value, move_is_check, new_depth, v;
    typeRootMoveList *p, *q;
    typeDynamic *Pos0 = Position->Dyn;
    uint32 move;
    int cpu, rp;
    int extend, LMR, value;
    int to;
    int i, j, x, moveno;
    int Alpha = -ValueMate, Target, Delta, Alpha2, Lower;
    int GoodMoves = 0;
    uint64 Nodes, NodesStore, nodes, y;
    if (depth < 14)
        for (i = 0; i < 0x100; i++)
            MPV[i].move = MPV[i].Value = 0;
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
        Target = -ValueInfinity;
        Delta = ValueMate;
        for (moveno = 0; MPV[moveno].move; moveno++)
            if (MPV[moveno].move == move)
                {
                Target = MPV[moveno].Value;
                break;
                }
        if (Target == -ValueInfinity)
            Target = 0;
        else
            Delta = 8;

        if (GoodMoves > 0)
            Alpha2 = MPV[0].Value - MultiCentiPawnPV;
        else
            Alpha2 = -ValueInfinity;

        Make(Position, move);
		EVAL (move, depth);
        move_is_check = (MoveIsCheck != 0);
        extend = 0;
        to = To(move);
			
        if (Pos1->cp || move_is_check)
			extend = 1;
		if (PassedPawnPush (to, FourthRank (to)))
			extend = 1;	
			
        LMR = 0;
        if (!extend && cnt >= (GoodMoves << 1) + 3 && depth >= 10)
            LMR = 1;
        if (!extend && cnt >= (GoodMoves << 1) + 6 && depth >= 10)
            LMR = 2;
        new_depth = depth - 2 + extend - LMR;
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

        if (GoodMoves < MultiPV || depth <= 2)
            {
            while (true)
                {
                Lower = MAX(Target - Delta, Alpha2);
				if (Is_Exact (Position->Dyn->exact))
					v = -Position->Dyn->Value;
				else
					v = -OppPV(Position, -Target - Delta, -Lower, new_depth, move_is_check);
                CheckHaltMulti();
                if (v < Target + Delta && v > Lower)
                    break;
                if (GoodMoves && v <= Alpha2)
                    break;
                new_depth += LMR;
                LMR = 0;
                MPV[moveno].move = move;
                MPV[moveno].depth = depth;
                MPV[moveno].alpha = Lower;
                MPV[moveno].beta = Target + Delta;
                MPV[moveno].Value = v;
                Undo(Position, move);
                Information(Position, GetClock() - StartClock, MPV[moveno].alpha, MPV[moveno].Value, MPV[moveno].beta);
                Make(Position, move);
				EVAL (move, depth);
                if (v >= Target + Delta)
                    Target = Target + (Delta >> 1);
                else
                    Target = Target - (Delta >> 1);
                if (Target + Delta >= DeltaCutoff || Target - Delta <= -DeltaCutoff)
                    {
                    Target = 0;
                    Delta = ValueMate;
                    }
                else
                    Delta += Delta >> 1;
                }
            }
        else
            {
            if (LowDepthConditionPV)
                {
				if (Is_Exact (Position->Dyn->exact))
					v = -Position->Dyn->Value;
				else if (move_is_check)
                    v = -OppLowDepthCheck(Position, -Alpha, new_depth);
                else
                    v = -OppLowDepth(Position, -Alpha, new_depth);
                CheckHaltMulti();
                }
            else
                {
                if (new_depth >= MinTransMoveDepth && Analysing)
                    {
                    int an = new_depth - 12;
                    v = ValueInfinity;
                    while (an <= new_depth && v > Alpha)
                        {
						if (Is_Exact (Position->Dyn->exact))
							v = -Position->Dyn->Value;
						else
							v = -OppPV(Position, -Alpha - 1, -Alpha, an, move_is_check);
                        CheckHaltMulti();
                        an += 4;
                        }
                    if (an > new_depth)
                        {
                        new_depth += LMR;
                        goto EnCircle;
                        }
                    }
				if (Is_Exact (Position->Dyn->exact))
					v = -Position->Dyn->Value;
				else if (move_is_check)
                    v = -OppCutCheck(Position, -Alpha, new_depth);
                else
                    v = -OppCut(Position, -Alpha, new_depth);
                CheckHaltMulti();
                }
            new_depth += LMR;
            if (v > Alpha)
                v = -OppPV(Position, -Alpha - 1, -Alpha, new_depth, move_is_check);
            EnCircle:
            CheckHaltMulti();
            Delta = 8;
            while (v > Alpha)
                {
                if (Is_Exact(Position->Dyn->exact))
                    v = -Position->Dyn->Value;
                else
                v = -OppPV(Position, -Alpha - Delta, -Alpha, new_depth, move_is_check);
                if (IvanAllHalt)
                    break;
                if (v < Alpha + Delta)
                    break;
                Delta += Delta >> 1;
                if (Alpha + Delta >= DeltaCutoff)
                    Delta = ValueMate - Alpha;
                }
            }
        UndoLabel:
        Undo(Position, move);
        CheckHalt();
            Nodes = 0;
            for (cpu = 0; cpu < NumThreads; cpu++)
                for (rp = 0; rp < RPperCPU; rp++)
                    Nodes += RootPosition[cpu][rp].nodes;
        p->nodes = Nodes - NodesStore;
        NodesStore = Nodes;
        if (v > Alpha)
            {
            p->value = v;
            if (v > best_value)
                HashLowerPV(Position->Dyn->Hash, move, depth, v);
            MPV[GoodMoves].move = move;
            MPV[GoodMoves].depth = depth;
            MPV[GoodMoves].alpha = -ValueMate;
            MPV[GoodMoves].beta = ValueMate;
            MPV[GoodMoves++].Value = v;
            ApplySort(GoodMoves, MPV);

            if (GoodMoves >= MultiPV)
                Alpha = MPV[MultiPV - 1].Value;
            else
                Alpha = MAX(MPV[0].Value - MultiCentiPawnPV, -ValueMate);

            RootBestMove = MPV[0].move;
            best_value = RootScore = MPV[0].Value;
			if (LowDepthPVs)
            	Information(Position, GetClock() - StartClock, -ValueMate, MPV[0].Value, ValueMate);
			else
				{
				if (depth > MinPVDepth)			
            		Information(Position, GetClock() - StartClock, -ValueMate, MPV[0].Value, ValueMate);
				}
            }
        else
            p->value = Alpha;
        p++;
        }
    MPV[GoodMoves].move = MoveNone;
    for (i = 0; i < GoodMoves; i++)
        {
        for (j = 0; j < Cnt; j++)
            {
            if (RootMoveList[j].move == MPV[i].move)
                {
                x = RootMoveList[i].move;
                RootMoveList[i].move = RootMoveList[j].move;
                RootMoveList[j].move = x;
                x = RootMoveList[i].value;
                RootMoveList[i].value = RootMoveList[j].value;
                RootMoveList[j].value = x;
                y = RootMoveList[i].nodes;
                RootMoveList[i].nodes = RootMoveList[j].nodes;
                RootMoveList[j].nodes = y;
                break;
                }
            }
        }

    for (p = RootMoveList + Cnt; p >= RootMoveList + GoodMoves; p--)
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
        HashExact(Position, MPV[0].move, depth, MPV[0].Value, FlagExact);
    Information(Position, GetClock() - StartClock, -ValueMate, MPV[0].Value, ValueMate);
    return MPV[0].Value;
    }