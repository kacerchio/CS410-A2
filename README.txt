Takumi House (houst@bu.edu) & Kristel Tan (ktan@bu.edu)
CAS CS410 Advanced Software Systems
Assignment 2
Due: November 13, 2015

README.txt

All group members contributed equally to this project. We were unable, however, to complete a solution for tsp_t.c. 

Run 'Makefile':
	$ make

Executable Files: 

	- myshell: Implements a basic shell program. Includes a simple command line parser to interpret commands and executes them according to the file operators in the spec. 

		*** Limitations ***
		- Does not support the semicolon ';' operator
		- 'ctrl + d' termination is handled as a failed execution

	- tsp_p: A multi-process implementation of of the Travelling Salesman Problem. Takes an input graph and returns valid paths from each city as a starting node to all other destinations. 

		*** Limitations ***
		- The program does not seem to return all paths for each node to every other node, but returns about 50% of them. There may be a bug in the allPaths(...) algorithm.

	- tsp_t: Did not complete

	- tspsort: Sorts the distances in a given file and sorts them in order from shortest to longest distance. 

		*** Limitations ***
		- Returns only the sorted distances and leaves out their respective paths 

Source Files: 

	- myshell.c: Compiled into executable 'myshell'
	- tsp_p.c: Compiled into executable 'tsp_p'
	- tspsort.c: Compiled into executable 'tsp_sort'
