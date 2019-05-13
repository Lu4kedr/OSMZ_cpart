// See https://c9sharedId.me/articles/gthreads/code0.html

#include "gthr.h"
#include <signal.h>

int sharedId=0;		/* this variable is shared */

void f(void) {
 // int i, id;

  //id = ++sharedId;
  while (true) {

  //  printf("F Thread id = %d, val = %d BEGINNING\n", id, ++i);
    uninterruptibleNanoSleep(0, 50000000);
//  printf("F Thread id = %d, val = %d END\n", id, ++i);
    uninterruptibleNanoSleep(0, 50000000);
    gtyield();		/* yield CPU to another thread */
  }
}

void g(void) {
 // static int sharedId;		/* this variable is shared */
 // int i, id;

  //id = ++sharedId;
  while (true) {

  //  printf("G Thread id = %d, val = %d BEGINNING\n", id, ++i);
    uninterruptibleNanoSleep(0, 50000000);
  //  printf("G Thread id = %d, val = %d END\n", id, ++i);
    uninterruptibleNanoSleep(0, 50000000);
    gtyield();		/* yield CPU to another thread */
  }
}

int main(void) {

  signal(SIGINT,printIds);
  gtinit();	
  gtgo(f,HIGH);
  gtgo(f,HIGH);
  gtgo(g,HIGH);
  gtgo(g,HIGH);
  gtret(1);		/* wait for all threads and return 1 */
}
