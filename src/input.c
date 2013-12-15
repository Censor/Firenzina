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
#define MaxVar 8
#define NumSubClasses 8

static bool HasDeletion = false;
static MutexType InputDelete[1];
static bool Init = false;
static bool QuitDemand = false;

static int UCIFen ()
	{
	Send("info string %s\n", EmitFen (RootPosition0, String1[0]));

#ifdef Log
	if (WriteLog)
		{
		log_file = fopen(log_filename, "a");
		fprintf(log_file, "info string %s\n", EmitFen (RootPosition0, String1[0]));
		close_log();
		}
#endif

	return true;
	}
typedef enum
    {
    UCISpin,
    UCICheck,
    UCIButton,
    UCIString,
    UCICombo,
    UCIIgnore
    } enumUCI;
typedef struct
    {
    char name[0x100];
    char sub_class[0x40];
    int type;
    sint32 min, max, def;
    void *var;
    int(*action)(int);
    } UCItype;

char Combo[1][MaxVar][16] =
    {
    { "Tic", "Tac", "Toe" }
    };
char TicTacToe[16];

char SubClassList[NumSubClasses][16] =
    {
	"Eval", "System", "Info", "Other", "RobboBases", "Search", "SMP", "Time"
    };
UCItype UCIOptions[256] =
    {
// System
    { "Hash", "System", UCISpin, 1, MAX_HASH_SIZE, DEFAULT_HASH_SIZE, &CurrentHashSize, &InitHash },
    { "Pawn_Hash", "System", UCISpin, 1, MAX_PAWN_HASH_SIZE, DEFAULT_PAWN_HASH_SIZE, &CurrentPHashSize, &InitPawnHashWrapper },

#ifdef RobboBases
	{ "TripleBase_Hash", "RobboBases", UCISpin, 1, MAX_TRIPLE_BASE_HASH, 1, NULL, &InitTripleHash },
	{ "TotalBase_Cache", "RobboBases", UCISpin, 1, MAX_TOTAL_BASE_CACHE, 1, NULL, &SetTotalBaseCache },
	{ "Dynamic_TripleBase_Cache", "RobboBases", UCISpin, 1, MAX_DYNAMIC_TRIPLE_BASE_CACHE, 1, NULL, &SetTripleBaseCache },
#endif

    { "Max_Threads", "SMP", UCISpin, 1, MaxCPUs, MaxCPUs, &OptMaxThreads, &InitSMP },
	{ "Min_Threads", "SMP", UCISpin, 1, MaxCPUs, 1, &OptMinThreads, &InitSMP }, // Added 5/22/2013 by Yuri Censor for Firenzina
    { "MultiPV", "System", UCISpin, 1, MAX_MULTIPV, DEFAULT_MULTIPV, &MultiPV, NULL },
// Split Depths	
    { "AN_Split_Depth", "SMP", UCISpin, MIN_AN_SPLIT_DEPTH, MAX_AN_SPLIT_DEPTH, DEFAULT_AN_SPLIT_DEPTH, &ANSplitDepth, NULL },
    { "CN_Split_Depth", "SMP", UCISpin, MIN_CN_SPLIT_DEPTH, MAX_CN_SPLIT_DEPTH, DEFAULT_CN_SPLIT_DEPTH, &CNSplitDepth, NULL },
    { "PV_Split_Depth", "SMP", UCISpin, MIN_PV_SPLIT_DEPTH, MAX_PV_SPLIT_DEPTH, DEFAULT_PV_SPLIT_DEPTH, &PVSplitDepth, NULL },
// Piece Values
    { "Pawn_Value", "Eval", UCISpin, 1, MAX_PAWN_VALUE, DEFAULT_PAWN_VALUE, &PValue, &InitMaterialValue },
    { "Knight_Value", "Eval", UCISpin, 1, MAX_KNIGHT_VALUE, DEFAULT_KNIGHT_VALUE, &NValue, &InitMaterialValue },
    { "Bishop_Value", "Eval", UCISpin, 1, MAX_BISHOP_VALUE, DEFAULT_BISHOP_VALUE, &BValue, &InitMaterialValue },
    { "Rook_Value", "Eval", UCISpin, 1, MAX_ROOK_VALUE, DEFAULT_ROOK_VALUE, &RValue, &InitMaterialValue },
    { "Queen_Value", "Eval", UCISpin, 1, MAX_QUEEN_VALUE, DEFAULT_QUEEN_VALUE, &QValue, &InitMaterialValue },
    { "Bishop_Pair_Value", "Eval", UCISpin, 1, MAX_BISHOP_PAIR_VALUE, DEFAULT_BISHOP_PAIR_VALUE, &BPValue, &InitMaterialValue },
// Eval Weights
    { "Draw_Weight", "Eval", UCISpin, 1, MAX_DRAW_WEIGHT, DEFAULT_DRAW_WEIGHT, &DrawWeight, NULL },
    { "King_Safety_Weight", "Eval", UCISpin, 1, MAX_KING_SAFETY_WEIGHT, DEFAULT_KING_SAFETY_WEIGHT, &KingSafetyWeight, NULL },
    { "Material_Weight", "Eval", UCISpin, 1, MAX_MATERIAL_WEIGHT, DEFAULT_MATERIAL_WEIGHT, &MaterialWeight, NULL },
    { "Mobility_Weight", "Eval", UCISpin, 1, MAX_MOBILITY_WEIGHT, DEFAULT_MOBILITY_WEIGHT, &MobilityWeight, NULL },
    { "Pawn_Weight", "Eval", UCISpin, 1, MAX_PAWN_WEIGHT, DEFAULT_PAWN_WEIGHT, &PawnWeight, &PawnHashReset },
    { "Positional_Weight", "Eval", UCISpin, 1, MAX_POSITIONAL_WEIGHT, DEFAULT_POSITIONAL_WEIGHT, &PositionalWeight, NULL },
    { "PST_Weight", "Eval", UCISpin, 1, MAX_PST_WEIGHT, DEFAULT_PST_WEIGHT, &PSTWeight, NULL },
// Lazy Eval
	{ "Lazy_Eval_Min", "Search", UCISpin, 1, MAX_LAZY_EVAL_MIN, DEFAULT_LAZY_EVAL_MIN, &LazyEvalMin, NULL },
	{ "Lazy_Eval_Max", "Search", UCISpin, 1, MAX_LAZY_EVAL_MAX, DEFAULT_LAZY_EVAL_MAX, &LazyEvalMax, NULL },
// Search Vars
    { "Aspiration_Window", "Search", UCISpin, 1, MAX_ASPIRATION_WINDOW, DEFAULT_ASPIRATION_WINDOW, &AspirationWindow, NULL },
    { "Count_Limit", "Search", UCISpin, 1, MAX_COUNT_LIMIT, DEFAULT_COUNT_LIMIT, &CountLimit, NULL },
    { "Delta_Cutoff", "Search", UCISpin, MIN_DELTA_CUTOFF, MAX_DELTA_CUTOFF, DEFAULT_DELTA_CUTOFF, &DeltaCutoff, NULL },
    { "Depth_Red_Min", "Search", UCISpin, 1, MAX_DEPTH_RED_MIN, DEFAULT_DEPTH_RED_MIN, &DepthRedMin, NULL },
    { "Height_Multiplier", "Search", UCISpin, 1, MAX_HEIGHT_MULTIPLIER, DEFAULT_HEIGHT_MULTIPLIER, &HeightMultiplier, NULL },
    { "History_Threshold", "Search", UCISpin, 1, MAX_HISTORY_THRESHOLD, DEFAULT_HISTORY_THRESHOLD, &HistoryThreshold, NULL },
    { "Low_Depth_Margin", "Search", UCISpin, 1, MAX_LOW_DEPTH_MARGIN, DEFAULT_LOW_DEPTH_MARGIN, &LowDepthMargin, NULL },
    { "Min_Depth_Multiplier", "Search", UCISpin, 1, MAX_MIN_DEPTH_MULTIPLIER, DEFAULT_MIN_DEPTH_MULTIPLIER, &MinDepthMultiplier, NULL },
    { "Min_Trans_Move_Depth", "Search", UCISpin, 1, MAX_MIN_TRANS_MOVE_DEPTH, DEFAULT_MIN_TRANS_MOVE_DEPTH, &MinTransMoveDepth, NULL },
	{ "Null_Reduction", "Search", UCISpin, 1, MAX_NULL_REDUCTION, DEFAULT_NULL_REDUCTION, &NullReduction, NULL },
    { "QS_Alpha_Threshold", "Search", UCISpin, 1, MAX_QS_ALPHA_THRESHOLD, DEFAULT_QS_ALPHA_THRESHOLD, &QSAlphaThreshold, NULL },
    { "Search_Depth_Min", "Search", UCISpin, 1, MAX_SEARCH_DEPTH_MIN, DEFAULT_SEARCH_DEPTH_MIN, &SearchDepthMin, NULL },
    { "Search_Depth_Reduction", "Search", UCISpin, 1, MAX_SEARCH_DEPTH_REDUCTION, DEFAULT_SEARCH_DEPTH_REDUCTION, &SearchDepthReduction, NULL },
    { "SEE_CutOff", "Search", UCISpin, 1, MAX_SEE_CUTOFF, DEFAULT_SEE_CUTOFF, &SEECutOff, NULL },
    { "SEE_Limit", "Search", UCISpin, 1, MAX_SEE_LIMIT, DEFAULT_SEE_LIMIT, &SEELimit, NULL },
    { "Top_Min_Depth", "Search", UCISpin, 1, MAX_TOP_MIN_DEPTH, DEFAULT_TOP_MIN_DEPTH, &TopMinDepth, NULL },
    { "Undo_Count_Threshold", "Search", UCISpin, 1, MAX_UNDO_COUNT_THRESHOLD, DEFAULT_UNDO_COUNT_THRESHOLD, &UndoCountThreshold, NULL },
	{ "Value_Cut", "Search", UCISpin, MIN_VALUE_CUT, MAX_VALUE_CUT, DEFAULT_VALUE_CUT, &ValueCut, NULL },
	{ "Verify_Reduction", "Search", UCISpin, 1, MAX_VERIFY_REDUCTION, DEFAULT_VERIFY_REDUCTION, &VerifyReduction, NULL },
// Prune Thresholds
    { "Prune_Check", "Search", UCISpin, 1, MAX_PRUNE_CHECK, DEFAULT_PRUNE_CHECK, &PruneCheck, NULL },
    { "Prune_Pawn", "Search", UCISpin, 1, MAX_PRUNE_PAWN, DEFAULT_PRUNE_PAWN, &PrunePawn, NULL },
    { "Prune_Minor", "Search", UCISpin, 1, MAX_PRUNE_MINOR, DEFAULT_PRUNE_MINOR, &PruneMinor, NULL },
    { "Prune_Rook", "Search", UCISpin, 1, MAX_PRUNE_ROOK, DEFAULT_PRUNE_ROOK, &PruneRook, NULL },
// Time Management
    { "Absolute_Factor", "Time", UCISpin, 1, MAX_ABSOLUTE_FACTOR, DEFAULT_ABSOLUTE_FACTOR, &AbsoluteFactor, NULL },
    { "Battle_Factor", "Time", UCISpin, 1, MAX_BATTLE_FACTOR, DEFAULT_BATTLE_FACTOR, &BattleFactor, NULL },
    { "Desired_Millis ", "Time", UCISpin, 1, MAX_DESIRED_MILLIS, DEFAULT_DESIRED_MILLIS, &DesiredMillis, NULL },
    { "Easy_Factor", "Time", UCISpin, 1, MAX_EASY_FACTOR, DEFAULT_EASY_FACTOR, &EasyFactor, NULL },
    { "Easy_Factor_Ponder", "Time", UCISpin, 1, MAX_EASY_FACTOR_PONDER, DEFAULT_EASY_FACTOR_PONDER, &EasyFactorPonder, NULL },
    { "Normal_Factor", "Time", UCISpin, 1, MAX_NORMAL_FACTOR, DEFAULT_NORMAL_FACTOR, &NormalFactor, NULL},
//UCI Info Strings
    { "CPU_Load_Info", "Info", UCICheck, 0, 0, false, &CPULoadInfo, NULL },
    { "Current_Move_Info", "Info", UCICheck, 0, 0, false, &CurrMoveInfo, NULL },
    { "Depth_Info", "Info", UCICheck, 0, 0, false, &DepthInfo, NULL },
    { "Hash_Full_Info", "Info", UCICheck, 0, 0, false, &HashFullInfo, NULL },
    { "Low_Depth_PVs", "Info", UCICheck, 0, 0, false, &LowDepthPVs, NULL },
    { "NPS_Info", "Info", UCICheck, 0, 0, false, &NPSInfo, NULL },

#ifdef RobboBases
    { "TBHit_Info", "Info", UCICheck, 0, 0, false, &TBHitInfo, NULL },
#endif

    { "Extend_In_Check", "Search", UCICheck, 0, 0, false, &ExtendInCheck, NULL },
    { "Ponder", "System", UCICheck, 0, 0, false, &Ponder, NULL },
    { "Split_at_CN", "SMP", UCICheck, 0, 0, true, &SplitAtCN, NULL },
    { "Verbose_UCI", "Info", UCICheck, 0, 0, false, &VerboseUCI, NULL },

#ifdef RobboBases
	{ "Verbose_RobboBases", "RobboBases", UCICheck, 0, 0, false, &VerboseRobboBases, NULL },
#endif

    { "Verify_Null", "Search", UCICheck, 0, 0, true, &VerifyNull, NULL },

#ifdef Log
    { "Write_Log", "System", UCICheck, 0, 0, false, &WriteLog, &create_log },
#endif

//RobboBases
#ifdef RobboBases
	{ "Use_RobboBases", "RobboBases", UCICheck, 0, 0, false, &UseRobboBases, NULL },
	{ "TripleBase_Directory", "RobboBases", UCIString, 0, 0, 0, TripleDir, &LoadRobboTripleBases },
	{ "TotalBase_Directory", "RobboBases", UCIString, 0, 0, 0, TotalDir, &RegisterRobboTotalBases },
	{ "Bulk_Load_Directory", "RobboBases", UCIString, 0, 0, 0, BulkDirectory, &RobboBulkDirectory },
#endif
    { "Clear_Hash", "Other", UCIButton,  0, 0, 0, NULL, &HashClearAll },
    { "Get_FEN_String", "Other", UCIButton,  0, 0, 0, NULL, &UCIFen },
    { "", "", -1, 0, 0, false, NULL, NULL }
    };

