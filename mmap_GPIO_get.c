/*
 * =====================================================================================
 *
 *       Filename:  mmap_GPIO_get.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2015年06月01日 14时28分15秒
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
#include "shm_com.h"

int main()
{
	void *shared_memory = (void *)0;
	struct shared_mmap_addr *shared_stuff;
	int shm_id;
	
	srand((unsigned int)getpid());
	
	shm_id = shmget((key_t)1234, sizeof(struct shared_mmap_addr), 0666 | IPC_CREAT);
	if (shm_id == -1){
		fprintf(stderr, "shmget failed\n");
		exit(EXIT_FAILURE);
	}
	
	shared_memory = shmat(shm_id, (void *)0, SHM_RDONLY);
	if (shared_memory == (void *)-1){
		fprintf(stderr, "shmat failed\n");
		exit(EXIT_FAILURE);
	}
	
	printf("Memory attached at %X\n", (int)shared_memory);
	
	shared_stuff = (struct shared_mmap_addr *)shared_memory;
	printf("GPIO_mmap_addr = %d\n", shared_stuff->mmap_addr);
	
	if (shmdt(shared_memory) == -1){
		fprintf(stderr, "shmdt failed\n");
		exit(EXIT_FAILURE);
	}
	exit(EXIT_SUCCESS);
}
