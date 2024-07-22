#include "hw.h"

int* single_digit(int n) {
	int k = 0; // 10^k를 표현하기 위한 정수
	int digit[10]; // n의 자릿수를 저장할 배열
	int front[10]; // 한 자릿수의 왼쪽에 올 수 있는 경우의 수
	int back; // 한 자릿수의 오른쪽에 올 수 있는 경우의 수
	int* rtn = (int*)malloc(sizeof(int) * 10); // 0~9의 개수를 저장한 배열

	for (int i = 0; i < 10; i++) rtn[i] = 0; // rtn을 0으로 초기화
 
	while (n > 0) { 
		digit[k] = n % 10; // 맨 오른쪽 자릿수
		front[k] = n / 10; // digit[k] 왼쪽에 있는 수

		n /= 10; 	
		k++; // 자릿수를 한 칸 이동
	}

	for (int i = 0; i < 10; i++) { // 0~9까지 반복
		int mul = 1, back = 0; // 1의 자리부터 관찰

		for (int j = 0; j < k; j++) { // 10^j를 표현
			if (digit[j] < i) // 10^j의 계수가 i보다 작을 때
				rtn[i] += front[j] * mul;
			// 왼쪽에 올 수 있는 경우의 수 x 자릿수
			else if (digit[j] > i) // 10^j의 계수가 i보다 클 때
				rtn[i] += (front[j] + 1) * mul;
			// (왼쪽에 올 수 있는 경우의 수 + 1) x 자릿수
			else // 10^j의 계수가 i와 같을 때
				rtn[i] += front[j] * mul + back + 1;
			// if + 오른쪽에 올 수 있는 경우의 수 + 1

			back += digit[j] * mul;
			mul *= 10; // 자릿수를 한 칸 이동		
		}
	}

	int mul = 1; // 1의 자리부터 관찰
	for (int i = 0; i < k; i++) { // 10^i를 표현
		rtn[0] -= mul;
		mul *= 10; // 필요 없는 0을 제거 ex 00001 == 1
	}
	
	return rtn; // rtn을 반환
}

