/*

	ofxWinMenu basic example - ofApp.cpp

	Example of using ofxWinMenu addon to create a menu for a Microsoft Windows application.

	Copyright (C) 2016-2017 Lynn Jarvis.

	https://github.com/leadedge

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

	03.11.16 - minor comment cleanup
	21.02.17 - rebuild for OF 0.9.8

*/
#include "ofApp.h"
#include <iostream>
using namespace std;

void ofApp::setup() {

	ofSetWindowTitle("Maze Example");
	ofSetFrameRate(15);
	ofBackground(255, 255, 255);

	windowWidth = ofGetWidth();
	windowHeight = ofGetHeight();

	/*
	isDFS = false;
	isBFS = false;
	isLHOW = false;
	isRHOW = false;
	*/

	isOpen = 0;

	ofSetWindowPosition((ofGetScreenWidth() - windowWidth) / 2, (ofGetScreenHeight() - windowHeight) / 2);

	myFont.loadFont("verdana.ttf", 12, true, true);

	hWnd = WindowFromDC(wglGetCurrentDC());

	ofSetEscapeQuitsApp(false);

	menu = new ofxWinMenu(this, hWnd);

	menu->CreateMenuFunction(&ofApp::appMenuFunction);

	HMENU hMenu = menu->CreateWindowMenu();

	HMENU hPopup = menu->AddPopupMenu(hMenu, "File");

	menu->AddPopupItem(hPopup, "Open", false, false);

	menu->AddPopupSeparator(hPopup);
	menu->AddPopupItem(hPopup, "Exit", false, false);

	hPopup = menu->AddPopupMenu(hMenu, "View");

	bShowInfo = true; 
	menu->AddPopupItem(hPopup, "Show DFS", false, false); // DFS 버튼
	bTopmost = false;
	menu->AddPopupItem(hPopup, "Show BFS", false, false); // BFS 버튼
	menu->AddPopupItem(hPopup, "Show Left Hand on Wall", false, false); // 좌선법 버튼
	menu->AddPopupItem(hPopup, "Show Right Hand on Wall", false, false); // 우선법 버튼
	bFullscreen = false;
	menu->AddPopupItem(hPopup, "Full screen", false, false);

	hPopup = menu->AddPopupMenu(hMenu, "Help");
	menu->AddPopupItem(hPopup, "About", false, false);

	menu->SetWindowMenu();

}

void ofApp::appMenuFunction(string title, bool bChecked) {

	ofFileDialogResult result;
	string filePath;
	size_t pos;

	//
	// File menu
	//
	if (title == "Open") {
		readFile();
	}
	if (title == "Exit") {
		ofExit(); // Quit the application
	}

	// Show DFS: DFS 알고리즘 수행
	if (title == "Show DFS") {
		//bShowInfo = bChecked;  // Flag is used elsewhere in Draw()
		if (isOpen)
		{
			// isDFS만 1로 설정
			isDFS = true;
			isBFS = false;
			isLHOW = false;
			isRHOW = false;
			DFS(); // DFS 알고리즘 수행
			bShowInfo = bChecked;
		}
		else
			cout << "you must open file first" << endl;

	}

	// Show BFS: BFS 알고리즘 수행
	if (title == "Show BFS") {
		doTopmost(bChecked); // Use the checked value directly

		if (isOpen)
		{
			// isBFS만 1로 설정
			isDFS = false;
			isBFS = true;
			isLHOW = false;
			isRHOW = false;
			BFS(); // BFS 알고리즘 수행
			bShowInfo = bChecked;
		}
		else
			cout << "you must open file first" << endl;
	}

	// Show Left Hand on Wall: 좌선법 알고리즘 수행
	if (title == "Show Left Hand on Wall") {
		doTopmost(bChecked); // Use the checked value directly

		if (isOpen)
		{
			// isLHOW만 1로 설정
			isDFS = false;
			isBFS = false;
			isLHOW = true;
			isRHOW = false;
			LHOW(); // 좌선법 알고리즘 수행
			bShowInfo = bChecked;
		}
		else
			cout << "you must open file first" << endl;
	}

	// Show Right Hand on Wall: 우선법 알고리즘 수행
	if (title == "Show Right Hand on Wall") {
		doTopmost(bChecked); // Use the checked value directly

		if (isOpen)
		{
			// isRHOW만 1로 설정
			isDFS = false;
			isBFS = false;
			isLHOW = false;
			isRHOW = true;
			RHOW(); // 우선법 알고리즘 수행
			bShowInfo = bChecked;
		}
		else
			cout << "you must open file first" << endl;
	}

	if (title == "Full screen") {
		bFullscreen = !bFullscreen; // Not auto-checked and also used in the keyPressed function
		doFullScreen(bFullscreen); // But als take action immediately
	}

	//
	// Help menu
	//
	if (title == "About") {
		ofSystemAlertDialog("ofxWinMenu\nbasic example\n\nhttp://spout.zeal.co");
	}

} // end appMenuFunction


