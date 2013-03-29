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

#include "fire.h"
#include "pawn_eval.h"

typedef struct
    {
    uint64 Edge, Middle, Center;
    uint8 ShelterEdge[8], ShelterMiddle[8], ShelterCenter[8];
    uint8 StormEdge[8], StormMiddle[8], StormCenter[8], ShelterDiag[8], Zero, ValueZero;
    } typePawnPtr;

typePawnPtr PawnPtr[8];

const uint8 ShelterAA[8] =
    {
    30, 0, 5, 15, 20, 25, 25, 25
    };
const uint8 ShelterAB[8] =
    {
    55, 0, 15, 40, 50, 55, 55, 55
    };
const uint8 ShelterAC[8] =
    {
    30, 0, 10, 20, 25, 30, 30, 30
    };
const uint8 StormAA[8] =
    {
    5, 0, 35, 15, 5, 0, 0, 0
    };
const uint8 StormAB[8] =
    {
    10, 0, 50, 20, 10, 0, 0, 0
    };
const uint8 StormAC[8] =
    {
    10, 0, 50, 20, 10, 0, 0, 0
    };
const uint8 ShelterBA[8] =
    {
    30, 0, 5, 15, 20, 25, 25, 25
    };
const uint8 ShelterBB[8] =
    {
    55, 0, 15, 40, 50, 55, 55, 55
    };
const uint8 ShelterBC[8] =
    {
    30, 0, 10, 20, 25, 30, 30, 30
    };
const uint8 StormBA[8] =
    {
    5, 0, 35, 15, 5, 0, 0, 0
    };
const uint8 StormBB[8] =
    {
    10, 0, 50, 20, 10, 0, 0, 0
    };
const uint8 StormBC[8] =
    {
    10, 0, 50, 20, 10, 0, 0, 0
    };
const uint8 ShelterCB[8] =
    {
    30, 0, 5, 15, 20, 25, 25, 25
    };
const uint8 ShelterCC[8] =
    {
    55, 0, 15, 40, 50, 55, 55, 55
    };
const uint8 ShelterCD[8] =
    {
    30, 0, 10, 20, 25, 30, 30, 30
    };
const uint8 StormCB[8] =
    {
    5, 0, 35, 15, 5, 0, 0, 0
    };
const uint8 StormCC[8] =
    {
    10, 0, 50, 20, 10, 0, 0, 0
    };
const uint8 StormCD[8] =
    {
    10, 0, 50, 20, 10, 0, 0, 0
    };
const uint8 ShelterDC[8] =
    {
    30, 0, 5, 15, 20, 25, 25, 25
    };
const uint8 ShelterDD[8] =
    {
    55, 0, 15, 40, 50, 55, 55, 55
    };
const uint8 ShelterDE[8] =
    {
    30, 0, 10, 20, 25, 30, 30, 30
    };
const uint8 StormDC[8] =
    {
    5, 0, 35, 15, 5, 0, 0, 0
    };
const uint8 StormDD[8] =
    {
    10, 0, 50, 20, 10, 0, 0, 0
    };
const uint8 StormDE[8] =
    {
    10, 0, 50, 20, 10, 0, 0, 0
    };
const uint8 ShelterLongDiagA[8] =
    {
    10, 0, 2, 4, 6, 8, 10, 10
    };
const uint8 ShelterLongDiagB[8] =
    {
    8, 0, 2, 4, 6, 7, 8, 8
    };
const uint8 ShelterLongDiagC[8] =
    {
    6, 0, 2, 3, 4, 5, 6, 6
    };
const uint8 ShelterLongDiagD[8] =
    {
    4, 0, 1, 2, 3, 4, 4, 4
    };
