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
#define TEST_DIR "/mlfs/transactions-abort"

void test_open_with_create(void) {
    // Try to open without O_CREAT, should fail
    int fd1 = open(TEST_DIR "/open", O_RDWR, 0600);
    if (fd1 >= 0) {
        printf("Test Failed: file already exists\n");
        return;
    }

    // Start transaction
    start_log_usr_tx();

    // Open the file and create it
    fd1 = open(TEST_DIR "/open", O_RDWR|O_CREAT, 0600);
    if (fd1 < 0) {
        perror("Test Failed: Unable to create file during transaction");
        abort_log_usr_tx();
        return;
    }

    // Abort the transaction
    abort_log_usr_tx();

    // Try to open without O_CREAT, should still fail
    fd1 = open(TEST_DIR "/open", O_RDWR, 0600);
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

void test_rmdir(void) {
    // Create directory
    int err = mkdir("/mlfs/existent", 700);
    if (err < 0) {
        perror("Test Failed: Unable to create directory");
        return;
    }

    // Verify it can be opened
    DIR* dir = opendir("/mlfs/existent");
    if (dir == NULL) {
        printf("Test Failed: dir does not exist after mkdir\n");
        return;
    }
    err = closedir(dir);
    if (err) {
        perror("Test Failed: Unable to close directory");
        return;
    }

    // Start transaction
    start_log_usr_tx();

    err = rmdir("/mlfs/existent");
    if (err) {
        perror("Test Failed: Unable to remove directory during transaction");
        abort_log_usr_tx();
        return;
    }

    // Abort the transaction
    abort_log_usr_tx();

    // Verify it still exists
    dir = opendir("/mlfs/existent");
    if (dir == NULL) {
        printf("Test Failed: dir does not exist after abort");
        return;
    }
    closedir(dir);
}

void test_unlink(void) {
    // Open with O_CREAT to ensure the file exists
    int fd1 = open(TEST_DIR "/unlink", O_RDWR|O_CREAT, 0600);
    if (fd1 < 0) {
        perror("Test Failed: unable to open starting file");
        return;
    }
    close(fd1);

    // Start transaction
    start_log_usr_tx();

    int err = unlink(TEST_DIR "/unlink");
    if (err < 0) {
        perror("Test Failed: Unable to unlink file during transaction");
        abort_log_usr_tx();
        return;
    }

    // Abort transaction
    abort_log_usr_tx();

    // Open without O_CREAT to verify it still exists
    fd1 = open(TEST_DIR "/unlink", O_RDWR, 0600);
    if (fd1 < 0) {
        perror("Test Failed: Unable to open file after abort");
        return;
    }
    close(fd1);
    printf("Test Passed");
}

void test_truncate(void) {
    // Create the file
    int fd1 = open(TEST_DIR "/truncate", O_RDWR|O_CREAT, 0600);
    if (fd1 < 0) {
        perror("Test Failed: Unable to open starting file");
        return;
    }
    close(fd1);

    // Start transaction
    start_log_usr_tx();

    int err = truncate(TEST_DIR "/truncate", 1024);
    if (err) {
        perror("Test Failed: Unable to truncate during transaction");
        abort_log_usr_tx();
        return;
    }

    // Abort transaction
    abort_log_usr_tx();

    // Verify the file still has size 0
    struct stat file_stats;
    err = stat(TEST_DIR "/truncate", &file_stats);
    if (err) {
        perror("Test Failed: Unable to get file stats");
        return;
    }
    if (file_stats.st_size != 0) {
        printf("Test Failed: file size has been increased after abort\n");
        return;
    }
    printf("Test Passed\n");
}

int main(int argc, char ** argv)
{
	init_fs();

	int ret = mkdir(TEST_DIR, 0600);
	if (ret < 0) {
		perror("mkdir\n");
		return 1;
	}

    printf("Testing open with O_CREAT\n");
    test_open_with_create();

    printf("\nTesting mkdir\n");
    test_mkdir();

    printf("\nTesting rmdir\n");
    test_rmdir();

    printf("\nTesting unlink\n");
    test_unlink();
    
	pause();
    return 0;
}


