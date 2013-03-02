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

typedef struct
    {
    sint16 Value;
    uint8 token, flags;
    } typeMaterial;
SlabDeclare2(typeMaterial, Material, 419904);

SlabDeclare1(typeMM, RookMM, 64);
SlabDeclare1(typeMM, BishopMM, 64);
SlabDeclare2(uint64, MMOrtho, 102400);
SlabDeclare2(uint64, MMDiag, 5248);

typePos RootPosition[MaxCPUs][RPperCPU];
typePos RootPosition0[1], NullParent[1];

#ifdef OneDimensional
#ifdef MultiplePosGain
SlabDeclare2(sint16, MaxPositionalGain, MaxCPUs * 0x10 * 010000);
#define MaxPosGain(piece, mos)                                        \
	MaxPositionalGain[Position->cpu * 0x10 * 010000 + (piece) * 010000 + (mos)]
#else
SlabDeclare2(sint16, MaxPositionalGain, 0x10 *010000);
#define MaxPosGain(piece, mos) MaxPositionalGain[(piece) * 010000 + (mos)]
#endif
#define HistoryValue(P, M) HistoryPITo (P, P->sq[From (M)],To (M))
#ifdef MultipleHistory
SlabDeclare2(uint16, History, MaxCPUs * 0x10 * 0100);
#define HistoryPITo(P, PI, To) History[P->cpu * 0x10 * 0100 + 0100 * (PI) + (To)]
#else
SlabDeclare2(uint16, History, 0x10 * 0100);
#define HistoryPITo(P, PI, To) History[(PI) * 0100 + (To)]
#endif
SlabDeclare2(sint32, PieceSquareValue, 0x10 * 0100);
#define PST(pi, sq) PieceSquareValue[(pi) * 0100 + (sq)]
#else
#ifdef MultiplePosGain
sint16 MaxPositionalGain[MaxCPUs][0x10][010000];
#define MaxPosGain(piece, mos) MaxPositionalGain[Position->cpu][piece][mos]
#else
sint16 MaxPositionalGain[0x10][010000];
#define MaxPosGain(piece, mos) MaxPositionalGain[piece][mos]
#endif
#define HistoryValue(P, M) HistoryPITo (P, P->sq[From (M)],To (M))
#ifdef MultipleHistory
uint16 History[MaxCPUs][0x10][0100];
#define HistoryPITo(P, PI, To) History[P->cpu][PI][To]
#else
uint16 History[0x10 * 0100];
#define HistoryPITo(P, PI, To) History[PI][To]
#endif
sint32 PieceSquareValue[0x10][0100];
#define PST(pi, sq) PieceSquareValue[pi][sq]
#endif