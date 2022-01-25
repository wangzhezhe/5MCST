#include "hellofunc.h"
#include <stdio.h>
void hellofunc(){
    //if we use the nm to check the generated binary file
    //there is no printf
    printf("hello, gcc\n");
}