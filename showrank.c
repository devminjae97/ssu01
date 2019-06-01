#include <stdio.h>

int showrank(void)
{
        FILE *ifp;

        ifp = fopen("ranking.txt","r+");

        char a;

        fopen("ranking.txt","r+");

        while ((a = getc(ifp)) != EOF)
                printf("%c", a);


        fclose(ifp);

        return 0;
}

