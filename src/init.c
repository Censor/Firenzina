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

#include <time.h>
#include "fire.h"

FILE *fp;
FILE *cfgFile;
char filename[256];
int RandRange = 0;

static int get_rand_num(int min, int max)
	{
	int range;
	int r;

	srand(rand()*GetTickCount());
	range = max - min + 1;
	r = (rand() % (range)) + min;
	return r;
	}
static void parse_option(const char *str)
	{
#ifdef RobboBases
	char *s;
#endif

    char arg[6][256];
    arg[0][0] = arg[1][0] = arg[2][0] = '\0';
    sscanf(str, "%s %s %s", arg[0], arg[1], arg[2]);

    if (!strcmp(arg[0], "[]")) { }
	
	// System
    if (!strcmp(arg[0], "Verbose_UCI"))
        {
        int input = atoi(arg[1]);
		Send("\n// System\n");
        if (input == 1)
            {
            VerboseUCI = true;
            Send("Verbose UCI: %s\n", "true");
			Send("info string Verbose UCI: %s\n", "true");

#ifdef Log
			if (WriteLog)
				{
				log_file = fopen(log_filename, "a");
				fprintf(log_file, "Verbose UCI: %s\n", "true");
				close_log();
				}
#endif
            }
        else
			{
            VerboseUCI = false;
            Send("Verbose UCI: %s\n", "false");
			if (VerboseUCI)
				Send("info string Verbose UCI: %s\n", "false");

#ifdef Log
			if (WriteLog)
				{
				log_file = fopen(log_filename, "a");
				fprintf(log_file, "Verbose UCI: %s\n", "false");
				close_log();
				}
#endif
            }
        }

#ifdef Log
    if (!strcmp(arg[0], "Write_Log"))
        {
        int input = atoi(arg[1]);
        if (input == 1)
            {
            WriteLog = true;
			Send("Write Log: %s\n", "true");
			if (VerboseUCI)
				Send("info string Write Log: %s\n", "true");
			create_log();
			log_file = fopen(log_filename, "a");
			fprintf(log_file, "%s found:\n", "fire.cfg");
			fprintf(log_file, "Write Log: %s\n", "true");
			close_log();
            }
        else
            {
            WriteLog = false;
			Send("Write Log: %s\n", "false");
			if (VerboseUCI)
				Send("info string Write Log: %s\n", "false");
            }
        }
#endif

    if (!strcmp(arg[0], "Hash"))
        {
        int input = atoi(arg[1]);
		OptHashSize = input;
		if (OptHashSize == 0)
			OptHashSize = DEFAULT_HASH_SIZE;
		Send("Hash: %d Mb\n", OptHashSize);
		if (VerboseUCI)
			Send("info string Hash: %d Mb\n", OptHashSize);

#ifdef Log
		if (WriteLog)
			{
			log_file = fopen(log_filename, "a");
			fprintf(log_file, "Hash: %d Mb\n", OptHashSize);
			close_log();
			}
#endif
        }
    if (!strcmp(arg[0], "Pawn_Hash"))
        {
        int input = atoi(arg[1]);
		OptPHashSize = input;
		if (OptPHashSize == 0)
			OptPHashSize = DEFAULT_PAWN_HASH_SIZE;
		Send("Pawn Hash: %d Mb\n", OptPHashSize);
		if (VerboseUCI)
			Send("info string Pawn Hash: %d Mb\n", OptPHashSize);

#ifdef Log
		if (WriteLog)
			{
			log_file = fopen(log_filename, "a");
			fprintf(log_file, "Pawn Hash: %d Mb\n", OptPHashSize);
			close_log();
			}
#endif
        }
    if (!strcmp(arg[0], "Max_Threads"))
        {
        int input = atoi(arg[1]);
		OptMaxThreads = input;
		if (OptMaxThreads <= 0) // Modified 5/22/2013 by Yuri Censor for Firenzina
			OptMaxThreads = MaxCPUs;
		else if (OptMaxThreads > MaxCPUs)
			OptMaxThreads = MaxCPUs;
		Send("Max Threads: %d\n", OptMaxThreads);
		if (VerboseUCI)
			Send("info string Max Threads: %d\n", OptMaxThreads);

#ifdef Log
		if (WriteLog)
			{
			log_file = fopen(log_filename, "a");
			fprintf(log_file, "Max Threads: %d\n", OptMaxThreads);
			close_log();
			}
#endif
        }
	if (!strcmp(arg[0], "Min_Threads")) // Added 5/22/2013 by Yuri Censor for Firenzina
        {
        int input = atoi(arg[1]);
		OptMinThreads = input;
		if (OptMinThreads <= 0)
			OptMinThreads = 1;
		else if (OptMinThreads > MaxCPUs)
			OptMinThreads = MaxCPUs;
		Send("Min Threads: %d\n", OptMinThreads);
		if (VerboseUCI)
			Send("info string Min Threads: %d\n", OptMinThreads);

#ifdef Log
		if (WriteLog)
			{
			log_file = fopen(log_filename, "a");
			fprintf(log_file, "Min Threads: %d\n", OptMinThreads);
			close_log();
			}
#endif
        }
    if (!strcmp(arg[0], "MultiPV"))
        {
        int input = atoi(arg[1]);
		MultiPV = input;
		Send("MultiPV: %d\n", MultiPV);
		if (VerboseUCI)
			Send("info string MultiPV: %d\n", MultiPV);

#ifdef Log
		if (WriteLog)
			{
			log_file = fopen(log_filename, "a");
			fprintf(log_file, "MultiPV: %d\n", MultiPV);
			close_log();
			}
#endif
        }

	// RobboBases
#ifdef RobboBases
    if (!strcmp(arg[0], "Use_RobboBases"))
        {
        int input = atoi(arg[1]);
		Send("\n// RobboBases\n");
        if (input == 1)
            {
            UseRobboBases = true;
            Send("Use RobboBases: %s\n", "true");
			if (VerboseUCI)
				Send("info string Use RobboBases: %s\n", "true");

#ifdef Log
			if (WriteLog)
				{
				log_file = fopen(log_filename, "a");
				fprintf(log_file, "Use RobboBases: %s\n", "true");
				close_log();
				}
#endif
            }
        else
			{
            UseRobboBases = false;
            Send("Use RobboBases: %s\n", "false");
			if (VerboseUCI)
				Send("info string Use RobboBases: %s\n", "false");

#ifdef Log
			if (WriteLog)
				{
				log_file = fopen(log_filename, "a");
				fprintf(log_file, "Use RobboBases: %s\n", "false");
				close_log();
				}
#endif
            }
        }
    if (!strcmp(arg[0], "Verbose_RobboBases"))
        {
        int input = atoi(arg[1]);
        if (input == 1)
            {
            VerboseRobboBases = true;
			Send("Verbose RobboBases: %s\n", "true");
			if (VerboseUCI)
				Send("info string Verbose RobboBases: %s\n", "true");

#ifdef Log
			if (WriteLog)
				{
				log_file = fopen(log_filename, "a");
				fprintf(log_file, "Verbose RobboBases: %s\n", "true");
				close_log();
				}
#endif
            }
        else
            {
            VerboseRobboBases = false;
			Send("Verbose RobboBases: %s\n", "false");
			if (VerboseUCI)
				Send("info string Verbose RobboBases: %s\n", "false");

#ifdef Log
			if (WriteLog)
				{
				log_file = fopen(log_filename, "a");
				fprintf(log_file, "Verbose RobboBases: %s\n", "false");
				close_log();
				}
#endif
            }
        }

	// TotalBases
    if (!strcmp(arg[0], "AutoLoad_TotalBases"))
        {
        int input = atoi(arg[1]);
		Send("\n// TotalBases\n");
        if (input == 1)
            {
            AutoloadTotalBases = true;
            Send("AutoLoad TotalBases: %s\n", "true");
			if (VerboseUCI)
				Send("info string AutoLoad TotalBases: %s\n", "true");

#ifdef Log
			if (WriteLog)
				{
				log_file = fopen(log_filename, "a");
				fprintf(log_file, "AutoLoad TotalBases: %s\n", "true");
				close_log();
				}
#endif
            }
        else
            {
            AutoloadTotalBases = false;
            Send("AutoLoad TotalBases: %s\n", "false");
			if (VerboseUCI)
				Send("info string AutoLoad TotalBases: %s\n", "false");

#ifdef Log
			if (WriteLog)
				{
				log_file = fopen(log_filename, "a");
				fprintf(log_file, "AutoLoad TotalBases: %s\n", "false");
				close_log();
				}
#endif
            }
        }
    if (!strcmp(arg[0], "TotalBase_Path"))
		{
        s = arg[1];
		strcpy(TotalDir, s);
		Send("TotalBase Path: %s\n", TotalDir);
		if (VerboseUCI)
			Send("info string TotalBase Path: %s\n", TotalDir);

#ifdef Log
		if (WriteLog)
			{
			log_file = fopen(log_filename, "a");
			fprintf(log_file, "TotalBase Path: %s\n", TotalDir);
			close_log();
			}
#endif
		}
    if (!strcmp(arg[0], "TotalBase_Cache"))
        {
        int input = atoi(arg[1]);
		TotalBaseCache = input;
		if (TotalBaseCache < 1)
			TotalBaseCache = 1;
		if (TotalBaseCache > MAX_TOTAL_BASE_CACHE)
			TotalBaseCache = MAX_TOTAL_BASE_CACHE;
		InitTotalBaseCache(TotalBaseCache);
		Send("TotalBase Cache: %d Mb\n", TotalBaseCache);
		if (VerboseUCI)
			Send("info string TotalBase Cache: %d Mb\n", TotalBaseCache);

#ifdef Log
		if (WriteLog)
			{
			log_file = fopen(log_filename, "a");
			fprintf(log_file, "TotalBase Cache: %d Mb\n", TotalBaseCache);
			close_log();
			}
#endif
        }
				
	// TripleBases
    if (!strcmp(arg[0], "AutoLoad_TripleBases"))
        {
        int input = atoi(arg[1]);
		Send("\n// TripleBases\n");
        if (input == 1)
            {
            AutoloadTripleBases = true;
            Send("AutoLoad TripleBases: %s\n", "true");
			if (VerboseUCI)
				Send("info string AutoLoad TripleBases: %s\n", "true");

#ifdef Log
			if (WriteLog)
				{
				log_file = fopen(log_filename, "a");
				fprintf(log_file, "AutoLoad TripleBases: %s\n", "true");
				close_log();
				}
#endif
            }
        else
            {
            AutoloadTripleBases = false;
            Send("AutoLoad TripleBases: %s\n", "false");
			if (VerboseUCI)
				Send("info string AutoLoad TripleBases: %s\n", "false");

#ifdef Log
			if (WriteLog)
				{
				log_file = fopen(log_filename, "a");
				fprintf(log_file, "AutoLoad TripleBases: %s\n", "false");
				close_log();
				}
#endif
            }
        }
    if (!strcmp(arg[0], "TripleBase_Path"))
        {
        s = arg[1];
		strcpy(TripleDir, s);
		Send("TripleBase Path: %s\n", TripleDir);
		if (VerboseUCI)
			Send("info string TripleBase Path: %s\n", TripleDir);

#ifdef Log
		if (WriteLog)
			{
			log_file = fopen(log_filename, "a");
			fprintf(log_file, "TripleBase Path: %s\n", TripleDir);
			close_log();
			}
#endif
		}
    if (!strcmp(arg[0], "TripleBase_Hash"))
        {
        TripleBaseHash = atoi(arg[1]);
		if (TripleBaseHash < 1)
			TripleBaseHash = 1;
		if (TripleBaseHash > MAX_TRIPLE_BASE_HASH)
			TripleBaseHash = MAX_TRIPLE_BASE_HASH;
        Send("TripleBase Hash: %d Mb\n", TripleBaseHash);
		if (VerboseUCI)
			Send("info string TripleBase Hash: %d Mb\n", TripleBaseHash);
		InitTripleHash(TripleBaseHash);

#ifdef Log
		if (WriteLog)
			{
			log_file = fopen(log_filename, "a");
			fprintf(log_file, "TripleBase Hash: %d Mb\n", TripleBaseHash);
			close_log();
			}
#endif
		}
    if (!strcmp(arg[0], "Dynamic_TripleBase_Cache"))
        {
        DynamicTripleBaseCache = atoi(arg[1]);
		if (DynamicTripleBaseCache < 1)
			DynamicTripleBaseCache = 1;
		if (DynamicTripleBaseCache > MAX_DYNAMIC_TRIPLE_BASE_CACHE)
			DynamicTripleBaseCache = MAX_DYNAMIC_TRIPLE_BASE_CACHE;
        Send("Dynamic TripleBase Cache: %d Mb\n", DynamicTripleBaseCache);
		if (VerboseUCI)
			Send("info string Dynamic TripleBase Cache: %d Mb\n", DynamicTripleBaseCache);
		SetTripleBaseCache(DynamicTripleBaseCache);

#ifdef Log
		if (WriteLog)
			{
			log_file = fopen(log_filename, "a");
			fprintf(log_file, "Dynamic TripleBase Cache: %d Mb\n", DynamicTripleBaseCache);
			close_log();
			}
#endif
		}
#endif

	// Lazy Eval
    if (!strcmp(arg[0], "LazyEvalMin"))
        {
		Send("\n[] Lazy Eval\n");
		if (atoi(arg[2]) == 0)
			LazyEvalMin = atoi(arg[1]);
		else
			{
			RandRange = atoi(arg[2]);
			LazyEvalMin = get_rand_num(LazyEvalMin * (100 - RandRange) / 100, LazyEvalMin * (100  + RandRange) / 100);
			}
        Send("Lazy Eval Min: %d\n", LazyEvalMin);
        }
    if (!strcmp(arg[0], "LazyEvalMax"))
        {
		if (atoi(arg[2]) == 0)
			LazyEvalMax = atoi(arg[1]);
		else
			{
			RandRange = atoi(arg[2]);
			LazyEvalMax = get_rand_num(LazyEvalMax * (100 - RandRange) / 100, LazyEvalMax * (100  + RandRange) / 100);
			}
        Send("Lazy Eval Max: %d\n", LazyEvalMax);
        }


	// Piece Values
    if (!strcmp(arg[0], "PawnValue"))
        {
		Send("\n[] Piece Values\n");		
		if (atoi(arg[2]) == 0)
			PValue = atoi(arg[1]);
		else
			{
			RandRange = atoi(arg[2]);
			PValue = get_rand_num(PValue * (100 - RandRange) / 100, PValue * (100  + RandRange) / 100);
			}
        Send("PawnValue: %d\n", PValue);
        }
    if (!strcmp(arg[0], "KnightValue"))
        {
		if (atoi(arg[2]) == 0)
			NValue = atoi(arg[1]);
		else
			{
			RandRange = atoi(arg[2]);
			NValue = get_rand_num(NValue * (100 - RandRange) / 100, NValue * (100  + RandRange) / 100);
			}
        Send("KnightValue: %d\n", NValue);
        }
    if (!strcmp(arg[0], "BishopValue"))
        {
		if (atoi(arg[2]) == 0)
			BValue = atoi(arg[1]);
		else
			{
			RandRange = atoi(arg[2]);
			BValue = get_rand_num(BValue * (100 - RandRange) / 100, BValue * (100  + RandRange) / 100);
			}
        Send("BishopValue: %d\n", BValue);
        }
    if (!strcmp(arg[0], "RookValue"))
        {
		if (atoi(arg[2]) == 0)
			RValue = atoi(arg[1]);
		else
			{
			RandRange = atoi(arg[2]);
			RValue = get_rand_num(RValue * (100 - RandRange) / 100, RValue * (100  + RandRange) / 100);
			}
        Send("RookValue: %d\n", RValue);
        }
    if (!strcmp(arg[0], "QueenValue"))
        {
		if (atoi(arg[2]) == 0)
			QValue = atoi(arg[1]);
		else
			{
			RandRange = atoi(arg[2]);
			QValue = get_rand_num(QValue * (100 - RandRange) / 100, QValue * (100  + RandRange) / 100);
			}
        Send("QueenValue: %d\n", QValue);
        }
    if (!strcmp(arg[0], "BishopPairValue"))
        {
		if (atoi(arg[2]) == 0)
			BPValue = atoi(arg[1]);
		else
			{
			RandRange = atoi(arg[2]);
			BPValue = get_rand_num(BPValue * (100 - RandRange) / 100, BPValue * (100  + RandRange) / 100);
			}
        Send("BishopPairValue: %d\n", BPValue);
        }

	// Prune Thresholds
    if (!strcmp(arg[0], "PruneCheck"))
        {
		Send("\n[] Prune Thresholds\n");
		if (atoi(arg[2]) == 0)
			PruneCheck = atoi(arg[1]);
		else
			{
			RandRange = atoi(arg[2]);
			PruneCheck = get_rand_num(PruneCheck * (100 - RandRange) / 100, PruneCheck * (100  + RandRange) / 100);
			}
        Send("PruneCheck: %d\n", PruneCheck);
        }
    if (!strcmp(arg[0], "PrunePawn"))
        {
		if (atoi(arg[2]) == 0)
			PrunePawn = atoi(arg[1]);
		else
			{
			RandRange = atoi(arg[2]);
			PrunePawn = get_rand_num(PrunePawn * (100 - RandRange) / 100, PrunePawn * (100  + RandRange) / 100);
			}
        Send("PrunePawn: %d\n", PrunePawn);
        }
    if (!strcmp(arg[0], "PruneMinor"))
        {
		if (atoi(arg[2]) == 0)
			PruneMinor = atoi(arg[1]);
		else
			{
			RandRange = atoi(arg[2]);
			PruneMinor = get_rand_num(PruneMinor * (100 - RandRange) / 100, PruneMinor * (100  + RandRange) / 100);
			}
        Send("PruneMinor: %d\n", PruneMinor);
        }
    if (!strcmp(arg[0], "PruneRook"))
        {
		if (atoi(arg[2]) == 0)
			PruneRook = atoi(arg[1]);
		else
			{
			RandRange = atoi(arg[2]);
			PruneRook = get_rand_num(PruneRook * (100 - RandRange) / 100, PruneRook * (100  + RandRange) / 100);
			}
        Send("PruneRook: %d\n", PruneRook);
        }

	// Search Parameters
    if (!strcmp(arg[0], "AspirationWindow"))
        {
		Send("\n[] Search Parameters\n");
		if (atoi(arg[2]) == 0)
			AspirationWindow = atoi(arg[1]);
		else
			{
			RandRange = atoi(arg[2]);
			AspirationWindow = get_rand_num(AspirationWindow * (100 - RandRange) / 100, AspirationWindow * (100  + RandRange) / 100);
			}
        Send("AspirationWindow: %d\n", AspirationWindow);
        }
    if (!strcmp(arg[0], "CountLimit"))
        {
		if (atoi(arg[2]) == 0)
			CountLimit = atoi(arg[1]);
		else
			{
			RandRange = atoi(arg[2]);
			CountLimit = get_rand_num(CountLimit * (100 - RandRange) / 100, CountLimit * (100  + RandRange) / 100);
			}
        Send("CountLimit: %d\n", CountLimit);
        }
   if (!strcmp(arg[0], "DeltaCutoff"))
        {
		if (atoi(arg[2]) == 0)
			DeltaCutoff = atoi(arg[1]);
		else
			{
			RandRange = atoi(arg[2]);
			DeltaCutoff = get_rand_num(DeltaCutoff * (100 - RandRange) / 100, DeltaCutoff * (100  + RandRange) / 100);
			}
        Send("DeltaCutoff: %d\n", DeltaCutoff);
        }
	if (!strcmp(arg[0], "ExtendInCheck"))
		{
		if (atoi(arg[2]) == 0)
			ExtendInCheck = atoi(arg[1]);
		else
			{
			RandRange = atoi(arg[2]);
			ExtendInCheck = get_rand_num(ExtendInCheck * (100 - RandRange) / 100, ExtendInCheck * (100  + RandRange) / 100);
			}
        if (ExtendInCheck == 1)
            {
            ExtendInCheck = true;
			Send("ExtendInCheck: %s\n", "true");
            }
        else
            {
            ExtendInCheck = false;
			Send("ExtendInCheck: %s\n", "false");
            }
        }
   if (!strcmp(arg[0], "HistoryThreshold"))
        {
		if (atoi(arg[2]) == 0)
			HistoryThreshold = atoi(arg[1]);
		else
			{
			RandRange = atoi(arg[2]);
			HistoryThreshold = get_rand_num(HistoryThreshold * (100 - RandRange) / 100, HistoryThreshold * (100  + RandRange) / 100);
			}
        Send("History Threshold: %d\n", HistoryThreshold);
        }
   if (!strcmp(arg[0], "LowDepthMargin"))
        {
		if (atoi(arg[2]) == 0)
			LowDepthMargin = atoi(arg[1]);
		else
			{
			RandRange = atoi(arg[2]);
			LowDepthMargin = get_rand_num(LowDepthMargin * (100 - RandRange) / 100, LowDepthMargin * (100  + RandRange) / 100);
			}
        Send("LowDepthCutOff: %d\n", LowDepthMargin);
        }
    if (!strcmp(arg[0], "MinDepthMultiplier"))
        {
		if (atoi(arg[2]) == 0)
			MinDepthMultiplier = atoi(arg[1]);
		else
			{
			RandRange = atoi(arg[2]);
			MinDepthMultiplier = get_rand_num(MinDepthMultiplier * (100 - RandRange) / 100, MinDepthMultiplier * (100  + RandRange) / 100);
			}
        Send("MinDepthMultiplier: %d\n", MinDepthMultiplier);
        }
    if (!strcmp(arg[0], "MinTransMoveDepth"))
        {
		if (atoi(arg[2]) == 0)
			MinTransMoveDepth = atoi(arg[1]);
		else
			{
			RandRange = atoi(arg[2]);
			MinTransMoveDepth = get_rand_num(MinTransMoveDepth * (100 - RandRange) / 100, MinTransMoveDepth * (100  + RandRange) / 100);
			}
        Send("MinTransMoveDepth: %d\n", MinTransMoveDepth);
        }
    if (!strcmp(arg[0], "NullReduction"))
        {
		if (atoi(arg[2]) == 0)
			NullReduction = atoi(arg[1]);
		else
			{
			RandRange = atoi(arg[2]);
			NullReduction = get_rand_num(NullReduction * (100 - RandRange) / 100, NullReduction * (100  + RandRange) / 100);
			}
        Send("NullReduction: %d\n", NullReduction);
        }
    if (!strcmp(arg[0], "QSAlphaThreshold"))
        {
		if (atoi(arg[2]) == 0)
			QSAlphaThreshold = atoi(arg[1]);
		else
			{
			RandRange = atoi(arg[2]);
			QSAlphaThreshold = get_rand_num(QSAlphaThreshold * (100 - RandRange) / 100, QSAlphaThreshold * (100  + RandRange) / 100);
			}
        Send("QSAlphaThreshold: %d\n", QSAlphaThreshold);
        }
    if (!strcmp(arg[0], "SearchDepthMin"))
        {
		if (atoi(arg[2]) == 0)
			SearchDepthMin = atoi(arg[1]);
		else
			{
			RandRange = atoi(arg[2]);
			SearchDepthMin = get_rand_num(SearchDepthMin * (100 - RandRange) / 100, SearchDepthMin * (100  + RandRange) / 100);
			};
        Send("SearchDepthMin: %d\n", SearchDepthMin);
        }
   if (!strcmp(arg[0], "SearchDepthReduction"))
        {
		if (atoi(arg[2]) == 0)
			SearchDepthReduction = atoi(arg[1]);
		else
			{
			RandRange = atoi(arg[2]);
			SearchDepthReduction = get_rand_num(SearchDepthReduction * (100 - RandRange) / 100, SearchDepthReduction * (100  + RandRange) / 100);
			}
        Send("SearchDepthReduction: %d\n", SearchDepthReduction);
        }
    if (!strcmp(arg[0], "SEECutOff"))
        {
		if (atoi(arg[2]) == 0)
			SEECutOff = atoi(arg[1]);
		else
			{
			RandRange = atoi(arg[2]);
			SEECutOff = get_rand_num(SEECutOff * (100 - RandRange) / 100, SEECutOff * (100  + RandRange) / 100);
			};
        Send("SEECutOff: %d\n", SEECutOff);
        }
    if (!strcmp(arg[0], "SEELimit"))
        {
		if (atoi(arg[2]) == 0)
			SEELimit = atoi(arg[1]);
		else
			{
			RandRange = atoi(arg[2]);
			SEELimit = get_rand_num(SEELimit * (100 - RandRange) / 100, SEELimit * (100  + RandRange) / 100);
			};
        Send("SEELimit: %d\n", SEELimit);
        }
   if (!strcmp(arg[0], "TopMinDepth"))
        {
		if (atoi(arg[2]) == 0)
			TopMinDepth = atoi(arg[1]);
		else
			{
			RandRange = atoi(arg[2]);
			TopMinDepth = get_rand_num(TopMinDepth * (100 - RandRange) / 100, TopMinDepth * (100  + RandRange) / 100);
			}
        Send("TopMinDepth: %d\n", TopMinDepth);
        }
    if (!strcmp(arg[0], "UndoCountThreshold"))
        {
		if (atoi(arg[2]) == 0)
			UndoCountThreshold = atoi(arg[1]);
		else
			{
			RandRange = atoi(arg[2]);
			UndoCountThreshold = get_rand_num(UndoCountThreshold * (100 - RandRange) / 100, UndoCountThreshold * (100  + RandRange) / 100);
			}
        Send("UndoCountThreshold: %d\n", UndoCountThreshold);
        }
    if (!strcmp(arg[0], "ValueCut"))
        {
		if (atoi(arg[2]) == 0)
			ValueCut = atoi(arg[1]);
		else
			{
			RandRange = atoi(arg[2]);
			ValueCut = get_rand_num(ValueCut * (100 - RandRange) / 100, ValueCut * (100  + RandRange) / 100);
			}
        Send("ValueCut: %d\n", ValueCut);
        }
	if (!strcmp(arg[0], "VerifyNull"))
		{
		if (atoi(arg[2]) == 0)
			VerifyNull = atoi(arg[1]);
		else
			{
			RandRange = atoi(arg[2]);
			VerifyNull = get_rand_num(VerifyNull * (100 - RandRange) / 100, VerifyNull * (100  + RandRange) / 100);
			}
        if (VerifyNull == 1)
            {
            VerifyNull = true;
			Send("VerifyNull: %s\n", "true");
            }
        else
            {
            VerifyNull = false;
			Send("VerifyNull: %s\n", "false");
            }
        }
    if (!strcmp(arg[0], "VerifyReduction"))
        {
		if (atoi(arg[2]) == 0)
			VerifyReduction = atoi(arg[1]);
		else
			{
			RandRange = atoi(arg[2]);
			VerifyReduction = get_rand_num(VerifyReduction * (100 - RandRange) / 100, VerifyReduction * (100  + RandRange) / 100);
			}
        Send("VerifyReduction: %d\n", VerifyReduction);
        }

	// Split Depth
	if (!strcmp(arg[0], "SplitAtCN"))
		{
		if (atoi(arg[2]) == 0)
			SplitAtCN = atoi(arg[1]);
		else
			{
			RandRange = atoi(arg[2]);
			SplitAtCN = get_rand_num(SplitAtCN * (100 - RandRange) / 100, SplitAtCN * (100  + RandRange) / 100);
			}
		Send("\n[] SplitDepth\n");
        if (SplitAtCN == 1)
            {
            SplitAtCN = true;
			Send("SplitAtCN: %s\n", "true");
            }
        else
            {
            SplitAtCN = false;
			Send("SplitAtCN: %s\n", "false");
			}
        }
    if (!strcmp(arg[0], "ANSplitDepth"))
        {
		if (atoi(arg[2]) == 0)
			ANSplitDepth = atoi(arg[1]);
		else
			{
			RandRange = atoi(arg[2]);
			ANSplitDepth = get_rand_num(ANSplitDepth * (100 - RandRange) / 100, ANSplitDepth * (100  + RandRange) / 100);
			}
        Send("ANSplitDepth: %d\n", ANSplitDepth);
        }
    if (!strcmp(arg[0], "CNSplitDepth"))
        {
		if (atoi(arg[2]) == 0)
			CNSplitDepth = atoi(arg[1]);
		else
			{
			RandRange = atoi(arg[2]);
			CNSplitDepth = get_rand_num(CNSplitDepth * (100 - RandRange) / 100, CNSplitDepth * (100  + RandRange) / 100);
			}
        Send("CNSplitDepth: %d\n", CNSplitDepth);
        }
    if (!strcmp(arg[0], "PVSplitDepth"))
        {
		if (atoi(arg[2]) == 0)
			PVSplitDepth = atoi(arg[1]);
		else
			{
			RandRange = atoi(arg[2]);
			PVSplitDepth = get_rand_num(PVSplitDepth * (100 - RandRange) / 100, PVSplitDepth * (100  + RandRange) / 100);
			}
        Send("PVSplitDepth: %d\n", PVSplitDepth);
        }
	// Time Management
    if (!strcmp(arg[0], "AbsoluteFactor"))
        {
		Send("\n[] Time Management\n");
		if (atoi(arg[2]) == 0)
			AbsoluteFactor = atoi(arg[1]);
		else
			{
			RandRange = atoi(arg[2]);
			AbsoluteFactor = get_rand_num(AbsoluteFactor * (100 - RandRange) / 100, AbsoluteFactor * (100  + RandRange) / 100);
			}
        Send("AbsoluteFactor: %d\n", AbsoluteFactor);
        }
    if (!strcmp(arg[0], "BattleFactor"))
        {
		if (atoi(arg[2]) == 0)
			BattleFactor = atoi(arg[1]);
		else
			{
			RandRange = atoi(arg[2]);
			BattleFactor = get_rand_num(BattleFactor * (100 - RandRange) / 100, BattleFactor * (100  + RandRange) / 100);
			}
        Send("BattleFactor: %d\n", BattleFactor);
        }
    if (!strcmp(arg[0], "DesiredMillis"))
        {
		if (atoi(arg[2]) == 0)
			DesiredMillis = atoi(arg[1]);
		else
			{
			RandRange = atoi(arg[2]);
			DesiredMillis = get_rand_num(DesiredMillis * (100 - RandRange) / 100, DesiredMillis * (100  + RandRange) / 100);
			}
        Send("DesiredMillis: %d\n", DesiredMillis);
        }
    if (!strcmp(arg[0], "EasyFactor"))
        {
		if (atoi(arg[2]) == 0)
			EasyFactor = atoi(arg[1]);
		else
			{
			RandRange = atoi(arg[2]);
			EasyFactor = get_rand_num(EasyFactor * (100 - RandRange) / 100, EasyFactor * (100  + RandRange) / 100);
			}
        Send("EasyFactor: %d\n", EasyFactor);
        }
    if (!strcmp(arg[0], "EasyFactorPonder"))
        {
		if (atoi(arg[2]) == 0)
			EasyFactorPonder = atoi(arg[1]);
		else
			{
			RandRange = atoi(arg[2]);
			EasyFactorPonder = get_rand_num(EasyFactorPonder * (100 - RandRange) / 100, EasyFactorPonder * (100  + RandRange) / 100);
			}
        Send("EasyFactorPonder: %d\n", EasyFactorPonder);
        }
    if (!strcmp(arg[0], "NormalFactor"))
        {
		if (atoi(arg[2]) == 0)
			NormalFactor = atoi(arg[1]);
		else
			{
			RandRange = atoi(arg[2]);
			NormalFactor = get_rand_num(NormalFactor * (100 - RandRange) / 100, NormalFactor * (100  + RandRange) / 100);
			}
        Send("NormalFactor: %d\n", NormalFactor);
        }

	// Weights
    if (!strcmp(arg[0], "DrawWeight"))
        {
		Send("\n[] Weights\n");
		if (atoi(arg[2]) == 0)
			DrawWeight = atoi(arg[1]);
		else
			{
			RandRange = atoi(arg[2]);
			DrawWeight = get_rand_num(DrawWeight * (100 - RandRange) / 100,DrawWeight * (100  + RandRange) / 100);
			}
        Send("DrawWeight: %d\n", DrawWeight);
        }
    if (!strcmp(arg[0], "KingSafetyWeight"))
        {
		if (atoi(arg[2]) == 0)
			KingSafetyWeight = atoi(arg[1]);
		else
			{
			RandRange = atoi(arg[2]);
			KingSafetyWeight = get_rand_num(KingSafetyWeight * (100 - RandRange) / 100, KingSafetyWeight * (100  + RandRange) / 100);
			}
        Send("KingSafetyWeight: %d\n", KingSafetyWeight);
        }
    if (!strcmp(arg[0], "MaterialWeight"))
        {
		if (atoi(arg[2]) == 0)
			MaterialWeight= atoi(arg[1]);
		else
			{
			RandRange = atoi(arg[2]);
			MaterialWeight = get_rand_num(MaterialWeight * (100 - RandRange) / 100, MaterialWeight * (100  + RandRange) / 100);
			}
        Send("MaterialWeight: %d\n", MaterialWeight);
        }
    if (!strcmp(arg[0], "MobilityWeight"))
        {
		if (atoi(arg[2]) == 0)
			MobilityWeight = atoi(arg[1]);
		else
			{
			RandRange = atoi(arg[2]);
			MobilityWeight = get_rand_num(MobilityWeight * (100 - RandRange) / 100, MobilityWeight * (100  + RandRange) / 100);
			}
        Send("MobilityWeight: %d\n", MobilityWeight);
        }
    if (!strcmp(arg[0], "PawnWeight"))
        {
		if (atoi(arg[2]) == 0)
			PawnWeight = atoi(arg[1]);
		else
			{
			RandRange = atoi(arg[2]);
			PawnWeight = get_rand_num(PawnWeight * (100 - RandRange) / 100, PawnWeight * (100  + RandRange) / 100);
			}
        Send("PawnWeight: %d\n", PawnWeight);
        }
    if (!strcmp(arg[0], "PositionalWeight"))
        {
		if (atoi(arg[2]) == 0)
			PositionalWeight = atoi(arg[1]);
		else
			{
			RandRange = atoi(arg[2]);
			PositionalWeight = get_rand_num(PSTWeight * (100 - RandRange) / 100, PositionalWeight * (100  + RandRange) / 100);
			}
        Send("PositionalWeight: %d\n", PositionalWeight);
        }	
    if (!strcmp(arg[0], "PSTWeight"))
        {
		if (atoi(arg[2]) == 0)
			PSTWeight = atoi(arg[1]);
		else
			{
			RandRange = atoi(arg[2]);
			PSTWeight = get_rand_num(PSTWeight * (100 - RandRange) / 100, PSTWeight * (100  + RandRange) / 100);
			}
        Send("PSTWeight: %d\n", PSTWeight);
        }	
	}

