#include "tetris.h"

static struct sigaction act, oact;
int count, score_number;
int maxScore;
Node *head; // 연결 리스트 head
RecNode *recHead; // 트리 root
time_t start, stop;
double duration;
long size;

int main(){
	int exit=0;

	initscr();
	createRankList(); // 연결 리스트 생성
	noecho();
	keypad(stdscr, TRUE);	

	srand((unsigned int)time(NULL));

	while(!exit){
		clear();
		switch(menu()){
		case MENU_PLAY: play(); break;
		case MENU_RANK: rank(); break;
		case MENU_REC_PLAY: recommendedPlay(); break;
		case MENU_EXIT: exit=1; break;
		default: break;
		}
	}

	endwin();
	system("clear");
	return 0;
}

void InitTetris(){
	int i,j;

	for(j=0;j<HEIGHT;j++)
		for(i=0;i<WIDTH;i++)
			field[j][i]=0;

	// nextBlock 초기화
	for (i = 0; i < VISIBLE_BLOCKS; i++)
		nextBlock[i] = rand() % 7;

	recHead = (RecNode*)malloc(sizeof(struct _RecNode));
	if (recHead == NULL)
		return;

	recHead->lv = 0;
	recHead->score = 0;
	recHead->parent = NULL;
	
	createTree(recHead, 1); // 트리 생성

	// 트리 head 초기화
	recHead->id = nextBlock[0];
	for (i = 0; i < HEIGHT; i++)
		recHead->f[i] = 0;

	maxScore = 0;
//	recommend(recHead);
	modified_recommend(recHead);

	blockRotate=0;
	blockY=-1;
	blockX=WIDTH/2-2;
	score=0;	
	gameOver=0;
	timed_out=0;
	
	DrawOutline();
	DrawField();
	// 1주차 과제
	DrawBlockWithFeatures(blockY, blockX, nextBlock[0], blockRotate);
	DrawNextBlock(nextBlock);
	PrintScore(score);
}

void DrawOutline(){	
	int i,j;
	/* 블럭이 떨어지는 공간의 태두리를 그린다.*/
	DrawBox(0,0,HEIGHT,WIDTH);

	/* next block을 보여주는 공간의 태두리를 그린다.*/
	move(2,WIDTH+10);
	printw("NEXT BLOCK");
	DrawBox(3,WIDTH+10,4,8);
	
	// 다음 다음 블록 테두리
	DrawBox(9, WIDTH + 10, 4, 8);

	// 점수 테두리, 실습 위치 수정
	move(15, WIDTH + 10);
	printw("SCORE");
	DrawBox(16, WIDTH + 10, 1, 8);
}

int GetCommand(){
	int command;
	command = wgetch(stdscr);
	switch(command){
	case KEY_UP:
		break;
	case KEY_DOWN:
		break;
	case KEY_LEFT:
		break;
	case KEY_RIGHT:
		break;
	case ' ':	/* space key*/
		/*fall block*/
		break;
	case 'q':
	case 'Q':
		command = QUIT;
		break;
	default:
		command = NOTHING;
		break;
	}
	return command;
}

int ProcessCommand(int command){
	int ret=1;
	int drawFlag=0;
	switch(command){
	case QUIT:
		ret = QUIT;
		break;
	case KEY_UP:
		if((drawFlag = CheckToMove(field,nextBlock[0],(blockRotate+1)%4,blockY,blockX)))
			blockRotate=(blockRotate+1)%4;
		break;
	case KEY_DOWN:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY+1,blockX)))
			blockY++;
		break;
	case KEY_RIGHT:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX+1)))
			blockX++;
		break;
	case KEY_LEFT:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX-1)))
			blockX--;
		break;
	default:
		break;
	}
	if(drawFlag) DrawChange(field,command,nextBlock[0],blockRotate,blockY,blockX);
	return ret;	
}

