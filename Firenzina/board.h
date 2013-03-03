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

#define File(s) ((s) & 7)
#define Rank(s) ((s) >> 3)
#define From(s) (((s) >> 6) & 077)
#define To(s) ((s) & 077)
typedef enum
    {
	wEnumOcc, wEnumP, wEnumN, wEnumK, wEnumBL, wEnumBD, wEnumR, wEnumQ,
	bEnumOcc, bEnumP, bEnumN, bEnumK, bEnumBL, bEnumBD, bEnumR, bEnumQ
    } EnumPieces;
typedef enum
    {
	A1, B1, C1, D1, E1, F1, G1, H1, A2, B2, C2, D2, E2, F2, G2, H2,
	A3, B3, C3, D3, E3, F3, G3, H3, A4, B4, C4, D4, E4, F4, G4, H4,
	A5, B5, C5, D5, E5, F5, G5, H5, A6, B6, C6, D6, E6, F6, G6, H6,
	A7, B7, C7, D7, E7, F7, G7, H7, A8, B8, C8, D8, E8, F8, G8, H8
    } EnumSquares;
typedef enum
    {
	R1, R2, R3, R4, R5, R6, R7, R8
    } EnumRanks;
typedef enum
    {
	FA, FB, FC, FD, FE, FF, FG, FH 
    } EnumFiles;
#define wBitboardK Position->bitboard[wEnumK]
#define wBitboardQ Position->bitboard[wEnumQ]
#define wBitboardR Position->bitboard[wEnumR]
#define wBitboardBL Position->bitboard[wEnumBL]
#define wBitboardBD Position->bitboard[wEnumBD]
#define wBitboardB (wBitboardBL|wBitboardBD)
#define wBitboardN Position->bitboard[wEnumN]
#define wBitboardP Position->bitboard[wEnumP]
#define wBitboardOcc Position->bitboard[wEnumOcc]
#define bBitboardK Position->bitboard[bEnumK]
#define bBitboardQ Position->bitboard[bEnumQ]
#define bBitboardR Position->bitboard[bEnumR]
#define bBitboardBL Position->bitboard[bEnumBL]
#define bBitboardBD Position->bitboard[bEnumBD]
#define bBitboardB (bBitboardBL|bBitboardBD)
#define bBitboardN Position->bitboard[bEnumN]
#define bBitboardP Position->bitboard[bEnumP]
#define bBitboardOcc Position->bitboard[bEnumOcc]

#ifdef OneDimensional
#define ShiftLeft45 (LineShift + 0100 * Direction_h1a8)
#define ShiftRight45 (LineShift + 0100 * Direction_a1h8)
#define ShiftAttack (LineShift + 0100 * Direction_horz)
#define ShiftLeft90 (LineShift + 0100 * Direction_vert)
#define AttLeft45 (LineMask + 0100 * 0100 * Direction_h1a8)
#define AttRight45 (LineMask + 0100 * 0100 * Direction_a1h8)
#define AttNormal (LineMask + 0100 * 0100 * Direction_horz)
#define AttLeft90 (LineMask + 0100 * 0100 * Direction_vert)
#else
#define ShiftLeft45 LineShift[Direction_h1a8]
#define ShiftRight45 LineShift[Direction_a1h8]
#define ShiftAttack LineShift[Direction_horz]
#define ShiftLeft90 LineShift[Direction_vert]
#define AttLeft45 LineMask[Direction_h1a8]
#define AttRight45 LineMask[Direction_a1h8]
#define AttNormal LineMask[Direction_horz]
#define AttLeft90 LineMask[Direction_vert]
#endif

typedef struct
    {
    uint64 mask, mult, shift;
    uint64 *index;
    } typeMM;
#define AttRocc(sq, OCC) RookMM[sq].index   \
  [((OCC & RookMM[sq].mask) * RookMM[sq].mult) >> RookMM[sq].shift]
#define AttBocc(sq, OCC) BishopMM[sq].index \
  [((OCC & BishopMM[sq].mask) * BishopMM[sq].mult) >> BishopMM[sq].shift]
#define AttB(sq) AttBocc (sq, Position->OccupiedBW)
#define AttR(sq) AttRocc (sq, Position->OccupiedBW)
#define AttFile(sq) (AttR (sq) & FileArray[File (sq)])
#define AttQ(fr) (AttR(fr) | AttB(fr))
#define MaxStack 256
#define NumBytesToCopy (64 + 128 + 32 + 128 + 4)

struct TP
    {
    uint8 sq[64];
    uint64 bitboard[16];
    uint64 OccupiedBW, _0, _1, _2;
    uint8 XrayW[64], XrayB[64];
    uint8 wtm, wKsq, bKsq, height;
    typeDynamic *Dyn, *DynRoot;
    uint64 Stack[MaxStack], StackHeight;
    uint64 nodes;

#ifdef RobboBases
    uint64 tbhits;
#endif

    uint8 cpu, rp;
    bool stop, used;
    MutexType padlock[1];
    int ChildCount;
    struct TP *parent, *children[MaxCPUs];
    SplitPoint *SplitPoint;
    };
typedef struct TP typePos;