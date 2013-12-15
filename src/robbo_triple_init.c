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

#if defined(__GNUC__)
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#endif

#include "fire.h"
#ifdef RobboBases
#include "robbo_totalbase.h"
#include "robbo_triplebase.h"

#define WindowsTriplePrefix "triple"
#define CheckByte(x, y)                                                 \
	if ((x) != (y))                                                        \
		{ Send ("Bad DynamicLoad watermark %5s %d %d\n", rtb->string, x, y); \
		return; }
void ReFactorDirectoryNaming(char *);
uint64 BytesTriple = 0, IndTriple = 0, IndTripleEx = 0;
void HackWindowsTriple(char * NOME);
void OpenTripleFile(RobboTripleBase *);

static uint32 fgetI(FILE *F)
    {
    int i;
    uint32 r = 0;
    for (i = 0; i < 4; i++)
        r |= (fgetc(F) << (8 * i));
    return r;
    }
static void RobboDynamicLoadSetUp(RobboTripleBase *rtb, FILE *F)
    {
    uint64 sz = rtb->size;
    uint64 heaps = (sz >> 30) + 1;
    int u, v, i, j, b, uc, hsz, uu;
    LoadCount++;
    CheckByte(fgetc(F), 0x53);
    CheckByte(fgetc(F), 0xa1);
    CheckByte(fgetc(F), 0x78);
    CheckByte(fgetc(F), 0x0e);
    u = fgetc(F);
    v = fgetc(F);
    rtb->prop_ind = u | (v << 8);
    if (u != 0x40)
		{
		if (VerboseRobboBases)
			{
	        Send("Dynamic Load requires 64 byte canals. Bytes %d\n", u);

#ifdef Log
			if (WriteLog)
				{
				log_file = fopen(log_filename, "a");
				fprintf(log_file, "Dynamic Load requires 64 byte canals. Bytes %d\n", u);
				close_log();
				}
#endif

			}
		}
    u = fgetc(F);
    if (u != 18)
		{
		if (VerboseRobboBases)
			{		
	        Send("Dynamic Load requires shift of 18. Shift %d\n", u);

#ifdef Log
			if (WriteLog)
				{
				log_file = fopen(log_filename, "a");
				fprintf(log_file, "Dynamic Load requires shift of 18. Shift %d\n", u);
				close_log();
				}
#endif

			}
		}
    u = fgetc(F);
    if (u != heaps)
		{
		if (VerboseRobboBases)
			{
        	Send("Heap count corrupted in DynamicLoad %d %d %s " Type64Bit "\n", u, heaps, rtb->string, rtb->size);

#ifdef Log
			if (WriteLog)
				{
				log_file = fopen(log_filename, "a");
				fprintf(log_file, "Heap count corrupted in DynamicLoad %d %d %s " Type64Bit "\n", u, heaps, rtb->string, rtb->size);
				close_log();
				}
#endif

			}
		}

    u = fgetI(F);
    for (i = 0; i < heaps; i++)
        {
        uu = fgetI(F);
        u = uu & 0xfffffff;
        u &= 0xfffffff;
        b = 0;
        uc = u;
        while (uc >>= 1)
            b++;
        rtb->packet_count[i] = u;
        rtb->bs_split[i] = b - 1;
        rtb->ind_split[i] = malloc((2 << b) * sizeof(uint32));
        rtb->packet_offset[i] = malloc(u * sizeof(uint32));
        for (j = 0; j < u; j++)
            rtb->ind_split[i][j] = fgetI(F);
        for (j = u; j < (2 << b); j++)
            rtb->ind_split[i][j] = (0x3fffffffULL << 2) | 0;
        for (j = 0; j < u; j++)
            rtb->packet_offset[i][j] = fgetI(F);
        if (uu & 0x10000000)
            {
            CheckByte(fgetc(F), 0x93);
            CheckByte(fgetc(F), 0x06);
            CheckByte(fgetc(F), 0xe1);
            CheckByte(fgetc(F), 0x88);
            hsz = fgetI(F);
            MemAlign(rtb->huffman_table[i], 0x40, hsz * sizeof(uint32));
            for (j = 0; j < hsz; j++)
                rtb->huffman_table[i][j] = fgetI(F);
            }
        }
    }
