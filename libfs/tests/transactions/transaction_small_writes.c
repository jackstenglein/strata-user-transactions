#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <mlfs/mlfs_interface.h>
#include <fcntl.h>
#include <time.h>

#define MEGABYTE 1024 * 1024
#define WRITE_SIZE 4096
#define NUM 4

//4 KB writes to 4 MB buffer
int main(int argc, char **argv) {
  int fd;
  int ret;
  char buffer[NUM * MEGABYTE];
  clock_t t;
  double time = 0.0;
  
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
  
  int loops = (NUM * MEGABYTE)/WRITE_SIZE;
  int offset = 0;
  
  t = clock();
  for(int i = 0; i < loops; ++i) {
     write(fd, buffer + offset, WRITE_SIZE);
     offset += WRITE_SIZE;
  }
  t = clock() - t;
  time = ((double)t)/CLOCKS_PER_SEC;
  //in millisec
  //fputs(time * 1000, record);
  printf("%.4f\n", time * 1000);
  
#ifdef USR_TX
  commit_log_tx();
#endif
  //  close(fd);
  shutdown_fs();
  return 0;
}
