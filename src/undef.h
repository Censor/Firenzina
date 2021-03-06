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

#undef MyPVNodeSMP
#undef OppPVNodeSMP
#undef MyAllSMP
#undef OppAllSMP
#undef MyCutSMP
#undef OppCutSMP
#undef MyMultiPV
#undef MyPVQsearch
#undef MyPVQsearchCheck
#undef MyQsearch
#undef MyQsearchCheck
#undef MyLowDepth
#undef MyLowDepthCheck
#undef MyExclude
#undef MyExcludeCheck
#undef MyCut
#undef MyCutCheck
#undef MyAll
#undef MyAllCheck
#undef MyPV
#undef MyRootNode
#undef MyAnalysis
#undef MyTop
#undef MyTopAnalysis
#undef MyAttacked
#undef OppAttacked
#undef MyAttackedPawns
#undef OppAttackedPawns
#undef OppPVQsearch
#undef OppPVQsearchCheck
#undef OppQsearch
#undef OppQsearchCheck
#undef OppLowDepth
#undef OppLowDepthCheck
#undef OppExclude
#undef OppExcludeCheck
#undef OppCut
#undef OppCutCheck
#undef OppAll
#undef OppAllCheck
#undef OppPV
#undef OppRootNode
#undef OppAnalysis
#undef OppTop
#undef OppTopAnalysis
#undef OppOccupied
#undef BitboardOppP
#undef BitboardOppN
#undef BitboardOppBL
#undef BitboardOppBD
#undef BitboardOppR
#undef BitboardOppQ
#undef BitboardOppK
#undef MyOccupied
#undef BitboardMyP
#undef BitboardMyN
#undef BitboardMyBL
#undef BitboardMyBD
#undef BitboardMyR
#undef BitboardMyQ
#undef BitboardMyK
#undef MyKingSq
#undef OppKingSq
#undef EnumOppP
#undef EnumOppN
#undef EnumOppBL
#undef EnumOppBD
#undef EnumOppR
#undef EnumOppQ
#undef EnumOppK
#undef EnumMyP
#undef EnumMyN
#undef EnumMyBL
#undef EnumMyBD
#undef EnumMyR
#undef EnumMyQ
#undef EnumMyK
#undef OppKingCheck
#undef MyCapture
#undef MyOrdinary
#undef MyPositionalGain
#undef MyEvasion
#undef MyQuietChecks
#undef MySEE
#undef Make
#undef Undo
#undef IllegalMove
#undef MoveIsCheck
#undef MyOK
#undef MyNext
#undef MyNull
#undef HasPiece
#undef SixthRank
#undef FifthRank
#undef FourthRank
#undef FourthRankBitBoard
#undef IsPassedPawn
#undef MyXray
#undef OppXray
#undef MyXrayTable
#undef OppXrayTable
#undef MyKingCheck
#undef MyCount
#undef CountEvasions
#undef InCheck
#undef OppKdanger
#undef EighthRank
#undef CaptureRight
#undef FromLeft
#undef CaptureLeft
#undef FromRight
#undef Forward
#undef ForwardLeft
#undef ForwardRight
#undef Backward
#undef ForwardShift
#undef BackShift
#undef SeventhRank
#undef BackShift2
#undef Forward2
#undef SecondRank
#undef SecondSixthRanks
#undef OnThirdRank
#undef SecondSixthRanks
#undef BitBoardSeventhRank
#undef BitBoardEighthRank
#undef WhiteA7
#undef WhiteH7
#undef CaptureEP
#undef PromQueenCap
#undef PromKnightCap
#undef CastleOO
#undef CastleOOO
#undef WhiteF1G1
#undef WhiteC1D1
#undef WhiteB1C1D1
#undef WhiteE1
#undef WhiteC1
#undef WhiteG1
#undef UnderProm
#undef OnSecondRank
#undef CanCaptureRight
#undef CanCaptureLeft
#undef PawnGuard
#undef FourthEighthRankNoH
#undef FourthEighthRankNoA
#undef BackRight
#undef BackRight2
#undef BackRight3
#undef BackLeft
#undef BackLeft2
#undef BackLeft3
#undef NumberRank5
#undef NumberRank1
#undef NumberRank2
#undef NumberRank7
#undef PieceIsMine
#undef PieceIsOpp
#undef Backward2
#undef PieceIsOppPawn
