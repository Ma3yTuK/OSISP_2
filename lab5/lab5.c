#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <memory.h>
#include <unistd.h>


typedef int BOOL;

#define TRUE 1;
#define FALSE 0;


const int number_of_threads = 1;
int* ARR;


struct call_args {
    int* arr;
    int size;
    int debth;
};


int partition(int* arr, int size) {
    int median_val = arr[0];
    int unsorted_part_begin = 1;
    int unsorted_part_end = size;

    while (unsorted_part_begin != unsorted_part_end)
    {
        if (arr[unsorted_part_begin] > median_val)
        {
            int tmp = arr[unsorted_part_begin];
            arr[unsorted_part_begin] = arr[--unsorted_part_end];
            arr[unsorted_part_end] = tmp;
        }
        else
        {
            unsorted_part_begin++;
        }
    }

    int median_pos = unsorted_part_begin - 1;
    arr[0] = arr[median_pos];
    arr[median_pos] = median_val;

    return median_pos;
}


void* sort(void* arg) {
    struct call_args* args = (struct call_args*) arg;
    
    if (args->size <= 1)
        return NULL;

    int median_pos = partition(args->arr, args->size);
    BOOL start_thread = 2 << (args->debth > 29 ? 29 : args->debth) <= number_of_threads;
    pthread_t new_thread;
    struct call_args new_args = {args->arr, median_pos, args->debth + 1};

    if (start_thread)
    {
        pthread_create(&new_thread, NULL, sort, &new_args);
        if (2 << (args->debth > 29 ? 29 : args->debth + 1) > number_of_threads)
        {
            printf("Thread %u processes indexes %d - %d\n", new_thread, args->arr - ARR, args->arr - ARR + median_pos - 1);
            printf("Thread %u processes indexes %d - %d\n", pthread_self(), args->arr - ARR + median_pos + 1, args->arr - ARR + args->size);
        }
    }
    else
    {
        sort(&new_args);
    }

    sort(&(struct call_args){args->arr + median_pos + 1, args->size - median_pos - 1, args->debth + 1});

    if (start_thread)
    {
        pthread_join(new_thread, NULL);
    }

    return NULL;
}


void gen_array(int* arr, int size)
{
    for (int i = 0; i < size; i++)
    {
        arr[i] = rand();
    }
}


int main(int argc, char** argv) {
    srand(time(NULL));
    const int arr_size = 100000000;
    int* arr = (int*)malloc(arr_size * sizeof(int));
    ARR = arr;

    gen_array(arr, arr_size);

    sort(&(struct call_args){arr, arr_size, 0});
    
    free(arr);

    return 0;
}