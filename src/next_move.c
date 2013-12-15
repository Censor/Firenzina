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

#ifndef next_move
#define next_move
#include "fire.h"
#include "next_move.c"
#include "white.h"
#else
#include "black.h"
#endif

uint32 MyNext(typePos* Position, typeNext* NextMove)
    {
    typeMoveList *p, *q, *list;
    uint32 move, Temp;
    switch (NextMove->phase)
        {
        case Trans1:
            NextMove->phase = CaptureGen;
            if (NextMove->trans_move && MyOK(Position, NextMove->trans_move))
                return(NextMove->trans_move);
        case CaptureGen:
            NextMove->phase = Capture_Moves;
            NextMove->move = 0;
            MyCapture(Position, NextMove->List, OppOccupied);
        case Capture_Moves:
            while (1)
                {
                p = NextMove->List + NextMove->move;
                move = p->move;
                if (!move)
                    break;
                q = p + 1;
                NextMove->move++;
                while (q->move)
                    {
                    if (move < q->move)
                        {
                        Temp = q->move;
                        q->move = move;
                        move = Temp;
                        }
                    q++;
                    }
                if ((move & 0x7fff) == NextMove->trans_move)
                    continue;
                if (!EasySEE(move) && !MySEE(Position, move))
                    NextMove->BadCaps[NextMove->bc++] = move;
                else
                    break;
                }
            if (move)
                return(move);
            NextMove->phase = Killer1;
            move = Position->Dyn->killer1;
            if (move && move != NextMove->trans_move && Position->sq[To(move)] == 0 && MyOK(Position, move))
                return(move);
        case Killer1:
            NextMove->phase = Killer2;
            move = Position->Dyn->killer2;
            if (move && move != NextMove->trans_move && Position->sq[To(move)] == 0 && MyOK(Position, move))
                return(move);
        case Killer2:
            NextMove->phase = Ordinary_Moves;
            NextMove->move = 0;
            list = MyOrdinary(Position, NextMove->List);
            SortOrdinary(NextMove->List, list, NextMove->trans_move, Position->Dyn->killer1, Position->Dyn->killer2);
        case Ordinary_Moves:
            move = (NextMove->List + NextMove->move)->move;
            NextMove->move++;
            if (move)
                return(move);
            NextMove->phase = BadCaps;
            NextMove->BadCaps[NextMove->bc] = 0;
            NextMove->move = 0;
        case BadCaps:
            move = NextMove->BadCaps[NextMove->move++];
            return(move);
        case Trans2:
            NextMove->phase = Capture_PGen2;
            if (NextMove->trans_move && MyOK(Position, NextMove->trans_move))
                return(NextMove->trans_move);
        case Capture_PGen2:
            NextMove->phase = CaptureMoves2;
            NextMove->move = 0;
            MyCapture(Position, NextMove->List, NextMove->Target);
        case CaptureMoves2:
            while (1)
                {
                p = NextMove->List + NextMove->move;
                move = p->move;
                if (!move)
                    break;
                q = p + 1;
                NextMove->move++;
                while (q->move)
                    {
                    if (move < q->move)
                        {
                        Temp = q->move;
                        q->move = move;
                        move = Temp;
                        }
                    q++;
                    }
                if ((move & 0x7fff) == NextMove->trans_move)
                    continue;
                else
                    break;
                }
            if (move)
                return(move);
            NextMove->move = 0;
            NextMove->phase = QuietChecks;
            MyQuietChecks(Position, NextMove->List, NextMove->Target);
        case QuietChecks:
            move = (NextMove->List + NextMove->move)->move;
            NextMove->move++;
            return(move);
        case Evade_Phase:
            move = (NextMove->List + NextMove->move)->move;
            NextMove->move++;
            return(move);
        case Trans3:
            NextMove->phase = CaptureGen3;
            if (NextMove->trans_move && MyOK(Position, NextMove->trans_move))
                return(NextMove->trans_move);
        case CaptureGen3:
            NextMove->phase = CaptureMoves3;
            NextMove->move = 0;
            MyCapture(Position, NextMove->List, OppOccupied);
        case CaptureMoves3:
            while (1)
                {
                p = NextMove->List + NextMove->move;
                move = p->move;
                if (!move)
                    break;
                q = p + 1;
                NextMove->move++;
                while (q->move)
                    {
                    if (move < q->move)
                        {
                        Temp = q->move;
                        q->move = move;
                        move = Temp;
                        }
                    q++;
                    }
                if ((move & 0x7fff) == NextMove->trans_move)
                    continue;
                else
                    break;
                }
            if (move)
                return(move);
            NextMove->move = 0;
            NextMove->phase = QuietChecks3;
            MyQuietChecks(Position, NextMove->List, OppOccupied);
        case QuietChecks3:
            move = (NextMove->List + NextMove->move)->move;
            NextMove->move++;
            if (move)
                return(move);
            NextMove->move = 0;
            NextMove->phase = PositionalGainPhase;
            MyPositionalGain(Position, NextMove->List, NextMove->mask);
        case PositionalGainPhase:
            move = (NextMove->List + NextMove->move)->move;
            NextMove->move++;
            return(move);
        case Fase0:
            return(0);
        }
    return 0;
    }
