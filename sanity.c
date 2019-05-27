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

void sbrkTest() {
  printf(0, "Testing sbrk()\n");
  sbrk(0);
  sbrk(1);
  printf(0, "Finished sbrk testings!\n");
}

int main() {
  // System Test
   systemTest();

  // Test
  sbrkTest();
  exit();
}
