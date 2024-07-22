#include "hw.h"

void print_out(int* num) {
	for (int i = 0; i < 10; i++) // 0~9를 가리키는 정수
		printf("%d ", num[i]); // 0~9의 개수를 출력
	printf("\n"); // 테스트 케이스를 구분하기 위해 개행 문자 출력

	return;
}
