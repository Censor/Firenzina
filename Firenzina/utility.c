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
#include "material_value.h"
#include "intrin.h"

#define Tweak (0x74d3c012a8bf965e)
int PreviousDepth, PreviousFast;

char *Notate(uint32 move, char *M)
    {
    int fr, to, pr;
    char c[16] = "0123nbrq";
    fr = From(move);
    to = To(move);
    if (move == MoveNone)
        {
        M[0] = 'N';
        M[1] = 'U';
        M[2] = M[3] = 'L';
        M[4] = 0;
        return M;
        }
    sprintf(M, "%c%c%c%c", 'a' + (fr & 7), '1' + ((fr >> 3) & 7), 'a' + (to & 7), '1' + ((to >> 3) & 7));
    if (MoveIsProm(move))
        {
        pr = (move & FlagMask) >> 12;
        sprintf(M + 4, "%c", c[pr]);
        }
    return M;
    }
uint32 FullMove(typePos *Position, uint32 x)
    {
    int pi, to = To(x), fr = From(x);
    if (!x)
        return x;
    pi = Position->sq[fr];
    if (pi == wEnumK || pi == bEnumK)
        {
        if (to - fr == 2 || fr - to == 2)
            x |= FlagOO;
        }
    if (To(x) != 0 && To(x) == Position->Dyn->ep && (pi == wEnumP || pi == bEnumP))
        x |= FlagEP;
    return x;
    }
uint32 NumericMove(typePos *Position, char *str)
    {
    int x;
    x = FullMove(Position, (str[2] - 'a') + ((str[3] - '1') << 3) + ((str[0] - 'a') << 6) + ((str[1] - '1') << 9));
    if (str[4] == 'b')
        x |= FlagPromB;
    if (str[4] == 'n')
        x |= FlagPromN;
    if (str[4] == 'r')
        x |= FlagPromR;
    if (str[4] == 'q')
        x |= FlagPromQ;
    return x;
    }

