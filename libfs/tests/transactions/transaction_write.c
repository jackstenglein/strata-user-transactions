#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <mlfs/mlfs_interface.h>

#define BUF_SIZE 4096 * 10

//just a couple of basic writes
int main(int argc, char **argv) {
  int fd;
  //int bytes, ret;
  char buffer[BUF_SIZE], str[BUF_SIZE];

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
  
  write(fd, buffer, strlen(buffer)/2);
  write(fd, buffer + (BUF_SIZE/2), strlen(buffer)/2);
  printf("---finished writing\n");

#ifdef USR_TX
  commit_log_tx();
#endif
  shutdown_fs();
  return 0;
}
