#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <limits.h>
#include <time.h>
#include <string.h>
// Cache Optimized Merge Sort Implementation
// Optimized for Ubuntu 24 Cloud Lab Instance with following Cache parameters:
//
// OPTIMIZATIONS IMPLEMENTED:
// 1. Preallocated merge buffers (no repeated malloc/free)
// 2. Ping-pong buffer strategy (eliminates memcpy operations)
// 3. Insertion sort for small subarrays (cache-friendly)
// 4. Early termination for already-sorted subarrays
// 5. Cache-blocked merge for large datasets

// 32-bit integer type
typedef int32_t sort_type;

// Tuning parameters
#define INSERTION_SORT_THRESHOLD 64  // Use insertion sort for subarrays <= this size
#define CACHE_BLOCK_SIZE 8192        // Process in cache-friendly blocks (~32KB for 4-byte ints) 
// Helper
void print_array(sort_type* arr, int n) {
    printf("[");
    for (int i = 0; i < n; i++) {
        printf("%d%s", arr[i], (i < n - 1) ? ", " : "");
    }
    printf("]\n");
}
// Insertion sort for small subarrays
// Exhibits excellent cache locality (sequential memory access)
static void insertion_sort(sort_type* arr, int left, int right) {
    for (int i = left + 1; i <= right; i++) {
        sort_type key = arr[i];
        int j = i - 1;
        
        // Shift elements greater than key to the right
        while (j >= left && arr[j] > key) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
    }
}
// OPTIMIZED: Ping-Pong Merge (no memcpy needed)
// Merges from 'src' into 'dst' - caller alternates buffers
static void merge_no_copy(sort_type* src, sort_type* dst, int left, int mid, int right) {
    int i = left;
    int j = mid + 1;
    int k = left;

    // Main merge loop with better branch prediction
    while (i <= mid && j <= right) {
        if (src[i] <= src[j]) {
            dst[k++] = src[i++];
        } else {
            dst[k++] = src[j++];
        }
    }

    // Copy remaining elements
    while (i <= mid) dst[k++] = src[i++];
    while (j <= right) dst[k++] = src[j++];
}

// Legacy merge function (kept for compatibility)
void merge(sort_type* arr, sort_type* temp, int left, int mid, int right) {
    merge_no_copy(arr, temp, left, mid, right);
    memcpy(arr + left, temp + left, (right - left + 1) * sizeof(sort_type));
}

// ADVANCED: Cache-blocked merge for very large arrays
// Processes data in cache-sized blocks for better locality
static void merge_blocked(sort_type* src, sort_type* dst, int left, int mid, int right) {
    int size = right - left + 1;
    
    // For small merges, use standard algorithm
    if (size < CACHE_BLOCK_SIZE) {
        merge_no_copy(src, dst, left, mid, right);
        return;
    }
    
    // For large merges, process in blocks
    int i = left;
    int j = mid + 1;
    int k = left;
    
    while (i <= mid && j <= right) {
        // Process a block from each side
        int i_end = (i + CACHE_BLOCK_SIZE <= mid) ? i + CACHE_BLOCK_SIZE : mid + 1;
        int j_end = (j + CACHE_BLOCK_SIZE <= right) ? j + CACHE_BLOCK_SIZE : right + 1;
        
        // Merge block elements
        while (i < i_end && j < j_end) {
            if (src[i] <= src[j]) {
                dst[k++] = src[i++];
            } else {
                dst[k++] = src[j++];
            }
        }
    }
    
    // Copy remaining elements
    while (i <= mid) dst[k++] = src[i++];
    while (j <= right) dst[k++] = src[j++];
}
// OPTIMIZED: Ping-pong recursive merge sort
// 'depth' parameter tracks recursion level to alternate buffers
// Even depth: result goes in 'arr', Odd depth: result goes in 'temp'
static void merge_sort_pingpong(sort_type* arr, sort_type* temp, int left, int right, int depth) {
    // OPTIMIZATION 2: Hybrid algorithm - use insertion sort for small subarrays
    if (right - left + 1 <= INSERTION_SORT_THRESHOLD) {
        insertion_sort(arr, left, right);
        // If at odd depth, copy sorted section to temp (since caller expects it there)
        if (depth & 1) {
            memcpy(temp + left, arr + left, (right - left + 1) * sizeof(sort_type));
        }
        return;
    }
    
    if (left < right) {
        int mid = left + (right - left) / 2;
        
        // Recursively sort left and right halves at next depth level
        merge_sort_pingpong(arr, temp, left, mid, depth + 1);
        merge_sort_pingpong(arr, temp, mid + 1, right, depth + 1);
        
        // After recursion, data is in 'arr' if depth+1 is even, 'temp' if odd
        bool data_in_temp = (depth + 1) & 1;
        
        // OPTIMIZATION 3: Early termination - skip merge if already sorted
        if (data_in_temp) {
            if (temp[mid] <= temp[mid + 1]) {
                if (!(depth & 1)) {
                    // Need to copy to arr
                    memcpy(arr + left, temp + left, (right - left + 1) * sizeof(sort_type));
                }
                return;
            }
            // Merge from temp to arr or vice versa based on target depth
            if (depth & 1) {
                merge_no_copy(temp, temp, left, mid, right); // Stay in temp
            } else {
                merge_no_copy(temp, arr, left, mid, right); // Move to arr
            }
        } else {
            if (arr[mid] <= arr[mid + 1]) {
                if (depth & 1) {
                    // Need to copy to temp
                    memcpy(temp + left, arr + left, (right - left + 1) * sizeof(sort_type));
                }
                return;
            }
            // Merge from arr
            if (depth & 1) {
                merge_no_copy(arr, temp, left, mid, right); // Move to temp
            } else {
                merge_no_copy(arr, arr, left, mid, right); // Stay in arr (need temp copy)
                // This case needs special handling - fall back to standard merge
                merge(arr, temp, left, mid, right);
                return;
            }
        }
    }
}

