// Takumi House (houset@bu.edu) & Kristel Tan (ktan@bu.edu)
// CAS CS410 Advanced Software Systems
// Due: November 13, 2015

// tspsort.c
// Reads in an array of paths found by TSP programs from a txt file, and sorts them in ascending order

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void quicksort(int, int [10],int,int);      // Forward declaration of quicksort algorithm

int main(int argc, const char * argv[]) {
    
    // Arbitrary number of lines for allocation
    // Memory reallocation is implemented, however, program cannot sort list of paths
    // of size greater than lines_allocated.

    int lines_allocated = 150;
    int max_line_len = 100;
    int allocfactor = 1;
    
    // Allocate lines of text and total distances taken from end of path strings
    char **paths = (char **)malloc(sizeof(char*) * lines_allocated);
    int *totaldists = (int *)malloc(sizeof(int) * lines_allocated);
    
    // Error checking for memory allocation
    if (paths == NULL) {
        
        fprintf(stderr,"Out of memory (1).\n");
        exit(1);
    }
    
    if (totaldists == NULL) {
        fprintf(stderr,"Out of memory (1).\n");
        exit(1);
    }
    
    // Opening file "paths.txt" containing paths for reading, which 'tsp_p' will write to
    // For now, full directory path is used
    
    FILE *fp = fopen("paths.txt", "r");
    
    // Error checking for opening file
    if (fp == NULL) {

        perror("Error opening file.\n");
        exit(2);
    }
    
    int i;
    for (i = 0; 1; i++) {
        int j;
        
        // Checking if 'i' has exceeded maximum lines allocated for the paths
        // Checking if reallocation of memory is required

        if (i >= lines_allocated) {
            int new_size;
            
            // Doubling allocation and re-allocating memory.
            new_size = lines_allocated * 2;
            paths = (char **)realloc(paths, sizeof(char*)*new_size);
            
            // Allocfactor keeps track of any reallocation so size of 'totaldist' array is accurate
            allocfactor = allocfactor + 1;
            
            // Error checking for memory allocation
            if (paths == NULL) {
                fprintf(stderr,"Out of memory.\n");

                exit(3);
            }
            lines_allocated = new_size;
        }
        
        paths[i] = malloc(max_line_len);                     // Allocating space for the next line
        
        if (paths[i] == NULL) {                              // Error checking for memory allocation

            fprintf(stderr,"Out of memory (3).\n");
            exit(4);
        }
        
        // Reading in a line from the file, making sure it is not NULL
        if (fgets(paths[i], max_line_len-1, fp) == NULL) {
            break;
        }
        
        // When reading in a line, the new line or carriage return character at
        // the end of the line is also read in, which means there will be a double line space.
        // This removes the new line or carriage return character from the end of the char string.
        
        for (j = strlen(paths[i]) - 1.0; j>=0 && (paths[i][j]=='\n' || paths[i][j]=='\r');j--) {
            ;
        }
        paths[i][j + 1]='\0';
        
        char *p = strrchr(paths[i], ' ');        // This isolates the final value in the string, the total dist.
        
        if (p && *(p + 1)) {                     // Once isolated, the total dist is stored in the totaldist array.

            totaldists[i] = atoi(p + 1);
        }
    }
    
    fclose(fp);                                  // Finished reading from file, closign the file.
    
    int j;
    printf("Examining paths:\n");
    
    for(j = 0; j < i; j++) {                     // Printing paths read in from the file.

        printf("%s\n", paths[j]);
    }
    printf("\n");
    
    // Executing quicksort algorithm on array of total distances
    quicksort(lines_allocated * allocfactor, totaldists, 0, lines_allocated * allocfactor - 1);
    
    printf("The total distances from shortest to longest: \n");      // Printing the sorted array of total distances
    for(i=0;i<lines_allocated;i++)
        if (totaldists[i] != 0) {
            printf(" %d\n", totaldists[i]);
        }
    printf("\n");
    
    
    // Freeing memory
    for (; i>=0; i--) {
        
        free(paths[i]);
    }
    free(paths);
    
    free(totaldists);
    return 0;
}

/* Quick sort algorithm adapted from http://www.cquestions.com/2008/01/c-program-for-quick-sort.html */

void quicksort(int size, int x[size], int first,int last) {
    int pivot,j,temp,i;

    if(first<last){
        pivot=first;
        i=first;
        j=last;

        while(i<j){
            while(x[i]<=x[pivot]&&i<last)
                i++;
            while(x[j]>x[pivot])
                j--;
            if(i<j){
                temp=x[i];
                x[i]=x[j];
                x[j]=temp;
            }
        }
        
        temp=x[pivot];
        x[pivot]=x[j];
        x[j]=temp;
        quicksort(size, x, first, j-1);
        quicksort(size, x, j+1, last);
    }
}


