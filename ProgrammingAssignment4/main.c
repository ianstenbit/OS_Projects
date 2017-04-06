#define PAGE_REF_STRING_LENGTH 20

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

int contains(int* arr, int num, int len){

  for(int i = 0; i < len; i++)
    if(arr[i] == num)
      return 1;

  return 0;

}

int arg_max(int* arr, int len){

  int max = -1;
  int argmax = 0;

  for(int i = 0; i < len; i++){
    if(arr[i] >= 0 && arr[i] > max){
      max = arr[i];
      argmax = i;
    }
  }

  return argmax;

}

int arg_min(int* arr, int len){

  int min = PAGE_REF_STRING_LENGTH + 1;
  int argmin = 0;

  for(int i = 0; i < len; i++){
    if(arr[i] >= 0 && arr[i] < min){
      min = arr[i];
      argmin = i;
    }
  }

  return argmin;

}

int OPT_helper(int* memory, int* page_ref, int loc, int max_frames){

  int* location = malloc(max_frames * sizeof(int));
  for(int i = 0; i < max_frames; i++){
    location[i] = PAGE_REF_STRING_LENGTH + 1;
  }

  for(int i = loc + 1; i < PAGE_REF_STRING_LENGTH; i++){

    for(int j = 0; j < max_frames; j++)
      if(page_ref[i] == memory[j] && location[j] == PAGE_REF_STRING_LENGTH + 1)
        location[j] = i;

  }

  int val = arg_max(location, max_frames);
  free(location);

  return val;

}

void algorithms(int* page_ref, int max_frames){

  int* FIFO_memory = malloc(max_frames * sizeof(int));
  memset(FIFO_memory, 1, max_frames * sizeof(int));
  int* LRU_memory = malloc(max_frames * sizeof(int));
  memset(LRU_memory, 1, max_frames * sizeof(int));
  int* OPT_memory = malloc(max_frames * sizeof(int));
  memset(OPT_memory, 1, max_frames * sizeof(int));

  int FIFO_faults = 0, LRU_faults = 0, OPT_faults = 0;

  int* entered = malloc(max_frames * sizeof(int));
  memset(entered, 0, max_frames * sizeof(int));

  int* used = malloc(max_frames * sizeof(int));
  memset(used, 0, max_frames * sizeof(int));


  for(int i = 0; i < PAGE_REF_STRING_LENGTH; i++){

    //FIFO
    if(!contains(FIFO_memory, page_ref[i], max_frames)){

      printf("Page fault in FIFO on page number %i at index %i\n", page_ref[i], i);
      printf("\nMemory before swap: [");
      for(int j = 0; j < i && j < max_frames) printf("%i,", FIFO_memory[j]);
      printf("\b]\n");

      FIFO_faults++;
      int repl_index = arg_min(entered, max_frames);
      FIFO_memory[repl_index] = page_ref[i];
      entered[repl_index] = i + 1;

      printf("\nMemory after swap: [");
      for(int j = 0; j < i && j < max_frames) printf("%i,", FIFO_memory[j]);
      printf("\b]\n");

    }

    //LRU
    if(!contains(LRU_memory, page_ref[i], max_frames)){

      printf("Page fault in LRU on page number %i at index %i\n", page_ref[i], i);
      printf("\nMemory before swap: [");
      for(int j = 0; j < i && j < max_frames) printf("%i,", LRU_memory[j]);
      printf("\b]\n");

      LRU_faults++;
      int repl_index = arg_min(used, max_frames);
      LRU_memory[repl_index] = page_ref[i];
      used[repl_index] = 0;

      printf("\nMemory after swap: [");
      for(int j = 0; j < i && j < max_frames) printf("%i,", LRU_memory[j]);
      printf("\b]\n");

    }

    for(int j = 0; j < max_frames; j++) if(LRU_memory[j] == page_ref[i]) used[j] = i;

    //OPT
    if(!contains(OPT_memory, page_ref[i], max_frames)){

      printf("Page fault in OPT on page number %i at index %i\n", page_ref[i], i);
      printf("\nMemory before swap: [");
      for(int j = 0; j < i && j < max_frames) printf("%i,", OPT_memory[j]);
      printf("\b]\n");

      OPT_faults++;
      int repl_index = OPT_helper(OPT_memory, page_ref, i, max_frames);
      OPT_memory[repl_index] = page_ref[i];

      printf("\nMemory after swap: [");
      for(int j = 0; j < i && j < max_frames) printf("%i,", OPT_memory[j]);
      printf("\b]\n");

    }


  }

  printf("\tFIFO Faults: %i\n", FIFO_faults);
  printf("\tLRU Faults: %i\n", LRU_faults);
  printf("\tOPT Faults: %i\n", OPT_faults);


  free(entered);
  free(used);

}


int main(int argc, char* argv[]){

    int page_ref_string[PAGE_REF_STRING_LENGTH];


    printf("String: ");
    for(int i = 0; i < PAGE_REF_STRING_LENGTH; i++){
      page_ref_string[i] = (int) ((rand()/(1.0*RAND_MAX)) * 10);
      printf("%i", page_ref_string[i]);
    }
    printf("\n");

    int population_of_page_frames = 0;

    for(int max_pages = 1; max_pages <= 7; max_pages++){

        printf("Number of Page Frames: %i\n", max_pages);

        //printf("\tFIFO: %i faults\n", FIFO(page_ref_string, max_pages, memory));
        //printf("\tLRU: %i faults\n", LRU(page_ref_string, max_pages, memory));
        //OPT(page_ref_string, max_pages, memory)

        algorithms(page_ref_string, max_pages);

    }

}
