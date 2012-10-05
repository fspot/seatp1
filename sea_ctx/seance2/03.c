#include <stdlib.h>
#include <stdio.h>

#define regread(name) \
 unsigned int name;                    \
 asm ("movl %%"#name", %0"                \
 : "=r"(name)                              \
 :                                    \
 : "%"#name);

#define regwrite(name,val) { \
 unsigned int name = val;                    \
 asm ("movl %0, %%"#name                \
 :                             \
 : "r"(name)                                   \
 : "%"#name);                     \
}

typedef void (func_t)(void*); /* a function that returns an int from an int */

struct ctx_s {
 //void * stack_address;
 unsigned int ebp, esp;
 void (*f_ptr)(void*);
 void * f_args;
 int started;
};

int init_ctx(struct ctx_s *ctx, int stack_size, func_t f, void *args);
void switch_to_ctx(struct ctx_s *ctx);

int init_ctx(struct ctx_s *ctx, int stack_size, func_t f, void *args)
{
 char * addr = (char*) malloc(stack_size);
 ctx->esp = addr+stack_size-8;
 ctx->ebp = ctx->esp;
 ctx->f_ptr = f;
 ctx->f_args = args;
 ctx->started = 0;
}

void switch_to_ctx(struct ctx_s *ctx)
{
 if (ctx->started)
 {
  
 } else {
  static void (*f_ptr)(void*) = NULL;
  static int old_esp, old_ebp;
  f_ptr = ctx->f_ptr;
  regread(esp);
  old_esp = esp;
  regread(ebp);
  old_ebp = ebp;
  regwrite(esp,ctx->esp);
  regwrite(ebp,ctx->ebp);
  f_ptr(ctx->f_args);
  regwrite(esp,old_esp);
  regwrite(ebp,old_ebp);
 }
}

struct ctx_s ctx_ping;
struct ctx_s ctx_pong;
void f_ping(void *arg);
void f_pong(void *arg);

int main(int argc, char *argv[])
{
 init_ctx(&ctx_ping, 16384, f_ping, NULL);
 init_ctx(&ctx_pong, 16384, f_pong, NULL);
 switch_to_ctx(&ctx_ping);
 exit(EXIT_SUCCESS);
}

void f_ping(void *args)
{
 while(1) {
  printf("A") ;
  switch_to_ctx(&ctx_pong);
  printf("B") ;
  switch_to_ctx(&ctx_pong);
  printf("C") ;
  switch_to_ctx(&ctx_pong);
 }
}

void f_pong(void *args)
{
 while(1) {
  printf("1") ;
  switch_to_ctx(&ctx_ping);
  printf("2") ;
  switch_to_ctx(&ctx_ping);
 }
}


