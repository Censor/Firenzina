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

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#define Engine "Firenzina"
#define Vers "2.3 xTreme"
#define Author "Yuri Censor and ZirconiumX, a clone of Fire 2.2 xTreme by Kranium"
#if defined(__GNUC__) 
#define STDIN_FileNO 0

#if defined(__i386__)
#define Platform "Linux 32"
#else
#define Platform "Linux 64"
//#define LinuxLargePages
#endif

#elif  defined(_WIN64) 
#define Platform "x64"
#define LargePages
#if defined(__GNUC__)
#define INLINE inline
#endif

#else
#define Platform "w32"
#define LargePages
#if defined(__GNUC__)
#define INLINE inline
#endif
#endif

#define true 1
#define false 0
#define bool int

#define InitCFG

#define Log
#define MatFactors
#define MultiplePosGain
#define MultipleHistory
#define OneDimensional

#define SlabMemory

#ifdef SlabMemory
#ifndef OneDimensional
#error SlabMemory requires OneDimensional
#endif
#define SlabDeclare1(Type, v, c) Type v[c]
#define SlabAlloc1(Type, v, c)
#define SlabDeclare2(Type, v, c) Type *v
#define SlabAlloc2(Type, v, c) v = FromSlab ((c) * sizeof (Type))
#define SlabDeclare3 SlabDeclare1
#define SlabAlloc3 SlabAlloc1
#else
#define SlabDeclare1(Type, v, c) Type v[c]
#define SlabAlloc1(Type, v, c)
#define SlabDeclare2(Type, v, c) Type v[c]
#define SlabAlloc2(Type, v, c)
#define SlabDeclare3 SlabDeclare1
#define SlabAlloc3 SlabAlloc1
#endif

//*******************************************************************************//
// Define default settings here (comment by Yuri Censor for Firenzina, 3/25/2013)//
//*******************************************************************************//
// System
#define MaxCPUs 64
#define RPperCPU 8
#define MaxSP 16
#define DEFAULT_HASH_SIZE 128
#define MAX_HASH_SIZE 65536
#define DEFAULT_PAWN_HASH_SIZE 32
#define MAX_PAWN_HASH_SIZE 1024
#define DEFAULT_MULTIPV 1
#define MAX_MULTIPV 250
#define DEFAULT_MIN_PV_DEPTH 15

// Split Depths
#define MIN_AN_SPLIT_DEPTH 8             // Original: 12
#define DEFAULT_AN_SPLIT_DEPTH 14        // Original: 12; Conservative: 12; Aggressive: 14
#define MAX_AN_SPLIT_DEPTH 24
#define MIN_CN_SPLIT_DEPTH 8             // Original: 12
#define DEFAULT_CN_SPLIT_DEPTH 16        // Original: 14; Conservative: 14; Aggressive: 16
#define MAX_CN_SPLIT_DEPTH 24
#define MIN_PV_SPLIT_DEPTH 8             // Original: 12
#define DEFAULT_PV_SPLIT_DEPTH 12        // Original: 12
#define MAX_PV_SPLIT_DEPTH 24

// Piece Values
#define DEFAULT_PAWN_VALUE 100           // Original: 100; DO NOT ALTER! IT SETS THE SCALE. Yuri Censor, 03/25/2013
#define MAX_PAWN_VALUE 200
#define DEFAULT_KNIGHT_VALUE 317         // Original: 320
#define MAX_KNIGHT_VALUE 640
#define DEFAULT_BISHOP_VALUE 333         // Original: 330
#define MAX_BISHOP_VALUE 640
#define DEFAULT_ROOK_VALUE 520           // Original: 510
#define MAX_ROOK_VALUE 1000
#define DEFAULT_QUEEN_VALUE 960          // Original: 1000
#define MAX_QUEEN_VALUE 2000
#define DEFAULT_BISHOP_PAIR_VALUE 50     // Original: 50
#define MAX_BISHOP_PAIR_VALUE 200

