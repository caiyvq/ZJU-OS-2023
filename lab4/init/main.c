#include "printk.h"
#include "sbi.h"
#include "defs.h"
#include "test.h"
extern void test();
extern char _stext[];
extern char _srodata[];

int start_kernel() {
    printk("2022");
    printk(" Hello RISC-V\n");
    printk("idle process is running!\n");
    
    // lab1*****************************
    // //思考题3
    // register int val;
    // val = csr_read(sstatus);
    // printk("sstatus: ");
    // printi(val);
    // printk("\n");

    // //思考题4
    // val = csr_read(sscratch);
    // printk("sscratch: ");
    // printi(val);
    // printk("\n");
    // csr_write(sscratch,9);
    // val = csr_read(sscratch);
    // printk("sscratch after write: ");
    // printi(val);
    // printk("\n");
    //**********************************

    //lab3
    //思考题1
    // *_stext = 0;
    // *_srodata = 0;
    // printk("_stext = %ld\n",*_stext);
    // printk("_srodata = %ld\n",*_srodata);

    test(); // DO NOT DELETE !!!

	return 0;
}
