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

#ifndef SEE
#define SEE
#include "fire.h"

static int Value[16] =
    {
    0, 100, 320, 12345678, 330, 330, 510, 1000,
    0, 100, 320, 12345678, 330, 330, 510, 1000
    };

void SetValueArray()
	{
	Value[0] = Value[8]  = 0;
	Value[1] = Value[9]  = PValue;
	Value[2] = Value[10] = NValue;
	Value[3] = Value[11] = 12345678;
	Value[4] = Value[12] = BValue;
	Value[5] = Value[13] = BValue;
	Value[6]=  Value[14] = RValue;
	Value[7] = Value[15] = QValue;
	}  

#include "SEE.c"
#include "white.h"
#else
#include "black.h"
#endif

bool MySEE(typePos *Position, uint32 move)
    {
    int fr, to, PieceValue, CaptureValue, d, dir;
    uint64 bit, cbf, mask, TableIndex[4], gcm = 0, T;
    int b, w;
    T = MyXray & OppOccupied;
    fr = From(move);
    to = To(move);
    while (T)
        {
        b = BSF(T);
        w = MyXrayTable[b];
        BitClear(b, T);
        if (fr != w && Line(to, b) != Line(b, OppKingSq))
            gcm |= SqSet[b];
        }
    gcm = ~gcm;
    PieceValue = Value[Position->sq[fr]];
    CaptureValue = Value[Position->sq[to]];
    if (PieceValue - CaptureValue > PValue && OppAttackedPawns[to] & BitboardOppP & gcm)
        return false;
    bit = (BitboardMyN | (BitboardOppN & gcm)) & AttN[to];
    d = PieceValue - CaptureValue;
    if (d > NValue && BitboardOppN & bit)
        return false;
    mask = BitboardMyQ | BitboardMyB | ((BitboardOppQ | BitboardOppB) & gcm);
    mask &= Diag[to];
    TableIndex[Direction_h1a8] = TableIndex[Direction_a1h8] = mask;
    bit |= AttB(to) & mask;
    if (d > BValue && (BitboardOppB & bit))
        return false;
    mask = BitboardMyQ | BitboardMyR | ((BitboardOppQ | BitboardOppR) & gcm);
    mask &= Ortho[to];
    TableIndex[Direction_horz] = TableIndex[Direction_vert] = mask;
    bit |= AttR(to) & mask;
    bit |= (BitboardMyK | BitboardOppK) & AttK[to];
    bit |= BitboardOppP & OppAttackedPawns[to] & gcm;
    bit |= BitboardMyP & MyAttackedPawns[to];
    cbf = ~(SqSet[fr] | SqSet[to]);
    bit &= cbf;
    dir = Line(fr, to);
    if (dir == Direction_h1a8 || dir == Direction_a1h8)
        bit |= AttB(fr) & TableIndex[dir] & cbf;
    if (dir == Direction_horz || dir == Direction_vert)
        bit |= AttR(fr) & TableIndex[dir] & cbf;
    CaptureValue -= PieceValue;
    do
        {
        cbf &= ~bit;
        mask = BitboardOppP & bit;
        if (mask)
            {
            bit ^= (~(mask - 1)) & mask;
            PieceValue = PValue;
            }
        else
            {
            mask = BitboardOppN & bit;
            if (mask)
                {
                bit ^= (~(mask - 1)) & mask;
                PieceValue = NValue;
                }
            else
                {
                mask = BitboardOppB & bit;
                if (mask)
                    {
                    PieceValue = BValue;
                    fr = BSF(mask);
                    mask = AttB(fr) & cbf & TableIndex[Direction_a1h8];
                    bit = mask | (SqClear[fr] & bit);
                    }
                else
                    {
                    mask = BitboardOppR & bit;
                    if (mask)
                        {
                        PieceValue = RValue;
                        fr = BSF(mask);
                        mask = AttR(fr) & cbf & TableIndex[Direction_horz];
                        bit = mask | (SqClear[fr] & bit);
                        }
                    else
                        {
                        mask = BitboardOppQ & bit;
                        if (mask)
                            {
                            PieceValue = QValue;
                            fr = BSF(mask);
                            if (Rank(fr) == Rank(to) || File(fr) == File(to))
                                mask = AttR(fr) & cbf & TableIndex[Direction_horz];
                            else
                                mask = AttB(fr) & cbf & TableIndex[Direction_a1h8];
                            bit = mask | (SqClear[fr] & bit);
                            }
                        else
                            {
                            if (!(BitboardOppK & bit))
                                return true;
                            PieceValue = 12345;
                            }
                        }
                    }
                }
            }
        CaptureValue += PieceValue;
        if (CaptureValue < -60)
            return false;
        mask = BitboardMyP & bit;
        if (mask)
            {
            bit ^= (~(mask - 1)) & mask;
            PieceValue = PValue;
            }
        else
            {
            mask = BitboardMyN & bit;
            if (mask)
                {
                bit ^= (~(mask - 1)) & mask;
                PieceValue = NValue;
                }
            else
                {
                mask = BitboardMyB & bit;
                if (mask)
                    {
                    PieceValue = BValue;
                    fr = BSF(mask);
                    mask = AttB(fr) & cbf & TableIndex[Direction_a1h8];
                    bit = mask | (SqClear[fr] & bit);
                    }
                else
                    {
                    mask = BitboardMyR & bit;
                    if (mask)
                        {
                        PieceValue = RValue;
                        fr = BSF(mask);
                        mask = AttR(fr) & cbf & TableIndex[Direction_horz];
                        bit = mask | (SqClear[fr] & bit);
                        }
                    else
                        {
                        mask = BitboardMyQ & bit;
                        if (mask)
                            {
                            PieceValue = QValue;
                            fr = BSF(mask);
                            if (Rank(fr) == Rank(to) || File(fr) == File(to))
                                mask = AttR(fr) & cbf & TableIndex[Direction_horz];
                            else
                                mask = AttB(fr) & cbf & TableIndex[Direction_a1h8];
                            bit = mask | (SqClear[fr] & bit);
                            }
                        else
                            {
                            if (!(BitboardMyK & bit))
                                return false;
                            if (CaptureValue > 6174)
                                return true;
                            PieceValue = 23456;
                            }
                        }
                    }
                }
            }
        CaptureValue -= PieceValue;
        } while (CaptureValue < -60);
    return true;
    }