#include <stdio.h>
#include <string.h>

char str[10000000], pat[3000];
int failure[3000];
int idx[3000] = {0,};

int main(void) {
	// 파일 입출력
	FILE *fp1, *fp2, *fp3;

	fp1 = fopen("string.txt", "r");
	fp2 = fopen("pattern.txt", "r");
	fp3 = fopen("result_kmp.txt", "w");

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

	// failure 배열 생성
	int lens = strlen(str);
	int lenp = strlen(pat);
	int i, j, cnt = 0;

	failure[0] = -1;
	for (j = 1; j < lenp; j++) {
		i = failure[j - 1];
		while((pat[j] != pat[i + 1]) && (i >= 0)) i = failure[i];
		if (pat[j] == pat[i + 1]) failure[j] = i + 1;
		else failure[j] = -1;
	}
	
	// kmp algorithm
	i = 0, j = 0;
	while (i < lens) {
		if (str[i] == pat[j]) {i++; j++;}
		else if (j == 0) i++;	
		else j = failure[j - 1] + 1;

		if (j == lenp) {
			idx[cnt++] = i - lenp;
			j = failure[j - 1] + 1;
		}
	}
	
	// result_kmp.txt에 결과 저장
	fprintf(fp3, "%d\n", cnt);	
	if (cnt) {
		for (i = 0; i < cnt; i++) fprintf(fp3, "%d ", idx[i]);
		fprintf(fp3, "\n");
	};
	
	fclose(fp1), fclose(fp2), fclose(fp3);
	return 0;
}