void DrawField(){
	int i,j;
	for(j=0;j<HEIGHT;j++){
		move(j+1,1);
		for(i=0;i<WIDTH;i++){
			if(field[j][i]==1){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(".");
		}
	}
}


void PrintScore(int score){
	// 점수 출력, 위치 수정
	move(17,WIDTH + 11);
	printw("%8d", score);
}

void DrawNextBlock(int *nextBlock){
	int i, j;

	// 다음 블록 그리기
	for (i = 0; i < 4; i++) {
		move(4 + i, WIDTH + 13);
		for (j = 0; j < 4; j++) {
			if (block[nextBlock[1]][0][i][j]) {
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(" ");
		}	
	}
	
	// 다음 다음 블록 그리기
	for (i = 0; i < 4; i++) {
		move(10 + i, WIDTH + 13);
		for (j = 0; j < 4; j++) {
			if (block[nextBlock[2]][0][i][j]) {
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(" ");
		}
	}
}

void DrawBlock(int y, int x, int blockID,int blockRotate,char tile){
	int i,j;
	for(i=0;i<4;i++)
		for(j=0;j<4;j++){
			if(block[blockID][blockRotate][i][j]==1 && i+y>=0){
				move(i+y+1,j+x+1);
				attron(A_REVERSE);
				printw("%c",tile);
				attroff(A_REVERSE);
			}
		}

	move(HEIGHT,WIDTH+10);
}

void DrawBox(int y,int x, int height, int width){
	int i,j;
	move(y,x);
	addch(ACS_ULCORNER);
	for(i=0;i<width;i++)
		addch(ACS_HLINE);
	addch(ACS_URCORNER);
	for(j=0;j<height;j++){
		move(y+j+1,x);
		addch(ACS_VLINE);
		move(y+j+1,x+width+1);
		addch(ACS_VLINE);
	}
	move(y+j+1,x);
	addch(ACS_LLCORNER);
	for(i=0;i<width;i++)
		addch(ACS_HLINE);
	addch(ACS_LRCORNER);
}

void play(){
	int command;
	clear();
	act.sa_handler = BlockDown;
	sigaction(SIGALRM,&act,&oact);
	InitTetris();
	do{
		if(timed_out==0){
			alarm(1);
			timed_out=1;
		}

		command = GetCommand();
		if(ProcessCommand(command)==QUIT){
			alarm(0);
			DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
			move(HEIGHT/2,WIDTH/2-4);
			printw("Good-bye!!");
			refresh();
			getch();

			return;
		}
	}while(!gameOver);

	alarm(0);
	getch();
	DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
	move(HEIGHT/2,WIDTH/2-4);
	printw("GameOver!!");
	refresh();
	getch();
	newRank(score);
}

char menu(){
	printw("1. play\n");
	printw("2. rank\n");
	printw("3. recommended play\n");
	printw("4. exit\n");
	return wgetch(stdscr);
}

/////////////////////////첫주차 실습에서 구현해야 할 함수/////////////////////////

int CheckToMove(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
	// 테트리스 블록 4x4
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (block[currentBlock][blockRotate][i][j]) {
				// 좌표가 범위를 벗어나거나 필드가 1일 때 만족
				if ((i + blockY) < 0 || (i + blockY) >= HEIGHT || (j + blockX) < 0 || (j + blockX) >= WIDTH || f[i + blockY][j + blockX])
					return 0; // 이동할 수 없음
			}
		}
	}
	return 1; // 이동할 수 있음
}

void DrawChange(char f[HEIGHT][WIDTH],int command,int currentBlock,int blockRotate, int blockY, int blockX){
	// prevBlockRotate: 블록의 이전 회전
	// prevBlockY: 블록의 이전 y 좌표, prevBlockX: 블록의 이전 x 좌표
	int i, j;
	int prevBlockRotate = blockRotate, prevBlockY = blockY, prevBlockX = blockX;
	
	// 블록의 이전 정보 역산
	switch(command) {
		case QUIT:
			break;
		case KEY_UP:
			prevBlockRotate = (prevBlockRotate + 3) % 4;
			break;
		case KEY_DOWN:
			prevBlockY--;
			break;
		case KEY_RIGHT:
			prevBlockX--;
			break;
		case KEY_LEFT:
			prevBlockX++;
			break;
		default:
			break;
	}

	// 이전 블록 지우기
	// DrawBlock에서 A_REVERSE 대신 A_NORMAL 사용
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			if (block[currentBlock][prevBlockRotate][i][j] && i + prevBlockY >= 0) {
				move(i + prevBlockY + 1, j + prevBlockX + 1);
				attron(A_NORMAL); // A_NORMAL: normal display
				printw(".");
				attroff(A_NORMAL);
			}
		}
	}
	
	move(HEIGHT, WIDTH + 10);

	// 이전 그림자 지우기
	for (; CheckToMove(field, nextBlock[0], prevBlockRotate, prevBlockY, prevBlockX); prevBlockY++);

	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			if (block[currentBlock][prevBlockRotate][i][j] && i + prevBlockY >= 1) {
				move(i + prevBlockY, j + prevBlockX + 1);
				attron(A_NORMAL);
				printw(".");
				attroff(A_NORMAL);
			}
		}
	}

	move(HEIGHT, WIDTH + 10);


	// 이전 추천 지우기
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			if (block[currentBlock][recHead->rotate][i][j] && i + recHead->y >= 0) {
				move(i + recHead->y + 1, j + recHead->x + 1);
				attron(A_NORMAL); // A_NORMAL: normal display
				printw(".");
				attroff(A_NORMAL);
			}
		}
	}

	move(HEIGHT, WIDTH + 10);	

	// 현재 블록 그리기
	DrawBlockWithFeatures(blockY, blockX, currentBlock, blockRotate);
}