void InitBitboards(typePos *Position)
    {
    int i, pi;
    uint64 O;

    BoardIsOk = false;
    for (i = 0; i < 16; i++)
        Position->bitboard[i] = 0;
    Position->Dyn->Hash = Position->Dyn->PawnHash = 0;
    Position->Dyn->material = 0;
    Position->Dyn->Static = 0;
    for (i = A1; i <= H8; i++)
        {
        if ((pi = Position->sq[i]))
            {
            Position->Dyn->Static += PST(pi, i);
            Position->Dyn->Hash ^= Hash(pi, i);
            if (pi == wEnumP || pi == bEnumP)
                Position->Dyn->PawnHash ^= Hash(pi, i);
            Position->Dyn->material += MaterialValue[pi];
            BitSet(i, Position->bitboard[Position->sq[i]]);
            }
        }
    wBitboardOcc = wBitboardK | wBitboardQ | wBitboardR | wBitboardB | wBitboardN | wBitboardP;
    bBitboardOcc = bBitboardK | bBitboardQ | bBitboardR | bBitboardB | bBitboardN | bBitboardP;
    Position->OccupiedBW = wBitboardOcc | bBitboardOcc;
    O = Position->OccupiedBW;
    if (POPCNT(wBitboardQ) > 1 || POPCNT(bBitboardQ) > 1
		|| POPCNT(wBitboardR) > 2 || POPCNT(bBitboardR) > 2
		|| POPCNT(wBitboardBL) > 1 || POPCNT(bBitboardBL) > 1
		|| POPCNT(wBitboardN) > 2 || POPCNT(bBitboardN) > 2
		|| POPCNT(wBitboardBD) > 1 || POPCNT(bBitboardBD) > 1)
        Position->Dyn->material |= 0x80000000;
    if (POPCNT(wBitboardK) != 1)
        BoardIsOk = false;
    if (POPCNT(bBitboardK) != 1)
        BoardIsOk = false;
    if (POPCNT(wBitboardQ) > 9)
        BoardIsOk = false;
    if (POPCNT(bBitboardQ) > 9)
        BoardIsOk = false;
    if (POPCNT(wBitboardR) > 10)
        BoardIsOk = false;
    if (POPCNT(bBitboardR) > 10)
        BoardIsOk = false;
    if (POPCNT(wBitboardBL) > 9)
        BoardIsOk = false;
    if (POPCNT(bBitboardBL) > 9)
        BoardIsOk = false;
    if (POPCNT(wBitboardBD) > 9)
        BoardIsOk = false;
    if (POPCNT(bBitboardBD) > 9)
        BoardIsOk = false;
    if (POPCNT(wBitboardBL | wBitboardBD) > 10)
        BoardIsOk = false;
    if (POPCNT(bBitboardBL | bBitboardBD) > 10)
        BoardIsOk = false;
    if (POPCNT(wBitboardN) > 10)
        BoardIsOk = false;
    if (POPCNT(bBitboardN) > 10)
        BoardIsOk = false;
    if (POPCNT(wBitboardP) > 8)
        BoardIsOk = false;
    if (POPCNT(bBitboardP) > 8)
        BoardIsOk = false;
    if (POPCNT(wBitboardOcc) > 16)
        BoardIsOk = false;
    if (POPCNT(bBitboardOcc) > 16)
        BoardIsOk = false;
    if ((wBitboardP | bBitboardP) & (Rank1 | Rank8))
        BoardIsOk = false;

    Position->wKsq = BSF(wBitboardK);
    Position->bKsq = BSF(bBitboardK);
    if ((WhiteOO && (Position->wKsq != E1 || !(wBitboardR & SqSet[H1])))
       || (WhiteOOO && (Position->wKsq != E1 || !(wBitboardR & SqSet[A1])))
          || (BlackOO && (Position->bKsq != E8 || !(bBitboardR & SqSet[H8])))
          || (BlackOOO && (Position->bKsq != E8 || !(bBitboardR & SqSet[A8]))))
        BoardIsOk = false;
    Position->Dyn->Hash ^= HashCastling[Position->Dyn->oo];
    if (Position->Dyn->ep)
        Position->Dyn->Hash ^= HashEP[Position->Dyn->ep & 7];
    Position->Dyn->PawnHash ^=
       HashCastling[Position->Dyn->oo] ^ Tweak ^ Hash(wEnumK, Position->wKsq) ^ Hash(bEnumK, Position->bKsq);
    if (Position->wtm)
        Position->Dyn->Hash ^= HashWTM;
    Mobility(Position);
    if (Position->wtm && Position->Dyn->wAtt & bBitboardK)
        BoardIsOk = false;
    if (!Position->wtm && Position->Dyn->bAtt & wBitboardK)
        BoardIsOk = false;
    if (POPCNT(Position->Dyn->bKcheck) >= 3 || POPCNT(Position->Dyn->wKcheck) >= 3)
        BoardIsOk = false;
    BoardIsOk = true;
    }
#ifdef WINDOWS
#include <time.h>
bool TryInput()
    {
    static int init = 0, is_pipe;
    static HANDLE stdin_h;
    DWORD val;
    if (SuppressInput)
        return false;
    if (!SearchIsDone && StallInput)
        return false;
    if (!init)
        {
        init = 1;
        stdin_h = GetStdHandle(STD_INPUT_HANDLE);
        is_pipe = !GetConsoleMode(stdin_h, &val);
        if (!is_pipe)
            {
            SetConsoleMode(stdin_h, val & ~(ENABLE_MOUSE_INPUT | ENABLE_WINDOW_INPUT));
            FlushConsoleInputBuffer(stdin_h);
            }
        }
    if (is_pipe)
        {
        if (!PeekNamedPipe(stdin_h, NULL, 0, NULL, &val, NULL))
            return 1;
        return val > 0;
        }
    else
        {
        GetNumberOfConsoleInputEvents(stdin_h, &val);
        return val > 1;
        }
    return 0;
    }
