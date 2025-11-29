#include <immintrin.h> // AVX2
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// 32-bit integer type
typedef int32_t sort_type;

// Helper
void print_array(sort_type *arr, int n) {
  printf("[");
  for (int i = 0; i < n; i++) {
    printf("%d%s", arr[i], (i < n - 1) ? ", " : "");
  }
  printf("]\n");
}

// Scalar Insertion Sort for small arrays
void insertion_sort(sort_type *arr, int n) {
  for (int i = 1; i < n; i++) {
    sort_type key = arr[i];
    int j = i - 1;
    while (j >= 0 && arr[j] > key) {
      arr[j + 1] = arr[j];
      j--;
    }
    arr[j + 1] = key;
  }
}

// AVX2 Optimized Merge
void merge_avx2(sort_type *arr, sort_type *temp, int left, int mid, int right) {
  int i = left;
  int j = mid + 1;
  int k = left;

  // Main comparison loop (Scalar for now, hard to vectorize efficiently without
  // branchless logic)
  while (i <= mid && j <= right) {
    if (arr[i] <= arr[j]) {
      temp[k++] = arr[i++];
    } else {
      temp[k++] = arr[j++];
    }
  }

  // Vectorized Copy for remaining elements
  // Copy remaining from left
  while (i <= mid) {
    // If enough elements, use AVX2
    if (mid - i + 1 >= 8) {
      _mm256_storeu_si256((__m256i *)&temp[k],
                          _mm256_loadu_si256((__m256i *)&arr[i]));
      k += 8;
      i += 8;
    } else {
      temp[k++] = arr[i++];
    }
  }

  // Copy remaining from right
  while (j <= right) {
    if (right - j + 1 >= 8) {
      _mm256_storeu_si256((__m256i *)&temp[k],
                          _mm256_loadu_si256((__m256i *)&arr[j]));
      k += 8;
      j += 8;
    } else {
      temp[k++] = arr[j++];
    }
  }

  // Vectorized Copy Back
  int p = left;
  while (p <= right) {
    if (right - p + 1 >= 8) {
      _mm256_storeu_si256((__m256i *)&arr[p],
                          _mm256_loadu_si256((__m256i *)&temp[p]));
      p += 8;
    } else {
      arr[p] = temp[p];
      p++;
    }
  }
}

// Recursive Sort with Threshold
void merge_sort_recursive(sort_type *arr, sort_type *temp, int left,
                          int right) {
  // Base case optimization: Insertion Sort for small chunks
  if (right - left + 1 <= 32) { // Threshold 32 is common
    insertion_sort(arr + left, right - left + 1);
    return;
  }

  if (left < right) {
    int mid = left + (right - left) / 2;
    merge_sort_recursive(arr, temp, left, mid);
    merge_sort_recursive(arr, temp, mid + 1, right);
    merge_avx2(arr, temp, left, mid, right);
  }
}

// Main Entry Point
void baseline_merge_sort(sort_type *arr, int n) {
  if (n <= 1)
    return;
  sort_type *temp = (sort_type *)malloc(n * sizeof(sort_type));
  if (!temp) {
    fprintf(stderr, "Malloc failed\n");
    exit(1);
  }

  merge_sort_recursive(arr, temp, 0, n - 1);

  free(temp);
}

// --- TEST CORRECTNESS ---
bool verify_sorted(sort_type *arr, int n) {
  for (int i = 0; i < n - 1; i++) {
    if (arr[i] > arr[i + 1])
      return false;
  }
  return true;
}

void run_test(const char *name, sort_type *arr, int n) {
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

// For calculating cost per GB as hw requests. We set hardware cost here,
// basically dollars per hour.
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
  sort_type *arr = (sort_type *)malloc(total_bytes);

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
  sort_type *t6 = (sort_type *)malloc(large_n * sizeof(sort_type));
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
  run_gb_test(4); // Requires ~8GB RAM
  // run_gb_test(8); // Requires ~16GB RAM
  // run_gb_test(10); // Requires ~20GB RAM

  return 0;
}