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
// Do not include this file in the Windows project! Yuri Censor for Firenzina, 3/25/2013
#include "fire.h"
#include <string.h>

#define MaxAge 256
#define MaxDepth 256

#define AgeDepthMix(x,y) \
	(((Age - (x)) & (MaxAge - 1)) * MaxDepth   + (MaxDepth - ((y) + 1)))

void IncrementAge(void)
{
    Age += 1;

    if (Age == MaxAge)
        Age = 0;
}

static uint64 HashSize = 0x400000;
static boolean FlagHashInit = 0;
void HashClear(void)
{
    int i;
    memset(HashTable, 0, HashSize * sizeof(typeHash));
    memset(PVHashTable, 0, 0x10000 * sizeof(typePVHash));
    for (i = 0; i < HashSize; i++)
        (HashTable + i)->age = (MaxAge / 2);
    Age = 0;
}
int InitHash(int mb)
{
    Age = 0;
    HashSize = ((1ULL << MSB(mb)) << 20) / sizeof(typeHash);

    if (HashSize > 0x100000000)
        HashSize = 0x100000000;
    mb = (HashSize * sizeof(typeHash)) >> 20;
    HashMask = HashSize - 4;

    if (FlagHashInit)
        AlignedFree(HashTable);
    FlagHashInit = true;
    MemAlign(HashTable, 64, HashSize * sizeof(typeHash));
    HashClear();
    return mb;
}
void HashLowerALL(typePos *Position, int move, int depth, int Value)
{
    int Depth, i, k = Position->Dyn->Hash & HashMask;
    typeHash *trans;
    int max = 0, w = 0;
    move &= 0x7fff;

    for (i = 0; i < 4; i++)
    {
        trans = HashTable + (k + i);

        if ((trans->hash ^ (Position->Dyn->Hash >> 32)) == 0 && (!trans->DepthLower || IsAll(trans))
                && trans->DepthLower <= depth)
        {
            trans->DepthLower = depth;
            trans->move = move;
            trans->ValueUpper = Value;
            trans->age = Age;
            trans->flags |= FlagLower | FlagAll;
            return;
        }
        Depth = MAX(trans->DepthLower, trans->DepthUpper);

        if (AgeDepthMix(trans->age, Depth) > max)
        {
            max = AgeDepthMix(trans->age, Depth);
            w = i;
        }
    }
    trans = HashTable + (k + w);
    trans->hash = (Position->Dyn->Hash >> 32);
    trans->DepthUpper = 0;
    trans->ValueLower = 0;
    trans->DepthLower = depth;
    trans->move = move;
    trans->ValueUpper = Value;
    trans->age = Age;
    trans->flags = FlagLower | FlagAll;
    return;
}
void HashUpperCUT(typePos *Position, int depth, int Value)
{
    int Depth, i, k = Position->Dyn->Hash & HashMask;
    typeHash *trans;
    int max = 0, w = 0;

    for (i = 0; i < 4; i++)
    {
        trans = HashTable + (k + i);

        if (!(trans->hash ^ (Position->Dyn->Hash >> 32)) && (!trans->DepthUpper || IsCut(trans))
                && trans->DepthUpper <= depth)
        {
            trans->DepthUpper = depth;
            trans->ValueLower = Value;
            trans->age = Age;
            trans->flags |= FlagUpper | FlagCut;
            return;
        }
        Depth = MAX(trans->DepthLower, trans->DepthUpper);

        if (AgeDepthMix(trans->age, Depth) > max)
        {
            max = AgeDepthMix(trans->age, Depth);
            w = i;
        }
    }
    trans = HashTable + (k + w);
    trans->hash = (Position->Dyn->Hash >> 32);
    trans->DepthLower = 0;
    trans->move = 0;
    trans->ValueUpper = 0;
    trans->DepthUpper = depth;
    trans->ValueLower = Value;
    trans->age = Age;
    trans->flags = FlagUpper | FlagCut;
    return;
}
void HashLower(uint64 Z, int move, int depth, int Value)
{
    int Depth, i, k = Z & HashMask;
    typeHash *trans;
    int max = 0, w = 0;
    move &= 0x7fff;

    for (i = 0; i < 4; i++)
    {
        trans = HashTable + (k + i);

        if (!(trans->hash ^ (Z >> 32)) && !IsExact(trans) && trans->DepthLower <= depth)
        {
            trans->DepthLower = depth;
            trans->move = move;
            trans->ValueUpper = Value;
            trans->age = Age;
            trans->flags |= FlagLower;
            trans->flags &= ~FlagAll;
            return;
        }
        Depth = MAX(trans->DepthLower, trans->DepthUpper);

        if (AgeDepthMix(trans->age, Depth) > max)
        {
            max = AgeDepthMix(trans->age, Depth);
            w = i;
        }
    }
    trans = HashTable + (k + w);
    trans->hash = (Z >> 32);
    trans->DepthUpper = 0;
    trans->ValueLower = 0;
    trans->DepthLower = depth;
    trans->move = move;
    trans->ValueUpper = Value;
    trans->age = Age;
    trans->flags = FlagLower;
    return;
}
void HashUpper(uint64 Z, int depth, int Value)
{
    int Depth, i, k = Z & HashMask;
    typeHash *trans;
    int max = 0, w = 0;

    for (i = 0; i < 4; i++)
    {
        trans = HashTable + (k + i);

        if (!(trans->hash ^ (Z >> 32)) && !IsExact(trans) && trans->DepthUpper <= depth)
        {
            trans->DepthUpper = depth;
            trans->ValueLower = Value;
            trans->age = Age;
            trans->flags |= FlagUpper;
            trans->flags &= ~FlagCut;
            return;
        }
        Depth = MAX(trans->DepthLower, trans->DepthUpper);

        if (AgeDepthMix(trans->age, Depth) > max)
        {
            max = AgeDepthMix(trans->age, Depth);
            w = i;
        }
    }
    trans = HashTable + (k + w);
    trans->hash = (Z >> 32);
    trans->DepthLower = 0;
    trans->move = 0;
    trans->ValueUpper = 0;
    trans->DepthUpper = depth;
    trans->ValueLower = Value;
    trans->age = Age;
    trans->flags = FlagUpper;
    return;
}
static void PVHash(typePos *Position, int move, int depth, int Value)
{
    int i, k = Position->Dyn->Hash & PVHashMask;
    typePVHash *trans;
    int w = 0, max = 0;

    for (i = 0; i < 4; i++)
    {
        trans = PVHashTable + (k + i);

        if (trans->hash == Position->Dyn->Hash)
        {
            trans->depth = depth;
            trans->Value = Value;
            trans->move = move;
            trans->age = Age;
            return;
        }

        if (AgeDepthMix(trans->age, trans->depth) > max)
        {
            max = AgeDepthMix(trans->age, trans->depth);
            w = i;
        }
    }
    trans = PVHashTable + (k + w);
    trans->hash = Position->Dyn->Hash;
    trans->depth = depth;
    trans->move = move;
    trans->Value = Value;
    trans->age = Age;
}

