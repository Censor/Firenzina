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

#include "fire.h"
#include "evaluation.h"

#define Ranks78 0xffff000000000000
#define Ranks678 0xffffff0000000000
#define Ranks12 0x000000000000ffff
#define Ranks123 0x00000000000ffffff

#ifdef HasPreFetch
#define PrefetchPawnHash __builtin_prefetch (&PawnPointer, 1, 2);
#else
#define PrefetchPawnHash
#endif

#define EvalTweak 0x12345678
#define GetEvalHash(Z) EvalHash[(Z ^ EvalTweak) & EvalHashMask]

#define QueenEnd ((Position->Dyn->flags & 28) == 4)
#define RookEnd ((Position->Dyn->flags & 28) == 8)

#define WhiteMinorOnly (8 << 2)
#define BlackMinorOnly (16 << 2)

typedef struct
    {
    uint64 RandKey;
    uint8 pad[56];
    } RAND;
static RAND Rand[MaxCPUs];

uint8 KPwtm[24576] =
	{
#include "kp_white.h"
	};
uint8 KPbtm[24576] =
	{
#include "kp_black.h"
	};

static uint32 Random32(int cpu)
    {
    Rand[cpu].RandKey = Rand[cpu].RandKey * 0x7913cc52088a6cfULL + 0x99f2e6bb0313ca0dULL;
    return ((Rand[cpu].RandKey >> 18) & 0xffffffff);
    }
void InitRandom32(uint64 x)
    {
    int cpu;
    for (cpu = 0; cpu < MaxCPUs; cpu++)
        {
        x = x * 0xb18ec564ff729005ULL + 0x86ee25701b5e244fULL;
        Rand[cpu].RandKey = x;
        }
    }
static void AdjustPositionalGain(typePos *Position, int move)
    {
    int v, p, m;
    if (Position->Dyn->cp)
        return;
    p = Position->sq[To(move)];
    m = move & 07777;
    v = ((Position->Dyn - 1)->PositionalValue) - Position->Dyn->PositionalValue;
    v = Position->wtm ? v : -v;
    if (MaxPosGain(p, m) <= v)
        MaxPosGain(p, m) = v;
    else
        MaxPosGain(p, m)--;
    }

static int MaterialValue(typePos *Position)
    {
    int Value = QValue * (POPCNT(wBitboardQ) - POPCNT(bBitboardQ));
    Value += RValue * (POPCNT(wBitboardR) - POPCNT(bBitboardR));
    Value += BValue * (POPCNT(wBitboardB) - POPCNT(bBitboardB));
    Value += NValue * (POPCNT(wBitboardN) - POPCNT(bBitboardN));
    Value += PValue * (POPCNT(wBitboardP) - POPCNT(bBitboardP));

    if (wBitboardBL && wBitboardBD)
        Value += BPValue;
    if (bBitboardBL && bBitboardBD)
        Value -= BPValue;

    Value *= (MaterialWeight << 10) / 100;
    Value >>= 10;
    return Value;
    }
static void KingPawnWhite(typePos *Position, int matval, uint8 Token, typePawnEval *PawnInfo)
    {
    int Value, WhiteLeader, BlackLeader, sq, rank;
    uint8 C;
    uint64 A, T, wPatt, bPatt;
    if (PawnInfo->PawnHash != Position->Dyn->PawnHash)
        PawnEval(Position, PawnInfo);
    Position->Dyn->wXray = Position->Dyn->bXray = 0;
    Value = ((Position->Dyn->Static)+(PawnInfo->Score));
    Value = (sint16)(Value & 0xffff);
    WhiteLeader = 0;
    C = PawnInfo->wPassedFiles;
    while (C)
        {
        sq = BSR(FileArray[BSF(C)] & wBitboardP);
        rank = Rank(sq);
        BitClear(0, C);
        if ((ShepherdWK[sq] & wBitboardK) == 0)
            {
            if (wBitboardOcc & OpenFileW[sq] || (bBitboardK & QuadrantBKwtm[sq]) == 0)
                continue;
            if (WhiteLeader <= rank)
                WhiteLeader = rank;
            }
        else if (WhiteLeader <= rank)
            WhiteLeader = rank;
        }
    BlackLeader = 0;
    C = PawnInfo->bPassedFiles;
    while (C)
        {
        sq = BSF(FileArray[BSF(C)] & bBitboardP);
        rank = R8 - Rank(sq);
        BitClear(0, C);
        if ((ShepherdBK[sq] & bBitboardK) == 0)
            {
            if (bBitboardOcc & OpenFileB[sq] || (wBitboardK & QuadrantWKwtm[sq]) == 0)
                continue;
            if (BlackLeader <= rank)
                BlackLeader = rank;
            }
        else if (BlackLeader <= rank)
            BlackLeader = rank;
        }
    Position->Dyn->Value = (Token * (Value + matval)) >> 7;
    Position->Dyn->bKcheck = Position->Dyn->wKcheck = 0;
    if (WhiteLeader > BlackLeader && (bBitboardP & InFrontB[R8 - WhiteLeader + 1]) == 0)
        Position->Dyn->Value += 150 + 50 * WhiteLeader;
    else if (BlackLeader > WhiteLeader + 1 && (wBitboardP & InFrontW[BlackLeader - 2]) == 0)
        Position->Dyn->Value -= 150 + 50 * BlackLeader;
    A = (wBitboardP &(~FileA)) << 7;
    T = A & bBitboardK;
    Position->Dyn->bKcheck |= (T >> 7);
    Position->Dyn->wAtt = A;
    A = (wBitboardP &(~FileH)) << 9;
    T = A & bBitboardK;
    Position->Dyn->bKcheck |= (T >> 9);
    Position->Dyn->wAtt |= A;
    wPatt = Position->Dyn->wAtt;
    Position->Dyn->wAtt |= AttK[Position->wKsq];
    A = (bBitboardP &(~FileH)) >> 7;
    T = A & wBitboardK;
    Position->Dyn->wKcheck |= (T << 7);
    Position->Dyn->bAtt = A;
    A = (bBitboardP &(~FileA)) >> 9;
    T = A & wBitboardK;
    Position->Dyn->wKcheck |= (T << 9);
    Position->Dyn->bAtt |= A;
    bPatt = Position->Dyn->bAtt;
    Position->Dyn->bAtt |= AttK[Position->bKsq];
    if (bBitboardK & AttK[Position->wKsq])
        {
        Position->Dyn->bKcheck |= SqSet[Position->wKsq];
        Position->Dyn->wKcheck |= SqSet[Position->bKsq];
        }
    if (Position->Dyn->Value > 0 && !wBitboardP)
        Position->Dyn->Value = 0;
    else if (Position->Dyn->Value < 0 && !bBitboardP)
        Position->Dyn->Value = 0;
    if (Position->Dyn->Value > 0)
        {
        if ((wBitboardP & ~FileH) == 0 && (bBitboardK | AttK[Position->bKsq]) & SqSet[H8])
            Position->Dyn->Value = 0;
        if ((wBitboardP & ~FileA) == 0 && (bBitboardK | AttK[Position->bKsq]) & SqSet[A8])
            Position->Dyn->Value = 0;
        if ((Position->Dyn->flags & 28) == 28)
            {
            sq = BSF(wBitboardP);
            rank = Rank(sq);
            Value = KPwtm[384 * Position->wKsq + 6 * Position->bKsq + rank - 1]&(1 << File(sq));
            if (!Value)
                Position->Dyn->Value = 0;
            else
                Position->Dyn->Value = ((sint16)(Position->Dyn->Static & 0xffff)) + 75 * rank + 250;
            }
        }
    else if (Position->Dyn->Value < 0)
        {
        if ((bBitboardP & ~FileH) == 0 && (wBitboardK | AttK[Position->wKsq]) & SqSet[H1])
            Position->Dyn->Value = 0;
        if ((bBitboardP & ~FileA) == 0 && (wBitboardK | AttK[Position->wKsq]) & SqSet[A1])
            Position->Dyn->Value = 0;
        if ((Position->Dyn->flags & 28) == 28)
            {
            sq = H8 - BSR(bBitboardP);
            rank = Rank(sq);
            Value = KPbtm[384 * (H8 - Position->bKsq) + 6 * (H8 - Position->wKsq) + rank - 1]&(1 << File(sq));
            if (!Value)
                Position->Dyn->Value = 0;
            else
                Position->Dyn->Value = ((sint16)(Position->Dyn->Static & 0xffff)) - 75 * rank - 250;
            }
		if (!((wBitboardP << 8) & ~Position->OccupiedBW) && !(wPatt & bBitboardOcc)
			&& !Position->Dyn->ep && !(AttK[Position->wKsq]& ~Position->Dyn->bAtt) && !Position->Dyn->wKcheck)
			Position->Dyn->Value = 0;
        }
    }
