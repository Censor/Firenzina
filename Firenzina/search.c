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

#include "fire.h"
#include "control.h"
#include "null_move.h"

#define Legal (Position->wtm ? !BlackInCheck : !WhiteInCheck)
#define InCheck (Position->wtm ? WhiteInCheck : BlackInCheck)
#define MaxMatePly 128

static void OutputBestMove(typePos *Position)
    {
    int i, k;
    bool b;
    TransPVDeclare();
    int PonderMove = MoveNone;
    if (!RootBestMove)
        {
        Send("bestmove NULL\n");

#ifdef Log
		if (WriteLog)
			{
			log_file = fopen(log_filename, "a");
			fprintf(log_file, "bestmove NULL\n");
			fprintf(log_file, "\n");
			close_log();
			}
#endif

        return;
        }
    Make(Position, RootBestMove);
	Mobility (Position);
    k = Position->Dyn->Hash & PVHashMask;
    for (i = 0; i < 4; i++)
        {
        Trans_pv = PVHashTable + (k + i);
        HyattHash(Trans_pv, trans_pv);
        if (trans_pv->hash == Position->Dyn->Hash)
            {
            PonderMove = trans_pv->move;
            break;
            }
        }
    if (Ponder && !PonderMove)
        {
        int depth;
        for (depth = 2; depth <= 10; depth += 2)
            Position->wtm
               ? PVNodeWhite(Position, -ValueInfinity, ValueInfinity, depth, Position->Dyn->bAtt & wBitboardK)
               : PVNodeBlack(Position, -ValueInfinity, ValueInfinity, depth, Position->Dyn->wAtt & bBitboardK);
        PonderMove = (Position->Dyn + 1)->move;
        }
    if ((Position->wtm ? !WhiteOK(Position, PonderMove) : !BlackOK(Position, PonderMove)))
        PonderMove = MoveNone;
    else
        {
        Make(Position, PonderMove);
		Mobility (Position);
        b = (Legal);
        Undo(Position, PonderMove);
        if (!b)
            PonderMove = MoveNone;
        }
    Undo(Position, RootBestMove);
    Send("bestmove %s ponder %s\n", Notate(RootBestMove, String1[Position->cpu]),
		Notate(PonderMove, String2[Position->cpu]));
	   
#ifdef Log
	if (WriteLog)
		{
		log_file = fopen(log_filename, "a");
		fprintf(log_file, "bestmove %s ponder %s\n", Notate(RootBestMove, String1[Position->cpu]),
			Notate(PonderMove, String2[Position->cpu]));
		fprintf(log_file, "\n");
		close_log();
		}
#endif
    }
static char *Modifier(int Alpha, int Value, int Beta, char *s)
    {
    s[0] = 0;
    if (Value <= Alpha)
        strcpy(s, " upperbound");
    else if (Value >= Beta)
        strcpy(s, " lowerbound");
    else
        strcpy(s, "");
    return s;
    }

static char *cp_mate(int Value, char *s)
    {
	if (Value > ValueMate - (MaxMatePly << 6))
		sprintf (s, "mate %d", (ValueMate + 64 - Value) >> 7);
	else if (Value < -ValueMate + (MaxMatePly << 6))
		sprintf (s, "mate %d",(-ValueMate - Value) >> 7);
    else
        sprintf(s, "cp %d", Value);
    return s;
    }
void Information(typePos *Position, sint64 x, int Alpha, int Value, int Beta)
    {
    uint64 t, nps, Nodes = 0;
    int cpu, rp;
    int sd, k, move;
    char pv[0x100 << 3], *q;
    TransPVDeclare();
    TransDeclare();
    uint64 HashStack[256];
    int i;
    int cnt = 0;
    bool B;
    int mpv;

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

    sd = 0;
    memset(HashStack, 0, (sizeof(uint64) << 8));
    t = x / 1000;
    if (t == 0)
        nps = 0;
    else
        nps = Nodes / t;
    if (MultiPV == 1)
        {
        MPV[0].move = RootBestMove;
        MPV[0].Value = Value;
        MPV[0].alpha = Alpha;
        MPV[0].beta = Beta;
        MPV[0].depth = RootDepth;
        }
    for (mpv = 0; mpv < MultiPV; mpv++)
        {
        move = MPV[mpv].move;
        if (move == MoveNone)
            break;
        q = pv;
        cnt = 0;
        HashStack[cnt++] = Position->Dyn->Hash;
        Notate(move, String1[Position->cpu]);
        strcpy(q, String1[Position->cpu]);
        q += strlen(String1[Position->cpu]);
        strcpy(q, " ");
        q++;
        while (move)
            {
            Make(Position, move);
			Mobility (Position);
            B = false;
            for (i = 0; i < cnt; i++)
                if (HashStack[i] == Position->Dyn->Hash)
                    B = true;
            if (B)
                break;
            HashStack[cnt++] = Position->Dyn->Hash;
            move = 0;
            k = Position->Dyn->Hash & PVHashMask;
            for (i = 0; i < 4; i++)
                {
                Trans_pv = PVHashTable + (k + i);
                HyattHash(Trans_pv, trans_pv);
                if (trans_pv->hash == Position->Dyn->Hash)
                    {
                    move = trans_pv->move;
                    break;
                    }
                }
            if (!move)
                {
                Trans = HashPointer(Position->Dyn->Hash);
                for (i = 0; i < 4; i++, Trans++)
                    {
                    HyattHash(Trans, trans);
                    if (trans->hash == Position->Dyn->Hash)
                        {
                        move = trans->move;
                        break;
                        }
                    }
                }
            if (!move || (Position->wtm ? !WhiteOK(Position, move) : !BlackOK(Position, move)))
                break;
            if (cnt > 250 || Position->Dyn->reversible > 100)
                break;
            Notate(move, String1[Position->cpu]);
            strcpy(q, String1[Position->cpu]);
            q += strlen(String1[Position->cpu]);
            strcpy(q, " ");
            q++;
            }
        q--;
        *q = 0;
        while (Position->Dyn != (Position->DynRoot + 1))
            {
            if (!Position->Dyn->move)
                UndoNull(Position);
            else
                Undo(Position, Position->Dyn->move);
            }
        Send("info multipv %d time " Type64Bit " nodes " Type64Bit " nps " Type64Bit, mpv + 1, t, Nodes, nps * 1000);

#ifdef RobboBases
		if (UseRobboBases)
			{
			if (TBHitInfo && TBHits)
				Send (" tbhits " Type64Bit, TBHits);
			}
#endif

        Send(" score %s%s depth %d pv %s\n", cp_mate(MPV[mpv].Value, String2[Position->cpu]),
			Modifier(MPV[mpv].alpha, MPV[mpv].Value, MPV[mpv].beta, String3[Position->cpu]), MPV[mpv].depth >> 1, pv);

#ifdef Log
		if (WriteLog)
			{
			log_file = fopen(log_filename, "a");
			fprintf(log_file, "info multipv %d time " Type64Bit " nodes " Type64Bit
				" nps " Type64Bit, mpv + 1, t, Nodes, nps * 1000);

#ifdef RobboBases
			if (UseRobboBases)
				{
				if (TBHitInfo && TBHits)
					fprintf(log_file, " tbhits " Type64Bit, TBHits);
				}
#endif

			fprintf(log_file, " score %s%s depth %d pv %s\n", cp_mate(MPV[mpv].Value, String2[Position->cpu]),
				Modifier(MPV[mpv].alpha, MPV[mpv].Value, MPV[mpv].beta, String3[Position->cpu]), MPV[mpv].depth >> 1, pv);
			close_log();
			}
#endif
        }
    }
