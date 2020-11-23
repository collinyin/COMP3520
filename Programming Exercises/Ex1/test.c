/* practice C test code */

#include <stdio.h>

int main(int argc, char * argv[]) {

    // arguments test

    // printf("argc = %d\n", argc);
    // printf("argv[0] = %s\n", argv[0]);
    // printf("argv[1] = %s\n", argv[1]);


    // pointer test

    int num = 5;
    int * num_ptr = &num;

    printf("%d\n", *num_ptr);
    printf("%d\n", *&num);

    return 0;
}