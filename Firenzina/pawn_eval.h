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

#define Score(x,y) (((x) << 16) + (y))
#define WhiteKingPawnDistance(pawn, king)                             \
  MAX (((king > pawn) ? 3 : 6) * ABS (Rank (pawn) - Rank (king)), \
        6 * ABS (File (pawn) - File (king)))
#define BlackKingPawnDistance(pawn, king)                             \
  MAX (((king < pawn) ? 3 : 6) * ABS (Rank (pawn) - Rank (king)), \
        6 * ABS (File (pawn) - File (king)))
static uint8 OpposingPawnsMult[9] =
    {
    6, 5, 4, 3, 2, 1, 0, 0, 0
    };
static uint8 PawnCountMult[9] =
    {
    6, 5, 4, 3, 2, 1, 0, 0, 0
    };
static const uint8 BlockedPawnValue[64] =
    {
	0, 0, 0, 0, 0, 0, 0, 0,
	1, 1, 2, 2, 2, 2, 1, 1,
	1, 2, 3, 3, 3, 3, 2, 1,
	1, 2, 3, 5, 5, 3, 2, 1,
	1, 2, 3, 5, 5, 3, 2, 1,
	1, 2, 3, 3, 3, 3, 2, 1,
	1, 1, 2, 2, 2, 2, 1, 1,
	0, 0, 0, 0, 0, 0, 0, 0

    };
static const sint32 MyKingPawnDistance[8] =
    {
    0, 0, 0, 1, 2, 3, 5, 0
    };
static const sint32 OppKingPawnDistance[8] =
    {
    0, 0, 0, 2, 4, 6, 10, 0
    };
static const sint32 PassedPawnValue[8] =
    {
    Score(0, 0), Score(0, 0), Score(0, 0), Score(10, 10),
	Score(20, 25), Score(40, 50), Score(60, 75), Score(0, 0)
    };
static const sint32 OutsidePassedPawnValue[8] =
    {
    Score(0, 0), Score(0, 0), Score(0, 0), Score(0, 0),
	Score(2, 5), Score(5, 10), Score(10, 20), Score(0, 0)
    };
static const sint32 ProtectedPassedPawnValue[8] =
    {
    Score(0, 0), Score(0, 0), Score(0, 0), Score(0, 0),
	Score(5, 10), Score(10, 15), Score(15, 25), Score(0, 0)
    };
static const sint32 ConnectedPassedPawnValue[8] =
    {
    Score(0, 0), Score(0, 0), Score(0, 0), Score(0, 0),
	Score(5, 10), Score(10, 15), Score(20, 30), Score(0, 0)
    };
static const sint32 CandidatePawnValue[8] =
    {
    Score(0, 0), Score(0, 0), Score(0, 0), Score(5, 5),
	Score(10, 12), Score(20, 25), Score(0, 0), Score(0, 0)
    };
#define Islands Score(0, 3)
#define Hole Score(1, 2)
#define DoubledClosed Score(2, 4)
#define DoubledOpen Score(4, 8)
#define DoubledClosedIsolated Score(2, 4)
#define DoubledOpenIsolated Score(6, 10)
#define IsolatedClosed Score(5, 8)
#define IsolatedOpen Score(15, 20)
#define BackwardClosed Score(5, 5)
#define BackwardOpen Score(10, 15)
#define KingAttPawn Score(0, 5)
#define KingOO Score(5, 0)
#define KingOOO Score(5, 0)