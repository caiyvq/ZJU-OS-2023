// arch/riscv/kernel/vm.c
#include "defs.h"
#include "printk.h"
#include "vm.h"
#include "mm.h"
#include<string.h>

/* early_pgtbl: 用于 setup_vm 进行 1GB 的 映射。 */
unsigned long  early_pgtbl[512] __attribute__((__aligned__(0x1000)));//page table entry

void setup_vm(void) {
    /* 
    1. 由于是进行 1GB 的映射 这里不需要使用多级页表 
    2. 将 va 的 64bit 作为如下划分： | high bit | 9 bit | 30 bit |
        high bit 可以忽略
        中间9 bit 作为 early_pgtbl 的 index
        低 30 bit 作为 页内偏移 这里注意到 30 = 9 + 9 + 12， 即我们只使用根页表， 根页表的每个 entry 都对应 1GB 的区域。 
    3. Page Table Entry 的权限 V | R | W | X 位设置为 1
    */
    memset(early_pgtbl, 0x0, PGSIZE);
    uint64 pa = PHY_START;
    //等值映射
    uint64 va = pa;
    //vpn2作为index
    int index = (va >> 30) & 0x1ff;
    //ppn2转化为pte
    early_pgtbl[index] = ((pa >> 30) << 28) | 0b1111;

    //映射到direct mapping area
    va = pa + PA2VA_OFFSET;
    index = (va >> 30) & 0x1ff;
    early_pgtbl[index] = ((pa >> 30) << 28) | 0b1111;
    printk("setup_vm done.\n");

}

/* swapper_pg_dir: kernel pagetable 根目录， 在 setup_vm_final 进行映射。 */
unsigned long  swapper_pg_dir[512] __attribute__((__aligned__(0x1000)));

extern char _stext[];
extern char _etext[];
extern char _srodata[];
extern char _erodata[];
extern char _sdata[];
extern char _edata[];


void setup_vm_final(void) {
    uint64 *pgtbl;
    uint64 va;
    uint64 pa;
    uint64 sz;
    uint64 perm;
    memset(swapper_pg_dir, 0x0, PGSIZE);

    // No OpenSBI mapping required

    // mapping kernel text X|-|R|V
    va = (uint64)_stext;
    pa = va - PA2VA_OFFSET;
    sz = _etext - _stext;
    perm = 0b1011;
    create_mapping(swapper_pg_dir, va, pa, sz, perm);

    // mapping kernel rodata -|-|R|V
    va = (uint64)_srodata;
    pa = va - PA2VA_OFFSET;
    sz = _erodata - _srodata;
    perm = 0b11;
    create_mapping(swapper_pg_dir, va, pa, sz, perm);

    // mapping other memory -|W|R|V
    va = (uint64)_sdata;
    pa = va - PA2VA_OFFSET;
    sz = _edata - _sdata;
    perm = 0b111;
    create_mapping(swapper_pg_dir, va, pa, sz, perm);


    // set satp with swapper_pg_dir

    // YOUR CODE HERE
    csr_write(satp, (uint64)swapper_pg_dir);

    // flush TLB
    asm volatile("sfence.vma zero, zero");

    // flush icache
    asm volatile("fence.i");
    return;
}

/**** 创建多级页表映射关系 *****/
/* 不要修改该接口的参数和返回值 */
void create_mapping(uint64 *pgtbl, uint64 va, uint64 pa, uint64 sz, uint64 perm) {
    /*
    pgtbl 为根页表的基地址
    va, pa 为需要映射的虚拟地址、物理地址
    sz 为映射的大小，单位为字节
    perm 为映射的权限 (即页表项的低 8 位)

    创建多级页表的时候可以使用 kalloc() 来获取一页作为页表目录
    可以使用 V bit 来判断页表项是否存在
    */
   uint64 va_end = va + sz;
   while (va < va_end) 
   {
        uint64 *page;
        uint64 *table;

        // vpn2
        uint64 vpn2 = ((va)>>30) & 0x1ff;
        //if not valid, alloc a new page
        if ((pgtbl[vpn2] & 1) == 0) 
        {
            page = (uint64*)kalloc();
            pgtbl[vpn2] = (1 | ((uint64)page >> 12) << 10);
        }
        table = &(pgtbl[vpn2]);

        // vpn1
        uint64 vpn1 = ((va)>>21) & 0x1ff;
        if ((table[vpn1] & 1) == 0) 
        {
            page = (uint64*)kalloc();
            table[vpn1] = (1 | ((uint64)page >> 12) << 10);
        }
        table = &(table[vpn1]);

        // vpn0
        uint64 vpn0 = ((va)>>12) & 0x1ff;
        if ((table[vpn0] & 1) == 0) 
        {
            page = (uint64*)kalloc();
            table[vpn0] = (1 | ((uint64)page >> 12) << 10);
        }
        table[vpn0] = ((pa >> 12) << 10) | perm;

        va += PGSIZE;
        pa += PGSIZE;
    }
}