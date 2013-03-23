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

void NanoSleep(int);
int InitPawnHash(int);
int InitPawnHashWrapper(int);
void InitArrays();
void InitCaptureValues();
void InitSlab(int);
void CreateMem(void **, int, uint64, int *, bool *, char *);
void FreeMem(void *, int *, bool *);
uint64 GetRand();
void HaltSearch(int, int);
void CheckDone(typePos *, int);
void InitSearch(typePos *, char *);
void Info(sint64);
void Eval (typePos *, int, int, int, int);
void Mobility(typePos *);
void EvalHashClear();
int InitEvalHash(int);
int EvalEnding(typePos *Position, int Value, uint64 wPatt, uint64 bPatt);
void SetValueArray();
void InitRandom32(uint64);
void SetPOPCNT();

#ifdef RobboBases
void TripleHashClear();
int InitTripleHash(int);
#endif

typeMoveList *CaptureMoves(typePos *, typeMoveList *, uint64);
typeMoveList *OrdinaryMoves(typePos *, typeMoveList *);
typeMoveList *EvasionMoves(typePos *, typeMoveList *, uint64);
void InitCaptureValues();
typeMoveList *WhiteCaptures(typePos *, typeMoveList *, uint64);
typeMoveList *BlackCaptures(typePos *, typeMoveList *, uint64);
typeMoveList *WhiteOrdinary(typePos *, typeMoveList *);
typeMoveList *BlackOrdinary(typePos *, typeMoveList *);
void SortOrdinary(typeMoveList *, typeMoveList *, uint32, uint32, uint32);
typeMoveList *QuietChecksWhite(typePos *, typeMoveList *, uint64);
typeMoveList *QuietChecksBlack(typePos *, typeMoveList *, uint64);
typeMoveList *PositionalMovesWhite(typePos *, typeMoveList *, int);
typeMoveList *PositionalMovesBlack(typePos *, typeMoveList *, int);
typeMoveList *BlackEvasions(typePos *, typeMoveList *, uint64);
typeMoveList *WhiteEvasions(typePos *, typeMoveList *, uint64);
void IncrementAge();
void HashClear();
int HashClearAll();
int InitHash(int);
void HashLowerAllNew(uint64, int, int, int, int, int);
void HashUpperCutNew(uint64, int, int, int, int);
void HashLowerNew(uint64, int, int, int, int, int, int);
void HashUpperNew(uint64, int, int, int, int, int);
void HashExactNew(uint64, int, int, int, int, int, int);
int InitPVHash(int);
int PVHashClear();
void Input(typePos *);
void ResetHistory();
void ResetPositionalGain();
void Make(typePos *, uint32);
void Undo(typePos *, uint32);
void MakeWhite(typePos *, uint32);
void UndoWhite(typePos *, uint32);
void MakeBlack(typePos *, uint32);
void UndoBlack(typePos *, uint32);
int InitMaterialValue();
uint32 NextWhite(typePos *, typeNext *);
uint32 NextBlack(typePos *, typeNext *);
bool WhiteOK(typePos *, uint32);
bool BlackOK(typePos *, uint32);
void InitPawns();
void PawnEval(typePos *, typePawnEval *);
int Move50(typePos *);
int PVQsearchWhite(typePos *, int, int, int);
int PVQsearchWhiteCheck(typePos *, int, int, int);
int PVQsearchBlack(typePos *, int, int, int);
int PVQsearchBlackCheck(typePos *, int, int, int);
void TopWhite(typePos *);
void TopBlack(typePos *);
int RootWhite(typePos *, int, int, int);
int RootBlack(typePos *, int, int, int);
int PVNodeWhite(typePos *, int, int, int, int);
int PVNodeBlack(typePos *, int, int, int, int);
int ExcludeWhite(typePos *, int, int, uint32);
int ExcludeWhiteCheck(typePos *, int, int, uint32);
int ExcludeBlack(typePos *, int, int, uint32);
int ExcludeBlackCheck(typePos *, int, int, uint32);
int CutNodeWhite(typePos *, int, int);
int CutNodeBlack(typePos *, int, int);
int CutNodeWhiteCheck(typePos *, int, int);
int CutNodeBlackCheck(typePos *, int, int);
int AllNodeWhite(typePos *, int, int);
int AllNodeBlack(typePos *, int, int);
int AllNodeWhiteCheck(typePos *, int, int);
int AllNodeBlackCheck(typePos *, int, int);
int LowDepthWhite(typePos *, int, int);
int LowDepthBlack(typePos *, int, int);
int LowDepthWhiteCheck(typePos *, int, int);
int LowDepthBlackCheck(typePos *, int, int);
int QsearchWhite(typePos *, int, int);
int QsearchBlack(typePos *, int, int);
int QsearchWhiteCheck(typePos *, int, int);
int QsearchBlackCheck(typePos *, int, int);
static void OutputBestMove();
void Search(typePos *);
void Information(typePos *, sint64, int, int, int);
bool WhiteSEE(typePos *, uint32);
bool BlackSEE(typePos *, uint32);
void InitPosition(typePos *, char *);
void InitStatic();
uint32 FullMove(typePos *, uint32);
uint32 NumericMove(typePos *, char *);
void ErrorEnd(char *, ...);
void ErrorFen(char *, ...);
void Send(char *, ...);
char *Notate(uint32, char *);
uint64 GetClock();
uint64 ProcessClock();
void InitBitboards(typePos *);
void NewGame(typePos *, bool);
bool TryInput();

