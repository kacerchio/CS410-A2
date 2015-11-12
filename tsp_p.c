// Takumi House (houset@bu.edu) & Kristel Tan (ktan@bu.edu)
// CAS CS410 Advanced Software Systems
// Due: November 13, 2015

// tsp_p.c
// Reads in an input graph and returns all possible paths from a given
// starting node and their respective distances

#include <stdio.h>
#include <stdlib.h>

#define MAX_ROW 12

// Calculates all paths given a starting node
void allPaths(int start, int des, int x, int m[x][x], int visited[], int path[], int currentIndex) {
 
    visited[start] = 1;
    path[currentIndex] = start;
    currentIndex++;
    printf("start = %d \n", start);
    
    if ((start == des) && currentIndex != 1) {
        printf("reached base case\n");
        for (int i = 0; i < currentIndex; i++) {
            printf("%d ", path[i]);
        }
    }
    else {
        for (int adj = 0; adj < currentIndex; adj++) {
            if ((m[start][adj] > 0) && (!visited[adj])) {
                printf("reached recursive call\n");
                allPaths(adj, des, x, m, visited, path, currentIndex);
            }
        }
    }
    
    currentIndex--;
    visited[start] = 0;
}

int main(void) {
    
    int i, j, k;        // Integers declared for loops
    int numCities;      // Will store the total number of cities
    int m[10][10];      // Max size of input graph is 10
    
    // Retrieve the number of desired cities using standard input
    printf("Enter the total number of cities (i.e. nodes): \n");
    if ( (scanf("%d", &numCities) == 1) && (numCities <= 10) && (numCities > 1)) {
        printf("OK, numCities = %d \n", numCities);
    }
    else {
        // Exit if numCities is not a valid int or between and including 2 and 10
        printf("tsp_p: Number of cities must be a valid integer and within the bounds of 2 and 10\n\n");
        exit(0);
    }
    
    // Retrieve the input graph, which will contain distances between each node
    printf("\nNow let's construct the input graph... \n");
    for (i = 0; i < numCities; i++) {
        printf("Enter the elements of row # %d: \n", i + 1);
        for (j = 0; j < numCities; j++) {
            scanf("%d", &m[i][j]);
        }
    }
    
    // Check that the diagonal of the input graph has a distance of 0
    k = 0;
    while (k < numCities) {
        if (m[k][k] != 0) {
            printf("\ntsp_p: The diagonal of the input graph must have distance of 0 \n\n");
            exit(0);
        }
        else {
            k++;
        }
    }
    
    // Check that the distances in the input graph are less than the max distance of 255
    for (i = 0; i < numCities; i++) {
        for (j = 0; j < numCities; j++) {
            if (m[i][j] > 255) {
                printf("\ntsp_p: The maximum distance between two cities is 255... ");
                printf("invalid distance found at m[%d][%d] (= %d). \n\n", i, j, m[i][j]);
                exit(0);
            }
        }
    }
    
    // Confirm the input graph is valid by printing it
    printf("\nOK, the input graph is: \n\n");
    for (i = 0; i < numCities; i++) {
        for (j = 0; j < numCities; j++) {
            printf("\t%d", m[i][j]);
        }
        printf("\n");
    }
    
    
}