void DetachDirectDynamicLoad(RobboTripleBase *rtb)
    {
    int heap;
    if (!rtb->direct)
        return;
    if (!(rtb->variant & RobboDynamicLoad))
        return;
    if (rtb->w[2] == 0)
        return;
    rtb->direct = false;
    for (heap = 0; heap < 1 + (rtb->size >> 30); heap++)
        {
        AlignedFree(rtb->direct_cache[heap].Index);
        AlignedFree(rtb->direct_cache[heap].Data);
        }
    BulkCount--;
    BulkBytes -= rtb->by;
    BulkInd -= rtb->index;
    }
void IncurDirectDynamicLoad(RobboTripleBase *rtb)
    {
    int heap, slice, ind, bytes, ind_sz, byte_sz, Slices;
    if (rtb->direct)
        return;
    rtb->direct = true;
    BulkCount++;
    if (!rtb->disk)
        OpenTripleFile(rtb);
    rtb->index = rtb->by = 0;
    for (heap = 0; heap < 1 + (rtb->size >> 30); heap++)
        {
        ind_sz = byte_sz = 0;
        Slices = rtb->packet_count[heap];
        fseek(rtb->disk, rtb->packet_offset[heap][Slices - 1], SEEK_SET);
        CheckByte(fgetc(rtb->disk), 0xf2);
        CheckByte(fgetc(rtb->disk), 0x77);
        CheckByte(fgetc(rtb->disk), 0x83);
        CheckByte(fgetc(rtb->disk), 0x83);
        fread(&ind, 4, 1, rtb->disk);
        fread(&bytes, 4, 1, rtb->disk);
        ind = (0x1220 * (Slices - 1) + ind);
        if (ind & 0xf)
            ind += 0x10 - (ind & 0xf);
        MemAlign(rtb->direct_cache[heap].Index, 0x40, ind * sizeof(uint32));
        bytes = (0x40000 * (Slices - 1) + bytes);
        if (bytes & 0x3f)
            bytes += 0x40 - (ind & 0x3f);
        MemAlign(rtb->direct_cache[heap].Data, 0x40, bytes);
        for (slice = 0; slice < Slices; slice++)
            {
            fseek(rtb->disk, rtb->packet_offset[heap][slice], SEEK_SET);
            CheckByte(fgetc(rtb->disk), 0xf2);
            CheckByte(fgetc(rtb->disk), 0x77);
            CheckByte(fgetc(rtb->disk), 0x83);
            CheckByte(fgetc(rtb->disk), 0x83);
            fread(&ind, 4, 1, rtb->disk);
            fread(&bytes, 4, 1, rtb->disk);
            fread(rtb->direct_cache[heap].Index + ind_sz, 4, ind, rtb->disk);
            fread(rtb->direct_cache[heap].Data + byte_sz, 1, bytes, rtb->disk);
            ind_sz += ind;
            byte_sz += bytes;
            BulkBytes += bytes;
            BulkInd += ind;
            }
        rtb->index += ind_sz;
        rtb->by += byte_sz;
        }
    fclose(rtb->disk);
    rtb->disk = NULL;
    }

