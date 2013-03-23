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
#include "make_unmake.h"
#include "material_value.h"

#if defined(__GNUC__)
#define INLINE inline
#endif


#define RevCastle(Pos) (Pos)->Dyn->reversible = 0

typedef enum
    {
    ooK = 1,
    ooQ = 2,
    ook = 4,
    ooq = 8
    } CastlingTable;
typedef enum
    {
    KQkq = ooK | ooQ | ook | ooq,
    Qkq = ooQ | ook | ooq,
    Kkq = ooK | ook | ooq,
    kq = ook | ooq,
    KQk = ooK | ooQ | ook,
    KQ = ooK | ooQ,
    KQq = ooK | ooQ | ooq
    } KQkqTable;
static const uint64 CastleTable[64] =
    {
    Kkq, KQkq, KQkq, KQkq, kq, KQkq, KQkq, Qkq,
    KQkq, KQkq, KQkq, KQkq, KQkq, KQkq, KQkq, KQkq,
    KQkq, KQkq, KQkq, KQkq, KQkq, KQkq, KQkq, KQkq,
    KQkq, KQkq, KQkq, KQkq, KQkq, KQkq, KQkq, KQkq,
    KQkq, KQkq, KQkq, KQkq, KQkq, KQkq, KQkq, KQkq,
    KQkq, KQkq, KQkq, KQkq, KQkq, KQkq, KQkq, KQkq,
    KQkq, KQkq, KQkq, KQkq, KQkq, KQkq, KQkq, KQkq,
    KQk, KQkq, KQkq, KQkq, KQ, KQkq, KQkq, KQq
    };
const static uint64 WhiteEP[8] =
    {
    Bitboard2 (B4, B4), Bitboard2 (A4, C4),
    Bitboard2 (B4, D4), Bitboard2 (C4, E4),
    Bitboard2 (D4, F4), Bitboard2 (E4, G4),
    Bitboard2 (F4, H4), Bitboard2 (G4, G4)
    };
const static uint64 BlackEP[8] =
    {
    Bitboard2 (B5, B5), Bitboard2 (A5, C5),
    Bitboard2 (B5, D5), Bitboard2 (C5, E5),
    Bitboard2 (D5, F5), Bitboard2 (E5, G5),
    Bitboard2 (F5, H5), Bitboard2 (G5, G5)
    };
const static uint8 PromW[8] =
    {
    0, 0, 0, 0, wEnumN, wEnumBL, wEnumR, wEnumQ
    };
const static uint8 PromB[8] =
    {
    0, 0, 0, 0, bEnumN, bEnumBL, bEnumR, bEnumQ
    };
static INLINE void MakeWhiteOO(typePos *Position, int to)
    {
    if (to == G1)
        {
        wBitboardOcc ^= F1H1;
        wBitboardR ^= F1H1;
        Position->OccupiedBW ^= F1H1;
        Position->Dyn->Static += PST(wEnumR, F1) - PST(wEnumR, H1);
        Position->Dyn->Hash ^= Hash(wEnumR, F1) ^ Hash(wEnumR, H1);
        Position->sq[H1] = 0;
        Position->sq[F1] = wEnumR;
        }
    else if (to == C1)
        {
        wBitboardOcc ^= A1D1;
        wBitboardR ^= A1D1;
        Position->OccupiedBW ^= A1D1;
        Position->Dyn->Static += PST(wEnumR, D1) - PST(wEnumR, A1);
        Position->Dyn->Hash ^= Hash(wEnumR, A1) ^ Hash(wEnumR, D1);
        Position->sq[A1] = 0;
        Position->sq[D1] = wEnumR;
        }
    }
static INLINE void MakeBlackOO(typePos *Position, int to)
    {
    if (to == G8)
        {
        bBitboardOcc ^= F8H8;
        bBitboardR ^= F8H8;
        Position->OccupiedBW ^= F8H8;
        Position->Dyn->Static += PST(bEnumR, F8) - PST(bEnumR, H8);
        Position->Dyn->Hash ^= Hash(bEnumR, F8) ^ Hash(bEnumR, H8);
        Position->sq[H8] = 0;
        Position->sq[F8] = bEnumR;
        }
    else if (to == C8)
        {
        bBitboardOcc ^= A8D8;
        bBitboardR ^= A8D8;
        Position->OccupiedBW ^= A8D8;
        Position->Dyn->Static += PST(bEnumR, D8) - PST(bEnumR, A8);
        Position->Dyn->Hash ^= Hash(bEnumR, A8) ^ Hash(bEnumR, D8);
        Position->sq[A8] = 0;
        Position->sq[D8] = bEnumR;
        }
    }
