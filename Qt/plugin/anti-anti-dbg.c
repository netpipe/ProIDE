/* A basic linux lkm to allow debugging and tracing of binarys    */
/* that uses anti-debugging technique like                        */
/* if (ptrace(PTRACE_TRACEME..		         		  */
/* to stop the debugger or tracer	                          */

/* just #gcc -c anti-anti-dbug.c ; insmod anti-anti-dbug.o          */
/* then strace or debug the protected binary			 */

/* by Dalnet SLACKo   slacko@mail.ru  */

#define __KERNEL__
#define MODULE

#include <linux/sched.h>
#include <linux/config.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <sys/syscall.h>
#include <asm/uaccess.h>

extern void *sys_call_table[];
int i=0;
long int(*saved)(int,pid_t,void *,void *);
	
long int _ptrace(int a,pid_t b,void * c,void * d) 
	{
	   if(a == 0) {
		i++;
		if (i == 2) {
		i = 0;
		return 0;
		  }
		} 
    	   return saved(a,b,c,d); 
	}

int init_module() {
	saved = sys_call_table[SYS_ptrace];
	sys_call_table[SYS_ptrace] = _ptrace;
	return 0;
	}
void cleanup_module() {
	sys_call_table[SYS_ptrace] = saved;
	}

 