void InitPawns()
    {
    int file, rank;
    int Target[8] =
        {
        FB, FB, FC, FD, FE, FF, FG, FG
        };

    int Switch[8] =
        {
        1, 1, 1, 1, -1, -1, -1, -1
        };

    for (file = FA; file <= FH; file++)
        {
        PawnPtr[file].Edge = FileArray[Target[file] - Switch[file]];
        PawnPtr[file].Middle = FileArray[Target[file]];
        PawnPtr[file].Center = FileArray[Target[file] + Switch[file]];
        }

    for (rank = R1; rank <= R8; rank++)
        {
        PawnPtr[FA].ShelterEdge[rank] = ShelterAA[rank];
        PawnPtr[FA].StormEdge[rank] = StormAA[rank];
        PawnPtr[FA].ShelterMiddle[rank] = ShelterAB[rank];
        PawnPtr[FA].StormMiddle[rank] = StormAB[rank];
        PawnPtr[FA].ShelterCenter[rank] = ShelterAC[rank];
        PawnPtr[FA].StormCenter[rank] = StormAC[rank];
        PawnPtr[FH].ShelterEdge[rank] = ShelterAA[rank];
        PawnPtr[FH].StormEdge[rank] = StormAA[rank];
        PawnPtr[FH].ShelterMiddle[rank] = ShelterAB[rank];
        PawnPtr[FH].StormMiddle[rank] = StormAB[rank];
        PawnPtr[FH].ShelterCenter[rank] = ShelterAC[rank];
        PawnPtr[FH].StormCenter[rank] = StormAC[rank];
        PawnPtr[FA].ShelterDiag[rank] = ShelterLongDiagA[rank];
        PawnPtr[FH].ShelterDiag[rank] = ShelterLongDiagA[rank];
        }

    for (rank = R1; rank <= R8; rank++)
        {
        PawnPtr[FB].ShelterEdge[rank] = ShelterBA[rank];
        PawnPtr[FB].StormEdge[rank] = StormBA[rank];
        PawnPtr[FB].ShelterMiddle[rank] = ShelterBB[rank];
        PawnPtr[FB].StormMiddle[rank] = StormBB[rank];
        PawnPtr[FB].ShelterCenter[rank] = ShelterBC[rank];
        PawnPtr[FB].StormCenter[rank] = StormBC[rank];
        PawnPtr[FG].ShelterEdge[rank] = ShelterBA[rank];
        PawnPtr[FG].StormEdge[rank] = StormBA[rank];
        PawnPtr[FG].ShelterMiddle[rank] = ShelterBB[rank];
        PawnPtr[FG].StormMiddle[rank] = StormBB[rank];
        PawnPtr[FG].ShelterCenter[rank] = ShelterBC[rank];
        PawnPtr[FG].StormCenter[rank] = StormBC[rank];
        PawnPtr[FB].ShelterDiag[rank] = ShelterLongDiagB[rank];
        PawnPtr[FG].ShelterDiag[rank] = ShelterLongDiagB[rank];
        }

    for (rank = R1; rank <= R8; rank++)
        {
        PawnPtr[FC].ShelterEdge[rank] = ShelterCB[rank];
        PawnPtr[FC].StormEdge[rank] = StormCB[rank];
        PawnPtr[FC].ShelterMiddle[rank] = ShelterCC[rank];
        PawnPtr[FC].StormMiddle[rank] = StormCC[rank];
        PawnPtr[FC].ShelterCenter[rank] = ShelterCD[rank];
        PawnPtr[FC].StormCenter[rank] = StormCD[rank];
        PawnPtr[FF].ShelterEdge[rank] = ShelterCB[rank];
        PawnPtr[FF].StormEdge[rank] = StormCB[rank];
        PawnPtr[FF].ShelterMiddle[rank] = ShelterCC[rank];
        PawnPtr[FF].StormMiddle[rank] = StormCC[rank];
        PawnPtr[FF].ShelterCenter[rank] = ShelterCD[rank];
        PawnPtr[FF].StormCenter[rank] = StormCD[rank];
        PawnPtr[FC].ShelterDiag[rank] = ShelterLongDiagC[rank];
        PawnPtr[FF].ShelterDiag[rank] = ShelterLongDiagC[rank];
        }

    for (rank = R1; rank <= R8; rank++)
        {
        PawnPtr[FD].ShelterEdge[rank] = ShelterDC[rank];
        PawnPtr[FD].StormEdge[rank] = StormDC[rank];
        PawnPtr[FD].ShelterMiddle[rank] = ShelterDD[rank];
        PawnPtr[FD].StormMiddle[rank] = StormDD[rank];
        PawnPtr[FD].ShelterCenter[rank] = ShelterDE[rank];
        PawnPtr[FD].StormCenter[rank] = StormDE[rank];
        PawnPtr[FE].ShelterEdge[rank] = ShelterDC[rank];
        PawnPtr[FE].StormEdge[rank] = StormDC[rank];
        PawnPtr[FE].ShelterMiddle[rank] = ShelterDD[rank];
        PawnPtr[FE].StormMiddle[rank] = StormDD[rank];
        PawnPtr[FE].ShelterCenter[rank] = ShelterDE[rank];
        PawnPtr[FE].StormCenter[rank] = StormDE[rank];
        PawnPtr[FD].ShelterDiag[rank] = ShelterLongDiagD[rank];
        PawnPtr[FE].ShelterDiag[rank] = ShelterLongDiagD[rank];
        }

    for (file = FA; file <= FH; file++)
        {
        PawnPtr[file].Zero = PawnPtr[file].ShelterEdge[R2] + PawnPtr[file].ShelterMiddle[R2] + PawnPtr[file].ShelterCenter[R2];
        PawnPtr[file].ValueZero = 10;
        }
    }
