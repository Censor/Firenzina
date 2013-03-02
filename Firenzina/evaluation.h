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

#define wOutpost 0x00007e7e7e000000
#define bOutpost 0x0000007e7e7e0000
#define Bitboard2(x, y) (1ULL << (x))|(1ULL << (y))
static const uint64 RookTrapped[64] =
    {
	0, Bitboard2 (A1, A2), Bitboard2 (A1, A2) | Bitboard2 (B1, B2), 0,
	0, Bitboard2 (H1, H2) | Bitboard2 (G1, G2), Bitboard2 (H1, H2), 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, Bitboard2 (A8, A7), Bitboard2 (A8, A7) | Bitboard2 (B8, B7), 0,
	0, Bitboard2 (H8, H7) | Bitboard2 (G8, G7), Bitboard2 (H8, H7), 0
    };
#define Score(x,y) (((x) << 16) + (y))

#define PawnAntiMobility Score (3, 10)
#define MobQ(Y) Score (2, 2) * POPCNT (Y)
#define MobB(Y, Z) Score (5, 5) * POPCNT (Y & Z)
#define MobR(Y) Score (2, 3) * POPCNT (Y)
#define MobN(Y, Z) Score (6, 8) * POPCNT (Y & Z)
#define xrayB0 Score(0, 0)
#define xrayBmP Score(3, 5)
#define xrayBmN Score(3, 5)
#define xrayBmK Score(3, 5)
#define xrayBmB Score(0, 0)
#define xrayBmR Score(3, 5)
#define xrayBmQ Score(0, 0)
#define xrayBoP Score(2, 5)
#define xrayBoN Score(2, 5)
#define xrayBoK Score(0, 0)
#define xrayBoB Score(0, 0)
#define xrayBoR Score(15, 25)
#define xrayBoQ Score(10, 20)
#define xrayR0 Score(0, 0)
#define xrayRmP Score(0, 0)
#define xrayRmN Score(3, 5)
#define xrayRmK Score(3, 5)
#define xrayRmB Score(3, 5)
#define xrayRmR Score(0, 0)
#define xrayRmQ Score(0, 0)
#define xrayRoP Score(2, 5)
#define xrayRoN Score(2, 5)
#define xrayRoK Score(0, 0)
#define xrayRoB Score(2, 5)
#define xrayRoR Score(0, 0)
#define xrayRoQ Score(10, 20)
static const uint32 wBxray[16] =
    {
    xrayB0, xrayBmP, xrayBmN, xrayBmK, xrayBmB, xrayBmB, xrayBmR, xrayBmQ,
	xrayB0, xrayBoP, xrayBoN, xrayBoK, xrayBoB, xrayBoB, xrayBoR, xrayBoQ
    };
static const uint32 bBxray[16] =
    {
    xrayB0, xrayBoP, xrayBoN, xrayBoK, xrayBoB, xrayBoB, xrayBoR, xrayBoQ,
	xrayB0, xrayBmP, xrayBmN, xrayBmK, xrayBmB, xrayBmB, xrayBmR, xrayBmQ
    };
static const uint32 wRxray[16] =
    {
    xrayR0, xrayRmP, xrayRmN, xrayRmK, xrayRmB, xrayRmB, xrayRmR, xrayRmQ,
	xrayR0, xrayRoP, xrayRoN, xrayRoK, xrayRoB, xrayRoB, xrayRoR, xrayRoQ
    };
static const uint32 bRxray[16] =
    {
    xrayR0, xrayRoP, xrayRoN, xrayRoK, xrayRoB, xrayRoB, xrayRoR, xrayRoQ,
	xrayR0, xrayRmP, xrayRmN, xrayRmK, xrayRmB, xrayRmB, xrayRmR, xrayRmQ
    };
#define xQ0d Score(0, 0)
#define xQmPd Score(1, 2)
#define xQmNd Score(2, 4)
#define xQmKd Score(2, 4)
#define xQmBd Score(0, 0)
#define xQmRd Score(2, 4)
#define xQmQd Score(0, 0)
#define xQoPd Score(0, 0)
#define xQoNd Score(2, 5)
#define xQoKd Score(0, 0)
#define xQoBd Score(0, 0)
#define xQoRd Score(2, 5)
#define xQoQd Score(0, 0)
#define xQ0hv Score(0, 0)
#define xQmPhv Score(0, 0)
#define xQmNhv Score(2, 4)
#define xQmKhv Score(2, 4)
#define xQmBhv Score(2, 4)
#define xQmRhv Score(0, 0)
#define xQmQhv Score(0, 0)
#define xQoPhv Score(0, 0)
#define xQoNhv Score(2, 5)
#define xQoKhv Score(0, 0)
#define xQoBhv Score(2, 5)
#define xQoRhv Score(0, 0)
#define xQoQhv Score(0, 0)
static const uint32 wQxrayD[16] =
    {
    xQ0d, xQmPd, xQmNd, xQmKd, xQmBd, xQmBd, xQmRd, xQmQd,
	xQ0d, xQoPd, xQoNd, xQoKd, xQoBd, xQoBd, xQoRd, xQoQd
    };
