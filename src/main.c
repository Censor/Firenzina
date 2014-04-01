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

void ResetPositionalGain()
    {
    int p, m;

#ifdef MultiplePosGain
    typePos Position[1];
    for (Position->cpu = 0; Position->cpu < MaxCPUs; Position->cpu++)
#endif

	for (p = 0; p < 0x10; p++)
		for (m = 0; m < 010000; m++)
			MaxPosGain(p, m) = 0;
    }

void ResetHistory()
    {
    int pi, sq;

#ifdef MultipleHistory
    typePos Position[1];
    for (Position->cpu = 0; Position->cpu < MaxCPUs; Position->cpu++)
#endif

	for (pi = 0; pi < 0x10; pi++)
		for (sq = A1; sq <= H8; sq++)
			HistoryPITo(Position, pi, sq) = 0x800;
    }

void InitHashTables()
    {
    PVHashTable = NULL;
    EvalHash = NULL;
    PawnHash = NULL;
    CurrentPHashSize = (1 << 16);
    EvalHashSize = (1 << 15);
	
#ifdef RobboBases
    TripleHash = NULL;
    TripleHashSize = (1 << 17);
#endif

	OptHashSize = DEFAULT_HASH_SIZE;
	OptPHashSize = DEFAULT_PAWN_HASH_SIZE;
    }

void InitRootPosition()
    {
    memset((void *)RootPosition0, 0, sizeof(typePos));
    RootPosition0->DynRoot = malloc(MaxPly * sizeof(typeDynamic));
    RootPosition0->Dyn = RootPosition0->DynRoot + 1;
    RootPosition0->stop = false;
    }

