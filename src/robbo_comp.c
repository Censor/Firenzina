/*******************************************************************************
Firenzina is a UCI chess playing engine by
Kranium (Norman Schmidt), Yuri Censor (Dmitri Gusev) and ZirconiumX (Matthew Brades).
Rededication: To the memories of Giovanna Tornabuoni and Domenico Ghirlandaio.
Special thanks to: Norman Schmidt, Jose Maria Velasco, Jim Ablett, Jon Dart, Andrey Chilantiev, Quoc Vuong.
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

#define uint8 unsigned char
#define RIP1 0xFD
#define RIP2 0xFE
#define RIP(S,s)                                   \
  { if (s > 0)                                     \
      { s--; while (1) { if (s & 1) S[w++] = RIP2; \
                    else S[w++] = RIP1;            \
                    if (s < 2) break;             \
          s = (s - 2) >> 1; }                    \
   s = 0; }  }
#define UINT64 unsigned long long int

void SuffixMake(uint8 *Data, int *IDX, int N);
static UINT64 bi;
typedef struct
    {
    unsigned char sim;
    int count;
    } tipoSC;
typedef struct
    {
    int wt;
    int c1, c2;
    } NODO;
typedef struct
    {
    int lun;
    int va;
    } bits;
typedef struct
    {
    int byte;
    int bit;
    unsigned char *O;
    } bitstring;
typedef struct
    {
    int c;
    int r;
    } cr;

static int BWTMake(unsigned char *bufI, unsigned char *bufO, int pro)
    {
    int i, u;
    int *indexes;
    unsigned char *BUFqsort = NULL;
    indexes = malloc(sizeof(int) * pro);
    SuffixMake(bufI, indexes, pro);
    for (i = 0; i < pro; i++)
        {
        u = indexes[i] + pro - 1;
        if (u >= pro)
            u -= pro;
        bufO[i] = bufI[u];
        }
    for (i = 0; i < pro; i++)
        {
        if (indexes[i] == 0)
            {
            free(indexes);
            return i;
            }
        }
	if (VerboseRobboBases)
		{
		Send("Bad bwt\n");

#ifdef Log
		if (WriteLog)
			{
			log_file = fopen(log_filename, "a");
			fprintf(log_file, "Bad bwt\n");
			close_log();
			}
#endif
		}

    exit(1);
    }
static int QsortTrie(const void *a, const void *b)
    {
    cr *x, *y;
    x = (cr *)a;
    y = (cr *)b;
    if (x->c < y->c)
        return -1;
    if (x->c > y->c)
        return 1;
    return 0;
    }
static int Estr(char *s, int l)
    {
    int x = 0;
    int i;
    for (i = 0; i < l; i++)
        {
        x <<= 1;
        if (s[i] == '1')
            x++;
        }
    return x;
    }
static void Trie(cr *T, NODO *U, int ue, char *s, int alt, bits *E)
    {
    UINT64 w;
    s[alt] = '0';
    s[alt + 1] = 0;
    if (U[ue].c1 > 0)
        Trie(T, U, U[ue].c1, s, alt + 1, E);
    else
        {
        w = T[-U[ue].c1].c;
        bi += (alt + 1) * w;
        E[T[-U[ue].c1].r].lun = alt + 1;
        E[T[-U[ue].c1].r].va = Estr(s, alt + 1);
        }
    s[alt] = '1';
    s[alt + 1] = 0;
    if (U[ue].c2 > 0)
        Trie(T, U, U[ue].c2, s, alt + 1, E);
    else
        {
        w = T[-U[ue].c2].c;
        bi += (alt + 1) * w;
        E[T[-U[ue].c2].r].lun = alt + 1;
        E[T[-U[ue].c2].r].va = Estr(s, alt + 1);
        }
    }
static void Huffman(cr *T, int c, bits *E)
    {
    int w, c1, c2, i, u = 1, t = 0, ue = 1, su = 0;
    char s[1024];
    NODO *U;
    qsort(T, c, sizeof(cr), QsortTrie);
    U = malloc((c + 1) * sizeof(NODO));
    while (c - t + ue - u > 1)
        {
        if (ue == u || (t < c && T[t].c <= U[u].wt))
            {
            c1 = -t;
            t++;
            }
        else
            {
            c1 = u;
            u++;
            }
        if (ue == u || (t < c && T[t].c <= U[u].wt))
            {
            c2 = -t;
            t++;
            }
        else
            {
            c2 = u;
            u++;
            }
        U[ue].c1 = c1;
        U[ue].c2 = c2;
        if (c1 <= 0)
            w = T[-c1].c;
        else
            w = U[c1].wt;
        if (c2 <= 0)
            w += T[-c2].c;
        else
            w += U[c2].wt;
        U[ue].wt = w;
        ue++;
        }
    bi = 0;
    Trie(T, U, ue - 1, s, 0, E);
    for (i = 0; i < c; i++)
        su += T[i].c;
    free(U);
    }
static void BitsMake(bits E, bitstring *BS)
    {
    int l = E.lun + BS->bit;
    int v = E.va, c = BS->bit;
    while (l >= 8)
        {
        BS->O[BS->byte++] |= (v << c) & 255;
        v >>= (8 - c);
        c = 0;
        l -= 8;
        }
    BS->O[BS->byte] |= (v << c);
    BS->bit = l;
    }
static int Inverse(int v, int l)
    {
    int u = 0, i;
    for (i = 0; i < l; i++)
        if (v &(1 << i))
            u |= (1 << (l - 1 - i));
    return u;
    }

static int QsortSC(const void *x, const void *y)
    {
    tipoSC *a, *b;
    a = (tipoSC *)x;
    b = (tipoSC *)y;
    if (a->count > b->count)
        return -1;
    if (a->count < b->count)
        return 1;
    return 0;
    }
static int MTFMake(unsigned char *I, unsigned char *O, int n)
    {
    unsigned char sim[256];
    int i, j, c = 0, w = 0, r = 0, J, B[256];
    tipoSC E[256];
    for (i = 0; i < 256; i++)
        B[i] = 0;
    for (i = 0; i < n; i++)
        B[I[i]]++;
    for (i = 0; i < 256; i++)
        if (B[i])
            sim[c++] = i;
    for (j = 0; j < c; j++)
        {
        E[j].count = B[sim[j]];
        E[j].sim = sim[j];
        }
    qsort(E, c, sizeof(tipoSC), QsortSC);
    O[w++] = c;
    for (j = 0; j < c; j++)
        sim[j] = E[j].sim;
    for (j = 0; j < c; j++)
        O[w++] = sim[j];
    for (i = 0; i < n; i++)
        {
        if (I[i] == sim[0])
            {
            r++;
            continue;
            }
        else
            {
            RIP(O, r);
            for (j = 1; j < c; j++)
                if (I[i] == sim[j])
                    break;
            if (j == c)
				{
				if (VerboseRobboBases)
					{
					Send("Symbol bad %d %d %d\n", I[i], i, sim[0]);

#ifdef Log
					if (WriteLog)
						{
						log_file = fopen(log_filename, "a");
						fprintf(log_file, "Symbol bad %d %d %d\n", I[i], i, sim[0]);
						close_log();
						}
#endif

					}
				}

            J = sim[j];
            O[w++] = j;
            r = 0;
            for (; j > 0; j--)
                sim[j] = sim[j - 1];
            sim[0] = J;
            }
        }
    if (r > 0)
        RIP(O, r);
    return w;
    }
#define SEG1 0xFB
#define SEG2 0xFC
#define RLE(X,t,r)                                 \
  { if (r > 0)                                     \
      { r--; while (1) { if (r & 1) X[w++] = SEG2; \
                    else X[w++] = SEG1;            \
                         if (r < 2) break;         \
          r = (r - 2) >> 1; }                      \
   r = 0; }                                        \
    X[w++]=t; }
static int RLEMake(unsigned char *A, int count)
    {
    int i, rip = 0, w = 0;
    if (A[0] == SEG1 || A[0] == SEG2 || A[0] == RIP1 || A[0] == RIP2)
        {
		if (VerboseRobboBases)
			{
			Send("Bad RLEMake 0/%d 0x%x\n", count, A[0]);

#ifdef Log
			if (WriteLog)
				{
				log_file = fopen(log_filename, "a");
				fprintf(log_file, "Bad RLEMake 0/%d 0x%x\n", count, A[0]);
				close_log();
				}
#endif

			for (i = 0; i < 256; i++)
				Send("0x%x ", A[i]);
			Send("\n");

#ifdef Log
			if (WriteLog)
				{
				log_file = fopen(log_filename, "a");
				for (i = 0; i < 256; i++)
					fprintf(log_file, "0x%x ", A[i]);
				fprintf(log_file, "\n");
				close_log();
				}
#endif
			}
        exit(1);
        }
    for (i = 1; i < count; i++)
        {
        if (A[i] == SEG1 || A[i] == SEG2 || A[i] == RIP1 || A[i] == RIP2)
            {
			if (VerboseRobboBases)
				{
				Send("Bad RLEMake %d/%d 0x%x\n", i, count, A[i]);
				for (i = 0; i < 256; i++)
					Send("0x%x ", A[i]);
				Send("\n");

#ifdef Log
				if (WriteLog)
					{
					log_file = fopen(log_filename, "a");
					for (i = 0; i < 256; i++)
						fprintf(log_file, "0x%x ", A[i]);
					fprintf(log_file, "\n");
					close_log();
					}
#endif

				}
            exit(1);
            }
        if (A[i] == A[i - 1])
            {
            rip++;
            continue;
            }
        RLE(A, A[i - 1], rip);
        }
    RLE(A, A[i - 1], rip);
    return w;
    }
#define MHL 24
int huffman_fare(unsigned char *Data, int nb, unsigned char *O)
    {
    int o = 0, f, i, y = 0, LK[256], KL[256], B[256];
    cr Y[256];
    bits E[256];
    bitstring BS[1];
    int min = 4096;
    if (nb > 4096)
        min = nb;
    for (i = 0; i < min; i++)
        O[i] = 0;
    O[o++] = nb & 255;
    O[o++] = (nb >> 8) & 255;
    O[o++] = (nb >> 16) & 255;
    O[o++] = (nb >> 24) & 255;
    for (i = 0; i < 256; i++)
        B[i] = 0;
    for (i = 0; i < nb; i++)
        B[Data[i]]++;
    BS->bit = 0;
    BS->byte = 0;
    y = 0;
    for (i = 0; i < 256; i++)
        Y[i].c = 0;
    for (i = 0; i < 256; i++)
        if (B[i])
            {
            LK[i] = y;
            KL[y] = i;
            Y[y].r = y;
            Y[y++].c = B[i];
            }
    while (1)
        {
        int m = 0;
        Huffman(Y, y, E);
        for (i = 0; i < y; i++)
            if (E[i].lun > m)
                m = E[i].lun;
        if (m <= MHL)
            break;
        for (i = 0; i < y; i++)
            Y[i].c = (1 + (Y[i].c >> 9)) << 8;
        }
    for (i = 0; i < y; i++)
        E[i].va = Inverse(E[i].va, E[i].lun);
    O[o++] = y;
    O[o++] = SEG1;
    O[o++] = SEG2;
    for (i = 0; i < y; i++)
        {
        O[o++] = KL[i];
        O[o++] = E[i].lun;
        }
    BS->byte = 0;
    BS->bit = 0;
    BS->O = O + o;
    for (i = 0; i < y; i++)
        BitsMake(E[i], BS);
    if (BS->bit)
        BS->byte++;
    f = BS->byte;
    BS->byte = 0;
    BS->bit = 0;
    BS->O = O + o + f;
    for (i = 0; i < nb; i++)
        BitsMake(E[LK[Data[i]]], BS);
    if (BS->bit)
        BS->byte++;
    return 4 + BS->byte + 3 + (y << 1) + f;
    }
#define uBWT 5
int Compression(unsigned char *A, int Touch, int TYPE)
    {
    int i, b, ex, v;
    unsigned int f = 0;
    unsigned char *B;
    if (Touch < 256)
        {
        Send("Compression < 256: %d\n", Touch);

#ifdef Log
		if (WriteLog)
			{
			log_file = fopen(log_filename, "a");
			fprintf(log_file, "Compression < 256: %d\n", Touch);
			close_log();
			}
#endif

        exit(1);
        }
    B = malloc(Touch + 4096);
    if (TYPE == uBWT)
        ex = 4;
    else
        ex = 0;
    b = RLEMake(A, Touch);
    if (TYPE == uBWT)
        {
        f = BWTMake(A, B, b);
        b = MTFMake(B, A, b);
        }
    v = huffman_fare(A, b, B);
    for (i = 0; i < v; i++)
        A[i + ex] = B[i];
    if (TYPE == uBWT)
        {
        A[0] = f & 255;
        A[1] = (f >> 8) & 255;
        A[2] = (f >> 16) & 255;
        A[3] = (f >> 24) & 255;
        }
    free(B);
    return v + ex;
    }
#endif