void BlockDown(int sig){
	int i, j;
	// 블록을 한 칸 내릴 수 있는지 확인
	if (CheckToMove(field, nextBlock[0], blockRotate, blockY + 1, blockX)) {
		// 블록 다시 그리기
		DrawChange(field, KEY_DOWN, nextBlock[0], blockRotate, ++blockY, blockX);
		timed_out = 0; // 시간 초기화
		return;
	}

	// 블록이 맨 위에서 움직이지 않았을 때 만족
	if (blockY == -1)
		gameOver = 1; // 게임오버

	// 필드에 블록 추가, 제거, 점수 합산
	score += (AddBlockToField(field, nextBlock[0], blockRotate, blockY, blockX) + DeleteLine(field));
	PrintScore(score); // 점수 출력

	// 블록 아이디 갱신
	for (i = 0; i < VISIBLE_BLOCKS - 1; i++)
		nextBlock[i] = nextBlock[i + 1];
	nextBlock[VISIBLE_BLOCKS - 1] = rand() % 7;
	DrawNextBlock(nextBlock); // 다음 블록 그리기


	recHead->id = nextBlock[0];
	for (i = 0; i < HEIGHT; i++) {
		recHead->f[i] = 0;
		for (j = 0; j < WIDTH; j++)
			recHead->f[i] |= (field[i][j] << j);
	}

	maxScore = 0;
//	recommend(recHead);
	modified_recommend(recHead);

	// 블록 정보 초기화
	blockRotate = 0;
	blockY = -1;
	blockX = WIDTH / 2 - 2;
	timed_out = 0; // 시간 초기화

	DrawField(); // 필드 그리기
}

int AddBlockToField(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
	// cnt: 접촉 면적, rtn: 리턴
	int cnt = 0, rtn = 0;

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			// 블록이 있는 필드를 1로 저장
			if (block[currentBlock][blockRotate][i][j]) {
				f[blockY + i][blockX + j] = 1;
				// early termination
				// 블록이 바닥에 있거나 블록 아래 필드가 있을 때 만족
				if ((blockY + i + 1) == HEIGHT || f[blockY + i + 1][blockX + j])
					cnt++;
			}
		}
	}
	rtn = cnt * 10;

	return rtn;
}

int DeleteLine(char f[HEIGHT][WIDTH]){
	// cnt: 지운 라인의 수, rtn: 리턴
	// delete_flag가 1이면 해당 라인의 필드가 모두 1, 아니면 0
	int cnt = 0, rtn = 0, delete_flag;

	for (int i = 0; i < HEIGHT; i++) {
		delete_flag = 1; // delete_flag 초기화
		for (int j = 0; j < WIDTH; j++) {
			// 필드가 0이면 만족
			if (!f[i][j]) {
				delete_flag = 0;
				break;
			}
		}

		if (!delete_flag) continue;

		cnt++; // 지운 라인의 수 증가
		// 지운 라인 위의 모든 정보를 한 칸 아래로 이동
		for (int p = i; p >= 1; p--) {
			for (int q = 0; q < WIDTH; q++)
				f[p][q] = f[p - 1][q];
		}
		for (int q = 0; q < WIDTH; q++)
			f[0][q] = 0;
	}
	rtn = (cnt * cnt) * 100; // 점수 공식

	return rtn;	
}

