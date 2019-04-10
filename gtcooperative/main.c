// See https://c9x.me/articles/gthreads/code0.html

#include "gthr.h"

void f(void) {
  static int x;		/* this variable is shared */
  int i, id;

  id = ++x;
  while (true) {

    printf("F Thread id = %d, val = %d BEGINNING\n", id, ++i);
    uninterruptibleNanoSleep(0, 50000000);
    printf("F Thread id = %d, val = %d END\n", id, ++i);
    uninterruptibleNanoSleep(0, 50000000);
    gtyield();		/* yield CPU to another thread */
  }
}

void g(void) {
  static int x;		/* this variable is shared */
  int i, id;

  id = ++x;
  while (true) {

    printf("G Thread id = %d, val = %d BEGINNING\n", id, ++i);
    uninterruptibleNanoSleep(0, 50000000);
    printf("G Thread id = %d, val = %d END\n", id, ++i);
    uninterruptibleNanoSleep(0, 50000000);
    gtyield();		/* yield CPU to another thread */
  }
}

int main(void) {
  gtinit();	
  gtgo(f);
  gtgo(f);
  gtgo(g);
  gtgo(g);
  gtret(1);		/* wait for all threads and return 1 */
}
