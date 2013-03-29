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
#define DoLocked(x) { Lock (SMP); (x); UnLock (SMP); }
#define MaxSplit 8

void NanoSleep(int x)
    {
#if defined(_WIN32) || defined(_WIN64)
    Sleep(x / 1000000);

#else
    struct timespec TS[1];
    TS->tv_sec = 0;
    TS->tv_nsec = x;
    nanosleep(TS, NULL);
#endif
    }
typedef struct
    {
    int cpu;
    } t_args;
t_args ARGS[MaxCPUs];
volatile int init_threads;
volatile int io_init;
typePos* volatile Working[MaxCPUs];
void ThreadStall(typePos *, int);
static bool SMPInit = false;
static int CurrCPUS = 0;
static bool volatile Destroy, Destroyed;

void EndSMP()
    {
    int cpu, rp, sp;
    IvanAllHalt = true;
    while (true)
        {
        for (cpu = 0; cpu < NumThreads; cpu++)
            SignalForLock(PThreadCondWait[cpu], PThreadCondMutex[cpu]);
        for (cpu = 0; cpu < NumThreads; cpu++)
            for (rp = 0; rp < RPperCPU; rp++)
                RootPosition[cpu][rp].stop = true;
        Lock(SMP);
        if (!SMPisActive)
            break;
        UnLock(SMP);
        }
    UnLock(SMP);
    for (cpu = 0; cpu < NumThreads; cpu++)
        for (rp = 0; rp < RPperCPU; rp++)
            RootPosition[cpu][rp].used = false;
    for (sp = 0; sp < MaxSP; sp++)
        RootSP[sp].active = false;
    }
IvanThread(A)
    {
    t_args *B;
    B = (t_args *)A;
    DoLocked(init_threads++);
        ThreadStall(NullParent, B->cpu);
    return(VoidStarType)NULL;
    }
IOThread(A)
    {
    while (true)
        {
        IOAwake = false;
        StallMode = true;

#ifdef RobboBases
		if (UseRobboBases)
			OfftoneTripleSMP();
#endif

        while (!SearchIsDone)
            NanoSleep(1000000);
        StallMode = false;
        io_init = true;
        SuppressInput = false;
        WaitAgain:
        WaitForLock(*Wakeup_IO, *Wakeup_Lock_IO);
        if (Destroy)
            break;
        if (!SMPisActive)
            goto WaitAgain;
        IOAwake = true;
        while (SMPisActive)
            {
            StallMode = true;
            NanoSleep(1000000);
            if (SMPisActive)
                CheckDone(RootPosition0, 0);
#ifdef RobboBases
		if (UseRobboBases)
			{
            if (LoadOnWeakProbe)
                while (SMPisActive && SubsumeTripleSMP())
                    CheckDone(RootPosition0, 0);
			}
#endif

            }
        }
    Destroyed = true;
    return(VoidStarType)NULL;
    }
static void SMPCleanup()
    {
    int cpu;
    for (cpu = 0; cpu < CurrCPUS; cpu++)
        {
#if !defined(_WIN32) && !defined(_WIN64)
        LockDestroy(&PThreadCondMutex[cpu]);
#endif
        if (cpu > 0)
            {
            Die[cpu] = true;
            SignalForLock(PThreadCondWait[cpu], PThreadCondMutex[cpu]);
            PThreadJoin(PThread[cpu]);
            Die[cpu] = false;
            }
#if !defined(_WIN32) && !defined(_WIN64)
        LockInit(&PThreadCondMutex[cpu]);
#endif
        }
    Destroy = true;
    Destroyed = false;
    SignalForLock(*Wakeup_IO, *Wakeup_Lock_IO);
    while (!Destroyed) { }
		{
		}
#if !defined(_WIN32) && !defined(_WIN64)
    LockDestroy(Wakeup_Lock_IO);
#endif
    Destroyed = false;
    }
int InitSMP()
    {
    int cpu, rp, sp;
	GetSysInfo();
    Destroy = false;
    if (CurrCPUS)
        SMPCleanup();
    Destroy = false;
    IvanAllHalt = false;
    CondInit(*Wakeup_IO, *Wakeup_Lock_IO);
    SMPFree = 0;
    for (cpu = 0; cpu < NumThreads; cpu++)
        for (rp = 0; rp < RPperCPU; rp++)
            {
            RootPosition[cpu][rp].used = false;
            RootPosition[cpu][rp].nodes = 0;

#ifdef RobboBases
		if (UseRobboBases)
            RootPosition[cpu][rp].tbhits = 0;
#endif

            }
    for (sp = 0; sp < MaxSP; sp++)
        RootSP[sp].active = false;
    for (cpu = 0; cpu < NumThreads; cpu++)
        Working[cpu] = NULL;
    for (cpu = 0; cpu < NumThreads; cpu++)
        Die[cpu] = false;
    NullParent->ChildCount = 123;
    io_init = false;
    PThreadCreate(&PThreadIO, NULL, io_thread, NULL);
    init_threads = 1;
    for (cpu = 1; cpu < NumThreads; cpu++)
        {
        ARGS[cpu].cpu = cpu;
        PThreadCreate(&PThread[cpu], NULL, SMPThread, &ARGS[cpu]);
        }
    while (init_threads < NumThreads || !io_init)
        NanoSleep(1000000);
    CurrCPUS = NumThreads;
    NanoSleep(1000000);
    return NumThreads;
    }
static void SPInit()
    {
    int sp;
    for (sp = 0; sp < MaxSP; sp++)
        {
        RootSP[sp].active = false;
        LockInit(RootSP[sp].splock);
        }
    }
void RPinit()
    {
    int cpu;
    int rp;
    if (SMPInit)
        return;
    LockInit(SMP);
    for (cpu = 0; cpu < MaxCPUs; cpu++)
        for (rp = 0; rp < RPperCPU; rp++)
            {
            memset((void *) &RootPosition[cpu][rp], 0, sizeof(typePos));
            RootPosition[cpu][rp].DynRoot = malloc(MaxPly * sizeof(typeDynamic));
            RootPosition[cpu][rp].used = false;
            RootPosition[cpu][rp].parent = NULL;
            RootPosition[cpu][rp].Dyn = RootPosition[cpu][rp].DynRoot;
            RootPosition[cpu][rp].cpu = cpu;
            RootPosition[cpu][rp].rp = rp;
            LockInit(RootPosition[cpu][rp].padlock);
            }
    for (cpu = 0; cpu < MaxCPUs; cpu++)
        CondInit(PThreadCondWait[cpu], PThreadCondMutex[cpu]);
    SPInit();
    SMPInit = true;
    }