void InitGlobals()
    {
// Eval Weights
    DrawWeight = DEFAULT_DRAW_WEIGHT;
    KingSafetyWeight = DEFAULT_KING_SAFETY_WEIGHT; 
    MaterialWeight = DEFAULT_MATERIAL_WEIGHT;
    MobilityWeight = DEFAULT_MOBILITY_WEIGHT; 
    PawnWeight = DEFAULT_PAWN_WEIGHT;     
	PositionalWeight = DEFAULT_POSITIONAL_WEIGHT; 
    PSTWeight = DEFAULT_PST_WEIGHT;

// Lazy Eval
	LazyEvalMin = DEFAULT_LAZY_EVAL_MIN;
	LazyEvalMax = DEFAULT_LAZY_EVAL_MAX;
	
// Piece Values
	PValue = DEFAULT_PAWN_VALUE;
	NValue = DEFAULT_KNIGHT_VALUE;
	BValue = DEFAULT_BISHOP_VALUE;
	RValue = DEFAULT_ROOK_VALUE;
	QValue = DEFAULT_QUEEN_VALUE;
	BPValue = DEFAULT_BISHOP_PAIR_VALUE;
	
// Prune Thresholds
	PruneCheck = DEFAULT_PRUNE_CHECK;
	PrunePawn = DEFAULT_PRUNE_PAWN;
	PruneMinor = DEFAULT_PRUNE_MINOR;
	PruneRook = DEFAULT_PRUNE_ROOK;
	
// Search Vars
	AspirationWindow = DEFAULT_ASPIRATION_WINDOW; 
	CountLimit = DEFAULT_COUNT_LIMIT; 
	DeltaCutoff = DEFAULT_DELTA_CUTOFF;
	DepthRedMin = DEFAULT_DEPTH_RED_MIN;
	ExtendInCheck = false;
	HeightMultiplier = DEFAULT_HEIGHT_MULTIPLIER;
	HistoryThreshold = DEFAULT_HISTORY_THRESHOLD;
	LowDepthMargin = DEFAULT_LOW_DEPTH_MARGIN; 
	MinDepthMultiplier = DEFAULT_MIN_DEPTH_MULTIPLIER;
	MinTransMoveDepth = DEFAULT_MIN_TRANS_MOVE_DEPTH;
	NullReduction = DEFAULT_NULL_REDUCTION;
	QSAlphaThreshold = DEFAULT_QS_ALPHA_THRESHOLD;
	SearchDepthMin = DEFAULT_SEARCH_DEPTH_MIN; 
	SearchDepthReduction = DEFAULT_SEARCH_DEPTH_REDUCTION; 
	SEECutOff = DEFAULT_SEE_CUTOFF;
	SEELimit = DEFAULT_SEE_LIMIT;
	TopMinDepth = DEFAULT_TOP_MIN_DEPTH; 
	UndoCountThreshold = DEFAULT_UNDO_COUNT_THRESHOLD; 
	ValueCut = DEFAULT_VALUE_CUT;
	VerifyNull = true;
	VerifyReduction = DEFAULT_VERIFY_REDUCTION; 

// Split Depths
	SplitAtCN = true;
	ANSplitDepth = DEFAULT_AN_SPLIT_DEPTH; 
	CNSplitDepth = DEFAULT_CN_SPLIT_DEPTH; 
	PVSplitDepth = DEFAULT_PV_SPLIT_DEPTH;

// Time Management
	AbsoluteFactor = DEFAULT_ABSOLUTE_FACTOR; 
	BattleFactor = DEFAULT_BATTLE_FACTOR; 
	EasyFactor = DEFAULT_EASY_FACTOR; 
	EasyFactorPonder = DEFAULT_EASY_FACTOR_PONDER;
	NormalFactor = DEFAULT_NORMAL_FACTOR;
	DesiredMillis = DEFAULT_DESIRED_MILLIS;

//UCI Info Strings
	CPULoadInfo = false;
	CurrMoveInfo = false;
	DepthInfo = false;
    HashFullInfo = false;	
	LowDepthPVs = false;
	NPSInfo = false;
	VerboseUCI = false;
	MinPVDepth = DEFAULT_MIN_PV_DEPTH;	
	
#ifdef RobboBases
	TBHitInfo = false;
#endif

//Miscellaneous

#ifdef Bench
	BenchMarking = false;
#endif

	CfgFile = 0;
	CfgFound = false;

#ifdef FischerRandom
	Chess960 = false;
#endif

    InfiniteLoop = false;
    MultiPV = DEFAULT_MULTIPV;
    NumThreads = 1;
    OptMaxThreads = MaxCPUs;
	OptMinThreads = 1; // Added 5/22/2013 by Yuri Censor for Firenzina
    Ponder = false;
	RandRange = 0;
    SearchIsDone = true;
    SMPisActive = false;
    StallMode = false;
    StallInput = false;
    SuppressInput = false;
	UCINewGame = false;

#ifdef Log
	WriteLog = false;
#endif

//RobboBases
#ifdef RobboBases
	AutoloadTotalBases = false;
	AutoloadTripleBases = false;
	TotalBaseCache = 1;	
	TripleBaseHash = 1;	
	DynamicTripleBaseCache = 1;
	UseRobboBases = false;
	VerboseRobboBases = false;
	strcpy(TripleDir, "");
	strcpy(TotalDir, "");
    TripleBasesLoaded = false;
    TotalBasesLoaded = false;	
    SearchRobboBases = true;	
#endif
    }

int main()
    {
	ShowBanner();
	SetPOPCNT();
	InitHashTables();
	InitRootPosition();
	InitGlobals();
    RPInit();

#ifdef InitCFG
    read_cfg_file("fire.cfg");
#endif

    CurrentHashSize = OptHashSize;
	CurrentPHashSize = OptPHashSize;
    InitHash(CurrentHashSize);
	
#ifdef RobboBases
    InitTripleHash(TripleBaseHash);
#endif

    InitCaptureValues();
    InitPawns();
    NewGame(RootPosition0, true);
    InputBuffer = malloc(65536);
    input_ptr = InputBuffer;
	
#ifdef RobboBases
    SearchRobboBases = true;
    TotalInit();
	if (UseRobboBases)
		{
        if (AutoloadTotalBases) 
            RegisterRobboTotalBases();
		InitTotalBaseCache(TotalBaseCache);
		if (AutoloadTripleBases) 
			LoadRobboTripleBases();
		}
#endif

// Commented out by JA:
//#ifdef LinuxLargePages
  //  LinuxHandleSignals();
//#endif
			
	InitSMP();
    while (1)
        Input(RootPosition0);
    return 0;
    }
