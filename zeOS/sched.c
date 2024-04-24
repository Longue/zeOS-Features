/*
 * sched.c - initializes struct for task 0 anda task 1
 */

#include <sched.h>
#include <mm.h>
#include <io.h>

union task_union task[NR_TASKS]
  __attribute__((__section__(".data.task")));

#if 0
struct task_struct *list_head_to_task_struct(struct list_head *l)
{
  return list_entry( l, struct task_struct, list);
}
#endif

extern struct list_head blocked;
struct list_head readyqueue;
struct list_head freequeue;
struct task_struct *idle_task;
int QUANTUM_TIKS = 0;

/* get_DIR - Returns the Page Directory address for task 't' */
page_table_entry * get_DIR (struct task_struct *t) 
{
	return t->dir_pages_baseAddr;
}

/* get_PT - Returns the Page Table address for task 't' */
page_table_entry * get_PT (struct task_struct *t) 
{
	return (page_table_entry *)(((unsigned int)(t->dir_pages_baseAddr->bits.pbase_addr))<<12);
}


// Devuelve el valor del Quantum del taskstruct especificado
int get_quantum (struct task_struct *t) {
	return t->QUANTUM;
}

// Actualiza el valor del Quatum del taskstruct especificado
void set_quantum (struct task_struct *t, int new_quantum) {
	t->QUANTUM = new_quantum;
}

// Decrementa un tick al quantum
void update_sched_data_rr() {
  --QUANTUM_TIKS;
  if (QUANTUM_TIKS < 0) QUANTUM_TIKS = 0;
}

// Decide si se debe cambiar el proceso (devuelve 1)
int needs_sched_rr () {
	if (QUANTUM_TIKS == 0 && !list_empty(&readyqueue))
		return 1;
	return 0;
}


// Actualiza el estado del proceso a uno nuevo
void update_process_state_rr (struct task_struct *t, struct list_head *dst_queue) {
  if (t->STATE != ST_RUN)
    list_del(&t->list);

  if (dst_queue != NULL) {
    list_add_tail(&t->list, dst_queue);
    if (dst_queue == &readyqueue) {
      t->STATE = ST_READY;
    }
    else
      t->STATE = ST_BLOCKED;
  }
  else
    t->STATE = ST_RUN;
}

// Selecciona el proximo proceso a ejecutar
void sched_next_rr() {

	struct task_struct *new;

	// Obtenemos el primer proceso en ready
	if (!list_empty(&readyqueue)) {
		struct list_head *new_head = list_first(&readyqueue);
		new = list_head_to_task_struct(new_head);
		update_process_state_rr(new, NULL);
	}
	else
		new = idle_task;

	new->STATE = ST_RUN;
  	QUANTUM_TIKS = new->QUANTUM;
	task_switch((union task_union *) new);
}

void schedule() {
	update_sched_data_rr();
	if (needs_sched_rr()) {
		update_process_state_rr(current(), &readyqueue);
		sched_next_rr();
	}
}

int allocate_DIR(struct task_struct *t) 
{
	int pos;

	pos = ((int)t-(int)task)/sizeof(union task_union);

	t->dir_pages_baseAddr = (page_table_entry*) &dir_pages[pos]; 

	return 1;
}

void cpu_idle(void)
{
	__asm__ __volatile__("sti": : :"memory");

	while(1)
	{
	;
	}
}

struct task_struct *list_head_to_task_struct(struct list_head *l) {
 	return (struct task_struct *) ( (unsigned long) l & 0xFFFFF000);
}

void register_stuff(unsigned long *curr_kernel_esp, unsigned long new_kernel_esp);

void inner_task_switch(union task_union *t) {

	// Actualizamos la TSS
	tss.esp0 = (unsigned long) &(t->stack[KERNEL_STACK_SIZE]);

	// Actualizamos el registro 0x175 MSR
	writeMSR((unsigned long) &(t->stack[KERNEL_STACK_SIZE]), 0x175);

	// Actualizamos la TLB con la trduccion de direcciones logicas-fisicas del proceso
	set_cr3(get_DIR(&t->task));

	register_stuff(&current()->KERNEL_ESP, (unsigned long) t->task.KERNEL_ESP);

}

void init_idle (void)
{
	// Obtenemos el primer union task_union libre de la cola de libres y lo eliminamos
	struct list_head *e = list_first( &freequeue );
	list_del(e);
	
	struct task_struct *ts = list_head_to_task_struct(e);
	union task_union *pcb = (union task_union *) ts;

	// Asignamos al pcb el pid 0
	ts->PID = 0;
	// Asignameos un quantum
	ts->QUANTUM = DEFAULT_QUANTUM;
	// Inicializamos los desbloqueos pendientes en 0
	ts->pending_unblocks  = 0;
	// No tiene padre
	ts->parent = NULL;
	// Inicializamos su lista de hijos
	INIT_LIST_HEAD(&ts->sons);
	
	// Agregamos un nuevo directorio
	allocate_DIR(ts);

	// Agregamos el codigo que debera ejecutar mas el tope de la pila
	pcb->stack[KERNEL_STACK_SIZE-1] = (unsigned long) &cpu_idle;
	pcb->stack[KERNEL_STACK_SIZE-2] = 0;

	// Almacenamos el puntero del tope de la pila de sistema
	ts->KERNEL_ESP = (unsigned long) &pcb->stack[KERNEL_STACK_SIZE-2];

	idle_task = ts;
}

void init_task1(void)
{
	// Obtenemos el primer union task_union libre de la cola de libres y lo eliminamos
	struct list_head *e = list_first( &freequeue );
	list_del(e); 

	struct task_struct *ts = list_head_to_task_struct(e);
	union task_union *pcb = (union task_union *) ts;
	
	// Asignamos al pcb el pid 1
	ts->PID = 1;
	// Asignamos el estado de RUN
	ts->STATE = ST_RUN;
	// Asignamos un quantum
	ts->QUANTUM = DEFAULT_QUANTUM;
	QUANTUM_TIKS = ts->QUANTUM;
	// Inicializamos los desbloqueos pendientes en 0
	ts->pending_unblocks  = 0;
	// No tiene padre
	ts->parent = NULL;
	// Inicializamos su lista de hijos
	INIT_LIST_HEAD(&ts->sons);

	// Agregamos un nuevo directorio
	allocate_DIR(ts);
	// Le asignamos las paginas de usuario y su traduccion a fisicas
	set_user_pages(ts);

	// Actualizamos la TSS
	tss.esp0 = (unsigned long) &(pcb->stack[KERNEL_STACK_SIZE]);

	// Actualizamos el registro 0x175 MSR
	writeMSR((unsigned long) &(pcb->stack[KERNEL_STACK_SIZE]), 0x175);

	// Actualizamos la TLB con la trduccion de direcciones logicas-fisicas del proceso 1
	set_cr3(get_DIR(ts));
}


void init_sched()
{
	INIT_LIST_HEAD(&freequeue);
	INIT_LIST_HEAD(&readyqueue);
	INIT_LIST_HEAD(&blocked);
	for (int i=0; i < NR_TASKS; ++i)
		list_add_tail(&task[i].task.list, &freequeue); 
}

struct task_struct* current()
{
  int ret_value;
  
  __asm__ __volatile__(
  	"movl %%esp, %0"
	: "=g" (ret_value)
  );
  return (struct task_struct*)(ret_value&0xfffff000);
}

