#include<stdio.h>
#include<stdlib.h>
#include<termio.h>
#include<string.h>


	/*
	 *  Sokoban / 19.06.07
	 *
	 *	Who made? Mj Cho, Dn Seo, Gw Lee, Yg Kim.
	 *
	 *	@:Player,  #:Wall,  $:Gold,  O:Safe,  .:Blank
	 *
	*/


int x = 0, y = 0, lvl = 0, isOnGame = 0;
int allMap[5][31][31] = {0};				// 전체맵
int curMap[31][31] = {0};					// 현재맵
int scores[5][5];							// 랭킹-점수
char names[5][5][11]={0};					// 랭킹-닉네임


int getch(void);							// 엔터없이 입력
int getMap(void);							// 맵불러오기
void setMap(int steps, char n[12]);			// 맵 출력
int onGame(char name[12]);					// 게임 
void mv(int ch, char name[12]);				// 움직이기
void display(void);							// 명령어 보기
void save(int steps, char n[12]);			// 현재 상태 저장
int load(char n[12]);						// 저장 상태 불러오기
int chckclr(void);							// 스테이지 클리어 검사
void getRank(void);							// 랭킹 불러오기
void top(void);								// 랭킹 출력

// 해야할것
int undo(int isUndo);						// 되돌리기
void ranking(int score, char n[12]);		// 점수 비교 및 저장
void end(void);
//



int main(void){

	char player_name[12] = {0};
	

	system("clear");
	
	getRank();

	printf("Loading...\n");
	if(getMap() == 1)			// 맵 불러올시 에러가 있으면
		goto exit;

	printf("Start...\n\n");
	// Get a name
	printf("input name : ");
	scanf("%10s", player_name);

	do{
		if(onGame(player_name)!=-1)
			lvl ++;
		
	}while(lvl < 5);
	
	if(lvl != 100){
		system("clear");
		printf("\n\n\n");
		printf("* * * * * * * * * * * * * * * * * *\n");
		printf("*                                 *\n");
		printf("*                                 *\n");
		printf("*                                 *\n");
		printf("*        Congraturations!!        *\n");
		printf("*                                 *\n");
		printf("*   You just clear all stages!!   *\n");
		printf("*                                 *\n");
		printf("*                                 *\n");
		printf("*                -                *\n");
		printf("*             sokoban             *\n");
		printf("*                -                *\n");
		printf("*                                 *\n");
		printf("*                                 *\n");
		printf("* * * * * * * * * * * * * * * * * *\n\n\n");
	}
	else{
		system("clear");
		printf("\n\n\n");
		printf("* * * * * * * * * * * * * * * * * *\n");
		printf("*                                 *\n");
		printf("*                                 *\n");
		printf("*                -                *\n");
		printf("*                                 *\n");
		printf("*             See You             *\n");
		printf("*                                 *\n");
		printf("*                -                *\n");
		printf("*                                 *\n");
		printf("*                                 *\n");
		printf("* * * * * * * * * * * * * * * * * *\n\n\n");
	}
	exit :

	return 0;
}


void getRank(void){

	int i, j;
	FILE *ifp;

		for(i=0; i<5; i++)
			for(j=0; j<5; j++){
				strcpy(names[i][j], "----------");
				scores[i][j] = 999999;
			}
	if((ifp = fopen("ranking", "r")) != NULL)
		for(i = 0; i<5; i++)
			for(j = 0; j<5; j++)
				fscanf(ifp, "%s %d\n", names[i][j], &scores[i][j]);

	fclose(ifp);
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
	
	int i,j,chcksafe = 0;
	
	system("clear");

	printf("Hello %s\n\nStage %d %8d Steps\n\n\n", n, lvl+1, steps);	

	for(i = 0; i < 31; i++){
		for(j = 0; j < 31; j++){
			switch(curMap[i][j]){
				case '@' : putchar('@'); x=j; y=i; break;			// 플레이어 위치
				case '#' : putchar('#'); break;
				case '$' : putchar('$'); break;
				case 'O' : putchar('O'); chcksafe++; break;
				case '.' : putchar(' '); break;
				case '\n' : putchar('\n');i++; j = -1; break;
				default : break;
			}
			
		}
	}
	
	
	printf("\n\n\n(Command) ");
	
	if(chcksafe == 0){
		chcksafe = chckclr();
		if(chcksafe == 1){
			isOnGame = 0;
			ranking(steps, n);
			
			if(lvl!=4){
			printf(" Clear!\n\nPress 'Enter' to Continue.");
			
				do{
					i=getch();
				}while(i != '\n');
			}
		}
	}

	//개발자 치트
	if(steps==111111){
		isOnGame=0;
		ranking(steps, n);

		printf(" goto nextLevel\n\nPress Enter.");

		do{
			i=getch();
		}while(i!='\n');
	}

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
	else
		undo(1);
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
	
re :

	//게임 값 초기화
	undo(0);
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
		int key=0;
		
		key = getch();

		switch(key){
			case 'h' :
			case 'j' :
			case 'k' :
			case 'l' : steps++; mv(key, name); setMap(steps, name); break; 	//이동
			case 'u' : 
				if(undo(2))
					steps++;
				setMap(steps, name);
				break;
			case 'r' : goto re; break;
			case 'n' : steps=-1; lvl=0; isOnGame=0; break;
			case 'e' : end(); break;
			case 's' : save(steps, name); break;
			case 'f' : steps=load(name); setMap(steps, name); break;
			case 'd' : display(); setMap(steps, name); break;	//명령어
			case 't' : top(); setMap(steps, name); break;
			case 'z' : setMap(111111, name); break;
			default : break;
		}

	}while(isOnGame == 1);	
	
	system("clear");
	
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
	printf("        Press 'Enter' to quit");

	do{
		ch = getch();
	}while(ch != '\n');

	return;
}


