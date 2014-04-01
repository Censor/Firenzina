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
*/

#ifndef move_gen
#define move_gen
#include "fire.h"
#define Add(L, x, y) { (L++)->move = (x) | (y);}
#define AddTo(T, w)                        \
  	{ while (T)                              \
		{ to = BSF(T); c = Position->sq[to]; \
   Add (List, (sq << 6) | to, w);   BitClear (to, T); } }
#include "init_gen.h"
#define PtoQ (0xd8 << 24)
#define PtoN (0xc2 << 24)
#define FlagCheck 0x8000
#define MoveAdd(L, x, pi, to, check)                                \
	{ (L++)->move = (x) | ((SqSet[to] & (check)) ? FlagCheck : 0)  \
		| (HistoryPITo (Position, (pi), (to)) << 16); }
#define MovesTo(T, pi, check)                                       \
	{ while (T)                                                       \
		{ to = BSF(T); MoveAdd (List, (sq << 6) | to, pi, to, check); \
   BitClear (to, T); } }
#define UnderPromWhite()                                            \
	{ if ((AttN[to] & bBitboardK) == 0)                              \
		MoveAdd (List, FlagPromN | (sq << 6) | to, wEnumP, to, 0);   \
		MoveAdd (List, FlagPromR | (sq << 6) | to, wEnumP, to, 0);      \
		MoveAdd (List, FlagPromB | (sq << 6) | to, wEnumP, to, 0); }
#define UnderPromBlack()                                            \
	{ if ((AttN[to] & wBitboardK) == 0)                              \
		MoveAdd (List, FlagPromN | (sq << 6) | to, bEnumP, to, 0);    \
		MoveAdd (List, FlagPromR | (sq << 6) | to, bEnumP, to, 0);      \
		MoveAdd (List, FlagPromB | (sq << 6) | to, bEnumP, to, 0); }
#define OK(x)                                                       \
	(((x & 0x7fff) != s1) && ((x & 0x7fff) != s2) && ((x & 0x7fff) != s3))
void SortOrdinary(typeMoveList *m1, typeMoveList *m2, uint32 s1, uint32 s2, uint32 s3)
    {
    typeMoveList *p, *q;
    int move;
    if (m1 == m2)
        return;
    for (p = m2 - 1; p >= m1; p--)
        {
        if (OK(p->move))
            break;
        p->move = 0;
        }
    while (p > m1)
        {
        p--;
        move = p->move;
        if (OK(move))
            {
            for (q = p + 1; q < m2; q++)
                {
                if (move < q->move)
                    (q - 1)->move = q->move;
                else
                    break;
                }
            q--;
            q->move = move;
            }
        else
            {
            m2--;
            for (q = p; q < m2; q++)
                q->move = (q + 1)->move;
            m2->move = 0;
            }
        }
    }
#define AddGain(L, x, pi, to)                          \
	{ int v = ((int) MaxPosGain ((pi), (x) & 07777)); \
    if (v >= av) (L++)->move = (x) | (v << 16); }
#define AddGainTo(T, pi)                               \
  { while (T)                                          \
      { to = BSF(T);                                   \
   AddGain (List, (sq << 6) | to, pi, to);   BitClear(to, T); } }
#define Sort                                           \
  for (p = List - 1; p >= sm; p--)                     \
    { move = p->move;                                  \
      for (q = p + 1; q < List; q++)                   \
   {                                                   \
     if (move < q->move) (q - 1)->move = q->move;    \
     else break;                                       \
   }                                                   \
      q--;                                             \
      q->move = move; }
typeMoveList *EvasionMoves(typePos *Position, typeMoveList *list, uint64 mask)
    {
    if (Position->wtm)
        return WhiteEvasions(Position, list, mask);
    return BlackEvasions(Position, list, mask);
    }
