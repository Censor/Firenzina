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

#if defined(__GNUC__)
#define INLINE inline
#endif

static INLINE void MakeNull(typePos* Position)
    {
    Position->nodes++;
    Position->Dyn->SavedFlags = Position->Dyn->flags;
    memcpy(Position->Dyn + 1, Position->Dyn, 64);
    Position->Dyn++;
    Position->Dyn->Hash ^= HashWTM;
    Position->wtm ^= 1;
    Position->height++;
	UpdateSeldepth(Position);
    Position->Dyn->reversible++;
    if (Position->Dyn->ep)
        {
        Position->Dyn->Hash ^= HashEP[Position->Dyn->ep & 7];
        Position->Dyn->ep = 0;
        }
    Position->Dyn->Value = -((Position->Dyn - 1)->Value + TempoValue);
    Position->Dyn->PositionalValue = (Position->Dyn - 1)->PositionalValue;
    Position->Dyn->lazy = (Position->Dyn - 1)->lazy;
    Position->Dyn->flags &= ~3;
    Position->Dyn->move = 0;
    Position->Stack[++(Position->StackHeight)] = Position->Dyn->Hash;
    }
static INLINE void UndoNull(typePos *Position)
    {
    Position->Dyn--;
    Position->StackHeight--;
    Position->height--;
    Position->wtm ^= 1;
    Position->Dyn->flags = Position->Dyn->SavedFlags;
    }
