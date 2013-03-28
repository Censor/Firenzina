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
#define MaxAge 256
#define MaxDepth 256

#define AgeDepthMix(a, x, y) \
  ((((a) - (x)) & (MaxAge - 1)) * MaxDepth + (MaxDepth - ((y) + 1)))

void IncrementAge()
    {
    GlobalAge += 1;
    if (GlobalAge == MaxAge)
        GlobalAge = 0;
    }
void HashLowerAllNew(uint64 Z, int move, int depth, int Value, int ht, int age)
    {
    int Depth, i;
    TransDeclare();
    int max = 0, w = 0, mix;
    move &= 0x7fff;
    Trans = HashPointer(Z);
    Value = ValueAdjustStore(Value, ht);
    for (i = 0; i < 4; i++, Trans++)
        {
        HyattHash(Trans, trans);
        if ((trans->hash ^ (Z >> 32)) == 0 && (!trans->DepthLower || IsAll(trans)) && trans->DepthLower <= depth)
            {
            trans->DepthLower = depth;
            trans->move = move;
            trans->ValueLower = Value;
            trans->age = age;
            trans->flags |= FlagLower | FlagAll;
            HyattHashWrite(trans, Trans);
            return;
            }
        Depth = MAX(trans->DepthLower, trans->DepthUpper);
        mix = AgeDepthMix(age, trans->age, Depth);
        if (mix > max)
            {
            max = mix;
            w = i;
            }
        }
    Trans = HashPointer(Z) + w;
    trans->hash = (Z >> 32);
    trans->DepthUpper = 0;
    trans->ValueUpper = 0;
    trans->DepthLower = depth;
    trans->move = move;
    trans->ValueLower = Value;
    trans->age = age;
    trans->flags = FlagLower | FlagAll;
    HyattHashWrite(trans, Trans);
    return;
    }
void HashUpperCutNew(uint64 Z, int depth, int Value, int ht, int age)
    {
    int Depth, i;
    TransDeclare();
    int max = 0, w = 0, mix;
    Trans = HashPointer(Z);
    Value = ValueAdjustStore(Value, ht);
    for (i = 0; i < 4; i++, Trans++)
        {
        HyattHash(Trans, trans);
        if (!(trans->hash ^ (Z >> 32)) && (!trans->DepthUpper || IsCut(trans)) && trans->DepthUpper <= depth)
            {
            trans->DepthUpper = depth;
            trans->ValueUpper = Value;
            trans->age = age;
            trans->flags |= FlagUpper | FlagCut;
            HyattHashWrite(trans, Trans);
            return;
            }
        Depth = MAX(trans->DepthLower, trans->DepthUpper);
        mix = AgeDepthMix(age, trans->age, Depth);
        if (mix > max)
            {
            max = mix;
            w = i;
            }
        }
    Trans = HashPointer(Z) + w;
    trans->hash = (Z >> 32);
    trans->DepthLower = 0;
    trans->move = 0;
    trans->ValueLower = 0;
    trans->DepthUpper = depth;
    trans->ValueUpper = Value;
    trans->age = age;
    trans->flags = FlagUpper | FlagCut;
    HyattHashWrite(trans, Trans);
    return;
    }
void HashLowerNew(uint64 Z, int move, int depth, int Value, int Flags, int ht, int age)
    {
    int Depth, i;
    TransDeclare();
    int max = 0, w = 0, mix;
    move &= 0x7fff;
    Trans = HashPointer(Z);
    Value = ValueAdjustStore(Value, ht);
    for (i = 0; i < 4; i++, Trans++)
        {
        HyattHash(Trans, trans);
        if (!(trans->hash ^ (Z >> 32)) && !IsExact(trans) && trans->DepthLower <= depth)
            {
            trans->DepthLower = depth;
            trans->move = move;
            trans->ValueLower = Value;
            trans->age = age;
            trans->flags &= ~(FlagAll | LowerFromPV);
            trans->flags |= FlagLower | Flags;
            HyattHashWrite(trans, Trans);
            return;
            }
        Depth = MAX(trans->DepthLower, trans->DepthUpper);
        mix = AgeDepthMix(age, trans->age, Depth);
        if (mix > max)
            {
            max = mix;
            w = i;
            }
        }
    Trans = HashPointer(Z) + w;
    trans->hash = (Z >> 32);
    trans->DepthUpper = 0;
    trans->ValueUpper = 0;
    trans->DepthLower = depth;
    trans->move = move;
    trans->ValueLower = Value;
    trans->age = age;
    trans->flags = FlagLower | Flags;
    HyattHashWrite(trans, Trans);
    return;
    }
