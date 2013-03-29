<<<<<<< HEAD:src/robbo_triplebase.h
/*
Firenzina is a UCI chess playing engine by Kranium (Norman Schmidt)
Firenzina is based on Ippolit source code: http://ippolit.wikispaces.com/
authors: Yakov Petrovich Golyadkin, Igor Igorovich Igoronov,
and Roberto Pescatore copyright: (C) 2009 Yakov Petrovich Golyadkin
date: 92th and 93rd year from Revolution
owners: PUBLICDOMAIN (workers)
dedication: To Vladimir Ilyich
=======
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
>>>>>>> Linux/Housekeeping/Bug fixes/Extend xTreme/Defs:Firenzina/robbo_triplebase.h
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

#define Clock ((double) (GetClock() - StartClock) / 1000000.0)
typedef struct
    {
    uint8 *Data;
    uint32 *Index;
    uint8 user;
    } TripleBaseCache;
typedef struct
    {
    int len, bits;
    } TypeSymbols;
typedef struct
    {
    uint8 *data;
    uint32 *ind;
    uint32 *ind_split[8];
    uint32 packet_count[8];
    uint32 *packet_offset[8];
    uint32 *huffman_table[8];
    int bs_split[8];
    TripleBaseCache direct_cache[8];
    uint8 variant, scacco;
    int prop_ind;
    uint8 w[4], wi[4];
    sint8 p[4];
    int num, index, by, bs;
    uint64 size, weak, hit;
    uint64 m[4];
    char nome[8];
    FILE *disk;
    char DirNome[256];
    char string[16], path[32];
    bool pawns, direct;
    sint8 efgh1, efgh2;
    uint16 efgh[4];
    uint8 efgh_shift[4], efgh_file[4];
    } RobboTripleBase;
RobboTripleBase *TableTripleBases;
int NumTripleBases;
uint64 BulkBytes;
uint64 BulkInd;
uint32 BulkCount, LoadCount, DirectCount;
int TrivialCount;
#define RobboScaccoKnowBit 8
#define RobboScaccoBit 4
#define RobboDynamicLoad 2
#define RobboDirectDynamicBit 1
#define vNon 0
#define eWinTriple 1
#define eDrawTriple 2
#define eLossTriple 3
void TripleUtility(RobboTripleBase *, char *);
RobboTripleBase *LookupTriple(char *);
void UnWindowsTriple(char *);
void InitTripleBase();
void ReadyTripleBase();
void VerifyTripleBase(int *);
bool DemandDynamicTripleCache(RobboTripleBase *, int, int, TripleBaseCache **, uint8 **, uint32 **, bool,
   bool);
#define TriplePrefix "TRIPLE"
