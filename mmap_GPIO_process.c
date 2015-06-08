/*
 * =====================================================================================
 *
 *       Filename:  mmap_GPIO_process.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2015年05月29日 14时19分08秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/shm.h>
#include <signal.h>
#include "shm_com.h"

#define UNHAPPEN 0
#define HAPPENED 1

int flag_happen;

/* The signal handler function (SA_SIGINFO that is use "sa_sigaction" not "sa_handler") */
void handler_receive(int signum, siginfo_t *siginfo, void *pvoid)
{
	flag_happen = HAPPENED;
	printf("recv SIGTERM, The process PID is %d that can exit\n",siginfo->si_int);
}

/* The custom handler function of error */
void my_err(const char *err_string, int line)
{
	fprintf(stderr, "line:%d ", line);
	perror(err_string);
	exit(1);
}

int main()
{
	void *shared_memory = (void *)0;
	struct shared_mmap_addr *shared_stuff;
	int shm_id;
	struct sigaction act;
	sigset_t newmask, oldmask, zeromask;
	
	printf("+[%d]\n", getpid());
	
	//srand((unsigned int)getpid());
	
	/* Initialize signal sets */
	sigemptyset(&newmask);
	sigemptyset(&zeromask);
	sigaddset(&newmask, SIGINT);
	
	/* shielded the signal of SIGINT */
	if (sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0){
		my_err("sigprocmask", __LINE__);
	} 
	else {
		printf("SIGINT blocked\n");
	}
	
	/* Create shared memory */
	shm_id = shmget((key_t)1234, sizeof(struct shared_mmap_addr), 0666 | IPC_CREAT);
	if (shm_id == -1){
		fprintf(stderr, "shmget failed\n");
		exit(EXIT_FAILURE);
	}
	
	/* Map shared memory */
	shared_memory = shmat(shm_id, (void *)0, 0);
	if (shared_memory == (void *)-1){
		fprintf(stderr, "shmat failed\n");
		exit(EXIT_FAILURE);
	}
	
	printf("Memory attached at %X\n", (int)shared_memory);
	
	shared_stuff = (struct shared_mmap_addr *)shared_memory;
	shared_stuff->PID = getpid(); 
	shared_stuff->mmap_addr = 0xff;
	
	/* assign act(struct) */
	act.sa_sigaction = handler_receive;
	act.sa_flags = SA_SIGINFO;  //use "sa_sigaction" not "sa_handler"
	
	/* Install the signal handler function */
	sigaction(SIGTERM, &act, NULL);
	
	//pause();
	while(flag_happen == UNHAPPEN){
		sigsuspend(&newmask);
	}
	
	printf("after event happend\n");
	/* ... do something else ... */
	
	/* Separate shared memory that is running */
	if (shmdt(shared_memory) == -1){
		fprintf(stderr, "shmdt failed\n");
		exit(EXIT_FAILURE);
	}
	
	/* Delete shared memory */
	if (shmctl(shm_id, IPC_RMID, 0) == -1){
		fprintf(stderr, "shmctl(IPC_RMID) failed\n");
		exit(EXIT_FAILURE);
	}
	
	/* recover the signal sets before it changes */
	if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0){
		my_err("sigprocmask", __LINE__);
	}
	
	return 0;
}
