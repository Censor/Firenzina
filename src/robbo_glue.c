<<<<<<< HEAD:src/robbo_glue.c
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
>>>>>>> Linux/Housekeeping/Bug fixes/Extend xTreme/Defs:Firenzina/robbo_glue.c
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

#include "fire.h"
#ifdef RobboBases
#include "robbo_totalbase.h"
#include "robbo_triplebase.h"

#define IsCheck     \
	(Position->wtm ?  \
	(wBitboardK & Position->Dyn->bAtt) : (bBitboardK & Position->Dyn->wAtt))
#define IsIllegal  \
	(!Position->wtm ? \
	(wBitboardK & Position->Dyn->bAtt) : (bBitboardK & Position->Dyn->wAtt))

extern uint32 RootBestMove;

void TotalBaseInit()
    {
    int pi1, pi2, pi3, pi4;
    int i;
    InitTotalBaseUtility(false);
    NumTotalBases = 0;
    for (pi1 = 0; pi1 < 16; pi1++)
        for (pi2 = 0; pi2 < 16; pi2++)
            for (pi3 = 0; pi3 < 16; pi3++)
                for (pi4 = 0; pi4 < 16; pi4++)
                    TotalBaseTable[pi1][pi2][pi3][pi4] = -1;
    TableTotalBases = malloc(MaxTables * sizeof(RobboTotalBase));
    for (i = 0; i < MaxTables; i++)
        (TableTotalBases + i)->Fdata = NULL;
    for (i = 0; i < MaxTables; i++)
        (TableTotalBases + i)->indexes = NULL;
    InitTotalBaseCache(TotalBaseCache);
    TotIndexes = 0;
    }
void TotalInit()
    {
    TotalBaseInit();
    ReadyTripleBase();
    NumTripleBases = 0;
    }
int RegisterRobboTotalBases()
    {
    char DIR[1024];
	Send("info string Registering TotalBases, please wait...\n");

#ifdef Log
	if (WriteLog)
		{
		log_file = fopen(log_filename, "a");
		fprintf(log_file, "info string Registering TotalBases, please wait...\n");
		close_log();
		}
#endif

    DeRegisterRobboTotalBases();
    strcpy(TotalBaseDir, TotalDir);
    sprintf(DIR, "%s", TotalBaseDir);
    GetTotalBase(DIR);
    sprintf(DIR, "%s/3", TotalBaseDir);
    GetTotalBase(DIR);
    sprintf(DIR, "%s/4", TotalBaseDir);
    GetTotalBase(DIR);
    sprintf(DIR, "%s/5", TotalBaseDir);
    GetTotalBase(DIR);
    sprintf(DIR, "%s/6", TotalBaseDir);
    GetTotalBase(DIR);
    sprintf(DIR, "%s/51", TotalBaseDir);
    GetTotalBase(DIR);
    sprintf(DIR, "%s/42", TotalBaseDir);
    GetTotalBase(DIR);
    sprintf(DIR, "%s/33", TotalBaseDir);
    GetTotalBase(DIR);
    sprintf(DIR, "%s/Z", TotalBaseDir);
    GetTotalBase(DIR);
    sprintf(DIR, "%s/345Z", TotalBaseDir);
    GetTotalBase(DIR);
    sprintf(DIR, "%s/6Z", TotalBaseDir);
    GetTotalBase(DIR);
    Send("info string " "%d TotalBases registered\n", NumTotalBases);

#ifdef Log
	if (WriteLog)
		{
		log_file = fopen(log_filename, "a");
		fprintf(log_file, "info string " "%d TotalBases registered\n", NumTotalBases);
		close_log();
		}
#endif
	TotalBasesLoaded = true;
    return true;
    }
int DeRegisterRobboTotalBases()
    {
    int pi1, pi2, pi3, pi4, i;
    RobboTotalBase *tb;
    for (i = 0; i < NumTotalBases; i++)
        {
        tb = (TableTotalBases + i);
        if (tb->indexes)
            free(tb->indexes);
        if (tb->Fdata)
            fclose(tb->Fdata);
        tb->Fdata = NULL;
        }
    NumTotalBases = 0;
    TotIndexes = 0;
    for (pi1 = 0; pi1 < 16; pi1++)
        for (pi2 = 0; pi2 < 16; pi2++)
            for (pi3 = 0; pi3 < 16; pi3++)
                for (pi4 = 0; pi4 < 16; pi4++)
                    TotalBaseTable[pi1][pi2][pi3][pi4] = -1;
    SetTotalBaseCache(TotalBaseCache);
	TotalBasesLoaded = false;
    return true;
    }