void LoadTriple(char *fnin, char *DIR)
    {
    int u, i, by, n, j, PR, ni, s;
    char FN[1024], NOME[1024];
    RobboTripleBase *triple;
    FILE *F;
    char Temp[1024];
    strcpy(NOME, fnin);
    for (i = 0; NOME[i] != '.'; i++);
    strcpy(Temp, fnin + 1 + i);
    if (Temp[0] == 'K' || Temp[0] == 'k')
        UnWindowsTriple(Temp);
    s = strlen(Temp);
    sprintf(FN, "%s/%s%s", DIR, TriplePrefix, NOME);
    F = fopen(FN, "rb");
    if (F == NULL)
        {
	    if (VerboseRobboBases)
			{
	        Send("No File %s\n", FN);

#ifdef Log
			if (WriteLog)
				{
				log_file = fopen(log_filename, "a");
				fprintf(log_file, "No File %s\n", FN);
				close_log();
				}
#endif

			}
        return;
        }
    if (VerboseRobboBases)
		{
        Send("info string RobboTriple %d %s\n", NumTripleBases, FN);

#ifdef Log
		if (WriteLog)
			{
			log_file = fopen(log_filename, "a");
			fprintf(log_file, "info string RobboTriple %d %s\n", NumTripleBases, FN);
			close_log();
			}
#endif

		}
    for (i = 1; i <= 4; i++)
        Temp[s + i] = 0;
    triple = TableTripleBases + NumTripleBases;
    TripleUtility(triple, Temp);
    strcpy(triple->DirNome, DIR);
#if !defined(_WIN32) && !defined(_WIN64)
    strcat(triple->DirNome, "/");
#else
    strcat(triple->DirNome, "\\");
#endif
    ReFactorDirectoryNaming(triple->DirNome);
    strcpy(triple->string, NOME + 2);
    sprintf(triple->path, "%s%s", TriplePrefix, NOME);
    triple->direct = false;
    triple->data = NULL;
    triple->ind = NULL;
    triple->disk = NULL;
    triple->size = triple->pawns ? 1806 : 462;
    triple->weak = triple->hit = 0;
    triple->scacco = false;
    for (i = 0; i < 4; i++)
        if (triple->p[i])
            TripleMaxUsage = MAX(TripleMaxUsage, (i + 3));
    for (i = 0; i < 4; i++)
        triple->size *= triple->m[i];
    for (i = 1; i <= 3; i++)
        {
        u = fgetc(F);
        triple->w[i] = u;
        }
    if (triple->w[2] == 0)
        {
        triple->direct = true;
        triple->w[3] = 0;
        fclose(F);
        F = 0;
        if (VerboseRobboBases)
			{
            Send("one result %s %d\n", FN, triple->w[1]);

#ifdef Log
			if (WriteLog)
				{
				log_file = fopen(log_filename, "a");
				fprintf(log_file, "one result %s %d\n", FN, triple->w[1]);
				close_log();
				}
#endif
			}

        TrivialCount++;
        return;
        }
    for (i = 1; i <= 3; i++)
        triple->wi[triple->w[i]] = i;
    if (VerboseRobboBases)
		{
        Send("w: %d/%d/%d  ", triple->w[1], triple->w[2], triple->w[3]);

#ifdef Log
		if (WriteLog)
			{
			log_file = fopen(log_filename, "a");
			fprintf(log_file, "w: %d/%d/%d  ", triple->w[1], triple->w[2], triple->w[3]);
			close_log();
			}
#endif
		}

    u = fgetc(F);
    triple->variant = u;
    if (u & RobboScaccoKnowBit)
        triple->scacco = ((u & RobboScaccoBit) != 0);
    else
        triple->scacco = (triple->p[2] != 0);
    if (u & RobboDynamicLoad)
        {
        RobboDynamicLoadSetUp(triple, F);
        fclose(F);
        if (0 && u & RobboDirectDynamicBit)
            IncurDirectDynamicLoad(triple);
        return;
        }
    triple->direct = true;
    by = 0;
    for (i = 0; i < 4; i++)
        by += (fgetc(F) << (i << 3));
    if (VerboseRobboBases)
		{
        Send("%d by  ", by);

#ifdef Log
		if (WriteLog)
			{
			log_file = fopen(log_filename, "a");
			fprintf(log_file, "%d by  ", by);
			close_log();
			}
#endif

		}
    n = 0;
    for (i = 0; i < 4; i++)
        n += (fgetc(F) << (8 * i));
    if (VerboseRobboBases)
		{
        Send("%d arch  ", n);

#ifdef Log
		if (WriteLog)
			{
			log_file = fopen(log_filename, "a");
			fprintf(log_file, "%d by  ", by);
			close_log();
			}
#endif
		}

    PR = 0;
    for (i = 0; i < 2; i++)
        PR += (fgetc(F) << (i << 3));
    if (VerboseRobboBases)
		{
        Send("PR %d  ", PR);

#ifdef Log
		if (WriteLog)
			{
			log_file = fopen(log_filename, "a");
			fprintf(log_file, "%d by  ", by);
			close_log();
			}
#endif
		}

    for (i = 0; i < 2; i++)
        fgetc(F);
    ni = by / PR;
    triple->prop_ind = PR;
    u = ni;
    i = 0;
    while (u >>= 1)
        i++;
    triple->bs = i - 1;
    triple->index = (2 << i) - 1;
    triple->ind = malloc((triple->index + 1) * sizeof(int));
    for (i = 0; i < ni; i++)
        {
        u = 0;
        for (j = 0; j < 4; j++)
            u += (fgetc(F) << (j << 3));
        triple->ind[i] = u;
        }
    for (i = ni; i < triple->index + 1; i++)
        triple->ind[i] = (n << 2);
    DirectCount++;
    triple->by = by;
    BytesTriple += by;
    IndTriple += triple->index + 1;
    IndTripleEx += ni;
    triple->data = malloc(PR * ni);
    fread(triple->data, 1, PR * ni, F);
    if (VerboseRobboBases)
        {
        Send("data (10): ");
        for (i = 0; i < 10; i++)
            Send("%x ", triple->data[i]);

#ifdef Log
		if (WriteLog)
			{
			log_file = fopen(log_filename, "a");
			fprintf(log_file, "data (10): ");
			for (i = 0; i < 10; i++)
				fprintf(log_file, "%x ", triple->data[i]);
			close_log();
			}
#endif

        }
    if (VerboseRobboBases)
		{
        Send("\n");

#ifdef Log
		if (WriteLog)
			{
			log_file = fopen(log_filename, "a");
			fprintf(log_file, "\n");
			close_log();
			}
#endif
		}

    fclose(F);
    }
