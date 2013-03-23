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

#define UINT64 unsigned long long int
#define SEG1 0xFB
#define SEG2 0xFC
#define uBWT 5
#define RIP1 0xFD
#define RIP2 0xFE

typedef struct
    {
    int byte;
    int bit;
    unsigned char *data;
    } bitstring;
static void StackMake(unsigned int *Stack, unsigned int *l, unsigned int *va, UINT64 *b, int ns)
    {
    int c, Hop = 0, JumpDue, prJump;
    unsigned char Prefix[256], Prefix2[256];
    unsigned int SA[256];
    unsigned int j, k, p, j2, i, pr;
    for (c = 0; c < 256; c++)
        Stack[c] = (1 << 24);
    for (c = 0; c < 256; c++)
        Prefix[c] = 0;
    for (c = 0; c < ns; c++)
        {
        if (l[c] <= 8)
            for (k = b[c]; k < 256; k += 1 << l[c])
                Stack[k] = va[c] | (l[c] << 24);
        else
            Prefix[b[c] & 255] = 1;
        }
    for (p = 0; p < 256; p++)
        if (Prefix[p])
            {
            SA[Hop] = p;
            Stack[p] = Hop++;
            }
    JumpDue = Hop;
    for (j = 0; j < Hop; j++)
        {
        pr = SA[j];
        for (i = 0; i < 256; i++)
            Prefix2[i] = 0;
        prJump = JumpDue;
        for (c = 0; c < ns; c++)
            if (pr == (b[c] & 255))
                {
                if (l[c] <= 16)
                    for (k = (b[c] >> 8); k < 256; k += (1 << (l[c] - 8)))
                        Stack[((j + 1) << 8) + k] = va[c] | (l[c] << 24);
                else
                    Prefix2[(b[c] >> 8) & 255] = 1;
                }
        for (p = 0; p < 256; p++)
            if (Prefix2[p])
                {
                SA[JumpDue] = p;
                Stack[((j + 1) << 8) + p] = JumpDue++;
                }
        for (j2 = prJump; j2 < JumpDue; j2++)
            {
            p = SA[j2];
            for (c = 0; c < ns; c++)
                if (p ==((b[c] >> 8) & 255) && pr == (b[c] & 255))
                    for (k = (b[c] >> 16); k < 256; k += (1 << (l[c] - 16)))
                        Stack[((j2 + 1) << 8) + k] = va[c] | (l[c] << 24);
            }
        }
    }
static UINT64 BitsMake(int n, bitstring *BS)
    {
    UINT64 r = 0;
    int l = n + BS->bit, bc = 0;
    while (l >= 8)
        {
        r |= (BS->data[BS->byte++] >> BS->bit) << bc;
        bc += (8 - BS->bit);
        BS->bit = 0;
        l -= 8;
        }
    if (l)
        r |= ((BS->data[BS->byte] >> BS->bit) & ((1 << (l - BS->bit)) - 1)) << bc;
    BS->bit = l;
    return r;
    }
static void BytesMake(unsigned char *Data, unsigned int *Stack, int pro, unsigned char *B)
    {
    int b = 0, vb = 0, d = 0;
    unsigned int va = 0, t;
    while (b < pro)
        {
        if (vb < 8)
            {
            va |= (((unsigned int)Data[d++]) << vb);
            vb += 8;
            }
        if (vb < 16)
            {
            va |= (((unsigned int)Data[d++]) << vb);
            vb += 8;
            }
        if (vb < 24)
            {
            va |= (((unsigned int)Data[d++]) << vb);
            vb += 8;
            }
        t = Stack[va & 255];
        if (t >= (1 << 24))
            {
            B[b++] = t & 255;
            va >>= t >> 24;
            vb -= t >> 24;
            continue;
            }
        t = Stack[((t + 1) << 8) + ((va >> 8) & 255)];
        if (t >= (1 << 24))
            {
            B[b++] = t & 255;
            va >>= t >> 24;
            vb -= t >> 24;
            continue;
            }
        t = Stack[((t + 1) << 8) + ((va >> 16) & 255)];
        B[b++] = t & 255;
        va >>= t >> 24;
        vb -= t >> 24;
        }
    }
