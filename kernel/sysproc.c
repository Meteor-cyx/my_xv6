#include "types.h"
#include "riscv.h"
#include "param.h"
#include "defs.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;


  argint(0, &n);
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// #ifdef LAB_PGTBL
int sys_pgaccess(void){
    uint64 vaddr;//起始虚拟地址
    int num;     //遍历页数目
    uint64 res_addr;  //用户存储返回结果的地址
    argaddr(0, &vaddr);
    argint(1, &num);
    argaddr(2, &res_addr);
    uint64 mask = 0;  //记录掩码
    struct proc* p = myproc();  //获取当前进程
    pagetable_t pagetable =  p->pagetable;  //获取进程的页表
    pte_t* pte;  //一会接收页表项
    for(int i = 0; i < num; i++){
        pte = (pagetable,vaddr+i*PGSIZE,0);
        if(*pte & PTE_A){
            *pte &= ~PTE_A;//清空pte_a位
            mask |= (1L<<i);
        }
    }
    copyout(pagetable,res_addr,(char*)&mask,sizeof(uint64));
    return 0;
}
// #endif

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}
