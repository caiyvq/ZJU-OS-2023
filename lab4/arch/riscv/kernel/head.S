.extern start_kernel

    .section .text.init
    .globl _start
_start:

    # 内核引导部分的代码
    
    #la sp, boot_stack_top
    #physical_start + opensbi_size
    #start of kernel
    li sp, 0x80200000

    #lab3------------
    jal ra, setup_vm
    
    jal ra, relocate
    #----------------
    
    #lab2-----------
    jal ra, mm_init

    #lab3-----
    jal ra, setup_vm_final
    #---------

    jal ra, task_init
    #---------------
    
    # 时钟中断部分的代码
    # ------------------

        # set stvec = _traps
        la a0, _traps
        csrw stvec, a0

    # ------------------

        # set sie[STIE] = 1
        li a0, 1<<5
        csrw sie, a0

    # ------------------

        # set first time interrupt
        jal ra, clock_set_next_event

    # ------------------

        # set sstatus[SIE] = 1
        li a0, 1<<1
        csrw sstatus, a0

    # ------------------

   
    # ------------------
    jal x0, start_kernel
    # ------------------

    .globl relocate
relocate:
    # set ra = ra + PA2VA_OFFSET
    # set sp = sp + PA2VA_OFFSET (If you have set the sp before)
 
    #   YOUR CODE HERE   #
    #addi立即数最大0x7ff
    addi t0, zero, 0xff
    slli t0, t0, 8
    addi t0, t0, 0xff
    slli t0, t0, 8
    addi t0, t0, 0xff
    slli t0, t0, 8
    addi t0, t0, 0xdf
    slli t0, t0, 8
    addi t0, t0, 0x80
    slli t0, t0, 24
    #t0 == 0xffffffdf80000000 (PA2VA_OFFSET)
    
    add ra, ra, t0
    add sp, sp, t0

    # set satp with early_pgtbl
#  63      60 59                  44 43                                0
#  ---------------------------------------------------------------------
# |   MODE   |         ASID         |                PPN                |
#  ---------------------------------------------------------------------
    #   YOUR CODE HERE   #
    #MODE = 8
    addi t2, zero, 8
    #ASID ( Address Space Identifier ) ： 此次实验中直接置 0 即可。
    slli t2, t2, 60
    #PPN ( Physical Page Number ) ：顶级页表的物理页号。我们的物理页的大小为 4KB， PA >> 12 == PPN。
    la t1, early_pgtbl
    sub t1, t1, t0
    srli t1, t1, 12
    add t1, t1, t2
    csrw satp, t1
    
    # flush tlb
    sfence.vma zero, zero

    # flush icache
    fence.i

    ret


    .section .bss.stack
    .globl boot_stack
boot_stack:
    .space 4096# <-- change to your stack size

    .globl boot_stack_top
boot_stack_top: