#include "Solver/header.h"

extern int **m;
extern int mask;   //flag mask (mask=1<<n2+1)
extern int n2;
extern int chkEnabled[5];
int cycleCounter;
extern FILE *logOutput;

int solver(int enableBruteForce, int maxCycles)
{
    int fin1=0;   //flags of completed rows
    int fin2=0;   //flags of completed columns
    int fin3=0;   //flags of completed squares
    int allCompleted=(mask>>1)-1;
    cycleCounter=0;

    print("pruning:\n");
    printPrune();

    do{       
        cycleCounter++;

        fin1=checkRow(fin1);
        fin2=checkColumn(fin2);
        fin3=checkSquare(fin3);

        if(fin1==-1 || fin2==-1 || fin3==-1)
        {
            return 0;   //error found
        }

    }while(fin3<allCompleted   &&   cycleCounter<maxCycles);


    if(DEBUG1) printf("fin1=%d  fin2=%d  fin3=%d\n",fin1,fin2,fin3);//////////////////


    if(fin3==allCompleted)
    {
        if(countIncompleted()==0)
            return (valid()?                                2 : 0);
        else
            if(chkEnabled[LV0]==0 && checkUnicity())
                return (validPartially()?   1 : 0);
            else
                return 0;
    }else{

        if(enableBruteForce && 0)
        {
            /*
            //risolvi con brute force
            int i,j=0, flag;

            print("Brute forcing\n");

            flag=1;

            //get the first int-of-flags
            for(i=0;    i<n2 && flag;   i++)
            {
                for(j=0;    j<n2 && flag;   j++)
                {
                    if(m[i][j] & mask)
                        flag=0;
                }
            }

            i--;
            j--;

            int oldValue=m[i][j];

            int k;
            for(k=1; k<=n2; k++)
            {
                if(oldValue & 1<<k)
                {
                    m[i][j]=k;
                    resetPossibilities(m);
                    if(prune(0,50,1,-1,-1)>0)
                    {
                        resetPossibilities(m);
                        return solver(0,50);
                    }
                }
            }

            m[i][j]=k;
            resetPossibilities(m);
            return solver(0,50);
            */
            return 0;

        }else{
            return 0;    //maxCycles limit exceeded: the program seems to be stucked
        }
    }
}

int valid()
{
    int i,j;
    int totR,totC;
    int num=(n2+1)*n2/2;

    for(i=0; i<n2; i++)
    {
        totR=0;
        totC=0;
        for(j=0; j<n2; j++)
        {
            totR+=m[i][j];
            totC+=m[j][i];
        }
        if(totR!=num)
            return 0;   //false
        if(totC!=num)
            return 0;   //false
    }
    return 1;   //true
}

int validPartially()
{
    int i,j;
    int totR,totC;
    int num=(n2+1)*n2/2;

    if(checkUnicity()!=1)
        return 0;

    for(i=0; i<n2; i++)
    {
        totR=0;
        totC=0;
        for(j=0; j<n2; j++)
        {
            if(m[i][j] & mask)
                totR+=checkOneValue(i,j);
            else
                totR+=m[i][j];

            if(m[j][i] & mask)
                totC+=checkOneValue(j,i);
            else
                totC+=m[j][i];
        }
        if(totR!=num)
            return 0;   //false
        if(totC!=num)
            return 0;   //false
    }
    return 1;   //true
}

void setLimitations(int pruning)
{
    switch(pruning)
    {
    case 0:
        chkEnabled[LV0]=1;
        chkEnabled[LV1]=1;
        chkEnabled[LV2]=1;
        chkEnabled[LV3]=1;
        chkEnabled[LV4]=1;
        break;
    case 1:
        chkEnabled[LV0]=0;
        chkEnabled[LV1]=1;
        chkEnabled[LV2]=1;
        chkEnabled[LV3]=1;
        chkEnabled[LV4]=1;
        break;
    case 2:
        chkEnabled[LV0]=0;
        chkEnabled[LV1]=1;
        chkEnabled[LV2]=1;
        chkEnabled[LV3]=1;
        chkEnabled[LV4]=0;
        break;
    case 3:
        chkEnabled[LV0]=0;
        chkEnabled[LV1]=1;
        chkEnabled[LV2]=1;
        chkEnabled[LV3]=0;
        chkEnabled[LV4]=0;
        break;
    case 4:
        chkEnabled[LV0]=0;
        chkEnabled[LV1]=1;
        chkEnabled[LV2]=0;
        chkEnabled[LV3]=0;
        chkEnabled[LV4]=0;
        break;
    default:
        printInt("Valore di pruning invalido: ",pruning);
        break;
    }
}
