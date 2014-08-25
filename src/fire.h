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
// Safeguard added by YC on 1/9/2014:
#ifndef __FIRE_H_INCLUDED__   // if fire.h hasn't been included yet...
#define __FIRE_H_INCLUDED__   // #define this so the compiler knows it has been included

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#define Engine "Firenzina"
#define Vers "2.4.1 xTreme"
#define Orig "based on Ippolit"
#define Author "Kranium, Yuri Censor and ZirconiumX, a derivative of Fire by Kranium" 
// ZirconiumX added, 3/19/2013
// Kranium added back, 11/23/2013
#define Compiler "Yuri Censor" // Modified by Yuri Censor for Firenzina, 2/23/2013; Was: compiled by NS (i.e., Norman Schmidt) 

#if defined(__GNUC__)
#define STDIN_FileNO 0

#if defined(__i386__)
#define Plat "Linux 32"
#elif defined(__ANDROID__)
#define boolean bool
#define Plat "Android"
#else
#define Plat "Linux 64"
//#define LinuxLargePages
#endif

#elif  defined(_WIN64)
#define Plat "x64"
#define LargePages

#if defined(__GNUC__)
#define INLINE inline
#endif

#else
#define Plat "w32"
#define LargePages
#if defined(__GNUC__)
#define INLINE inline
#endif
#endif

#define true 1
#define false 0
#if defined(__GNUC__)
#define bool uint8
#elif defined(__INTEL_COMPILER) || defined(_MSC_VER)
#define bool boolean // Added 5/17/2013
#endif

#define Bench
//#define FischerRandom    // was added, but commented out by NS
#define HasPopCNT
#define HasPreFetch
#define HasIntrinsics
#define InitCFG            // was commented out by NS; uncommented by YC, 12/03/2013 
#define Log                // was commented out by NS; uncommented by YC, 01/15/2014
#define MatFactors         // was commented out by NS; uncommented by YC, 12/03/2013
//#define MultiplePosGain  // was commented out by NS
//#define MultipleHistory  // was commented out by NS
#define OneDimensional
#define RobboBases
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
#define MaxCPUs 16
#define RPperCPU 8
#define MaxSP 16 // It looks like this number must match MaxCPUs. Yuri Censor, 12/15/2013
#define DEFAULT_HASH_SIZE 128
#define MAX_HASH_SIZE 65536
#define DEFAULT_PAWN_HASH_SIZE 32
#define MAX_PAWN_HASH_SIZE 1024
#define DEFAULT_MULTIPV 1
#define MAX_MULTIPV 250
#define DEFAULT_MIN_PV_DEPTH 15

// Lazy Eval
#define DEFAULT_LAZY_EVAL_MIN 150        // Original: 150; Conservative: 90; Aggressive 70
#define MAX_LAZY_EVAL_MIN 300
#define DEFAULT_LAZY_EVAL_MAX 300
#define MAX_LAZY_EVAL_MAX 600

// Piece Values
#define DEFAULT_PAWN_VALUE 100           // Original: 100; DO NOT ALTER! IT SETS THE SCALE. Yuri Censor, 03/25/2013
#define MAX_PAWN_VALUE 200
#define DEFAULT_KNIGHT_VALUE 300         // Original: 320; Modified: 317 (03/28/2013); GH: 300 (5/21/2013)
#define MAX_KNIGHT_VALUE 640
#define DEFAULT_BISHOP_VALUE 310         // Original: 330; Modified: 333 (03/28/2013); GH: 310 (5/21/2013)
#define MAX_BISHOP_VALUE 640
#define DEFAULT_ROOK_VALUE 500           // Original: 510; Modified: 520 (03/28/2013); GH: 500 (5/21/2013)
#define MAX_ROOK_VALUE 1000
#define DEFAULT_QUEEN_VALUE 950          // Original: 1000; Modified: 960 (03/28/2013); GH: 950 (5/21/2013)
#define MAX_QUEEN_VALUE 2000
#define DEFAULT_BISHOP_PAIR_VALUE 45     // Original: 50; Modified: 50 (03/28/2013); GH: 45 (5/21/2013)
#define MAX_BISHOP_PAIR_VALUE 200