void HashExact(typePos *Position, int move, int depth, int Value, int Flags)
{
    int Depth, i, j, k = Position->Dyn->Hash & HashMask;
    typeHash *trans;
    int max = 0, w = 0;
    move &= 0x7fff;
    PVHash(Position, move, depth, Value);

    for (i = 0; i < 4; i++)
    {
        trans = HashTable + (k + i);

        if ((trans->hash ^ (Position->Dyn->Hash >> 32)) == 0 && MAX(trans->DepthUpper, trans->DepthLower) <= depth)
        {
            trans->DepthUpper = trans->DepthLower = depth;
            trans->move = move;
            trans->ValueLower = trans->ValueUpper = Value;
            trans->age = Age;
            trans->flags = Flags;

            for (j = i + 1; j < 4; j++)
            {
                trans = HashTable + (k + j);

                if ((trans->hash ^ (Position->Dyn->Hash >> 32)) == 0
                        && MAX(trans->DepthUpper, trans->DepthLower) <= depth)
                {
                    memset(trans, 0, 16);
                    trans->age = Age ^ (MaxAge / 2);
                }
            }
            return;
        }
        Depth = MAX(trans->DepthLower, trans->DepthUpper);

        if (AgeDepthMix(trans->age, Depth) > max)
        {
            max = AgeDepthMix(trans->age, Depth);
            w = i;
        }
    }
    trans = HashTable + (k + w);
    trans->hash = (Position->Dyn->Hash >> 32);
    trans->DepthUpper = trans->DepthLower = depth;
    trans->move = move;
    trans->ValueLower = trans->ValueUpper = Value;
    trans->age = Age;
    trans->flags = Flags;
    return;
}
