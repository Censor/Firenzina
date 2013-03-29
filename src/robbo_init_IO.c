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
#ifdef RobboBases
#include "robbo_totalbase.h"

#define IsUVWXZ(x) (((x) == 'U') || ((x) == 'V') || ((x) == 'W') || \
	((x) == 'X') || ((x) == 'Z'))
#define RTBx(a, b, c, d) TotalBaseTable[p[a]][p[b]][p[c]][p[d]] = -1;
#define WINDOWS_Prefix "icomp."
int Stat(char * filename);

static int canonico_windows(char *AB, bool *simm)
    {
    char Sort[32] = "KQRBNPEFGHIJZUVWX";
    char A[32];
    int j, n, n2, i, SO[32];
    strcpy(A, AB);
    A[16] = 0;
    for (i = 0; i < 16; i++)
        if (A[i] == '0')
            A[i] = 0;
    for (n = 0; n < 16; n++)
        if (!A[n])
            break;
    for (i = n; i < 16; i++)
        A[i] = 0;
    for (i = 0; i < 16; i++)
        if (A[i] >= 'a')
            A[i] -= 32;
    for (i = 0; i < 16; i++)
        if (A[i] == 0)
            for (j = i; j < 4; j++)
                A[j] = 0;
    for (i = 0; i < 16; i++)
        if (!strchr(Sort, A[i]))
            A[i] = 0;
    for (n = 1; A[n] != 'K'; n++);
    for (i = 0; i < n; i++)
        for (j = 0; j < strlen(Sort); j++)
            if (Sort[j] == A[i])
                SO[i] = j;
    for (i = n - 1; i >= 1; i--)
        for (j = i + 1; j < n && SO[j - 1] > SO[j]; j++)
            {
            Exchange(A[j - 1], A[j]);
            Exchange(SO[j - 1], SO[j]);
            }
    for (n2 = n + 1; A[n2]; n2++);
    for (i = n; i < n2; i++)
        for (j = 0; j < strlen(Sort); j++)
            if (Sort[j] == A[i])
                SO[i] = j;
    for (i = n + 1; i < n2; i++)
        for (j = 0; j < strlen(Sort); j++)
            if (Sort[j] == A[i])
                SO[i] = j;
    for (i = n2 - 1; i >= n + 1; i--)
        for (j = i + 1; j < n2 && SO[j - 1] > SO[j]; j++)
            {
            Exchange(A[j - 1], A[j]);
            Exchange(SO[j - 1], SO[j]);
            }
    if (n == 0)
        *simm = true;
    if (n == 1 && IsUVWXZ(Sort[SO[0]]))
        *simm = true;
    if (n == 2 && Sort[SO[0]] == (Sort[SO[1]] ^ 32))
        *simm = true;
    if (n == 2 && Sort[SO[0]] == 'Z' && Sort[SO[1]] == 'Z')
        *simm = true;
    if (n == 3 && IsUVWXZ(Sort[SO[1]])
       && ((Sort[SO[0]] == 'Z' && Sort[SO[2]] == 'Z') || Sort[SO[0]] == (Sort[SO[2]] ^ 32)))
        *simm = true;
    if (n == 4 && Sort[SO[1]] == 'Z' && Sort[SO[2]] == 'Z'
       && ((Sort[SO[0]] == 'Z' && Sort[SO[3]] == 'Z') || Sort[SO[0]] == (Sort[SO[3]] ^ 32)))
        *simm = true;
    if (n == 4 && (Sort[SO[0]] == (Sort[SO[2]] ^ 32)) && (Sort[SO[1]] == (Sort[SO[3]] ^ 32)))
        *simm = true;
    if (n2 == 2)
        *simm = true;
    if (n2 == 3 && IsUVWXZ(Sort[SO[1]]))
        *simm = true;
    if (n2 == 4 && Sort[SO[1]] == Sort[SO[3]])
        *simm = true;
    if (n2 == 4 && Sort[SO[1]] == 'Z' && Sort[SO[2]] == 'Z')
        *simm = true;
    if (n2 == 5 && IsUVWXZ(Sort[SO[2]]) && ((Sort[SO[1]] == 'Z' && Sort[SO[3]] == 'Z') || Sort[SO[1]] == Sort[SO[4]]))
        *simm = true;
    if (n2 == 6 && Sort[SO[2]] == 'Z' && Sort[SO[3]] == 'Z'
       && ((Sort[SO[1]] == 'Z' && Sort[SO[4]] == 'Z') || Sort[SO[1]] == Sort[SO[5]]))
        *simm = true;
    if (n2 == 6 && Sort[SO[1]] == Sort[SO[4]] && Sort[SO[2]] == Sort[SO[5]])
        *simm = true;
    return n;
    }
