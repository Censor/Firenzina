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
#ifdef RobboBases
#include "robbo_triplebase.h"
#include "robbo_totalbase.h"

#define wEnumB wEnumBL
#define bEnumB bEnumBL
#define NewBitString(bs, Data) { bs->byte = bs->bit = 0; bs->data = Data; }

static unsigned char nMune[64];
static unsigned char alfMune[64];
static unsigned char nepedMune[64];
static unsigned char bipedMune[64];
static unsigned char BLockedMune[64];
static const uint8 NX[4][2] =
    {
    { 0, 0 },
    { 2, 3 },
    { 1, 3 },
    { 1, 2 }
    };
extern MutexType TripleBaseUsage[1];

#define reQU(x)                                                      \
  { if (P[x] == wEnumP) Q[x] = bipedMune[Q[x]];                      \
    else if (P[x] == bEnumP) Q[x] = nepedMune[Q[x]];                 \
    else if (P[x] == BlockedPawn) Q[x] = BLockedMune[Q[x]];         \
    else if (P[x] == wEnumB || P[x] == bEnumB) Q[x] = alfMune[Q[x]]; \
    else Q[x] = nMune[Q[x]]; }

#define vaWIN 1
#define vaDRAW 0
#define vaLOSS -1
#define InCheck                                       \
  (Position->wtm ?                                     \
   (wBitboardK & Position->Dyn->bAtt) : (bBitboardK & Position->Dyn->wAtt))
#define IsIllegal                                     \
	(!Position->wtm ?                                    \
	(wBitboardK & Position->Dyn->bAtt) : (bBitboardK & Position->Dyn->wAtt))
#define SaveTripleHash()                             \
	TripleHash[Position->Dyn->Hash & TripleHashMask] = \
	(Position->Dyn->Hash & 0xfffffffffffffff0) | ((*Value) + 1);

typedef struct
    {
    int byte, bit;
    uint8 *data;
    } BitString;

static char EnumN[65] =
    {
    A1, A2, B1, B2, A3, A4, B3, B4, C1, C2, D1, D2, C3, C4, D3, D4, A8, A7, B8, B7, A6, A5, B6, B5, C8, C7, D8, D7, C6,
       C5, D6, D5, H1, H2, G1, G2, H3, H4, G3, G4, F1, F2, E1, E2, F3, F4, E3, E4, H8, H7, G8, G7, H6, H5, G6, G5, F8,
       F7, E8, E7, F6, F5, E6, E5, -1
    };
static char EnumBiPed[49] =
    {
    A2, B2, C2, D2, E2, F2, G2, H2, A3, B3, C3, D3, E3, F3, G3, H3, A4, B4, C4, D4, E4, F4, G4, H4, A5, B5, C5, D5, E5,
       F5, G5, H5, A6, B6, C6, D6, E6, F6, G6, H6, A7, B7, C7, D7, E7, F7, G7, H7, -1
    };
static char EnumNePed[49] =
    {
    A7, B7, C7, D7, E7, F7, G7, H7, A6, B6, C6, D6, E6, F6, G6, H6, A5, B5, C5, D5, E5, F5, G5, H5, A4, B4, C4, D4, E4,
       F4, G4, H4, A3, B3, C3, D3, E3, F3, G3, H3, A2, B2, C2, D2, E2, F2, G2, H2, -1
    };
static char EnumAlf[65] =
    {
    A1, A3, B2, C1, A5, B4, C3, D2, E1, A7, B6, C5, D4, E3, F2, G1, B8, C7, D6, E5, F4, G3, H2, D8, E7, F6, G5, H4, F8,
       G7, H6, H8, H1, H3, G2, F1, H5, G4, F3, E2, D1, H7, G6, F5, E4, D3, C2, B1, G8, F7, E6, D5, C4, B3, A2, E8, D7,
       C6, B5, A4, C8, B7, A6, A8, -1
    };
static char EnumBlocked[41] =
    {
    A2, B2, C2, D2, E2, F2, G2, H2, A3, B3, C3, D3, E3, F3, G3, H3, A4, B4, C4, D4, E4, F4, G4, H4, A5, B5, C5, D5, E5,
       F5, G5, H5, A6, B6, C6, D6, E6, F6, G6, H6, -1
    };
int TripleBaseTable[16][16][16][16];