void read_cfg_file(char *file_name)
    {
    char parambuf[256];
    cfgFile = fopen(file_name, "rt");
    if (cfgFile)
        {
		CfgFound = true;
		Send("info string %s found\n\n", "fire.cfg");
        while (!feof(cfgFile))
            {
            strcpy(parambuf, "");
            fgets(parambuf, 256, cfgFile);
            parse_option(parambuf);
            }
        fclose(cfgFile);
        cfgFile = NULL;
        Send("\n");
        }
    else
        {
		CfgFound = false;
		Send("info string %s not found\n\n", "fire.cfg");
        }
    }

void gen_def_cfg_file(char *file_name)
    {
	Send("writing default fire.cfg\n");
	fp = fopen(file_name, "w");
	fprintf(fp, "[] " Engine  " " Vers " default fire.cfg\n");

	fprintf(fp, "\n// System\n");
    fprintf(fp, "Verbose_UCI %d\n", 0);
    fprintf(fp, "Write_Log %d\n", 0);
    fprintf(fp, "Hash %d\n", DEFAULT_HASH_SIZE);
    fprintf(fp, "Pawn_Hash %d\n", DEFAULT_PAWN_HASH_SIZE);
    fprintf(fp, "Max_Threads %d\n", OptMaxThreads);
	fprintf(fp, "Min_Threads %d\n", OptMinThreads); // Added 5/22/2013 by Yuri Censor for Firenzina
    fprintf(fp, "MultiPV %d\n", DEFAULT_MULTIPV);

#ifdef RobboBases
    fprintf(fp, "\n// RobboBases\n");
    fprintf(fp, "Use_RobboBases %d\n", 0);
    fprintf(fp, "Verbose_RobboBases %d\n", 0);

    fprintf(fp, "\n// TotalBases\n");
    fprintf(fp, "AutoLoad_TotalBases %d\n", 0);
	fprintf(fp, "TotalBase_Path %s\n", "c:\\TotalBases");
    fprintf(fp, "TotalBase_Cache %d\n", 1);

    fprintf(fp, "\n// TripleBases\n");
    fprintf(fp, "AutoLoad_TripleBases %d\n", 0);
    fprintf(fp, "TripleBase_Path %s\n", "c:\\TripleBases");
    fprintf(fp, "TripleBase_Hash %d\n", 1);
    fprintf(fp, "Dynamic_TripleBase_Cache %d\n", 1);
#endif

	// Lazy Eval
	fprintf(fp, "\n[] Lazy Eval\n");
	fprintf(fp, "LazyEvalMin %d %d\n", DEFAULT_LAZY_EVAL_MIN, 0);
	fprintf(fp, "LazyEvalMax %d %d\n", DEFAULT_LAZY_EVAL_MAX, 0);

	// Piece Values	
	fprintf(fp, "\n[] Piece Values\n");
	fprintf(fp, "PawnValue %d %d\n", DEFAULT_PAWN_VALUE , 0);
	fprintf(fp, "KnightValue %d %d\n", DEFAULT_KNIGHT_VALUE, 0);
	fprintf(fp, "BishopValue %d %d\n", DEFAULT_BISHOP_VALUE, 0);
	fprintf(fp, "RookValue %d %d\n", DEFAULT_ROOK_VALUE, 0);
	fprintf(fp, "QueenValue %d %d\n", DEFAULT_QUEEN_VALUE, 0);
	fprintf(fp, "BishopPairValue %d %d\n", DEFAULT_BISHOP_PAIR_VALUE, 0);	

	// Prune Thresholds	
	fprintf(fp, "\n[] Prune Thresholds\n");
	fprintf(fp, "PruneCheck %d %d\n", DEFAULT_PRUNE_CHECK, 0);
	fprintf(fp, "PrunePawn %d %d\n", DEFAULT_PRUNE_PAWN, 0);
	fprintf(fp, "PruneMinor %d %d\n", DEFAULT_PRUNE_MINOR, 0);
	fprintf(fp, "PruneRook %d %d\n", DEFAULT_PRUNE_ROOK, 0);

	// Search
	fprintf(fp, "\n[] Search\n");
	fprintf(fp, "AspirationWindow %d %d\n", DEFAULT_ASPIRATION_WINDOW, 0);
	fprintf(fp, "CountLimit %d %d\n", 5, 0);
	fprintf(fp, "CutMargin %d %d\n", 1125, 0);
	fprintf(fp, "DeltaCutoff %d %d\n", MIN_DELTA_CUTOFF, 0);
	fprintf(fp, "ExcludeMargin %d %d\n", 1125, 0);
	fprintf(fp, "ExtendInCheck %d %d\n", 0, 0);
	fprintf(fp, "HistoryThreshold %d %d\n", DEFAULT_HISTORY_THRESHOLD, 0);
	fprintf(fp, "LowDepthMargin %d %d\n", DEFAULT_LOW_DEPTH_MARGIN, 0);
	fprintf(fp, "MinDepthMultiplier %d %d\n", DEFAULT_MIN_DEPTH_MULTIPLIER, 0);
	fprintf(fp, "MinTransMoveDepth %d %d\n", DEFAULT_MIN_TRANS_MOVE_DEPTH, 0);
	fprintf(fp, "NullReduction %d %d\n", DEFAULT_NULL_REDUCTION, 0);
	fprintf(fp, "QSAlphaThreshold %d %d\n", DEFAULT_QS_ALPHA_THRESHOLD, 0);
	fprintf(fp, "SearchDepthMin %d %d\n", DEFAULT_SEARCH_DEPTH_MIN, 0);
	fprintf(fp, "SearchDepthReduction %d %d\n", DEFAULT_SEARCH_DEPTH_REDUCTION, 0);
	fprintf(fp, "SEECutOff %d %d\n", DEFAULT_SEE_CUTOFF, 0);
	fprintf(fp, "SEELimit %d %d\n", DEFAULT_SEE_LIMIT, 0);
	fprintf(fp, "TopMinDepth %d %d\n", DEFAULT_TOP_MIN_DEPTH, 0);
	fprintf(fp, "UndoCountThreshold %d %d\n", DEFAULT_UNDO_COUNT_THRESHOLD, 0);
	fprintf(fp, "ValueCut %d %d\n", DEFAULT_VALUE_CUT, 0);
	fprintf(fp, "VerifyNull %d %d\n", 1, 0);
	fprintf(fp, "VerifyReduction %d %d\n", DEFAULT_VERIFY_REDUCTION, 0);

	// Split Depth
	fprintf(fp, "\n[] Split Depth\n");
	fprintf(fp, "SplitAtCN %d %d\n", 1, 0);
	fprintf(fp, "ANSplitDepth %d %d\n", DEFAULT_AN_SPLIT_DEPTH, 0);
	fprintf(fp, "CNSplitDepth %d %d\n", DEFAULT_CN_SPLIT_DEPTH, 0);
	fprintf(fp, "PVSplitDepth %d %d\n", DEFAULT_PV_SPLIT_DEPTH, 0);

	// Time Management
	fprintf(fp, "\n[] Time Management\n");
	fprintf(fp, "AbsoluteFactor %d %d\n", DEFAULT_ABSOLUTE_FACTOR, 0);
	fprintf(fp, "BattleFactor %d %d\n", DEFAULT_BATTLE_FACTOR, 0);
	fprintf(fp, "DesiredMillis %d %d\n", DEFAULT_DESIRED_MILLIS, 0);
	fprintf(fp, "EasyFactor %d %d\n", DEFAULT_EASY_FACTOR, 0);
	fprintf(fp, "EasyFactorPonder %d %d\n", DEFAULT_EASY_FACTOR_PONDER, 0);
	fprintf(fp, "NormalFactor %d %d\n", DEFAULT_NORMAL_FACTOR, 0);	

	// Weights
	fprintf(fp, "\n[] Weights\n");
	fprintf(fp, "DrawWeight %d %d\n", DEFAULT_DRAW_WEIGHT, 0);
	fprintf(fp, "KingSafetyWeight %d %d\n", DEFAULT_KING_SAFETY_WEIGHT, 0);
	fprintf(fp, "MaterialWeight %d %d\n", DEFAULT_MATERIAL_WEIGHT, 0);
	fprintf(fp, "MobilityWeight %d %d\n", DEFAULT_MOBILITY_WEIGHT, 0);
	fprintf(fp, "PawnWeight %d %d\n", DEFAULT_PAWN_WEIGHT, 0);
	fprintf(fp, "PositionalWeight %d %d\n", DEFAULT_POSITIONAL_WEIGHT, 0);
	fprintf(fp, "PSTWeight %d %d\n", DEFAULT_PST_WEIGHT, 0);			

	fclose(fp);
    Send("done\n\n");
    }