int NomeCanonico(char *A, bool *simm)
    {
    char Sort[32] = "QRBNPEFGHIJZUVWXqrbnpefghij";
    int j, n, i, SO[MaxNum];
    if (A[0] == 'K' || A[0] == 'k')
        return canonico_windows(A, simm);
    A[MaxNum] = 0;
    for (i = 0; i < MaxNum; i++)
        if (A[i] == '0')
            A[i] = 0;
    for (i = 0; i < MaxNum; i++)
        if (A[i] == 0)
            for (j = i; j < 4; j++)
                A[j] = 0;
    for (i = 0; i < MaxNum; i++)
        if (!strchr(Sort, A[i]))
            A[i] = 0;
    for (n = 0; A[n]; n++);
    for (i = 0; i < n; i++)
        for (j = 0; j < strlen(Sort); j++)
            if (Sort[j] == A[i])
                SO[i] = j;
    for (i = n - 1; i >= 0; i--)
        for (j = i + 1; j < n && SO[j - 1] > SO[j]; j++)
            {
            Exchange(A[j - 1], A[j]);
            Exchange(SO[j - 1], SO[j]);
            }
    if (n == 0)
        *simm = true;
    if (n == 1 && IsUVWXZ(Sort[SO[0]]))
        *simm = true;
    if (n == 2 && Sort[SO[0]] == (Sort[SO[1]] ^ 32))
        *simm = true;
    if (n == 2 && Sort[SO[0]] == 'Z' && Sort[SO[1]] == 'Z')
        *simm = true;
    if (n == 3 && IsUVWXZ(Sort[SO[1]])
       && ((Sort[SO[0]] == 'Z' && Sort[SO[2]] == 'Z') || Sort[SO[0]] == (Sort[SO[2]] ^ 32)))
        *simm = true;
    if (n == 4 && Sort[SO[1]] == 'Z' && Sort[SO[2]] == 'Z'
       && ((Sort[SO[0]] == 'Z' && Sort[SO[3]] == 'Z') || Sort[SO[0]] == (Sort[SO[3]] ^ 32)))
        *simm = true;
    if (n == 4 && (Sort[SO[0]] == (Sort[SO[2]] ^ 32)) && (Sort[SO[1]] == (Sort[SO[3]] ^ 32)))
        *simm = true;
    return n;
    }
int RobboTotalSweep(char *NOME)
    {
    RobboTotalBase *tb;
    int i, p[4] =
        {
        0, 0, 0, 0
        }, v, EFGH = -1, IJ1 = -1, IJ2 = -1, file, file1, file2;
    for (i = 0; i < 4 && NOME[i]; i++)
        {
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
                if (IJ1 == -1)
                    IJ1 = i;
                else
                    IJ2 = i;
                p[i] = wEnumP;
                break;
            case 'E':
            case 'F':
            case 'G':
            case 'H':
                EFGH = i;
            case 'P':
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
            case 'e':
            case 'f':
            case 'g':
            case 'h':
                EFGH = i;
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
                file = 1;
                break;
            case 'H':
            case 'h':
            case 'X':
                file = 0;
                break;
            }
        switch (NOME[i])
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
    v = TotalBaseTable[p[0]][p[1]][p[2]][p[3]];
    if (v == -1)
        return -1;
    v &= 0xffff;
    tb = TableTotalBases + v;
    if (EFGH != -1)
        v = tb->efgh[file];
    if (IJ1 != -1)
        v = tb->efgh[file1 + (file2 << 1)];
    return v;
    }
