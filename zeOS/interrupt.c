/*
 * interrupt.c -
 */
#include <types.h>
#include <interrupt.h>
#include <segment.h>
#include <hardware.h>
#include <io.h>

#include <zeos_interrupt.h>

Gate idt[IDT_ENTRIES];
Register    idtR;

extern int zeos_ticks;
extern union task_union *idle_task;

char char_map[] =
{
  '\0','\0','1','2','3','4','5','6',
  '7','8','9','0','\'','�','\0','\0',
  'q','w','e','r','t','y','u','i',
  'o','p','`','+','\0','\0','a','s',
  'd','f','g','h','j','k','l','�',
  '\0','�','\0','�','z','x','c','v',
  'b','n','m',',','.','-','\0','*',
  '\0','\0','\0','\0','\0','\0','\0','\0',
  '\0','\0','\0','\0','\0','\0','\0','7',
  '8','9','-','4','5','6','+','1',
  '2','3','0','\0','\0','\0','<','\0',
  '\0','\0','\0','\0','\0','\0','\0','\0',
  '\0','\0'
};

void setInterruptHandler(int vector, void (*handler)(), int maxAccessibleFromPL)
{
  /***********************************************************************/
  /* THE INTERRUPTION GATE FLAGS:                          R1: pg. 5-11  */
  /* ***************************                                         */
  /* flags = x xx 0x110 000 ?????                                        */
  /*         |  |  |                                                     */
  /*         |  |   \ D = Size of gate: 1 = 32 bits; 0 = 16 bits         */
  /*         |   \ DPL = Num. higher PL from which it is accessible      */
  /*          \ P = Segment Present bit                                  */
  /***********************************************************************/
  Word flags = (Word)(maxAccessibleFromPL << 13);
  flags |= 0x8E00;    /* P = 1, D = 1, Type = 1110 (Interrupt Gate) */

  idt[vector].lowOffset       = lowWord((DWord)handler);
  idt[vector].segmentSelector = __KERNEL_CS;
  idt[vector].flags           = flags;
  idt[vector].highOffset      = highWord((DWord)handler);
}

void setTrapHandler(int vector, void (*handler)(), int maxAccessibleFromPL)
{
  /***********************************************************************/
  /* THE TRAP GATE FLAGS:                                  R1: pg. 5-11  */
  /* ********************                                                */
  /* flags = x xx 0x111 000 ?????                                        */
  /*         |  |  |                                                     */
  /*         |  |   \ D = Size of gate: 1 = 32 bits; 0 = 16 bits         */
  /*         |   \ DPL = Num. higher PL from which it is accessible      */
  /*          \ P = Segment Present bit                                  */
  /***********************************************************************/
  Word flags = (Word)(maxAccessibleFromPL << 13);

  //flags |= 0x8F00;    /* P = 1, D = 1, Type = 1111 (Trap Gate) */
  /* Changed to 0x8e00 to convert it to an 'interrupt gate' and so
     the system calls will be thread-safe. */
  flags |= 0x8E00;    /* P = 1, D = 1, Type = 1110 (Interrupt Gate) */

  idt[vector].lowOffset       = lowWord((DWord)handler);
  idt[vector].segmentSelector = __KERNEL_CS;
  idt[vector].flags           = flags;
  idt[vector].highOffset      = highWord((DWord)handler);
}

void clock_handler();
void kbd_handler();
void p_f_handler();
void syscall_handler_sysenter();

void clock_routine() {
  ++zeos_ticks;
  zeos_show_clock();
  schedule();
}

void kbd_routine() {
  char key = inb(0x60);
  if (key & 0x80) {
    char c = char_map[key&0x7F]; 

    // Test task_switch
    //if (c == 'r') task_switch(idle_task);

    if (c != '\0') printc_xy(0, 0, c);
    else printc_xy(0, 0, 'C');
  }
}

void long_to_hex(unsigned long num, char* hexa) {
  char* value = "0123456789ABCDEF";
  for (int i=7; i >= 0; --i) {
    int n = num & 0x0000000f;
    hexa[i] = value[n];
    num = num >> 4;
  }
  hexa[8]='\0';
}

void p_f_routine(unsigned long param, unsigned long eip) {
  char hexa[9];
  long_to_hex(eip, hexa);

  printk("\n");
  printk("Process generates a PAGE FAULT exception at EIP: 0x");
  printk(hexa);
  printk("\n");

  while(1){}
}

void writeMSR(long unsigned reg, long unsigned value);

void setIdt()
{
  //Inicializamos el registro MSR
  writeMSR(__KERNEL_CS, 0x174);
  writeMSR(INITIAL_ESP, 0x175);
  writeMSR(syscall_handler_sysenter, 0x176);

  /* Program interrups/exception service routines */
  idtR.base  = (DWord)idt;
  idtR.limit = IDT_ENTRIES * sizeof(Gate) - 1;
  
  set_handlers();

  /* ADD INITIALIZATION CODE FOR INTERRUPT VECTOR */

  setInterruptHandler(14, p_f_handler, 0);
  setInterruptHandler(32, clock_handler, 0);
  setInterruptHandler(33, kbd_handler, 0);

  //setTrapHandler(0x80, system_call_handler, 3);
  //setTrapHandler(0x80, syscall_handler_sysenter, 3);

  set_idt_reg(&idtR);
}

