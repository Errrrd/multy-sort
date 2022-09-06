#ifdef _OPENMP

#include <stdio.h>
#include <iostream>
#include <omp.h>

#define TH 8
#define SZ 399999

using namespace std;

template<typename T>
int partition(T array[], int start, int end) {
    int pivot = (start + end) / 2;
    int tmp;
    int hi = end - 1;
    int lo = start;
    
    tmp = array[end];
    array[end] = array[pivot];
    array[pivot] = tmp;
    for ( ; lo <= hi; ) {
        if ( array[end] < array[lo] ) {
            if ( array[end] >= array[hi] ) {
                tmp = array[hi];
                array[hi] = array[lo];
                array[lo] = tmp;
                lo += 1;
                hi -= 1;
            } else {
                hi -= 1;
            }
        } else {
            lo += 1;
        }
    }
    tmp = array[lo];
    array[lo] = array[end];
    array[end] = tmp;
    return lo;
}
template<typename T>
bool check(T array[], int start, int end) {
    for(int i = start+1; i < end; i++) {
        if (array[i-1] > array[i]) { return false; }
    }
    return true;
}
template<typename T>
void quickSort(T array[], int lo, int hi) {
    int pivot = partition(array, lo, hi);
    if( lo < pivot - 1 ) {
        quickSort(array, lo, pivot-1);
    }
    if( pivot + 1 < hi ) {
        quickSort(array, pivot+1, hi);
    }
}

template<typename T>
void arrayPrint(FILE *out, T array[], int size) {
    int last = size - 1;
    
    for ( int i = 0; i < last; i++ ) {
        fprintf(out, "%d ", array[i]);
    }
    fprintf(out, "%d\n", array[last]);
}
//*
template<typename T>
void merge(T array[], int lo, int mid, int hi) {
    int size = hi - lo;
    T *buffer = new T[size];
    
    for ( int i = 0, j = lo, k = mid; i < size; ) {
        if ( array[j] < array[k] && j < mid ) {
            buffer[i] = array[j];
            i += 1;
            j += 1;
        } else if ( k < hi ) {
            buffer[i] = array[k];
            i += 1;
            k += 1;
        }
        if ( k == hi && j < mid ) {
            buffer[i] = array[j];
            i += 1;
            j += 1;
        }
    }
    
    for ( int i = 0, j = lo; i < size; i++, j++ ) {
        array[j] = buffer[i];
    }
    
    delete [] buffer;
}
//*/

template<typename T>
int arrayScan(FILE *in, T array[], int len) {
    int size = 0;
    
    for ( ; size < len && fscanf(in, "%d", &array[size]) == 1; size++ );
    return size;
}

int main() {
    FILE *in = fopen("task.in", "r");
    FILE *out = fopen("task.out", "w");
    int *array = new int[SZ];
    int size = arrayScan(in, array, SZ);
    int delta = size / TH;
    cout << "task.in opened" << endl;
    fclose(in);
    omp_set_dynamic(0);      // запретить библиотеке openmp менять число потоков во время исполнения
    omp_set_num_threads(TH); // установить число потоков: TH
    
    cout << "size = " << size << endl;
    if (TH > 1) {
        #pragma omp parallel
        {
            #pragma omp for
            for(int i = 0; i < TH; i++ ) {
                int start = delta * i;
                int end = start + delta;
                
                if ( i == TH - 1 ) {
                    end = size;
                }
                //cout <<"\ncreating thread, " << i << endl;
                printf("creating thread - %d\n, start:%d, end:%d\n", i, start, end);
                
                quickSort(array, start, end-1);
                printf("thread %d check slice: %s\n", i, check(array, start, end)?"ok":"fail");
            }
        }
        
        for( int step = delta; step < size; ) {
            int start = 0;
            int mid = start + step;
            int end;
            
            step *= 2;
            #pragma omp for
            for(end = start + step; end <= size; end += step) {
                //if(size-end < step) { end = size;}
                merge(array, start, mid, end);
                printf("check merge %d - %d - %d: %s\n", start, mid, end, check(array, start, end)?"ok":"fail");
                //printf("merge %d - %d - %d\n", start, mid, end);
                start += step;
                mid += step;
            }
            
            if ( mid < size ) {
                {
                    printf("merge %d - %d - %d\n", start, mid, size);
                    merge(array, start, mid, size);
                }
            }
            
        }
    } else {
        quickSort(array, 0, SZ-1);
    }
    printf("array check all: %s\n", check(array, 0, SZ)?"ok":"fail");
    arrayPrint(out, array, size);

    delete [] array;
    fclose(out);
    
    return 0;
}

#endif