void RobboTotalSott(char *NOME)
    {
    int u, i, p[4] =
        {
        0, 0, 0, 0
        }, v;
    int i0, i1, i2, i3, file, file1, file2;
    RobboTotalBase *tb;
    for (i = 0; i < 4 && NOME[i]; i++)
        {
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
            case 'E':
            case 'F':
            case 'G':
            case 'H':
            case 'P':
                p[i] = wEnumP;
                break;
            case 'i':
            case 'j':
            case 'e':
            case 'f':
            case 'g':
            case 'h':
            case 'p':
                p[i] = bEnumP;
                break;
            case 'U':
            case 'V':
            case 'W':
            case 'X':
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
                file = 1;
                break;
            case 'H':
            case 'h':
            case 'X':
                file = 0;
                break;
            }
        switch (NOME[i])
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
            }
        }
    v = TotalBaseTable[p[0]][p[1]][p[2]][p[3]];
    if (v == -1)
        return;
    v &= 0xffff;
    tb = TableTotalBases + v;
    if (tb->indexes)
        free(tb->indexes);
    if (tb->efgh1 != -1)
        {
        for (u = 0; u < 4; u++)
            if (tb->efgh[u] != 0xffff && (TableTotalBases + tb->efgh[u])->Fdata)
                {
                fclose((TableTotalBases + tb->efgh[u])->Fdata);
                (TableTotalBases + tb->efgh[u])->Fdata = NULL;
                }
        }
    else if (tb->Fdata)
        {
        fclose(tb->Fdata);
        tb->Fdata = NULL;
        }

    for (i0 = 0; i0 < 4; i0++)
        for (i1 = 0; i1 < 4; i1++)
            {
            if (i0 == i1)
                continue;
            for (i2 = 0; i2 < 4; i2++)
                {
                if (i2 == i1 || i2 == i0)
                    continue;
                for (i3 = 0; i3 < 4; i3++)
                    {
                    if (i3 == i2 || i3 == i1 || i3 == i0)
                        continue;
                    RTBx(i0, i1, i2, i3);
                    }
                }
            }
    }
void UnWindows(char *NOME)
    {
    int c = 1, w = 0;
    while (NOME[c] != 'k' && NOME[c] != 'K')
        {
        NOME[w] = NOME[c++];
        if (NOME[w] > 96)
            NOME[w] -= 32;
        w++;
        }
    c++;
    while (NOME[c])
        {
        NOME[w] = NOME[c++];
        if (NOME[w] < 96)
            NOME[w] += 32;
        w++;
        }
    for (; w <= 4; w++)
        NOME[w] = 0;
    }
void hack_windows_total(char *NOME)
    {
    char Temp[16];
    int j = 0, c = 1;
    strcpy(Temp, NOME);
    Temp[0] = 'K';
    while (NOME[j] >= 'A' && NOME[j] <= 'Z')
        Temp[c++] = NOME[j++];
    Temp[c++] = 'K';
    while (NOME[j])
        Temp[c++] = NOME[j++] - 32;
    Temp[c] = 0;
    strcpy(NOME, Temp);
    }

void get_name(char *output, char *input)
    {
    strcpy(output, input);
    if (output[0] == 'K' || output[0] == 'k')
        UnWindows(output);

#ifdef NomeWindows
    hack_windows_total(output);
#endif

    }
