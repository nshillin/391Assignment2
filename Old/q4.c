#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>

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
	}
	sqlite3_finalize(stmt);
	return 0;
}

int main(int argc, char **argv){
	if( argc!=6 ){
		fprintf(stderr, "Usage: %s <x1> <y1> <x2> <y2> <c>\n", argv[0]);
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
	char *sql_stmt = sqlite3_mprintf("select distinct poi.id from poirtree as poi, poi_tag " \
																	 "where poi.id = poi_tag.id " \
																	 "and minX>=%s and minY>=%s " \
																	 "and maxX<=%s and maxY<=%s " \
																	 "and key = 'class' and value = '%s';" \
																	 ,x1,y1,x2,y2,class);

	printQuery(db,sql_stmt);
}