//--------------------------------------------------------------
void ofApp::update() {

}


//--------------------------------------------------------------
void ofApp::draw() {

	char str[256];
	//ofBackground(0, 0, 0, 0);
	ofSetColor(100);
	ofSetLineWidth(5);
	int i, j;

	// TO DO : DRAW MAZE; 
	// 저장된 자료구조를 이용해 미로를 그린다.
	// add code here

	// 미로 그리기: ' ', '+' 생략, if 조건문에서 (i - 1) 또는 (j - 1)이 -1이 되는 경우 배제
	for (int i = 0; i < HEIGHT; i++) {
		for (int j = 0; j < WIDTH; j++) {
			if (input[i][j] == '-')
				ofDrawLine((j - 1) * LENGTH, i * LENGTH, (j + 1) * LENGTH, i * LENGTH);
			else if (input[i][j] == '|')
				ofDrawLine(j * LENGTH, (i - 1) * LENGTH, j * LENGTH, (i + 1) * LENGTH);
		}
	}
	// 입구, 출구 그리기, 입구: 노란색, 출구: 청록색
	if (isOpen) {
		ofSetColor(255, 255, 0);
		ofDrawRectangle((start.col - 1) * LENGTH, (start.row - 1) * LENGTH, LENGTH * 2, LENGTH * 2);
		ofSetColor(0, 255, 255);
		ofDrawRectangle((end.col - 1) * LENGTH, (end.row - 1) * LENGTH, LENGTH * 2, LENGTH * 2);
	}
	// 게임에서 현재 위치 그리기, 자홍색
	if (gameFlag) {
		ofSetColor(255, 0, 255);
		ofDrawRectangle((game.col - 1) * LENGTH, (game.row - 1) * LENGTH, LENGTH * 2, LENGTH * 2);
	}
	// DFS 경로 그리기
	if (isDFS) {
		ofSetLineWidth(5);
		if (isOpen)
			dfsdraw();
		else
			cout << "You must open file first" << endl;
	}
	// BFS 경로 그리기
	if (isBFS) {
		ofSetLineWidth(5);
		if (isOpen)
			bfsdraw();
		else
			cout << "You must open file first" << endl;
	}
	// 좌선법 경로 그리기
	if (isLHOW) {
		ofSetLineWidth(5);
		if (isOpen)
			drawLHOW();
		else
			cout << "You must open file first" << endl;
	}
	// 우선법 경로 그리기
	if (isRHOW) {
		ofSetLineWidth(5);
		if (isOpen)
			drawRHOW();
		else
			cout << "You must open file first" << endl;
	}
	if (bShowInfo) {
		// Show keyboard duplicates of menu functions
		ofSetColor(200);
		sprintf(str, " comsil project");
		myFont.drawString(str, 15, ofGetHeight() - 20);
	}

} // end Draw