static void KingPawnBlack(typePos *Position, int matval, uint8 Token, typePawnEval *PawnInfo)
    {
    int Value, WhiteLeader, BlackLeader, sq, rank;
    uint8 C;
    uint64 A, T, wPatt, bPatt;
    if (PawnInfo->PawnHash != Position->Dyn->PawnHash)
        PawnEval(Position, PawnInfo);
    Position->Dyn->wXray = Position->Dyn->bXray = 0;
    Value = ((Position->Dyn->Static)+(PawnInfo->Score));
    Value = (sint16)(Value & 0xffff);
    WhiteLeader = 0;
    C = PawnInfo->wPassedFiles;
    while (C)
        {
        sq = BSR(FileArray[BSF(C)] & wBitboardP);
        rank = Rank(sq);
        BitClear(0, C);
        if ((ShepherdWK[sq] & wBitboardK) == 0)
            {
            if (wBitboardOcc & OpenFileW[sq] || (bBitboardK & QuadrantBKbtm[sq]) == 0)
                continue;
            if (WhiteLeader <= rank)
                WhiteLeader = rank;
            }
        else if (WhiteLeader <= rank)
            WhiteLeader = rank;
        }
    BlackLeader = 0;
    C = PawnInfo->bPassedFiles;
    while (C)
        {
        sq = BSF(FileArray[BSF(C)] & bBitboardP);
        rank = R8 - Rank(sq);
        BitClear(0, C);
        if ((ShepherdBK[sq] & bBitboardK) == 0)
            {
            if (bBitboardOcc & OpenFileB[sq] || (wBitboardK & QuadrantWKbtm[sq]) == 0)
                continue;
            if (BlackLeader <= rank)
                BlackLeader = rank;
            }
        else if (BlackLeader <= rank)
            BlackLeader = rank;
        }
    Position->Dyn->Value = -(Token * (Value + matval)) >> 7;
    Position->Dyn->bKcheck = Position->Dyn->wKcheck = 0;
    if (WhiteLeader > BlackLeader + 1 && (bBitboardP & InFrontB[R8 - WhiteLeader + 2]) == 0)
        Position->Dyn->Value -= 150 + 50 * WhiteLeader;
    else if (BlackLeader > WhiteLeader && (wBitboardP & InFrontW[BlackLeader - 1]) == 0)
        Position->Dyn->Value += 150 + 50 * BlackLeader;
    A = (wBitboardP &(~FileA)) << 7;
    T = A & bBitboardK;
    Position->Dyn->bKcheck |= (T >> 7);
    Position->Dyn->wAtt = A;
    A = (wBitboardP &(~FileH)) << 9;
    T = A & bBitboardK;
    Position->Dyn->bKcheck |= (T >> 9);
    Position->Dyn->wAtt |= A;
    wPatt = Position->Dyn->wAtt;
    Position->Dyn->wAtt |= AttK[Position->wKsq];
    A = (bBitboardP &(~FileH)) >> 7;
    T = A & wBitboardK;
    Position->Dyn->wKcheck |= (T << 7);
    Position->Dyn->bAtt = A;
    A = (bBitboardP &(~FileA)) >> 9;
    T = A & wBitboardK;
    Position->Dyn->wKcheck |= (T << 9);
    Position->Dyn->bAtt |= A;
    bPatt = Position->Dyn->bAtt;
    Position->Dyn->bAtt |= AttK[Position->bKsq];
    if (bBitboardK & AttK[Position->wKsq])
        {
        Position->Dyn->bKcheck |= SqSet[Position->wKsq];
        Position->Dyn->wKcheck |= SqSet[Position->bKsq];
        }
    if (Position->Dyn->Value < 0 && !wBitboardP)
        Position->Dyn->Value = 0;
    else if (Position->Dyn->Value > 0 && !bBitboardP)
        Position->Dyn->Value = 0;
    if (Position->Dyn->Value < 0)
        {
        if ((wBitboardP & ~FileH) == 0 && (AttK[Position->bKsq] | bBitboardK) & SqSet[H8])
            Position->Dyn->Value = 0;
        if ((wBitboardP & ~FileA) == 0 && (AttK[Position->bKsq] | bBitboardK) & SqSet[A8])
            Position->Dyn->Value = 0;
        if ((Position->Dyn->flags & 28) == 28)
            {
            sq = BSF(wBitboardP);
            rank = Rank(sq);
            Value = KPbtm[384 * Position->wKsq + 6 * Position->bKsq + rank - 1]&(1 << File(sq));
            if (!Value)
                Position->Dyn->Value = 0;
            else
                Position->Dyn->Value = -((sint16)(Position->Dyn->Static & 0xffff)) - 75 * rank - 250;
            }
        }
    else if (Position->Dyn->Value > 0)
        {
        if ((bBitboardP & ~FileH) == 0 && (AttK[Position->wKsq] | wBitboardK) & SqSet[H1])
            Position->Dyn->Value = 0;
        if ((bBitboardP & ~FileA) == 0 && (AttK[Position->wKsq] | wBitboardK) & SqSet[A1])
            Position->Dyn->Value = 0;
        if ((Position->Dyn->flags & 28) == 28)
            {
            sq = H8 - BSR(bBitboardP);
            rank = Rank(sq);
            Value = KPwtm[384 * (H8 - Position->bKsq) + 6 * (H8 - Position->wKsq) + rank - 1]&(1 << File(sq));
            if (!Value)
                Position->Dyn->Value = 0;
            else
                Position->Dyn->Value = -((sint16)(Position->Dyn->Static & 0xffff)) + 75 * rank + 250;
            }
		if (!((bBitboardP >> 8) & ~Position->OccupiedBW) && !(bPatt & wBitboardOcc)
			&& !Position->Dyn->ep && !(AttK[Position->bKsq]& ~Position->Dyn->wAtt) && !Position->Dyn->bKcheck)
			Position->Dyn->Value = 0;
        }
    }

