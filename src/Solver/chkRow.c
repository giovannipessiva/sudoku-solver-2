#include <stdio.h>
#include <stdlib.h>
#include "Solver/header.h"

extern int cycleCounter;

extern int n;   //base of the sudoku
extern int n2;   //size of the sudoku (n2==n*n)
extern int mask;   //flag mask (mask=1<<n2+1)
extern int **m;   //matrix n^2 * n^2 for the sudoku
extern int chkEnabled[5];     //dont set numbers, just simplify int-of-flags



int checkRow(int fin){
    int i,j,k,l,value,square;
    int numbers;   //flag mask of completed numbers
    int unique;   //flag mask to find unique flags
    int unique_plus;   //flag mask to exclude not-unique flags
    int finished;   //counter of completed columns
    int *line=(int *) calloc(n,sizeof(int));   //to eliminate doubled flags
    int *totLine=(int *) calloc(n,sizeof(int));   //to eliminate doubled flags

    if(line==NULL || totLine==NULL)
    {
        fprintf(stderr,"Allocation error (4)\n");
        exit(ERR_ALLOC);
    }


    for(i=0; i<n2; i++)
        if( (fin & (1<<i)) ==0)   //if this row is not completed
        {
            finished=0;
            unique=0;
            unique_plus=0;
            //set the flag mask of completed numbers
            numbers=mask | (mask-1);   //numbers = 1....1 (n2+1 ones)
            for(j=0; j<n2; j++)
            {
                if((m[i][j] & mask) ==0)   //if it's a number
                    numbers=numbers & (numbers^(1<<m[i][j]));   //set to 0 its flag in numbers mask
                else{   //otherwise, if it's an int of flags
                    unique_plus=unique_plus | (unique & m[i][j]);  //detect duplicated flags
                    unique=unique^m[i][j];   //set to 0 flags that are not uniques
                }
            }

            unique=unique ^ (unique & unique_plus);
            if((unique & mask)==0)   //make sure it has flags's flag
                unique+=mask;


            for(j=0; j<n2; j++)
            {
                ////////////////////////////////WHYYYYYYYYYY
                square=(i/n)*n + (j/n);   //position of this square in array q

                if(m[i][j] & mask)   //if it's a flag
                {
                    m[i][j]= m[i][j] & numbers;   //use the mask


                    if((unique & m[i][j])>mask && chkEnabled[LV2])   //check if it has a unique flag
                    {
                        if(DEBUG1) printf("%d uniR: (%d,%d)=%d >>>>%d \n",cycleCounter,i,j,m[i][j],m[i][j] & unique);//////////////////
                        m[i][j]=unique & m[i][j];   //set the other flags to 0
                    }

                    value=0;
                    value=checkOneValue(i,j);
                    if(value!=0)
                    {
                        if(value==-1)
                            return -1;  //error in the sudoku

                        if(DEBUG1) printf("NEW: (%d,%d)=%d >>>>%d \n",i,j,m[i][j],value);//////////////////
                        setNum(i,j,value);
                        finished++;
                        numbers=numbers | (1<<value);  //add new number to the mask
                    }
                }else{
                    finished++;
                }
            }


            if(cycleCounter>CHECK_START)   //Since it's an expensive check, it won't start at the beginning
            {
                if(chkEnabled[LV3])
                {
                    /* LV3: this part will check if there's a flags allineation
                    if a flags allineation if found for a number,
                    a function will set to 0 the number's flags in that line. */

                    for(k=0; k<n; k++)
                    {
                        line[k]=0;
                        for(l=0; l<n; l++)
                        {
                            //save the possible numbers of the lines
                            if(m[i][k*n+l] & mask)
                                line[k]=line[k] | m[i][k*n+l];
                        }
                    }

                    //compare the flags to exclude common numbers
                    for(k=0; k<n; k++)
                    {
                        totLine[k]=0;
                        for(l=0; l<n; l++)
                            if(k!=l)
                                totLine[k]= totLine[k] | line[l];
                    }


                    //check of allineations
                    for(k=0; k<n; k++)
                    {
                        line[k]=line[k] & (line[k] ^ totLine[k]);
                        if((line[k] & mask))   //make sure it has NOT flags's flag
                            line[k]-=mask;
                        if(line[k]>1)
                        {
                            if(DEBUG2) printf("square (row) allineation: %d,n*%d, =%d [cycler=%d]\n",i,k,line[k],cycleCounter);

                            rowWarnSquare(i,k,line[k]);
                        }
                    }
                }


                //LV4
                for(j=1; j<n2-1 && chkEnabled[LV4]; j++)
                    if(m[i][j] & mask)   //if it's a flag
                    {
                        //find groups of cells with identical flags
                        k=1;
                        value=0;
                        for(l=j+1; l<n2; l++){
                            if(m[i][l] & mask)   //if it's a flag
                                if(areEquals(m[i][j],m[i][l]))
                                    k++;
                        }
                        if(k==countValues(i,j)){
                            //i found a group of identical flags,
                            //now i can semplify them from the cells that are not in the group
                            value=m[i][j];

                            for(l=0; l<n2; l++)
                            {
                                if(m[i][l] & mask)   //if it's a flag
                                    if(areEquals(value,m[i][l])==0){
                                        square=(i/n)*n + (l/n);   //position of this square in array q
                                        m[i][l]=( (m[i][l] ^ m[i][j]) & m[i][l] ) | mask; //*/
                                    }
                            }
                        }
                    }
            }

            if(finished==n2)
                fin=fin | (1<<i);   //set the flag for this row as completed
    }


    free(line);
    free(totLine);
	return fin;
}

void rowWarnSquare(int i,int k,int val){
    int p,r;

    for(p=(i-i%n); p<(i-i%n+n); p++)
        if(p!=i)   //if it's not the allineation
            for(r=k*n; r<(k*n+n); r++)
                if(m[p][r] & mask){
                    if(DEBUG2) printf("subSqR: (%d,%d)=%d >>>>%d \n",p,r,m[p][r],m[p][r] & (m[p][r] ^ val));
                    m[p][r]=m[p][r] & (m[p][r] ^ val);
                }
}
