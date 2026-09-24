#include <stdio.h>

int main() {
    
    return 0
}

int maxIntegerA() {
    //Integer Array using version A
    //TODO: Where do I get the array to test from?
    int[] intArray = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10];
    size_t n = sizeof(intArray) / sizeof(intArray[0])
    int max = intArray[0];

    for(int i = 0; i < n; i++) {
        if(intArray[i] > max) {
            max = intArray[i];
        }
    }
    return max;
}

int maxIntegerB() {
    //Integer Array using version B
    //TODO: Where do I get the array to test from?
    int[] intArray = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10];
    size_t n = sizeof(intArray) / sizeof(intArray[0])
    int max = intArray[0];

    for(int i = 0; i < n; i++) {
        max = (intArray[i] > max) ? intArray[i] : max;
    }
    return max;
}

int maxDoubleA() {
    //Double Array using A
    //TODO: Where do I get the array to test from
    double[] doubleArray = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 11.33];
    size_t n = sizeof(doubleArray) / sizeof(doubleArray[0]);

    for(int i = 0; i < n; i++) {
        if(doubleArray[i] > max) {
            max = doubleArray[i];
        }
    }
    return max;

}

int maxDoubleB() {
    //Double Array using B
    //TODO: Where do I get the array to test from
    double[] doubleArray = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 11.33];
    size_t n = sizeof(doubleArray) / sizeof(doubleArray[0]);

    for(int i = 0; i < n; i++) {
        max = (doubleArray[i] > max) ? doubleArray[i] : max;
    }
    return max;

}
