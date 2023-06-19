#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}
//exec2
int
sys_exec2(void)
{
  char *path, *argv[MAXARG];
  int i,stacksize;
  uint uargv, uarg;

  if(argstr(0, &path) < 0 || argint(1, (int*)&uargv) < 0){
    return -1;
  }
  memset(argv, 0, sizeof(argv));
  for(i=0;; i++){
    if(i >= NELEM(argv))
      return -1;
    if(fetchint(uargv+4*i, (int*)&uarg) < 0)
      return -1;
    if(uarg == 0){
      argv[i] = 0;
      break;
    }
    if(fetchstr(uarg, &argv[i]) < 0)
      return -1;
    if(argint(2, &stacksize)<0)
      return -1;
  }
  return exec2(path, argv,stacksize);
}

//setmemorylimit
int
sys_setmemorylimit(void)
{
  int pid, limit;
  if(argint(0, &pid) < 0 || argint(1, &limit) < 0)
    return -1;

  return setmemorylimit(pid,limit);
}

//print_pinfo
int
sys_print_pinfo(void){
  
  int pid;
  if(argint(0, &pid) < 0)
    return -1;

	return print_pinfo();
}

int sys_thread_create(void)
{
  int thread, routine, arg;

  if(argint(0, &thread) < 0)
    return -1;

  if(argint(1, &routine) < 0)
    return -1;

  if(argint(2, &arg) < 0)
    return -1;

  return thread_create((thread_t*)thread, (void*)routine, (void*)arg);
}

int sys_thread_exit(void)
{
  int retval;
  
  if(argint(0, &retval) < 0)
    return -1;

  thread_exit((void*)retval);
  return 0;
}

int sys_thread_join(void)
{
  int thread, retval;

  if(argint(0, &thread) < 0)
    return -1;

  if(argint(1, &retval) < 0)
    return -1;
    
  return thread_join((thread_t)thread, (void**)retval);
}