// 1주차 과제
void DrawShadow(int y, int x, int blockID,int blockRotate){
	// 그림자의 좌표
	int shadowY = y, shadowX = x;

	for (; CheckToMove(field, nextBlock[0], blockRotate, shadowY, shadowX); shadowY++);

	DrawBlock(--shadowY, shadowX, nextBlock[0], blockRotate, '/');
}

void DrawBlockWithFeatures(int y, int x, int blockID, int blockRotate) {
	// 블록 레이아웃이 그림자 레이아웃보다 앞
	DrawShadow(y, x, blockID, blockRotate);	
	DrawRecommend(recHead->y, recHead->x, recHead->id, recHead->rotate);
	DrawBlock(y, x, blockID, blockRotate, ' ');
}

// 2주차
void createRankList(){
	FILE *fp;
	int tempScore; // temp 점수
	char tempName[NAMELEN]; // temp 이름
	Node *temp, *curr; // temp: 임시 노드, curr: 연결 리스트 현재 노드

	fp = fopen("rank.txt", "r");
	if (fp == NULL)
		return;

	score_number = 0; // score_number 초기화
	head = NULL; // 연결 리스트 초기화
	
	// 개수 입력
	if (fscanf(fp, "%d", &score_number) != EOF) {
		// 이름, 점수 입력
		while (fscanf(fp, "%s %d", tempName, &tempScore) != EOF) {
			temp = (Node*)malloc(sizeof(struct _Node));
			if (temp == NULL)
				return;

			temp->score = tempScore; // 점수 저장
			strcpy(temp->name, tempName); // 이름 저장
			temp->link = NULL; // 연결 리스트 마지막 노드

			// 연결 리스트에 노드가 없을 때
			if (!head) {
				head = temp; // head 갱신
				curr = head; // curr 갱신
				continue;
			}
			
			// 연결 리스트에 노드가 있을 때
			curr->link = temp; // curr, temp 연결
			curr = temp; // curr 갱신
		}
	}

	fclose(fp);
}

void rank(){
	int X = 1, Y = score_number, ch;
	clear();

	// 메뉴 출력
	printw("1. list ranks from X to Y\n");
	printw("2. list ranks by a specific name\n");
	printw("3. delete a specific rank\n");

	noecho(); // 입력 표시 X
	ch = wgetch(stdscr); // 메뉴 입력

	if (ch == '1') {
		Node *curr = head; // 연결 리스트 현재 노드

		// 입력이 두 자릿수 이상일 수 있으므로 wgetch 대신 scanw 사용
		echo(); // 입력 표시 O
		printw("X: ");
		scanw("%d", &X); // X 입력
		printw("Y: ");
		scanw("%d", &Y); // Y 입력
		noecho(); // 입력 표시 X

		printw("      name      |   score   \n");
		printw("----------------------------\n");

		// 비정상 입력
		if (X <= 0 || Y <= 0 || X > score_number || Y > score_number || X > Y) {
			printw("\nsearch failure: no rank in the list\n");
			getch();
			return;
		}

		count = 0; // count 초기화
		while (1) {
			count++;
			// X <= count <= Y
			if (count >= X && count <= Y)
				printw(" %-15s| %d\n", curr->name, curr->score);
			else {
				// count > Y 또는 연결 리스트 이탈
				if (count > Y || !curr)
					break;
			}
			curr = curr->link; // 다음 노드 이동
		}
	
	}

	else if ( ch == '2') {
		char str[NAMELEN+1];
		int check = 0;
		Node *curr = head; // 연결 리스트 현재 노드

		echo(); // 입력 표시 O
		printw("input the name: ");
		scanw("%s", str);
		noecho(); // 입력 표시 X

		printw("      name      |   score   \n");
		printw("----------------------------\n");
		
		while (curr) {
			// str == curr->name이면 0 리턴
			if (!strcmp(str, curr->name)) {
				printw(" %-15s| %d\n", curr->name, curr->score);
				check++;
			}
			curr = curr->link; // 다음 노드 이동
		}
		
		// check가0이면 검색 실패
		if (!check)
			printw("\nsearch failure: no name in the list\n");
	}

	else if ( ch == '3') {
		int num, currRank = 1; // num: key rank, currRank: 현재 노드 랭크
		// temp: 임시 노드, prev: 이전 노드, curr: 현재 노드
		Node *temp, *prev = NULL, *curr = head;

		echo(); // 입력 표시 O
		printw("input the rank: ");
		scanw("%d", &num);
		noecho(); // 입력 표시 X

		// 비정상 입력 또는 NULL head
		if (num <= 0 || num > score_number || !head) {
			printw("\nsearch failure: the rank not in the list\n");
			getch();
			return;
		}

		while (curr) {
			if (num == currRank) {
				temp = curr;
				// 첫 번째 노드 삭제
				if (num == 1)
					head = curr->link; // 노드가 1개일 때도 성립
				// 마지막 노드 삭제
				else if (num == score_number)
					prev->link = NULL;
				// 중간 노드 삭제
				else
					prev->link = curr->link; // 이전, 다음 노드 연결

				free(temp); // 동적 할당 해제
				score_number--; // 노드 개수 감소
				writeRankFile(); // rank.txt 갱신

				printw("\nresult: the rank deleted\n");
					break;
			}
			prev = curr; // 현재 노드 이동
			curr = curr->link; // 다음 노드 이동
			currRank++; // 현재 노드 랭크 증가
		}
	}

	getch();
}

