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
#include <dirent.h>
#include <mlfs/mlfs_interface.h>

#define BUF_SIZE 4096
#define TEST_DIR "/mlfs/transactions-create"

void test_open_with_create(void) {
    // Try to open without O_CREAT, should fail
    fd1 = open(TEST_DIR "/test1", O_RDWR, 0600);
    if (fd1 >= 0) {
        printf("Test Failed: file already exists\n");
        return;
    }

    // Start transaction
    start_log_usr_tx();

    // Open the file and create it
    fd1 = open(TEST_DIR "/test1", O_RDWR|O_CREAT, 0600);
    if (fd1 < 0) {
        perror("Test Failed: Unable to create file during transaction");
        abort_log_usr_tx();
        return;
    }

    // Abort the transaction
    abort_log_usr_tx();

    // Try to open without O_CREAT, should still fail
    fd1 = open(TEST_DIR "/test1", O_RDWR, 0600);
    if (fd1 >= 0) {
        printf("Test Failed: file exists after abort\n");
        return;
    }

    printf("Test Passed\n");
}

void test_mkdir(void) {

    // Try to open non-existent directory, should fail
    DIR* dir = opendir("/mlfs/nonexistent");
    if (dir != NULL) {
        printf("Test Failed: dir already exists\n");
        return;
    }

    // Start transaction
    start_log_usr_tx();

    // Create the directory
    int err = mkdir("/mlfs/nonexistent", 700);
    if (err < 0) {
        perror("Test Failed: Unable to create directory during transaction");
        abort_log_usr_tx();
        return;
    }

    // Abort the transaction
    abort_log_usr_tx();

    // Try to open again, should still fail
    dir = opendir("/mlfs/nonexistent");
    if (dir != NULL) {
        printf("Test Failed: dir exists after abort\n");
        return;
    }

    printf("Test Passed\n");
}

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

    printf("Testing open with O_CREAT\n");
    test_open_with_create();

    printf("Testing mkdir\n");
    test_mkdir();
    

    printf("Test Passed\n");
	pause();
    return 0;
}


