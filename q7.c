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
	float minX;
	float minY;
	float maxX;
	float maxY;
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
	printf("%li|%f|%f|%f|%f\nMinDist = %f\n", nn->nodeno,nn->minX,nn->maxX,nn->minY,nn->maxY,nn->MinDist);
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
	int check;
	regex_t reg;
	size_t numOfMatches = 1;
	regmatch_t matches[numOfMatches];
	char *result;
	float nnDist = INFINITY;
	Branch *nn = NULL;

	Branch *children_head = NULL;
	Branch *children_tail = NULL;

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
		int i = 0;
		while (regexec(&reg, columnText, numOfMatches, matches, 0) == 0) {
			int n = matches[0].rm_eo-matches[0].rm_so;
			if (n>0&&n<strlen(columnText)) {
				char temp[n];
				for(int j = matches[0].rm_so; j < matches[0].rm_eo; ++j) {
					temp[j-matches[0].rm_so] = columnText[j];
				}
				if (i==0) { //number is nodeno
					Branch *b = (Branch *)malloc(sizeof(Branch));
					if (children_head == NULL) { children_head = b; }
					if (children_tail != NULL) { children_tail->next = b; }
					b->prev = children_tail;
					b->next = NULL;
					children_tail = b;
					b->nodeno = atoi(temp);
				} else if (i==1) { //number is minX
					children_tail->minX = atof(temp);
				} else if (i==2) { //number is maxX
					children_tail->maxX = atof(temp);
				} else if (i==3) { //number is minY
					children_tail->minY = atof(temp);
				} else { //number is maxY
					children_tail->maxY = atof(temp);
					float rect[2][2] = {{children_tail->minX,children_tail->maxX},{children_tail->minY,children_tail->maxY}};
					children_tail->MinDist = mindist(point, rect);
					children_tail->MinMaxDist = minmaxdist(point, rect);
				}
				columnText += matches[0].rm_eo+1;
				i = (i+1)%5;
			}
		}
	} else { //This is an object, not a node
		return(NULL);
	}
	
	sqlite3_finalize(stmt);
	//Pruning
	float minminmax = pruning_min_minmaxdist(children_head);
	Branch *temp = children_head;
	while (temp != NULL) {
		Branch *buffer = temp;
		temp = temp->next;
		if (buffer->MinDist > minminmax) {
			if (buffer->prev != NULL) { buffer->prev->next = buffer->next; }
			else { children_head = buffer->next; }
			if (buffer->next != NULL) { buffer->next->prev = buffer->prev; }
			else { children_tail = buffer->prev; }
			free(buffer);
		}
	}
	
	//Recursion
	temp = children_head;
	while (temp != NULL) {
		Branch *buffer = temp;
		temp = temp->next;
		Branch *nn = nnRecursive(buffer->nodeno, point, db, stmt);
		if (nn != NULL) { //If buffer was a node and not an object
			//Replace node with nearest neighbor object within it
			if (buffer->prev != NULL) {
				buffer->prev->next = nn;
				nn->prev = buffer->prev;
			} else { children_head = nn; }
			if (buffer->next != NULL) {
				buffer->next->prev = nn;
				nn->next = buffer->next;
			} else { children_tail = nn; }
			free(buffer);
		}
	}
	
	//Pruning
	temp = children_head;
	while (temp != NULL) {
		Branch *buffer = temp;
		temp = temp->next;
		if (buffer->MinDist > minminmax) {
			if (buffer->prev != NULL) { buffer->prev->next = buffer->next; }
			else { children_head = buffer->next; }
			if (buffer->next != NULL) { buffer->next->prev = buffer->prev; }
			else { children_tail = buffer->prev; }
			free(buffer);
		}
	}
	
	//MinDist
	temp = children_head;
	while (temp != NULL) {
		if (temp->MinDist < nnDist) {
			nn = temp;
			nnDist = temp->MinDist;
		}
		temp = temp->next;
	}
	
	//free linked list
	temp = children_head;
	while (temp != NULL) {
		Branch *buffer = temp;
		temp = temp->next;
		if (buffer != nn) {
			if (buffer->prev != NULL) { buffer->prev->next = buffer->next; }
			else { children_head = buffer->next; }
			if (buffer->next != NULL) { buffer->next->prev = buffer->prev; }
			else { children_tail = buffer->prev; }
			free(buffer);
		}
	}
	return nn;
}

float pruning_min_minmaxdist(Branch *linkedlist) { //Pass in the head of a linked list
	float result = INFINITY;
	while (linkedlist != NULL) {
		if (linkedlist->MinMaxDist < result) { result = linkedlist->MinMaxDist; }
		linkedlist = linkedlist->next;
	}
	return result;
}