UCItype BaseUCIOptions[256] =
    {
// System
    { "Hash", "System", UCISpin, 1, MAX_HASH_SIZE, DEFAULT_HASH_SIZE, &CurrentHashSize, &InitHash },
    { "Pawn_Hash", "System", UCISpin, 1, MAX_PAWN_HASH_SIZE, DEFAULT_PAWN_HASH_SIZE, &CurrentPHashSize, &InitPawnHashWrapper },
    { "Max_Threads", "SMP", UCISpin, 1, MaxCPUs, MaxCPUs, &OptMaxThreads, &InitSMP },
	{ "Min_Threads", "SMP", UCISpin, 1, MaxCPUs, 1, &OptMinThreads, &InitSMP }, // Added 5/22/2013 by Yuri Censor for Firenzina
    { "MultiPV", "System", UCISpin, 1, MAX_MULTIPV, DEFAULT_MULTIPV, &MultiPV, NULL },

#ifdef FischerRandom
    { "UCI_Chess960", "Other", UCICheck, 0, 0, false, &Chess960, NULL },
#endif

    { "", "", -1, 0, 0, false, NULL, NULL }
    };

static void uci()
    {
    UCItype *uci;
    char string[256];
    char *str;
    char TYPE[5][16] =
        {
		"spin", "check", "button", "string", "combo"
        };
	Send("id name %s %s %s\n", Engine, Vers, Plat);
    Send("id author %s\n", Author);

#ifdef Log
	if (WriteLog)
		{
		log_file = fopen(log_filename, "a");
		fprintf(log_file, "id name %s %s %s\n", Engine, Vers, Plat);
		fprintf(log_file, "id author %s\n", Author);
		close_log();
		}
#endif

#ifdef InitCFG
	if (!CfgFound)
		{
#endif

		for (uci = UCIOptions; uci->type != -1; uci++)
			{
			str = string;
			sprintf(str, "option name %s type %s", uci->name, TYPE[uci->type]);
			str += strlen(str);
			if (uci->type == UCICheck)
				sprintf(str, " default %s", uci->def ? "true" : "false");
			else if (uci->type == UCIString)
				sprintf(str, " default NULL");
			else if (uci->type == UCISpin)
				sprintf(str, " min %d max %d default %d", uci->min, uci->max, uci->def);
			else if (uci->type == UCICombo)
				{
				int i;
				for (i = 0; i < uci->max; i++)
					{
					sprintf(str, " var %s", Combo[uci->min][i]);
					str += strlen(str);
					}
				sprintf(str, " default %s", Combo[uci->min][uci->def]);
				}
			str += strlen(str);
			sprintf(str, "\n");
			Send("%s", string);

	#ifdef Log
			if (WriteLog)
				{
				log_file = fopen(log_filename, "a");
				fprintf(log_file, "%s", string);
				close_log();
				}
	#endif

			}

#ifdef InitCFG
		}
	else
		{
		for (uci = BaseUCIOptions; uci->type != -1; uci++)
			{
			str = string;
			sprintf(str, "option name %s type %s", uci->name, TYPE[uci->type]);
			str += strlen(str);
			if (uci->type == UCICheck)
				sprintf(str, " default %s", uci->def ? "true" : "false");
			else if (uci->type == UCIString)
				sprintf(str, " default NULL");
			else if (uci->type == UCISpin)
				sprintf(str, " min %d max %d default %d", uci->min, uci->max, uci->def);
			else if (uci->type == UCICombo)
				{
				int i;
				for (i = 0; i < uci->max; i++)
					{
					sprintf(str, " var %s", Combo[uci->min][i]);
					str += strlen(str);
					}
				sprintf(str, " default %s", Combo[uci->min][uci->def]);
				}
			str += strlen(str);
			sprintf(str, "\n");
			Send("%s", string);

	#ifdef Log
			if (WriteLog)
				{
				log_file = fopen(log_filename, "a");
				fprintf(log_file, "%s", string);
				close_log();
				}
	#endif

			}
		}
#endif

    Send("uciok\n");

#ifdef Log
		if (WriteLog)
			{
			log_file = fopen(log_filename, "a");
			fprintf(log_file, "uciok\n");
			close_log();
			}
#endif

    }
