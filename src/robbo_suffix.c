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

#include <stdio.h>
#include <stdlib.h>

#define true 1
#define MIN(x, y) ((x) <= (y) ? (x) : (y))
#define MAX(x, y) ((x) >= (y) ? (x) : (y))
#define uint8 unsigned char
#define AuxArrVal(x) (AUX[(IDX[x] + h >= N) ? (IDX[x] + h - N) : (IDX[x] + h)])
#define SwapIdxVal(p, q) { int _x; _x = IDX[p]; IDX[p] = IDX[q]; IDX[q] = _x; }
#define Median3(a, b, c)                       \
	(AuxArrVal(a) < AuxArrVal(b) ?               \
	(AuxArrVal(b) < AuxArrVal(c) ? (b) :        \
    AuxArrVal(c) < AuxArrVal(a) ? (a) : (c)) : \
	(AuxArrVal(b) > AuxArrVal(c) ? (b) :        \
    AuxArrVal(c) > AuxArrVal(a) ? (a) : (c)))
#define SELECT_LIMIT (9)
#define SMALL_ARRAY_SIZE (9)
#define LARGE_ARRAY_SIZE (81)

static void SamePacket(int *IDX, int *AUX, int lo, int hi)
    {
    AUX[IDX[lo]] = hi;
    if (lo == hi)
        IDX[lo] = -1;
    else
        {
        while (lo < hi)
            AUX[IDX[++lo]] = hi;
        }
    }
static void SelectionSorter(int *IDX, int *AUX, int N, int h, int lo, int hi)
    {
    int s, small, val, pack_sz;
    while (lo < hi)
        {
        pack_sz = 1;
        s = lo + pack_sz;
        small = AuxArrVal(lo);
        while (s <= hi)
            {
            val = AuxArrVal(s);
            if (val == small)
                {
                SwapIdxVal(s, lo + pack_sz);
                pack_sz++;
                }
            if (val < small)
                {
                small = val;
                SwapIdxVal(lo, s);
                pack_sz = 1;
                }
            s++;
            }
        SamePacket(IDX, AUX, lo, lo + pack_sz - 1);
        lo = lo + pack_sz;
        }
    if (lo == hi)
        {
        AUX[IDX[lo]] = lo;
        IDX[lo] = -1;
        }
    }
static int PivotSelectionValue(int *IDX, int *AUX, int N, int h, int w, int sz)
    {
    int med;
    if (sz < SMALL_ARRAY_SIZE)
        med = w + (sz >> 1);
    else if (sz < LARGE_ARRAY_SIZE)
        med = Median3(w, w + (sz >> 1), w + sz - 1);
    else
        {
        int lo, mid, hi, sh, m1, m2, m3;
        lo = w;
        mid = w + (sz >> 1);
        hi = w + sz - 1;
        sh = (sz >> 3);
        m1 = Median3(lo, lo + sh, lo + (sh << 1));
        m2 = Median3(mid - sh, mid, mid + sh);
        m3 = Median3(hi - (sh << 1), hi - sh, hi);
        med = Median3(m1, m2, m3);
        }
    return AuxArrVal(med);
    }
static void MainSorter(int *IDX, int *AUX, int N, int h, int LO, int HI)
    {
    int lo, slo, hi, shi, piv, val, s, q;
    if ((HI - LO + 1) < SELECT_LIMIT)
        {
        SelectionSorter(IDX, AUX, N, h, LO, HI);
        return;
        }
    piv = PivotSelectionValue(IDX, AUX, N, h, LO, HI - LO + 1);
    lo = LO;
    hi = HI;
    slo = lo;
    shi = hi;
    while (true)
        {
        while (slo <= shi && (val = AuxArrVal(slo)) <= piv)
            {
            if (val == piv)
                {
                SwapIdxVal(lo, slo);
                lo++;
                }
            slo++;
            }
        while (shi >= slo && (val = AuxArrVal(shi)) >= piv)
            {
            if (val == piv)
                {
                SwapIdxVal(hi, shi);
                hi--;
                }
            shi--;
            }
        if (slo > shi)
            break;
        SwapIdxVal(slo, shi);
        slo++;
        shi--;
        }
    s = MIN(lo - LO, slo - lo);
    for (q = 0; q < s; q++)
        SwapIdxVal(LO + q, slo - s + q);
    s = MIN(HI - hi, hi - shi);
    for (q = 0; q < s; q++)
        SwapIdxVal(slo + q, HI + 1 - s + q);
    if (slo > lo)
        MainSorter(IDX, AUX, N, h, LO, LO - 1 + slo - lo);
    SamePacket(IDX, AUX, LO + slo - lo, HI - (hi - shi));
    if (hi > shi)
        MainSorter(IDX, AUX, N, h, HI + 1 - (hi - shi), HI);
    }
void SuffixSort(int *IDX, int *AUX, int N)
    {
    int w, ind, sort_run, h = 1;
    MainSorter(IDX, AUX, N, 0, 0, N - 1);
    while (IDX[0] > -N)
        {
        w = 0;
        sort_run = 0;
        while (w < N)
            {
            ind = IDX[w];
            if (ind < 0)
                {
                w += -ind;
                sort_run += -ind;
                }
            else
                {
                if (sort_run)
                    {
                    IDX[w - sort_run] = -sort_run;
                    sort_run = 0;
                    }
                MainSorter(IDX, AUX, N, h, w, AUX[ind]);
                w = AUX[ind] + 1;
                }
            }
        if (sort_run)
            IDX[w - sort_run] = -sort_run;
        h <<= 1;
        }
    }
void SuffixMake(uint8 *Data, int *IDX, int N)
    {
    int *AUX;
    int i;
    AUX = malloc(N * sizeof(int));
    for (i = 0; i < N; i++)
        {
        AUX[i] = Data[i];
        IDX[i] = i;
        }
    SuffixSort(IDX, AUX, N);
    for (i = 0; i < N; i++)
        IDX[AUX[i]] = i;
    free(AUX);
    }
