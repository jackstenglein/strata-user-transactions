// write
// open
// mkdir
// rmdir
// unlink
// truncate
// rename

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <mlfs/mlfs_interface.h>

#define BUF_SIZE 4096
#define TEST_DIR "/mlfs/transactions"

void update_write_count(char* buffer, int fd, char* str, int count) {
	lseek(fd, 0, SEEK_SET);
	sprintf(buffer, "%s %d\n", str, count);
	printf("New data: %s\n", buffer);
    write(fd, buffer, strlen(buffer));
}

int get_write_count(char* buffer, int fd) {
    int bytes = read(fd, buffer, BUF_SIZE);
    printf("Read data: %s\n", buffer);
	lseek(fd, 0, SEEK_SET);
	if (bytes > 0) {
        char str[BUF_SIZE];
        int write_count;
		sscanf(buffer, "%s %d\n", str, &write_count);
        return write_count;
	} else {
		return 0;
	}
}

int main(int argc, char ** argv)
{
    int fd1, fd2, fd3;
    int bytes, ret;
    char buffer[BUF_SIZE], str[BUF_SIZE];
	int write_count;

	init_fs();

	ret = mkdir(TEST_DIR, 0600);

	if (ret < 0) {
		perror("mkdir\n");
		return 1;
	}

    fd1 = open(TEST_DIR "/test1", O_RDWR|O_CREAT, 0600);
    if (fd1 < 0) {
        perror("open test1");
        return 1;
    }

    fd2 = open(TEST_DIR "/test2", O_RDWR|O_CREAT, 0600);
    if (fd2 < 0) {
        perror("open test2");
        return 1;
    }

    fd3 = open(TEST_DIR "/test3", O_RDWR|O_CREAT, 0600);
    if (fd3 < 0) {
        perror("open test3");
        return 1;
    }

    int fd1_count = get_write_count(buffer, fd1);
    int fd2_count = get_write_count(buffer, fd2);
    int fd3_count = get_write_count(buffer, fd3);

    // Start transaction
    start_log_usr_tx();

    // Write to each file
    update_write_count(buffer, fd1, "test1", ++fd1_count);
    update_write_count(buffer, fd2, "test2", ++fd2_count);
    update_write_count(buffer, fd3, "test3", ++fd3_count);

    // Abort transaction
    abort_log_usr_tx();

    // Close and reopen files
    close(fd1);
    close(fd2);
    close(fd3);

    fd1 = open(TEST_DIR "/test1", O_RDWR|O_CREAT, 0600);
    if (fd1 < 0) {
        perror("open test1");
        return 1;
    }
    fd2 = open(TEST_DIR "/test2", O_RDWR|O_CREAT, 0600);
    if (fd2 < 0) {
        perror("open test2");
        return 1;
    }
    fd3 = open(TEST_DIR "/test3", O_RDWR|O_CREAT, 0600);
    if (fd3 < 0) {
        perror("open test3");
        return 1;
    }

    // Check write counts to show each file aborted
    int fd1_final = get_write_count(buffer, fd1);
    int fd2_final = get_write_count(buffer, fd2);
    int fd3_final = get_write_count(buffer, fd3);

    if (fd1_final != fd1_count-1 || fd2_final != fd2_count-1 || fd3_final != fd3_count-1) {
        printf("Test Failed: final write counts are not as expected.\n");
    } else {
        printf("Test Passed\n");
    }

	shutdown_fs();
    return 0;
}


