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
#ifdef RobboBases
#include "robbo_totalbase.h"

#define TotalCachePer 65536
#define GetCache0(a, b) ((123 * (a) + 321 * (b)) & (TBCacheCount - 1))
#define GetCache1(a, b) ((179 * (a) + 557 * (b)) & (TBCacheCount - 1))
#define GetCache2(a, b) ((671 * (a) + 409 * (b)) & (TBCacheCount - 1))
#define GetCache3(a, b) ((773 * (a) + 187 * (b)) & (TBCacheCount - 1))
#define GetCache(x ,a, b)                                         \
  ((x) >= 2) ? (((x) == 2) ? GetCache2 (a, b) : GetCache3 (a, b)) \
             : (((x) == 1) ? GetCache1 (a, b) : GetCache0 (a, b))
static uint32 ExcludeCount = 0;
static int fc = 0;
static volatile uint64 CurrentXOR = 0;
static uint8 *** TotalBase_Cache = NULL;
uint8 *Block;
static volatile uint64 *CacheInfo[4];
static uint64 TBCacheCount = 0;
static MutexType SweepStackLock[1];

int SetTotalBaseCache(int mb)
    {
    int i, j;
    mb = 1 << BSR(mb);
    TBCacheCount = mb << 2;
    for (i = 0; i < 4; i++)
        if (CacheInfo[i])
            free(CacheInfo[i]);
    for (i = 0; i < 4; i++)
        if (TotalBase_Cache[i])
            {
            for (j = 0; j < TotalBaseCache << 2; j++)
                if (TotalBase_Cache[i][j])
                    free(TotalBase_Cache[i][j]);
            free(TotalBase_Cache[i]);
            }
    CacheInfo[0] = malloc(TBCacheCount * sizeof(uint64));
    CacheInfo[1] = malloc(TBCacheCount * sizeof(uint64));
    CacheInfo[2] = malloc(TBCacheCount * sizeof(uint64));
    CacheInfo[3] = malloc(TBCacheCount * sizeof(uint64));
    TotalBase_Cache[0] = malloc(TBCacheCount * sizeof(uint8 *));
    TotalBase_Cache[1] = malloc(TBCacheCount * sizeof(uint8 *));
    TotalBase_Cache[2] = malloc(TBCacheCount * sizeof(uint8 *));
    TotalBase_Cache[3] = malloc(TBCacheCount * sizeof(uint8 *));
    for (j = 0; j < 4; j++)
        for (i = 0; i < TBCacheCount; i++)
            TotalBase_Cache[j][i] = NULL;
    for (j = 0; j < 4; j++)
        for (i = 0; i < TBCacheCount; i++)
            CacheInfo[j][i] = 0xffffffff;
	if (VerboseRobboBases)
		Send("info string TotalBase Cache is %dmb + (1mb)\n", mb);
    TotalBaseCache = mb;
    return mb;
    }
void InitTotalBaseCache(uint64 mb)
    {
    int i;
    LockInit(SweepStackLock);
    TotalBase_Cache = malloc(sizeof(uint8 **) << 2);
    Block = malloc(128 + (TotalCachePer << 4));
    for (i = 0; i < 4; i++)
        {
        CacheInfo[i] = NULL;
        TotalBase_Cache[i] = NULL;
        }
    SetTotalBaseCache(mb);
    }

static uint8 SweepStack(RobboTotalBase *tb, uint64 PositionCount)
    {
    uint64 n = tb->num;
    uint64 zo = PositionCount / TotalCachePer, ci = (n << 32) | zo, ZO;
    int h[4], e, i;
    uint8 *BPTR;
    uint8 u;
    uint64 SaveXOR;
    Prim:
    SaveXOR = CurrentXOR;
    for (i = 0; i < 4; i++)
        {
        h[i] = GetCache(i, n, zo);
        if (CacheInfo[i][h[i]] == ci)
            {
            u = TotalBase_Cache[i][h[i]][PositionCount % TotalCachePer];
            if (CacheInfo[i][h[i]] == ci)
                return u;
            }
        }
    Lock(SweepStackLock);
    if (SaveXOR != CurrentXOR)
        {
        UnLock(SweepStackLock);
        goto Prim;
        }
    ExcludeCount++;
    e = -1;
    for (i = 0; i < 4; i++)
        if (CacheInfo[i][h[i]] == 0xffffffff)
            e = i;
    if (e == -1)
        {
        e = 0;
        for (i = 0; i < 16; i++)
            e += (ExcludeCount >> i);
        e &= 3;
        }
    if (!TotalBase_Cache[e][h[e]])
        TotalBase_Cache[e][h[e]] = malloc(TotalCachePer);
    CacheInfo[e][h[e]] = 0xfffeefff;
    if (tb->Blockpro == 0x100000)
        {
        ZO = zo & 0xf;
        zo >>= 4;
        }
    if (tb->TYPE == DiskCompressed || tb->TYPE == DiskHuffman)
        {
        uint32 ba = tb->indexes[zo];
        sint32 lun = tb->indexes[zo + 1] - ba;
        uint64 f = 0;
        uint64 pr = ba;
        char STR[1024];
        while (zo >= tb->supp_indice[f])
            f++;
        pr += (f << 32);
        if (!tb->Fdata)
            {
            fc++;
            if (fc == MaximumFopen)
                {
                for (i = 0; i < MaxTables; i++)
                    if ((TableTotalBases + i)->Fdata)
                        {
                        fclose((TableTotalBases + i)->Fdata);
                        (TableTotalBases + i)->Fdata = NULL;
                        }
                fc = 1;
                }
            sprintf(STR, "%s%s%s", tb->DirNome, CompressPrefix + 1, tb->string);
            tb->Fdata = fopen(STR, "rb");
            }
        fseek(tb->Fdata, pr, SEEK_SET);
        fread(Block, 1, lun, tb->Fdata);
        if (tb->Blockpro == 0x100000)
            {
            BPTR = Block + ((uint32 *)(Block + (ZO << 3)))[0];
            lun = ((uint32 *)(Block + ((ZO << 3) + 4)))[0];
            }
        else
            BPTR = Block;
        BlockDecompress(TotalBase_Cache[e][h[e]], BPTR, lun, tb->TYPE);
        }
    CacheInfo[e][h[e]] = ci;
    CurrentXOR ^= ci | (((uint64)h[e]) << 44) | (((uint64)(e)) << 62);
    u = TotalBase_Cache[e][h[e]][PositionCount % TotalCachePer];
    UnLock(SweepStackLock);
    return u;
    }
int TotalValue(RobboTotalBase *tb, uint64 PositionCount)
    {
    uint8 u;
    u = SweepStack(tb, PositionCount);
    return u;
    }
void KingSlice(RobboTotalBase *tb, int rf)
    {
    uint64 r, u = rf *tb->PawnMult, v = u + tb->PawnMult;
    for (r = u; r < v; r += TotalCachePer)
        SweepStack(tb, r);
    }
#endif
