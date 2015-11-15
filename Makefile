# Takumi House (houset@bu.edu) & Kristel Tan (ktan@bu.edu)
# CAS CS410 Advanced Software Systems
# Due: November 13, 2015

# Makefile

all: myshell tsp_p tspsort 

myshell: myshell.c 
	gcc myshell.c -o myshell 

tsp_p: tsp_p.c
	gcc tsp_p.c -o tsp_p

tspsort: tspsort.c
	gcc tspsort.c -o tspsort

clean: 
	rm -rf myshell
	rm -rf tsp_p
	rm -rf tspsort
	rm -rf paths.txt