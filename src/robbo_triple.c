<<<<<<< HEAD:src/robbo_triple.c
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
>>>>>>> Linux/Housekeeping/Bug fixes/Extend xTreme/Defs:Firenzina/robbo_triple.c
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
#ifdef RobboBases
#include "robbo_triplebase.h"
#include "robbo_totalbase.h"

#define SaferFree(x) if ((x)) free ((x))
#define SaferFreeAligned(x) if ((x)) AlignedFree ((x))

uint64 BytesTriple, IndTriple, IndTripleEx;
RobboTripleBase *TableTripleBases;
int NumTripleBases = 0;
uint64 StartClock;
int TripleBaseTable[16][16][16][16];

void UnWindowsTriple(char *A)
    {
    int s = strlen(A);
    if (A[1] == 'w')
        A[0] = A[2] - 32;
    if (A[1] == 'b')
        A[0] = A[2];
    if (s > 3 && A[3] == 'w')
        A[1] = A[4] - 32;
    if (s > 3 && A[3] == 'b')
        A[1] = A[4];
    if (s > 5 && A[5] == 'w')
        A[2] = A[6] - 32;
    if (s > 5 && A[5] == 'b')
        A[2] = A[6];
    if (s > 7 && A[7] == 'w')
        A[3] = A[8] - 32;
    if (s > 7 && A[7] == 'b')
        A[3] = A[8];
    A[s >> 1] = 0;
    }
void HackWindowsTriple(char *NOME)
    {
    char Temp[16];
    int i;
    strcpy(Temp, NOME);
    NOME[0] = 'K';
    for (i = 0; i < 4; i++)
        {
        if (Temp[i] == 0)
            NOME[(i << 1) + 1] = NOME[(i << 1) + 2] = 0;
        else if (Temp[i] >= 'A' && Temp[i] <= 'Z')
            {
            NOME[(i << 1) + 1] = 'w';
            NOME[(i << 1) + 2] = Temp[i] - 'A' + 'a';
            }
        else
            {
            NOME[(i << 1) + 1] = 'b';
            NOME[(i << 1) + 2] = Temp[i];
            }
        }
    NOME[9] = 0;
    }
void TripleEnumerateReset()
    {
    int i;
    for (i = 0; i < NumTripleBases; i++)
        (TableTripleBases + i)->weak = (TableTripleBases + i)->hit = 0;
    }
RobboTripleBase *LookupTriple(char *I)
    {
    int v, p[4] =
        {
        0, 0, 0, 0
        }, i;
    int EFGH = -1, IJ1 = -1, IJ2 = -1;
    int file, file1, file2;
    RobboTripleBase *rtb;
    for (i = 0; i < 4 && i < strlen(I); i++)
        {
        switch (I[i])
            {
            case 'Q':
                p[i] = wEnumQ;
                break;
            case 'q':
                p[i] = bEnumQ;
                break;
            case 'R':
                p[i] = wEnumR;
                break;
            case 'r':
                p[i] = bEnumR;
                break;
            case 'B':
                p[i] = wEnumB;
                break;
            case 'b':
                p[i] = bEnumB;
                break;
            case 'N':
                p[i] = wEnumN;
                break;
            case 'n':
                p[i] = bEnumN;
                break;
            case 'E':
            case 'F':
            case 'G':
            case 'H':
                EFGH = i;
            case 'e':
            case 'f':
            case 'g':
            case 'h':
                EFGH = i;
            case 'P':
                p[i] = wEnumP;
                break;
            case 'p':
                p[i] = bEnumP;
                break;
            case 'U':
            case 'V':
            case 'W':
            case 'X':
                EFGH = i;
            case 'Z':
                p[i] = BlockedPawn;
                break;
            case 'I':
            case 'J':
                if (IJ1 == -1)
                    IJ1 = i;
                else
                    IJ2 = i;
                p[i] = wEnumP;
                break;
            case 'i':
            case 'j':
                if (IJ1 == -1)
                    IJ1 = i;
                else
                    IJ2 = i;
                p[i] = bEnumP;
                break;
            default:
                return NULL;
            }
        switch (I[i])
            {
            case 'E':
            case 'e':
            case 'U':
                file = 3;
                break;
            case 'F':
            case 'f':
            case 'V':
                file = 2;
                break;
            case 'G':
            case 'g':
            case 'W':
                file = 1;
                break;
            case 'H':
            case 'h':
            case 'X':
                file = 0;
                break;
            }
        switch (I[i])
            {
            case 'I':
                file1 = 0;
                break;
            case 'J':
                file1 = 1;
                break;
            case 'i':
                file2 = 0;
                break;
            case 'j':
                file2 = 1;
                break;
            }
        }
    v = TripleBaseTable[p[0]][p[1]][p[2]][p[3]];
    if (v == -1)
        return NULL;
    v &= 0xffff;
    rtb = TableTripleBases + v;
    if (EFGH != -1)
        v = rtb->efgh[file];
    if (IJ1 != -1)
        v = rtb->efgh[file1 + (file2 << 1)];
    return (TableTripleBases + v);
    }
