#include "tetris.h"

static struct sigaction act, oact;

int main(){
	int exit=0;

	initscr();
	noecho();
	keypad(stdscr, TRUE);	

	srand((unsigned int)time(NULL));

	while(!exit){
		clear();
		switch(menu()){
		case MENU_PLAY: play(); break;
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

	nextBlock[0]=rand()%7;
	nextBlock[1]=rand()%7;
	nextBlock[2]=rand()%7; // 1주차 과제
	blockRotate=0;
	blockY=-1;
	blockX=WIDTH/2-2;
	score=0;	
	gameOver=0;
	timed_out=0;

	DrawOutline();
	DrawField();
	DrawBlockWithFeatures(blockY, blockX, nextBlock[0], blockRotate); // 1주차 과제
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

// 1주차 실습

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
	int prevBlockRotate = blockRotate + 4, prevBlockY = blockY, prevBlockX = blockX;
	
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
	prevBlockRotate %= 4;

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

	for (; CheckToMove(field, nextBlock[0], prevBlockRotate, prevBlockY, prevBlockX); prevBlockY++);	

	// 이전 그림자 지우기
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

	// 현재 블록 그리기
	DrawBlockWithFeatures(blockY, blockX, currentBlock, blockRotate);
}

void BlockDown(int sig){
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
	nextBlock[0] = nextBlock[1];
	nextBlock[1] = nextBlock[2]; // 1주차 과제
	nextBlock[2] = rand() % 7;
	DrawNextBlock(nextBlock); // 다음 블록 그리기

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
	DrawBlock(y, x, blockID, blockRotate, ' ');
}

//
void createRankList(){
	// user code
}

void rank(){
	// user code
}

void writeRankFile(){
	// user code
}

void newRank(int score){
	// user code
}

void DrawRecommend(int y, int x, int blockID,int blockRotate){
	// user code
}

int recommend(RecNode *root){
	int max=0; // 미리 보이는 블럭의 추천 배치까지 고려했을 때 얻을 수 있는 최대 점수

	// user code

	return max;
}

void recommendedPlay(){
	// user code
}
