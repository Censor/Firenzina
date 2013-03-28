/*******************************************************************************
Firenzina is a UCI chess playing engine by
Yuri Censor (Dmitri Gusev) and ZirconiumX (Matthew Brades).
Rededication: To the memories of Giovanna Tornabuoni and Domenico Ghirlandaio.
Special thanks to: Norman Schmidt, Jose Maria Velasco, Jim Ablett, Jon Dart.
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
#define MaxAge 256

static uint64 PVHashSize = 0x20000;
static uint64 HashSize;
static bool PVFlagHashInit = false;
static bool LargePVHash = false;
static bool LargeEvalHash = false;
static bool LargePawnsHash = false;
static bool PawnHashWrapper = false;
static bool FlagHashInit = false;
static bool Use = false;
static int LargeEvalNumber = -1;
static int LargePV = -1;
static int LargePawns = -1;
static int Large = -1;

#ifdef RobboBases
static bool LargeTripleHash = false;
static int LargeTripleNumber = -1;
#endif

int PVHashClear()
    {
    memset(PVHashTable, 0, PVHashSize * sizeof(typePVHash));
    return false;
    }
int InitPVHash(int mb)
    {
    if (mb > 1024)
        mb = 1024;
    if (mb < 1)
        mb = 1;
    PVHashSize = ((1ULL << BSR(mb)) << 20) / sizeof(typePVHash);
    mb = (PVHashSize * sizeof(typePVHash)) >> 20;
    PVHashMask = PVHashSize - 4;
    if (PVFlagHashInit)
        FreeMem(PVHashTable, &LargePV, &LargePVHash);
    PVFlagHashInit = true;
    CreateMem(&PVHashTable, 64, PVHashSize * sizeof(typePVHash), &LargePV, &LargePVHash, "PVHash");
    PVHashClear();
    return mb;
    }
void QuitPV()
    {
    FreeMem(PVHashTable, &LargePV, &LargePVHash);
    }
int PawnHashReset()
    {
    memset(PawnHash, 0, CurrentPHashSize * sizeof(typePawnEval));
    return false;
    }
int InitPawnHash(int mb)
    {
    if (mb > 1024)
        mb = 1024;
    if (mb < 1)
        mb = 1;
    CurrentPHashSize = ((1ULL << BSR(mb)) << 20) / sizeof(typePawnEval);
    mb = (CurrentPHashSize * sizeof(typePawnEval)) >> 20;
    if (PawnHash)
        FreeMem(PawnHash, &LargePawns, &LargePawnsHash);
    CreateMem(&PawnHash, 64, CurrentPHashSize * sizeof(typePawnEval), &LargePawns, &LargePawnsHash, "PawnHash");
    PawnHashReset();
    return mb;
    }
int InitPawnHashWrapper(int mb)
    {
    PawnHashWrapper = true;
    return InitPawnHash(mb);
    }
void QuitLargePawns()
    {
    FreeMem(PawnHash, &LargePawns, &LargePawnsHash);
    }

#ifdef RobboBases
void TripleHashClear()
    {
    int c;
    for (c = 0; c < TripleHashSize; c++)
        TripleHash[c] = 0;
    }
int InitTripleHash(int mb)
    {
    uint64 size;
    if (mb > 4096)
        mb = 4096;
    if (mb < 1)
        mb = 1;
    TripleHashSize = ((1ULL << BSR(mb)) << 20) / sizeof(uint64);
    TripleHashMask = TripleHashSize - 1;
    mb = (TripleHashSize * sizeof(uint64)) >> 20;
    if (TripleHash)
        FreeMem(TripleHash, &LargeTripleNumber, &LargeTripleHash);
    size = TripleHashSize * sizeof(uint64);
	size = MAX((1 << 21), size);
    CreateMem(&TripleHash, 64, size, &LargeTripleNumber, &LargeTripleHash, "TripleHash");
    TripleHashClear();
    return mb;
    }
void QuitTripleHash()
    {
    FreeMem(TripleHash, &LargeTripleNumber, &LargeTripleHash);
    }
#endif

void EvalHashClear()
    {
    int c;
    for (c = 0; c < EvalHashSize; c++)
        EvalHash[c] = 0;
    }
int InitEvalHash(int kb)
    {
    uint64 size;
    if (kb > 1048576)
        kb = 1048576;
    if (kb < 1)
        kb = 1;
    EvalHashSize = ((1ULL << BSR(kb)) << 10) / sizeof(uint64);
    EvalHashMask = EvalHashSize - 1;
    kb = (EvalHashSize * sizeof(uint64)) >> 10;
    if (EvalHash)
        FreeMem(EvalHash, &LargeEvalNumber, &LargeEvalHash);
    size = EvalHashSize * sizeof(uint64);
	size = MAX((1 << 21), size);

#ifdef LinuxLargePages
        size = MAX((1 << 21), size);
#endif

    CreateMem(&EvalHash, 64, size, &LargeEvalNumber, &LargeEvalHash, "EvalHash");
    EvalHashClear();
    return kb;
    }
void QuitEvalHash()
    {
    FreeMem(EvalHash, &LargeEvalNumber, &LargeEvalHash);
    }
void HashClear()
    {
    uint64 i;
    memset(HashTable, 0, HashSize * sizeof(typeHash));
    for (i = 0; i < HashSize; i++)
        (HashTable + i)->age = (MaxAge >> 1);
    GlobalAge = 0;
    }
int HashClearAll()
    {
    PVHashClear();
    HashClear();
    EvalHashClear();
	return true;
    }
int InitHash(int mb)
    {
    int PawnHash_MB;
    GlobalAge = 0;
    mb = CurrentHashSize;
    HashSize = ((1ULL << BSR(mb)) << 20) / sizeof(typeHash);
    if (HashSize > 0x100000000)
        HashSize = 0x100000000;
    mb = (HashSize * sizeof(typeHash)) >> 20;
    HashMask = HashSize - 4;
    if (FlagHashInit)
        FreeMem(HashTable, &Large, &Use);
    else
        SetupPrivileges();
    FlagHashInit = true;
    CreateMem(&HashTable, 128, HashSize * sizeof(typeHash), &Large, &Use, "Hash");
    HashClear();
    CurrentHashSize = mb;
    if (!PawnHashWrapper)
        PawnHash_MB = (((mb << 1) - 1) >> 3);
    else
        PawnHash_MB = (CurrentPHashSize * sizeof(typePawnEval)) >> 20;
    InitPawnHash(PawnHash_MB);
    InitPVHash((PVHashSize * sizeof(typePVHash)) >> 20);
    InitEvalHash((EvalHashSize * sizeof(uint64)) >> 10);

#ifdef SlabMemory
    InitSlab(16);
#else
    InitSlab(0);
#endif

    return mb;
    }
void DetachAllMemory()
    {
    FreeMem(HashTable, &Large, &Use);
    }