uint64 GetClock()
    {
    return(GetTickCount() * 1000ULL);
    }
	
uint64 ProcessClock()
{
	FILETIME ftProcCreation, ftProcExit, ftProcKernel, ftProcUser;
	LARGE_INTEGER user_time, kernel_time;
	uint64 x;
	uint64 tt = 10; 
	
	GetProcessTimes(GetCurrentProcess(), &ftProcCreation, &ftProcExit, &ftProcKernel, &ftProcUser);

	user_time.LowPart = ftProcUser.dwLowDateTime;
	user_time.HighPart = ftProcUser.dwHighDateTime;
	kernel_time.LowPart = ftProcKernel.dwLowDateTime;
	kernel_time.HighPart = ftProcKernel.dwHighDateTime;  	
	x = (uint64) (user_time.QuadPart + kernel_time.QuadPart) / tt;
	return x;
}
#else
#include <unistd.h>
bool TryInput()
    {
    int v;
    fd_set fd[1];
    struct timeval tv[1];
    if (SuppressInput)
        return false;
    if (!SearchIsDone && StallInput)
        return false;
    FD_Zero(fd);
    FD_SET(STDIN_FILENO, fd);
    tv->tv_sec = 0;
    tv->tv_usec = 0;
    v = select(STDIN_FILENO + 1, fd, NULL, NULL, tv);
    return(v > 0);
    }
#include <sys/time.h>
uint64 GetClock()
    {
    uint64 x;
    struct timeval tv;
    gettimeofday(&tv, NULL);
    x = tv.tv_sec;
    x *= 1000000;
    x += tv.tv_usec;
    return x;
    }
uint64 ProcessClock()
    {
    return (uint64)clock();
    }
#endif

#include <stdarg.h>
void Send(char *fmt, ...)
    {
    va_list Value;
    va_start(Value, fmt);
    vfprintf(stdout, fmt, Value);
    va_end(Value);
    fflush(stdout);
    }
void ErrorEnd(char *fmt, ...)
    {
    va_list Value;
    va_start(Value, fmt);
    va_end(Value);
    fprintf(stdout, "Error\n");
    vfprintf(stdout, fmt, Value);
    exit(1);
    }
void ErrorFen(char *fmt, ...)
    {
    va_list Value;
    va_start(Value, fmt);
    va_end(Value);
    fprintf(stdout, "Fen Error\n");
    vfprintf(stdout, fmt, Value);
    exit(1);
    }
void NewGame(typePos *Position, bool full)
    {
    int i;
    for (i = A1; i <= H8; i++)
        Position->sq[i] = 0;
    memset(Position->DynRoot, 0, (sizeof(typeDynamic) << 8));
    Position->Dyn = Position->DynRoot + 1;
    Position->wtm = true;
    Position->height = 0;
    Position->Dyn->oo = 0x0f;
    Position->Dyn->ep = 0;
    Position->Dyn->reversible = 0;
    for (i = A2; i <= H2; i++)
        Position->sq[i] = wEnumP;
    for (i = A7; i <= H7; i++)
        Position->sq[i] = bEnumP;
    Position->sq[D1] = wEnumQ;
    Position->sq[D8] = bEnumQ;
    Position->sq[E1] = wEnumK;
    Position->sq[E8] = bEnumK;
    Position->sq[A1] = Position->sq[H1] = wEnumR;
    Position->sq[A8] = Position->sq[H8] = bEnumR;
    Position->sq[B1] = Position->sq[G1] = wEnumN;
    Position->sq[B8] = Position->sq[G8] = bEnumN;
    Position->sq[C1] = wEnumBD;
    Position->sq[F1] = wEnumBL;
    Position->sq[C8] = bEnumBL;
    Position->sq[F8] = bEnumBD;
    PreviousDepth = 1000;
    PreviousFast = false;
    isNewGame = true;
    Position->StackHeight = 0;
    InitBitboards(Position);
    if (!full)
        return;
    PVHashClear();
    HashClear();
    EvalHashClear();
    ResetHistory();
    ResetPositionalGain();
    PawnHashReset();
    }