void CleanupTriple()
    {
    int i, j, k, l;
    RobboTripleBase *rtb;
    for (i = 0; i < NumTripleBases; i++)
        {
        rtb = (TableTripleBases + i);
        SaferFree(rtb->ind);
        SaferFree(rtb->data);
        for (j = 0; j < 8; j++)
            {
            SaferFreeAligned(rtb->direct_cache[j].Data);
            SaferFreeAligned(rtb->direct_cache[j].Index);
            SaferFreeAligned(rtb->huffman_table[j]);
            SaferFree(rtb->ind_split[j]);
            SaferFree(rtb->packet_offset[j]);
            }
        }
    for (i = 0; i < 16; i++)
        for (j = 0; j < 16; j++)
            for (k = 0; k < 16; k++)
                for (l = 0; l < 16; l++)
                    TripleBaseTable[i][j][k][l] = -1;
    NumTripleBases = 0;
    BytesTriple = IndTriple = IndTripleEx = 0;
    BulkBytes = BulkInd = 0;
    BulkCount = LoadCount = DirectCount = 0;
    TripleMaxUsage = 0;
    TrivialCount = 0;
    }
void InitTripleBase()
    {
    int i, j, k, l;
    for (i = 0; i < 16; i++)
        for (j = 0; j < 16; j++)
            for (k = 0; k < 16; k++)
                for (l = 0; l < 16; l++)
                    TripleBaseTable[i][j][k][l] = -1;
    TableTripleBases = malloc(TripleMax * sizeof(RobboTripleBase));
    InitTripleBaseCache(1);
    }
void RobboTriple_delete(char *NOME)
    {
    RobboTripleBase *triple;
    int heaps, h;
    triple = LookupTriple(NOME);
    if (triple == NULL)
        return;
    heaps = (triple->size >> 30) + 1;
    for (h = 0; h < heaps; h++)
        free(triple->ind_split[h]);
    for (h = 0; h < heaps; h++)
        free(triple->packet_offset[h]);
    }
