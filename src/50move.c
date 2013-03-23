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

int Move50(typePos *Position)
    {
    typeMoveList List[256];
    typeMoveList *p;
    EvasionMoves(Position, List, 0xffffffffffffffff);
    p = List;
    while (p->move)
        {
        Make(Position, p->move);
        Mobility(Position);
        if (!Position->wtm ? (wBitboardK & Position->Dyn->bAtt) : (bBitboardK & Position->Dyn->wAtt))
            {
            Undo(Position, p->move);
            p++;
            continue;
            }
        Undo(Position, p->move);
        return 0;
        }
    return HeightMultiplier * Height(Position) - ValueMate;
    }
