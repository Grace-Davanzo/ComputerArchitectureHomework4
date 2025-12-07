#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// - Branchless comparison (CMOV)
// - Software Prefetching
// - Hybrid Sort

typedef int32_t sort_type;

#define INSERTION_SORT_THRESHOLD 32

// Prefetch macro (compiler intrinsic)
#if defined(__GNUC__) || defined(__clang__)
#define PREFETCH(addr) __builtin_prefetch((addr), 0, 1)
#else
#define PREFETCH(addr) (void)0
#endif

// Print helper
void print_array(sort_type *arr, int n) {
  printf("[");
  for (int i = 0; i < n; i++) {
    printf("%d%s", arr[i], (i < n - 1) ? ", " : "");
  }
  printf("]\n");
}

// Branchless Merge
void merge(sort_type *arr, sort_type *temp, int left, int mid, int right) {
  int i = left;
  int j = mid + 1;
  int k = left;

  const int PREFETCH_DISTANCE = 32;

  while (i <= mid && j <= right) {
    // Software Prefetching: Bring future data into cache
    if (i + PREFETCH_DISTANCE <= mid) {
      PREFETCH(&arr[i + PREFETCH_DISTANCE]);
    }
    if (j + PREFETCH_DISTANCE <= right) {
      PREFETCH(&arr[j + PREFETCH_DISTANCE]);
    }
    if (k + PREFETCH_DISTANCE <= right) {
      PREFETCH(&temp[k + PREFETCH_DISTANCE]);
    }

    sort_type ai = arr[i];
    sort_type aj = arr[j];

    // Branchless comparison: Avoids pipeline flush on misprediction
    int take_left = (ai <= aj);

    // Conditional move logic
    temp[k++] = take_left ? ai : aj;

    // Increment indices avoiding branches
    i += take_left;
    j += 1 - take_left;
  }

  while (i <= mid)
    temp[k++] = arr[i++];
  while (j <= right)
    temp[k++] = arr[j++];
  for (i = left; i <= right; i++)
    arr[i] = temp[i];
}

static void insertion_sort(sort_type *arr, int left, int right) {
  for (int i = left + 1; i <= right; ++i) {
    sort_type key = arr[i];
    int j = i - 1;
    while (j >= left && arr[j] > key) {
      arr[j + 1] = arr[j];
      --j;
    }
    arr[j + 1] = key;
  }
}

// Recursive Sort
void merge_sort_recursive(sort_type *arr, sort_type *temp, int left,
                          int right) {
  int len = right - left + 1;

  // Hybrid base case
  if (len <= INSERTION_SORT_THRESHOLD) {
    insertion_sort(arr, left, right);
    return;
  }

  if (left < right) {
    int mid = left + (right - left) / 2;
    merge_sort_recursive(arr, temp, left, mid);
    merge_sort_recursive(arr, temp, mid + 1, right);
    merge(arr, temp, left, mid, right);
  }
}

// Main Wrapper
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

  if (n <= 20) {
    printf("Before: ");
    print_array(arr, n);
  }

  clock_t start = clock();
  baseline_merge_sort(arr, n);
  clock_t end = clock();

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

#define HOURLY_COST 0.10

void run_gb_test(int gb) {
  printf("\n============================================================\n");
  printf("       RUNNING LARGE SCALE TEST: %d GB\n", gb);
  printf("============================================================\n");

  size_t total_bytes = (size_t)gb * 1024 * 1024 * 1024;
  size_t num_elements = total_bytes / sizeof(sort_type);

  printf("[INFO] Dataset Configuration:\n");
  printf("   - Size:   %.4f GB\n", (double)total_bytes / 1e9);
  printf("   - Count:  %zu elements\n", num_elements);

  printf("[INFO] Allocating Memory...\n");
  sort_type *arr = (sort_type *)malloc(total_bytes);
  if (arr == NULL) {
    fprintf(stderr, "[ERROR] Malloc failed!\n");
    return;
  }

  printf("[INFO] Generating random 32-bit integers...\n");
  srand(time(NULL));
  for (size_t i = 0; i < num_elements; i++) {
    uint32_t r32 = ((rand() & 0xFFFF) << 16) | (rand() & 0xFFFF);
    arr[i] = (sort_type)r32;
  }

  printf("[INFO] Sorting...\n");
  clock_t start = clock();
  baseline_merge_sort(arr, num_elements);
  clock_t end = clock();

  double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
  printf("[INFO] Verifying correctness...\n");

  if (verify_sorted(arr, num_elements)) {
    printf("\n[RESULT] SUCCESS!\n");
    printf("   - Time Taken:   %.4f seconds\n", time_taken);

    double throughput = (double)gb / time_taken;
    printf("   - Throughput:   %.4f GB/s\n", throughput);

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
  sort_type t1[] = {12, 7, 14, 9, 10, 11};
  run_test("Small Random", t1, 6);

  sort_type t2[] = {INT_MAX, 0, INT_MIN, -1, 1, INT_MAX - 1, INT_MIN + 1};
  run_test("32-bit Edge Cases", t2, 7);

  sort_type t3[] = {1, 2, 3, 4, 5, 6, 7, 8};
  run_test("Already Sorted", t3, 8);

  sort_type t4[] = {100, 90, 80, 70, 60, 50, 40};
  run_test("Reverse Sorted", t4, 7);

  sort_type t5[] = {5, 1, 5, 2, 5, 3};
  run_test("Duplicates", t5, 6);

  int large_n = 100000;
  sort_type *t6 = (sort_type *)malloc(large_n * sizeof(sort_type));
  srand(42);
  for (int i = 0; i < large_n; i++) {
    uint32_t r = (rand() << 16) | rand();
    t6[i] = (sort_type)r;
  }
  run_test("Large Random (100k)", t6, large_n);
  free(t6);

  run_gb_test(1);
  run_gb_test(2);
  run_gb_test(4);

  return 0;
}