void TripleStatistics()
    {
	if (1 + (NumTripleBases / 256) + (int)((BytesTriple + (IndTriple << 2)) >> 20) == 1)
		{
		Send("info string %d TripleBases Loaded\n", NumTripleBases);

#ifdef Log
		if (WriteLog)
			{
			log_file = fopen(log_filename, "a");
			fprintf(log_file, "info string %d TripleBases Loaded\n", NumTripleBases);
			close_log();
			}
#endif

		}
	else
		{
		Send("info string %d TripleBases Loaded (%dmb)\n",
			NumTripleBases, 1 + (NumTripleBases / 256) + (int)((BytesTriple + (IndTriple << 2)) >> 20));

#ifdef Log
		if (WriteLog)
			{
			log_file = fopen(log_filename, "a");
			fprintf(log_file, "info string %d TripleBases Loaded (%dmb)\n",
				NumTripleBases, 1 + (NumTripleBases / 256) + (int)((BytesTriple + (IndTriple << 2)) >> 20));
			close_log();
			}
#endif

		}
    }
void TripleEmit()
    {
    Send("info string %d RobboTripleBases in all\n", NumTripleBases);
    Send("info string %d CompleteDirect (%dmb) plus %d BulkDynamic (%dmb)\n", DirectCount,
       (int)((BytesTriple + (IndTriple << 2)) >> 20), BulkCount, (int)((BulkBytes + (BulkInd << 2)) >> 20));
    Send("info string Loadable are %d more RobboTripleBases" " "
       "Plus: %d buy the trivially!\n", LoadCount - BulkCount, TrivialCount);

#ifdef Log
		if (WriteLog)
			{
			log_file = fopen(log_filename, "a");
			fprintf(log_file, "info string Now %d RobboTripleBases in all\n", NumTripleBases);
			fprintf(log_file, "info string %d CompleteDirect (%dmb) plus %d BulkDynamic (%dmb)\n", DirectCount,
				(int)((BytesTriple + (IndTriple << 2)) >> 20), BulkCount, (int)((BulkBytes + (BulkInd << 2)) >> 20));
			fprintf(log_file, "info string Loadable are %d more RobboTripleBases" " "
				"Plus: %d buy the trivially!\n", LoadCount - BulkCount, TrivialCount);
			close_log();
			}
#endif

    }

#if !defined(_WIN32) && !defined(_WIN64)
void ReFactorDirectoryNaming(char *S)
    {
    char T[1024];
    int i, j = 0;
    for (i = 0; S[i]; i++)
        {
        if (i > 0 && S[i - 1] == '/' && S[i] == '/')
            continue;
        T[j++] = S[i];
        }
    if (T[j - 1] != '/')
        T[j++] = '/';
    T[j] = 0;
    strcpy(S, T);
    }
#else
void ReFactorDirectoryNaming(char *S)
    {
    char T[1024];
    int i, j = 0;
    for (i = 0; S[i]; i++)
        if (S[i] == '/')
            S[i] = '\\';
    for (i = 0; S[i]; i++)
        {
        if (i > 0 && S[i - 1] == '\\' && S[i] == '\\')
            continue;
        T[j++] = S[i];
        }
    if (T[j - 1] != '\\')
        T[j++] = '\\';
    T[j] = 0;
    strcpy(S, T);
    }
#endif

