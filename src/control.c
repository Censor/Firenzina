/*******************************************************************************
Firenzina is a UCI chess playing engine by
Kranium (Norman Schmidt), Yuri Censor (Dmitri Gusev) and ZirconiumX (Matthew Brades).
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

#include "fire.h"
#include "control.h"

#define Infinity 0x7ffffffffffffff
#define StrTok(p) p = strtok (NULL, " ")

static sint64 LastMessage;
static sint64 AbsoluteTime, DesiredTime, Increment;
static int Depth;
static bool NewPonderhit;
jmp_buf J;
static sint64 BattleTime, EasyTime, NormalTime;

void PonderHit()
    {
    if (!DoPonder)
        return;
    Ponder_Hit = true;
    DoPonder = false;
    NewPonderhit = true;
    AbsoluteTime += GetClock() - StartClock;
    }
void HaltSearch(int d, int rank)
    {
    Stop = true;
    DoPonder = false;
    DoInfinite = false;
    SuppressInput = true;
        SMPAllHalt = true;
        if (d == 0)
            EndSMP();
    }

static int DoHashFull(uint64 x)
    {
	int d;
	float c = 0.499f;
	typeHash* Trans;
	for (d = 0; d < 1000; d++)
		{
		x = (0xeca97530f2468bd1) * x + 0x43218765edcb09af;
		Trans = HashPointer0 (x);
		if (Trans->hash && Trans->age == GlobalAge)
			c += 1.0f;
		}
  return (int) c;
    }

void Info(sint64 x)
    {
    uint64 t, nps, Nodes = 0;
	int cpu, rp, hash_full = 0;
    clock_t u;

#ifdef RobboBases
    uint64 TBHits = 0;
#endif

    if (x < 0)
        return;
	DoOutput = true;

    for (cpu = 0; cpu < NumThreads; cpu++)
        for (rp = 0; rp < RPperCPU; rp++)
            Nodes += RootPosition[cpu][rp].nodes;

#ifdef RobboBases
	if (UseRobboBases)
		{
		for (cpu = 0; cpu < NumThreads; cpu++)
			for (rp = 0; rp < RPperCPU; rp++)
				TBHits += RootPosition[cpu][rp].tbhits;
		}
#endif

    u = clock();

    if (HashFullInfo)
		hash_full = DoHashFull (((sint64) u) + x + Nodes);
    t = x / 1000;

    if (t == 0)
        nps = 0;
    else
        nps = Nodes / t;
    u = ProcessClock() - CPUtime;

	if (NPSInfo)
		Send("info time " Type64Bit " nodes " Type64Bit " nps " Type64Bit"\n", t, Nodes, nps * 1000);

	if (HashFullInfo && hash_full)
		Send ("info hashfull %d\n", hash_full);

#ifdef RobboBases
	if (UseRobboBases)
		{
		if (TBHitInfo && TBHits)
			Send("info tbhits " Type64Bit"\n", TBHits);
		}
#endif

	if (CPULoadInfo)
    	Send("info cpuload %d\n", (int) MIN(((double) u / (double) ((x - LastMessage) * NumCPUs) * 1000.0), 1000));

#ifdef Log
	if (WriteLog)
		{
		log_file = fopen(log_filename, "a");

		if (NPSInfo)
			fprintf(log_file, "info time " Type64Bit " nodes " Type64Bit " nps " Type64Bit"\n", t, Nodes, nps * 1000);

		if (HashFullInfo && hash_full)
			fprintf(log_file, "info hashfull %d\n", hash_full);

#ifdef RobboBases
		if (UseRobboBases)
			{
			if (TBHitInfo && TBHits)
				fprintf(log_file, " tbhits " Type64Bit, TBHits);
			}
#endif

		if (CPULoadInfo)
			fprintf(log_file, "info cpuload %d\n",
				(int)MIN(((double)u / (double)((x - LastMessage) * NumCPUs) * 1000.0), 1000));
		close_log();
		}
#endif

    LastMessage = x;
    CPUtime += u;
    }

void CheckDone(typePos * Position, int d)
    {
    sint64 x;
    if (!RootBestMove)
        return;
    if (SMPAllHalt)
        {
        HaltSearch(d, 1);
        return;
        }
    if (SuppressInput)
        return;
    if (!JumpIsSet)
        return;
    x = GetClock() - StartClock;
    if (d && d == Depth)
        {
        HaltSearch(d, 1);
        return;
        }
    if (!BenchMarking  && x - LastMessage > 1000000)
        Info(x);
    if (DoPonder)
        goto End;
    if (DoInfinite)
        goto End;
    if (d >= 1 && d < 8)
        goto End;
    if (x > AbsoluteTime)
        {
        HaltSearch(d, 1);
        return;
        }
    if (d == 0 && !NewPonderhit)
        goto End;
    NewPonderhit = false;

    if (!BadMove && x >= BattleTime)
        {
        HaltSearch(d, 2);
        return;
        }
    if (EasyMove && x >= EasyTime)
        {
        HaltSearch(d, 3);
        return;
        }
    if (!BattleMove && x >= NormalTime && !BadMove)
        {
        HaltSearch(d, 4);
        return;
        }
    End:
    if (d)
        return;

#ifdef Bench
	if (!BenchMarking)
		{
#endif

		while (TryInput())
			{
			Input(Position);
			if (d == 0 && !SMPisActive)
				return;
			}
#ifdef Bench
		}
#endif

	}
void TimeManager(sint64 Time, sint64 OppTime, sint64 Increment, int mtg)
    {
	if (mtg)
        {
        if (mtg > 25)
            mtg = 25;
        DesiredTime = Time / mtg + Increment;
        AbsoluteTime = (Time * mtg) / ((mtg << 2) - 3) - MIN(1000000, Time / 10);
        if (mtg == 1)
            AbsoluteTime -= MIN(1000000, AbsoluteTime / 10);
        if (AbsoluteTime < 1000)
            AbsoluteTime = 1000;
        }
    else
        {
        AbsoluteTime = (Time * AbsoluteFactor) / 100 - 10000;
        if (AbsoluteTime < 1000)
            AbsoluteTime = 1000;
        DesiredTime = (Time * DesiredMillis) / 1000 + Increment;
        }
    if (DesiredTime > AbsoluteTime)
        DesiredTime = AbsoluteTime;
    if (DesiredTime < 1000)
        DesiredTime = 1000;
    EasyTime = (DesiredTime * EasyFactor) / 100;
    if (Ponder)
        EasyTime = (DesiredTime * EasyFactorPonder) / 100;
    BattleTime = (DesiredTime * BattleFactor) / 100;
    NormalTime = (DesiredTime * NormalFactor) / 100;
    }

void InitSearch(typePos * Position, char *str)
    {
    char *p;
    sint64 wtime = Infinity, btime = Infinity, winc = 0, binc = 0, Time, OppTime, mtg = 0;
    int sm = 0;
    Depth = 255;
	seldepth = 0;
    AbsoluteTime = DesiredTime = Infinity;
    Stop = false;
    DoInfinite = false;
    DoPonder = false;
    NewPonderhit = false;
    DoSearchMoves = false;
    LastMessage = 0;
    p = strtok(str, " ");
    for (StrTok(p); p != NULL; StrTok(p))
        {
        if (!strcmp(p, "depth"))
            {
            StrTok(p);
            Depth = MAX(1, atoi(p));
            }
        else if (!strcmp(p, "movetime"))
            {
            StrTok(p);
            AbsoluteTime = MAX(1, atoll(p)) * 1000;
            }
        else if (!strcmp(p, "wtime"))
            {
            StrTok(p);
            wtime = atoll(p) * 1000;
            }
        else if (!strcmp(p, "winc"))
            {
            StrTok(p);
            winc = atoll(p) * 1000;
            }
        else if (!strcmp(p, "btime"))
            {
            StrTok(p);
            btime = atoll(p) * 1000;
            }
        else if (!strcmp(p, "binc"))
            {
            StrTok(p);
            binc = atoll(p) * 1000;
            }
        else if (!strcmp(p, "movestogo"))
            {
            StrTok(p);
            mtg = atoi(p);
            }
        else if (!strcmp(p, "infinite"))
            DoInfinite = true;
        else if (!strcmp(p, "ponder"))
            DoPonder = true;
        else if (!strcmp(p, "searchmoves"))
            DoSearchMoves = true;
        else if (DoSearchMoves)
            SearchMoves[sm++] = NumericMove(Position, p);
        else
            ErrorEnd("go string: %s", p);
        }
    BattleTime = Infinity;
    NormalTime = Infinity;
    EasyTime = Infinity;

    Time = Position->wtm ? wtime : btime;
    OppTime = Position->wtm ? btime : wtime;
    if (Time < 0)
        Time = 0;
    if (Time == Infinity)
        goto End;
    Increment = Position->wtm ? winc : binc;
        TimeManager(Time, OppTime, Increment, mtg);
    End:
    if (Time == Infinity)
        Analysing = true;
    else
        Analysing = false;
    if (DoSearchMoves)
        SearchMoves[sm] = MoveNone;
    }
