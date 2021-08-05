/*
 * ptrace.c (tab 3)
 * v0.01 2001/02/05
 * v0.02 2001/12/09
 *
 * Copyright (C) 2001 by P.Gleichmann <ravemax@dextrose.com>
 *
 * Based on "ExecTrace" by Trent Waddington.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation under the terms of the GNU General Public License is hereby
 * granted. No representations are made about the suitability of this software
 * for any purpose. It is provided "as is" without express or implied warranty.
 * See the GNU General Public License for more details.
 * 
 * Modified by Waba <wabasoft@yahoo.fr> on 09/12/01 :
 * Added the -b option to use breakpoints instead of single-steps (see README)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <getopt.h>
#include <sys/wait.h>
#include <sys/ptrace.h>
#include <linux/user.h>
#include <bfd.h>
#include <ctype.h>
#include <unistd.h>
#include <signal.h>

typedef enum { FALSE = 0,TRUE } Bool;
typedef unsigned long Uint32;
typedef unsigned char byte;
typedef unsigned int address;
typedef unsigned int word;

/*******************************************************************************\
*                                                                               *
*                                  #defines                                     *
*                                                                               *
\*******************************************************************************/
#define OUTPREF "[ptrace] "	// Prefix of stdout, to don't confuse ptrace output and prog output
#define VERSION "0.02"
#define IOP 0xf1		// The Illegal OP
//#undef CHECK_ERRORS	// Undefine CHECK_ERRORS if you want to go REALLY fast
#define CHECK_ERRORS	// (but doesn't change a lot)
#define MAX_SIGS 10	// Max number of authorized continued signals (prevents infinite loop)
					// Set it to 0 to disable checking

/*******************************************************************************\
*                                                                               *
*                                 Global vars                                   *
*                                                                               *
\*******************************************************************************/
/*
 * The file-extension
 */
const char* FileExt = "trc";

/*
 * Commandline-options
 */
Bool onlyText = FALSE,
     verbose  = FALSE;
char* outFName = NULL;
char *BP_file = NULL;


/*******************************************************************************\
*                                                                               *
*                                    Macros                                     *
*                                                                               *
\*******************************************************************************/
/* eptrace macro: checks for errors only if CHECK_ERRORS isdef */
/* Note: only for non-PEEK ptraces, because we can't return a value */
#ifdef CHECK_ERRORS
#	define eptrace(action,pid,addr,val)					\
		if ( (ptrace (action, pid, addr, val)) == -1) {	\
			perror ("ptrace");							\
			exit (1);									\
		}
#else
#	define eptrace(action,pid,addr,val)	\
		ptrace (action, pid, addr, val)
#endif
/* eptrace - end */


/*******************************************************************************\
*                                                                               *
*                               Breakpoints functions                           *
*                                                                               *
\*******************************************************************************/
/* BP_set sets a breakpoint by replacing the normal instruction
 * with IOP (0xf1), an illegal instruction which generate a SIGTRAP.
 *
 * Return value: the normal instruction, to be restored later
 */
inline byte BP_set (int pid, address offset)
{
	word prev=0;				// Previous value
	
	prev = ptrace (PTRACE_PEEKTEXT, pid, offset, 0);
	if (prev == -1) {	// Hoping that we won't encounter 0xffffffff at offset
		perror ("ptrace");
		printf (OUTPREF "Error in BP_set, on pid=%d and offset=0x%x.\n", pid, offset);
		exit (1);
	}

	eptrace (PTRACE_POKETEXT, pid, offset, (prev & 0xffffff00) | IOP);
	
	return (0xff & prev);	// Returns only the opcode byte
}

/* BP_unset unsets a BP previously set with BT_set.
 * It simply puts prev at offset to replace the 0xf1
 */
inline void BP_unset (int pid, address offset, byte prev)
{
	register word new;		// The new value we'll write

	new = ptrace (PTRACE_PEEKTEXT, pid, offset, 0);

	// Current value except for last byte
	eptrace (PTRACE_POKETEXT, pid, offset, (new & 0xffffff00) | prev);
}



/*******************************************************************************\
*                                                                               *
*                         Args parsing functions & such                         *
*                                                                               *
\*******************************************************************************/
void Version()
{
	printf("PTrace version %s\n", VERSION);
	exit(EXIT_SUCCESS);
}

