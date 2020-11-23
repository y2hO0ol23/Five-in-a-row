#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <windows.h>
#include <conio.h>

using namespace std;

#pragma warning(disable : 4996)

#define VALUE 15

#define LEFT 75
#define RIGHT 77
#define UP 72
#define DOWN 80
#define ENTER 13

#define color(a) SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), a)

void gotoxy(int x, int y) {

	COORD pos = { (2 * x),y };

	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

void set_game();
void draw_plate();
void draw_stone(int x, int y, int num);
void set(int x,int y);
void move(int key);

int is_connect(int x, int y, int type);
int dxic[4] = { 1,0,1,1 };
int dyic[4] = { 0,1,1,-1 };

void ai();
int win_rate(int x, int y, int type); // 경우의 수 가져오는 함수
long int profit[VALUE + 2][VALUE + 2]; //이익을 보는 수
int dxwr[4] = { 1,0,1,1 };
int dywr[4] = { 0,1,1,-1 };

int map[VALUE + 2][VALUE + 2];
int stone_count;
int Xpos, Ypos;
int winner;
int turn;

int main() {
	CONSOLE_CURSOR_INFO Curinfo;
	Curinfo.dwSize = 1;
	Curinfo.bVisible = 0;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &Curinfo);
	int key = 0;
	set_game();
	while (1) {
		if (kbhit()) key = getch();
		if (turn == 1) { // 플레이어 턴일때
			switch(key){ // 움직임
				case LEFT:
				case RIGHT:
				case UP:
				case DOWN:
					break;
				case ENTER: // 엔터를 클릭할시 돌 설치
					set(Xpos, Ypos);

				default: //움직이는 것이 아닐경우 키 초기화
					key = 0;
			}
			if (key) move(key); // 초기화 되어있지 않으면 이동한 것으로 간주
		}
		else if (turn == 2) { // ai 턴이라면
			ai();
		}
		key = 0; // 키 초기화
		gotoxy(0, 0);
		draw_plate();
		if (winner > 0) { // 승자가 정해졌다면
			gotoxy(0, VALUE + 3); // 문구를 표시할 좌표로 이동 후 (맨 아래)
			if (winner == 1) printf("You Win!"); // 플레이어가 이겼다면
			if (winner == 2) printf("You lose..."); // 플레이어가 졌다면
			break;
		}
	}
}

void set_game() {
	winner = 0;
	turn = 1;
	Xpos = (VALUE / 2) + (VALUE % 2), Ypos = (VALUE / 2) + (VALUE % 2); //시작점을 한 가운데로 설정
	for (int i = 0; i < VALUE + 2; i++) { // 오목판 초기화
		map[i][0] = -1;
		map[0][i] = -1;
		map[i][VALUE + 1] = -1;
		map[VALUE + 1][i] = -1;
	}
	gotoxy(0, 0); //커서를 0,0으로 이동후
	draw_plate(); //맵을 그림
}

void draw_plate() {
	for (int i = 1; i <= VALUE; i++) {
		for (int j = 1; j <= VALUE; j++) {
			if (i == 1) {  // 윗줄일때
				if (j == 1) draw_stone(i, j, 0); //첫째 열일때
				else if (j == VALUE) draw_stone(i, j, 2); // 마지막 열일때
				else draw_stone(i, j, 1); // 가운데 열일때
			}
			else if(i == VALUE) { // 아래줄일때
				if (j == 1) draw_stone(i, j, 3); //첫째 열일때
				else if (j == VALUE) draw_stone(i, j, 5);// 마지막 열일때
				else draw_stone(i, j, 4);// 가운데 열일때
			}
			else { //  가운데 행일때
				if (j == 1) draw_stone(i, j, 6);//첫째 열일때
				else if (j == VALUE) draw_stone(i, j, 8);// 마지막 열일때
				else draw_stone(i, j, 7);// 가운데 열일때
			}
		}
		printf("\n");
	}

}

void draw_stone(int x, int y, int num) {
	if(x == Xpos && y == Ypos) color(6);
	if (map[x][y] == 1) {// 검은돌
		printf("○");
		color(7);
		return;
	}
	if (map[x][y] == 2) { // 흰돌
		printf("●");
		color(7);
		return;
	}
	if (x == Xpos && y == Ypos) { // 커서
		printf("□");
		color(7);
		return;
	}
	if (num == 0) printf("┌ "); // ┌┬┬┬┬┬┬┬┐
	if (num == 1) printf("┬ "); // ├┼┼┼┼┼┼┼┤
	if (num == 2) printf("┐ "); // ├┼┼┼┼┼┼┼┤
	if (num == 3) printf("└ "); // ├┼┼┼┼┼┼┼┤
	if (num == 4) printf("┴ "); // ├┼┼┼┼┼┼┼┤
	if (num == 5) printf("┘ "); // ├┼┼┼┼┼┼┼┤
	if (num == 6) printf("├ "); // ├┼┼┼┼┼┼┼┤
	if (num == 7) printf("┼ "); // ├┼┼┼┼┼┼┼┤
	if (num == 8) printf("┤ "); // └┴┴┴┴┴┴┴┘  각각의 조각들
	color(7);
}