typeMoveList *OrdinaryMoves(typePos *Position, typeMoveList *list)
    {
    if (Position->wtm)
        return WhiteOrdinary(Position, list);
    return BlackOrdinary(Position, list);
    }
typeMoveList *CaptureMoves(typePos *Position, typeMoveList *list, uint64 mask)
    {
    if (Position->wtm)
        return WhiteCaptures(Position, list, mask & bBitboardOcc);
    return BlackCaptures(Position, list, mask & wBitboardOcc);
    }
#include "move_gen.c"
#include "white.h"
#else
#include "black.h"
#endif
typeMoveList *MyEvasion(typePos *Position, typeMoveList *List, uint64 c2)
    {
    uint64 U, T, att, mask;
    int sq, to, fr, c, king, pi;
    king = MyKingSq;
    att = MyKingCheck;
    sq = BSF(att);
    pi = Position->sq[sq];
    mask = (~OppAttacked) &(((pi == EnumOppP) ? AttK[king] : 0) | Evade(king, sq)) & (~MyOccupied) &c2;
    BitClear(sq, att);
    if (att)
        {
        sq = BSF(att);
        pi = Position->sq[sq];
        mask = mask &(PieceIsOppPawn(pi) | Evade(king, sq));
        sq = king;
        AddTo(mask, CaptureValue[EnumMyK][c]);
        List->move = 0;
        return List;
        }
    c2 &= InterPose(king, sq);
    sq = king;
    AddTo(mask, CaptureValue[EnumMyK][c]);
    if (!c2)
        {
        List->move = 0;
        return List;
        }
    if (CaptureRight &(c2 & OppOccupied))
        {
        to = BSF(c2 & OppOccupied);
        c = Position->sq[to];
        if (EighthRank(to))
            {
            Add(List, FlagPromQ | FromLeft(to) | to, (0x20 << 24) + CaptureValue[EnumMyP][c]);
            Add(List, FlagPromN | FromLeft(to) | to, 0);
            Add(List, FlagPromR | FromLeft(to) | to, 0);
            Add(List, FlagPromB | FromLeft(to) | to, 0);
            }
        else
            Add(List, FromLeft(to) | to, CaptureValue[EnumMyP][c]);
        }
    if (CaptureLeft &(c2 & OppOccupied))
        {
        to = BSF(c2 & OppOccupied);
        c = Position->sq[to];
        if (EighthRank(to))
            {
            Add(List, FlagPromQ | FromRight(to) | to, (0x20 << 24) + CaptureValue[EnumMyP][c]);
            Add(List, FlagPromN | FromRight(to) | to, 0);
            Add(List, FlagPromR | FromRight(to) | to, 0);
            Add(List, FlagPromB | FromRight(to) | to, 0);
            }
        else
            Add(List, FromRight(to) | to, CaptureValue[EnumMyP][c]);
        }
    to = Position->Dyn->ep;
    if (to)
        {
        if (CaptureRight & SqSet[to] && SqSet[Backward(to)] & c2)
            Add(List, FlagEP | FromLeft(to) | to, CaptureValue[EnumMyP][EnumOppP]);
        if (CaptureLeft & SqSet[to] && SqSet[Backward(to)] & c2)
            Add(List, FlagEP | FromRight(to) | to, CaptureValue[EnumMyP][EnumOppP]);
        }
    T = BitboardMyP & BackShift((c2 & OppOccupied) ^ c2);
    while (T)
        {
        fr = BSF(T);
        BitClear(fr, T);
        if (SeventhRank(fr))
            {
            Add(List, FlagPromQ | (fr << 6) | Forward(fr), CaptureValue[EnumMyP][0]);
            Add(List, FlagPromN | (fr << 6) | Forward(fr), 0);
            Add(List, FlagPromR | (fr << 6) | Forward(fr), 0);
            Add(List, FlagPromB | (fr << 6) | Forward(fr), 0);
            }
        else
            Add(List, (fr << 6) | Forward(fr), CaptureValue[EnumMyP][0]);
        }
    T = BitboardMyP & BackShift2((c2 & OppOccupied) ^ c2) & SecondRank & BackShift(~Position->OccupiedBW);
    while (T)
        {
        fr = BSF(T);
        BitClear(fr, T);
        Add(List, (fr << 6) | Forward2(fr), CaptureValue[EnumMyP][0]);
        }
    for (U = BitboardMyN; U; BitClear(sq, U))
        {
        sq = BSF(U);
        T = AttN[sq] & c2;
        AddTo(T, CaptureValue[EnumMyN][c]);
        }
    for (U = BitboardMyB; U; BitClear(sq, U))
        {
        sq = BSF(U);
        T = AttB(sq) & c2;
        AddTo(T, CaptureValue[EnumMyBL][c]);
        }
    for (U = BitboardMyR; U; BitClear(sq, U))
        {
        sq = BSF(U);
        T = AttR(sq) & c2;
        AddTo(T, CaptureValue[EnumMyR][c]);
        }
    for (U = BitboardMyQ; U; BitClear(sq, U))
        {
        sq = BSF(U);
        T = AttQ(sq) & c2;
        AddTo(T, CaptureValue[EnumMyQ][c]);
        }
    List->move = 0;
    return List;
    }
