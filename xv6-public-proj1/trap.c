#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "x86.h"
#include "traps.h"
#include "spinlock.h"

// Interrupt descriptor table (shared by all CPUs).
struct gatedesc idt[256];
extern uint vectors[];  // in vectors.S: array of 256 entry pointers
struct spinlock tickslock;
uint ticks;

void
tvinit(void)
{
  int i;

  for(i = 0; i < 256; i++)
    SETGATE(idt[i], 0, SEG_KCODE<<3, vectors[i], 0);
  SETGATE(idt[T_SYSCALL], 1, SEG_KCODE<<3, vectors[T_SYSCALL], DPL_USER);
  SETGATE(idt[T_SCHEDULELOCK],1, SEG_KCODE<<3, vectors[T_SCHEDULELOCK],DPL_USER);
  SETGATE(idt[T_SCHEDULEUNLOCK],1, SEG_KCODE<<3, vectors[T_SCHEDULEUNLOCK],DPL_USER);
  initlock(&tickslock, "time");
}

//idt를 로드하는 함수
void
idtinit(void)
{
  lidt(idt, sizeof(idt));
}

//PAGEBREAK: 41
//인터럽트가 발생될때 호출되는 함수로 인터럽트 번호(trapno)에 따라 처리된다. 시스템콜에 대한 처리도 포함
void
trap(struct trapframe *tf)
{
  if(tf->trapno == T_SYSCALL){
    if(myproc()->killed)
      exit();
    myproc()->tf = tf;
    syscall();
    if(myproc()->killed)
      exit();
    return;
  }

  switch(tf->trapno){
  case T_IRQ0 + IRQ_TIMER:
    if(cpuid() == 0){
      acquire(&tickslock);
      ticks++;
      //프로세스 ticks 증가
      if(myproc()) myproc()->ticks++;
      wakeup(&ticks);
      release(&tickslock);
    }
    lapiceoi();
    break;
  case T_IRQ0 + IRQ_IDE:
    ideintr();
    lapiceoi();
    break;
  case T_IRQ0 + IRQ_IDE+1:
    // Bochs generates spurious IDE1 interrupts.
    break;
  case T_IRQ0 + IRQ_KBD:
    kbdintr();
    lapiceoi();
    break;
  case T_IRQ0 + IRQ_COM1:
    uartintr();
    lapiceoi();
    break;
  case T_IRQ0 + 7:
  case T_IRQ0 + IRQ_SPURIOUS:
    cprintf("cpu%d: spurious interrupt at %x:%x\n",
            cpuid(), tf->cs, tf->eip);
    lapiceoi();
    break;

  case T_SCHEDULELOCK:
    schedulerLock(2018008959);
    break;
  case T_SCHEDULEUNLOCK:
    schedulerUnlock(2018008959);
    break;

  //PAGEBREAK: 13
  default:
    if(myproc() == 0 || (tf->cs&3) == 0){
      // In kernel, it must be our mistake.
      cprintf("unexpected trap %d from cpu %d eip %x (cr2=0x%x)\n",
              tf->trapno, cpuid(), tf->eip, rcr2());
      panic("trap");
    }
    // In user space, assume process misbehaved.
    cprintf("pid %d %s: trap %d err %d on cpu %d "
            "eip 0x%x addr 0x%x--kill proc\n",
            myproc()->pid, myproc()->name, tf->trapno,
            tf->err, cpuid(), tf->eip, rcr2());
    myproc()->killed = 1;
  }

  // Force process exit if it has been killed and is in user space.
  // (If it is still executing in the kernel, let it keep running
  // until it gets to the regular system call return.)
  if(myproc() && myproc()->killed && (tf->cs&3) == DPL_USER)
    exit();

  // Force process to give up CPU on clock tick.
  // If interrupts were on while locks held, would need to check nlock.

	if(myproc() && myproc()->state == RUNNING && tf->trapno == T_IRQ0+IRQ_TIMER) 
  {
    if(ticks >= 100){   //100ticks가 되면 priorityBoosting을 실행한다.
      priorityBoosting();
      yield();
    }
    if(myproc()->scheduler_lock == 0)
    {
      if(ticks >= 100) 
      {
        ticks = 0;
        priorityBoosting();
      }
      else
      {
        if(myproc()->ticks >= ((2*myproc()->queue) + 4)) //프로세스가 2n+4 ticks을 모두 사용하면 다음 큐레벨로 이동한다. 
        {
          if(myproc()->queue == 2)
          {
            if(myproc()->priority == 0) myproc()->priority = 0;   //priority 가 0인 경우 0으로 유지
            else myproc()->priority -= 1;   //time quantum을 모두 사용한 경우 priority 1 감소
          }
          else myproc()->queue += 1;
          myproc()->ticks = 0;
        }
      }
      yield();
    }
    
    
    else if(myproc()->scheduler_lock == 1)
    {
      if(ticks >= 100)    //global tick 이 100 이상인 경우
      {
        myproc()->queue = 0;    //L0 큐로 보낸다
        myproc()->scheduler_lock= 0;
        priorityBoosting();     //priorityboosting을 하여 우선실행되도록한다.
        yield();
      }
    }
	}

  // Check if the process has been killed since we yielded
  if(myproc() && myproc()->killed && (tf->cs&3) == DPL_USER)
    exit();
}
