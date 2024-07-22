/*

	ofxWinMenu basic example - ofApp.h

	Copyright (C) 2016-2017 Lynn Jarvis.

	http://www.spout.zeal.co

	=========================================================================
	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU Lesser General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
	=========================================================================
*/

#pragma once

#include "ofMain.h"
#include "ofxWinMenu.h" // Menu addon
#include <string.h>

#define MAX_HEIGHT 100 // 미로의 최대 높이
#define LENGTH 10 // 선 단위

class ofApp : public ofBaseApp {

public:

	void setup();
	void update();
	void draw();

	void keyPressed(int key); // Traps escape key if exit disabled
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
	bool readFile();
	void freeMemory();
	bool DFS();
	void dfsdraw();
	int HEIGHT;//미로의 높이
	int WIDTH;//미로의 너비
	char** input;//텍스트 파일의 모든 정보를 담는 이차원 배열이다.
	int** visited;//방문여부를 저장할 포인
	int maze_col;//미로칸의 열의 인덱스를 가리킨다.
	int maze_row;//미로칸의 행의 인덱스를 가리킨다.
	int k;
	int isOpen; //파일이 열렸는지를 판단하는 변수. 0이면 안열렸고 1이면 열렸다.
	int isDFS = false;
	int isBFS = false;
	// Menu
	ofxWinMenu* menu; // Menu object
	void appMenuFunction(string title, bool bChecked); // Menu return function

	// Used by example app
	ofTrueTypeFont myFont;
	ofImage myImage;
	float windowWidth, windowHeight;
	HWND hWnd; // Application window
	HWND hWndForeground; // current foreground window

	// Example menu variables
	bool bShowInfo;
	bool bFullscreen;
	bool bTopmost;
	bool isdfs;
	// Example functions
	void doFullScreen(bool bFull);
	void doTopmost(bool bTop);

	// 추가한 변수 및 함수
	int** graph; // 그래프
	int dir[4][2] = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}}; // 방향

	// 방
	typedef struct room {
		int row;
		int col;
	};

	// 이전 방을 저장하기 위한 2차원 배열
	room** pathDFS;
	room** pathBFS;

	// BFS
	bool BFS();
	void bfsdraw();

	// 노드
	struct node {
		room roomPos;
		struct node* link;
	};
	typedef struct node* nodePointer;

	// 스택, 큐 초기화
	nodePointer top = NULL;
	nodePointer front = NULL;
	nodePointer rear = NULL;
	nodePointer topPathDFS = NULL;
	nodePointer topPathBFS = NULL;
	
	// 스택 push
	void adds(nodePointer* top, int row, int col) {
		nodePointer temp = (nodePointer)malloc(sizeof(struct node));
		if (temp == NULL) return;

		temp->roomPos.row = row;
		temp->roomPos.col = col;
		temp->link = *top;
		*top = temp;
	}

	// 스택 pop
	room deletes(nodePointer* top) {
		nodePointer temp = *top;
		room item;

		if (*top == NULL) {
			item.row = -1;
			item.col = -1;
			return item;
		}

		item = temp->roomPos;
		*top = temp->link;
		free(temp);

		return item;
	}

	// 큐 enqueue
	void addq(nodePointer* front, nodePointer* rear, int row, int col) {
		nodePointer temp = (nodePointer)malloc(sizeof(struct node));
		if (temp == NULL) return;

		temp->roomPos.row = row;
		temp->roomPos.col = col;
		temp->link = NULL;
		if (*front) (*rear)->link = temp;
		else *front = temp;
		*rear = temp;
	}

	// 큐 dequeue
	room deleteq(nodePointer* front) {
		nodePointer temp = *front;
		room item;

		if (*front == NULL) {
			item.row = -1;
			item.col = -1;
			return item;
		}

		item = temp->roomPos;
		*front = temp->link;
		free(temp);

		return item;
	}

	// 프로젝트
	// 좌선법, 우선법을 구현하기 위한 구조체
	typedef struct pRoom {
		int dir; // 방향
		int row; // 행
		int col; // 열
	};

	room start, end; // start: 입구, end: 출구
	int countDFS; // DFS에서 방문 횟수
	int countBFS; // BFS에서 방문 횟수
	room game; // 게임에서 현재 위치
	int gameFlag = 0; // 게임 플래그, 1이면 게임 실행
	int pDir[4][2] = { {0, 1}, {1, 0}, {0, -1}, {-1, 0} }; // 방향, 동남서북
	int isLHOW = false; // 좌선법 플래그, 1이면 좌선법 수행
	int isRHOW = false; // 우선법 플래그, 1이면 우선법 수행
	room** pathLHOW; // 좌선법에서 이전 방을 저장하기 위한 2차원 배열
	room** pathRHOW; // 우선법에서 이전 방을 저장하기 위한 2차원 배열
	nodePointer topPathLHOW = NULL; // 좌선법에서 방문한 방을 저장하기 위한 스택
	nodePointer topPathRHOW = NULL; // 우선법에서 방문한 방을 저장하기 위한 스택

	bool LHOW(); // 좌선법 알고리즘
	bool RHOW(); // 우선법 알고리즘
	void drawLHOW(); // 좌선법으로 찾은 경로를 그리는 함수
	void drawRHOW(); // 우선법으로 찾은 경로를 그리는 함수
	void freePath(); // 스택, 큐를 동적 할당 해제하는 함수
};
