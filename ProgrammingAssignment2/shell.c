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

        int tok = 0;
        while(*(args[tok])){
          printf("Token: %s\n", args[tok++]);
          fflush(stdout);
        }
        /*
        Fork, execvp, and wait() if needed
        */

    }

    for(int i = 0; i < MAX_LINE/2+2; i++){
      free(args[i]);
    }

    return 0;

}
