#include <stdio.h>

// Recursive function to calculate Fibonacci numbers
unsigned long long int fibonacci(int n) {
    if (n <= 1) {
        return n;
    } else {
        return fibonacci(n-1) + fibonacci(n-2);
    }
}

int main() {
    int n = 43; // This will generate a large Fibonacci number and consume significant CPU time.

    printf("Calculating the %dth Fibonacci number...\n", n);
    unsigned long long int fibNumber = fibonacci(n);
    printf("Fibonacci number %d is %llu.\n", n, fibNumber);

    return 0;
}