static void BWTAnnul(unsigned char *I, int pro, unsigned char *O, int ind)
    {
    int B[257], *L, i, j, w = 0, *b;
    unsigned char *S;
    L = malloc(sizeof(int) * pro);
    S = malloc(pro);
    b = B + 1;
    for (i = 0; i <= 256; i++)
        B[i] = 0;
    for (i = 0; i < pro; i++)
        b[I[i]]++;
    for (i = 0; i < 256; i++)
        for (j = 0; j < b[i]; j++)
            S[w++] = i;
    for (i = 1; i < 256; i++)
        b[i] += b[i - 1];
    for (i = 0; i < pro; i++)
        L[b[I[i] - 1]++] = i;
    for (i = 0; i < pro; i++)
        {
        O[i] = S[ind];
        ind = L[ind];
        }
    free(L);
    free(S);
    }
static unsigned int HuffmanAnnul(unsigned char *I, unsigned char *O)
    {
    long w = 0;
    int i, ns, seg1, seg2;
    bitstring BS[1];
    UINT64 pro = 0, bits[256];
    unsigned int va[256], lun[256], *Stack;
    for (w = 0; w < 4; w++)
        pro += I[w] << (w << 3);
    ns = I[w++];
    seg1 = I[w++];
    seg2 = I[w++];
    for (i = 0; i < ns; i++)
        {
        va[i] = I[w++];
        lun[i] = I[w++];
        }
    for (i = 0; i < ns; i++)
        if (lun[i] > 24)
            {
			if (VerboseRobboBases)
				{
				Send("HuffmanAnnul lun %d %d\n", i, lun[i]);

#ifdef Log
				if (WriteLog)
					{
					log_file = fopen(log_filename, "a");
					fprintf(log_file, "HuffmanAnnul lun %d %d\n", i, lun[i]);
					close_log();
					}
#endif
				}
            exit(1);
            }
    BS->data = (I + w);
    BS->bit = 0;
    BS->byte = 0;
    for (i = 0; i < ns; i++)
        bits[i] = BitsMake(lun[i], BS);
    Stack = malloc(65536 * sizeof(int));
    if (BS->bit)
        BS->byte++;
    w += BS->byte;
    StackMake(Stack, lun, va, bits, ns);
    BS->data = (I + w);
    BS->bit = 0;
    BS->byte = 0;
    BytesMake(BS->data, Stack, pro, O);
    free(Stack);
    return pro;
    }

static unsigned int RLEAnnul(unsigned char *I, int pro, unsigned char *O)
    {
    int i = 0, o = 0, s = 0, rip = 1, b, j;
    while (i < pro)
        {
        b = I[i++];
        if (b == SEG1)
            rip += (1 << s++);
        else if (b == SEG2)
            rip += (2 << s++);
        else
            {
            for (j = 0; j < rip; j++)
                O[o++] = b;
            rip = 1;
            s = 0;
            }
        }
    return o;
    }
static unsigned int mtfAnnul(unsigned char *I, int pro, unsigned char *O)
    {
    int b, j, w, i = 0, o = 0, c, rip = 0, s = 0;
    unsigned char sim[256];
    c = I[i++];
    for (j = 0; j < c; j++)
        sim[j] = I[i++];
    while (i < pro)
        {
        b = I[i++];
        if (b == RIP1)
            rip += (1 << s++);
        else if (b == RIP2)
            rip += (2 << s++);
        else
            {
            w = sim[b];
            for (j = 0; j < rip; j++)
                O[o++] = sim[0];
            rip = 1;
            s = 0;
            for (; b > 0; b--)
                sim[b] = sim[b - 1];
            sim[0] = w;
            }
        }
    if (rip)
        for (j = 0; j < rip; j++)
            O[o++] = sim[0];
    return o;
    }

int BlockDecompress(unsigned char *O, unsigned char *I, int lun, int TYPE)
    {
    int i = 0, f = 0, b;
    unsigned char *B;
    B = malloc(65536);
    if (TYPE == uBWT)
        for (i = 0; i < 4; i++)
            f |= I[i] << (i << 3);
    b = HuffmanAnnul(I + i, B);
    if (TYPE == uBWT)
        {
        b = mtfAnnul(B, b, O);
        BWTAnnul(O, b, B, f);
        }
    b = RLEAnnul(B, b, O);
    free(B);
    return b;
    }
#endif
