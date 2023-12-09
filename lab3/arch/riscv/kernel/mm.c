#include "defs.h"
#include "string.h"
#include "mm.h"

#include "printk.h"

extern char _ekernel[];

struct {
    struct run *freelist;
} kmem;

uint64 kalloc() {
    struct run *r;

    r = kmem.freelist;
    kmem.freelist = r->next;
    
    memset((void *)r, 0x0, PGSIZE);
    return (uint64) r;
}

void kfree(uint64 addr) {
    struct run *r;

    // PGSIZE align 
    addr = addr & ~(PGSIZE - 1);

    memset((void *)addr, 0x0, (uint64)PGSIZE);

    r = (struct run *)addr;
    r->next = kmem.freelist;
    kmem.freelist = r;

    return ;
}

void kfreerange(char *start, char *end) {
    char *addr = (char *)PGROUNDUP((uint64)start);
    for (; (uint64)(addr) + PGSIZE <= (uint64)end; addr += PGSIZE) {
        kfree((uint64)addr);
    }
}

//mm.c 中初始化的函数接收的起始结束地址需要调整为虚拟地址。
void mm_init(void) {
    // kfreerange(_ekernel, (char *)PHY_END);
    kfreerange(_ekernel, (char *)(PHY_END + PA2VA_OFFSET));
    printk("mm_init done!\n");
}

