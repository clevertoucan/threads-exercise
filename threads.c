#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct argument {
	int * arr;
	int size;
} Argument;

int max = -1;
unsigned long sum = 0;
static pthread_mutex_t mutex;


void * calc_max(void * void_arg){
	Argument * arg = (Argument *) void_arg;
	int local_max = -1, i;
	for(i = 0; i < arg->size; i++){
		if(local_max < 0 || arg->arr[i] > local_max){
			local_max = arg->arr[i]; 
		}
	}
	pthread_mutex_lock(&mutex);
	if(max < 0|| local_max > max){
		max = local_max;
	} 
	pthread_mutex_unlock(&mutex);
	return NULL;
}

void * calc_sum(void * void_arg){
	Argument * arg = (Argument *) void_arg;
	int local_sum = 0, i;
	for(i = 0; i < arg->size; i++){
		local_sum += arg->arr[i];
	}
	pthread_mutex_lock(&mutex);
	sum+=local_sum;
	pthread_mutex_unlock(&mutex);
	
	return NULL;
}

int main(int args, char* argv[]){
	int size = atoi(argv[1]), chunk;
	int threads = atoi(argv[2]);
	int seed = atoi(argv[3]);
	int task = atoi(argv[4]);
	int * array = malloc(sizeof(int)*size), i;
	Argument * arg;
	pthread_t * tids = malloc(sizeof(pthread_t)*threads);
	
	printf("%d %d %d %d %s\n", size, threads, seed, task, argv[5]);
	srand(seed);
	pthread_mutex_init(&mutex, NULL);
	
	for(i = 0; i < size; i++){
		array[i] = rand();
	}
	
	chunk = size/threads;
	
	if(task == 1){
		for(i = 0; i < threads; i++){
			arg = malloc(sizeof(Argument));
			arg->arr = array + i*(chunk);
			arg->size = chunk;
			if(i == threads-1){
				arg->size += size % threads;
			}
			pthread_create(&tids[i], NULL, calc_max, (void *)arg);
		}
		for(i = 0; i < threads; i++){
			pthread_join(tids[i], NULL);
		}
		
		printf("%d\n", max);
	} else if(task == 2){
		for(i = 0; i < threads; i++){
			arg = malloc(sizeof(Argument));
			arg->arr = array + i*(chunk);
			arg->size = chunk;
			if(i == threads-1){
				arg->size += size % threads;
			}
			pthread_create(&tids[i], NULL, calc_sum, (void *)arg);
		}
		for(i = 0; i < threads; i++){
			pthread_join(tids[i], NULL);
		}
		
		printf("%lu\n", sum);
	}
	pthread_mutex_destroy(&mutex);
	return 0;
}
