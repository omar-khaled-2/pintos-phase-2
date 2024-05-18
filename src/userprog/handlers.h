#ifndef USERPROG_HANDLERS_H
#define USERPROG_HANDLERS_H
#include <stdbool.h>
void exit_handler(struct intr_frame *frame);

void execute_handler(struct intr_frame *frame);

void wait_handler(struct intr_frame *frame);

void create_handler(struct intr_frame *frame);


void remove_handler(struct intr_frame *frame);


void open_handler(struct intr_frame *frame);
void get_file_size_handler(struct intr_frame *frame);

void read_handler(struct intr_frame *frame);

void write_handler(struct intr_frame *frame);


void close_handler(struct intr_frame *frame);


void halt_handler(struct intr_frame *frame);


void tell_handler(struct intr_frame *frame);

void seek_handler(struct intr_frame *frame);


bool valid_in_virtual_memory(void *val);

#endif