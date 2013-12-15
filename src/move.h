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
*****************************************************************************/

#define WhiteInCheck (Position->Dyn->bAtt & wBitboardK)
#define BlackInCheck (Position->Dyn->wAtt & bBitboardK)
#define Pos1 (Pos0 + 1)
#define MoveIsCheckWhite (Pos1->wKcheck)
#define MoveIsCheckBlack (Pos1->bKcheck)
#define Height(x) ((x)->height)
#define MaxPly 2048
#define WhiteOO (Position->Dyn->oo & 0x1)
#define WhiteOOO (Position->Dyn->oo & 0x2)
#define BlackOO (Position->Dyn->oo & 0x4)
#define BlackOOO (Position->Dyn->oo & 0x8)
#define FlagEP 030000
#define FlagOO 010000
#define FlagMask 070000
#define FlagPromQ 070000
#define FlagPromR 060000
#define FlagPromB 050000
#define FlagPromN 040000
#define MoveIsEP(x) (((x) & FlagMask) == FlagEP)
#define MoveIsProm(x) (((x) & FlagMask) >= FlagPromN)
#define MoveIsOO(x) (((x) & FlagMask) == FlagOO)
#define MoveHistory(x) (((x) & 060000) == 0)
#define Direction_h1a8 0
#define Direction_a1h8 1
#define Direction_horz 2
#define Direction_vert 3
#define BadDirection 37
#define MoveNone 0
typedef struct
    {
    uint32 move;
    } typeMoveList;
typedef struct
    {
    uint32 move;
    sint32 value;
    uint64 nodes;
    } typeRootMoveList;
typedef struct
    {
    int phase, mask, bc;
    uint32 trans_move, move, exclude;
    uint64 Target;
    typeMoveList List[256];
    uint32 BadCaps[64];
    } typeNext;

typedef enum
    {
    Trans1,
    CaptureGen,
    Capture_Moves,
    Killer1,
    Killer2,
    Ordinary_Moves,
    BadCaps,
    Trans2,
    Capture_PGen2,
    CaptureMoves2,
    QuietChecks,
    Evade_Phase,
    Trans3,
    CaptureGen3,
    CaptureMoves3,
    QuietChecks3,
   PositionalGainPhase,
    Fase0
    } EnumPhases;

typedef struct
    {
    uint32 move;
    sint32 Value, alpha, beta;
    uint32 depth, _0;
    uint64 nodes;
    } typeMPV;
typeMPV MPV[256];
int MultiPV;
bool DoSearchMoves;
uint32 SearchMoves[256];
