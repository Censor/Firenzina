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
#ifdef RobboBases
#include "robbo_triplebase.h"
#include "robbo_totalbase.h"

static TripleBaseCache *TripleCache[4];
static volatile uint64 *TripleCacheNotE[4];
static MutexType TripleBaseDiskAccess[1];
static uint64 TripleCacheNum = 0;
static uint64 CurrentTripleCacheSize = 0;
MutexType TripleBaseUsage[1];

static volatile uint32 TripleLoaded = 0;
#define CheckByte(x, y)                                                   \
	if (x != y) { Send ("Bad Triple watermark %s %d %d %d\n",              \
		rtb->string, rtb->num, heap, slice); return false; }
#define CheckByte0(x, y)                                                   \
	if (x != y) { Send ("Bad Triple watermark (SMP) %s %d " Type64Bit "\n", \
		rtb->string, rtb->num, meld); }
#define GetCache0(a, b, c)                                              \
	((411 * (a) + 517 * (b) + 373 * (c)) & (TripleCacheNum - 1))
#define GetCache1(a, b, c)                                              \
	((725 * (a) + 309 * (b) + 611 * (c)) & (TripleCacheNum - 1))
#define GetCache2(a, b, c)                                              \
	((887 * (a) + 521 * (b) + 225 * (c)) & (TripleCacheNum - 1))
#define GetCache3(a, b, c)                                              \
	((921 * (a) + 1011 * (b) + 703 * (c)) & (TripleCacheNum - 1))
#define GetCache(x ,a, b, c)                                            \
	((x) >= 2) ? (((x) == 2) ? GetCache2 (a, b, c) : GetCache3 (a, b, c)) \
		: (((x) == 1) ? GetCache1 (a, b, c) : GetCache0 (a, b, c))
#define MaxFopen 64
static uint32 open_file_count = 0;

typedef struct
    {
    RobboTripleBase *rtb;
    int e, he;
    uint64 offset, meld;
    } typeTNG;
static typeTNG NextGet[1];

int SetTripleBaseCache(int mb)
    {
    int i, j;
    mb = 1 << BSR(mb);
    TripleCacheNum = mb;
    for (i = 0; i < 4; i++)
        if (TripleCacheNotE[i])
            free(TripleCacheNotE[i]);
    for (i = 0; i < 4; i++)
        if (TripleCache[i])
            {
            for (j = 0; j < CurrentTripleCacheSize; j++)
                if (TripleCache[i][j].Data)
                    AlignedFree(TripleCache[i][j].Data);
            for (j = 0; j < CurrentTripleCacheSize; j++)
                if (TripleCache[i][j].Data)
                    AlignedFree(TripleCache[i][j].Index);
            free(TripleCache[i]);
            }
    TripleCacheNotE[0] = malloc(TripleCacheNum * sizeof(uint64));
    TripleCacheNotE[1] = malloc(TripleCacheNum * sizeof(uint64));
    TripleCacheNotE[2] = malloc(TripleCacheNum * sizeof(uint64));
    TripleCacheNotE[3] = malloc(TripleCacheNum * sizeof(uint64));
    TripleCache[0] = malloc(TripleCacheNum * sizeof(TripleBaseCache));
    TripleCache[1] = malloc(TripleCacheNum * sizeof(TripleBaseCache));
    TripleCache[2] = malloc(TripleCacheNum * sizeof(TripleBaseCache));
    TripleCache[3] = malloc(TripleCacheNum * sizeof(TripleBaseCache));
    for (j = 0; j < 4; j++)
        for (i = 0; i < TripleCacheNum; i++)
            {
            TripleCache[j][i].Data = NULL;
            TripleCache[j][i].Index = NULL;
            TripleCache[j][i].user = 0;
            }
    for (j = 0; j < 4; j++)
        for (i = 0; i < TripleCacheNum; i++)
            TripleCacheNotE[j][i] = 0xffffffff;
    CurrentTripleCacheSize = mb;
	if (VerboseRobboBases)
		Send("info string TripleCacheSize is %dmb\n", mb);
    return mb;
    }
void InitTripleBaseCache(uint64 mb)
    {
    int i;
    LockInit(TripleBaseUsage);
    LockInit(TripleBaseDiskAccess);
    for (i = 0; i < 4; i++)
        {
        TripleCacheNotE[i] = NULL;
        TripleCache[i] = NULL;
        }
    SetTripleBaseCache(mb);
    NextGet->rtb = NULL;
    NextGet->e = 0;
    NextGet->he = 0;
    NextGet->offset = 0;
    NextGet->meld = 0;
    }
void OpenTripleFile(RobboTripleBase *rtb)
    {
    int i;
    char STR[512];
    if (rtb->disk)
        return;
    open_file_count++;
    if (open_file_count == MaxFopen)
        {
        for (i = 0; i < TripleMax; i++)
            if (rtb->disk)
                {
                fclose(rtb->disk);
                rtb->disk = NULL;
                }
        open_file_count = 1;
        }
    sprintf(STR, "%s/%s", rtb->DirNome, rtb->path);
    rtb->disk = fopen(STR, "rb");
    }
void OfftoneTripleSMP()                                                    
    {
    NextGet->rtb = NULL;
    }