// Prune Thresholds
#define DEFAULT_PRUNE_CHECK 10
#define MAX_PRUNE_CHECK 30
#define DEFAULT_PRUNE_PAWN 160
#define MAX_PRUNE_PAWN 320
#define DEFAULT_PRUNE_MINOR 500
#define MAX_PRUNE_MINOR 1000
#define DEFAULT_PRUNE_ROOK 800
#define MAX_PRUNE_ROOK 1600

// RobboBases
#define MAX_TOTAL_BASE_CACHE 1024
#define MAX_TRIPLE_BASE_HASH 4096
#define MAX_DYNAMIC_TRIPLE_BASE_CACHE 65536

// Search Vars
#define DEFAULT_ASPIRATION_WINDOW 6      // Original: 8; Conservative: 8; Aggressive: 6
#define MAX_ASPIRATION_WINDOW 100
#define DEFAULT_COUNT_LIMIT 5
#define MAX_COUNT_LIMIT 10
#define MIN_DELTA_CUTOFF 20000
#define DEFAULT_DELTA_CUTOFF 25000
#define MAX_DELTA_CUTOFF 30000
#define DEFAULT_DEPTH_RED_MIN 12
#define MAX_DEPTH_RED_MIN 24
#define DEFAULT_HEIGHT_MULTIPLIER 64
#define MAX_HEIGHT_MULTIPLIER 128
#define DEFAULT_HISTORY_THRESHOLD 50
#define MAX_HISTORY_THRESHOLD 100
#define DEFAULT_LOW_DEPTH_MARGIN 1125    // Original: 1125; Conservative: 1150; Aggressive: 1200
#define MAX_LOW_DEPTH_MARGIN 2000
#define DEFAULT_MIN_DEPTH_MULTIPLIER 48
#define MAX_MIN_DEPTH_MULTIPLIER 96
#define DEFAULT_MIN_TRANS_MOVE_DEPTH 16
#define MAX_MIN_TRANS_MOVE_DEPTH 32
#define DEFAULT_NULL_REDUCTION 8
#define MAX_NULL_REDUCTION 16
#define DEFAULT_QS_ALPHA_THRESHOLD 200
#define MAX_QS_ALPHA_THRESHOLD 400
#define DEFAULT_SEARCH_DEPTH_MIN 20      // Original: 20; Conservative: 22; Aggressive: 22
#define MAX_SEARCH_DEPTH_MIN 40
#define DEFAULT_SEARCH_DEPTH_REDUCTION 6 // Original: 6; Conservative: 6; Aggressive: 8
#define MAX_SEARCH_DEPTH_REDUCTION 12
#define MAX_SEE_CUTOFF 240
#define DEFAULT_SEE_CUTOFF 120
#define MAX_SEE_LIMIT 12348
#define DEFAULT_SEE_LIMIT 6174
#define DEFAULT_TOP_MIN_DEPTH 14         // Original: 14; Conservative: 14; Aggressive: 16
#define MAX_TOP_MIN_DEPTH 28
#define DEFAULT_UNDO_COUNT_THRESHOLD 15 // Original: 15; Conservative: 15; Aggressive: 17
#define MAX_UNDO_COUNT_THRESHOLD 20
#define MIN_VALUE_CUT 1000
#define DEFAULT_VALUE_CUT 15000
#define MAX_VALUE_CUT 30000
#define DEFAULT_VERIFY_REDUCTION 2       // Original: 2; Conservative: 3; Aggressive: 1
#define MAX_VERIFY_REDUCTION 16

// Split Depths
#define MIN_AN_SPLIT_DEPTH 8             // Original: 12
#define DEFAULT_AN_SPLIT_DEPTH 12        // Original: 12; Conservative: 12; Aggressive: 14
#define MAX_AN_SPLIT_DEPTH 24
#define MIN_CN_SPLIT_DEPTH 8             // Original: 12
#define DEFAULT_CN_SPLIT_DEPTH 14        // Original: 14; Conservative: 14; Aggressive: 16
#define MAX_CN_SPLIT_DEPTH 24
#define MIN_PV_SPLIT_DEPTH 8             // Original: 12
#define DEFAULT_PV_SPLIT_DEPTH 12        // Original: 12
#define MAX_PV_SPLIT_DEPTH 24

