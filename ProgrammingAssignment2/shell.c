#include <stdio.h>
#include <unistd.h>

#define MAX_LINE 80

int main(){

    char* args[MAX_LINE/2 + 1];
    int should_run = 1;

    while(should_run){
        printf("osh>");
        fflush(stdout);

        /*
        Fork, execvp, and wait() if needed
        */

    }

    return 0;

}