void Search(typePos *Position)
    {
    int z, i;
    typeDynamic *p, *q, *S;
    typePos *Pos;
    SearchIsDone = false;
    SuppressInput = false;
    isNewGame = false;
    StartClock = GetClock();
    CPUtime = ProcessClock();
    DoOutput = false;
    Ponder_Hit = false;
    NodeCheck = 0;
    RootPosition0->nodes = 0;
    RootBestMove = RootDepth = RootScore = 0;
    Position->StackHeight = 0;
    if (Position->Dyn->reversible > 110)
        goto SkipRepCheck;
    Position->StackHeight = -1;
    S = MAX(Position->DynRoot + 1, Position->Dyn - Position->Dyn->reversible);
    for (p = S; p <= Position->Dyn; p++)
        Position->Stack[++(Position->StackHeight)] = p->Hash;
    if (Analysing)
        {
        bool Repetition;
        for (p = S; p < Position->Dyn; p++)
            {
            Repetition = false;
            for (q = p + 2; q < Position->Dyn; q += 2)
                if (p->Hash == q->Hash)
                    {
                    Repetition = true;
                    break;
                    }
            if (!Repetition)
                Position->Stack[p - Position->Dyn + Position->Dyn->reversible] = 0;
            (p + 1)->move = 0;
            }
        }
    if (Position->StackHeight == -1)
        Position->StackHeight = 0;
    SkipRepCheck:
    memcpy(Position->DynRoot + 1, Position->Dyn, sizeof(typeDynamic));
    memset(Position->DynRoot + 2, 0, 254 * sizeof(typeDynamic));
    memset(Position->DynRoot, 0, sizeof(typeDynamic));
    Position->Dyn = Position->DynRoot + 1;
    Position->height = 0;

    if (Analysing)
        GlobalAge = 0;
    else
        IncrementAge();
		
    for (i = 0; i < 256; i++)
        (Position->DynRoot + i)->age = GlobalAge;
		
#ifdef RobboBases
	if (UseRobboBases)
		{
		SearchRobboBases = true;
		SuppressInput = true;
		if (RobboMake(Position))
			goto INFINITY;
		SuppressInput = false;
		}
#endif

    RootPrevious = -ValueMate;
    EasyMove = false;
    JumpIsSet = true;
    Pos = Position;

#ifdef RobboBases
	if (UseRobboBases)
		RootPosition0->tbhits = 0;
#endif

    StubIvan();
    Pos = &RootPosition[0][0];
    z = setjmp(J);
    if (!z)
        {
        if (Pos->wtm)
            TopWhite(Pos);
        else
            TopBlack(Pos);
        }
    SuppressInput = true;
	Lock(SMP);
	SMPisActive = false;
	UnLock(SMP);
    JumpIsSet = false;
    PreviousDepth = RootDepth;
    if (Pos == Position)
        {
        while (Pos->Dyn != (Pos->DynRoot + 1))
            {
            if (!Pos->Dyn->move)
                UndoNull(Pos);
            else
                Undo(Pos, Pos->Dyn->move);
            }
        }
    Information(Position, GetClock() - StartClock, -32767, RootScore, 32767);

#ifdef RobboBases
    INFINITY:
#endif

    SuppressInput = false;
    if (DoInfinite && !Stop)
        {
        InfiniteLoop = true;
        while (!Stop)
            Input(Position);
        }
    if (DoPonder && !Stop && !Ponder_Hit)
        {
        InfiniteLoop = true;
        while (!Stop && !Ponder_Hit)
            Input(Position);
        }
    InfiniteLoop = false;
    SuppressInput = true;
    OutputBestMove(Position);
    SuppressInput = false;
    SearchIsDone = true;
    if (isNewGame)
        NewGame(Position, true);
    while (StallMode)
        NanoSleep(1000000);
    }