static const uint32 bQxrayD[16] =
    {
    xQ0d, xQoPd, xQoNd, xQoKd, xQoBd, xQoBd, xQoRd, xQoQd,
	xQ0d, xQmPd, xQmNd, xQmKd, xQmBd, xQmBd, xQmRd, xQmQd
    };
static const uint32 wQxrayO[16] =
    {
    xQ0hv, xQmPhv, xQmNhv, xQmKhv, xQmBhv, xQmBhv, xQmRhv, xQmQhv,
	xQ0hv, xQoPhv, xQoNhv, xQoKhv, xQoBhv, xQoBhv, xQoRhv, xQoQhv
    };
static const uint32 bQxrayO[16] =
    {
    xQ0hv, xQoPhv, xQoNhv, xQoKhv, xQoBhv, xQoBhv, xQoRhv, xQoQhv,
	xQ0hv, xQmPhv, xQmNhv, xQmKhv, xQmBhv, xQmBhv, xQmRhv, xQmQhv
    };
static const uint8 BishopTrapSq[64] =
    {
	0x00,  C2,  0x00, 0x00, 0x00, 0x00,  F2,  0x00,
	B3,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  G3,
	B4,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  G4,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	B5,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  G5,
	B6,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  G6,
	0x00,  C7,  0x00, 0x00, 0x00, 0x00,  F7,  0x00
    };
#define BishopTrapValue Score(40, 40)
#define BishopTrapGuardValue Score(40, 40)
static const uint8 GoodBishopTrapSq[64] =
    {
	0x00,  D1,  0x00, 0x00, 0x00, 0x00,  E1,  0x00,
	C2,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  F2,
	C3,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  F3,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	C6,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  F6,
	C7,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  F7,
	0x00,  D8,  0x00, 0x00, 0x00, 0x00,  E8,  0x00
    };
static const uint32 PassedPawnMeClear[8] =
    {
    0, 0, 0, Score(0, 0), Score(0, 0), Score(3, 5), Score(5, 10), 0
    };
static const uint32 PassedPawnOppClear[8] =
    {
    0, 0, 0, Score(0, 0), Score(5, 10), Score(15, 30), Score(25, 50)
    };
static const uint32 PassedPawnCanMove[8] =
    {
    0, 0, 0, Score(1, 2), Score(2, 3), Score(3, 5), Score(5, 10), 0
    };
static const uint32 PassedPawnIsFree[8] =
    {
    0, 0, 0, Score(0, 0), Score(5, 10), Score(10, 20), Score(20, 40)
    };
#define QguardK Score(5, 2)
#define RguardK Score(3, 1)
#define BguardK Score(2, 1)
#define NguardK Score(4, 2)
#define DoubQueen7th Score(10, 15)
#define DoubRook7thKingPawn Score(10, 20)
#define MultipleAtt Score(15, 25)
#define Queen7th Score(5, 25)
#define KingAttUnguardedPawn Score(0, 5)
#define PattQ Score(8, 12)
#define RattQ Score(5, 5)
#define NattRQ Score(7, 10)
#define bAttRQ Score(7, 10)
#define PattR Score(7, 10)
#define PattN Score(5, 7)
#define PattB Score(5, 7)
#define bAttN Score(5, 5)
#define NattB Score(5, 5)
#define Qatt Score(4, 4)
#define RattBN Score(4, 5)
#define RattP Score(2, 3)
#define NattP Score(3, 4)
#define bAttP Score(3, 4)

#define RookHalfOpen Score(3, 6)
#define RookOpenFile Score(20, 10)
#define RookOpenFixedMinor Score(10, 0)
#define RookOpenMinor Score(15, 5)
#define RookHalfOpenPawn Score(5, 5)
#define RookHalfOpenKing Score(15, 0)
#define RookKing8th Score(5, 10)
#define Rook7thKingPawn Score(10, 30)
#define Rook6thKingPawn Score(5, 15)
#define OutpostBishop Score(1, 2)
#define OutpostBishopGuarded Score(3, 4)
#define OutpostRook Score(1, 2)
#define OutpostRookGuarded Score(3, 4)
#define OutpostKnight Score(2, 3)
#define OutpostKnightPawn Score(2, 3)
#define OutpostKnightAttacks Score(5, 5)
#define OutpostKnight5th Score(2, 2)
#define OutpostKnightONde Score(3, 3)
static const uint32 KingSafetyMult[16] =
    {
    0, 1, 4, 9, 16, 25, 36, 49,
	50, 50, 50, 50, 50, 50, 50, 50
    };
#define Hit(x,y) ((x) << 16) + (y)
#define HitP Hit(1, 0)
#define HitQ Hit(1, 40)
#define HitR Hit(1, 25)
#define HitN Hit(1, 15)
#define HitB Hit(1, 15)
#define HitK Hit(0, 0)
#define KingSafetyDivider 8
static const uint32 RankQueenEnd[8] =
    {
    0, 0, 0, Score(5, 5), Score(10, 10), Score(20, 20), Score(40, 40), 0
    };

#define Rook7thEnd Score(100, 100)
#define Rook6thEnd Score(25, 25)
#define Queen7thEnd Score(10, 10)

static const uint64 CrampFile[8] =
    {
    FileB, 0, 0, 0, 0, 0, 0, FileG
    };