#ifdef RobboBases
bool RobboMake(typePos *);
void InitTotalBaseUtility(bool);
void TotalInit();
int SetTotalBaseCache(int);
int SetTripleBaseCache(int);
void InitTotalBaseCache(uint64);
void InitTripleBaseCache(uint64);
bool TripleValue(typePos *, int *, bool, bool);
void InitTotalBase();
void TripleStatistics();
void CleanupTriple();
int LoadRobboTripleBases();
int RegisterRobboTotalBases();
int UnLoadRobboTripleBases();
int DeRegisterRobboTotalBases();
int RobboBulkDirectory();
int RobboBulkLoad();
int RobboBulkDirectoryDetach();
int RobboBulkDetach();
void OfftoneTripleSMP();
void QuitTripleHash();
void MainLine(typePos *Position);
bool SubsumeTripleSMP();
#endif

bool IvanSplit(typePos *, typeNext *, int, int, int, int, int *);
void FailHigh(SplitPoint *, typePos *, uint32);
void StubIvan();
static void ThreadHalt(typePos *);
void ThreadStall(typePos *, int);
void WhitePVNodeSMP(typePos *);
void BlackPVNodeSMP(typePos *);
void WhiteAllSMP(typePos *);
void BlackAllSMP(typePos *);
void WhiteCutSMP(typePos *);
void BlackCutSMP(typePos *);
int InitSMP();
void EndSMP();
void RPinit();
int PawnHashReset();
void PonderHit();
void ShowBanner();
void GetSysInfo();
void WhiteTopAnalysis(typePos *);
void BlackTopAnalysis(typePos *);
int WhiteAnalysis(typePos *, int, int, int);
int BlackAnalysis(typePos *, int, int, int);
int WhiteMultiPV(typePos *, int);
int BlackMultiPV(typePos *, int);
char *EmitFen(typePos *, char *);
void BenchMark(typePos *, char *);
int Stat(char *);
void QuitLargePawns();
void QuitPV();
void DetachAllMemory();
void QuitEvalHash();
void SetupPrivileges();
void MagicMultInit();
static void FillSlab();

#ifdef InitCFG
void read_cfg_file(char *file_name);
void gen_cfg_file(char *file_name);
static int get_rand_num(int min, int max);
#endif

#ifdef Log
int create_log();
void close_log();
#endif

#ifdef SlabMemory
void DeleteSlab();
#endif