static uint8 NextByteDecoded(BitString *BS, uint32 *Huffman)
    {
    uint32 t, va = BS->data[BS->byte] >> BS->bit;
    if (BS->byte + 1 < 0x40)
        va |= (((uint32)(BS->data[BS->byte + 1])) << 8) >> BS->bit;
    t = Huffman[va & 0xff];
    if (t >= (1 << 24))
        {
        BS->bit += (t >> 24);
        BS->byte += (BS->bit >> 3);
        BS->bit &= 0x7;
        return (t & 0xff);
        }
    if (BS->byte + 2 < 0x40)
        va |= (((uint32)(BS->data[BS->byte + 2])) << 16) >> BS->bit;
    t = Huffman[0x100 * (t + 1) + ((va >> 8) & 0xff)];
    BS->bit += (t >> 24);
    BS->byte += (BS->bit >> 3);
    BS->bit &= 0x7;
    return (t & 0xff);
    }
static int GetDataHuffman(uint8 *Data, int ba, uint8 *AB, uint8 *A, int PR, int sb, uint32 *Huffman)
    {
    int n, b, i = 0, r = A[sb];
    uint8 ch, rank;
    BitString BS[1];
    NewBitString(BS, Data);
    while (ba >= 0 && BS->byte < PR)
        {
        ch = NextByteDecoded(BS, Huffman);
        rank = (ch & 64) >> 6;
        n = ch & 31;
        b = 0;
        if (ch & 32)
            {
            if (!ba)
                return AB[NX[r][rank]];
            else
                ba--;
            }
        else
            r = NX[r][rank];
        while (ch & 128)
            {
            b++;
            ch = NextByteDecoded(BS, Huffman);
            n += ((ch & 127) << (7 * b - 2));
            }
        ba -= n;
        }
    return AB[r];
    }
static int GetData(uint8 *E, int ba, uint8 *AB, uint8 *A, int PR, int sb, uint32 *HuffmanTable)
    {
    int n, b, i = 0, r = A[sb];
    uint8 ch, rank;
    if (HuffmanTable)
        return GetDataHuffman(E, ba, AB, A, PR, sb, HuffmanTable);
    while (ba >= 0 && i < PR)
        {
        ch = E[i++];
        rank = (ch & 64) >> 6;
        n = ch & 31;
        b = 0;
        if (ch & 32)
            {
            if (!ba)
                return AB[NX[r][rank]];
            else
                ba--;
            }
        else
            r = NX[r][rank];
        while (ch & 128)
            {
            b++;
            ch = E[i++];
            n += ((ch & 127) << (7 * b - 2));
            }
        ba -= n;
        }
    return AB[r];
    }
static bool Dynamic256kbBlockLookup(RobboTripleBase *rtb, int heap, uint64 ind, int w, int *res,
   bool ProbingWeak, bool ProbingImpale)
    {
    int f;
    uint32 *I;
    uint8 *D;
    uint32 z, cmp;
    TripleBaseCache *Cache;
    if (!DemandDynamicTripleCache(rtb, heap, w, &Cache, &D, &I, ProbingWeak, ProbingImpale))
        return false;
    z = 8;
    cmp = I[z] >> 2;
    z += ((ind > cmp) - (ind < cmp)) << 2;
    cmp = I[z] >> 2;
    z += ((ind > cmp) - (ind < cmp)) << 1;
    cmp = I[z] >> 2;
    z += ((ind > cmp) - (ind < cmp)) << 0;
    cmp = I[z] >> 2;
    z -= (ind < cmp);
    I += I[z + 16];
    f = (z << 14);
    z = 8;
    cmp = I[z] >> 2;
    z += ((ind > cmp) - (ind < cmp)) << 2;
    cmp = I[z] >> 2;
    z += ((ind > cmp) - (ind < cmp)) << 1;
    cmp = I[z] >> 2;
    z += ((ind > cmp) - (ind < cmp)) << 0;
    cmp = I[z] >> 2;
    z -= (ind < cmp);
    I += I[z + 16];
    f += (z << 10);
    z = 8;
    cmp = I[z] >> 2;
    z += ((ind > cmp) - (ind < cmp)) << 2;
    cmp = I[z] >> 2;
    z += ((ind > cmp) - (ind < cmp)) << 1;
    cmp = I[z] >> 2;
    z += ((ind > cmp) - (ind < cmp)) << 0;
    cmp = I[z] >> 2;
    z -= (ind < cmp);
    if (ind == 0x3fffffff)
        while ((I[z] >> 2) == 0x3fffffff)
            z--;
    f += (z << 6);
    *res = GetData(D + f, ind - (I[z] >> 2), rtb->w, rtb->wi, 64, I[z] & 3, rtb->huffman_table[heap]);
    rtb->hit++;
    if (!rtb->direct)
        {
        Lock(TripleBaseUsage);
        Cache->user--;
        UnLock(TripleBaseUsage);
        }
    return true;
    }
