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

#include "fire.h"
#include <time.h>

#ifdef Bench
#define numPos 32
#define numCycles 4

static uint64 TotalTime = 0;
static uint64 TotalNodes = 0;
static FILE *LogFile;
static char FileName[256];

static int BestANSD = 0;
static int BestANNPS = 0;
static int BestCNSD = 0;
static int BestCNNPS = 0;
static int BestPVSD = 0;
static int BestPVNPS = 0;

static void CreateBenchLog();
static void CreateSDBenchLog();

char BenchmarkPos[numPos][128] =
	{
	"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq -",
	"r1bn1rk1/ppp1qppp/3pp3/3P4/2P1n3/2B2NP1/PP2PPBP/2RQK2R w K -",
	"r2q1rk1/1bppbppp/p4n2/n2Np3/Pp2P3/1B1P1N2/1PP2PPP/R1BQ1RK1 w - -",
	"rnb2rk1/1pq1bppp/p3pn2/3p4/3NPP2/2N1B3/PPP1B1PP/R3QRK1 w - -",
	"2rq1rk1/p3bppp/bpn1pn2/2pp4/3P4/1P2PNP1/PBPN1PBP/R2QR1K1 w - -",
	"rn3rk1/1p2ppbp/1pp3p1/3n4/3P1Bb1/2N1PN2/PP3PPP/2R1KB1R w K -",
	"r1bq1rk1/3nbppp/p1p1pn2/1p4B1/3P4/2NBPN2/PP3PPP/2RQK2R w K -",
	"r3kbnr/1bpq2pp/p2p1p2/1p2p3/3PP2N/1PN5/1PP2PPP/R1BQ1RK1 w kq -",
	"r1b1k2r/pp1nqp1p/2p3p1/3p3n/3P4/2NBP3/PPQ2PPP/2KR2NR w kq -",
	"r2q1rk1/1b2ppbp/ppnp1np1/2p5/P3P3/2PP1NP1/1P1N1PBP/R1BQR1K1 w - -",
	"r2q1rk1/pp2ppbp/2n1bnp1/3p4/4PPP1/1NN1B3/PPP1B2P/R2QK2R w KQ -",
	"2q1r1k1/1ppb4/r2p1Pp1/p4n1p/2P1n3/5NPP/PP3Q1K/2BRRB2 w - -",
	"7r/1p2k3/2bpp3/p3np2/P1PR4/2N2PP1/1P4K1/3B4 b - -",
	"4k3/p1P3p1/2q1np1p/3N4/8/1Q3PP1/6KP/8 w - -",
	"2r1b1k1/R4pp1/4pb1p/1pBr4/1Pq2P2/3N4/2PQ2PP/5RK1 b - -",
	"6k1/p1qb1p1p/1p3np1/2b2p2/2B5/2P3N1/PP2QPPP/4N1K1 b - -",
	"3q4/pp3pkp/5npN/2bpr1B1/4r3/2P2Q2/PP3PPP/R4RK1 w - -",
	"3rr1k1/pb3pp1/1p1q1b1p/1P2NQ2/3P4/P1NB4/3K1P1P/2R3R1 w - -",
	"r1b1r1k1/p1p3pp/2p2n2/2bp4/5P2/3BBQPq/PPPK3P/R4N1R b - -",
	"3r4/1b2k3/1pq1pp2/p3n1pr/2P5/5PPN/PP1N1QP1/R2R2K1 b - -",
	"2r4k/pB4bp/6p1/6q1/1P1n4/2N5/P4PPP/2R1Q1K1 b - -",
	"1N2k3/5p2/p2P2p1/3Pp3/pP3b2/5P1r/P7/1K4R1 b - - 0 1",
	"2k2R2/6r1/8/B2pp2p/1p6/3P4/PP2b3/2K5 b - - 0 1",
	"2k5/1pp5/2pb2p1/7p/6n1/P5N1/1PP3PP/2K1B3 b - - 0 1",
	"2n5/1k6/3pNn2/3ppp2/7p/4P2P/1P4P1/5NK1 w - - 0 1",
	"5nk1/B4p2/7p/6p1/3N3n/2r2PK1/5P1P/4R3 b - - 0 1",
	"8/1p3pkp/p1r3p1/3P3n/3p1P2/3P4/PP3KP1/R3N3 b - - 0 1",
	"8/2B2k2/p2p2pp/2pP1p2/2P2P2/2b1N1PP/P4K2/2n5 b - - 0 1",
	"8/4p1kp/1n1p2p1/nPp5/b5P1/P5KP/3N1P2/4NB2 w - - 0 1",
	"r1b3k1/2p4p/3p1p2/1p1P4/1P3P2/P5P1/5KNP/R7 b - - 0 1",
	"1k2b3/1pp5/4r3/R3N1pp/1P3P2/p5P1/2P4P/1K6 w - - 0 1",
	"8/3k4/3p4/8/8/3P4/3K4/8 w - - 0 1",
	};