int UnLoadRobboTripleBases()
    {
    CleanupTriple();
    TripleBasesLoaded = false;
    return true;
    }
int LoadRobboTripleBases()
    {
    char DIR[1024];
	Send("info string Loading TripleBases, please wait...\n");

#ifdef Log
	if (WriteLog)
		{
		log_file = fopen(log_filename, "a");
		fprintf(log_file, "info string Loading TripleBases, please wait...\n");
		close_log();
		}
#endif

    CleanupTriple();
    strcpy(TripleBaseDir, TripleDir);
    sprintf(DIR, "%s", TripleBaseDir);
    GetTripleBase(DIR);
    sprintf(DIR, "%s/2", TripleBaseDir);
    GetTripleBase(DIR);
    sprintf(DIR, "%s/3", TripleBaseDir);
    GetTripleBase(DIR);
    sprintf(DIR, "%s/4", TripleBaseDir);
    GetTripleBase(DIR);
    sprintf(DIR, "%s/5", TripleBaseDir);
    GetTripleBase(DIR);
    sprintf(DIR, "%s/6", TripleBaseDir);
    GetTripleBase(DIR);
    sprintf(DIR, "%s/51", TripleBaseDir);
    GetTripleBase(DIR);
    sprintf(DIR, "%s/42", TripleBaseDir);
    GetTripleBase(DIR);
    sprintf(DIR, "%s/33", TripleBaseDir);
    GetTripleBase(DIR);
    sprintf(DIR, "%s/345Z", TripleBaseDir);
    GetTripleBase(DIR);
    sprintf(DIR, "%s/Z", TripleBaseDir);
    GetTripleBase(DIR);
    sprintf(DIR, "%s/6Z", TripleBaseDir);
    GetTripleBase(DIR);
    TripleStatistics();
    TripleBasesLoaded = true;
    return true;
    }
bool RobboMove(typePos *Position, uint32 *am, int *v, int *cap)
    {
    int va, v2, av = 0xff, zv = 0x00, cp;
    typeMoveList move_list[256];
    typeMoveList *q;
    uint32 m;
    if (Position->Dyn->oo)
        return false;
    if (!RobboTotalBaseScore(Position, &va) || va == dLoss)
        return false;
    Mobility(Position);
    if (IsCheck)
        EvasionMoves(Position, move_list, 0xffffffffffffffff);
    else
        {
        q = CaptureMoves(Position, move_list, 0xffffffffffffffff);
        OrdinaryMoves(Position, q);
        }
    for (q = move_list; q->move; q++)
        {
        m = q->move & 0x7fff;
        Position->StackHeight = 0;
        Make(Position, m);
        Position->StackHeight = 1;
        Mobility(Position);
        if (IsIllegal)
            {
            Undo(Position, m);
            Position->StackHeight = 1;
            continue;
            }
        if (!RobboTotalBaseScore(Position, &v2))
            {
            Undo(Position, m);
            Position->StackHeight = 1;
            return false;
            }
        if (DiskLoss(va))
            {
            uint32 m3;
            int v3;
            if (!RobboMove(Position, &m3, &v3, &cp))
                {
                Undo(Position, m);
                Position->StackHeight = 1;
                return false;
                }
            if (v3 > zv
               && ((!Position->Dyn->cp && !(m & FlagMask) && ((!cp && (!(m3 & FlagMask))))) || va <= dLoss + 1))
                {
                *am = m;
                *v = zv = v3;
                *cap = Position->Dyn->cp;
                }
            }
        Undo(Position, m);
        Position->StackHeight = 1;
        if (DiskDraw(va) && DiskDraw(v2))
            {
            *am = m;
            *v = dDraw;
            *cap = false;
            return true;
            }
        if (DiskWin(va) && DiskLoss(v2))
            {
            if (Position->sq[To(m)] || (m & FlagMask))
                {
                *am = m;
                *v = v2;
                *cap = true;
                return true;
                }
            if (v2 < av)
                {
                *am = m;
                *v = av = v2;
                *cap = false;
                }
            }
        }
    return true;
    }