static int WhiteKingDanger(typePos *Position, int wKs)
    {
    int e, RankWa, RankWb, RankWc, RankBa, RankBb, RankBc, v, rank = Rank(wKs);
    uint64 T, A = wBitboardP & NotInFrontB[rank];
    typePawnPtr Z = PawnPtr[File(wKs)];
    T = A & Z.Edge;
    RankWa = BSF(T);
    if (!T)
        RankWa = 0;
    RankWa >>= 3;
    T = A & Z.Middle;
    RankWb = BSF(T);
    if (!T)
        RankWb = 0;
    RankWb >>= 3;
    T = A & Z.Center;
    RankWc = BSF(T);
    if (!T)
        RankWc = 0;
    RankWc >>= 3;
    T = bBitboardP & Z.Edge;
    RankBa = BSF(T);
    if (!T)
        RankBa = 0;
    RankBa >>= 3;
    T = bBitboardP & Z.Middle;
    RankBb = BSF(T);
    if (!T)
        RankBb = 0;
    RankBb >>= 3;
    T = bBitboardP & Z.Center;
    RankBc = BSF(T);
    if (!T)
        RankBc = 0;
    RankBc >>= 3;
    v = (Z.ShelterEdge)[RankWa]+(Z.ShelterMiddle)[RankWb]+(Z.ShelterCenter)[RankWc];
    if (v == Z.Zero)
        v = Z.ValueZero;
    T = A & LongDiag[wKs];
    e = BSF(T);
    if (!T)
        e = 0;
    e >>= 3;
    v += (Z.ShelterDiag)[e];
    e = (Z.StormEdge)[RankBa];
    if (RankBa == (RankWa + 1))
        e >>= 1;
    v += e;
    e = (Z.StormMiddle)[RankBb];
    if (RankBb == (RankWb + 1))
        e >>= 1;
    v += e;
    e = (Z.StormCenter)[RankBc];
    if (RankBc == (RankWc + 1))
        e >>= 1;
    v += e;
    return v;
    }
