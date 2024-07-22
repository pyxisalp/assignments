#include "waterObj.h"

waterObj::waterObj(int size) {
	// x, y 동적 할당
	x = new double[size];
	y = new double[size];
	len = 0; // 경로 길이 0으로 초기화
}

waterObj::~waterObj() {
	// x, y 동적 할당 해제
	delete[] x;
	delete[] y;
}

void waterObj::get_path(int* dot, int** lines, int num_of_line, int height) {
	int i = 0; // x, y 인덱스

	// 선택한 점의 좌표 저장
	x[0] = dot[0];
	y[0] = dot[1]; 

	while (y[i] < height) {
		int j = 0; // 점과 선분의 거리가 최소인 인덱스
		int min = height; // 점과 선분의 최소 거리
		double min_intersection = height; // 점과 선분의 거리가 최소일 때, 점의 x 좌표를 선분에 대입한다.
		double intersection = height; // 점의 x 좌표를 선분에 대입한다.

		for (int k = 0; k < num_of_line; k++) {
			// 선분의 기울기
			double slope = double((lines[k][3] - lines[k][1])) / double((lines[k][2] - lines[k][0]));
			// 점의 x 좌표가 선분의 x 좌표 범위에 있고, 점이 선분의 위에 존재할 때 TRUE
			if ((x[i] > lines[k][0]) && (x[i] < lines[k][2]) &&
				(y[i] < slope * (x[i] - lines[k][0]) + lines[k][1])) {
				intersection = slope * (x[i] - lines[k][0]) + lines[k][1];
				// 계산한 intersection - y[i]가 min보다 작으면 TRUE
				if (intersection - y[i] < min) {
					// 점과 선분의 최소 거리에 대한 정보 갱신
					min = intersection - y[i];
					min_intersection = intersection;
					j = k;
				}
			}
		}

		// for 반복문의 if 조건문을 거치지 않으면 TRUE
		if (min_intersection >= height) {
			// 윈도우의 하단을 마지막 경로로 추가
			x[i + 1] = x[i];
			y[i + 1] = height - 50.0;
			i++;
			break;
		}

		// 선분의 위를 경로로 추가
		x[i + 1] = x[i];
		y[i + 1] = min_intersection;
		i++;

		// 선분의 낮은 곳을 경로로 추가
		// 윈도우의 y 좌표가 클수록 아래로 내려간다.
		if (lines[j][1] > lines[j][3]) {
			x[i + 1] = lines[j][0];
			y[i + 1] = lines[j][1];
		}
		else {
			x[i + 1] = lines[j][2];
			y[i + 1] = lines[j][3];
		}
		i++;
	}

	len = i; // 경로 길이 설정
}