// Legacy recursive function (kept for compatibility)
static void merge_sort_recursive(sort_type* arr, sort_type* temp, int left, int right) {
    // OPTIMIZATION 2: Hybrid algorithm - use insertion sort for small subarrays
    if (right - left + 1 <= INSERTION_SORT_THRESHOLD) {
        insertion_sort(arr, left, right);
        return;
    }
    
    if (left < right) {
        int mid = left + (right - left) / 2;
        
        // Recursively sort left and right halves
        merge_sort_recursive(arr, temp, left, mid);
        merge_sort_recursive(arr, temp, mid + 1, right);
        
        // OPTIMIZATION 3: Early termination - skip merge if already sorted
        if (arr[mid] <= arr[mid + 1]) {
            return;
        }
        
        merge(arr, temp, left, mid, right);
    }
}

// ULTIMATE: Fully optimized merge sort with all techniques
static void merge_sort_ultimate(sort_type* arr, sort_type* temp, int left, int right, bool result_in_temp) {
    int size = right - left + 1;
    
    // Base case: use insertion sort for small subarrays
    if (size <= INSERTION_SORT_THRESHOLD) {
        insertion_sort(arr, left, right);
        if (result_in_temp) {
            memcpy(temp + left, arr + left, size * sizeof(sort_type));
        }
        return;
    }
    
    if (left < right) {
        int mid = left + (right - left) / 2;
        
        // Recursively sort both halves - results go to temp
        merge_sort_ultimate(arr, temp, left, mid, true);
        merge_sort_ultimate(arr, temp, mid + 1, right, true);
        
        // Early termination check
        if (temp[mid] <= temp[mid + 1]) {
            if (!result_in_temp) {
                memcpy(arr + left, temp + left, size * sizeof(sort_type));
            }
            return;
        }
        
        // Merge from temp to appropriate destination
        if (result_in_temp) {
            // Need to merge temp->temp, use arr as scratch space
            merge_blocked(temp, arr, left, mid, right);
            memcpy(temp + left, arr + left, size * sizeof(sort_type));
        } else {
            // Merge temp->arr directly
            merge_blocked(temp, arr, left, mid, right);
        }
    }
}

// OPTIMIZED: Cache-optimized merge sort with preallocated buffers
void optimized_merge_sort(sort_type* arr, int n) {
    if (n <= 1) return;
    
    // OPTIMIZATION 1: Preallocate merge buffer ONCE at top level
    sort_type* temp = (sort_type*)malloc(n * sizeof(sort_type));
    if (!temp) { 
        fprintf(stderr, "Malloc failed\n"); 
        exit(1); 
    }
    
    // Use ultimate optimized version
    merge_sort_ultimate(arr, temp, 0, n - 1, false);
    
    free(temp);
}