void RobboTotalAdd(RobboTotalBase *tb, char *nome, char *DIRECTORY)
    {
    int i, p[4], v, w, u;
    char NOME2[16], NOME[16];
    bool PawnFlag = false;
    int c = 1, file;
    for (i = 0; i < 16; i++)
        NOME[i] = NOME2[i] = 0;
    strcpy(NOME, nome);
    if (NOME[0] == 'K' || NOME[0] == 'k')
        UnWindows(NOME);
    if (NOME[0] == '.')
        return;
    v = RobboTotalSweep(NOME);
    if (v != -1 && v != 0xffff)
        return;
    strcpy(NOME2, NOME);

#ifdef NomeWindows
    hack_windows_total(NOME2);
#endif

	if (VerboseRobboBases)
		{
        Send("info string Register RobboTotalBase #%d %s\n", NumTotalBases, NOME2);

#ifdef Log
		if (WriteLog)
			{
			log_file = fopen(log_filename, "a");
			fprintf(log_file, "info string Register RobboTotalBase #%d %s\n", NumTotalBases, NOME2);
			close_log();
			}
#endif

		}
    sprintf(tb->DirNome, "%s/", DIRECTORY);
    strcpy(tb->string, NOME2);
    tb->efgh1 = tb->efgh2 = -1;
    for (i = 0; i < 4; i++)
        {
        tb->efgh_shift[i] = 0;
        tb->efgh_file[i] = -1;
        tb->efgh[i] = 0xffff;
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
                tb->efgh1 = i;
                p[i] = wEnumP;
                break;
            case 'E':
            case 'F':
            case 'G':
            case 'H':
                tb->efgh1 = i;
            case 'P':
                p[i] = wEnumP;
                break;
            case 'i':
            case 'j':
                tb->efgh2 = i;
                p[i] = bEnumP;
                break;
            case 'e':
            case 'f':
            case 'g':
            case 'h':
                tb->efgh1 = i;
            case 'p':
                p[i] = bEnumP;
                break;
            case 'U':
            case 'V':
            case 'W':
            case 'X':
                tb->efgh1 = i;
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
                tb->m[i] = 48;
            else if (p[i] == BlockedPawn)
                tb->m[i] = 40;
            else
                tb->m[i] = 64;
            if (tb->efgh2 == i)
                {
                tb->m[i] >>= 1;
                tb->efgh_shift[i] = 1;
                tb->efgh_file[i] = file;
                tb->m[tb->efgh1] <<= 1;
                tb->efgh_shift[tb->efgh1] = 1;
                file = (file << 1)+ tb->efgh_file[tb->efgh1];
                }
            else if (tb->efgh1 == i)
                {
                tb->m[i] >>= 2;
                tb->efgh_shift[i] = 2;
                tb->efgh_file[i] = file;
                }
            }
        else
            tb->m[i] = 1;
        }
    if (p[0] && p[0] == p[3])
        {
        tb->m[0] = (tb->m[0] * (tb->m[0] - 1) * (tb->m[0] - 2) * (tb->m[0] - 3)) / 24;
        tb->m[1] = tb->m[2] = tb->m[3] = 1;
        }
    else if (p[0] && p[0] == p[2])
        {
        tb->m[0] = (tb->m[0] * (tb->m[0] - 1) * (tb->m[0] - 2)) / 6;
        tb->m[1] = tb->m[2] = 1;
        }
    else if (p[1] && p[1] == p[3])
        {
        tb->m[1] = (tb->m[1] * (tb->m[1] - 1) * (tb->m[1] - 2)) / 6;
        tb->m[2] = tb->m[3] = 1;
        }
    else
        {
        if (p[0] && p[0] == p[1])
            {
            tb->m[0] = (tb->m[0] * (tb->m[0] - 1)) >> 1;
            tb->m[1] = 1;
            }
        if (p[1] && p[1] == p[2])
            {
            tb->m[1] = (tb->m[1] * (tb->m[1] - 1)) >> 1;
            tb->m[2] = 1;
            }
        if (p[2] && p[2] == p[3])
            {
            tb->m[2] = (tb->m[2] * (tb->m[2] - 1)) >> 1;
            tb->m[3] = 1;
            }
        }
    for (i = 0; i < 4; i++)
        {
        tb->p[i] = p[i];
        if (p[i] == wEnumP || p[i] == bEnumP || p[i] == BlockedPawn)
            PawnFlag = true;
        }
    tb->num = NumTotalBases;
    tb->Pawn = PawnFlag;
    tb->pro = 1;
    for (i = 0; i < 4; i++)
        tb->pro *= tb->m[i];
    tb->PawnMult = tb->pro;
    if (!PawnFlag)
        tb->pro *= 462;
    else
        tb->pro *= 1806;
    if (TotalBaseTable[p[0]][p[1]][p[2]][p[3]] != -1)
        goto EfghijFix;
    TotalBaseTable[p[0]][p[1]][p[2]][p[3]] = NumTotalBases + (0 << 16) + (1 << 18) + (2 << 20) + (3 << 22);
    TotalBaseTable[p[1]][p[0]][p[2]][p[3]] = NumTotalBases + (1 << 16) + (0 << 18) + (2 << 20) + (3 << 22);
    TotalBaseTable[p[0]][p[2]][p[1]][p[3]] = NumTotalBases + (0 << 16) + (2 << 18) + (1 << 20) + (3 << 22);
    TotalBaseTable[p[1]][p[2]][p[0]][p[3]] = NumTotalBases + (2 << 16) + (0 << 18) + (1 << 20) + (3 << 22);
    TotalBaseTable[p[2]][p[0]][p[1]][p[3]] = NumTotalBases + (1 << 16) + (2 << 18) + (0 << 20) + (3 << 22);
    TotalBaseTable[p[2]][p[1]][p[0]][p[3]] = NumTotalBases + (2 << 16) + (1 << 18) + (0 << 20) + (3 << 22);
    TotalBaseTable[p[0]][p[1]][p[3]][p[2]] = NumTotalBases + (0 << 16) + (1 << 18) + (3 << 20) + (2 << 22);
    TotalBaseTable[p[0]][p[2]][p[3]][p[1]] = NumTotalBases + (0 << 16) + (3 << 18) + (1 << 20) + (2 << 22);
    TotalBaseTable[p[1]][p[0]][p[3]][p[2]] = NumTotalBases + (1 << 16) + (0 << 18) + (3 << 20) + (2 << 22);
    TotalBaseTable[p[1]][p[2]][p[3]][p[0]] = NumTotalBases + (3 << 16) + (0 << 18) + (1 << 20) + (2 << 22);
    TotalBaseTable[p[2]][p[0]][p[3]][p[1]] = NumTotalBases + (1 << 16) + (3 << 18) + (0 << 20) + (2 << 22);
    TotalBaseTable[p[2]][p[1]][p[3]][p[0]] = NumTotalBases + (3 << 16) + (1 << 18) + (0 << 20) + (2 << 22);
    TotalBaseTable[p[0]][p[3]][p[1]][p[2]] = NumTotalBases + (0 << 16) + (2 << 18) + (3 << 20) + (1 << 22);
    TotalBaseTable[p[0]][p[3]][p[2]][p[1]] = NumTotalBases + (0 << 16) + (3 << 18) + (2 << 20) + (1 << 22);
    TotalBaseTable[p[1]][p[3]][p[0]][p[2]] = NumTotalBases + (2 << 16) + (0 << 18) + (3 << 20) + (1 << 22);
    TotalBaseTable[p[1]][p[3]][p[2]][p[0]] = NumTotalBases + (3 << 16) + (0 << 18) + (2 << 20) + (1 << 22);
    TotalBaseTable[p[2]][p[3]][p[0]][p[1]] = NumTotalBases + (2 << 16) + (3 << 18) + (0 << 20) + (1 << 22);
    TotalBaseTable[p[2]][p[3]][p[1]][p[0]] = NumTotalBases + (3 << 16) + (2 << 18) + (0 << 20) + (1 << 22);
    TotalBaseTable[p[3]][p[0]][p[1]][p[2]] = NumTotalBases + (1 << 16) + (2 << 18) + (3 << 20) + (0 << 22);
    TotalBaseTable[p[3]][p[0]][p[2]][p[1]] = NumTotalBases + (1 << 16) + (3 << 18) + (2 << 20) + (0 << 22);
    TotalBaseTable[p[3]][p[1]][p[0]][p[2]] = NumTotalBases + (2 << 16) + (1 << 18) + (3 << 20) + (0 << 22);
    TotalBaseTable[p[3]][p[1]][p[2]][p[0]] = NumTotalBases + (3 << 16) + (1 << 18) + (2 << 20) + (0 << 22);
    TotalBaseTable[p[3]][p[2]][p[0]][p[1]] = NumTotalBases + (2 << 16) + (3 << 18) + (1 << 20) + (0 << 22);
    TotalBaseTable[p[3]][p[2]][p[1]][p[0]] = NumTotalBases + (3 << 16) + (2 << 18) + (1 << 20) + (0 << 22);
    if (tb->efgh1 != -1)
        tb->efgh[file] = NumTotalBases;
    NumTotalBases++;
    return;
    EfghijFix:
    v = TotalBaseTable[p[0]][p[1]][p[2]][p[3]] & 0xffff;
    for (u = 0; u < 4; u++)
        if (((w = (TableTotalBases + v)->efgh[u])) != 0xffff)
            {
            (TableTotalBases + w)->efgh[file] = NumTotalBases;
            tb->efgh[u] = w;
            }
    tb->efgh[file] = NumTotalBases;
    NumTotalBases++;
    }

