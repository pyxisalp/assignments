#pragma once

#include "ofMain.h"

class waterObj {
	public:
		double* x; // 경로 x 좌표 배열
		double* y; // 경로 y 좌표 배열
		int len; // 경로 길이
		waterObj(int size); // 생성자
		~waterObj(); // 소멸자
		void get_path(int *dot, int **lines, int num_of_line, int height); // 경로 찾기
};