typeMoveList *MyPositionalGain(typePos *Position, typeMoveList *List, int av)
    {
    uint64 empty = ~Position->OccupiedBW, U, T;
    int to, sq;
    typeMoveList *sm, *p, *q;
    int move;
    sm = List;
    for (U = ForwardShift(BitboardMyP & SecondSixthRanks) & empty; U; BitClear(sq, U))
        {
        to = BSF(U);
        if (OnThirdRank(to) && Position->sq[Forward(to)] == 0)
            AddGain(List, (Backward(to) << 6) | Forward(to), EnumMyP, Forward(to));
        AddGain(List, (Backward(to) << 6) | to, EnumMyP, to);
        }
    for (U = BitboardMyN; U; BitClear(sq, U))
        {
        sq = BSF(U);
        T = AttN[sq] & empty;
        AddGainTo(T, EnumMyN);
        }
    for (U = BitboardMyBL; U; BitClear(sq, U))
        {
        sq = BSF(U);
        T = AttB(sq) & empty;
        AddGainTo(T, EnumMyBL);
        }
    for (U = BitboardMyBD; U; BitClear(sq, U))
        {
        sq = BSF(U);
        T = AttB(sq) & empty;
        AddGainTo(T, EnumMyBD);
        }
    for (U = BitboardMyR; U; BitClear(sq, U))
        {
        sq = BSF(U);
        T = AttR(sq) & empty;
        AddGainTo(T, EnumMyR);
        }
    for (U = BitboardMyQ; U; BitClear(sq, U))
        {
        sq = BSF(U);
        T = AttQ(sq) & empty;
        AddGainTo(T, EnumMyQ);
        }
    sq = MyKingSq;
    T = AttK[sq] & empty &(~OppAttacked);
    AddGainTo(T, EnumMyK);
    List->move = 0;
    Sort;
    return List;
    }