static bool RobboDynamicValue(RobboTripleBase *rtb, uint64 ind, int *res, bool ProbingWeak,
   bool ProbingImpale)
    {
    int heap, l, w;
    uint32 *I;
    heap = ind >> 30;
    ind &= 0x3fffffff;
    l = rtb->bs_split[heap];
    I = rtb->ind_split[heap];
    if (l == -1)
        w = 0;
    else
        {
        w = (2 << l) - 1;
        while (l >= 0)
            {
            uint32 u = (I[w] >> 2);
            w += ((ind > u) - (ind < u)) << l;
            l--;
            }
        while (ind >= (I[w] >> 2))
            w++;
        w--;
        }
    return Dynamic256kbBlockLookup(rtb, heap, ind, w, res, ProbingWeak, ProbingImpale);
    }
static bool RobboTripleValue(RobboTripleBase *rtb, uint64 ind, int *res, bool ProbingWeak,
   bool ProbingImpale)
    {
    int w = 0, l, u;
    if (rtb->w[2] == 0)
        {
        *res = (rtb->w[1]);
        return true;
        }
    if (rtb->variant & RobboDynamicLoad)
        return RobboDynamicValue(rtb, ind, res, ProbingWeak, ProbingImpale);
    l = rtb->bs;
    if (l == -1)
        w = 0;
    else
        {
        w = (2 << l) - 1;
        while (l >= 0)
            {
            u = rtb->ind[w] >> 2;
            w += ((ind > u) - (ind < u)) << l;
            l--;
            }
        while (ind >= (rtb->ind[w] >> 2))
            w++;
        w--;
        }
    *res = GetData((rtb->data)+(rtb->prop_ind * w), ind - (rtb->ind[w] >> 2), rtb->w, rtb->wi, rtb->prop_ind,
       rtb->ind[w] & 3, NULL);
    rtb->hit++;
    return true;
    }

