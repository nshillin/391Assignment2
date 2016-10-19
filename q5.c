#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "sqlite3.h"

int create_indexes();

int main(int argc, char **argv) {

	sqlite3 *db;
	sqlite3_stmt *stmt;

	int check;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s <integer value>\n", argv[0]);
		return(1);
	}

	check = sqlite3_open("assignment2.db", &db);
	if(check != SQLITE_OK) {
		fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return(1);
	}
	
	check = create_indexes(db);
	if (check) { return 1; }
	
	srand(time(NULL));
	int l = atoi(argv[1]);
	int boxes[100][4];

	for (int i = 0; i < 100; i++) {
		boxes[i][0] = rand() % (999 - l); //Returns a number for the min coordinate between 0 and 999-l (leaving room for dimensions)
		boxes[i][1] = boxes[i][0] + l;
		boxes[i][2] = rand() % (999 - l); //As above
		boxes[i][3] = boxes[i][2] + l;
	}
	
	struct timespec start, finish;
	char select_string[255];

	clock_gettime(CLOCK_REALTIME, &start);
	
	for (int i = 0; i < 100; i++) {
		check = sprintf(select_string, "SELECT COUNT(*) FROM poirtree \
				WHERE minX >= %i AND maxX <= %i \
				AND minY >= %i AND maxY <= %i;",
				boxes[i][0], boxes[i][1], boxes[i][2], boxes[i][3]);
		for (int j = 0; j < 20; j++) {
			check = sqlite3_prepare_v2(db, select_string, -1, &stmt, 0);
			if(check != SQLITE_OK) {
				fprintf(stderr, "Cannot prepare statement: %s\n", sqlite3_errmsg(db));
				sqlite3_close(db);
				return(1);
			}
			while((check = sqlite3_step(stmt)) == SQLITE_ROW) { /*One return value*/ }
			sqlite3_finalize(stmt);
		}
	}
	clock_gettime(CLOCK_REALTIME, &finish);
	//1000 ms/s / 2000 queries = 1/2
	float avgTime_rTree = ((float)(finish.tv_sec - start.tv_sec) + (float)(finish.tv_nsec - start.tv_nsec)/1000000000) / 2;
	
	clock_gettime(CLOCK_REALTIME, &start);
	
	for (int i = 0; i < 100; i++) {
		check = sprintf(select_string, "SELECT COUNT(*) FROM poi \
				WHERE minX >= %i AND maxX <= %i \
				AND minY >= %i AND maxY <= %i;",
				boxes[i][0], boxes[i][1], boxes[i][2], boxes[i][3]);
		for (int j = 0; j < 20; j++) {
			check = sqlite3_prepare_v2(db, select_string, -1, &stmt, 0);
			if(check != SQLITE_OK) {
				fprintf(stderr, "Cannot prepare statement: %s\n", sqlite3_errmsg(db));
				sqlite3_close(db);
				return(1);
			}
			while((check = sqlite3_step(stmt)) == SQLITE_ROW) { /*One return value*/ }
			sqlite3_finalize(stmt);
		}
	}
	
	clock_gettime(CLOCK_REALTIME, &finish);
	float avgTime_control = ((float)(finish.tv_sec - start.tv_sec) + (float)(finish.tv_nsec - start.tv_nsec)/1000000000) / 2;
	
	printf("Parameter l: %i\nAverage runtime with r-tree: %f ms\nAverage runtime without r-tree: %f ms\n",
			l, avgTime_rTree, avgTime_control);
	
	return(0);
}

//Creates coordinate indexes if they do not exist.
int create_indexes(sqlite3 *db) {
	sqlite3_stmt *stmt;
	int check;
	char *query1 = "CREATE INDEX IF NOT EXISTS minX_index \
			ON poi (minX);";
	char *query2 = "CREATE INDEX IF NOT EXISTS maxX_index \
			ON poi (maxX);";
	char *query3 = "CREATE INDEX IF NOT EXISTS minY_index \
			ON poi (minY);";
	char *query4 = "CREATE INDEX IF NOT EXISTS maxY_index \
			ON poi (maxY);";
	
	check = sqlite3_prepare_v2(db, query1, -1, &stmt, 0);
	if(check != SQLITE_OK) {
		fprintf(stderr, "Cannot prepare statement: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return(1);
	}
	if((check = sqlite3_step(stmt)) != SQLITE_DONE) {
		fprintf(stderr, "Cannot update: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return(1);
	}
	sqlite3_finalize(stmt);
	
	check = sqlite3_prepare_v2(db, query2, -1, &stmt, 0);
	if(check != SQLITE_OK) {
		fprintf(stderr, "Cannot prepare statement: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return(1);
	}
	if((check = sqlite3_step(stmt)) != SQLITE_DONE) {
		fprintf(stderr, "Cannot update: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return(1);
	}
	sqlite3_finalize(stmt);
	
	check = sqlite3_prepare_v2(db, query3, -1, &stmt, 0);
	if(check != SQLITE_OK) {
		fprintf(stderr, "Cannot prepare statement: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return(1);
	}
	if((check = sqlite3_step(stmt)) != SQLITE_DONE) {
		fprintf(stderr, "Cannot update: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return(1);
	}
	sqlite3_finalize(stmt);
	
	check = sqlite3_prepare_v2(db, query4, -1, &stmt, 0);
	if(check != SQLITE_OK) {
		fprintf(stderr, "Cannot prepare statement: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return(1);
	}
	if((check = sqlite3_step(stmt)) != SQLITE_DONE) {
		fprintf(stderr, "Cannot update: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return(1);
	}
	sqlite3_finalize(stmt);
	return(0);
}
