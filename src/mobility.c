/******************************************************************************
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
void Mobility(typePos *Position)
    {
    uint64 U, A, T, AttB, AttR;
    int b;
    Position->Dyn->wXray = 0;
    Position->Dyn->bXray = 0;
    A = AttK[Position->wKsq];
    Position->Dyn->wAtt = A;
    if (A & bBitboardK)
       Position->Dyn->bKcheck = SqSet[Position->wKsq];
    else
       Position->Dyn->bKcheck = 0;
    A = AttK[Position->bKsq];
    Position->Dyn->bAtt = A;
    if (A & wBitboardK)
       Position->Dyn->wKcheck = SqSet[Position->bKsq];
    else
       Position->Dyn->wKcheck = 0;
    for (U = wBitboardN; U; BitClear(b, U))
        {
        b = BSF(U);
        A = AttN[b];
        Position->Dyn->wAtt |= A;
        if (A & bBitboardK)
           Position->Dyn->bKcheck |= SqSet[b];
        }
    for (U = wBitboardB; U; BitClear(b, U))
        {
        b = BSF(U);
        A = AttB(b);
        Position->Dyn->wAtt |= A;
        if (A & bBitboardK)
           Position->Dyn->bKcheck |= SqSet[b];
        else if (bBitboardK & Diag[b])
            {
            T = AttB(Position->bKsq) & A;
            Position->Dyn->wXray |= T;
            if (T)
               Position->XrayW[BSF(T)] = b;
            }
        }
    for (U = wBitboardR; U; BitClear(b, U))
        {
        b = BSF(U);
        A = AttR(b);
        Position->Dyn->wAtt |= A;
        if (A & bBitboardK)
           Position->Dyn->bKcheck |= SqSet[b];
        else if (bBitboardK & Ortho[b])
            {
            T = AttR(Position->bKsq) & A;
            Position->Dyn->wXray |= T;
            if (T)
               Position->XrayW[BSF(T)] = b;
            }
        }
    for (U = wBitboardQ; U; BitClear(b, U))
        {
        b = BSF(U);
        AttR = AttR(b);
        AttB = AttB(b);
        A = AttB | AttR;
        Position->Dyn->wAtt |= A;
        if (A & bBitboardK)
           Position->Dyn->bKcheck |= SqSet[b];
        else if (bBitboardK & Diag[b])
            {
            T = AttB(Position->bKsq) & AttB;
            Position->Dyn->wXray |= T;
            if (T)
               Position->XrayW[BSF(T)] = b;
            }
        else if (bBitboardK & Ortho[b])
            {
            T = AttR(Position->bKsq) & AttR;
            Position->Dyn->wXray |= T;
            if (T)
               Position->XrayW[BSF(T)] = b;
            }
        }
    for (U = bBitboardN; U; BitClear(b, U))
        {
        b = BSF(U);
        A = AttN[b];
        Position->Dyn->bAtt |= A;
        if (A & wBitboardK)
           Position->Dyn->wKcheck |= SqSet[b];
        }
    for (U = bBitboardB; U; BitClear(b, U))
        {
        b = BSF(U);
        A = AttB(b);
        Position->Dyn->bAtt |= A;
        if (A & wBitboardK)
           Position->Dyn->wKcheck |= SqSet[b];
        else if (wBitboardK & Diag[b])
            {
            T = AttB(Position->wKsq) & A;
            Position->Dyn->bXray |= T;
            if (T)
               Position->XrayB[BSF(T)] = b;
            }
        }
    for (U = bBitboardR; U; BitClear(b, U))
        {
        b = BSF(U);
        A = AttR(b);
        Position->Dyn->bAtt |= A;
        if (A & wBitboardK)
           Position->Dyn->wKcheck |= SqSet[b];
        else if (wBitboardK & Ortho[b])
            {
            T = AttR(Position->wKsq) & A;
            Position->Dyn->bXray |= T;
            if (T)
               Position->XrayB[BSF(T)] = b;
            }
        }
    for (U = bBitboardQ; U; BitClear(b, U))
        {
        b = BSF(U);
        AttB = AttB(b);
        AttR = AttR(b);
        A = AttB | AttR;
        Position->Dyn->bAtt |= A;
        if (A & wBitboardK)
           Position->Dyn->wKcheck |= SqSet[b];
        else if (wBitboardK & Diag[b])
            {
            T = AttB(Position->wKsq) & AttB;
            Position->Dyn->bXray |= T;
            if (T)
               Position->XrayB[BSF(T)] = b;
            }
        else if (wBitboardK & Ortho[b])
            {
            T = AttR(Position->wKsq) & AttR;
            Position->Dyn->bXray |= T;
            if (T)
               Position->XrayB[BSF(T)] = b;
            }
        }
    A = (wBitboardP &(~FileA)) << 7;
    T = A & bBitboardK;
    Position->Dyn->bKcheck |= (T >> 7);
    Position->Dyn->wAtt |= A;
    A = (wBitboardP &(~FileH)) << 9;
    T = A & bBitboardK;
    Position->Dyn->bKcheck |= (T >> 9);
    Position->Dyn->wAtt |= A;
    A = (bBitboardP &(~FileH)) >> 7;
    T = A & wBitboardK;
    Position->Dyn->wKcheck |= (T << 7);
    Position->Dyn->bAtt |= A;
    A = (bBitboardP &(~FileA)) >> 9;
    T = A & wBitboardK;
    Position->Dyn->wKcheck |= (T << 9);
    Position->Dyn->bAtt |= A;
    }
