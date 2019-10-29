#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <unistd.h>
#include <pthread.h>

pthread_t tret1, tret2;

void generateTXL()
{
    system("ls *.txt > index.txl");
    system("echo EOF >> index.txl");
}

int checkNumberOfFiles()
{
    FILE *txtList;
    txtList = fopen("index.txl", "r");

    int txtCount = 0, stat = 0;
    char tmpTxt[64];

    do
    {
        fscanf(txtList, "%s", tmpTxt);

        if (strcmp(tmpTxt, "EOF") != 0)
        {
            txtCount++;
        }
        else
        {
            stat = 1;
        }
    } while (stat == 0);

    fclose(txtList);

    return txtCount;
}

int checkTXLContent(char nf[])
{
    FILE *fp = fopen(nf, "r");

    char c;
    int count = 0;
    int stat1 = 0, stat2 = 0;

    while((c = fgetc(fp)) != EOF)
    {
        if(c == ' ' || c == '\n' || c == '.' || c == ',' || c == '-')
        {
            stat1 = 1;
            if(stat1 == 1 && stat2 == 1)
            {
                count++;
                stat1 = 0;
                stat2 = 0;
            }
        }
        else if((c >= 'a' && c <= 'z') || c >= 'A' && c <= 'Z')
        {
            stat2 = 1;
        }
    }

    fclose(fp);
    return count;
}

void fillList(char txtFileList[][128], int nFiles)
{
    int i;

    FILE *txtList;
    txtList = fopen("index.txl", "r");

    for (i = 0; i < nFiles; i++)
    {
        fscanf(txtList, "%s", txtFileList[i]);
    }

    fclose(txtList);
}

void forkedProcess(char txtFileList[][128], int nFiles)
{
    int pid1 = fork();
    
    switch (pid1)
    {
    case 0:
        printf("-- Child Process --\n");
        for (int i = nFiles/2; i < nFiles; i++)
        {
            printf("%s: %d kata\n", txtFileList[i], checkTXLContent(txtFileList[i]));
        }
        break;
    
    default:
        printf("-- Parent Process --\n");
        for (int i = 0; i < nFiles/2; i++)
        {
            printf("%s: %d kata\n", txtFileList[i], checkTXLContent(txtFileList[i]));
        }
        break;
    }
}

void *myfunc(void *myvar)
{

    char *mag;
    mag = (char *) myvar; 

    generateTXL();
    int n = checkNumberOfFiles();
    int i = 0, j = i + 1;

    char txtFileList[n][128];
    fillList(txtFileList, n);

    for (i = 0; i < n; i++)
    {
        if (pthread_equal(tret1, pthread_self()))
        {
            if (i == 0)
            {
                printf("%s\n", mag);
                printf("%s: %d kata\n", txtFileList[i], checkTXLContent(txtFileList[i]));
                printf("\n");
                sleep(1);
            }
            else
            {
                i++;
                printf("%s\n", mag);
                printf("%s: %d kata\n", txtFileList[i], checkTXLContent(txtFileList[i]));
                printf("\n");
                sleep(1);
            }
        }
        else if (pthread_equal(tret2, pthread_self()))
        {
            i++;
            printf("%s\n", mag);
            printf("%s: %d kata\n", txtFileList[i], checkTXLContent(txtFileList[i]));
            printf("\n");
            sleep(1);
        }
    }

    return NULL;
}

int main()
{
    char c;
    printf("Fork or Thread? (F/T)\n");
    scanf(" %c", &c);

    if (c == 'F')
    {
        generateTXL();

        int nFiles = checkNumberOfFiles();
    
        printf("Jumlah file dengan ekstensi *.txt: %d\n", nFiles);

        char txtFileList[nFiles][128];

        fillList(txtFileList, nFiles);

        forkedProcess(txtFileList, nFiles);
    }
    else if (c == 'T')
    {

        char *mag1 = "== First Thread ==\n";
        char *mag2 = "== Second Thread ==\n";
        int t1, t2;

        int nFiles = checkNumberOfFiles();
    
        printf("Jumlah file dengan ekstensi *.txt: %d\n", nFiles);

        t1 = pthread_create(&tret1, NULL, myfunc, (void *)mag1);    
        t2 = pthread_create(&tret2, NULL, myfunc, (void *)mag2);    
                
        pthread_join(tret1, NULL);
        pthread_join(tret2, NULL);
    }

    return 0;
}