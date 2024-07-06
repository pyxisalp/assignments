#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int n, *arr, *arr_l, *arr_r, *arr2;

void insertion_sort(void) {
	int i, j, key;

	for (i = 1; i < n; i++) {
		key = arr[i];
		for (j = i - 1; j >= 0 && arr[j] > key; j--) arr[j + 1] = arr[j];
		arr[j + 1] = key;
	}

	return;
}

int partition(int left, int right) {
	int i, pivot, temp;

	pivot = left;
	for (i = left; i < right; i++) {
		if (arr[i] < arr[right]) {
			temp = arr[i];
			arr[i] = arr[pivot];
			arr[pivot] = temp;
			pivot++;
		}
	}
	temp = arr[right];
	arr[right] = arr[pivot];
	arr[pivot] = temp;

	return pivot;
}

void quick_sort(int left, int right) {
	int pivot;

	if (right - left > 0) {
		pivot = partition(left, right);
		quick_sort(left, pivot - 1);
		quick_sort(pivot + 1, right);
	}

	return;
}

void merge(int p, int q, int r) {
	int i, j, k, nl, nr;

	nl = q - p + 1;
	nr = r - q;
	arr_l = (int*)malloc(sizeof(int) * nl);
	arr_r = (int*)malloc(sizeof(int) * nr);
	for (i = 0; i < nl; i++) arr_l[i] = arr[i + p];
	for (j = 0; j < nr; j++) arr_r[j] = arr[j + q + 1];
	i = j = 0;
	k = p;
	while (i < nl && j < nr) {
		if (arr_l[i] <= arr_r[j]) {
			arr[k] = arr_l[i];
			i++;
		}
		else {
			arr[k] = arr_r[j];
			j++;
		}
		k++;
	}
	while (i < nl) {
		arr[k] = arr_l[i];
		i++;
		k++;
	}
	while (j < nr) {
		arr[k] = arr_r[j];
		j++;
		k++;
	}
	free(arr_l);
	free(arr_r);

	return;
}

void merge_sort(int p, int r) {
	int q;

	if (p >= r) return;
	q = (p + r) / 2;
	merge_sort(p, q);
	merge_sort(q + 1, r);
	merge(p, q, r);

	return;
}

int randomized_partition(int left, int right) {
	int i, temp;

	i = rand() % (right - left + 1) + left;
	temp = arr[right];
	arr[right] = arr[i];
	arr[i] = temp;

	return partition(left, right);
}

void randomized_quick_sort(int left, int right) {
	int pivot;


	while (right - left > 0) {
		pivot = randomized_partition(left, right);
		if (pivot - left < right - pivot) {
			randomized_quick_sort(left, pivot - 1);
			left = pivot + 1;
		}
		else {
			randomized_quick_sort(pivot + 1, right);
			right = pivot - 1;
		}
	}

	return;
}


void improved_sort(int left, int right) {
	if (n <= 16) insertion_sort();
	else randomized_quick_sort(0, n - 1);

	return;
}

int main(int argc, char* argv[]) {
	int c, i;
	char name[100];
	FILE *fp1, *fp2;
	clock_t start, end;
	srand(time(NULL));

	fp1 = fopen(argv[1], "r");
	if (!fp1) exit(1);

	c = argv[2][0] - '0';
	sprintf(name, "result_%d_%s", c, argv[1]);

	fp2 = fopen(name, "w");
	if (!fp2) exit(1);

	fscanf(fp1, "%d", &n);
	arr = (int*)malloc(sizeof(int) * n);
	for (i = 0; i < n; i++) fscanf(fp1, "%d", &arr[i]);

	start = clock();

	switch (c) {
		case 1:
			insertion_sort();
			break;
		case 2:
			quick_sort(0, n - 1);
			break;
		case 3:
			merge_sort(0, n - 1);
			break;
		case 4:
			improved_sort(0, n - 1);
			break;

	}

	end = clock();

	// test
/*
	for (i = 0; i < n; i++) printf("%d ", arr[i]);
	printf("\n");
*/

	fprintf(fp2, "%s\n", argv[1]);
	fprintf(fp2, "%d\n", c);
	fprintf(fp2, "%d\n", n);
	fprintf(fp2, "%lf\n", (double)(end - start) / CLOCKS_PER_SEC);
	for (i = 0; i < n; i++) fprintf(fp2, "%d ", arr[i]);
	fprintf(fp2, "\n");

	free(arr);
	fclose(fp1);
	fclose(fp2);

	return 0;
}