void writeRankFile(){
	Node *curr; // 현재 노드
	FILE *fp = fopen("rank.txt", "w"); // 파일 쓰기
	if (fp == NULL)
		return;

	fprintf(fp, "%d\n", score_number); // 노드 개수 쓰기

	curr = head;
	while (curr) {
		fprintf(fp, "%s %d\n", curr->name, curr->score); // 노드 이름, 점수 쓰기
		curr = curr->link; // 다음 노드 이동
	}

	fclose(fp); // 파일 쓰기 종료
}

void newRank(int score){
	// 목적: GameOver시 호출되어 사용자 이름을 입력받고 score와 함께 리스트의 적절한 위치에 저장
	char str[NAMELEN+1];
	int i, j;
	Node *temp, *prev, *curr; // temp: 임시 노드, prev: 이전 노드, curr: 현재 노드
	clear();

	echo(); // 입력 표시 O
	printw("your name: ");
	scanw("%s", str);
	noecho(); // 입력 표시 X

	temp = (Node*)malloc(sizeof(struct _Node));
	if (temp == NULL)
		return;
	temp->score = score; // temp 노드에 점수 저장
	strcpy(temp->name, str); // temp 노드에 이름 저장

	// score_number가 0이면 NULL head
	if(!score_number) {
		temp->link = NULL; // 마지막 노드
		head = temp; // head 갱신
		writeRankFile();
		return;
	}

	prev = NULL;
	curr = head;

	// 다음 노드가 NULL이거나 temp 노드 점수보다 클 때
	while (curr->link && score < curr->score) {
		prev = curr; // 현재 노드 이동
		curr = curr->link; // 다음 노드 이동
	}

	// while 반복문을 통과한 curr은 마지막 노드 또는 temp 노드 점수보다 작거나 같은 노드
	if (score >= curr->score) {
		temp->link = curr; // 현재 노드 왼쪽에 삽입
		// 현재 노드가 첫 번째 노드
		if (head == curr)
			head = temp; // head 갱신
		else {
			prev->link = temp; // 이전, temp 노드 연결
			temp->link = curr; // temp, 현재 노드 연결
		}
	}
	else {
		curr->link = temp; // 현재 노드 오른쪽에 삽입
		temp->link = NULL; // 마지막 노드
	}
		
	score_number++; // 노드 개수 증가
	
	writeRankFile();
}

void DrawRecommend(int y, int x, int blockID, int blockRotate){
	DrawBlock(y, x, blockID, blockRotate, 'R');
}