void Eval(typePos *Position, int min, int max, int move, int depth)
    {
    typePawnEval *PawnPointer;
    int index, matval, Value, MobValue = 0;
    int b, rank, antiphase, phase;
    int to, cp, wKs, bKs;
    uint64 U, wKatt, bKatt, A, AttB, AttR;
    sint32 wKhit, bKhit;
    uint64 wGoodMinor, bGoodMinor, wSafeMob, bSafeMob, wOKxray, bOKxray;
    uint64 T, bPatt, wPatt;
    int open, end;
    uint8 bGoodAtt, wGoodAtt;
    uint8 Token;
    int v, positional;
    typePawnEval PawnInfo[1];
    int ch;
    PawnPointer = PawnHash + (Position->Dyn->PawnHash &(CurrentPHashSize - 1));
    PrefetchPawnHash;
    index = (Position->Dyn->material >> 8) & 0x7ffff;
    Token = Material[index].token;
    Position->Dyn->flags = Material[index].flags;
    Position->Dyn->exact = false;
    if (!(Position->Dyn->material & 0x80000000))
        matval = Material[index].Value;
    else
        {
        if (POPCNT(wBitboardQ) > 1 || POPCNT(bBitboardQ) > 1 || POPCNT(wBitboardR) > 2 || POPCNT(bBitboardR) > 2
           || POPCNT(wBitboardBL) > 1 || POPCNT(bBitboardBL) > 1 || POPCNT(wBitboardBD) > 1 || POPCNT(bBitboardBD) > 1
           || POPCNT(wBitboardN) > 2 || POPCNT(bBitboardN) > 2)
            {
            Token = 0x80;
            matval = MaterialValue(Position);
            Position->Dyn->flags = 0;
            if (wBitboardQ | wBitboardR | wBitboardB | wBitboardN)
                Position->Dyn->flags |= 2;
            if (bBitboardQ | bBitboardR | bBitboardB | bBitboardN)
                Position->Dyn->flags |= 1;
            }
        else
            {
            matval = Material[index].Value;
            Position->Dyn->material &= 0x7fffffff;
            }
        }
    if (((Position->Dyn->Hash ^ GetEvalHash(Position->Dyn->Hash)) & 0xffffffffffff0000) == 0)
        {
        Value = (int)((sint16)(GetEvalHash(Position->Dyn->Hash) & 0xffff));
        Position->Dyn->lazy = 0;
        Mobility(Position);
        Position->Dyn->PositionalValue = ((Position->wtm) ? Value : -Value) - matval;
        Position->Dyn->Value = Value;
        if (move && !(Position->Dyn - 1)->lazy)
            AdjustPositionalGain(Position, move);
        if (Value > ValueCut || Value < -ValueCut)
            Position->Dyn->exact = true;
        return;
        }

#ifdef RobboBases
    if(UseRobboBases && TripleBasesLoaded && SearchRobboBases
		&& depth >= TripleWeakProbeDepth && TripleCondition (Position))
        {
		int va;
		Mobility(Position);
		if (TripleValue (Position, &va, depth < TripleDefiniteProbeDepth &&
			Height (Position) > TripleDefiniteProbeHeight, false))
			{
			int c = POPCNT(Position->OccupiedBW);
			int molt = 21000 - (c << 7) - (Height(Position) << 6);

			if(va == 0)
				Position->Dyn->Value = 0;
			else
				{
				Position->Dyn->Value = (va > 0) ? molt : -molt;
				Position->Dyn->Value += (Position->wtm ? matval : -matval);
				}
			Position->Dyn->lazy = 1;
			Position->Dyn->PositionalValue = 0;
			GetEvalHash(Position->Dyn->Hash) =
				(Position->Dyn->Hash & 0xffffffffffff0000) | (Position->Dyn->Value & 0xffff);
			Position->Dyn->exact = true;
			return;
			}
		}
#endif

    memcpy(PawnInfo, PawnPointer, sizeof(typePawnEval));
    PawnInfo->PawnHash ^= (((uint64 *)(PawnInfo)) + 0x1)[0];
    PawnInfo->PawnHash ^= (((uint64 *)(PawnInfo)) + 0x2)[0];
    PawnInfo->PawnHash ^= (((uint64 *)(PawnInfo)) + 0x3)[0];
    if ((Position->Dyn->material & 0xff) == 0)
        {
        Position->wtm
           ? KingPawnWhite(Position, matval, Token, PawnInfo) : KingPawnBlack(Position, matval, Token, PawnInfo);
        Position->Dyn->lazy = 1;
        Position->Dyn->PositionalValue = 0;
        if (Position->Dyn->Value == 0)
            Position->Dyn->Value = 1;
        GetEvalHash(Position->Dyn->Hash) =
           (Position->Dyn->Hash & 0xffffffffffff0000) | (Position->Dyn->Value & 0xffff);
        return;
        }
    if ((Position->Dyn->flags & WhiteMinorOnly || Position->Dyn->flags & BlackMinorOnly)
       && PawnInfo->PawnHash != Position->Dyn->PawnHash)
        PawnEval(Position, PawnInfo);
    if ((Position->Dyn->flags & WhiteMinorOnly && PawnInfo->wPfile_count <= 1)
       || (Position->Dyn->flags & BlackMinorOnly && PawnInfo->bPfile_count <= 1)
	   || (Position->Dyn->flags & 128));
    else
        {
        if (Position->wtm)
            {
            positional = (Position->Dyn - 1)->PositionalValue;
            cp = Position->Dyn->cp;
            to = To(move);
            Value = PST(Position->sq[to], to) - PST(Position->sq[to], From(move));
            if (cp)
                Value -= PST(cp, to);
            phase = MIN(Position->Dyn->material & 0xff, 32);
            end = (sint16)(Value & 0xffff);
            open = (end < 0) + (sint16)((Value >> 16) & 0xffff);
            antiphase = 32 - phase;
            Value = (end * antiphase + open * phase) >> 5;
            positional += Value;
            v = positional + matval;
            if (v < -max - (int)((Position->Dyn - 1)->lazy << 4) || v > -min + (int)((Position->Dyn - 1)->lazy << 4))
                {
                Position->Dyn->lazy = (Position->Dyn - 1)->lazy + 1;
                Position->Dyn->Value = v;
                Position->Dyn->PositionalValue = positional;
                Mobility(Position);
                return;
                }
            }
        else
            {
            positional = (Position->Dyn - 1)->PositionalValue;
            cp = Position->Dyn->cp;
            to = To(move);
            Value = PST(Position->sq[to], to) - PST(Position->sq[to], From(move));
            if (cp)
                Value -= PST(cp, to);
            phase = MIN(Position->Dyn->material & 0xff, 32);
            end = (sint16)(Value & 0xffff);
            open = (end < 0) + (sint16)((Value >> 16) & 0xffff);
            antiphase = 32 - phase;
            Value = (end * antiphase + open * phase) >> 5;
            positional += Value;
            v = positional + matval;
            if (v < min - (int)((Position->Dyn - 1)->lazy << 4) || v > max + (int)((Position->Dyn - 1)->lazy << 4))
                {
                Position->Dyn->lazy = (Position->Dyn - 1)->lazy + 1;
                Position->Dyn->Value = -v;
                Position->Dyn->PositionalValue = positional;
                Mobility(Position);
                return;
                }
            }
        }
    wKs = Position->wKsq;
    bKs = Position->bKsq;
    wKatt = AttK[wKs];
    bKatt = AttK[bKs];
    bGoodAtt = wGoodAtt = 0;
    if (PawnInfo->PawnHash != Position->Dyn->PawnHash)
        PawnEval(Position, PawnInfo);
    Value = (PawnInfo->Score);
    end = (sint16)(Position->Dyn->Static & 0xffff);
    open = (end < 0) + (sint16)((Position->Dyn->Static >> 16) & 0xffff);
    open *= (PSTWeight << 10) / 100;
    open >>= 10;
    end *= (PSTWeight << 10) / 100;
    end >>= 10;
    Value += Score(open, end);
    Position->Dyn->wXray = 0;
    A = (wBitboardP &(~FileA)) << 7;
    T = A & bBitboardK;
    Position->Dyn->bKcheck = (T >> 7);
    wPatt = A;
    A = (wBitboardP &(~FileH)) << 9;
    T = A & bBitboardK;
    Position->Dyn->bKcheck |= (T >> 9);
    wPatt |= A;
    Position->Dyn->wAtt = wPatt;
    A = (bBitboardP &(~FileH)) >> 7;
    T = A & wBitboardK;
    Position->Dyn->wKcheck = (T << 7);
    bPatt = A;
    A = (bBitboardP &(~FileA)) >> 9;
    T = A & wBitboardK;
    Position->Dyn->wKcheck |= (T << 9);
    bPatt |= A;
    Position->Dyn->bAtt = bPatt;
    bOKxray = (~bBitboardP) &~wPatt;
    wOKxray = (~wBitboardP) &~bPatt;
    wGoodMinor = (wBitboardN | wBitboardB) & wPatt;
    bGoodMinor = (bBitboardN | bBitboardB) & bPatt;
    if (wPatt & bKatt)
        bKhit = HitP;
    else
        bKhit = 0;
    U = (Position->OccupiedBW >> 8) & wBitboardP;
	MobValue -= POPCNT(U) * PawnAntiMobility;
    wSafeMob = ~(bPatt | wBitboardOcc);
    U = wBitboardQ;
    while (U)
        {
        b = BSF(U);
        BitClear(b, U);
        AttB = AttB(b);
        AttR = AttR(b);
        if (bBitboardK & Diag[b])
            {
            T = AttB(bKs) & AttB;
            if (T)
                {
                Value += wQxrayD[Position->sq[BSF(T)]];
                Position->Dyn->wXray |= T;
                Position->XrayW[BSF(T)] = b;
                }
            }
        else if (bBitboardK & Ortho[b])
            {
            T = AttR(bKs) & AttR;
            if (T)
                {
                Value += wQxrayO[Position->sq[BSF(T)]];
                Position->Dyn->wXray |= T;
                Position->XrayW[BSF(T)] = b;
                }
            }
        A = AttB | AttR;
        T = A & wSafeMob;
        Position->Dyn->wAtt |= A;
        if (A & bKatt)
            bKhit += HitQ;
        if (A & bBitboardK)
            Position->Dyn->bKcheck |= SqSet[b];
        if (A & wKatt)
            Value += QguardK;
        MobValue += MobQ(T);
        if (A &(~bPatt) & bBitboardOcc)
            Value += Qatt;
        if (bBitboardP & AttPb[b])
            {
            Value -= PattQ;
            bGoodAtt += 1;
            }
        if (Rank(b) == R7)
            {
            if ((bBitboardP | bBitboardK) & Ranks78)
                {
                Value += Queen7th;
                if (wBitboardR & Rank7 & AttR && bBitboardK & Rank8)
                    Value += DoubQueen7th;
                }
            }
        }
    U = wBitboardR;
    while (U)
        {
        b = BSF(U);
        BitClear(b, U);
        A = AttR(b);
        Position->Dyn->wAtt |= A;
        if (bBitboardK & Ortho[b])
            {
            T = AttR(bKs) & A;
            if (T)
                {
                Value += wRxray[Position->sq[BSF(T)]];
                Position->Dyn->wXray |= T;
                Position->XrayW[BSF(T)] = b;
                }
            }

		if (A & bKatt)
            bKhit += HitR;
        if (A & bBitboardK)
            Position->Dyn->bKcheck |= SqSet[b];
        if (A & wKatt)
            Value += RguardK;
        MobValue += MobR(A & wOKxray);
        if (A &(~bPatt) & bBitboardP)
            Value += RattP;
        if (A &((bBitboardN | bBitboardB) & ~bPatt))
            Value += RattBN;
        if (A & bBitboardQ)
            {
            Value += RattQ;
            wGoodAtt += 1;
            }
        if (bBitboardP & AttPb[b])
            {
            Value -= PattR;
            bGoodAtt += 1;
            }

        if ((wBitboardP &OpenFileW[b]) == 0)
            {
            Value += RookHalfOpen;
            if ((bBitboardP & OpenFileW[b]) == 0)
                {
                T = bGoodMinor & OpenFileW[b];
                if (!T)
                    Value += RookOpenFile;
                else
                    {
                    int t = BSF(T);
                    if ((IsolatedFiles[File(t)] & InFrontB[Rank(t)] & wBitboardP) == 0)
                        Value += RookOpenFixedMinor;
                    else
                        Value += RookOpenMinor;
                    }
                }
            else
                {
                T = OpenFileW[b] & bBitboardP;
                if (T)
                    {
                    int t = BSF(T);
                    if ((IsolatedFiles[File(t)] & InFrontW[Rank(t)] & bBitboardP) == 0)
                        Value += RookHalfOpenPawn;
                    }
                }
            if (bBitboardK & OpenFileW[b])
                Value += RookHalfOpenKing;
            }
        if (SqSet[b] & wOutpost && (IsolatedFiles[File(b)] & InFrontW[Rank(b)] & bBitboardP) == 0)
            {
            if (wBitboardP & AttPw[b])
                {
                Value += OutpostRook;
                if (A &(bKatt | (bBitboardOcc & ~bPatt)) & RankArray[Rank(b)])
                    Value += OutpostRookGuarded;
                }
            }
        if (Rank(b) == R8)
            {
            if (bBitboardK & Rank8)
                Value += RookKing8th;
            }
        if (Rank(b) == R7)
            {
            if ((bBitboardP | bBitboardK) & Ranks78)
                {
                Value += Rook7thKingPawn;
                if (bBitboardK & Rank8 && (wBitboardQ | wBitboardR) & Rank7 & A)
                    Value += DoubRook7thKingPawn;
                }
            }
        if (Rank(b) == R6 && (bBitboardP | bBitboardK) & Ranks678)
            Value += Rook6thKingPawn;
        }
    wSafeMob |= bBitboardOcc ^ bBitboardP;
    U = wBitboardB;
    while (U)
        {
        b = BSF(U);
        BitClear(b, U);
        A = AttB(b);
        Position->Dyn->wAtt |= A;
        if (bBitboardK & Diag[b])
            {
            T = AttB(bKs) & A;
            if (T)
                {
                Value += wBxray[Position->sq[BSF(T)]];
                Position->Dyn->wXray |= T;
                Position->XrayW[BSF(T)] = b;
                }
            }

		if (A & bKatt)
            bKhit += HitB;
        if (A & bBitboardK)
            Position->Dyn->bKcheck |= SqSet[b];
        if (A & wKatt)
            Value += BguardK;
        MobValue += MobB(A & wSafeMob, InFrontW[Rank(b)]);
        if (A &(~bPatt) & bBitboardP)
            Value += bAttP;
        if (A &(~bPatt) & bBitboardN)
            Value += bAttN;
        if (A &(bBitboardR | bBitboardQ))
            {
            Value += bAttRQ;
            wGoodAtt += 1;
            }
        if (bBitboardP & AttPb[b])
            {
            Value -= PattB;
            bGoodAtt += 1;
            }
        if (SqSet[b] & White)
            {
            Value -= (PawnInfo->wPlight + (PawnInfo->bPlight >> 1)) * Score(1, 1);
            Value += POPCNT(bBitboardP & White & InFrontB[Rank(b)] & ~bPatt) * Score(0, 2);
            }
        else
            {
            Value -= (PawnInfo->wPdark + (PawnInfo->bPdark >> 1)) * Score(1, 1);
            Value += POPCNT(bBitboardP & Black & InFrontB[Rank(b)] & ~bPatt) * Score(0, 2);
            }
        if (SqSet[b] & wOutpost && (IsolatedFiles[File(b)] & InFrontW[Rank(b)] & bBitboardP) == 0)
            {
            if (wBitboardP & AttPw[b])
                {
                Value += OutpostBishop;
                if (A &(bKatt | (bBitboardOcc & ~bPatt)))
                    Value += OutpostBishopGuarded;
                }
            }
        if (Position->sq[BishopTrapSq[b]] == bEnumP)
            {
            Value -= BishopTrapValue;
            if (Position->sq[GoodBishopTrapSq[b]] == bEnumP)
                Value -= BishopTrapGuardValue;
            }
        }
    U = wBitboardN;
    while (U)
        {
        b = BSF(U);
        BitClear(b, U);
        A = AttN[b];
        Position->Dyn->wAtt |= A;
        if (A &(bKatt | bBitboardK))
            bKhit += HitN;
        if (A & bBitboardK)
            Position->Dyn->bKcheck |= SqSet[b];
        if (A &(wKatt | wBitboardK))
            Value += NguardK;
        MobValue += MobN(A & wSafeMob, InFrontW[Rank(b)]);
        if (Rank(b) <= R4 && Position->sq[b + 24] == bEnumP)
            Value -= Score(R5 - Rank(b), R5 - Rank(b));
        if (A &(~bPatt) & bBitboardP)
            Value += NattP;
        if (A &(~bPatt) & bBitboardB)
            Value += NattB;
        if (A &(bBitboardR | bBitboardQ))
            {
            Value += NattRQ;
            wGoodAtt += 1;
            }
        if (bBitboardP & AttPb[b])
            {
            Value -= PattN;
            bGoodAtt += 1;
            }
        if (SqSet[b] & wOutpost && (IsolatedFiles[File(b)] & InFrontW[Rank(b)] & bBitboardP) == 0)
            {
            Value += OutpostKnight;
            if (wBitboardP & AttPw[b])
                {
                Value += OutpostKnightPawn;
                if (A &(bKatt | (bBitboardOcc & ~bPatt)))
                    {
                    Value += OutpostKnightAttacks;
                    if (Rank(b) == R5)
                        Value += OutpostKnight5th;
                    if (File(b) == FD || File(b) == FE)
                        Value += OutpostKnightONde;
                    }
                }
            }
        }
    if (bPatt & wKatt)
        wKhit = HitP;
    else
        wKhit = 0;
    U = (Position->OccupiedBW << 8) & bBitboardP;
    Position->Dyn->bXray = 0;
	MobValue += POPCNT(U) * PawnAntiMobility;
    bSafeMob = ~(wPatt | bBitboardOcc);
    U = bBitboardQ;
    while (U)
        {
        b = BSF(U);
        BitClear(b, U);
        AttB = AttB(b);
        AttR = AttR(b);
        if (wBitboardK & Diag[b])
            {
            T = AttB(wKs) & AttB;
            if (T)
                {
                Value -= bQxrayD[Position->sq[BSF(T)]];
                Position->Dyn->bXray |= T;
                Position->XrayB[BSF(T)] = b;
                }
            }
        else if (wBitboardK & Ortho[b])
            {
            T = AttR(wKs) & AttR;
            if (T)
                {
                Value -= bQxrayO[Position->sq[BSF(T)]];
                Position->Dyn->bXray |= T;
                Position->XrayB[BSF(T)] = b;
                }
            }
        A = AttB | AttR;
        T = A & bSafeMob;
        Position->Dyn->bAtt |= A;
        if (A & wKatt)
            wKhit += HitQ;
        if (A & wBitboardK)
            Position->Dyn->wKcheck |= SqSet[b];
        if (A & bKatt)
            Value -= QguardK;
        MobValue -= MobQ(T);
        if (A &(~wPatt) & wBitboardOcc)
            Value -= Qatt;
        if (wBitboardP & AttPw[b])
            {
            Value += PattQ;
            wGoodAtt += 1;
            }
        if (Rank(b) == R2)
            {
            if ((wBitboardP | wBitboardK) & Ranks12)
                {
                Value -= Queen7th;
                if (bBitboardR & Rank2 & AttR && wBitboardK & Rank1)
                    Value -= DoubQueen7th;
                }
            }
        }
    U = bBitboardR;
    while (U)
        {
        b = BSF(U);
        BitClear(b, U);
        A = AttR(b);
        Position->Dyn->bAtt |= A;
        if (wBitboardK & Ortho[b])
            {
            T = A & AttR(wKs);
            if (T)
                {
                Value -= bRxray[Position->sq[BSF(T)]];
                Position->Dyn->bXray |= T;
                Position->XrayB[BSF(T)] = b;
                }
            }

        if (A & wKatt)
            wKhit += HitR;
        if (A & wBitboardK)
            Position->Dyn->wKcheck |= SqSet[b];
        if (A & bKatt)
            Value -= RguardK;
        MobValue -= MobR(A & bOKxray);
        if (A &(~wPatt) & wBitboardP)
            Value -= RattP;
        if (A &(wBitboardN | wBitboardB) & ~wPatt)
            Value -= RattBN;
        if (A & wBitboardQ)
            {
            Value -= RattQ;
            bGoodAtt += 1;
            }
        if (wBitboardP & AttPw[b])
            {
            Value += PattR;
            wGoodAtt += 1;
            }

        if ((bBitboardP & OpenFileB[b]) == 0)
            {
            Value -= RookHalfOpen;
            if ((wBitboardP & OpenFileB[b]) == 0)
                {
                T = wGoodMinor & OpenFileB[b];
                if (!T)
                    Value -= RookOpenFile;
                else
                    {
                    int t = BSR(T);
                    if ((IsolatedFiles[File(t)] & InFrontW[Rank(t)] & bBitboardP) == 0)
                        Value -= RookOpenFixedMinor;
                    else
                        Value -= RookOpenMinor;
                    }
                }
            else
                {
                T = OpenFileB[b] & wBitboardP;
                if (T)
                    {
                    int t = BSR(T);
                    if ((IsolatedFiles[File(t)] & InFrontB[Rank(t)] & wBitboardP) == 0)
                        Value -= RookHalfOpenPawn;
                    }
                }
            if (wBitboardK & OpenFileB[b])
                Value -= RookHalfOpenKing;
            }
        if (SqSet[b] & bOutpost && (IsolatedFiles[File(b)] & InFrontB[Rank(b)] & wBitboardP) == 0)
            {
            if (bBitboardP & AttPb[b])
                {
                Value -= OutpostRook;
                if (A &(wKatt | (wBitboardOcc & ~wPatt)) & RankArray[Rank(b)])
                    Value -= OutpostRookGuarded;
                }
            }
        if (Rank(b) == R1)
            {
            if (wBitboardK & Rank1)
                Value -= RookKing8th;
            }
        if (Rank(b) == R2)
            {
            if ((wBitboardP | wBitboardK) & Ranks12)
                {
                Value -= Rook7thKingPawn;
                if (wBitboardK & Rank1 && (bBitboardQ | bBitboardR) & Rank2 & A)
                    Value -= DoubRook7thKingPawn;
                }
            }
        if (Rank(b) == R3 && (wBitboardP | wBitboardK) & Ranks123)
            Value -= Rook6thKingPawn;
        }
    bSafeMob |= wBitboardOcc ^ wBitboardP;
    U = bBitboardB;
    while (U)
        {
        b = BSF(U);
        BitClear(b, U);
        A = AttB(b);
        Position->Dyn->bAtt |= A;
        if (wBitboardK & Diag[b])
            {
            T = A & AttB(wKs);
            if (T)
                {
                Value -= bBxray[Position->sq[BSF(T)]];
                Position->Dyn->bXray |= T;
                Position->XrayB[BSF(T)] = b;
                }
            }

        if (A & wKatt)
            wKhit += HitB;
        if (A & wBitboardK)
            Position->Dyn->wKcheck |= SqSet[b];
        if (A & bKatt)
            Value -= BguardK;
        MobValue -= MobB(A & bSafeMob, InFrontB[Rank(b)]);
        if (A &(~wPatt) & wBitboardP)
            Value -= bAttP;
        if (A &(~wPatt) & wBitboardN)
            Value -= bAttN;
        if (A &(wBitboardR | wBitboardQ))
            {
            Value -= bAttRQ;
            bGoodAtt += 1;
            }
        if (wBitboardP & AttPw[b])
            {
            Value += PattB;
            wGoodAtt += 1;
            }
        if (SqSet[b] & White)
            {
            Value += (PawnInfo->bPlight + (PawnInfo->wPlight >> 1)) * Score(1, 1);
            Value -= POPCNT(wBitboardP & White & InFrontW[Rank(b)] & ~wPatt) * Score(0, 2);
            }
        else
            {
            Value += (PawnInfo->bPdark + (PawnInfo->wPdark >> 1)) * Score(1, 1);
            Value -= POPCNT(wBitboardP & Black & InFrontW[Rank(b)] & ~wPatt) * Score(0, 2);
            }
        if (SqSet[b] & bOutpost && (IsolatedFiles[File(b)] & InFrontB[Rank(b)] & wBitboardP) == 0)
            {
            if (bBitboardP & AttPb[b])
                {
                Value -= OutpostBishop;
                if (A &(wKatt | (wBitboardOcc & ~wPatt)))
                    Value -= OutpostBishopGuarded;
                }
            }
        if (Position->sq[BishopTrapSq[b]] == wEnumP)
            {
            Value += BishopTrapValue;
            if (Position->sq[GoodBishopTrapSq[b]] == wEnumP)
                {
                Value += BishopTrapGuardValue;
                }
            }
        }
    U = bBitboardN;
    while (U)
        {
        b = BSF(U);
        BitClear(b, U);
        A = AttN[b];
        Position->Dyn->bAtt |= A;
        if (A &(wKatt | wBitboardK))
            wKhit += HitN;
        if (A & wBitboardK)
            Position->Dyn->wKcheck |= SqSet[b];
        if (A &(bKatt | bBitboardK))
            Value -= NguardK;
        MobValue -= MobN(A & bSafeMob, InFrontB[Rank(b)]);
        if (Rank(b) >= R5 && Position->sq[b - 24] == wEnumP)
            Value += Score(Rank(b) - R4, Rank(b) - R4);
        if (A &(~wPatt) & wBitboardP)
            Value -= NattP;
        if (A &(~wPatt) & wBitboardB)
            Value -= NattB;
        if (A &(wBitboardR | wBitboardQ))
            {
            Value -= NattRQ;
            bGoodAtt += 1;
            }
        if (wBitboardP & AttPw[b])
            {
            Value += PattN;
            wGoodAtt += 1;
            }
        if (SqSet[b] & bOutpost && (IsolatedFiles[File(b)] & InFrontB[Rank(b)] & wBitboardP) == 0)
            {
            Value -= OutpostKnight;
            if (bBitboardP & AttPb[b])
                {
                Value -= OutpostKnightPawn;
                if (A &(wKatt | (wBitboardOcc & ~wPatt)))
                    {
                    Value -= OutpostKnightAttacks;
                    if (Rank(b) == R4)
                        Value -= OutpostKnight5th;
                    if (File(b) == FD || File(b) == FE)
                        Value -= OutpostKnightONde;
                    }
                }
            }
        }
    end = (sint16)(MobValue & 0xffff);
    open = (end < 0) + (sint16)((MobValue >> 16) & 0xffff);
    open *= (MobilityWeight << 10) / 100;
    open >>= 10;
    end *= (MobilityWeight << 10) / 100;
    end >>= 10;
    MobValue = Score(open, end);
    Value += MobValue;
    Position->Dyn->wAtt |= wKatt;
    Position->Dyn->bAtt |= bKatt;
    if (bKatt & wBitboardK)
        {
        Position->Dyn->wKcheck |= SqSet[Position->bKsq];
        Position->Dyn->bKcheck |= SqSet[Position->wKsq];
        }
    if ((~Position->Dyn->bAtt) &wKatt & bBitboardP)
        Value += KingAttUnguardedPawn;
    T = RookTrapped[wKs] & wBitboardR;
    if (T)
        {
        int t = BSF(T);
        T = OpenFileW[t] & wBitboardP;
        if (T)
            {
            t = BSF(T);
            t >>= 3;
            Value -= Score(10 * (6 - t), 0);
            }
        }
    phase = MIN(Position->Dyn->material & 0xff, 32);
    if (wKatt & bKatt)
        wKhit += HitK;
    ch = (((KingSafetyMult[wKhit >> 16] * (wKhit & 0xffff)) / KingSafetyDivider) << 16) + PawnInfo->wKdanger;
    ch >>= 16;
    ch *= (KingSafetyWeight << 10) / 100;
    ch >>= 10;
    ch <<= 16;
    if (!bBitboardQ)
        {
        ch >>= 16;
        ch *= POPCNT(bBitboardR | bBitboardN | bBitboardB);
        ch >>= 3;
        ch <<= 16;
        }
    Position->Dyn->wKdanger = (uint8)(((uint32)(ch * phase)) >> 23);
    Value -= ch;
    if ((~Position->Dyn->wAtt) &bKatt & wBitboardP)
        {
        Value -= KingAttUnguardedPawn;
        }
    T = RookTrapped[bKs] & bBitboardR;
    if (T)
        {
        int t = BSR(T);
        T = OpenFileB[t] & bBitboardP;
        if (T)
            {
            t = BSR(T);
            t >>= 3;
            Value += Score(10 * (t - 1), 0);
            }
        }
    if (wKatt & bKatt)
        bKhit += HitK;
    ch = (((KingSafetyMult[bKhit >> 16] * (bKhit & 0xffff)) / KingSafetyDivider) << 16) + PawnInfo->bKdanger;
    ch >>= 16;
    ch *=  (KingSafetyWeight << 10) / 100;
    ch >>= 10;
    ch <<= 16;
    if (!wBitboardQ)
        {
        ch >>= 16;
        ch *= POPCNT(wBitboardR | wBitboardN | wBitboardB);
        ch >>= 3;
        ch <<= 16;
        }
    Position->Dyn->bKdanger = (uint8)(((uint32)(ch * phase)) >> 23);
    Value += ch;
    if (wGoodAtt >= 2)
        Value += MultipleAtt;
    if (bGoodAtt >= 2)
        Value -= MultipleAtt;

    if ((wBitboardR | wBitboardQ) & CrampFile[File(bKs)])
        {
        Value += Score(0, 5);
        if ((CrampFile[File(bKs)]&(wBitboardP | bBitboardP)) == 0)
            Value += Score(5, 15);
        }
    if ((bBitboardR | bBitboardQ) & CrampFile[File(wKs)])
        {
        Value -= Score(0, 5);
        if ((CrampFile[File(wKs)]&(bBitboardP | wBitboardP)) == 0)
            Value -= Score(5, 15);
        }
    U = PawnInfo->wPassedFiles;
    while (U)
        {
        b = BSR(FileArray[BSF(U)] & wBitboardP);
        BitClear(0, U);
        rank = Rank(b);
        if (rank <= R3)
            continue;
        if (RookEnd)
            {
            if (wBitboardR & OpenFileW[b])
                {
                if (rank == R7)
                    Value -= Rook7thEnd;
                else if (rank == R6)
                    Value -= Rook6thEnd;
                }
            if (OpenFileW[b] & wBitboardK && CrampFile[File(wKs)] & bBitboardR)
                Value -= Score(0, 1 << (rank - R2));
            }
        if (Position->sq[b + 8] == 0)
            Value += PassedPawnCanMove[rank];
        if ((OpenFileW[b] & wBitboardOcc) == 0)
            Value += PassedPawnMeClear[rank];
        if ((OpenFileW[b] & bBitboardOcc) == 0)
            Value += PassedPawnOppClear[rank];
        if ((OpenFileW[b]&(~Position->Dyn->wAtt) & Position->Dyn->bAtt) == 0)
            Value += PassedPawnIsFree[rank];
        if (QueenEnd)
            {
            if (rank == R7 && wBitboardQ & OpenFileW[b])
                Value -= Queen7thEnd;
            Value += RankQueenEnd[rank];
            }
        }
    U = PawnInfo->bPassedFiles;
    while (U)
        {
        b = BSF(FileArray[BSF(U)] & bBitboardP);
        BitClear(0, U);
        rank = Rank(b);
        if (rank >= R6)
            continue;
        if (RookEnd)
            {
            if (bBitboardR & OpenFileB[b])
                {
                if (rank == R2)
                    Value += Rook7thEnd;
                else if (rank == R3)
                    Value += Rook6thEnd;
                }
            if (OpenFileB[b] & bBitboardK && CrampFile[File(bKs)] & wBitboardR)
                Value += Score(0, 1 << (R7 - rank));
            }
        if (Position->sq[b - 8] == 0)
            Value -= PassedPawnCanMove[7 - rank];
        if ((OpenFileB[b] & bBitboardOcc) == 0)
            Value -= PassedPawnMeClear[7 - rank];
        if ((OpenFileB[b] & wBitboardOcc) == 0)
            Value -= PassedPawnOppClear[7 - rank];
        if ((OpenFileB[b] & Position->Dyn->wAtt & ~Position->Dyn->bAtt) == 0)
            Value -= PassedPawnIsFree[7 - rank];
        if (QueenEnd)
            {
            if (rank == R2 && bBitboardQ & OpenFileB[b])
                Value += Queen7thEnd;
            Value -= RankQueenEnd[7 - rank];
            }
        }
    end = (sint16)(Value & 0xffff);
    open = (end < 0) + (sint16)((Value >> 16) & 0xffff);
    antiphase = 32 - phase;
    Value = end * antiphase + open * phase;
    Value = ((Value >> 5) * PositionalWeight / 100) + matval;
    Value = (Value * Token) >> 7;
    if (Value > 0)
        Value -= ((PawnInfo->wDrawWeight * MIN(Value, 100)) >> 6) * DrawWeight / 100;
    else
        Value += ((PawnInfo->bDrawWeight * MIN(-Value, 100)) >> 6) * DrawWeight / 100;

    Value = EvalEnding(Position, Value, wPatt, bPatt);
    if (RandomCount)
        {
        uint32 r;
        int n, adj = 0;
        int mask = (1 << RandomBits) - 1;
        r = Random32(Position->cpu);
        for (n = 0; n < RandomCount; n++)
            {
            adj = r & mask;
            r >>= RandomBits;
            Value += (r & 1) ? adj : -adj;
            r >>= 1;
            }
        }
    Position->Dyn->Value = Position->wtm ? Value : -Value;
    Position->Dyn->PositionalValue = Value - matval;
    Position->Dyn->lazy = 0;
    GetEvalHash(Position->Dyn->Hash) = (Position->Dyn->Hash & 0xffffffffffff0000) | (Position->Dyn->Value & 0xffff);
    if (move && !(Position->Dyn - 1)->lazy)
        AdjustPositionalGain(Position, move);
    }
