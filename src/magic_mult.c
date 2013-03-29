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

static int BI[64], RI[64];
static int BS[64] =
    {
    6, 5, 5, 5, 5, 5, 5, 6,
    5, 5, 5, 5, 5, 5, 5, 5,
    5, 5, 7, 7, 7, 7, 5, 5,
    5, 5, 7, 9, 9, 7, 5, 5,
    5, 5, 7, 9, 9, 7, 5, 5,
    5, 5, 7, 7, 7, 7, 5, 5,
    5, 5, 5, 5, 5, 5, 5, 5,
    6, 5, 5, 5, 5, 5, 5, 6
    };
static int RS[64] =
    {
    12, 11, 11, 11, 11, 11, 11, 12,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    12, 11, 11, 11, 11, 11, 11, 12
    };
static uint64 BMask[64], RMask[64];
static uint64 RMult[64] =
    {
    0x6C80104004208000, 0x1080102004400080, 0x208020001000800A, 0x2080100208000480,
    0x1280040080080002, 0x7200041002000801, 0x0400011008008204, 0x028002C131000080,
    0x0042002200410080, 0x8201401000C12000, 0x1082801000A00280, 0x0001001000090020,
    0x0001000800110006, 0x0001000804000300, 0x00040002040128B0, 0x2120801040800100,
    0x0041228002804000, 0x0000810020401101, 0x4080808010002000, 0x0008008010060880,
    0xA000050008001100, 0x4800808004000200, 0x2400040041021028, 0x0008020008610084,
    0x4080400080008022, 0x000C400C80200485, 0x0010200080100882, 0x0208100080080080,
    0x0048080080800400, 0x0021002900021400, 0x0000280400010250, 0x400A004200008421,
    0x0080002000C00042, 0x0800201000400040, 0x0489001041002000, 0x8010080080801000,
    0x6120800800800400, 0x8101008209000400, 0x8060904104000208, 0x011029114E00008C,
    0x0000401080288000, 0x0000201000404000, 0x8230040800202000, 0x0204201005010008,
    0x0008000400088080, 0x0002020004008080, 0x0140040200010100, 0x2100810080420004,
    0x0080004003200240, 0x00009060C0030300, 0x1044144104200100, 0x400040100A002200,
    0x0010800800040080, 0x2280020080040080, 0x0000100881020400, 0xD082241044870200,
    0x3005218001015043, 0x880200104A210082, 0x02112200C039D082, 0x0800100004210109,
    0x0051000204080011, 0x8011000812040025, 0x00A03008108A212C, 0x10810C0301402082
    };
static uint64 BMult[64] =
    {
    0x26101008A48C0040, 0x5208021084290011, 0x0211041400404100, 0x0420920040050020,
    0x8A44042040010AA0, 0xC200901068402000, 0x0004110482202582, 0x800040209008A006,
    0x0042409084210040, 0x0000119002044040, 0x2000100400544008, 0x0080080A08200004,
    0x0000340422080402, 0x0000309010089620, 0x8249140C88043000, 0x9260008201108200,
    0xA008444019880482, 0x0110400210060490, 0x0008041000401020, 0x0004001801A06010,
    0x09A2211400A01808, 0x001080811000A003, 0x8401802602B00801, 0x30102802020A024A,
    0x8020044012242844, 0x1016132010040800, 0x0208040002003202, 0x0020080001004008,
    0x0401001001004010, 0x1004090008104208, 0x0098025000A20810, 0x2421102001041100,
    0x4201901005082001, 0x2001012000101C80, 0x1064020809011440, 0x0032200804010104,
    0x04200C8400008220, 0x0108044900009000, 0x0102082450090400, 0x0A01112604190243,
    0x0012022021000420, 0x204A0A0F04402010, 0x240604A028038400, 0xC0000A0122007408,
    0x0400085902408C01, 0x0051A01800800440, 0x0190100081204080, 0x0090850042842100,
    0x0032014320070100, 0x4028240A18048840, 0x0143404404040004, 0x4121130042020018,
    0x0000010610440002, 0x0438602002022000, 0x01481050808900E1, 0x020D04180B410000,
    0x0402008201012011, 0x0000804408880812, 0x00000050444C5000, 0x1820000048420226,
    0x0820000010020204, 0x0004084044880084, 0x1800602002008520, 0x1264043818010098,
    };
