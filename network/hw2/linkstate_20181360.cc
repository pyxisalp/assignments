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

// n: # of vertices, visited: visited or not, dir: connected w/ i
int n, inf = 10001;
int **graph, **result, **visited, **dir, **d, **p, **spt;

// link-state routing algorithm function
void dijkstra(int i) {
	int j, k = 0, min, minIdx;

	visited[i][i] = 1;
	spt[i][k++] = i;

	for (j = 0; j < n; j++)
		if (i != j && graph[i][j] < inf)
			p[i][j] = i;

	while (k < n) {
		min = inf, minIdx = -1;

		for (j = 0; j < n; j++)
			if (!visited[i][j] && d[i][j] > 0)
				if (p[i][j] != -1 && d[i][j] < min)
					min = d[i][j], minIdx = j;

		if (min == inf)	break;

		visited[i][minIdx] = 1;
		result[i][minIdx] = min;
		d[i][minIdx] = 0;
		spt[i][k++] = minIdx;

		for (j = 0; j < n; j++) {
			if (!visited[i][j] && d[i][j] > 0) {
				if (p[i][j] > minIdx && d[i][j] == (result[i][minIdx] + graph[minIdx][j]))
					p[i][j] = minIdx;
				else if (d[i][j] > result[i][minIdx] + graph[minIdx][j]) {
					d[i][j] = result[i][minIdx] + graph[minIdx][j];
					p[i][j] = minIdx;
				}
			}
		}
	}

	for (j = 0; j < n; j++) {
		k = j;
		if (i != j && p[i][k] != -1) {
			while (p[i][k] != i) k = p[i][k];
			dir[i][j] = k;	
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
		printf("usage: linkstate topologyfile messagesfile changesfile\n");
		return 1;
	}

	fpT = fopen(argv[1], "r");
	fpM = fopen(argv[2], "r");
	fpC = fopen(argv[3], "r");

	if (fpT == NULL || fpM == NULL || fpC == NULL) {
		printf("Error: open input file.\n");
		return 2;
	}

	fpO = fopen("output_ls.txt", "w");
	if (fpO == NULL)
		return 3;

	fscanf(fpT, "%d", &n);

	graph = (int**)malloc(sizeof(int*) * n);
	result = (int**)malloc(sizeof(int*) * n);
	visited = (int**)malloc(sizeof(int*) * n);
	dir = (int**)malloc(sizeof(int*) * n);
	d = (int**)malloc(sizeof(int*) * n);
	p = (int**)malloc(sizeof(int*) * n);
	spt = (int**)malloc(sizeof(int*) * n);

	for (i = 0; i < n; i++) {
		graph[i] = (int*)malloc(sizeof(int) * n);
		result[i] = (int*)malloc(sizeof(int) * n);
		visited[i] = (int*)malloc(sizeof(int) * n);
		dir[i] = (int*)malloc(sizeof(int) * n);
		d[i] = (int*)malloc(sizeof(int) * n);
		p[i] = (int*)malloc(sizeof(int) * n);
		spt[i] = (int*)malloc(sizeof(int) * n);
	}

	for (i = 0; i < n; i++) {
		for (j = 0; j < n; j++) {
			graph[i][j] = inf;
			result[i][j] = inf;
			visited[i][j] = 0;
			dir[i][j] = -1;
			d[i][j] = inf;
			p[i][j] = -1;
			spt[i][j] = -1;
		}
	}

	for (i = 0; i < n; i++) {
		graph[i][i] = 0;
		result[i][i] = 0;
		dir[i][i] = i;
		d[i][i] = 0;
	}

	// make a undirected graph
	while (fscanf(fpT, "%d %d %d", &src, &dst, &weight) != EOF) {
		graph[src][dst] = weight;
		graph[dst][src] = weight;
		result[src][dst] = weight;
		result[dst][src] = weight;
		dir[src][dst] = dst;
		dir[dst][src] = src;
                d[src][dst] = weight;
                d[dst][src] = weight;
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

	for (i = 0; i < n; i++)
		dijkstra(i);

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
				visited[i][j] = 0;
				dir[i][j] = -1;
				d[i][j] = graph[i][j];
	                        p[i][j] = -1;
                	        spt[i][j] = -1;
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

        	for (i = 0; i < n; i++)
	                dijkstra(i);

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
		free(visited[i]);
		free(dir[i]);
		free(d[i]);
		free(p[i]);
		free(spt[i]);
	}

	free(graph);
	free(result);
	free(visited);
	free(dir);
	free(d);
	free(p);
	free(spt);

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

        printf("Complete. Output file written to output_ls.txt.\n");

	fclose(fpT), fclose(fpM), fclose(fpC), fclose(fpO);

	return 0;
}
