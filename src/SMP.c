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
#define DoLocked(x) { Lock (SMP); (x); UnLock (SMP); }
#define MaxSplit 8
#include <signal.h>
volatile uint64 SMPFree;
typePos
* volatile Working[MaxCPUs];
void StubIvan()
    {
    typePos *RP00;
    int h, cpu, rp, sp;
    int volatile x;
    RP00 = &RootPosition[0][0];
    RP00->used = true;
    RP00->stop = false;
    memcpy(RP00, RootPosition0, NumBytesToCopy);
    memcpy(RP00->DynRoot, RootPosition0->DynRoot, (sizeof(typeDynamic) << 1));
    RP00->Dyn = RP00->DynRoot + 1;
    h = RootPosition0->StackHeight;
    memcpy(RP00->Stack, RootPosition0->Stack, h * sizeof(uint64));
    RP00->StackHeight = h;
    RP00->ChildCount = 0;
    RP00->parent = NULL;
    for (cpu = 0; cpu < NumThreads; cpu++)
        RP00->children[cpu] = NULL;
    IvanAllHalt = false;
    for (cpu = 0; cpu < NumThreads; cpu++)
        for (rp = 0; rp < RPperCPU; rp++)
            {
            RootPosition[cpu][rp].used = false;
            RootPosition[cpu][rp].stop = false;
            RootPosition[cpu][rp].nodes = 0;

#ifdef RobboBases
		if (UseRobboBases)
            RootPosition[cpu][rp].tbhits = 0;
#endif

            }
    RootPosition[0][0].used = true;
    for (sp = 0; sp < MaxSP; sp++)
        RootSP[sp].active = false;
    for (cpu = 0; cpu < NumThreads; cpu++)
        Working[cpu] = NULL;
    NullParent->ChildCount = 123;
    RP00->SplitPoint = NULL;
    Working[0] = RP00;
    SMPisActive = true;
    SignalForLock(*Wakeup_IO, *Wakeup_Lock_IO);
    for (x = 0; x < 10000; x++);
    SearchIsDone = true;
    while (!IOAwake)
        {
        SignalForLock(*Wakeup_IO, *Wakeup_Lock_IO);
        NanoSleep(1000000);
        }
    SearchIsDone = false;
    }
static void SMPGoodHistory(typePos *Pos, uint32 m, SplitPoint *sp)
    {
    int sv = HistoryValue(Pos, m);
    HistoryValue(Pos, m) = sv +(((0xff00 - sv) * sp->depth) >> 8);
    }
void FailHigh(SplitPoint *sp, typePos *Position, uint32 m)
    {
    int cpu;
    Lock(sp->splock);
    if (sp->tot || Position->stop)
        {
        UnLock(sp->splock);
        return;
        }
    sp->tot = true;
    sp->move = m;
    sp->value = sp->beta;
    UnLock(sp->splock);
    if (Position->sq[To(m)] == 0 && MoveHistory(m))
        SMPGoodHistory(Position, m, sp);
    if (sp->node_type == NodeTypeAll)
        HashLowerAll(Position, m, sp->depth, sp->beta);
    else
        HashLower(Position->Dyn->Hash, m, sp->depth, sp->beta);
    Lock(SMP);
    Lock(Position->parent->padlock);
    if (!Position->stop)
        {
        for (cpu = 0; cpu < NumThreads; cpu++)
            if (Position->parent->children[cpu] && cpu != Position->cpu)
                ThreadHalt(Position->parent->children[cpu]);
        }
    UnLock(Position->parent->padlock);
    UnLock(SMP);
    }

#if defined(__GNUC__)
#define INLINE inline
#endif


static INLINE void SMPBadHistory(typePos *Position, uint32 m, SplitPoint *sp)
    {
    if ((Position->Dyn + 1)->cp == 0 && MoveHistory(m))
        {
        int sv = HistoryValue(Position, m);
        if (Position->Dyn->Value > sp->alpha - 50)
            HistoryValue(Position, m) = sv -((sv * sp->depth) >> 8);
        }
    }