// Time Management
#define DEFAULT_ABSOLUTE_FACTOR 25       // Original: 25; Conservative: 25; Aggressive: 30
#define MAX_ABSOLUTE_FACTOR 100
#define DEFAULT_BATTLE_FACTOR   100      // Original: 100; Conservative: 95; Aggressive: 75
#define MAX_BATTLE_FACTOR 200
#define DEFAULT_DESIRED_MILLIS 40
#define MAX_DESIRED_MILLIS 80
#define DEFAULT_EASY_FACTOR 15           // Original: 15; Conservative: 15; Aggressive: 20
#define MAX_EASY_FACTOR 100
#define DEFAULT_EASY_FACTOR_PONDER 33    // Original: 33
#define MAX_EASY_FACTOR_PONDER 100
#define DEFAULT_NORMAL_FACTOR 75         // Original: 75; Conservative: 80; Aggressive: 50
#define MAX_NORMAL_FACTOR 200

// Weights
#define DEFAULT_DRAW_WEIGHT 128          // Original: 100
#define MAX_DRAW_WEIGHT 256
#define DEFAULT_KING_SAFETY_WEIGHT 128   // Original: 100; Conservative: 130; Aggressive: 120
#define MAX_KING_SAFETY_WEIGHT 256
#define DEFAULT_MATERIAL_WEIGHT 128     // Original: 100
#define MAX_MATERIAL_WEIGHT 256
#define DEFAULT_MOBILITY_WEIGHT 128      // Original: 100; Conservative: 115; Aggressive: 130
#define MAX_MOBILITY_WEIGHT 256
#define DEFAULT_PAWN_WEIGHT 128          // Original: 100
#define MAX_PAWN_WEIGHT 256
#define DEFAULT_POSITIONAL_WEIGHT 128    // Original: 100; Conservative: 120; Aggressive: 110
#define MAX_POSITIONAL_WEIGHT 256
#define DEFAULT_PST_WEIGHT 128           // Original: 100
#define MAX_PST_WEIGHT 256

#include "win-linux.h"
#include "hash.h"

int NumCPUs;
uint64 NodeCheck;
#define ZobRev(Pos) (0ULL)
#define CheckHalt() { if (Position->stop) { return(0); } }
#define Height(x)((x)->height)
#define Is_Exact(x) (x)
#if defined(__GNUC__)
#define prefetch(x) __builtin_prefetch((char*)(x))
#elif defined(__INTEL_COMPILER) || defined(_MSC_VER)
#define prefetch(x) _mm_prefetch((char*)(x), _MM_HINT_T2) // Added 4/15/2013
#endif
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

#define UpdateSeldepth(x) \
	if ((x)->height > seldepth) seldepth = (x)->height;

#define MAX(x, y) (((x) >= (y)) ? (x) : (y))
#define MIN(x, y) (((x) <= (y)) ? (x) : (y))
#define ABS(x) (((x) >= 0) ? (x) : -(x))
#define FileDistance(x, y) (ABS(File(x) - File(y)))
#define RankDistance(x, y) (ABS(Rank(x) - Rank(y)))
//#define ValueRed (depth >> 1)
//#define DepthRed (MIN (12, ValueRed))


#define BitClear(b, B) B &= (B - 1)
#define BitSet(b, B) B |=((uint64) 1) << (b)

#define Score(x,y) (((x) << 16) + (y))
#define ValueMate 30000
#define ValueInfinity 32750
#define MultiCentiPawnPV 65535
#define RandomCount 0
#define RandomBits 1

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

#ifdef FischerRandom
bool Chess960;
uint8 Chess960KingRookFile;
uint8 Chess960QueenRookFile;
uint8 Chess960KingFile;
#endif

#ifdef Bench
bool BenchMarking;
#endif

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
int OptMinThreads; // Added 5/22/2013 by Yuri Censor for Firenzina
int RandRange;
int seldepth;

#if defined(_WIN32) && !defined(__GNUC__) || defined(_WIN64) && !defined(__GNUC__)
long long (*POPCNT) (unsigned long long); // Modification by Yuri Censor for Firenzina, 2/17/2013
      // Reason: To comply with types for _mm_popcnt_u64 in MS Visual Studio Ultimate 2012
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
int CountLimit;
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
int SEECutOff;
int SEELimit;

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
int DesiredMillis;
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
#endif