bool RobboTotalRegister(char *nome, char *DIRECTORY)
    {
    int i, j, v;
    RobboTotalBase *tb;
    char NOME[64], NomeFile[1024], String[16];
    FILE *F;
    uint8 A[4];
    uint32 n = 0;
    int num_indexes = 0;
    int e = 0;
    strcpy(NOME, nome);
    if (NOME[0] == 'K' || NOME[0] == 'k')
        UnWindows(NOME);
    for (i = 0; i < 4; i++)
        if (NOME[i] == '0')
            NOME[i] = 0;
    for (i = 0; i < 4; i++)
        if (NOME[i] == 0)
            for (j = i; j < 4; j++)
                NOME[j] = 0;
    NOME[4] = 0;
    v = RobboTotalSweep(NOME);
    if (v != -1 && v != 0xffff)
        return false;
    get_name(String, NOME);
    sprintf(NomeFile, "%s/%s%s", DIRECTORY, CompressPrefix + 1, String);
    if (Stat(NomeFile) == -1)
        {
        if (String[0] == 'K')
            UnWindows(String);
        else
            hack_windows_total(String);
        sprintf(NomeFile, "%s/%s%s", DIRECTORY, CompressPrefix + 1, String);
        if (Stat(NomeFile) == -1)
            {
			if (VerboseRobboBases)
				{
				Send("Stat failed %s\n", NomeFile);

#ifdef Log
				if (WriteLog)
					{
					log_file = fopen(log_filename, "a");
					fprintf(log_file, "Stat failed %s\n", NomeFile);
					close_log();
					}
#endif

				}
            return false;
            }
        }
    sprintf(NomeFile, "%s/%s%s", DIRECTORY, CompressPrefix, String);
    F = fopen(NomeFile, "rb");
    if (!F)
        {
		if (VerboseRobboBases)
			{
			Send("FOPEN failed %s\n", NomeFile);

#ifdef Log
			if (WriteLog)
				{
				log_file = fopen(log_filename, "a");
				fprintf(log_file, "FOPEN failed %s\n", NomeFile);
				close_log();
				}
#endif

			}
        return false;
        }
    fread(A, 1, 4, F);
    if (A[0] != 0xca || A[1] != 0x7f || A[2] != 0x0b || A[3] != 0xaa)
        {
		if (VerboseRobboBases)
			{
			Send("iCOMP bytes failed %s\n", NomeFile);

#ifdef Log
			if (WriteLog)
				{
				log_file = fopen(log_filename, "a");
				fprintf(log_file, "iCOMP bytes failed %s\n", NomeFile);
				close_log();
				}
#endif

			}
        fclose(F);
        return false;
        }
    tb = TableTotalBases + NumTotalBases;
    RobboTotalAdd(tb, NOME, DIRECTORY);
    tb->TYPE = DiskCompressed;
    tb->Fdata = NULL;
    strcpy(tb->string, String);

    fread(A, 1, 4, F);
    for (i = 0; i < 4; i++)
        n |= A[i] << (i << 3);
    tb->Blockpro = n &((1 << 28) - 1);
    tb->BWT = n >> 28;
    fread(A, 1, 4, F);
    fread(A, 1, 4, F);
    for (i = 0; i < 4; i++)
        num_indexes |= A[i] << (i << 3);
    tb->indexes = malloc(num_indexes << 2);
    fread(tb->indexes, 4, num_indexes, F);
    TotIndexes += num_indexes;
    fclose(F);

    for (i = 0; i < 8; i++)
        tb->supp_indice[i] = 0xffffffff;
    for (i = 1; i <= num_indexes - 1; i++)
        if (tb->indexes[i] < tb->indexes[i - 1])
            tb->supp_indice[e++] = i;
    tb->num_indexes = num_indexes;
    return true;
    }