void BenchMark(typePos* Position, char* GoString)
	{
	int i, j;
	char String[1024], GO_String[1024];
	int cpu, rp;
	uint64 Nodes, C, Time;
	HashClearAll();
	BenchMarking = true;
	Send("%s\n\n", "benchmarking, please wait...");
	for (j = 0; j < numCycles; j++)
		{
	    for (i = 0; i < numPos; i++)
			{
	        Send("position %d\n", i + 1);
			sprintf(String, "%s %s\n", "fen", BenchmarkPos[i]);
			InitPosition(Position, String);
			strcpy(GO_String, GoString);
			InitSearch(Position, GO_String);
			C = GetClock();
			Search(Position);
			Time = GetClock() - C;
			Nodes = 0;
			for (cpu = 0; cpu < NumThreads; cpu++)
				for (rp = 0; rp < RPperCPU; rp++)
				Nodes += RootPosition[cpu][rp].nodes;
			Send("nodes: %lld\n", Nodes);			
	        Send("time : %lld ms\n", Time / 1000);
			Send("NPS  : %lld kNPS\n\n", ((Nodes * 1000) / Time));
	#ifdef Log
		if (WriteLog)
			{
			log_file = fopen(log_filename, "a");
			fprintf(log_file, "Position %d: Nodes: %lld Time: %lldms\n", 1 + i, Nodes, Time / 1000);
			close_log();
			}
	#endif

	        TotalNodes += Nodes;
			TotalTime += Time;
	        }
		HashClearAll();
		}
    Send("total nodes: %lld\n", TotalNodes);
    Send("total time : %lld ms\n", TotalTime / 1000);
    Send("total NPS  : %lld kNPS\n\n", ((TotalNodes * 1000) / TotalTime));
	BenchMarking = false;
	HashClearAll();
	InitPosition(Position, "startpos");
	CreateBenchLog();

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

void SDBenchMark(typePos* Position, char *GoString)
    {
    int i, j;
    char String[1024], GO_String[1024];
    int cpu, rp;
    uint64 Nodes = 0, C, Time = 0;
    uint64 TotalTime = 0, TotalNodes = 0, ThisNPS = 0;
	HashClearAll();
	BenchMarking = true;
	CreateSDBenchLog();
	Send("benchmarking AN Split depths, please wait...\n");
    for (i = MIN_AN_SPLIT_DEPTH; i <= MAX_AN_SPLIT_DEPTH; i += 2)
        {
		ANSplitDepth = i;
		for (j = 0; j < numPos; j++)
			{
			sprintf(String, "%s %s\n", "fen", BenchmarkPos[j]);
			InitPosition(Position, String);
			strcpy(GO_String, GoString);
			InitSearch(Position, GO_String);
			C = GetClock();
			Search(Position);
			Time = GetClock() - C;
			Nodes = 0;
			for (cpu = 0; cpu < NumThreads; cpu++)
				for (rp = 0; rp < RPperCPU; rp++)
					Nodes += RootPosition[cpu][rp].nodes;
			TotalNodes += Nodes;
			TotalTime += Time;
			}
		Send("AN Split depth : %d\n", i);
		Send("average kNPS: %lld\n\n", ((TotalNodes * 1000) / TotalTime));

		LogFile = fopen(FileName, "a");
		fprintf(LogFile, "AN split depth  : %d\n", i);
		fprintf(LogFile, "average kNPS : %lld\n\n", ((TotalNodes * 1000) / TotalTime));
		fclose(LogFile);

		ThisNPS = ((TotalNodes * 1000) / TotalTime);
		if (ThisNPS > BestANNPS)
			{
			BestANNPS = ThisNPS;
			BestANSD = i;
			}
		}
	ANSplitDepth = BestANSD;
    Send("fastest AN Split depth: %d\n\n", BestANSD);

	LogFile = fopen(FileName, "a");
	fprintf(LogFile, "fastest AN Split depth: %d\n", BestANSD);
	fprintf(LogFile, "----------------------\n\n");

	fclose(LogFile);

	Send("benchmarking CN Split depths, please wait...\n\n");
    for (i = MIN_CN_SPLIT_DEPTH; i <= MAX_CN_SPLIT_DEPTH; i += 2)
        {
		CNSplitDepth = i;
		for (j = 0; j < numPos; j++)
			{
			sprintf(String, "%s %s\n", "fen", BenchmarkPos[j]);
			InitPosition(Position, String);
			strcpy(GO_String, GoString);
			InitSearch(Position, GO_String);
			C = GetClock();
			Search(Position);
			Time = GetClock() - C;
			Nodes = 0;
			for (cpu = 0; cpu < NumThreads; cpu++)
				for (rp = 0; rp < RPperCPU; rp++)
					Nodes += RootPosition[cpu][rp].nodes;
			TotalNodes += Nodes;
			TotalTime += Time;
			}
		Send("CN Split depth : %d\n", i);
		Send("average kNPS: %lld\n\n", ((TotalNodes * 1000) / TotalTime));

		LogFile = fopen(FileName, "a");
		fprintf(LogFile, "CN split depth  : %d\n", i);
		fprintf(LogFile, "average kNPS : %lld\n\n", ((TotalNodes * 1000) / TotalTime));
		fclose(LogFile);

		ThisNPS = ((TotalNodes * 1000) / TotalTime);
		if (ThisNPS > BestCNNPS)
			{
			BestCNNPS = ThisNPS;
			BestCNSD = i;
			}
		}
	CNSplitDepth = BestCNSD;
    Send("fastest CN Split depth: %d\n\n", BestCNSD);

	LogFile = fopen(FileName, "a");
	fprintf(LogFile, "fastest CN Split depth: %d\n", BestCNSD);
	fprintf(LogFile, "----------------------\n\n");
	fclose(LogFile);

	Send("benchmarking PV Split depths, please wait...\n\n");
    for (i = MIN_PV_SPLIT_DEPTH; i <= MAX_PV_SPLIT_DEPTH; i += 2)
        {
		PVSplitDepth = i;
		for (j = 0; j < numPos; j++)
			{
			sprintf(String, "%s %s\n", "fen", BenchmarkPos[j]);
			InitPosition(Position, String);
			strcpy(GO_String, GoString);
			InitSearch(Position, GO_String);
			C = GetClock();
			Search(Position);
			Time = GetClock() - C;
			Nodes = 0;
			for (cpu = 0; cpu < NumThreads; cpu++)
				for (rp = 0; rp < RPperCPU; rp++)
					Nodes += RootPosition[cpu][rp].nodes;
			TotalNodes += Nodes;
			TotalTime += Time;
			}
		Send("PV Split depth : %d\n", i);
		Send("average kNPS: %lld\n\n", ((TotalNodes * 1000) / TotalTime));

		LogFile = fopen(FileName, "a");
		fprintf(LogFile, "PV split depth  : %d\n", i);
		fprintf(LogFile, "average kNPS : %lld\n\n", ((TotalNodes * 1000) / TotalTime));
		fclose(LogFile);

		ThisNPS = ((TotalNodes * 1000) / TotalTime);
		if (ThisNPS > BestPVNPS)
			{
			BestPVNPS = ThisNPS;
			BestPVSD = i;
			}
		}
	PVSplitDepth = BestPVSD;
    Send("fastest PV Split depth: %d\n\n", BestPVSD);

	LogFile = fopen(FileName, "a");
	fprintf(LogFile, "fastest PV Split depth: %d\n", BestPVSD);
	fprintf(LogFile, "----------------------\n\n");
	fclose(LogFile);
    Send("results written to:\n");
    Send("%s\n\n", FileName);

	BenchMarking = false;
	HashClearAll();
	InitPosition(Position, "startpos");
    }	
static void CreateBenchLog(void)
    {
    char buf[256];
    time_t now;
    struct tm tnow;
    time(&now);
    tnow = *localtime(&now);
    strftime(buf, 32, "%H-%M_%b-%d", &tnow);
    sprintf(FileName, "benchmark_%s.txt", buf);
    Send("results written to:\n");
    Send("benchmark_%s.txt\n\n", buf);
    LogFile = fopen(FileName, "wt");
	fprintf(LogFile, "// " Engine " " Vers " " Plat "\n");
	fprintf(LogFile, "// benchmark results\n\n");
	fprintf(LogFile, "total nodes: %lld\n", TotalNodes);
	fprintf(LogFile, "total time : %lld ms\n", TotalTime / 1000);
	fprintf(LogFile, "total NPS  : %lld kNPS\n\n", ((TotalNodes * 1000) / TotalTime));
    fclose(LogFile);
	}
static void CreateSDBenchLog(void)
    {
    char buf[256];
    time_t now;
    struct tm tnow;
    time(&now);
    tnow = *localtime(&now);
    strftime(buf, 32, "%H-%M_%b-%d", &tnow);
    sprintf(FileName, "sd_benchmark_%s.txt",buf);
    LogFile = fopen(FileName, "wt");
	fprintf(LogFile, "// " Engine " " Vers " " Plat "\n");
	fprintf(LogFile, "// SD benchmark results\n\n");
    fclose(LogFile);
	}

#endif