void MakeWhite(typePos *Position, uint32 move)
    {
    int fr, to, pi, fl, cp, z;
    uint64 mask;
    Position->nodes++;
    memcpy(Position->Dyn + 1, Position->Dyn, 32);
    fr = From(move);
    to = To(move);
    pi = Position->sq[fr];
    Position->Dyn++;
    Position->Dyn->reversible++;
    Position->Dyn->move = move;
    fl = CastleTable[fr] & CastleTable[to] & Position->Dyn->oo;
    Position->Dyn->Hash ^= HashCastling[Position->Dyn->oo ^ fl];
    Position->Dyn->PawnHash ^= HashCastling[Position->Dyn->oo ^ fl];
    Position->Dyn->oo = fl;
    if (Position->Dyn->ep)
        {
        Position->Dyn->Hash ^= HashEP[Position->Dyn->ep & 7];
        Position->Dyn->ep = 0;
        }
    Position->sq[fr] = 0;
    mask = SqClear[fr];
    wBitboardOcc &= mask;
    Position->bitboard[pi] &= mask;
    ClearOccupied(mask, fr);
    Position->Dyn->Static += PST(pi, to) - PST(pi, fr);
    mask = Hash(pi, fr) ^ Hash(pi, to);
    cp = Position->sq[to];
    Position->Dyn->cp = cp;
    Position->Dyn->Hash ^= mask;
    if (pi == wEnumP)
        Position->Dyn->PawnHash ^= mask;
    Position->wtm ^= 1;
    Position->height++;
    Position->Dyn->Hash ^= HashWTM;
    if (pi == wEnumK)
        {
        Position->Dyn->PawnHash ^= mask;
        Position->wKsq = to;
        }
    if (cp)
        {
        mask = SqClear[to];
        bBitboardOcc &= mask;
        Position->bitboard[cp] &= mask;
        Position->Dyn->material -= MaterialValue[cp];
        Position->Dyn->Static -= PST(cp, to);
        if (cp == bEnumP)
            Position->Dyn->PawnHash ^= Hash(cp, to);
        Position->Dyn->Hash ^= Hash(cp, to);
        Position->Dyn->reversible = 0;
        }
    else
        {
        mask = SqSet[to];
        SetOccupied(mask, to);
        if (MoveIsOO(move))
            {
            RevCastle(Position);
            MakeWhiteOO(Position, to);
            }
        }
    Position->sq[to] = pi;
    wBitboardOcc |= SqSet[to];
    Position->bitboard[pi] |= SqSet[to];
    if (pi == wEnumP)
        {
        Position->Dyn->reversible = 0;
        if (MoveIsEP(move))
            {
            z = to ^ 8;
            mask = SqClear[z];
            bBitboardOcc &= mask;
            bBitboardP &= mask;
            ClearOccupied(mask, z);
            Position->Dyn->material -= MaterialValue[bEnumP];
            Position->Dyn->Static -= PST(bEnumP, z);
            Position->Dyn->Hash ^= Hash(bEnumP, z);
            Position->Dyn->PawnHash ^= Hash(bEnumP, z);
            Position->sq[z] = 0;
            }
        else if (MoveIsProm(move))
            {
            pi = PromW[(move & FlagMask) >> 12];
            if (pi == wEnumBL && SqSet[to] & Black)
                pi = wEnumBD;
            Position->sq[to] = pi;
            if (Position->bitboard[pi])
                Position->Dyn->material |= 0x80000000;
            wBitboardP &= SqClear[to];
            Position->bitboard[pi] |= SqSet[to];
            Position->Dyn->material += MaterialValue[pi] - MaterialValue[wEnumP];
            Position->Dyn->Static += PST(pi, to) - PST(wEnumP, to);
            Position->Dyn->Hash ^= Hash(pi, to) ^ Hash(wEnumP, to);
            Position->Dyn->PawnHash ^= Hash(wEnumP, to);
            }
        else if ((to ^ fr) == 16)
            {
            if (WhiteEP[to & 7] & bBitboardP)
                {
                z = (fr + to) >> 1;
                Position->Dyn->ep = z;
                Position->Dyn->Hash ^= HashEP[z & 7];
                }
            }
        }
    Position->Stack[++(Position->StackHeight)] = Position->Dyn->Hash;
    }
