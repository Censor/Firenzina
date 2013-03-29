<<<<<<< HEAD:src/init_gen.h
/*
Firenzina is a UCI chess playing engine by Kranium (Norman Schmidt)
Firenzina is based on Ippolit source code: http://ippolit.wikispaces.com/
authors: Yakov Petrovich Golyadkin, Igor Igorovich Igoronov,
and Roberto Pescatore copyright: (C) 2009 Yakov Petrovich Golyadkin
date: 92th and 93rd year from Revolution
owners: PUBLICDOMAIN (workers)
dedication: To Vladimir Ilyich
=======
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
>>>>>>> Linux/Housekeeping/Bug fixes/Extend xTreme/Defs:Firenzina/init_gen.h
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

static uint32 CaptureValue[16][16];
void InitCaptureValues()
    {
    CaptureValue[wEnumP][bEnumQ] = (0xd0 << 24) + (0x02 << 20);
    CaptureValue[wEnumN][bEnumQ] = (0xcf << 24) + (0x02 << 20);
    CaptureValue[wEnumBL][bEnumQ] = (0xce << 24) + (0x02 << 20);
    CaptureValue[wEnumBD][bEnumQ] = (0xce << 24) + (0x02 << 20);
    CaptureValue[wEnumR][bEnumQ] = (0xcd << 24) + (0x02 << 20);
    CaptureValue[wEnumQ][bEnumQ] = (0xcc << 24) + (0x01 << 20);
    CaptureValue[wEnumP][bEnumR] = (0xc8 << 24) + (0x02 << 20);
    CaptureValue[wEnumN][bEnumR] = (0xc7 << 24) + (0x02 << 20);
    CaptureValue[wEnumBL][bEnumR] = (0xc6 << 24) + (0x02 << 20);
    CaptureValue[wEnumBD][bEnumR] = (0xc6 << 24) + (0x02 << 20);
    CaptureValue[wEnumR][bEnumR] = (0xc5 << 24) + (0x01 << 20);
    CaptureValue[wEnumQ][bEnumR] = (0xc4 << 24) + (0x00 << 20);
    CaptureValue[wEnumP][bEnumBL] = (0xc0 << 24) + (0x02 << 20);
    CaptureValue[wEnumN][bEnumBL] = (0xbf << 24) + (0x01 << 20);
    CaptureValue[wEnumBL][bEnumBL] = (0xbe << 24) + (0x01 << 20);
    CaptureValue[wEnumBD][bEnumBL] = (0xbe << 24) + (0x01 << 20);
    CaptureValue[wEnumR][bEnumBL] = (0xbd << 24) + (0x00 << 20);
    CaptureValue[wEnumQ][bEnumBL] = (0xbc << 24) + (0x00 << 20);
    CaptureValue[wEnumP][bEnumBD] = (0xc0 << 24) + (0x02 << 20);
    CaptureValue[wEnumN][bEnumBD] = (0xbf << 24) + (0x01 << 20);
    CaptureValue[wEnumBL][bEnumBD] = (0xbe << 24) + (0x01 << 20);
    CaptureValue[wEnumBD][bEnumBD] = (0xbe << 24) + (0x01 << 20);
    CaptureValue[wEnumR][bEnumBD] = (0xbd << 24) + (0x00 << 20);
    CaptureValue[wEnumQ][bEnumBD] = (0xbc << 24) + (0x00 << 20);
    CaptureValue[wEnumP][bEnumN] = (0xb8 << 24) + (0x02 << 20);
    CaptureValue[wEnumN][bEnumN] = (0xb7 << 24) + (0x01 << 20);
    CaptureValue[wEnumBL][bEnumN] = (0xb6 << 24) + (0x01 << 20);
    CaptureValue[wEnumBD][bEnumN] = (0xb6 << 24) + (0x01 << 20);
    CaptureValue[wEnumR][bEnumN] = (0xb5 << 24) + (0x00 << 20);
    CaptureValue[wEnumQ][bEnumN] = (0xb4 << 24) + (0x00 << 20);
    CaptureValue[wEnumP][bEnumP] = (0xb0 << 24) + (0x01 << 20);
    CaptureValue[wEnumN][bEnumP] = (0xaf << 24) + (0x00 << 20);
    CaptureValue[wEnumBL][bEnumP] = (0xae << 24) + (0x00 << 20);
    CaptureValue[wEnumBD][bEnumP] = (0xae << 24) + (0x00 << 20);
    CaptureValue[wEnumR][bEnumP] = (0xad << 24) + (0x00 << 20);
    CaptureValue[wEnumQ][bEnumP] = (0xac << 24) + (0x00 << 20);
    CaptureValue[wEnumK][bEnumQ] = (0xcb << 24) + (0x03 << 20);
    CaptureValue[wEnumK][bEnumR] = (0xc3 << 24) + (0x03 << 20);
    CaptureValue[wEnumK][bEnumBL] = (0xbb << 24) + (0x03 << 20);
    CaptureValue[wEnumK][bEnumBD] = (0xbb << 24) + (0x03 << 20);
    CaptureValue[wEnumK][bEnumN] = (0xb3 << 24) + (0x03 << 20);
    CaptureValue[wEnumK][bEnumP] = (0xab << 24) + (0x03 << 20);
    CaptureValue[wEnumK][0] = (0x07 << 24) + (0x00 << 15);
    CaptureValue[wEnumP][0] = (0x06 << 24) + (0x01 << 15);
    CaptureValue[wEnumN][0] = (0x05 << 24) + (0x01 << 15);
    CaptureValue[wEnumBL][0] = (0x04 << 24) + (0x01 << 15);
    CaptureValue[wEnumBD][0] = (0x04 << 24) + (0x01 << 15);
    CaptureValue[wEnumR][0] = (0x03 << 24) + (0x01 << 15);
    CaptureValue[wEnumQ][0] = (0x02 << 24) + (0x01 << 15);
    CaptureValue[bEnumP][wEnumQ] = (0xd0 << 24) + (0x02 << 20);
    CaptureValue[bEnumN][wEnumQ] = (0xcf << 24) + (0x02 << 20);
    CaptureValue[bEnumBL][wEnumQ] = (0xce << 24) + (0x02 << 20);
    CaptureValue[bEnumBD][wEnumQ] = (0xce << 24) + (0x02 << 20);
    CaptureValue[bEnumR][wEnumQ] = (0xcd << 24) + (0x02 << 20);
    CaptureValue[bEnumQ][wEnumQ] = (0xcc << 24) + (0x01 << 20);
    CaptureValue[bEnumP][wEnumR] = (0xc8 << 24) + (0x02 << 20);
    CaptureValue[bEnumN][wEnumR] = (0xc7 << 24) + (0x02 << 20);
    CaptureValue[bEnumBL][wEnumR] = (0xc6 << 24) + (0x02 << 20);
    CaptureValue[bEnumBD][wEnumR] = (0xc6 << 24) + (0x02 << 20);
    CaptureValue[bEnumR][wEnumR] = (0xc5 << 24) + (0x01 << 20);
    CaptureValue[bEnumQ][wEnumR] = (0xc4 << 24) + (0x00 << 20);
    CaptureValue[bEnumP][wEnumBL] = (0xc0 << 24) + (0x02 << 20);
    CaptureValue[bEnumN][wEnumBL] = (0xbf << 24) + (0x01 << 20);
    CaptureValue[bEnumBL][wEnumBL] = (0xbe << 24) + (0x01 << 20);
    CaptureValue[bEnumBD][wEnumBL] = (0xbe << 24) + (0x01 << 20);
    CaptureValue[bEnumR][wEnumBL] = (0xbd << 24) + (0x00 << 20);
    CaptureValue[bEnumQ][wEnumBL] = (0xbc << 24) + (0x00 << 20);
    CaptureValue[bEnumP][wEnumBD] = (0xc0 << 24) + (0x02 << 20);
    CaptureValue[bEnumN][wEnumBD] = (0xbf << 24) + (0x01 << 20);
    CaptureValue[bEnumBL][wEnumBD] = (0xbe << 24) + (0x01 << 20);
    CaptureValue[bEnumBD][wEnumBD] = (0xbe << 24) + (0x01 << 20);
    CaptureValue[bEnumR][wEnumBD] = (0xbd << 24) + (0x00 << 20);
    CaptureValue[bEnumQ][wEnumBD] = (0xbc << 24) + (0x00 << 20);
    CaptureValue[bEnumP][wEnumN] = (0xb8 << 24) + (0x02 << 20);
    CaptureValue[bEnumN][wEnumN] = (0xb7 << 24) + (0x01 << 20);
    CaptureValue[bEnumBL][wEnumN] = (0xb6 << 24) + (0x01 << 20);
    CaptureValue[bEnumBD][wEnumN] = (0xb6 << 24) + (0x01 << 20);
    CaptureValue[bEnumR][wEnumN] = (0xb5 << 24) + (0x00 << 20);
    CaptureValue[bEnumQ][wEnumN] = (0xb4 << 24) + (0x00 << 20);
    CaptureValue[bEnumP][wEnumP] = (0xb0 << 24) + (0x01 << 20);
    CaptureValue[bEnumN][wEnumP] = (0xaf << 24) + (0x00 << 20);
    CaptureValue[bEnumBL][wEnumP] = (0xae << 24) + (0x00 << 20);
    CaptureValue[bEnumBD][wEnumP] = (0xae << 24) + (0x00 << 20);
    CaptureValue[bEnumR][wEnumP] = (0xad << 24) + (0x00 << 20);
    CaptureValue[bEnumQ][wEnumP] = (0xac << 24) + (0x00 << 20);
    CaptureValue[bEnumK][wEnumQ] = (0xcb << 24) + (0x03 << 20);
    CaptureValue[bEnumK][wEnumR] = (0xc3 << 24) + (0x03 << 20);
    CaptureValue[bEnumK][wEnumBL] = (0xbb << 24) + (0x03 << 20);
    CaptureValue[bEnumK][wEnumBD] = (0xbb << 24) + (0x03 << 20);
    CaptureValue[bEnumK][wEnumN] = (0xb3 << 24) + (0x03 << 20);
    CaptureValue[bEnumK][wEnumP] = (0xab << 24) + (0x03 << 20);
    CaptureValue[bEnumK][0] = (0x07 << 24) + (0x00 << 15);
    CaptureValue[bEnumP][0] = (0x06 << 24) + (0x01 << 15);
    CaptureValue[bEnumN][0] = (0x05 << 24) + (0x01 << 15);
    CaptureValue[bEnumBL][0] = (0x04 << 24) + (0x01 << 15);
    CaptureValue[bEnumBD][0] = (0x04 << 24) + (0x01 << 15);
    CaptureValue[bEnumR][0] = (0x03 << 24) + (0x01 << 15);
    CaptureValue[bEnumQ][0] = (0x02 << 24) + (0x01 << 15);
    }
