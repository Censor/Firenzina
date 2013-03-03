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

MutexType PThreadCondMutex[MaxCPUs];
MutexType Wakeup_Lock_IO[1];
MutexType SMP[1];
CondType PThreadCondWait[MaxCPUs];
CondType Wakeup_IO[1];
bool volatile Die[MaxCPUs];
bool volatile SMPisActive;
bool volatile IvanAllHalt;
bool volatile IOAwake;
bool volatile InfiniteLoop;
bool volatile SearchIsDone;
bool volatile StallMode;
int volatile NumThreads;
typedef struct
    {
    int alpha;
    int beta;
    int depth;
    int node_type;
    int value;
    uint32 move;
    uint32 good_move;
    uint32 childs;
    typeNext *MovePick;
    bool tot;
    bool active;
    MutexType splock[1];
    } SplitPoint;
SplitPoint RootSP[MaxSP];
volatile uint64 SMPFree;
#define NodeTypePV 1
#define NodeTypeAll 2
#define NodeTypeCut 3
#include <setjmp.h>

