

//curMap[i][j] : ��������. �����.


void save(int steps, char name[12]){		// ���������� �̸��޾ƿ���

	FILE *ofp;

	ofp = fopen("sokoban", "w");			// �б�����, ���ϸ� sokoban

	for(int i=0; i<31; i++)
		for(int j=0; j<31; j++){
			if(curMap[i][j]==0){			// ���� ��
				i=31;
				j=31;						// for�� �����ϱ� ���� ������
			}
			else {
				fprintf(ofp, "%c", curMap[i][j]);			// putc ��� fprintf  ���������� putc�� char�� ���� �ִµ� map������ int��.
				if(curMap[i][j]=='\n'){						// ����ó��
					i++;
					j=-1;									// ����ȸ������ j++�� �ǹǷ� -1�� ���� 
				}
			}
		}

	fclose(ofp);

	return;
}