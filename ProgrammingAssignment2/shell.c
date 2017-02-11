#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>

#define MAX_LINE 80
#define HISTORY_LENGTH 10

struct History {

    char** commands[10];
    int num_commands;

};

void tokenize(char** result, char* input){

    int input_index = 0;
    int output_index = 0;

    while(input[input_index]){
        int output_subindex = 0;
        while(input[input_index] && input[input_index] != ' ')
          result[output_index][output_subindex++] = input[input_index++];
        result[output_index][output_subindex] = 0;
        while(input[input_index] && input[input_index] == ' ') input_index++;
        output_index++;
    }

    result[output_index][0] = 0;

}

void printHistory(struct History* history){

    for(int i = history->num_commands - 1, j = 0; i >= 0 && j < 10; j++,i--){
       printf("%i ", i+1);
       int loc = 0;
       while(history->commands[i%10][loc]) printf("%s ", history->commands[i%10][loc++]);
       printf("\n");
       fflush(stdout);
    }
}

void addToHistory(char** args, struct History* history){
    if(history->num_commands >= 10) free(history->commands[(history->num_commands) % HISTORY_LENGTH]);
    history->commands[(history->num_commands) % HISTORY_LENGTH] = args;
    history->num_commands++;
}

char** getHistoryCommand(struct History* h, char* arg){

  if(arg[1] == '!'){
    if(h->num_commands > 0)
      return h->commands[(h->num_commands-1)%HISTORY_LENGTH];
    else {
      printf("No commands in history\n");
      fflush(stdout);
      return 0;
    }
  }

  int num = atoi(arg + 1);
  if(num > 0 && num >= h->num_commands - 10 && num <= h->num_commands)
    return h->commands[(num-1)%HISTORY_LENGTH];

  printf("No such command in history\n");
  fflush(stdout);
  return 0;
}

int run_command(char** args, struct History* history){

  int i = 0;
  while(args[i][0]) i++;

  int should_wait = 1;

  char** args_cat = malloc((i+1) * sizeof(char*));
  for(int j = 0; j < i; j++){
     args_cat[j] = malloc((strlen(args[j]) + 1) * sizeof(char));
     strcpy(args_cat[j], args[j]);
  }

  args_cat[i] = 0;

  if(i >= 1){

      if(strlen(args_cat[0]) >= 2){
        if(args_cat[0][0] == '!'){
          args_cat = getHistoryCommand(history, args_cat[0]);
          if(args_cat == 0) return 1;
          int loc = 0;
          while(args_cat[loc]) printf("%s ", args_cat[loc++]);
          printf("\n");
          fflush(stdout);

          i = 0;
          while(args_cat[i]) i++;
        }
      }

      if(args_cat[i-1][0] == '&' && args_cat[i-1][1] == 0){
        should_wait = 0;
        free(args_cat[i-1]);
        args_cat[i-1] = 0;
      }

      if(strlen(args_cat[0]) == 7 && strcmp(args_cat[0], "history") == 0) printHistory(history);

      if(strlen(args_cat[0]) == 4 && strcmp(args_cat[0], "exit") == 0) return 0;

  }

  addToHistory(args_cat, history);

  pid_t pid = fork();

  switch(pid){
    case -1:
        printf("Error forking\n");
        break;
    case 0:
        execvp(args_cat[0], args_cat);
        exit(0);
    default:
        if(should_wait) waitpid(pid, 0, 0);
        else{
          args_cat[i-1] = malloc(2*sizeof(char));
          args_cat[i-1][0] = '&';
          args_cat[i-1][1] = 0;
        }
  }

  return 1;

}

void cleanup(struct History* h){

  for(int i = h->num_commands - 1, j = 0; i >= 0 && j < 10; j++,i--){
     int loc = 0;
     for(int i2 = h->num_commands - 1, j2 = 0; i2 >= 0 && j2 < 10; j2++,i2--){
       if(h->commands[i%10] == h->commands[i2%10])
          h->commands[i2%10] = 0;
     }

     if(h->commands[i%10] == 0) continue;
     while(h->commands[i%10][loc]) free(h->commands[i%10][loc++]);
     free(h->commands[i%10]);
  }

  free(h->commands);

}


int main(){

    struct History* h = malloc(sizeof(struct History));
    h->num_commands = 0;

    char* args[MAX_LINE/2 + 2];
    for(int i = 0; i < MAX_LINE/2+2; i++){
      args[i] = malloc((MAX_LINE-1) * sizeof(char));
    }

    int should_run = 1;

    while(should_run){
        printf("osh>");
        fflush(stdout);

        char tmp[MAX_LINE + 1];
        fgets(tmp, MAX_LINE+1, stdin);

        tmp[strlen(tmp) - 1] = 0;
        tokenize(args, tmp);
        fflush(stdout);

        should_run = run_command(args, h);

    }

    for(int i = 0; i < MAX_LINE/2+2; i++){
      free(args[i]);
    }

    cleanup(h);

    return 0;

}
