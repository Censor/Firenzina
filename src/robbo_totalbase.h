/*******************************************************************************
Firenzina is a UCI chess playing engine by
Yuri Censor (Dmitri Gusev) and ZirconiumX (Matthew Brades).
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

uint8 SpearPawn[4][64];
#define Triangle 0x00000000080c0e0f
#define Margin 0x0000000008040201
#define TriMar 0x80c0e0f0f8fcfeff
#define MaxTables 4096
#define TripleMax 4096
#define MaximumFopen 64
uint16 InverseNormalKing[462], InversePawnKing[1806];
uint16 Sq2Blocked[780], Sq2Pawn[1128], Sq2Normal[2016];
uint32 Sq3blocked[9880], Sq3Pawn[17296], Sq3normal[41664];
uint32 Sq4blocked[91390], Sq4Pawn[194580], Sq4normal[635376];
sint16 KingNormal[4096], KingPawn[4096];
int SameO4[64][64][64][64];
int SameO3[64][64][64];
int SameO2[64][64];
typedef struct
    {
    uint8 pi[4], sq[4], wK, bK;
    bool wtm, Pawn;
    uint32 n;
    uint64 Blocked;
    bool DTR;
    } type_PiSq;
int RB_CPUS;
uint64 ListSize;
#if !defined(_WIN32) && !defined(_WIN64) && !defined(__GNUC__)
#define UseSpinLocks true
#endif
#ifdef UseSpinLocks
#define RobboLockType pthread_spinlock_t
#define RobboLockIt(x) pthread_spin_lock (x)
#define RobboUnLockIt(x) pthread_spin_unlock (x)
#define RobboLock(tb, ind) pthread_spin_lock (&(tb->locks)[ind]);
#define RobboUnLock(tb, ind) pthread_spin_unlock (&(tb->locks)[ind]);
#define RobboLockInit(x) pthread_spin_init (x, 1)
#define RobboLockDestroy(x) pthread_spin_destroy (x)
#else
#define RobboLockType MutexType
#define RobboLockIt(x) Lock (x)
#define RobboUnLockIt(x) UnLock (x)
#define RobboLock(tb, ind) Lock (&(tb->locks)[ind]);
#define RobboUnLock(tb, ind) UnLock (&(tb->locks)[ind]);
#define RobboLockInit(x) LockInit (x)
#define RobboLockDestroy(x) LockDestroy (x)
#endif
MutexType RB_SMP[1];
MutexType FWriteLock[1];
bool DiskSwap;
typedef struct
    {
    uint64 m[4];
    uint8 *data, *Mar, *Win, *Loss;
    uint8 p[4];
    uint64 pro, PawnMult, PawnMult8, supp;
    int num, maximum_rima, shift, num_indexes;
    uint32 *indexes, Blockpro, BWT;
    bool Pawn, simm, Memory, TYPE, is_uno, DTR;
    sint8 efgh1, efgh2, _0;
    uint16 efgh[4];
    uint8 efgh_shift[4], efgh_file[4];
    FILE *Fdata, *Frima, *Fvit, *Fmar, *Fper, *Fsupp;
    int *rima_pro, *vit_pro, *per_pro, *mar_pro;
    uint32 supp_indice[8];
    char DirNome[256];
    char string[16];
    RobboLockType *locks;
    } RobboTotalBase;
typeMoveList *GenCapturesTotal(typePos *, typeMoveList *);
typeMoveList *WhiteQueenPromotions(typePos *, typeMoveList *, uint64);
void LoadTriple(char *, char *);
void GetTripleBase(char *);
#define Mark(A,w) (A[(w) >> 3] & (1 << ((w) & 7)))
#define Set(A,w) (A[(w) >> 3] |= (1 << ((w) & 7)))
#define UnSet(A,w) (A[(w) >> 3] &= ~(1 << ((w) & 7)))
int CountWhite(typePos *, uint64 *);
void InitTotalBase();
void DiskFread1(uint8 *, uint64, uint64, FILE *, uint64);
void DiskFread2(uint8 *, uint64, uint64, FILE *, uint64);
int DiskFwrite1(uint8 *, uint64, uint64, FILE *);
int DiskFwrite2(uint8 *, uint64, uint64, FILE *);
void UnWindows(char *);

#define DIV(X,a,b)                            \
  { if ((b) == 1) X = (a);                    \
    else if ((b) == 10) X = (a) / 10;         \
    else if ((b) == 12) X = (a) / 12;         \
    else if ((b) == 24) X = (a) / 24;         \
    else if ((b) == 40) X = (a) / 40;         \
    else if ((b) == 48) X = (a) / 48;         \
    else if ((b) == 64) X = (a) / 64;         \
    else if ((b) == 780) X = (a) / 780;       \
    else if ((b) == 1128) X = (a) / 1128;     \
    else if ((b) == 2016) X = (a) / 2016;     \
    else if ((b) == 9880) X = (a) / 9880;     \
    else if ((b) == 17296) X = (a) / 17296;   \
    else if ((b) == 41664) X = (a) / 41664;   \
    else if ((b) == 91390) X = (a) / 91390;   \
    else if ((b) == 194580) X = (a) / 194580; \
    else if ((b) == 635376) X = (a) / 635376; }
uint8 SpearHoriz[64], SpearVert[64], SpearDiag[64];
typedef struct
    {
    uint8 sq[4], pi[4], rip[4], rip_ai[4], efgh_shift[4], wK, bK;
    bool capW[4], capB[4];
    uint64 m[4], KingSlice_molt, Double, ind, Occupied, Pawn, KingSlice, PawnOnFourth;
    } tipo_fPos;
bool TotIndOtt(type_PiSq *, int *, uint64 *, uint64 *, int *);
uint64 MakeMark(RobboTotalBase *, int, RobboTotalBase *);
uint64 MakeRima(RobboTotalBase *, int, RobboTotalBase *);
void InitVariation();
int Compression(unsigned char *, int, int);
int BlockDecompress(unsigned char *, unsigned char *, int, int);
void InitTotalBaseCache(uint64);
bool BitboardPQ(typePos *, type_PiSq *);
char TripleBaseDir[1024];
uint64 TotIndexes;
#define wEnumB wEnumBL
#define bEnumB bEnumBL
#define BlockedPawn 8
#define MaxNum 4
int NomeCanonico(char *, bool *);
void RobboTotalSott(char *);
char TotalBaseDir[1024];
char CompressPrefix[1024];
int RobboTotalSweep(char *);
void GetTotalBase(char *);
#define DiskCompressed 0x05
#define DiskHuffman 0x03
RobboTotalBase *TableTotalBases;
int NumTotalBases;
int TotalBaseTable[16][16][16][16];
int ColScamb[16];
#define DiskNon 1
#define InCheck    \
  (Position->wtm ?  \
   (wBitboardK & Position->Dyn->bAtt) : (bBitboardK & Position->Dyn->wAtt))
#define IsIllegal  \
	(!Position->wtm ? \
	(wBitboardK & Position->Dyn->bAtt) : (bBitboardK & Position->Dyn->wAtt))
#define MaximumRima 125
#define ValueBroken (MaximumRima + 1)
#define ValueUnknown (MaximumRima + 2)
#define ValueWin (MaximumRima + 3)
#define ValueDraw (MaximumRima + 4)
#define FirstLoss (MaximumRima + 5)
#define LossIn(x) (FirstLoss + (x))
#define eWinTotal(Value)((Value) == ValueWin)
#define eDrawTotal(Value)((Value) == ValueDraw)
#define eLossTotal(Value)((Value) >= FirstLoss)
#define DrawLoss(Value) ((Value) >= ValueDraw)
#define Largh (240 - MaximumRima)
#define LossEnd ((FirstLoss) + (Largh))
#define dBreak 0x00
#define dWin 0x01
#define dDraw 0x02
#define dLoss (dDraw + 1)
#define DiskDrawLoss(x) ((x) >= dDraw)
#define DiskWin(x) ((x) == dWin)
#define DiskDraw(x) ((x) == dDraw)
#define DiskLoss(x) ((x) >= dLoss)
bool OttIndice(type_PiSq *, uint64 *, uint64 *, int *);
bool RobboTotalBaseScore(typePos *, int *);
int TotalValue(RobboTotalBase *, uint64);
void KingSlice(RobboTotalBase *, int);
void RobboTotalAdd(RobboTotalBase *, char *, char *);
bool RobboTotalRegister(char *, char *);
void InitTotalBaseUtility(bool);
void InitTotalBaseCache();
void InitTripleBase();
#define Exchange(x,y) { x ^= y; y ^= x; x ^= y; }
#define SpearPiece(u,x) x = SpearPawn[u][x]
#define SpearNorm(u,x) x = SpearNorm[u][x]
uint8 oSpear[64][64], SpearNorm[8][64];
void Send(char *fmt, ...);
