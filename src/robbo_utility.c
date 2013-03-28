<<<<<<< HEAD:src/robbo_utility.c
/*
Firenzina is a UCI chess playing engine by Kranium (Norman Schmidt)
Firenzina is based on Ippolit source code: http://ippolit.wikispaces.com/
authors: Yakov Petrovich Golyadkin, Igor Igorovich Igoronov,
and Roberto Pescatore copyright: (C) 2009 Yakov Petrovich Golyadkin
date: 92th and 93rd year from Revolution
owners: PUBLICDOMAIN (workers)
dedication: To Vladimir Ilyich
=======
/*******************************************************************************
Firenzina is a UCI chess playing engine by
Yuri Censor (Dmitri Gusev) and ZirconiumX (Matthew Brades).
Rededication: To the memories of Giovanna Tornabuoni and Domenico Ghirlandaio.
Special thanks to: Norman Schmidt, Jose Maria Velasco, Jim Ablett, Jon Dart.
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
>>>>>>> Linux/Housekeeping/Bug fixes/Extend xTreme/Defs:Firenzina/robbo_utility.c
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

#ifdef RobboBases
#include "robbo_totalbase.h"

static char RiMap[16] =
    {
    0, wEnumP, wEnumN, wEnumK, wEnumB, wEnumB, wEnumR, wEnumQ, 8, bEnumP, bEnumN, bEnumK, bEnumB, bEnumB, bEnumR, bEnumQ
    };

bool BitboardPQ(typePos *Position, type_PiSq *PiSq)
    {
    uint64 T;
    int n = 0, sq;
    T = Position->OccupiedBW ^ (wBitboardK | bBitboardK);
    PiSq->wtm = Position->wtm;
    PiSq->wK = Position->wKsq;
    PiSq->bK = Position->bKsq;
    PiSq->Pawn = false;
    while (T && n < 4)
        {
        sq = BSF(T);
        if (PiSq->Blocked & SqSet[sq])
            {
            PiSq->pi[n] = BlockedPawn;
            T ^= SqSet[sq + 8];
            }
        else
            PiSq->pi[n] = RiMap[Position->sq[sq]];
        if (PiSq->pi[n] == wEnumP || PiSq->pi[n] == bEnumP || PiSq->pi[n] == BlockedPawn)
            PiSq->Pawn = true;
        PiSq->sq[n++] = sq;
        BitClear(sq, T);
        }
    PiSq->n = n;
    if (n == 4 && T)
        return false;
    return true;
    }

bool TotIndOtt(type_PiSq *PiSq, int *tb_num, uint64 *ind, uint64 *Double, int *KingSlice)
    {
    int K1, K2, rf;
    int i = 0, v, PI[4], Squ[4], x;
    RobboTotalBase *TotalBase;
    bool simm = false;
    if (Double != NULL)
        *Double = -1;
    if (PiSq->wtm)
        {
        K1 = PiSq->wK;
        K2 = PiSq->bK;
        }
    else
        {
        K1 = PiSq->bK;
        K2 = PiSq->wK;
        for (i = 0; i < 4; i++)
            PiSq->pi[i] = ColScamb[PiSq->pi[i]];
        }
    v = TotalBaseTable[PiSq->pi[0]][PiSq->pi[1]][PiSq->pi[2]][PiSq->pi[3]];
    TotalBase = TableTotalBases + (v % 65536);
    if ((v % 65536) == -1)
        return false;
    for (i = 0; i < 4; i++)
        {
        x = (v >> (16 + (i << 1))) & 3;
        Squ[i] = PiSq->sq[x];
        PI[i] = PiSq->pi[x];
        }
    if (PiSq->Pawn)
        {
        int w = ((File(K2) >= FE) << 1) + (!PiSq->wtm);
        for (i = 0; i < 4; i++)
            {
            SpearPiece(w, Squ[i]);
            if (PI[i] == BlockedPawn && !PiSq->wtm)
                Squ[i] -= 8;
            }
        SpearPiece(w, K1);
        SpearPiece(w, K2);
        rf = KingPawn[(K1 << 6) + K2];
        }
    else
        {
        uint8 w = oSpear[K1][K2];
        for (i = 0; i < 4; i++)
            SpearNorm(w, Squ[i]);
        SpearNorm(w, K1);
        SpearNorm(w, K2);
        if (Rank(K1) == File(K1) && Rank(K2) == File(K2))
            simm = true;
        rf = KingNormal[(K1 << 6) + K2];
        }
    if (rf == -1)
        return false;
    *KingSlice = rf;
    for (i = 0; i < 4; i++)
        {
        if (!PI[i])
            Squ[i] = 0;
        if (PI[i] == wEnumP || PI[i] == bEnumP || PI[i] == BlockedPawn)
            Squ[i] -= 8;
        }
    *tb_num = v % 65536;
    if (TotalBase->efgh2 != -1)
        {
        int f1 = File(Squ[TotalBase->efgh1]);
        int f2 = File(Squ[TotalBase->efgh2]);
        f1 = MIN(f1, (FH - f1)) & 1;
        f2 = MIN(f2, (FH - f2)) & 1;
        *tb_num = TotalBase->efgh[f1 + (f2 << 1)];
        }
    else if (TotalBase->efgh1 != -1)
        {
        int f = File(Squ[TotalBase->efgh1]);
        *tb_num = TotalBase->efgh[MIN(f, (FH - f))];
        }
    if (*tb_num == -1 || *tb_num == 0xffff)
        return false;
    for (i = 0; i < 4; i++)
        Squ[i] >>= TotalBase->efgh_shift[i];
    if (PI[0] == PI[3] && PI[3])
        {
        int m0, m1, m2, m3, t0, t1, u0, u1;
        t0 = MAX(Squ[0], Squ[1]);
        u0 = MIN(Squ[0], Squ[1]);
        t1 = MAX(Squ[2], Squ[3]);
        u1 = MIN(Squ[2], Squ[3]);
        m0 = MAX(t0, t1);
        m1 = MIN(t0, t1);
        m2 = MAX(u0, u1);
        m3 = MIN(u0, u1);
        if (m1 < m2)
            Exchange(m1, m2);
        Squ[0] = (m0 * (m0 - 1) * (m0 - 2) * (m0 - 3)) / 24 + (m1 * (m1 - 1) * (m1 - 2)) / 6 + ((m2 * (m2 - 1)) >> 1) + m3;
        Squ[1] = Squ[2] = Squ[3] = 0;
        }
    else if (PI[0] == PI[2] && PI[2])
        {
        Squ[0] = SameO3[Squ[0]][Squ[1]][Squ[2]];
        Squ[1] = Squ[2] = 0;
        }
    else if (PI[1] == PI[3] && PI[3])
        {
        Squ[1] = SameO3[Squ[1]][Squ[2]][Squ[3]];
        Squ[2] = Squ[3] = 0;
        }
    else
        {
        if (PI[0] == PI[1] && PI[1])
            {
            Squ[0] = SameO2[Squ[0]][Squ[1]];
            Squ[1] = 0;
            }
        if (PI[1] == PI[2] && PI[2])
            {
            Squ[1] = SameO2[Squ[1]][Squ[2]];
            Squ[2] = 0;
            }
        if (PI[2] == PI[3] && PI[3])
            {
            Squ[2] = SameO2[Squ[2]][Squ[3]];
            Squ[3] = 0;
            }
        }
    *ind = ((Squ[0] * TotalBase->m[1] + Squ[1]) * TotalBase->m[2] + Squ[2]) * TotalBase->m[3] + Squ[3];
    if (Double == NULL)
        return true;
    if (simm)
        {
        if (PI[0] == PI[3] && PI[3])
            {
            i = Sq4normal[Squ[0]];
            Squ[0] = i & 077;
            Squ[1] = (i >> 6) & 077;
            Squ[2] = (i >> 12) & 077;
            Squ[3] = i >> 18;
            }
        else if (PI[0] == PI[2] && PI[2])
            {
            i = Sq3normal[Squ[0]];
            Squ[0] = i & 077;
            Squ[1] = (i >> 6) & 077;
            Squ[2] = i >> 12;
            }
        else if (PI[1] == PI[3] && PI[3])
            {
            i = Sq3normal[Squ[1]];
            Squ[1] = i & 077;
            Squ[2] = (i >> 6) & 077;
            Squ[3] = i >> 12;
            }
        else
            {
            if (PI[0] == PI[1] && PI[1])
                {
                i = Sq2Normal[Squ[0]];
                Squ[0] = i & 077;
                Squ[1] = i >> 6;
                }
            if (PI[1] == PI[2] && PI[2])
                {
                i = Sq2Normal[Squ[1]];
                Squ[1] = i & 077;
                Squ[2] = i >> 6;
                }
            if (PI[2] == PI[3] && PI[3])
                {
                i = Sq2Normal[Squ[2]];
                Squ[2] = i & 077;
                Squ[3] = i >> 6;
                }
            }
        for (i = 0; i < 4; i++)
            SpearNorm(1, Squ[i]);
        if (PI[0] == PI[3] && PI[3])
            {
            int m0, m1, m2, m3, t0, t1, u0, u1;
            t0 = MAX(Squ[0], Squ[1]);
            u0 = MIN(Squ[0], Squ[1]);
            t1 = MAX(Squ[2], Squ[3]);
            u1 = MIN(Squ[2], Squ[3]);
            m0 = MAX(t0, t1);
            m1 = MIN(t0, t1);
            m2 = MAX(u0, u1);
            m3 = MIN(u0, u1);
            if (m1 < m2)
                Exchange(m1, m2);
            Squ[0] =
               (m0 * (m0 - 1) * (m0 - 2) * (m0 - 3)) / 24 + (m1 * (m1 - 1) * (m1 - 2)) / 6 + ((m2 * (m2 - 1)) >> 1) + m3;
            Squ[1] = Squ[2] = Squ[3] = 0;
            }
        else if (PI[0] == PI[2] && PI[2])
            {
            Squ[0] = SameO3[Squ[0]][Squ[1]][Squ[2]];
            Squ[1] = Squ[2] = 0;
            }
        else if (PI[1] == PI[3] && PI[3])
            {
            Squ[1] = SameO3[Squ[1]][Squ[2]][Squ[3]];
            Squ[2] = Squ[3] = 0;
            }
        else
            {
            if (PI[0] == PI[1] && PI[1])
                {
                Squ[0] = SameO2[Squ[0]][Squ[1]];
                Squ[1] = 0;
                }
            if (PI[1] == PI[2] && PI[2])
                {
                Squ[1] = SameO2[Squ[1]][Squ[2]];
                Squ[2] = 0;
                }
            if (PI[2] == PI[3] && PI[3])
                {
                Squ[2] = SameO2[Squ[2]][Squ[3]];
                Squ[3] = 0;
                }
            }
        *Double = ((Squ[0] * TotalBase->m[1] + Squ[1]) * TotalBase->m[2] + Squ[2]) * TotalBase->m[3] + Squ[3];
        }
    else
        *Double = -1;
    return true;
    }
bool MakePQ(type_PiSq *PiSq, int *Value, uint64 *ind, uint64 *Double, int *rf)
    {
    int tb;
    RobboTotalBase *TotalBase;
    *Value = 0;
    if (!TotIndOtt(PiSq, &tb, ind, Double, rf))
        return false;
    TotalBase = TableTotalBases + tb;
    *Value = TotalValue(TotalBase, (TotalBase->PawnMult) * (*rf)+(*ind));
    if ((*Value) != dBreak)
        return true;
    return false;
    }
static bool TotalBaseScore(typePos *Position, int *Value)
    {
    uint64 ind, U, V, W;
    int rf;
    type_PiSq PiSq[1];
    bool b;
    PiSq->pi[0] = PiSq->pi[1] = PiSq->pi[2] = PiSq->pi[3] = 0;
    PiSq->sq[0] = PiSq->sq[1] = PiSq->sq[2] = PiSq->sq[3] = 0;
    U = wBitboardP &(bBitboardP >> 8);
    V = (U << 1) & U & ~FileA;
    W = (V << 1) & V;
    PiSq->Blocked = (U & ~V) | (W & ~((W << 1) & W));
    b = BitboardPQ(Position, PiSq);
    if (b)
        b = MakePQ(PiSq, Value, &ind, NULL, &rf);
    while (!b && PiSq->Blocked && PiSq->n < 4)
        {
        BitClear(0, PiSq->Blocked);
        b = BitboardPQ(Position, PiSq);
        if (b)
            b = MakePQ(PiSq, Value, &ind, NULL, &rf);
        }
    return b;
    }
bool OttIndice(type_PiSq *PiSq, uint64 *ind, uint64 *Double, int *rf)
    {
    int tb;
    if (TotIndOtt(PiSq, &tb, ind, Double, rf))
        return true;
    return false;
    }
bool RobboTotalBaseScore(typePos *Position, int *Value)
    {
    int ep = Position->Dyn->ep, move, w, vb = 255;
    bool b, b1, b2;
    if (Position->Dyn->oo)
        return false;
    if (!ep)
        return TotalBaseScore(Position, Value);
    b = TotalBaseScore(Position, Value);
    if (!b)
        return false;
    if (*Value == dWin)
        return true;
    if (Position->wtm)
        {
        if (File(ep) != FH && (wBitboardP & SqSet[ep - 7]))
            {
            move = ((ep - 7) << 6) | ep | FlagEP;
            Make(Position, move);
            Mobility(Position);
            if (!IsIllegal)
                {
                b1 = TotalBaseScore(Position, &w);
                if (w < vb)
                    vb = w;
                }
            Undo(Position, move);
            }
        if (File(ep) != FA && (wBitboardP & SqSet[ep - 9]))
            {
            move = ((ep - 9) << 6) | ep | FlagEP;
            Make(Position, move);
            Mobility(Position);
            if (!IsIllegal)
                {
                b2 = TotalBaseScore(Position, &w);
                if (w < vb)
                    vb = w;
                }
            Undo(Position, move);
            }
        }
    else
        {
        if (File(ep) != FH && (bBitboardP & SqSet[ep + 9]))
            {
            move = ((ep + 9) << 6) | ep | FlagEP;
            Make(Position, move);
            Mobility(Position);
            if (!IsIllegal)
                {
                b1 = TotalBaseScore(Position, &w);
                if (w < vb)
                    vb = w;
                }
            Undo(Position, move);
            }
        if (File(ep) != FA && (bBitboardP & SqSet[ep + 7]))
            {
            move = ((ep + 7) << 6) | ep | FlagEP;
            Make(Position, move);
            Mobility(Position);
            if (!IsIllegal)
                {
                b2 = TotalBaseScore(Position, &w);
                if (w < vb)
                    vb = w;
                }
            Undo(Position, move);
            }
        }
    if (DiskLoss(vb))
        vb = dWin;
    else if (vb == dWin)
        vb = dLoss;
    if (DiskLoss(vb) && DiskLoss(*Value))
        *Value = MAX (*Value, vb);
    else
        *Value = MIN(*Value, vb);
    return true;
    }
#endif
