#ifndef UTILS_H
#define UTILS_H

// Made by Comrade Orion!
// Utility macros for various bit manipulations and sorting

// LFSR polynomials
#define LF_POLY_ODD  (0x29CE5C)
#define LF_POLY_EVEN (0x870804)

// Get the nth bit of x
#define BIT(x, n) (((x) >> (n)) & 1)

// Get the nth bit of x, but flipped based on big-endian order
#define BEBIT(x, n) BIT(x, (n) ^ 24)

// Swap the endianness of a 32-bit integer
#define SWAPENDIAN(x) \
    ((x) = (((x) >> 8 & 0xff00ff) | ((x) & 0xff00ff) << 8), (x) = ((x) >> 16) | ((x) << 16))

// Swap two variables of the same type - optimized ARM assembly implementation
// Uses direct register operations instead of XOR for better performance on STM32WB55
#define SWAP(x, y)                                              \
    do {                                                        \
        if(&(x) != &(y)) {                                      \
            unsigned int* px = &(x);                            \
            unsigned int* py = &(y);                            \
            asm volatile("ldr r2, [%0] \n" /* Load x into r2 */ \
                         "ldr r3, [%1] \n" /* Load y into r3 */ \
                         "str r2, [%1] \n" /* Store x into y */ \
                         "str r3, [%0] \n" /* Store y into x */ \
                         :                                      \
                         : "r"(px), "r"(py)                     \
                         : "r2", "r3", "memory");               \
        }                                                       \
    } while(0)

// Memory prefetch macro for cache optimization
#define PREFETCH(addr) __builtin_prefetch(addr, 0, 3)

// Swap two elements in an array if the first is greater than the second
#define SWAP_IF_GREATER(array, a, b)        \
    do {                                    \
        if((array)[a] > (array)[b]) {       \
            unsigned int temp = (array)[a]; \
            (array)[a] = (array)[b];        \
            (array)[b] = temp;              \
        }                                   \
    } while(0)

// Find the median of three elements in an array (low, mid, high)
// Optimized version that minimizes branches and comparisons
#define MEDIAN_OF_THREE(array, low, high)    \
    ({                                       \
        int mid = low + ((high - low) >> 1); \
        SWAP_IF_GREATER(array, low, mid);    \
        SWAP_IF_GREATER(array, low, high);   \
        SWAP_IF_GREATER(array, mid, high);   \
        (array)[mid];                        \
    })

// Insertion sort for small arrays - highly efficient for small chunks
#define INSERTION_SORT(array, low, high)          \
    do {                                          \
        for(int i = low + 1; i <= high; i++) {    \
            unsigned int key = (array)[i];        \
            int j = i - 1;                        \
            while(j >= low && (array)[j] > key) { \
                (array)[j + 1] = (array)[j];      \
                j--;                              \
            }                                     \
            (array)[j + 1] = key;                 \
        }                                         \
    } while(0)

// Partition for quicksort with median-of-three pivot and optimized boundary handling
#define PARTITION(array, low, high, pivot_index)   \
    ({                                             \
        unsigned int pivot = (array)[pivot_index]; \
        SWAP((array)[pivot_index], (array)[high]); \
        int i = low - 1;                           \
        for(int j = low; j < high; j++) {          \
            if((array)[j] <= pivot) {              \
                i++;                               \
                SWAP((array)[i], (array)[j]);      \
            }                                      \
        }                                          \
        SWAP((array)[i + 1], (array)[high]);       \
        i + 1;                                     \
    })

// Optimized thresholds based on STM32WB55 cache and memory performance
#define INSERTION_SORT_THRESHOLD 16 // 9.664 KB Free - confirmed optimal value

// Macro to determine optimal pivot index using median-of-three method
#define GET_PIVOT_INDEX(array, low, high)    \
    ({                                       \
        int mid = low + ((high - low) >> 1); \
        SWAP_IF_GREATER(array, low, mid);    \
        SWAP_IF_GREATER(array, low, high);   \
        SWAP_IF_GREATER(array, mid, high);   \
        mid;                                 \
    })

#endif // UTILS_H