typeMoveList *MyCapture(typePos *Position, typeMoveList *List, uint64 mask)
    {
    uint64 U, T, AttR, AttB;
    int sq, to, c;
    to = Position->Dyn->ep;
    if (to)
        {
        if (CaptureLeft & SqSet[to])
            Add(List, FlagEP | FromRight(to) | to, CaptureEP);
        if (CaptureRight & SqSet[to])
            Add(List, FlagEP | FromLeft(to) | to, CaptureEP);
        }
    if ((mask & MyAttacked) == 0)
        goto NoTarget;
    T = CaptureLeft &(~BitBoardEighthRank) & mask;
    while (T)
        {
        to = BSF(T);
        c = Position->sq[to];
        Add(List, FromRight(to) | to, CaptureValue[EnumMyP][c]);
        BitClear(to, T);
        }
    T = CaptureRight &(~BitBoardEighthRank) & mask;
    while (T)
        {
        to = BSF(T);
        c = Position->sq[to];
        Add(List, FromLeft(to) | to, CaptureValue[EnumMyP][c]);
        BitClear(to, T);
        }
    for (U = BitboardMyN; U; BitClear(sq, U))
        {
        sq = BSF(U);
        T = AttN[sq] & mask;
        AddTo(T, CaptureValue[EnumMyN][c]);
        }
    for (U = BitboardMyB; U; BitClear(sq, U))
        {
        sq = BSF(U);
        AttB = AttB(sq);
        T = AttB & mask;
        AddTo(T, CaptureValue[EnumMyBL][c]);
        }
    for (U = BitboardMyR; U; BitClear(sq, U))
        {
        sq = BSF(U);
        AttR = AttR(sq);
        T = AttR & mask;
        AddTo(T, CaptureValue[EnumMyR][c]);
        }
    for (U = BitboardMyQ; U; BitClear(sq, U))
        {
        sq = BSF(U);
        AttR = AttR(sq);
        AttB = AttB(sq);
        T = (AttB | AttR) & mask;
        AddTo(T, CaptureValue[EnumMyQ][c]);
        }
    sq = BSF(BitboardMyK);
    T = AttK[sq] & mask &(~OppAttacked);
    AddTo(T, CaptureValue[EnumMyK][c]);
    NoTarget:
    for (U = BitboardMyP & BitBoardSeventhRank; U; BitClear(sq, U))
        {
        sq = BSF(U);
        to = Forward(sq);
        if (Position->sq[to] == 0)
            {
            Add(List, FlagPromQ | (sq << 6) | to, PtoQ);
            if (AttN[to] & BitboardOppK)
                Add(List, FlagPromN | (sq << 6) | to, PtoN);
            }
        to = ForwardLeft(sq);
        if (sq != WhiteA7 && SqSet[to] & mask)
            {
            c = Position->sq[to];
            Add(List, FlagPromQ | (sq << 6) | to, PromQueenCap);
            if (AttN[to] & BitboardOppK)
                Add(List, FlagPromN | (sq << 6) | to, PromKnightCap);
            }
        to = ForwardRight(sq);
        if (sq != WhiteH7 && SqSet[to] & mask)
            {
            c = Position->sq[to];
            Add(List, FlagPromQ | (sq << 6) | to, PromQueenCap);
            if (AttN[to] & BitboardOppK)
                Add(List, FlagPromN | (sq << 6) | to, PromKnightCap);
            }
        }
    List->move = 0;
    return List;
    }
