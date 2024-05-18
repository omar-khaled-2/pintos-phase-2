#include "userprog/handlers.h"
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
#include "lib/kernel/list.h"
#include "userprog/calls.h"
#include "userprog/process.h"



bool valid_in_virtual_memory(void *val){
  return val != NULL && is_user_vaddr(val) && pagedir_get_page(thread_current()->pagedir, val) != NULL;
}



void halt_handler(struct intr_frame *frame){
  halt();
}


void exit_handler(struct intr_frame *frame)
{
  int status = *((int *)frame->esp + 1);
  if (!is_user_vaddr(status)){
    frame->eax = -1;
    return exit(-1);
  }
  frame->eax = status;
  exit(status);
}


void execute_handler(struct intr_frame *frame)
{
  char *filename = (char *)(*((int *)frame->esp + 1));
  frame->eax = process_execute(filename);
}




void wait_handler(struct intr_frame *frame)
{
  if (!valid_in_virtual_memory((int *)frame->esp + 1))
    exit(-1);
  tid_t tid = *((int *)frame->esp + 1);
  frame->eax = wait(tid);
}


void create_handler(struct intr_frame *frame)
{
  char *file = (char *)*((int *)frame->esp + 1);
  if (!valid_in_virtual_memory(file))
    exit(-1);
  unsigned initial_size = (unsigned)*((int *)frame->esp + 2);
  frame->eax = create(file, initial_size);
}



void remove_handler(struct intr_frame *frame)
{
  char *file = (char *)(*((int *)frame->esp + 1));
  if (!valid_in_virtual_memory(file))
    exit(-1);
  frame->eax = remove(file);
}



void open_handler(struct intr_frame *frame)
{
  char *file = (char *)(*((int *)frame->esp + 1));
  if (!valid_in_virtual_memory(file))
    exit(-1);
  
  frame->eax = open(file);
}




void get_file_size_handler(struct intr_frame *frame)
{
  int file_descriptor = (int)(*((int *)frame->esp + 1));
  struct user_file *f = get_file(file_descriptor);
  if (f == NULL){ 
    frame->eax = -1;
    return;
  }

  lock_acquire(&file_lock);
  frame->eax = file_length(f->file);
  lock_release(&file_lock);
  
}

void read_handler(struct intr_frame *frame)
{
  int file_descriptor = (int)(*((int *)frame->esp + 1));
  char *buffer = (char *)(*((int *)frame->esp + 2));
  if (file_descriptor == 1 || !valid_in_virtual_memory(buffer))
     exit(-1);
  
  unsigned size = *((unsigned *)frame->esp + 3);
  frame->eax = read(file_descriptor, buffer, size);
}

void write_handler(struct intr_frame *frame)
{
  int file_descriptor = *((int *)frame->esp + 1);
  char *buffer = (char *)(*((int *)frame->esp + 2));
  if (file_descriptor == 0 || !valid_in_virtual_memory(buffer))
    exit(-1);
  unsigned size = (unsigned)(*((int *)frame->esp + 3));
  frame->eax = write(file_descriptor, buffer, size);
}


void close_handler(struct intr_frame *frame)
{
  int file_descriptor = (int)(*((int *)frame->esp + 1));
  if (file_descriptor < 2)
    exit(-1);
  
  frame->eax = close(file_descriptor);
}



void tell_handler(struct intr_frame *frame){
  tell(frame);
}

void seek_handler(struct intr_frame *frame){
  seek(frame);
}