bool SubsumeTripleSMP()
    {
    RobboTripleBase *rtb;
    uint64 off_set, meld;
    int ind, bytes, e, he;
    if (!NextGet->rtb)
        return false;
    Lock(TripleBaseUsage);
    rtb = NextGet->rtb;
    off_set = NextGet->offset;
    e = NextGet->e;
    he = NextGet->he;
    meld = NextGet->meld;
    NextGet->rtb = NULL;
    if (TripleCache[e][he].user)
        {
        UnLock(TripleBaseUsage);
        return false;
        }
    TripleCacheNotE[e][he] = 0xffffbad0;
    UnLock(TripleBaseUsage);
    Lock(TripleBaseDiskAccess);
    if (!rtb->disk)
        OpenTripleFile(rtb);
    fseek(rtb->disk, off_set, SEEK_SET);
    CheckByte0(fgetc(rtb->disk), 0xf2);
    CheckByte0(fgetc(rtb->disk), 0x77);
    CheckByte0(fgetc(rtb->disk), 0x83);
    CheckByte0(fgetc(rtb->disk), 0x83);
    fread(&ind, 4, 1, rtb->disk);
    fread(&bytes, 4, 1, rtb->disk);
    fread(TripleCache[e][he].Index, 4, ind, rtb->disk);
    fread(TripleCache[e][he].Data, 1, bytes, rtb->disk);
    UnLock(TripleBaseDiskAccess);
    Lock(TripleBaseUsage);
    TripleCacheNotE[e][he] = meld;
    UnLock(TripleBaseUsage);
    return true;
    }
bool DemandDynamicTripleCache(RobboTripleBase *rtb, int heap, int slice, TripleBaseCache ** Cache, uint8 ** Data,
   uint32 ** Index, bool ProbingWeak, bool ProbingImpale)
    {
    int i, h[4], e, bytes, ind;
    uint64 meld = (((uint64)rtb->num) << 48) | (((uint64)heap) << 32) | slice;
    if (rtb->direct == true)
        {
        *Cache = &rtb->direct_cache[heap];
        *Data = ((*Cache)->Data) + (0x40000 * slice);
        *Index = ((*Cache)->Index) + (0x1220 * slice);
        return true;
        }
    Redo:
    for (i = 0; i < 4; i++)
        {
        h[i] = GetCache(i, rtb->num, heap, slice);
        if (TripleCacheNotE[i][h[i]] == meld)
            {
            Lock(TripleBaseUsage);
            if (TripleCacheNotE[i][h[i]] != meld)
                {
                UnLock(TripleBaseUsage);
                if (ProbingImpale)
                    goto Redo;
                return false;
                }
            TripleCache[i][h[i]].user++;
            *Cache = &TripleCache[i][h[i]];
            *Data = (*Cache)->Data;
            *Index = (*Cache)->Index;
            UnLock(TripleBaseUsage);
            return true;
            }
        }
    TripleLoaded++;
    e = -1;
    for (i = 0; i < 4; i++)
        if (TripleCacheNotE[i][h[i]] == 0xffffffff)
            e = i;
    if (e == -1)
        {
        e = 0;
        for (i = 0; i < 12; i += 3)
            e += (TripleLoaded >> i);
        e &= 3;
        }
    Lock(TripleBaseUsage);
	
#define TripleConditionDisk      \
	(!TripleCache[e][h[e]].user && TripleCacheNotE[e][h[e]] != 0xffffbad0)
	while (!TripleConditionDisk)
        {
        e ^= 1;
        if (TripleConditionDisk)
            break;
        e ^= 2;
        if (TripleConditionDisk)
            break;
        e ^= 1;
        if (TripleConditionDisk)
            break;
        UnLock(TripleBaseUsage);
        if (ProbingImpale)
            goto Redo;
        return false;
        }
    if (!ProbingWeak)
        {
        TripleCache[e][h[e]].user = 1;
        TripleCacheNotE[e][h[e]] = 0xffffbad0;
        }
    UnLock(TripleBaseUsage);
    if (!TripleCache[e][h[e]].Data)
        MemAlign(TripleCache[e][h[e]].Data, 0x40, 0x40000);
    if (!TripleCache[e][h[e]].Index)
        MemAlign(TripleCache[e][h[e]].Index, 0x40, 0x1220 * sizeof(uint32));

    if (ProbingWeak)
        {
        Lock(TripleBaseUsage);
        if (TripleCacheNotE[e][h[e]] == 0xffffbad0)
            {
            UnLock(TripleBaseUsage);
            return false;
            }
        rtb->weak++;
        NextGet->rtb = rtb;
        NextGet->e = e;
        NextGet->he = h[e];
        NextGet->offset = rtb->packet_offset[heap][slice];
        NextGet->meld = meld;
        UnLock(TripleBaseUsage);
        return false;
        }
    Lock(TripleBaseDiskAccess);
    if (!rtb->disk)
        OpenTripleFile(rtb);
    fseek(rtb->disk, rtb->packet_offset[heap][slice], SEEK_SET);
    CheckByte(fgetc(rtb->disk), 0xf2);
    CheckByte(fgetc(rtb->disk), 0x77);
    CheckByte(fgetc(rtb->disk), 0x83);
    CheckByte(fgetc(rtb->disk), 0x83);
    fread(&ind, 4, 1, rtb->disk);
    fread(&bytes, 4, 1, rtb->disk);
    fread(TripleCache[e][h[e]].Index, 4, ind, rtb->disk);
    fread(TripleCache[e][h[e]].Data, 1, bytes, rtb->disk);
    UnLock(TripleBaseDiskAccess);
    *Cache = &TripleCache[e][h[e]];
    *Data = (*Cache)->Data;
    *Index = (*Cache)->Index;
    Lock(TripleBaseUsage);
    TripleCacheNotE[e][h[e]] = meld;
    UnLock(TripleBaseUsage);
    return true;
    }
#endif