static void SetOption(char *string)
    {
    UCItype *uci;
    char *v;
    char *u;
    int Value, r;
    while (string[0] == ' ' || string[0] == '\t')
        string++;
    r = strlen(string) - 1;
    while (string[r] == ' ' || string[r] == '\t')
        string[r--] = 0;
    u = strstr(string, "value");
    if (u)
        {
        while ((*u) == ' ' || (*u) == '\t')
            u--;
        }
    else
        u = string + strlen(string);
    for (uci = UCIOptions; uci->type != -1; uci++)
        {
        if (!memcmp(uci->name, string, u - string - 1))
            {
            if (uci->type == UCIButton)
                {
                Value = uci->action(true);
				if (VerboseUCI)
					Send ("info string Button %s pushed %s\n", uci->name, Value ? "true" : "false");
                return;
                }
            v = strstr(string, "value");
            if (!v)
                return;
            if (uci->type == UCIString)
                {
                strcpy((char *)(uci->var), v + 6);
				if (VerboseUCI)
					Send("info string Option %s %s\n", uci->name, (char*) (uci->var));

#ifdef Log
				if (WriteLog)
					{
					log_file = fopen(log_filename, "a");
					fprintf(log_file, "info string Option %s %s\n", uci->name, (char*) (uci->var));
					close_log();
					}
#endif

                if (uci->action)
                    (uci->action)(true);
                }
            else if (uci->type == UCICheck)
                {
                if (!strcmp(v + 6, "false"))
                    {
                    if (* ((bool *)(uci->var)) == false)
                        {
						if (VerboseUCI)
							Send ("info string Option %s %s\n", uci->name, "false");

#ifdef Log
						if (WriteLog)
							{
							log_file = fopen(log_filename, "a");
							fprintf(log_file, "info string Option %s %s\n", uci->name, "false");
							close_log();
							}
#endif

                        return;
                        }
                    * ((bool *)(uci->var)) = false;
                    if (uci->action)
                        uci->action(false);
                    }
                if (!strcmp(v + 6, "true"))
                    {
                    if (* ((bool *)(uci->var)) == true)
                        {
						if (VerboseUCI)
							Send ("info string Option %s %s\n", uci->name, "true");

#ifdef Log
						if (WriteLog)
							{
							log_file = fopen(log_filename, "a");
							fprintf(log_file, "info string Option %s %s\n", uci->name, "true");
							close_log();
							}
#endif

                        return;
                        }
                    * ((bool *)(uci->var)) = true;
                    if (uci->action)
                        uci->action(true);
                    }
				if (VerboseUCI)
					Send ("info string Option %s %s\n", uci->name, (* ((bool*) (uci->var))) ? "true" : "false");

#ifdef Log
				if (WriteLog)
					{
					log_file = fopen(log_filename, "a");
					fprintf(log_file, "info string Option %s %s\n", uci->name,(*((bool*) (uci->var))) ? "true" : "false");
					close_log();
					}
#endif

                }
            else if (uci->type == UCISpin)
                {
                Value = atoi(v + 6);
                if (Value < uci->min)
                    Value = uci->min;
                if (Value > uci->max)
                    Value = uci->max;
                if (uci->var)
                    {
                    if (* ((sint32 *)(uci->var)) == Value)
                        {
						if (VerboseUCI)
							Send ("info string Option %s %d\n", uci->name, Value);

#ifdef Log
						if (WriteLog)
							{
							log_file = fopen(log_filename, "a");
							fprintf(log_file, "info string Option %s %d\n", uci->name, Value);
							close_log();
							}
#endif

                        return;
                        }
                    * ((sint32 *)(uci->var)) = Value;
                    }
                if (uci->action)
                    {
                    r = uci->action(Value);
                    if (r)
                        Value = r;
                    }
				if (VerboseUCI)
					Send ("info string Option %s %d\n", uci->name, Value);

#ifdef Log
				if (WriteLog)
					{
					log_file = fopen(log_filename, "a");
					fprintf(log_file, "info string Option %s %d\n", uci->name, Value);
					close_log();
					}
#endif

                }
            else if (uci->type == UCICombo)
                {
                strcpy((char *)(uci->var), v + 6);
				if (VerboseUCI)
					Send ("info string Option %s %s\n", uci->name, (char*) (uci->var));

#ifdef Log
				if (WriteLog)
					{
					log_file = fopen(log_filename, "a");
					fprintf(log_file, "info string Option %s %s\n", uci->name, (char*) (uci->var));
					close_log();
					}
#endif

                if (uci->action)
                    (uci->action)(true);
                }
            return;
            }
        }
	if (VerboseUCI)
		Send ("info string Option unknown: %s\n", string);

#ifdef Log
	if (WriteLog)
		{
		log_file = fopen(log_filename, "a");
		fprintf(log_file, "info string Option unknown: %s\n", string);
		close_log();
		}
#endif

    }

