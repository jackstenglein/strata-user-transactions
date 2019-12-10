#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <mlfs/mlfs_interface.h>
#include <fcntl.h>
#include <time.h>

#define BUF_SIZE 4096 * 10

int main(int argc, char **argv) {
  int fd;
  int ret;
  char buffer[BUF_SIZE], str[BUF_SIZE];
  clock_t t;
  double time = 0.0;
  
  init_fs();
  ret = mkdir("/mlfs/", 0600);

  printf("---mkdir\n");
  fd = open("/mlfs/foo", O_RDWR | O_CREAT, 0600);

  if(fd < 0) {
    perror("error opening fd\n");
    return 1;
  }

  printf("--writing\n");
#ifdef USR_TX
  start_log_tx();
#endif
  
  t = clock();
  write(fd, buffer, strlen(buffer)/2);
  write(fd, buffer + (BUF_SIZE/2), strlen(buffer)/2);

  t = clock() - t;
  time = ((double)t)/CLOCKS_PER_SEC;
  //time taken in milliseconds
  printf("%.4f\n", time * 1000);
  
#ifdef USR_TX
  commit_log_tx();
#endif
  
  shutdown_fs();
  return 0;
}