typeMoveList *MyOrdinary(typePos *Position, typeMoveList *List)
    {
    uint64 empty = ~Position->OccupiedBW, U, T, Rook, Bishop, Pawn;
    int to, sq, opks = OppKingSq;

#ifdef FischerRandom
    if (Chess960)
        {
        if (CastleOO)
            {
            uint64 T = 0;
            int f;

            for ( f = Chess960KingFile; f <= FG; f++ )
                T |= SqSet[f + 8 * NumberRank1];

            if (T & OppAttacked)
                goto NO_OO;
            T = 0;

            for ( f = Chess960KingRookFile + 1; f <= FF; f++ )
                T |= SqSet[f + 8 * NumberRank1];

            for ( f = Chess960KingRookFile - 1; f >= FF; f-- )
                T |= SqSet[f + 8 * NumberRank1];

            for ( f = Chess960KingFile + 1; f <= FG; f++ )
                T |= SqSet[f + 8 * NumberRank1];
            T &= SqClear[Chess960KingFile + 8 * NumberRank1];
            T &= SqClear[Chess960KingRookFile + 8 * NumberRank1];

            if (T & Position->OccupiedBW)
                goto NO_OO;
            MoveAdd(List,
                FlagOO | (Chess960KingRookFile + 8 * NumberRank1) | ((Chess960KingFile + 8 * NumberRank1) << 6),
                EnumMyK, Chess960KingRookFile + 8 * NumberRank1, 0);
            NO_OO:
            ;
            }
        if (CastleOOO)
            {
            uint64 T = 0;
            int f;

            for ( f = Chess960KingFile; f <= FC; f++ )
                T |= SqSet[f + 8 * NumberRank1];

            for ( f = Chess960KingFile; f >= FC; f-- )
                T |= SqSet[f + 8 * NumberRank1];

            if (T & OppAttacked)
                goto NO_OOO;
            T = 0;

            for ( f = Chess960QueenRookFile + 1; f <= FD; f++ )
                T |= SqSet[f + 8 * NumberRank1];

            for ( f = Chess960QueenRookFile - 1; f >= FD; f-- )
                T |= SqSet[f + 8 * NumberRank1];

            for ( f = Chess960KingFile + 1; f <= FC; f++ )
                T |= SqSet[f + 8 * NumberRank1];

            for ( f = Chess960KingFile - 1; f >= FC; f-- )
                T |= SqSet[f + 8 * NumberRank1];
            T &= SqClear[Chess960KingFile + 8 * NumberRank1];
            T &= SqClear[Chess960QueenRookFile + 8 * NumberRank1];

            if (T & Position->OccupiedBW)
                goto NO_OOO;
            MoveAdd(List,
                FlagOO | (Chess960QueenRookFile + 8 * NumberRank1) | ((Chess960KingFile + 8 * NumberRank1) << 6),
                EnumMyK, Chess960QueenRookFile + 8 * NumberRank1, 0);
            NO_OOO:
            ;
            }
        }
    else
        {
	if (CastleOO && ((Position->OccupiedBW | OppAttacked) & WhiteF1G1) == 0)
		MoveAdd(List, FlagOO | (WhiteE1 << 6) | WhiteG1, EnumMyK, WhiteG1, 0);
	if (CastleOOO && (Position->OccupiedBW &WhiteB1C1D1) == 0 && (OppAttacked &WhiteC1D1) == 0)
		MoveAdd(List, FlagOO | (WhiteE1 << 6) | WhiteC1, EnumMyK, WhiteC1, 0);
        }
#else
	if (CastleOO && ((Position->OccupiedBW | OppAttacked) & WhiteF1G1) == 0)
		MoveAdd(List, FlagOO | (WhiteE1 << 6) | WhiteG1, EnumMyK, WhiteG1, 0);
	if (CastleOOO && (Position->OccupiedBW &WhiteB1C1D1) == 0 && (OppAttacked &WhiteC1D1) == 0)
		MoveAdd(List, FlagOO | (WhiteE1 << 6) | WhiteC1, EnumMyK, WhiteC1, 0);
#endif

    Pawn = MyAttackedPawns[opks];
    if (BitboardMyQ | BitboardMyR)
        Rook = AttR(opks);
    if (BitboardMyQ | BitboardMyB)
        Bishop = AttB(opks);
    for (U = ForwardShift(BitboardMyP & SecondSixthRanks) & empty; U; BitClear(sq, U))
        {
        to = BSF(U);
        if (OnThirdRank(to) && Position->sq[Forward(to)] == 0)
            MoveAdd(List, (Backward(to) << 6) | Forward(to), EnumMyP, Forward(to), Pawn);
        MoveAdd(List, (Backward(to) << 6) | to, EnumMyP, to, Pawn);
        }
    for (U = BitboardMyQ; U; BitClear(sq, U))
        {
        sq = BSF(U);
        T = AttQ(sq) & empty;
        MovesTo(T, EnumMyQ, Rook | Bishop);
        }
    for (U = BitboardMyR; U; BitClear(sq, U))
        {
        sq = BSF(U);
        T = AttR(sq) & empty;
        MovesTo(T, EnumMyR, Rook);
        }
    for (U = BitboardMyB; U; BitClear(sq, U))
        {
        sq = BSF(U);
        T = AttB(sq) & empty;
        MovesTo(T, ((SqSet[sq] & Black) ? EnumMyBD : EnumMyBL), Bishop);
        }
    sq = BSF(BitboardMyK);
    T = AttK[sq] & empty &(~OppAttacked);
    MovesTo(T, EnumMyK, 0);
    for (U = BitboardMyN; U; BitClear(sq, U))
        {
        sq = BSF(U);
        T = AttN[sq] & empty;
        MovesTo(T, EnumMyN, AttN[opks]);
        }
    for (U = BitboardMyP & BitBoardSeventhRank; U; BitClear(sq, U))
        {
        sq = BSF(U);
        to = Forward(sq);
        if (Position->sq[to] == 0)
            UnderProm();
        to = ForwardLeft(sq);
        if (sq != WhiteA7 && SqSet[to] & OppOccupied)
            UnderProm();
        to = ForwardRight(sq);
        if (sq != WhiteH7 && SqSet[to] & OppOccupied)
            UnderProm();
        }
    List->move = 0;
    return List;
    }