// Eval Weights
#define DEFAULT_DRAW_WEIGHT 100          // Original: 100
#define MAX_DRAW_WEIGHT 200
#define DEFAULT_KING_SAFETY_WEIGHT 120   // Original: 100; Conservative: 130; Aggressive: 120
#define MAX_KING_SAFETY_WEIGHT 200
#define DEFAULT_MATERIAL_WEIGHT 100      // Original: 100
#define MAX_MATERIAL_WEIGHT 200
#define DEFAULT_MOBILITY_WEIGHT 130      // Original: 100; Conservative: 115; Aggressive: 130
#define MAX_MOBILITY_WEIGHT 200
#define DEFAULT_PAWN_WEIGHT 100          // Original: 100; DO NOT ALTER! IT SETS THE SCALE. Yuri Censor, 03/25/2013
#define MAX_PAWN_WEIGHT 200
#define DEFAULT_POSITIONAL_WEIGHT 110    // Original: 100; Conservative: 120; Aggressive: 110
#define MAX_POSITIONAL_WEIGHT 200
#define DEFAULT_PST_WEIGHT 100           // Original: 100
#define MAX_PST_WEIGHT 200

// Lazy Eval
#define DEFAULT_LAZY_EVAL_MIN 70        // Original: 150; Conservative: 90; Aggressive 70
#define MAX_LAZY_EVAL_MIN 300
#define DEFAULT_LAZY_EVAL_MAX 300
#define MAX_LAZY_EVAL_MAX 600

// Search Vars
#define DEFAULT_ASPIRATION_WINDOW 6      // Original: 8; Conservative: 8; Aggressive: 6
#define MAX_ASPIRATION_WINDOW 100
#define MIN_DELTA_CUTOFF 20000
#define DEFAULT_DELTA_CUTOFF 25000
#define MAX_DELTA_CUTOFF 30000
#define DEFAULT_DEPTH_RED_MIN 12
#define MAX_DEPTH_RED_MIN 24
#define DEFAULT_HEIGHT_MULTIPLIER 64
#define MAX_HEIGHT_MULTIPLIER 128
#define DEFAULT_HISTORY_THRESHOLD 50
#define MAX_HISTORY_THRESHOLD 100
#define DEFAULT_LOW_DEPTH_MARGIN 1200    // Original: 1125; Conservative: 1150; Aggressive: 1200
#define MAX_LOW_DEPTH_MARGIN 2000
#define DEFAULT_MIN_DEPTH_MULTIPLIER 48
#define MAX_MIN_DEPTH_MULTIPLIER 96
#define DEFAULT_MIN_TRANS_MOVE_DEPTH 16
#define MAX_MIN_TRANS_MOVE_DEPTH 32
#define DEFAULT_NULL_REDUCTION 8
#define MAX_NULL_REDUCTION 16

// Prune Thresholds
#define DEFAULT_PRUNE_CHECK 10
#define MAX_PRUNE_CHECK 30
#define DEFAULT_PRUNE_PAWN 160
#define MAX_PRUNE_PAWN 320
#define DEFAULT_PRUNE_MINOR 500
#define MAX_PRUNE_MINOR 1000
#define DEFAULT_PRUNE_ROOK 800
#define MAX_PRUNE_ROOK 1600

// More Search Vars
#define DEFAULT_QS_ALPHA_THRESHOLD 200
#define MAX_QS_ALPHA_THRESHOLD 400
#define DEFAULT_SEARCH_DEPTH_MIN 22      // Original: 20; Conservative: 22; Aggressive: 22
#define MAX_SEARCH_DEPTH_MIN 40
#define DEFAULT_SEARCH_DEPTH_REDUCTION 8 // Original: 6; Conservative: 6; Aggressive: 8
#define MAX_SEARCH_DEPTH_REDUCTION 12
#define DEFAULT_TOP_MIN_DEPTH 16         // Original: 14; Conservative: 14; Aggressive: 16
#define MAX_TOP_MIN_DEPTH 28
#define DEFAULT_UNDO_COUNT_THRESHOLD 17  // Original: 15; Conservative: 15; Aggressive: 17
#define MAX_UNDO_COUNT_THRESHOLD 20
#define MIN_VALUE_CUT 1000
#define DEFAULT_VALUE_CUT 15000
#define MAX_VALUE_CUT 30000
#define DEFAULT_VERIFY_REDUCTION 1       // Original: 2; Conservative: 3; Aggressive: 1
#define MAX_VERIFY_REDUCTION 16

