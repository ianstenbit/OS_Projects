#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define MAX_LINE 80

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

void run_command(char** args){

  int i = 0;
  while(args[i][0]) i++;

  int should_wait = 0;

  char** args_cat = malloc(i * sizeof(char*));
  for(int j = 0; j < i; j++){
     args_cat[j] = args[j];
     printf("Token: %s\n", args[j]);
  }

  if(args_cat[i-1][0] == '&' && args_cat[i-1][1] == 0){
    should_wait = 1;
    args_cat[i-1] = 0;
  } else {
    args_cat[i] = 0;
  }

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
  }


}


int main(){

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

        printf("Input: %s\n", tmp);
        tokenize(args, tmp);
        fflush(stdout);

        run_command(args);

    }

    for(int i = 0; i < MAX_LINE/2+2; i++){
      free(args[i]);
    }

    return 0;

}