void BulkLoadDirectory(char *T)
    {
    int i;
    char S[4096];
    strcpy(S, T);
    ReFactorDirectoryNaming(S);
	if (VerboseRobboBases)
		{
		Send("info string RobboBulkLoad for registered TripleBases with directory %s\n", S);

#ifdef Log
		if (WriteLog)
			{
			log_file = fopen(log_filename, "a");
			fprintf(log_file, "info string RobboBulkLoad for registered TripleBases with directory %s\n", S);
			close_log();
			}
#endif

		}
    for (i = 0; i < NumTripleBases; i++)
        if (!strcmp(S, (TableTripleBases + i)->DirNome))
            IncurDirectDynamicLoad(TableTripleBases + i);
    }
int RobboBulkDirectory()
    {
    char Temp[4096];
    char *p;
    strcpy(Temp, BulkDirectory);
    ReFactorDirectoryNaming(Temp);
    for (p = strtok(Temp, "|"); p != NULL; p = strtok(NULL, "|"))
        BulkLoadDirectory(p);
	if (VerboseRobboBases)
		TripleEmit();
    return true;
    }
int BulkLoadOne(char *S)
    {
    RobboTripleBase *triple;
    triple = LookupTriple(S);
    if (triple == NULL)
        {
        if (VerboseRobboBases)
			{
			Send("info string Unfound string in BulkLoad %s\n", S);

#ifdef Log
			if (WriteLog)
				{
				log_file = fopen(log_filename, "a");
				fprintf(log_file, "info string Unfound string in BulkLoad %s\n", S);
				close_log();
				}
#endif

			}
        return false;
        }
    if (triple->efgh1 != -1 && strcspn(S, "EFGHefghIJijUVWX") == strlen(S))
        {
        int i;
        for (i = 0; i < 4; i++)
            IncurDirectDynamicLoad(TableTripleBases + (triple->efgh[i]));
        }
    else
        IncurDirectDynamicLoad(triple);
	return true;
    }
int RobboBulkLoad()
    {
    char Temp[4096];
    char *p;
    strcpy(Temp, BulkName);
    for (p = strtok(Temp, "|"); p != NULL; p = strtok(NULL, "|"))
        BulkLoadOne(p);
	if (VerboseRobboBases)
		TripleEmit();
    return true;
    }
int BulkDetachOne(char *S)
    {
    RobboTripleBase *triple;
    triple = LookupTriple(S);
    if (triple == NULL)
        {
        if (VerboseRobboBases)
			{
			Send("info string Unfound string in BulkDetach %s\n", S);

#ifdef Log
			if (WriteLog)
				{
				log_file = fopen(log_filename, "a");
				fprintf(log_file, "info string Unfound string in BulkDetach %s\n", S);
				close_log();
				}
#endif

			}
        return false;
        }
    if (triple->efgh1 != -1 && strcspn(S, "EFGHefghIJijUVWX") == strlen(S))
        {
        int i;
        for (i = 0; i < 4; i++)
            DetachDirectDynamicLoad(TableTripleBases + (triple->efgh[i]));
        }
    else
        DetachDirectDynamicLoad(triple);
	return true;
    }
int RobboBulkDetach()
    {
    char Temp[4096];
    char *p;
    strcpy(Temp, BulkName);
    for (p = strtok(Temp, "|"); p != NULL; p = strtok(NULL, "|"))
        BulkDetachOne(p);
	if (VerboseRobboBases)
		TripleEmit();
    return true;
    }
void BulkDetachDirectory(char *T)
    {
    int i;
    char S[4096];
    strcpy(S, T);
    ReFactorDirectoryNaming(S);
    if (VerboseRobboBases)
		{
		Send("info string RobboBulkLoad for registered TripleBases with directory %s\n", S);

#ifdef Log
		if (WriteLog)
			{
			log_file = fopen(log_filename, "a");
			fprintf(log_file, "info string RobboBulkLoad for registered TripleBases with directory %s\n", S);
			close_log();
			}
#endif

		}

    for (i = 0; i < NumTripleBases; i++)
        if (!strcmp(S, (TableTripleBases + i)->DirNome))
            DetachDirectDynamicLoad(TableTripleBases + i);
    }
int RobboBulkDirectoryDetach()
    {
    char Temp[4096];
    char *p;
    strcpy(Temp, BulkDirectory);
    ReFactorDirectoryNaming(Temp);
    for (p = strtok(Temp, "|"); p != NULL; p = strtok(NULL, "|"))
        BulkDetachDirectory(p);
	if (VerboseRobboBases)
		TripleEmit();
    return true;
    }
