// trap.c 
#include "clock.h"
#include "printk.h"
#include "proc.h"
void trap_handler(unsigned long scause, unsigned long sepc) {
    // 通过 `scause` 判断trap类型
    // 如果是interrupt 判断是否是timer interrupt
    // 如果是timer interrupt 则打印输出相关信息, 并通过 `clock_set_next_event()` 设置下一次时钟中断
    // `clock_set_next_event()` 见 4.3.4 节
    // 其他interrupt / exception 可以直接忽略

    // YOUR CODE HERE
    if (scause >> 63) // 最高位为1就是interrupt
    {
        unsigned long rest = scause - 0x8000000000000000;
        if (rest == 5)// 剩余位表示5时是timer interrupt
        { 
           //printk("kernel is running!\n[S] Supervisor Mode Timer Interrupt\n");
            clock_set_next_event();
            //lab2
            do_timer();
        }
    }
}