void SharedDelete()
    {
    Lock(InputDelete);
    if (HasDeletion)
        {
        UnLock(InputDelete);
        return;
        }
    QuitPV();
    QuitLargePawns();
    DetachAllMemory();
    QuitEvalHash();

#ifdef RobboBases
    QuitTripleHash();
#endif

#ifdef SlabMemory
    DeleteSlab();
    HasDeletion = true;
#endif

    UnLock(InputDelete);
    }
static void Quit()
    {
    SharedDelete();
    exit(0);
    }

static void ParseInput(typePos *Position, char *I)
    {
    if (!Init)
        {
        Init = true;
        LockInit(InputDelete);
        }
    UCINewGame = false;
    if (!strcmp(I, "quit"))
        {
        QuitDemand = true;
        ParseInput(Position, "stop");
        Quit();
        }
    else if (!strcmp(I, "stop"))
        {
        if (SearchIsDone)
            return;
        HaltSearch(0, 0);
        if (InfiniteLoop)
            return;
        StallMode = true;
        while (!SearchIsDone)
            NanoSleep(1000000);
        StallMode = false;
        SuppressInput = false;
        return;
        }
    else if (!strcmp(I, "isready"))
        {
		Send ("readyok\n");

#ifdef Log
	if (WriteLog)
		{
		log_file = fopen(log_filename, "a");
		fprintf(log_file, "readyok\n");
		close_log();
		}
#endif

        return;
        }
    if (!strcmp(I, "stall"))
        {
        StallInput = true;
        return;
        }
    else if (!strcmp(I, "ponderhit"))
        {
        PonderHit();
        return;
        }
    else if (!strcmp(I, "ucinewgame"))
        {
        if (SearchIsDone)
            {
            NewGame(Position, true);
            return;
            }
        UCINewGame = true;
        HaltSearch(0, 0);
        if (InfiniteLoop)
            return;
        StallMode = true;
        while (!SearchIsDone)
            NanoSleep(1000000);
        StallMode = false;
        SuppressInput = false;
        return;
        }
    if (JumpIsSet)
        return;

#ifdef Bench
    if (!strcmp(I, "bench"))
        BenchMark(Position, "go movetime 1000"); // Returned 5/22/2013. Since 4/13/2013 was: "go depth 10"
    else if (!memcmp(I, "benchmark", 9))
        BenchMark(Position, I + 10);

	if (!strcmp(I, "sd-bench"))
        SDBenchMark(Position, "go movetime 1000"); // Returned 5/22/2013. Since 4/13/2013 was: "go depth 10"
    else if (!memcmp(I, "benchmark", 9))
        SDBenchMark(Position, I + 10);
#endif

#ifdef RobboBases
    if (!memcmp(I, "mainline", 8))
        MainLine(Position);
#endif

    if (!memcmp(I, "go", 2))
        {
        InitSearch(Position, I);
        if (BoardIsOk)
            Search(Position);
        }
    else if (!memcmp(I, "position", 8))
        InitPosition(Position, I + 9);
    else if (!memcmp(I, "setoption name", 14))
        SetOption(I + 15);
    else if (!strcmp(I, "uci"))
        uci();
		
#ifdef InitCFG
    if (!strcmp(I, "default"))
		gen_def_cfg_file("fire.cfg");
    if (!strcmp(I, "current"))
		gen_cur_cfg_file();
#endif	
	
    SuppressInput = false;
    }

