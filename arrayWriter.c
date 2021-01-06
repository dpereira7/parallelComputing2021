#include<stdio.h>
#include<omp.h>
#include<math.h>
#include <stdlib.h>

#define ARRAY_MAX_SIZE 500000


int main() 
{ 

	int arr1[ARRAY_MAX_SIZE];
    int arr2[ARRAY_MAX_SIZE];
    int arr3[ARRAY_MAX_SIZE];

    for( int i = 0; i < 30000; i++ )
    {
       arr1[i] = rand() % 50 +1;
      // printf("%d\n", arr[i]);

    }

      for( int i = 0; i < 100000; i++ )
    {
       arr2[i] = rand() % 50 +1;
      // printf("%d\n", arr[i]);

    }

      for( int i = 0; i <= 500000; i++ )
    {
       arr3[i] = rand() % 50 +1;
      // printf("%d\n", arr[i]);

    }

    FILE *file = fopen("array1.txt", "w");
    FILE *fp = fopen("array2.txt", "w");
    FILE *fy = fopen("array3.txt", "w");

    for( int i = 0; arr1[i]; i++ )
    {
       fprintf(file, "%d\n",arr1[i]);

    }
    
    fclose(file);

    for( int i = 0; arr2[i]; i++ )
    {
       fprintf(fp, "%d\n",arr2[i]);

    }
    fclose(fp);

    for( int i = 0; arr3[i]; i++ )
    {
       fprintf(file, "%d\n",arr3[i]);

    }
    fclose(fy);

	return 0; 
} 