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

typedef struct
    {
    uint64 volatile PawnHash;
    uint8 wPfile_count, bPfile_count, OpenFileCount;
    bool locked;
    uint32 wKdanger, bKdanger;
    uint8 wPlight, wPdark, bPlight, bPdark, wPassedFiles, bPassedFiles, wDrawWeight, bDrawWeight;
    uint32 Score;
    } typePawnEval;
typePawnEval *PawnHash;
#define CheckHalt() { if (Position->stop) { return(0); } }
#define FlagLower 1
#define FlagUpper 2
#define FlagCut 4
#define FlagAll 8
#define FlagExact 16
#define FlagMoveLess 32
#define UpperFromPV 64
#define LowerFromPV 128
#define IsCut(rank) ((rank->flags) & FlagCut)
#define IsAll(rank) ((rank->flags) & FlagAll)
#define IsExact(rank) ((rank)->flags & FlagExact)
typedef struct
    {
    uint32 hash;
    uint8 flags, age, DepthUpper, DepthLower;
    sint16 ValueLower, ValueUpper;
    uint16 move;
    uint8 reversibles, sticky;
    } typeHash;
typeHash *HashTable;
uint64 HashMask, GlobalAge;
#define HashPointer(x) (HashTable + ((x) & HashMask))
#define HashPointer0(x) (HashTable + ((x) & (3 + HashMask)))
#define VAS(v) ValueAdjustStore ((v), (Height (Position)))
#define AgePass (Position->Dyn->age)
#define HashLowerAll(Pos, mv, dp, v)  \
	HashLowerAllNew (Pos->Dyn->Hash, mv, dp, v, Height (Pos), AgePass)
#define HashUpperCut(Pos, dp, v)      \
	HashUpperCutNew (Pos->Dyn->Hash, dp, v, Height (Pos), AgePass)
#define HashLower(Z, mv, dp, v)       \
	HashLowerNew (Z, mv, dp, v, 0, Height (Position), AgePass)
#define HashUpper(Z, dp, v)           \
	HashUpperNew (Z, dp, v, 0, Height (Position), AgePass)
#define HashLowerPV(Z, mv, dp, v)     \
	HashLowerNew (Z, mv, dp, v, LowerFromPV, Height (Position), AgePass)
#define HashUpperPV(Z, dp, v)         \
	HashUpperNew (Z, dp, v, UpperFromPV, Height (Position), AgePass)
#define HashExact(Pos, mv, dp, v, FL) \
	HashExactNew (Pos->Dyn->Hash, mv, dp, v, FL, Height (Position), AgePass)
#define ValueAdjustStore(Value, height) (Value)
#define HashUpperValue(trans) (trans->ValueUpper)
#define HashLowerValue(trans) (trans->ValueLower)
#define HashUpperBound(trans) (trans->ValueUpper)
#define HashLowerBound(trans) (trans->ValueLower)
#define Trans trans
#define Trans_pv trans_pv
#define TransDeclare() typeHash *trans
#define TransPVDeclare() typePVHash *trans_pv
#define UpdateAge() trans->age = (AgePass);
typedef struct
    {
    uint64 hash;
    sint32 Value;
    uint16 move;
    uint8 depth, age;
    } typePVHash;
typePVHash *PVHashTable;
uint64 PVHashMask;
sint64 EvalHashSize;
uint64 EvalHashMask;
uint64 *EvalHash;
int CurrentHashSize;
int CurrentPHashSize;

#ifdef RobboBases
sint64 TripleHashSize;
uint64 TripleHashMask;
uint64 *TripleHash;
#endif