static uint64 BishopAtt(uint64 O, int sq)
    {
    uint64 T = 0;
    int f, r;
    f = File(sq);
    r = Rank(sq);
    for (f--, r--; f >= FA && r >= R1 && !(SqSet[8 * r + f]&O); f--, r--)
        T |= SqSet[8 * r + f];
    if (f > FA && r > R1)
        T |= SqSet[8 * r + f];
    f = File(sq);
    r = Rank(sq);
    for (f++, r--; f <= FH && r >= R1 && !(SqSet[8 * r + f]&O); f++, r--)
        T |= SqSet[8 * r + f];
    if (f < FH && r > R1)
        T |= SqSet[8 * r + f];
    f = File(sq);
    r = Rank(sq);
    for (f++, r++; f <= FH && r <= R8 && !(SqSet[8 * r + f]&O); f++, r++)
        T |= SqSet[8 * r + f];
    if (f < FH && r < R8)
        T |= SqSet[8 * r + f];
    f = File(sq);
    r = Rank(sq);
    for (f--, r++; f >= FA && r <= R8 && !(SqSet[8 * r + f]&O); f--, r++)
        T |= SqSet[8 * r + f];
    if (f > FA && r < R8)
        T |= SqSet[8 * r + f];
    return T;
    }
static uint64 RookAtt(uint64 O, int sq)
    {
    uint64 T = 0;
    int f, r;
    f = File(sq);
    r = Rank(sq);
    for (f--; f >= FA && !(SqSet[8 * r + f]&O); f--)
        T |= SqSet[8 * r + f];
    if (f > FA)
        T |= SqSet[8 * r + f];
    f = File(sq);
    r = Rank(sq);
    for (f++; f <= FH && !(SqSet[8 * r + f]&O); f++)
        T |= SqSet[8 * r + f];
    if (f < FH)
        T |= SqSet[8 * r + f];
    f = File(sq);
    r = Rank(sq);
    for (r++; r <= R8 && !(SqSet[8 * r + f]&O); r++)
        T |= SqSet[8 * r + f];
    if (r < R8)
        T |= SqSet[8 * r + f];
    f = File(sq);
    r = Rank(sq);
    for (r--; r >= R1 && !(SqSet[8 * r + f]&O); r--)
        T |= SqSet[8 * r + f];
    if (r > R1)
        T |= SqSet[8 * r + f];
    return T;
    }
void MagicMultInit()
    {
    int sq, ATT[64], i, j, cnt, b;
    uint64 T;
    BI[A1] = RI[A1] = 0;
    for (sq = A1; sq < H8; sq++)
        {
        BI[sq + 1] = BI[sq] + (1 << BS[sq]);
        RI[sq + 1] = RI[sq] + (1 << RS[sq]);
        }
    for (sq = A1; sq <= H8; sq++)
        {
        BMask[sq] = Diag[sq] & 0x007e7e7e7e7e7e00;
        RMask[sq] = Ortho[sq];
        if (Rank(sq) != R1)
            RMask[sq] &= ~Rank1;
        if (Rank(sq) != R8)
            RMask[sq] &= ~Rank8;
        if (File(sq) != FA)
            RMask[sq] &= ~FileA;
        if (File(sq) != FH)
            RMask[sq] &= ~FileH;
        }
    for (sq = A1; sq <= H8; sq++)
        {
        BishopMM[sq].index = MMDiag + BI[sq];
        RookMM[sq].index = MMOrtho + RI[sq];
        BishopMM[sq].shift = 64 - BS[sq];
        RookMM[sq].shift = 64 - RS[sq];
        RookMM[sq].mult = RMult[sq];
        BishopMM[sq].mult = BMult[sq];
        RookMM[sq].mask = RMask[sq];
        BishopMM[sq].mask = BMask[sq];
        }
    for (sq = A1; sq <= H8; sq++)
        {
        T = BMask[sq];
        cnt = 0;
        while (T)
            {
            b = BSF(T);
            BitClear(b, T);
            ATT[cnt++] = b;
            }
        for (i = 0; i < (1 << cnt); i++)
            {
            T = 0;
            for (j = 0; j < cnt; j++)
                if (i &(1 << j))
                    T |= 1ULL << ATT[j];
            BishopMM[sq].index[(T *BishopMM[sq].mult) >> BishopMM[sq].shift] = BishopAtt(T, sq);
            }
        T = RMask[sq];
        cnt = 0;
        while (T)
            {
            b = BSF(T);
            BitClear(b, T);
            ATT[cnt++] = b;
            }
        for (i = 0; i < (1 << cnt); i++)
            {
            T = 0;
            for (j = 0; j < cnt; j++)
                if (i &(1 << j))
                    T |= 1ULL << ATT[j];
            RookMM[sq].index[(T *RookMM[sq].mult) >> RookMM[sq].shift] = RookAtt(T, sq);
            }
        }
    }
