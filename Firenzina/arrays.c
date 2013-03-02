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

#define Distance(i, j)(MAX (FileDistance (i, j), RankDistance (i, j)))

const uint8 Left90[64] =
    {
    7, 15, 23, 31, 39, 47, 55, 63,
    6, 14, 22, 30, 38, 46, 54, 62,
    5, 13, 21, 29, 37, 45, 53, 61,
    4, 12, 20, 28, 36, 44, 52, 60,
    3, 11, 19, 27, 35, 43, 51, 59,
    2, 10, 18, 26, 34, 42, 50, 58,
    1, 9, 17, 25, 33, 41, 49, 57,
    0, 8, 16, 24, 32, 40, 48, 56
    };
const uint8 Left45[64] =
    {
    0, 2, 5, 9, 14, 20, 27, 35,
    1, 4, 8, 13, 19, 26, 34, 42,
    3, 7, 12, 18, 25, 33, 41, 48,
    6, 11, 17, 24, 32, 40, 47, 53,
    10, 16, 23, 31, 39, 46, 52, 57,
    15, 22, 30, 38, 45, 51, 56, 60,
    21, 29, 37, 44, 50, 55, 59, 62,
    28, 36, 43, 49, 54, 58, 61, 63
    };
const uint8 Right45[64] =
    {
    28, 21, 15, 10, 6, 3, 1, 0,
    36, 29, 22, 16, 11, 7, 4, 2,
    43, 37, 30, 23, 17, 12, 8, 5,
    49, 44, 38, 31, 24, 18, 13, 9,
    54, 50, 45, 39, 32, 25, 19, 14,
    58, 55, 51, 46, 40, 33, 26, 20,
    61, 59, 56, 52, 47, 41, 34, 27,
    63, 62, 60, 57, 53, 48, 42, 35
    };
static int Left54[64], Right54[64], Left09[64];
static int Shift[64] =
    {
    1,
    2, 2,
    4, 4, 4,
    7, 7, 7, 7,
    11, 11, 11, 11, 11,
    16, 16, 16, 16, 16, 16,
    22, 22, 22, 22, 22, 22, 22,
    29, 29, 29, 29, 29, 29, 29, 29,
    37, 37, 37, 37, 37, 37, 37,
    44, 44, 44, 44, 44, 44,
    50, 50, 50, 50, 50,
    55, 55, 55, 55,
    59, 59, 59,
    62, 62,
    64
  };
static int Length[64], Where[64];
static int Hop[8] =
    {
    6, 10, 15, 17, -6, -10, -15, -17
    };
static uint64 randkey = 1;
uint16 RAND16()
    {
    randkey = randkey * 8765432181103515245ULL + 1234567891ULL;
    return((randkey >> 32) % 65536);
    }
uint64 GetRand()
    {
    return(((uint64)RAND16()) << 48) | (((uint64)RAND16()) << 32) |
		(((uint64)RAND16()) << 16) | (((uint64)RAND16()) << 0);
    }

void RandHash()
    {
    int i, j;

    GetRand();
    HashCastling[0] = 0;
    HashCastling[1] = GetRand();
    HashCastling[2] = GetRand();
    HashCastling[4] = GetRand();
    HashCastling[8] = GetRand();
    for (i = 0; i < 16; i++)
        {
        if (POPCNT(i) < 2)
            continue;
        HashCastling[i] = 0;
        for (j = 1; j < 16; j <<= 1)
            if (i & j)
                HashCastling[i] ^= HashCastling[j];
        }
    for (i = 0; i < 16; i++)
        for (j = A1; j <= H8; j++)
            Hash(i, j) = GetRand();
    for (i = FA; i <= FH; i++)
        HashEP[i] = GetRand();
    for (i = 0; i < 16; i++)
        HashRev[i] = 0ULL;

    InitRandom32(GetRand());
    }