#if defined(_WIN32) || defined(_WIN64)
#include <io.h>
#include <conio.h>
void Input(typePos * Position)
    {
    int r = 0;
    if (SuppressInput)
        return;
    if (QuitDemand)
        ParseInput(Position, "quit");
    if (_kbhit())
        {
        InputBuffer[r] = (char)getche();
        while (InputBuffer[r++] != '\r')
            InputBuffer[r] = (char)getche();
        }
    else
        {
        _read(_fileno(stdin), InputBuffer, 1);
        while (InputBuffer[r++] != '\n')
            _read(_fileno(stdin), InputBuffer + r, 1);
        }
    InputBuffer[r - 1] = 0;
    InputBuffer[r] = 0;

#ifdef Log
	if (WriteLog)
		{
		log_file = fopen(log_filename, "a");
		fprintf(log_file, InputBuffer);
		fprintf(log_file, "\n");
		close_log();
		}
#endif

    ParseInput(Position, InputBuffer);
    if (QuitDemand)
        ParseInput(Position, "quit");
    }
#else
#include <unistd.h>
void Input(typePos *Position)
    {
    int i, r = 0;
    if (SupressInput)
        return;
    if (QuitDemand)
        ParseInput(Position, "quit");
    read(STDIN_FileNO, InputBuffer, 1);
    ReadMe:
    while (InputBuffer[r++] != '\n')
        read(STDIN_FileNO, InputBuffer + r, 1);
    if (InputBuffer[r - 2] == '\\')
        {
        r -= 3;
        goto ReadMe;
        }
    InputBuffer[r - 1] = 0;
    InputBuffer[r] = 0;
    ParseInput(Position, InputBuffer);
    if (QuitDemand)
        ParseInput(Position, "quit");
    }
#endif
