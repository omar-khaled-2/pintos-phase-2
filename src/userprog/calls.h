#ifndef USERPROG_CALLS_H
#define USERPROG_CALLS_H


#include "threads/thread.h"
#include "threads/synch.h"



struct lock file_lock;

void exit(int status);

void halt(void);

tid_t wait(tid_t tid);

int create(const char *file, unsigned initial_size);
int remove(const char *file);

int open(const char *file);


struct user_file *get_file(int file_descriptor);

int read(int file_descriptor, void *buffer, unsigned size);

int write(int file_descriptor, const void *buffer, unsigned size);
void seek(struct intr_frame *frame);


void tell(struct intr_frame *frame);

int close(int file_descriptor);

#endif