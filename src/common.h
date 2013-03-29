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

#define TempoValue (4 + ((Position->Dyn->material & 0xff) >> 2))
#define TempoValue2 (4 + ((Position->Dyn->material & 0xff) >> 2))
#define EVAL(m, dp) Eval (Position, -0x7fff0000, 0x7fff0000, m, dp)
#define EvalLazy(B, A, p, m) Eval (Position, (B) - (p), (A) + (p), m, depth)
#define EvalQsearch(v, m) EvalLazy ((v), (v), LazyEvalMin, (m))
#define EvalLowDepth(v, m) EvalLazy ((v), (v), LazyEvalMin, (m))
#define EvalCutAll(v, m) EvalLazy ((v), (v), LazyEvalMax, (m))
#define MaxPositional(m) MaxPosGain (Position->sq[From (m)], (m) & 07777)
#define IsRepetition(x)                                  \
  (value > x && Pos0->reversible >= 2 &&                 \
   ((To(move) << 6) | From(move)) == (Pos0 - 1)->move && \
   Position->sq[To(move)] == 0)
#define IsInterpose(x) (x & (1 << 15))
#define EasySEE(x) (x & 0x300000)
#define PassedPawnPush(to, x) \
  (Position->sq[to] == EnumMyP && x && (BitboardOppP & IsPassedPawn[to]) == 0)
#define PieceIsWhite(pi) (pi <= 7)
#define PieceIsBlack(pi) (pi >= 8)
#define QSearchCondition (new_depth <= 1)
#define LowDepthCondition (new_depth <= 9)
#define LowDepthConditionPV (new_depth <= 9)