void Usage()
{
	printf("Usage: ptrace [options] program [<arguments>]\n"
			"Options:\n"
			"  -h  --help              Show this help\n"
			"      --version           Version\n"
			"  -o  --logfile filename  Output to 'filename' (default 'program'.trc)\n"
			"  -v  --verbose           Verbose-mode\n"
			"  -t  --text              Only .text section\n"
			"  -b  --breakpoints file  Enables breakpoints mode (see docs), loading from 'file'\n");
	exit(EXIT_SUCCESS);
}

/*
 * My "getopt"-clone.
 * Necessary, because getopt parses options even after the program-name
 */
typedef struct _CmdlineOpts {
	char*	name;
	int 	smap;
	Bool 	reqArg;
	#define CMD_NO_ARG  FALSE
	#define CMD_ARG_REQ TRUE
} CmdlineOpts;

typedef void (*CmdlineCb)(int,const char*);

int ParseCmdline(int argc,char** argv,const CmdlineOpts* lopt,
		const char* sopt,CmdlineCb cmdcb)
{
	int   argPos;
	char* argPtr,*soPtr;
	const CmdlineOpts *co;

	for (argPos = 1; argPos < argc; argPos++) {
		if (*argv[argPos] != '-')
			break;
		argPtr = argv[argPos]+1;
		if (*argPtr == '\0') {
			fprintf(stderr,OUTPREF "error: '-' without option-character found\n");
			exit(EXIT_FAILURE);
		}
		// long options
		if (*argPtr == '-') {
			for (co = lopt; co->name; co++) {
				if (!strcmp(co->name,argPtr+1))
					break;
			}
			if (!co->name) {
				fprintf(stderr,OUTPREF "error: unknown longoption '%s'\n",argPtr+1);
				exit(EXIT_FAILURE);
			}
			if (co->reqArg == CMD_ARG_REQ) {
				if ((argPos == argc-1) || (*argv[argPos+1] == '-')) {
					fprintf(stderr,OUTPREF "error: missing option-parameter to '%s'\n",
							argPtr+1);
					exit(EXIT_FAILURE);
				}
				cmdcb(co->smap,argv[argPos+1]);
				argPos++;
			} else {
				cmdcb(co->smap,NULL);
			}
		// normal options, can be combined
		} else {
			while (*argPtr) {
				if (!(soPtr = strchr(sopt,*argPtr))) {
					fprintf(stderr,OUTPREF "error: unknown option '%c'\n",*argPtr);
					exit(EXIT_FAILURE);
				}
				if (soPtr[1] == ':') {
					if ((argPos == argc-1) || (*argv[argPos+1] == '-')) {
						fprintf(stderr,OUTPREF "error: missing option-parameter to '%c'\n",
								*argPtr);
						exit(EXIT_FAILURE);
					}
					cmdcb(*argPtr,argv[argPos+1]);
					argPos++;
					break;
				} else {
					cmdcb(*argPtr,NULL);
				}
				argPtr++;
			}
		}
	}

	return argPos;
}

void HandleOpts(int i,const char* oarg)
{
	switch (i) {
		case 1 :
			Version();
		case 'h' :
			Usage();
		case 'o' :
			outFName = (char*)oarg;
			break;
		case 'v' :
			verbose = TRUE;
			break;
		case 't' :
			onlyText = TRUE;
			break;
		case 'b' :
			BP_file = (char *) oarg;
			break;
	}
}


/*******************************************************************************\
*                                                                               *
*                               Tracing functions                               *
*                                                                               *
\*******************************************************************************/
/*
 * Opens the file containing the BP lines (from objdump -d or exported from LDasm)
 * and sets the BPs, saving the opcodes in tab[] (must be codeHigh-codeLow big)
 */
