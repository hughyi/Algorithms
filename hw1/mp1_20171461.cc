#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// find the max number between two numbers
int max(int a, int b)
{
  return (a > b) ? a : b; 
}

// implementation of naive algorithm using brute force search
int mss_n6(int **arr, int n, int m)
{
  int sum, max_sum = arr[0][0];
  for (int i = 0; i < n; i++)
  {
    for (int j = 0; j < m; j++)
    {
      for (int k = i; k < n; k++) // k, l indicates the end of row and col
      {
        for (int l = j; l < m; l++)
        {
          sum = 0;
          for (int x = i; x <= k; x++)
          {
            for (int y = j; y <= l; y++)
            {
              sum += arr[x][y];
            }
          }
          max_sum = max(max_sum, sum);
        }
      }
    }
  }
  return max_sum;
}

// implementation of O(n^4) MSS algorithm
int mss_n4(int** arr, int n, int m) 
{
  int sum, max_sum = arr[0][0];
  int temp_sum[n + 1][m + 1];

  // calculate the cumulative sum of matrix
  for(int i = 0; i <= n; i++) 
  {
    for(int j = 0; j <= m; j++) 
    {
      if(i == 0 || j == 0) 
        temp_sum[i][j] = 0; // set to 0 if either row or column is 0
      else 
        temp_sum[i][j] = temp_sum[i - 1][j] + temp_sum[i][j - 1] - temp_sum[i - 1][j - 1] + arr[i - 1][j - 1];
    }
  }

  // find the maximum sum subrectangle
  for(int top_row = 0; top_row < n; top_row++) 
  {
    for(int bottom_row = top_row; bottom_row < n; bottom_row++) 
    {
      for(int left_col = 0; left_col < m; left_col++) 
      {
        for(int right_col = left_col; right_col < m; right_col++) 
        {
          sum = temp_sum[bottom_row + 1][right_col + 1] - temp_sum[bottom_row + 1][left_col] - temp_sum[top_row][right_col + 1] + temp_sum[top_row][left_col];
          
          max_sum = max(max_sum, sum);
        }
      }
    }
  }

  return max_sum;
}

// implementation of Kadane's algorithm to get maximum sum subarray
int kadane(int *arr, int n)
{
  int max_current = arr[0];
  int max_global = arr[0];

  for (int i = 1; i < n; i++)
  {
    max_current = max(arr[i], max_current + arr[i]);
    max_global = max(max_global, max_current);
  }
  
  return max_global;
}

// optimal approach using Kadane's Algorithm
int mss_n3(int **arr, int n, int m)
{
  int max_sum = arr[0][0];

  // dynamically allocate array to store sum of elements in each row
  for (int i = 0; i < m; i++) // fix left column
  {
    int *store = (int *)malloc(n * sizeof(int));
    memset(store, 0, n * sizeof(int)); // initialize row store array to 0

    for (int j = i; j < m; j++) // fix right column
    {
      for (int k = 0; k < n; k++) // access row
      {
        store[k] += arr[k][j];
      }
      max_sum = max(max_sum, kadane(store, n));
    }
    
    free(store);
  }
  return max_sum;
}

int main(int argc, char *argv[])
{
  char output_filename[50];
  char *input_filename;
  int algo_index, row_num, col_num, rv;
  int **arr;
  clock_t start_time, end_time;
  
  // error handling of num of arguments
  if (argc != 3)
  {
    fprintf(stderr, "usage: ./mp1_20171461 input_filename algorithm_index\n");
    return 1;
  }

  algo_index = atoi(argv[2]);
  // error handling of algorithm index
  if (algo_index < 1 || algo_index > 3)
  {
    fprintf(stderr, "Error: algorithm index must be 1, 2, or 3\n");
    return 1;
  }

  FILE *infile = fopen(argv[1], "r");
  // error handling of input file open 
  if (NULL == infile)
  {
    fprintf(stderr, "The input file does not exist.\n");
    return 1;
  }
  else
  {
    // read input file for output file name
    input_filename = argv[1];
    // attempt to read row and column numbers from the input file
    if (fscanf(infile, "%d %d", &row_num, &col_num) != 2)
    {
      printf("Error reading integers from file\n");
      fclose(infile);
      return 1;
    }

    // dynamically allocate memory for the array
    arr = (int **)malloc(row_num * sizeof(int *));
    for (int i = 0; i < row_num; i++)
    {
      arr[i] = (int *)malloc(col_num * sizeof(int));
    }

    // read the elements of the array from input file
    for (int i = 0; i < row_num; i++)
    {
      for (int j = 0; j < col_num; j++)
      {
        fscanf(infile, "%d", &arr[i][j]);
      }
    }

    // measure running time of the algorithms
    start_time = clock();

    if (algo_index == 1) // n^6
    {
      rv = mss_n6(arr, row_num, col_num);
    }
    else if (algo_index == 2) // n^4
    {
      rv = mss_n4(arr, row_num, col_num);
    }
    else if (algo_index == 3) // n^3
    {
      rv = mss_n3(arr, row_num, col_num);
    }

    end_time = clock();

    // create the output file name
    sprintf(output_filename, "result_%s", input_filename);

    FILE *outfile = fopen(output_filename, "w");

    // write in output file
    fprintf(outfile, "%s\n", input_filename);
    fprintf(outfile, "%d\n", algo_index);
    fprintf(outfile, "%d\n", row_num);
    fprintf(outfile, "%d\n", col_num);
    fprintf(outfile, "%d\n", rv);
    fprintf(outfile, "%ld", end_time - start_time);

    fclose(outfile);
  }

  // free dynamically allocated memory
  for (int i = 0; i < row_num; i++) 
  {
      free(arr[i]);
  }
  free(arr);
  
  fclose(infile);
  return 0;
}