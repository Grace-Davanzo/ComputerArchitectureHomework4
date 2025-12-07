#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Data type (32-bit signed int)
typedef int32_t sort_type;

// Debug print helper
void print_array(sort_type *arr, int n) {
  printf("[");
  for (int i = 0; i < n; i++) {
    printf("%d%s", arr[i], (i < n - 1) ? ", " : "");
  }
  printf("]\n");
}

// Standard merge logic
void merge(sort_type *arr, sort_type *temp, int left, int mid, int right) {
  int i = left;
  int j = mid + 1;
  int k = left;

  // Merge sorted halves
  while (i <= mid && j <= right) {
    if (arr[i] <= arr[j]) {
      temp[k++] = arr[i++];
    } else {
      temp[k++] = arr[j++];
    }
  }

  // Copy remaining elements
  while (i <= mid)
    temp[k++] = arr[i++];
  while (j <= right)
    temp[k++] = arr[j++];
  for (i = left; i <= right; i++)
    arr[i] = temp[i];
}

// Recursive function
void merge_sort_recursive(sort_type *arr, sort_type *temp, int left,
                          int right) {
  if (left < right) {
    int mid = left + (right - left) / 2;
    merge_sort_recursive(arr, temp, left, mid);
    merge_sort_recursive(arr, temp, mid + 1, right);
    merge(arr, temp, left, mid, right);
  }
}

// Main wrapper
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

// Verification helper
bool verify_sorted(sort_type *arr, int n) {
  for (int i = 0; i < n - 1; i++) {
    if (arr[i] > arr[i + 1])
      return false;
  }
  return true;
}

void run_test(const char *name, sort_type *arr, int n) {
  printf("\n=== Running Test: %s (n=%d) ===\n", name, n);

  // Print inputs if small
  if (n <= 20) {
    printf("Before: ");
    print_array(arr, n);
  }

  clock_t start = clock();
  baseline_merge_sort(arr, n);
  clock_t end = clock();

  // Print outputs if small
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

// Hardware cost rate ($/hr) for estimation
#define HOURLY_COST 0.10

void run_gb_test(int gb) {
  printf("\n============================================================\n");
  printf("       RUNNING LARGE SCALE TEST: %d GB\n", gb);
  printf("============================================================\n");

  // 1. Setup dimensions
  size_t total_bytes = (size_t)gb * 1024 * 1024 * 1024;
  size_t num_elements = total_bytes / sizeof(sort_type);

  printf("[INFO] Dataset Configuration:\n");
  printf("   - Size:   %.4f GB\n", (double)total_bytes / 1e9);
  printf("   - Count:  %zu elements\n", num_elements);

  // 2. Allocation
  printf("[INFO] Allocating Memory...\n");
  sort_type *arr = (sort_type *)malloc(total_bytes);

  if (arr == NULL) {
    fprintf(stderr, "[ERROR] Malloc failed! Need %d GB free RAM.\n", gb * 2);
    return;
  }

  // 3. Data Generation (Random)
  printf("[INFO] Generating random 32-bit integers...\n");
  srand(time(NULL));
  for (size_t i = 0; i < num_elements; i++) {
    uint32_t r32 = ((rand() & 0xFFFF) << 16) | (rand() & 0xFFFF);
    arr[i] = (sort_type)r32;
  }

  // 4. Execution
  printf("[INFO] Sorting...\n");
  clock_t start = clock();

  baseline_merge_sort(arr, num_elements);

  clock_t end = clock();

  // 5. Verification & Cost Analysis
  double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
  printf("[INFO] Verifying correctness...\n");

  if (verify_sorted(arr, num_elements)) {
    printf("\n[RESULT] SUCCESS!\n");
    printf("   - Time Taken:   %.4f seconds\n", time_taken);

    // Throughput
    double throughput = (double)gb / time_taken;
    printf("   - Throughput:   %.4f GB/s\n", throughput);

    // Hardware Cost Calculation
    double cost_per_sec = HOURLY_COST / 3600.0;
    double total_run_cost = time_taken * cost_per_sec;
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

  // Test 2: Edge Cases
  sort_type t2[] = {INT_MAX, 0, INT_MIN, -1, 1, INT_MAX - 1, INT_MIN + 1};
  run_test("32-bit Edge Cases", t2, 7);

  // Test 3: Already Sorted
  sort_type t3[] = {1, 2, 3, 4, 5, 6, 7, 8};
  run_test("Already Sorted", t3, 8);

  // Test 4: Reverse Sorted
  sort_type t4[] = {100, 90, 80, 70, 60, 50, 40};
  run_test("Reverse Sorted", t4, 7);

  // Test 5: Duplicates
  sort_type t5[] = {5, 1, 5, 2, 5, 3};
  run_test("Duplicates", t5, 6);

  // Test 6: Large Random (100k)
  int large_n = 100000;
  sort_type *t6 = (sort_type *)malloc(large_n * sizeof(sort_type));
  srand(42);
  for (int i = 0; i < large_n; i++) {
    uint32_t r = (rand() << 16) | rand();
    t6[i] = (sort_type)r;
  }
  run_test("Large Random (100k)", t6, large_n);
  free(t6);

  // Test 7: Large Scale (Gigabytes)
  run_gb_test(1); // 1GB (Requires ~2GB RAM)
  run_gb_test(2); // 2GB (Requires ~4GB RAM)
  run_gb_test(4); // 4GB (Requires ~8GB RAM)
  // run_gb_test(8);
  // run_gb_test(10);

  return 0;
}