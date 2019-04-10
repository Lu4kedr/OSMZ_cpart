#include "gthr.h"

void gthandle(int sig) {
  gtyield();
}

void gtinit(void) {
  gtcur = & gttbl[0];
  gtcur -> st = Running;
  signal(SIGALRM, gthandle);
}

void __attribute__((noreturn)) gtret(int ret) {
  if (gtcur != & gttbl[0]) {
    gtcur -> st = Unused;
    gtyield();
    assert(!"reachable");
  }
  while (gtyield());
  exit(ret);
}

bool gtyield(void) {
  struct gt * p;
  struct gtctx * old, * new;

  resetSig(SIGALRM);

  p = gtcur;
  while (p -> st != Ready) {
    if (++p == & gttbl[MaxGThreads])
      p = & gttbl[0];
    if (p == gtcur)
      return false;
  }

  if (gtcur -> st != Unused)
    gtcur -> st = Ready;
  p -> st = Running;
  old = & gtcur -> ctx;
  new = & p -> ctx;
  gtcur = p;
  gtswtch(old, new);
  return true;
}

void gtstop(void) {
  gtret(0);
}

int gtgo(void( * f)(void)) {
  char * stack;
  struct gt * p;

  for (p = & gttbl[0];; p++)
    if (p == & gttbl[MaxGThreads])
      return -1;
    else if (p -> st == Unused)
    break;

  stack = malloc(StackSize);
  if (!stack)
    return -1;

  *(uint64_t * ) & stack[StackSize - 8] = (uint64_t) gtstop;
  *(uint64_t * ) & stack[StackSize - 16] = (uint64_t) f;
  p -> ctx.rsp = (uint64_t) & stack[StackSize - 16];
  p -> st = Ready;

  return 0;
}

void resetSig(int sig) {
  if (sig == SIGALRM) {
    // Clear pending alarms
    alarm(0);
  }

  sigset_t set;
  sigemptyset( & set);
  sigaddset( & set, sig);

  sigprocmask(SIG_UNBLOCK, & set, NULL);

  if (sig == SIGALRM) {
    // Generate alarms
    ualarm(500, 500);
  }
}

int uninterruptibleNanoSleep(time_t sec, long nanosec) {
  struct timespec req;
  req.tv_sec = sec;
  req.tv_nsec = nanosec;

  do {
    if (0 != nanosleep( & req, & req)) {
      if (errno != EINTR)
        return -1;
    } else {
      break;
    }
  } while (req.tv_sec > 0 || req.tv_nsec > 0);
  return 0; /* Return success */
}