static bool TripleIndex(type_PiSq *PISQ, int *tb_num, uint64 *ind)
    {
    int v, i, x, K1, K2, P[4], Q[4];
    int rf;
    RobboTripleBase *rtb;
    if (PISQ->wtm)
        {
        K1 = PISQ->wK;
        K2 = PISQ->bK;
        }
    else
        {
        K1 = PISQ->bK;
        K2 = PISQ->wK;
        for (i = 0; i < 4; i++)
            PISQ->pi[i] = ColScamb[PISQ->pi[i]];
        }
    v = TripleBaseTable[PISQ->pi[0]][PISQ->pi[1]][PISQ->pi[2]][PISQ->pi[3]];
    if ((v % 65536) == -1)
        return false;
    *tb_num = v % 65536;
    rtb = TableTripleBases + (v % 65536);
    if (rtb->w[2] == 0)
        return true;
    for (i = 0; i < 4; i++)
        {
        x = (v >> (16 + (i << 1))) & 3;
        Q[i] = PISQ->sq[x];
        P[i] = PISQ->pi[x];
        }
    if (PISQ->Pawn)
        {
        int w = ((File(K2) >= FE) << 1) + (!PISQ->wtm);
        for (i = 0; i < 4; i++)
            {
            SpearPiece(w, Q[i]);
            if (P[i] == BlockedPawn && !PISQ->wtm)
                Q[i] -= 8;
            }
        SpearPiece(w, K1);
        SpearPiece(w, K2);
        rf = KingPawn[(K1 << 6) + K2];
        }
    else
        {
        uint8 w = oSpear[K1][K2];
        for (i = 0; i < 4; i++)
            SpearNorm(w, Q[i]);
        SpearNorm(w, K1);
        SpearNorm(w, K2);
        rf = KingNormal[(K1 << 6) + K2];
        }
    if (rf == -1)
        return false;
    for (i = 0; i < 4; i++)
        reQU(i);
    for (i = 0; i < 4; i++)
        if (!P[i])
            Q[i] = 0;
    if (rtb->efgh2 != -1)
        {
        int f1 = File(Q[rtb->efgh1]);
        int f2 = File(Q[rtb->efgh2]);
        f1 = MIN(f1, (FH - f1)) & 1;
        f2 = MIN(f2, (FH - f2)) & 1;
        *tb_num = rtb->efgh[f1 + (f2 << 1)];
        }
    else if (rtb->efgh1 != -1)
        {
        int f = File(Q[rtb->efgh1]);
        *tb_num = rtb->efgh[MIN(f, (FH - f))];
        }
    if (*tb_num == -1 || *tb_num == 0xffff)
        return false;
    for (i = 0; i < 4; i++)
        Q[i] >>= rtb->efgh_shift[i];

    if (P[0] == P[3] && P[3])
        {
        int m0, m1, m2, m3, t0, t1, u0, u1;
        t0 = MAX(Q[0], Q[1]);
        u0 = MIN(Q[0], Q[1]);
        t1 = MAX(Q[2], Q[3]);
        u1 = MIN(Q[2], Q[3]);
        m0 = MAX(t0, t1);
        m1 = MIN(t0, t1);
        m2 = MAX(u0, u1);
        m3 = MIN(u0, u1);
        if (m1 < m2)
            Exchange(m1, m2);
        Q[0] = (m0 * (m0 - 1) * (m0 - 2) * (m0 - 3)) / 24 + (m1 * (m1 - 1) * (m1 - 2)) / 6 + ((m2 * (m2 - 1)) >> 1) + m3;
        Q[1] = Q[2] = Q[3] = 0;
        }
    else if (P[0] == P[2] && P[2])
        {
        Q[0] = SameO3[Q[0]][Q[1]][Q[2]];
        Q[1] = Q[2] = 0;
        }
    else if (P[1] == P[3] && P[3])
        {
        Q[1] = SameO3[Q[1]][Q[2]][Q[3]];
        Q[2] = Q[3] = 0;
        }
    else
        {
        if (P[0] == P[1] && P[1])
            {
            Q[0] = SameO2[Q[0]][Q[1]];
            Q[1] = 0;
            }
        if (P[1] == P[2] && P[2])
            {
            Q[1] = SameO2[Q[1]][Q[2]];
            Q[2] = 0;
            }
        if (P[2] == P[3] && P[3])
            {
            Q[2] = SameO2[Q[2]][Q[3]];
            Q[3] = 0;
            }
        }
    *ind = ((((rf * rtb->m[0]) + Q[0]) * rtb->m[1] + Q[1]) * rtb->m[2] + Q[2]) * rtb->m[3] + Q[3];
    return true;
    }
static RobboTripleBase *ThisTripleBase(typePos *Position)
    {
    type_PiSq PiSq[1];
    bool b;
    int i, v;
    uint64 U, V, W;
    PiSq->pi[0] = PiSq->pi[1] = PiSq->pi[2] = PiSq->pi[3] = 0;
    PiSq->sq[0] = PiSq->sq[1] = PiSq->sq[2] = PiSq->sq[3] = 0;
    U = wBitboardP &(bBitboardP >> 8);
    V = (U << 1) & U & ~FileA;
    W = (V << 1) & V;
    PiSq->Blocked = (U & ~V) | (W & ~((W << 1) & W));
    b = BitboardPQ(Position, PiSq);
    if (Position->wtm)
        for (i = 0; i < 4; i++)
            PiSq->pi[i] = ColScamb[PiSq->pi[i]];
    v = TripleBaseTable[PiSq->pi[0]][PiSq->pi[1]][PiSq->pi[2]][PiSq->pi[3]];
    if ((v % 65536) != -1)
        return TableTripleBases + (v % 65536);
    while (PiSq->Blocked && PiSq->n < 4)
        {
        BitClear(0, PiSq->Blocked);
        b = BitboardPQ(Position, PiSq);
        if (Position->wtm)
            for (i = 0; i < 4; i++)
                PiSq->pi[i] = ColScamb[PiSq->pi[i]];
        v = TripleBaseTable[PiSq->pi[0]][PiSq->pi[1]][PiSq->pi[2]][PiSq->pi[3]];
        if ((v % 65536) != -1)
            return TableTripleBases + (v % 65536);
        }
    return NULL;
    }
