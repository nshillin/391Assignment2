Group: Jonathan Ohman, Noah Shillington

Libraries used:
stdio
stdlib
string
sqlite3
math
time
regex

Commands to build:

Q0: python Q0.py
 // Will produce poi_translated.tsv from poi.tsv (used in Q0.txt)

Q4: gcc -g q4.c sqlite3.c -lpthread -ldl -lm -DSQLITE_ENABLE_RTREE=1 -std=c99
 // Run with:
	./a.out <x1> <y1> <x2> <y2> <c>

Q5: gcc -g q5.c sqlite3.c -lpthread -ldl -lm -DSQLITE_ENABLE_RTREE=1
 // Run with:
	./a.out <integer value>

Q7: gcc -g q7.c sqlite3.c -lpthread -ldl -lm -DSQLITE_ENABLE_RTREE=1 -std=c99
 // Run with:
	./a.out <x-coordinate> <y-coordinate>

Q8: gcc -g q8.c sqlite3.c -lpthread -ldl -lm -DSQLITE_ENABLE_RTREE=1 -std=c99
 // Run with:
	./a.out <x-coordinate> <y-coordinate> <k>

We did not collaborate with anyone else to complete this assignment.