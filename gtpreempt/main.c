// Based on https://c9x.me/articles/gthreads/code0.html

#include "gthr.h"

void f(void) {
  static int x;
  int i, id;

  id = ++x;
  while (true) {

    printf("F Thread id = %d, val = %d BEGINNING\n", id, ++i);
    uninterruptibleNanoSleep(0, 50000000);
    printf("F Thread id = %d, val = %d END\n", id, ++i);
    uninterruptibleNanoSleep(0, 50000000);
  }
}

void g(void) {
  static int x;
  int i, id;

  id = ++x;
  while (true) {

    printf("G Thread id = %d, val = %d BEGINNING\n", id, ++i);
    uninterruptibleNanoSleep(0, 50000000);
    printf("G Thread id = %d, val = %d END\n", id, ++i);
    uninterruptibleNanoSleep(0, 50000000);

  }
}

int main(void) {
  gtinit();
  gtgo(f);
  gtgo(f);
  gtgo(g);
  gtgo(g);
  gtret(1);
}
