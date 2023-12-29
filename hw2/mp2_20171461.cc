#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define CUT_OFF 16

void insertion_sort(int arr[], int n)
{
  int i, j, key;
  for (int i = 0; i < n; i++)
  {
    key = arr[i];
    j = i - 1;
    while (j >= 0 && arr[j] > key)
    {
      arr[j + 1] = arr[j];
      j = j - 1;
    }
    arr[j + 1] = key;
  }
}

void swap(int *a, int *b)
{
  int t = *a;
  *a = *b;
  *b = t;
}

int partition(int arr[], int low, int high)
{
  int pivot = arr[high];
  int i = (low - 1);

  for (int j = low; j < high; j++)
  {
    if (arr[j] <= pivot)
    {
      i++;
      swap(&arr[i], &arr[j]);
    }
  }
  swap(&arr[i + 1], &arr[high]);
  return (i + 1); 
}

void quick_sort(int arr[], int low, int high)
{
  if (low < high)
  {
    int pi = partition(arr, low, high);

    quick_sort(arr, low, pi - 1);
    quick_sort(arr, pi + 1, high);
  }
}

void merge(int arr[], int left, int mid, int right)
{
  int i, j, k;
  int n1 = mid - left + 1;
  int n2 = right - mid;
  int left_arr[n1], right_arr[n2];

  for (i = 0; i < n1; i++)
    left_arr[i] = arr[left + i];
  for (j = 0; j < n2; j++)
    right_arr[j] = arr[mid + 1 + j];

  i = 0;
  j = 0;
  k = left;

  while (i < n1 && j < n2)
  {
    if (left_arr[i] <= right_arr[j])
      arr[k] = left_arr[i++];
    else
      arr[k] = right_arr[j++];
    k++;
  }

  while (i < n1)
  {
    arr[k++] = left_arr[i++];
  }
  while (j < n2)
  {
    arr[k++] = right_arr[j++];
  }
}

void merge_sort(int arr[], int left, int right)
{
  if (left < right)
  {
    int mid = left + (right - left) / 2;

    merge_sort(arr, left, mid);
    merge_sort(arr, mid + 1, right);

    merge(arr, left, mid, right);
  }
}

void heapify(int arr[], int n, int i) 
{
  int largest = i;
  int left = 2 * i + 1;
  int right = 2 * i + 2;

  if (left < n && arr[left] > arr[largest])
    largest = left;

  if (right < n && arr[right] > arr[largest])
    largest = right;

  if (largest != i) 
  {
    swap(&arr[i], &arr[largest]);
    heapify(arr, n, largest);
  }
}

void heap_sort(int arr[], int n) 
{
  for (int i = n / 2 - 1; i >= 0; i--)
    heapify(arr, n, i);

  for (int i = n - 1; i >= 0; i--) 
  {
    swap(&arr[0], &arr[i]);
    heapify(arr, i, 0);
  }
}

int median_of_three(int arr[], int low, int high) 
{
  int mid = low + (high - low) / 2;

  if (arr[low] < arr[mid]) 
  {
    if (arr[mid] < arr[high]) {
      return mid;
    } else if (arr[low] < arr[high]) {
      return high;
    } else {
      return low;
    }
  } else {
    if (arr[low] < arr[high]) {
      return low;
    } else if (arr[mid] < arr[high]) {
      return high;
    } else {
      return mid;
    }
  }
}

void intro_sort_util(int arr[], int low, int high, int depth_limit) 
{
  if (high - low > CUT_OFF) 
  {
    if (depth_limit == 0) 
    {
      heap_sort(arr + low, high - low + 1);
      return;
    }

    int pivot_index = median_of_three(arr, low, high);
    swap(&arr[pivot_index], &arr[high]);

    int pi = partition(arr, low, high);
    intro_sort_util(arr, low, pi - 1, depth_limit - 1);
    intro_sort_util(arr, pi + 1, high, depth_limit - 1);
  } 
  else 
  {
    insertion_sort(arr + low, high - low + 1);
  }
}

void intro_sort(int arr[], int n) 
{
  int depth_limit = 2 * log(n);
  intro_sort_util(arr, 0, n - 1, depth_limit);
}

int main(int argc, char *argv[])
{
  char output_filename[50];
  char *input_filename;
  int algo_index, elements_num;
  int *arr;
  clock_t start_time, end_time;
  
  if (argc != 3)
  {
    fprintf(stderr, "usage: ./mp2_20171461 input_filename algorithm_index\n");
    return 1;
  }

  algo_index = atoi(argv[2]);

  if (algo_index < 1 || algo_index > 4)
  {
    fprintf(stderr, "Error: algorithm index must be 1, 2, 3, or 4\n");
    return 1;
  }

  FILE *infile = fopen(argv[1], "r");

  if (NULL == infile)
  {
    fprintf(stderr, "The input file does not exist.\n");
    return 1;
  }
  input_filename = argv[1];

  fscanf(infile, "%d", &elements_num);

  arr = (int *)malloc(elements_num * sizeof(int));

  for (int i = 0; i < elements_num; i++)
  {
    fscanf(infile, "%d", &arr[i]);
  }
  fclose(infile);

  start_time = clock();

  if (algo_index == 1) // insertion sort
  {
    insertion_sort(arr, elements_num);
  }
  else if (algo_index == 2) // quicksort
  {
    quick_sort(arr, 0, elements_num - 1);
  }
  else if (algo_index == 3) // merge sort
  {
    merge_sort(arr, 0, elements_num - 1);
  }
  else if (algo_index == 4) // introsort
  {
    intro_sort(arr, elements_num);
  }
  
  end_time = clock();

  sprintf(output_filename, "result_%d_%s", algo_index, input_filename);

  FILE *outfile = fopen(output_filename, "w");

  fprintf(outfile, "%s\n", input_filename);
  fprintf(outfile, "%d\n", algo_index);
  fprintf(outfile, "%d\n", elements_num);
  fprintf(outfile, "%.6f\n", ((double)(end_time - start_time)) / CLOCKS_PER_SEC);
  for (int i = 0; i < elements_num; i++)
  {
    fprintf(outfile, "%d ", arr[i]);
  }

  fclose(outfile);

  free(arr);
  
  return 0;
}