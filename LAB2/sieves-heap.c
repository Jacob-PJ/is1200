#include <stdio.h>
#include <stdlib.h>

#define COLUMNS 6

void print_number(int n) {
    printf("%2d", n);
    printf("\t");
}

void print_sieves(int n) {
    int counter = 0;
    int* numbers = (int*)malloc((n + 1) * sizeof(int));

    for (int i = 0; i <= n; i++)
        numbers[i] = 0;

    for (int i = 2; i * i <= n; i++) {
        if (numbers[i] == 0) {
            for (int j = i * i; j <= n; j += i)
                numbers[j] = 1;
        }
    }

    for (int i = 2; i <= n; i++) {
        if (numbers[i] == 0) {
            print_number(i);
            if (counter == COLUMNS) {
                printf("\n");
                counter = 0;
            }
            counter++;
        }
    }

    free(numbers); // Free the dynamically allocated memory
}

int main(int argc, char* argv[]) {
    if (argc == 2)
        print_sieves(atoi(argv[1]));
    else
        printf("Please state an integer number.\n");
    return 0;
}
