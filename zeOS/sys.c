/*
 * sys.c - Syscalls implementation
 */
#include <devices.h>

#include <utils.h>

#include <io.h>

#include <mm.h>

#include <mm_address.h>

#include <sched.h>

#include <errno.h>

extern int zeos_ticks;
extern struct list_head blocked;
extern struct list_head readyqueue;
extern struct list_head freequeue;
extern struct task_struct *idle_task;
int PID = 1000;

#define LECTURA 0
#define ESCRIPTURA 1
#define BUFF_SIZE 512

char lbuff[BUFF_SIZE];

int check_fd(int fd, int permissions)
{
  if (fd!=1) return -9; /*EBADF*/
  if (permissions!=ESCRIPTURA) return -13; /*EACCES*/
  return 0;
}

int sys_ni_syscall()
{
	return -38; /*ENOSYS*/
}

int sys_getpid()
{
	return current()->PID;
}

int ret_from_fork() {
  return 0;
}

int sys_fork()
{

  // creates the child process
  if (list_empty(&freequeue)) return -ENOMEM; //error de no pcbs libres

  // Obtenemos el primer union task_union libre de la cola de libres y lo eliminamos
	struct list_head *e = list_first( &freequeue );
	list_del(e);
  
	struct task_struct *new_ts = list_head_to_task_struct(e);
	union task_union *new_pcb = (union task_union *) new_ts;

  struct task_struct *cur_ts = current();
  union task_union *cur_pcb = (union task_union *) cur_ts;

  copy_data(cur_pcb, new_pcb, sizeof(union task_union));

  allocate_DIR(new_ts);

  page_table_entry *cur_table = get_PT(cur_ts);
  page_table_entry *new_table = get_PT(new_ts);

  // Mapeo las mismas paginas logicas/fisicas del kernel en el nuevo proceso
  for(int page=0; page < NUM_PAG_KERNEL; ++page) {
    set_ss_pag(new_table, page, get_frame(cur_table, page));
  }

  // Mapeo las mismas paginas logicas/fisicas del codigo en el nuevo proceso
  for(int page=PAG_LOG_INIT_CODE; page < PAG_LOG_INIT_CODE+NUM_PAG_CODE; ++page) {
    set_ss_pag(new_table, page, get_frame(cur_table, page));
  }

  // Mapeo las nuevas logicas a nuevas fisicas
  for (int page=PAG_LOG_INIT_DATA; page < PAG_LOG_INIT_DATA+NUM_PAG_DATA; ++page) {
    
    // Pido un nuevo frame fisico
    int page_frame_num = alloc_frame();
    
    if (page_frame_num == -1) {
      
      // Itereo al reves para liberar todos los frames y eliminar las traducciones fisicas
      for (int del_page=page; page >= PAG_LOG_INIT_DATA; --page) {
        free_frame(get_frame(new_table, del_page));
        del_ss_pag(new_table, del_page);
      }

      // Revierto lo cambios y devuevlo el error de falta de memoria fisica
      list_add_tail(e, &freequeue);
      return -EAGAIN; // error no frames fisicos
    }

    // Creo la asociacion logica/fisica
    set_ss_pag(new_table, page, page_frame_num);

  }

  // Copiamos el contenido de data a una nuevas paginas logicas (que no se usaron)
  // y las mapeamos a las fisicas anteriomente obtenidas
  for (int page=PAG_LOG_AFTER_CODE; page < PAG_LOG_AFTER_CODE+NUM_PAG_DATA; ++page) {
    set_ss_pag(cur_table, page, get_frame(new_table, page-NUM_PAG_CODE-NUM_PAG_DATA));
  }

  // Duplicamos data
  copy_data((void *) L_USER_START, (void *) L_AFTER_CODE, NUM_PAG_DATA*PAGE_SIZE); 

  // Deshacemos los mapeos y volvmeos actualizar la TLB
  for (int page=PAG_LOG_AFTER_CODE+NUM_PAG_DATA-1; page >= PAG_LOG_AFTER_CODE; --page) {
    del_ss_pag(cur_table, page);
  }
  
  set_cr3(get_DIR(cur_ts));

  // NEW_PID
  new_ts->PID = ++PID;
  new_ts->STATE = ST_READY;
  new_ts->QUANTUM = cur_ts->QUANTUM;
  new_ts->pending_unblocks = 0;
  // Le guardo la direccion al list head del padre
  new_ts->parent = cur_ts;
  // Inicializo la lista de hijos del hijo
  INIT_LIST_HEAD(&new_ts->sons);

  // Añado el hijo a la lista de hijos del padre
  list_add(&new_ts->anchor_sons_list, &cur_ts->sons);

  new_pcb->stack[KERNEL_STACK_SIZE-18] = (unsigned long) &ret_from_fork;
  new_pcb->stack[KERNEL_STACK_SIZE-19] = 0;

  new_ts->KERNEL_ESP = (unsigned long) &new_pcb->stack[KERNEL_STACK_SIZE-19];

  list_add_tail(&new_ts->list, &readyqueue);
  return new_ts->PID;
}