/*
int recommend(RecNode *root){
	int p, q, idx = 0;
	RecNode *first;
	int level = root->lv;

	// 재귀 함수 종료
	if (root->lv == VISIBLE_BLOCKS) {
		// 현재 노드의 점수가 최대 점수보다 클 때
		if (root->score > maxScore) {
			maxScore = root->score;

			// level이 1인 노드 찾기
			first = root;
			while (first->lv != 1)
				first = first->parent;

			// root에 정보 저장
			recHead->rotate = first->rotate;
			recHead->x = first->x;
			recHead->y = first->y;
		}
		return 0;
	}

	// 블록의 유효한 회전
	for (int k = 0; k < effRotation[nextBlock[root->lv]]; k++) {
		int xLen = xLength[nextBlock[root->lv]][k % 2]; // 블록의 x 길이

		// 블록을 쌓을 수 있는 위치
		for (int j = 0; j <= WIDTH - xLen; j++, idx++) {
			// parent 필드 복사
			for (p = 0; p < HEIGHT; p++)
				for (q = 0; q < WIDTH; q++)
					root->c[idx]->f[p][q] = root->f[p][q];

			// 그림자 생성 위치
			int i = 0;
			for (; CheckToMove(root->c[idx]->f, nextBlock[root->lv], k, i, j); i++);

			// 점수 계산
			root->c[idx]->score = (AddBlockToField(root->c[idx]->f, nextBlock[root->lv], k, --i, j) + DeleteLine(root->c[idx]->f) + root->score);

			// child 정보 저장
			root->c[idx]->id = nextBlock[root->lv];
			root->c[idx]->rotate = k;
			root->c[idx]->x = j;
			root->c[idx]->y = i;

			recommend(root->c[idx]);			
		}
	}

	return 0;
}
*/

void recommendedPlay() {
	start = time(NULL);
	int command;
	clear();
	act.sa_handler = BlockDown;
	sigaction(SIGALRM, &act, &oact);
	InitTetris();

	do {
		if (timed_out == 0){
			ualarm(100000, 0);
			timed_out = 1;
		}

		command = wgetch(stdscr);
		if (command == 'q' || command == 'Q')
			command = QUIT;

		
		if (blockRotate != recHead->rotate && CheckToMove(field, nextBlock[0], (blockRotate + 1) % 4, blockY, blockX)) {
			blockRotate = (blockRotate + 1) % 4;
			DrawChange(field, KEY_UP, nextBlock[0], blockRotate, blockY, blockX);
		}
		else if (blockX != recHead->x) {
			if (blockX < recHead->x && CheckToMove(field, nextBlock[0], blockRotate, blockY, blockX + 1)) {
			blockX++;
			DrawChange(field, KEY_RIGHT, nextBlock[0], blockRotate, blockY, blockX);
			}	
			else if (blockX > recHead->x && CheckToMove(field, nextBlock[0], blockRotate, blockY, blockX - 1)) {
			blockX--;
			DrawChange(field, KEY_LEFT, nextBlock[0], blockRotate, blockY, blockX);
			}
		}
		else if (blockY != recHead->y && CheckToMove(field, nextBlock[0], blockRotate, blockY + 1, blockX)) {
			blockY++;
			DrawChange(field, KEY_DOWN, nextBlock[0], blockRotate, blockY, blockX);
		}

		if (command == QUIT) {
			alarm(0);
			DrawBox(HEIGHT / 2 - 1, WIDTH / 2 - 5, 1, 10);
			move(HEIGHT / 2, WIDTH / 2 - 4);
			printw("Good-bye!!");
			refresh();
			getch();

			return;
		}
	} while(!gameOver);
	

	alarm(0);
	getch();
	DrawBox(HEIGHT / 2 - 1, WIDTH / 2 - 5, 1, 10);
	move(HEIGHT / 2, WIDTH / 2 - 4);
	printw("GameOver!!");
	stop = time(NULL);
	
	duration = stop - start;
	size = evalSize(0);

	move(20, 15);
	printw("playing time: %lf", duration);
	move(21, 15);
	printw("memory usage: %ld", size);
	move(22, 15);
	printw("score: %d", score);
	move(23, 15);
	printw("time efficiency: %lf", score / duration);
	move(24, 15);
	printw("space efficiency: %lf", (double)score / size);
	
	refresh();
	getch();
}

void createTree(RecNode *root, int level) {
	RecNode **temp1, *temp2;

	// 재귀 탈출
	if (level > VISIBLE_BLOCKS)
		return;

	// 노드 동적 할당
	temp1 = (RecNode**)malloc(sizeof(RecNode*) * CHILDREN_MAX);
	if (temp1 == NULL)
		return;

	root->c = temp1;
	
	// 노드의 child 생성 및 초기화
	for (int i = 0; i < CHILDREN_MAX; i++) {
		temp2 = (RecNode*)malloc(sizeof(struct _RecNode));
		if (temp2 == NULL)
			return;

		temp2->lv = level;
		temp2->score = 0;
		temp2->parent = root;
		root->c[i] = temp2;

		createTree(root->c[i], level + 1);
	}
}