static void SearchCutNode(typePos *Position)
    {
    SplitPoint *sp;
    sp = Position->SplitPoint;
    Lock(sp->splock);
    sp->childs++;
    UnLock(sp->splock);
    Position->wtm ? WhiteCutSMP(Position) : BlackCutSMP(Position);
    Lock(sp->splock);
    sp->childs--;
    if (!sp->tot && !sp->childs && !Position->stop)
        HashUpperCut(Position, sp->depth, sp->value);
    UnLock(sp->splock);
    }
static void SearchAllNode(typePos *Position)
    {
    SplitPoint *sp;
    sp = Position->SplitPoint;
    Lock(sp->splock);
    sp->childs++;
    UnLock(sp->splock);
    Position->wtm ? WhiteAllSMP(Position) : BlackAllSMP(Position);
    Lock(sp->splock);
    sp->childs--;
    if (!sp->tot && !sp->childs && !Position->stop)
        HashUpper(Position->Dyn->Hash, sp->depth, sp->value);
    UnLock(sp->splock);
    }
void search(typePos *Position)
    {
    SplitPoint *sp;
    sp = Position->SplitPoint;
    if (sp->node_type == NodeTypeAll)
        {
        SearchAllNode(Position);
        return;
        }
    if (sp->node_type == NodeTypeCut)
        {
        SearchCutNode(Position);
        return;
        }
    Lock(sp->splock);
    sp->childs++;
    UnLock(sp->splock);
    Position->wtm ? WhitePVNodeSMP(Position) : BlackPVNodeSMP(Position);
    Lock(sp->splock);
    sp->childs--;
    if (!sp->tot && !sp->childs && !Position->stop)
        {
        uint32 m = sp->good_move;
        if (m)
            {
            HashExact(Position, m, sp->depth, sp->value, FlagExact);
            if (Position->sq[To(m)] == 0 && MoveHistory(m))
                SMPGoodHistory(Position, m, sp);
            }
        else
            HashUpper(Position->Dyn->Hash, sp->depth, sp->value);
        }
    UnLock(sp->splock);
    }
static void CopyFromChild(typePos *Parent, typePos *Child)
    {
    if (Child->SplitPoint->value >= Child->SplitPoint->beta)
        Parent->Dyn->move = Child->SplitPoint->move;
    else
        Parent->Dyn->move = 0;
    }
void ThreadStall(typePos *Parent, int cpu)
    {
    typePos *W;
    while (true)
        {
        DoLocked(SMPFree |= (1 << cpu));
        while (!Working[cpu] && Parent->ChildCount)
            {
            if (Die[cpu])
                return;
#if defined(_WIN32) || defined(_WIN64)
            WaitForLock(PThreadCondWait[cpu], PThreadCondMutex[cpu]);
#else
            Lock(&PThreadCondMutex[cpu]);
            if (Working[cpu] || !Parent->ChildCount)
                {
                UnLock(&PThreadCondMutex[cpu]);
                break; // doble wakeup ?
                }
            Wait(&PThreadCondWait[cpu], &PThreadCondMutex[cpu]);
            UnLock(&PThreadCondMutex[cpu]);
#endif
            if (Die[cpu])
                return;
            }
        Lock(SMP);
        SMPFree &= ~(1 << cpu);
        W = Working[cpu];
        if (!W && Parent != NullParent && !Parent->ChildCount)
            {
            Working[cpu] = Parent;
            UnLock(SMP);
            return;
            }
        UnLock(SMP);
        if (!W)
            continue;
        search(W);
        Lock(SMP);
        Lock(W->parent->padlock);
        CopyFromChild(W->parent, W);
        W->parent->ChildCount--;
        if (W->parent->ChildCount == 0)
            {
            int icpu = W->parent->cpu;
            SignalForLock(PThreadCondWait[icpu], PThreadCondMutex[icpu]);
            }
        W->parent->children[cpu] = NULL;
        UnLock(W->parent->padlock);
        Working[cpu] = NULL;
        W->used = false;
        UnLock(SMP);
        }
    }
