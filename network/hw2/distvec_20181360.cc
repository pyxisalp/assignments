#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// message node
typedef struct _mNode {
	int src;
	int dst;
	char data[1000];
	struct _mNode *link;
} mNode;

// change node
typedef struct _cNode {
	int src;
	int dst;
	int weight;
	struct _cNode *link;
} cNode;

mNode *mHead = NULL, *mTail = NULL, *mCurr;
cNode *cHead = NULL, *cTail = NULL, *cCurr;

// n: # of vertices, dir: connected w/ i
int n, inf = 10001;
int **graph, **result, **dir;

// distance vector routing algorithm function
void distanceVector(void) {
	// update: routing table updated or not
	int i, j, k, update = 1;

	while (update) {
		update = 0;
		
		for (k = 0; k < n; k++) {
			for (i = 0; i < n; i++) {
				for (j = 0; j < n; j++) {
					if (i != j && graph[i][k] && dir[i][j] > k && result[i][j] == graph[i][k] + result[k][j]) {
						dir[i][j] = k;
						update = 1;
					}
                                        else if (i != j && graph[i][k] && result[i][j] > graph[i][k] + result[k][j]) {
                                                result[i][j] = graph[i][k] + result[k][j];
                                                dir[i][j] = k;
                                         	update = 1;
                                        }

				}
			}
		}
	}
}