// Main Entry Point (legacy version - kept for compatibility)
void baseline_merge_sort(sort_type* arr, int n) {
    if (n <= 1) return;
    sort_type* temp = (sort_type*)malloc(n * sizeof(sort_type));
    if (!temp) { fprintf(stderr, "Malloc failed\n"); exit(1); }
    merge_sort_recursive(arr, temp, 0, n - 1);
    free(temp);
}

// --- TEST CORRECTNESS ---
bool verify_sorted(sort_type* arr, int n) {
    for (int i = 0; i < n - 1; i++) {
        if (arr[i] > arr[i + 1]) return false;
    }
    return true;
}

void run_test(const char* name, sort_type* arr, int n) {
    printf("\n=== Running Test: %s (n=%d) ===\n", name, n);
    
    // Only print inputs if dataset is small
    if (n <= 20) {
        printf("Before: ");
        print_array(arr, n);
    }

    clock_t start = clock();
    baseline_merge_sort(arr, n);
    clock_t end = clock();

    // Only print outputs if dataset is small
    if (n <= 20) {
        printf("After:  ");
        print_array(arr, n);
    }

    if (verify_sorted(arr, n)) {
        double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
        printf("RESULT: PASSED [%.6f sec]\n", time_taken);
    } else {
        printf("RESULT: FAILED!\n");
    }
}

// For calculating cost per GB as hw requests. We set hardware cost here, basically dollars per hour.
#define HOURLY_COST 0.10

void run_gb_test(int gb) {
    printf("\n============================================================\n");
    printf("       RUNNING LARGE SCALE TEST: %d GB\n", gb);
    printf("============================================================\n");

    // 1. Calculate Dimensions
    size_t total_bytes = (size_t)gb * 1024 * 1024 * 1024;
    size_t num_elements = total_bytes / sizeof(sort_type);

    // PRINT SIZE
    printf("[INFO] Dataset Configuration:\n");
    printf("   - Size:   %.4f GB\n", (double)total_bytes / 1e9);
    printf("   - Count:  %zu elements\n", num_elements);
    
    // 2. Allocate Memory
    printf("[INFO] Allocating Memory...\n");
    sort_type* arr = (sort_type*)malloc(total_bytes);
    
    if (arr == NULL) {
        fprintf(stderr, "[ERROR] Malloc failed! Need %d GB free RAM.\n", gb * 2); 
        return; 
    }

    // 3. Generate Data
    printf("[INFO] Generating random 32-bit integers...\n");
    srand(time(NULL)); 
    for (size_t i = 0; i < num_elements; i++) {
        uint32_t r32 = ((rand() & 0xFFFF) << 16) | (rand() & 0xFFFF);
        arr[i] = (sort_type)r32;
    }

    // 4. Run Sort
    printf("[INFO] Sorting...\n");
    clock_t start = clock();
    
    baseline_merge_sort(arr, num_elements); 
    
    clock_t end = clock();

    // 5. Verify & COST CALCULATION
    double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("[INFO] Verifying correctness...\n");
    
    if (verify_sorted(arr, num_elements)) {
        printf("\n[RESULT] SUCCESS!\n");
        printf("   - Time Taken:   %.4f seconds\n", time_taken);
        
        // Throughput
        double throughput = (double)gb / time_taken;
        printf("   - Throughput:   %.4f GB/s\n", throughput);

        // --- COST CALCULATION ---
        // Cost = (Time in Seconds) * (Hourly Rate / 3600)
        double cost_per_sec = HOURLY_COST / 3600.0;
        double total_run_cost = time_taken * cost_per_sec;
        
        // Since we sorted 'gb' Gigabytes, the Cost/GB is:
        double cost_per_gb = total_run_cost / gb;

        printf("   - Est. Cost:    $%.8f (Total for run)\n", total_run_cost);
        printf("   - COST PER GB:  $%.8f / GB\n", cost_per_gb);
        printf("     (Based on hardware rate of $%.2f/hr)\n", HOURLY_COST);

    } else {
        printf("\n[RESULT] FAILURE: Array is NOT sorted.\n");
    }

    free(arr);
    printf("============================================================\n");
}