#if defined(_WIN32) || defined(_WIN64)
#include <sys/types.h>
#include <sys/stat.h>
int Stat(char *filename)
    {
    struct _stat buf[1];
    if (_stat(filename, buf) == -1)
        return -1;
    return (buf->st_mode & _S_IREAD) ? 0 : -1;
    }
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
int Stat(char *filename)
    {
    struct stat buf[1];
    if (stat(filename, buf) == -1)
        return -1;
    return (buf->st_mode & S_IRUSR) ? 0 : -1;
    }
#endif

static void add_nome_total(char *NOME, char *DIRECTORY)
    {
    int n = strlen(CompressPrefix);
    if (memcmp(NOME, CompressPrefix, n) && memcmp(NOME, WINDOWS_Prefix, n))
        return;
    RobboTotalRegister(NOME + n, DIRECTORY);
    }
static void TotalBaseCountList()
    {
	if (VerboseRobboBases)
		{
		Send("info string Now %d RobboTotalBases with %dmb for indexing\n", NumTotalBases, (int)((TotIndexes << 2) >> 20));

#ifdef Log
		if (WriteLog)
			{
			log_file = fopen(log_filename, "a");
			fprintf(log_file, "info string Now %d RobboTotalBases with %dmb for indexing\n", NumTotalBases, (int)((TotIndexes << 2) >> 20));
			close_log();
			}
#endif

		}
    }
