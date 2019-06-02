#include<stdio.h>
#include<stdlib.h>
#include<termio.h>


	/*
	 *  Sokoban / 19.05.30
	 *
	 *	Who made? Mj Cho, Dn Seo, Gw Lee, Yg Kim.
	 *
	 *	@:Player,  #:Wall,  $:Gold,  O:Safe,  .:Blank
	 *
	*/


int x = 0, y = 0, lvl = 0, isOnGame = 0;
int allMap[5][31][31] = {0};
int curMap[31][31] = {0};

int getch(void);							// 엔터없이 입력
int getMap(void);							// 맵불러오기
void setMap(int steps, char n[12]);			// 맵 출력
int onGame(char name[12]);					// 게임 
void mv(int ch, char name[12]);				// 움직이기
void display(void);							// 명령어 보기
void save(int steps);						// 현재 상태 저장

// 해야할것
void undo(void);
int load(void);
void top(int);
//



int main(void){

	char player_name[12] = {0};

	system("clear");

	printf("Loading...\n");
	if(getMap() == 1)			// 맵 불러올시 에러가 있으면
		goto end;

	printf("Start...\n\n");
	
	// Get a name
	printf("input name : ");
	scanf("%10s", player_name);

	do{
		if(onGame(player_name)!=-1)
			lvl ++;
		
	}while(lvl < 6);

	end :

	return 0;
}


int getch(void){					// 엔터 없이 입력
	int ch;

	struct termios buf;
	struct termios save;

	tcgetattr(0, &save);
	buf = save;

	buf.c_lflag&=~(ICANON|ECHO);
	buf.c_cc[VMIN] = 1;
	buf.c_cc[VTIME] = 0;

	tcsetattr(0, TCSAFLUSH, &buf);
	
	ch = getchar();
	tcsetattr(0, TCSAFLUSH, &save);

	return ch;
}	


int getMap(void){					// map 파일에서 맵 가져오기
	
	int c, lvls = 0, i = 0, j = 0, cmp = 0;

	FILE *ifp;
	
	// Load maps
    if ((ifp = fopen("map","r")) == NULL){
		printf("Error : Cannot open the map file!\n");
		system("cat >> eLog <<EOF\nError : Cannot open the map file!\nEOF");
		fclose(ifp);
		return 1;
	}
	else{
		while((c = getc(ifp)) != EOF){				// '1' = 49, '\n' = 10
			switch(c){
				case '\n' :
					if(j != 0){                     // 스테이지가 바뀌지 않았을 때 
						allMap[lvls][i++][j] = c;
						j = 0;
					}
			 		break;
				case 'e' :
					break;
				case '2' :
				case '3' :
				case '4' :
				case '5' :
					lvls++; i=0; j=0;
					if(cmp != 0){                   // 금과 금고개수 확인
						printf("Error : [Stage-%d] The number of $ and O is different!\n", lvls+1);
						system("cat >> eLog <<EOF\nError : The number of $ and O is different!\nEOF");
						fclose(ifp);
						return 1;
					}
					break;
				case '$' :					// $,O,@,#,.
				case 'O' :
				case '@' :
				case '#' :
				case '.' :
					if(c == '$')
						cmp++;
					else if(c == 'O')
						cmp--;
					allMap[lvls][i][j++] = c;
					break;
			}
		}
	}
	fclose(ifp);

	printf("\nDone\n\n");
 		
	return 0;
}


void setMap(int steps, char n[12]){					// 맵 최신화, 출력
	
	int i,j;
	
	system("clear");

	printf("Hello %s\n\nStage %d %8d Steps\n\n\n", n, lvl, steps);	

	for(i = 0; i < 31; i++){
		for(j = 0; j < 31; j++){
			switch(curMap[i][j]){
				case '@' : putchar('@'); x=j; y=i; break;			// 플레이어 위치
				case '#' : putchar('#'); break;
				case '$' : putchar('$'); break;
				case 'O' : putchar('O'); break;
				case '.' : putchar(' '); break;
				case '\n' : putchar('\n');i++; j = -1; break;
				default : break;
			}
			
		}
	}
	
	printf("\n\n\n(Command) ");
	
	return;

}


