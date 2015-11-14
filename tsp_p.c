// Takumi House (houset@bu.edu) & Kristel Tan (ktan@bu.edu)
// CAS CS410 Advanced Software Systems
// Due: November 13, 2015

// tsp_p.c
// Reads in an input graph and returns all possible paths from a given
// starting node and their respective distances

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>

#define MAX_ROW 12
#define SHM_SIZE 1024

/* Returns array of adjacent nodes to given city */
int * adjacent(int start, int size, int m[size][size]) {
    
    int *adjNodes = malloc(sizeof(int) * (size - 1));       // Allocate memory for adjNodes
    int count = 0;                                          // Will keep track of index for adjNodes
    int i = 0;
    
    // Searches down a column in the input graph for valid distances
    // and stores the node in adjNodes if valid
    
    for (i = 0; i < size; i++) {
        if ((m[start][i] != -1) && (start != i)) {
            adjNodes[count] =  i;
            count++;
        }
    }
    /*
     int *ptr = adjNodes;
     printf("\nadjNodes = { ");
     for (int j = 0; j < count; j++) {
     printf("%d,", ptr[j]);
     }
     printf("}\n");*/
    return adjNodes;
}

/* Calculates total distance of a given traveled path */
int calcDistance(int path[], int currentIndex, int size, int m[size][size]) {
    int sumDist = m[path[0]][path[1]];
    int i;
    if (currentIndex > 2) {
        for (i = 2; i < currentIndex; i++) {
            sumDist = sumDist + m[path[i - 1]][path[i]];
        }
    }
    return sumDist + m[path[currentIndex - 1]][path[0]];
}

/* Calculates all valid paths given a starting node and its adjacent nodes */
void allPaths(int start, int des, int size, int m[size][size], int visited[], int path[], int currentIndex, FILE* f) {
    
    visited[start] = 1;
    path[currentIndex] = start;
    currentIndex++;
    int i;
    

    
    if (f == NULL)
    {
        printf("Error opening file!\n");
        exit(1);
    }
    
    if (start == des) {
        for (i = 0; i < currentIndex; i++) {
            fprintf(f, "%d ", path[i]);
            fprintf(stdout, "%d ", path[i]);
        }
        fprintf(stdout, "%d", path[0]);
        fprintf(f, "%d", path[0]);
        
        int sumDist = calcDistance(path, currentIndex, size, m);
        
        fprintf(stdout, "\t --> total distance: %d", sumDist);
        fprintf(f, "\t --> total distance: %d", sumDist);
        fprintf(stdout, "\n");
        fprintf(f, "\n");
    }
    else {
        int adjLength = 0;
        int *adjNodes = adjacent(start, size, m);
        
        for (i = 0; i < size; i++) {
            if ((m[start][i] != -1) && (start != i)) {
                adjLength++;
            }
        }
        
        for (i = 0; i < adjLength; i++) {
            if (visited[adjNodes[i]] != 1) {
                allPaths(adjNodes[i], des, size, m, visited, path, currentIndex, f);
                continue;
            }
        }
    }
    
    currentIndex--;
    visited[start] = 0;
}

int main(void) {
    
    int i, j, k;        // Integers declared for loops
    int numCities;      // Will store the total number of cities
    
    //     file.text will be appendable. If it does not exist, then it will be created
    FILE *f = fopen("paths.txt", "w");
    
    // Retrieve the number of desired cities using standard input
    fprintf(stdout, "Enter the total number of cities (i.e. nodes): \n");
    if ((fscanf(stdin, "%d", &numCities) == 1) && (numCities <= 10) && (numCities > 1)) {
        fprintf(stdout, "OK, numCities = %d \n", numCities);
    }
    else {
        // Exit if numCities is not a valid int or between and including 2 and 10
        fprintf(stderr, "tsp_p: Number of cities must be a valid integer and within the bounds of 2 and 10\n\n");
        exit(1);
    }
    
    int m[numCities][numCities];      // Max size of input graph is 10
    
    // Retrieve the input graph, which will contain distances between each node
    fprintf(stdout, "\nNow let's construct the input graph... \n");
    for (i = 0; i < numCities; i++) {
        fprintf(stdout, "Enter the elements of row # %d: \n", i + 1);
        for (j = 0; j < numCities; j++) {
            fscanf(stdin,"%d", &m[i][j]);
        }
    }
    
    // Check that the diagonal of the input graph has a distance of 0
    k = 0;
    while (k < numCities) {
        if (m[k][k] != 0) {
            fprintf(stderr, "\ntsp_p: The diagonal of the input graph must have distance of 0 \n\n");
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
                fprintf(stderr, "\ntsp_p: The maximum distance between two cities is 255... ");
                fprintf(stderr, "invalid distance found at m[%d][%d] (= %d). \n\n", i, j, m[i][j]);
                exit(1);
            }
        }
    }
    
    // Confirm the input graph is valid by printing it
    fprintf(stdout, "\nOK, the input graph is: \n\n");
    for (i = 0; i < numCities; i++) {
        for (j = 0; j < numCities; j++) {
            fprintf(stdout, "\t%d", m[i][j]);
        }
        fprintf(stdout, "\n");
    }
    
    fprintf(stdout, "\n");
    fprintf(stdout, "All possible paths for this input graph: \n");
    
    //int status = 0;
    int city, des;
    //int shmid;
    //char *shm;
    
    // For each city, fork a new child process and calculate all the valid paths to the other cities
    
    for (city = 0; city < numCities; city++) {
        for (des = 0; des < numCities; des++) {
            int path[MAX_ROW];
            int visited[numCities];
            if (des != city) {
                allPaths(city, des, numCities, m, visited, path, 0, f);
            }
        }
    }
    
    for (city = 0; city < numCities; city++) {
        
        int shmid;
        int *result;
        int status = 0;
        
        shmid = shmget(IPC_PRIVATE, SHM_SIZE, IPC_CREAT | S_IRUSR | S_IWUSR);
        result = (int*)shmat(shmid, NULL,0);
        
        int pid = fork();
        if (pid == -1) {
            fprintf(stderr, "tsp_p: fork failed\n");
            shmdt(result);
            exit(1);
        }
        else if (pid == 0) {
            for (des = 0; des < numCities; des++) {
                int path[MAX_ROW];
                int visited[numCities];
                if (des != city) {
                    allPaths(city, des, numCities, m, visited, path, 0, f);
                }
            }
            shmdt(result);
            exit(0);
        }
        else {
            wait(&status);
        }
    }
    
    return 0;
}