void MakeBlack(typePos *Position, uint32 move)
    {
    int fr, to, pi, fl, cp, z;
    uint64 mask;
    Position->nodes++;
    memcpy(Position->Dyn + 1, Position->Dyn, 32);
    fr = From(move);
    to = To(move);
    pi = Position->sq[fr];
    Position->Dyn++;
    Position->Dyn->reversible++;
    Position->Dyn->move = move;
    fl = CastleTable[fr] & CastleTable[to] & Position->Dyn->oo;
    Position->Dyn->Hash ^= HashCastling[Position->Dyn->oo ^ fl];
    Position->Dyn->PawnHash ^= HashCastling[Position->Dyn->oo ^ fl];
    Position->Dyn->oo = fl;
    if (Position->Dyn->ep)
        {
        Position->Dyn->Hash ^= HashEP[Position->Dyn->ep & 7];
        Position->Dyn->ep = 0;
        }
    Position->sq[fr] = 0;
    mask = SqClear[fr];
    bBitboardOcc &= mask;
    Position->bitboard[pi] &= mask;
    ClearOccupied(mask, fr);
    Position->Dyn->Static += PST(pi, to) - PST(pi, fr);
    mask = Hash(pi, fr) ^ Hash(pi, to);
    cp = Position->sq[to];
    Position->Dyn->cp = cp;
    Position->Dyn->Hash ^= mask;
    if (pi == bEnumP)
        Position->Dyn->PawnHash ^= mask;
    Position->wtm ^= 1;
    Position->height++;
    Position->Dyn->Hash ^= HashWTM;
    if (pi == bEnumK)
        {
        Position->Dyn->PawnHash ^= mask;
        Position->bKsq = to;
        }
    if (cp)
        {
        mask = SqClear[to];
        wBitboardOcc &= mask;
        Position->bitboard[cp] &= mask;
        Position->Dyn->material -= MaterialValue[cp];
        Position->Dyn->Static -= PST(cp, to);
        if (cp == wEnumP)
            Position->Dyn->PawnHash ^= Hash(cp, to);
        Position->Dyn->Hash ^= Hash(cp, to);
        Position->Dyn->reversible = 0;
        }
    else
        {
        mask = SqSet[to];
        SetOccupied(mask, to);
        if (MoveIsOO(move))
            {
            RevCastle(Position);
            MakeBlackOO(Position, to);
            }
        }
    Position->sq[to] = pi;
    bBitboardOcc |= SqSet[to];
    Position->bitboard[pi] |= SqSet[to];
    if (pi == bEnumP)
        {
        Position->Dyn->reversible = 0;
        if (MoveIsEP(move))
            {
            z = to ^ 8;
            mask = SqClear[z];
            wBitboardOcc &= mask;
            wBitboardP &= mask;
            ClearOccupied(mask, z);
            Position->Dyn->material -= MaterialValue[wEnumP];
            Position->Dyn->Static -= PST(wEnumP, z);
            Position->Dyn->Hash ^= Hash(wEnumP, z);
            Position->Dyn->PawnHash ^= Hash(wEnumP, z);
            Position->sq[z] = 0;
            }
        else if (MoveIsProm(move))
            {
            pi = PromB[(move & FlagMask) >> 12];
            if (pi == bEnumBL && SqSet[to] & Black)
                pi = bEnumBD;
            Position->sq[to] = pi;
            if (Position->bitboard[pi])
                Position->Dyn->material |= 0x80000000;
            Position->bitboard[bEnumP] &= SqClear[to];
            Position->bitboard[pi] |= SqSet[to];
            Position->Dyn->material += MaterialValue[pi] - MaterialValue[bEnumP];
            Position->Dyn->Static += PST(pi, to) - PST(bEnumP, to);
            Position->Dyn->Hash ^= Hash(pi, to) ^ Hash(bEnumP, to);
            Position->Dyn->PawnHash ^= Hash(bEnumP, to);
            }
        else if ((to ^ fr) == 16)
            {
            if (BlackEP[to & 7] & wBitboardP)
                {
                z = (fr + to) >> 1;
                Position->Dyn->ep = z;
                Position->Dyn->Hash ^= HashEP[z & 7];
                }
            }
        }
    Position->Stack[++(Position->StackHeight)] = Position->Dyn->Hash;
    }
void Make(typePos *Position, uint32 move)
    {
    if (Position->wtm)
        {
        if (NodeCheck & 4095)
            NodeCheck--;
        Position->nodes--;
        MakeWhite(Position, move);
        }
    else
        {
        Position->nodes--;
        MakeBlack(Position, move);
        }
    }
