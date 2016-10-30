#include <math.h>
#include <stdio.h>
#include <string.h>
#include "sqlite.h"

float mindist(float point[2], float rect[2][2]); //rect is 2 coordinates, each 2 values
float minmaxdist(float point[2], float rect[2][2]);

typedef struct {
	int nodeno;
	float minX;
	float maxX;
	float minY;
	float maxY;
	float MinDist;
	float MinMaxDist;
	Branch *prev;
	Branch *next;
} Branch;

Branch *nnRecursive(int nodeno, sqlite3 *db, sqlite3_stmt *stmt);
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

	Branch *nn = nnRecursive(root_no, db, stmt);
	
	fprintf(stdout, "%i|%f|%f|%f|%f",nn->nodeno,nn->minX,nn->maxX,nn->minY,nn->maxY);
	
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

Branch *nnRecursive(int nodeno, sqlite3 *db, sqlite3_stmt *stmt) {
	float nnDist = INFINITY;
	Branch *nn = NULL;
	
	char leafCheck[255];
	char getChildren[1000];
	char getObjects[255];
	sprintf(leafCheck, "SELECT COUNT(*) FROM poirtree_parent \
			WHERE parentnode = %i", nodeno); //Number of child nodes (leaf if 0)
	sprintf(getChildren, "SELECT pp.nodeno, MIN(minX), MAX(maxX), MIN(minY), MAX(maxY) \
			FROM poirtree p, poirtree_rowid pr, poirtree_parent pp \
			WHERE id = rowid \
			AND pp.parentnode = %i \
			AND (pr.nodeno = pp.nodeno \
			OR pr.nodeno IN \
			(WITH RECURSIVE rChild(nodeno, parentnode) AS ( \
			SELECT nodeno, parentnode FROM poirtree_parent UNION \
			SELECT rChild.nodeno, poirtree_parent.parentnode \
			FROM poirtree_parent, rChild \
			WHERE rChild.parentnode = poirtree_parent.nodeno) \
			SELECT nodeno FROM rChild \
			WHERE parentnode = pp.nodeno)) \
			GROUP BY pp.nodeno;", nodeno); //MBR of all objects of each child node (direct or indirect)
	sprintf(getObjects, "SELECT id, minX, maxX, minY, maxY \
			FROM poirtree, poirtree_rowid \
			WHERE id = rowid AND nodeno = %i", nodeno); //MBRs of each object of this node
	
	Branch *nodes_head = NULL; //Linked list of nodes - head
	Branch *nodes_tail = NULL; //Linked list of nodes - tail
	Branch *objects_head = NULL; //Linked list of objects - head
	Branch *objects_tail = NULL; //Linked list of objects - tail
	
	check = sqlite3_prepare_v2(db, leafCheck, -1, &stmt, 0);
	if(check != SQLITE_OK) {
		fprintf(stderr, "Cannot prepare statement: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return(1);
	}
	bool isLeaf = ((check = sqlite3_step(stmt)) != SQLITE_ROW); //true if node is a leaf
	sqlite3_finalize(stmt);
	
	float min_minmaxDist = INFINITY; //Smallest minmaxdist, against which mindist values are measured in pruning 1.
	
	if (isLeaf) {
		check = sqlite3_prepare_v2(db, getChildren, -1, &stmt, 0);
		if(check != SQLITE_OK) {
			fprintf(stderr, "Cannot prepare statement: %s\n", sqlite3_errmsg(db));
			sqlite3_close(db);
			return(1);
		}
		while((check = sqlite3_step(stmt)) == SQLITE_ROW) {
			Branch *b = (Branch*)malloc(sizeof(Branch));
			
			b->nodeno = sqlite3_column_int(stmt, 0);
			b->minX = (float)sqlite3_column_double(stmt, 1);
			b->maxX = (float)sqlite3_column_double(stmt, 2);
			b->minY = (float)sqlite3_column_double(stmt, 3);
			b->maxY = (float)sqlite3_column_double(stmt, 4);
			b->next = NULL;
			float rect[2][2] = {{b.minX,b.maxX},{b.minY,b.maxY}};
			
			b->MinDist = mindist(float point[2], float rect[2][2]);
			b->MinMaxDist = minmaxdist(float point[2], float rect[2][2]);
			if (b->MinMaxDist < min_minmaxDist) { min_minmaxDist = b->MinMaxDist; }
			
			if (nodes_head == NULL) { nodes_head = b; }
			if (nodes_tail != NULL) { nodes_tail->next = b; }
			b->prev = nodes_tail;
			nodes_tail = b;
		}
		sqlite3_finalize(stmt);
	} else {
		check = sqlite3_prepare_v2(db, getObjects, -1, &stmt, 0);
		if(check != SQLITE_OK) {
			fprintf(stderr, "Cannot prepare statement: %s\n", sqlite3_errmsg(db));
			sqlite3_close(db);
			return(1);
		}
		while((check = sqlite3_step(stmt)) == SQLITE_ROW) {
			Branch *b = (Branch*)malloc(sizeof(Branch));
			
			b->nodeno = sqlite3_column_int(stmt, 0);
			b->minX = (float)sqlite3_column_double(stmt, 1);
			b->maxX = (float)sqlite3_column_double(stmt, 2);
			b->minY = (float)sqlite3_column_double(stmt, 3);
			b->maxY = (float)sqlite3_column_double(stmt, 4);
			b->next = NULL;
			float rect[2][2] = {{b.minX,b.maxX},{b.minY,b.maxY}};
			
			b->MinDist = mindist(float point[2], float rect[2][2]);
			b->MinMaxDist = minmaxdist(float point[2], float rect[2][2]);
			if (b->MinMaxDist < min_minmaxDist) { min_minmaxDist = b->MinMaxDist; }
			
			if (objects_head == NULL) { objects_head = b; }
			if (objects_tail != NULL) { objects_tail->next = b; }
			b->prev = objects_tail;
			objects_tail = b;
		}
		sqlite3_finalize(stmt);
	}
	
	//Pruning 1: Remove MBRs farther than furthest point of nearest MBR
	Branch *ll_pointer = nodes_head;
	float nodes_minminmax = pruning_min_minmaxdist(nodes_head);
	while (ll_pointer != NULL) {
		if (ll_pointer->MinDist > nodes_minminmax) {
			//Remove link
			Branch *buffer = ll_pointer;
			ll_pointer = ll_pointer->next;
			if (buffer->prev == NULL) { nodes_head = buffer->next; }
			else { buffer->prev->next = buffer->next; }
			if (buffer->next == NULL) { nodes_tail = buffer->prev; }
			else { buffer->next->prev = buffer->prev; }
			free(buffer);
		}
	}
	//Pruning 2: Remove objects farther than furthest point of nearest MBR
	ll_pointer = objects_head;
	while (ll_pointer != NULL) {
		if (ll_pointer->MinDist > nodes_minminmax) {
			//Remove link
			Branch *buffer = ll_pointer;
			ll_pointer = ll_pointer->next;
			if (buffer->prev == NULL) { objects_head = buffer->next; }
			else { buffer->prev->next = buffer->next; }
			if (buffer->next == NULL) { objects_tail = buffer->prev; }
			else { buffer->next->prev = buffer->prev; }
			free(buffer);
		}
	}
	
	ll_pointer = nodes_head;
	while (ll_pointer != NULL) {
		//Run nearestneighbor on branch, add to objects (will return object), delete branch
		Branch *b = nnRecursive(ll_pointer->nodeno, db, stmt);
		
		if (objects_head == NULL) { objects_head = b; }
		if (objects_tail != NULL) { objects_tail->next = b; }
		b->prev = objects_tail;
		objects_tail = b;

		Branch *buffer = ll_pointer;
		ll_pointer = ll_pointer->next;
		if (buffer->prev == NULL) { nodes_head = buffer->next; }
		else { buffer->prev->next = buffer->next; }
		if (buffer->next == NULL) { nodes_tail = buffer->prev; }
		else { buffer->next->prev = buffer->prev; }
		free(buffer);
		
		//Pruning 2: Remove objects farther than furthest point of nearest MBR
		Branch *ll_pointer2 = objects_head;
		while (ll_pointer2 != NULL) {
			if (ll_pointer2->MinDist > nodes_minminmax) {
				//Remove link
				Branch *buffer2 = ll_pointer2;
				ll_pointer2 = ll_pointer2->next;
				if (buffer2->prev == NULL) { objects_head = buffer2->next; }
				else { buffer2->prev->next = buffer2->next; }
				if (buffer2->next == NULL) { objects_tail = buffer2->prev; }
				else { buffer2->next->prev = buffer2->prev; }
				free(buffer2);
			}
		}

		//Pruning 3: Remove MBRs farther than furthest point of nearest object
		ll_pointer2 = nodes_head;
		float objs_minminmax = pruning_min_minmaxdist(objects_head);
		while (ll_pointer2 != NULL) {
			if (ll_pointer2->MinDist > objs_minminmax) {
				//Remove link
				Branch *buffer2 = ll_pointer2;
				ll_pointer2 = ll_pointer2->next;
				if (buffer2->prev == NULL) { nodes_head = buffer2->next; }
				else { buffer2->prev->next = buffer2->next; }
				if (buffer2->next == NULL) { nodes_tail = buffer2->prev; }
				else { buffer2->next->prev = buffer2->prev; }
				free(buffer2);
			}
		}
	}
	//At this point: only objects are left as nn candidates.
	ll_pointer = objects_head;
	while (ll_pointer != NULL) {
		if (ll_pointer->MinDist < nnDist) {
			nnDist = ll_pointer->minDist;
			nn = ll_pointer;
			ll_pointer = ll_pointer->next;
		}
	}
	//free linked lists.
	ll_pointer = nodes_head;
	while (ll_pointer != NULL) {
		Branch *buffer = ll_pointer;
		ll_pointer = ll_pointer->next;
		free(buffer);
	}
	ll_pointer = objects_head;
	while (ll_pointer != NULL) {
		Branch *buffer = ll_pointer;
		ll_pointer = ll_pointer->next;
		free(buffer);
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
