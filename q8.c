#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>

// http://stackoverflow.com/questions/25241406/best-option-for-supplying-quadtree-gps-data-to-an-app

int printQuery(sqlite3 *db, char sql_stmt[]) {
	sqlite3_stmt *stmt;
	int rc;
	rc = sqlite3_prepare_v2(db, sql_stmt, -1, &stmt, 0);
	if (rc != SQLITE_OK) {
			fprintf(stderr, "Preparation failed: %s\n", sqlite3_errmsg(db));
			sqlite3_close(db);
			exit(-1);
	}
	while((rc = sqlite3_step(stmt)) == SQLITE_ROW) {

		printf("%s\n", sqlite3_column_text(stmt, 0));
		printf("%i\n%i\n", sqlite3_column_type(stmt, 1),SQLITE_TEXT);

		//	int size = sqlite3_column_bytes(stmt, 1);
		//	void* p = sqlite3_column_blob(stmt,1);
			//data(p, p+size);
		//	type(sqlite3_column_blob(stmt,1)[size]));
		//	printf("%s\n", " ");
	//		char *Z = (char *) malloc(size);
	//		sqlite3_blob_read(p, Z, size, 0);
	}
	sqlite3_finalize(stmt);
	return 0;
}

int main(int argc, char **argv){
	if( argc!=3 ){
		fprintf(stderr, "Usage: %s <x1> <y1>\n", argv[0]);
		return(1);
	}

	sqlite3 *db; //the database
	int rc = sqlite3_open("assignment2.db", &db);
	if( rc ){
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		exit(-1);
	}

	char *x1 = argv[1];
	char *y1 = argv[2];
	char *x2 = argv[3];
	char *y2 = argv[4];
	char *class = argv[5];
	char *sql_stmt = sqlite3_mprintf("select nodeno, rtreenode(2,data) from poirtree_node where nodeno=1;");

	printQuery(db,sql_stmt);
}
