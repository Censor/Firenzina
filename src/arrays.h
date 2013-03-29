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

#define HashWTM (0x1220ab8c00000004)

#ifdef OneDimensional
#define InterPose(x, y) Interpose[64 * (x) + (y)]
#define Evade(x, y) Evade[64 * (x) + (y)]
#define Line(x, y) Line[64 * (x) + (y)]
#define Hash(pi, sq) Zobrist[64 * (pi) + (sq)]
SlabDeclare3(uint64, Interpose, 0100 * 0100);
SlabDeclare3(uint64, Evade, 0100 * 0100);
SlabDeclare3(sint8, Line, 0100 * 0100);
SlabDeclare3(uint64, Zobrist, 0x10 * 0100);
#else
#define InterPose(x, y) Interpose[x][y]
#define Evade(x, y) Evade[x][y]
#define Line(x, y) Line[x][y]
#define Hash(pi, sq) Zobrist[pi][sq]
uint64 Interpose[64][64], Evade[64][64];
sint8 Line[64][64];
uint64 Zobrist[16][64];
#endif

#define Rank1 0x00000000000000ff
#define Rank2 0x000000000000ff00
#define Rank3 0x0000000000ff0000
#define Rank4 0x00000000ff000000
#define Rank5 0x000000ff00000000
#define Rank6 0x0000ff0000000000
#define Rank7 0x00ff000000000000
#define Rank8 0xff00000000000000

#define FileA 0x0101010101010101
#define FileB 0x0202020202020202
#define FileC 0x0404040404040404
#define FileD 0x0808080808080808
#define FileE 0x1010101010101010
#define FileF 0x2020202020202020
#define FileG 0x4040404040404040
#define FileH 0x8080808080808080

static const uint64 RankArray[8] =
    {
	0x00000000000000ff, 0x000000000000ff00,
	0x0000000000ff0000, 0x00000000ff000000,
	0x000000ff00000000, 0x0000ff0000000000,
	0x00ff000000000000, 0xff00000000000000
    };
static const uint64 FileArray[8] =
    {
	0x0101010101010101, 0x0202020202020202,
	0x0404040404040404, 0x0808080808080808,
	0x1010101010101010, 0x2020202020202020,
	0x4040404040404040, 0x8080808080808080
    };

SlabDeclare1(uint64, SqSet, 64);
SlabDeclare1(uint64, SqClear, 64);
SlabDeclare1(uint64, NonDiag, 64);
SlabDeclare1(uint64, NonOrtho, 64);
SlabDeclare1(uint64, Ortho, 64);
SlabDeclare1(uint64, Diag, 64);
SlabDeclare1(uint64, OrthoDiag, 64);
SlabDeclare1(uint64, OpenFileW, 64);
SlabDeclare1(uint64, OpenFileB, 64);
SlabDeclare1(uint64, PassedPawnW, 64);
SlabDeclare1(uint64, PassedPawnB, 64);
SlabDeclare1(uint64, ProtectedPawnW, 64);
SlabDeclare1(uint64, ProtectedPawnB, 64);
SlabDeclare1(uint64, IsolatedPawnW, 64);
SlabDeclare1(uint64, IsolatedPawnB, 64);
SlabDeclare1(uint64, ConnectedPawns, 64);
SlabDeclare1(uint64, InFrontW, 8);
SlabDeclare1(uint64, NotInFrontW, 8);
SlabDeclare1(uint64, InFrontB, 8);
SlabDeclare1(uint64, NotInFrontB, 8);
SlabDeclare1(uint64, IsolatedFiles, 8);
SlabDeclare1(uint64, FilesLeft, 8);
SlabDeclare1(uint64, FilesRight, 8);
SlabDeclare1(uint64, Doubled, 64);
SlabDeclare1(uint64, Left2, 64);
SlabDeclare1(uint64, Right2, 64);
SlabDeclare1(uint64, Left1, 64);
SlabDeclare1(uint64, Right1, 64);
SlabDeclare1(uint64, Adjacent, 64);
SlabDeclare1(uint64, LongDiag, 64);
SlabDeclare1(uint64, Northwest, 64);
SlabDeclare1(uint64, Southwest, 64);
SlabDeclare1(uint64, Northeast, 64);
SlabDeclare1(uint64, Souteast, 64);
SlabDeclare1(uint64, QuadrantWKwtm, 64);
SlabDeclare1(uint64, QuadrantBKwtm, 64);
SlabDeclare1(uint64, QuadrantWKbtm, 64);
SlabDeclare1(uint64, QuadrantBKbtm, 64);
SlabDeclare1(uint64, ShepherdWK, 64);
SlabDeclare1(uint64, ShepherdBK, 64);
SlabDeclare1(uint64, HashCastling, 16);
SlabDeclare1(uint64, HashEP, 8);
SlabDeclare1(uint64, HashRev, 16);
SlabDeclare1(uint64, AttN, 64);
SlabDeclare1(uint64, AttK, 64);
SlabDeclare1(uint64, AttPw, 64);
SlabDeclare1(uint64, AttPb, 64);

static const uint64 Ranks2to6NotA = 0x0000fefefefefe00;
static const uint64 Ranks2to6NotAB = 0x0000fcfcfcfcfc00;
static const uint64 Ranks2to6 = 0x0000ffffffffff00;
static const uint64 Ranks2to6NotH = 0x00007f7f7f7f7f00;
static const uint64 Ranks2to6NotGH = 0x00003f3f3f3f3f00;
static const uint64 Ranks3to7NotA = 0x00fefefefefe0000;
static const uint64 Ranks3to7NotAB = 0x00fcfcfcfcfc0000;
static const uint64 Ranks3to7 = 0x00ffffffffffff0000;
static const uint64 Ranks3to7NotGH = 0x003f3f3f3f3f0000;
static const uint64 Ranks3to7NotH = 0x007f7f7f7f7f0000;
static const uint64 White = 0x55aa55aa55aa55aa;
static const uint64 Black = 0xaa55aa55aa55aa55;
static const uint64 Rank2NotA = 0x000000000000fe00;
static const uint64 Rank2NotH = 0x0000000000007f00;
static const uint64 Rank7NotA = 0x00fe000000000000;
static const uint64 Rank7NotH = 0x007f000000000000;
static const uint64 NotA = 0xfefefefefefefefe;
static const uint64 NotH = 0x7f7f7f7f7f7f7f7f;
static const uint64 F1G1 = 0x0000000000000060;
static const uint64 C1D1 = 0x000000000000000c;
static const uint64 B1C1D1 = 0x000000000000000e;
static const uint64 F8G8 = 0x6000000000000000;
static const uint64 C8D8 = 0x0c00000000000000;
static const uint64 B8C8D8 = 0x0e00000000000000;