int main(int argc, char *argv[]) {
	int i, j, src, dst, weight, curr;
	char buf[1000], hop[1000];
	mNode *mTemp;
	cNode *cTemp;
	FILE *fpT, *fpM, *fpC, *fpO;

	if (argc != 4) {
		printf("usage: distvec topologyfile messagesfile changesfile\n");
		return 1;
	}

	fpT = fopen(argv[1], "r");
	fpM = fopen(argv[2], "r");
	fpC = fopen(argv[3], "r");

	if (fpT == NULL || fpM == NULL || fpC == NULL) {
		printf("Error: open input file.\n");
		return 2;
	}

	fpO = fopen("output_dv.txt", "w");
	if (fpO == NULL)
		return 3;

	fscanf(fpT, "%d", &n);

	graph = (int**)malloc(sizeof(int*) * n);
	result = (int**)malloc(sizeof(int*) * n);
	dir = (int**)malloc(sizeof(int*) * n);

	for (i = 0; i < n; i++) {
		graph[i] = (int*)malloc(sizeof(int) * n);
		result[i] = (int*)malloc(sizeof(int) * n);
		dir[i] = (int*)malloc(sizeof(int) * n);
	}

	for (i = 0; i < n; i++) {
		for (j = 0; j < n; j++) {
			graph[i][j] = inf;
			result[i][j] = inf;
			dir[i][j] = -1;
		}
	}

	for (i = 0; i < n; i++) {
		graph[i][i] = 0;
		result[i][i] = 0;
		dir[i][i] = i;
	}

	// make a undirected graph
	while (fscanf(fpT, "%d %d %d", &src, &dst, &weight) != EOF) {
		graph[src][dst] = weight;
		graph[dst][src] = weight;
		result[src][dst] = weight;
		result[dst][src] = weight;
		dir[src][dst] = dst;
		dir[dst][src] = src;
	}

	// make a linked list of messages
	while (fscanf(fpM, "%d %d %[^\n]s", &src, &dst, buf) != EOF) {
		mTemp = (mNode*)malloc(sizeof(mNode));
		if (mTemp == NULL) return 3;
		mTemp->src = src;
		mTemp->dst = dst;
		strcpy(mTemp->data, buf);
		mTemp->link = NULL;

		if (!mHead) {
			mHead = mTemp;
			mTail = mTemp;
		}
		else {
			mTail->link = mTemp;
			mTail = mTemp;
		}
	}

	// make a linked list of changes
	while (fscanf(fpC, "%d %d %d", &src, &dst, &weight) != EOF) {
		cTemp = (cNode*)malloc(sizeof(mNode));
		if (cTemp == NULL) return 3;
		cTemp->src = src;
		cTemp->dst = dst;
		cTemp->weight = weight;
		cTemp->link = NULL;

		if (!cHead) {
			cHead = cTemp;
			cTail = cTemp;
		}
		else {
			cTail->link = cTemp;
			cTail = cTemp;
		}
	}

	distanceVector();

	for (i = 0; i < n; i++) {
		for (j = 0; j < n; j++)
			if (result[i][j] != inf)
				fprintf(fpO, "%d %d %d\n", j, dir[i][j], result[i][j]);
		fprintf(fpO, "\n");
	}

	mCurr = mHead;
	while (mCurr) {
		if (result[mCurr->src][mCurr->dst] == inf) {
			fprintf(fpO, "from %d to %d cost infinite hops unreachable message %s\n", mCurr->src, mCurr->dst, mCurr->data);
			break;
		}
		else {
			sprintf(hop, "%d", mCurr->src);

			curr = dir[mCurr->src][mCurr->dst];
			while (curr != mCurr->dst) {
				sprintf(hop, "%s %d", hop, curr);
				curr = dir[curr][mCurr->dst];
			}

			fprintf(fpO, "from %d to %d cost %d hops %s message %s\n", mCurr->src, mCurr->dst, result[mCurr->src][mCurr->dst], hop, mCurr->data);
			mCurr = mCurr->link;
		} 
	}
	fprintf(fpO, "\n");

	cCurr = cHead;
	while (cCurr) {
		if (cCurr->weight != -999) {
			graph[cCurr->src][cCurr->dst] = cCurr->weight;
			graph[cCurr->dst][cCurr->src] = cCurr->weight;
		}
		else {
			graph[cCurr->src][cCurr->dst] = inf;
			graph[cCurr->dst][cCurr->src] = inf;
		}

	        for (i = 0; i < n; i++) {
	                for (j = 0; j < n; j++) {
	                        result[i][j] = graph[i][j];
	                        dir[i][j] = -1;
                	}
	        }

		for (i = 0; i < n; i++) {
			for (j = 0; j < n; j++) {
				if (i == j)
					dir[i][i] = i;
				else if (graph[i][j] < inf)
					dir[i][j] = j;
			}
		}	

        	distanceVector();

        	for (i = 0; i < n; i++) {
                	for (j = 0; j < n; j++)
				if (result[i][j] != inf)
	                        	fprintf(fpO, "%d %d %d\n", j, dir[i][j], result[i][j]);
	                fprintf(fpO, "\n");
        	}

	        mCurr = mHead;
        	while (mCurr) {
			if (result[mCurr->src][mCurr->dst] == inf) {
				fprintf(fpO, "from %d to %d cost infinite hops unreachable message %s\n", mCurr->src, mCurr->dst, mCurr->data);
				break;
			}
			else {
	        	        sprintf(hop, "%d", mCurr->src);

        	        	curr = dir[mCurr->src][mCurr->dst];
	        	        while (curr != mCurr->dst) {
        	        	        sprintf(hop, "%s %d", hop, curr);
                	        	curr = dir[curr][mCurr->dst];
		                }

        		        fprintf(fpO, "from %d to %d cost %d hops %s message %s\n", mCurr->src, mCurr->dst, result[mCurr->src][mCurr->dst], hop, mCurr->data);
                		mCurr = mCurr->link;
			}
	        }
	        fprintf(fpO, "\n");

		cCurr = cCurr->link;
	}

	for (i = 0; i < n; i++) {
		free(graph[i]);
		free(result[i]);
		free(dir[i]);
	}

	free(graph);
	free(result);
	free(dir);

	mCurr = mHead;
	while (mCurr) {
		mTemp = mCurr;
		mCurr = mCurr->link;
		free(mTemp);
	}

	cCurr = cHead;
	while (cCurr) {
		cTemp = cCurr;
		cCurr = cCurr->link;
		free(cTemp);
	}

	printf("Complete. Output file written to output_dv.txt.\n");

	fclose(fpT), fclose(fpM), fclose(fpC), fclose(fpO);

	return 0;
}
