#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct {
	int src;
	int dst;
	int weight;
} element;

clock_t start, end;
double runningTime;
int n = 0, m = 0, v, e; // v: # of vertices, e: # of edges
long min = 0;
element heap[50000001]; // min heap
int parent[10000]; // disjoint set
element mst[9999]; // minimum spanning tree

void insert(element item) {
	int i;
	if (n == 50000000) exit(1);

	for (i = ++n; (i != 1) && (item.weight < heap[i / 2].weight); heap[i] = heap[i / 2], i /= 2);
	heap[i] = item;
}

element delete(void) {
	int parent, child;
	element item, temp;
	if (n == 0) exit(1);

	item = heap[1];
	temp = heap[n--];
	parent = 1;
	child = 2;

	while (child <= n) {
		if ((child < n) && (heap[child].weight > heap[child + 1].weight)) child++;
		if (temp.weight <= heap[child].weight) break;
		heap[parent] = heap[child];
		parent = child;
		child *= 2;
	}
	heap[parent] = temp;

	return item;
}

int sFind(int i) {
	while (parent[i] >= 0) i = parent[i];

	return i;
}

void wUnion(int i, int j) {
	int temp;

	temp = parent[i] + parent[j];
	if (parent[i] > parent[j]) {
		parent[i] = j;
		parent[j] = temp;
	}
	else {
		parent[j] = i;
		parent[i] = temp;
	}
}

int main(int argc, char* argv[]) {
	start = clock();

	int i, j, src, dst, weight;
	element temp;
	FILE *fp1, *fp2;

	if (argc != 2) {
		printf("usage: ./fp1 input_filename\n");
		exit(1);
	}

	fp1 = fopen(argv[1], "r");
	if (fp1 == NULL) {
		printf("The input file does not exist.\n");
		exit(1);
	}

	fp2 = fopen("fp1_result.txt", "w");
	if (fp2 == NULL) exit(1);

	fscanf(fp1, "%d", &v);
	fscanf(fp1, "%d", &e);
	for (i = 0; i < e; i++) {
		fscanf(fp1, "%d %d %d", &src, &dst, &weight);
		temp.src = src;
		temp.dst = dst;
		temp.weight = weight;
		insert(temp);
	}

	for (i = 0; i < v; i++) parent[i] = -1;

	while ((m < v - 1) && n) {
		temp = delete();
		i = sFind(temp.src);
		j = sFind(temp.dst);

		if (i != j) {
			wUnion(i, j);
			mst[m] = temp;
			min += mst[m].weight;
			m++;
		}
	}

	for (i = 0; i < m; i++)
		fprintf(fp2, "%d %d %d\n", mst[i].src, mst[i].dst, mst[i].weight);
	fprintf(fp2, "%ld\n", min);
	if (m == v - 1)	fprintf(fp2, "CONNECTED\n");
	else fprintf(fp2, "DISCONNECTED\n");

	end = clock();
	runningTime = (double)(end - start) / CLOCKS_PER_SEC;

	printf("output written to fp1_result.txt.\n");
	printf("running time: %lf seconds\n", runningTime);

	fclose(fp1);
	fclose(fp2);

	return 0;
}
