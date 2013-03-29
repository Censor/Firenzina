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
static uint8 *SlabRootLoc = NULL;
static uint8 *CurrentSlabLoc;
static sint64 CurrentSlabSize = 0;
void *FromSlab(uint64 sz)
    {
    void *A;
    uint64 pageset = ((uint64)(CurrentSlabLoc)) & 0xfff;
    if (sz >= 0x1000 && pageset & 0xfff)
        CurrentSlabLoc += (0x1000 - pageset);
    A = CurrentSlabLoc;
    if (sz & 0x3f)
        sz = sz + (0x40 - (sz & 0x3f));
    CurrentSlabLoc += sz;
    if (CurrentSlabLoc - SlabRootLoc > CurrentSlabSize)
        {
        CurrentSlabLoc -= sz;
        Send("FromSlab %d failed\n", (int)(sz));
        return NULL;
        }
    return A;
    }

static int SlabNumber = -1;
static bool LargeSlab = false;
void DeleteSlab()
    {
    FreeMem(SlabRootLoc, &SlabNumber, &LargeSlab);
    }
void InitSlab(int mb)
    {

#ifdef SlabMemory
    if (SlabRootLoc)
        DeleteSlab();
    CurrentSlabSize = mb * 0x100000;
    CreateMem(&SlabRootLoc, 64, CurrentSlabSize, &SlabNumber, &LargeSlab, "SLAB");
	CurrentSlabLoc = SlabRootLoc + (0x40 - ((uint64)(SlabRootLoc) &0x3f));
#endif

    FillSlab();
    }
static void FillSlab()
    {
    SlabAlloc1(uint64, AttN, 64);
    SlabAlloc1(uint64, AttK, 64);
    SlabAlloc1(uint64, AttPw, 64);
    SlabAlloc1(uint64, AttPb, 64);
    SlabAlloc1(typeMM, RookMM, 64);
    SlabAlloc1(typeMM, BishopMM, 64);
    SlabAlloc2(uint64, MMOrtho, 102400);
    SlabAlloc2(uint64, MMDiag, 5248);
    SlabAlloc1(uint64, SqSet, 64);
    SlabAlloc1(uint64, SqClear, 64);
    SlabAlloc1(uint64, NonDiag, 64);
    SlabAlloc1(uint64, NonOrtho, 64);
    SlabAlloc1(uint64, Ortho, 64);
    SlabAlloc1(uint64, Diag, 64);
    SlabAlloc1(uint64, OrthoDiag, 64);
    SlabAlloc1(uint64, OpenFileW, 64);
    SlabAlloc1(uint64, OpenFileB, 64);
    SlabAlloc1(uint64, PassedPawnW, 64);
    SlabAlloc1(uint64, PassedPawnB, 64);
    SlabAlloc1(uint64, ProtectedPawnW, 64);
    SlabAlloc1(uint64, ProtectedPawnB, 64);
    SlabAlloc1(uint64, IsolatedPawnW, 64);
    SlabAlloc1(uint64, IsolatedPawnB, 64);
    SlabAlloc1(uint64, ConnectedPawns, 64);
    SlabAlloc1(uint64, InFrontW, 8);
    SlabAlloc1(uint64, NotInFrontW, 8);
    SlabAlloc1(uint64, InFrontB, 8);
    SlabAlloc1(uint64, NotInFrontB, 8);
    SlabAlloc1(uint64, IsolatedFiles, 8);
    SlabAlloc1(uint64, FilesLeft, 8);
    SlabAlloc1(uint64, FilesRight, 8);
    SlabAlloc1(uint64, Doubled, 64);
    SlabAlloc1(uint64, Left2, 64);
    SlabAlloc1(uint64, Right2, 64);
    SlabAlloc1(uint64, Left1, 64);
    SlabAlloc1(uint64, Right1, 64);
    SlabAlloc1(uint64, Adjacent, 64);
    SlabAlloc1(uint64, LongDiag, 64);
    SlabAlloc1(uint64, Northwest, 64);
    SlabAlloc1(uint64, Southwest, 64);
    SlabAlloc1(uint64, Northeast, 64);
    SlabAlloc1(uint64, Souteast, 64);
    SlabAlloc1(uint64, QuadrantWKwtm, 64);
    SlabAlloc1(uint64, QuadrantBKwtm, 64);
    SlabAlloc1(uint64, QuadrantWKbtm, 64);
    SlabAlloc1(uint64, QuadrantBKbtm, 64);
    SlabAlloc1(uint64, ShepherdWK, 64);
    SlabAlloc1(uint64, ShepherdBK, 64);
    SlabAlloc3(uint64, Interpose, 0100 * 0100);
    SlabAlloc3(uint64, Evade, 0100 * 0100);
    SlabAlloc3(uint64, Zobrist, 0x10 * 0100);
    SlabAlloc3(sint8, Line, 0100 * 0100);
    SlabAlloc1(uint64, HashCastling, 16);
    SlabAlloc1(uint64, HashEP, 8);
    SlabAlloc1(uint64, HashRev, 16);

#ifdef MultiplePosGain
    SlabAlloc2(sint16, MaxPositionalGain, MaxCPUs * 0x10 * 010000);
#else
    SlabAlloc2(sint16, MaxPositionalGain, 0x10 * 010000);
#endif

#ifdef MultipleHistory
    SlabAlloc2(uint16, History, MaxCPUs * 0x10 * 0100);
#else
    SlabAlloc2(uint16, History, 0x10 * 0100);
#endif

    SlabAlloc2(sint32, PieceSquareValue, 0x10 * 0100);
    SlabAlloc2(typeMaterial, Material, 419904);
    ResetHistory();
    ResetPositionalGain();
    InitArrays();
    InitMaterialValue();
    InitStatic();
    }