bool RobboMake(typePos *Position)
    {
    int va, v2, cp;
    uint32 m;
    Position->Dyn->Hash ^= 0xfa73e65b089c41d2;
    SearchRobboBases = true;
    Eval(Position, -0x7fff0000, 0x7fff0000, 0, 0xff);
    Position->Dyn->Hash ^= 0xfa73e65b089c41d2;
    if (Position->Dyn->exact)
        SearchRobboBases = false;
    else
        SearchRobboBases = true;
    if (!TotalCondition(Position) || !RobboTotalBaseScore(Position, &va))
        return false;
    if (!RobboMove(Position, &m, &v2, &cp))
        return false;
    if (DiskDraw(va))
        {
        Send("info multipv 1 depth 0 score cp 0 pv %s\n", Notate(m, String1[Position->cpu]));

#ifdef Log
		if (WriteLog)
			{
			log_file = fopen(log_filename, "a");
			fprintf(log_file, "info multipv 1 depth 0 score cp 0 pv %s\n", Notate(m, String1[Position->cpu]));
			close_log();
			}
#endif

        RootBestMove = m;
        return true;
        }
    if (DiskLoss(va))
        {
        Send("info multipv 1 depth 0 score cp %d pv %s\n", -28000 + 1000 * POPCNT(Position->OccupiedBW) + v2,
			Notate(m, String1[Position->cpu]));

#ifdef Log
		if (WriteLog)
			{
			log_file = fopen(log_filename, "a");
			fprintf(log_file, "info multipv 1 depth 0 score cp %d pv %s\n", -28000 + 1000 * POPCNT(Position->OccupiedBW) + v2,
				Notate(m, String1[Position->cpu]));
			close_log();
			}
#endif

        RootBestMove = m;
        return true;
        }
    if (DiskWin(va))
        {
        Send("info multipv 1 depth 0 score cp %d pv %s\n", 28000 - 1000 * POPCNT(Position->OccupiedBW) - v2,
			Notate(m, String1[Position->cpu]));

#ifdef Log
		if (WriteLog)
			{
			log_file = fopen(log_filename, "a");
			fprintf(log_file, "info multipv 1 depth 0 score cp %d pv %s\n", 28000 - 1000 * POPCNT(Position->OccupiedBW) - v2,
				Notate(m, String1[Position->cpu]));
			close_log();
			}
#endif

        RootBestMove = m;
        return true;
        }
    return false;
    }
static void MainLineIterate(typePos *Position)
    {
    int va, v2, cp;
    uint32 m;
    (Position->Dyn + 1)->move = MoveNone;
    Mobility(Position);
    if (!RobboTotalBaseScore(Position, &va))
        {
        if (VerboseRobboBases)
			{
			Send("info string Failure in MainLine\n");

#ifdef Log
			if (WriteLog)
				{
				log_file = fopen(log_filename, "a");
				fprintf(log_file, "info string Failure in MainLine\n");
				close_log();
				}
#endif
			}

        (Position->Dyn + 1)->move = MoveNone;
        Position->StackHeight = 0;
        return;
        }
    if (!RobboMove(Position, &m, &v2, &cp))
        {
        (Position->Dyn + 1)->move = MoveNone;
        Position->StackHeight = 0;
        return;
        }
    if (va == dDraw)
        {
        Position->StackHeight = 0;
        Make(Position, m);
        Position->StackHeight = 1;
        Undo(Position, m);
        return;
        }
    m &= 0x7fff;
    Position->StackHeight = 0;
    Make(Position, m);
    Position->StackHeight = 1;
    MainLineIterate(Position);
    Undo(Position, m);
    Position->StackHeight = 1;
    }
void MainLine(typePos *Position)
    {
    typeDynamic *p;
    char String[16];
    int va;
    MainLineIterate(Position);
    if (!RobboTotalBaseScore(Position, &va) || va == dBreak)
        {
        if (VerboseRobboBases)
			{
			Send("info string Failure in MainLine\n");

#ifdef Log
			if (WriteLog)
				{
				log_file = fopen(log_filename, "a");
				fprintf(log_file, "info string Failure in MainLine\n");
				close_log();
				}
#endif

			}
        return;
        }
    sprintf(String, "%d", va - dLoss);
    Send("mainline (%c%s): ", (va == dWin) ? 'W' :((va == dDraw) ? 'D' : 'L'), (va < dLoss) ? "" : String);
    for (p = Position->Dyn + 1; p->move; p++)
        Send("%s ", Notate(p->move & 0x7fff, String));
    Send("\n");

#ifdef Log
	if (WriteLog)
		{
		log_file = fopen(log_filename, "a");
		fprintf(log_file, "mainline (%c%s): ", (va == dWin) ? 'W' :((va == dDraw) ? 'D' : 'L'), (va < dLoss) ? "" : String);
		for (p = Position->Dyn + 1; p->move; p++)
			fprintf(log_file, "%s ", Notate(p->move & 0x7fff, String));
		fprintf(log_file, "\n");
		close_log();
		}
#endif

    }
#endif
