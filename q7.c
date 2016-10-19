#include <stdio.h>
#include <string.h>
#include "sqlite.h"

int main(int argc, char **argv) {

	sqlite3 *db;
	sqlite3_stmt *stmt;

	int check;

	if (argc != 3) {
		fprintf(stderr, "Usage: %s <x-coordinate> <y-coordinate>\n", argv[0]);
		return(1);
	}
	
	float coord[2] = {atof(argv[1]), atof(argv[2])};
	
	return 0;
}