void ShowBanner()
    {
    char *startup_banner = "" Engine " " Vers " " Platform "\n"
        "by Yuri Censor, a clone of Fire 2.2 xTreme by Kranium, based on Ippolit\n" // Modification by Yuri Censor for Firenzina, 2/16/2013
        "compiled by Yuri Censor\n" // Modified by Yuri Censor for Firenzina, 2/23/2013; Was: compiled by NS (i.e., Norman Schmidt)
        "" __DATE__ " " __TIME__ "\n\n";

    Send(startup_banner);
    fflush(stdout);

#ifdef Log
		if (WriteLog)
			{
			log_file = fopen(log_filename, "a");
			fprintf(log_file, startup_banner);
			close_log();
			}
#endif
    }

void GetSysInfo()
    {
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    NumCPUs = sysinfo.dwNumberOfProcessors;

    if(NumCPUs < 1)
        NumCPUs = 1;

    if(NumCPUs > 1)
		{
        Send("\n%d CPUs found\n", NumCPUs);

#ifdef Log
		if (WriteLog)
			{
			log_file = fopen(log_filename, "a");
			fprintf(log_file, "\n%d CPUs found\n", NumCPUs);
			close_log();
			}
#endif

		}
    else
		{
        Send("\n%d CPU found\n", NumCPUs);

#ifdef Log
		if (WriteLog)
			{
			log_file = fopen(log_filename, "a");
			fprintf(log_file, "\n%d CPU found\n", NumCPUs);
			close_log();
			}
#endif

		}
	    
	NumThreads = NumCPUs<<1; // Modification by Yuri Censor for Firenzina, 2/16/2013
	                         // Was: NumThreads = NumCPUs; 
	                         // Problem: We couldn't have more threads than processors
    if(NumThreads > MaxCPUs)
        NumThreads = MaxCPUs;

    if(NumThreads > OptMaxThreads)
        NumThreads = OptMaxThreads;


    if(NumThreads > 1)
		{
        Send("using %d threads\n\n", NumThreads);

#ifdef Log
		if (WriteLog)
			{
			log_file = fopen(log_filename, "a");
			fprintf(log_file, "using %d threads\n\n", NumThreads);
			close_log();
			}
#endif

		}
    else
		{
        Send("using %d thread\n\n", NumThreads);

#ifdef Log
		if (WriteLog)
			{
			log_file = fopen(log_filename, "a");
			fprintf(log_file, "using %d thread\n\n", NumThreads);
			close_log();
			}
#endif
		}
    }

void SetPOPCNT()
	{
  	int CPUInfo[4] = {-1};
  	__cpuid(CPUInfo, 0x00000001);
  	HasPopcnt = (CPUInfo[2] >> 23) & 1;
  	if(HasPopcnt)
  		{
    	POPCNT = &PopcntHard;
    	Send("Hardware POPCNT supported\n\n");

#ifdef Log
		if (WriteLog)
			{
			log_file = fopen(log_filename, "a");
			fprintf(log_file, "Hardware POPCNT supported\n\n");
			close_log();
			}
#endif
  		}
  	else
		POPCNT = &PopcntEmul;
	}

#ifdef Log
int create_log(void)
    {
    char buf[256];
    time_t now;
    struct tm tnow;
    time(&now);
    tnow = *localtime(&now);
    strftime(buf, 32, "%d%b-%H%M", &tnow);
    sprintf(log_filename, "%s %s %s %s.txt", Engine, Vers, Platform, buf);
    log_file = fopen(log_filename, "wt");
	close_log();
	return false;
    }

void close_log(void)
    {
    fclose(log_file);
    }
#endif