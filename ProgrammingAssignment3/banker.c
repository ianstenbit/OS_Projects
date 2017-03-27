#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_CUSTOMERS 5
#define NUM_RESOURCES 3

int available[NUM_RESOURCES];
int maximum[NUM_CUSTOMERS][NUM_RESOURCES];
int allocation[NUM_CUSTOMERS][NUM_RESOURCES];
int need[NUM_CUSTOMERS][NUM_RESOURCES];

pthread_t threads[NUM_CUSTOMERS];
int customer_numbers[NUM_CUSTOMERS];

//This lock is used to protect available, max, alloc, and need (also, indirectly, the print-out buffer)
pthread_mutex_t lock;

void print_mat(int mat[NUM_CUSTOMERS][NUM_RESOURCES]){

  for(int i = 0; i < NUM_CUSTOMERS; i++){

    printf("    [");
    for(int j = 0; j < NUM_RESOURCES; j++) printf("%i, ", mat[i][j]);
    printf("\b\b]\n");

  }

}

void print_status(){

  printf("======= Current Status =======\n");
  printf("Available: \n    [");
  for(int i = 0; i < NUM_RESOURCES; i++) printf("%i, ", available[i]);
  printf("\b\b]\n");

  printf("\nMaximum: \n");
  print_mat(maximum);

  printf("\nAllocation: \n");
  print_mat(allocation);

  printf("\nNeed: \n");
  print_mat(need);

  printf("\n===== End Status =====\n\n");

}

void update(int cid, int* request){

  for(int i = 0; i < NUM_RESOURCES; i++){
    available[i] -= request[i];
    allocation[cid][i] += request[i];
    need[cid][i] -= request[i];
  }

}

int less_than_or_equal(int* a, int* b, int len){

  for(int i = 0; i < len; i++){
    if(a[i] > b[i]) return 0;
  }

  return 1;

}

int request_resources(int cid, int* request){

  int work[NUM_RESOURCES];
  int finish[NUM_CUSTOMERS];

  int order[NUM_CUSTOMERS];

  update(cid, request);

  for(int i = 0; i < NUM_RESOURCES; i++) {
    work[i] = available[i];
    if(available[i] < 0) return -1;
  }

  for(int i = 0; i < NUM_CUSTOMERS; i++){
    finish[i] = 0;
  }

  for(int iteration = 0; iteration < NUM_CUSTOMERS; iteration++){

    int found = 0;

    for(int i = 0; i < NUM_CUSTOMERS && !found; i++){

      if(finish[i] == 0 && less_than_or_equal(need[i], work, NUM_RESOURCES) == 1){

        found = 1;
        for(int j = 0; j < NUM_RESOURCES; j++) work[j] += allocation[i][j];
        order[iteration] = i;
        finish[i] = 1;

      }

    }

  }

  printf("Safe ordering: [");

  for(int i = 0; i < NUM_CUSTOMERS; i++) printf("%i, ", order[i]);
  printf("\b\b].\n");

  return 0;

}

void make_request(int cid){

  int request[NUM_RESOURCES];

  for(int i = 0; i < NUM_RESOURCES; i++)
    request[i] = (int)((rand()/(1.0*RAND_MAX)) * need[cid][i]);



  pthread_mutex_lock(&lock);

  printf("New request for customer %i: [", cid);

  for(int i = 0; i < NUM_RESOURCES; i++) printf("%i, ", request[i]);
  printf("\b\b].\n");

  if(request_resources(cid, request) == -1){
    printf("Denied request for customer %i: [", cid);

    for(int i = 0; i < NUM_RESOURCES; i++)
      request[i] = -request[i];
    update(cid, request);
    for(int i = 0; i < NUM_RESOURCES; i++)
      request[i] = -request[i];

  } else
    printf("Accepted request for customer %i: [", cid);

  for(int i = 0; i < NUM_RESOURCES; i++) printf("%i, ", request[i]);
  printf("\b\b].\n\n");
  pthread_mutex_unlock(&lock);


}

int release_resources(int cid, int* release){

  for(int i = 0; i < NUM_RESOURCES; i++)
      available[i] += release[i];

  return 0;

}

void do_release(int cid){

  int release[NUM_RESOURCES];
  for(int i = 0; i < NUM_RESOURCES; i++){
    release[i] = (int)((rand()/(3.0*RAND_MAX)) * allocation[cid][i]);
  }

  pthread_mutex_lock(&lock);

  for(int i = 0; i < NUM_RESOURCES; i++){
    need[cid][i] += release[i];
    allocation[cid][i] -= release[i];
  }

  if(release_resources(cid, release) == -1)
    printf("FAILED release for customer %i: [", cid);
  else
    printf("Release for customer %i: [", cid);

  for(int i = 0; i < NUM_RESOURCES; i++) printf("%i, ", release[i]);
  printf("\b\b].\n\n");


  pthread_mutex_unlock(&lock);

}


void* customer_loop(void* customer){

  int customer_num = *((int *) customer);

  while(1){

    make_request(customer_num);
    do_release(customer_num);


    pthread_mutex_lock(&lock);
    print_status();
    pthread_mutex_unlock(&lock);

    //Sleep for random time 0-1 second
    usleep( 1000000 * (rand()/(1.0 * RAND_MAX)));

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
      maximum[j][i] = (int)((rand()/(1.0 * RAND_MAX)) * available[i]) / 2;
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
