/*******************************************************************************
Firenzina is a UCI chess playing engine by Yuri Censor (Dmitri Gusev).
Rededication: To the memories of Giovanna Tornabuoni and Domenico Ghirlandaio.
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

char BenchmarkPos[16][128] =
    {
	"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
	"rnbqkb1r/pppp1ppp/4pn2/8/2PP4/2N5/PP2PPPP/R1BQKBNR b KQkq - 1 3",
	"rnbqkb1r/pp2pppp/1n1p4/8/2PP4/8/PP3PPP/RNBQKBNR w KQkq - 0 6",
	"r1bq1rk1/pppp1ppp/5n2/4n3/2P5/1PN3P1/P2PPKBP/R1BQ3R b - - 0 8",
	"r4rk1/ppqbbppp/2nppn2/8/4PP2/1NN1B3/PPP1B1PP/R2Q1RK1 w - - 1 11",
	"r2q1rk1/1pp2ppp/p1pbb3/4P3/4NB2/8/PPP2PPP/R2QR1K1 b - - 0 13",
	"r4rk1/3np1bp/pq1p2p1/2pP3n/6P1/2N1Bp2/PPQ1BPP1/R4RK1 w - - 0 16",
	"rnb2r2/p1p2pk1/1p1pqn1p/P7/Q1PPp1pP/2P3P1/4PPB1/1RB1K1NR b K - 3 18",
	"4rk1r/1b2pNbp/pq2Bn1p/1ppP4/P1p2Q2/2N4P/1P3PP1/R3K2R w KQ - 8 21",
	"r3r1k1/2p1np1p/1p2p1pB/p1q1P3/P1P1Q3/3R3P/1P3PP1/5RK1 b - - 2 23",
	"r2q1r1k/6np/1p1p1pp1/pNpPn3/P1P1P1P1/1PB1Q2P/5R2/5R1K w - - 0 26",
	"2kn4/ppN1R3/3p4/6rp/2NP3n/2P5/PP5r/4KR2 b - - 4 28",
	"r1br2k1/pp4p1/4p1Bp/4P3/2Rp3N/4n1P1/PP2P2P/R5K1 w - - 8 31",
	"6k1/1p3pp1/p2p4/3P1P2/P1Bpn3/1P2q3/2P4P/5Q1K b - - 0 33",
	"4q3/r4pkp/1p1P4/2n1P1p1/2Q2b2/7P/2R1B1P1/5R1K w - - 0 36",
	"3rr1k1/p4pbp/2bN1p2/8/2B3P1/2P3Bn/P2N4/3R1K2 b - - 1 38"
    };
void BenchMark(typePos *Position, char *GoString)
    {
    int i;
    char String[1024], Go_String[1024];
    int cpu, rp;
    uint64 Nodes, C, TIME;
    uint64 TotalTime = 0, TotalNodes = 0;
    for (i = 0; i < 16; i++)
        {
        sprintf(String, "%s %s\n", "fen", BenchmarkPos[i]);
        InitPosition(Position, String);
        strcpy(Go_String, GoString);
        InitSearch(Position, Go_String);
        C = GetClock();
        Search(Position);
        TIME = GetClock() - C;
        Nodes = 0;
            for (cpu = 0; cpu < NumThreads; cpu++)
                for (rp = 0; rp < RPperCPU; rp++)
                    Nodes += RootPosition[cpu][rp].nodes;
        Send("Position %d: Nodes: %lld Time: %lldms\n", 1 + i, Nodes, TIME / 1000);

#ifdef Log
	if (WriteLog)
		{
		log_file = fopen(log_filename, "a");
		fprintf(log_file, "Position %d: Nodes: %lld Time: %lldms\n", 1 + i, Nodes, TIME / 1000);
		close_log();
		}
#endif

        TotalNodes += Nodes;
        TotalTime += TIME;
        }
    Send("Total Nodes: %lld Time: %lldms\n", TotalNodes, TotalTime / 1000);
    Send("Total NPS: %lld\n", ((TotalNodes * 1000) / TotalTime) * 1000);

#ifdef Log
	if (WriteLog)
		{
		log_file = fopen(log_filename, "a");
		fprintf(log_file, "Total Nodes: %lld Time: %lldms\n", TotalNodes, TotalTime / 1000);
		fprintf(log_file, "Total NPS: %lld\n", ((TotalNodes * 1000) / TotalTime) * 1000);
		close_log();
		}
#endif
    }
