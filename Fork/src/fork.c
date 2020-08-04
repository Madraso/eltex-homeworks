#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

void creat_child_proc() {
	if (fork() == 0) {
		printf("Process - %d created child process - %d\n", getppid(), getpid());
		exit(0);
	}
	else {
		wait(NULL);
		printf("Process - %d created child process - %d\n", getppid(), getpid());
	}
}

void fork_tree() {
	printf("Main process pid - %d\n", getpid());
	if (fork() == 0) {
		printf("Process - %d created child process - %d\n", getppid(), getpid());
        if (fork() == 0) {
            printf("Process - %d created child process - %d\n", getppid(), getpid());
        }
        else wait(NULL);
    }
    else {
    	wait(NULL);
    	if (fork() == 0) {
    		printf("Process - %d created child process - %d\n", getppid(), getpid());
    		if (fork() != 0) {
            	if (fork() == 0) {
            		printf("Process - %d created child process - %d\n", getppid(), getpid());
            	}
            	else wait(NULL);
            }
            else printf("Process - %d created child process - %d\n", getppid(), getpid());
    	}
    	else wait(NULL);
    }
}

int main() {
	printf("--TASK 1: CREATING CHILD PROCESS--\n");
	creat_child_proc();
	sleep(1);
	printf("\n--TASK 2: CREATING FORK TREE--\n");
	fork_tree();
}
