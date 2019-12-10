#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include "mlfs/mlfs_interface.h"

#define TESTDIR "/mlfs/test_dir"
#define N_FILES 1000

void printStats(struct stat* file_stats) {
    printf("\t\tDevice: %d\n", file_stats->st_dev);
	printf("\t\tInode: %d\n", file_stats->st_ino);
	printf("\t\tMode: %d\n", file_stats->st_mode);
	printf("\t\tHard Links: %d\n", file_stats->st_nlink);
	printf("\t\tUser ID: %d\n", file_stats->st_uid);
	printf("\t\tGroup ID: %d\n", file_stats->st_gid);
	printf("\t\tRdev: %d\n", file_stats->st_rdev);
	printf("\t\tSize: %d\n", file_stats->st_size);
	printf("\t\tBlock size: %d\n", file_stats->st_blksize);
	printf("\t\tBlocks: %d\n", file_stats->st_blocks);
}

int main(int argc, char ** argv)
{
    struct stat buf;
    int i, ret = 0;
	
	init_fs();

	/*
    if ((ret = lstat(TESTDIR "/files/files1", &buf)) == 0) {
		printf("%s: inode number %lu", TESTDIR "files/file1", buf.st_ino);
	}
	*/

	/*
    if ((ret = rmdir(TESTDIR)) < 0 && errno != ENOENT) {
        perror("rmdir");
        exit(1);
    }
	*/

    if ((ret = mkdir(TESTDIR, 0700)) < 0) {
        perror("mkdir");
        exit(1);
    }

    if ((ret = creat(TESTDIR "/file", 0600)) < 0) {
        perror("open");
        exit(1);
    }

    if ((ret = unlink(TESTDIR "/file")) < 0) {
        perror("unlink");
        exit(1);
    }

    if ((ret = mkdir(TESTDIR "/files", 0600)) < 0) {
        perror("open");
        exit(1);
    }

	for (i = 0; i < N_FILES; i++) {
		char file_path[4096];
		memset(file_path, 0, 4096);

		sprintf(file_path, "%s%d", TESTDIR "/files/file", i);
		if ((ret = creat(file_path, 0600)) < 0) {
			perror("open");
			exit(1);
		}
	}

    if ((ret = unlink(TESTDIR "/files/file3")) < 0) {
        perror("unlink");
        exit(1);
    }

	if ((ret = creat(TESTDIR "/files/file3", 0600)) < 0) {
		perror("open");
		exit(1);
	}

    struct stat file_stats;
    if ((ret = stat(TESTDIR "/files/file3", &file_stats)) < 0) {
        perror("stat");
        exit(1);
    }
    printf("file stats before digesting: \n");
    printStats(&file_stats);

	shutdown_fs();

    init_fs();

    if ((ret = stat(TESTDIR "/files/file3", &file_stats)) < 0) {
        perror("stat");
        exit(1);
    }
    printf("file stats after digesting: \n");
    printStats(&file_stats);

    shutdown_fs();

    return 0;
}