int modified_recommend(RecNode *root) {
	int p, q, idx = 0;
	RecNode *first;
	int level = root->lv;

	// 재귀 함수 종료
	if (root->lv == VISIBLE_BLOCKS) {
		// 현재 노드의 점수가 최대 점수보다 클 때
		if (root->score > maxScore) {
			maxScore = root->score;

			// level이 1인 노드 찾기
			first = root;
			while (first->lv != 1)
				first = first->parent;

			// root에 정보 저장
			recHead->rotate = first->rotate;
			recHead->x = first->x;
			recHead->y = first->y;
		}
		return 0;
	}

	// 블록의 유효한 회전
	for (int k = 0; k < effRotation[nextBlock[root->lv]]; k++) {
		int xLen = xLength[nextBlock[root->lv]][k % 2]; // 블록의 x 길이

		// 블록을 쌓을 수 있는 위치
		for (int j = 0; j <= WIDTH - xLen; j++, idx++) {
			// parent 필드 복사
			for (p = 0; p < HEIGHT; p++)
				root->c[idx]->f[p] = root->f[p];

			// 그림자 생성 위치
			int i = 0;
			for (; Check(root->c[idx]->f, nextBlock[root->lv], k, i, j); i++);

			// 점수 계산
			root->c[idx]->score = (AddDelete(root->c[idx]->f, nextBlock[root->lv], k, --i, j) + root->score);

			// child 정보 저장
			root->c[idx]->id = nextBlock[root->lv];
			root->c[idx]->rotate = k;
			root->c[idx]->x = j;
			root->c[idx]->y = i;

			modified_recommend(root->c[idx]);			
		}
	}

	return 0;
}

int Check(short f[HEIGHT], int currentBlock, int blockRotate, int blockY, int blockX) {
	// 테트리스 블록 4x4
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (block[currentBlock][blockRotate][i][j]) {
				// 좌표가 범위를 벗어나거나 필드가 1일 때 만족
				if ((i + blockY) < 0 || (i + blockY) >= HEIGHT || (j + blockX) < 0 || (j + blockX) >= WIDTH || (f[i + blockY] >> (j + blockX)) & 1)
					return 0; // 이동할 수 없음
			}
		}
	}
	return 1; // 이동할 수 있음
}

int AddDelete(short f[HEIGHT], int currentBlock, int blockRotate, int blockY, int blockX) {
	// cnt: 접촉 면적, rtn: 리턴
	// delete_flag가 1이면 해당 라인의 필드가 모두 1, 아니면 0
	int cnt = 0, rtn = 0, delete_flag;

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			// 블록이 있는 필드를 1로 저장
			if (block[currentBlock][blockRotate][i][j]) {
				f[blockY + i] |= (1 << (blockX + j));
				// early termination
				// 블록이 바닥에 있거나 블록 아래 필드가 있을 때 만족
				if ((blockY + i + 1) == HEIGHT || (f[blockY + i + 1] >> (blockX + j)) & 1)
					cnt++;
			}
		}
	}
	rtn += cnt * 10;
	
	// cnt: 지운 라인의 수
	cnt = 0;

	for (int i = 0; i < HEIGHT; i++) {
		delete_flag = 1; // delete_flag 초기화
		for (int j = 0; j < WIDTH; j++) {
			// 필드가 0이면 만족
			if (!((f[i] >> j) & 1)) {
				delete_flag = 0;
				break;
			}
		}

		if (!delete_flag) continue;

		cnt++; // 지운 라인의 수 증가
		// 지운 라인 위의 모든 정보를 한 칸 아래로 이동
		for (int p = i; p >= 1; p--) {
			f[p] = f[p - 1];
		}
		f[0] = 0;
	}
	rtn += (cnt * cnt) * 100; // 점수 공식

	return rtn;
}

long evalSize(int level) {
	if (level <= VISIBLE_BLOCKS)
		return sizeof(RecNode) * (CHILDREN_MAX + 1) + evalSize(level + 1) * CHILDREN_MAX;
	return 0;
}
