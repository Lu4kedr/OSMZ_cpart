#include "gthr.h"

// Allocate the thread 0 and mark it as running
// A bootstrap thread
void gtinit(void) {
  gtcur = & gttbl[0];
  gtcur -> st = Running;
}

// User request to stop the current thread
// This function must not return thus  __attribute__((noreturn))
void __attribute__((noreturn)) gtret(int ret) {
  // If the current thread is not the bootstrap thread we simply mark the thread structure as unused and yield the CPU to another process.
  if (gtcur != & gttbl[0]) {
    gtcur -> st = Unused;
    gtyield();
    assert(!"reachable");
  }
  // If main thread (bootstrap) it will first wait to be the only runnable thread then exit the whole process with the passed return code.
  while (gtyield())
  ;
  exit(ret);
}

bool gtyield(void) {
  struct gt * p;
  struct gtctx * old, * new;

  p = gtcur;
  // Find a new thread to run.
  while (p -> st != Ready) {
    if (++p == & gttbl[MaxGThreads])
      p = & gttbl[0];
    if (p == gtcur)
      return false;
  }

  // Switch from the current thread to this new one
  if (gtcur -> st != Unused)
    gtcur -> st = Ready;
  p -> st = Running;
  // If a target thread is found, we store a pointer to its execution context in new and we store a pointer to ours in old.
  old = & gtcur -> ctx;
  new = & p -> ctx;
  gtcur = p;
  // gtswtch never "returns" in the same thread
  gtswtch(old, new);
  return true;
}

void gtstop(void) {
  gtret(0);
}

// Static helper will be useful to create new threads
int gtgo(void( * f)(void)) {
  char * stack;
  struct gt * p;

  // Find an unused slot in the thread table.
  for (p = & gttbl[0];; p++)
    if (p == & gttbl[MaxGThreads])
      return -1;
    else if (p -> st == Unused)
    break;

  // Create and setup a private stack for the new thread.
  stack = malloc(StackSize);
  if (!stack)
    return -1;

  // Setup the execution context of the new thread and mark it as ready to run.
  // If f returns we make the CPU return into gtstop.
  *(uint64_t * ) & stack[StackSize - 8] = (uint64_t) gtstop;
  // Push the address of f on top of the stack — this way it will be used as return address for gtswtch
  *(uint64_t * ) & stack[StackSize - 16] = (uint64_t) f;
  p -> ctx.rsp = (uint64_t) & stack[StackSize - 16];
  p -> st = Ready;

  return 0;
}

int uninterruptibleNanoSleep(time_t sec, long nanosec) {
  struct timespec req;
  req.tv_sec = sec;
  req.tv_nsec = nanosec;

  do {
    //  Suspends the execution of the calling thread 
    //  If the call is interrupted by a signal handler, nanosleep() returns -1, sets errno to EINTR, and writes the remaining time into the structure
    if (0 != nanosleep( & req, & req)) {
      if (errno != EINTR)
        return -1;
    } else {
      break;
    }
  } while (req.tv_sec > 0 || req.tv_nsec > 0);
  return 0; /* Return success */
}