static bool TripleWork(typePos *Position, int *r, bool ProbingWeak, bool ProbingImpale)
    {
    int tb;
    uint64 ind, U, V, W;
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
        b = TripleIndex(PiSq, &tb, &ind);
    if (b)
        {
        RobboTripleBase *rtb = (TableTripleBases + tb);
        if (rtb->w[2] == 0)
            *r = 2 - rtb->w[1];
        else
            {
            if (!RobboTripleValue(rtb, ind, r, ProbingWeak, ProbingImpale))
                return false;
            *r = 2 - (*r);
            }
        Position->tbhits++;
        return true;
        }
    while (PiSq->Blocked && PiSq->n < 4)
        {
        BitClear(0, PiSq->Blocked);
        b = BitboardPQ(Position, PiSq);
        if (b)
            b = TripleIndex(PiSq, &tb, &ind);
        if (b)
            {
            RobboTripleBase *rtb = (TableTripleBases + tb);
            if (rtb->w[2] == 0)
                *r = 2 - rtb->w[1];
            else
                {
                if (!RobboTripleValue(rtb, ind, r, ProbingWeak, ProbingImpale))
                    return false;
                *r = 2 - (*r);
                }
            Position->tbhits++;
            return true;
            }
        }
    return false;
    }

bool TripleValue(typePos *Position, int *Value, bool ProbingWeak, bool ProbingImpale)
    {
    int r = vaLOSS, b = vaLOSS, i, move;
    typeMoveList List[256], *list;
    uint64 H;
    if (Position->Dyn->oo)
        return false;
    if (IsIllegal)
        return false;
    H = TripleHash[Position->Dyn->Hash & TripleHashMask];
    if (((H ^ Position->Dyn->Hash) & 0xfffffffffffffff0) == 0)
        {
        *Value = (H & 0xf) - 1;
        Position->tbhits++;
        return true;
        }
    if (InCheck)
        {
        RobboTripleBase *rtb = ThisTripleBase(Position);
        if (!rtb || !rtb->scacco)
            goto NoScacco;
        list = EvasionMoves(Position, List, 0xffffffffffffffff);
        if (list == List)
            {
            *Value = vaLOSS;
            SaveTripleHash();
            return true;
            }
        for (i = 0; i < list - List; i++)
            {
            move = List[i].move & 0x7fff;
            Make(Position, move);
            Mobility(Position);
            if (IsIllegal)
                {
                Undo(Position, move);
                continue;
                }
            if (!TripleValue(Position, &r, ProbingWeak, ProbingImpale))
                {
                Undo(Position, move);
                return false;
                }
            r = -r;
            Undo(Position, move);
            if (r == vaWIN)
                {
                *Value = vaWIN;
                SaveTripleHash();
                return true;
                }
            if (r == vaDRAW)
                b = vaDRAW;
            }
        *Value = b;
        SaveTripleHash();
        return true;
        }
    NoScacco:
    list = CaptureMoves(Position, List, 0xffffffffffffffff);
    for (i = 0; i < list - List; i++)
        {
        move = List[i].move & 0x7fff;
        if ((move >> 12) == 0x4)
            continue;
        Make(Position, move);
        Mobility(Position);
        if (!IsIllegal)
            {
            if (!TripleValue(Position, &r, ProbingWeak, ProbingImpale))
                {
                Undo(Position, move);
                return false;
                }
            r = -r;
            }
        Undo(Position, move);
        if (r == vaWIN)
            {
            *Value = r;
            SaveTripleHash();
            return true;
            }
        if (r == vaDRAW)
            b = vaDRAW;
        }
    if (!TripleWork(Position, &r, ProbingWeak, ProbingImpale))
        return false;
    if (b > r)
        *Value = b;
    else
        *Value = r;
    SaveTripleHash();
    return true;
    }
void ReadyTripleBase()
    {
    int i;
    for (i = A1; i <= H8; i++)
        nMune[EnumN[i]] = i;
    for (i = A1; i <= H8; i++)
        alfMune[EnumAlf[i]] = i;
    for (i = 0; i < 48; i++)
        bipedMune[EnumBiPed[i]] = i;
    for (i = 0; i < 48; i++)
        nepedMune[EnumNePed[i]] = i;
    for (i = 0; i < 40; i++)
        BLockedMune[EnumBlocked[i]] = i;
    InitTripleBase();
    }
#endif
