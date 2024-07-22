#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct _room {
	int row;
	int col;
	struct _room *next;
} room;

int width, height;
int **maze;
room **roomNum;

void merge(int before, int after) {
	room *curr, *tail;
	curr = roomNum[before];
	tail = roomNum[after];

	while (curr->next) {
		maze[curr->row][curr->col] = after;
		curr = curr->next;
	}

	while (tail->next)
		tail = tail->next;

	maze[curr->row][curr->col] = after;
	tail->next = roomNum[before];
//	roomNum[after] = roomNum[before];
//	roomNum[before]->next = NULL;
}

void makeMaze(void) {
//	srand(time(NULL));
	int i, j, num = 1, deleteFlag, linkFlag;
	room* temp;
	// num:  방 번호, deleteFlag: 1이면 벽 제거, linkFlag: 1이면 연결 완료

	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j++)
			if (!maze[i * 2][j * 2]) { // 방 번호가 0이면 방 번호 지정
				maze[i * 2][j * 2] = num;
				temp = (room*)malloc(sizeof(room));
				temp->row = i * 2;
				temp->col = j * 2;
				temp->next = NULL;
				roomNum[num++] = temp;
			}

		for (j = 0; j < width - 1; j++) {
			if (maze[i * 2][j * 2] != maze[i * 2][(j + 1) * 2]) {
				deleteFlag = rand() % 2;
				if (deleteFlag) {
					maze[i * 2][j * 2 + 1] = -1; // 벽 제거
					merge(maze[i * 2][(j + 1) * 2], maze[i * 2][j * 2]);
//					maze[i * 2][(j + 1) * 2] = maze[i * 2][j * 2]; // 같은 집합
				}
				else if (i == height - 1) {
					maze[i * 2][j * 2 + 1] = -1;
					merge(maze[i * 2][(j + 1) * 2], maze[i * 2][j * 2]);
//					maze[i * 2][(j + 1) * 2] = maze[i * 2][j * 2];
				}
			}
		}

		if (i == height - 1)
			break;

		linkFlag = 0;
		for (j = 0; j < width; j++) {
			deleteFlag = rand() % 2;
			if (deleteFlag) {
				if (linkFlag && maze[(i + 1) * 2][(j - 1) * 2] == maze[i * 2][(j - 1) * 2] && maze[(i + 1) * 2][(j - 1) * 2] == maze[i * 2][j * 2])
					;
				else {
				maze[i * 2 + 1][j * 2] = -1; // 벽 제거
				temp = (room*)malloc(sizeof(room));
				temp->row = (i + 1) * 2;
				temp->col = j * 2;
				temp->next = roomNum[maze[i * 2][j * 2]];
				roomNum[maze[i * 2][j * 2]] = temp;
				maze[(i + 1) * 2][j * 2] = maze[i * 2][j * 2]; // 같은 집합
				linkFlag = 1;
//				if (maze[i * 2][j * 2] != maze[i * 2][(j + 1) * 2])
//					linkFlag = 0;
				}
			}

			if (j == width - 1 || maze[i * 2][j * 2] != maze[i * 2][(j + 1) * 2]) {
				if (!linkFlag) {
					maze[i * 2 + 1][j * 2] = -1;
					temp = (room*)malloc(sizeof(room));
					temp->row = (i + 1) * 2;
					temp->col = j * 2;
					temp->next = roomNum[maze[i * 2][j * 2]];
					roomNum[maze[i * 2][j * 2]] = temp;
					maze[(i + 1) * 2][j * 2] = maze[i * 2][j * 2];
				}	
				linkFlag = 0;
			}
			if (maze[i * 2][j * 2] != maze[i * 2][(j + 1) * 2])
				linkFlag = 0;
		}
	}
}

int main(void) {
	int i, j;
	FILE *fp = fopen("20181360.maz", "w"); // 파일 쓰기
	if (fp == NULL)
		return 1;

	scanf("%d", &width); // 너비 입력
	scanf("%d", &height); // 높이 입력

	maze = (int**)malloc(sizeof(int*) * (height * 2 - 1));
	for (i = 0; i < height * 2 - 1; i++) {
		maze[i] = (int*)malloc(sizeof(int) * (width * 2 - 1));
		memset(maze[i], 0, sizeof(int) * (width * 2 - 1));
	}

	roomNum = (room**)malloc(sizeof(room*) * height * width);

	makeMaze();

//	for (int i = 0; i < height; i++) {
//		for (int j = 0; j < width; j++)
//			printf("%2d ", maze[i * 2][j * 2]);
//		printf("\n");
//	}

	for (j = 0; j < width * 2 + 1; j++) {
		if (j % 2 == 1)
			fprintf(fp, "-");
		else
			fprintf(fp, "+");
	}
	fprintf(fp, "\n");

	for (i = 0; i < height * 2 - 1; i++) {
		if (i % 2 == 1)
			fprintf(fp, "+");
		else
			fprintf(fp, "|");

		for (j = 0; j < width * 2 - 1; j++) {
			if (i % 2 == 1 && j % 2 == 1)
				fprintf(fp, "+");
			else if (maze[i][j])
				fprintf(fp, " ");
			else if (!maze[i][j]) {
				if (i % 2 == 1)
					fprintf(fp, "-");
				else if (j % 2 == 1)
					fprintf(fp, "|");
			}
		}

		if (i % 2 == 1)
			fprintf(fp, "+");
		else
			fprintf(fp, "|");

		fprintf(fp, "\n");
	}

	for (j = 0; j < width * 2 + 1; j++) {
		if (j % 2 == 1)
			fprintf(fp, "-");
		else
			fprintf(fp, "+");
	}
	fprintf(fp, "\n");

	return 0;
}
