// test
#include <stdio.h>
#include <assert.h>


int main() {

    int x = 5;
    if (x == 7)
        printf("YES\n");
    printf("NO\n");
    assert(x == 5);

    return 0;
}