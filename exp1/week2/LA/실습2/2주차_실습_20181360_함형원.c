#include <stdio.h>

int main(void) {
	int i;
	double num;

	for (i = 0; i < 5; i++) {
		num = ((double)i)/2 + i;
		printf("num is %lf \n", num);
	}

	return 0;
}