void SetBPsFromFile (int pid, const char *fname, byte tab[], long codeLow, long codeHigh)
{
	FILE *fd;
	char buf[512];	// Should be enough
	address offset;
	byte prev;
	unsigned int i=0;
	
	if ((fd = fopen (fname, "r")) == NULL) {
		fprintf (stderr, OUTPREF "Error: failed to open '%s' for reading.\n", fname);
		exit (EXIT_FAILURE);
	}
	
	if (verbose)
		printf (OUTPREF "Setting up breakpoints, range 0x%x - 0x%x from %s... ", 
			(unsigned int) codeLow, (unsigned int) codeHigh, fname);
	
	while (fgets (buf, 512, fd)) {
		if ((buf[0] == ':' || buf[0] == ' ') && isdigit (buf[1]) && isxdigit (buf[4]) // Poor check
			&& strlen (buf) > 30) {	// strlen to filter out the follow-lines in the objdump output
			offset = (address) strtol (buf+1, NULL, 16);
			if (codeLow <= (long) offset && (long) offset <= codeHigh) {
				prev = BP_set (pid, offset);
				if (prev != IOP) {	// In case we would have same address pointed twice in the file, we would save
					tab[offset - codeLow] = prev;	// into our tab the IOP, and so restore the op to IOP, running
					++i;								// into an infinite loop
				} else if (verbose) {							
					fprintf (stderr, "\n" OUTPREF "Warning: tried to set same breakpoint (0x%x) twice.\n", 
						(unsigned int) offset);
				}
			} else
				fprintf (stderr, "\n" OUTPREF "Warning: out-of-range offset (0x%x != 0x%x-0x%x) in '%s', may be wrong file.\n",
					(unsigned int) offset, (unsigned int) codeLow, (unsigned int) codeHigh, fname);
		}
	}

	if (verbose)
		printf ("%u breakpoints inserted.\n", i);
		
	fclose (fd);
}

	
/*
 * Opens the file using the BFD to retrieve the object-description
 */
bfd* BFDInit(const char* fname)
{
	bfd* bfh;
	char **matching;

	bfd_init();
	if ((bfh = bfd_openr(fname,0))) {
		if (bfd_check_format_matches(bfh,bfd_object,&matching))
			return bfh;
   	    bfd_close(bfh);
	}
	fprintf(stderr,OUTPREF "error: BFD - %s\n",bfd_errmsg(bfd_get_error()));
	exit(EXIT_FAILURE);
}

/*
 * Determines the code-range (low and upper border) and its size
 */
void CodeRange(const char* fname,long* low,long* high,long* size)
{
	bfd* bf;
	asection* secp;
	long ltmp;

	bf = BFDInit(fname);
	*low = LONG_MAX;
	*high = LONG_MIN;

	for (secp = bf->sections; secp != NULL; secp = secp->next) {
		if (verbose) {
            printf(OUTPREF "section: %s\tvma: %08lX\tsize: %08lX\tflags: %d\n",
			       secp->name,secp->vma,secp->_cooked_size,secp->flags);
		}
		if (secp->flags & SEC_CODE) {
			ltmp = secp->vma+secp->_cooked_size-1;
			if (onlyText) {
				if (!strcmp(secp->name,".text")) {
					*low  = secp->vma;
					*high = ltmp;
					if (!verbose)
						break;
				}
			} else {
				if (secp->vma < *low)
					*low = secp->vma;
				if (ltmp > *high)
					*high = ltmp;
			}
		}
	}
	*size = (*high - *low)+1;
	if (verbose)
		printf(OUTPREF "\nCode from %08lX to %08lX (%ld bytes)\n",*low,*high,*size);
}

/*
 * Single steps a file
 */