static void BlockTotalIndexRegister(char *DIR, FILE *F)
    {
    char NOME[16], String[16];
    uint8 A[4];
    unsigned char B[16];
    RobboTotalBase *tb;
    int i, num_indexes, n, v, e;
    bool SOTT;
    char NomeFile[1024];
    NOME[4] = 0;
    while (fread(B, 1, 4, F))
        {
        if (B[0] != 0xee || B[1] != 0x63 || B[2] != 0x70 || B[3] != 0x41)
            {
			if (VerboseRobboBases)
				{
				Send("BlockIndex mask failure!\n");

#ifdef Log
				if (WriteLog)
					{
					log_file = fopen(log_filename, "a");
					fprintf(log_file, "BlockIndex mask failure!\n");
					close_log();
					}
#endif

				}
            return;
            }
        fread(NOME, 1, 4, F);
        v = RobboTotalSweep(NOME);
        if (v != -1 && v != 0xffff)
            {
            tb = TableTotalBases + (v & 0xffff);
			if (VerboseRobboBases)
				{
				Send("Notably loaded %s\n", tb->string);

#ifdef Log
				if (WriteLog)
					{
					log_file = fopen(log_filename, "a");
					fprintf(log_file, "Notably loaded %s\n", tb->string);
					close_log();
					}
#endif

				}
            free(tb->indexes);
            if (tb->Fdata)
                fclose(tb->Fdata);
            }
        get_name(String, NOME);
        sprintf(NomeFile, "%s/%s%s", DIR, CompressPrefix + 1, String);
        if (Stat(NomeFile) == -1)
            {
            if (String[0] == 'K')
                UnWindows(String);
            else
                hack_windows_total(String);
            sprintf(NomeFile, "%s/%s%s", DIR, CompressPrefix + 1, String);
            if (Stat(NomeFile) == -1)
                {
				if (VerboseRobboBases)
					{
					Send("Stat failed %s\n", NomeFile);

#ifdef Log
					if (WriteLog)
						{
						log_file = fopen(log_filename, "a");
						fprintf(log_file, "Stat failed %s\n", NomeFile);
						close_log();
						}
#endif

					}
                SOTT = true;
                }
            else
                SOTT = false;
            }
        else
            SOTT = false;
        if (!SOTT)
            {
            tb = TableTotalBases + NumTotalBases;
            RobboTotalAdd(tb, NOME, DIR);
            strcpy(tb->string, String);
            tb->TYPE = DiskCompressed;
            tb->Fdata = NULL;
            }
        fread(A, 1, 4, F);
        for (i = 0; i < 4; i++)
            n |= A[i] << (i << 3);
        tb->Blockpro = n &((1 << 28) - 1);
        tb->BWT = n >> 28;
        fread(A, 1, 4, F);
        num_indexes = 0;
        for (i = 0; i < 4; i++)
            num_indexes |= A[i] << (i << 3);
        tb->indexes = malloc(num_indexes << 2);
        fread(tb->indexes, 4, num_indexes, F);
        if (!SOTT)
            TotIndexes += num_indexes;
        for (i = 0; i < 8; i++)
            tb->supp_indice[i] = 0xffffffff;
        e = 0;
        for (i = 1; i <= num_indexes - 1; i++)
            if (tb->indexes[i] < tb->indexes[i - 1])
                tb->supp_indice[e++] = i;
        tb->num_indexes = num_indexes;
        }
    TotalBaseCountList();
    }
