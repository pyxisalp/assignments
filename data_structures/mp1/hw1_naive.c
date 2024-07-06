#include <stdio.h>
#include <string.h>

char str[10000000], pat[3000];
int idx[3000] = {0, };

int main(void) {
	// 파일 입출력
	FILE *fp1, *fp2, *fp3;

	fp1 = fopen("string.txt", "r");
	fp2 = fopen("pattern.txt", "r");
	fp3 = fopen("result_naive.txt", "w");

	if (fp1 == NULL) {
		printf("The string file does not exist.\n");
		return 1;
	}
	if (fp2 == NULL) {
		printf("The pattern file does not exist.\n");
		return 2;
	}
	if (fp3 == NULL) {
		printf("Fail to execute the result file.\n");
		return 3;
	}

	// str에 문자열, pat에 패턴 저장
	if (fgets(str, 10000000, fp1) == NULL) {
		fprintf(fp3, "0\n");
		return 4;
	}
	if (fgets(pat, 3000, fp2) == NULL) {
		fprintf(fp3, "0\n");
		return 5;
	}

	// cnt에 반복되는 패턴의 개수 저장
	int lens = strlen(str);
	int lenp = strlen(pat);
	int i = 0, cnt = 0;

	// naive algorithm
	while(i <= lens - lenp) {
		int j = 0;
		while (j < lenp) {
			if (str[i + j] != pat[j]) break;
			j++;
		}
		if (j == lenp) idx[cnt++] = i;
		i++;
	}
	
	// result_naive.txt에 결과 저장
	fprintf(fp3, "%d\n", cnt);
	if (cnt) {
		for (i = 0; i < cnt; i++) fprintf(fp3, "%d ", idx[i]);
		fprintf(fp3, "\n");
	}

	fclose(fp1), fclose(fp2), fclose(fp3);
	return 0;
}