void transfer_sons() {
  struct list_head* s;

  while (!list_empty(&current()->sons)) {
    s = list_first(&current()->sons);
    list_del(s);
    list_add(s, &idle_task->sons);
    list_head_to_task_struct(s)->parent = idle_task;
  }
}

void sys_exit() {

  // Liberamos el PCB del proceso
  struct task_struct *cur_ts = current();
  list_add_tail(&cur_ts->list, &freequeue);

  // Elimino el hijo actual de la lista de hijos de su padre
  if (current()->PID != 1) {
    list_del(&cur_ts->anchor_sons_list);
  }
  
  current()->PID = -1;

  // Le paso los hijos al proceso idle
  transfer_sons();

  page_table_entry *cur_table = get_PT(cur_ts);

  // Liberamos los frames correspondientes a la data de usuario
  for(int page=USER_FIRST_PAGE; page < USER_FIRST_PAGE+NUM_PAG_DATA; ++page) {
    free_frame(get_frame(cur_table, page));
    del_ss_pag(cur_table, page);
  }

  list_add_tail(&current()->list, &freequeue);
  sched_next_rr();
}

void sys_block() {
  if (current()->pending_unblocks > 0) {
    --current()->pending_unblocks;
  }
  else {
    //Lo añade a la lista de bloqueados siguiendo una FIFO
    update_process_state_rr(current(), &blocked);
    sched_next_rr();
  }
}

// Si el proceso actual posee un hijo con este pid, devuelve un puntero al proceso, sino uno a null
struct task_struct* get_son(int pid) {
  struct list_head* s = list_first(&current()->sons);
  list_for_each(s, &current()->sons) {
    struct task_struct *son = list_head_to_task_struct(s);
    
    if (son->PID == pid) {
      return son;
    }
  }
  return NULL;
}

int sys_unblock(int pid) {

  struct task_struct *son = get_son(pid);

  if (son != NULL) {
    if (son->STATE == ST_BLOCKED) {
      update_process_state_rr(son, &readyqueue);
    }
    else {
      ++son->pending_unblocks;
    }
    return 0;
  }

  return -1;
}

int sys_gettime() {
  return zeos_ticks;
}

int sys_write(int fd, void* buffer, int size) {
  int ret = check_fd(fd, ESCRIPTURA);
  if (ret != 0) return ret;
  if (buffer == NULL) return -EFAULT; //EFAULT
  if (size < 0) return  -EINVAL;
  if (!access_ok(VERIFY_READ, buffer, size)) return -EFAULT;
  
  int bytes = size;
  while (bytes > BUFF_SIZE) {
    copy_from_user(buffer, lbuff, BUFF_SIZE);
    ret = sys_write_console(lbuff, BUFF_SIZE);
    bytes -= ret;
    buffer += ret;
  }
  if (bytes > 0) {
    copy_from_user(buffer, lbuff, bytes);
    ret = sys_write_console(lbuff, bytes);
    bytes -= ret;
  }
  return (size-bytes);
}