void InitArrays()
    {
    int sq2, i, sq = 0, j, file, rank, king, dir;
    uint64 T;

    for (i = 1; i <= 8; i++)
        for (j = 1; j <= i; j++)
            {
            Length[sq] = i;
            Where[sq++] = j - 1;
            }
    for (i = 7; i >= 1; i--)
        for (j = 1; j <= i; j++)
            {
            Length[sq] = i;
            Where[sq++] = j - 1;
            }
    for (i = A1; i <= H8; i++)
        {
        Left54[Left45[i]] = i;
        Left09[Left90[i]] = i;
        Right54[Right45[i]] = i;
        }
    for (i = A1; i <= H8; i++)
        {
        SqSet[i] = 0;
        BitSet(i, SqSet[i]);
        SqClear[i] = ~SqSet[i];
        }

    for (i = A1; i <= H8; i++)
        {
        AttN[i] = 0;
        for (j = 0; j < 8; j++)
            {
            sq = i + Hop[j];
            if ((sq < A1) || (sq > H8))
                continue;
            if ((FileDistance(i, sq) > 2) || (RankDistance(i, sq) > 2))
                continue;
            BitSet(sq, AttN[i]);
            }
        }
    for (i = A1; i <= H8; i++)
        {
        AttK[i] = 0;
        for (j = A1; j <= H8; j++)
            {
            if (MAX(FileDistance(i, j), RankDistance(i, j)) == 1)
                BitSet(j, AttK[i]);
            }
        }
    for (i = A1; i <= H1; i++)
        {
        AttPw[i] = 0;
        AttPb[i] = SqSet[i + 7] | SqSet[i + 9];
        }
    for (i = A2; i <= H7; i++)
        {
        AttPw[i] = SqSet[i - 7] | SqSet[i - 9];
        AttPb[i] = SqSet[i + 7] | SqSet[i + 9];
        }
    for (i = A8; i <= H8; i++)
        {
        AttPb[i] = 0;
        AttPw[i] = SqSet[i - 7] | SqSet[i - 9];
        }
    for (i = A1; i <= A8; i += 8)
        {
        AttPw[i] = SqSet[i - 7];
        AttPb[i] = SqSet[i + 9];
        }
    for (i = H1; i <= H8; i += 8)
        {
        AttPw[i] = SqSet[i - 9];
        AttPb[i] = SqSet[i + 7];
        }
    AttPw[A1] = 0;
    AttPw[A2] = SqSet[B1];
    AttPb[A7] = SqSet[B8];
    AttPb[A8] = 0;
    AttPw[H1] = 0;
    AttPw[H2] = SqSet[G1];
    AttPb[H7] = SqSet[G8];
    AttPb[H8] = 0;
    IsolatedFiles[FA] = FileB;
    IsolatedFiles[FH] = FileG;
    for (file = FB; file <= FG; file++)
        IsolatedFiles[file] = FileArray[file - 1] | FileArray[file + 1];
    for (sq = A1; sq <= H8; sq++)
        {
        IsolatedPawnW[sq] = 0;
        IsolatedPawnB[sq] = 0;
        file = File(sq);
        rank = Rank(sq);
        if (rank < R8)
            IsolatedPawnW[sq] |= IsolatedFiles[file] & RankArray[rank + 1];
        if (rank < R7)
            IsolatedPawnW[sq] |= IsolatedFiles[file] & RankArray[rank + 2];
        if (rank > R1)
            IsolatedPawnB[sq] |= IsolatedFiles[file] & RankArray[rank - 1];
        if (rank > R2)
            IsolatedPawnB[sq] |= IsolatedFiles[file] & RankArray[rank - 2];
        ConnectedPawns[sq] = IsolatedPawnW[sq] | IsolatedPawnB[sq] | (RankArray[rank] & IsolatedFiles[file]);
        }
    for (rank = R1; rank <= R8; rank++)
        {
        InFrontW[rank] = 0;
        for (j = rank + 1; j <= R8; j++)
            InFrontW[rank] |= RankArray[j];
        NotInFrontW[rank] = ~InFrontW[rank];
        }
    for (rank = R8; rank >= R1; rank--)
        {
        InFrontB[rank] = 0;
        for (j = rank - 1; j >= R1; j--)
            InFrontB[rank] |= RankArray[j];
        NotInFrontB[rank] = ~InFrontB[rank];
        }

    for (sq = A1; sq <= H8; sq++)
        {
        PassedPawnW[sq] = (IsolatedFiles[File(sq)] | FileArray[File(sq)]) & InFrontW[Rank(sq)];
        PassedPawnB[sq] = (IsolatedFiles[File(sq)] | FileArray[File(sq)]) & InFrontB[Rank(sq)];
        }
    for (sq = A1; sq <= H8; sq++)
        {
        if (File(sq) >= FC)
            Left2[sq] = SqSet[sq - 2];
        else
            Left2[sq] = 0;
        if (File(sq) <= FF)
            Right2[sq] = SqSet[sq + 2];
        else
            Right2[sq] = 0;
        if (File(sq) >= FB)
            Left1[sq] = SqSet[sq - 1];
        else
            Left1[sq] = 0;
        if (File(sq) <= FG)
            Right1[sq] = SqSet[sq + 1];
        else
            Right1[sq] = 0;
        Adjacent[sq] = Left1[sq] | Right1[sq];
        }
    for (sq = A1; sq <= H8; sq++)
        {
        ProtectedPawnW[sq] = (IsolatedFiles[File(sq)]) &NotInFrontW[Rank(sq)];
        ProtectedPawnB[sq] = (IsolatedFiles[File(sq)]) &NotInFrontB[Rank(sq)];
        }
    for (sq = A1; sq <= H8; sq++)
        {
        file = File(sq);
        rank = Rank(sq);
        LongDiag[sq] = 0;
        if (file <= FD)
            {
            while (file < FH && rank < R8)
                {
                file++;
                rank++;
                LongDiag[sq] |= SqSet[(rank << 3) + file];
                }
            file = File(sq);
            rank = Rank(sq);
            while (file < FH && rank > R1)
                {
                file++;
                rank--;
                LongDiag[sq] |= SqSet[(rank << 3) + file];
                }
            }
        else
            {
            while (file > FA && rank < R8)
                {
                file--;
                rank++;
                LongDiag[sq] |= SqSet[(rank << 3) + file];
                }
            file = File(sq);
            rank = Rank(sq);
            while (file > FA && rank > R1)
                {
                file--;
                rank--;
                LongDiag[sq] |= SqSet[(rank << 3) + file];
                }
            }
        }
    for (sq = A1; sq <= H8; sq++)
        OpenFileW[sq] = FileArray[File(sq)] & InFrontW[Rank(sq)];
    for (sq = A1; sq <= H8; sq++)
        OpenFileB[sq] = FileArray[File(sq)] & InFrontB[Rank(sq)];
    for (sq = A1; sq <= H8; sq++)
        Doubled[sq] = FileArray[File(sq)] ^ (1ULL << sq);

    for (sq = A1; sq <= H8; sq++)
        {
        QuadrantBKwtm[sq] = QuadrantBKbtm[sq] = 0;
        j = (sq & 7) + 56;
        if (Rank(sq) == R2)
            sq2 = sq + 8;
        else
            sq2 = sq;
        for (i = A1; i <= H8; i++)
            {
            if (Distance(sq2, j) < Distance(j, i) - 1)
                BitSet(i, QuadrantBKbtm[sq]);
            if (Distance(sq2, j) < Distance(j, i))
                BitSet(i, QuadrantBKwtm[sq]);
            }
        }
    for (sq = A1; sq <= H8; sq++)
        {
        QuadrantWKwtm[sq] = QuadrantWKbtm[sq] = 0;
        j = (sq & 7);
        if (Rank(sq) == R7)
            sq2 = sq - 8;
        else
            sq2 = sq;
        for (i = A1; i <= H8; i++)
            {
            if (Distance(sq2, j) < Distance(j, i) - 1)
                BitSet(i, QuadrantWKwtm[sq]);
            if (Distance(sq2, j) < Distance(j, i))
                BitSet(i, QuadrantWKbtm[sq]);
            }
        }
    for (sq = A1; sq <= H8; sq++)
        {
        ShepherdWK[sq] = ShepherdBK[sq] = 0;
        file = File(sq);
        if (file == FA || file == FH)
            T = IsolatedFiles[file];
        else
            T = IsolatedFiles[file] | FileArray[file];
        if (Rank(sq) >= R6)
            ShepherdWK[sq] |= (T & Rank8);
        if (Rank(sq) >= R5)
            ShepherdWK[sq] |= (T & Rank7);
        if (Rank(sq) <= R3)
            ShepherdBK[sq] |= (T & Rank1);
        if (Rank(sq) <= R4)
            ShepherdBK[sq] |= (T & Rank2);
        }
    for (sq = A1; sq <= H8; sq++)
        {
        Northwest[sq] = (Rank(sq) != R8 && File(sq) != FA) ? SqSet[sq + 7] : 0;
        Northeast[sq] = (Rank(sq) != R8 && File(sq) != FH) ? SqSet[sq + 9] : 0;
        Southwest[sq] = (Rank(sq) != R1 && File(sq) != FA) ? SqSet[sq - 9] : 0;
        Souteast[sq] = (Rank(sq) != R1 && File(sq) != FH) ? SqSet[sq - 7] : 0;
        }
    for (sq = A1; sq <= H8; sq++)
        for (king = A1; king <= H8; king++)
            {
            Evade(king, sq) = AttK[king];
            if (Rank(king) == Rank(sq))
                {
                if (File(king) != FA)
                    Evade(king, sq) ^= SqSet[king - 1];
                if (File(king) != FH)
                    Evade(king, sq) ^= SqSet[king + 1];
                }
            if (File(king) == File(sq))
                {
                if (Rank(king) != R1)
                    Evade(king, sq) ^= SqSet[king - 8];
                if (Rank(king) != R8)
                    Evade(king, sq) ^= SqSet[king + 8];
                }
            if ((Rank(king) - Rank(sq)) == (File(king) - File(sq)))
                {
                if (Rank(king) != R8 && File(king) != FH)
                    Evade(king, sq) ^= SqSet[king + 9];
                if (Rank(king) != R1 && File(king) != FA)
                    Evade(king, sq) ^= SqSet[king - 9];
                }
            if ((Rank(king) - Rank(sq)) == (File(sq) - File(king)))
                {
                if (Rank(king) != R8 && File(king) != FA)
                    Evade(king, sq) ^= SqSet[king + 7];
                if (Rank(king) != R1 && File(king) != FH)
                    Evade(king, sq) ^= SqSet[king - 7];
                }
            if (AttK[king] & SqSet[sq])
                Evade(king, sq) |= SqSet[sq];
            }
    for (file = FA; file <= FH; file++)
        {
        FilesLeft[file] = FilesRight[file] = 0;
        for (i = FA; i < file; i++)
            FilesLeft[file] |= FileArray[i];
        for (i = file + 1; i <= FH; i++)
            FilesRight[file] |= FileArray[i];
        }
    for (sq = A1; sq <= H8; sq++)
        for (king = A1; king <= H8; king++)
            {
            InterPose(king, sq) = SqSet[sq];
            dir = 0;
            if (Rank(king) == Rank(sq))
                {
                if (king > sq)
                    dir = 1;
                else
                    dir = -1;
                }
            if (File(king) == File(sq))
                {
                if (king > sq)
                    dir = 8;
                else
                    dir = -8;
                }
            if ((Rank(king) - Rank(sq)) == (File(king) - File(sq)))
                {
                if (king > sq)
                    dir = 9;
                else
                    dir = -9;
                }
            if ((Rank(king) - Rank(sq)) == (File(sq) - File(king)))
                {
                if (king > sq)
                    dir = 7;
                else
                    dir = -7;
                }
            if (dir)
                for (i = sq; i != king; i += dir)
                    BitSet(i, InterPose(king, sq));
            }
    for (sq = A1; sq <= H8; sq++)
        {
        Ortho[sq] = RankArray[Rank(sq)] | FileArray[File(sq)];
        Diag[sq] = 0;
        for (file = File(sq), rank = Rank(sq); file <= FH && rank <= R8; file++, rank++)
            BitSet((rank << 3) + file, Diag[sq]);
        for (file = File(sq), rank = Rank(sq); file <= FH && rank >= R1; file++, rank--)
            BitSet((rank << 3) + file, Diag[sq]);
        for (file = File(sq), rank = Rank(sq); file >= FA && rank <= R8; file--, rank++)
            BitSet((rank << 3) + file, Diag[sq]);
        for (file = File(sq), rank = Rank(sq); file >= FA && rank >= R1; file--, rank--)
            BitSet((rank << 3) + file, Diag[sq]);
        Ortho[sq] &= SqClear[sq];
        Diag[sq] &= SqClear[sq];
        NonOrtho[sq] = ~Ortho[sq];
        NonDiag[sq] = ~Diag[sq];
        OrthoDiag[sq] = Ortho[sq] | Diag[sq];
        }
    for (j = A1; j <= H8; j++)
        for (i = A1; i <= H8; i++)
            {
            Line(i, j) = BadDirection;
            if (i == j)
                continue;
            if (Rank(j) == Rank(i))
                Line(i, j) = Direction_horz;
            if (File(j) == File(i))
                Line(i, j) = Direction_vert;
            if ((File(i) - File(j)) == (Rank(i) - Rank(j)))
                Line(i, j) = Direction_a1h8;
            if ((File(j) - File(i)) == (Rank(i) - Rank(j)))
                Line(i, j) = Direction_h1a8;
            }
    randkey = 1;
    RandHash();
    MagicMultInit();
    }