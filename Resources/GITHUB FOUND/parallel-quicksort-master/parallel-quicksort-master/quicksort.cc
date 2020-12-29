#include <stdio.h>
#include <omp.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>



#define NOW()(omp_get_wtime())
typedef double mytime_t;

//printf("END %lu %s ",end, comments);   	
int i;
#define PRINT(x, end, comments) 	\
		for (i = 0; i<end; i++) 	\
		{							\
			printf("%ld\t", x[i]); 	\
		}							\
		printf("\n");

	
void prefixsum_parallel(long *x, int N) {
    
	float *suma;
    #pragma omp parallel
    {
        const int ithread = omp_get_thread_num();
        const int nthreads = omp_get_num_threads();
        #pragma omp single
        {
            suma = new float[nthreads+1];
            suma[0] = 0;
        }
        float sum = 0;
        #pragma omp for schedule(static)
        for (int i=0; i<N; i++) {
            sum += x[i];
            x[i] = sum;
        }
        suma[ithread+1] = sum;
        #pragma omp barrier
        float offset = 0;
        for(int i=0; i<(ithread+1); i++) {
            offset += suma[i];
        }
        #pragma omp for schedule(static)
        for (int i=0; i<N; i++) {
            x[i] += offset;
        }
    }
    delete[] suma;
}

bool lesser_than_comparator (long a, long b)
{
		return a <= b;
}

bool greater_than_comparator (long a, long b)
{
		return a > b;
}

int partition (long *x, long pivot, long *partition_part, bool (*comparator)(long, long), const char *compare, size_t n)
{
	int ii;
	int total_elems;
	long b[n] = {0};
	long s[n] = {0};
	//long *b = new long[n];
	//long *s = new long[n];
	//printf("HOIIIII-1\n");
#pragma omp parallel
	{
#pragma omp for
			for (ii=0; ii<n-1; ii++)
			{
					if (comparator(x[ii],pivot))
					{
							b[ii] = 1;		
					}
			}

#pragma omp for
			for (ii=0; ii<n-1; ii++)
			{
					s[ii] = b[ii];
			}
	
#pragma omp barrier
#pragma omp single nowait
			{
			prefixsum_parallel (s, n-1);

			//PRINT(s, n, compare);

			total_elems = s[n-2];
			}
			//printf("TOTAL_ELEMS %d\n", total_elems);
#pragma omp for
			for (ii=0; ii<n-1; ii++)
			{
					if (b[ii] == 1)
					{
							partition_part[s[ii] - 1] = x[ii];
					}
			}
	}

	//delete[] b;
	//delete[] s;
	//printf("HOIIIII-3\n");
	return total_elems;	
}

void copy_array (long *dest_array, long *source_array, int start_index, int end_index)
{
	int j;
	//printf("END INDEX %d\n", end_index);
#pragma omp parallel
	{
#pragma omp for
			for (j=start_index; j<end_index; j++)
			{
					//printf("SOURCE INDEX %d %ld\n", j, source_array[j-start_index]); 
					dest_array[j] = source_array[j-start_index];
			}
	}	
}