// Time Management
#define DEFAULT_ABSOLUTE_FACTOR 30       // Original: 25; Conservative: 25; Aggressive: 30
#define MAX_ABSOLUTE_FACTOR 100
#define DEFAULT_BATTLE_FACTOR   75       // Original: 100; Conservative: 95; Aggressive: 75
#define MAX_BATTLE_FACTOR 200
#define DEFAULT_EASY_FACTOR 20           // Original: 15; Conservative: 15; Aggressive: 20
#define MAX_EASY_FACTOR 100
#define DEFAULT_EASY_FACTOR_PONDER 33    // Original: 33
#define MAX_EASY_FACTOR_PONDER 100
#define DEFAULT_NORMAL_FACTOR 50         // Original: 75; Conservative: 80; Aggressive: 50
#define MAX_NORMAL_FACTOR 200

// RobboBases
#define MAX_TOTAL_BASE_CACHE 1024
#define MAX_TRIPLE_BASE_HASH 4096
#define MAX_DYNAMIC_TRIPLE_BASE_CACHE 65536

#define MaxCPUs 64
#define RPperCPU 8
#define MaxSP 16

#include "win-linux.h"
#include "hash.h"

int NumCPUs;
uint64 NodeCheck;
#define ZobRev(Pos) (0ULL)
#define CheckHalt() { if (Position->stop) { return(0); } }
#define Height(x)((x)->height)
#define Is_Exact(x) (x)
#include "move.h"
bool BoardIsOk, isNewGame;

#define CheckRepetition(in_check)                                             \
	CheckHalt ();                                                              \
	if (in_check && Position->Dyn->reversible == 100) return Move50 (Position); \
	if (Position->Dyn->reversible >= 100) return(0);                           \
	for (i = 4; i <= Position->Dyn->reversible &&                               \
		i <= Position->StackHeight; i += 2)                                  \
	if (Position->Stack[Position->StackHeight - i] ==                         \
		(Position->Dyn->Hash ^ ZobRev (Position))) return(0);

typedef struct
    {
    uint64 Hash, PawnHash;
    uint32 material;
    sint32 Static;
    uint8 age, _9, wKdanger, bKdanger;
    uint8 oo, reversible, ep, cp;
    uint64 wAtt, bAtt, wXray, bXray;
    sint32 Value, PositionalValue;
    uint16 _5, _6, killer1, killer2, move;
    uint8 _0, _3, exact, lazy, SavedFlags, flags;
    uint64 wKcheck, bKcheck, _1, _2, _8;
    } typeDynamic;

#include "SMP.h"
#include "board.h"
#include "slab_memory.h"

#define MAX(x, y) (((x) >= (y)) ? (x) : (y))
#define MIN(x, y) (((x) <= (y)) ? (x) : (y))
#define ABS(x) (((x) >= 0) ? (x) : -(x))
#define FileDistance(x, y) (ABS(File(x) - File(y)))
#define RankDistance(x, y) (ABS(Rank(x) - Rank(y)))
#define DepthRed (MIN (12, depth / 2))

#define ValueMate 30000
#define ValueInfinity 32750
#define CountLimit 5
#define MultiCentiPawnPV 65535
#define RandomCount 0
#define RandomBits 1

#define DesiredMillis 40
#define MaxDepth 256

#ifdef RobboBases
#define TripleWeakProbeDepth -10
#define TripleDefiniteProbeDepth 40
#define TripleDefiniteProbeHeight 5
#define LoadOnWeakProbe true
#endif

