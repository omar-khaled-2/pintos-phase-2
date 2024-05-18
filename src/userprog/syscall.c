#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "devices/shutdown.h"
#include "threads/init.h"
#include "threads/vaddr.h"
#include "filesys/file.h"
#include "filesys/filesys.h"
#include "userprog/handlers.c"
#include "lib/kernel/list.h"
#include "userprog/calls.c"





static void
syscall_handler(struct intr_frame *frame UNUSED);


bool valid_esp(struct intr_frame *frame){
  
  return valid_in_virtual_memory((int *)frame->esp) || ((*(int *)frame->esp) < 0) || (*(int *)frame->esp) > 12;
}


void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
  lock_init(&file_lock);
  
}

static void
syscall_handler(struct intr_frame *frame UNUSED)
{
  if(!valid_esp(frame))
    exit(-1);

  int esp = *(int *)frame->esp;

  switch (esp){
    case SYS_HALT:
      return halt_handler(frame);
    case SYS_EXIT:
      return exit_handler(frame);
    case SYS_EXEC:
      return execute_handler(frame);
    case SYS_WAIT:
      return wait_handler(frame);
    case SYS_CREATE:
      return create_handler(frame);
    case SYS_REMOVE:
      return remove_handler(frame);
    case SYS_OPEN:
      return open_handler(frame);
    case SYS_FILESIZE:
      return get_file_size_handler(frame);
    case SYS_READ:
      return read_handler(frame);
    case SYS_WRITE:
      return write_handler(frame);
    case SYS_SEEK:
      return seek_handler(frame);
    case SYS_TELL:
      return tell_handler(frame);
    case SYS_CLOSE:
      return close_handler(frame);

  }

}