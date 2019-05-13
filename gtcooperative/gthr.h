#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>
#include <sys/resource.h> 

#define MEGA 10e6
#define TRESHOLD 5e9
extern int sharedId;


enum priority_t {
    HIGH=3,	
    MED=2,	
    LOW=1		
};
// MaxGThreads gives number of threads available, currently fixed to 5
// StackSize constant gives the size of a thread stack, currently fixed to 4Mb
enum {
  MaxGThreads = 5,
  StackSize = 0x400000,

};

// Structure captures a CPU state 
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
    Unused,		// This thread structure is free and available for use
    Running,	// The state of the thread currently being run, there can only be one in this state
    Ready,		// Describes a valid thread whose execution is currently suspended
  }
  st;
  uint64_t id;
  struct timespec executionStartTime;
  struct timespec waitingTime;
  double totalRunningTime;
  enum priority_t priority;

};


// All the thread structures we will use will be stored in the gttbl array.
struct gt gttbl[MaxGThreads];
// Pointer points to the thread being currently executed.
struct gt * gtcur;
void printIds();
void gtinit(void);
void gtret(int ret);
void gtswtch(struct gtctx * old, struct gtctx * new);
bool gtyield(void);
void gtstop(void);
int gtgo(void( * f)(void), enum priority_t priority);
int uninterruptibleNanoSleep(time_t sec, long nanosec);
