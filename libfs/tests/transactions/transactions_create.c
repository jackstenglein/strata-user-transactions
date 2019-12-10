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
#define TEST_DIR "/mlfs/transactions-create"

int main(int argc, char ** argv)
{
    int fd1;
    int ret;
    char buffer[BUF_SIZE], str[BUF_SIZE];
	int write_count;

	init_fs();

	ret = mkdir(TEST_DIR, 0600);
	if (ret < 0) {
		perror("mkdir\n");
		return 1;
	}

    // Try to open without O_CREAT, should fail
    fd1 = open(TEST_DIR "/test1", O_RDWR, 0600);
    if (fd1 >= 0) {
        printf("Test Failed: file already exists\n");
        return 1;
    }

    // Start transaction
    start_log_usr_tx();

    // Open the file and create it
    fd1 = open(TEST_DIR "/test1", O_RDWR|O_CREAT, 0600);
    if (fd1 < 0) {
        perror("open with create");
        return 1;
    }

    // Abort the transaction
    abort_log_usr_tx();

    // Try to open without O_CREAT, should still fail
    fd1 = open(TEST_DIR "/test1", O_RDWR, 0600);
    if (fd1 >= 0) {
        printf("Test Failed: file exists after abort\n");
        return 1;
    }

    printf("Test Passed\n");
	shutdown_fs();
    return 0;
}


