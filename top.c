#include <stdio.h> //헤더파일 명시는 붙일 때 지워도 되나?
#include <stdlib.h> //일단 잘 작동하는지 확인할려면 필요한 듯?

void top(int)
{
	FILE *ofp;

	ofp = fopen("ranking.txt","r+");

	int score, level; //level 입력받아야됨 아직은
	int save_rank[5]; //순위 바뀔 때 기존 순위 임시 저장
	char new_name, name[5]; //문자 다루는 법?

	int rank[5][5];

	for (int k=4;k>=0;i--)
		save_rank[k] = rank[level-1][k]; //기존 순위 임시로 복사

	for (int i=4;i>=0;i--){ //점수와 기존 순위가 같을 때는?
		if (score > rank[level-1][i]){
			rank[level-1][i] = score; //점수를 랭킹에 복사
			if (i<=3)
				rank[level-1][i+1] = save_rank[i]; //기존 순위를 한 단계 밑으로 저장
		}
	}
}



//텍스트 파일에 있는 숫자랑 문자를  변수로 어떻게 옮기지?
//텍스트 파일에다가 쓰는 것 : fprintf ??
//텍스트 파일에 있는 것을 입력받는 것 : fscanf ??
//이름 텍스트 파일하고 점수 텍스트 파일 따로해도 되나?
