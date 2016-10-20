#include <math.h>
#include <stdio.h>
#include <string.h>
#include "sqlite.h"

float mindist(float point[2], float rect[2][2]); //rect is 2 coordinates, each 2 values
float minmaxdist(float point[2], float rect[2][2]);
void nnRecursive(int nodeno);

typedef struct {
	int nodeno;
	float minX;
	float maxX;
	float minY;
	float maxY;
	float MinDist;
	float MinMaxDist;
} Branch;

int main(int argc, char **argv) {

	sqlite3 *db;
	sqlite3_stmt *stmt;

	int check;

	if (argc != 3) {
		fprintf(stderr, "Usage: %s <x-coordinate> <y-coordinate>\n", argv[0]);
		return(1);
	}
	
	float coord[2] = {atof(argv[1]), atof(argv[2])};
	
	check = sqlite3_open("assignment2.db", &db);
	if(check != SQLITE_OK) {
		fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return(1);
	}
	
	char *root_select = "SELECT nodeno FROM poirtree WHERE id NOT IN \
			(SELECT nodeno FROM poirtree_parent);"; //Select the root node, which will be absent from poirtree_parent
	
	check = sqlite3_prepare_v2(db, select_string, -1, &stmt, 0);
	if(check != SQLITE_OK) {
		fprintf(stderr, "Cannot prepare statement: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return(1);
	}
	
	int root_no;
	while((check = sqlite3_step(stmt)) == SQLITE_ROW) { root_no = sqlite3_column_int(stmt, 0) }
	sqlite3_finalize(stmt);

	nnRecursive(root_no); //TODO
	
	return 0;
}


float mindist(float point[2], float rect[2][2]) {
	float result = 0;
	for (int i = 0; i < 2; i++) {
		if (point[i] < rect[0][i]) {
			result += pow(point[i] - rect[0][i], 2);
		} else if (point[i] > rect[1][i]) {
			result += pow(point[i] - rect[1][i], 2);
		} //else: point is within rect w.r.t. this dimension; 0 distance added
	}
	return result;
}

float minmaxdist(float point[2], float rect[2][2]) {
	float result = INFINITY; //will unconditionally be overwritten
	for (int k = 0; k < 2; i++) {
		/*Algorithm calls to iterate over all i (1<=i<=n,i!=k). In 2-space, i is 1 value:
		1 for k=2 or 2 for k=1, ie. (k+1)%2 when i and k are zero-indexed.*/
		int i = (k+1)%2;
		float rm_k, rM_i, temp_dist;
		if (point[k] <= (rect[0][k] + rect[1][k])/2) { rm_k = rect[0][k]; }
		else { rm_k = rect[1][k]; }
		if (point[i] >= (rect[0][i] + rect[1][i])/2) { rM_i = rect[0][i]; }
		else { rM_i = rect[1][i]; }
		
		temp_dist = pow(point[k] - rm_k,2) + pow(point[i] - rM_i,2);
		if (temp_dist < result) {
			result = temp_dist; //result will be minimum of temp_dist values
		}
	}
	return result;
}

void nnRecursive(int nodeno) {
	float nnDist = INFINITY;
	
	Branch *abl = malloc(sizeof(*abl) * 100);
	//TODO
	free (abl);
}