void HashUpperNew(uint64 Z, int depth, int Value, int Flags, int ht, int age)
    {
    int Depth, i;
    TransDeclare();
    int max = 0, w = 0, mix;
    Trans = HashPointer(Z);
    Value = ValueAdjustStore(Value, ht);
    for (i = 0; i < 4; i++, Trans++)
        {
        HyattHash(Trans, trans);
        if (!(trans->hash ^ (Z >> 32)) && !IsExact(trans) && trans->DepthUpper <= depth)
            {
            trans->DepthUpper = depth;
            trans->ValueUpper = Value;
            trans->age = age;
            trans->flags &= ~(FlagCut | UpperFromPV);
            trans->flags |= FlagUpper | Flags;
            HyattHashWrite(trans, Trans);
            return;
            }
        Depth = MAX(trans->DepthLower, trans->DepthUpper);
        mix = AgeDepthMix(age, trans->age, Depth);
        if (mix > max)
            {
            max = mix;
            w = i;
            }
        }
    Trans = HashPointer(Z) + w;
    trans->hash = (Z >> 32);
    trans->DepthLower = 0;
    trans->move = 0;
    trans->ValueLower = 0;
    trans->DepthUpper = depth;
    trans->ValueUpper = Value;
    trans->age = age;
    trans->flags = FlagUpper | Flags;
    HyattHashWrite(trans, Trans);
    return;
    }
static void pv_zobrist(uint64 Z, int move, int depth, int Value, int age)
    {
    int i, k = Z & PVHashMask;
    TransPVDeclare();
    int w = 0, max = 0, mix;
     for (i = 0; i < 4; i++)
        {
        Trans_pv = PVHashTable + (k + i);
        HyattHash(Trans_pv, trans_pv);
        if (trans_pv->hash == Z)
            {
            trans_pv->depth = depth;
            trans_pv->Value = Value;
            trans_pv->move = move;
            trans_pv->age = age;
            HyattHashWrite(trans_pv, Trans_pv);
            return;
            }
        mix = AgeDepthMix(age, trans_pv->age, trans_pv->depth);
        if (mix > max)
            {
            max = mix;
            w = i;
            }
        }
    Trans_pv = PVHashTable + (k + w);
    trans_pv->hash = Z;
    trans_pv->depth = depth;
    trans_pv->move = move;
    trans_pv->Value = Value;
    trans_pv->age = age;
    HyattHashWrite(trans_pv, Trans_pv);
    }
void HashExactNew(uint64 Z, int move, int depth, int Value, int Flags, int ht, int age)
    {
    int Depth, i, j;
    TransDeclare();
    int max = 0, w = 0, mix;
    move &= 0x7fff;
    pv_zobrist(Z, move, depth, Value, age);
    Trans = HashPointer(Z);
    Value = ValueAdjustStore(Value, ht);
    for (i = 0; i < 4; i++, Trans++)
        {
        HyattHash(Trans, trans);
        if ((trans->hash ^ (Z >> 32)) == 0 && MAX(trans->DepthUpper, trans->DepthLower) <= depth)
            {
            trans->DepthUpper = trans->DepthLower = depth;
            trans->move = move;
            trans->ValueUpper = trans->ValueLower = Value;
            trans->age = age;
            trans->flags = Flags;
            HyattHashWrite(trans, Trans);
            for (j = i + 1; j < 4; j++)
                {
                Trans++;
                HyattHash(Trans, trans);
                if ((trans->hash ^ (Z >> 32)) == 0 && MAX(trans->DepthUpper, trans->DepthLower) <= depth)
                    {
                    memset(trans, 0, 16);
                    trans->age = age ^ (MaxAge >> 1);
                    HyattHashWrite(trans, Trans);
                    }
                }
            return;
            }
        Depth = MAX(trans->DepthLower, trans->DepthUpper);
        mix = AgeDepthMix(age, trans->age, Depth);
        if (mix > max)
            {
            max = mix;
            w = i;
            }
        }
    Trans = HashPointer(Z) + w;
    trans->hash = (Z >> 32);
    trans->DepthUpper = trans->DepthLower = depth;
    trans->move = move;
    trans->ValueUpper = trans->ValueLower = Value;
    trans->age = age;
    trans->flags = Flags;
    HyattHashWrite(trans, Trans);
    return;
    }
