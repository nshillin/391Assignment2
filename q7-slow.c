#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include <math.h>

// http://stackoverflow.com/questions/25241406/best-option-for-supplying-quadtree-gps-data-to-an-app

int printQuery(sqlite3 *db, char sql_stmt[], double coord[2], int k) {
	sqlite3_stmt *stmt;
	int rc;
	rc = sqlite3_prepare_v2(db, sql_stmt, -1, &stmt, 0);
	if (rc != SQLITE_OK) {
			fprintf(stderr, "Preparation failed: %s\n", sqlite3_errmsg(db));
			sqlite3_close(db);
			exit(-1);
	}
	double mindist[k];
	double point[k];
	for (int i = 0; i<k; i++) {
		mindist[i] = INFINITY;
		point[i] = 0;
	}
	while((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
		double x = sqlite3_column_double(stmt, 1) + ((sqlite3_column_double(stmt, 2) - sqlite3_column_double(stmt, 1))/2);
		double y = sqlite3_column_double(stmt, 3) + ((sqlite3_column_double(stmt, 4) - sqlite3_column_double(stmt, 3))/2);
		double distance = sqrt(pow(x-coord[0], 2) + pow(y-coord[1], 2));
		for (int position = 0; position<k; position++) {
			if (mindist[position] > distance) {
				for (int i = k-1; i>position; i--) {
					mindist[i] = mindist[i-1];
				}
				mindist[position] = distance;
				point[position] = sqlite3_column_double(stmt, 0);
				break;
			}
		}
	}
	for (int i = 0; i<k; i++) {
		printf("Distance = %f, Point = %.0lf\n", mindist[i], point[i]);
	}
	sqlite3_finalize(stmt);
	return 0;
}

int main(int argc, char **argv){
	sqlite3 *db;
	sqlite3_stmt *stmt;
	int check;

	if (argc != 3) {
		fprintf(stderr, "Usage: %s <x-coordinate> <y-coordinate>\n", argv[0]);
		return(1);
	}
	double coord[2] = {atof(argv[1]), atof(argv[2])};
	int k = 1;

	check = sqlite3_open("assignment2.db", &db);
	if(check != SQLITE_OK) {
		fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return(1);
	}

	char *x1 = argv[1];
	char *y1 = argv[2];
	char *x2 = argv[3];
	char *y2 = argv[4];
	char *class = argv[5];
	char *sql_stmt = sqlite3_mprintf("select * from poirtree;");

	printQuery(db,sql_stmt,coord,k);
}
