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

// ------------------------ pmalloc Test -------------------------

void pmallocTest() {
  printf(0, "Testing pmalloc\n");
  void *x = pmalloc();
  if (!pfree(x)){
    printf(0, "pfree fail\n");
  }  printf(0, "Finished pmalloc testings!\n");
}
// ------------------------ protect Test -------------------------

void protectTest() {
  printf(0, "Testing protect\n");
  void *x = pmalloc();
  if (!protect_page(x)){
     printf(0,"protect test1 - fail\n");
  }
  if (!pfree(x)){
    printf(0, "pfree fail\n");
  }

  void *y = malloc(3);
  if (protect_page(y) != -1){
    printf(0,"protect test2 - fail\n");
  }
  free(y);
  printf(0, "Finished protect testings!\n");
}
int main() {
  // System Test
   systemTest();

  // pmalloc Test
  pmallocTest();

  // Protect Test
  protectTest();

  exit();
}