static int BlackKingDanger(typePos *Position, int bKs)
    {
    int e, RankWa, RankWb, RankWc, RankBa, RankBb, RankBc, v, rank = Rank(bKs);
    uint64 T, A = bBitboardP & NotInFrontW[rank];
    typePawnPtr Z = PawnPtr[File(bKs)];
    T = A & Z.Edge;
    RankBa = BSR(T);
    if (!T)
        RankBa = 56;
    RankBa >>= 3;
    RankBa = 7 - RankBa;
    T = A & Z.Middle;
    RankBb = BSR(T);
    if (!T)
        RankBb = 56;
    RankBb >>= 3;
    RankBb = 7 - RankBb;
    T = A & Z.Center;
    RankBc = BSR(T);
    if (!T)
        RankBc = 56;
    RankBc >>= 3;
    RankBc = 7 - RankBc;
    T = wBitboardP & Z.Edge;
    RankWa = BSR(T);
    if (!T)
        RankWa = 56;
    RankWa >>= 3;
    RankWa = 7 - RankWa;
    T = wBitboardP & Z.Middle;
    RankWb = BSR(T);
    if (!T)
        RankWb = 56;
    RankWb >>= 3;
    RankWb = 7 - RankWb;
    T = wBitboardP & Z.Center;
    RankWc = BSR(T);
    if (!T)
        RankWc = 56;
    RankWc >>= 3;
    RankWc = 7 - RankWc;
    v = (Z.ShelterEdge)[RankBa]+(Z.ShelterMiddle)[RankBb]+(Z.ShelterCenter)[RankBc];
    if (v == Z.Zero)
        v = Z.ValueZero;
    T = A & LongDiag[bKs];
    e = BSR(T);
    if (!T)
        e = 56;
    e >>= 3;
    e = 7 - e;
    v += (Z.ShelterDiag)[e];
    e = (Z.StormEdge)[RankWa];
    if (RankWa == (RankBa + 1))
        e >>= 1;
    v += e;
    e = (Z.StormMiddle)[RankWb];
    if (RankWb == (RankBb + 1))
        e >>= 1;
    v += e;
    e = (Z.StormCenter)[RankWc];
    if (RankWc == (RankBc + 1))
        e >>= 1;
    v += e;
    return v;
    }
