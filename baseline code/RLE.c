#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// - Run-Length Encoding Compression
// - Adaptive Strategy (Heuristic)
// - Increases Arithmetic Intensity

typedef int32_t sort_type;

// RLE Structure
typedef struct {
  sort_type value;
  int count;
} Run;

// Helper
void print_array(sort_type *arr, int n) {
  printf("[");
  for (int i = 0; i < n; i++) {
    printf("%d%s", arr[i], (i < n - 1) ? ", " : "");
  }
  printf("]\n");
}

// Fallback: Standard Merge
void merge(sort_type *arr, sort_type *temp, int left, int mid, int right) {
  int i = left;
  int j = mid + 1;
  int k = left;

  while (i <= mid && j <= right) {
    if (arr[i] <= arr[j]) {
      temp[k++] = arr[i++];
    } else {
      temp[k++] = arr[j++];
    }
  }

  while (i <= mid)
    temp[k++] = arr[i++];
  while (j <= right)
    temp[k++] = arr[j++];
  for (i = left; i <= right; i++)
    arr[i] = temp[i];
}

void merge_sort_recursive(sort_type *arr, sort_type *temp, int left,
                          int right) {
  if (left < right) {
    int mid = left + (right - left) / 2;
    merge_sort_recursive(arr, temp, left, mid);
    merge_sort_recursive(arr, temp, mid + 1, right);
    merge(arr, temp, left, mid, right);
  }
}

// RLE Sort Logic

// Stable Merge for Runs
void merge_runs(Run *arr, Run *temp, int left, int mid, int right) {
  int i = left;
  int j = mid + 1;
  int k = left;

  while (i <= mid && j <= right) {
    // Stability: <= preserves order
    if (arr[i].value <= arr[j].value) {
      temp[k++] = arr[i++];
    } else {
      temp[k++] = arr[j++];
    }
  }

  while (i <= mid)
    temp[k++] = arr[i++];
  while (j <= right)
    temp[k++] = arr[j++];
  for (i = left; i <= right; i++)
    arr[i] = temp[i];
}

void merge_sort_runs_recursive(Run *arr, Run *temp, int left, int right) {
  if (left < right) {
    int mid = left + (right - left) / 2;
    merge_sort_runs_recursive(arr, temp, left, mid);
    merge_sort_runs_recursive(arr, temp, mid + 1, right);
    merge_runs(arr, temp, left, mid, right);
  }
}

// Compression (O(N))
// Returns count of unique runs
int compress_runs(sort_type *arr, int n, Run *runs_out) {
  if (n == 0)
    return 0;

  int run_idx = 0;
  runs_out[0].value = arr[0];
  runs_out[0].count = 1;

  for (int i = 1; i < n; i++) {
    if (arr[i] == runs_out[run_idx].value) {
      runs_out[run_idx].count++;
    } else {
      run_idx++;
      runs_out[run_idx].value = arr[i];
      runs_out[run_idx].count = 1;
    }
  }
  return run_idx + 1;
}

// Decompression (O(N))
void decompress_runs(Run *runs, int num_runs, sort_type *arr) {
  int arr_idx = 0;
  for (int i = 0; i < num_runs; i++) {
    sort_type val = runs[i].value;
    int count = runs[i].count;

    for (int k = 0; k < count; k++) {
      arr[arr_idx++] = val;
    }
  }
}

// Main Logic: Adaptive Sort
void innovative_merge_sort(sort_type *arr, int n) {
  if (n <= 1)
    return;

  // Alloc worst-case (all unique)
  Run *runs = (Run *)malloc(n * sizeof(Run));
  if (!runs) {
    fprintf(stderr, "Malloc failed\n");
    exit(1);
  }

  // 1. Compress
  int num_runs = compress_runs(arr, n, runs);

  // Heuristic: Use RLE if compression > 20% (num_runs < 0.8 * n)
  if ((double)num_runs < (double)n * 0.8) {
    // RLE Path
    Run *temp_runs = (Run *)malloc(num_runs * sizeof(Run));
    if (!temp_runs) {
      free(runs);
      exit(1);
    }

    // 2. Sort compressed runs
    merge_sort_runs_recursive(runs, temp_runs, 0, num_runs - 1);

    // 3. Decompress back
    decompress_runs(runs, num_runs, arr);

    free(temp_runs);
  } else {
    // Fallback Path: Standard Merge Sort
    sort_type *temp = (sort_type *)malloc(n * sizeof(sort_type));
    if (!temp) {
      free(runs);
      exit(1);
    }

    merge_sort_recursive(arr, temp, 0, n - 1);

    free(temp);
  }

  free(runs);
}

// Verification
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
  innovative_merge_sort(arr, n);
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

  // Generate data with limited range to force duplicates
  printf(
      "[INFO] Generating data (Limited Range 0-1000 for RLE potential)...\n");
  srand(time(NULL));
  for (size_t i = 0; i < num_elements; i++) {
    arr[i] = (sort_type)(rand() % 1000);
  }

  printf("[INFO] Sorting...\n");
  time_t start = time(NULL);
  innovative_merge_sort(arr, num_elements);
  time_t end = time(NULL);

  double time_taken = difftime(end, start);
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

  sort_type t5[] = {5, 1, 5, 2, 5, 3};
  run_test("Duplicates (RLE Candidate)", t5, 6);

  run_gb_test(1);
  run_gb_test(2);
  run_gb_test(4);

  return 0;
}