void mv(int ch, char name[12]){
	
	static int checkgoldmv;					// 금을 움직여야 하는지 체크하는 변수
	int tmpx = x, tmpy = y;
	
	char isS[12] = {0};

	checkgoldmv = 0;
	if(name[10] == '$'){
		switch(ch){
			case 'h' : tmpx--; break;
			case 'l' : tmpx++; break;
			case 'j' : tmpy++; break;
			case 'k' : tmpy--; break;
			default : break;
		}
	}	
	switch(ch){
		case 'h' : tmpx--; break;
		case 'l' : tmpx++; break;
		case 'j' : tmpy++; break;
		case 'k' : tmpy--; break;
		default : break;
	}
	
	switch(curMap[tmpy][tmpx]){
		case '.' : 
		case 'O' : 
			if(name[10] != '$'){
				curMap[tmpy][tmpx]='@';
				if(allMap[lvl][y][x]=='O')
					curMap[y][x]=allMap[lvl][y][x];
				else
					curMap[y][x]='.';
			}
			else{
				curMap[tmpy][tmpx]='$';
				checkgoldmv ++;
			}
			break;
		case '$' :
			if(name[10] != '$'){
				isS[10] = '$';

				mv(ch, isS);						// 금 앞에 무엇이 있는지 재귀함수를 통해 판단
				if(checkgoldmv == 1){
					curMap[tmpy][tmpx]='@';
					if(allMap[lvl][y][x]=='O')
						curMap[y][x]=allMap[lvl][y][x];
					else
						curMap[y][x]='.';
				}
			}
			break;
		case '#' :
		default : break;
	}

	return;
}


int onGame(char name[12]){				
	
	int steps;	
	int undoArr[2][5];
	
re :

	//게임 값 초기화
	for(int i=0; i<2; i++)
		for(int j=0; j<5; j++)
			undoArr[i][j] = 0;
	if(isOnGame==0)
		steps = 0;	
	for(int i = 0; i < 31; i++){
		for(int j = 0; j < 31; j++){
			curMap[i][j]=allMap[lvl][i][j];
		}
	}

	//게임 시작
	isOnGame = 1;

	setMap(steps, name);


	do{
		int key;
		
		key = getch();

		switch(key){
			case 'h' :
			case 'j' :
			case 'k' :
			case 'l' : steps++; mv(key, name); setMap(steps, name); break; 	//이동
			case 'u' : break;
			case 'r' : goto re; break;
			case 'n' : steps=-1; lvl=0; isOnGame=0; break;
			case 'e' : break;
			case 's' : save(steps); break;
			case 'f' : steps=load(); setMap(steps, name); break;
			case 'd' : display(); setMap(steps, name); break;	//명령어
			case 't' : break;
			default : break;
		}

	}while(isOnGame == 1);
	
	return steps;
}


void display(void){							// 커맨드 보기
	int ch = '0';
	
	system("clear");
	printf("\n\n\n");
	printf("* * * * * * * * * * * * * * * * * * *\n");
	printf("*              Command              *\n");
	printf("*                                   *\n");
	printf("* h(up), j(left), k(right), l(down) *\n");
	printf("* u(undo)                           *\n");
	printf("* r(retry current stage)            *\n");
	printf("* n(new game)                       *\n");
	printf("* e(exit game)                      *\n");
	printf("* s(save current)                   *\n");
	printf("* f(load saved file)                *\n");
	printf("* d(display Command)                *\n");
	printf("* t(top ranking)                    *\n");
	printf("*                                   *\n");
	printf("* * * * * * * * * * * * * * * * * * *\n\n");
	printf("          Press 'q' to quit");

	do{
		ch = getch();
	}while(ch != 'q' && ch != 'Q');
	
	return;
}


void save(int steps){			// 현재 맵 저장하기. steps 수도 저장
	
	FILE *ofp;
	
	ofp = fopen("sokoban", "w");
	
	
	fprintf(ofp, "%d\n%d\n", lvl, steps);

	for(int i=0; i<31; i++)
		for(int j=0; j<31; j++){
			if(curMap[i][j]==0){
				i=31;
				j=31;
			}
			else {
				fprintf(ofp,"%c", curMap[i][j]);
				if(curMap[i][j]=='\n'){
					i++;
					j=-1;
				}
			}
		}

	fclose(ofp);
	
	return;
}


int load(void){					// 현재 맵에 저장할 때 먼저 현재 맵을 다 지우고 저장하기!			
	
	FILE *ifp;
	int c=0, i=0, j=0, steps;

	for(int i=0; i<31; i++){
		for(int j=0; j<31; j++){
			curMap[i][j] = 0;
		}
	}



	system("clear");



	// Load savd file
    if ((ifp = fopen("sokoban","r")) == NULL){
		system("cat >> eLog <<EOF\nError : There's no save file!\nEOF");
		fclose(ifp);
		return 1;
	}
	else{
		fscanf(ifp, "%d\n%d\n%", &lvl, &steps);
		while((c = getc(ifp)) != EOF){
			if(c == '\n'){
				curMap[i++][j] = c;
				j = 0;
			}
			else
				curMap[i][j++] = c;
		}
	}	

	return steps;
}