void PawnEval(typePos *Position, typePawnEval *Result)
    {
    int c, Value = 0, B, DistanceWhiteKing, DistanceBlackKing, BestWhiteKingDistance, BestBlackKingDistance;
    int wKs = Position->wKsq, bKs = Position->bKsq;
    int b, rank, file, v, ValuePassedPawn;
	int v1, v2;
    uint64 T, U, V, Connected;
    typePawnEval *ptr;
    Result->wPlight = Result->bPlight = Result->wPdark = Result->bPdark = 0;
    Result->wKdanger = Result->bKdanger = 0;
    Result->wPassedFiles = Result->bPassedFiles = 0;
    BestBlackKingDistance = BestWhiteKingDistance = 30000;
    Connected = 0;
    c = 0;
    for (file = FA; file <= FH; file++)
        {
        if ((wBitboardP & FileArray[file]) == 0)
            c = 0;
        else
            {
            if (c == 0)
                Value -= Islands;
            c = 1;
            }
        }
    T = wBitboardP;
    while (T)
        {
        b = BSF(T);
        BitClear(b, T);
        rank = Rank(b);
        file = File(b);
        DistanceWhiteKing = WhiteKingPawnDistance(b, wKs);
        if (DistanceWhiteKing < BestWhiteKingDistance)
            BestWhiteKingDistance = DistanceWhiteKing;
        DistanceBlackKing = WhiteKingPawnDistance(b, bKs);
        if (DistanceBlackKing < BestBlackKingDistance)
            BestBlackKingDistance = DistanceBlackKing;
        if (SqSet[b] & White)
            {
            Result->wPlight += BlockedPawnValue[b];
            if (Position->sq[b + 8] == bEnumP)
                Result->wPlight += BlockedPawnValue[b];
            }
        else
            {
            Result->wPdark += BlockedPawnValue[b];
            if (Position->sq[b + 8] == bEnumP)
                Result->wPdark += BlockedPawnValue[b];
            }
        if (wBitboardP & Left2[b] && (wBitboardP & InFrontW[rank - 1] & FileArray[file - 1]) == 0)
            Value -= Hole;
        if ((wBitboardP | bBitboardP) & OpenFileW[b])
            {
            if (wBitboardP & Doubled[b])
                {
                Value -= DoubledClosed;
                if ((wBitboardP & IsolatedFiles[file]) == 0)
                    {
                    Value -= DoubledClosedIsolated;
                    }
                }
            if ((wBitboardP & IsolatedFiles[file]) == 0)
                {
                Value -= IsolatedClosed;
                continue;
                }
            if ((wBitboardP & ProtectedPawnW[b]) == 0)
                {
                B = b + 8;
                if ((wBitboardP & AttPb[b]) == 0)
                    {
                    B += 8;
                    if ((wBitboardP & AttPb[b + 8]) == 0)
                        B += 8;
                    }
                if (bBitboardP & AttPb[B])
                    Value -= BackwardClosed;
                }
            continue;
            }
        if (wBitboardP & Doubled[b])
            {
            Value -= DoubledOpen;
            if ((wBitboardP & IsolatedFiles[file]) == 0)
                Value -= DoubledOpenIsolated;
            }
        if ((wBitboardP & IsolatedFiles[file]) == 0)
            Value -= IsolatedOpen;
        else
            {
            if ((wBitboardP & ProtectedPawnW[b]) == 0)
                {
                B = b + 8;
                if ((wBitboardP & AttPb[b]) == 0)
                    {
                    B += 8;
                    if ((wBitboardP & AttPb[b + 8]) == 0)
                        B += 8;
                    }
                if (bBitboardP & AttPb[B])
                    Value -= BackwardOpen;
                }
            }
        if ((bBitboardP & PassedPawnW[b]) == 0)
            goto PassedW;
        if (bBitboardP & PassedPawnW[b] & ~AttPb[b])
            {
            Value += CandidatePawnValue[rank];
            continue;
            }
        if (POPCNT(AttPb[b] & bBitboardP) > POPCNT(AttPw[b] & wBitboardP))
            {
            Value += CandidatePawnValue[rank];
            continue;
            }
        PassedW:
        ValuePassedPawn = PassedPawnValue[rank];
        if (wBitboardP & AttPw[b])
            ValuePassedPawn += ProtectedPassedPawnValue[rank];
        if ((bBitboardP & FilesLeft[file]) == 0 || (bBitboardP & FilesRight[file]) == 0)
            ValuePassedPawn += OutsidePassedPawnValue[rank];
        V = ConnectedPawns[b] & Connected;
        Connected |= SqSet[b];
        if (V)
            {
            ValuePassedPawn += ConnectedPassedPawnValue[rank] + ConnectedPassedPawnValue[Rank(BSF(V))];
            BitClear(0, V);
            if (V)
                ValuePassedPawn += ConnectedPassedPawnValue[rank] + ConnectedPassedPawnValue[Rank(BSF(V))];
            }
        Value += ValuePassedPawn;
        Result->wPassedFiles |= (uint8)(1 << file);
        if (b <= H3)
            continue;
        Value += (WhiteKingPawnDistance(b + 8, bKs) * OppKingPawnDistance[Rank(b)]);
        Value -= (WhiteKingPawnDistance(b + 8, wKs) * MyKingPawnDistance[Rank(b)]);
        }
    c = 0;
    for (file = FA; file <= FH; file++)
        {
        if ((bBitboardP & FileArray[file]) == 0)
            c = 0;
        else
            {
            if (c == 0)
                Value += Islands;
            c = 1;
            }
        }
    Connected = 0;
    T = bBitboardP;
    while (T)
        {
        b = BSF(T);
        BitClear(b, T);
        rank = Rank(b);
        file = File(b);
        DistanceBlackKing = BlackKingPawnDistance(b, bKs);
        if (DistanceBlackKing < BestBlackKingDistance)
            BestBlackKingDistance = DistanceBlackKing;
        DistanceWhiteKing = BlackKingPawnDistance(b, wKs);
        if (DistanceWhiteKing < BestWhiteKingDistance)
            BestWhiteKingDistance = DistanceWhiteKing;
        if (SqSet[b] & White)
            {
            Result->bPlight += BlockedPawnValue[b];
            if (Position->sq[b - 8] == wEnumP)
                Result->bPlight += BlockedPawnValue[b];
            }
        else
            {
            Result->bPdark += BlockedPawnValue[b];
            if (Position->sq[b - 8] == wEnumP)
                Result->bPdark += BlockedPawnValue[b];
            }
        if (bBitboardP & Left2[b] && (bBitboardP & InFrontB[rank + 1] & FileArray[file - 1]) == 0)
            {
            Value += Hole;
            }
        if ((wBitboardP | bBitboardP) & OpenFileB[b])
            {
            if (bBitboardP & Doubled[b])
                {
                Value += DoubledClosed;
                if ((bBitboardP & IsolatedFiles[file]) == 0)
                    Value += DoubledClosedIsolated;
                }
            if ((bBitboardP & IsolatedFiles[file]) == 0)
                {
                Value += IsolatedClosed;
                continue;
                }
            if ((bBitboardP & ProtectedPawnB[b]) == 0)
                {
                B = b - 8;
                if ((bBitboardP & AttPw[b]) == 0)
                    {
                    B -= 8;
                    if ((bBitboardP & AttPw[b - 8]) == 0)
                        B -= 8;
                    }
                if (wBitboardP & AttPw[B])
                    Value += BackwardClosed;
                }
            continue;
            }
        if (bBitboardP & Doubled[b])
            {
            Value += DoubledOpen;
            if ((bBitboardP & IsolatedFiles[file]) == 0)
                {
                Value += DoubledOpenIsolated;
                }
            }
        if ((bBitboardP & IsolatedFiles[file]) == 0)
            Value += IsolatedOpen;
        else
            {
            if ((bBitboardP & ProtectedPawnB[b]) == 0)
                {
                B = b - 8;
                if ((bBitboardP & AttPw[b]) == 0)
                    {
                    B -= 8;
                    if ((bBitboardP & AttPw[b - 8]) == 0)
                        B -= 8;
                    }
                if (wBitboardP & AttPw[B])
                    Value += BackwardOpen;
                }
            }
        if ((wBitboardP & PassedPawnB[b]) == 0)
            goto PassedB;
        if (wBitboardP & PassedPawnB[b] & ~AttPw[b])
            {
            Value -= CandidatePawnValue[7 - rank];
            continue;
            }
        if (POPCNT(AttPw[b] & wBitboardP) > POPCNT(AttPb[b] & bBitboardP))
            {
            Value -= CandidatePawnValue[7 - rank];
            continue;
            }
        PassedB:
        ValuePassedPawn = PassedPawnValue[7 - rank];
        if (bBitboardP & AttPb[b])
            ValuePassedPawn += ProtectedPassedPawnValue[7 - rank];
        if ((wBitboardP & FilesLeft[file]) == 0 || (wBitboardP & FilesRight[file]) == 0)
            ValuePassedPawn += OutsidePassedPawnValue[7 - rank];
        V = ConnectedPawns[b] & Connected;
        Connected |= SqSet[b];
        if (V)
            {
            ValuePassedPawn += ConnectedPassedPawnValue[7 - rank] + ConnectedPassedPawnValue[7 - (BSF(V) >> 3)];
            BitClear(0, V);
            if (V)
                ValuePassedPawn += ConnectedPassedPawnValue[7 - rank] + ConnectedPassedPawnValue[7 - (BSF(V) >> 3)];
            }
        Value -= ValuePassedPawn;
        Result->bPassedFiles |= (uint8)(1 << file);
        if (b >= A6)
            continue;
        Value -= (BlackKingPawnDistance(b - 8, wKs) * OppKingPawnDistance[R8 - Rank(b)]);
        Value += (BlackKingPawnDistance(b - 8, bKs) * MyKingPawnDistance[R8 - Rank(b)]);
        }
    T = 0;
    for (rank = R2; rank <= R7; rank++)
        T |= ((wBitboardP >> ((rank << 3))) & 0xff);
    U = 0;
    for (rank = R2; rank <= R7; rank++)
        U |= ((bBitboardP >> ((rank << 3))) & 0xff);
    Result->wPfile_count = POPCNT(T);
    Result->bPfile_count = POPCNT(U);
    Result->OpenFileCount = 8 - POPCNT(T | U);
    Result->wDrawWeight = OpposingPawnsMult[POPCNT(T & ~U)] * PawnCountMult[Result->wPfile_count];
    Result->bDrawWeight = OpposingPawnsMult[POPCNT(U & ~T)] * PawnCountMult[Result->bPfile_count];
    if (wBitboardP | bBitboardP)
        Value += BestBlackKingDistance - BestWhiteKingDistance;
    T = ((bBitboardP &(~FileA)) >> 9) | ((bBitboardP &(~FileH)) >> 7);
    if ((~T) &AttK[wKs] & bBitboardP)
        Value += KingAttPawn;
    if (Position->Dyn->oo & 1)
        Value += KingOO;
    if (Position->Dyn->oo & 2)
        Value += KingOOO;
    T = ((wBitboardP &(~FileA)) << 7) | ((wBitboardP &(~FileH)) << 9);
    if ((~T) &AttK[bKs] & wBitboardP)
        Value -= KingAttPawn;
    if (Position->Dyn->oo & 4)
        Value -= KingOO;
    if (Position->Dyn->oo & 8)
        Value -= KingOOO;
	v1 = (sint16)(Value & 0xffff);
	v2 = (v1 < 0) + (sint16)((Value >> 16) & 0xffff);
	v1 *= (PawnWeight << 10) / 100;
	v1 >>= 10;
	v2 *= (PawnWeight << 10) / 100;
	v2 >>= 10;
	Value = Score(v2, v1);
    Result->Score = Value;
    v = WhiteKingDanger(Position, wKs);
    if (WhiteOO)
        v = MIN(v, 5 + WhiteKingDanger(Position, G1));
    if (WhiteOOO)
        v = MIN(v, 5 + WhiteKingDanger(Position, C1));
    Result->wKdanger = Score(v, 0);
    v = BlackKingDanger(Position, bKs);
    if (BlackOO)
        v = MIN(v, 5 + BlackKingDanger(Position, G8));
    if (BlackOOO)
        v = MIN(v, 5 + BlackKingDanger(Position, C8));
    Result->bKdanger = Score(v, 0);
    Result->PawnHash = Position->Dyn->PawnHash;
    Result->PawnHash ^= (((uint64 *)(Result)) + 0x1)[0];
    Result->PawnHash ^= (((uint64 *)(Result)) + 0x2)[0];
    Result->PawnHash ^= (((uint64 *)(Result)) + 0x3)[0];
    ptr = PawnHash + (Position->Dyn->PawnHash &(CurrentPHashSize - 1));
    memcpy(ptr, Result, sizeof(typePawnEval));
    Result->PawnHash ^= (((uint64 *)(Result)) + 0x1)[0];
    Result->PawnHash ^= (((uint64 *)(Result)) + 0x2)[0];
    Result->PawnHash ^= (((uint64 *)(Result)) + 0x3)[0];
    return;
    }
