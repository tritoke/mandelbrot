#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct arg {
  int threadnum;
  int * target_array;
} arg;

void * process_a_thing(void * varg) {
  arg * a = (arg *) varg;
  printf("\nthread %d started.\n", a->threadnum);
  sleep(a->threadnum);
  for (int i = 0; i < 10; i++)
    a->target_array[i] = a->threadnum * i;
  printf("\nthread %d done.\n", a->threadnum);
  return NULL;
}

int main(void) {
  // use 8 threads
  int num_threads = 8;
  // allocate the space for all 10 numbers the threads will write
  int * nums = malloc(sizeof(int) * num_threads * 10);
  // allocate the space for the pointers to the threads
  pthread_t tid;
  // create the array of arguments
  arg ** args = malloc(sizeof(arg *) * num_threads); 
  for (int i = 0; i < num_threads; i++) {
    args[i] = malloc(sizeof(arg));
    args[i]->threadnum = i;
    args[i]->target_array = &nums[i*10];
    if (pthread_create(&tid, NULL, process_a_thing, (void *) args[i])) {
      fprintf(stderr, "Error creating threads\n");
      return 1;
    } else printf("thread %d created\n", i);
  }

  if (pthread_join(tid, NULL)) {
    fprintf(stderr, "Error joining thread\n");
    return 2;
  } else puts("joined threads");

  printf("WE HAVE THE NUMBERSSSSSS\n");
  for (int i = 0; i < num_threads; i++) {
    free(args[i]);
    printf("Thread: %d\n", i);
    for (int j = 0; j < 10; j++) {
      printf("%d\n", nums[10*i + j]);
    }
  }
  free(args);
  free(nums);
  return 0;
}