void gen_cur_cfg_file()
    {
    char buf[256];
    time_t now;
    struct tm tnow;
    time(&now);
    tnow = *localtime(&now);
    strftime(buf, 32, "%d-%b %H-%M", &tnow);
    sprintf(filename, "%s %s %s %s.cfg", Engine, Vers, Plat, buf);

	if(rename("fire.cfg", filename) == 0)
		printf("%s has been renamed %s\n", "fire.cfg", filename);
	else
		fprintf(stderr, "Error renaming %s\n", "fire.cfg");

    fp = fopen("fire.cfg", "wt");
	Send("writing new fire.cfg\n");
	fprintf(fp, "[] " Engine  " " Vers " custom fire.cfg\n"); 

	// Lazy Eval
	fprintf(fp, "\n[] Lazy Eval\n");
	fprintf(fp, "LazyEvalMin %d %d\n", LazyEvalMin, 0);
	fprintf(fp, "LazyEvalMax %d %d\n", LazyEvalMax, 0);

	// Piece Values	
	fprintf(fp, "\n[] Piece Values\n");
	fprintf(fp, "PawnValue %d %d\n", PValue, 0);
	fprintf(fp, "KnightValue %d %d\n", NValue, 0);
	fprintf(fp, "BishopValue %d %d\n", BValue, 0);
	fprintf(fp, "RookValue %d %d\n", RValue, 0);
	fprintf(fp, "QueenValue %d %d\n", QValue, 0);
	fprintf(fp, "BishopPairValue %d %d\n", BPValue, 0);	

	// Prune Thresholds	
	fprintf(fp, "\n[] Prune Thresholds\n");
	fprintf(fp, "PruneCheck %d %d\n", PruneCheck, 0);
	fprintf(fp, "PrunePawn %d %d\n", PrunePawn, 0);
	fprintf(fp, "PruneMinor %d %d\n", PruneMinor, 0);
	fprintf(fp, "PruneRook %d %d\n", PruneRook, 0);

	// Search
	fprintf(fp, "\n[] Search\n");
	fprintf(fp, "AspirationWindow %d %d\n", AspirationWindow, 0);
	fprintf(fp, "CountLimit %d %d\n", CountLimit, 0);
	fprintf(fp, "DeltaCutoff %d %d\n", DeltaCutoff, 0);
	fprintf(fp, "ExtendInCheck %d %d\n", ExtendInCheck, 0);
	fprintf(fp, "HistoryThreshold %d %d\n", HistoryThreshold, 0);
	fprintf(fp, "LowDepthMargin %d %d\n", LowDepthMargin, 0);
	fprintf(fp, "MinDepthMultiplier %d %d\n", MinDepthMultiplier, 0);
	fprintf(fp, "MinTransMoveDepth %d %d\n", MinTransMoveDepth, 0);
	fprintf(fp, "NullReduction %d %d\n", NullReduction, 0);
	fprintf(fp, "QSAlphaThreshold %d %d\n", QSAlphaThreshold, 0);
	fprintf(fp, "SearchDepthMin %d %d\n", SearchDepthMin, 0);
	fprintf(fp, "SearchDepthReduction %d %d\n", SearchDepthReduction, 0);
	fprintf(fp, "SEECutOff %d %d\n", SEECutOff, 0);
	fprintf(fp, "SEELimit %d %d\n", SEELimit, 0);
	fprintf(fp, "TopMinDepth %d %d\n", TopMinDepth, 0);
	fprintf(fp, "UndoCountThreshold %d %d\n", UndoCountThreshold, 0);
	fprintf(fp, "ValueCut %d %d\n", ValueCut, 0);
	fprintf(fp, "VerifyNull %d %d\n", VerifyNull, 0);
	fprintf(fp, "VerifyReduction %d %d\n", VerifyReduction, 0);

	// Split Depth
	fprintf(fp, "\n[] Split Depth\n");
	fprintf(fp, "SplitAtCN %d %d\n", SplitAtCN, 0);
	fprintf(fp, "ANSplitDepth %d %d\n", ANSplitDepth, 0);
	fprintf(fp, "CNSplitDepth %d %d\n", CNSplitDepth, 0);
	fprintf(fp, "PVSplitDepth %d %d\n", PVSplitDepth, 0);

	// Time Management
	fprintf(fp, "\n[] Time Management\n");
	fprintf(fp, "AbsoluteFactor %d %d\n", AbsoluteFactor, 0);
	fprintf(fp, "BattleFactor %d %d\n", BattleFactor, 0);
	fprintf(fp, "DesiredMillis %d %d\n", DesiredMillis, 0);
	fprintf(fp, "EasyFactor %d %d\n", EasyFactor, 0);
	fprintf(fp, "EasyFactorPonder %d %d\n", EasyFactorPonder, 0);
	fprintf(fp, "NormalFactor %d %d\n", NormalFactor, 0);	

	// Weights
	fprintf(fp, "\n[] Weights\n");
	fprintf(fp, "KingSafetyWeight %d %d\n", KingSafetyWeight, 0);
	fprintf(fp, "MaterialWeight %d %d\n", MaterialWeight, 0);
	fprintf(fp, "MobilityWeight %d %d\n", MobilityWeight, 0);
	fprintf(fp, "PawnWeight %d %d\n", PawnWeight, 0);
	fprintf(fp, "PositionalWeight %d %d\n", PositionalWeight, 0);	
	fprintf(fp, "PSTWeight %d %d\n", PSTWeight, 0);			

	fclose(fp);
    Send("done\n\n");
    }