void TripleUtility(RobboTripleBase *rtb, char *iNOME)
    {
    int i, p[4];
    char NOME[256];
    int file;
    int u, v, w;
    rtb->disk = NULL;
    rtb->direct = false;
    for (i = 0; i < 8; i++)
        rtb->direct_cache[i].Data = NULL;
    for (i = 0; i < 8; i++)
        rtb->direct_cache[i].Index = NULL;
    for (i = 0; i < 8; i++)
        rtb->huffman_table[i] = NULL;
    strcpy(NOME, iNOME);
    if (NOME[0] == 'K' || NOME[0] == 'k')
        UnWindows(NOME);
    strcpy(rtb->nome, NOME);
    for (i = 0; NOME[i]; i++);
    for (; i < 4; i++)
        NOME[i] = 0;
    rtb->pawns = false;
    rtb->efgh1 = -1;
    rtb->efgh2 = -1;
    for (i = 0; i < 4; i++)
        {
        rtb->efgh_shift[i] = 0;
        rtb->efgh_file[i] = -1;
        rtb->efgh[i] = 0xffff;
        switch (NOME[i])
            {
            case 'Q':
                p[i] = wEnumQ;
                break;
            case 'q':
                p[i] = bEnumQ;
                break;
            case 'R':
                p[i] = wEnumR;
                break;
            case 'r':
                p[i] = bEnumR;
                break;
            case 'B':
                p[i] = wEnumB;
                break;
            case 'b':
                p[i] = bEnumB;
                break;
            case 'N':
                p[i] = wEnumN;
                break;
            case 'n':
                p[i] = bEnumN;
                break;
            case 'I':
            case 'J':
                rtb->efgh1 = i;
                p[i] = wEnumP;
                break;
            case 'E':
            case 'F':
            case 'G':
            case 'H':
                rtb->efgh1 = i;
            case 'P':
                p[i] = wEnumP;
                break;
            case 'i':
            case 'j':
                rtb->efgh2 = i;
                p[i] = bEnumP;
                break;
            case 'e':
            case 'f':
            case 'g':
            case 'h':
                rtb->efgh1 = i;
            case 'p':
                p[i] = bEnumP;
                break;
            case 'U':
            case 'V':
            case 'W':
            case 'X':
                rtb->efgh1 = i;
            case 'Z':
                p[i] = BlockedPawn;
                break;
            default:
                p[i] = 0;
            }
        switch (NOME[i])
            {
            case 'E':
            case 'e':
            case 'U':
                file = 3;
                break;
            case 'F':
            case 'f':
            case 'V':
                file = 2;
                break;
            case 'G':
            case 'g':
            case 'W':
            case 'J':
            case 'j':
                file = 1;
                break;
            case 'H':
            case 'h':
            case 'X':
            case 'I':
            case 'i':
                file = 0;
            }
        if (p[i])
            {
            if (p[i] == wEnumP || p[i] == bEnumP)
                rtb->m[i] = 48;
            else if (p[i] == BlockedPawn)
                rtb->m[i] = 40;
            else
                rtb->m[i] = 64;
            if (p[i] == wEnumP || p[i] == bEnumP || p[i] == BlockedPawn)
                rtb->pawns = true;
            if (rtb->efgh2 == i)
                {
                rtb->m[i] >>= 1;
                rtb->efgh_shift[i] = 1;
                rtb->efgh_file[i] = file;
                if (rtb->efgh1 != -1)
                    {
                    rtb->m[rtb->efgh1] <<= 1;
                    rtb->efgh_shift[rtb->efgh1] = 1;
                    file = (file << 1) + rtb->efgh_file[rtb->efgh1];
                    }
                }
            else if (rtb->efgh1 == i)
                {
                if (rtb->efgh2 != -1)
                    {
                    rtb->m[i] >>= 1;
                    rtb->efgh_shift[i] = 1;
                    rtb->efgh_file[i] = file;
                    file = (rtb->efgh_file[rtb->efgh2] << 1) + file;
                    }
                else
                    {
                    rtb->m[i] >>= 2;
                    rtb->efgh_shift[i] = 2;
                    rtb->efgh_file[i] = file;
                    }
                }
            }
        else
            rtb->m[i] = 1;
        rtb->p[i] = p[i];
        }
    if (p[0] && p[0] == p[3])
        {
        rtb->m[0] = (rtb->m[0] * (rtb->m[0] - 1) * (rtb->m[0] - 2) * (rtb->m[0] - 3)) / 24;
        rtb->m[1] = rtb->m[2] = rtb->m[3] = 1;
        }
    else if (p[0] && p[0] == p[2])
        {
        rtb->m[0] = (rtb->m[0] * (rtb->m[0] - 1) * (rtb->m[0] - 2)) / 6;
        rtb->m[1] = rtb->m[2] = 1;
        }
    else if (p[1] && p[1] == p[3])
        {
        rtb->m[1] = (rtb->m[1] * (rtb->m[1] - 1) * (rtb->m[1] - 2)) / 6;
        rtb->m[2] = rtb->m[3] = 1;
        }
    else
        {
        if (p[0] && p[0] == p[1])
            {
            rtb->m[0] = (rtb->m[0] * (rtb->m[0] - 1)) >> 1;
            rtb->m[1] = 1;
            }
        if (p[1] && p[1] == p[2])
            {
            rtb->m[1] = (rtb->m[1] * (rtb->m[1] - 1)) >> 1;
            rtb->m[2] = 1;
            }
        if (p[2] && p[2] == p[3])
            {
            rtb->m[2] = (rtb->m[2] * (rtb->m[2] - 1)) >> 1;
            rtb->m[3] = 1;
            }
        }
    rtb->num = NumTripleBases;
    if (TripleBaseTable[p[0]][p[1]][p[2]][p[3]] != -1)
        goto EfghijFix;
    TripleBaseTable[p[0]][p[1]][p[2]][p[3]] = NumTripleBases + (0 << 16) + (1 << 18) + (2 << 20) + (3 << 22);
    TripleBaseTable[p[1]][p[0]][p[2]][p[3]] = NumTripleBases + (1 << 16) + (0 << 18) + (2 << 20) + (3 << 22);
    TripleBaseTable[p[0]][p[2]][p[1]][p[3]] = NumTripleBases + (0 << 16) + (2 << 18) + (1 << 20) + (3 << 22);
    TripleBaseTable[p[1]][p[2]][p[0]][p[3]] = NumTripleBases + (2 << 16) + (0 << 18) + (1 << 20) + (3 << 22);
    TripleBaseTable[p[2]][p[0]][p[1]][p[3]] = NumTripleBases + (1 << 16) + (2 << 18) + (0 << 20) + (3 << 22);
    TripleBaseTable[p[2]][p[1]][p[0]][p[3]] = NumTripleBases + (2 << 16) + (1 << 18) + (0 << 20) + (3 << 22);
    TripleBaseTable[p[0]][p[1]][p[3]][p[2]] = NumTripleBases + (0 << 16) + (1 << 18) + (3 << 20) + (2 << 22);
    TripleBaseTable[p[0]][p[2]][p[3]][p[1]] = NumTripleBases + (0 << 16) + (3 << 18) + (1 << 20) + (2 << 22);
    TripleBaseTable[p[1]][p[0]][p[3]][p[2]] = NumTripleBases + (1 << 16) + (0 << 18) + (3 << 20) + (2 << 22);
    TripleBaseTable[p[1]][p[2]][p[3]][p[0]] = NumTripleBases + (3 << 16) + (0 << 18) + (1 << 20) + (2 << 22);
    TripleBaseTable[p[2]][p[0]][p[3]][p[1]] = NumTripleBases + (1 << 16) + (3 << 18) + (0 << 20) + (2 << 22);
    TripleBaseTable[p[2]][p[1]][p[3]][p[0]] = NumTripleBases + (3 << 16) + (1 << 18) + (0 << 20) + (2 << 22);
    TripleBaseTable[p[0]][p[3]][p[1]][p[2]] = NumTripleBases + (0 << 16) + (2 << 18) + (3 << 20) + (1 << 22);
    TripleBaseTable[p[0]][p[3]][p[2]][p[1]] = NumTripleBases + (0 << 16) + (3 << 18) + (2 << 20) + (1 << 22);
    TripleBaseTable[p[1]][p[3]][p[0]][p[2]] = NumTripleBases + (2 << 16) + (0 << 18) + (3 << 20) + (1 << 22);
    TripleBaseTable[p[1]][p[3]][p[2]][p[0]] = NumTripleBases + (3 << 16) + (0 << 18) + (2 << 20) + (1 << 22);
    TripleBaseTable[p[2]][p[3]][p[0]][p[1]] = NumTripleBases + (2 << 16) + (3 << 18) + (0 << 20) + (1 << 22);
    TripleBaseTable[p[2]][p[3]][p[1]][p[0]] = NumTripleBases + (3 << 16) + (2 << 18) + (0 << 20) + (1 << 22);
    TripleBaseTable[p[3]][p[0]][p[1]][p[2]] = NumTripleBases + (1 << 16) + (2 << 18) + (3 << 20) + (0 << 22);
    TripleBaseTable[p[3]][p[0]][p[2]][p[1]] = NumTripleBases + (1 << 16) + (3 << 18) + (2 << 20) + (0 << 22);
    TripleBaseTable[p[3]][p[1]][p[0]][p[2]] = NumTripleBases + (2 << 16) + (1 << 18) + (3 << 20) + (0 << 22);
    TripleBaseTable[p[3]][p[1]][p[2]][p[0]] = NumTripleBases + (3 << 16) + (1 << 18) + (2 << 20) + (0 << 22);
    TripleBaseTable[p[3]][p[2]][p[0]][p[1]] = NumTripleBases + (2 << 16) + (3 << 18) + (1 << 20) + (0 << 22);
    TripleBaseTable[p[3]][p[2]][p[1]][p[0]] = NumTripleBases + (3 << 16) + (2 << 18) + (1 << 20) + (0 << 22);
    if (rtb->efgh1 != -1)
        rtb->efgh[file] = NumTripleBases;
    NumTripleBases++;
    return;
    EfghijFix:
    v = TripleBaseTable[p[0]][p[1]][p[2]][p[3]] & 0xffff;
    for (u = 0; u < 4; u++)
        if (((w = (TableTripleBases + v)->efgh[u])) != 0xffff)
            {
            (TableTripleBases + w)->efgh[file] = NumTripleBases;
            rtb->efgh[u] = w;
            }
    rtb->efgh[file] = NumTripleBases;
    NumTripleBases++;
    }
#endif
