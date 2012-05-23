#include <stdio.h>
#include <stdlib.h>
#include "Solver/header.h"

extern int n;   //base of the sudoku
extern int n2;   //size of the sudoku (n2=n*n)
extern int mask;   //flag mask (mask=1<<n2+1)
extern int **m;   //matrix n^2 * n^2 for the sudoku
extern int chkEnabled[5];     //dont set numbers, just simplify int-of-flags


/**
int checkOneValue(int i,int j);
Check if the cell (i,j), considered an int-of-flags,
has just one flag set to 1.
If it is so, return the number of that flag,
because it's the only possible number in that position.
If an error is detected (no possible values) returns -1;
Otherwise, it returns 0.

*/
int checkOneValue(int i,int j)
{
    //check if it has just one possible number (just one flag to 1)
    int value=0;
    int tmp=m[i][j];
    int l;

    if(!chkEnabled[LV1])
        return 0;

    for(l=1; l<n2+1; l++)
    {
        tmp=tmp>>1;
        if(tmp&1)
        {
            if(value==0)
                value=l;
            else
                value=-1;
        }
    }

    if(value!=0)
    {
        if(value!=-1)
            return value;
        else
            return 0;
    }else{
        if((m[i][j] & mask)==0)
            return 0;   //it is not an int of flag!!!
        else{
            printInt("Error in the sudoku! A cell don't have any possible values: \n",m[i][j]);
            printInt("i=",i);
            printInt("j=",j);
            return -1;
        }
    }
}

/**
int checkUnicity()
Check if every cell of int-of-flags has
just one possible value (see: pruning==1)
*/
int checkUnicity()
{
    int i,j;

    for(i=0;    i<n2;   i++)
        for(j=0;    j<n2;   j++)
            if(m[i][j] & mask)
                if(!checkOneValue(i,j))
                    return 0;
    return 1;
}

/**
int countIncompleted()
Return the number of int-of-flags cells
*/
int countIncompleted()
{
    int i,j,n=0;

    for(i=0;    i<n2;   i++)
        for(j=0;    j<n2;   j++)
            if(m[i][j] & mask)
                n++;
    return n;
}

/**
int countValues(int i,int j);
Count the possible values in the cell (i,j),
considered as an int-of-flags.
*/
int countValues(int i,int j)
{
    int tmp=m[i][j];
    int tot=0;
    int l;

    for(l=1; l<n2+1; l++)
    {
        tmp=tmp>>1;
        if(tmp&1)
            tot++;
    }

    return tot;
}

/**
int areEquals(int i,int j);
Check if i and j (int-of-flags) have the same flags.
*/
int areEquals(int i,int j){
    i=i>>1;
    j=j>>1;
    if(i==j)
        return 1;
    else
        return 0;
}

/**
void setNum(int i,int j,int num);
Set m[i][j]=num and cancel num's flag from his square, his row and his column
*/
void setNum(int i,int j,int num)
{
    int k,l;
    int square=(i/n)*n + j/n;
    int tmp;

    tmp=m[i][j];
    m[i][j]=num;

    for(k=i-i%n; k<i-i%n+n; k++)
        for(l=j-j%n; l<j-j%n+n; l++)
            if(m[k][l] & mask)
                m[k][l]=m[k][l] & (m[k][l] ^ 1<<num);

    for(k=0; k<n2; k++){
        square=(k/n)*n + j/n;
        if(m[k][j] & mask)
            m[k][j]=m[k][j] & (m[k][j] ^ 1<<num);
        square=(i/n)*n + k/n;
        if(m[i][k] & mask)
            m[i][k]=m[i][k] & (m[i][k] ^ 1<<num);
    }

    if(!chkEnabled[LV0])
        m[i][j]=tmp;
}
