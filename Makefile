all:mmap_GPIO_get mmap_GPIO_process
	
mmap_GPIO_get:mmap_GPIO_get.c shm_com.h
	gcc mmap_GPIO_get.c -o mmap_GPIO_get
	
mmap_GPIO_process:mmap_GPIO_process.c shm_com.h
	gcc mmap_GPIO_process.c -o mmap_GPIO_process
	
clean:
	rm -f mmap_GPIO_get mmap_GPIO_process