void set(int x,int y) {
	if (map[x][y] == 0) {
		map[x][y] = 1;
		stone_count++;
		if (is_connect(x, y, 1) == 5) winner = 1;
		turn = 2;
		return;
	}
	if (map[x][y] > 0) {
		gotoxy(0, VALUE + 3);
		printf("Already stone on there");
	}
}

void move(int key) {
	gotoxy(0, VALUE + 3);
	printf("                      ");
	if (key == LEFT && map[Xpos][Ypos - 1] != -1) Ypos--;
	if (key == RIGHT && map[Xpos][Ypos + 1] != -1) Ypos++;
	if (key == UP && map[Xpos - 1][Ypos] != -1) Xpos--;
	if (key == DOWN && map[Xpos + 1][Ypos] != -1) Xpos++;
}

int is_connect(int x, int y, int type) {
	int rt = -5;
	int max = 0;
	for (int i = 0; i < 4; i++) {
		int p = 1;
		int m = -1;
		int cnt = 1;
		while (map[x + (dxic[i] * p)][y + (dyic[i] * p)] == type) p++;
		while (map[x + (dxic[i] * m)][y + (dyic[i] * m)] == type) m--;
		int cmp = p + -m - 6;
		if (((rt < 0) ? -rt : rt) > ((cmp < 0) ? -cmp : cmp)) rt = cmp;
	}
	return rt + 5;
}

void ai() {
	for (int i = 1; i <= VALUE; i++)
		for (int j = 1; j <= VALUE; j++)
			profit[i][j] = 0; // 이길 확률을 초기화
	for (int i = 1; i <= VALUE; i++) {
		for (int j = 1; j <= VALUE; j++) {
			if (map[i][j] == 0 && is_connect(i, j , 2) == 5) { // 끝낼 수 있는 위치가 있다면
				map[i][j] = 2; // 그곳에 돌을 두고
				stone_count++; 
				winner = 2; // 게임을 이김
				return;
			}
			if (map[i][j] == 0) { //돌을 놓을 수 잇는 공간이라면
				profit[i][j] = (win_rate(i, j, 1) * 3)+ win_rate(i, j, 2); // 그 위치의 값을 계산
			}
			//if (map[i][j] == 2) win_rate(i, j, 2);
		}
	}
	int bestx = 0, besty = 0;
	int best = 0;
	gotoxy(0, VALUE + 1);
	for (int i = 1; i <= VALUE; i++) {
		for (int j = 1; j <= VALUE; j++) {
			if (map[i][j] == 0 && best < profit[i][j]) { //돌을 놓을 수 있는 공간이 이길 확률이 가장 높은 경우
				bestx = i; //확률과 좌표를 저장
				besty = j;
				best = profit[i][j];
			}
			//printf("%6d", profit[i][j]);
		}
		//printf("\n");
	}
	map[bestx][besty] = 2; // 가장 높을 확률을 가지고 있는 위치에 돌을 설치
	stone_count++;
	turn = 1; // 턴을 종료
}

int win_rate(int x, int y, int type) { // 아직 확률 계산이 정확하지 않음 - 여러가지를 시도해 보는중
	int all = 0;
	int cnt_rate = 1;
	for (int i = 0; i < 4; i++) {
		int p = 1;
		int m = -1;
		while ((map[x + (dxic[i] * p)][y + (dyic[i] * p)] == type || map[x + (dxic[i] * p)][y + (dyic[i] * p)] == 0) && p < 4) { // 4칸 안에 있는 돌 겟수 만큼
			p++;
			if(map[x + (dxic[i] * p)][y + (dyic[i] * p)] == type) cnt_rate *= 2; //확률값을 제곱
		}
		while ((map[x + (dxic[i] * m)][y + (dyic[i] * m)] == type || map[x + (dxic[i] * m)][y + (dyic[i] * m)] == 0) && m > -4) {// 똑같이 반대 4칸 안에 있는 돌 겟수 만큼
			m--;
			if(map[x + (dxic[i] * m)][y + (dyic[i] * m)] == type) cnt_rate *= 2; //확률값을 제곱
		}
	}
	return cnt_rate; // 그 값을 반환

	// 모든 경우의 수를 측정하기에는 오랜 시간이 걸려 효율적이지 않음
	// 상대가 이미 있는 돌을 이용하여 5줄을 만들 있는 경우를 다른 곳에 만든 후, 막을 수 있는 갯수와
	// 자신이 5줄을 만들 수 있는 경우와 만들고 연결될 수 있는 갯수를 가져오는 방법을 사용해볼 예정
}