static void ThreadHalt(typePos *Pos)
    {
    int n;
    Lock(Pos->padlock);
    Pos->stop = true;
    for (n = 0; n < NumThreads; n++)
        {
        if (Pos->children[n] != NULL)
            ThreadHalt(Pos->children[n]);
        }
    UnLock(Pos->padlock);
    }
static typePos *GetPosition(int cpu)
    {
    int u;
    for (u = 0; u < RPperCPU; u++)
        if (!RootPosition[cpu][u].used)
            break;
    if (u == RPperCPU)
        return NULL;
    RootPosition[cpu][u].used = true;
    RootPosition[cpu][u].stop = false;
    return &RootPosition[cpu][u];
    }
static void CopyPosition(typePos *Child, typePos *Parent)
    {
    int h;
    memcpy(Child, Parent, NumBytesToCopy);
    memcpy(Child->DynRoot, Parent->Dyn - 1, (sizeof(typeDynamic) << 1));
    Child->Dyn = Child->DynRoot + 1;
    h = Parent->StackHeight;
    memcpy(Child->Stack, Parent->Stack, h * sizeof(uint64));
    Child->StackHeight = h;
    }
static typePos *CopyToChild(int icpu, typePos *Parent)
    {
    typePos *Child;
    int cpu;
    Child = GetPosition(icpu);
    if (!Child)
        return NULL;
    for (cpu = 0; cpu < NumThreads; cpu++)
        Child->children[cpu] = NULL;
    CopyPosition(Child, Parent);
    return Child;
    }
static void EndSplitpoint(SplitPoint *sp)
    {
    sp->active = false;
    }
static SplitPoint *new_splitpoint()
    {
    int sp;
    for (sp = 0; sp < MaxSP; sp++)
        if (!RootSP[sp].active)
            return &RootSP[sp];
    return NULL;
    }
bool IvanSplit(typePos *Position, typeNext *NextMove, int depth, int beta, int alpha, int NodeType, int * r)
    {
    int cpu;
    int split;
    typePos *Child;
    SplitPoint *sp;
    Lock(SMP);
    for (cpu = 0; cpu < NumThreads; cpu++)
        if (!Working[cpu])
            break;
    if (Position->stop || cpu == NumThreads)
        {
        UnLock(SMP);
        return false;
        }
    Working[Position->cpu] = NULL;
    Position->ChildCount = 0;
    sp = new_splitpoint();
    if (sp == NULL)
        {
        Working[Position->cpu] = Position;
        UnLock(SMP);
        return false;
        }
    Lock(sp->splock);
    sp->alpha = alpha;
    sp->beta = beta;
    sp->depth = depth;
    sp->node_type = NodeType;
    if (NodeType != NodeTypePV)
        sp->value = sp->beta - 1;
    else
        sp->value = sp->alpha;
    sp->move = MoveNone;
    sp->good_move = MoveNone;
    sp->childs = 0;
    sp->MovePick = NextMove;
    sp->tot = false;
    sp->active = true;
    UnLock(sp->splock);
    split = 0;
    for (cpu = 0; cpu < NumThreads && split < MaxSplit; cpu++)
        {
        Position->children[cpu] = NULL;
        if (Working[cpu] == NULL)
            {
            Child = CopyToChild(cpu, Position);
            if (!Child)
                continue;
            split++;
            Position->children[cpu] = Child;
            Child->cpu = cpu;
            Child->parent = Position;
            Child->stop = false;
            Child->SplitPoint = sp;
            Position->ChildCount++;
            }
        }
    if (split == 0)
        {
        Working[Position->cpu] = Position;
        Lock(sp->splock);
        EndSplitpoint(sp);
        UnLock(sp->splock);
        UnLock(SMP);
        return false;
        }
    for (cpu = 0; cpu < NumThreads; cpu++)
        {
        if (Position->children[cpu])
            {
            Working[cpu] = Position->children[cpu];
            SignalForLock(PThreadCondWait[cpu], PThreadCondMutex[cpu]);
            }
        }
    UnLock(SMP);
    ThreadStall(Position, Position->cpu);
    Lock(SMP);
    Lock(sp->splock);
    * r = sp->value;
    EndSplitpoint(sp);
    UnLock(sp->splock);
    UnLock(SMP);
    return true;
    }
