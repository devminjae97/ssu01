#include <stdio.h>

int main(void)
{
        FILE *ifp;

        ifp = fopen("ranking.txt","r+");

        char a,b,c;
        int A, B, C;

        fopen("sdn.txt","r+");

        while ((a = getc(ifp)) != EOF)
                printf("%c", a);


        fclose(ifp);

        return 0;
}