static bool HasTotalBlockIndex(char *DIR)
    {
    FILE *F;
    char Name[256];
    sprintf(Name, "%s/Block.Index", DIR);
    F = fopen(Name, "rb");
    if (!F)
        return false;
	if (VerboseRobboBases)
		{
		Send("Using Block.Index for %s\n", DIR);

#ifdef Log
		if (WriteLog)
			{
			log_file = fopen(log_filename, "a");
			fprintf(log_file, "Using Block.Index for %s\n", DIR);
			close_log();
			}
#endif

		}
    BlockTotalIndexRegister(DIR, F);
    fclose(F);
    return true;
    }
#if defined(_WIN32) || defined(_WIN64)
void GetTotalBase(char *A)
    {
    char B[1024];
    WIN32_FIND_DATA ffd;
    HANDLE hFind = INVALID_HANDLE_VALUE;
    if (HasTotalBlockIndex(A))
        return;
	if (VerboseRobboBases)
		{
		Send("info string Reading directory %s\n", A);

#ifdef Log
		if (WriteLog)
			{
			log_file = fopen(log_filename, "a");
			fprintf(log_file, "info string Reading directory %s\n", A);
			close_log();
			}
#endif

		}
    strcpy(B, A);
    strcat(B, "\\*");
    hFind = FindFirstFile(B, &ffd);
    if (hFind == INVALID_HANDLE_VALUE)
        {
		if (VerboseRobboBases)
			{
			Send("info string Directory %s not found\n", A);

#ifdef Log
			if (WriteLog)
				{
				log_file = fopen(log_filename, "a");
				fprintf(log_file, "info string Directory %s not found\n", A);
				close_log();
				}
#endif
			}

        return;
        }
    do
        {
        if (!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            add_nome_total(ffd.cFileName, A);
        } while (FindNextFile(hFind, &ffd));
    FindClose(hFind);
    TotalBaseCountList();
    }
#else
#include <fcntl.h>
#include <dirent.h>
#include <string.h>
void GetTotalBase(char *A)
    {
    DIR *dir;
    struct dirent *DE;
    if (HasTotalBlockIndex(A))
        return;
	if (VerboseRobboBases)
		{
	    Send("info string Reading directory %s\n", A);

#ifdef Log
		if (WriteLog)
			{
			log_file = fopen(log_filename, "a");
			fprintf(log_file, "info string Reading directory %s\n", A);
			close_log();
			}
#endif
		}

    dir = opendir(A);
    if (!dir)
        {
		if (VerboseRobboBases)
			{
			Send("info string Directory %s not found\n", A);

#ifdef Log
			if (WriteLog)
				{
				log_file = fopen(log_filename, "a");
				fprintf(log_file, "info string Directory %s not found\n", A);
				close_log();
				}
#endif

			}
        return;
        }
    while ((DE = readdir(dir)))
        add_nome_total(DE->d_name, A);
    closedir(dir);
    TotalBaseCountList();
    }
#endif
#endif
