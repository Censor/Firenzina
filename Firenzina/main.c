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

	OptHashSize = 128;
	OptPHashSize = 32;
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
    DrawWeight = 100;
    KingSafetyWeight = 100;
    MaterialWeight = 100;
    MobilityWeight = 100;
    PawnWeight = 100;
	PositionalWeight = 100;
    PSTWeight = 100;

// Lazy Eval
	LazyEvalMin = 150;
	LazyEvalMax = 300;
	
// Piece Values
	PValue = 100;
	NValue = 320;
	BValue = 330;
	RValue = 510;
	QValue = 1000;
	BPValue = 50;
	
// Prune Thresholds
	PrunePawn = 160;
	PruneMinor = 500;
	PruneRook = 800;
	PruneCheck = 10;
	
// Search Vars
	AspirationWindow = 8;
	DeltaCutoff = 25000;
	DepthRedMin = 12;
	ExtendInCheck = false;
	HeightMultiplier = 64;
	HistoryThreshold = 50;
	LowDepthMargin = 1125;
	MinDepthMultiplier = 48;
	MinTransMoveDepth = 16;
	NullReduction = 8;
	QSAlphaThreshold = 200;
	SearchDepthMin = 20;
	SearchDepthReduction = 6;
	TopMinDepth = 14;
	UndoCountThreshold = 15;
	ValueCut = 15000;
	VerifyNull = true;
	VerifyReduction = 2;

// Split Depths
	SplitAtCN = true;
	ANSplitDepth = 12;
	CNSplitDepth = 14;
	PVSplitDepth = 12;

// Time Management
	AbsoluteFactor = 25;
	BattleFactor = 100;
	EasyFactor = 15;
	EasyFactorPonder = 33;
	NormalFactor = 75;

//UCI Info Strings
	CPULoadInfo = false;
	CurrMoveInfo = false;
	DepthInfo = false;
    HashFullInfo = false;	
	LowDepthPVs = false;
	NPSInfo = false;
	VerboseUCI = false;
	MinPVDepth = 15;	
	
#ifdef RobboBases
	TBHitInfo = false;
#endif

//Miscellaneous
	CfgFile = 0;
	CfgFound = false;
    InfiniteLoop = false;
    MultiPV = 1;
    NumThreads = 1;
    OptMaxThreads = MaxCPUs;
    Ponder = false;
	RandRange = 20;
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
    RPinit();

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

#ifdef LinuxLargePages
    LinuxHandleSignals();
#endif
			
	InitSMP();
    while (1)
        Input(RootPosition0);
    return 0;
    }