long* quicksort(long *x, size_t n)
{
    //printf("SIZE N %ld\n", n);
	//printf("INCOMING ORIGINAL ARRAY \n");
	int ll;
#if 0
	for (ll=0;ll<n;ll++)
			printf("%ld\t", x[ll]);
#endif

	if (n == 0)
	{
		//printf("%ld\t", x[0]);
        return NULL;
	}
	else if (n==1)
	{
			//printf("%ld\n", x[0]);
			long *s = (long *)malloc(sizeof(long));
			//long *s = new long[1];
			
			s[0] = x[0];
			return s;
	}

    //get the pivot
	long pivot = x[n-1];
	int i;

	long left_partition[n] = {0};
    long right_partition[n] = {0};
	/*
	 * Have a separate single array for 
	 */
	
	int left_elems, right_elems;

#pragma omp parallel sections
			{
				#pragma omp section
				{//printf("1111111111111111111\n");	
					left_elems 	= partition (x, pivot, left_partition, lesser_than_comparator, "LESSER_THAN: ", n);
				}	
					#pragma omp section
					{//printf("2211212211211222111\n");	
					right_elems = partition (x, pivot, right_partition, greater_than_comparator, "GREATER_THAN: ", n);
					}
			}
	//printf("HOIIIII\n");
#if 0
	int j;
	//printf("LEFT PARTITION\n");
	for (j=0;j<left_elems;j++)
		printf("%ld\n", left_partition[j]);	
#endif

#if 1
	long *left_sort = (long *)malloc (sizeof(long) * left_elems);
	long *right_sort = (long *)malloc (sizeof(long) * right_elems);
#endif
#if 0	
	long *left_sort = new long[left_elems];
	long *right_sort = new long[right_elems];
#endif
#pragma omp parallel sections
	{
#pragma omp section
	left_sort = quicksort (left_partition, left_elems);
#pragma omp section
	right_sort = quicksort (right_partition, right_elems);
	}	
#if 0	
	printf("LEFT ELEMS %d RIGHT ELEMS %d\n", left_elems, right_elems);
	printf("LEFT SORT\n");
#endif
	int k;

#if 0	
	for (k=0;k<left_elems;k++)
	{
		printf("%ld\t", left_sort[k]);
	}
	printf("\nRIGHT SORT\n");
	for (k=0;k<right_elems;k++)
	{
		printf("%ld\t", right_sort[k]);
	}
	printf("\n");
#endif

	//long *output_array = new long[right_elems+left_elems+1];
	long *output_array = (long *)malloc(sizeof(long)*(right_elems+left_elems+1));
	int size = right_elems + left_elems + 1;
#if 0	
	left_sorted_array = quicksort (left_partition, left_elems);
	right_sorted_array = quicksort (right_partition, right_elems);
#endif
		
#if 1
#pragma omp parallel sections
	{
#pragma omp section
			{
					if (left_sort)
					{
							copy_array (output_array, left_sort, 0, left_elems);
					}
			}
#pragma omp section
			output_array[left_elems] = pivot;
#pragma omp section
			{
					if (right_sort)
					{
							copy_array (output_array, right_sort, left_elems + 1, left_elems + 1 + right_elems);
					}
			}
	}
	//delete[] left_sort;
	//delete[] right_sort;

#if 0	
	for (k=0; k<size; k++)
			printf("OUT %d : %ld\n", k, output_array[k]);
	printf("\n");
#endif
	return output_array;

#endif
}


int main(int argc, char *argv[])
{
	//long x[] = {4,3,1,2,7,8,0,6,9,5};
	//long x[] = {23,21,5,6,7,77,89,1,11,10,8,4,3,15};
	//long x[] = {23,21,5,6,7,77,89,1,11,10,8,4,3,6,5,21,15};
	//long *y = (long *)malloc(sizeof(long) * 10);
	//quicksort (x, 10);

		
		//long len = 100000;
		//long len = 100000;
		//long len = 10;
		//long len = 100;

		int j =0;
		int i=0;
		FILE *fp;
		char *line = NULL;
		size_t len_word = 0;
		ssize_t read;
		size_t num_of_elements;

		num_of_elements = atol(argv[1]);
		long x[num_of_elements];


		fp = fopen(argv[2], "r");
		if (fp == NULL)
			exit(0);


		while(((read = getline(&line, &len_word, fp)) != -1) && (i<num_of_elements))
		{
			x[i++] = atol(line);
		}	
		
		
		fclose(fp);
		if (line)
			free(line);

#if 0
		int min = 5;
		srand (time(0));
		for (i=0; i<len; i++)
		{
			x[i] = min + (rand()%len);
			printf("%ld\n", x[i]);
		}
#endif	
		long *y = new long[num_of_elements];
		mytime_t start = NOW();
		y=quicksort (x, num_of_elements);
		mytime_t end = NOW();

		printf("TIME ELAPSED %lf\n", end - start);
		PRINT(y, num_of_elements, "ORIGINAL_ARRAY: \n");
}
