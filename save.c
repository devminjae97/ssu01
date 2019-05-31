

//curMap[i][j] : 전역변수. 현재맵.


void save(int steps, char name[12]){		// 현재점수와 이름받아오기

	FILE *ofp;

	ofp = fopen("sokoban", "w");			// 읽기전용, 파일명 sokoban

	for(int i=0; i<31; i++)
		for(int j=0; j<31; j++){
			if(curMap[i][j]==0){			// 맵의 끝
				i=31;
				j=31;						// for문 종료하기 위한 값지정
			}
			else {
				fprintf(ofp, "%c", curMap[i][j]);			// putc 대신 fprintf  쓰는이유는 putc는 char만 쓸수 있는데 map파일은 int라서.
				if(curMap[i][j]=='\n'){						// 개행처리
					i++;
					j=-1;									// 다음회차에서 j++가 되므로 -1로 지정 
				}
			}
		}

	fclose(ofp);

	return;
}
