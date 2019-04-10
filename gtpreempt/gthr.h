#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>

enum {
  MaxGThreads = 5,
    StackSize = 0x400000,
};

struct gt {
  struct gtctx {
    uint64_t rsp;
    uint64_t r15;
    uint64_t r14;
    uint64_t r13;
    uint64_t r12;
    uint64_t rbx;
    uint64_t rbp;
  }
  ctx;
  enum {
    Unused,
    Running,
    Ready,
  }
  st;
};

struct gt gttbl[MaxGThreads];
struct gt * gtcur;

void gtinit(void);
void gtret(int ret);
void gtswtch(struct gtctx * old, struct gtctx * new);
bool gtyield(void);
void gtstop(void);
int gtgo(void( * f)(void));
void resetSig(int sig);
void gthandle(int sig);
int uninterruptibleNanoSleep(time_t sec, long nanosec);