#include "hw.h"

int main(void) {
	int t, n; // t: 테스트 케이스의 수, n: 페이지의 수
	scanf("%d", &t); // t 입력
	
	for (int i = 0; i < t; i++) { // t번 n 입력
		int *num; // num : 배열을 저장할 포인터
		scanf("%d", &n); // n 입력

		num = single_digit(n); // 0~9의 개수를 저장한 배열을 할당
		print_out(num); // 배열을 출력
		free(num); // 동적 할당 해제
	}

	return 0;
}