static void AddNomeTriple(char *NOME, char *DIR)
    {
    int n = strlen(TriplePrefix);
    if (strstr(NOME, "TRIPLE_") || strstr(NOME, "triple_"))
        return;
    if (strstr(NOME, "bz2") || strstr(NOME, "00") || strstr(NOME, "tar"))
        return;
    if (memcmp(NOME, TriplePrefix, n) && memcmp(NOME, WindowsTriplePrefix, n))
        return;
    LoadTriple(NOME + n, DIR);
    }

static void BlockTripleIndexRegister(char *DIR, FILE *F)
    {
    char NOME[16], Temp[16], Temp2[16];
    uint8 A[4];
    unsigned char B[16];
    RobboTripleBase *triple;
    int i, heaps, h, j;
    bool IS_OK;
    char NomeFile[1024], Path[128];
    NOME[4] = 0;
    while (fread(B, 1, 4, F))
        {
        if (B[0] != 0x09 || B[1] != 0x36 || B[2] != 0xf4 || B[3] != 0x2b)
            {
		    if (VerboseRobboBases)
				{
	            Send("BlockIndex mask failure!\n");

#ifdef Log
				if (WriteLog)
					{
					log_file = fopen(log_filename, "a");
					fprintf(log_file, "BlockIndex mask failure!\n");
					close_log();
					}
#endif
				}
            return;
            }
        fread(NOME, 1, 4, F);
        if (NOME[2] == 0)
            {
            strcpy(Temp, NOME);
            sprintf(NomeFile, "%s/TRIPLE0.%s", DIR, Temp);
            if (Stat(NomeFile) == -1)
                {
                if (Temp[0] == 'K' || Temp[0] == 'k')
                    UnWindows(Temp);
                else
                    HackWindowsTriple(Temp);
                sprintf(NomeFile, "%s/TRIPLE0.%s", DIR, Temp);
                if (Stat(NomeFile) == -1)
                    continue;
                }
            strcpy(Temp2, "0.");
            strcat(Temp2, Temp);
            LoadTriple(Temp2, DIR);
            continue;
            }
        fread(A, 1, 4, F);
        if (A[1] == 0)
            {
            triple = TableTripleBases + NumTripleBases;
            TripleUtility(triple, NOME);
            triple->direct = true;
            triple->w[1] = A[0];
            triple->w[2] = triple->w[3] = 0;
            TrivialCount++;
            continue;
            }
        strcpy(Temp, NOME);

#ifdef NomeWindows
        HackWindowsTriple(Temp);
#endif

        sprintf(Path, "TRIPLE%c.%s", NOME[3] ? '2' : '3', Temp);
        sprintf(NomeFile, "%s/%s", DIR, Path);
        if (Stat(NomeFile) == -1)
            {
            if (Temp[0] == 'K' || Temp[0] == 'k')
                UnWindows(Temp);
            else
                HackWindowsTriple(Temp);
            sprintf(Path, "TRIPLE%c.%s", NOME[3] ? '2' : '3', Temp);
            sprintf(NomeFile, "%s/%s", DIR, Path);
            if (Stat(NomeFile) == -1)
                IS_OK = false;
            else
                IS_OK = true;
            }
        else
            IS_OK = true;
        triple = TableTripleBases + NumTripleBases;
        if (IS_OK)
            {
            TripleUtility(triple, NOME);
            strcpy(triple->DirNome, DIR);

#if !defined(_WIN32) && !defined(_WIN64)
            strcat(triple->DirNome, "/");
#else
            strcat(triple->DirNome, "\\");
#endif

            ReFactorDirectoryNaming(triple->DirNome);
            strcpy(triple->string, NOME);
            strcpy(triple->path, Path);
            }
        triple->direct = false;
        triple->data = NULL;
        triple->ind = NULL;
        triple->disk = NULL;
        triple->size = triple->pawns ? 1806 : 462;
        triple->weak = triple->hit = 0;
        triple->scacco = false;
        for (i = 1; i <= 3; i++)
            triple->w[i] = A[i - 1];
        for (i = 1; i <= 3; i++)
            triple->wi[triple->w[i]] = i;
        triple->variant = A[3];
        for (i = 0; i < 4; i++)
            if (triple->p[i] && IS_OK)
                TripleMaxUsage = MAX(TripleMaxUsage, (i + 3));
        for (i = 0; i < 4; i++)
            triple->size *= triple->m[i];
        fread(A, 1, 4, F);
        heaps = A[3];
        for (h = 0; h < heaps; h++)
            {
            int u = fgetI(F);
            int t = u;
            int b = 0;
            while (t >>= 1)
                b++;
            triple->packet_count[h] = u;
            triple->bs_split[h] = b - 1;
            triple->ind_split[h] = malloc((2 << b) * sizeof(uint32));
            triple->packet_offset[h] = malloc(u * sizeof(uint32));
            for (j = 0; j < u; j++)
                triple->ind_split[h][j] = fgetI(F);
            for (j = u; j < (2 << b); j++)
                triple->ind_split[h][j] = (0x3fffffffULL << 2) | 0;
            for (j = 0; j < u; j++)
                triple->packet_offset[h][j] = fgetI(F);
            }
        if (!IS_OK)
            {
            for (h = 0; h < heaps; h++)
                free(triple->ind_split[h]);
            for (h = 0; h < heaps; h++)
                free(triple->packet_offset[h]);
            }
        else
            LoadCount++;
        }
	if (VerboseRobboBases)
		TripleEmit();
    }
