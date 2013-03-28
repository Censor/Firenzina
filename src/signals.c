/*******************************************************************************
Firenzina is a UCI chess playing engine by
Yuri Censor (Dmitri Gusev) and ZirconiumX (Matthew Brades).
Rededication: To the memories of Giovanna Tornabuoni and Domenico Ghirlandaio.
Special thanks to: Norman Schmidt, Jose Maria Velasco, Jim Ablett, Jon Dart.
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
#include <signal.h>
static MutexType DeletionLock[1];
static void SignalHandle(int signo)
    {
    struct sigaction sig_action[1];
    Lock(DeletionLock);
    SharedDelete();
    sig_action->sa_handler = SIG_DFL;
    sigaction(signo, sig_action, NULL);
    ;
    UnLock(DeletionLock);
    raise(signo);
    }
void LinuxHandleSignals()
    {
    struct sigaction sig_action[1];
    sig_action->sa_handler = SignalHandle;
    sigaction(SIGABRT, sig_action, NULL);
    sigaction(SIGHUP, sig_action, NULL);
    sigaction(SIGINT, sig_action, NULL);
    sigaction(SIGSEGV, sig_action, NULL);
    sigaction(SIGTERM, sig_action, NULL);
    LockInit(DeletionLock);
    }
#endif
