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

#define InCheck  (Position->wtm ?             \
	(Position->Dyn->bAtt & wBitboardK) : \
	(Position->Dyn->wAtt & bBitboardK))

static const char StartPosition[80] = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

char *EmitFen(typePos *Position, char *ARR)
    {
    int r, f, e = 0;
    int n = 0;
    char PieceChar[32] = "0PNKBBRQ0pnkbbrq";
    for (r = R8; r >= R1; r--)
        {
        for (f = FA; f <= FH; f++)
            {
            if (!Position->sq[(r << 3) + f])
                e++;
            else
                {
                if (e)
                    ARR[n++] = '0' + e;
                e = 0;
                ARR[n++] = PieceChar[Position->sq[(r << 3) + f]];
                }
            }
        if (e)
            ARR[n++] = '0' + e;
        e = 0;
        if (r != R1)
            ARR[n++] = '/';
        }
    ARR[n++] = ' ';
    ARR[n++] = (Position->wtm) ? 'w' : 'b';
    ARR[n++] = ' ';
    if (WhiteOO)
        ARR[n++] = 'K';
    if (WhiteOOO)
        ARR[n++] = 'Q';
    if (BlackOO)
        ARR[n++] = 'k';
    if (BlackOOO)
        ARR[n++] = 'q';
    if (!Position->Dyn->oo)
        ARR[n++] = '-';
    ARR[n++] = ' ';
    if (!Position->Dyn->ep)
        ARR[n++] = '-';
    else
        {
        ARR[n++] = (Position->Dyn->ep & 7) + 'a';
        ARR[n++] = (Position->Dyn->ep >> 3) + '1';
        }
    ARR[n++] = ' ';
    if (Position->Dyn->reversible >= 100)
        ARR[n++] = (Position->Dyn->reversible / 100) + '0';
    if (Position->Dyn->reversible >= 10)
        ARR[n++] = ((Position->Dyn->reversible / 10) % 10) + '0';
    ARR[n++] = (Position->Dyn->reversible % 10) + '0';
    ARR[n++] = ' ';
    ARR[n++] = '0';
    ARR[n++] = 0;
    return ARR;
    }
static void ParseFen(typePos *Position, char *I)
    {
    int rank = 7, file = 0, c = 0, i, p;
    for (i = A1; i <= H8; i++)
        Position->sq[i] = 0;
    while (1)
        {
        if (rank < 0 || file > 8)
            ErrorFen("FEN %s file: %d rank: %d\n", I, file, rank);
        p = I[c++];
        if (p == 0)
            return;
        if (file == 8 && p != '/')
            ErrorFen("FEN %s file: %d rank: %d piece: %d\n", I, file, rank, p);
        switch (p)
            {
            case '/':
                rank--;
                file = 0;
                break;
            case 'p':
                Position->sq[file + (rank << 3)] = bEnumP;
                file++;
                break;
            case 'b':
                if (SqSet[file + (rank << 3)] & Black)
                    Position->sq[file + (rank << 3)] = bEnumBD;
                else
                    Position->sq[file + (rank << 3)] = bEnumBL;
                file++;
                break;
            case 'n':
                Position->sq[file + (rank << 3)] = bEnumN;
                file++;
                break;
            case 'r':
                Position->sq[file + (rank << 3)] = bEnumR;
                file++;
                break;
            case 'q':
                Position->sq[file + (rank << 3)] = bEnumQ;
                file++;
                break;
            case 'k':
                Position->sq[file + (rank << 3)] = bEnumK;
                file++;
                break;
            case 'P':
                Position->sq[file + (rank << 3)] = wEnumP;
                file++;
                break;
            case 'B':
                if (SqSet[file + (rank << 3)] & Black)
                    Position->sq[file + (rank << 3)] = wEnumBD;
                else
                    Position->sq[file + (rank << 3)] = wEnumBL;
                file++;
                break;
            case 'N':
                Position->sq[file + (rank << 3)] = wEnumN;
                file++;
                break;
            case 'R':
                Position->sq[file + (rank << 3)] = wEnumR;
                file++;
                break;
            case 'Q':
                Position->sq[file + (rank << 3)] = wEnumQ;
                file++;
                break;
            case 'K':
                Position->sq[file + (rank << 3)] = wEnumK;
                file++;
                break;
            case '1':
                file += 1;
                break;
            case '2':
                file += 2;
                break;
            case '3':
                file += 3;
                break;
            case '4':
                file += 4;
                break;
            case '5':
                file += 5;
                break;
            case '6':
                file += 6;
                break;
            case '7':
                file += 7;
                break;
            case '8':
                file += 8;
                break;
            default:
                ErrorFen("FEN %s file:%d rank:%d piece:%d\n", I, file, rank, p);
            }
        if ((rank == 0) && (file >= 8))
            break;
        }
    }
