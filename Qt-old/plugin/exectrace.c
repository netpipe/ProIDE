/* ExecTrace by Trent Waddington 11 May 2000

   ExecTrace is a linux only debugging tool.  It uses ptrace to track the
   execution of a child program compiled with debugging info.  To use
   ExecTrace you would do something similar to the following:

     gcc testprog.c -o testprog -ggdb
     exectrace ./testprog 
  
   and ExecTrace will generate a (big) file called "exec.log" which
   contains the source lines of your program as it executes.  You may wish
   to tail -f this file as the program is running (perhaps in another
   window of your X session).  This can be useful when you have a program
   that segfaults for no apparent reason and you want to know 1) where the 
   segfault occured and 2) what code was run leading up to the segfault.     

   This code is distributed under the GNU Public Licence (GPL) version 2.  
   See http://www.gnu.org/ for further details of the GPL.  If you do not
   have a web browser you can read the LICENSE file in this directory.
*/

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ptrace.h>
#include <stdio.h>
#include <linux/user.h>

#include <bfd.h>
#include <stdio.h>
#include <stdlib.h>

bfd *bf;
char lastline[1024];

char *get_line_for_vma(bfd_vma src) {
CONST char *filename;
CONST char *functionname;
unsigned int line;
asection *p;
static asymbol **syms=NULL;

  for (p = bf->sections; p != NULL; p = p->next) {
    /*printf("processing section %s vma %08X size %i\n",p->name,p->vma,p->_cooked_size); */
    if (src>=p->vma && src<=p->vma+p->_cooked_size) {
      int off = src-p->vma;
      if (bfd_find_nearest_line (bf, p, syms, off, &filename,
                               &functionname, &line)) {
        static char theline[1024];
        int l;
	FILE *f = fopen(filename,"r");
        sprintf(theline,"line %i in function %s of %s\n",line,functionname,filename);
        if (strncmp(theline,lastline,1024)) { 
          strncpy(lastline,theline,1024);
  	  if (!f) {
	    return theline;
	  }
          for (l=0; l<line; l++)
            fgets(theline,1024,f);
          fclose(f);
	  strtok(theline,"\r\n");
          return theline;
        } 
	if (f) fclose(f);
      }
    }
  }
  return NULL;
}

int initfile(char *fname) {
bfd_error_type t;
  bfd_init();
  bf = bfd_openr(fname,0);
  if (bf) {
    char **matching;
    if (bfd_check_format_matches (bf, bfd_object, &matching))
    {
      return 1;
    }

    if (bfd_get_error () == bfd_error_file_ambiguously_recognized)
    {
      printf("ambiguously recognized\n");
    }
    bfd_close(bf);
  } else {
    t = bfd_get_error();
    printf("error %s\n",bfd_errmsg(t));
  }
  return 0;
}

void main(int argc,char **argv) {
int status;
int pid;
struct user_regs_struct regs;
FILE *log;
char *logfile = "exec.log";
int lfs = 0;
  if (argc<2) {
    printf("usage: exectrace [-l logfile (defaults to exec.log)] <program> <arguments>\n");
    exit(0);
  }
  if (*argv[1]=='-') {
    if (argv[1][1]!='l') {
      printf("unknown option %s\n",argv[1]);
      exit(0);
    }
    logfile=argv[2]; 
    lfs=1;
  }
  log = fopen(logfile,"w");  
  initfile(lfs?argv[3]:argv[1]);
  if ((pid=fork())==0) {
    ptrace(PTRACE_TRACEME,0,0,0);
    execvp(lfs?argv[3]:argv[1],lfs?argv+3:argv+1);
  }
  wait(&status);
  while (!WIFEXITED(status)) {
    if (WIFSTOPPED(status)) {
      char *line;
      ptrace(PTRACE_GETREGS,pid,0,&regs);
      line = get_line_for_vma(regs.eip); 
      if (line)
        fprintf(log,"%s\n",line);
      ptrace(PTRACE_SINGLESTEP,pid,0,0);
    }
    wait(&status);
  }
  fclose(log);
}
