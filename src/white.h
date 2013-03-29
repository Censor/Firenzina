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

#include "undef.h"
#define MyPVNodeSMP WhitePVNodeSMP
#define OppPVNodeSMP BlackPVNodeSMP
#define MyAllSMP WhiteAllSMP
#define OppAllSMP BlackAllSMP
#define MyCutSMP WhiteCutSMP
#define OppCutSMP BlackCutSMP
#define MyMultiPV WhiteMultiPV
#define MyPVQsearch PVQsearchWhite
#define MyPVQsearchCheck PVQsearchWhiteCheck
#define MyQsearch QsearchWhite
#define MyQsearchCheck QsearchWhiteCheck
#define MyLowDepth LowDepthWhite
#define MyLowDepthCheck LowDepthWhiteCheck
#define MyExclude ExcludeWhite
#define MyExcludeCheck ExcludeWhiteCheck
#define MyCut CutNodeWhite
#define MyCutCheck CutNodeWhiteCheck
#define MyAll AllNodeWhite
#define MyAllCheck AllNodeWhiteCheck
#define MyPV PVNodeWhite
#define MyRootNode RootWhite
#define MyAnalysis WhiteAnalysis
#define MyTop TopWhite
#define MyTopAnalysis WhiteTopAnalysis
#define MyAttacked Position->Dyn->wAtt
#define OppAttacked Position->Dyn->bAtt
#define MyAttackedPawns AttPw
#define OppAttackedPawns AttPb
#define OppPVQsearch PVQsearchBlack
#define OppPVQsearchCheck PVQsearchBlackCheck
#define OppQsearch QsearchBlack
#define OppQsearchCheck QsearchBlackCheck
#define OppLowDepth LowDepthBlack
#define OppLowDepthCheck LowDepthBlackCheck
#define OppExclude ExcludeBlack
#define OppExcludeCheck ExcludeBlackCheck
#define OppCut CutNodeBlack
#define OppCutCheck CutNodeBlackCheck
#define OppAll AllNodeBlack
#define OppAllCheck AllNodeBlackCheck
#define OppPV PVNodeBlack
#define OppRootNode RootBlack
#define OppAnalysis BlackAnalysis
#define OppTop TopBlack
#define OppTopAnalysis BlackTopAnalysis
#define OppOccupied bBitboardOcc
#define BitboardOppP bBitboardP
#define BitboardOppN bBitboardN
#define BitboardOppBL bBitboardBL
#define BitboardOppBD bBitboardBD
#define BitboardOppB (BitboardOppBL | BitboardOppBD)
#define BitboardOppR bBitboardR
#define BitboardOppQ bBitboardQ
#define BitboardOppK bBitboardK
#define MyOccupied wBitboardOcc
#define BitboardMyP wBitboardP
#define BitboardMyN wBitboardN
#define BitboardMyBL wBitboardBL
#define BitboardMyBD wBitboardBD
#define BitboardMyB (BitboardMyBL | BitboardMyBD)
#define BitboardMyR wBitboardR
#define BitboardMyQ wBitboardQ
#define BitboardMyK wBitboardK
#define MyKingSq Position->wKsq
#define OppKingSq Position->bKsq
#define EnumOppP bEnumP
#define EnumOppN bEnumN
#define EnumOppBL bEnumBL
#define EnumOppBD bEnumBD
#define EnumOppR bEnumR
#define EnumOppQ bEnumQ
#define EnumOppK bEnumK
#define EnumMyP wEnumP
#define EnumMyN wEnumN
#define EnumMyBL wEnumBL
#define EnumMyBD wEnumBD
#define EnumMyR wEnumR
#define EnumMyQ wEnumQ
#define EnumMyK wEnumK
#define OppKingCheck Position->Dyn->bKcheck
#define MyCapture WhiteCaptures
#define MyOrdinary WhiteOrdinary
#define MyPositionalGain PositionalMovesWhite
#define MyEvasion WhiteEvasions
#define MyQuietChecks QuietChecksWhite
#define MySEE WhiteSEE
#define Make MakeWhite
#define Undo UndoWhite
#define IllegalMove MoveIsCheckWhite
#define MoveIsCheck MoveIsCheckBlack
#define MyOK WhiteOK
#define MyNext NextWhite
#define MyNull ((Position->Dyn->flags) & 2)
#define HasPiece (wBitboardOcc ^ wBitboardK ^ wBitboardP)
#define FourthRank(x) ((x) >= A4)
#define FifthRank(x) ((x) <= A5)
#define SixthRank(x) ((x) >= A6)
#define FourthRankBitBoard Rank4
#define IsPassedPawn PassedPawnW
#define MyXray (Position->Dyn->wXray)
#define OppXray (Position->Dyn->bXray)
#define MyXrayTable Position->XrayW
#define OppXrayTable Position->XrayB
#define MyKingCheck Position->Dyn->wKcheck
#define MyCount CountWhite
#define CountEvasions count_white_evasions
#define InCheck WhiteInCheck
#define OppKdanger bKdanger
#define EighthRank(x) (x >= A8)
#define CaptureRight ((wBitboardP & ~FileH) << 9)
#define FromLeft(x) (((x) - 9) << 6)
#define CaptureLeft ((wBitboardP & ~FileA) << 7)
#define FromRight(x) (((x) - 7) << 6)
#define Forward(x) ((x) + 8)
#define ForwardLeft(x) ((x) + 7)
#define ForwardRight(x) ((x) + 9)
#define Backward(x) ((x) - 8)
#define ForwardShift(x) ((x) << 8)
#define BackShift(x) ((x) >> 8)
#define SeventhRank(x) (x >= A7)
#define BackShift2(x) ((x) >> 16)
#define Forward2(x) ((x) + 16)
#define SecondRank Rank2
#define OnThirdRank(x) ((x & 070) == 020)
#define SecondSixthRanks Ranks2to6
#define BitBoardSeventhRank Rank7
#define BitBoardEighthRank Rank8
#define WhiteA7 A7
#define WhiteH7 H7
#define CaptureEP CaptureValue[wEnumP][bEnumP]
#define PromQueenCap ((0x28 << 24) + CaptureValue[wEnumP][c])
#define PromKnightCap ((0x1a << 24) + CaptureValue[wEnumP][c])
#define CastleOO WhiteOO
#define CastleOOO WhiteOOO
#define WhiteF1G1 F1G1
#define WhiteC1D1 C1D1
#define WhiteB1C1D1 B1C1D1
#define WhiteE1 E1
#define WhiteC1 C1
#define WhiteG1 G1
#define UnderProm UnderPromWhite
#define OnSecondRank(x) ((x & 070) == 010)
#define CanCaptureRight (((SqSet[fr] & NotH) << 9) & bBitboardOcc & mask)
#define CanCaptureLeft (((SqSet[fr] & NotA) << 7) & bBitboardOcc & mask)
#define PawnGuard(x,y) \
 (Position->Dyn->wAtt & SqSet[x] || AttFile((y)) & (wBitboardR|wBitboardQ))
#define FourthEighthRankNoH (0x7f7f7f7f7f000000)
#define FourthEighthRankNoA (0xfefefefefe000000)
#define BackRight(x) ((x) - 7)
#define BackRight2(x) ((x) - 15)
#define BackRight3(x) ((x) - 23)
#define BackLeft(x) ((x) - 9)
#define BackLeft2(x) ((x) - 17)
#define BackLeft3(x) ((x) - 25)
#define NumberRank5 R5
#define NumberRank2 R2
#define NumberRank7 R7
#define PieceIsMine PieceIsWhite
#define PieceIsOpp PieceIsBlack
#define Backward2(x) ((x) - 16)
#define PieceIsOppPawn(x) ((x == bEnumP) ? 0xffffffffffffffff : 0)
