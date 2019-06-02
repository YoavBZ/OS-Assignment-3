#include "types.h"
#include "user.h"

// ------------------------ System Test -------------------------

void systemTest() {
  printf(0, "Testing system functionality\n");
  if (fork() == 0) {
    sleep(50);
    exit();
  } else {
    wait();
  }
  printf(0, "Finished system testings!\n");
}

// ------------------------ System Test -------------------------

void pmallocTest() {
  printf(0, "Testing pmalloc\n");
  void *x = pmalloc();
  void *x1 = pmalloc();
  void *x2 = pmalloc();
  printf(1, "%d\n", pfree(x));
  printf(1, "%d\n", pfree(x1));
  printf(1, "%d\n", pfree(x2));
  printf(0, "Finished pmalloc testings!\n");
}

int main() {
  // System Test
   systemTest();

  // Test
  pmallocTest();
  exit();
}