static bool HasTripleBlockIndex(char *DIR)
    {
    FILE *F;
    char Name[256];
    sprintf(Name, "%s/BlockTriple.Index", DIR);
    F = fopen(Name, "rb");
    if (!F)
        return false;
	if (VerboseRobboBases)
		{
    	Send("Using BlockTriple.Index for %s\n", DIR);

#ifdef Log
		if (WriteLog)
			{
			log_file = fopen(log_filename, "a");
			fprintf(log_file, "Using BlockTriple.Index for %s\n", DIR);
			close_log();
			}
#endif
		}

    BlockTripleIndexRegister(DIR, F);
    fclose(F);
    return true;
    }

#if defined(_WIN32) || defined(_WIN64)
void GetTripleBase(char *A)
    {
    char B[1024];
    WIN32_FIND_DATA ffd;
    HANDLE hFind = INVALID_HANDLE_VALUE;
    if (HasTripleBlockIndex(A))
        return;
	if (VerboseRobboBases)
		{
		Send("info string Reading directory %s\n", A);

#ifdef Log
		if (WriteLog)
			{
			log_file = fopen(log_filename, "a");
			fprintf(log_file, "info string Reading directory %s\n", A);
			close_log();
			}
#endif

		}
    strcpy(B, A);
    strcat(B, "\\*");
    hFind = FindFirstFile(B, &ffd);
    if (hFind == INVALID_HANDLE_VALUE)
        {
        if (VerboseRobboBases)
			{
			Send("info string Directory %s not found\n", A);

#ifdef Log
			if (WriteLog)
				{
				log_file = fopen(log_filename, "a");
				fprintf(log_file, "info string Directory %s not found\n", A);
				close_log();
				}
#endif

			}
        return;
        }
    do
        {
        if (!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            AddNomeTriple(ffd.cFileName, A);
        } while (FindNextFile(hFind, &ffd));
    FindClose(hFind);
	if (VerboseRobboBases)
		TripleEmit();
    }
#else

void GetTripleBase(char *A)
    {
    DIR *DIR;
    struct dirent DE[1], *ptr;
    if (HasTripleBlockIndex(A))
        return;
	if (VerboseRobboBases)
		{
		Send("info string Reading directory %s\n", A);

#ifdef Log
		if (WriteLog)
			{
			log_file = fopen(log_filename, "a");
			fprintf(log_file, "info string Directory %s not found\n", A);
			close_log();
			}
#endif

		}
    DIR = opendir(A);
    if (!DIR)
        {
        if (VerboseRobboBases)
			{
			Send("info string Directory %s not found\n", A);

#ifdef Log
			if (WriteLog)
				{
				log_file = fopen(log_filename, "a");
				fprintf(log_file, "info string Directory %s not found\n", A);
				close_log();
				}
#endif

			}
        return;
        }
    while (1)
        {
        readdir_r(DIR, DE, &ptr);
        if (!ptr)
            break;
        AddNomeTriple(DE->d_name, A);
        }
    closedir(DIR);
	if (VerboseRobboBases)
		TripleEmit();
    }
#endif
#endif
