#include "test_schedule.h"
#include "proc.h"
#include "printk.h"

extern char current[];

char task_test_char[NR_TASKS];
/*
这是一个字符数组，用于存储每个任务的字符标识。每个任务被分配一个唯一的字符标识，从 'A' 开始，
然后逐渐增加，直到 'Z'，然后再从 'a' 开始递增，以确保每个任务有一个唯一的标识。
*/
uint64 task_test_priority[NR_TASKS];
uint64 task_test_counter[NR_TASKS];
uint64 task_test_index;
char task_test_output[(NR_TASKS+1) * 13];

void test_init(int num_tasks) {
    task_test_index = 0;
    char init_char = 'A';
    uint64 priority = PRIORITY_SEED;
    uint64 counter = COUNTER_SEED;

    for (char i = 0; i < num_tasks; i++){
        if(i < 26){
            task_test_char[i] = init_char + i;
        }
        else{
            task_test_char[i] = init_char + (i - 26) + 32;
        }

        //这行代码用于生成一系列伪随机的任务优先级。它是一个伪随机数生成器的简单实现
        priority = (priority * 5) % 97;
        task_test_priority[i] = priority;

        counter = (counter * 5) % 193;
        task_test_counter[i] = counter % 13 + 1;

        task_test_output[i] = '\0';
    }
    task_test_output[num_tasks]  = '\0';
}

void schedule_test(){
    return;
    /**
     * empty for null test
    */
}