typeMoveList *MyQuietChecks(typePos *Position, typeMoveList *List, uint64 mask)
    {
    int opks, king, sq, to, fr, pi;
    uint64 U, T, V;
    typeMoveList *list;
    uint32 move;
    uint64 gcm;
    gcm = ~MyXray;
    mask = (~mask) &~MyOccupied;
    ;
    list = List;
    king = OppKingSq;
    list = List;
    for (U = MyXray & MyOccupied; U; BitClear(fr, U))
        {
        fr = BSF(U);
        pi = Position->sq[fr];
        if (pi == EnumMyP)
            {
            if (File(fr) != File(king) && !SeventhRank(fr) && Position->sq[Forward(fr)] == 0)
                {
                (List++)->move = (fr << 6) | Forward(fr);
                if (OnSecondRank(fr) && Position->sq[Forward2(fr)] == 0)
                    (List++)->move = (fr << 6) | Forward2(fr);
                }
            if (CanCaptureRight && Rank(fr) != NumberRank7)
                (List++)->move = (fr << 6) | ForwardRight(fr);
            if (CanCaptureLeft && Rank(fr) != NumberRank7)
                (List++)->move = (fr << 6) | ForwardLeft(fr);
            }
        else if (pi == EnumMyN)
            {
            V = AttN[fr] & mask;
            while (V)
                {
                to = BSF(V);
                (List++)->move = (fr << 6) | to;
                BitClear(to, V);
                }
            }
        else if (pi == EnumMyBL || pi == EnumMyBD)
            {
            V = AttB(fr) & mask;
            while (V)
                {
                to = BSF(V);
                (List++)->move = (fr << 6) | to;
                BitClear(to, V);
                }
            }
        else if (pi == EnumMyR)
            {
            V = AttR(fr) & mask;
            while (V)
                {
                to = BSF(V);
                (List++)->move = (fr << 6) | to;
                BitClear(to, V);
                }
            }
        else if (pi == EnumMyK)
            {
            if (File(fr) == File(king) || Rank(fr) == Rank(king))
                V = AttK[fr] & NonOrtho[king] & mask &(~OppAttacked);
            else
                V = AttK[fr] & NonDiag[king] & mask &(~OppAttacked);
            while (V)
                {
                to = BSF(V);
                (List++)->move = (fr << 6) | to;
                BitClear(to, V);
                }
            }
        }
    opks = OppKingSq;
    T = CaptureLeft &(~BitBoardEighthRank) & mask & OppOccupied & MyAttackedPawns[opks];
    while (T)
        {
        to = BSF(T);
        (List++)->move = FromRight(to) | to;
        BitClear(to, T);
        }
    T = CaptureRight &(~BitBoardEighthRank) & mask & OppOccupied & MyAttackedPawns[opks];
    while (T)
        {
        to = BSF(T);
        (List++)->move = FromLeft(to) | to;
        BitClear(to, T);
        }
    for (U = BitboardMyQ; U; BitClear(sq, U))
        {
        sq = BSF(U);
        T = AttQ(sq) & AttQ(king) & mask;
        while (T)
            {
            to = BSF(T);
            BitClear(to, T);
            if ((OppAttackedPawns[to] & BitboardOppP & gcm) == 0 && (AttN[to] & BitboardOppN & gcm) == 0)
                {
                move = (sq << 6) | to;
                if (MySEE(Position, move))
                    (List++)->move = (sq << 6) | to;
                }
            }
        }
    for (U = BitboardMyR; U; BitClear(sq, U))
        {
        sq = BSF(U);
        T = AttR(sq) & AttR(king) & mask;
        while (T)
            {
            to = BSF(T);
            BitClear(to, T);
            if ((OppAttackedPawns[to] & BitboardOppP & gcm) == 0 && (AttN[to] & BitboardOppN & gcm) == 0)
                {
                move = (sq << 6) | to;
                if (MySEE(Position, move))
                    (List++)->move = (sq << 6) | to;
                }
            }
        }
    for (U = BitboardMyB; U; BitClear(sq, U))
        {
        sq = BSF(U);
        T = AttB(sq) & AttB(king) & mask;
        while (T)
            {
            to = BSF(T);
            BitClear(to, T);
            if ((OppAttackedPawns[to] & BitboardOppP & gcm) == 0)
                {
                move = (sq << 6) | to;
                if (MySEE(Position, move))
                    (List++)->move = (sq << 6) | to;
                }
            }
        }
    for (U = BitboardMyN; U; BitClear(sq, U))
        {
        sq = BSF(U);
        T = AttN[sq] & AttN[king] & mask;
        while (T)
            {
            to = BSF(T);
            BitClear(to, T);
            if ((OppAttackedPawns[to] & BitboardOppP & gcm) == 0)
                {
                move = (sq << 6) | to;
                if (MySEE(Position, move))
                    (List++)->move = (sq << 6) | to;
                }
            }
        }
    if (BitboardOppK & FourthEighthRankNoH && Position->sq[BackRight(opks)] == 0)
        {
        if (Position->sq[BackRight2(opks)] == EnumMyP)
            {
            fr = BackRight2(opks);
            to = BackRight(opks);
            move = (fr << 6) | to;
            if (PawnGuard(to, fr) && MySEE(Position, move))
                (List++)->move = move;
            }
        if (Rank(opks) == NumberRank5 && Position->sq[BackRight2(opks)] == 0
           && Position->sq[BackRight3(opks)] == EnumMyP)
            {
            to = BackRight(opks);
            fr = BackRight3(opks);
            move = (fr << 6) | to;
            if (PawnGuard(to, fr) && MySEE(Position, move))
                (List++)->move = move;
            }
        }
    if (BitboardOppK & FourthEighthRankNoA && Position->sq[BackLeft(opks)] == 0)
        {
        if (Position->sq[BackLeft2(opks)] == EnumMyP)
            {
            fr = BackLeft2(opks);
            to = BackLeft(opks);
            move = (fr << 6) | to;
            if (PawnGuard(to, fr) && MySEE(Position, move))
                (List++)->move = move;
            }
        if (Rank(opks) == NumberRank5 && Position->sq[BackLeft2(opks)] == 0
           && Position->sq[BackLeft3(opks)] == EnumMyP)
            {
            to = BackLeft(opks);
            fr = BackLeft3(opks);
            move = (fr << 6) | to;
            if (PawnGuard(to, fr) && MySEE(Position, move))
                (List++)->move = move;
            }
        }
    List->move = MoveNone;
    return List;
    }
