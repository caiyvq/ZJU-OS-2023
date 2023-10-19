.extern start_kernel

    .section .text.init
    .globl _start
_start:

    # 内核引导部分的代码
    
    la sp, boot_stack_top
    
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

    .section .bss.stack
    .globl boot_stack
boot_stack:
    .space 4096 # <-- change to your stack size

    .globl boot_stack_top
boot_stack_top: