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



#if defined(LinuxLargePages)
#include <sys/ipc.h>
#include <sys/shm.h>
#endif

#if defined(LinuxLargePages)
void SetupPrivileges() { }
void CreateMem(void ** A, int align, uint64 size, int *w, bool *use, char *string)
    {
        *w = shmget(IPC_PRIVATE, size, IPC_CREAT | SHM_R | SHM_W | SHM_HUGETLB);
        if ((*w) == -1)
            MemAlign((*A), align, size);
        else
            {
           (*A) = shmat((*w), NULL, 0x0);
           (*use) = true;
			if (VerboseUCI)
				Send("info string %s as HUGETLB %d\n", string, size >> 20);
				
#ifdef Log
			if (WriteLog)
				{
				log_file = fopen(log_filename, "a");
				if (VerboseUCI)
					fprintf(log_file, "info string %s as HUGETLB %d\n", string, size >> 20);
				close_log();
				}
#endif

            }

    if ((*w) == -1)
       (*use) = false;
    }

void FreeMem(void *A, int *w, bool *use)
    {
    if (!A)
        return;
    if ((*w) == -1)
        {
        AlignedFree(A);
        return;
        }
    shmdt(A);
    shmctl((*w), IPC_RMID, NULL);
   (*w) = -1;
   (*use) = false;
    }
#endif

#ifdef LargePages
void CreateMem( void ** A, int align, uint64 size, int *w, bool *use, char *string )
    {
    DWORD ErrorLP;
    if (NoSupport == true)
        goto NotSupported;
    (*A) = VirtualAlloc(NULL, size, MEM_LARGE_PAGES | MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if ((*A))
        {
        (*use) = true;
        Send("%s LP Size = %dM allocated successfully\n", string, size >> 20);
        if (VerboseUCI)
            Send("info string %s Large Pages size %d\n", string, size >> 20);

#ifdef Log
        if (WriteLog)
            {
            log_file = fopen(log_filename, "a");
            fprintf(log_file, "%s LP Size = %dM allocated successfully\n", string, size >> 20);
            if (VerboseUCI)
                fprintf(log_file, "info string %s Large Pages size %d\n", string, size >> 20);
            close_log();
            }
#endif

        }
    else
        {
        ErrorLP = GetLastError();
        if (ErrorLP == 1450)
            Send("LP %s = %dM : Not enough memory, please reboot system\n", string, size >> 20);
        else if (ErrorLP == 87)
            Send("LP %s = %dM : Invalid memory size parameter\n", string, size >> 20);
        else if (ErrorLP == 1314)
            Send("LP %s = %dM : No privileges, enable in gpedit.msc\n", string, size >> 20);

#ifdef Log
        if (WriteLog)
            {
            log_file = fopen(log_filename, "a");
            if (ErrorLP == 1450)
                fprintf(log_file, "LP %s = %dM : Not enough memory, please reboot system\n", string, size >> 20);
            else if (ErrorLP == 87)
                fprintf(log_file, "LP %s = %dM : Invalid memory size parameter\n", string, size >> 20);
            else if (ErrorLP == 1314)
                fprintf(log_file, "LP %s = %dM : No privileges, enable in gpedit.msc\n", string, size >> 20);
            close_log();
            }
#endif

        NotSupported:
        (*use) = false;
        MemAlign((*A), align, size);
        }
    }
void FreeMem( void *A, int *w, bool *use )
    {
    if (!A)
        return;
    if (!( *use))
        {
        AlignedFree(A);
        return;
        }
    VirtualFree(A, 0, MEM_RELEASE);
    }
void SetupPrivileges()
    {
    HANDLE token_handle;
    TOKEN_PRIVILEGES tp;
    int MinimumPSize = GetLargePageMinimum();
    if (!MinimumPSize)
        {
        Send("Large Pages not supported\n");
        if (VerboseUCI)
            Send("info string Large Pages not supported\n");

#ifdef Log
        if (WriteLog)
            {
            log_file = fopen(log_filename, "a");
            fprintf(log_file, "Large Pages not supported\n");
            if (VerboseUCI)
                fprintf(log_file, "info string Large Pages not supported\n");
            close_log();
            }
#endif

        NoSupport = true;
        return;
        }
    NoSupport = false;
    Send("Large Pages supported\n");
    Send("Minimum Large Pages size = %i\n", MinimumPSize);
    if (VerboseUCI)
        {
        Send("info string Large Pages supported\n");
        Send("info string Minimum Large Pages size = %i\n", MinimumPSize);
        }

#ifdef Log
    if (WriteLog)
        {
        log_file = fopen(log_filename, "a");
        fprintf(log_file, "Large Pages supported\n");
        fprintf(log_file, "Minimum Large Pages size = %i\n", MinimumPSize);
        if (VerboseUCI)
            {
            fprintf(log_file, "info string Large Pages supported\n");
            fprintf(log_file, "info string Minimum Large Pages size = %i\n", MinimumPSize);
            }
        close_log();
        }
#endif

    OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &token_handle);
    LookupPrivilegeValue(NULL, TEXT("SeLockMemoryPrivilege"), &tp.Privileges[0].Luid);
    tp.PrivilegeCount = 1;
    tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    AdjustTokenPrivileges(token_handle, false, &tp, 0, NULL, 0);
    CloseHandle(token_handle);
    }
#endif

#if !defined( LinuxLargePages) && !defined(LargePages)
void FreeMem( void *A, int *w, bool *use )
    {
    AlignedFree(A);
    }
void CreateMem( void ** A, int align, uint64 size, int *w, bool *use, char *string )
    {
    MemAlign((*A), align, size);
    }
void SetupPrivileges() { }
#endif
