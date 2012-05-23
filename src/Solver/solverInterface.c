#include "Solver/header.h"

int n;   //base of the sudoku
int n2;   //size of the sudoku (n2=n*n)
int mask;   //flag mask (mask=1<<n2+1)
int **m;   //matrix n^2 * n^2 for the sudoku
int chkEnabled[5];
FILE *logOutput;

int prune(int **matrix,int num,int tier, int lastX, int lastY)
{ 
    resetPossibilities(matrix);
    m=matrix;
    n=num;
    n2=n*n;
    mask=1<<(n2+1);

    openLogFile();
    print("(prune)\n");
    printMatrix(logOutput,0);

    //check if its is solvable
    setLimitations(1);
    int result = solver(0,50);

    int oldValue=0;

    if(result==0    &&  lastX!=-1   && lastY!=-1)
    {
        //check if its is solvable after resetting the last value
        oldValue=m[lastX][lastY];
        m[lastX][lastY]=mask;

        resetPossibilities(m);
        result=solver(0,50);

        m[lastX][lastY]=oldValue;

        if(result!=0)
            result=-1;  //last value was incorrect!
    }


    //prune
    resetPossibilities(m);
    setLimitations(tier);   //tier = 0 ---> solve
                            //tier = 1 ---> total pruning
                            //tier = 2 ---> advanced pruning
                            //tier = 3 ---> medium pruning
                            //tier = 4 ---> basic pruning
    solver(0,50);

    closeLogFile();
    return result;
}

int solveRational(int **matrix,int num, int lastX, int lastY)
{
    resetPossibilities(matrix);
    m=matrix;
    n=num;
    n2=n*n;
    mask=1<<(n2+1);

    openLogFile();
    print("(solveRational)\n");
    printMatrix(logOutput,0);

    setLimitations(1);
    int result=solver(0,50);

    int oldValue=0;

    if(result==0    &&  lastX!=-1   && lastY!=-1)
    {
        oldValue=m[lastX][lastY];
        m[lastX][lastY]=mask;

        resetPossibilities(m);
        setLimitations(1);
        result=solver(0,50);

        if(result!=0) 
            result=-1;  //last value was incorrect!

        m[lastX][lastY]=oldValue;
    }

    resetPossibilities(m);
    setLimitations(0);
    solver(0,50);

    closeLogFile();
    return result;
}

int solveBruteForce(int **matrix,int num)
{
    resetPossibilities(matrix);
    m=matrix;
    n=num;
    n2=n*n;
    mask=1<<(n2+1);

    openLogFile();
    print("(solveBruteForce)\n");
    printMatrix(logOutput,0);

    setLimitations(0);
    int result=solver(1,50);
    closeLogFile();
    return result;
}

void openLogFile()
{
#if DEBUG_LOG
    logOutput=fopen("SS_solverLog.txt","wt");
    if(logOutput==NULL)
        exit(ERR_FILE);
    print("[Log del solver in C di SudokuSolver2]\n");
#endif
}

void closeLogFile()
{
#if DEBUG_LOG
    fclose(logOutput);
#endif
}

void print(const char *text)
{
#if DEBUG_LOG
    fprintf(logOutput, text);
    fflush(logOutput);
#endif
}

void printInt(const char *text,int num)
{
#if DEBUG_LOG
    char buf[20];
    fprintf(logOutput, text);
    fprintf(logOutput, itoa(num, (char *) &buf,10));
    fprintf(logOutput, "\n");
    fflush(logOutput);
#endif
}

void printPrune()
{
#if DEBUG_LOG
    printInt("> LV0:",chkEnabled[LV0]);
    printInt("> LV1:",chkEnabled[LV1]);
    printInt("> LV2:",chkEnabled[LV2]);
    printInt("> LV3:",chkEnabled[LV3]);
    printInt("> LV4:",chkEnabled[LV4]);
    print("\n");
    fflush(logOutput);
#endif
}

void printMatrix(FILE *out,int showPossibilities)
{
#if DEBUG_LOG
    int i,j;

    if(showPossibilities)
    {
        for(i=0; i<n2; i++)
        {
            for(j=0; j<n2; j++)
            {
                if(m[i][j]<1000)
                    fprintf(out," ");
                if(m[i][j]<100)
                    fprintf(out," ");
                if(m[i][j]<10)
                    fprintf(out," ");

                fprintf(out,"%d%s%s",m[i][j],DELIM,DELIM);
            }
            fprintf(out,"\n");
        }
    }else{
        for(i=0; i<n2; i++)
        {
            for(j=0; j<n2; j++)
            {
                if(m[i][j]>=mask)
                    fprintf(out,"%s%s",EMPTY,DELIM);
                else
                    fprintf(out,"%d%s",m[i][j],DELIM);
            }
            fprintf(out,"\n");
        }
    }
    fprintf(out,"\n");

    fflush(logOutput);
#endif
}

void resetPossibilities(int **matrix)
{
    int i,j;

    for(i=0; i<n2; i++)
        for(j=0; j<n2; j++)
            if(matrix[i][j]>=mask)
                matrix[i][j]=2047;
}
