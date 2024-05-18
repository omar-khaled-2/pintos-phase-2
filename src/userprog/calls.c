#include "userprog/calls.h"
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
#include "userprog/handlers.h"
#include "lib/kernel/list.h"
#include "userprog/process.h"


#include <string.h>




void exit(int status)
{
    struct thread *current = thread_current();
    char * name = thread_current()->name;
    char * save_ptr;
    char * executable = strtok_r (name, " ", &save_ptr);
    thread_current()->exit_status = status;
    printf("%s: exit(%d)\n",executable,status);
    thread_exit();
}


void halt(void)
{
  printf("(halt) begin\n");
  shutdown_power_off();
}


tid_t wait(tid_t tid)
{
  return process_wait(tid);
}


int create(const char *filename, unsigned initial_size)
{
  lock_acquire(&file_lock);
  int result = filesys_create(filename, initial_size);
  lock_release(&file_lock);
  return result;
}


int remove(const char *filename)
{

  lock_acquire(&file_lock);
  int result = filesys_remove(filename);
  lock_release(&file_lock);
  return result;
}


int open(const char *filename)
{
  static unsigned long file_descriptor = 2;
  lock_acquire(&file_lock);
  struct file *f = filesys_open(filename);
  lock_release(&file_lock);

  if (f == NULL)
    return -1;


  struct user_file *user_file = (struct user_file *)malloc(sizeof(struct user_file));
  int user_file_descriptor = file_descriptor;
  user_file->file_descriptor = file_descriptor;
  user_file->file = f;
  
  lock_acquire(&file_lock);
  file_descriptor++;
  lock_release(&file_lock);


  
  list_push_back(&thread_current()->files, &user_file->elem);
  return user_file_descriptor;
  
}


struct user_file *get_file(int file_descriptor){


  struct list *files = &(thread_current())->files;


  for (struct list_elem *e = list_begin(files); e != list_end(files); e = list_next(e)){
    struct user_file *f = list_entry(e, struct user_file, elem);
    if (f->file_descriptor == file_descriptor) 
      return f;
  }
  return NULL;
}

int read(int fd, void *buffer, unsigned size)
{
  int res = size;
  if (fd == 0)
  { // stdin .. 0 at end of file
    while (size--)
    {
      lock_acquire(&file_lock);
      char ch = input_getc();
      lock_release(&file_lock);
      buffer += ch;
    }
    return res;
  }

  struct user_file *user_file = get_file(fd);
  if (user_file == NULL)
  { // fail
    return -1;
  }
  else
  {
    struct file *file = user_file->file;
    lock_acquire(&file_lock);
    size = file_read(file, buffer, size);
    lock_release(&file_lock);
    return size;
  }
}

int write(int file_descriptor, const void *buffer, unsigned size)
{
  if (file_descriptor == 1){ 
    lock_acquire(&file_lock);
    putbuf(buffer, size);
    lock_release(&file_lock);
    return size;
  }

  struct user_file *f = get_file(file_descriptor);
  if (f == NULL)
    return -1;


  lock_acquire(&file_lock);
  int result = file_write(f->file, buffer, size);
  lock_release(&file_lock);
  return result;
  
}

void seek(struct intr_frame *frame)
{
  int file_descriptor = (int)(*((int *)frame->esp + 1));
  unsigned position = (unsigned)(*((int *)frame->esp + 2));
  struct user_file *f = get_file(file_descriptor);
  if (f == NULL)
    frame->eax = -1;
  
  lock_acquire(&file_lock);
  file_seek(f->file, position);
  frame->eax = position;
  lock_release(&file_lock);

}


void tell(struct intr_frame *frame){
  int fd = (int)(*((int *)frame->esp + 1));
  struct user_file *f = get_file(fd);
  if (f == NULL)
    frame->eax = -1;

  lock_acquire(&file_lock);
  frame->eax = file_tell(f->file);
  lock_release(&file_lock);
}


int close(int file_descriptor)
{
  struct user_file *f = get_file(file_descriptor);
  if (f == NULL)
    return -1;
  
  lock_acquire(&file_lock);
  file_close(f->file);
  lock_release(&file_lock);
  list_remove(&f->elem);
  return 1;

}

