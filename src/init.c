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

#include "fire.h"


#if defined(__GNUC__) && !defined(__MINGW32__) && !defined(__MINGW64__)

#include <sys/time.h>



static int GetTickCount() {

    struct timeval t;

    gettimeofday(&t, NULL);

    return t.tv_sec*1000 + t.tv_usec/1000;

}
#endif

#ifdef InitCFG
FILE *fp;

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
    arg[0][0] = arg[1][0] = arg[2][0] = arg[3][0] = arg[4][0] = arg[5][0] = '\0';
    sscanf(str, "%s %s %s %s %s %s", arg[0], arg[1], arg[2], arg[3], arg[4], arg[5]);

    if (!strcmp(arg[0], "//")) { }

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
		if (OptMaxThreads == 0)
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
    if (!strcmp(arg[0], "Random_Range"))
        {
        int input = atoi(arg[1]);
		RandRange = input;
		Send("Random Range: %d\n", RandRange);
		if (VerboseUCI)
			Send("info string Random Range: %d\n", RandRange);

#ifdef Log
		if (WriteLog)
			{
			log_file = fopen(log_filename, "a");
			fprintf(log_file, "Random Range: %d\n", RandRange);
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

	// Eval Weights
    if (!strcmp(arg[0], "Draw_Weight"))
        {
		Send("\n// Eval Weights\n");		
        DrawWeight = atoi(arg[1]);
		if (DrawWeight < 1)
			DrawWeight = 1;
		if (DrawWeight > MAX_DRAW_WEIGHT)
			DrawWeight = MAX_DRAW_WEIGHT;
        Send("Draw Weight: %d\n", DrawWeight);
		if (VerboseUCI)
			Send("info string Draw Weight: %d\n", DrawWeight);

#ifdef Log
		if (WriteLog)
			{
			log_file = fopen(log_filename, "a");
			fprintf(log_file, "Draw Weight: %d\n", DrawWeight);
			close_log();
			}
#endif
        }
    if (!strcmp(arg[0], "King_Safety_Weight"))
        {
        KingSafetyWeight = atoi(arg[1]);
		if (KingSafetyWeight < 1)
			KingSafetyWeight = 1;
		if (KingSafetyWeight > MAX_KING_SAFETY_WEIGHT)
			KingSafetyWeight = MAX_KING_SAFETY_WEIGHT;
        Send("King Safety Weight: %d\n", KingSafetyWeight);
		if (VerboseUCI)
			Send("info string King Safety Weight: %d\n", KingSafetyWeight);
#ifdef Log
		if (WriteLog)
			{
			log_file = fopen(log_filename, "a");
			fprintf(log_file, "King Safety Weight: %d\n", KingSafetyWeight);
			close_log();
			}
#endif
        }
    if (!strcmp(arg[0], "Material_Weight"))
        {
        MaterialWeight = atoi(arg[1]);
		// Addition by Yuri Censor for Firenzina (3/26/2013) begins here:
		if (MaterialWeight < 1) // bracketing was missing here,
			MaterialWeight = 1; // Yuri Censor added it.
		if (MaterialWeight > MAX_MATERIAL_WEIGHT)
			MaterialWeight = MAX_MATERIAL_WEIGHT;
        // Addition by Yuri Censor for Firenzina (3/26/2013) ends here.
        Send("Material Weight: %d\n", MaterialWeight);
		if (VerboseUCI)
			Send("info string Material Weight: %d\n", MaterialWeight);
#ifdef Log
		if (WriteLog)
			{
			log_file = fopen(log_filename, "a");
			fprintf(log_file, "Material Weight: %d\n", MaterialWeight);
			close_log();
			}
#endif
        }
    if (!strcmp(arg[0], "Mobility_Weight"))
        {
        MobilityWeight = atoi(arg[1]);
		if (MobilityWeight < 1)
			MobilityWeight = 1;
		if (MobilityWeight > MAX_MOBILITY_WEIGHT)
			MobilityWeight = MAX_MOBILITY_WEIGHT;
        Send("Mobility Weight: %d\n", MobilityWeight);
		if (VerboseUCI)
			Send("info string Mobility Weight: %d\n", MobilityWeight);
#ifdef Log
		if (WriteLog)
			{
			log_file = fopen(log_filename, "a");
			fprintf(log_file, "Mobility Weight: %d\n", MobilityWeight);
			close_log();
			}
#endif
        }
    if (!strcmp(arg[0], "Pawn_Weight"))
        {
        PawnWeight = atoi(arg[1]);
		if (PawnWeight < 1)
			PawnWeight = 1;
		if (PawnWeight > MAX_PAWN_WEIGHT)
			PawnWeight = MAX_PAWN_WEIGHT;
        Send("Pawn Weight: %d\n", PawnWeight);
		if (VerboseUCI)
			Send("info string Pawn Weight: %d\n", PawnWeight);
#ifdef Log
		if (WriteLog)
			{
			log_file = fopen(log_filename, "a");
			fprintf(log_file, "Pawn Weight: %d\n", PawnWeight);
			close_log();
			}
#endif
        }
    if (!strcmp(arg[0], "Positional_Weight"))
        {
        PositionalWeight = atoi(arg[1]);
		if (PositionalWeight < 1)
			PositionalWeight = 1;
		if (PositionalWeight > MAX_POSITIONAL_WEIGHT)
			PositionalWeight = MAX_POSITIONAL_WEIGHT;		
        Send("Positional Weight: %d\n", PositionalWeight);
		if (VerboseUCI)
			Send("info string Positional Weight: %d\n", PositionalWeight);
        }
    if (!strcmp(arg[0], "PST_Weight"))
        {
        PSTWeight = atoi(arg[1]);
        Send("PST Weight: %d\n", PSTWeight);
		if (VerboseUCI)
			Send("info string PST Weight: %d\n", PSTWeight);
        }		
    if (!strcmp(arg[0], "PST_Weight"))
        {
        PSTWeight = atoi(arg[1]);
		if (PSTWeight < 1)
			PSTWeight = 1;
		if (PSTWeight > MAX_PST_WEIGHT)
			PSTWeight = MAX_PST_WEIGHT;
        Send("PST Weight: %d\n", PSTWeight);
		if (VerboseUCI)
			Send("info string PST Weight: %d\n", PSTWeight);
#ifdef Log
		if (WriteLog)
			{
			log_file = fopen(log_filename, "a");
			fprintf(log_file, "PST Weight: %d\n", PSTWeight);
			close_log();
			}
#endif
        }

	// Lazy Eval
    if (!strcmp(arg[0], "Lazy_Eval_Min"))
        {
		Send("\n// Lazy Eval\n");		
        LazyEvalMin = atoi(arg[1]);
		if (LazyEvalMin < 1)
			LazyEvalMin = 1;
		if (LazyEvalMin > MAX_LAZY_EVAL_MIN)
			LazyEvalMin = MAX_LAZY_EVAL_MIN;
        Send("Lazy Eval Min: %d\n", LazyEvalMin);
		if (VerboseUCI)
			Send("info string Lazy Eval Min: %d\n", LazyEvalMin);
#ifdef Log
		if (WriteLog)
			{
			log_file = fopen(log_filename, "a");
			fprintf(log_file, "Lazy Eval Min: %d\n", LazyEvalMin);
			close_log();
			}
#endif
        }
    if (!strcmp(arg[0], "Lazy_Eval_Max"))
        {
        LazyEvalMax = atoi(arg[1]);
		if (LazyEvalMax < 1)
			LazyEvalMax = 1;
		if (LazyEvalMax > MAX_LAZY_EVAL_MAX)
			LazyEvalMax = MAX_LAZY_EVAL_MAX;
        Send("Lazy Eval Max: %d\n", LazyEvalMax);
		if (VerboseUCI)
			Send("info string Lazy Eval Max: %d\n", LazyEvalMax);
#ifdef Log
		if (WriteLog)
			{
			log_file = fopen(log_filename, "a");
			fprintf(log_file, "Lazy Eval Max: %d\n", LazyEvalMax);
			close_log();
			}
#endif
        }

	// Piece Values
    if (!strcmp(arg[0], "Pawn_Value"))
        {
		Send("\n// Piece Values\n");		
        PValue = atoi(arg[1]);
		if (PValue < 1)
			PValue = 1;
		if (PValue > MAX_PAWN_VALUE)
			PValue = MAX_PAWN_VALUE;
        Send("Pawn Value: %d\n", PValue);
		if (VerboseUCI)
			Send("info string Pawn Value: %d\n", PValue);
#ifdef Log
		if (WriteLog)
			{
			log_file = fopen(log_filename, "a");
			fprintf(log_file, "Pawn Value: %d\n", PValue);
			close_log();
			}
#endif
        }
    if (!strcmp(arg[0], "Knight_Value"))
        {
        NValue = atoi(arg[1]);
		if (NValue < 1)
			NValue = 1;
		if (NValue > MAX_KNIGHT_VALUE)
			NValue = MAX_KNIGHT_VALUE;
        Send("Knight Value: %d\n", NValue);
		if (VerboseUCI)
			Send("info string Knight Value: %d\n", NValue);
#ifdef Log
		if (WriteLog)
			{
			log_file = fopen(log_filename, "a");
			fprintf(log_file, "Knight Value: %d\n", NValue);
			close_log();
			}
#endif
        }
    if (!strcmp(arg[0], "Bishop_Value"))
        {
        BValue = atoi(arg[1]);
		if (BValue < 1)
			BValue = 1;
		if (BValue > MAX_BISHOP_VALUE)
			BValue = MAX_BISHOP_VALUE;
        Send("Bishop Value: %d\n", BValue);
		if (VerboseUCI)
			Send("info string Bishop Value: %d\n", BValue);
#ifdef Log
		if (WriteLog)
			{
			log_file = fopen(log_filename, "a");
			fprintf(log_file, "Bishop Value: %d\n", BValue);
			close_log();
			}
#endif
        }
    if (!strcmp(arg[0], "Rook_Value"))
        {
        RValue = atoi(arg[1]);
		if (RValue < 1)
			RValue = 1;
		if (RValue > MAX_ROOK_VALUE)
			RValue = MAX_ROOK_VALUE;
        Send("Rook Value: %d\n", RValue);
		if (VerboseUCI)
			Send("info string Rook Value: %d\n", RValue);
#ifdef Log
		if (WriteLog)
			{
			log_file = fopen(log_filename, "a");
			fprintf(log_file, "Rook Value: %d\n", RValue);
			close_log();
			}
#endif
        }
    if (!strcmp(arg[0], "Queen_Value"))
        {
        QValue = atoi(arg[1]);
		if (QValue < 1)
			QValue = 1;
		if (QValue > MAX_QUEEN_VALUE)
			QValue = MAX_QUEEN_VALUE;
        Send("Queen Value: %d\n", QValue);
		if (VerboseUCI)
			Send("info string Queen Value: %d\n", QValue);
#ifdef Log
		if (WriteLog)
			{
			log_file = fopen(log_filename, "a");
			fprintf(log_file, "Queen Value: %d\n", QValue);
			close_log();
			}
#endif
        }
    if (!strcmp(arg[0], "Bishop_Pair_Value"))
        {
        BPValue = atoi(arg[1]);
		if (BPValue < 1)
			BPValue = 1;
		if (BPValue > MAX_BISHOP_PAIR_VALUE)
			BPValue = MAX_BISHOP_PAIR_VALUE;
        Send("Bishop Pair Value: %d\n", BPValue);
		if (VerboseUCI)
			Send("info string Bishop Pair Value: %d\n", BPValue);
#ifdef Log
		if (WriteLog)
			{
			log_file = fopen(log_filename, "a");
			fprintf(log_file, "Bishop Pair Value: %d\n", BPValue);
			close_log();
			}
#endif
        }
		
	// Prune Thresholds
    if (!strcmp(arg[0], "Prune_Check"))
        {
		Send("\n// Prune Thresholds\n");
        PruneCheck = atoi(arg[1]);
		if (PruneCheck < 1)
			PruneCheck = 1;
		if (PruneCheck > MAX_PRUNE_CHECK)
			PruneCheck = MAX_PRUNE_CHECK;
        Send("Prune Check: %d\n", PruneCheck);
		if (VerboseUCI)
			Send("info string Prune Check: %d\n", PruneCheck);
#ifdef Log
		if (WriteLog)
			{
			log_file = fopen(log_filename, "a");
			fprintf(log_file, "Prune Check: %d\n", PruneCheck);
			close_log();
			}
#endif
        }
    if (!strcmp(arg[0], "Prune_Pawn"))
        {
        PrunePawn = atoi(arg[1]);
		if (PrunePawn < 1) // Bug fix by Yuri Censor for Firenzina, 3/27/2013:
			PrunePawn = 1; // Was: PruneCheck
		if (PrunePawn > MAX_PRUNE_PAWN)
			PrunePawn = MAX_PRUNE_PAWN;
        Send("Prune Pawn: %d\n", PrunePawn);
		if (VerboseUCI)
			Send("info string Prune Pawn: %d\n", PrunePawn);
#ifdef Log
		if (WriteLog)
			{
			log_file = fopen(log_filename, "a");
			fprintf(log_file, "Prune Pawn: %d\n", PrunePawn);
			close_log();
			}
#endif
        }
    if (!strcmp(arg[0], "Prune_Minor"))
        {
        PruneMinor = atoi(arg[1]);
		if (PruneMinor < 1) // Bug fix by Yuri Censor for Firenzina, 3/27/2013:
			PruneMinor = 1; // Was: PruneCheck
		if (PruneMinor > MAX_PRUNE_MINOR)
			PruneMinor = MAX_PRUNE_MINOR;
        Send("Prune Minor: %d\n", PruneMinor);
		if (VerboseUCI)
			Send("info string Prune Minor: %d\n", PruneMinor);
#ifdef Log
		if (WriteLog)
			{
			log_file = fopen(log_filename, "a");
			fprintf(log_file, "Prune Minor: %d\n", PruneMinor);
			close_log();
			}
#endif
        }
    if (!strcmp(arg[0], "Prune_Rook"))
        {
        PruneRook = atoi(arg[1]);
		if (PruneRook < 1) // Bug fix by Yuri Censor for Firenzina, 3/27/2013:
			PruneRook = 1; // Was: PruneCheck
		if (PruneRook > MAX_PRUNE_ROOK)
			PruneRook = MAX_PRUNE_ROOK;
        Send("Prune Rook: %d\n", PruneRook);
		if (VerboseUCI)
			Send("info string Prune Rook: %d\n", PruneRook);
#ifdef Log
		if (WriteLog)
			{
			log_file = fopen(log_filename, "a");
			fprintf(log_file, "Prune Rook: %d\n", PruneRook);
			close_log();
			}
#endif
        }
		
	// Search Parameters
    if (!strcmp(arg[0], "Aspiration_Window"))
        {
		Send("\n// Search Parameters\n");
        AspirationWindow = atoi(arg[1]);
		if (AspirationWindow < 1)
			AspirationWindow = 1;
		if (AspirationWindow > MAX_ASPIRATION_WINDOW)
			AspirationWindow = MAX_ASPIRATION_WINDOW;
        Send("Aspiration Window: %d\n", AspirationWindow);
		if (VerboseUCI)
			Send("info string Aspiration Window: %d\n", AspirationWindow);
#ifdef Log
		if (WriteLog)
			{
			log_file = fopen(log_filename, "a");
			fprintf(log_file, "Aspiration Window: %d\n", AspirationWindow);
			close_log();
			}
#endif
        }
   if (!strcmp(arg[0], "Delta_Cutoff"))
        {
        DeltaCutoff = atoi(arg[1]);
		if (DeltaCutoff < MIN_DELTA_CUTOFF)
			DeltaCutoff = MIN_DELTA_CUTOFF;
		if (DeltaCutoff > MAX_DELTA_CUTOFF) 
			DeltaCutoff = MAX_DELTA_CUTOFF; 
        Send("Delta Cutoff: %d\n", DeltaCutoff);
		if (VerboseUCI)
			Send("info string Delta Cutoff: %d\n", DeltaCutoff);
#ifdef Log
		if (WriteLog)
			{
			log_file = fopen(log_filename, "a");
			fprintf(log_file, "Delta Cutoff: %d\n", DeltaCutoff);
			close_log();
			}
#endif
        }
    if (!strcmp(arg[0], "DepthRed_Min"))
        {
        DepthRedMin = atoi(arg[1]);
		if (DepthRedMin < 1)
			DepthRedMin = 1;
		if (DepthRedMin > MAX_DEPTH_RED_MIN) 
			DepthRedMin = MAX_DEPTH_RED_MIN; 
        Send("DepthRed Min: %d\n", DepthRedMin);
		if (VerboseUCI)
			Send("info string DepthRed Min: %d\n", DepthRedMin);
#ifdef Log
		if (WriteLog)
			{
			log_file = fopen(log_filename, "a");
			fprintf(log_file, "DepthRed Min: %d\n", DepthRedMin);
			close_log();
			}
#endif
        }
	if (!strcmp(arg[0], "Extend_In_Check"))
		{
        int input = atoi(arg[1]);
        if (input == 1)
            {
            ExtendInCheck = true;
			Send("Extend In Check: %s\n", "true");
			if (VerboseUCI)
				Send("info string Extend In Check: %s\n", "true");

#ifdef Log
			if (WriteLog)
				{
				log_file = fopen(log_filename, "a");
				fprintf(log_file, "Extend In Check: %s\n", "true");
				close_log();
				}
#endif
            }
        else
            {
            ExtendInCheck = false;
			Send("Extend In Check: %s\n", "false");
			if (VerboseUCI)
				Send("info string Extend In Check: %s\n", "false");


#ifdef Log
			if (WriteLog)
				{
				log_file = fopen(log_filename, "a");
				fprintf(log_file, "Extend In Check: %s\n", "false");
				close_log();
				}
#endif
            }
        }
    if (!strcmp(arg[0], "Height_Multiplier"))
        {
        HeightMultiplier = atoi(arg[1]);
		if (HeightMultiplier < 1)
			HeightMultiplier = 1;
		if (HeightMultiplier > MAX_HEIGHT_MULTIPLIER)
			HeightMultiplier = MAX_HEIGHT_MULTIPLIER;
        Send("Height Multiplier: %d\n", HeightMultiplier);
		if (VerboseUCI)
			Send("info string Height Multiplier: %d\n", HeightMultiplier);
#ifdef Log
		if (WriteLog)
			{
			log_file = fopen(log_filename, "a");
			fprintf(log_file, "Height Multiplier: %d\n", HeightMultiplier);
			close_log();
		}
#endif
        }
   if (!strcmp(arg[0], "History_Threshold"))
        {
        HistoryThreshold = atoi(arg[1]);
		if (HistoryThreshold < 1)
			HistoryThreshold = 1;
		if (HistoryThreshold > MAX_HISTORY_THRESHOLD)
			HistoryThreshold = MAX_HISTORY_THRESHOLD;
        Send("History Threshold: %d\n", HistoryThreshold);
		if (VerboseUCI)
			Send("info string History Threshold: %d\n", HistoryThreshold);
#ifdef Log
		if (WriteLog)
			{
			log_file = fopen(log_filename, "a");
			fprintf(log_file, "History Threshold: %d\n", HistoryThreshold);
			close_log();
			}
#endif
        }
   if (!strcmp(arg[0], "Low_Depth_Margin"))
        {
        LowDepthMargin = atoi(arg[1]);
		if (LowDepthMargin < 1)
			LowDepthMargin = 1;
		if (LowDepthMargin > MAX_LOW_DEPTH_MARGIN)
			LowDepthMargin = MAX_LOW_DEPTH_MARGIN;
        Send("Low Depth CutOff: %d\n", LowDepthMargin);
		if (VerboseUCI)
			Send("info string Low Depth CutOff: %d\n", LowDepthMargin);
#ifdef Log
		if (WriteLog)
			{
			log_file = fopen(log_filename, "a");
			fprintf(log_file, "History Threshold: %d\n", LowDepthMargin);
			close_log();
			}
#endif
        }
    if (!strcmp(arg[0], "Min_Depth_Multiplier"))
        {
        MinDepthMultiplier = atoi(arg[1]);
		if (MinDepthMultiplier < 1)
			MinDepthMultiplier = 1;
		if (MinDepthMultiplier > MAX_MIN_DEPTH_MULTIPLIER) 
			MinDepthMultiplier = MAX_MIN_DEPTH_MULTIPLIER; 
        Send("Min Depth Multiplier: %d\n", MinDepthMultiplier);
		if (VerboseUCI)
			Send("info string Min Depth Multiplier: %d\n", MinDepthMultiplier);
#ifdef Log
		if (WriteLog)
			{
			log_file = fopen(log_filename, "a");
			fprintf(log_file, "Min Depth Multiplier: %d\n", MinDepthMultiplier);
			close_log();
			}
#endif
        }
    if (!strcmp(arg[0], "Min_Trans_Move_Depth"))
        {
        MinTransMoveDepth = atoi(arg[1]);
		if (MinTransMoveDepth < 1)
			MinTransMoveDepth = 1;
		if (MinTransMoveDepth > MAX_MIN_TRANS_MOVE_DEPTH) 
			MinTransMoveDepth = MAX_MIN_TRANS_MOVE_DEPTH; 
        Send("Min Trans Move Depth: %d\n", MinTransMoveDepth);
		if (VerboseUCI)
			Send("info string Min Trans Move Depth: %d\n", MinTransMoveDepth);
#ifdef Log
		if (WriteLog)
			{
			log_file = fopen(log_filename, "a");
			fprintf(log_file, "Min Trans Move Depth: %d\n", MinTransMoveDepth);
			close_log();
			}
#endif
        }
    if (!strcmp(arg[0], "Null_Reduction"))
        {
        NullReduction = atoi(arg[1]);
		if (NullReduction < 1)
			NullReduction = 1;
		if (NullReduction > MAX_NULL_REDUCTION)
			NullReduction = MAX_NULL_REDUCTION;
        Send("Null Reduction: %d\n", NullReduction);
		if (VerboseUCI)
			Send("info string Null Reduction: %d\n", NullReduction);
#ifdef Log
		if (WriteLog)
			{
			log_file = fopen(log_filename, "a");
			fprintf(log_file, "Null Reduction: %d\n", NullReduction);
			close_log();
			}
#endif
        }
    if (!strcmp(arg[0], "QS_Alpha_Threshold"))
        {
        QSAlphaThreshold = atoi(arg[1]);
		if (QSAlphaThreshold < 1)
			QSAlphaThreshold = 1;
		if (QSAlphaThreshold > MAX_QS_ALPHA_THRESHOLD) 
			QSAlphaThreshold = MAX_QS_ALPHA_THRESHOLD; 
        Send("QS Alpha Threshold: %d\n", QSAlphaThreshold);
		if (VerboseUCI)
			Send("info string QS Alpha Threshold: %d\n", QSAlphaThreshold);
#ifdef Log
		if (WriteLog)
			{
			log_file = fopen(log_filename, "a");
			fprintf(log_file, "QS Alpha Threshold: %d\n", QSAlphaThreshold);
			close_log();
			}
#endif
        }
    if (!strcmp(arg[0], "Search_Depth_Min"))
        {
        SearchDepthMin = atoi(arg[1]);
		if (SearchDepthMin < 1)
			SearchDepthMin = 1;
		if (SearchDepthMin > MAX_SEARCH_DEPTH_MIN)
			SearchDepthMin = MAX_SEARCH_DEPTH_MIN;
        Send("Search Depth Min: %d\n", SearchDepthMin);
		if (VerboseUCI)
			Send("info string Search Depth Min: %d\n", SearchDepthMin);
#ifdef Log
		if (WriteLog)
			{
			log_file = fopen(log_filename, "a");
			fprintf(log_file, "Search Depth Min: %d\n", SearchDepthMin);
			close_log();
			}
#endif
        }
   if (!strcmp(arg[0], "Search_Depth_Reduction"))
        {
        SearchDepthReduction = atoi(arg[1]);
		if (SearchDepthReduction < 1)
			SearchDepthReduction = 1;
		if (SearchDepthReduction > MAX_SEARCH_DEPTH_REDUCTION)
			SearchDepthReduction = MAX_SEARCH_DEPTH_REDUCTION;
        Send("Search Depth Reduction: %d\n", SearchDepthReduction);
		if (VerboseUCI)
			Send("info string Search Depth Reduction: %d\n", SearchDepthReduction);
#ifdef Log
		if (WriteLog)
			{
			log_file = fopen(log_filename, "a");
			fprintf(log_file, "Search Depth Reduction: %d\n", SearchDepthReduction);
			close_log();
			}
#endif
        }
   if (!strcmp(arg[0], "Top_Min_Depth"))
        {
        TopMinDepth = atoi(arg[1]);
		if (TopMinDepth < 1)
			TopMinDepth = 1;
		if (TopMinDepth > MAX_TOP_MIN_DEPTH) 
			TopMinDepth = MAX_TOP_MIN_DEPTH; 
        Send("Top Min Depth: %d\n", TopMinDepth);
		if (VerboseUCI)
			Send("info string Top Min Depth: %d\n", TopMinDepth);
#ifdef Log
		if (WriteLog)
			{
			log_file = fopen(log_filename, "a");
			fprintf(log_file, "Top Min Depth: %d\n", TopMinDepth);
			close_log();
			}
#endif
        }
    if (!strcmp(arg[0], "Undo_Count_Threshold"))
        {
        UndoCountThreshold = atoi(arg[1]);
		if (UndoCountThreshold < 1)
			UndoCountThreshold = 1;
		if (UndoCountThreshold > MAX_UNDO_COUNT_THRESHOLD) 
			UndoCountThreshold = MAX_UNDO_COUNT_THRESHOLD; 
        Send("Undo Count Threshold: %d\n", UndoCountThreshold);
		if (VerboseUCI)
			Send("info string Undo Count Threshold: %d\n", UndoCountThreshold);
#ifdef Log
		if (WriteLog)
			{
			log_file = fopen(log_filename, "a");
			fprintf(log_file, "Undo Count Threshold: %d\n", UndoCountThreshold);
			close_log();
			}
#endif
        }
    if (!strcmp(arg[0], "Value_Cut"))
        {
        ValueCut = atoi(arg[1]);
        Send("Value Cut: %d\n", ValueCut);
		if (ValueCut < MIN_VALUE_CUT)
			ValueCut = MIN_VALUE_CUT;
		if (ValueCut > MAX_VALUE_CUT)
			ValueCut = MAX_VALUE_CUT;
		if (VerboseUCI)
			Send("info string Value Cut: %d\n", ValueCut);
#ifdef Log
		if (WriteLog)
			{
			log_file = fopen(log_filename, "a");
			fprintf(log_file, "Value Cut: %d\n", ValueCut);
			close_log();
			}
#endif
        }
	if (!strcmp(arg[0], "Verify_Null"))
		{
        int input = atoi(arg[1]);
        if (input == 1)
            {
            VerifyNull = true;
			Send("Verify Null: %s\n", "true");
			if (VerboseUCI)
				Send("info string Verify Null: %s\n", "true");

#ifdef Log
			if (WriteLog)
				{
				log_file = fopen(log_filename, "a");
				fprintf(log_file, "Verify Null: %s\n", "true");
				close_log();
				}
#endif
            }
        else
            {
            VerifyNull = false;
			Send("Verify Null: %s\n", "false");
			if (VerboseUCI)
				Send("info string Verify Null: %s\n", "false");

#ifdef Log
			if (WriteLog)
				{
				log_file = fopen(log_filename, "a");
				fprintf(log_file, "Verify Null: %s\n", "false");
				close_log();
				}
#endif
            }
        }
    if (!strcmp(arg[0], "Verify_Reduction"))
        {
        VerifyReduction = atoi(arg[1]);
		if (VerifyReduction < 1)
			VerifyReduction = 1;
		if (VerifyReduction > MAX_VERIFY_REDUCTION)
			VerifyReduction = MAX_VERIFY_REDUCTION;
        Send("Verify Reduction: %d\n", VerifyReduction);
		if (VerboseUCI)
			Send("info string Verify Reduction: %d\n", VerifyReduction);
#ifdef Log
		if (WriteLog)
			{
			log_file = fopen(log_filename, "a");
			fprintf(log_file, "Verify Reduction: %d\n", VerifyReduction);
			close_log();
			}
#endif
        }

	// Split Depths
	if (!strcmp(arg[0], "Split_At_CN"))
		{
        int input = atoi(arg[1]);
		Send("\n// Split Depths\n");
        if (input == 1)
            {
            SplitAtCN = true;
			Send("Split At CN: %s\n", "true");
			if (VerboseUCI)
				Send("info string Split At CN: %s\n", "true");

#ifdef Log
			if (WriteLog)
				{
				log_file = fopen(log_filename, "a");
				fprintf(log_file, "Split At CN: %s\n", "true");
				close_log();
				}
#endif
            }
        else
            {
            SplitAtCN = false;
			Send("Split At CN: %s\n", "false");
			if (VerboseUCI)
				Send("info string Split At CN: %s\n", "false");

#ifdef Log
			if (WriteLog)
				{
				log_file = fopen(log_filename, "a");
				fprintf(log_file, "Split At CN: %s\n", "false");
				close_log();
				}
#endif
            }
        }
    if (!strcmp(arg[0], "AN_Split_Depth"))
        {
        ANSplitDepth = atoi(arg[1]);
		if (ANSplitDepth < MIN_AN_SPLIT_DEPTH) 
			ANSplitDepth = MIN_AN_SPLIT_DEPTH; 
		if (ANSplitDepth > MAX_AN_SPLIT_DEPTH)
			ANSplitDepth = MAX_AN_SPLIT_DEPTH;
        Send("AN Split Depth: %d\n", ANSplitDepth);
		if (VerboseUCI)
			Send("info string AN Split Depth: %d\n", ANSplitDepth);
#ifdef Log
		if (WriteLog)
			{
			log_file = fopen(log_filename, "a");
			fprintf(log_file, "AN Split Depth: %d\n", ANSplitDepth);
			close_log();
			}
#endif
        }
    if (!strcmp(arg[0], "CN_Split_Depth"))
        {
        CNSplitDepth = atoi(arg[1]);
		if (CNSplitDepth < MIN_CN_SPLIT_DEPTH) 
			CNSplitDepth = MIN_CN_SPLIT_DEPTH; 
		if (CNSplitDepth > MAX_CN_SPLIT_DEPTH)
			CNSplitDepth = MAX_CN_SPLIT_DEPTH;
        Send("CN Split Depth: %d\n", CNSplitDepth);
		if (VerboseUCI)
			Send("info string CN Split Depth: %d\n", CNSplitDepth);
#ifdef Log
		if (WriteLog)
			{
			log_file = fopen(log_filename, "a");
			fprintf(log_file, "CN Split Depth: %d\n", CNSplitDepth);
			close_log();
			}
#endif
        }
    if (!strcmp(arg[0], "PV_Split_Depth"))
        {
        PVSplitDepth = atoi(arg[1]);
		if (PVSplitDepth < MIN_PV_SPLIT_DEPTH) 
			PVSplitDepth = MIN_PV_SPLIT_DEPTH; 
		if (PVSplitDepth > MAX_PV_SPLIT_DEPTH)
			PVSplitDepth = MAX_PV_SPLIT_DEPTH;
        Send("PV Split Depth: %d\n", PVSplitDepth);
		if (VerboseUCI)
			Send("info string PV Split Depth: %d\n", PVSplitDepth);
#ifdef Log
		if (WriteLog)
			{
			log_file = fopen(log_filename, "a");
			fprintf(log_file, "PV Split Depth: %d\n", PVSplitDepth);
			close_log();
			}
#endif
        }

	// Time Management
    if (!strcmp(arg[0], "Absolute_Factor"))
        {
		Send("\n// Time Management\n");
        AbsoluteFactor = atoi(arg[1]);
		if (AbsoluteFactor < 1)
			AbsoluteFactor = 1;
		if (AbsoluteFactor > MAX_ABSOLUTE_FACTOR)
			AbsoluteFactor = MAX_ABSOLUTE_FACTOR;
        Send("Absolute Factor: %d\n", AbsoluteFactor);
		if (VerboseUCI)
			Send("info string Absolute Factor: %d\n", AbsoluteFactor);
#ifdef Log
		if (WriteLog)
			{
			log_file = fopen(log_filename, "a");
			fprintf(log_file, "Absolute Factor: %d\n", AbsoluteFactor);
			close_log();
			}
#endif
        }
    if (!strcmp(arg[0], "Battle_Factor"))
        {
        BattleFactor = atoi(arg[1]);
		if (BattleFactor < 1)
			BattleFactor = 1;
		if (BattleFactor > MAX_BATTLE_FACTOR)
			BattleFactor = MAX_BATTLE_FACTOR;
        Send("Battle Factor: %d\n", BattleFactor);
		if (VerboseUCI)
			Send("info string Battle Factor: %d\n", BattleFactor);
#ifdef Log
		if (WriteLog)
			{
			log_file = fopen(log_filename, "a");
			fprintf(log_file, "Battle Factor: %d\n", BattleFactor);
			close_log();
			}
#endif
        }
    if (!strcmp(arg[0], "Easy_Factor"))
        {
        EasyFactor = atoi(arg[1]);
		if (EasyFactor < 1)
			EasyFactor = 1;
		if (EasyFactor > MAX_EASY_FACTOR)
			EasyFactor = MAX_EASY_FACTOR;
        Send("Easy Factor: %d\n", EasyFactor);
		if (VerboseUCI)
			Send("info string Easy Factor: %d\n", EasyFactor);
#ifdef Log
		if (WriteLog)
			{
			log_file = fopen(log_filename, "a");
			fprintf(log_file, "Easy Factor: %d\n", EasyFactor);
			close_log();
			}
#endif
        }
    if (!strcmp(arg[0], "Easy_Factor_Ponder"))
        {
        EasyFactorPonder = atoi(arg[1]);
		if (EasyFactorPonder < 1)
			EasyFactorPonder = 1;
		if (EasyFactorPonder > MAX_EASY_FACTOR_PONDER)
			EasyFactorPonder = MAX_EASY_FACTOR_PONDER;
        Send("Easy Factor Ponder: %d\n", EasyFactorPonder);
		if (VerboseUCI)
			Send("info string Easy Factor Ponder: %d\n", EasyFactorPonder);
#ifdef Log
		if (WriteLog)
			{
			log_file = fopen(log_filename, "a");
			fprintf(log_file, "Easy Factor Ponder: %d\n", EasyFactorPonder);
			close_log();
			}
#endif
        }
    if (!strcmp(arg[0], "Normal_Factor"))
        {
        NormalFactor = atoi(arg[1]);
		if (NormalFactor < 1)
			NormalFactor = 1;
		if (NormalFactor > MAX_NORMAL_FACTOR)
			NormalFactor = MAX_NORMAL_FACTOR;
        Send("Normal Factor: %d\n", NormalFactor);
		if (VerboseUCI)
			Send("info string Normal Factor: %d\n", NormalFactor);
#ifdef Log
		if (WriteLog)
			{
			log_file = fopen(log_filename, "a");
			fprintf(log_file, "Normal Factor: %d\n", NormalFactor);
			close_log();
			}
#endif
        }

	// UCI Info Strings
    if (!strcmp(arg[0], "CPULoad_Info"))
        {
        int input = atoi(arg[1]);
		Send("\n// UCI Info Strings\n");
        if (input == 1)
            {
            CPULoadInfo = true;
			Send("CPU Load Info: %s\n", "true");
			if (VerboseUCI)
				Send("info string CPU Load Info: %s\n", "true");

#ifdef Log
			if (WriteLog)
				{
				log_file = fopen(log_filename, "a");
				fprintf(log_file, "CPU Load Info: %s\n", "true");
				close_log();
				}
#endif
            }
        else
            {
            CPULoadInfo = false;
			Send("CPU Load Info: %s\n", "false");
			if (VerboseUCI)
				Send("info string CPU Load Info: %s\n", "false");

#ifdef Log
			if (WriteLog)
				{
				log_file = fopen(log_filename, "a");
				fprintf(log_file, "CPU Load Info: %s\n", "false");
				close_log();
				}
#endif
            }
        }
    if (!strcmp(arg[0], "CurrentMove_Info"))
        {
        int input = atoi(arg[1]);

        if (input == 1)
            {
            CurrMoveInfo = true;
			Send("Current Move Info: %s\n", "true");
			if (VerboseUCI)
				Send("info string Current Move Info: %s\n", "true");

#ifdef Log
			if (WriteLog)
				{
				log_file = fopen(log_filename, "a");
				fprintf(log_file, "Current Move Info: %s\n", "true");
				close_log();
				}
#endif
            }
        else
            {
            CurrMoveInfo = false;
			Send("Current Move Info: %s\n", "false");
			if (VerboseUCI)
				Send("info string Current Move Info: %s\n", "false");

#ifdef Log
			if (WriteLog)
				{
				log_file = fopen(log_filename, "a");
				fprintf(log_file, "Current Move Info: %s\n", "false");
				close_log();
				}
#endif
            }
        }
    if (!strcmp(arg[0], "Depth_Info"))
        {
        int input = atoi(arg[1]);

        if (input == 1)
            {
            DepthInfo = true;
			Send("Depth Info: %s\n", "true");
			if (VerboseUCI)
				Send("info string Depth Info: %s\n", "true");

#ifdef Log
			if (WriteLog)
				{
				log_file = fopen(log_filename, "a");
				fprintf(log_file, "Depth Info: %s\n", "true");
				close_log();
				}
#endif
            }
        else
            {
            DepthInfo = false;
			Send("Depth Info: %s\n", "false");
			if (VerboseUCI)
				Send("info string Depth Info: %s\n", "false");

#ifdef Log
			if (WriteLog)
				{
				log_file = fopen(log_filename, "a");
				fprintf(log_file, "Depth Info: %s\n", "false");
				close_log();
				}
#endif
            }
        }
   if (!strcmp(arg[0], "HashFull_Info"))
        {
        int input = atoi(arg[1]);

        if (input == 1)
            {
            HashFullInfo = true;
			Send("Hash Full Info: %s\n", "true");
			if (VerboseUCI)
				Send("info string Hash Full Info: %s\n", "true");

#ifdef Log
			if (WriteLog)
				{
				log_file = fopen(log_filename, "a");
				fprintf(log_file, "Hash Full Info: %s\n", "true");
				close_log();
				}
#endif
            }
        else
            {
            HashFullInfo = false;
			Send("Hash Full Info: %s\n", "false");
			if (VerboseUCI)
				Send("info string Hash Full Info: %s\n", "false");

#ifdef Log
			if (WriteLog)
				{
				log_file = fopen(log_filename, "a");
				fprintf(log_file, "Hash Full Info: %s\n", "false");
				close_log();
				}
#endif
            }
        }
   if (!strcmp(arg[0], "LowDepth_PVs"))
        {
        int input = atoi(arg[1]);

        if (input == 1)
            {
            LowDepthPVs = true;
			Send("Low Depth PVs: %s\n", "true");
			if (VerboseUCI)
				Send("info string Low Depth PVs: %s\n", "true");

#ifdef Log
			if (WriteLog)
				{
				log_file = fopen(log_filename, "a");
				fprintf(log_file, "Low Depth PVs: %s\n", "true");
				close_log();
				}
#endif
            }
        else
            {
            LowDepthPVs = false;
			Send("Low Depth PVs: %s\n", "false");
			if (VerboseUCI)
				Send("info string Low Depth PVs: %s\n", "false");

#ifdef Log
			if (WriteLog)
				{
				log_file = fopen(log_filename, "a");
				fprintf(log_file, "Low Depth PVs: %s\n", "false");
				close_log();
				}
#endif
            }
        }
    if (!strcmp(arg[0], "NPS_Info"))
        {
        int input = atoi(arg[1]);

        if (input == 1)
            {
            NPSInfo = true;
			Send("NPS Info: %s\n", "true");
			if (VerboseUCI)
				Send("info string NPS Info: %s\n", "true");
#ifdef Log
			if (WriteLog)
				{
				log_file = fopen(log_filename, "a");
				fprintf(log_file, "NPS Info: %s\n", "true");
				close_log();
				}
#endif
            }
        else
            {
            NPSInfo = false;
			Send("NPS Info: %s\n", "false");
			if (VerboseUCI)
				Send("info string NPS Info: %s\n", "false");
#ifdef Log
			if (WriteLog)
				{
				log_file = fopen(log_filename, "a");
				fprintf(log_file, "NPS Info: %s\n", "false");
				close_log();
				}
#endif
            }
		}

#ifdef RobboBases
    if (!strcmp(arg[0], "TBHit_Info"))
        {
        int input = atoi(arg[1]);

        if (input == 1)
            {
            TBHitInfo = true;
			Send("TB Hit Info: %s", "true");
			if (VerboseUCI)
				Send("info string TB Hit Info: %s\n", "true");

#ifdef Log
			if (WriteLog)
				{
				log_file = fopen(log_filename, "a");
				fprintf(log_file, "TB Hit Info: %s\n", "true");
				close_log();
				}
#endif
            }
        else
            {
            TBHitInfo = false;
			Send("TB Hit Info: %s", "false");
			if (VerboseUCI)
				Send("info string TB Hit Info: %s\n", "false");

#ifdef Log
			if (WriteLog)
				{
				log_file = fopen(log_filename, "a");
				fprintf(log_file, "TB Hit Info: %s\n", "false");
				close_log();
				}
#endif
            }
        }
#endif

	}

void read_cfg_file(char *file_name)
    {
    char parambuf[256];
    FILE *cfgFile;
    cfgFile = fopen(file_name, "rt");

    if (cfgFile)
        {
		CfgFound = true;
		Send("info string %s found\n", "fire.cfg");
		Send("info string UCI options disabled\n");
#ifdef Log
			if (WriteLog)
				{
				log_file = fopen(log_filename, "a");
				fprintf(log_file, "info string %s found\n", "fire.cfg");
				fprintf(log_file, "info string UCI options disabled\n");
				close_log();
				}
#endif

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
		Send("info string %s not found\n", "fire.cfg");
		Send("info string UCI options enabled\n");
#ifdef Log
			if (WriteLog)
				{
				log_file = fopen(log_filename, "a");
				fprintf(log_file, "info string %s not found\n", "fire.cfg");
				fprintf(log_file, "info string UCI options enabled\n");
				close_log();
				}
#endif
        }
    }

void gen_cfg_file(char *file_name)
    {
	if (CfgFile == 0)
		Send("writing default fire.cfg\n");
	else if (CfgFile == 1)
		Send("writing random fire.cfg\n");
	else if (CfgFile == 2)
		Send("writing random eval fire.cfg\n");
	else if (CfgFile == 3)
		Send("writing random material fire.cfg\n");
	else if (CfgFile == 4)
		Send("writing random prune fire.cfg\n");
	else if (CfgFile == 5)
		Send("writing random search fire.cfg\n");
	else if (CfgFile == 6)
		Send("writing random time fire.cfg\n");
	else
		Send("writing default fire.cfg\n");

#ifdef Log
	if (WriteLog)
		{
		log_file = fopen(log_filename, "a");
		if (CfgFile == 0)
			fprintf(fp, "writing default fire.cfg\n");
		else if (CfgFile == 1)
			fprintf(fp, "writing random fire.cfg\n");
		else if (CfgFile == 2)
			fprintf(fp, "writing random eval fire.cfg\n");
		else if (CfgFile == 3)
			fprintf(fp, "writing random material fire.cfg\n");
		else if (CfgFile == 4)
			fprintf(fp, "writing random prune fire.cfg\n");
		else if (CfgFile == 5)
			fprintf(fp, "writing random search fire.cfg\n");
		else if (CfgFile == 6)
			fprintf(fp, "writing random time fire.cfg\n");
		else
			fprintf(fp, "writing default fire.cfg\n");
		close_log();
		}
#endif

    fp = fopen(file_name, "w");
	if (CfgFile == 0)
		fprintf(fp, "// " Engine " " Vers " default fire.cfg\n");
	else if (CfgFile == 1)
		fprintf(fp, "// " Engine " " Vers " random fire.cfg\n");
	else if (CfgFile == 2)
		fprintf(fp, "// " Engine " " Vers " random eval fire.cfg\n");
	else if (CfgFile == 3)
		fprintf(fp, "// " Engine " " Vers " random material fire.cfg\n");
	else if (CfgFile == 4)
		fprintf(fp, "// " Engine " " Vers " random prune fire.cfg\n");
	else if (CfgFile == 5)
		fprintf(fp, "// " Engine " " Vers " random search fire.cfg\n");
	else if (CfgFile == 6)
		fprintf(fp, "// " Engine " " Vers " random time fire.cfg\n");
	else
		fprintf(fp, "// " Engine " " Vers " default fire.cfg\n");

    fprintf(fp, "\n// System\n");
    fprintf(fp, "Verbose_UCI %d\n", 0);
    fprintf(fp, "Write_Log %d\n", 0);
    fprintf(fp, "Hash %d\n", DEFAULT_HASH_SIZE);
    fprintf(fp, "Pawn_Hash %d\n", DEFAULT_PAWN_HASH_SIZE);
    fprintf(fp, "Max_Threads %d\n", OptMaxThreads);
    fprintf(fp, "MultiPV %d\n", DEFAULT_MULTIPV);
    fprintf(fp, "Random_Range %d\n", RandRange);

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

	fprintf(fp, "\n// Eval Weights\n");
	if (CfgFile == 1 || CfgFile == 2)
		{
		fprintf(fp, "Draw_Weight %d\n", get_rand_num(DrawWeight * (100 - RandRange) / 100, DrawWeight * (100  + RandRange) / 100));
		fprintf(fp, "King_Safety_Weight %d\n", get_rand_num(KingSafetyWeight * (100 - RandRange) / 100, KingSafetyWeight * (100  + RandRange) / 100));
		fprintf(fp, "Material_Weight %d\n", get_rand_num(MaterialWeight * (100 - RandRange) / 100, MaterialWeight * (100  + RandRange) / 100));
		fprintf(fp, "Mobility_Weight %d\n", get_rand_num(MobilityWeight * (100 - RandRange) / 100, MobilityWeight * (100  + RandRange) / 100));
		fprintf(fp, "Pawn_Weight %d\n", get_rand_num(PawnWeight * (100 - RandRange) / 100, PawnWeight * (100  + RandRange) / 100));
		fprintf(fp, "Positional_Weight %d\n", get_rand_num(PositionalWeight * (100 - RandRange) / 100, PositionalWeight * (100  + RandRange) / 100));
		fprintf(fp, "PST_Weight %d\n", get_rand_num(PSTWeight * (100 - RandRange) / 100, PSTWeight * (100  + RandRange) / 100));
		}
		else
		{
		fprintf(fp, "Draw_Weight %d\n", DEFAULT_DRAW_WEIGHT);
		fprintf(fp, "King_Safety_Weight %d\n", DEFAULT_KING_SAFETY_WEIGHT);
		fprintf(fp, "Material_Weight %d\n", DEFAULT_MATERIAL_WEIGHT);
		fprintf(fp, "Mobility_Weight %d\n", DEFAULT_MOBILITY_WEIGHT);
		fprintf(fp, "Pawn_Weight %d\n", DEFAULT_PAWN_WEIGHT);
		fprintf(fp, "Positional_Weight %d\n", DEFAULT_POSITIONAL_WEIGHT);
		fprintf(fp, "PST_Weight %d\n", DEFAULT_PST_WEIGHT);
		}

	fprintf(fp, "\n// Lazy Eval\n");
	if (CfgFile == 1 || CfgFile == 2)
		{
		fprintf(fp, "Lazy_Eval_Min %d\n", get_rand_num(LazyEvalMin * (100 - RandRange) / 100, LazyEvalMin * (100  + RandRange) / 100));
		fprintf(fp, "Lazy_Eval_Max %d\n", get_rand_num(LazyEvalMax * (100 - RandRange) / 100, LazyEvalMax * (100  + RandRange) / 100));
		}
	else
		{
		fprintf(fp, "Lazy_Eval_Min %d\n", DEFAULT_LAZY_EVAL_MIN);
		fprintf(fp, "Lazy_Eval_Max %d\n", DEFAULT_LAZY_EVAL_MAX);
		}

	fprintf(fp, "\n// Piece Values\n");
	if (CfgFile == 1 || CfgFile == 3)
		{
		fprintf(fp, "Pawn_Value %d\n", get_rand_num(PValue * (100 - RandRange) / 100, PValue * (100  + RandRange) / 100));
		fprintf(fp, "Knight_Value %d\n", get_rand_num(NValue * (100  - RandRange) / 100, NValue * (100  + RandRange) / 100));
		fprintf(fp, "Bishop_Value %d\n", get_rand_num(BValue * (100  - RandRange) / 100, BValue * (100  + RandRange) / 100));
		fprintf(fp, "Rook_Value %d\n", get_rand_num(RValue * (100  - RandRange) / 100, RValue * (100 + RandRange) / 100));
		fprintf(fp, "Queen_Value %d\n", get_rand_num(QValue * (100  - RandRange) / 100, QValue * (100  + RandRange) / 100));
		fprintf(fp, "Bishop_Pair_Value %d\n", get_rand_num(BPValue * (100  - RandRange) / 100, BPValue * (100  + RandRange) / 100));
		}
		else
		{
		fprintf(fp, "Pawn_Value %d\n", DEFAULT_PAWN_VALUE);
		fprintf(fp, "Knight_Value %d\n", DEFAULT_KNIGHT_VALUE);
		fprintf(fp, "Bishop_Value %d\n", DEFAULT_BISHOP_VALUE);
		fprintf(fp, "Rook_Value %d\n", DEFAULT_ROOK_VALUE);
		fprintf(fp, "Queen_Value %d\n", DEFAULT_QUEEN_VALUE);
		fprintf(fp, "Bishop_Pair_Value %d\n", DEFAULT_BISHOP_PAIR_VALUE);
		}

	fprintf(fp, "\n// Prune Thresholds\n");
	if (CfgFile == 1 || CfgFile == 4)
		{
		fprintf(fp, "Prune_Check %d\n", get_rand_num(PruneCheck * (100 - RandRange) / 100, PruneCheck * (100  + RandRange) / 100));
		fprintf(fp, "Prune_Pawn %d\n", get_rand_num(PrunePawn * (100 - RandRange) / 100, PrunePawn * (100  + RandRange) / 100));
		fprintf(fp, "Prune_Minor %d\n", get_rand_num(PruneMinor * (100 - RandRange) / 100, PruneMinor * (100  + RandRange) / 100));
		fprintf(fp, "Prune_Rook %d\n", get_rand_num(PruneRook * (100 - RandRange) / 100, PruneRook * (100  + RandRange) / 100));
		}
	else
		{
		fprintf(fp, "Prune_Check %d\n", DEFAULT_PRUNE_CHECK);
		fprintf(fp, "Prune_Pawn %d\n", DEFAULT_PRUNE_PAWN);
		fprintf(fp, "Prune_Minor %d\n", DEFAULT_PRUNE_MINOR);
		fprintf(fp, "Prune_Rook %d\n", DEFAULT_PRUNE_ROOK);
		}

	fprintf(fp, "\n// Search Parameters\n");
	if (CfgFile == 1 || CfgFile == 5)
		{
    fprintf(fp, "Aspiration_Window %d\n", get_rand_num(AspirationWindow * (100 - RandRange) / 100, AspirationWindow * (100  + RandRange) / 100));
	fprintf(fp, "Delta_Cutoff %d\n", get_rand_num(DeltaCutoff * (100 - RandRange) / 100, DeltaCutoff * (100  + RandRange) / 100));
	fprintf(fp, "DepthRed_Min %d\n", get_rand_num(DepthRedMin * (100 - RandRange) / 100, DepthRedMin * (100  + RandRange) / 100));
	fprintf(fp, "Extend_In_Check %d\n", get_rand_num(0, 1));
	fprintf(fp, "Height_Multiplier %d\n", get_rand_num(HeightMultiplier * (100 - RandRange) / 100, HeightMultiplier * (100  + RandRange) / 100));
	fprintf(fp, "History_Threshold %d\n", get_rand_num(HistoryThreshold * (100 - RandRange) / 100, HistoryThreshold * (100  + RandRange) / 100));
	fprintf(fp, "Low_Depth_Margin %d\n", get_rand_num(LowDepthMargin * (100 - RandRange) / 100, LowDepthMargin * (100  + RandRange) / 100));
	fprintf(fp, "Min_Depth_Multiplier %d\n", get_rand_num(MinDepthMultiplier * (100 - RandRange) / 100, MinDepthMultiplier * (100  + RandRange) / 100));
	fprintf(fp, "Min_Trans_Move_Depth %d\n", get_rand_num(MinTransMoveDepth * (100 - RandRange) / 100, MinTransMoveDepth * (100  + RandRange) / 100));
    fprintf(fp, "Null_Reduction %d\n", get_rand_num(NullReduction * (100 - RandRange) / 100, NullReduction * (100  + RandRange) / 100));
	fprintf(fp, "QS_Alpha_Threshold %d\n", get_rand_num(QSAlphaThreshold * (100 - RandRange) / 100, QSAlphaThreshold * (100  + RandRange) / 100));
	fprintf(fp, "Search_Depth_Min %d\n", get_rand_num(SearchDepthMin * (100 - RandRange) / 100, SearchDepthMin * (100  + RandRange) / 100));
	fprintf(fp, "Search_Depth_Reduction %d\n", get_rand_num(SearchDepthReduction * (100 - RandRange) / 100, SearchDepthReduction * (100  + RandRange) / 100));
	fprintf(fp, "Top_Min_Depth %d\n", get_rand_num(TopMinDepth * (100 - RandRange) / 100, TopMinDepth * (100  + RandRange) / 100));
	fprintf(fp, "Undo_Count_Threshold %d\n", get_rand_num(UndoCountThreshold * (100 - RandRange) / 100, UndoCountThreshold * (100  + RandRange) / 100));
	fprintf(fp, "Value_Cut %d\n", get_rand_num(ValueCut * (100 - RandRange) / 100, ValueCut * (100  + RandRange) / 100));
    fprintf(fp, "Verify_Null %d\n", get_rand_num(0, 1));
    fprintf(fp, "Verify_Reduction %d\n", get_rand_num(VerifyReduction * (100 - RandRange) / 100, VerifyReduction * (100  + RandRange) / 100));
		}
	else
		{
		fprintf(fp, "Aspiration_Window %d\n", DEFAULT_ASPIRATION_WINDOW);
		fprintf(fp, "Delta_Cutoff %d\n", DEFAULT_DELTA_CUTOFF);
		fprintf(fp, "DepthRed_Min %d\n", DEFAULT_DEPTH_RED_MIN);
		fprintf(fp, "Extend_In_Check %d\n", 0);
		fprintf(fp, "Height_Multiplier %d\n", DEFAULT_HEIGHT_MULTIPLIER);
		fprintf(fp, "History_Threshold %d\n", DEFAULT_HISTORY_THRESHOLD);
		fprintf(fp, "Low_Depth_Margin %d\n", DEFAULT_LOW_DEPTH_MARGIN);
		fprintf(fp, "Min_Depth_Multiplier %d\n", DEFAULT_MIN_DEPTH_MULTIPLIER);
		fprintf(fp, "Min_Trans_Move_Depth %d\n", DEFAULT_MIN_TRANS_MOVE_DEPTH);
		fprintf(fp, "Null_Reduction %d\n", DEFAULT_NULL_REDUCTION);
		fprintf(fp, "QS_Alpha_Threshold %d\n", DEFAULT_QS_ALPHA_THRESHOLD);
		fprintf(fp, "Search_Depth_Min %d\n", DEFAULT_SEARCH_DEPTH_MIN);
		fprintf(fp, "Search_Depth_Reduction %d\n", DEFAULT_SEARCH_DEPTH_REDUCTION);
		fprintf(fp, "Top_Min_Depth %d\n", DEFAULT_TOP_MIN_DEPTH);
		fprintf(fp, "Undo_Count_Threshold %d\n", DEFAULT_UNDO_COUNT_THRESHOLD);
		fprintf(fp, "Value_Cut %d\n", DEFAULT_VALUE_CUT);
		fprintf(fp, "Verify_Null %d\n", 1);
		fprintf(fp, "Verify_Reduction %d\n", DEFAULT_VERIFY_REDUCTION);
		}

	fprintf(fp, "\n// Split Depths\n");
	if (CfgFile == 1 || CfgFile == 5)
		{
		fprintf(fp, "Split_At_CN %d\n", get_rand_num(0, 1));
		fprintf(fp, "AN_Split_Depth %d\n", get_rand_num(ANSplitDepth * (100 - RandRange) / 100, ANSplitDepth * (100  + RandRange) / 100));
		fprintf(fp, "CN_Split_Depth %d\n", get_rand_num(CNSplitDepth * (100 - RandRange) / 100, CNSplitDepth * (100  + RandRange) / 100));
		fprintf(fp, "PV_Split_Depth %d\n", get_rand_num(PVSplitDepth * (100 - RandRange) / 100, PVSplitDepth * (100  + RandRange) / 100));
		}
		else
		{
		fprintf(fp, "Split_At_CN %d\n", 1);
		fprintf(fp, "AN_Split_Depth %d\n", DEFAULT_AN_SPLIT_DEPTH);
		fprintf(fp, "CN_Split_Depth %d\n", DEFAULT_CN_SPLIT_DEPTH);
		fprintf(fp, "PV_Split_Depth %d\n", DEFAULT_PV_SPLIT_DEPTH);
		}

	fprintf(fp, "\n// Time Management\n");
	if (CfgFile == 1 || CfgFile == 6)
		{
		fprintf(fp, "Absolute_Factor %d\n", get_rand_num(AbsoluteFactor * (100 - RandRange) / 100, AbsoluteFactor * (100  + RandRange) / 100));
		fprintf(fp, "Battle_Factor %d\n", get_rand_num(BattleFactor * (100 - RandRange) / 100, BattleFactor * (100  + RandRange) / 100));
		fprintf(fp, "Easy_Factor %d\n", get_rand_num(EasyFactor * (100 - RandRange) / 100, EasyFactor * (100  + RandRange) / 100));
		fprintf(fp, "Easy_Factor_Ponder %d\n", get_rand_num(EasyFactorPonder * (100 - RandRange) / 100, EasyFactorPonder * (100  + RandRange) / 100));
		fprintf(fp, "Normal_Factor %d\n", get_rand_num(NormalFactor * (100 - RandRange) / 100, NormalFactor * (100  + RandRange) / 100));
		}
		else
		{
		fprintf(fp, "Absolute_Factor %d\n", DEFAULT_ABSOLUTE_FACTOR);
		fprintf(fp, "Battle_Factor %d\n", DEFAULT_BATTLE_FACTOR);
		fprintf(fp, "Easy_Factor %d\n", DEFAULT_EASY_FACTOR);
		fprintf(fp, "Easy_Factor_Ponder %d\n", DEFAULT_EASY_FACTOR_PONDER);
		fprintf(fp, "Normal_Factor %d\n", DEFAULT_NORMAL_FACTOR);
		}

	fprintf(fp, "\n// UCI Info strings\n");
    fprintf(fp, "CPULoad_Info %d\n", 0);
    fprintf(fp, "CurrentMove_Info %d\n", 0);
    fprintf(fp, "Depth_Info %d\n", 0);
    fprintf(fp, "HashFull_Info %d\n", 0);
    fprintf(fp, "LowDepth_PVs %d\n", 0);
	fprintf(fp, "NPS_Info %d\n", 0);
	fprintf(fp, "TBHit_Info %d\n", 0);
	
    fclose(fp);
    Send("done\n\n");

#ifdef Log
	if (WriteLog)
		{
		log_file = fopen(log_filename, "a");
		fprintf(log_file, "done.\n\n");
		close_log();
		}
#endif
    }
#endif
