#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int m, n, flag = 1, M = -20000; // m: # of rows, n: # of columns
int **arr, **sum, *col; // arr: input array, sum: accumulated sum of arr, col: accumulated sum of a column

// o(n^6)
int mss1(void) {
	int i1, i2, i3, j1, j2, j3, temp, rtn = -200000000;

	for (i1 = 0; i1 < m; i1++) {
		for (j1 = 0; j1 < n; j1++) {
			for (i2 = i1; i2 < m; i2++) {
				for (j2 = j1; j2 < n; j2++) {
					temp = 0;
					for (i3 = i1; i3 <= i2; i3++)
						for (j3 = j1; j3 <= j2; j3++)
							temp += arr[i3][j3];
					if (temp > rtn) rtn = temp;
				}
			}
		}
	}
	if (flag) rtn = M;

	return rtn;
}

// o(n^4)
int mss2(void) {
	int i1, i2, j1, j2, temp, rtn = -200000000;

	for (i1 = 0; i1 < m; i1++) {
		for (j1 = 0; j1 < n; j1++) {
			for (i2 = i1; i2 < m; i2++) {
				for (j2 = j1; j2 < n; j2++) {
					temp = sum[i2][j2];
					if (i1 != 0) temp -= sum[i1 - 1][j2];
					if (j1 != 0) {
						temp -= sum[i2][j1 - 1];
						if (i1 != 0) temp += sum[i1 - 1][j1 - 1];
					}
					if (temp > rtn) rtn = temp;
				}
			}
		}
	}
	if (flag) rtn = M;

	return rtn;
}

// o(n^3)
int mss3(void) {
	int i1, i2, j, temp1, temp2, rtn = -200000000;

	for (i1 = 0; i1 < m; i1++) {
		for (j = 0; j < n; j++) col[j] = 0;
		for (i2 = i1; i2 < m; i2++) {
			for (j = 0; j < n; j++) col[j] += arr[i2][j];
			temp1 = 0;
			temp2 = -200000000;
			for (j = 0; j < n; j++) {
				temp1 += col[j];
				if (temp1 > temp2) temp2 = temp1;
				if (temp1 < 0) temp1 = 0;
			}
			if (temp2 > rtn) rtn = temp2;
		}

	}
	if (flag) rtn = M;

	return rtn;
}


int main(int argc, char* argv[]) {
	int i, j, v, mss, entry;
	char name[100];
	FILE *fp1, *fp2;
	clock_t start, end;

	fp1 = fopen(argv[1], "r");
	if (!fp1) exit(1);

	sprintf(name, "result_%s", argv[1]);

	fp2 = fopen(name, "w");
	if (!fp2) exit(1);

	v = argv[2][0] - '0';

	fscanf(fp1, "%d %d", &m, &n); // read # of rows, columns

	// dynamic allocations
	arr = (int**)malloc(sizeof(int*) * m);
	sum = (int**)malloc(sizeof(int*) * m);
	col = (int*)malloc(sizeof(int) * n);
	for (i = 0; i < m; i++) {
			arr[i] = (int*)malloc(sizeof(int) * n);
			sum[i] = (int*)malloc(sizeof(int) * n);
	}

	start = clock();

	// initialize arr, sum
	for (i = 0; i < m; i++) {
		for (j = 0; j < n; j++) {
			fscanf(fp1, "%d", &entry);
			if (entry > 0) flag = 0;
			if (M < entry) M = entry;
			arr[i][j] = entry;
			sum[i][j] = 0;
		}
	}

	// calculate sum
	for (i = 0; i < m; i++) {
		for (j = 0; j < n; j++) {
			if (j == 0) sum[i][j] = arr[i][j];
			else sum[i][j] = arr[i][j] + sum[i][j - 1];
		}
	}
	for (j = 0; j < n; j++)
		for (i = 1; i < m; i++)
			sum[i][j] += sum[i - 1][j];
	
	// select an algorithm
	switch (v) {
		case 1:
			mss = mss1();
			break;
		case 2:
			mss = mss2();
			break;
		case 3:
			mss = mss3();
			break;
	}

	end = clock();

	// test
	/*
        printf("%s\n", name);
        printf("%d\n", v);
        printf("%d\n", m);
        printf("%d\n", n);
        printf("%d\n", mss);
        printf("%ld\n", (double)(end - start));	
	*/

	// output
	fprintf(fp2, "%s\n", name);
	fprintf(fp2, "%d\n", v);
	fprintf(fp2, "%d\n", m);
	fprintf(fp2, "%d\n", n);
	fprintf(fp2, "%d\n", mss);
	fprintf(fp2, "%ld\n", (long)(end - start));

	fclose(fp1);
	fclose(fp2);

	return 0;
}
