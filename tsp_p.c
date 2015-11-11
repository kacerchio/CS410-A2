// Takumi House (houset@bu.edu) & Kristel Tan (ktan@bu.edu)
// CAS CS410 Advanced Software Systems
// Due: November 13, 2015

// tsp_p.c
// Multi-process version of the Travelling Salesman Problem
// Reads in an input graph and returns all possible paths from a given
// starting node and their respective distances

#include <stdio.h>

#define MAX_LINE 15

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
    
    int m[2][2] = {0, 1, 1, 0};
    int visited[] = {0, 0};
    int path[MAX_LINE];
    
    allPaths(1, 1, 2, m, visited, path, 0);
    
}