void TraceFile(char **prog)
{
	long codeLow,codeHigh,codeSize;
	char logName[FILENAME_MAX];
	char* logNPtr = logName;
	FILE* logF;
	Uint32* ipBuf;
	int status,pid;
	struct user_regs_struct regs;
	long loop;
	byte *opBuf = 0;	// Will contain the replaced opcodes (in -b case)

	CodeRange(prog[0],&codeLow,&codeHigh,&codeSize);

	// create logfile and alloc mem
	if (outFName) {
		if (strstr(outFName,FileExt))
			logNPtr = outFName;
		else
			sprintf(logName,"%s.%s",outFName,FileExt);
	} else
		sprintf(logName,"%s.%s",prog[0],FileExt);
	if (!(logF = fopen(logNPtr,"wt"))) {
		fprintf(stderr,OUTPREF "error: failed to create '%s'\n",logNPtr);
		exit(EXIT_FAILURE);
	}
	if (!(ipBuf = (Uint32*)malloc(codeSize*4))) {
		fprintf(stderr,OUTPREF "error: not enough memory (wow)\n");
		fclose(logF);
		unlink(logName);
		exit(EXIT_FAILURE);
	}
	if (BP_file) {	// If -b
		opBuf = (byte *) malloc (sizeof (byte) * codeSize);
		memset (opBuf, IOP, sizeof (byte) * codeSize);	// Initialise opBuf to an impossible opcode
	}

	// trace
	if ((pid = fork()) == 0) {
		eptrace(PTRACE_TRACEME,0,0,0);
		execvp(prog[0],prog);
	}
	wait(&status);

	if (BP_file) { 		// Breakpoints version (-b)
		byte sig_count = -1;	// Count the number of happened signals (will be 0 at the SIGTRAP of PTRACE_TRACEME)
		SetBPsFromFile (pid, BP_file, opBuf, codeLow, codeHigh);

		while (!WIFEXITED(status)) {
			if (WIFSTOPPED(status)) {
				eptrace(PTRACE_GETREGS,pid,0,&regs);
				--regs.eip;		// Decrements eip to come back on the BPed instruction
				if ((regs.eip >= codeLow) && (regs.eip <= codeHigh) && (opBuf[regs.eip-codeLow] != IOP)) {
					// Here it should be one of our breakpoints
					eptrace (PTRACE_SETREGS, pid, 0, &regs);	// Decrements EIP
					
					BP_unset (pid, regs.eip, opBuf[regs.eip-codeLow]);	// Removes the BP
					opBuf[regs.eip-codeLow] = IOP;
					ipBuf[regs.eip-codeLow]++;	// Keeps trace
					
					eptrace (PTRACE_CONT, pid, 0, 0);	// And continue.
					
				} else {	// Unknown one
					int sig = WSTOPSIG (status);
					++sig_count;
					if (verbose) 
						fprintf (stderr, OUTPREF "Program stopped inside our code range, but not on one of our breakpoints "
							"(eip: 0x%x, opcode: %x, opBuf[]: 0x%x, signal %i).\n", (unsigned int) regs.eip+1,
							(unsigned int) ((ptrace (PTRACE_PEEKTEXT, pid, regs.eip+1, 0) & 0xff)),	// Not really regs.eip-1
							(unsigned int) opBuf[regs.eip-codeLow],
							sig);
					if (sig != SIGABRT && sig != SIGFPE && sig != SIGSEGV && sig != SIGILL && sig != SIGTERM 
						&& (!MAX_SIGS || sig_count < MAX_SIGS)) {	// Infinite loop checking
						eptrace (PTRACE_CONT, pid, 0, 0);
					} else {
						if (verbose && (MAX_SIGS && sig_count >= MAX_SIGS))
							printf (OUTPREF "Killing child due to too many signals (%i).\n", MAX_SIGS);
						else if (verbose)
							printf (OUTPREF "Child exiting on signal %d...\n", sig);
						eptrace (PTRACE_KILL, pid, 0, 0);
						break;
					}
				}
			}
			wait(&status);
		}
	} else {	// Original tracer (no -b)
		while (!WIFEXITED(status)) {
			if (WIFSTOPPED(status)) {
				eptrace(PTRACE_GETREGS,pid,0,&regs);
				if ((regs.eip >= codeLow) && (regs.eip <= codeHigh))
					ipBuf[regs.eip-codeLow]++;
				eptrace(PTRACE_SINGLESTEP,pid,0,0);
			}
			wait(&status);
		}
	}

	// print results
	for (loop = 0; loop < codeSize; loop++) {
		if (ipBuf[loop] > 0)
			fprintf(logF,"%08lx = %ld\n",codeLow+loop,ipBuf[loop]);
	}

	if (BP_file)
		free (opBuf);
	free(ipBuf);
	fclose(logF);
}

int main(int argc,char** argv)
{
	static const CmdlineOpts cmdOpt[] = {
		{"help",        'h', CMD_NO_ARG },
		{"logfile",     'o', CMD_ARG_REQ},
		{"text",        't', CMD_NO_ARG },
		{"verbose",     'v', CMD_NO_ARG },
		{"version",      1,  CMD_NO_ARG },
		{"breakpoints", 'b', CMD_ARG_REQ},
		{}	// Doesn't trigger a warning this way (and seems to run fine)
	};

	int argPos = ParseCmdline(argc,argv,cmdOpt,"ho:tvb:",HandleOpts);
	if (argPos == argc) {
		fprintf(stderr,OUTPREF "error: no program specified\n"
			"type 'ptrace -h' for help\n");
		return EXIT_FAILURE;
	}
	TraceFile(argv+argPos);

	return EXIT_SUCCESS;
}

