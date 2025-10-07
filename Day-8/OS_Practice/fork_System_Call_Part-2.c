#include "stdio.h"
#include "unistd.h"

void forkWithAnd(){
    if(fork() && fork()){
        fork();
    }
    printf("And ");
}

void forkWithOR(){
    if(fork() || fork()){
        fork();
    }
    printf("Or ");
}

int main(){
    forkWithAnd();
    return 0;
}