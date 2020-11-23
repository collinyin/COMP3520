#include <stdio.h>
#include <string.h>

void q2(void) {
    // using scanf for input
    printf("What is your name?\n");
    char name1[20];
    scanf("%s", name1);
    printf("Your name is %s\n", name1);

    // // using fgets for input
    // printf("What is your name?\n");
    // char name[20];
    // fgets(name, 20, stdin);
    // printf("Your name is %s\n", name);
}

void q3(void) {
    char str[50];
    while (scanf("%s", str) != EOF) {
        printf("%s\n", str);
    }
}

void q4(void) {
    const char * ptr = "hello";
    const char array[] = "hello";
    const char array2[] = { 'h', 'e', 'l', 'l', 'o' };
    const char array3[] = { 'h', 'e', 'l', 'l', 'o', '\0' }; 
    const char array4[5] = { 'h', 'e', 'l', 'l', 'o' }; 
    const char array5[6] = { 'h', 'e', 'l', 'l', 'o', 0 }; 
    const char array6[20] = { 'h', 'e', 'l', 'l', 'o' }; 
    const char array7[20] = { 0 };
    const char array8[20] = "hello";

    printf("%zu %zu\n", sizeof(ptr), sizeof(array));
    // prediction: 8 bytes, 6 bytes (correct!)

    printf("%zu %zu\n", sizeof(array2), sizeof(array3));
    // prediction: 5 bytes, 6 bytes (correct!)

    printf("%zu %zu\n", sizeof(*ptr), sizeof(&array));
    // prediction: 1 bytes, 8 bytes (correct!)

    printf("%zu %zu\n", sizeof(&array2), sizeof(&array3));
}

void q7(int a[], int b[]) {
    int c[1];
    printf("a: %d b: %d\n", a[0], b[0]);
    
    c[0] = a[0];
    a[0] = b[0];
    b[0] = c[0];

    printf("a: %d b: %d\n", a[0], b[0]);

}

void q9(char str[], int length) {
    char reversed[100];
    int i = 0;
    printf("%s\n", reversed);
    while (strcmp(&str[i],"\n") != 0) {
        printf("%d\n", i);
        reversed[length - i - 1] = str[i];
        i++;
    }

    printf("%s\n", reversed);
}


int main(void) {

    // q2();
    // q3();
    // q4();

    // // question 7. swap
    // int a[1] = {5};
    // int b[1] = {6};
    // q7(a,b);

    // question 9. reverse
    char str[100];
    fgets(str, 100, stdin);
    int length = strlen(str);
    q9(str, length);

    return 0;
}