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

#include <stdio.h>
#include <stdlib.h>

#define bool char

#define WINDOWS
//#define WINDOWS_X64

#define Engine "Firenzina"
#define Vers "2.2.2 xTreme"
#define Author "Yuri Censor, a clone of Fire 2.2 xTreme by Kranium, w/ bugfixed by ZirconiumX"

#define HasPopCNT
#define HasPreFetch
#define InitCFG
//#define LargePages
//#define LinuxLargePages
#define Log
#define MatFactors
#define MultiplePosGain
#define MultipleHistory
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
//int (*POPCNT)(uint64);
long long (*POPCNT) (unsigned long long); // Modification by Yuri Censor for Firenzina, 2/17/2013
      // Reason: To comply with types for _mm_popcnt_u64 in MS Visual Studio Ultimate 2012
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
bool SearchRobboBases;
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
bool VerifyNull;
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
bool SplitAtCN;
int ANSplitDepth;
int CNSplitDepth;
int PVSplitDepth;
bool SlitDepth;

//Time Management
int AbsoluteFactor;
int BattleFactor;
int EasyFactor;
int EasyFactorPonder;
int NormalFactor;

//UCI Info Strings
bool CPULoadInfo;
bool CurrMoveInfo;
bool DepthInfo;
bool HashFullInfo;	
bool LowDepthPVs;
bool NPSInfo;
bool VerboseUCI;
int MinPVDepth;

#ifdef RobboBases
	bool TBHitInfo;
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
