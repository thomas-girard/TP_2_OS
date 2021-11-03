//
// Console input and output, to the uart.
// Reads are line at a time.
// Implements special input characters:
//   newline -- end of line
//   control-h -- backspace
//   control-u -- kill line
//   control-d -- end of file
//   control-p -- print process list
//

#include <stdarg.h>

#include "types.h"
#include "param.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "fs.h"
#include "file.h"
#include "memlayout.h"
#include "riscv.h"
#include "defs.h"
#include "proc.h"

#define BACKSPACE 0x100
#define C(x)  ((x)-'@')  // Control-x
#define NBCONSOLES 3

//
// send one character to the uart.
//
void
consputc(int c)
{
  extern volatile int panicked; // from printf.c

  if(panicked){
    for(;;)
      ;
  }

  if(c == BACKSPACE){
    // if the user typed backspace, overwrite with a space.
    uartputc('\b'); uartputc(' '); uartputc('\b');
  } else {
    uartputc(c);
  }
}

struct cons_t {
  struct spinlock lock;
  
  // input
#define INPUT_BUF 128
  char buf[INPUT_BUF];
  uint r;  // Read index
  uint w;  // Write index
  uint e;  // Edit index
} consoles[NBCONSOLES];

struct spinlock console_number_lock;
int console_number = 0;
struct cons_t* cons;

//
// user write()s to the console go here.
//
int
consolewrite(struct file *f, int user_src, uint64 src, int n)
{
  int i;

  struct cons_t* cons = &consoles[f->minor-1];
  acquire(&console_number_lock);
  while(console_number != f->minor - 1){
    sleep(cons, &console_number_lock);
  }
  release(&console_number_lock);
  char* buf = bd_malloc(n);
  for(i = 0; i < n; i++){
    if(either_copyin(&buf[i], user_src, src+i, 1) == -1)
      break;
  }
  acquire(&cons->lock);
  for(int j = 0; j < i; j++){
    consputc(buf[j]);
  }
  release(&cons->lock);
  bd_free(buf);
  return n;
}

//
// user read()s from the console go here.
// copy (up to) a whole input line to dst.
// user_dist indicates whether dst is a user
// or kernel address.
//
int
consoleread(struct file *f, int user_dst, uint64 dst, int n)
{
  uint target;
  int c;

  target = n;
  struct cons_t* cons = &consoles[f->minor-1];
  acquire(&console_number_lock);
  while(console_number != f->minor - 1){
    sleep(cons, &console_number_lock);
  }
  release(&console_number_lock);
  int len = 0;
  char* buf = bd_malloc(n);
  acquire(&cons->lock);
  while(n > 0){
    // wait until interrupt handler has put some
    // input into cons->buffer.
    while(cons->r == cons->w){
      if(myproc()->killed){
        release(&cons->lock);
        return -1;
      }
      sleep(&cons->r, &cons->lock);
    }

    c = cons->buf[cons->r++ % INPUT_BUF];

    if(c == C('D')){  // end-of-file
      if(n < target){
        // Save ^D for next time, to make sure
        // caller gets a 0-byte result.
        cons->r--;
      }
      break;
    }

    // copy the input byte to the user-space buffer.
    buf[len++] = c;
    --n;

    if(c == '\n'){
      // a whole line has arrived, return to
      // the user-level read().
      break;
    }
  }
  release(&cons->lock);
  either_copyout(user_dst, dst, buf, len);
  bd_free(buf);
  return target - n;
}

//
// the console input interrupt handler.
// uartintr() calls this for input character.
// do erase/kill processing, append to cons->buf,
// wake up consoleread() if a whole line has arrived.
//
void
consoleintr(int c)
{
  acquire(&cons->lock);

  switch(c){
  case C('P'):  // Print process list.
    procdump();
    break;
  case C('Q'):  // Print priority list
    priodump();
    break;
  case C('U'):  // Kill line.
    while(cons->e != cons->w &&
          cons->buf[(cons->e-1) % INPUT_BUF] != '\n'){
      cons->e--;
      consputc(BACKSPACE);
    }
    break;
  case C('S'): // switch consoles
  {
    acquire(&console_number_lock);
    struct spinlock* old = &cons->lock;
    console_number = (console_number + 1) % NBCONSOLES;
    cons = &consoles[console_number];
    acquire(&cons->lock);
    release(old);
    wakeup(cons);
    printf("Switched to console number %d\n", console_number);
    release(&console_number_lock);
    break;
  }
  case C('H'): // Backspace
  case '\x7f':
    if(cons->e != cons->w){
      cons->e--;
      consputc(BACKSPACE);
    }
    break;
  default:
    if(c != 0 && cons->e-cons->r < INPUT_BUF){
      c = (c == '\r') ? '\n' : c;

      // echo back to the user.
      consputc(c);

      // store for consumption by consoleread().
      cons->buf[cons->e++ % INPUT_BUF] = c;

      if(c == '\n' || c == C('D') || cons->e == cons->r+INPUT_BUF){
        // wake up consoleread() if a whole line (or end-of-file)
        // has arrived.
        cons->w = cons->e;
        wakeup(&cons->r);
      }
    }
    break;
  }
  
  release(&cons->lock);
}

void
consoleinit(void)
{
  initlock(&console_number_lock, "console_number_lock");
  console_number = 0;
  cons = &consoles[console_number];
  for(int i = 0; i < NBCONSOLES; i++){
    initlock(&consoles[i].lock, "cons");
  }

  uartinit();

  // connect read and write system calls
  // to consoleread and consolewrite.
  devsw[CONSOLE].read = consoleread;
  devsw[CONSOLE].write = consolewrite;
}
