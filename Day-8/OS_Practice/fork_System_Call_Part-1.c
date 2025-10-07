#include <stdio.h>
#include <unistd.h>

int main(){
    // Fork generate a child Process for the Parent Process
    for(int i=0;i<4;i++)fork(); // See the Power of Multithreading in your PC

    // 15 Child Process and 1 Parent Process execute this Statement Parallelely
    printf("Welcome to the System Level Programming\n"); 
}