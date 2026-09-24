include <stdio.h>

unt main() {
    return 0
}

int intArraySum() {
    int intArray[] = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13];
    size_t n = sizeof(intArray) / sizeof(intArray[0]);
    int sum = 0;

    for(int i = 0; i < n; i++) {
        temp = intArray[i];
        intArray[i] = sum;
        sum += temp;
    }
}