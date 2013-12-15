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

#define MyPVNodeSMP BlackPVNodeSMP
#define OppPVNodeSMP WhitePVNodeSMP
#define MyAllSMP BlackAllSMP
#define OppAllSMP WhiteAllSMP
#define MyCutSMP BlackCutSMP
#define OppCutSMP WhiteCutSMP
#define MyMultiPV BlackMultiPV
#define MyPVQsearch PVQsearchBlack
#define MyPVQsearchCheck PVQsearchBlackCheck
#define MyQsearchCheck QsearchBlackCheck
#define MyQsearch QsearchBlack
#define MyLowDepth LowDepthBlack
#define MyLowDepthCheck LowDepthBlackCheck
#define MyExclude ExcludeBlack
#define MyExcludeCheck ExcludeBlackCheck
#define MyCut CutNodeBlack
#define MyCutCheck CutNodeBlackCheck
#define MyAll AllNodeBlack
#define MyAllCheck AllNodeBlackCheck
#define MyPV PVNodeBlack
#define MyRootNode RootBlack
#define MyAnalysis BlackAnalysis
#define MyTop TopBlack
#define MyTopAnalysis BlackTopAnalysis
#define MyAttacked Position->Dyn->bAtt
#define OppAttacked Position->Dyn->wAtt
#define MyAttackedPawns AttPb
#define OppAttackedPawns AttPw
#define OppPVQsearch PVQsearchWhite
#define OppPVQsearchCheck PVQsearchWhiteCheck
#define OppQsearch QsearchWhite
#define OppQsearchCheck QsearchWhiteCheck
#define OppLowDepth LowDepthWhite
#define OppLowDepthCheck LowDepthWhiteCheck
#define OppExclude ExcludeWhite
#define OppExcludeCheck ExcludeWhiteCheck
#define OppCut CutNodeWhite
#define OppCutCheck CutNodeWhiteCheck
#define OppAll AllNodeWhite
#define OppAllCheck AllNodeWhiteCheck
#define OppPV PVNodeWhite
#define OppRootNode RootWhite
#define OppAnalysis WhiteAnalysis
#define OppTop TopWhite
#define OppTopAnalysis WhiteTopAnalysis
#define OppOccupied wBitboardOcc
#define BitboardOppP wBitboardP
#define BitboardOppN wBitboardN
#define BitboardOppBL wBitboardBL
#define BitboardOppBD wBitboardBD
#define BitboardOppB (BitboardOppBL | BitboardOppBD)
#define BitboardOppR wBitboardR
#define BitboardOppQ wBitboardQ
#define BitboardOppK wBitboardK
#define MyOccupied bBitboardOcc
#define BitboardMyP bBitboardP
#define BitboardMyN bBitboardN
#define BitboardMyBL bBitboardBL
#define BitboardMyBD bBitboardBD
#define BitboardMyB (BitboardMyBL | BitboardMyBD)
#define BitboardMyR bBitboardR
#define BitboardMyQ bBitboardQ
#define BitboardMyK bBitboardK
#define MyKingSq Position->bKsq
#define OppKingSq Position->wKsq
#define EnumOppP wEnumP
#define EnumOppN wEnumN
#define EnumOppBL wEnumBL
#define EnumOppBD wEnumBD
#define EnumOppR wEnumR
#define EnumOppQ wEnumQ
#define EnumOppK wEnumK
#define EnumMyP bEnumP
#define EnumMyN bEnumN
#define EnumMyBL bEnumBL
#define EnumMyBD bEnumBD
#define EnumMyR bEnumR
#define EnumMyQ bEnumQ
#define EnumMyK bEnumK
#define OppKingCheck Position->Dyn->wKcheck
#define MyCapture BlackCaptures
#define MyOrdinary BlackOrdinary
#define MyPositionalGain PositionalMovesBlack
#define MyEvasion BlackEvasions
#define MyQuietChecks QuietChecksBlack
#define MySEE BlackSEE
#define Make MakeBlack
#define Undo UndoBlack
#define IllegalMove MoveIsCheckBlack
#define MoveIsCheck MoveIsCheckWhite
#define MyOK BlackOK
#define MyNext NextBlack
#define MyNull ((Position->Dyn->flags) & 1)
#define HasPiece (bBitboardOcc ^ bBitboardK ^ bBitboardP)
#define SixthRank(x) ((x) <= H3)
#define FifthRank(x) ((x) <= H4)
#define FourthRank(x) ((x) <= H5)
#define IsPassedPawn PassedPawnB
#define MyXray (Position->Dyn->bXray)
#define OppXray (Position->Dyn->wXray)
#define MyXrayTable Position->XrayB
#define OppXrayTable Position->XrayW
#define MyKingCheck Position->Dyn->bKcheck
#define MyCount CountBlack
#define CountEvasions count_black_evasions
#define InCheck BlackInCheck
#define OppKdanger wKdanger
#define EighthRank(x) (x <= H1)
#define CaptureRight ((bBitboardP & ~FileH) >> 7)
#define FromLeft(x) (((x) + 7) << 6)
#define CaptureLeft ((bBitboardP & ~FileA) >> 9)
#define FromRight(x) (((x) + 9) << 6)
#define Forward(x) ((x) - 8)
#define ForwardLeft(x) ((x) - 9)
#define ForwardRight(x) ((x) - 7)
#define Backward(x) ((x) + 8)
#define ForwardShift(x) ((x) >> 8)
#define BackShift(x) ((x) << 8)
#define SeventhRank(x) (x <= H2)
#define BackShift2(x) ((x) << 16)
#define Forward2(x) ((x) - 16)
#define SecondRank Rank7
#define OnThirdRank(x) ((x & 070) == 050)
#define FourthRankBitBoard Rank5
#define SecondSixthRanks Ranks3to7
#define BitBoardSeventhRank Rank2
#define BitBoardEighthRank Rank1
#define WhiteA7 A2
#define WhiteH7 H2
#define CaptureEP CaptureValue[bEnumP][wEnumP]
#define PromQueenCap ((0x28 << 24) + CaptureValue[bEnumP][c])
#define PromKnightCap ((0x1a << 24) + CaptureValue[bEnumP][c])
#define CastleOO BlackOO
#define CastleOOO BlackOOO
#define WhiteF1G1 F8G8
#define WhiteC1D1 C8D8
#define WhiteB1C1D1 B8C8D8
#define WhiteE1 E8
#define WhiteC1 C8
#define WhiteG1 G8
#define UnderProm UnderPromBlack
#define OnSecondRank(x) ((x & 070) == 060)
#define CanCaptureRight (((SqSet[fr] & NotH) >> 7) & wBitboardOcc & mask)
#define CanCaptureLeft (((SqSet[fr] & NotA) >> 9) & wBitboardOcc & mask)
#define PawnGuard(x,y) \
 (Position->Dyn->bAtt & SqSet[x] || AttFile((y)) & (bBitboardR | bBitboardQ))
#define FourthEighthRankNoH (0x0000007f7f7f7f7f)
#define FourthEighthRankNoA (0x000000fefefefefe)
#define BackRight(x) ((x) + 9)
#define BackRight2(x) ((x) + 17)
#define BackRight3(x) ((x) + 25)
#define BackLeft(x) ((x) + 7)
#define BackLeft2(x) ((x) + 15)
#define BackLeft3(x) ((x) + 23)
#define NumberRank5 R4
#define NumberRank1 R8
#define NumberRank2 R7
#define NumberRank7 R2
#define PieceIsMine PieceIsBlack
#define PieceIsOpp PieceIsWhite
#define Backward2(x) ((x) + 16)
#define PieceIsOppPawn(x) ((x == wEnumP) ? 0xffffffffffffffff : 0)
