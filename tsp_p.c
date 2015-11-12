// Takumi House (houset@bu.edu) & Kristel Tan (ktan@bu.edu)
// CAS CS410 Advanced Software Systems
// Due: November 13, 2015

// tsp_p.c
// Reads in an input graph and returns all possible paths from a given
// starting node and their respective distances

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_ROW 12

/* Returns array of adjacent nodes to given city */
int * adjacent(int start, int size, int m[size][size]) {
    
    int *adjNodes = malloc(sizeof(int) * (size - 1));       // Allocate memory for adjNodes
    int count = 0;                                          // Will keep track of index for adjNodes
    
    // Searches down a column in the input graph for valid distances
    // and stores the node in adjNodes if valid
    for (int i = 0; i < size; i++) {
        if (m[i][start] > 0) {
            adjNodes[count] = i;
            count++;
        }
    }
    return adjNodes;
}

/* Calculates total distance of a given traveled path */
int calcDistance(int path[], int currentIndex, int x, int m[x][x]) {
    int sumDist = m[path[0]][path[1]];
    if (currentIndex > 2) {
        for (int i = 2; i < currentIndex; i++) {
            sumDist = sumDist + m[path[i - 1]][path[i]];
        }
    }
    return sumDist + m[path[currentIndex - 1]][path[0]];
}

/* Calculates all valid paths given a starting node and its adjacent nodes */
void allPaths(int start, int des, int x, int m[x][x], int visited[], int path[], int currentIndex) {
    
    visited[start] = 1;
    path[currentIndex] = start;
    currentIndex++;
    
    if (start == des) {
        for (int i = 0; i < currentIndex; i++) {
            printf("%d ", path[i]);
        }
        printf("%d", path[0]);
        int sumDist = calcDistance(path, currentIndex, x, m);
        printf("\t\t total distance: %d", sumDist);
        printf("\n");
    }
    else {
        int *adjNodes = adjacent(start, x, m);
        int *ptr = adjNodes;
        for (int i = 0; i < (sizeof(ptr) / sizeof(int)); i++) {
            if (visited[adjNodes[i]] != 1) {
                allPaths(adjNodes[i], des, x, m, visited, path, currentIndex);
            }
        }
    }
    
    currentIndex--;
    visited[start] = 0;
}

int main(void) {
    
    int i, j, k;        // Integers declared for loops
    int numCities;      // Will store the total number of cities
    
    // Retrieve the number of desired cities using standard input
    printf("Enter the total number of cities (i.e. nodes): \n");
    if ( (scanf("%d", &numCities) == 1) && (numCities <= 10) && (numCities > 1)) {
        printf("OK, numCities = %d \n", numCities);
    }
    else {
        // Exit if numCities is not a valid int or between and including 2 and 10
        printf("tsp_p: Number of cities must be a valid integer and within the bounds of 2 and 10\n\n");
        exit(1);
    }
    
    int m[numCities][numCities];      // Max size of input graph is 10
    
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
            exit(1);
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
                exit(1);
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
    
    printf("\n");
    printf("All possible paths for this input graph: \n");
    
    int status = 0;
    for (int city = 0; city < numCities; city++) {
        pid_t pid = fork();
        if (pid == -1) {
            fprintf(stderr, "tsp_p: fork failed\n");
            exit(1);
        }
        else if (pid == 0) {
            for (int des = 0; des < numCities; des++) {
                int path[MAX_ROW];
                int visited[numCities];
                if (des != city) {
                    allPaths(city, des, numCities, m, visited, path, 0);
                }
            }
            exit(0);
        }
        else {
            wait(&status);
        }
    }

    return 0;
}

