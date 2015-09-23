
#include <pthread.h>

void * worker_function(void * arg) {
  printf("%s\n\n", arg);
}

int main(int argc, char** argv) {
  pthread_t * threads;
  int i;
  int length = atoi(argv[1]);
  printf("Argc is %d\n",argc);
  printf("Length is %d\n", length);
  threads = (pthread_t*) malloc(length * sizeof(pthread_t));
  char hello[] = "Hello! ";
  hello[6] = 0;
  for (i = 0; i < length; i++) {
    printf("Creating a thread\n");
    pthread_create(&threads[i], NULL, worker_function, &hello);
  }

  for (i = 0; i < length; i++) {
    pthread_join(&threads[i], NULL);
    printf("Joining a thread\n");
  }
}
