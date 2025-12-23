#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "userprog/process.h"
#include "threads/vaddr.h"

static void syscall_handler(struct intr_frame*);

void syscall_init(void) { intr_register_int(0x30, 3, INTR_ON, syscall_handler, "syscall"); }

/* 在文件开头添加 */
bool is_user_vaddr(const void *vaddr);
void *pagedir_get_page(uint32_t *pd, const void *uaddr);


// 检查用户输入的地址是否是可以用的
static void
check_user_ptr(const void *uaddr)
{
  struct thread *t = thread_current();
    if (uaddr == NULL ||
        t->pcb == NULL ||
        !is_user_vaddr(uaddr) ||
        pagedir_get_page(t->pcb->pagedir, uaddr) == NULL)
    {
        thread_exit();
    }
}

int sys_write(int fd, const void *buffer, unsigned size) {
    check_user_ptr(buffer);  // 安全检查
    if (fd == 1) {
        putbuf(buffer, size); // 输出到终端
        return size;
    }
    return -1;
}

static void syscall_handler(struct intr_frame* f UNUSED) {
  uint32_t* args = ((uint32_t*)f->esp);

  /*
   * The following print statement, if uncommented, will print out the syscall
   * number whenever a process enters a system call. You might find it useful
   * when debugging. It will cause tests to fail, however, so you should not
   * include it in your final submission.
   */

  /* printf("System call number: %d\n", args[0]); */


  if (args[0] == SYS_EXIT) {
    f->eax = args[1];
    printf("%s: exit(%d)\n", thread_current()->pcb->process_name, args[1]);
    process_exit();
  } else if (args[0] == SYS_WRITE) {
    f->eax = sys_write(args[1], (void *)args[2], args[3]);
  }
}


