#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Function to check if a number is prime
bool isPrime(int number) {
    if (number <= 1) return false;
    if (number <= 3) return true;
    if (number % 2 == 0 || number % 3 == 0) return false;

    for (int i = 5; i * i <= number; i += 6) {
        if (number % i == 0 || number % (i + 2) == 0) return false;
    }

    return true;
}

int main() {
    int limit = 10000000; // Find primes up to this limit
    int primeCount = 0;

    printf("Calculating prime numbers up to %d...\n", limit);

    for (int number = 2; number <= limit; number++) {
        if (isPrime(number)) {
            primeCount++;
        }
    }

    printf("Total prime numbers found: %d\n", primeCount);

    return 0;
}
