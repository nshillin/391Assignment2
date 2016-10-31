#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include "sqlite3.h"

float mindist(float point[2], float rect[2][2]); //rect is 2 coordinates, each 2 values
float minmaxdist(float point[2], float rect[2][2]);

typedef struct node {
	long nodeno;
	float MinDist;
	float MinMaxDist;
	struct node *prev;
	struct node *next;
} Branch;

Branch *nnRecursive(long nodeno, float point[2], sqlite3 *db, sqlite3_stmt *stmt);
float pruning_min_minmaxdist(Branch *linkedlist);

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

	Branch *nn = nnRecursive(1, coord, db, stmt);

	sqlite3_close(db);

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
	for (int k = 0; k < 2; k++) {
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

Branch *nnRecursive(long nodeno, float point[2], sqlite3 *db, sqlite3_stmt *stmt) {
	fprintf(stdout, "Beginning nn on node %li\n", nodeno);
	int check;
	regex_t reg;
	size_t numOfMatches = 1;
	regmatch_t matches[numOfMatches];
	char *result;
	float nnDist = INFINITY;
	Branch *nn = NULL;

	char getChildren[255];
	sprintf(getChildren, "SELECT rtreenode(2,data) FROM poirtree_node \
			WHERE nodeno = %li;", nodeno); //MBRs of all children of nodeno

	float min_minmaxDist = INFINITY; //Smallest minmaxdist, against which mindist values are measured in pruning 1.

	check = sqlite3_prepare_v2(db, getChildren, -1, &stmt, 0);
	if(check != SQLITE_OK) {
		fprintf(stderr, "Cannot prepare statement: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return(NULL);
	}
	if((check = sqlite3_step(stmt)) == SQLITE_ROW) { //true if node, false if object
		regcomp(&reg, "[0-9]+\\.?[0-9]*", REG_EXTENDED);
		const char *columnText = sqlite3_column_text(stmt, 0);
		while (regexec(&reg, columnText, numOfMatches, matches, 0) == 0) {
			int n = matches[0].rm_eo-matches[0].rm_so;
			if (n>0
				&&n<strlen(columnText)) {
				for(int j = matches[0].rm_so; j < matches[0].rm_eo; ++j) {
                			printf("%c", columnText[j]);
				}
				printf("\n");
			}
			columnText += matches[0].rm_eo+1;
		}
	} else {
		return(NULL);
	}
	sqlite3_finalize(stmt);

	return NULL;
}

float pruning_min_minmaxdist(Branch *linkedlist) { //Pass in the head of a linked list
	float result = INFINITY;
	while (linkedlist != NULL) {
		if (linkedlist->MinMaxDist < result) { result = linkedlist->MinMaxDist; }
		linkedlist = linkedlist->next;
	}
	return result;
}