#define ScoreReduction(x)(((uint32) (MIN ((x), 96))) >> 5)
#define KingDangerAdjust(wK, bK) (0)

#define BitClear(b, B) B &= (B - 1)
#define BitSet(b, B) B |=((uint64) 1) << (b)

// System
volatile bool DoPonder;
volatile bool Ponder_Hit;
volatile bool DoInfinite;
volatile bool SuppressInput;
volatile bool Stop;
volatile bool UCINewGame;

bool Ponder;
bool CfgFound;
bool DoOutput;
bool StallInput;
bool NoSupport;

int CfgFile;
int MultiPV;
int OptHashSize;
int OptPHashSize;
int OptMaxThreads;
int RandRange;

#if defined(_WIN32) && !defined(__GNUC__) || defined(_WIN64) && !defined(__GNUC__)
int (*POPCNT)(uint64);
#endif

uint8 HasPopcnt;

#ifdef Log
bool WriteLog;
#endif

// Eval Weights
int DrawWeight;
int KingSafetyWeight;
int MaterialWeight;
int MobilityWeight;
int PawnWeight;
int PositionalWeight;
int PSTWeight;

// Lazy Eval
int LazyEvalMin;
int LazyEvalMax;

//Piece Values
int PValue;
int NValue;
int BValue;
int RValue;
int QValue;
int BPValue;

// Pruning Thresholds
int PrunePawn;
int PruneMinor;
int PruneRook;
int PruneCheck;

// RobboBases
#ifdef RobboBases
bool UseRobboBases;
bool VerboseRobboBases;
bool AutoloadTotalBases;
bool AutoloadTripleBases;
char TotalDir[1024];
char TripleDir[1024];
int TotalBaseCache;
int TripleBaseHash;
int DynamicTripleBaseCache;
bool TotalBasesLoaded;
bool TripleBasesLoaded;
bool  SearchRobboBases;
int TripleMaxUsage;
char BulkDirectory[1024];
char BulkName[1024];
#endif

//Search Vars
int AspirationWindow;
bool ExtendInCheck;
int ValueCut;
int NullReduction;
int VerifyReduction;
bool  VerifyNull;
int DeltaCutoff;
int DepthRedMin;
int HeightMultiplier;
int HistoryThreshold;
int LowDepthMargin;
int MinDepthMultiplier;
int MinTransMoveDepth;
int QSAlphaThreshold;
int SearchDepthMin;
int SearchDepthReduction;
int TopMinDepth;
int UndoCountThreshold;

//Split Depths
bool  SplitAtCN;
int ANSplitDepth;
int CNSplitDepth;
int PVSplitDepth;
bool  SlitDepth;

//Time Management
int AbsoluteFactor;
int BattleFactor;
int EasyFactor;
int EasyFactorPonder;
int NormalFactor;

//UCI Info Strings
bool  CPULoadInfo;
bool  CurrMoveInfo;
bool DepthInfo;
bool HashFullInfo;
bool LowDepthPVs;
bool  NPSInfo;
bool VerboseUCI;
int MinPVDepth;

#ifdef RobboBases
	bool  TBHitInfo;
#endif

#define CheckForMate(v) \
	{ \
	if (v < -ValueMate + 64) return (-ValueMate + 64); \
	if (v > ValueMate - 64) return (ValueMate - 64); \
	} \

#ifdef Log
FILE *log_file;
char log_filename[256];
#endif

#include "arrays.h"
#include "functions.h"
#include "common.h"

char String1[MaxCPUs][64], String2[MaxCPUs][64];
char String3[MaxCPUs][64], String4[MaxCPUs][64];

#ifdef RobboBases
#define TripleCondition(Position) \
	(POPCNT (Position->OccupiedBW ^ (wBitboardP & (bBitboardP >> 8))) <= TripleMaxUsage)
#define TotalCondition(Position)  \
	(POPCNT (Position->OccupiedBW ^ (wBitboardP & (bBitboardP >> 8))) <= 6)
#endif

char *InputBuffer, *input_ptr;
