#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <mlfs/mlfs_interface.h>

#define GIGABYTE 1024 * 1024 * 1024
#define WRITE_SIZE 4096

//4 KB writes to 1 GB buffer
int main(int argc, char **argv) {
  int fd;
  //int bytes, ret;
  char buffer[GIGABYTE], str[GIGABYTE];

  init_fs();
  ret = mkdir("/mlfs/", 0600);

  printf("---mkdir\n");
  fd = open("/mlfs/foo2", O_RDWR | O_CREAT, 0600);

  if(fd < 0) {
    perror("error opening fd\n");
    return 1;
  }

  printf("--writing\n");
#ifdef USR_TX
  start_log_tx();  
#endif
  int loops = GIGABYTE/WRITE_SIZE;
  int offset = 0;
  for(int i = 0; i < loops; ++i) {
     write(fd, buffer + offset, WRITE_SIZE);
     offset += WRITE_SIZE;
  }
  
  printf("---finished writing\n");

#ifdef USR_TX
  commit_log_tx();
#endif
  shutdown_fs();
  return 0;
}
