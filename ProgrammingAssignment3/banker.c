#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>

#define NUM_CUSTOMERS 5
#define NUM_RESOURCES 3

int available[NUM_RESOURCES];
int maximum[NUM_CUSTOMERS][NUM_RESOURCES];
int allocation[NUM_CUSTOMERS][NUM_RESOURCES];
int need[NUM_CUSTOMERS][NUM_RESOURCES];

//MUTEX FOR AVAILABLE

pthread_t threads[NUM_CUSTOMERS];
int customer_numbers[NUM_CUSTOMERS];

pthread_mutex_t lock;

int request_resources(int cid, int* request){

  return 0;

}

void make_request(int cid){

  int request[NUM_RESOURCES];
  for(int i = 0; i < NUM_RESOURCES; i++)
    request[i] = (int)((rand()/(1.0*RAND_MAX)) * need[cid][i]);

  pthread_mutex_lock(&lock);
  if(request_resources(cid, request))
    printf("Denied request for customer %i: ", cid);
  else
    printf("Accepted request for customer %i: [", cid);

  for(int i = 0; i < NUM_RESOURCES; i++) printf("%i, ", request[i]);
  printf("\b\b].\n");
  pthread_mutex_unlock(&lock);


}

int release_resources(int cid, int* release){

  pthread_mutex_lock(&lock);

  for(int i = 0; i < NUM_RESOURCES; i++)
      available[i] += release[i];

  pthread_mutex_unlock(&lock);

  return -1;

}

void do_release(int cid){

  int release[NUM_RESOURCES];
  for(int i = 0; i < NUM_RESOURCES; i++){
    release[i] = (int)((rand()/(1.0*RAND_MAX)) * allocation[cid][i]);
    need[cid][i] += release[i];
    allocation[cid][i] -= release[i];
  }

  if(release_resources(cid, release))
    printf("Release for customer %i: ", cid);
  else
    printf("FAILED release for customer %i: [", cid);

  for(int i = 0; i < NUM_RESOURCES; i++) printf("%i, ", release[i]);
  printf("\b\b].\n");

}


void* customer_loop(void* customer){

  int customer_num = *((int *) customer);
  printf("Customer Number: %i\n", customer_num);

  while(1){

    make_request(customer_num);
    do_release(customer_num);

  }

  return NULL;

}

int main(int argc, char* argv[]){

  if (pthread_mutex_init(&lock, NULL) != 0){
    printf("\n mutex init failed\n");
    return 3;
  }

  for(int i = 0; i < NUM_RESOURCES; i++){
    available[i] = atoi(argv[i+1]);

    for(int j = 0; j < NUM_CUSTOMERS; j++){
      maximum[j][i] = available[i]/2;
      allocation[j][i] = 0;
      need[j][i] = maximum[j][i];
    }

  }

  for(int i = 0; i < NUM_CUSTOMERS; i++){
    customer_numbers[i] = i;
    if(pthread_create(threads+i, NULL, customer_loop, customer_numbers+i)) {
      printf("Error on thread create\n");
      return 1;
    }
  }

  for(int i = 0; i < NUM_CUSTOMERS; i++){
    if(pthread_join(threads[i], NULL)) {
      printf("Error on thread join\n");
      return 2;
    }
  }

  pthread_mutex_destroy(&lock);

}