void save(int steps, char n[12]){			// 현재 맵 저장하기. steps 수도 저장
	
	FILE *ofp;
	
	ofp = fopen("sokoban", "w");
	
	
	fprintf(ofp, "%s\n%d\n%d\n", n, lvl, steps);

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


int load(char n[12]){					// 현재 맵에 저장할 때 먼저 현재 맵을 다 지우고 저장하기!			
	
	FILE *ifp;
	int c=0, i=0, j=0, steps;


	system("clear");



	// Load savd file
    if ((ifp = fopen("sokoban","r")) == NULL){
		system("cat >> eLog <<EOF\nError : There's no save file!\nEOF");
		fclose(ifp);
		return 1;
	}
	else{
		for(int i=0; i<31; i++)
			for(int j=0; j<31; j++)
				curMap[i][j] = 0;

		fscanf(ifp, "%s\n%d\n%d\n%", n, &lvl, &steps);
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


void top(void){

	int i, j, num = 0;

	printf("t ");

	num = getchar();

	system("clear");


	if(num=='\n'){		// enter
	 	printf("\n      *Hall of Fame*     \n");
		for(i=0; i<5; i++){
			printf("\n│ [stage %d]              │\n", i+1);
			for(j=0; j<5; j++)
				printf("│ %d  %-11s %7d │\n",j+1, names[i][j], scores[i][j]);
		}
	}
	else if(num>='1'&&num<='5'){
		i = num - '1';
	 	printf("\n      *Hall of Fame*     \n");
		printf("\n│ [stage %d]              │\n", i+1);
		for(j=0; j<5; j++)
			printf("│ %d  %-11s %7d │\n",j+1, names[i][j], scores[i][j]);		
		i = getch();		// dummy input
	}
	else
		return;

	printf("\n\n   Press 'Enter' to quit");
		
	do{
		int key = getch();
		if(key == '\n')
			break;
	}while(1);
	return;
}


int chckclr(void){			// 클리어 체크 
	for(int i=0; i<31; i++){
		for(int j=0; j<31; j++){
			if(allMap[lvl][i][j] == 'O' && curMap[i][j] != '$')
				return 0;
		}
	}

	return 1;
}

int undo(int isUndo){		// 0:reset, 1:save history, 2:undo
	static int history[5][31][31];
	static int target;
	int i,j,k;
	
	switch(isUndo){
		case 0 :
			target=0;
			for(i=0;i<5;i++)
				for(j=0;j<31;j++)
					for(k=0;k<31;k++)
						history[i][j][k] = 'n';
			break;
		case 1 :
//			system("clear");
			target++;
//			printf("%d\n\n\n\n\n",target%5);
			for(i=0;i<31;i++)
				for(j=0;j<31;j++){
					
					history[target%5][i][j]=curMap[i][j];
					
//					printf("%c",history[target%5][i][j]);
				}
			break;
		case 2 :
			if(history[target%5][0][0]=='n'){
//				printf("get in%d\n",target);
				return 0;
			}
			else{
				system("clear");
//				printf("%d / %c\n", target%5, history[target%5][0][0]);
				for(i=0;i<31;i++)
					for(j=0;j<31;j++){
						curMap[i][j]=history[target%5][i][j];
//						printf("%c", history[target%5][i][j]);

					}
//				printf("done?\n");
//				scanf("%d",&x);
				history[target%5][0][0] = 'n';
				
				target--;
			}
			break;
		default :
			break;
	}
	return 1;
}


void ranking(int score, char n[12]){

	int l, i, j;
	
	FILE *ofp;

	ofp = fopen("ranking","w");

	// 현재 레벨에서 5위부터 확인
	// 5위보다 낮으면 5위에 넣기
	if(scores[lvl][4] > score){
		scores[lvl][4] = score;
		
		for(i = 0; i < 10; i++)
			names[lvl][4][i] = n[i];
		// 4위부턴 i, i-1위 비교
		for(i = 3; i >= 0; i--){
			if(scores[lvl][i] > score){
				scores[lvl][i+1] = scores[lvl][i];
				scores[lvl][i] = score;
				

				for(j = 0; j < 10; j++){
					names[lvl][i+1][j] = names[lvl][i][j];
					names[lvl][i][j] = n[j];
				}
			}
			else
				i = -11;
		}
	
		// 저장
		for(l = 0; l < 5; l++)
			for(i = 0; i < 5; i++){
				for(j = 0; j < 10; j++)
					fprintf(ofp, "%c", names[l][i][j]);	
				
				fprintf(ofp, " %d\n", scores[l][i]);
			}
	}
	fclose(ofp);

	return;
}

void end(void){
	
	isOnGame = 0;
	lvl = 99;

	return;
}