static char *ReadFEN(typePos *Position, char *I)
    {
    char i[1024];
    bool ok;
    int ep;
    sscanf(I, "%s", i);
    ParseFen(Position, i);
    memset(Position->DynRoot, 0, (sizeof(typeDynamic) << 8));
    Position->Dyn = Position->DynRoot;
    I += strlen(i) + 1;
    sscanf(I, "%s", i);
    if (i[0] == 'w')
        Position->wtm = true;
    else if (i[0] == 'b')
        Position->wtm = false;
    else
        ErrorFen("FEN wb %s\n", i);
    I += strlen(i) + 1;
    sscanf(I, "%s", i);
    Position->Dyn->oo = 16;
    if (!strcmp(i, "KQkq"))
        Position->Dyn->oo = 15;
    if (!strcmp(i, "Qkq"))
        Position->Dyn->oo = 14;
    if (!strcmp(i, "Kkq"))
        Position->Dyn->oo = 13;
    if (!strcmp(i, "kq"))
        Position->Dyn->oo = 12;
    if (!strcmp(i, "KQq"))
        Position->Dyn->oo = 11;
    if (!strcmp(i, "Qq"))
        Position->Dyn->oo = 10;
    if (!strcmp(i, "Kq"))
        Position->Dyn->oo = 9;
    if (!strcmp(i, "q"))
        Position->Dyn->oo = 8;
    if (!strcmp(i, "KQk"))
        Position->Dyn->oo = 7;
    if (!strcmp(i, "Qk"))
        Position->Dyn->oo = 6;
    if (!strcmp(i, "Kk"))
        Position->Dyn->oo = 5;
    if (!strcmp(i, "k"))
        Position->Dyn->oo = 4;
    if (!strcmp(i, "KQ"))
        Position->Dyn->oo = 3;
    if (!strcmp(i, "Q"))
        Position->Dyn->oo = 2;
    if (!strcmp(i, "K"))
        Position->Dyn->oo = 1;
    if (!strcmp(i, "-"))
        Position->Dyn->oo = 0;
    if (Position->Dyn->oo == 16)
        ErrorFen("FEN oo %s\n", i);
    I += strlen(i) + 1;
    sscanf(I, "%s", i);
    Position->Dyn->ep = 0;
    if (!strcmp(i, "-"))
        ep = 0;
    else
        {
        ep = (i[0] - 'a') + ((i[1] - '1') << 3);
        if (ep > H8)
            ErrorFen("FEN ep %s\n", i);
        ok = 0;
        }
    if (ep)
        {
        if (Position->wtm)
            {
            if (File(ep) != FA && (Position->sq[ep - 9] == wEnumP))
                ok = true;
            if (File(ep) != FH && (Position->sq[ep - 7] == wEnumP))
                ok = true;
            }
        else
            {
            if (File(ep) != FA && (Position->sq[ep + 7] == bEnumP))
                ok = true;
            if (File(ep) != FH && (Position->sq[ep + 9] == bEnumP))
                ok = true;
            }
        if (ok)
            Position->Dyn->ep = ep;
        }
    I += strlen(i) + 1;
    sscanf(I, "%s", i);
    Position->Dyn->reversible = (uint8)atoi(i);
    I += strlen(i) + 1;
    sscanf(I, "%s", i);
    I += strlen(i) + 1;
    InitBitboards(Position);
    return I;
    }

static void ReadMoves(typePos *Position, char *I)
    {
    typeMoveList List[256], *list;
    char T[256];
    int i;
    uint32 full;
    while (I[0])
        {
        Mobility(Position);
        if (InCheck)
            {
            list = EvasionMoves(Position, List, 0xffffffffffffffff);
            list++;
            }
        else
            {
            list = CaptureMoves(Position, List, Position->OccupiedBW);
            list = OrdinaryMoves(Position, list);
            }
        full = FullMove(Position, (I[2] - 'a') + ((I[3] - '1') << 3) + ((I[0] - 'a') << 6) + ((I[1] - '1') << 9));
        sscanf(I, "%s", T);
        if (strlen(T) == 5)
            {
            if (I[4] == 'b')
                full |= FlagPromB;
            if (I[4] == 'n')
                full |= FlagPromN;
            if (I[4] == 'r')
                full |= FlagPromR;
            if (I[4] == 'q')
                full |= FlagPromQ;
            }
        for (i = 0; i < list - List; i++)
            {
            if (full == (List[i].move & 0x7fff))
                {
                Make(Position, full);
                Position->StackHeight = 0;
                break;
                }
            }
        if (i == list - List)
            {
            ErrorEnd("moves? %s\n", T);
            }
        I += strlen(T);
        while (I[0] == ' ' || I[0] == '\t')
            I++;
        }
    }
void InitPosition(typePos *Position, char *I)
    {
    char i[1024], *II, *J;
    NodeCheck = 0;
    II = malloc(strlen(I) + 256);
    strcpy(II, I);
    J = strstr(II, "moves");
    sscanf(I, "%s", i);
    if (!strcmp(i, "startpos"))
        {
        ReadFEN(Position, StartPosition);
        I += strlen("startpos") + 1;
        }
    if (!strcmp(i, "fen"))
        {
        I += strlen("fen") + 1;
        I = ReadFEN(Position, I);
        }
    Position->StackHeight = 0;
    if (J && I[0] == 'm')
        {
        sscanf(I, "%s", i);
        if (!strcmp(i, "moves"))
            {
            I += strlen("moves");
            while (I[0] == ' ' || I[0] == '\t')
                I++;
            ReadMoves(Position, I);
            }
        }
    else
        {
        if (J)
            {
            J += strlen("moves");
            while (J[0] == ' ')
                J++;
            ReadMoves(Position, J);
            }
        }
    free(II);
    Position->height = 0;
    if (isNewGame)
        ResetPositionalGain();
    Mobility(Position);
    }