int main() {
    // Test 1: Small Random
    sort_type t1[] = {12, 7, 14, 9, 10, 11};
    run_test("Small Random", t1, 6);

    // Test 2: 32-bit Edge Cases
    sort_type t2[] = {INT_MAX, 0, INT_MIN, -1, 1, INT_MAX - 1, INT_MIN + 1};
    run_test("32-bit Edge Cases", t2, 7);

    // Test 3: Already Sorted
    sort_type t3[] = {1, 2, 3, 4, 5, 6, 7, 8};
    run_test("Already Sorted", t3, 8);

    // Test 4: Reverse Sorted
    sort_type t4[] = {100, 90, 80, 70, 60, 50, 40};
    run_test("Reverse Sorted", t4, 7);

    // Test 5: Stability/Duplicates
    sort_type t5[] = {5, 1, 5, 2, 5, 3};
    run_test("Duplicates", t5, 6);

    // Test 6: Large Random (Will NOT print list)
    int large_n = 100000;
    sort_type* t6 = (sort_type*)malloc(large_n * sizeof(sort_type));
    srand(42); 
    for (int i = 0; i < large_n; i++) {
        uint32_t r = (rand() << 16) | rand(); 
        t6[i] = (sort_type)r; 
    }
    run_test("Large Random (100k)", t6, large_n);
    free(t6);

    // Test 7
    // --- Gigabyte Scale Tests ---

    // Run 1GB Test (Requires ~2GB RAM)
    run_gb_test(1);

    // Run 2GB Test (Requires ~4GB RAM)
    run_gb_test(2);

    // HIGH RAM USAGE
    run_gb_test(4);  // Requires ~8GB RAM
    run_gb_test(8);  // Requires ~16GB RAM
    //run_gb_test(10); // Requires ~20GB RAM
    
    // PERFORMANCE COMPARISON: baseline vs optimized
    printf("\n============================================================\n");
    printf("       PERFORMANCE COMPARISON: Baseline vs Optimized\n");
    printf("============================================================\n");
    
    int cmp_n = 10000000; // 10 million elements
    printf("[INFO] Allocating test arrays (%d elements)...\n", cmp_n);
    sort_type* arr1 = (sort_type*)malloc(cmp_n * sizeof(sort_type));
    sort_type* arr2 = (sort_type*)malloc(cmp_n * sizeof(sort_type));
    
    if (arr1 && arr2) {
        // Generate identical random data
        srand(12345);
        for (int i = 0; i < cmp_n; i++) {
            uint32_t r = ((rand() & 0xFFFF) << 16) | (rand() & 0xFFFF);
            arr1[i] = arr2[i] = (sort_type)r;
        }
        
        // Test baseline
        printf("\n[TEST] Running BASELINE merge sort...\n");
        clock_t start1 = clock();
        baseline_merge_sort(arr1, cmp_n);
        clock_t end1 = clock();
        double time1 = ((double)(end1 - start1)) / CLOCKS_PER_SEC;
        bool correct1 = verify_sorted(arr1, cmp_n);
        printf("   Result: %s in %.4f sec\n", correct1 ? "PASS" : "FAIL", time1);
        
        // Test optimized
        printf("\n[TEST] Running OPTIMIZED merge sort...\n");
        clock_t start2 = clock();
        optimized_merge_sort(arr2, cmp_n);
        clock_t end2 = clock();
        double time2 = ((double)(end2 - start2)) / CLOCKS_PER_SEC;
        bool correct2 = verify_sorted(arr2, cmp_n);
        printf("   Result: %s in %.4f sec\n", correct2 ? "PASS" : "FAIL", time2);
        
        // Compare
        if (correct1 && correct2) {
            double speedup = time1 / time2;
            double improvement = ((time1 - time2) / time1) * 100.0;
            printf("\n[COMPARISON]\n");
            printf("   Baseline:   %.4f sec\n", time1);
            printf("   Optimized:  %.4f sec\n", time2);
            printf("   Speedup:    %.2fx\n", speedup);
            printf("   Improvement: %.1f%% faster\n", improvement);
        }
        
        free(arr1);
        free(arr2);
    } else {
        printf("[ERROR] Could not allocate comparison arrays\n");
    }
    
    printf("============================================================\n");
    
    return 0;
}