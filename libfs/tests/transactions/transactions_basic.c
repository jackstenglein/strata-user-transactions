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

int update_file(char* buffer, int fd, char* default_str) {
    int write_count;
    int bytes = read(fd, buffer, BUF_SIZE);
	printf("Read data: %s\n", buffer);
	lseek(fd, 0, SEEK_SET);
	if (bytes > 0) {
        char str[BUF_SIZE];
		sscanf(buffer, "%s %d\n", str, &write_count);
		memset(buffer, 0, BUF_SIZE);
		sprintf(buffer, "%s %d\n", str, ++write_count);
	} else {
		sprintf(buffer, "%s 0\n", default_str);
	}
	printf("New data: %s\n", buffer);
    write(fd, buffer, strlen(buffer));
    return write_count;
}

int get_write_count(char* buffer, int fd) {
    int bytes = read(fd, buffer, BUF_SIZE);
	lseek(fd, 0, SEEK_SET);
	if (bytes > 0) {
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

    // Start transaction
    start_log_usr_tx();

    // Write to each file
    int fd1_updated = update_file(buffer, fd1, "test1");
    int fd2_updated = update_file(buffer, fd2, "test2");
    int fd3_updated = update_file(buffer, fd3, "test3");

    // Commit transaction
    commit_log_usr_tx();

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

    // Check write counts to show each file committed
    int final_fd1 = get_write_count(buffer, fd1);
    int final_fd2 = get_write_count(buffer, fd2);
    int final_fd3 = get_write_count(buffer, fd3);

    if (final_fd1 != updated_fd1 || final_fd2 != updated_fd2 || final_fd3 != updated_fd3) {
        printf("Test Failed: final write counts are not as expected.\n");
    } else {
        printf("Test Passed\n");
    }

	shutdown_fs();
    return 0;
}