void ofApp::doFullScreen(bool bFull)
{
	// Enter full screen
	if (bFull) {
		// Remove the menu but don't destroy it
		menu->RemoveWindowMenu();
		// hide the cursor
		ofHideCursor();
		// Set full screen
		ofSetFullscreen(true);
	}
	else {
		// return from full screen
		ofSetFullscreen(false);
		// Restore the menu
		menu->SetWindowMenu();
		// Restore the window size allowing for the menu
		ofSetWindowShape(windowWidth, windowHeight + GetSystemMetrics(SM_CYMENU));
		// Centre on the screen
		ofSetWindowPosition((ofGetScreenWidth() - ofGetWidth()) / 2, (ofGetScreenHeight() - ofGetHeight()) / 2);
		// Show the cursor again
		ofShowCursor();
		// Restore topmost state
		if (bTopmost) doTopmost(true);
	}

} // end doFullScreen


void ofApp::doTopmost(bool bTop)
{
	if (bTop) {
		// get the current top window for return
		hWndForeground = GetForegroundWindow();
		// Set this window topmost
		SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		ShowWindow(hWnd, SW_SHOW);
	}
	else {
		SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		ShowWindow(hWnd, SW_SHOW);
		// Reset the window that was topmost before
		if (GetWindowLong(hWndForeground, GWL_EXSTYLE) & WS_EX_TOPMOST)
			SetWindowPos(hWndForeground, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		else
			SetWindowPos(hWndForeground, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	}
} // end doTopmost


//--------------------------------------------------------------
void ofApp::keyPressed(int key) {

	// Escape key exit has been disabled but it can be checked here
	if (key == VK_ESCAPE) {
		// Disable fullscreen set, otherwise quit the application as usual
		if (bFullscreen) {
			bFullscreen = false;
			doFullScreen(false);
		}
		else {
			ofExit();
		}
	}

	// Remove or show screen info
	if (key == ' ') {
		bShowInfo = !bShowInfo;
		// Update the menu check mark because the item state has been changed here
		menu->SetPopupItem("Show DFS", bShowInfo);
	}

	if (key == 'f') {
		bFullscreen = !bFullscreen;
		doFullScreen(bFullscreen);
		// Do not check this menu item
		// If there is no menu when you call the SetPopupItem function it will crash
	}

	// i키 입력: 입구, 출구 설정
	if (key == 'i') {
		int r, c;

		printf("Entrance and Exit Settings\n");
		printf("The left top corner entry : 1 1\n");

		// 유효한 방을 입력할 때까지 반복
		while (1) {
			printf("Input an entry of an entrance: ");
			scanf("%d %d", &r, &c);
			if (r <= 0 || c <= 0 || r * 2 - 1 > HEIGHT - 2 || c * 2 - 1 > WIDTH - 2)
				printf("Invalid input\n");
			else {
				start.row = r * 2 - 1;
				start.col = c * 2 - 1;
				break;
			}
		}

		// 유효한 방을 입력할 때까지 반복
		while (1) {
			printf("Input an entry of an exit: ");
			scanf("%d %d", &r, &c);
			if (r <= 0 || c <= 0 || r * 2 - 1 > HEIGHT - 2 || c * 2 - 1 > WIDTH - 2)
				printf("Invalid input\n");
			else {
				end.row = r * 2 - 1;
				end.col = c * 2 - 1;
				break;
			}
		}

		// 알고리즘 다시 수행
		if (isDFS) DFS(); 
		if (isBFS) BFS();
		if (isLHOW) LHOW();
		if (isRHOW) RHOW();
	}

	// q키 입력: 게임 모드 on/off
	if (key == 'q') {
		gameFlag = 1 - gameFlag;
		if (gameFlag) {
			// 현재 위치를 입구로 설정
			game.row = start.row;
			game.col = start.col;
			printf("GAME START\n");
		}
	}
} // end keyPressed

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {
	// 게임 모드, 방향키 입력으로 현재 위치 이동, 벽이면 이동 불가
	if (gameFlag) {
		if (key == OF_KEY_UP && !graph[game.row - 1][game.col])
			game.row -= 2;
		else if (key == OF_KEY_DOWN && !graph[game.row + 1][game.col])
			game.row += 2;
		else if (key == OF_KEY_LEFT && !graph[game.row][game.col - 1])
			game.col -= 2;
		else if (key == OF_KEY_RIGHT && !graph[game.row][game.col + 1])
			game.col += 2;

		// 출구를 찾으면 게임 종료
		if (game.row == end.row && game.col == end.col) {
			gameFlag = 0;
			printf("CLEAR\n");
		}
	}
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {

}
bool ofApp::readFile()
{
	ofFileDialogResult openFileResult = ofSystemLoadDialog("Select .maz file");
	string filePath;
	size_t pos;
	// Check whether the user opened a file
	if (openFileResult.bSuccess) {
		ofLogVerbose("User selected a file");

		//We have a file, check it and process it
		string fileName = openFileResult.getName();
		//string fileName = "maze0.maz";
		printf("file name is %s\n", fileName);
		filePath = openFileResult.getPath();
		printf("Open\n");
		pos = filePath.find_last_of(".");
		if (pos != string::npos && pos != 0 && filePath.substr(pos + 1) == "maz") {

			ofFile file(fileName);

			if (!file.exists()) {
				cout << "Target file does not exists." << endl;
				return false;
			}
			else {
				cout << "We found the target file." << endl;
				isOpen = 1;
			}

			ofBuffer buffer(file);

			// Input_flag is a variable for indication the type of input.
			// If input_flag is zero, then work of line input is progress.
			// If input_flag is one, then work of dot input is progress.
			int input_flag = 0;

			// Idx is a variable for index of array.
			int idx = 0;

			// Read file line by line
			int cnt = 0;

			// TO DO
			// .maz 파일을 input으로 받아서 적절히 자료구조에 넣는다

			ofBuffer::Line it = buffer.getLines().begin();
			string line = *it;
			WIDTH = line.size(); // 미로의 너비 저장
			HEIGHT = 1; // 미로의 높이 저장
			it++;

			// input 동적 할당
			input = (char**)malloc(sizeof(char*) * MAX_HEIGHT);
			for (int i = 0; i < MAX_HEIGHT; i++)
				input[i] = (char*)malloc(sizeof(char) * WIDTH);

			strcpy(input[idx++], line.c_str()); // 입력 한 줄 복사
			for (ofBuffer::Line end = buffer.getLines().end(); it != end; it++, HEIGHT++) {
				string line = *it;
				strcpy(input[idx++], line.c_str()); // 입력 한 줄 라인 복
			}

			// 필요 없는 동적 할당 해제
			for (int i = idx; i < MAX_HEIGHT; i++)
				free(input[i]);

			// graph, visited 동적 할당
			// pathDFS, pathBFS 동적 할당
			graph = (int**)malloc(sizeof(int*) * HEIGHT);
			visited = (int**)malloc(sizeof(int*) * HEIGHT);
			pathDFS = (room**)malloc(sizeof(room*) * HEIGHT);
			pathBFS = (room**)malloc(sizeof(room*) * HEIGHT);
			pathLHOW = (room**)malloc(sizeof(room*) * HEIGHT);
			pathRHOW = (room**)malloc(sizeof(room*) * HEIGHT);
			for (int i = 0; i < HEIGHT; i++) {
				graph[i] = (int*)malloc(sizeof(int) * WIDTH);
				visited[i] = (int*)malloc(sizeof(int) * WIDTH);
				pathDFS[i] = (room*)malloc(sizeof(room) * WIDTH);
				pathBFS[i] = (room*)malloc(sizeof(room) * WIDTH);
				pathLHOW[i] = (room*)malloc(sizeof(room) * WIDTH);
				pathRHOW[i] = (room*)malloc(sizeof(room) * WIDTH);
			}
			
			// graph[i][j] = 1: 벽, = 0: 방, visited 초기화
			// pathDFS, pathBFS 초기화
			// pathLHOW, pathRHOW 초기화
			for (int i = 0; i < HEIGHT; i++) {
				for (int j = 0; j < WIDTH; j++) {
					if(input[i][j] == '+' || input[i][j] == '-' || input[i][j] == '|')
						graph[i][j] = 1;
					else
						graph[i][j] = 0;
					visited[i][j] = 0;
					pathDFS[i][j].row = -1;
					pathDFS[i][j].col = -1;
					pathBFS[i][j].row = -1;
					pathBFS[i][j].col = -1;
					pathLHOW[i][j].row = -1;
					pathLHOW[i][j].col = -1;
					pathRHOW[i][j].row = -1;
					pathRHOW[i][j].col = -1;
				}
			}

			// 기본 입구, 출구
			start.row = 1;
			start.col = 1;
			end.row = HEIGHT - 2;
			end.col = WIDTH - 2;

			// 알고리즘 플래그 초기화
			isDFS = false;
			isBFS = false;
			isLHOW = false;
			isRHOW = false;

			return true;
		}
		else {
			printf("  Needs a '.maz' extension\n");
			return false;
		}
	}
}

void ofApp::freeMemory() {
	nodePointer temp;

	// input, graph, visited 동적 할당 해제
	for (int i = 0; i < HEIGHT; i++) {
		free(input[i]);
		free(graph[i]);
		free(visited[i]);
		free(pathDFS[i]);
		free(pathBFS[i]);
	}
	free(input);
	free(graph);
	free(visited);
	free(pathDFS);
	free(pathBFS);

	// DFS 스택 동적 할당 해제
	while (top) {
		temp = top;
		top = top->link;
		free(temp);
	}

	// DFS 경로 동적 할당 해제
	while (topPathDFS) {
		temp = topPathDFS;
		topPathDFS = topPathDFS->link;
		free(temp);
	}

	// BFS 큐 동적 할당 해제
	while (front) {
		temp = front;
		front = front->link;
		free(temp);
	}

	// BFS 경로 동적 할당 해제
	while (topPathBFS) {
		temp = topPathBFS;
		topPathBFS = topPathBFS->link;
		free(temp);
	}

	// 좌선법 경로 동적 할당 해제
	while (topPathLHOW) {
		temp = topPathLHOW;
		topPathLHOW = topPathLHOW->link;
		free(temp);
	}

	// 우선법 경로 동적 할당 해제
	while (topPathRHOW) {
		temp = topPathRHOW;
		topPathRHOW = topPathRHOW->link;
		free(temp);
	}
}

bool ofApp::DFS() {
	room curr, next; // curr: 현재 방, next: 다음 방

	// visited, pathDFS 초기화
	for (int i = 0; i < HEIGHT; i++) {
		for (int j = 0; j < WIDTH; j++) {
			visited[i][j] = 0;
			pathDFS[i][j].row = -1;
			pathDFS[i][j].col = -1;
		}
	}
	
	freePath();

	top = NULL; // 스택 초기화
	topPathDFS = NULL; // 경로 스택 초기화
	adds(&top, start.row, start.col); // 입구 push
	countDFS = 0; // 방문 횟수 초기화

	while (top) {
		curr = deletes(&top); // pop

		if (visited[curr.row][curr.col]) continue; // 현재 방을 방문했으면 true
		visited[curr.row][curr.col] = 1; // 현재 방 방문
		adds(&topPathDFS, curr.row, curr.col); // 경로 push
		countDFS++; // 경로 길이 증가

		if (curr.row == end.row && curr.col == end.col) break; // 현재 방 == 출구

		for (int i = 0; i < 4; i++) {
			// 다음 방
			next.row = curr.row + dir[i][0];
			next.col = curr.col + dir[i][1];

			// 다음 방이 미로에 있고, 방문하지 않고, 벽이 아니면 true
			if (0 < next.row && next.row < HEIGHT - 1 &&
				0 < next.col && next.col < WIDTH - 1 &&
				!visited[next.row][next.col] &&
				!graph[next.row][next.col]) {
				adds(&top, next.row, next.col); // 다음 방 push
				// 다음 방의 이전 방 = 현재 방
				pathDFS[next.row][next.col].row = curr.row; 
				pathDFS[next.row][next.col].col = curr.col;
			}
		}
	}

	printf("# of visited rooms in DFS: %d\n", countDFS);

	return 0;
}

void ofApp::dfsdraw() {
	// 경로 스택 top부터 그리기
	nodePointer currNode = topPathDFS;
	room curr, prev = pathDFS[currNode->roomPos.row][currNode->roomPos.col];

	// 노드의 방과 이전 방 연결
	ofSetColor(0, 0, 255);
	while (currNode && prev.row != -1 && prev.col != -1) {
		ofDrawLine(currNode->roomPos.col * LENGTH, currNode->roomPos.row * LENGTH, prev.col * LENGTH, prev.row * LENGTH);
		currNode = currNode->link;
		prev = pathDFS[currNode->roomPos.row][currNode->roomPos.col];
	}

	// 출구부터 그리기
	curr.row = end.row;
	curr.col = end.col;
	prev = pathDFS[curr.row][curr.col];

	// 현재 방과 이전 방 연결
	ofSetColor(255, 0, 0);
	while (prev.row != -1 && prev.col != -1) {
		ofDrawLine(prev.col * LENGTH, prev.row * LENGTH, curr.col * LENGTH, curr.row * LENGTH);
		curr = prev;
		prev = pathDFS[prev.row][prev.col];
	}
}

bool ofApp::BFS() {
	room curr, next; // curr: 현재 방, next: 다음 방

	// visited, pathBFS 초기화
	for (int i = 0; i < HEIGHT; i++) {
		for (int j = 0; j < WIDTH; j++) {
			visited[i][j] = 0;
			pathBFS[i][j].row = -1;
			pathBFS[i][j].col = -1;
		}
	}

	freePath();

	// 큐 초기화
	front = NULL;
	rear = NULL;
	topPathBFS = NULL; // 경로 스택 초기화
	countBFS = 0; // 방문 횟수 초기화

	addq(&front, &rear, start.row, start.col); // 입구 enqueue

	while (front) {
		curr = deleteq(&front); // dequeue

		if (visited[curr.row][curr.col]) continue; // 현재 방을 방문했으면 true
		visited[curr.row][curr.col] = 1; // 현재 방 방문
		adds(&topPathBFS, curr.row, curr.col); // 경로 push
		countBFS++; // 경로 길이 증가

		if (curr.row == end.row && curr.col == end.col) break; // 현재 방 == 출구

		for (int i = 0; i < 4; i++) {
			// 다음 방
			next.row = curr.row + dir[i][0];
			next.col = curr.col + dir[i][1];

			// 다음 방이 미로에 있고, 방문하지 않고, 벽이 아니면 true
			if (0 < next.row && next.row < HEIGHT - 1 &&
				0 < next.col && next.col < WIDTH - 1 &&
				!visited[next.row][next.col] &&
				!graph[next.row][next.col]) {
				addq(&front, &rear, next.row, next.col); // 다음 방 enqueue
				// 다음 방의 이전 방 = 현재 방
				pathBFS[next.row][next.col].row = curr.row;
				pathBFS[next.row][next.col].col = curr.col;
			}
		}
	}

	printf("# of visited rooms in BFS: %d\n", countBFS);

	return 0;
}

void ofApp::bfsdraw() {
	// 경로 스택 top부터 그리기
	nodePointer currNode = topPathBFS;
	room curr, prev = pathBFS[currNode->roomPos.row][currNode->roomPos.col];

	// 노드의 방과 이전 방 연결
	ofSetColor(0, 0, 255);
	while (currNode && prev.row != -1 && prev.col != -1) {
		ofDrawLine(currNode->roomPos.col * LENGTH, currNode->roomPos.row * LENGTH, prev.col * LENGTH, prev.row * LENGTH);
		currNode = currNode->link;
		prev = pathBFS[currNode->roomPos.row][currNode->roomPos.col];
	}

	// 출구부터 그리기
	curr.row = end.row;
	curr.col = end.col;
	prev = pathBFS[curr.row][curr.col];

	// 현재 방과 이전 방 연결
	ofSetColor(255, 0, 0);
	while (prev.row != -1 && prev.col != -1) {
		ofDrawLine(prev.col * LENGTH, prev.row * LENGTH, curr.col * LENGTH, curr.row * LENGTH);
		curr = prev;
		prev = pathBFS[prev.row][prev.col];
	}
}

bool ofApp::LHOW() {
	pRoom curr, next1, next2; // curr: 현재 방, next: 다음 방

	// pathLHOW 초기화
	for (int i = 0; i < HEIGHT; i++) {
		for (int j = 0; j < WIDTH; j++) {
			pathLHOW[i][j].row = -1;
			pathLHOW[i][j].col = -1;
		}
	}

	freePath(); // 스택, 큐 동적 할당 해제

	topPathLHOW = NULL; // 경로 스택 초기화

	// 현재 위치를 입구로 설정
	curr.dir = 0;
	curr.row = start.row;
	curr.col = start.col;

	while (1) {
		adds(&topPathLHOW, curr.row, curr.col); // 경로 push

		if (curr.row == end.row && curr.col == end.col) break; // 현재 방 == 출구

		// 왼쪽으로 회전
		next1.dir = (curr.dir + 3) % 4;
		next1.row = curr.row + pDir[next1.dir][0];
		next1.col = curr.col + pDir[next1.dir][1];

		// 직진
		next2.dir = curr.dir;
		next2.row = curr.row + pDir[next2.dir][0];
		next2.col = curr.col + pDir[next2.dir][1];

		// 왼쪽으로 이동할 수 있으면 왼쪽으로 설정
		if (0 < next1.row && next1.row < HEIGHT - 1 &&
			0 < next1.col && next1.col < WIDTH - 1 &&
			!graph[next1.row][next1.col])
			curr.dir = next1.dir;
		// 왼쪽으로 이동할 수 없고, 직진할 수 없으면 오른쪽으로 설정
		else if (0 >= next2.row || next2.row >= HEIGHT - 1 ||
			0 >= next2.col || next2.col >= WIDTH - 1 ||
			graph[next2.row][next2.col])
			curr.dir = (curr.dir + 1) % 4;

		// 다음 위치가 유효하면 pathLHOW에 이전 방을 저장하고, 현재 위치를 다음 위치로 설정
		if (0 < curr.row + pDir[curr.dir][0] && curr.row + pDir[curr.dir][0] < HEIGHT - 1 &&
			0 < curr.col + pDir[curr.dir][1] && curr.col + pDir[curr.dir][1] < WIDTH - 1 &&
			!graph[curr.row + pDir[curr.dir][0]][curr.col + pDir[curr.dir][1]]) {
			pathLHOW[curr.row + pDir[curr.dir][0]][curr.col + pDir[curr.dir][1]].row = curr.row;
			pathLHOW[curr.row + pDir[curr.dir][0]][curr.col + pDir[curr.dir][1]].col = curr.col;

			curr.row += pDir[curr.dir][0];
			curr.col += pDir[curr.dir][1];
		}
	}

	return 0;
}

void ofApp::drawLHOW() {
	// 경로 스택 top부터 그리기
	nodePointer currNode = topPathLHOW;
	room curr, prev = pathLHOW[currNode->roomPos.row][currNode->roomPos.col];

	// 노드의 방과 이전 방 연결
	ofSetColor(0, 0, 255);
	while (currNode && prev.row != -1 && prev.col != -1) {
		ofDrawLine(currNode->roomPos.col * LENGTH, currNode->roomPos.row * LENGTH, prev.col * LENGTH, prev.row * LENGTH);
		currNode = currNode->link;
		prev = pathLHOW[currNode->roomPos.row][currNode->roomPos.col];
		if (prev.row == start.row && prev.col == start.col) break;
	}
}

bool ofApp::RHOW() {
	pRoom curr, next1, next2; // curr: 현재 방, next: 다음 방

	// pathRHOW 초기화
	for (int i = 0; i < HEIGHT; i++) {
		for (int j = 0; j < WIDTH; j++) {
			pathRHOW[i][j].row = -1;
			pathRHOW[i][j].col = -1;
		}
	}

	freePath(); // 스택, 큐 동적 할당 해제

	topPathRHOW = NULL; // 경로 스택 초기화

	// 현재 위치를 입구로 설정
	curr.dir = 0;
	curr.row = start.row;
	curr.col = start.col;

	while (1) {
		adds(&topPathRHOW, curr.row, curr.col); // 경로 push

		if (curr.row == end.row && curr.col == end.col) break; // 현재 방 == 출구

		// 왼쪽으로 회전
		next1.dir = (curr.dir + 1) % 4;
		next1.row = curr.row + pDir[next1.dir][0];
		next1.col = curr.col + pDir[next1.dir][1];

		// 직진
		next2.dir = curr.dir;
		next2.row = curr.row + pDir[next2.dir][0];
		next2.col = curr.col + pDir[next2.dir][1];

		// 오른쪽으로 이동할 수 있으면 오른쪽으로 설정
		if (0 < next1.row && next1.row < HEIGHT - 1 &&
			0 < next1.col && next1.col < WIDTH - 1 &&
			!graph[next1.row][next1.col])
			curr.dir = next1.dir;
		// 오른쪽으로 이동할 수 없고, 직진할 수 없으면 왼쪽으로 설정
		else if (0 >= next2.row || next2.row >= HEIGHT - 1 ||
			0 >= next2.col || next2.col >= WIDTH - 1 ||
			graph[next2.row][next2.col])
			curr.dir = (curr.dir + 3) % 4;

		// 다음 위치가 유효하면 pathRHOW에 이전 방을 저장하고, 현재 위치를 다음 위치로 설정
		if (0 < curr.row + pDir[curr.dir][0] && curr.row + pDir[curr.dir][0] < HEIGHT - 1 &&
			0 < curr.col + pDir[curr.dir][1] && curr.col + pDir[curr.dir][1] < WIDTH - 1 &&
			!graph[curr.row + pDir[curr.dir][0]][curr.col + pDir[curr.dir][1]]) {
			pathRHOW[curr.row + pDir[curr.dir][0]][curr.col + pDir[curr.dir][1]].row = curr.row;
			pathRHOW[curr.row + pDir[curr.dir][0]][curr.col + pDir[curr.dir][1]].col = curr.col;

			curr.row += pDir[curr.dir][0];
			curr.col += pDir[curr.dir][1];
		}
	}

	return 0;
}

void ofApp::drawRHOW() {
	// 경로 스택 top부터 그리기
	nodePointer currNode = topPathRHOW;
	room curr, prev = pathRHOW[currNode->roomPos.row][currNode->roomPos.col];

	// 노드의 방과 이전 방 연결
	ofSetColor(0, 0, 255);
	while (currNode && prev.row != -1 && prev.col != -1) {
		ofDrawLine(currNode->roomPos.col * LENGTH, currNode->roomPos.row * LENGTH, prev.col * LENGTH, prev.row * LENGTH);
		currNode = currNode->link;
		prev = pathRHOW[currNode->roomPos.row][currNode->roomPos.col];
		if (prev.row == start.row && prev.col == start.col) break;
	}
}

void ofApp::freePath() {
	nodePointer temp;

	// DFS 스택 동적 할당 해제
	while (top) {
		temp = top;
		top = top->link;
		free(temp);
	}

	// DFS 경로 동적 할당 해제
	while (topPathDFS) {
		temp = topPathDFS;
		topPathDFS = topPathDFS->link;
		free(temp);
	}

	// BFS 큐 동적 할당 해제
	while (front) {
		temp = front;
		front = front->link;
		free(temp);
	}

	// BFS 경로 동적 할당 해제
	while (topPathBFS) {
		temp = topPathBFS;
		topPathBFS = topPathBFS->link;
		free(temp);
	}

	// 좌선법 경로 동적 할당 해제
	while (topPathLHOW) {
		temp = topPathLHOW;
		topPathLHOW = topPathLHOW->link;
		free(temp);
	}

	// 우선법 경로 동적 할당 해제
	while (topPathRHOW) {
		temp = topPathRHOW;
		topPathRHOW = topPathRHOW->link;
		free(temp);
	}

	// 스택, 큐 NULL 초기화
	top = NULL;
	front = NULL;
	rear = NULL;
	topPathDFS = NULL;
	topPathBFS = NULL;
	topPathLHOW = NULL;
	topPathRHOW = NULL;
}